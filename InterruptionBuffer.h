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
    static InterruptionBuffer* m_interruptionBuffer;
public:
    void initInterruptionBuffer(int size, int* buffer);
    void inputInterruptionBuffer(int data);
    int getInterruptionBufferData();
    bool isInterruptionBufferEmpty();
    static InterruptionBuffer* getInterruptionBuffer();
} ;

#define BUFFER_OVERFLOW 1

#endif
