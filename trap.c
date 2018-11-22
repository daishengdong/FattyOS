#include <klib.h>
#include <ktype.h>

struct arm_iframe {
#if ARM_WITH_VFP
    k_uint32_t fpexc;
#endif
    k_uint32_t usp;
    k_uint32_t ulr;
    k_uint32_t r0;
    k_uint32_t r1;
    k_uint32_t r2;
    k_uint32_t r3;
    k_uint32_t r12;
    k_uint32_t lr;
    k_uint32_t pc;
    k_uint32_t spsr;
};


int platform_irq(struct arm_iframe *frame)
{
	print_char('\n');print_char('\n');
	print_int(frame->usp);
	print_char('\t');
	print_int(frame->ulr);
	print_char('\t');
	print_int(frame->r0);
	print_char('\t');
	print_int(frame->r1);
	print_char('\t');
	print_int(frame->r2);
	print_char('\t');
	print_int(frame->r3);
	print_char('\t');
	print_int(frame->r12);
	print_char('\t');
	print_int(frame->lr);
	print_char('\t');
	print_int(frame->pc);
	print_char('\t');
	print_int(frame->spsr);
	print_char('\n');print_char('\n');
}

