#include "AsmPack.h"
#include "PaintPack.h"
#include "GdtIdt.h"
#include "Platform.h"
#include "Utils.h"
#include "InterruptionBuffer.h"

//change from c to cpp, ld link error because can't find the implement.
void * __gxx_personality_v0=0;
void * _Unwind_Resume =0;

extern InterruptionBuffer globalInterruptionBuffer;

extern "C" void MKOSMain(void)
{
    log_1 = 0x98765;
    startUpFinished = 0;

    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

    int screenWidth = bootInfo->m_screenWidth;
    int screenHeight = bootInfo->m_screenHeight;
    char* vram = bootInfo->m_vram;
    int interruptionBufferData[128];
    globalInterruptionBuffer.initInterruptionBuffer(128, interruptionBufferData);
    initGdtIdt();
    initPic();
	asmSti(); 
	asmOut8(PIC0_IMR, 0xf9); 
	asmOut8(PIC1_IMR, 0xef); 
	initKeyboard();
	initPalette();     
    initScreen(vram, screenWidth, screenHeight);
    char charScreenWidth[10];

    intToCharArray(charScreenWidth, screenWidth);

    const char* screenWidthStr = "Screen Width = ";
    char result[30];
    stringcat(screenWidthStr, charScreenWidth, result);

    printString(vram, screenWidth, 8, 8, COL000000, result);

    char mouseCursorImage[256];
	int mx = (screenWidth - 16) / 2;
	int my = (screenHeight - 28 - 16) / 2;

    initMouseCursor(mouseCursorImage, COL008484);
    paintBlock(vram, screenWidth, 16, 16, mx, my, mouseCursorImage);


    enableMouse();
    doLog();
    startUpFinished = 1;
    while (true) {
		asmCli();
		if (globalInterruptionBuffer.isInterruptionBufferEmpty()) {
			asmStiHlt();
		} else {
			unsigned char data = globalInterruptionBuffer.getInterruptionBuffer();
			asmSti();
            int intData = (int)data;
            char b[10];
            intToCharArray(b, intData);
            initScreen(bootInfo->m_vram, bootInfo->m_screenWidth, bootInfo->m_screenHeight);
            printString(bootInfo->m_vram, bootInfo->m_screenWidth, 8, 8, COLFFFFFF, b);
		}
    }
}


