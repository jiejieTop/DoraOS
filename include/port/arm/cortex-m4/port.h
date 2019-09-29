#ifndef _PORT_H_
#define _PORT_H_

#include <dos_def.h>
#include <task.h>


#define         SYSPRI2_REG                 ( * ( ( volatile dos_uint32 * ) 0xe000ed20 ) )
 
#define         PENDSV_PRI                  ( ( ( dos_uint32 ) 0xFF ) << 16UL )
#define         SYSTICK_PRI                 ( ( ( dos_uint32 ) 0xFF ) << 24UL )


#define         INT_CTRL_REG                ( * ( ( volatile dos_uint32 * ) 0xe000ed04 ) )
#define         PENDSVSET_BIT               ( 1UL << 28UL )

#ifndef         SYSTICK_CLOCK_HZ
    #define     SYSTICK_CLOCK_HZ            1000
#endif

/* Constants required to manipulate the VFP. */
#define portFPCCR					( ( volatile uint32_t * ) 0xe000ef34 ) /* Floating point context control register. */
#define portASPEN_AND_LSPEN_BITS	( 0x3UL << 30UL )

#define         SYSTICK_CTRL_REG            ( * ( ( volatile dos_uint32 * ) 0xe000e010 ) )
#define         SYSTICK_LOAD_REG            ( * ( ( volatile dos_uint32 * ) 0xe000e014 ) )
#define         SYSTICK_VAL_REG             ( * ( ( volatile dos_uint32 * ) 0xe000e018 ) )
#define         SYSPRI2_REG                 ( * ( ( volatile dos_uint32 * ) 0xe000ed20 ) )
  
#define         SYSTICK_ENABLE_BIT          ( 1UL << 0UL )
#define         SYSTICK_INT_BIT             ( 1UL << 1UL )
#define         SYSTICK_CLK_BIT             ( 1UL << 2UL )
#define         SYSTICK_COUNT_FLAG_BIT      ( 1UL << 16UL )

#define         PENDSVCLEAR_BIT             ( 1UL << 27UL )
#define         PEND_SYSTICK_CLEAR_BIT      ( 1UL << 25UL )


#define DOS_TASK_YIELD()            \
{                                   \
  INT_CTRL_REG |= PENDSVSET_BIT;    \
}

typedef dos_void (*Dos_TaskFunction)( dos_void * );

dos_uint32 *dos_stack_init(dos_uint32 *sp , 
                          void *task_entry,
                          dos_void *parameter);
                          
dos_uint32 dos_start_scheduler(dos_void);
dos_bool dos_context_is_interrupt(dos_void);
dos_uint32 Interrupt_Disable(dos_void);
dos_void Interrupt_Enable(dos_uint32 pri);
dos_uint32 hard_ware_clz(dos_uint32 pri);
dos_void Dos_EnableVFP(dos_void);
dos_uint32 Dos_GetIPSR(dos_void);
dos_uint32 Dos_GetAPSR(dos_void);
dos_uint32 Dos_GetXPSR(dos_void);
dos_uint32 Dos_GetPSP(dos_void);
dos_uint32 Dos_GetMSP(dos_void);
dos_void Dos_StartFirstTask(dos_void);   


#endif

