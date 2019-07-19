	PRESERVE8
	THUMB
		
	EXPORT  Interrupt_Disable
	EXPORT  Interrupt_Enable
	EXPORT  SVC_Handler
	EXPORT  PendSV_Handler
	EXPORT  Dos_StartFirstTask
		
	IMPORT	Dos_CurrentTCB
	IMPORT	Dos_SwitchTask

	AREA    |.text|, CODE, READONLY

Interrupt_Disable
	mrs     r0, PRIMASK
	CPSID   I
	BX      r14

Interrupt_Enable
	MSR     PRIMASK, r0
	BX      r14
	
SVC_Handler
	ldr	r3, =Dos_CurrentTCB
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
	msr psp, r0
	mov r0, #0
	msr	basepri, r0
	orr r14, #0xd
	bx r14

PendSV_Handler
    mrs	r12, PRIMASK
    CPSID   I
	mrs r0, psp
	ldr	r3, =Dos_CurrentTCB
	ldr	r2, [r3]
	stmdb r0!, {r4-r11}
	str r0, [r2]
	stmdb sp!, {r3, r14}
	bl Dos_SwitchTask 
	ldmia sp!, {r3, r14}
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
	msr psp, r0
	msr	PRIMASK, r12
	orr	lr, lr, #0x04
	bx r14
	
Dos_StartFirstTask
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	msr msp, r0
	cpsie i
	cpsie f
	svc 0 
	nop	
	END
