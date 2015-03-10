/*
 * Small/lib/string.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-01-31 11:19:48
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

//**********************************************************************

// F: 求字符串长度
// I: 字符串指针
// O: 字符串长度
int strlen(char *str)
{
	int count = 0;

	while(*str)
	{
		++count;
		++str;
	}

	return count;
}

// F: 复制字符串
// I: 目的位置指针，源字符串指针
// O: 目的字符串指针
char * strcpy(char *des, char *src)
{
	int count = strlen(src);

	for(int i=0; i<count; i++)
	{
		des[i] = src[i];
	}
	des[count] = 0;

	return des;
}

//**********************************************************************

// F: 内存复制
// I: 目的内存指针，原内存指针，复制字节数
// O: 目的内存指针
void * memcpy(void *des, void *src, int count)
{
	for(int i=0; i<count; i++)
	{
		((char *)des)[i] = ((char *)src)[i];
	}

	return des;
}

// F: 内存填充
// I: 目的内存指针，填充字符，填充数
// O: 目的内存指针
void * memset(void *des, char c, int count)
{
	for(int i=0; i<count; i++)
	{
		((char *)des)[i] = c;
	}

	return des;
}

// F: 初始化内存为0
// I: 目的内存指针，填充数
// O: 目的内存指针
void * zeromem(void *des, int count)
{
	return memset(des, 0, count);
}
