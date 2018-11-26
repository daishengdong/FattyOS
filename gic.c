#include <klib.h>
#include <ktype.h>
#include <realview.h>
#include <gic.h>

struct arm_gic
{
    k_uint32_t offset;

    k_uint32_t dist_hw_base;
    k_uint32_t cpu_hw_base;
};
static struct arm_gic _gic_table[ARM_GIC_MAX_NR];

#define GIC_CPU_CTRL(hw_base)               __REG32((hw_base) + 0x00)
#define GIC_CPU_PRIMASK(hw_base)            __REG32((hw_base) + 0x04)
#define GIC_CPU_BINPOINT(hw_base)           __REG32((hw_base) + 0x08)
#define GIC_CPU_INTACK(hw_base)             __REG32((hw_base) + 0x0c)
#define GIC_CPU_EOI(hw_base)                __REG32((hw_base) + 0x10)
#define GIC_CPU_RUNNINGPRI(hw_base)         __REG32((hw_base) + 0x14)
#define GIC_CPU_HIGHPRI(hw_base)            __REG32((hw_base) + 0x18)

#define GIC_DIST_CTRL(hw_base)              __REG32((hw_base) + 0x000)
#define GIC_DIST_TYPE(hw_base)              __REG32((hw_base) + 0x004)
#define GIC_DIST_IGROUP(hw_base, n)         __REG32((hw_base) + 0x080 + ((n)/32) * 4)
#define GIC_DIST_ENABLE_SET(hw_base, n)     __REG32((hw_base) + 0x100 + ((n)/32) * 4)
#define GIC_DIST_ENABLE_CLEAR(hw_base, n)   __REG32((hw_base) + 0x180 + ((n)/32) * 4)
#define GIC_DIST_PENDING_SET(hw_base, n)    __REG32((hw_base) + 0x200 + ((n)/32) * 4)
#define GIC_DIST_PENDING_CLEAR(hw_base, n)  __REG32((hw_base) + 0x280 + ((n)/32) * 4)
#define GIC_DIST_ACTIVE_SET(hw_base, n)     __REG32((hw_base) + 0x300 + ((n)/32) * 4)
#define GIC_DIST_ACTIVE_CLEAR(hw_base, n)   __REG32((hw_base) + 0x380 + ((n)/32) * 4)
#define GIC_DIST_PRI(hw_base, n)            __REG32((hw_base) + 0x400 +  ((n)/4) * 4)
#define GIC_DIST_TARGET(hw_base, n)         __REG32((hw_base) + 0x800 +  ((n)/4) * 4)
#define GIC_DIST_CONFIG(hw_base, n)         __REG32((hw_base) + 0xc00 + ((n)/16) * 4)
#define GIC_DIST_SOFTINT(hw_base)           __REG32((hw_base) + 0xf00)
#define GIC_DIST_CPENDSGI(hw_base, n)       __REG32((hw_base) + 0xf10 + ((n)/4) * 4)
#define GIC_DIST_ICPIDR2(hw_base)           __REG32((hw_base) + 0xfe8)

static unsigned int _gic_max_irq;

#define RT_ASSERT(exp) \
	if (!exp) { \
		printk("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__); \
	}

void arm_gic_mask(k_uint32_t index, int irq)
{
    k_uint32_t mask = 1 << (irq % 32);

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

    GIC_DIST_ENABLE_CLEAR(_gic_table[index].dist_hw_base, irq) = mask;
}

void arm_gic_umask(k_uint32_t index, int irq)
{
    k_uint32_t mask = 1 << (irq % 32);

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

	// TODO: I don't know why, but it works. Maybe uart is not init. Uart code will bring in later.
	print_str("++++\n");
    GIC_DIST_ENABLE_SET(_gic_table[index].dist_hw_base, irq) = mask;
}

int arm_gic_dist_init(k_uint32_t index, k_uint32_t dist_base, int irq_start)
{
    unsigned int gic_type, i;
    k_uint32_t cpumask = 1 << 0;

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    _gic_table[index].dist_hw_base = dist_base;
    _gic_table[index].offset = irq_start;

    /* Find out how many interrupts are supported. */
    gic_type = GIC_DIST_TYPE(dist_base);
    _gic_max_irq = ((gic_type & 0x1f) + 1) * 32;

    /*
     * The GIC only supports up to 1020 interrupt sources.
     * Limit this to either the architected maximum, or the
     * platform maximum.
     */
    if (_gic_max_irq > 1020)
        _gic_max_irq = 1020;
    if (_gic_max_irq > ARM_GIC_NR_IRQS)
        _gic_max_irq = ARM_GIC_NR_IRQS;

    cpumask |= cpumask << 8;
    cpumask |= cpumask << 16;

    GIC_DIST_CTRL(dist_base) = 0x0;

    /* Set all global interrupts to be level triggered, active low. */
    for (i = 32; i < _gic_max_irq; i += 16)
        GIC_DIST_CONFIG(dist_base, i) = 0x0;

    /* Set all global interrupts to this CPU only. */
    for (i = 32; i < _gic_max_irq; i += 4)
        GIC_DIST_TARGET(dist_base, i) = cpumask;

    /* Set priority on all interrupts. */
    for (i = 0; i < _gic_max_irq; i += 4)
        GIC_DIST_PRI(dist_base, i) = 0xa0a0a0a0;

    /* Disable all interrupts. */
    for (i = 0; i < _gic_max_irq; i += 32)
        GIC_DIST_ENABLE_CLEAR(dist_base, i) = 0xffffffff;

#if 0
    /* All interrupts defaults to IGROUP1(IRQ). */
    for (i = 0; i < _gic_max_irq; i += 32)
        GIC_DIST_IGROUP(dist_base, i) = 0xffffffff;
#endif

    /* Enable group0 and group1 interrupt forwarding. */
    GIC_DIST_CTRL(dist_base) = 0x03;

    return 0;
}

int arm_gic_cpu_init(k_uint32_t index, k_uint32_t cpu_base)
{
    RT_ASSERT(index < ARM_GIC_MAX_NR);

    _gic_table[index].cpu_hw_base = cpu_base;

    GIC_CPU_PRIMASK(cpu_base) = 0xf0;
    /* Enable CPU interrupt */
    GIC_CPU_CTRL(cpu_base) = 0x01;

    return 0;
}

