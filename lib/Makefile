#
# Small/lib/Makefile
#
# (C) 2012-2013 Yafei Zheng
# V0.2 2013-02-01 00:34:51
#
# Email: e9999e@163.com, QQ: 1039332004
#

# --------------------------------------------
# 变量定义

AR		=	ar
CC		=	gcc
CEFLAGES		=	-Wall -O0 -std=c99 -I../include  -nostdinc -fno-builtin
# --------------------------------------------

# --------------------------------------------
all: lib.a

OBJS = string.o stdio.o

lib.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

string.o: string.c ../include/string.h
	$(CC) $(CEFLAGES) -c $<

stdio.o: stdio.c ../include/stdarg.h ../include/console.h
	$(CC) $(CEFLAGES) -c $<
# --------------------------------------------

# --------------------------------------------
.PHONY: clean

clean:
	-rm *.o *.a
# --------------------------------------------