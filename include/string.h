/*
 * Small/include/string.h
 * 
 * (C) 2012-2013 Yafei Zheng
 * V0.2 2013-01-31 23:57:47
 *
 * Email: e9999e@163.com, QQ: 1039332004
 */

#ifndef _STRING_H_
#define _STRING_H_
//**********************************************************************

extern int strlen(char *str);
extern char * strcpy(char *des, char *src);

extern void * memcpy(void *des, void *src, int count);
extern void * memset(void *des, char c, int count);
extern void * zeromem(void *des, int count);

//**********************************************************************
#endif // _STRING_H_