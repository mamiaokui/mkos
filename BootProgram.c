extern void hlt(void);
extern void writeMemory8(int address, int value);
extern void cli(void);
extern void out8(int port, int data);
extern int loadEflags(void);
extern void storeEflags(int eflags);


void initPalette(void);
void setPalette(int start, int end, unsigned char *rgb);


void MKOSMain(void)
{
    int i;
	//initPalette(); //not working, don't know the reason.

    char *p = (char *)0xa0000;

    for (i = 0; i <= 0xffff; i++) {
        p[i] = i&0xf;
    }

    while (1)
        hlt();
}


void initPalette(void)
{
	static unsigned char table_rgb[16 * 3] = {
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
		0x84, 0x84, 0x84, // dark gray
	};
    	setPalette(0, 15, table_rgb);
	return;
}

void setPalette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = loadEflags();
	cli(); 					
        out8(0x03c8, start);	
	for (i = start; i <= end; i++) {
		out8(0x03c9, rgb[0]/4);
		out8(0x03c9, rgb[1]/4);
		out8(0x03c9, rgb[2]/4);
		rgb += 3;
	}
	storeEflags(eflags);
	return;
}
