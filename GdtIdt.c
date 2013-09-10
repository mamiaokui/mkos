#include "GdtIdt.h"
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
	setGDTI(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
    setGDTI(gdt + 2, 0x00100000, 0x00280000, 0x409a);
	asmLoadGDTR(0xffff, 0x0010000);

	for (i = 0; i < 256; i++) {
		setIDTI(idt + i, 0, 0, 0);
	}
	asmLoadIDTR(0x7ff, 0x20000);
	return;
}
