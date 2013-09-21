#define PIT_CTRL 0x43
#define PIT_CNT 0x40

#include "Timer.h"
#include "AsmPack.h"
#include "GdtIdt.h"
#include "Platform.h"
#include "MemoryManager.h"
#include "InterruptionBuffer.h"
Timer* Timer::m_timer = 0;
void Timer::initPit()
{
    asmOut8(PIT_CTRL, 0x34);
    asmOut8(PIT_CNT, 0x9c);
    asmOut8(PIT_CNT, 0x2e);
}



Timer* Timer::getTimer()
{
    if (m_timer == 0)
    {
        extern MemoryManager* globalMemoryManager;
        m_timer = (Timer*) globalMemoryManager->malloc(sizeof(Timer));
        m_timer->initPit();
    }
    return m_timer;
}

int count = 0;
void int20Handler(int* arg)
{
	asmOut8(PIC0_OCW2, 0x60); 
    count ++;
	return;
}
