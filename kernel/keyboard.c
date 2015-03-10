/*
 * Small/kernel/keyboard.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-01-31 23:11:04
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

/*
 * ���ļ��������̻���������ش������
 */

#include "types.h"
#include "keyboard.h"
#include "console.h"
#include "keymap.h"
#include "string.h"
#include "sys_set.h"

KB_QUEUE kb_queue;						// ���̻����������У�
unsigned char mode = 0;					// shift, ctrl, alt ��־
unsigned char lock = 0;					// num_lock, caps_lock, scroll_lock ��־
unsigned char e0 = 0;					// e0 ��־

// F: ���̴�����򡣽����������ж�ʱ�ļ���ɨ����ת��֮�󣬷����ں�ά���ļ��̻���������keyboard_int���á�
// I: NULL
// O: NULL
void keyboard_hander(void)
{
	unsigned char scan_code = in_b(0x60);
	
	if(kb_queue.count < KEY_BUFF_SIZE)	// ����������������ɨ���룬�������8042���룬����8042�����ٽ����ַ�
	{		
		if(TRUE == cook_scan_code(scan_code, &kb_queue.q_buff[kb_queue.q_tail])) // ת������Ӧ����
		{
			((++kb_queue.q_tail) >= KEY_BUFF_SIZE) ? kb_queue.q_tail=0 : 0;
			(kb_queue.count)++;			

			console_write(&kb_queue.q_buff[kb_queue.q_head], 1);// ע����ȡ��ַ

			((++kb_queue.q_head) >= KEY_BUFF_SIZE) ? kb_queue.q_head=0 : 0;
			(kb_queue.count)--;
		}
	}
}

// F: �������ɨ���룬����ת��Ϊ��Ӧ�ļ������� e0��shift��!shift & !e0 ��3�����
// I: ɨ����
// O: ��ֵ
unsigned int scan_to_key(unsigned char scan_code)
{
	/*
	 * �����������������! ����ԭ��ELF�ļ���data���ݶ�ҳ�������£��ο� tools/build.c
	 */
/*
	// [??] ע�⣬�����include��֮����������������Ϊ�����ȫ�ֱ������ں������⣬����ʼ��ʧ�ܣ����´���! ��Ŀǰ�в����ԭ��
	#include "keymap.h"
*/

	unsigned int key = 0;

	if(scan_code & 0x80)// Break or Make ?
	{
		key |= FLAG_BREAK;
	}

	if(e0)// e0��λ
	{
		// ע�⣬�������� 0x7f ���� ~(0x80) ������
		key |= key_map[2 + KEY_MAP_COLS * (scan_code & 0x7f)];
	}
	else if((SHIFT_L_DOWN & mode) || (SHIFT_R_DOWN & mode))// shift����
	{
		key |= key_map[1 + KEY_MAP_COLS * (scan_code & 0x7f)];
	}
	else// !shift and !e0
	{
		key |= key_map[0 + KEY_MAP_COLS * (scan_code & 0x7f)];
	}

	return key;
}

// F: ����Ӧ��ɨ���봦��ɶ�Ӧ�ļ���������mode��lock��e0�������Ӧ�ļ�ת��Ϊ��Ӧ���ַ������ַ���
//    ע������shift+��Ӧ��������һ���ַ����Ӧ���ʲ�Ӧ����ֿ������˴���Linux-0.11���������ƣ���
//        �ڲ��������ж�ʱ���ɹ��ܼ������ַ��������д���
// I: ɨ���롢ת��֮����ַ�ָ��
// O: �����������ַ����򷵻� TRUE
BOOL cook_scan_code(unsigned char scan_code, char *cooked)
{
	unsigned int key = 0;	// ����
	*cooked = 0;			// cooked == 0,����FALSE

	if(0xe0 == scan_code)
	{
		e0 = 1;
	}
	else
	{
		key = scan_to_key(scan_code);
		e0 = 0;					// ע�⣬��λ e0 ���� scan_to_key ������֮��
	
		if(FLAG_BREAK & key)	// BREAK��ֻ����shift��ctrl�� alt
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
			if(key >= FLAG_CONTR_KEY)// ���Ƽ�
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
				case BACKSPACE:				// �˸�
					*cooked = 8; break;
				case TAB:					// ˮƽ�Ʊ�
					*cooked = 9; break;
				case ENTER:					// ����
					*cooked = 10; break;
				case DELETE:				// ɾ��
					*cooked = 127; break;
				default: break;
				}
			}
			else	// С�� FLAG_CONTR_KEY��256�� ����ͨ��
			{
				*cooked = key;
				if((CAPS_LOCK_DOWN & lock) && (('a'<=*cooked) && (*cooked<='z'))) // ��д������������Сд�ַ�
				{
					*cooked -= 32;
				}
			}
		}
	}

	return ((0 == *cooked) ? FALSE : TRUE);
}

// F: ��ʼ������
// I: NULL
// O: NULL
void keyboard_init(void)
{
	/*
	 * ����ԭ�����������! ԭ���ǣ���Ϊ��ʼ��Ϊ0��ȫ�ֱ�����bss���������OS���ʱ�����ʱû����ʼ��Ϊ0���ʳ���
	 * ����ʹ��֮ǰ��ʼ������ʵֻ�Ǹ��ݵ�ַ��ʼ������ʼ��֮ǰ��û�иñ�����
	 */
	// ��Ȼ�ڶ���ȫ�ֱ���ʱ�������˳�ʼ���������ǳ����ˣ������ڴ��ٴγ�ʼ����
	// ������GCC��������֧��ȫ�ֱ�������ʱ��ʼ����������δ֪�� :-(
	mode = lock = e0 = 0;

	kb_queue.q_head = kb_queue.q_tail = kb_queue.count = 0;
	zeromem(kb_queue.q_buff, KEY_BUFF_SIZE * sizeof(char));
}
