#include "Utils.h"
#include "Platform.h"
#include "PaintPack.h"
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

