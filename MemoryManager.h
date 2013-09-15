#ifndef MemoryManager_h
#define MemoryManager_h
class MemoryManager
{
private:
    unsigned int m_memorySize;
    unsigned int checkMemory();
    unsigned int checkMemorySub();
public:
    MemoryManager();
    unsigned int getMemorySize();
    unsigned int volatile m_checkNumber1;
    unsigned int volatile m_checkNumber2;
    unsigned int volatile m_largeNumber;
};
#endif
