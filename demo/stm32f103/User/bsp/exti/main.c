/**
  ******************************************************************************
  * @file    main.c
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   main
  ******************************************************************************
  * @attention 此demo是无RTOS版本，需要RTOS的demo请关注杰杰的GitHub：
  *	GitHub：https://github.com/jiejieTop
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "include.h"
/**
  ******************************************************************
														函数声明
  ******************************************************************
  */ 
static void BSP_Init(void);

void Btn1_Dowm_CallBack(void *btn)
{
  PRINT_INFO("1按键单击!");
}

void Btn1_Double_CallBack(void *btn)
{
  PRINT_INFO("1按键双击!");
}

void Btn1_Long_CallBack(void *btn)
{
  PRINT_INFO("1按键长按!");
}

void Btn2_Dowm_CallBack(void *btn)
{
  PRINT_INFO("2按键单击!");
}

void Btn2_Double_CallBack(void *btn)
{
  PRINT_INFO("2按键双击!");
}

void Btn2_Long_CallBack(void *btn)
{
  PRINT_INFO("2按键长按!");
}

/**
  ******************************************************************
													   变量声明
  ******************************************************************
  */ 
	

Button_t test_button;
Button_t test2_button; 
/**
  ******************************************************************
  * @brief   主函数
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
int main(void)
{

	BSP_Init();
  
//  PRINT_DEBUG("当前电平：%d",Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN));
  
  Button_Create("test_button",
            &test_button, 
            Read_KEY1_Level, 
            KEY_ON);
  Button_Attach(&test_button,BUTTON_DOWM,Btn1_Dowm_CallBack);
  Button_Attach(&test_button,BUTTON_ALL_RIGGER,Btn1_Double_CallBack);
  Button_Attach(&test_button,BUTTON_LONG,Btn1_Long_CallBack);
  
  Button_Create("test2_button",
            &test2_button, 
            Read_KEY2_Level, 
            KEY_ON);
  Button_Attach(&test2_button,BUTTON_DOWM,Btn2_Dowm_CallBack);
  Button_Attach(&test2_button,BUTTON_ALL_RIGGER,Btn2_Double_CallBack);
  Button_Attach(&test2_button,BUTTON_LONG,Btn2_Long_CallBack);
  
  
	while(1)                            
	{
    Button_Cycle_Process(&test_button);
    Button_Cycle_Process(&test2_button);
		Delay_ms(20);
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
	
	CRC_Config();
	
	/* 打印信息 */
	PRINT_INFO("welcome to learn jiejie stm32 library!\n");
	
}




/********************************END OF FILE***************************************/
