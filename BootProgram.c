extern void asmHlt(void);
extern void asmWriteMemory8(int address, int value);
extern void asmCli(void);
extern void asmOut8(int port, int data);
extern int asmLoadEflags(void);
extern void asmStoreEflags(int eflags);



void initPalette(void);
void setPalette(int start, int end, unsigned char *rgb);


void MKOSMain(void)
{
    int i;
	initPalette(); //not working, don't know the reason.

    char *p = (char *)0xa0000;

    for (i = 0; i <= 0xffff; i++) {
        asmWriteMemory8(p+i, i&0x0f);
    }

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
