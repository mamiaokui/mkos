#include "StartupManager.h"
#include "AsmTools.h"
#include "PaintPack.h"
#include "GdtIdt.h"
#include "Platform.h"
#include "Utils.h"
#include "InterruptionBuffer.h"
#include "LayerManager.h"
#include "KeyBoardMouseHandler.h"
#include "MemoryManager.h"
#include "Timer.h"


void StartupManager::init()
{
    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

    m_screenWidth = bootInfo->m_screenWidth;
    m_screenHeight = bootInfo->m_screenHeight;
    initGdtIdt();
    initPic();
	asmSti(); 
	asmOut8(PIC0_IMR, 0xf8); 
	asmOut8(PIC1_IMR, 0xef); 
    Timer::getTimer();
    KeyBoardMouseHandler::getHandler()->initKeyboard();
	initPalette();     


    m_layerManager = LayerManager::getLayerManager();
    m_layerBackground = m_layerManager->generateLayer(m_screenWidth, m_screenHeight);
    initScreen(m_layerBackground->getBuffer(), m_screenWidth, m_screenHeight);


    
    char charScreenWidth[30];
    extern MemoryManager* globalMemoryManager;
    int memory = globalMemoryManager->getMemorySize();

    intToCharArray(charScreenWidth, memory);
    const char* screenWidthStr = "memory(MB) = ";
    char result[30];
    stringcat(screenWidthStr, charScreenWidth, result);

    printString(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COL000000, result);

    m_layerManager->changeZOrderTop(m_layerBackground);

    unsigned char mouseCursorImage[256];

    initMouseCursor(mouseCursorImage);
    m_layerMouse = m_layerManager->generateLayer(16, 16);
    paintBlock(m_layerMouse->getBuffer(), 16, 16, 16, 0, 0, mouseCursorImage);

    Layer* layerWindow = m_layerManager->generateWindow(160, 68, "MKOS");
    m_layerWindow = layerWindow;
    m_layerManager->changeZOrderTop(layerWindow);
    layerWindow->setPosition(30, 30);
    m_layerManager->changeZOrderTop(m_layerMouse);

    KeyBoardMouseHandler::getHandler()->enableMouse();
}

void StartupManager::loop()
{
	int mx = (m_screenWidth - 16) / 2;
	int my = (m_screenHeight - 28 - 16) / 2;
    m_layerMouse->setPosition(mx, my);

    extern int count;
    while (true)
    {
        char charScreenWidth[30];
        intToCharArray(charScreenWidth, count);
        m_layerManager->initWindow(m_layerWindow, m_layerWindow->getWidth(), m_layerWindow->getHeight(), "MKOS");
        printString(m_layerWindow->getBuffer(), m_layerWindow->getWidth(), 8, 30, COL000000, charScreenWidth);
        m_layerManager->repaint(m_layerWindow->getX() + 8, m_layerWindow->getY() + 30, 50, 16);

		asmCli();
		if (InterruptionBuffer::getInterruptionBuffer()->isInterruptionBufferEmpty()) 
        {
			asmStiHlt();
		} else 
        {
			int intData = InterruptionBuffer::getInterruptionBuffer()->getInterruptionBufferData();
			asmSti();
            if (intData < 512)
            {
                char b[10];
                intToCharArray(b, intData);
                initScreen(m_layerBackground->getBuffer(), m_screenWidth, m_screenHeight);
                printString(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COLFFFFFF, b);
                m_layerManager->repaint(0, 0, m_screenWidth, m_screenHeight);
            }
            else if (intData >= 512)
            {
                bool output =  KeyBoardMouseHandler::getHandler()->receiveMouseInterruption(intData - 512);
                if (output)
                {
                    int* outputData = KeyBoardMouseHandler::getHandler()->getMouseData();

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
                    if (mx > m_screenWidth - 16)
                        mx = m_screenWidth -16;

                    if (my > m_screenHeight - 16)
                        my = m_screenHeight -16;

                    m_layerMouse->setPosition(mx, my);

                }
            }
		}
    }
}
