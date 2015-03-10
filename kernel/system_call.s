#
# Small/kernel/system_call.s
# 
# (C) 2012-2013 Yafei Zheng
# V0.2 2013-01-31 23:18:14
#
# Email: e9999e@163.com, QQ: 1039332004
#

#
# ���ļ���������ϵͳ�����Լ��жϴ������
#

.globl keyboard_int, timer_int

# �����жϴ������
.align 4
keyboard_int:
	pushl	%ds
	pushl	%eax	
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	movw	$0x10,%bx			# ȫ�����ݶ�ѡ���
	movw	%bx,%ds
	xorb	%al,%al
	inb		$0x64,%al			# 8042������64h�˿�λ0���鿴��������Ƿ��������������һ���ַ�
	andb	$0x01,%al
	cmpb	$0, %al
	je		1f
	call	keyboard_hander		# ���������������ü��̴���������һ���ַ�
# ����ע�͵Ĵ��������μ������룬Ȼ�����������ڸ�λ�������롣��8042��Ҳ���Բ���
#	inb		$0x61,%al
#	orb		$0x80,%al
#	.word	0x00eb,0x00eb		# �˴���2��jmp $+2��$Ϊ��ǰָ���ַ������ʱ���ã���ͬ
#	outb	%al,$0x61
#	andb	$0x7f,%al
#	.word	0x00eb,0x00eb
#	outb	%al,$0x61
1:	movb	$0x20,%al			# ��8259A��оƬ����EOI����,����ISR�е���Ӧλ����
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
	movb	$0x20,%al			# ��8259A��оƬ����EOI����,����ISR�е���Ӧλ����
	outb	%al,$0x20
	iret
