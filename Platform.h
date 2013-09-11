#ifndef PLATFORM_H
#define PLATFORM_H
#define BOOTINFO_ADDRESS 0x0ff0 //look for the head of AsmHead.asm for reason.
typedef struct {
    int m_vmode;
    int m_screenWidth;
    int m_screenHeight;
    void* m_vram;
} BootInfo;
#define bool char
#define true 1
#define false 0

#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e


void initPic();
#endif
