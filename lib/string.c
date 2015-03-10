/*
 * Small/lib/string.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-01-31 11:19:48
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

//**********************************************************************

// F: ���ַ�������
// I: �ַ���ָ��
// O: �ַ�������
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

// F: �����ַ���
// I: Ŀ��λ��ָ�룬Դ�ַ���ָ��
// O: Ŀ���ַ���ָ��
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

// F: �ڴ渴��
// I: Ŀ���ڴ�ָ�룬ԭ�ڴ�ָ�룬�����ֽ���
// O: Ŀ���ڴ�ָ��
void * memcpy(void *des, void *src, int count)
{
	for(int i=0; i<count; i++)
	{
		((char *)des)[i] = ((char *)src)[i];
	}

	return des;
}

// F: �ڴ����
// I: Ŀ���ڴ�ָ�룬����ַ��������
// O: Ŀ���ڴ�ָ��
void * memset(void *des, char c, int count)
{
	for(int i=0; i<count; i++)
	{
		((char *)des)[i] = c;
	}

	return des;
}

// F: ��ʼ���ڴ�Ϊ0
// I: Ŀ���ڴ�ָ�룬�����
// O: Ŀ���ڴ�ָ��
void * zeromem(void *des, int count)
{
	return memset(des, 0, count);
}
