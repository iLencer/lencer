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

// F: ���ù��λ�ã�ע����Ļ�����Ǵ� (console.screen_org/2)�� ��ʼ
// I: ���λ��
// O: NULL
void set_cursor_pos(unsigned int curpos)
{
	// �ǲ�����ʾ�����򷵻�
	if((curpos < console.screen_org/2) || (curpos >= (console.screen_org+160*25)/2))
	{ return; }
	
	console.cursor_pos = curpos;
	out_b(CRT_ADDR_REG, CRT_CUR_HIGH_REG);
	out_b(CRT_DATA_REG, (curpos >> 8) & 0xff);
	out_b(CRT_ADDR_REG, CRT_CUR_LOW_REG);
	out_b(CRT_DATA_REG, curpos & 0xff);
}

// F: ���ݵ�ǰ���λ������ƶ����
// I: ���ƫ��ֵ�������ɸ�����
// O: NULL
void move_cursor(int offset)
{
	return set_cursor_pos(console.cursor_pos + offset);
}

// F: ������Ļ��ʾ��ʼ��ַ��������Դ濪ʼ��
// I: ��Ļ��ʾ��ʼ��ַ
// O: NULL
void set_screen_org(unsigned int org)
{
	org = org / 160 * 160;	// �ж���

	if((org < console.org) || ((org + 160 * 25) > (console.org + console.size)))
	{ return; }

	console.screen_org = org;
	org /= 2;				// org����ʾ���ڴ濪ʼ��ַ������2�����������
	out_b(CRT_ADDR_REG, CRT_START_HIGH_REG);
	out_b(CRT_DATA_REG, (org >> 8) & 0xff);
	out_b(CRT_ADDR_REG, CRT_START_LOW_REG);
	out_b(CRT_DATA_REG, org & 0xff);
}

// F: ���ݵ�ǰ��Ļ��ʾ��ʼ��������ƫ�Ƶ���
// I: ƫ��ֵ�������ɸ�����
// O: NULL
void move_screen_org(int offset)
{
	return set_screen_org(console.screen_org + offset);
}

// F: ����̨д�ַ���������ͬʱ�ƶ����͸ı�current_pos
// I: д����ָ�롢��Ҫд���ַ���
// O: NULL
void console_write_string(char *write_buff, int count)
{
	if(count <= 0)
	{ return; }

	// ע�⣺����Ƕ�����޸� current_pos
	__asm__( 
		"movw	$0x18,%%bx\n\t"	/* 0x18 Ϊ�Դ��ѡ��� */ 
		"mov	%%bx,%%gs\n\t"	/* ע�⣺�˴�����gs */
		"movw	$0x10,%%bx\n\t"	/* 0x10Ϊ���ݶ�ѡ��� */
		"mov	%%bx,%%ds\n\t"
		"movl	$0,%%ebx\n\t" 
	"1:"
		"mov	$0x02,%%ah\n\t" 
		"mov	%%ds:(%%edx),%%al\n\t" 
		"movl	%2,%%edi\n\t"	/* ע���ڴ�Լ����ʹ�� */
		"movw	%%ax,%%gs:(%%edi)\n\t" 
		"addl	$2,%2\n\t"
		"inc	%%edx\n\t"
		"inc	%%ebx\n\t"
		"cmpl	%%ecx,%%ebx\n\t" 
		"jne	1b\n\t"
		:: "d"(write_buff),"c"(count),"m"(console.current_pos) :); 

	console.cursor_pos += count;
	// ���λ�ó�����Ļ��Χ������Ļ�¹������ڿ��ܴ� console.org ���¿�ʼ�������������current_pos
	if(console.cursor_pos > ((console.screen_org + 160 * 25) / 2 - 1))
	{
		screen_down();
		set_cursor_pos((console.screen_org + 160 * 24) / 2 / 80 * 80);	// ��/ 80 * 80��ʹ���λ��һ���ʼ��
		console.current_pos = console.cursor_pos * 2;
	}
	else
	{
		set_cursor_pos(console.cursor_pos);
	}
}

// F: ����̨�����ڴ档���� (count * 2) �ֽڡ�!!!ע�⣺�˺���ֻ������Դ��ַ��Ŀ�ĵ�ַ�����Դ�������
// I: Ŀ�ĵ�ַ��Դ��ַ������ �� �����������ֽڡ�!!!ע�⣺�˴���ѡ�������0x18����ֻ���� des �� src �Ķ�ѡ�������0x18ʱ�ſ���ʹ�á�
// O: NULL
void console_copy_memory(unsigned long des, unsigned long src, int count)
{
	if((count <= 0) || (des == src))
	{ return; }

	__asm__(
		"movw	$0x18,%%bx\n\t"		/* �Դ��ѡ��� */
		"movw	%%bx,%%gs\n\t"		/* Ŀ�ĶΣ�ע�⣺�˴�����gs */
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

// F: ���һ���ַ���ֻ���һ���е��ַ���
// I: NULL
// O: NULL
void clean_char(void)
{
	char c = 0x20;	// ��������ַ����ַ������ո�

	if(0 == (console.cursor_pos % 80))// ����һ�е��ʼ
	{ return; }

	move_cursor(-1);
	console.current_pos -= 2;
	console_write_string(&c, 1); // ȡ��ַ
	move_cursor(-1);
	console.current_pos -= 2;
}

// F: ���һ���ַ���
// I: Ҫ����������ַ��Ŀ�ʼ��ַ
// O: NULL
void clean_line(unsigned long start)
{
	char s[2] = {0x20, 0x02};	// ���������Ļ�����ո���ʾ����

	start = start / 160 * 160;	// �ж���

	// ÿ��һ���֣���2�ֽڡ�!!!ע�⣺�˴�Դ��ַ��Ŀ�ĵ�ַ�Ķ�ѡ�����ͬ���ʲ�����console_copy_memory()
	__asm__(
		"movw	$0x18,%%bx\n\t"		/* �Դ��ѡ��� */
		"movw	%%bx,%%gs\n\t"		/* Ŀ�ĶΣ�ע�⣺�˴�����gs */
	"1:"
		"movw	%%ds:(%%esi),%%bx\n\t"
		"movw	%%bx,%%gs:(%%edi)\n\t"
		"addl	$2,%%edi\n\t"
		"dec	%%ecx\n\t"
		"test	%%ecx,%%ecx\n\t"
		"jnz	1b\n\t"
		: :"D"(start),"S"(s),"c"(80) : );
}

// F: ��Ļ����һ�С�Ҳ���ַ��Ϲ��������ﲢû�жԹ���current_pos�������á�
// I: NULL
// O: NULL
void screen_down(void)
{
	// ��������ƾͳ�������̨�ڴ����ƣ�����Ļ��ʾ���Ƶ�����̨�ʼ�������������á�
	if((console.org + console.size - console.screen_org - 160 * 25) < 160)
	{
		console_copy_memory(console.org, console.screen_org+160, 24*80);	// ���� (25-1) �С�ע�⣬���Ƶ�����Ϊ �� �����������ֽ�
		console.screen_org = console.org;
	}
	else
	{
		console.screen_org += 160;
	}
	set_screen_org(console.screen_org);
	clean_line(console.screen_org + 24 * 160); // �����Ļ����֮������һ�е��ַ�
}

// F: ����̨д����
// I: д����ָ�롢Ҫд������̨���ַ���
// O: NULL
__volatile__ void console_write(char *buff, int count)
{
	if(count <= 0)
	{ return; }

	int index = 0;	// д��������
	int i = 0;		// ��ʱ����ѭ������
	char c = 0x20;  // ���ַ������ո�
	while(count--)
	{
		switch(buff[index])
		{
		case '\b':	// �˸�
			clean_char();
			break;
		case '\t':	// ˮƽ�Ʊ�4�ַ�����
//			for(i=0; i<(4 - (console.cursor_pos % 4)); i++)	// ����!!! ��Ϊcursor_pos�ڴ˻�仯��
			i = 4 - (console.cursor_pos % 4);
			for(; i>0; i--)
			{
				console_write_string(&c, 1);
			}
			break;
		case '\n':	// ����
			if(console.cursor_pos >= (console.screen_org + 160 * 24) / 2)		// ��괦�����һ�У�����Ļ�¹�
			{
				screen_down();
				set_cursor_pos((console.screen_org + 160 * 24) / 2 / 80 * 80);	// ��/ 80 * 80��ʹ���λ��һ���ʼ��
			}
			else
			{
				set_cursor_pos((console.cursor_pos + 80) / 80 * 80);			// ��/ 80 * 80��ʹ���λ��һ���ʼ��
			}
			console.current_pos = console.cursor_pos * 2;
			break;
		case '\r':	// �س�
			set_cursor_pos(console.cursor_pos / 80 * 80);
			console.current_pos = console.cursor_pos * 2;
			break;
		default:	// д�ַ�
			console_write_string(&buff[index], 1);
			break;
		}

		index++;
	}
}

// F: ����̨��ʼ��
// I: NULL
// O: NULL
void console_init(void)
{
	// ȡ�ù��λ�ã����λ����boot�б�����
	__asm__( 
		"movw	$0x10,%%bx\n\t" 
		"movw	%%bx,%%ds\n\t" 
		"movl	$0x90000,%%ecx\n\t"/* 0x90000! Ӧ��boot�����Ӧ!!! */ 
		"xor	%%ebx,%%ebx\n\t" 
		"movw	%%ds:(%%ecx),%%bx\n\t" 
		:"=b"(console.cursor_pos) : :);
	set_cursor_pos(console.cursor_pos);
	console.current_pos = console.cursor_pos * 2; // ��ǰ����λ��������Դ濪ʼ��

	console.org = 0;			// ������Դ�ο�ʼλ��
	console.size = 32 * 1024;	// 32kb��ע�⣺head.s���Դ��������������Ϊ�޳�32kb
	console.screen_org = console.org;
	set_screen_org(console.screen_org);
}