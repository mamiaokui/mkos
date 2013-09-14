#ifndef InterruptionBuffer_h
#define InterruptionBuffer_h

class InterruptionBuffer{
private:
    int* m_buffer;
    int m_nextRead;
    int m_nextWrite;
    int m_size;
    int m_free;
    int m_flags;
public:
    void initInterruptionBuffer(int size, int* buffer);
    void inputInterruptionBuffer(int data);
    int getInterruptionBuffer();
    bool isInterruptionBufferEmpty();
} ;

#define BUFFER_OVERFLOW 1

#endif
