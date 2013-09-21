#include "AsmTools.h"
#include "Utils.h"
#include "Platform.h"

#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

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

void waitKeyboardReady(void)
{
	for (;;) {
		if ((asmIn8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void initKeyboard(void)
{
	waitKeyboardReady();
	asmOut8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	waitKeyboardReady();
	asmOut8(PORT_KEYDAT, KBC_MODE);
	return;
}


void enableMouse(void)
{
	waitKeyboardReady();
	asmOut8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	waitKeyboardReady();
	asmOut8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return;
}

