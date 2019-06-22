#ifndef __USART_H
#define	__USART_H


#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include <stdio.h>

/** 
  * 串口宏定义，不同的串口挂载的总线和IO不一样，移植时需要修改这几个宏
	* 1-修改总线时钟的宏，uart1挂载到apb2总线，其他uart挂载到apb1总线
	* 2-修改GPIO的宏
  */
 
/* 使用串口DMA接收 */
#define  USE_USART_DMA_RX  	0

/* 使用串口DMA发送 */
#define  USE_USART_DMA_TX 	0

#if USE_USART_DMA_RX  
// 串口对应的DMA请求通道
#define  USART_RX_DMA_CHANNEL     DMA1_Channel5
// 外设寄存器地址
#define  USART_DR_ADDRESS        (&DEBUG_USARTx->DR)
// 一次接收的数据量
#define  USART_RX_BUFF_SIZE            255 
/* 声明接收buff数组 */
extern uint8_t Usart_Rx_Buf[USART_RX_BUFF_SIZE];
#else
// 一次接收的数据量
#define  USART_RX_BUFF_SIZE            255 
/* 声明接收buff数组 */
extern uint8_t Usart_Rx_Buf[USART_RX_BUFF_SIZE];
#endif

#if USE_USART_DMA_TX
// 串口对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel4
// 外设寄存器地址
#define  USART_DR_ADDRESS        (&DEBUG_USARTx->DR)
// 一次发送的数据量
#define  USART_TX_BUFF_SIZE            1000 
/* 声明发送buff数组 */
extern uint8_t Usart_Tx_Buf[USART_TX_BUFF_SIZE];
#endif
	
	
	
// 串口1-USART1
#define  DEBUG_USARTx                   USART1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  DEBUG_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10

#define  DEBUG_USART_IRQ                USART1_IRQn
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler


// 串口2-USART2
//#define  DEBUG_USARTx                   USART2
//#define  DEBUG_USART_CLK                RCC_APB1Periph_USART2
//#define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
//#define  DEBUG_USART_BAUDRATE           115200

//// USART GPIO 引脚宏定义
//#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
//#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
//    
//#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
//#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_2
//#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
//#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_3

//#define  DEBUG_USART_IRQ                USART2_IRQn
//#define  DEBUG_USART_IRQHandler         USART2_IRQHandler

// 串口3-USART3
//#define  DEBUG_USARTx                   USART3
//#define  DEBUG_USART_CLK                RCC_APB1Periph_USART3
//#define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
//#define  DEBUG_USART_BAUDRATE           115200

//// USART GPIO 引脚宏定义
//#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOB)
//#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
//    
//#define  DEBUG_USART_TX_GPIO_PORT       GPIOB   
//#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_10
//#define  DEBUG_USART_RX_GPIO_PORT       GPIOB
//#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_11

//#define  DEBUG_USART_IRQ                USART3_IRQn
//#define  DEBUG_USART_IRQHandler         USART3_IRQHandler

// 串口4-UART4
//#define  DEBUG_USARTx                   UART4
//#define  DEBUG_USART_CLK                RCC_APB1Periph_UART4
//#define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
//#define  DEBUG_USART_BAUDRATE           115200

//// USART GPIO 引脚宏定义
//#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOC)
//#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
//    
//#define  DEBUG_USART_TX_GPIO_PORT       GPIOC   
//#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_10
//#define  DEBUG_USART_RX_GPIO_PORT       GPIOC
//#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_11

//#define  DEBUG_USART_IRQ                UART4_IRQn
//#define  DEBUG_USART_IRQHandler         UART4_IRQHandler


// 串口5-UART5
//#define  DEBUG_USARTx                   UART5
//#define  DEBUG_USART_CLK                RCC_APB1Periph_UART5
//#define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
//#define  DEBUG_USART_BAUDRATE           115200

//// USART GPIO 引脚宏定义
//#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)
//#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
//    
//#define  DEBUG_USART_TX_GPIO_PORT       GPIOC   
//#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_12
//#define  DEBUG_USART_RX_GPIO_PORT       GPIOD
//#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_2

//#define  DEBUG_USART_IRQ                UART5_IRQn
//#define  DEBUG_USART_IRQHandler         UART5_IRQHandler



/* 串口初始化，根据宏定义决定是否配置dma */
void USART_Config(void);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

#endif /* __USART_H */
