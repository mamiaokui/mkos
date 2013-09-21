#ifndef KeyBoardMouseHandler_h
#define KeyBoardMouseHandler_h
class MouseDataDecoder 
{
private:
    int m_mouseState;
    int m_mouseRawData[3];
    int m_mouseDecodeData[3];
public:
    MouseDataDecoder();
    bool receiveMouseInterruption(int data);
    int* getMouseData();
};

class KeyBoardMouseHandler
{
public:
    static KeyBoardMouseHandler* getHandler();
    void waitKeyboardReady(void);
    void initKeyboard(void);
    void enableMouse(void);
    bool receiveMouseInterruption(int data);
    int* getMouseData() {return m_mouseDataDecoder.getMouseData();}
private:
    MouseDataDecoder m_mouseDataDecoder;
    static KeyBoardMouseHandler* m_handler;
};
#endif
