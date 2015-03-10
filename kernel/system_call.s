#
# Small/kernel/system_call.s
# 
# (C) 2012-2013 Yafei Zheng
# V0.2 2013-01-31 23:18:14
#
# Email: e9999e@163.com, QQ: 1039332004
#

#
# 此文件包含部分系统调用以及中断处理程序。
#

.globl keyboard_int, timer_int

# 键盘中断处理程序
.align 4
keyboard_int:
	pushl	%ds
	pushl	%eax	
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	movw	$0x10,%bx			# 全局数据段选择符
	movw	%bx,%ds
	xorb	%al,%al
	inb		$0x64,%al			# 8042，测试64h端口位0，查看输出缓冲是否满，若满则读出一个字符
	andb	$0x01,%al
	cmpb	$0, %al
	je		1f
	call	keyboard_hander		# 输出缓冲满，则调用键盘处理程序读出一个字符
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

timer_int:
#	pushl	$1
#	call	move_cursor
#	addl	$4,%esp
	movb	$0x20,%al			# 向8259A主芯片发送EOI命令,将其ISR中的相应位清零
	outb	%al,$0x20
	iret
