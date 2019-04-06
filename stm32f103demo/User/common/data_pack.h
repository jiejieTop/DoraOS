#ifndef __DATA_PACK_H
#define __DATA_PACK_H

#include "include.h"


/**
  ******************************************************************
													   宏定义
  ******************************************************************
  */ 

#define USE_SYSTEM_ID   0
#define USE_SYSTEM_CMD  0
#define USE_DATA_CRC    0

/* 数据帧头 */
#define DATA_HEAD   0x02

/* 数据帧尾 */
#define DATA_TAIL   0x03





/**
  ******************************************************************
													   变量声明
  ******************************************************************
  */ 
typedef struct datapack
{
	uint8_t data_head; //数据头
	uint8_t data_tail; //数据尾
//	uint8_t data_type; 			//数据类型
	uint16_t data_length; // 数据长度
	uint32_t data_crc; // 数据校验
}DataPack_t;




/**
  ******************************************************************
														函数声明
  ******************************************************************
  */ 
int32_t Send_DataPack(void *buff,
                      uint8_t data_len
#if USE_SYSTEM_ID
                      ,uint8_t sys_id
#endif
#if USE_SYSTEM_CMD
                      ,uint8_t sys_cmd
#endif
);

err_t DataPack_Process(uint8_t* buff,DataPack_t* datapack);

#if USE_USART_DMA_RX
void Receive_DataPack(void);
#else
void Receive_DataPack(void);
#endif

#if USE_USART_DMA_TX
void DMA_Send_Data(uint16_t len);
#else
err_t Usart_Send_Data(uint8_t *buf, uint16_t len);
#endif




#endif /* __DATA_PACK_H */
