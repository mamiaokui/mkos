#include "MemoryManager.h"
#include "StartupManager.h"

//change from c to cpp, ld link error because can't find the implement.
void * __gxx_personality_v0=0;
void * _Unwind_Resume =0;

MemoryManager* globalMemoryManager;

extern "C" void MKOSMain(void)
{
    MemoryManager memoryManager;
    globalMemoryManager = &memoryManager;

    StartupManager startupManager;
    startupManager.init();
    startupManager.loop();
}


