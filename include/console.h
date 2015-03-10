/*
 * Small/include/console.h
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-02-01 20:03:47
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "types.h"

//**********************************************************************

// ����̨�ṹ��������ʾ�ַ�
typedef struct console_struct {
	unsigned int org;				// ��ǰ����̨���Դ��еĿ�ʼλ�ã�������Դ濪ʼ�� (bytes)
	unsigned int size;				// ��ǰ����̨��С (bytes)
	unsigned int screen_org;		// ��Ļ��ʾ��ʼλ�ã�������Դ濪ʼ�� (bytes)
	unsigned int current_pos;		// ��ǰ��ʾλ�ã�������Դ濪ʼ�� (bytes)
	unsigned int cursor_pos;		// ���λ�á�ע�⣺���������Դ�ģ���������������Ļ�� (bytes/2)
}CONSOLE;

#define CON_LINES	25				// ����̨��ʾ����
#define CON_COLS	80				// ����̨��ʾ����

// VGA�Կ���ؼĴ������˴��ǿ���̨��ص�
#define CRT_ADDR_REG		0x3D4			// ��ַ�˿�
#define CRT_DATA_REG		0x3D5			// ���ݶ˿�
#define CRT_CUR_HIGH_REG	0x0E			// ���λ�ø�λ�Ĵ���
#define CRT_CUR_LOW_REG		0x0F			// ���λ�õ�λ�Ĵ���
#define CRT_START_HIGH_REG	0x0C			// ��Ļ��ʾ��ʼλ�ø�λ�Ĵ���
#define CRT_START_LOW_REG	0x0D			// ��Ļ��ʾ��ʼλ�õ�λ�Ĵ���

//**********************************************************************
extern void set_cursor_pos(unsigned int curpos);
extern void move_cursor(int offset);
extern void set_screen_org(unsigned int org);
extern void move_screen_org(int offset);
extern void console_write_string(char *write_buff, int count);
extern void console_copy_memory(unsigned long des, unsigned long src, int count);
extern void clean_char(void);
extern void clean_line(unsigned long start);
extern void screen_down(void);
extern void console_write(char *buff, int count);
extern void console_init(void);

//**********************************************************************
#endif // _CONSOLE_H_