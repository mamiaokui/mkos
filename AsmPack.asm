; AsmFunction.asm
; TAB=4
[BITS 32]

		GLOBAL	asmHlt	
        GLOBAL  asmWriteMemory8
        GLOBAL  asmCli
        GLOBAL  asmStiHlt
        GLOBAL  asmOut8
        GLOBAL  asmIn8
        GLOBAL  asmLoadEflags
        GLOBAL  asmStoreEflags
        GLOBAL  asmLoadCr0
        GLOBAL  asmStoreCr0
        GLOBAL  asmLog
        GLOBAL  asmLoadGDTR
        GLOBAL  asmLoadIDTR
        GLOBAL  asmInt21Handler
        GLOBAL  asmInt27Handler
        GLOBAL  asmInt2cHandler
        GLOBAL  asmInt20Handler
        GLOBAL  globalString
        GLOBAL  asmSti
        EXTERN  int21Handler
        EXTERN  int27Handler
        EXTERN  int2cHandler
        EXTERN  int20Handler





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

asmStiHlt:
        STI
		HLT
		RET


asmOut8: ;void asmOut8(int port, int data);
		mov		dx,[esp+4]		; port
		mov		al,[esp+8]		; data

		out		dx,al
        nop
        nop
		RET

asmIn8:        ; int asmIn8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
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

asmLoadCr0: 
        MOV EAX, CR0
		RET

asmStoreCr0: 
		MOV		EAX,[ESP+4]
        MOV     CR0, EAX
		RET


asmLog: ;void asmStoreEflags(int eflags);
        MOV		EAX, [ESP+4]
        MOV [0x8888], EAX
        jmp LBhlt

asmLoadGDTR:
        MOV		AX,[ESP+4]		
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

asmLoadIDTR:
        MOV		AX,[ESP+4]
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

        
LBhlt:  hlt
        jmp LBhlt
		RET

asmInt21Handler:
        PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	int21Handler
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

asmInt27Handler:
        PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	int27Handler
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

asmInt2cHandler:
        PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	int2cHandler
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

asmInt20Handler:
        PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	int20Handler
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD


asmSti:
		STI
		RET


[SECTION .data]
globalString:
        db "I love mamk", 0x0

