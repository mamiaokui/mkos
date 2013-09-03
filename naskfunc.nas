; naskfunc
; TAB=4

[BITS 32]

		GLOBAL	ioHlt	
        GLOBAL  writeMemory8


[SECTION .text]
ioHlt:	; void ioHlt(void);

		HLT
		RET

writeMemory8: ;void writeMemory8(int, int);

    mov ecx, [esp + 4]
    mov al,  [esp + 8]
    mov [ecx], al
    ret