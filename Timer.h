class Timer
{
private: 
    void initPit();
    static Timer* m_timer;
public:
    static Timer* getTimer();

};
