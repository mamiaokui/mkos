#include "MemoryManager.h"
#include "AsmTools.h"
#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLED 0x60000000
#define MAX_MEMORY 0xffffffff
MemoryManager::MemoryManager()
{
    m_checkNumber1 = 0xaa55aa55;
    m_checkNumber2 = 0x55aa55aa;
    m_largeNumber = 0xffffffff;
    m_freeItemCount = 1;
    //bootprogram address is near 2M, so start at 4M.
    m_frees[0].m_address = 4*1024*1024;
    m_frees[0].m_size = (getMemorySize() - 4) * 1024 * 1024;
}

unsigned int MemoryManager::malloc(unsigned int size)
{
    size = ((size + 0xfff) >> 12) << 12;
    mallocInternal(size);
}

unsigned int MemoryManager::mallocInternal(unsigned int size)
{
    for (int i = 0; i < m_freeItemCount; i++)
    {
        if (m_frees[i].m_size > size)
        {
            unsigned int address = m_frees[i].m_address;
            m_frees[i].m_address += size;
            m_frees[i].m_size -= size;
            if (m_frees[i].m_size == 0)
            {
                m_freeItemCount--;
                for(; i < m_freeItemCount; i++)
                {
                    m_frees[i] = m_frees[i+1];
                }
            }
            return address;
        }
    }
    return 0;
}

void MemoryManager::free(unsigned int address, unsigned int size)
{
    size = ((size + 0xfff) >> 12) << 12;
    freeInternal(address, size);
}

void MemoryManager::freeInternal(unsigned int address, unsigned int size)
{
    int i;
    for (i = 0; i < m_freeItemCount; i++)
    {
        if (m_frees[i].m_address > address)
            break;
    }

    int state = 0;

    if (i > 0)
    {
        if (m_frees[i - 1].m_address + m_frees[i - 1].m_size == address)
        {
            m_frees[i - 1].m_size += size;
            address = m_frees[i - 1].m_address;
            size = m_frees[i - 1].m_size;
            state++;
        }
    }

    if (address + size == m_frees[i].m_address)
    {
        m_frees[i].m_size += size;
        m_frees[i].m_address = address;
        size = m_frees[i].m_size;
        state++;
    }

    if (state == 0)
    {
        for (int j = m_freeItemCount; j > i; j--)
        {
            m_frees[j] = m_frees[j - 1];
        }
        m_freeItemCount++;
        m_frees[i].m_address = address;
        m_frees[i].m_size = size;
    }
    else if (state == 1)
    {
        //very good, don't need to do anything.
    }
    else if (state == 2)
    {
        for(; i < m_freeItemCount; i++)
        {
            m_frees[i-1] = m_frees[i];
        }
        m_freeItemCount--;
    }
}

unsigned int MemoryManager::getMemorySize()
{
    if (m_memorySize == 0)
        m_memorySize = checkMemory();
    return m_memorySize;
}

unsigned int MemoryManager::checkMemory()
{
#ifdef MKDEBUG
    return 36;
#else
    bool is486 = false;
    unsigned int result;
    unsigned int eflags = asmLoadEflags();
    eflags |= EFLAGS_AC_BIT;
    asmStoreEflags(eflags);
    eflags = asmLoadEflags();
    if ((eflags & EFLAGS_AC_BIT) != 0)
        is486 = true;
    
    eflags &= ~EFLAGS_AC_BIT;
    asmStoreEflags(eflags);
    if (is486)
    {
        unsigned int cr0 = asmLoadCr0();
        cr0 |= CR0_CACHE_DISABLED;
        asmStoreCr0(cr0);
    }

    result = checkMemorySub();

    if (is486)
    {
        unsigned int cr0 = asmLoadCr0();
        cr0 &= ~CR0_CACHE_DISABLED;
        asmStoreCr0(cr0);
    }
    return result;
#endif
}

unsigned int MemoryManager::checkMemorySub()
{
    unsigned int i = 0x1000000;
    for (; i < (unsigned int)(0xffffffff); i+=0x1000000/4)
    {
        unsigned int* p = (unsigned int*)(i + 0xffc);
        unsigned int old = *p;
        *p = m_checkNumber1;
        *p ^= m_largeNumber;
        if (*p != m_checkNumber2)
        {
            *p = old;
            break;
        }
        *p ^= m_largeNumber;
        if (*p != m_checkNumber1)
        {
            *p = old;
            break;
        }
        *p = old;

    }
    return i/1024/1024;
}

void MemoryManager::report(void (*reporter)(FreeItem&))
{
    for (int i = 0; i < m_freeItemCount; i++)
    {
        reporter(m_frees[i]);
    }
}


