/*
 * Small/kernel/console.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-02-01 20:01:45
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

#include "types.h"
#include "sys_set.h"
#include "console.h"

CONSOLE	console;

// F: 设置光标位置，注意屏幕行列是从 (console.screen_org/2)处 开始
// I: 光标位置
// O: NULL
void set_cursor_pos(unsigned int curpos)
{
	// 是不可显示处，则返回
	if((curpos < console.screen_org/2) || (curpos >= (console.screen_org+160*25)/2))
	{ return; }
	
	console.cursor_pos = curpos;
	out_b(CRT_ADDR_REG, CRT_CUR_HIGH_REG);
	out_b(CRT_DATA_REG, (curpos >> 8) & 0xff);
	out_b(CRT_ADDR_REG, CRT_CUR_LOW_REG);
	out_b(CRT_DATA_REG, curpos & 0xff);
}

// F: 根据当前光标位置相对移动光标
// I: 光标偏移值，可正可负可零
// O: NULL
void move_cursor(int offset)
{
	return set_cursor_pos(console.cursor_pos + offset);
}

// F: 设置屏幕显示开始地址，相对于显存开始处
// I: 屏幕显示开始地址
// O: NULL
void set_screen_org(unsigned int org)
{
	org = org / 160 * 160;	// 行对齐

	if((org < console.org) || ((org + 160 * 25) > (console.org + console.size)))
	{ return; }

	console.screen_org = org;
	org /= 2;				// org是显示的内存开始地址，除以2则可用于设置
	out_b(CRT_ADDR_REG, CRT_START_HIGH_REG);
	out_b(CRT_DATA_REG, (org >> 8) & 0xff);
	out_b(CRT_ADDR_REG, CRT_START_LOW_REG);
	out_b(CRT_DATA_REG, org & 0xff);
}

// F: 根据当前屏幕显示开始处，进行偏移调整
// I: 偏移值，可正可负可零
// O: NULL
void move_screen_org(int offset)
{
	return set_screen_org(console.screen_org + offset);
}

// F: 控制台写字符串函数，同时移动光标和改变current_pos
// I: 写缓冲指针、需要写的字符数
// O: NULL
void console_write_string(char *write_buff, int count)
{
	if(count <= 0)
	{ return; }

	// 注意：此内嵌汇编会修改 current_pos
	__asm__( 
		"movw	$0x18,%%bx\n\t"	/* 0x18 为显存段选择符 */ 
		"mov	%%bx,%%gs\n\t"	/* 注意：此处需用gs */
		"movw	$0x10,%%bx\n\t"	/* 0x10为数据段选择符 */
		"mov	%%bx,%%ds\n\t"
		"movl	$0,%%ebx\n\t" 
	"1:"
		"mov	$0x02,%%ah\n\t" 
		"mov	%%ds:(%%edx),%%al\n\t" 
		"movl	%2,%%edi\n\t"	/* 注意内存约束的使用 */
		"movw	%%ax,%%gs:(%%edi)\n\t" 
		"addl	$2,%2\n\t"
		"inc	%%edx\n\t"
		"inc	%%ebx\n\t"
		"cmpl	%%ecx,%%ebx\n\t" 
		"jne	1b\n\t"
		:: "d"(write_buff),"c"(count),"m"(console.current_pos) :); 

	console.cursor_pos += count;
	// 光标位置超出屏幕范围，则屏幕下滚。由于可能从 console.org 重新开始，故需调整光标和current_pos
	if(console.cursor_pos > ((console.screen_org + 160 * 25) / 2 - 1))
	{
		screen_down();
		set_cursor_pos((console.screen_org + 160 * 24) / 2 / 80 * 80);	// （/ 80 * 80）使光标位于一行最开始处
		console.current_pos = console.cursor_pos * 2;
	}
	else
	{
		set_cursor_pos(console.cursor_pos);
	}
}

// F: 控制台复制内存。复制 (count * 2) 字节。!!!注意：此函数只适用于源地址和目的地址都是显存的情况。
// I: 目的地址、源地址、复制 字 数，而不是字节。!!!注意：此处段选择符都是0x18，故只有在 des 和 src 的段选择符都是0x18时才可以使用。
// O: NULL
void console_copy_memory(unsigned long des, unsigned long src, int count)
{
	if((count <= 0) || (des == src))
	{ return; }

	__asm__(
		"movw	$0x18,%%bx\n\t"		/* 显存段选择符 */
		"movw	%%bx,%%gs\n\t"		/* 目的段，注意：此处需用gs */
	"1:"
		"movw	%%gs:(%%esi),%%bx\n\t"
		"movw	%%bx,%%gs:(%%edi)\n\t"
		"addl	$2,%%esi\n\t"
		"addl	$2,%%edi\n\t"
		"dec	%%ecx\n\t"
		"test	%%ecx,%%ecx\n\t"
		"jnz	1b\n\t"
		: :"D"(des),"S"(src),"c"(count) : );
}

// F: 清除一个字符。只清除一行中的字符。
// I: NULL
// O: NULL
void clean_char(void)
{
	char c = 0x20;	// 用于清除字符的字符，即空格

	if(0 == (console.cursor_pos % 80))// 处于一行的最开始
	{ return; }

	move_cursor(-1);
	console.current_pos -= 2;
	console_write_string(&c, 1); // 取地址
	move_cursor(-1);
	console.current_pos -= 2;
}

// F: 清除一行字符。
// I: 要清除的那行字符的开始地址
// O: NULL
void clean_line(unsigned long start)
{
	char s[2] = {0x20, 0x02};	// 用于清除屏幕，即空格、显示属性

	start = start / 160 * 160;	// 行对齐

	// 每次一个字，即2字节。!!!注意：此处源地址和目的地址的段选择符不同，故不能用console_copy_memory()
	__asm__(
		"movw	$0x18,%%bx\n\t"		/* 显存段选择符 */
		"movw	%%bx,%%gs\n\t"		/* 目的段，注意：此处需用gs */
	"1:"
		"movw	%%ds:(%%esi),%%bx\n\t"
		"movw	%%bx,%%gs:(%%edi)\n\t"
		"addl	$2,%%edi\n\t"
		"dec	%%ecx\n\t"
		"test	%%ecx,%%ecx\n\t"
		"jnz	1b\n\t"
		: :"D"(start),"S"(s),"c"(80) : );
}

// F: 屏幕下移一行。也即字符上滚。在这里并没有对光标和current_pos进行设置。
// I: NULL
// O: NULL
void screen_down(void)
{
	// 如果再下移就超出控制台内存限制，则将屏幕显示复制到控制台最开始处，并进行设置。
	if((console.org + console.size - console.screen_org - 160 * 25) < 160)
	{
		console_copy_memory(console.org, console.screen_org+160, 24*80);	// 复制 (25-1) 行。注意，复制的数量为 字 数，而不是字节
		console.screen_org = console.org;
	}
	else
	{
		console.screen_org += 160;
	}
	set_screen_org(console.screen_org);
	clean_line(console.screen_org + 24 * 160); // 清除屏幕下移之后的最后一行的字符
}

// F: 控制台写函数
// I: 写缓冲指针、要写到控制台的字符数
// O: NULL
__volatile__ void console_write(char *buff, int count)
{
	if(count <= 0)
	{ return; }

	int index = 0;	// 写缓冲索引
	int i = 0;		// 临时控制循环变量
	char c = 0x20;  // 空字符，即空格
	while(count--)
	{
		switch(buff[index])
		{
		case '\b':	// 退格
			clean_char();
			break;
		case '\t':	// 水平制表，4字符对齐
//			for(i=0; i<(4 - (console.cursor_pos % 4)); i++)	// 错误!!! 因为cursor_pos在此会变化。
			i = 4 - (console.cursor_pos % 4);
			for(; i>0; i--)
			{
				console_write_string(&c, 1);
			}
			break;
		case '\n':	// 换行
			if(console.cursor_pos >= (console.screen_org + 160 * 24) / 2)		// 光标处于最后一行，则屏幕下滚
			{
				screen_down();
				set_cursor_pos((console.screen_org + 160 * 24) / 2 / 80 * 80);	// （/ 80 * 80）使光标位于一行最开始处
			}
			else
			{
				set_cursor_pos((console.cursor_pos + 80) / 80 * 80);			// （/ 80 * 80）使光标位于一行最开始处
			}
			console.current_pos = console.cursor_pos * 2;
			break;
		case '\r':	// 回车
			set_cursor_pos(console.cursor_pos / 80 * 80);
			console.current_pos = console.cursor_pos * 2;
			break;
		default:	// 写字符
			console_write_string(&buff[index], 1);
			break;
		}

		index++;
	}
}

// F: 控制台初始化
// I: NULL
// O: NULL
void console_init(void)
{
	// 取得光标位置，光标位置在boot中被保存
	__asm__( 
		"movw	$0x10,%%bx\n\t" 
		"movw	%%bx,%%ds\n\t" 
		"movl	$0x90000,%%ecx\n\t"/* 0x90000! 应与boot中相对应!!! */ 
		"xor	%%ebx,%%ebx\n\t" 
		"movw	%%ds:(%%ecx),%%bx\n\t" 
		:"=b"(console.cursor_pos) : :);
	set_cursor_pos(console.cursor_pos);
	console.current_pos = console.cursor_pos * 2; // 当前输入位置相对于显存开始处

	console.org = 0;			// 相对于显存段开始位置
	console.size = 32 * 1024;	// 32kb，注意：head.s中显存段描述符需设置为限长32kb
	console.screen_org = console.org;
	set_screen_org(console.screen_org);
}