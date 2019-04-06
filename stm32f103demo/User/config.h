#ifndef __CONFIG_H
#define __CONFIG_H

/************************************************************
  * @brief   config.h
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    这个文件用于配置全局的宏定义 使能/失能
  *          一般不在此文件修改宏定义
  *          此文件保证系统的正常运作
  ***********************************************************/
  
#include "include.h"

/* 使用串口 DMA 发送 */
#ifndef USE_USART_DMA_TX
#define  USE_USART_DMA_TX  1
#endif

#ifndef USART_TX_BUFF_SIZE
// 一次发送的数据量
#define  USART_TX_BUFF_SIZE            1000 
/* 声明发送buff数组 */
extern uint8_t Usart_Tx_Buf[USART_TX_BUFF_SIZE];
#endif

/* 使用串口 DMA 接收 */
#ifndef USE_USART_DMA_RX
#define  USE_USART_DMA_RX  1
#endif

// 一次接收的数据传递 （字节）
#ifndef USART_RX_BUFF_SIZE
#define  USART_RX_BUFF_SIZE            1000 
/* 声明接收buff数组 */
extern uint8_t Usart_Rx_Buf[USART_RX_BUFF_SIZE];
#endif

/* 使用CRCk */
#ifndef USE_DATA_CRC
#define USE_DATA_CRC  0
#endif

/* 使能打印调试日志 */
#ifndef PRINT_DEBUG_ENABLE
#define PRINT_DEBUG_ENABLE	0		/* 打印调试信息 */
#endif
#ifndef PRINT_ERR_ENABLE
#define PRINT_ERR_ENABLE		0		/* 打印错误信息 */
#endif
#ifndef PRINT_INFO_ENABLE
#define PRINT_INFO_ENABLE		1		/* 打印个人信息 */
#endif

/* 关于系统延时 推荐使用内核精确延时	*/

#ifndef USE_DWT_DELAY
#define USE_DWT_DELAY		1		/* 使用dwt内核精确延时 */
#endif

#if USE_DWT_DELAY
#define USE_TICK_DELAY		0		/* 不使用SysTick延时 */
#else
#define USE_TICK_DELAY		1		/* 使用SysTick延时 */
#endif


#endif /* __CONFIG_H */

/********************************END OF FILE***************************************/