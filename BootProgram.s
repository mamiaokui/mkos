	.file	"BootProgram.c"
	.globl	table_rgb
	.data
	.align 32
	.type	table_rgb, @object
	.size	table_rgb, 48
table_rgb:
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	0
	.byte	-1
	.byte	-1
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	-1
	.byte	0
	.byte	-1
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-58
	.byte	-58
	.byte	-58
	.byte	-124
	.byte	0
	.byte	0
	.byte	0
	.byte	-124
	.byte	0
	.byte	-124
	.byte	-124
	.byte	0
	.byte	0
	.byte	0
	.byte	-124
	.byte	-124
	.byte	0
	.byte	-124
	.byte	0
	.byte	-124
	.byte	-124
	.byte	-124
	.byte	-124
	.byte	-124
	.text
	.globl	MKOSMain
	.type	MKOSMain, @function
MKOSMain:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	call	initPalette
	movl	$655360, -16(%ebp)
	movl	$0, -12(%ebp)
	jmp	.L2
.L3:
	movl	-12(%ebp), %eax
	movl	%eax, %edx
	andl	$15, %edx
	movl	-12(%ebp), %ecx
	movl	-16(%ebp), %eax
	addl	%ecx, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	asmWriteMemory8
	addl	$1, -12(%ebp)
.L2:
	cmpl	$65535, -12(%ebp)
	jle	.L3
.L4:
	call	asmHlt
	jmp	.L4
	.cfi_endproc
.LFE0:
	.size	MKOSMain, .-MKOSMain
	.globl	initPalette
	.type	initPalette, @function
initPalette:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	$table_rgb, 8(%esp)
	movl	$15, 4(%esp)
	movl	$0, (%esp)
	call	setPalette
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	initPalette, .-initPalette
	.globl	setPalette
	.type	setPalette, @function
setPalette:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	call	asmLoadEflags
	movl	%eax, -16(%ebp)
	call	asmCli
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$968, (%esp)
	call	asmOut8
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	jmp	.L8
.L9:
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	%eax, %edx
	movl	16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	shrb	$2, %al
	movzbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$969, (%esp)
	call	asmOut8
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	leal	1(%eax), %edx
	movl	16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	shrb	$2, %al
	movzbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$969, (%esp)
	call	asmOut8
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	leal	2(%eax), %edx
	movl	16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	shrb	$2, %al
	movzbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$969, (%esp)
	call	asmOut8
	addl	$1, -12(%ebp)
.L8:
	movl	-12(%ebp), %eax
	cmpl	12(%ebp), %eax
	jle	.L9
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	asmStoreEflags
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	setPalette, .-setPalette
	.ident	"GCC: (Ubuntu/Linaro 4.7.3-1ubuntu1) 4.7.3"
	.section	.note.GNU-stack,"",@progbits
