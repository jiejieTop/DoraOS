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



#ifndef   __ASM
#define __ASM                     __asm
#endif
#ifndef   __INLINE
#define __INLINE                  __inline
#endif
#ifndef   __STATIC_INLINE
#define __STATIC_INLINE           static __inline
#endif
#ifndef   __NO_RETURN
#define __NO_RETURN               __declspec(noreturn)
#endif
#ifndef   __USED
#define __USED                    __attribute__((used))
#endif
#ifndef   __WEAK
#define __WEAK                    __attribute__((weak))
#endif
#ifndef   __UNALIGNED_UINT32
#define __UNALIGNED_UINT32(x)     (*((__packed uint32_t *)(x)))
#endif
#ifndef   __ALIGNED
#define __ALIGNED(x)              __attribute__((aligned(x)))
#endif
#ifndef   __PACKED
#define __PACKED                  __attribute__((packed))
#endif
#ifndef   __PACKED_STRUCT
#define __PACKED_STRUCT           __packed struct
#endif



typedef dos_void (*Dos_TaskFunction)( dos_void * );

dos_uint32 *dos_stack_init(dos_uint32 *sp ,
                           void *task_entry,
                           dos_void *parameter);

dos_uint32 dos_port_start(dos_void);
dos_bool dos_context_is_interrupt(dos_void);
dos_uint32 Interrupt_Disable(dos_void);
dos_void Interrupt_Enable(dos_uint32 pri);
dos_uint32 hard_ware_clz(dos_uint32 pri);
dos_uint32 Dos_GetIPSR(dos_void);
dos_uint32 Dos_GetAPSR(dos_void);
dos_uint32 Dos_GetXPSR(dos_void);
dos_uint32 Dos_GetPSP(dos_void);
dos_uint32 Dos_GetMSP(dos_void);
dos_void Dos_StartFirstTask(dos_void);


#endif

