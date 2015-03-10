/*
 * Small/init/main.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.1 2013-01-30 21:48:27
 * V0.2 2013-02-01 15:36:45
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */
#include "sys_set.h"
#include "sys_nr.h"
#include "string.h"
#include "console.h"
#include "keyboard.h"
#include "stdio.h"

extern int keyboard_int(void);
extern int timer_int(void);

int main(void)
{
	char showmsg[] = "Small/init/main Start!\n!!\b\bMy printf() Start!\n";

	console_init();
//	console_write(showmsg, strlen(showmsg));

	set_idt(INT_R0, keyboard_int, NR_KEYBOARD_INT);
	set_idt(INT_R0, timer_int, NR_TIMER_INT);
	keyboard_init();

	sti();
	
	// My printf()
	// ע�⣺printf("abc\n") �ᱻgcc�滻Ϊ���ú��� puts("abc")���ʣ����ڱ���ʱ����� -fno-builtin ѡ��ر�GCC�����ú����滻���ܡ�
	printf(".............................\n");
	printf("Hello %u!\n%sCopyright ZYF 2012%d\n", 2013, showmsg, -2013);
	printf(".............................\n");

	printf("=== OK.\n");

	for(;;)
	{
		for(int i=0; i<30; i++)
			for(int j=0; j<10000; j++)
				;
//		console_write(showmsg, strlen(showmsg));
	}

	return 0;
}