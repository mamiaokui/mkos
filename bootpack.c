extern void ioHlt(void);
extern void writeMemory8(int address, int value);

void hariman(void)
{
    int i;
    int j;
    for (j = 0; j < 100; j++) {
        for (i = 0xa0000; i <= 0xaffff; i++) {
            writeMemory8(i, j);
        }
    }
    while (1)
        ioHlt();
}
