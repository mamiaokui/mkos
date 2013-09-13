#include "Utils.h"
#include "Platform.h"
#include "PaintPack.h"
int enableLog;
int log_1;
int log_2;
int startUpFinished;
void intToCharArray(char* dest, int number)
{
    int index = 0;
    bool isNegative = false;
    if (number < 0)
    {
        isNegative = true;
        number = 0 - number;
    }

    if (number == 0)
    {
        dest[index++] = '0';
        dest[index++] = '\0';
        return;
    }

    while(number!=0)
    {
        int digit = number - (number/10)*10;
        char digitChar = digit + '0';
        dest[index] = digitChar;
        number = number/10;
        index++;        
    }

    if (isNegative)
    {
        dest[index] = '-';
        index++;
    }
    dest[index] = '\0';

    index--;// forget '\0'
    
    int i;
    for (i = 0; i <= index / 2; i++)
    {
        int temp = dest[i];
        dest[i] = dest[index - i];
        dest[index -i] = temp;
    }

}

void stringcat(const char* begin, const char* end, char* result)
{
    int beginIndex = 0;
    int resultIndex = 0;
    while (begin[beginIndex] != '\0')
    {
        result[resultIndex] = begin[beginIndex];
        ++beginIndex;
        ++resultIndex;
    }
    int endIndex = 0;
    while(end[endIndex] != '\0')
    {
        result[resultIndex] = end[endIndex];
        ++endIndex;
        ++resultIndex;
    }
    result[resultIndex] = '\0';
}

void logInC(int a ,int b)
{
    if (enableLog == 0 && startUpFinished == 0)
        return;
    log_1 = a;
    log_2 = b;
    if (startUpFinished)
        doLog();
}

void doLog()
{
    if (log_1 == 0x98765)
        return;
    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

    int screenWidth = bootInfo->m_screenWidth;
    int screenHeight = bootInfo->m_screenHeight;
    char* vram = bootInfo->m_vram;

    char aValue[10];
    char bValue[10];
    char result[20];
    intToCharArray(aValue, log_1);
    intToCharArray(bValue, log_2);
    printString(vram, screenWidth, 8, 30, COL000000, aValue);    
    printString(vram, screenWidth, 8, 60, COL000000, bValue);    

}
