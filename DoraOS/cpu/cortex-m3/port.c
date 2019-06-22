#include <port.h>

//static void Dos_TaskExit( void )
//{
//  DOS_PRINT_DEBUG("asdasd");
//	while(1);
//}
extern void Dos_TaskExit(void);
dos_uint32 *Dos_StackInit(dos_uint32 *top_of_stack , 
                          void *dos_task_entry,
                          dos_void *parameter)
{
	top_of_stack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
	*top_of_stack = 0x01000000;	/* xPSR */
	top_of_stack--;
	*top_of_stack = ((dos_uint32)dos_task_entry)&0xfffffffeUL;	/* PC */
	top_of_stack--;
	*top_of_stack = (dos_uint32)Dos_TaskExit;	/* LR */
	/* R12, R3, R2 and R1. */
  top_of_stack --;	
  top_of_stack --;	
  top_of_stack --;	
  top_of_stack --;	
  top_of_stack --;	
	*top_of_stack = ( dos_uint32 ) parameter;	/* R0 */
  /* R11, R10, R9, R8, R7, R6, R5 and R4. */
	top_of_stack --;	
  top_of_stack --;
  top_of_stack --;
  top_of_stack --;
  top_of_stack --;
  top_of_stack --;
  top_of_stack --;
  top_of_stack --;
  
  return top_of_stack;
}



__asm void SVC_Handler( void )
{
  extern Dos_CurrentTCB;
  
  PRESERVE8

	ldr	r3, =Dos_CurrentTCB	  /* 加载Dos_CurrentTCB的地址到r3 */
	ldr r1, [r3]			        /* 加载Dos_CurrentTCB到r1 */
	ldr r0, [r1]			        /* 加载Dos_CurrentTCB指向的值到r0，目前r0的值等于第一个任务堆栈的栈顶 */
	ldmia r0!, {r4-r11}		    /* 以r0为基地址，将栈里面的内容加载到r4~r11寄存器，同时r0会递增 */
	msr psp, r0				        /* 将r0的值，即任务的栈指针更新到psp */
	isb
	mov r0, #0              /* 设置r0的值为0 */
	msr	basepri, r0         /* 设置basepri寄存器的值为0，即所有的中断都没有被屏蔽 */
	orr r14, #0xd           /* 当从SVC中断服务退出前,通过向r14寄存器最后4位按位或上0x0D，
                               使得硬件在退出时使用进程堆栈指针PSP完成出栈操作并返回后进入线程模式、返回Thumb状态 */
    
	bx r14                  /* 异常返回，这个时候栈中的剩下内容将会自动加载到CPU寄存器：
                               xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0（任务的形参）
                               同时PSP的值也将更新，即指向任务栈的栈顶 */
}

__asm dos_uint32 Interrupt_Disable(void) 
{
  PRESERVE8
  mrs     r0, PRIMASK
  CPSID   I
  BX      LR
  nop
  nop
}

__asm void Interrupt_Enable(dos_uint32 pri) 
{
  PRESERVE8
  MSR     PRIMASK, r0
  BX      LR
  nop
  nop
}

__asm void PendSV_Handler( void )
{
	extern Dos_CurrentTCB;
	extern Dos_SwitchTask;

	PRESERVE8

    /* 当进入PendSVC Handler时，上一个任务运行的环境即：
       xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0（任务的形参）
       这些CPU寄存器的值会自动保存到任务的栈中，剩下的r4~r11需要手动保存 */
    /* 获取任务栈指针到r0 */
	mrs r0, psp
	isb

	ldr	r3, =Dos_CurrentTCB		/* 加载Dos_CurrentTCB的地址到r3 */
	ldr	r2, [r3]                /* 加载Dos_CurrentTCB到r2 */

	stmdb r0!, {r4-r11}			/* 将CPU寄存器r4~r11的值存储到r0指向的地址 */
	str r0, [r2]                /* 将任务栈的新的栈顶指针存储到当前任务TCB的第一个成员，即栈顶指针 */				
                               

	stmdb sp!, {r3, r14}        /* 将R3和R14临时压入堆栈，因为即将调用函数Dos_SwitchTask,
                                  调用函数时,返回地址自动保存到R14中,所以一旦调用发生,R14的值会被覆盖,因此需要入栈保护;
                                  R3保存的当前激活的任务TCB指针(Dos_CurrentTCB)地址,函数调用后会用到,因此也要入栈保护 */
  bl Interrupt_Disable
	dsb
	isb
	bl Dos_SwitchTask       /* 调用函数Dos_SwitchTask，寻找新的任务运行,通过使变量Dos_CurrentTCB指向新的任务来实现任务切换 */ 
  
  bl Interrupt_Enable
	ldmia sp!, {r3, r14}        /* 恢复r3和r14 */

	ldr r1, [r3]
	ldr r0, [r1] 				    /* 当前激活的任务TCB第一项保存了任务堆栈的栈顶,现在栈顶值存入R0*/
	ldmia r0!, {r4-r11}			/* 出栈 */
	msr psp, r0
	isb
	bx r14                        
	nop
  nop
  nop
  nop
}

__asm void Dos_StartFirstTask( void )
{
	PRESERVE8

	/* 在Cortex-M中，0xE000ED08是SCB_VTOR这个寄存器的地址，
       里面存放的是向量表的起始地址，即MSP的地址 */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]

	/* 设置主堆栈指针msp的值 */
	msr msp, r0
    
	/* 使能全局中断 */
	cpsie i
	cpsie f
	dsb
	isb
	
    /* 调用SVC去启动第一个任务 */
	svc 0  
	nop
	nop
}


dos_uint32 Dos_StartScheduler( void )
{
    /* 配置PendSV 和 SysTick 的中断优先级为最低 */
	SYSPRI2_REG |= PENDSV_PRI;
  SYSPRI2_REG |= SYSTICK_PRI;

  SYSTICK_LOAD_REG = ( DOS_SYSTEM_CLOCK_HZ / DOS_SYSTICK_CLOCK_HZ ) - 1UL;
  
	SYSTICK_CTRL_REG = ( SYSTICK_CLK_BIT |
                       SYSTICK_INT_BIT |
                       SYSTICK_ENABLE_BIT );

	/* 启动第一个任务，不再返回 */
	Dos_StartFirstTask();

	return 0;
}




