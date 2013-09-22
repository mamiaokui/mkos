#ifndef GDTIDT_H
#define GDTIDT_H
typedef struct {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
} SegmentDescriptionItem;

typedef struct {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
} InteruptionDescriptionItem;
void initGdtIdtInterruption();

//set Global Segment Description Table Item
void setGDTI(SegmentDescriptionItem* gdti, unsigned int segmentSize, int base, int acessRight);
void setIDTI(InteruptionDescriptionItem *idti, int offset, int selector, int accessRight);

#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1


void initPic();



extern "C" void int27Handler(int* arg);
extern "C" void asmInt27Handler(void);


#endif
