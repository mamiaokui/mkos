#include "InterruptionBuffer.h"

void InterruptionBuffer::initInterruptionBuffer(int size, unsigned char* buffer)
{
    this->m_size = size;
    this->m_free = size;
    this->m_buffer = buffer;
    this->m_flags = 0;
    this->m_nextRead = 0;
    this->m_nextWrite = 0;
    this->m_buffer = buffer;
}


void InterruptionBuffer::inputInterruptionBuffer(unsigned char data)
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

unsigned char InterruptionBuffer::getInterruptionBuffer()
{
    this->m_flags = 0;
    if (this->m_free == this->m_size)
    {
        this->m_flags |= BUFFER_OVERFLOW;
        return 0;
    }
    unsigned char data;
    data = this->m_buffer[this->m_nextRead];
    this->m_nextRead++;
    if (this->m_nextRead == this->m_size)
        this->m_nextRead = 0;

    this->m_free++;
    return data;
}
bool InterruptionBuffer::isInterruptionBufferEmpty()
{
    return this->m_free == this->m_free;
}
