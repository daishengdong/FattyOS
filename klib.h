#ifndef __KLIB_H__
#define __KLIB_H__

#include <ktype.h>

#ifdef __cplusplus
extern "C" {
#endif

char *itoa(char *str, int num);
void print_str(char *s);
void print_char(char c);
void print_nchar(char *s, int n);
void print_int(int input);
void printk(const char *fmt, ...);

void delay(int time);
void *k_memset(void *_dst, int c, size_t n);
void * k_memcpy(void * dst, const void *src, unsigned int n);


#ifdef __cplusplus
}
#endif

#endif
