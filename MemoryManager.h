#ifndef MemoryManager_h
#define MemoryManager_h
struct FreeItem {
    unsigned int m_address;
    unsigned int m_size;
};
class MemoryManager
{
private:
    unsigned int m_memorySize;
    int m_freeItemCount;
    FreeItem m_frees[1000];

    unsigned int checkMemory();
    unsigned int checkMemorySub();
    unsigned int mallocInternal(unsigned int size);
    void freeInternal(unsigned int address, unsigned int size);

public:
    MemoryManager();
    unsigned int getMemorySize();
    unsigned int volatile m_checkNumber1;
    unsigned int volatile m_checkNumber2;
    unsigned int volatile m_largeNumber;

    //4K for one block, malloc blocks.
    unsigned int malloc(unsigned int size);

    //TODO: bad design, no one use free function, so memory leak in the OS. 
    //needredisign it
    void free(unsigned int address, unsigned int size);
    void report(void (*reporter)(FreeItem&));
};
#endif
