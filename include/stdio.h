/*
 * Small/include/stdio.h
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-02-07 00:07:47
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

#ifndef _STDIO_H_
#define _STDIO_H_
//**********************************************************************
#include "stdarg.h"

extern int itoa(char *buff, int value, int base, unsigned int flag);
extern int vsprintf(char *buff, const char *fmt, va_list args);
extern int printf(const char *fmt, ...);

//**********************************************************************
#endif // _STDIO_H_