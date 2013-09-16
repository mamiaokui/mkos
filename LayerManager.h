#ifndef LayerManager_h
#define LayerManager_h
#define MAX_LAYERS 100
#define LAYER_INUSE 1
class Layer
{
public:
    unsigned char* m_buffer;
    int m_width;
    int m_height;
    int m_x;
    int m_y;
    int m_zorder;
    unsigned int m_flags;
    Layer();
    void init(int width, int height);
};

class LayerManager
{
private:
    unsigned char* m_vram;
    unsigned char* m_vramTemp;
    int m_screenWidth;
    int m_screenHeight;
    int m_layerTop;
    Layer* m_layers[100];
    Layer m_layerContainer[100];
    static LayerManager* m_layerManager;
    int m_layerCount;
public:
    static LayerManager* getLayerManager();
    void init(unsigned char* vram, int screenWidth, int screenHeight);
    Layer* generateLayer();
    void changeZOrderTop(Layer* layer);
    void repaint(int width, int height);
    
};


#endif
