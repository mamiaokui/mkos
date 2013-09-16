#include "LayerManager.h"
#include "MemoryManager.h"
LayerManager* LayerManager::m_layerManager = 0;
Layer::Layer()
{
    m_buffer = 0;
    m_width = 0;
    m_height = 0;
    m_x = 0;
    m_y = 0;
    m_zorder = -1;
    m_flags = 0;
}

void Layer::init(int width, int height)
{
    extern MemoryManager* globalMemoryManager;
    m_buffer = (unsigned char*)globalMemoryManager->malloc(width * height);
    m_width = width;
    m_height = height;
}


void LayerManager::init(unsigned char* vram, int screenWidth, int screenHeight)
{
    m_vram = vram;
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    extern MemoryManager* globalMemoryManager;
    m_vramTemp = (unsigned char*)globalMemoryManager->malloc(m_screenWidth * m_screenHeight);
    m_layerTop = -1;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        m_layers[i] = 0;
    }

    m_layerCount = 0;
}

LayerManager* LayerManager::getLayerManager()
{
    if (m_layerManager == 0)
    {
        extern MemoryManager* globalMemoryManager;
        m_layerManager = (LayerManager*) globalMemoryManager->malloc(sizeof(LayerManager));
    }
    return m_layerManager;
}

Layer* LayerManager::generateLayer()
{
    Layer* layer;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        if (m_layerContainer[i].m_flags == 0)
        {
            layer = m_layerContainer + i;
            layer->m_flags = LAYER_INUSE;
            layer->m_zorder = -1;
            for (int i = m_layerCount; i > 0; i--)
            {
                if (m_layers[i - 1]->m_zorder > layer->m_zorder)
                {
                    m_layers[i] = m_layers[i - 1];
                }
                else
                {
                    m_layers[i] = layer;
                    break;
                }
            }
            return layer;
        }
    }

    m_layerCount++;

    // guess never come to here;
    return 0;
}

void LayerManager::changeZOrderTop(Layer* layer)
{
    int oldZorder = layer->m_zorder;
    int currentIndex = 0;
    for (int i = m_layerCount - 1; i >= 0; i--)
    {
        if(m_layers[i]->m_zorder == oldZorder)
        {
            currentIndex = i;
            break;
        }
    }
    
    for (int i = currentIndex; i < m_layerCount - 1; i++)
    {
        m_layers[i] = m_layers[i + 1];
    }
    m_layers[m_layerCount - 1] = layer;
    repaint(layer->m_width, layer->m_height);
}


void LayerManager::repaint(int width, int height)
{
    //todo!
}
