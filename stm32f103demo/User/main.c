/**
  ******************************************************************************
  * @file    main.c
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   main
  ******************************************************************************
  */ 
#include "include.h"
#include "task.h"


/**
  ******************************************************************
													   变量声明
  ******************************************************************
  */ 


DOS_TaskCB_t task = DOS_NULL;
DOS_TaskCB_t task1 = DOS_NULL;
/**
  ******************************************************************
														函数声明
  ******************************************************************
  */ 
  
static void BSP_Init(void);

void test_task(void *Parameter)
{
  while(1)
  {
    Dos_Interrupt_Disable();
    LED1_TOGGLE;
    Dos_Interrupt_Enable(0);
    Delay_ms(1000);
  }
}
void test1_task(void *Parameter)
{
  while(1)
  {
    Dos_Interrupt_Disable();
    DOS_PRINT_DEBUG("123\n");
    Dos_Interrupt_Enable(0);
    Delay_ms(1000);
  }
}
/**
  ******************************************************************
  * @brief   主函数
  * @author  jiejie
  * @version V1.0
  * @date    2019-xx-xx
  ******************************************************************
  */ 
int main(void)
{
  dos_uint8 *p = DOS_NULL;
  dos_uint8 *p1 = DOS_NULL;
  dos_uint8 *p2 = DOS_NULL;
  dos_uint8 *p3 = DOS_NULL;
  BSP_Init();
  
  Dos_SystemInit();
  
  p = Dos_MemAlloc(16);
  p2 = Dos_MemAlloc(128);
  task = Dos_TaskCreate( "task",
                  &test_task,
                  DOS_NULL,
                  512,
                  1);
  
  task1 = Dos_TaskCreate( "task1",
                &test1_task,
                DOS_NULL,
                512,
                2);
  
  p3 = Dos_MemAlloc(512);
  p1 = Dos_MemAlloc(16);  
  DOS_PRINT_DEBUG(" p = %x",(dos_uint32)p);
  DOS_PRINT_DEBUG(" p = %x",(dos_uint32)p2);
  DOS_PRINT_DEBUG(" p = %x",(dos_uint32)p3);
  DOS_PRINT_DEBUG(" p = %x",(dos_uint32)p1);
  
  Dos_Start();
  
  while(1)                            
  {
        

  }
}



/**
  ******************************************************************
  * @brief   BSP_Init，用于所有板级初始化
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
static void BSP_Init(void)
{
	/* LED 初始化 */
	LED_GPIO_Config();
	
#if USE_DWT_DELAY
	/* 内核精确定时器初始化 */
	CPU_TS_TmrInit();
#else
	/* 滴答定时器初始化 */
	SysTick_Init();
#endif
	
	/* 串口初始化 */
	USART_Config();
  
	/* 按键初始化 */
  Key_GPIO_Config();
  
//	/* 外部中断初始化 */
//	EXTI_Key_Config(); 
	
	
	/* 打印信息 */
	DOS_PRINT_INFO("welcome to learn jiejie stm32 library!\n");
	
}


/********************************END OF FILE***************************************/




















