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
 * �˳��������windows(VC6.0)��ʹ�ã�Ҳ������Unix/Linux(GCC 4.6.3)��ʹ�á�
 * �˳���ȥ����ص�MINIXͷ��GCCͷ����OS��ģ����װ��һ�顣�ں�ldʱ������ѡ�� -Ttext
 * ���ô���ο�ʼ�������ַ��
 */

#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 1024			// �������ֽ���

#define OFFSET_BOOT 32			// ���ļ���ʼ����ƫ����(�ֽ���),����ȥ��boot.s��as86����֮���MINIXͷ(32B)

/*
 * ��0.2�濪ʼ������ �궨�� �� ��Ӧ��ע�� ����!!! ����װʱ����ʼ��ELF��ʽ��systemģ��������洦����Ϊ��
 * ELF�ļ��п�ִ��������ļ��п�ʼλ�ò�һ����0x1000���ʶ�ȡELF�ļ�ͷ����ȷ����ִ�д������ļ��п�ʼλ�á�
 *
 * ���⣬����ELF�ļ����� ld ����ʱĬ����ҳ�����(ҳ��С0x1000)����ʹ�� -N ѡ��ر����ݶ�ҳ���룬�������ݶ�
 * ������֮��data�κ�bss���������ó���!!! ��Ϊ���� OS�������ǽ�����OS����֮�ϵ�Ӧ�ó���!!!
 *
 * ����ע�⣬����bss�Σ�������֮�󣬲������ʼ��Ϊ0 !!! ��Ϊ���� OS�������ǽ�����OS����֮�ϵ�Ӧ�ó���!!! 
 * �����ںˣ�����δ��ʼ�����ʼ��Ϊ0��ȫ�ֱ���������ldʱ�ᱻ����bss�������ʹ��֮ǰ�����ʼ������ʹ��
 * �ѳ�ʼ��Ϊ0��ȫ�ֱ�����
 */
//#define OFFSET_SYSTEM 0x1000	// ȥ��systemģ���0x1000B��GCCͷ����Linux-0.11�У���1024B����Ϊ��ʱ��GCC��
								// �ɵĿ�ִ���ļ��� a.out ��ʽ�ģ������õ�GCC(gcc 4.6.3 for Start OS)���ɵ�
								// �� ELF ��ʽ������ELF�ļ�ͷ����ο�ELF������ݡ�

/*
 * �������ע�ͣ���0.2�濪ʼ��ʹ�����½ṹ������ִ�д������ļ��еĿ�ʼλ�á�
 *
 * ���½ṹ���еĳ�Ա��ELF�ļ��� ELFͷ�ͳ���ͷ�� �еĳ�Ա��ѡ������ELFͷ���ļ��ʼ��������ͷ�������ELFͷ��
 * ��ִ�д��뿪ʼ��������ļ���ʼ��ƫ��ֵ = p_offset
 */
struct elf_header {
	unsigned short e_ehsize;	/* ELFͷ�����ȣ��ó�Ա��ELFͷ��ƫ��ֵΪ0x28 */
	unsigned long p_offset;		/* �����λ��������ļ���ʼ����ƫ�������ó�Ա�ڳ���ͷ����ƫ��ֵΪ0x4 */
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

	// 0.2������ǰ������������
	fseek(fp_system, 0x28L, SEEK_SET);
	fread(buff, sizeof(unsigned short), 1, fp_system);	// ע�⣬���ﲻ���� fscanf()
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

// 0.2����ǰ��

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
