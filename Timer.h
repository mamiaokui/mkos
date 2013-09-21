#ifndef Timer_h
#define Timer_h
class Timer
{
private: 
    void initPit();
    static Timer* m_timer;
public:
    static Timer* getTimer();

};
extern "C" void int20Handler(int* arg);
extern "C" void asmInt20Handler(void);

#endif
