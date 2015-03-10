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

#define CAPS		(1<<0)	// ��д
#define SIGNED		(1<<1)	// �з���
#define PERCENT		(1<<7)	// �ٷֺű�־

char printbuff[1024] = {0};	// ��ӡ����

// F: ����ת��Ϊ�ַ���
// I: ת��֮�󻺳���ָ�롢��ת�����������ơ�ת����־
// O: ת��֮����ַ�����
int itoa(char *buff, int value, int base, unsigned int flag)
{
	char dicta[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char dictA[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int index = 0;			// buff ������
	char stack[20] = {0};	// ����10�������Ľ���

	if(16 == base)	// 16����
	{
		index = 0;
		buff[index++] = '0';
		buff[index++] = ((flag & CAPS) ? 'X' : 'x');
		for(int i=sizeof(int)*2; i>0; i--)	// �赹��
		{
			// �������ƣ����õ��ĵ�4λ������Ϊ�ֵ�������������õ�4λ�����ƶ�Ӧ��ʮ��������
			buff[index++] = ((flag & CAPS) ? dictA[(value>>((i-1)*4)) & 0xf] : dicta[(value>>((i-1)*4)) & 0xf]);
		}
	}
	else if(10 == base)	// 10����
	{
		index = 0;
		if(flag & SIGNED)	// �з��ţ����ж��Ƿ���Ӹ��š�-��
		{
			(value & (1 << (sizeof(int)*8-1))) ? (buff[index++] = '-') : 0;
		}

		int j = 0;
		while(0 != value)
		{
			// ע�⣺���Ǹ���������תΪ��������ȡ�࣬����ȡ���õ����벻���Ľ��������������������ͨ�á�
			// ���⣬ע�� & �� % �����ȼ���!!!
			if(value & (1 << (sizeof(int)*8-1)))	// ���λΪ1
			{
				stack[j++] = dicta[(flag & SIGNED) ? (((~((value&0x7fff)-1))&0x7fff) % 10) : (((unsigned int)value) % 10)];
			}
			else	// ���λΪ0���������Ǹ���
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
		// û���κ���!
	}
	buff[index] = 0;

	return index;
}

// F: ��ʽ���ַ�����Ŀǰ֧�֣�	%d(�з���10����), %u(�޷���10����), %x(�޷���Сд16����), %X(�޷��Ŵ�д16����),
//								%c(���ַ�), %s(�ַ���), %%(%����)
// I: �洢������ָ�롢����ʽ���ַ���ָ�롢va_list ��ָ��
// O: ��ʽ��֮��Ļ������ַ���
int vsprintf(char *buff, const char *fmt, va_list args)
{
	int count = 0;			// ��ʽ��֮��Ļ������ַ���
	unsigned int flag = 0;	// ת����־
	char *p = fmt;
	char *tmp = 0;			// ���� %s

	while(*p)
	{
		switch(*p)
		{
		case '%':	// % �� %%
			(flag & PERCENT) ? (buff[count++] = *p, flag &= (0xffff ^ PERCENT)) : (flag |= PERCENT);
			break;
		case 'd':	// d �� %d
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			flag |= SIGNED;
			count += itoa(&buff[count], va_arg(args, int), 10, flag);
			flag &= (0xffff ^ SIGNED);
			flag &= (0xffff ^ PERCENT);		// ע�⣺ÿ�ζ�Ҫȡ�� %�� ��־
			break;
		case 'u':	// u �� %u
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			count += itoa(&buff[count], va_arg(args, int), 10, flag);
			flag &= (0xffff ^ PERCENT);
			break;
		case 'x':	// x �� %x
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			count += itoa(&buff[count], va_arg(args, int), 16, flag);
			flag &= (0xffff ^ PERCENT);
			break;
		case 'X':	// X �� %X
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
		case 'c':	// c �� %c
			if(!(flag & PERCENT))
			{
				buff[count++] = *p;
				break;
			}
			buff[count++] = va_arg(args, char);
			flag &= (0xffff ^ PERCENT);
			break;
		case 's':	// s �� %s
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
		default:	// ����
			buff[count++] = *p;
			break;
		}
		p++;
	}
	buff[count] = 0;

	return count;
}

// F: printf
// I: �ɱ����
// O: ����ַ��ĸ���
int printf(const char *fmt, ...)
{
	int count = 0;
	va_list vaptr = 0;

	va_start(vaptr, fmt);
	console_write(printbuff, count=vsprintf(printbuff, fmt, vaptr));
	va_end(vaptr);

	return count;
}