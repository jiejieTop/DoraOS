#include <port.h>
#include <dos_config.h>

#ifndef DOS_SYSTEM_CLOCK_HZ
#error "DOS_SYSTEM_CLOCK_HZ must be defined as the system frequency, such as '#define DOS_SYSTEM_CLOCK_HZ (SystemCoreClock)' "
#endif // !DOS_SYSTEM_CLOCK_HZ


extern void dos_task_exit(void);

dos_uint32 *dos_stack_init(dos_uint32 *sp ,
                           void *task_entry,
                           dos_void *parameter)
{
    *--sp = 0x01000000L;  /* xPSR */
    *--sp = ((dos_uint32)task_entry); /* PC */
    *--sp = (dos_uint32)dos_task_exit;  /* LR */
    /* R12, R3, R2 and R1. */
    sp -=4;
    *--sp = ( dos_uint32 ) parameter; /* R0 */
    *--sp = ( dos_uint32 ) 0xfffffffd;
    /* R11, R10, R9, R8, R7, R6, R5 and R4. */
    sp -=8;

    return sp;
}



dos_uint32 dos_port_start( void )
{
    /* Configure PendSV and SysTick with the lowest interrupt priority */
    SYSPRI2_REG |= (PENDSV_PRI | SYSTICK_PRI);


    SYSTICK_LOAD_REG = ( DOS_SYSTEM_CLOCK_HZ / DOS_SYSTICK_CLOCK_HZ ) - 1UL;

    SYSTICK_CTRL_REG = ( SYSTICK_CLK_BIT |
                         SYSTICK_INT_BIT |
                         SYSTICK_ENABLE_BIT );

//    Dos_EnableVFP();

    /* Lazy save always. */
//  *( portFPCCR ) |= portASPEN_AND_LSPEN_BITS;

    /* Start the first task, no longer return */
    Dos_StartFirstTask();

    return 0;
}


/**
 * Is the context environment interrupted?
 */
dos_bool dos_context_is_interrupt(void)
{
    return (Dos_GetIPSR() != 0) ? DOS_TRUE : DOS_FALSE;
}





