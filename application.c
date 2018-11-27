#include <klib.h>
#include <kheap.h>

void arch_init(void)
{
	return;
}


void kernel_init(void)
{
	void *p = malloc(20);
	print_int((unsigned int)p);
	print_char('Y');
	print_char('\n');

	p = malloc(40);
	print_int((unsigned int)p);
	print_char('x');
	print_char('\n');
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
	kernel_init();

    while (1) {
		delay(500);
		print_str("hello artos!\n");
    }
}
