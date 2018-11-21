#define UFCON0  ((volatile unsigned int *)(0x10009000))

void print(char *p) {
    while (*p) {
        *UFCON0 = *p++;
    };
}
