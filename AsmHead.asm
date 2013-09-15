%macro Descriptor 3                                     ; 定义一个宏Descriptor 后面的3表示可以接受3个参数
	dw	%2 & 0FFFFh	                        			; 段界限 1				(2 字节) %2表示应用宏调用中的第2个参数
	dw	%1 & 0FFFFh			                        	; 段基址 1				(2 字节) &位与运算符
	db	(%1 >> 16) & 0FFh		                    	; 段基址 2				(1 字节)
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)	            ; 属性 1 + 段界限 2 + 属性 2		(2 字节)
	db	(%1 >> 24) & 0FFh			                    ; 段基址 3				(1 字节)
%endmacro ; 共 8 字节

DA_DRW		equ	92h	    ; 存在的可读写数据段属性值
DA_DRWA		equ	93h	    ; 存在的已访问可读写数据段类型值
DA_C		equ	98h	    ; 存在的只执行代码段属性值
DA_32		equ	4000h	; 32 位段


VMODE	equ		0x0ff0		
SCRNX	equ		0x0ff4		
SCRNY	equ		0x0ff8		
VRAM	equ		0x0ffc

GDT     equ     0x10000
IDT     equ     0x20000
        


;;; IPL_POS	    equ		0x00100000	;for IPL
;;; ASMHEAD_POS	equ		0x00008200  ;for AsmHead
BOOTPROGRAM_POS	        equ		0x00100000	;for BootProgram
BOOTPROGRAM_POS_RELLOC	equ		0x00200000	;for BootProgram
        


        

org	0x8200
	jmp	LABEL_BEGIN

[SECTION .gdt]

;                                  段基址,                     段界限,              属性
LABEL_GDT:	        	Descriptor	       0,                  0,                 0   	; 空描述符
LABEL_DESC_CODE32:  	Descriptor	       0,         0x10000000,      DA_C + DA_32	; 非一致代码段, 32  段界限为数据段地址跨度减1
LABEL_DESC_BOOTPROGRAM:	Descriptor	 BOOTPROGRAM_POS_RELLOC,     0x100000,      DA_C + DA_32	; 非一致代码段, 32  段界限为数据段地址跨度减1
LABEL_DESC_VIDEO:   	Descriptor	 0xa0000,          320*200 -1,           DA_DRW	; 显存首地址
; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	    ; GDT界限
		dd	0		            ; GDT基地址

; GDT 选择子
SELECTOR_CODE_32		equ	LABEL_DESC_CODE32	- LABEL_GDT    ;代码段选择子
SELECTOR_BOOTPROGRAM		equ	LABEL_DESC_BOOTPROGRAM	- LABEL_GDT    ;代码段选择子
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT    ;数据段选择子
; END of [SECTION .gdt]

[SECTION .s16]
[BITS	16]
memset0:
		mov		eax,0x0
		mov		[edi],eax
		add		edi, 4
		sub		ecx,1
		JNZ		memset0		
		ret

        
        
memcpy16:
		mov		eax,[esi]
		add		esi, 4
		mov		[edi],eax
		add		edi, 4
		sub		ecx,1
		JNZ		memcpy16		
		ret

LABEL_BEGIN:
		mov		BYTE [VMODE],8
		mov		WORD [SCRNX],320
		mov		WORD [SCRNY],200
		mov		DWORD [VRAM],0x000a0000
        
        mov edi, 0xa0000
        mov ecx, 320*200/4
        call memset0

        
        mov		AL,0x13	
		mov		AH,0x00
		INT		0x10

        mov edi, 0xa0000
        mov ecx, 320*200/4
        call memset0



; for bootprogram

		mov		ebx, bootprogram
        mov     esi, ebx
        mov     edi, BOOTPROGRAM_POS
        mov     ecx, 1024*1024*5/4
        CALL    memcpy16



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
	jmp	dword SELECTOR_CODE_32:0	    ; 执行这一句会把 SELECTOR_CODE_32 装入 cs, 并跳转到 Code32Selector:0  处
                                        ;将代码段的选择子装入cs

[BITS	32]
[SECTION .s32]; 32 位代码段. 由实模式跳入.


;以下才是真正的数据段，此处与数据段段描述符相呼应。
LABEL_SEG_CODE32:
    call RellocELF
    mov esp, 0x3FFFF0;0x200000 to near 0x400000 is stack.
    jmp 16:0

; 遍历每一个 Program Header，根据 Program Header 中的信息来确定把什么放进内存，放到什么位置，以及放多少。
RellocELF:	
	xor	esi, esi
	mov	cx, word [BOOTPROGRAM_POS + 2Ch]    ;  ecx <- pELFHdr->e_phnum
    ;; dec cs because the last one like GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x4 cause copy problem.
    dec cx
	movzx	ecx, cx					
	mov	esi, [BOOTPROGRAM_POS + 1Ch]	    ; esi <- pELFHdr->e_phoff
	add	esi, BOOTPROGRAM_POS		        ; esi <- OffsetOfKernel + pELFHdr->e_phoff
.Begin:
	mov	eax, [esi + 0]
	cmp	eax, 0				            ; PT_NULL
	jz	.NoAction
	push	dword [esi + 010h]		    ; size	
   	mov	eax, [esi + 04h]	
	add	eax, BOOTPROGRAM_POS	        ; memcpy(	(void*)(pPHdr->p_vaddr),
	push	eax				            ; src		uchCode + pPHdr->p_offset,
	push	dword [esi + 08h]	        ; dst			pPHdr->p_filesz;
        
	call	memcpy32

	add	esp, 12			
.NoAction:
	add	esi, 020h			            ; esi += pELFHdr->e_phentsize
	dec	ecx

    jnz	.Begin     
	ret

memcpy32:
   	push	ebp
	mov	ebp, esp

   	push	esi
	push	edi
	push	ecx
    mov ecx, [ebp + 16]
    mov esi, [ebp + 12]
    mov edi, [ebp + 8]
copying:        
    mov		eax,[esi]
    inc		esi
    mov		[edi],eax
    inc		edi
    sub		ecx,1
    JNZ		copying
cpyend:
   	mov	eax, [ebp + 8]	; 返回值
   	pop	ecx
	pop	edi
	pop	esi
   	mov	esp, ebp
	pop	ebp

    ret

bootprogram:
