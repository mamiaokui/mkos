#ifndef LayerManager_h
#define LayerManager_h
#define MAX_LAYERS 100
#define LAYER_INUSE 1
class LayerManager;

#define BOOTINFO_ADDRESS 0x0ff0 //look for the head of AsmHead.asm for reason.
typedef struct {
    int m_vmode;
    int m_screenWidth;
    int m_screenHeight;
    unsigned char* m_vram;
} BootInfo;


class Layer
{
private:
    unsigned char* m_buffer;
    int m_width;
    int m_height;
    int m_x;
    int m_y;
    int m_zorder;
    unsigned int m_flags;
    Layer();
    void init(LayerManager* layerManager, int width, int height);
    int m_indexInLayers;
    LayerManager* m_layerManager;
    friend class LayerManager;
public:
    void setPosition(int x, int y, bool needRepaint = true);
    unsigned char* getBuffer();
    int getX() {return m_x;}
    int getY() {return m_y;}
    int getWidth() {return m_width;}
    int getHeight() {return m_height;}
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

    static LayerManager* m_layerManager;
    int m_layerCount;
public:
    Layer m_layerContainer[100];
    static LayerManager* getLayerManager();
    void init(unsigned char* vram, int screenWidth, int screenHeight);
    Layer* generateLayer(int width, int height);
    Layer* generateWindow(int width, int height, char* prompt);
    void changeZOrderTop(Layer* layer);
    void repaint(int x, int y, int width, int height);
    bool rectClip(int a[4], int b[4]);
    void moveLayer(Layer* layer, int x, int y);
    int min(int a, int b) {return (a < b)?a:b;}
    int max(int a, int b) {return (a > b)?a:b;}
    void initWindow(Layer* layer, int width, int height, char*prompt);
    int getScreenWidth() {return m_screenWidth;}
    int getScreenHeight() {return m_screenHeight;}
};


#endif
