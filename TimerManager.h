#ifndef TimerManager_h
#define TimerManager_h
#include "AsmTools.h"
#define TIMER_COUNT 100
class TimerCallback
{
public:
    void callback(int id)
    {
        extern int count;
        count = id;
        if (id == 123)
        {
            asmTaskSwitch4();
        }
    }
};

class Timer
{
public:
    int m_fireTime;
    int m_ID;
    TimerCallback* m_timerCallback;
    int m_flags;
    Timer()
    {
        init();
    }
    void init()
    {
        m_fireTime = -1;
        m_ID = -1;
        m_timerCallback = 0;
        m_flags = 0;
    }
};

class TimerManager
{
private:
    static TimerManager* m_timerManager;
    Timer* m_sortedTimers[TIMER_COUNT];
    Timer m_timerContainer[TIMER_COUNT];
    int m_currentTime;
    int m_timerCount;
public:
    static TimerManager* getTimerManager();
    void setTimer(TimerCallback*, int timeout, int id);
    void freeTimer(TimerCallback*);
    void tick();
};

extern "C" void int20Handler(int* arg);
extern "C" void asmInt20Handler(void);

#endif
