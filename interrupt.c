#include <klib.h>
#include <hw.h>
#include "realview.h"


#define MAX_HANDLERS	NR_IRQS_PBA8

#define REALVIEW_GIC_CPU_BASE       0x1E000100  /* Generic interrupt controller CPU interface */
#define REALVIEW_GIC_DIST_BASE      0x1E001000  /* Generic interrupt controller distributor */


/* exception and interrupt handler table */
struct rt_irq_desc isr_table[MAX_HANDLERS];

void interrupt_init(void)
{
	k_uint32_t gic_cpu_base;
	k_uint32_t gic_dist_base;

#if 0 // do it in _vector_reset
	/* initialize vector table */
	rt_hw_vector_init();
#endif

	/* initialize exceptions table */
	k_memset(isr_table, 0x00, sizeof(isr_table));
	
	/* initialize ARM GIC */
	gic_dist_base = REALVIEW_GIC_DIST_BASE;
	gic_cpu_base = REALVIEW_GIC_CPU_BASE;
	
	arm_gic_dist_init(0, gic_dist_base, 0);
	arm_gic_cpu_init(0, gic_cpu_base);

}

/**
 * This function will mask a interrupt.
 * @param vector the interrupt number
 */
void interrupt_mask(int vector)
{
    arm_gic_mask(0, vector);
}

/**
 * This function will un-mask a interrupt.
 * @param vector the interrupt number
 */
void interrupt_umask(int vector)
{
    arm_gic_umask(0, vector);
}

/**
 * This function will install a interrupt service routine to a interrupt.
 * @param vector the interrupt number
 * @param new_handler the interrupt service routine to be installed
 * @param old_handler the old interrupt service routine
 */
rt_isr_handler_t interrupt_install(int vector, rt_isr_handler_t handler,
        void *param, char *name)
{
    rt_isr_handler_t old_handler = NULL;

    if (vector < MAX_HANDLERS)
    {
        old_handler = isr_table[vector].handler;

        if (handler != NULL)
        {
#ifdef RT_USING_INTERRUPT_INFO
            rt_strncpy(isr_table[vector].name, name, RT_NAME_MAX);
#endif /* RT_USING_INTERRUPT_INFO */
            isr_table[vector].handler = handler;
            isr_table[vector].param = param;
        }
    }

    return old_handler;
}

void enable_irq(void)
{

}
