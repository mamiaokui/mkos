#include "LayerManager.h"
#ifndef MKDEBUG
#include "MemoryManager.h"
#else
#include <iostream>
using namespace std;
unsigned char* globalVram;
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

void Layer::init(LayerManager* layerManager, int width, int height)
{
#ifndef MKDEBUG
    extern MemoryManager* globalMemoryManager;
    m_buffer = (unsigned char*)globalMemoryManager->malloc(width * height);
#else
    m_buffer = (unsigned char*)new char[width * height];
#endif
    m_width = width;
    m_height = height;
    m_layerManager = layerManager;
}

void Layer::setPosition(int x, int y, bool needRepaint)
{
    int oldX = m_x;
    int oldY = m_y;
    m_x = x;
    m_y = y;
    if (needRepaint)
    {
        m_layerManager->repaint(oldX, oldY, m_width, m_height);
        m_layerManager->repaint(m_x, m_y, m_width, m_height);
    }
}

unsigned char* Layer::getBuffer()
{
    return m_buffer;
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
    m_vramTemp = (unsigned char*)new char [m_screenWidth * m_screenHeight];
    for (int i = 0; i < m_screenHeight * m_screenHeight; i++)
    {
        m_vramTemp[i] = '.';
    }
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
        m_layerManager = new LayerManager();
#endif
        BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

#ifndef MKDEBUG
        int screenWidth = bootInfo->m_screenWidth;
        int screenHeight = bootInfo->m_screenHeight;
        unsigned char* vram = bootInfo->m_vram;
#else
        int screenWidth = 100;
        int screenHeight = 50;
        
        unsigned char* vram = (unsigned char*)new char[screenWidth * screenHeight];
        globalVram = vram;

#endif

        m_layerManager->init(vram, screenWidth, screenHeight);
    }
    return m_layerManager;
}

Layer* LayerManager::generateLayer(int width, int height)
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
                    m_layers[index+i]->m_indexInLayers = index+i;
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
                m_layers[0]->m_indexInLayers = 0;
            }
            else
            {
                m_layers[index+1] = layer;
                m_layers[index+1]->m_indexInLayers = index + 1;
            }
            m_layerCount++;
            layer->init(this, width, height);
            return layer;
        }
    }

    // guess never come to here;
    return 0;
}

void LayerManager::changeZOrderTop(Layer* layer)
{
    int oldZorder = layer->m_zorder;
    int currentIndex = layer->m_indexInLayers;
    /*
    for (int i = m_layerCount - 1; i >= 0; i--)
    {
        if(m_layers[i]->m_zorder == oldZorder)
        {
            currentIndex = i;
            break;
        }
    }
    */

    if (oldZorder == -1)
        m_layerTop++;
    
    for (int i = currentIndex; i < m_layerCount - 1; i++)
    {
        m_layers[i] = m_layers[i + 1];
        m_layers[i]->m_indexInLayers = i;
    }
    m_layers[m_layerCount - 1] = layer;
    m_layers[m_layerCount - 1]->m_indexInLayers = m_layerCount - 1;
    layer->m_zorder = m_layerTop; 
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
            for (int y = paintPositionY; y < paintPositionY + blockHeight; y++)
                for ( int x = paintPositionX; x < paintPositionX + blockWidth; x++)
                {
                    unsigned char gdb = m_layers[i]->m_buffer[(y-m_layers[i]->m_y) * m_layers[i]->m_width + x-m_layers[i]->m_x];
                    if (gdb != 'a' && gdb != 'b' && gdb != 'c' && gdb != '.')
                        gdb ++;
                    m_vramTemp[y * m_screenWidth + x] = m_layers[i]->m_buffer[(y-m_layers[i]->m_y) * m_layers[i]->m_width + x-m_layers[i]->m_x];
                }
        }
    }


    for ( int y = yPos; y < yPos + height; y++)
    {
        for (int x = xPos; x < xPos + width; x++)
            m_vram[y * m_screenWidth + x] = m_vramTemp[y * m_screenWidth + x];
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
    int xSmallerBackup[4];
    for (int i = 0; i < 4; i++)
    {
        xSmallerBackup[i] = xSmaller[i];
    }
    if (xSmaller[0] + xSmaller[2] > xLarger[0] && xSmaller[1] + xSmaller[3] > xLarger[1])
    {
        xSmaller[0] = xLarger[0];
        xSmaller[1] = xLarger[1];
        xSmaller[2] = xSmallerBackup[0] + xSmaller[2] - xLarger[0];
        xSmaller[3] = xSmallerBackup[1] + xSmaller[3] - xLarger[1];

        for (int i = 0; i < 4; i++)
        {
            a[i] = xSmaller[i];
        }
        
        return true;
    }

    return false;
}
