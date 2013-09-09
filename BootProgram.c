extern void asmHlt(void);
extern void asmWriteMemory8(int address, int value);
extern void asmCli(void);
extern void asmOut8(int port, int data);
extern int asmLoadEflags(void);
extern void asmStoreEflags(int eflags);
extern void asmLog(int value);
extern char globalString;
extern char globalFont;

#define COL000000		0
#define COLFF0000		1
#define COL00FF00		2
#define COLFFFF00		3
#define COL0000FF		4
#define COLFF00FF		5
#define COL00FFFF		6
#define COLFFFFFF		7
#define COLC6C6C6		8
#define COL840000		9
#define COL008400		10
#define COL848400		11
#define COL000084		12
#define COL840084		13
#define COL008484		14
#define COL848484		15

#define BOOTINFO_ADDRESS 0x0ff0 //look for the head of AsmHead.asm for reason.

typedef struct  {
    int m_vmode;
    int m_screenWidth;
    int m_screenHeight;
    void* m_vram;
} BootInfo;




void initPalette(void);
void setPalette(int start, int end, unsigned char *rgb);
void drawRect(unsigned char *vram, int screenWidth, unsigned char c, int x0, int y0, int x1, int y1);
void printFont(char *vram, int xsize, int x, int y, char c, char character);
void printString(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void intToCharArray(char* dest, int number);
void stringcat(char* begin, char* end, char* result);
void initScreen(char *vram, int width, int height);
//the mouse cursor size is 16*16
void initMouseCursor(char *mouseBuffer256, char backgroundColor);

#define bool char
#define true 1
#define false 0


void MKOSMain(void)
{
    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);
    int *temp = (int*)(BOOTINFO_ADDRESS);;

    int screenWidth = bootInfo->m_screenWidth;
    int screenHeight = bootInfo->m_screenHeight;
    char* vram = (char*)bootInfo->m_vram;

	initPalette();     
    initScreen(vram, screenWidth, screenHeight);
    char charScreenWidth[10];

    intToCharArray(charScreenWidth, screenWidth);

    char* screenWidthStr = "Screen Width = ";
    char result[30];
    stringcat(screenWidthStr, charScreenWidth, result);

    printString(vram, screenWidth, 8, 8, COL000000, result);
    

    while (true)
        asmHlt();
}

	unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00, // black
		0xff, 0x00, 0x00, // bright red
		0x00, 0xff, 0x00, // bright green
		0xff, 0xff, 0x00, // bright yellow
		0x00, 0x00, 0xff, // bright blue
		0xff, 0x00, 0xff, // bright purple
		0x00, 0xff, 0xff, // light blue
		0xff, 0xff, 0xff, // white
		0xc6, 0xc6, 0xc6, // bright gray
		0x84, 0x00, 0x00, // dark red
		0x00, 0x84, 0x00, // dark green
		0x84, 0x84, 0x00, // dark yellow
		0x00, 0x00, 0x84, // dark blue
		0x84, 0x00, 0x84, // dark purple
		0x00, 0x84, 0x84, // light dark blue
		0x84, 0x84, 0x84 // dark gray
	};


void initPalette(void)
{
    setPalette(0, 15, table_rgb);
	return;
}

void setPalette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = asmLoadEflags();
	asmCli(); 					
    asmOut8(0x03c8, start);	
	for (i = start; i <= end; i++) {
        asmOut8(0x03c9, (rgb[i*3])/4);
        asmOut8(0x03c9, (rgb[i*3+1])/4);
        asmOut8(0x03c9, (rgb[i*3+2])/4);
	}

	asmStoreEflags(eflags);
	return;
}

void drawRect(unsigned char *vram, int screenWidth, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * screenWidth + x] = c;
	}
	return;
}

void printFont(char *vram, int xsize, int x, int y, char c, char ascii)
{
    char* fontBase = (char*)(&globalFont); //font data in memory
	int i;
	char *vramPoint, line ;
    char* font = fontBase + ascii * 16; //every string cost 16 * sizeof(char)
	for (i = 0; i < 16; i++) {
		vramPoint = vram + (y + i) * xsize + x;
		line = font[i];
        int j;
        for (j = 0; j <= 7; j++) {
            if ((line & 1<<(7-j)) != 0) { vramPoint[j] = c;}
        }
	}
	return;
}



void printString(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
    for(; *s != '\0'; s++) {
        printFont(vram, xsize, x, y, c, s[0]);
        x += 8;
    }
	return;
}

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

void stringcat(char* begin, char* end, char* result)
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

void initScreen(char *vram, int screenWidth, int screenHeight)
{
	drawRect(vram, screenWidth, COL008484,  0,         0,          screenWidth -  1, screenHeight - 29);
	drawRect(vram, screenWidth, COLC6C6C6,  0,         screenHeight - 28, screenWidth -  1, screenHeight - 28);
	drawRect(vram, screenWidth, COLFFFFFF,  0,         screenHeight - 27, screenWidth -  1, screenHeight - 27);
	drawRect(vram, screenWidth, COLC6C6C6,  0,         screenHeight - 26, screenWidth -  1, screenHeight -  1);

	drawRect(vram, screenWidth, COLFFFFFF,  3,         screenHeight - 24, 59,         screenHeight - 24);
	drawRect(vram, screenWidth, COLFFFFFF,  2,         screenHeight - 24,  2,         screenHeight -  4);
	drawRect(vram, screenWidth, COL848484,  3,         screenHeight -  4, 59,         screenHeight -  4);
	drawRect(vram, screenWidth, COL848484, 59,         screenHeight - 23, 59,         screenHeight -  5);
	drawRect(vram, screenWidth, COL000000,  2,         screenHeight -  3, 59,         screenHeight -  3);
	drawRect(vram, screenWidth, COL000000, 60,         screenHeight - 24, 60,         screenHeight -  3);

	drawRect(vram, screenWidth, COL848484, screenWidth - 47, screenHeight - 24, screenWidth -  4, screenHeight - 24);
	drawRect(vram, screenWidth, COL848484, screenWidth - 47, screenHeight - 23, screenWidth - 47, screenHeight -  4);
	drawRect(vram, screenWidth, COLFFFFFF, screenWidth - 47, screenHeight -  3, screenWidth -  4, screenHeight -  3);
	drawRect(vram, screenWidth, COLFFFFFF, screenWidth -  3, screenHeight - 24, screenWidth -  3, screenHeight -  3);
}

void initMouseCursor(char *mouseBuffer256, char backgroundColor)
{
    static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (x = 0; x < 16; x++) {
		for (y = 0; y < 16; y++) {
			if (cursor[x][y] == '*') {
				mouseBuffer256[x * 16 + y] = COL000000;
			}
			if (cursor[x][y] == 'O') {
				mouseBuffer256[x * 16 + y] = COLFFFFFF;
			}
			if (cursor[x][y] == '.') {
				mouseBuffer256[x * 16 + y] = backgroundColor;
			}
		}
	}
	return;

}
