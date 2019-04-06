/* 头文件 */

#include "include.h"
/**
  ******************************************************************
  * @brief   数据帧打包格式
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * 
	* --------------------------------------------------------------------
	* | 起始帧 | 数据长度|  设备id  | 设备命令 | 有效数据 | 校验 | 结束帧 |
	* --------------------------------------------------------------------
	* |  0x02  | length  |system id |   cmd    |   buff   | crc  |  0x03  |
	* --------------------------------------------------------------------
	* |  uint8 | uint8   |  uint8   |  uint8   |   buff   |uint32|  uint8 |
	* -------------------------------------------------------------------- 
	* |  1字节 | 1字节   |  1字节   |  1字节   |   buff   |4字节 |  1字节 |
	* --------------------------------------------------------------------
	******************************************************************
  */ 
  
/**                    Usart_Rx_Sta 
	* ----------------------------------------------------------
	* | 1  1  0  0 | 0  0  0  0 | 0  0  0  0 | 0  0  0  0|
	* -----------------------------------------------------------
  * 最高两位用于保存数据是否接收完成   0：未完成 1：完成
  * 次高位用于保存是否收到数据帧头     0：未完成 1：完成
  * 其他位用于保存数据包长度,数据包最大长度为(255)个字节
*/
/* 接收状态标记 */
uint16_t Usart_Rx_Sta = 0;  

  
/**
  ******************************************************************
														内部调用函数声明
  ******************************************************************
  */ 


/**
  ******************************************************************
  * @brief   发送数据，以数据包形式
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * @param   buff:数据起始地址
  * @return  0：发送成功，其他表示失败
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
)
{
#if USE_USART_DMA_TX	
	uint8_t *pTxBuf = Usart_Tx_Buf;
#else
	/* 使用普通串口发送 */
	int32_t res;
	uint8_t Usart_Tx_Buf[data_len+9];
	uint8_t *pTxBuf = Usart_Tx_Buf;
#endif
	
#if USE_DATA_CRC 
	__IO uint32_t CRCValue;
#endif
	/* buff 无效	*/
	if(NULL == buff)
	{
		PRINT_ERR("data Is Null \n");
		ASSERT(ASSERT_ERR);
		return -1;
	}
	/* 数据帧头 */
	*pTxBuf ++= (uint8_t)(DATA_HEAD); 

	/* 数据长度 */ 
	*pTxBuf ++= (uint8_t)(data_len);
  
  /* 设备id */ 
#if USE_SYSTEM_ID
  *pTxBuf ++= (uint8_t)(sys_id);
#endif
  /* 设备命令 */
#if USE_SYSTEM_ID
  *pTxBuf ++= (uint8_t)(sys_id);
#endif

	/* 有效数据 */
	if(0 != data_len)
	{
		memcpy(pTxBuf , buff , data_len);
	}
	/* 数据偏移 */
	pTxBuf = (pTxBuf + data_len);
	
#if USE_DATA_CRC 
	/* 使用CRC校验 */
	CRCValue = CRC_CalcBlockCRC((uint32_t *)buff, data_len);
	/* 校验值 */
	*pTxBuf ++= (uint8_t)(CRCValue >> 24); 
	*pTxBuf ++= (uint8_t)(CRCValue >> 16); 
	*pTxBuf ++= (uint8_t)(CRCValue >> 8);  
	*pTxBuf ++= (uint8_t)(CRCValue); 
#endif
	/* 数据帧尾 */
	*pTxBuf ++= (uint8_t)(DATA_TAIL); 

#if USE_USART_DMA_TX	
	DMA_Send_Data((pTxBuf - Usart_Tx_Buf));
#else
	res = Usart_Send_Data(Usart_Tx_Buf,(pTxBuf - Usart_Tx_Buf));
	if(res != ERR_OK)
	{
		PRINT_ERR("uart write error %d \n", res);
		ASSERT(ASSERT_ERR);
		return res;
	}
#endif
	
	return ERR_OK;
}

#if USE_USART_DMA_TX
/************************************************************
  * @brief   DMA_Send_Data
	* @param   len : 发送的数据长度 （字节）
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    
  ***********************************************************/
void DMA_Send_Data(uint16_t len)
{
 
	DMA_Cmd(USART_TX_DMA_CHANNEL, DISABLE);                     //关闭DMA传输 
//	
////	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//确保DMA可以被设置  
//		
	DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL,len);          //设置数据传输量  
// 
	DMA_Cmd(USART_TX_DMA_CHANNEL, ENABLE); 
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, ENABLE);       //开启DMA传输 
}	  
#else

err_t Usart_Send_Data(uint8_t *buf, uint16_t len)
{
	uint16_t i = 0;
	/* 判断 buff 非空 */
	if((NULL == buf)||(len == 0))
	{
    PRINT_ERR("send data is null!");
		return ERR_NULL;
	}
	/* 循环发送，一个字节 */
	for(i=0; i<len; i++)
	{
		Usart_SendByte(DEBUG_USARTx,buf[i]);
	}
  PRINT_DEBUG("send data length  is %d!",len);
	return ERR_OK;
}  

#endif

/************************************************************
  * @brief   Uart_DMA_Rx_Data
  * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    使用串口 DMA 接收时调用的函数
  ***********************************************************/
#if USE_USART_DMA_RX
void Receive_DataPack(void)
{
	/* 接收的数据长度 */
	uint32_t buff_length;
	
	/* 关闭DMA ，防止干扰 */
	DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);  /* 暂时关闭dma，数据尚未处理 */ 
	
	/* 获取接收到的数据长度 单位为字节*/
	buff_length = USART_RX_BUFF_SIZE - DMA_GetCurrDataCounter(USART_RX_DMA_CHANNEL);
  
  /* 获取数据长度 */
  Usart_Rx_Sta = buff_length;

	PRINT_DEBUG("buff_length = %d\n ",buff_length);
  
	/* 清DMA标志位 */
	DMA_ClearFlag( DMA1_FLAG_TC5 );          
	
	/* 重新赋值计数值，必须大于等于最大可能接收到的数据帧数目 */
	USART_RX_DMA_CHANNEL->CNDTR = USART_RX_BUFF_SIZE;    
  
	/* 此处应该在处理完数据再打开，如在 DataPack_Process() 打开*/
	DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);      
	
	/* 给出信号 ，发送接收到新数据标志，供前台程序查询 */
	
  /* 标记接收完成，在 DataPack_Handle 处理*/
  Usart_Rx_Sta |= 0xC000;
  
	/* 
	DMA 开启，等待数据。注意，如果中断发送数据帧的速率很快，MCU来不及处理此次接收到的数据，
	中断又发来数据的话，这里不能开启，否则数据会被覆盖。有2种方式解决：

  1. 在重新开启接收DMA通道之前，将Rx_Buf缓冲区里面的数据复制到另外一个数组中，
  然后再开启DMA，然后马上处理复制出来的数据。

  2. 建立双缓冲，重新配置DMA_MemoryBaseAddr的缓冲区地址，那么下次接收到的数据就会
  保存到新的缓冲区中，不至于被覆盖。
	*/
}
#else
void Receive_DataPack(void)
{
  uint8_t res;
  /* 读取数据会清除中断标志位 */
  res = USART_ReceiveData(DEBUG_USARTx);

	if((Usart_Rx_Sta&0x8000)==0)//接收未完成
	{
		if(Usart_Rx_Sta&0x4000)//接收到了DATA_HEAD
		{
			if(res!=DATA_TAIL)/* 收到的不是数据帧尾 */
			{
				/* 正常接收数据 */
				Usart_Rx_Buf[Usart_Rx_Sta&0XFF]=res ;
				Usart_Rx_Sta++;
			}
			else
			{
        if((Usart_Rx_Sta & 0XFF)<=DATA_TAIL)
        {
          /* 数据长度小于等于3个字节，
          说明收到的数据不是数据帧尾，
          很可能是数据长度 0x03，
          需要正常接收*/
          /* 正常接收数据 */
          Usart_Rx_Buf[Usart_Rx_Sta&0XFF]=res ;
          Usart_Rx_Sta++;
        }
        else
        {
          /* 把数据帧尾也接收 */
          Usart_Rx_Buf[Usart_Rx_Sta&0XFF]=res ;
          Usart_Rx_Sta++;
          Usart_Rx_Sta|=0x8000;		/* 接收完成了 */ 
          PRINT_DEBUG("receive ok!");
          PRINT_DEBUG("buff_length = %d",Usart_Rx_Sta&0XFF);
        }
			}
		}
		else /* 还没收到DATA_HEAD */
		{	
			if(res==DATA_HEAD)	/* 收到数据帧头 */
			{
				Usart_Rx_Buf[Usart_Rx_Sta&0XFF]=res ;
				Usart_Rx_Sta++;
				Usart_Rx_Sta|=0x4000;/* 标记接收到帧头 */
			}
			else/* 接收错误 */
			{
				Usart_Rx_Sta = 0;
				PRINT_ERR("receive fail!");
			}		 
		}
	} 
}
	
#endif

/************************************************************
  * @brief   DataPack_Process
	* @param   buff:数据保存的起始地址。datapack:数据信息保存的结构体指针
  * @return  返回0代表接收成功。其他代表错误 。
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    数据包处理，解析数据
  ***********************************************************/
err_t DataPack_Process(uint8_t* buff,DataPack_t* datapack)
{
  uint16_t data_len;
  uint16_t check_data_len;
	uint8_t *pbuff = Usart_Rx_Buf;
  
  if((NULL == buff)||(NULL == datapack))
  {
    PRINT_ERR("buff or len is NULL\n");
		ASSERT(ASSERT_ERR);
    return ERR_NULL;
  }
  /* 接收完成 */
  if( Usart_Rx_Sta & 0x8000 )
  {
    /* 获取数据长度 */
    data_len = Usart_Rx_Sta & 0xffff;
    /* 清除接收完成标志位 */
    Usart_Rx_Sta = 0;
    if(data_len < 4)
    {
      PRINT_ERR("datapack is mar!");
      return ERR_UNUSE;
    }
    else
    {
      datapack->data_length = *(pbuff+1)<<8|*(pbuff+2);
    }
#if USE_DATA_CRC
    check_data_len = data_len - 8;
		PRINT_DEBUG("check_data_len = %d",check_data_len);
#else
    check_data_len = data_len - 4;
		PRINT_DEBUG("check_data_len = %d",check_data_len);
#endif
    /* 校验数据包是否完整 */
    if(check_data_len == datapack->data_length)
    {
      memcpy(buff,pbuff+3,datapack->data_length);
			memset(Usart_Rx_Buf,0,data_len);
			PRINT_DEBUG("data_length = %d",datapack->data_length);
			PRINT_DEBUG("data = %s",buff);
			PRINT_DEBUG("data handle ok！");
    }
    else
    {
			PRINT_DEBUG("data_length = %d",datapack->data_length);
			PRINT_ERR("data length is not equal!");
			memset(Usart_Rx_Buf,0,data_len);
      buff = NULL;
      datapack->data_length = 0;
    }
#if USE_USART_DMA_RX
    /* 打开DMA，可以进行下一次接收 */
    DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);
#endif
  return ERR_OK;
  }
  return ERR_NULL;
}

/********************************** 例子 ********************************************/
///* 
//				uint8_t res[50];

//				DataPack_t datapack;
//				int32_t err;

//				err = DataPack_Process(res,&datapack);
//				if(ERR_OK == err)
//				{
//					Send_DataPack(res,datapack.data_length);
//				}
//*/

//					void DEBUG_USART_IRQHandler(void)
//					{
//					#if USE_USART_DMA_RX
//						/* 使用串口DMA */
//						if(USART_GetITStatus(DEBUG_USARTx,USART_IT_IDLE)!=RESET)
//						{		
//							/* 处理数据 */
//							Uart_DMA_Rx_Data();
//							// 清除空闲中断标志位
//							USART_ReceiveData( DEBUG_USARTx );
//						}	
//					#else
//						/* 接收中断 */
//						if(USART_GetITStatus(DEBUG_USARTx,USART_IT_RXNE)!=RESET)
//						{		
//							Receive_DataPack();
//						}
//					#endif
//					}

/********************************** 例子 ********************************************/


/********************************END OF FILE***************************************/

