#ifndef StartupManager_h
#define StartupManager_h

class Layer;
class LayerManager;
class KeyBoardMouseHandler;
class StartupManager
{
private:
    int m_screenWidth;
    int m_screenHeight;
    Layer* m_layerBackground;
    Layer* m_layerMouse;
    Layer* m_layerWindow;
    LayerManager* m_layerManager;
    KeyBoardMouseHandler* m_keyboardMouseHandler;
public:
    void init();
    void reportMemory();
    void loop();
    void countNumber();
    void logInt(int intData);
};
#endif
