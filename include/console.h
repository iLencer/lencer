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

// 控制台结构，用于显示字符
typedef struct console_struct {
	unsigned int org;				// 当前控制台在显存中的开始位置，相对于显存开始处 (bytes)
	unsigned int size;				// 当前控制台大小 (bytes)
	unsigned int screen_org;		// 屏幕显示开始位置，相对于显存开始处 (bytes)
	unsigned int current_pos;		// 当前显示位置，相对于显存开始处 (bytes)
	unsigned int cursor_pos;		// 光标位置。注意：是依据于显存的，并不是依据于屏幕的 (bytes/2)
}CONSOLE;

#define CON_LINES	25				// 控制台显示行数
#define CON_COLS	80				// 控制台显示列数

// VGA显卡相关寄存器，此处是控制台相关的
#define CRT_ADDR_REG		0x3D4			// 地址端口
#define CRT_DATA_REG		0x3D5			// 数据端口
#define CRT_CUR_HIGH_REG	0x0E			// 光标位置高位寄存器
#define CRT_CUR_LOW_REG		0x0F			// 光标位置低位寄存器
#define CRT_START_HIGH_REG	0x0C			// 屏幕显示开始位置高位寄存器
#define CRT_START_LOW_REG	0x0D			// 屏幕显示开始位置低位寄存器

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