#include "AsmTools.h"
#include "GdtIdt.h"
#include "PaintPack.h"
#include "Utils.h"
#include "InterruptionBuffer.h"
#include "KeyBoardMouseHandler.h"
#include "Timer.h"

void initPic(void)
{
	asmOut8(PIC0_IMR,  0xff  ); //disable all pic0 interuption
	asmOut8(PIC1_IMR,  0xff  ); //disable all pic1 interuption
	asmOut8(PIC0_ICW1, 0x11  ); //edge trigger mode
	asmOut8(PIC0_ICW2, 0x20  ); //irq0--7 is recept by 0x20--0x27
	asmOut8(PIC0_ICW3, 1 << 2); //pic1 is connect by irq2
	asmOut8(PIC0_ICW4, 0x01  ); //no buffer mode
	asmOut8(PIC1_ICW1, 0x11  ); //edge trigger mode
	asmOut8(PIC1_ICW2, 0x28  ); //irq8-15 is recept by 0x28-0x2f
	asmOut8(PIC1_ICW3, 2     ); //pic1 is connect by irq2
	asmOut8(PIC1_ICW4, 0x01  ); //no buffer mode
	asmOut8(PIC0_IMR,  0xfb  ); //11111011 interuption is not allowed
	asmOut8(PIC1_IMR,  0xff  ); //11111111 interuption is not allowed
	return;
}


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

void initGdtIdtInterruption()
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
	asmLoadGDTR(0xffff, (int)gdt);

	for (i = 0; i < 256; i++) {
		setIDTI(idt + i, 0, 0, 0);
	}
	setIDTI(idt + 0x20, (int) asmInt20Handler-0x200000, 2 * 8, AR_INTGATE32);
    setIDTI(idt + 0x21, (int) asmInt21Handler-0x200000, 2 * 8, AR_INTGATE32);
	setIDTI(idt + 0x27, (int) asmInt27Handler-0x200000, 2 * 8, AR_INTGATE32);
	setIDTI(idt + 0x2c, (int) asmInt2cHandler-0x200000, 2 * 8, AR_INTGATE32);

	asmLoadIDTR(0x7ff, (int)idt);

    initPic();
	asmSti(); 
	asmOut8(PIC0_IMR, 0xf8); 
	asmOut8(PIC1_IMR, 0xef); 
	return;
}

void int27Handler(int* arg)
{
	asmOut8(PIC0_OCW2, 0x67); 
	return;
}

