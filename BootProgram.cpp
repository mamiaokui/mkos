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

class MouseDataDecoder 
{
private:
    int m_mouseState;
    int m_mouseData[3];
public:
    MouseDataDecoder()
    {
        m_mouseState = 0;
        for (int i = 0; i < 3; i++)
        {
            m_mouseData[i] = 0;
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
            m_mouseData[0] = data;
            m_mouseState = 2;
        }
        else if (m_mouseState == 2)
        {
            m_mouseData[1] = data;
            m_mouseState = 3;
        }
        else if (m_mouseState == 3)
        {
            m_mouseData[2] = data;
            m_mouseState = 1;
            ok = true;
        }
        return ok;
    }

    int* getMouseData()
    {
        return m_mouseData;
    }
};

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
                initScreen(bootInfo->m_vram, bootInfo->m_screenWidth, bootInfo->m_screenHeight);
                printString(bootInfo->m_vram, bootInfo->m_screenWidth, 8, 8, COLFFFFFF, b);
            }
            else if (intData >= 512)
            {
                bool output =  mouseDataDecoder.receiveMouseInterruption(intData - 512);
                if (output)
                {
                    initScreen(bootInfo->m_vram, bootInfo->m_screenWidth, bootInfo->m_screenHeight);
                    int* outputData = mouseDataDecoder.getMouseData();
                    char b0[10];
                    intToCharArray(b0, outputData[0]);
                    printString(bootInfo->m_vram, bootInfo->m_screenWidth, 8, 30, COLFFFFFF, b0);

                    char b1[10];
                    intToCharArray(b1, outputData[1]);
                    printString(bootInfo->m_vram, bootInfo->m_screenWidth, 60, 30, COLFFFFFF, b1);

                    char b2[10];
                    intToCharArray(b2, outputData[2]);
                    printString(bootInfo->m_vram, bootInfo->m_screenWidth, 90, 30, COLFFFFFF, b2);

                }
            }
		}
    }
}


