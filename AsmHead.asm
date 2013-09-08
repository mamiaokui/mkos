%macro Descriptor 3                                     ; 定义一个宏Descriptor 后面的3表示可以接受3个参数
	dw	%2 & 0FFFFh	                        			; 段界限 1				(2 字节) %2表示应用宏调用中的第2个参数
	dw	%1 & 0FFFFh			                        	; 段基址 1				(2 字节) &位与运算符
	db	(%1 >> 16) & 0FFh		                    	; 段基址 2				(1 字节)
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)	            ; 属性 1 + 段界限 2 + 属性 2		(2 字节)
	db	(%1 >> 24) & 0FFh			                    ; 段基址 3				(1 字节)
%endmacro ; 共 8 字节

DA_DRW		EQU	92h	; 存在的可读写数据段属性值
DA_DRWA		EQU	93h	; 存在的已访问可读写数据段类型值
DA_C		EQU	98h	; 存在的只执行代码段属性值
DA_32		EQU	4000h	; 32 位段


VMODE	EQU		0x0ff0		
SCRNX	EQU		0x0ff4		
SCRNY	EQU		0x0ff8		
VRAM	EQU		0x0ffc


IPLPOS	EQU		0x00100000	;for IPL
ASMHEADPOS	EQU		0x00008200 ;for AsmHead
BOOTPROGRAM	EQU		0x00280000	;for BootProgram
FONTDATA	EQU		0x0010000	;for FONTDATA
        


        

org	0x8200
	jmp	LABEL_BEGIN

        [SECTION .gdt]
; GDT
;                                  段基址,            段界限,              属性
LABEL_GDT:	    	Descriptor	       0,                0,                0   	; 空描述符
LABEL_DESC_CODE32:	Descriptor	       0,       0x10000000,     DA_C + DA_32	; 非一致代码段, 32  段界限为数据段地址跨度减1
LABEL_DESC_VIDEO:	Descriptor	 0xa0000,       320*200 -1,           DA_DRW	; 显存首地址
; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	    ; GDT界限
		dd	0		            ; GDT基地址

; GDT 选择子
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT    ;代码段选择子
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT    ;数据段选择子
; END of [SECTION .gdt]

[SECTION .s16]
[BITS	16]
memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy		
		RET

LABEL_BEGIN:
		MOV		AL,0x13	
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

		MOV		AL,0xff
		OUT		0x21,AL
		NOP					
		OUT		0xa1,AL

; for ipl

		MOV		ESI,0x7c00	
		MOV		EDI,IPLPOS	
		MOV		ECX,512/4
		CALL	memcpy


; for asmhead
		MOV		ESI,ASMHEADPOS
		MOV		EDI,IPLPOS+512
		MOV		ECX,bootprogram-0x8200
		CALL	memcpy

;;; for font
   		MOV		EBX, bootprogram
        MOV     ESI, EBX
        MOV     EDI, FONTDATA
        MOV     ECX, 3592/4
        CALL    memcpy


; for bootprogram

		MOV		EBX, bootprogram + 3952
        MOV     ESI, EBX
        MOV     EDI, BOOTPROGRAM
        MOV     ECX, 1024*1024*5/4
        CALL    memcpy


	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0x100

	; 初始化 32 位代码段描述符
	xor	eax, eax                                  ;目的是将eax置0，相当与mov eax 0指令，但是xor指令比mov指令快3个机器指令
	mov	ax, cs                                    ;此处cs为数据段的基地址
	shl	eax, 4                                    ;逻辑左移指令  
	add	eax, LABEL_SEG_CODE32                     ;将LABEL_SEG_CODE32的地址传递给eax
        
    ;下面将eax寄存器中的数据(段基地)分别注入数据段描述表中的各个位置
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16                                   ;逻辑右移指令
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT	
	mov	dword [GdtPtr + 2], eax	

	; 加载 GDTR
	lgdt	[GdtPtr] 

	; 关中断 
	cli                                            ;执行此命令后 cpu不响应可屏蔽中断。

	; 打开地址线A20
	in	al, 92h                                    ;从指定的外设寄存器取信息送入累加器。
	or	al, 00000010b                              ;逻辑加指令。
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0                                   ;CR0为控制寄存器
	or	eax, 1                                     
	mov	cr0, eax                                   ;对CRO寄存器的第一位置1，表示容许保护模式开启。

	; 真正进入保护模式
	jmp	dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs, 并跳转到 Code32Selector:0  处
                                        ;将代码段的选择子装入cs

[BITS	32]
[SECTION .bss]
StackSpace resb 2 * 1024
StackTop:		
        
[SECTION .s32]; 32 位代码段. 由实模式跳入.


;以下才是真正的数据段，此处与数据段段描述符相呼应。
LABEL_SEG_CODE32:
    mov esp, StackTop
    call RellocELF

	jmp	BOOTPROGRAM

; 遍历每一个 Program Header，根据 Program Header 中的信息来确定把什么放进内存，放到什么位置，以及放多少。however, something wrong when copy all the program header info, so I only copy the first one.
RellocELF:	
	xor	esi, esi
	mov	cx, word [BOOTPROGRAM + 2Ch]    ;  ecx <- pELFHdr->e_phnum
	movzx	ecx, cx					
	mov	esi, [BOOTPROGRAM + 1Ch]	    ; esi <- pELFHdr->e_phoff
	add	esi, BOOTPROGRAM		        ; esi <- OffsetOfKernel + pELFHdr->e_phoff
.Begin:
	mov	eax, [esi + 0]
	cmp	eax, 0				            ; PT_NULL
	jz	.NoAction
	push	dword [esi + 010h]		    ; size	
   	mov	eax, [esi + 04h]	
	add	eax, BOOTPROGRAM	            ; memcpy(	(void*)(pPHdr->p_vaddr),
	push	eax				            ; src		uchCode + pPHdr->p_offset,
	push	dword [esi + 08h]	        ; dst			pPHdr->p_filesz;
        
	call	MemCpy			

	add	esp, 12			
.NoAction:
	add	esi, 020h			            ; esi += pELFHdr->e_phentsize
	dec	ecx
    ;;          	jnz	.Begin        ;;fixme: delete this code is wrong.
	ret
; ------------------------------------------------------------------------
; 内存拷贝，仿 memcpy
; ------------------------------------------------------------------------
; void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
; ------------------------------------------------------------------------
MemCpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [esp + 20]	; Destination
	mov	esi, [esp + 24]	; Source
	mov	ecx, [esp + 28]	; Counter


.1:
	cmp	ecx, 0	
	jz	.2		

	mov	al, [ds:esi]
	inc	esi			
					
	mov	byte [es:edi], al
	inc	edi		

	dec	ecx	
	jmp	.1	
.2:
	mov	eax, [ebp + 8]

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			

SegCode32Len	equ	$ - LABEL_SEG_CODE32 
bootprogram:
