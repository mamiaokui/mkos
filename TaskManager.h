#ifndef TaskManager_h
#define TaskManager_h
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

class TaskManager
{
public:
    TSS32* getTSS()
    {
        extern MemoryManager* globalMemoryManager;
        TSS32* tss = (TSS32*)globalMemoryManager->malloc(sizeof(TSS32));
        return tss;
    }
};
#endif
