#ifndef KeyBoardMouseHandler_h
#define KeyBoardMouseHandler_h
class Layer;
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
    void initMouseCursorImage(unsigned char*);
    void init(Layer* mouseLayer);
    void moveLayerToMiddle();
    bool handleMouseInput(int data);
    int getX() { return m_mouseX;}
    int getY() { return m_mouseY;}
private:
    MouseDataDecoder m_mouseDataDecoder;
    static KeyBoardMouseHandler* m_handler;
    Layer* m_layerMouse;
    int m_mouseX;
    int m_mouseY;
};

extern "C" void int21Handler(int* arg);
extern "C" void asmInt21Handler(void);
extern "C" void int2cHandler(int* arg);
extern "C" void asmInt2cHandler(void);

#endif
