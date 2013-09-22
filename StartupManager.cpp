#include "StartupManager.h"
#include "AsmTools.h"
#include "PaintPack.h"
#include "GdtIdt.h"
#include "Utils.h"
#include "InterruptionBuffer.h"
#include "LayerManager.h"
#include "KeyBoardMouseHandler.h"
#include "MemoryManager.h"
#include "Timer.h"

void StartupManager::init()
{
    initGdtIdtInterruption();
    Timer::getTimer();
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
                logInt(intData);
            }
            else if (intData < 1024)
            {
                m_keyboardMouseHandler->handleMouseInput(intData);
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
    m_layerManager->repaint(m_layerWindow->getX() + 8, m_layerWindow->getY() + 30, 50, 16);
}


void StartupManager::logInt(int intData)
{
    char b[10];
    intToCharArray(b, intData);
    initScreen(m_layerBackground->getBuffer(), m_screenWidth, m_screenHeight);
    printString(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COLFFFFFF, b);
    m_layerManager->repaint(0, 0, m_screenWidth, m_screenHeight);
 
}
