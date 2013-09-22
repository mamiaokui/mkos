#include "TimerManager.h"
#include "MemoryManager.h"
#include "GdtIdt.h"
#include "AsmTools.h"
#define PIT_CTRL 0x43
#define PIT_CNT 0x40

TimerManager* TimerManager::m_timerManager = 0;
TimerManager* TimerManager::getTimerManager()
{
    if (m_timerManager == 0)
    {
        extern MemoryManager* globalMemoryManager;
        m_timerManager = (TimerManager*) globalMemoryManager->malloc(sizeof(TimerManager));

        asmOut8(PIT_CTRL, 0x34);
        asmOut8(PIT_CNT, 0x9c);
        asmOut8(PIT_CNT, 0x2e);
    }
    return m_timerManager;
}
void TimerManager::setTimer(void (*timerCallback)())
{
    
}

int count = 0;
void int20Handler(int* arg)
{
	asmOut8(PIC0_OCW2, 0x60); 
    count ++;
	return;
}

