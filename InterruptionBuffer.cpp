#include "InterruptionBuffer.h"
#include "MemoryManager.h"
InterruptionBuffer* InterruptionBuffer::m_interruptionBuffer = 0;

InterruptionBuffer* InterruptionBuffer::getInterruptionBuffer()
{
    if (m_interruptionBuffer == 0)
    {
        extern MemoryManager* globalMemoryManager;
        m_interruptionBuffer = (InterruptionBuffer*) globalMemoryManager->malloc(sizeof(InterruptionBuffer));
        int* interruptionBufferData = (int*) globalMemoryManager->malloc(128 * sizeof(int));
        m_interruptionBuffer->initInterruptionBuffer(128, interruptionBufferData);
    }
}
void InterruptionBuffer::initInterruptionBuffer(int size, int* buffer)
{
    this->m_size = size;
    this->m_free = size;
    this->m_buffer = buffer;
    this->m_flags = 0;
    this->m_nextRead = 0;
    this->m_nextWrite = 0;
    this->m_buffer = buffer;
}


void InterruptionBuffer::inputInterruptionBuffer(int data)
{
    this->m_flags = 0;
    if (this->m_free == 0)
    {
        this->m_flags |= BUFFER_OVERFLOW;
        return ;
    }

    this->m_buffer[this->m_nextWrite] = data;
    this->m_nextWrite++;
    if (this->m_nextWrite == this->m_size) {
        this->m_nextWrite = 0;
    }

    this->m_free--;
}

int InterruptionBuffer::getInterruptionBufferData()
{
    this->m_flags = 0;
    if (this->m_free == this->m_size)
    {
        this->m_flags |= BUFFER_OVERFLOW;
        return 0;
    }
    int data;
    data = this->m_buffer[this->m_nextRead];
    this->m_nextRead++;
    if (this->m_nextRead == this->m_size)
        this->m_nextRead = 0;

    this->m_free++;
    return data;
}
bool InterruptionBuffer::isInterruptionBufferEmpty()
{
    return this->m_free == this->m_size;
}
