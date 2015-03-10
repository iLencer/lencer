/*
 * Small/kernel/keyboard.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-01-31 23:11:04
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

/*
 * 此文件包含键盘缓冲区的相关处理程序。
 */

#include "types.h"
#include "keyboard.h"
#include "console.h"
#include "keymap.h"
#include "string.h"
#include "sys_set.h"

KB_QUEUE kb_queue;						// 键盘缓冲区（队列）
unsigned char mode = 0;					// shift, ctrl, alt 标志
unsigned char lock = 0;					// num_lock, caps_lock, scroll_lock 标志
unsigned char e0 = 0;					// e0 标志

// F: 键盘处理程序。将产生键盘中断时的键盘扫描码转换之后，放入内核维护的键盘缓冲区，被keyboard_int调用。
// I: NULL
// O: NULL
void keyboard_hander(void)
{
	unsigned char scan_code = in_b(0x60);
	
	if(kb_queue.count < KEY_BUFF_SIZE)	// 若缓冲区满，则丢弃扫描码，但必须从8042读入，否则8042将不再接收字符
	{		
		if(TRUE == cook_scan_code(scan_code, &kb_queue.q_buff[kb_queue.q_tail])) // 转换成相应按键
		{
			((++kb_queue.q_tail) >= KEY_BUFF_SIZE) ? kb_queue.q_tail=0 : 0;
			(kb_queue.count)++;			

			console_write(&kb_queue.q_buff[kb_queue.q_head], 1);// 注意是取地址

			((++kb_queue.q_head) >= KEY_BUFF_SIZE) ? kb_queue.q_head=0 : 0;
			(kb_queue.count)--;
		}
	}
}

// F: 处理键盘扫描码，将其转换为对应的键。包括 e0，shift，!shift & !e0 这3种情况
// I: 扫描码
// O: 键值
unsigned int scan_to_key(unsigned char scan_code)
{
	/*
	 * 下述错误已完美解决! 错误原因：ELF文件中data数据段页对齐所致，参考 tools/build.c
	 */
/*
	// [??] 注意，这里的include。之所以这样做，是因为如果把全局变量放在函数体外，则会初始化失败，导致错误! 但目前尚不清楚原因。
	#include "keymap.h"
*/

	unsigned int key = 0;

	if(scan_code & 0x80)// Break or Make ?
	{
		key |= FLAG_BREAK;
	}

	if(e0)// e0置位
	{
		// 注意，以下需是 0x7f ，而 ~(0x80) 则会出错
		key |= key_map[2 + KEY_MAP_COLS * (scan_code & 0x7f)];
	}
	else if((SHIFT_L_DOWN & mode) || (SHIFT_R_DOWN & mode))// shift按下
	{
		key |= key_map[1 + KEY_MAP_COLS * (scan_code & 0x7f)];
	}
	else// !shift and !e0
	{
		key |= key_map[0 + KEY_MAP_COLS * (scan_code & 0x7f)];
	}

	return key;
}

// F: 将对应的扫描码处理成对应的键，并设置mode，lock，e0，最后将相应的键转换为相应的字符或功能字符。
//    注：由于shift+相应键，有另一种字符相对应。故不应将其分开处理。此处和Linux-0.11的做法相似，即
//        在产生键盘中断时，由功能键控制字符键来进行处理。
// I: 扫描码、转换之后的字符指针
// O: 若是完整的字符，则返回 TRUE
BOOL cook_scan_code(unsigned char scan_code, char *cooked)
{
	unsigned int key = 0;	// 按键
	*cooked = 0;			// cooked == 0,返回FALSE

	if(0xe0 == scan_code)
	{
		e0 = 1;
	}
	else
	{
		key = scan_to_key(scan_code);
		e0 = 0;					// 注意，复位 e0 须在 scan_to_key 被调用之后
	
		if(FLAG_BREAK & key)	// BREAK，只处理shift，ctrl， alt
		{
			key &= 0x7fff;
			switch(key)
			{
			case SHIFT_L:
				mode &= (0xff ^ SHIFT_L_DOWN); break;
			case SHIFT_R:
				mode &= (0xff ^ SHIFT_R_DOWN); break;
			case CTRL_L:
				mode &= (0xff ^ CTRL_L_DOWN); break;
			case CTRL_R:
				mode &= (0xff ^ CTRL_R_DOWN); break;
			case ALT_L:
				mode &= (0xff ^ ALT_L_DOWN); break;
			case ALT_R:
				mode &= (0xff ^ ALT_R_DOWN); break;
			default: break;
			}
		}
		else	// MAKE
		{
			key &= 0x7fff;
			if(key >= FLAG_CONTR_KEY)// 控制键
			{
				switch(key)
				{
				case SHIFT_L:
					mode |= SHIFT_L_DOWN; break;
				case SHIFT_R:
					mode |= SHIFT_R_DOWN; break;
				case CTRL_L:
					mode |= CTRL_L_DOWN; break;
				case CTRL_R:
					mode |= CTRL_R_DOWN; break;
				case ALT_L:
					mode |= ALT_L_DOWN; break;
				case ALT_R:
					mode |= ALT_R_DOWN; break;
				case NUM_LOCK:
					(lock & NUM_LOCK_DOWN) ? (lock &= (0xff ^ NUM_LOCK_DOWN)) : (lock |= NUM_LOCK_DOWN); break;
				case CAPS_LOCK:
					(lock & CAPS_LOCK_DOWN) ? (lock &= (0xff ^ CAPS_LOCK_DOWN)) : (lock |= CAPS_LOCK_DOWN); break;
				case SCROLL_LOCK:
					(lock & SCROLL_LOCK_DOWN) ? (lock &= (0xff ^ SCROLL_LOCK_DOWN)) : (lock |= SCROLL_LOCK_DOWN); break;
				case BACKSPACE:				// 退格
					*cooked = 8; break;
				case TAB:					// 水平制表
					*cooked = 9; break;
				case ENTER:					// 换行
					*cooked = 10; break;
				case DELETE:				// 删除
					*cooked = 127; break;
				default: break;
				}
			}
			else	// 小于 FLAG_CONTR_KEY（256） 的普通键
			{
				*cooked = key;
				if((CAPS_LOCK_DOWN & lock) && (('a'<=*cooked) && (*cooked<='z'))) // 大写锁定，并且是小写字符
				{
					*cooked -= 32;
				}
			}
		}
	}

	return ((0 == *cooked) ? FALSE : TRUE);
}

// F: 初始化程序
// I: NULL
// O: NULL
void keyboard_init(void)
{
	/*
	 * 下述原因已完美解决! 原因是：因为初始化为0的全局变量在bss段里，但这是OS，故被加载时没被初始化为0，故出错。
	 * 须在使用之前初始化。其实只是根据地址初始化，初始化之前还没有该变量。
	 */
	// 虽然在定义全局变量时，进行了初始化，但还是出错了，故需在此再次初始化。
	// 或许是GCC编译器不支持全局变量定义时初始化，但我尚未知否 :-(
	mode = lock = e0 = 0;

	kb_queue.q_head = kb_queue.q_tail = kb_queue.count = 0;
	zeromem(kb_queue.q_buff, KEY_BUFF_SIZE * sizeof(char));
}
