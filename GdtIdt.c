#include "GdtIdt.h"
#include "Platform.h"
#include "PaintPack.h"
#include "Utils.h"
void setGDTI(SegmentDescriptionItem* gdti, unsigned int segmentSize, int base, int acessRight)
{
	if (segmentSize > 0xfffff) {
		acessRight |= 0x8000; /* G_bit = 1 */
		segmentSize /= 0x1000;
	}
	gdti->limit_low    = segmentSize & 0xffff;
	gdti->base_low     = base & 0xffff;
	gdti->base_mid     = (base >> 16) & 0xff;
	gdti->access_right = acessRight & 0xff;
	gdti->limit_high   = ((segmentSize >> 16) & 0x0f) | ((acessRight >> 8) & 0xf0);
	gdti->base_high    = (base >> 24) & 0xff;
	return;
}

void setIDTI(InteruptionDescriptionItem *idti, int offset, int selector, int accessRight)
{
    idti->offset_low   = offset & 0xffff;
	idti->selector     = selector;
	idti->dw_count     = (accessRight >> 8) & 0xff;
	idti->access_right = accessRight & 0xff;
	idti->offset_high  = (offset >> 16) & 0xffff;
}

void initGdtIdt()
{
	SegmentDescriptionItem* gdt = (SegmentDescriptionItem*) 0x10000;
	InteruptionDescriptionItem* idt = (InteruptionDescriptionItem*) 0x0020000;
	int i;
    //forget the small three bits
	for (i = 0; i < (1<<13); i++) {
		setGDTI(gdt + i, 0, 0, 0);
	}
	setGDTI(gdt + 1, 0xffffffff, 0x00000000, 0x409a);
    setGDTI(gdt + 2, 0x00100000, 0x00200000, 0x409a);
	asmLoadGDTR(0xffff, gdt);

	for (i = 0; i < 256; i++) {
		setIDTI(idt + i, 0, 0, 0);
	}
    setIDTI(idt + 0x21, (int) asmInt21Handler-0x200000, 2 * 8, AR_INTGATE32);
	setIDTI(idt + 0x27, (int) asmInt27Handler-0x200000, 2 * 8, AR_INTGATE32);
	setIDTI(idt + 0x2c, (int) asmInt2cHandler-0x200000, 2 * 8, AR_INTGATE32);

	asmLoadIDTR(0x7ff, idt);
	return;
}

#define PORT_KEYBOARD		0x0060

void int21Handler(int* arg)
{
    asmOut8(PIC0_OCW2, 0x61);
    unsigned char data;
	data = asmIn8(PORT_KEYBOARD);
    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);

    int intData = (int)data;
    char b[10];
    intToCharArray(b, intData);
    initScreen((char*)0xa0000, 320, 200);
    printString(bootInfo->m_vram, bootInfo->m_screenWidth, 8, 8, COLFFFFFF, b);
}

void int27Handler(int* arg)
{
	asmOut8(PIC0_OCW2, 0x67); 
	return;
}

void int2cHandler(int* arg)
{

    BootInfo* bootInfo = (BootInfo*)(BOOTINFO_ADDRESS);
    printString(bootInfo->m_vram, bootInfo->m_screenWidth, 8, 8, COLFFFFFF, "INT:2c IRQ:12  mouse");
	for (;;) {
        asmHlt();
	}

}
