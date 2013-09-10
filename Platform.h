#ifndef PLATFORM_H
#define PLATFORM_H
typedef struct {
    int m_vmode;
    int m_screenWidth;
    int m_screenHeight;
    void* m_vram;
} BootInfo;
#define bool char
#define true 1
#define false 0
#endif
