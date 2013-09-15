#include "MemoryManager.h"
#include "AsmPack.h"
#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLED 0x60000000
#define MAX_MEMORY 0xffffffff
MemoryManager::MemoryManager()
{
    m_checkNumber1 = 0xaa55aa55;
    m_checkNumber2 = 0x55aa55aa;
    m_largeNumber = 0xffffffff;
}
unsigned int MemoryManager::getMemorySize()
{
    if (m_memorySize == 0)
        m_memorySize = checkMemory();
    return m_memorySize;
}

unsigned int MemoryManager::checkMemory()
{
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
}

unsigned int MemoryManager::checkMemorySub()
{
    unsigned int i = 0x1000000;
    for (; i < (unsigned int)(0xffffffff); i+=0x1000)
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
