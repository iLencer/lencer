/*
 * Small/lib/stdio.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-02-06 17:17:21
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

#include "stdarg.h"
#include "console.h"

#define CAPS		(1<<0)	// 大写
#define SIGNED		(1<<1)	// 有符号
#define PERCENT		(1<<7)	// 百分号标志

char printbuff[1024] = {0};	// 打印缓冲

// F: 整数转换为字符串
// I: 转换之后缓冲区指针、欲转换整数、进制、转换标志
// O: 转换之后的字符个数
int itoa(char *buff, int value, int base, unsigned int flag)
{
	char dicta[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char dictA[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int index = 0;			// buff 的索引
	char stack[20] = {0};	// 用于10进制数的解析

	if(16 == base)	// 16进制
	{
		index = 0;
		buff[index++] = '0';
		buff[index++] = ((flag & CAPS) ? 'X' : 'x');
		for(int i=sizeof(int)*2; i>0; i--)	// 需倒着
		{
			// 将其右移，将得到的低4位的数作为字典数组的索引，得到4位二进制对应的十六进制数
			buff[index++] = ((flag & CAPS) ? dictA[(value>>((i-1)*4)) & 0xf] : dicta[(value>>((i-1)*4)) & 0xf]);
		}
	}
	else if(10 == base)	// 10进制
	{
		index = 0;
		if(flag & SIGNED)	// 有符号，则判断是否需加负号“-”
		{
			(value & (1 << (sizeof(int)*8-1))) ? (buff[index++] = '-') : 0;
		}

		int j = 0;
		while(0 != value)
		{
			// 注意：若是负数，则将其转为正数进行取余，因负数取余会得到意想不到的结果。但除法运算正负数通用。
			// 另外，注意 & 比 % 的优先级低!!!
			if(value & (1 << (sizeof(int)*8-1)))	// 最高位为1
			{
				stack[j++] = dicta[(flag & SIGNED) ? (((~((value&0x7fff)-1))&0x7fff) % 10) : (((unsigned int)value) % 10)];
			}
			else	// 最高位为0，不可能是负数
			{
				stack[j++] = dicta[(flag & SIGNED) ? ((value) % 10) : (((unsigned int)value) % 10)];
			}
			value = (flag & SIGNED) ? ((value) / 10) : (((unsigned int)value) / 10);
		}
		if(0 == j)
		{
			buff[index++] = '0';
		}
		while(0 != j)
		{
			buff[index++] = stack[--j];
		}
	}
	else
	{
		index = 0;
		// 没做任何事!
	}
	buff[index] = 0;

	return index;
}

// F: 格式化字符串。目前支持：	%d(有符号10进制), %u(无符号10进制), %x(无符号小写16进制), %X(无符号大写16进制),
//								%c(单字符), %s(字符串), %%(%本身)
// I: 存储缓冲区指针、欲格式化字符串指针、va_list 型指针
// O: 格式化之后的缓冲区字符数
int vsprintf(char *buff, const char *fmt, va_list args)
{
	int count = 0;			// 格式化之后的缓冲区字符数
	unsigned int flag = 0;	// 转换标志
	char *p = fmt;
	char *tmp = 0;			// 用于 %s

	while(*p)
	{
		switch(*p)
		{
		case '%':	// % 和 %%
			(flag & PERCENT) ? (buff[count++] = *p, flag &= (0xffff ^ PERCENT)) : (flag |= PERCENT);
			break;
		case 'd':	// d 和 %d
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			flag |= SIGNED;
			count += itoa(&buff[count], va_arg(args, int), 10, flag);
			flag &= (0xffff ^ SIGNED);
			flag &= (0xffff ^ PERCENT);		// 注意：每次都要取消 %号 标志
			break;
		case 'u':	// u 和 %u
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			count += itoa(&buff[count], va_arg(args, int), 10, flag);
			flag &= (0xffff ^ PERCENT);
			break;
		case 'x':	// x 和 %x
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			count += itoa(&buff[count], va_arg(args, int), 16, flag);
			flag &= (0xffff ^ PERCENT);
			break;
		case 'X':	// X 和 %X
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			flag |= CAPS;
			count += itoa(&buff[count], va_arg(args, int), 16, flag);
			flag &= (0xffff ^ CAPS);
			flag &= (0xffff ^ PERCENT);
			break;
		case 'c':	// c 和 %c
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			buff[count++] = va_arg(args, char);
			flag &= (0xffff ^ PERCENT);
			break;
		case 's':	// s 和 %s
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			tmp = va_arg(args, char *);
			while(*tmp)
			{
				buff[count++] = *tmp;
				tmp++;
			}
			flag &= (0xffff ^ PERCENT);
			break;
		default:	// 其他
			buff[count++] = *p;
			break;
		}
		p++;
	}
	buff[count] = 0;

	return count;
}

// F: printf
// I: 可变参数
// O: 输出字符的个数
int printf(const char *fmt, ...)
{
	int count = 0;
	va_list vaptr = 0;

	va_start(vaptr, fmt);
	console_write(printbuff, count=vsprintf(printbuff, fmt, vaptr));
	va_end(vaptr);

	return count;
}