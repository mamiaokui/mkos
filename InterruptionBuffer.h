#ifndef InterruptionBuffer_h
#define InterruptionBuffer_h

class InterruptionBuffer{
private:
    unsigned char* m_buffer;
    int m_nextRead;
    int m_nextWrite;
    int m_size;
    int m_free;
    int m_flags;
public:
    void initInterruptionBuffer(int size, unsigned char* buffer);
    void inputInterruptionBuffer(unsigned char data);
    unsigned char getInterruptionBuffer();
    bool isInterruptionBufferEmpty();
} ;

#define BUFFER_OVERFLOW 1

#endif
