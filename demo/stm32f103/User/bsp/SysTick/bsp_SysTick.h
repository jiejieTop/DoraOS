#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "include.h"

void SysTick_Init(void);

#if USE_TICK_DELAY
void Delay_us(__IO uint32_t us);
#define Delay_ms(ms)	Delay_us(100*ms)

void SysTick_Delay_Us( __IO uint32_t us);
void SysTick_Delay_Ms( __IO uint32_t ms);
#endif

#endif /* __SYSTICK_H */
