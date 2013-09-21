#ifndef PLATFORM_H
#define PLATFORM_H
#define BOOTINFO_ADDRESS 0x0ff0 //look for the head of AsmHead.asm for reason.
typedef struct {
    int m_vmode;
    int m_screenWidth;
    int m_screenHeight;
    unsigned char* m_vram;
} BootInfo;

void waitKeyboardReady(void);
void initKeyboard(void);
void enableMouse(void);
#endif
