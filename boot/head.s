#
# Small/boot/head.s
#
# (C) 2012-2013 Yafei Zheng
# V0.0 2012-12-7 10:44:39
# V0.1 2013-01-30 02:40:57
#
# Email: e9999e@163.com, QQ: 1039332004
#

# **************************************************************************************************
# 编译器：GNU as 2.21.1
#
# head.s的主要工作如下：
#	1.重新设置GDT，IDT，内核堆栈（任务0用户态堆栈）
#	2.设置默认中断
# **************************************************************************************************

.globl startup_32

SCRN_SEL	= 0x18				# 屏幕显示内存段选择符

.text
startup_32:
	movl	$0x10,%eax
	mov		%ax,%ds
	lss		init_stack,%esp
# 在新的位置重新设置GDT,IDT
	call	setup_gdt
	call	setup_idt
	movl	$0x10,%eax
	mov		%ax,%ds
	mov		%ax,%es
	mov		%ax,%fs
	mov		%ax,%gs
	lss		init_stack,%esp
# OK! We begin to run the MAIN function now.
	movl	$0,scr_loc
	pushl	$main
#debug:
#	jmp		debug
	ret

# ----------------------------------------------------
.align 4
setup_gdt:
	lgdt	gdt_new_48
	ret

.align 4
setup_idt:
	pushl	%edx
	pushl	%eax
	pushl	%ecx
	pushl	%edi
	lea		ignore_int,%edx
	movl	$0x00080000,%eax
	mov		%dx,%ax
	movw	$0x8e00,%dx			# 中断门类型，特权级0
	lea		idt,%edi
	movl	$256,%ecx
rp:	movl	%eax,(%edi)
	movl	%edx,4(%edi)
	addl	$8,%edi
	dec		%ecx
	cmpl	$0,%ecx
	jne		rp
	lidt	idt_new_48
	popl	%edi
	popl	%ecx
	popl	%eax
	popl	%edx
	ret

.align 4
write_char:
	pushl	%ebx
	pushl	%gs
	movw	$SCRN_SEL,%bx
	mov		%bx,%gs
	movl	scr_loc,%ebx
	shl		$1,%ebx
	movw	%ax,%gs:(%ebx)
	shr		$1,%ebx
	inc		%ebx
	cmpl	$2000,%ebx
	jne		1f
	movl	$0,%ebx
1:	movl	%ebx,scr_loc
	popl	%gs
	popl	%ebx
	ret

.align 4
ignore_int:
	pushl	%ds
	pushl	%eax	
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	movl	$0x0449,%eax		# 字符"I"，属性：红色 黑底 不闪烁 不高亮
	movl	$0x10,%ebx
	mov		%bx,%ds
	call	write_char
	xorb	%al,%al
	inb		$0x64,%al			# 8042，测试64h端口位0，查看输出缓冲是否满，若满则读出一个字符
	andb	$0x01,%al
	cmpb	$0, %al
	je		1f
	inb		$0x60,%al			# 输出缓冲满，读出一个字符
	mov		$0x02,%ah
	movl	$0x10,%ebx
	mov		%bx,%ds
	call	write_char
# 以下注释的代码是屏蔽键盘输入，然后再允许，用于复位键盘输入。在8042中也可以不用
#	inb		$0x61,%al
#	orb		$0x80,%al
#	.word	0x00eb,0x00eb		# 此处是2条jmp $+2，$为当前指令地址，起延时作用，下同
#	outb	%al,$0x61
#	andb	$0x7f,%al
#	.word	0x00eb,0x00eb
#	outb	%al,$0x61
1:	movb	$0x20,%al			# 向8259A主芯片发送EOI命令,将其ISR中的相应位清零
	outb	%al,$0x20
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	popl	%ds
	iret

# ----------------------------------------------------
scr_loc:						# 屏幕当前显示位置，从左上角到右下角依次显示哑中断字符
	.long 0

# GDT,IDT定义
.align 4
gdt_new_48:
	.word 256*8-1
	.long gdt

idt_new_48:
	.word 256*8-1
	.long idt

.align 8
gdt:
	.quad 0x0000000000000000
	.quad 0x00c09a00000007ff	# 代码段，选择符0x08。
	.quad 0x00c09200000007ff	# 数据段，选择符0x10。
	.quad 0x00c0920b80000002	# 显示内存段，选择符0x18。临时
	.fill 252,8,0

idt:
	.fill 256,8,0

# 内核初始化堆栈，也是后来任务0的用户栈
.align 4
	.fill 128,4,0
init_stack:
	.long init_stack
	.word 0x10
