#ifndef __HW_H__
#define __HW_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Interrupt handler definition
 */
typedef void (*rt_isr_handler_t)(int vector, void *param);

struct rt_irq_desc {
	rt_isr_handler_t handler;
	void            *param;
};


#ifdef __cplusplus
}
#endif

#endif

