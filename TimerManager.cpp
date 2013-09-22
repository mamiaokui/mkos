#include "TimerManager.h"
#include "MemoryManager.h"
#include "GdtIdt.h"
#include "AsmTools.h"
#include "InterruptionBuffer.h"
#define PIT_CTRL 0x43
#define PIT_CNT 0x40
int count = 0;

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
        m_timerManager->m_currentTime = 0;
        m_timerManager->m_timerCount = 0;
    }
    return m_timerManager;
}

void TimerManager::setTimer(TimerCallback* callback, int timeout, int id)
{
    for (int i = 0; i < TIMER_COUNT; i++)
    {
        if (m_timerContainer[i].m_flags == 0)
        {

            m_timerContainer[i].m_flags = 1;
            m_timerContainer[i].m_ID = id;
            m_timerContainer[i].m_fireTime = timeout + m_currentTime;
            extern MemoryManager* globalMemoryManager;
            TimerCallback* callback = (TimerCallback*) globalMemoryManager->malloc(sizeof(TimerCallback));
            
            m_timerContainer[i].m_timerCallback = callback;
            int j;
            for (j = m_timerCount - 1; j >=0; j--)
            {
                if (m_sortedTimers[j]->m_fireTime > m_timerContainer[i].m_fireTime)
                {
                    m_sortedTimers[j + 1] = m_sortedTimers[j];
                }
                else
                    break;
            }
            m_sortedTimers[j + 1] = &m_timerContainer[i];
            m_timerCount++;
            break;
        }
    }
}

void TimerManager::freeTimer(TimerCallback* timerCallback)
{
    for (int i = 0; i < m_timerCount; i++)
    {
        if (m_sortedTimers[i]->m_timerCallback == timerCallback)
        {
            m_sortedTimers[i]->init();
            for (int j = i; j < m_timerCount - 1; j++)
            {
                m_sortedTimers[j] = m_sortedTimers[j + 1];
            }
            m_timerCount--;
            break;
        }
    }
}

void TimerManager::tick()
{
    m_currentTime++;
    if (m_timerCount == 0)
    {
        return;
    }
    if(m_sortedTimers[0]->m_fireTime == m_currentTime)
    {
        m_sortedTimers[0]->m_timerCallback->callback(m_sortedTimers[0]->m_ID);
        m_sortedTimers[0]->init();
        for (int i = 1; i < m_timerCount; i++)
        {
            m_sortedTimers[i - 1] = m_sortedTimers[i];
        }
        m_timerCount--;
    }
}


void int20Handler(int* arg)
{
	asmOut8(PIC0_OCW2, 0x60); 
	InterruptionBuffer::getInterruptionBuffer()->inputInterruptionBuffer(1024);
	return;
}

