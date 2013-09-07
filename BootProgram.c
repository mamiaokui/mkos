extern void asmHlt(void);
extern void asmWriteMemory8(int address, int value);
extern void asmCli(void);
extern void asmOut8(int port, int data);
extern int asmLoadEflags(void);
extern void asmStoreEflags(int eflags);
extern void asmLog(int value);
/*
#define BLACK 0
#define BLACKRED 1
#define BRIGHTGREEN 2
#define BRIGHTYELLOW 3
#define BRIGHTBLUE 4
#define BRIGHTPURPLE 5
#define LIGHTBLUE 6
#define WHITE 7
#define BRIGHTGRAY 8
#define DARKRED 9
#define DARKGREEN 10
#define DARKYELLOW 11
#define DARKBLUE 12
#define DARKPURPLE 13 
#define LIGHTDARKBLUE 14
#define DARKGRAY 15
*/

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
#define FONT_ADDRESS 0x0010000

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


void MKOSMain(void)
{
//	extern char asmFont[4096];
    int i;

    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);
    int *temp = (int*)(BOOTINFO_ADDRESS);;

    int screenWidth = bootInfo->m_screenWidth;
    int screenHeight = bootInfo->m_screenHeight;
    char* vram = (char*)bootInfo->m_vram;

	initPalette();     

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
    char* font = (char*)(0x10000);

	printFont(vram, screenWidth,  8, 8, COLFFFFFF, 'A');

    while (1)
        asmHlt();
}


void initPalette(void)
{
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


void printFont(char *vram, int xsize, int x, int y, char c, char character)
{
    char* fontData = (char*)(0x10000);
	int i;
	char *p, d /* data */;
    char* font = fontData + character * 16;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void printString(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
    /*
	for (; *s != 0x00; s++) {
		printFont(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
    */

	return;
}
