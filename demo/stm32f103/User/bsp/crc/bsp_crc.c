/**
  ******************************************************************************
  * @file    bsp_ds18b20.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   CRC计算相关
  ******************************************************************************
  */
#include "include.h"
//#include "./crc/bsp_crc.h"

__IO uint32_t CRCValue = 0;		 // 用于存放产生的CRC校验值

/*
 * 函数名：CRC_Config
 * 描述  ：使能CRC时钟
 * 输入  ：无
 * 输出  ：无
 * 调用  : 外部调用
 */
void CRC_Config(void)
{
	/* Enable CRC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}


