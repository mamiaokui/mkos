; AsmFunction.asm
; TAB=4
[BITS 32]

		GLOBAL	asmHlt	
        GLOBAL  asmWriteMemory8
        GLOBAL  asmCli
        GLOBAL  asmOut8
        GLOBAL  asmLoadEflags
        GLOBAL  asmStoreEflags
        GLOBAL  asmLog



[SECTION .text]
asmHlt:	; void asmHlt(void);
		HLT
		RET

asmWriteMemory8: ;void asmWriteMemory8(int, int);

    mov ecx, [esp + 4]
    mov al,  [esp + 8]
    mov [ecx], al
    ret

asmCli: ;void asmCli(void);
		CLI
		RET

asmOut8: ;void asmOut8(int port, int data);
		mov		dx,[esp+4]		; port
		mov		al,[esp+8]		; data
		out		dx,al
		RET

asmLoadEflags: ;int asmLoadEflags(void);
		PUSHFD		; PUSH EFLAGS INTO STACK
		POP		EAX
		RET

asmStoreEflags: ;void asmStoreEflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EAX TO EFLAGS
		RET


asmLog: ;void asmStoreEflags(int eflags);
        MOV		EAX, [ESP+4]
        MOV [0x8888], EAX
LBhlt:  hlt
        jmp LBhlt
		RET
