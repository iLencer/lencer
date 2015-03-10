/*
 * Small/include/sys_set.h
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-01-31 21:23:13
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

/*
 * 此头文件包含系统常用的宏定义等。
 *
 * 主要包括：描述符表、端口操作、中断屏蔽等的相关操作的宏定义。
 */

#ifndef _SYS_SET_H_
#define _SYS_SET_H_
//**********************************************************************
#define cli()	__asm__("cli")
#define sti()	__asm__("sti")

// 从指定端口读入数据
#define in_b(src) ({ \
	unsigned char var; \
	__asm__( \
	"inb	%%dx,%%al\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	: "=a"(var) : "d"(src) :);/*需放入 dx 中*/ \
var;})

// 向指定端口写数据
#define out_b(des, data) \
	__asm__( \
	"outb	%%al,%%dx\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	"jmp	1f\n\t" \
	"1:		\n\t" \
	: : "a"(data),"d"(des) : )/*需放入 dx 中*/

//**********************************************************************
// 以下用于设置描述符表

// 描述符结构体
typedef struct desc_struct {
	unsigned long a,b;
}desc_table[256];

extern desc_table idt,gdt;

/*
 * NOTE! 中断门和陷阱门的DPL用于访问保护。若检查DPL之后，具有访问权限，
 * 则CPU自动取门描述符中的段选择符载入CS。
 *
 * 因此，对于系统调用，内核将其特权级设置为RING3，使用户程序可以访问。
 * 而进入系统调用之后，CS的特权级为RING0，因为门描述符中的段选择符为0x8。
 */
#define INT_R0	0x8e00		// 中断门类型，特权级0
#define INT_R3	0xee00		// 中断门类型，特权级3
#define TRAP_R0	0x8f00		// 陷阱门类型，特权级0
#define TRAP_R3	0xef00		// 陷阱门类型，特权级3

// 设置中断门描述符
#define set_idt(FLAG,FUNC_ADDR,NR_INT) \
	__asm__( \
	"movl	%%eax,%%edx\n\t" \
	"movl	$0x00080000,%%eax\n\t"/* 注意：段选择符为 0x00080000 */ \
	"mov	%%dx,%%ax\n\t" \
	"movw	%%cx,%%dx\n\t" \
	"movl	%%eax,(%%edi)\n\t" \
	"movl	%%edx,4(%%edi)\n\t" \
	: : "c"(FLAG),"a"(FUNC_ADDR),"D"(&idt[NR_INT]) : ) // 注意: idt是中断描述符结构体数组，故不能再乘以8，另外注意是取地址。

//**********************************************************************
#endif // _SYS_SET_H_