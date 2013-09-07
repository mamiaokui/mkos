; ==========================================
; pmtest1.asm
; 编译方法：nasm pmtest1.asm -o pmtest1.com
; ==========================================
%include	"pm.inc"	; 常量, 宏, 以及一些说明 预处理指令都以%号打头
CYLS	EQU		0x0ff0
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2		
SCRNX	EQU		0x0ff4		
SCRNY	EQU		0x0ff6		
VRAM	EQU		0x0ff8


IPLPOS	EQU		0x00100000	;for ipl
ASMHEADPOS	EQU		0x00008200 ;for asmhead
BOTPAK	EQU		0x00280000	;for bootpack


        

org	0x8200
	jmp	LABEL_BEGIN

        [SECTION .gdt]
; GDT
;                                         段基址,      段界限     , 属性
LABEL_GDT:		Descriptor	       0,                0, 0     		; 空描述符
LABEL_DESC_CODE32:	Descriptor	       0, 0x10000000, DA_C + DA_32	; 非一致代码段, 32  段界限为数据段地址跨度减1
LABEL_DESC_VIDEO:	Descriptor	 0xa0000,           320*200 -1, DA_DRW		; 显存首地址
; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	; GDT界限
		dd	0		; GDT基地址

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


		MOV		AH,0x02
		INT		0x16 		
		MOV		[LEDS],AL


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
		MOV		ECX,bootpack-0x8200
		CALL	memcpy

; for bootpack

		MOV		EBX, bootpack
        MOV     ESI, EBX
        MOV     EDI, BOTPAK
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
	add	eax, LABEL_GDT		; eax <- gdt 基地址
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 加载 GDTR
	lgdt	[GdtPtr]                                   ;

	; 关中断 
	cli                                                ;执行此命令后 cpu不响应可屏蔽中断。

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
; END of [SECTION .s16]

        [SECTION .s32]; 32 位代码段. 由实模式跳入.
        [BITS	32]

;以下才是真正的数据段，此处与数据段段描述符相呼应。
LABEL_SEG_CODE32:
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)

	mov	edi, 320*100	; 屏幕第 101 行, 第 0 列。
    mov ax, 0x8888
;   	mov	[gs:edi], ax

    mov edi, 0
    mov ax, 8
;    mov [gs:edi], ax

	; 到此停止
	jmp	BOTPAK+0x1000

SegCode32Len	equ	$ - LABEL_SEG_CODE32  ;表示从LABEL_SEG_CODE32:到此处的地址之距离
; END of [SECTION .s32]

bootpack:
