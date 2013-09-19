#ifndef StartupManager_h
#define StartupManager_h

class Layer;
class LayerManager;
class StartupManager
{
private:
    int m_screenWidth;
    int m_screenHeight;
    Layer* m_layerBackground;
    Layer* m_layerMouse;
    LayerManager* m_layerManager;
public:
    void init();

    void loop();
};
#endif
