; naskfunc
; TAB=4
[BITS 32]

		GLOBAL	hlt	
        GLOBAL  writeMemory8
        GLOBAL  cli
        GLOBAL  out8
        GLOBAL  loadEflags
        GLOBAL  storeEflags



[SECTION .text]
hlt:	; void hlt(void);
		HLT
		RET

writeMemory8: ;void writeMemory8(int, int);

    mov ecx, [esp + 4]
    mov al,  [esp + 8]
    mov [ecx], al
    ret

cli: ;void cli(void);
		CLI
		RET

out8: ;void out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

loadEflags: ;int loadEflags(void);
		PUSHFD		; PUSH EFLAGS INTO STACK
		POP		EAX
		RET

storeEflags: ;void storeEflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EAX TO EFLAGS
		RET
