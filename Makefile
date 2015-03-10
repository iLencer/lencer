#
# Small/Makefile
#
# (C) 2012-2013 Yafei Zheng
# V0.1 2013-01-28 12:29:58
#
# Email: e9999e@163.com, QQ: 1039332004
#

# --------------------------------------------
# ��������
AS86 		=	as86
LD86 		= 	ld86
AS86EFLAGES	=	-0 -a
LD86EFLAGES	=	-0 -s

AS		= 	as
LD		=	ld
ASEFLAGES	=
# -Mѡ����������map�ļ�
# -Nѡ��ȡ�����ݶ�ҳ���룬����������ʼ����ȫ�ֱ������ʴ��󣩡�
LDEFLAGES	=	-N -s -x -Ttext 0 -e startup_32 -M

CC		=	gcc
# ��ʹ��gcc�Դ���׼ͷ�ļ����ʴ˱������ò�������build.c
# c99��׼֧����forѭ����ʼ������ж������
# -fno-builtin ѡ��ر�GCC�����ú����滻���ܣ������ֹ�� printf("abc\n") �滻Ϊ puts("abc")
CEFLAGES		=	-Wall -O0 -std=c99 -Iinclude -nostdinc -fno-builtin


LIBS = lib/lib.a
KERNEL = kernel/kernel.o
# --------------------------------------------

# --------------------------------------------
all: Image

Image: boot/boot tools/system tools/build
	tools/build

# ע�� �� build.c ʹ��gcc���ļ����ʲ�ʹ��includeĿ¼��Ϊͷ�ļ�Ŀ¼
tools/build: tools/build.c
	$(CC) -o $@ $<

boot/boot: boot/boot.s
	$(AS86) $(AS86EFLAGES) -o boot/boot.o boot/boot.s
	$(LD86) $(LD86EFLAGES) -o boot/boot boot/boot.o

# ����ʹ�� LD ���ӣ������� CC�������и���ģ����ֵ�˳��������ȷ����ᵼ�����Ӵ���
tools/system: boot/head.o init/main.o $(KERNEL) $(LIBS)
	$(LD) $(LDEFLAGES) \
	boot/head.o init/main.o $(KERNEL) $(LIBS) \
	-o tools/system > System.map

lib/lib.a:
	(cd lib; make)

kernel/kernel.o:
	(cd kernel; make)

init/main.o: init/main.c include/sys_set.h include/sys_nr.h include/keyboard.h include/string.h include/stdio.h
	$(CC) $(CEFLAGES) -c $< -o $@

boot/head.o: boot/head.s
	$(AS) $(ASEFLAGES) -o boot/head.o boot/head.s
# --------------------------------------------

# --------------------------------------------
# ���
.PHONY: clean

clean:
	-rm Image System.map \
	boot/boot boot/*.o \
	tools/build tools/system \
	init/main.o
	(cd kernel; make clean)
	(cd lib; make clean)
# --------------------------------------------