extern void hlt(void);
extern void writeMemory8(int address, int value);
extern void cli(void);
extern void out8(int port, int data);
extern int loadEflags(void);
extern void storeEflags(int eflags);


void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);


void MKOSMain(void)
{
    int i;
    int j;
    for (j = 0; j < 100; j++) {
        for (i = 0xa0000; i <= 0xaffff; i++) {
            writeMemory8(i, j);
        }
    }
    while (1)
        hlt();
}
