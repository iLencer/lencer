/*
 * Small/init/main.c
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.1 2013-01-30 21:48:27
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

int main(void)
{
	char showmsg[] = "Small/init/main Start !!!";

	// 输出提示信息
	__asm__(
		"movw	$0x18,%%bx\n\t"
		"mov	%%bx,%%gs\n\t"
		"movl	$0,%%ebx\n\t"
		"movl	$25,%%ecx\n\t"/* 25 个字符*/
	"rp:"
		"mov	$0x02,%%ah\n\t"
		"mov	%%ds:(%%edx),%%al\n\t"
		"shl	$1,%%ebx\n\t"
		"movw	%%ax,%%gs:3040(%%ebx)\n\t"/* 3040/2 处开始*/
		"shr	$1,%%ebx\n\t"
		"inc	%%ebx\n\t"
		"inc	%%edx\n\t"
		"cmpl	%%ecx,%%ebx\n\t"
		"jne	rp\n\t"
		:: "d"(showmsg) :);

	__asm__("sti");

	for(;;)
	{}

	return 0;
}