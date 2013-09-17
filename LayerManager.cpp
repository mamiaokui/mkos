#include "LayerManager.h"
#ifndef MKDEBUG
#include "MemoryManager.h"
#else
#include <iostream>
using namespace std;
#endif
#include "Platform.h"
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
#ifndef MKDEBUG
    extern MemoryManager* globalMemoryManager;
    m_buffer = (unsigned char*)globalMemoryManager->malloc(width * height);
#else
    m_buffer = (unsigned char*)new unsigned char(width * height);
#endif
    m_width = width;
    m_height = height;
}


void LayerManager::init(unsigned char* vram, int screenWidth, int screenHeight)
{
    m_vram = vram;
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
#ifndef MKDEBUG
    extern MemoryManager* globalMemoryManager;
    m_vramTemp = (unsigned char*)globalMemoryManager->malloc(m_screenWidth * m_screenHeight);
#else
    m_vramTemp = (unsigned char*)new unsigned char (m_screenWidth * m_screenHeight);
#endif
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
#ifndef MKDEBUG
        extern MemoryManager* globalMemoryManager;
        m_layerManager = (LayerManager*) globalMemoryManager->malloc(sizeof(LayerManager));
#else
        unsigned int layerManagerSize = sizeof(LayerManager);
        m_layerManager = (LayerManager*) new unsigned char(layerManagerSize);
#endif
        BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

#ifndef MKDEBUG
        int screenWidth = bootInfo->m_screenWidth;
        int screenHeight = bootInfo->m_screenHeight;
        unsigned char* vram = bootInfo->m_vram;
#else
        int screenWidth = 320;
        int screenHeight = 200;
        
        unsigned char* vram = new unsigned char[320*200];
        cout << "vram is " << &vram[0] << endl;

#endif

        m_layerManager->init(vram, screenWidth, screenHeight);
    }
    return m_layerManager;
}

Layer* LayerManager::generateLayer()
{
    Layer* layer = 0;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        if (m_layerContainer[i].m_flags == 0)
        {
            layer = m_layerContainer + i;
            layer->m_flags = LAYER_INUSE;
            layer->m_zorder = -1;

            int index;
            for (index = m_layerCount-1; index >= 0; index--)
            {
                if (m_layers[index]->m_zorder > layer->m_zorder)
                {
                    m_layers[index+i] = m_layers[index];
                }
                else
                {
                    break;
                }
            }

            //not found
            if (index == -1)
            {
                m_layers[0] = layer;
            }
            else
            {
                m_layers[index+1] = layer;
            }
            m_layerCount++;
            return layer;
        }
    }

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
    repaint(layer->m_x, layer->m_y, layer->m_width, layer->m_height);
}


void LayerManager::repaint(int xPos, int yPos, int width, int height)
{
    for (int i = 0; i < m_layerCount; i++)
    {
        if(m_layers[i]->m_zorder == -1)
            continue;

        int rectRepaint[4] = {xPos, yPos, width, height};
        int rectLayer[4] = {m_layers[i]->m_x, m_layers[i]->m_y, m_layers[i]->m_width, m_layers[i]->m_height};
        if (rectClip(rectLayer, rectRepaint))
        {
            int paintPositionX = rectLayer[0];
            int paintPositionY = rectLayer[1];
            int blockWidth = rectLayer[2];
            int blockHeight = rectLayer[3];
            for (int y = paintPositionY; y <= blockHeight; y++)
                for ( int x = paintPositionX; x <= blockWidth; x++)
                {
                    m_vramTemp[y * m_screenWidth + x] = m_layers[i]->m_buffer[(y-m_layers[i]->m_y) * m_layers[i]->m_width + x];
                }
        }
    }

    for (int x = xPos; x <= width; x++)
    {
        for ( int y = yPos; y <= height; y++)
        {
            m_vram[y * m_screenWidth + x] = m_vramTemp[y * m_screenWidth + x];
        }
    }
}

bool LayerManager::rectClip(int a[4], int b[4])
{
    int* xSmaller;
    int* xLarger;
    if (a[0] < b[0])
    {
        xSmaller = a;
        xLarger = b;
    }
    else
    {
        xSmaller = b;
        xLarger = a;
    }
    
    //has cliped
    if (a[0] + a[2] > b[0] && a[1] + a[3] > b[1])
    {
        a[0] = b[0];
        a[1] = b[1];
        a[2] = a[0] + a[2] - b[0];
        a[3] = a[1] + a[3] - b[1];
        return true;
    }

    return false;
}
