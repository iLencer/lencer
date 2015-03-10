#
# Small/Makefile
#
# (C) 2012-2013 Yafei Zheng
# V0.1 2013-01-28 12:29:58
#
# Email: e9999e@163.com, QQ: 1039332004
#

# --------------------------------------------
# 变量定义
AS86 		=	as86
LD86 		= 	ld86
AS86EFLAGES	=	-0 -a
LD86EFLAGES	=	-0 -s

AS		= 	as
LD		=	ld
ASEFLAGES	=
# -M选项用于生成map文件
# -N选项取消数据段页对齐，否则会出错（初始化的全局变量访问错误）。
LDEFLAGES	=	-N -s -x -Ttext 0 -e startup_32 -M

CC		=	gcc
# 不使用gcc自带标准头文件，故此编译配置不适用于build.c
# c99标准支持在for循环初始化语句中定义变量
# -fno-builtin 选项，关闭GCC的内置函数替换功能，比如禁止将 printf("abc\n") 替换为 puts("abc")
CEFLAGES		=	-Wall -O0 -std=c99 -Iinclude -nostdinc -fno-builtin


LIBS = lib/lib.a
KERNEL = kernel/kernel.o
# --------------------------------------------

# --------------------------------------------
all: Image

Image: boot/boot tools/system tools/build
	tools/build

# 注意 ： build.c 使用gcc库文件，故不使用include目录作为头文件目录
tools/build: tools/build.c
	$(CC) -o $@ $<

boot/boot: boot/boot.s
	$(AS86) $(AS86EFLAGES) -o boot/boot.o boot/boot.s
	$(LD86) $(LD86EFLAGES) -o boot/boot boot/boot.o

# 以下使用 LD 连接，而不用 CC。命令中各个模块出现的顺序若不正确，则会导致连接错误。
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
# 清除
.PHONY: clean

clean:
	-rm Image System.map \
	boot/boot boot/*.o \
	tools/build tools/system \
	init/main.o
	(cd kernel; make clean)
	(cd lib; make clean)
# --------------------------------------------