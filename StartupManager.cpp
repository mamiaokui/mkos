#include "StartupManager.h"
#include "AsmTools.h"
#include "PaintPack.h"
#include "GdtIdt.h"
#include "Utils.h"
#include "InterruptionBuffer.h"
#include "LayerManager.h"
#include "KeyBoardMouseHandler.h"
#include "MemoryManager.h"
#include "TimerManager.h"

void StartupManager::init()
{
    initGdtIdtInterruption();
    TimerManager::getTimerManager();
	initPalette();     
    m_layerManager = LayerManager::getLayerManager();
    m_screenWidth = m_layerManager->getScreenWidth();
    m_screenHeight = m_layerManager->getScreenHeight();
    m_layerBackground = m_layerManager->generateLayer(m_layerManager->getScreenWidth(), m_layerManager->getScreenHeight());
    initScreen(m_layerBackground->getBuffer(), m_screenWidth, m_screenHeight);
    reportMemory();
    m_layerManager->changeZOrderTop(m_layerBackground);
    m_layerMouse = m_layerManager->generateLayer(16, 16);
    m_keyboardMouseHandler = KeyBoardMouseHandler::getHandler();
    m_keyboardMouseHandler->init(m_layerMouse);
    m_layerWindow = m_layerManager->generateWindow(160, 68, "MKOS");
    m_layerManager->moveLayerToMiddle(m_layerWindow);
    m_layerManager->changeZOrderTop(m_layerWindow);
    m_keyboardMouseHandler->moveLayerToMiddle();
    m_layerManager->changeZOrderTop(m_layerMouse);
    m_interruptionBuffer = InterruptionBuffer::getInterruptionBuffer();
}

void StartupManager::loop()
{
    TimerManager::getTimerManager()->setTimer(0, 300, 123);
    TimerManager::getTimerManager()->setTimer(0, 200, 12);
    TimerManager::getTimerManager()->setTimer(0, 400, 1234);
    TimerManager::getTimerManager()->setTimer(0, 500, 12345);
    TimerManager::getTimerManager()->setTimer(0, 100, 1);
    while (true)
    {
        countNumber();
		asmCli();
		if (m_interruptionBuffer->isInterruptionBufferEmpty()) 
        {
			asmStiHlt();
		} 
        else 
        {
			int intData = m_interruptionBuffer->getInterruptionBufferData();
			asmSti();
            if (intData < 512)
            {
                static char keytable[0x54] = {
                    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
                    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
                    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
                    'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
                    0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
                    '2', '3', '0', '.'
                };

                log(keytable[intData]);
            }
            else if (intData < 1024)
            {
                m_keyboardMouseHandler->handleMouseInput(intData);
            }
            else if (intData < 1536)
            {
                TimerManager::getTimerManager()->tick();
            }
		}
    }
}

void StartupManager::reportMemory()
{
    char charMemory[30];
    extern MemoryManager* globalMemoryManager;
    int memory = globalMemoryManager->getMemorySize();

    intToCharArray(charMemory, memory);
    const char* charMemoryPreffix = "memory(MB) = ";
    char result[30];
    stringcat(charMemoryPreffix, charMemory, result);
    printString(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COL000000, result);
}

void StartupManager::countNumber()
{
    char charScreenWidth[30];
    extern int count;
    intToCharArray(charScreenWidth, count);
    m_layerManager->initWindow(m_layerWindow, m_layerWindow->getWidth(), m_layerWindow->getHeight(), "MKOS");
    printString(m_layerWindow->getBuffer(), m_layerWindow->getWidth(), 8, 30, COL000000, charScreenWidth);
    m_layerManager->repaint(m_layerWindow->getX() + 8, m_layerWindow->getY() + 30, 80, 16);
}


void StartupManager::log(int intData)
{
    char b[10];
    intToCharArray(b, intData);
    initScreen(m_layerBackground->getBuffer(), m_screenWidth, m_screenHeight);
    printString(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COLFFFFFF, b);
    m_layerManager->repaint(0, 0, m_screenWidth, m_screenHeight);
 
}

void StartupManager::log(char charData)
{
    initScreen(m_layerBackground->getBuffer(), m_screenWidth, m_screenHeight);
    printFont(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COLFFFFFF, charData);
    m_layerManager->repaint(0, 0, m_screenWidth, m_screenHeight);

}
