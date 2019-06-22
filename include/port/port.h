#ifndef _PORT_H_
#define _PORT_H_

#include <dos_def.h>

#include <task.h>
#include "./led/bsp_led.h" 

#define       SYSPRI2_REG				          ( * ( ( volatile dos_uint32 * ) 0xe000ed20 ) )
 
#define       PENDSV_PRI				          ( ( ( dos_uint32 ) 0xFF ) << 16UL )
#define       SYSTICK_PRI				          ( ( ( dos_uint32 ) 0xFF ) << 24UL )


#define       INT_CTRL_REG		            ( * ( ( volatile dos_uint32 * ) 0xe000ed04 ) )
#define       PENDSVSET_BIT		            ( 1UL << 28UL )

#ifndef       SYSTICK_CLOCK_HZ
	#define       SYSTICK_CLOCK_HZ          1000
#endif

#define       SYSTICK_CTRL_REG			      ( * ( ( volatile dos_uint32 * ) 0xe000e010 ) )
#define       SYSTICK_LOAD_REG			      ( * ( ( volatile dos_uint32 * ) 0xe000e014 ) )
#define       SYSTICK_VAL_REG	            ( * ( ( volatile dos_uint32 * ) 0xe000e018 ) )
#define       SYSPRI2_REG				          ( * ( ( volatile dos_uint32 * ) 0xe000ed20 ) )
  
#define       SYSTICK_ENABLE_BIT			    ( 1UL << 0UL )
#define       SYSTICK_INT_BIT			        ( 1UL << 1UL )
#define       SYSTICK_CLK_BIT	            ( 1UL << 2UL )
#define       SYSTICK_COUNT_FLAG_BIT		  ( 1UL << 16UL )

#define       PENDSVCLEAR_BIT 			      ( 1UL << 27UL )
#define       PEND_SYSTICK_CLEAR_BIT		  ( 1UL << 25UL )


#define       Dos_Interrupt_Disable       Interrupt_Disable
#define       Dos_Interrupt_Enable        Interrupt_Enable

#define DOS_TASK_YIELD()			      \
{                                   \
  INT_CTRL_REG = PENDSVSET_BIT;     \
}

typedef void (*Dos_TaskFunction)( void * );

dos_uint32 *Dos_StackInit(dos_uint32 *top_of_stack , 
                          void *dos_task_entry,
                          dos_void *parameter);
                          
dos_uint32 Dos_StartScheduler( void );

__asm dos_uint32 Dos_Interrupt_Disable(void);

__asm void Dos_Interrupt_Enable(dos_uint32 pri);


#endif

