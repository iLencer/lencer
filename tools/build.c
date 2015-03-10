/*
 * Small/tools/build.c
 *
 * (C) 2012-2013 Yafei Zheng
 * V0.0 2012-12-7 19:53:15
 * V0.1 2013-01-30 19:44:27
 * V0.2 2013-02-08 10:43:16
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

/*
 * 此程序可以在windows(VC6.0)下使用，也可以在Unix/Linux(GCC 4.6.3)下使用。
 * 此程序去除相关的MINIX头和GCC头，将OS各模块组装到一块。内核ld时，需用选项 -Ttext
 * 设置代码段开始的虚拟地址。
 */

#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 1024			// 缓冲区字节数

#define OFFSET_BOOT 32			// 从文件开始处的偏移量(字节数),用来去除boot.s被as86编译之后的MINIX头(32B)

/*
 * 从0.2版开始，以下 宏定义 及 相应的注释 作废!!! 在组装时，开始对ELF格式的system模块进行正规处理。因为在
 * ELF文件中可执行语句在文件中开始位置不一定是0x1000，故读取ELF文件头，以确定可执行代码在文件中开始位置。
 *
 * 另外，对于ELF文件，在 ld 链接时默认是页对齐的(页大小0x1000)。需使用 -N 选项，关闭数据段页对齐，避免数据段
 * 被载入之后，data段和bss段数据引用出错!!! 因为这是 OS，而不是建立在OS基础之上的应用程序!!!
 *
 * 还需注意，对于bss段，被载入之后，并不会初始化为0 !!! 因为这是 OS，而不是建立在OS基础之上的应用程序!!! 
 * 对于内核，若是未初始化或初始化为0的全局变量，则在ld时会被放置bss段里，故在使用之前必须初始化，即使是
 * 已初始化为0的全局变量。
 */
//#define OFFSET_SYSTEM 0x1000	// 去除system模块的0x1000B的GCC头。在Linux-0.11中，是1024B，因为当时的GCC生
								// 成的可执行文件是 a.out 格式的，而我用的GCC(gcc 4.6.3 for Start OS)生成的
								// 是 ELF 格式。关于ELF文件头，请参考ELF相关内容。

/*
 * 如上面的注释，从0.2版开始，使用以下结构解析可执行代码在文件中的开始位置。
 *
 * 以下结构体中的成员是ELF文件中 ELF头和程序头表 中的成员节选，其中ELF头在文件最开始处，程序头表紧接着ELF头。
 * 可执行代码开始处相对于文件开始处偏移值 = p_offset
 */
struct elf_header {
	unsigned short e_ehsize;	/* ELF头部长度，该成员在ELF头中偏移值为0x28 */
	unsigned long p_offset;		/* 代码段位置相对于文件开始处的偏移量，该成员在程序头表中偏移值为0x4 */
}elf;

int main(void)
{
	char buff[BUFF_SIZE] = {0};
	FILE *fp_system = NULL, *fp_boot = NULL, *fp_Image = NULL;
	int count = 0, size_os = 0;

	if(! ((fp_system=fopen("tools/system","rb")) && (fp_boot=fopen("boot/boot","rb")) && (fp_Image=fopen("Image","wb"))))
	{
		printf("Error: can't open some file!!!\n");
		printf("\npress Enter to exit...");
		getchar();
		return -1;
	}

	fseek(fp_boot, OFFSET_BOOT, SEEK_SET);

	// 0.2版与以前的区别在这里
	fseek(fp_system, 0x28L, SEEK_SET);
	fread(buff, sizeof(unsigned short), 1, fp_system);	// 注意，这里不能用 fscanf()
	elf.e_ehsize = *(unsigned short*)buff;
	fseek(fp_system, elf.e_ehsize+0x4L, SEEK_SET);
	fread(buff, sizeof(unsigned long), 1, fp_system);
	elf.p_offset = *(unsigned long*)buff;
	fseek(fp_system, elf.p_offset, SEEK_SET);
//	printf("elf.e_ehsize = [16]%X\n", elf.e_ehsize);
//	printf("elf.p_offset = [16]%X\n", elf.p_offset);

	for(; (count=fread(buff,1,sizeof(buff),fp_boot))>0; size_os+=count,fwrite(buff,count,1,fp_Image)) {}
	fclose(fp_boot);
	for(; (count=fread(buff,1,sizeof(buff),fp_system))>0; size_os+=count,fwrite(buff,count,1,fp_Image)) {}
	fclose(fp_system);
	fclose(fp_Image);
	
	printf("===OK!\nSize of OS is %d Bytes.\n",size_os);

	printf("build exit.\n\n");
	return 0;
}


/*

// 0.2版以前的

int main(void)
{
	char buff[BUFF_SIZE] = {0};
	FILE *fp_system = NULL, *fp_boot = NULL, *fp_Image = NULL;
	int count = 0, size_os = 0;

	if(! ((fp_system=fopen("tools/system","rb")) && (fp_boot=fopen("boot/boot","rb")) && (fp_Image=fopen("Image","wb"))))
	{
		printf("Error: can't open some file!!!\n");
		printf("\npress Enter to exit...");
		getchar();
		return -1;
	}

	fseek(fp_boot,OFFSET_BOOT,SEEK_SET);
	fseek(fp_system,OFFSET_SYSTEM,SEEK_SET);

	for(; (count=fread(buff,1,sizeof(buff),fp_boot))>0; size_os+=count,fwrite(buff,count,1,fp_Image)) {}
	fclose(fp_boot);
	for(; (count=fread(buff,1,sizeof(buff),fp_system))>0; size_os+=count,fwrite(buff,count,1,fp_Image)) {}
	fclose(fp_system);
	fclose(fp_Image);
	
	printf("===OK!\nSize of OS is %d Bytes.\n",size_os);

	printf("build exit.\n\n");
	return 0;
}

*/
