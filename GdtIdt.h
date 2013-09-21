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
void initGdtIdt();

//set Global Segment Description Table Item
void setGDTI(SegmentDescriptionItem* gdti, unsigned int segmentSize, int base, int acessRight);
void setIDTI(InteruptionDescriptionItem *idti, int offset, int selector, int accessRight);

extern "C" void int21Handler(int* arg);
extern "C" void asmInt21Handler(void);

extern "C" void int27Handler(int* arg);
extern "C" void asmInt27Handler(void);

extern "C" void int2cHandler(int* arg);
extern "C" void asmInt2cHandler(void);

#endif
