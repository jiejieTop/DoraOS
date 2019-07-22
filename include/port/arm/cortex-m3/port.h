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


/**
 * Get IPSR Register 
 * Returns the content of the IPSR Register.
 * return IPSR Register value
 */
__STATIC_INLINE dos_uint32 __get_IPSR(void)
{
  register dos_uint32 __regIPSR          __ASM("ipsr");
  return(__regIPSR);
}


/**
 * Get APSR Register
 * Returns the content of the APSR Register.
 * return APSR Register value
 */
__STATIC_INLINE dos_uint32 __get_APSR(void)
{
  register dos_uint32 __regAPSR          __ASM("apsr");
  return(__regAPSR);
}


/**
 * Get xPSR Register
 * Returns the content of the xPSR Register.
 * return xPSR Register value
 */
__STATIC_INLINE dos_uint32 __get_xPSR(void)
{
  register dos_uint32 __regXPSR          __ASM("xpsr");
  return(__regXPSR);
}


/**
 * Get Process Stack Pointer
 * Returns the current value of the Process Stack Pointer (PSP).
 * return PSP Register value
 */
__STATIC_INLINE dos_uint32 __get_PSP(void)
{
  register dos_uint32 __regProcessStackPointer  __ASM("psp");
  return(__regProcessStackPointer);
}


/**
 * Set Process Stack Pointer
 * Assigns the given value to the Process Stack Pointer (PSP).
 * @param [in]  topOfProcStack  Process Stack Pointer value to set
 */
__STATIC_INLINE void __set_PSP(dos_uint32 topOfProcStack)
{
  register dos_uint32 __regProcessStackPointer  __ASM("psp");
  __regProcessStackPointer = topOfProcStack;
}


/**
 * Get Main Stack Pointer
 * Returns the current value of the Main Stack Pointer (MSP).
 * return MSP Register value
 */
__STATIC_INLINE dos_uint32 __get_MSP(void)
{
  register dos_uint32 __regMainStackPointer     __ASM("msp");
  return(__regMainStackPointer);
}


/**
 * Set Main Stack Pointer
 * Assigns the given value to the Main Stack Pointer (MSP).
 * @param [in] topOfMainStack  Main Stack Pointer value to set
 */
__STATIC_INLINE void __set_MSP(dos_uint32 topOfMainStack)
{
  register dos_uint32 __regMainStackPointer     __ASM("msp");
  __regMainStackPointer = topOfMainStack;
}


typedef dos_void (*Dos_TaskFunction)( dos_void * );

dos_uint32 *Dos_StackInit(dos_uint32 *sp , 
                          void *task_entry,
                          dos_void *parameter);
                          
dos_uint32 Dos_StartScheduler( dos_void );
dos_bool Dos_ContextIsInt(dos_void);
dos_uint32 Interrupt_Disable(dos_void);
dos_void Interrupt_Enable(dos_uint32 pri);
dos_uint32 HardWare_Clz(dos_uint32 pri);
dos_void Dos_StartFirstTask( dos_void );   


#endif

