#include "AsmPack.h"
#include "PaintPack.h"
#include "GdtIdt.h"
#include "Platform.h"
#include "Utils.h"
#include "InterruptionBuffer.h"

extern InterruptionBuffer globalInterruptionBuffer;

void MKOSMain(void)
{
    log_1 = 0x98765;
    startUpFinished = 0;

    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

    int screenWidth = bootInfo->m_screenWidth;
    int screenHeight = bootInfo->m_screenHeight;
    char* vram = (char*)bootInfo->m_vram;
    char interruptionBufferData[128];
    initGdtIdt();
    initPic();
	asmSti(); 
    globalInterruptionBuffer.initInterruptionBuffer(128, interruptionBufferData);
	initPalette();     
    initScreen(vram, screenWidth, screenHeight);
    char charScreenWidth[10];

    intToCharArray(charScreenWidth, screenWidth);

    char* screenWidthStr = "Screen Width = ";
    char result[30];
    stringcat(screenWidthStr, charScreenWidth, result);

    printString(vram, screenWidth, 8, 8, COL000000, result);

    char mouseCursorImage[256];
	int mx = (screenWidth - 16) / 2;
	int my = (screenHeight - 28 - 16) / 2;

    initMouseCursor(mouseCursorImage, COL008484);
    paintBlock(vram, screenWidth, 16, 16, mx, my, mouseCursorImage);


    
    doLog();
    startUpFinished = 1;
	asmOut8(PIC0_IMR, 0xf9); 
	asmOut8(PIC1_IMR, 0xef); 

    while (true) {
		asmCli();
		if (isInterruptionBufferEmpty(&globalInterruptionBuffer)) {
			asmStiHlt();
		} else {
			char data = getInterruptionBuffer(&globalInterruptionBuffer);
			asmSti();
            int intData = (int)data;
            char b[10];
            intToCharArray(b, intData);
            initScreen(bootInfo->m_vram, bootInfo->m_screenWidth, bootInfo->m_screenHeight);
            printString(bootInfo->m_vram, bootInfo->m_screenWidth, 8, 8, COLFFFFFF, b);
		}
    }
}


