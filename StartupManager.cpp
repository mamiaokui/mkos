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
#include "TaskManager.h"
extern "C" void task_b_main(void)
{
    for (;;)
    {
        asmHlt();
    }
}

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
    
    TaskManager taskManager;
    TSS32* taska = taskManager.getTSS();
    taska->ldtr = 0;
    taska->iomap = 0x40000000;

    TSS32* taskb = taskManager.getTSS();
    taskb->ldtr = 0;
    taskb->iomap = 0x40000000;

    SegmentDescriptionItem* gdt = (SegmentDescriptionItem*)GDT_ADDR;
	setGDTI(gdt + 3, 103, (int)taska, AR_TSS32);
	setGDTI(gdt + 4, 103, (int)taskb, AR_TSS32);
	asmLoadTR(3 * 8);
    extern MemoryManager* globalMemoryManager;
    int task_b_esp = globalMemoryManager->malloc(64*1024) + 64*1024;
//	int task_b_esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
    void(*ptr)(void) = (void(*)())(int(&task_b_main) - 0x200000);
	taskb->eip = int(ptr);
	taskb->eflags = 0x00000202; /* IF = 1; */
	taskb->eax = 0;
	taskb->ecx = 0;
	taskb->edx = 0;
	taskb->ebx = 0;
	taskb->esp = task_b_esp;
	taskb->ebp = 0;
	taskb->esi = 0;
	taskb->edi = 0;
	taskb->es = 0 * 8;
	taskb->cs = 2 * 8;
	taskb->ss = 0 * 8;
	taskb->ds = 0 * 8;
	taskb->fs = 0 * 8;
	taskb->gs = 0 * 8;

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
			int data = m_interruptionBuffer->getInterruptionBufferData();
			asmSti();
            if (data < 512)
            {
                logKeyboard(data);
            }
            else if (data < 1024)
            {
                bool oneMotion = m_keyboardMouseHandler->handleMouseInput(data);
                if (oneMotion)
                {
                    m_layerWindow->setPosition(m_keyboardMouseHandler->getX()-160/2, m_keyboardMouseHandler->getY());
                }
            }
            else if (data < 1536)
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
    return;
    char b[10];
    intToCharArray(b, intData);
    initScreen(m_layerBackground->getBuffer(), m_screenWidth, m_screenHeight);
	drawRect(m_layerBackground->getBuffer(), m_screenWidth, COL008484,  0,         0,          20, 20);
    printString(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COLFFFFFF, b);
    m_layerManager->repaint(0, 0, m_screenWidth, m_screenHeight);
 
}

void StartupManager::log(char charData)
{
	drawRect(m_layerBackground->getBuffer(), m_screenWidth, COL008484,  0,         0,          150, 30);
    printFont(m_layerBackground->getBuffer(), m_screenWidth, 8, 8, COLFFFFFF, charData);
    m_layerManager->repaint(0, 0, 150, 30);
}

void StartupManager::logKeyboard(int data)
{
    static char keytable[0x54] = {
        0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.'
    };
    if (keytable[data] != 0)
        log(keytable[data]);

}
