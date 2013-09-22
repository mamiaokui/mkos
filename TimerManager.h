#ifndef TimerManager_h
#define TimerManager_h
class TimerManager
{
private:
    static TimerManager* m_timerManager;
    
public:
    static TimerManager* getTimerManager();
    void setTimer(void (*timerCallback)());
    void freeTimer(void (*timerCallback)());
};

extern "C" void int20Handler(int* arg);
extern "C" void asmInt20Handler(void);

#endif
