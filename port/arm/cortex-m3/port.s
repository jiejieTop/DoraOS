	THUMB
	PRESERVE8
	AREA    |.text|, CODE, READONLY
		
	EXPORT  Interrupt_Disable
	EXPORT  Interrupt_Enable
	EXPORT  SVC_Handler
	EXPORT  PendSV_Handler
	EXPORT  Dos_StartFirstTask
		
	IMPORT	Dos_CurrentTCB
	IMPORT	Dos_SwitchTask
		

Interrupt_Disable
	mrs     r0, PRIMASK
	CPSID   I
	BX      lr

Interrupt_Enable
	MSR     PRIMASK, r0
	BX      lr
	
SVC_Handler
	ldr	r2, =Dos_CurrentTCB
	ldr r3, [r2]
	ldr r0, [r3]
	ldmia r0!, {r4-r11}
	msr psp, r0
	mov r0, #0
	msr	basepri, r0
	orr lr, #0xd
	bx lr

PendSV_Handler
    mrs	r1, PRIMASK
    CPSID   I
	mrs r0, psp
	ldr	r2, =Dos_CurrentTCB
	ldr r3, [r2]
	stmdb r0!, {r4-r11}
	str r0, [r3] 
	stmdb sp!, {r1, r2, lr}
	bl Dos_SwitchTask 
	ldmia sp!, {r1, r2, lr}
	ldr r3, [r2]
	ldr r0, [r3]
	ldmia r0!, {r4-r11}
	msr psp, r0
	msr	PRIMASK, r1
	orr	lr, lr, #0x04
	bx lr


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
