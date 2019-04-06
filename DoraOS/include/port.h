#ifndef _PORT_H_
#define _PORT_H_

#include <dos_def.h>

#include <task.h>
#include "./led/bsp_led.h" 


//#define portSY_FULL_READ_WRITE		( 15 )

#define Dos_Interrupt_Disable   Interrupt_Disable
#define Dos_Interrupt_Enable    Interrupt_Enable

#define portYIELD()																\
{																				\
	/* 触发PendSV，产生上下文切换 */								                \
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;								\
	__dsb( portSY_FULL_READ_WRITE );											\
	__isb( portSY_FULL_READ_WRITE );											\
}


typedef void (*Dos_TaskFunction)( void * );

dos_uint32 *Dos_StackInit(dos_uint32 *top_of_stack , 
                          void *dos_task_entry,
                          dos_void *parameter);
                          
dos_uint32 Dos_StartScheduler( void );

__asm dos_uint32 Dos_Interrupt_Disable(void);

__asm void Dos_Interrupt_Enable(dos_uint32 pri);


#endif

