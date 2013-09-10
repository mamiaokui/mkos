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
