#include "AsmPack.h"
#include "PaintPack.h"
#include "GdtIdt.h"
#include "Platform.h"
#include "Utils.h"
#include "InterruptionBuffer.h"
#include "MemoryManager.h"
#include "LayerManager.h"

//change from c to cpp, ld link error because can't find the implement.
void * __gxx_personality_v0=0;
void * _Unwind_Resume =0;

extern InterruptionBuffer globalInterruptionBuffer;
MemoryManager* globalMemoryManager;

class MouseDataDecoder 
{
private:
    int m_mouseState;
    int m_mouseRawData[3];
    int m_mouseDecodeData[3];
public:
    MouseDataDecoder()
    {
        m_mouseState = 0;
        for (int i = 0; i < 3; i++)
        {
            m_mouseRawData[i] = 0;
            m_mouseDecodeData[i] = 0;
        }
    }

    bool receiveMouseInterruption(int data)
    {
        bool ok = false;
        if (m_mouseState == 0)
        {
            if (data == 0xfa)
                m_mouseState = 1;
        }
        else if (m_mouseState == 1)
        {
            if ((data & 0xc8) == 0x08)
            {
                //right
                m_mouseRawData[0] = data;
                m_mouseState = 2;
            }
        }
        else if (m_mouseState == 2)
        {
            m_mouseRawData[1] = data;
            m_mouseState = 3;
        }
        else if (m_mouseState == 3)
        {
            m_mouseRawData[2] = data;
            m_mouseState = 1;
            m_mouseDecodeData[0] = m_mouseRawData[0] & 0x07;
            m_mouseDecodeData[1] = m_mouseRawData[1];
            m_mouseDecodeData[2] = m_mouseRawData[2];
            if ((m_mouseRawData[0] & 0x10) != 0)
            {
                m_mouseDecodeData[1] |= 0xffffff00;
            }
            else if ((m_mouseRawData[0] & 0x20) != 0)
            {
                m_mouseDecodeData[2] |= 0xffffff00;                
            }

            m_mouseDecodeData[2] = -m_mouseDecodeData[2];
            ok = true;
        }
        return ok;
    }

    int* getMouseData()
    {
        return m_mouseDecodeData;
    }
};

extern "C" void MKOSMain(void)
{
    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

    int screenWidth = bootInfo->m_screenWidth;
    int screenHeight = bootInfo->m_screenHeight;
    unsigned char* vram = bootInfo->m_vram;
    int interruptionBufferData[128];
    globalInterruptionBuffer.initInterruptionBuffer(128, interruptionBufferData);
    initGdtIdt();
    initPic();
	asmSti(); 
	asmOut8(PIC0_IMR, 0xf9); 
	asmOut8(PIC1_IMR, 0xef); 
	initKeyboard();
	initPalette();     
    MemoryManager memoryManager;
    globalMemoryManager = &memoryManager;
    LayerManager* layerManager = LayerManager::getLayerManager();
    Layer* layerBackground = layerManager->generateLayer(screenWidth, screenHeight);
    initScreen(layerBackground->getBuffer(), screenWidth, screenHeight);


    
    char charScreenWidth[30];
    int memory = memoryManager.getMemorySize();

    intToCharArray(charScreenWidth, memory);
    const char* screenWidthStr = "memory(MB) = ";
    char result[30];
    stringcat(screenWidthStr, charScreenWidth, result);

    printString(layerBackground->getBuffer(), screenWidth, 8, 8, COL000000, result);

    layerManager->changeZOrderTop(layerBackground);

    unsigned char mouseCursorImage[256];
	int mx = (screenWidth - 16) / 2;
	int my = (screenHeight - 28 - 16) / 2;

    initMouseCursor(mouseCursorImage);
    Layer* layerMouse = layerManager->generateLayer(16, 16);
    paintBlock(layerMouse->getBuffer(), screenWidth, 16, 16, 0, 0, mouseCursorImage);
    layerManager->changeZOrderTop(layerMouse);

    enableMouse();
    
    MouseDataDecoder mouseDataDecoder;
    while (true)
    {
		asmCli();
		if (globalInterruptionBuffer.isInterruptionBufferEmpty()) 
        {
			asmStiHlt();
		} else 
        {
			int intData = globalInterruptionBuffer.getInterruptionBuffer();
			asmSti();
            if (intData < 512)
            {
                char b[10];
                intToCharArray(b, intData);
                initScreen(layerBackground->getBuffer(), bootInfo->m_screenWidth, bootInfo->m_screenHeight);
                printString(layerBackground->getBuffer(), bootInfo->m_screenWidth, 8, 8, COLFFFFFF, b);
                layerManager->repaint(0, 0, bootInfo->m_screenWidth, bootInfo->m_screenHeight);
            }
            else if (intData >= 512)
            {
                bool output =  mouseDataDecoder.receiveMouseInterruption(intData - 512);
                if (output)
                {
                    int* outputData = mouseDataDecoder.getMouseData();
                    mx += outputData[1];
                    //fix me. don't know the true reason. mouse always receive large negative y move.
                    //temp fix.
                    if (outputData[2] < -200)
                    {
                        outputData[2] = 0;
                    }
                    my += outputData[2];
                    if (mx < 0)
                    {
                        mx = 0;
                    }
                    if (my < 0)
                    {
                        my = 0;
                    }
                    if (mx > screenWidth - 16)
                        mx = screenWidth -16;

                    if (my > screenHeight - 16)
                        my = screenHeight -16;

                    layerMouse->setPosition(mx, my);

                }
            }
		}
    }
}


