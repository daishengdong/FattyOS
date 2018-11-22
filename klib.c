#include <klib.h>

#define UART0_BASE  ((volatile unsigned int *)(0x10009000))

#define UART0_OUTPUT(c) (*UART0_BASE = c)

// ------------------------------------------------------------------------
// 仿 itoa
// 数字前面的 0 不被显示出来, 比如 000B800 被显示成 B800
char *itoa(char *str, int num)
{
    char    *p = str;
    char    ch;
    int     i;
    // flag 用来标识第一个非 0 的数
    int     flag = 0;

    *p++ = '0';
    *p++ = 'x';
    if (num == 0)
		*p++ = '0';
    else
        for (i = 28; i >= 0; i -= 4) {
            ch = (num >> i) & 0xF;
            if (flag || (ch > 0)) {
                flag = 1;
                ch += '0';
                if (ch > '9')
					ch += 7;
                *p++ = ch;
            }
        }
    *p = 0;
    return str;
}

void print_nchar(char *s, int n)
{
	while (*s && n--) {
		UART0_OUTPUT(*s++);
	}
}

void print_str(char *s)
{
    while (*s) {
        UART0_OUTPUT(*s++);
    };
}

void print_char(char c)
{
	UART0_OUTPUT(c);
}

void print_int(int input)
{
    char output[16];
    itoa(output, input);
    print_str(output);
}

void __attribute__((optimize("O0"))) delay(int time)
{
    int i, j, k;
    for (k = 0; k < time; ++k)
        for (i = 0; i < 100; ++i)
            for (j = 0; j < 10000; ++j)
                ;
}

void *k_memset(void *dst, int c, size_t n)
{
    k_uint8_t *d = dst;
    while (n-- > 0) {
        *d++ = c;
    }
    return dst;
}

void * k_memcpy(void * dst, const void *src, unsigned int n) {
	char *tmp = (char *) dst, *s = (char *) src;
	while (n--)
		*tmp++ = *s++;
	return dst;
}
