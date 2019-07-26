	THUMB
	PRESERVE8
	AREA    |.text|, CODE, READONLY
		
	EXPORT  Interrupt_Disable
	EXPORT  Interrupt_Enable
	EXPORT  HardWare_Clz
	EXPORT	Dos_EnableVFP
	EXPORT  Dos_GetIPSR
	EXPORT  Dos_GetAPSR	
	EXPORT  Dos_GetXPSR
	EXPORT  Dos_GetPSP	
	EXPORT  Dos_GetMSP
	EXPORT  SVC_Handler
	EXPORT  PendSV_Handler
	EXPORT  Dos_StartFirstTask

		
	IMPORT	Dos_CurrentTCB
	IMPORT	Dos_SwitchTask
		

Interrupt_Disable
	mrs     r0, primask
	CPSID   I
	bx      lr

Interrupt_Enable
	msr     primask, r0
	bx      lr
	
HardWare_Clz
	rbit	r0,	r0
	clz 	r0,	r0
	bx      lr

Dos_EnableVFP
	ldr.w r0, =0xE000ED88
	ldr	r1, [r0]
	orr	r1, r1, #0xF00000
	str r1, [r0]
	bx	r14
	nop
	
Dos_GetIPSR
	mrs r0, ipsr
	bx lr
	
Dos_GetAPSR
	mrs r0, apsr
	bx lr
	
Dos_GetXPSR
	mrs r0, xpsr
	bx lr

Dos_GetPSP
	mrs r0, psp
	bx lr

Dos_GetMSP
	mrs r0, msp
	bx lr

SVC_Handler
	ldr	r2, =Dos_CurrentTCB
	ldr r3, [r2]
	ldr r0, [r3]
	ldmia r0!, {r4-r11, lr}
	msr psp, r0
	mov r0, #0
	msr	basepri, r0
	orr lr, #0xd
	bx lr

PendSV_Handler

	mrs r0, psp
	isb
	ldr	r3, =Dos_CurrentTCB
	ldr	r2, [r3]

	tst r14, #0x10
	it eq
	vstmdbeq r0!, {s16-s31}

	stmdb r0!, {r4-r11, r14}

	str r0, [r2]

	stmdb sp!, {r3}
	mov r0, #( 5 << (8 - 4) )
	msr basepri, r0
	dsb
	isb
	bl Dos_SwitchTask
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3}

	ldr r1, [r3]
	ldr r0, [r1]

	ldmia r0!, {r4-r11, r14}


	tst r14, #0x10
	it eq
	vldmiaeq r0!, {s16-s31}

	msr psp, r0
	isb

	bx r14
;    mrs	r1, primask
;    CPSID   I
;	mrs r0, psp
;	ldr	r2, =Dos_CurrentTCB
;	ldr	r3, [r2]
;	tst lr, #0x10
;	it eq
;	vstmdbeq r0!, {s16-s31}
;	stmdb r0!, {r4-r11, lr}
;	str r0, [r3]
;	stmdb sp!, {r1,r2}
;	bl Dos_SwitchTask
;	ldmia sp!, {r1,r2}
;	ldr r3, [r2]
;	ldr r0, [r3]
;	ldmia r0!, {r4-r11, lr}
;	tst lr, #0x10
;	it eq
;	vldmiaeq r0!, {s16-s31}
;	msr psp, r0
;	msr	primask, r1
;	orr	lr, lr, #0x04
;	bx lr


Dos_StartFirstTask

	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]

	msr msp, r0

	cpsie i
	cpsie f
	dsb
	isb

	svc 0
	nop

;	ldr r0, =0xE000ED08
;	ldr r0, [r0]
;	ldr r0, [r0]
;	msr msp, r0
;	cpsie i
;	cpsie f
;	svc 0
	END
