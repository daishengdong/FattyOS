#include <klib.h>

void arch_init(void)
{
	return;
}




void platform_init(void)
{
	interrupt_init();
	timer_init();
}

void artos_main(void)
{
    arch_init();
	platform_init();

    while (1) {
		delay(100);
		print_str("hello artos!\n");
    }
}
