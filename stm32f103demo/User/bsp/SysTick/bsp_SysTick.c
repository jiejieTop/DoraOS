/* 头文件 */
#include "include.h"

static __IO u32 TimingDelay;
 
/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
#if USE_TICK_DELAY
	if (SysTick_Config(SystemCoreClock / 100000))	// ST3.5.0库版本
	{ 
		/* Capture error */
		while (1);
	}
#endif
}

/**
  * @brief   us延时程序,1us为一个单位
  * @param  
  *		@arg nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
  * @retval  无
  */
#if USE_TICK_DELAY
void Delay_us(__IO uint32_t us)
{ 
	TimingDelay = us;	

	// 使能滴答定时器  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}
#endif



/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

#if 0
// 这个 固件库函数 在 core_cm3.h中
static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{ 
  // reload 寄存器为24bit，最大值为2^24
	if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);
  
  // 配置 reload 寄存器的初始值	
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;
	
	// 配置中断优先级为 1<<4-1 = 15，优先级为最低
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
	
	// 配置 counter 计数器的值
  SysTick->VAL   = 0;
	
	// 配置systick 的时钟为 72M
	// 使能中断
	// 使能systick
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    
  return (0); 
}
#endif

// couter 减1的时间 等于 1/systick_clk
// 当counter 从 reload 的值减小到0的时候，为一个循环，如果开启了中断则执行中断服务程序，
// 同时 CTRL 的 countflag 位会置1
// 这一个循环的时间为 reload * (1/systick_clk)
#if USE_TICK_DELAY
void SysTick_Delay_Us( __IO uint32_t us)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock/1000000);
	
	for(i=0;i<us;i++)
	{
		// 当计数器的值减小到0的时候，CRTL寄存器的位16会置1	
		while( !((SysTick->CTRL)&(1<<16)) );
	}
	// 关闭SysTick定时器
	SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Delay_Ms( __IO uint32_t ms)
{
	uint32_t i;	
	SysTick_Config(SystemCoreClock/1000);
	
	for(i=0;i<ms;i++)
	{
		// 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
		// 当置1时，读取该位会清0
		while( !((SysTick->CTRL)&(1<<16)) );
	}
	// 关闭SysTick定时器
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}
#endif


/**
  ******************************************************************
  * @brief   SysTick_Handler中断服务函数
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
//void SysTick_Handler(void)
//{
//#if USE_TICK_DELAY
//	TimingDelay_Decrement();	
//#endif
//}


/*********************************************END OF FILE**********************/
