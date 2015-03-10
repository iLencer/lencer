/*
 * Small/include/sys_set.h
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-01-31 21:23:13
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

/*
 * ��ͷ�ļ�����ϵͳ���õĺ궨��ȡ�
 *
 * ��Ҫ���������������˿ڲ������ж����εȵ���ز����ĺ궨�塣
 */

#ifndef _SYS_SET_H_
#define _SYS_SET_H_
//**********************************************************************
#define cli()	__asm__("cli")
#define sti()	__asm__("sti")

// ��ָ���˿ڶ�������
#define in_b(src) ({ \
	unsigned char var; \
	__asm__( \
	"inb	%%dx,%%al\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	: "=a"(var) : "d"(src) :);/*����� dx ��*/ \
var;})

// ��ָ���˿�д����
#define out_b(des, data) \
	__asm__( \
	"outb	%%al,%%dx\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	: : "a"(data),"d"(des) : )/*����� dx ��*/

//**********************************************************************
// ��������������������

// �������ṹ��
typedef struct desc_struct {
	unsigned long a,b;
}desc_table[256];

extern desc_table idt,gdt;

/*
 * NOTE! �ж��ź������ŵ�DPL���ڷ��ʱ����������DPL֮�󣬾��з���Ȩ�ޣ�
 * ��CPU�Զ�ȡ���������еĶ�ѡ�������CS��
 *
 * ��ˣ�����ϵͳ���ã��ں˽�����Ȩ������ΪRING3��ʹ�û�������Է��ʡ�
 * ������ϵͳ����֮��CS����Ȩ��ΪRING0����Ϊ���������еĶ�ѡ���Ϊ0x8��
 */
#define INT_R0	0x8e00		// �ж������ͣ���Ȩ��0
#define INT_R3	0xee00		// �ж������ͣ���Ȩ��3
#define TRAP_R0	0x8f00		// ���������ͣ���Ȩ��0
#define TRAP_R3	0xef00		// ���������ͣ���Ȩ��3

// �����ж���������
#define set_idt(FLAG,FUNC_ADDR,NR_INT) \
	__asm__( \
	"movl	%%eax,%%edx\n\t" \
	"movl	$0x00080000,%%eax\n\t"/* ע�⣺��ѡ���Ϊ 0x00080000 */ \
	"mov	%%dx,%%ax\n\t" \
	"movw	%%cx,%%dx\n\t" \
	"movl	%%eax,(%%edi)\n\t" \
	"movl	%%edx,4(%%edi)\n\t" \
	: : "c"(FLAG),"a"(FUNC_ADDR),"D"(&idt[NR_INT]) : ) // ע��: idt���ж��������ṹ�����飬�ʲ����ٳ���8������ע����ȡ��ַ��

//**********************************************************************
#endif // _SYS_SET_H_