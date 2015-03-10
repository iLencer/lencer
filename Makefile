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
AS86EFLAGES	=	-0 -a -o
LD86EFLAGES	=	-0 -s -o

AS		= 	as
LD		=	ld
ASEFLAGES	=	-o
LDEFLAGES	=	-Ttext 0 -o

CC		=	gcc
CEFLAGES		=
# --------------------------------------------

# --------------------------------------------
all: Image

Image: boot/boot tools/system tools/build
	tools/build

tools/build: tools/build.c
	$(CC) $(CEFLAGES) -o $@ $<

boot/boot: boot/boot.s
	$(AS86) $(AS86EFLAGES) boot/boot.o boot/boot.s
	$(LD86) $(LD86EFLAGES) boot/boot boot/boot.o

# 以下使用 LD 连接，而不用 CC
tools/system: boot/head.o init/main.o
	$(LD) $(LDEFLAGES) tools/system \
	boot/head.o init/main.o \
	> System.map

init/main.o: init/main.c
	$(CC) $(CEFLAGES) -c $< -o $@

boot/head.o: boot/head.s
	$(AS) $(ASEFLAGES) boot/head.o boot/head.s
# --------------------------------------------

# --------------------------------------------
# 清除
.PHONY: clean

clean:
	-rm Image System.map \
	boot/boot boot/*.o \
	tools/build tools/system \
	init/main.o
# --------------------------------------------