#include "AsmPack.h"
#include "Utils.h"

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
