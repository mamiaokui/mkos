extern void asmHlt(void);
extern void asmWriteMemory8(int address, int value);
extern void asmCli(void);
extern void asmOut8(int port, int data);
extern int asmLoadEflags(void);
extern void asmStoreEflags(int eflags);
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

#define SCREENWIDTH    320
#define SCREENHEIGHT   200
#define VRAM (char*)(0xa0000)




void initPalette(void);
void setPalette(int start, int end, unsigned char *rgb);
void drawRect(unsigned char *vram, int screenWidth, unsigned char c, int x0, int y0, int x1, int y1);


void MKOSMain(void)
{
    int i;
	initPalette(); //not working, don't know the reason.

    char *p = (char *)0xa0000;

	drawRect(VRAM, SCREENWIDTH, COL008484,  0,         0,          SCREENWIDTH -  1, SCREENHEIGHT - 29);
	drawRect(VRAM, SCREENWIDTH, COLC6C6C6,  0,         SCREENHEIGHT - 28, SCREENWIDTH -  1, SCREENHEIGHT - 28);
	drawRect(VRAM, SCREENWIDTH, COLFFFFFF,  0,         SCREENHEIGHT - 27, SCREENWIDTH -  1, SCREENHEIGHT - 27);
	drawRect(VRAM, SCREENWIDTH, COLC6C6C6,  0,         SCREENHEIGHT - 26, SCREENWIDTH -  1, SCREENHEIGHT -  1);

	drawRect(VRAM, SCREENWIDTH, COLFFFFFF,  3,         SCREENHEIGHT - 24, 59,         SCREENHEIGHT - 24);
	drawRect(VRAM, SCREENWIDTH, COLFFFFFF,  2,         SCREENHEIGHT - 24,  2,         SCREENHEIGHT -  4);
	drawRect(VRAM, SCREENWIDTH, COL848484,  3,         SCREENHEIGHT -  4, 59,         SCREENHEIGHT -  4);
	drawRect(VRAM, SCREENWIDTH, COL848484, 59,         SCREENHEIGHT - 23, 59,         SCREENHEIGHT -  5);
	drawRect(VRAM, SCREENWIDTH, COL000000,  2,         SCREENHEIGHT -  3, 59,         SCREENHEIGHT -  3);
	drawRect(VRAM, SCREENWIDTH, COL000000, 60,         SCREENHEIGHT - 24, 60,         SCREENHEIGHT -  3);

	drawRect(VRAM, SCREENWIDTH, COL848484, SCREENWIDTH - 47, SCREENHEIGHT - 24, SCREENWIDTH -  4, SCREENHEIGHT - 24);
	drawRect(VRAM, SCREENWIDTH, COL848484, SCREENWIDTH - 47, SCREENHEIGHT - 23, SCREENWIDTH - 47, SCREENHEIGHT -  4);
	drawRect(VRAM, SCREENWIDTH, COLFFFFFF, SCREENWIDTH - 47, SCREENHEIGHT -  3, SCREENWIDTH -  4, SCREENHEIGHT -  3);
	drawRect(VRAM, SCREENWIDTH, COLFFFFFF, SCREENWIDTH -  3, SCREENHEIGHT - 24, SCREENWIDTH -  3, SCREENHEIGHT -  3);



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
