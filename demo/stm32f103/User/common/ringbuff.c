/*
	***********************************************************
  * @brief   ringbuff.c
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    此文件用于处理环形缓冲区数据
  ***********************************************************
	*/

#include "include.h"
	
#define kmalloc(size, mask) 		malloc(size)
#define kfree(ptr) 							free(ptr)
#define BUG_ON(cond) 						ASSERT(!(cond))


#define min(a,b)  ((a)>(b) ? (b) : (a)) 

/* 不同编译器选择 */
#if defined(__GNUC__) || defined(__x86_64__)
#define TPOOL_COMPILER_BARRIER() __asm__ __volatile("" : : : "memory")

static inline void FullMemoryBarrier()
{
    __asm__ __volatile__("mfence": : : "memory");
}
#define smp_mb() FullMemoryBarrier()
#define smp_rmb() TPOOL_COMPILER_BARRIER()
#define smp_wmb() TPOOL_COMPILER_BARRIER()	
#endif

/*********************** 内部调用函数 ******************************/

/*********************************** mutex **************************************************/
#if USE_MUTEX
/************************************************************
  * @brief   create_mutex
  * @param   mutex:创建信号量句柄
  * @return  创建成功为1，0为不成功。
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    创建一个互斥量,用户在os中互斥使用ringbuff，
  *          支持的os有rtt、win32、ucos、FreeRTOS、LiteOS
  ***********************************************************/
static err_t create_mutex(MUTEX_T *mutex)
{
  err_t ret = 0;

//	*mutex = rt_mutex_create("test_mux",RT_IPC_FLAG_PRIO); /* rtt */
//	ret = (err_t)(*mutex != RT_NULL);
	
//	*mutex = CreateMutex(NULL, FALSE, NULL);		/* Win32 */
//	ret = (err_t)(*mutex != INVALID_HANDLE_VALUE);

//	*mutex = OSMutexCreate(0, &err);		/* uC/OS-II */
//	ret = (err_t)(err == OS_NO_ERR);

//	*mutex = xSemaphoreCreateMutex();	/* FreeRTOS */
//	ret = (err_t)(*mutex != NULL);

//  ret = LOS_MuxCreate(&mutex);  /* LiteOS */
//	ret = (err_t)(ret != LOS_OK);
  return ret;
}
/************************************************************
  * @brief   deleta_mutex
  * @param   mutex:互斥量句柄
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    删除一个互斥量，支持的os有rtt、win32、ucos、FreeRTOS、LiteOS
  ***********************************************************/
static err_t deleta_mutex(MUTEX_T *mutex)
{
	err_t ret;
	
//	ret = rt_mutex_delete(mutex);	/* rtt */
	
//	ret = CloseHandle(mutex);	/* Win32 */

//	OSMutexDel(mutex, OS_DEL_ALWAYS, &err);	/* uC/OS-II */
//	ret = (err_t)(err == OS_NO_ERR);

//  vSemaphoreDelete(mutex);		/* FreeRTOS */
//	ret = 1;

//  ret = LOS_MuxDelete(&mutex);  /* LiteOS */
//	ret = (err_t)(ret != LOS_OK);

	return ret;
}
/************************************************************
  * @brief   request_mutex
  * @param   mutex:互斥量句柄
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    请求一个互斥量，得到互斥量的线程才允许进行访问缓冲区
  *          支持的os有rtt、win32、ucos、FreeRTOS、LiteOS
  ***********************************************************/
static err_t request_mutex(MUTEX_T *mutex)
{
	err_t ret;

//	ret = (err_t)(rt_mutex_take(mutex, MUTEX_TIMEOUT) == RT_EOK);/* rtt */
	
//	ret = (err_t)(WaitForSingleObject(mutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0);	/* Win32 */

//	OSMutexPend(mutex, MUTEX_TIMEOUT, &err));		/* uC/OS-II */
//	ret = (err_t)(err == OS_NO_ERR);

//	ret = (err_t)(xSemaphoreTake(mutex, MUTEX_TIMEOUT) == pdTRUE);	/* FreeRTOS */

//  ret = (err_t)(LOS_MuxPend(mutex,MUTEX_TIMEOUT) == LOS_OK);  		/* LiteOS */

	return ret;
}
/************************************************************
  * @brief   release_mutex
  * @param   mutex:互斥量句柄
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    释放互斥量，当线程使用完资源必须释放互斥量
  *          支持的os有rtt、win32、ucos、FreeRTOS、LiteOS
  ***********************************************************/
static void release_mutex(MUTEX_T *mutex)
{
//	rt_mutex_release(mutex);/* rtt */
	
//	ReleaseMutex(mutex);		/* Win32 */

//	OSMutexPost(mutex);		/* uC/OS-II */

//	xSemaphoreGive(mutex);	/* FreeRTOS */

//  LOS_MuxPost(mutex);   /* LiteOS */
}
#endif
/*********************************** mutex **************************************************/

/*********************************** data **************************************************/
static int32_t fls(int32_t x)
{
  int r = 32;

  if (!x)
          return 0;
  if (!(x & 0xffff0000u)) {
          x <<= 16;
          r -= 16;
  }
  if (!(x & 0xff000000u)) {
          x <<= 8;
          r -= 8;
  }
  if (!(x & 0xf0000000u)) {
          x <<= 4;
          r -= 4;
  }
  if (!(x & 0xc0000000u)) {
          x <<= 2;
          r -= 2;
  }
  if (!(x & 0x80000000u)) {
          x <<= 1;
          r -= 1;
  }
  return r;
}

#ifdef __GNUC__
#define __attribute_const__     __attribute__((__const__))
#endif

/************************************************************
  * @brief   roundup_pow_of_two
  * @param   size：传递进来的数据长度
  * @return  size：返回处理之后的数据长度
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    用于处理数据，使数据长度必须为 2^n
	*					 如果不是，则转换，丢弃多余部分，如
	*					 roundup_pow_of_two(66) -> 返回 64
  ***********************************************************/
static unsigned long roundup_pow_of_two(unsigned long x)
{
	return (1 << (fls(x-1)-1));				//向下对齐
  //return (1UL << fls(x - 1));			//向上对齐，用动态内存可用使用
}

/*********************** 内部调用函数 ******************************/

/************************************************************
  * @brief   Create_RingBuff
  * @param   rb：环形缓冲区句柄
  *          buffer：环形缓冲区的数据区域
  *          size：环形缓冲区的大小，缓冲区大小要为2^n
  * @return  err_t：ERR_OK表示创建成功，其他表示失败
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    用于创建一个环形缓冲区
  ***********************************************************/
err_t Create_RingBuff(RingBuff_t* rb, 
                      uint8_t *buffer,
                      uint32_t size
								)
{
	if((rb == NULL)||(buffer == NULL)||(size == 0))
	{
		PRINT_ERR("data is null!");
		return ERR_NULL;
	}
	
	PRINT_DEBUG("ringbuff size is %d!",size);
	/* 缓冲区大小必须为2^n字节,系统会强制转换,
		 否则可能会导致指针访问非法地址。
		 空间大小越大,强转时丢失内存越多 */
	if(size&(size - 1))
	{
		size = roundup_pow_of_two(size);
		PRINT_DEBUG("change ringbuff size is %d!",size);
	}

	rb->buffer = buffer;
	rb->size = size;
	rb->in = rb->out = 0;
#if USE_MUTEX	
  /* 创建信号量不成功 */
  if(!create_mutex(rb->mutex))
  {
    PRINT_ERR("create mutex fail!");
    ASSERT(ASSERT_ERR);
    return ERR_NOK;
  }
#endif
	PRINT_DEBUG("create ringBuff ok!");
	return ERR_OK;
}

/************************************************************
  * @brief   Delete_RingBuff
  * @param   rb：环形缓冲区句柄
  * @return  err_t：ERR_OK表示成功，其他表示失败
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    删除一个环形缓冲区
  ***********************************************************/
err_t Delete_RingBuff(RingBuff_t *rb)
{
	if(rb == NULL)
	{
		PRINT_ERR("ringbuff is null!");
		return ERR_NULL;
	}
	
	rb->buffer = NULL;
	rb->size = 0;
	rb->in = rb->out = 0;
#if USE_MUTEX	
  if(!deleta_mutex(rb->mutex))
  {
    PRINT_DEBUG("deleta mutex is fail!");
    return ERR_NOK;
  }
#endif
	return ERR_OK;
}

/************************************************************
  * @brief   Write_RingBuff
  * @param   rb:环形缓冲区句柄
  * @param   wbuff:写入的数据起始地址
  * @param   len:写入数据的长度(字节)
  * @return  len:实际写入数据的长度(字节)
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    这个函数会从buff空间拷贝len字节长度的数据到
             rb环形缓冲区中的空闲空间。
  ***********************************************************/
uint32_t Write_RingBuff(RingBuff_t *rb,
                        uint8_t *wbuff, 
                        uint32_t len)
{
  uint32_t l;
#if USE_MUTEX
  /* 请求互斥量，成功才能进行ringbuff的访问 */
  if(!request_mutex(rb->mutex))
  {
    PRINT_DEBUG("request mutex fail!");
    return 0;
  }
  else  /* 获取互斥量成功 */
  {
#endif
    len = min(len, rb->size - rb->in + rb->out);

    /* 第一部分的拷贝:从环形缓冲区写入数据直至缓冲区最后一个地址 */
    l = min(len, rb->size - (rb->in & (rb->size - 1)));
    memcpy(rb->buffer + (rb->in & (rb->size - 1)), wbuff, l);

    /* 如果溢出则在缓冲区头写入剩余的部分
       如果没溢出这句代码相当于无效 */
    memcpy(rb->buffer, wbuff + l, len - l);

    rb->in += len;
    
    PRINT_DEBUG("write ringBuff len is %d!",len);
#if USE_MUTEX
  }
  /* 释放互斥量 */
  release_mutex(rb->mutex);
#endif
  return len;
}

/************************************************************
  * @brief   Read_RingBuff
  * @param   rb:环形缓冲区句柄
  * @param   wbuff:读取数据保存的起始地址
  * @param   len:想要读取数据的长度(字节)
  * @return  len:实际读取数据的长度(字节)
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    这个函数会从rb环形缓冲区中的数据区域拷贝len字节
             长度的数据到rbuff空间。
  ***********************************************************/
uint32_t Read_RingBuff(RingBuff_t *rb,
                       uint8_t *rbuff, 
                       uint32_t len)
{
  uint32_t l;
#if USE_MUTEX
  /* 请求互斥量，成功才能进行ringbuff的访问 */
  if(!request_mutex(rb->mutex))
  {
    PRINT_DEBUG("request mutex fail!");
    return 0;
  }
  else
  {
#endif
    len = min(len, rb->in - rb->out);

    /* 第一部分的拷贝:从环形缓冲区读取数据直至缓冲区最后一个 */
    l = min(len, rb->size - (rb->out & (rb->size - 1)));
    memcpy(rbuff, rb->buffer + (rb->out & (rb->size - 1)), l);

    /* 如果溢出则在缓冲区头读取剩余的部分
       如果没溢出这句代码相当于无效 */
    memcpy(rbuff + l, rb->buffer, len - l);

    rb->out += len;
    
    PRINT_DEBUG("read ringBuff len is %d!",len);
#if USE_MUTEX
  }
  /* 释放互斥量 */
  release_mutex(rb->mutex);
#endif
  return len;
}

/************************************************************
  * @brief   CanRead_RingBuff
	* @param   rb:环形缓冲区句柄
	* @return  uint32:可读数据长度 0 / len
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    可读数据长度
  ***********************************************************/
uint32_t CanRead_RingBuff(RingBuff_t *rb)
{
	if(NULL == rb)
	{
		PRINT_ERR("ringbuff is null!");
		return 0;
	}
	if(rb->in == rb->out)
		return 0;
	
	if(rb->in > rb->out)
		return (rb->in - rb->out);
	
	return (rb->size - (rb->out - rb->in));
}

/************************************************************
  * @brief   CanRead_RingBuff
	* @param   rb:环形缓冲区句柄
	* @return  uint32:可写数据长度 0 / len
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    可写数据长度
  ***********************************************************/
uint32_t CanWrite_RingBuff(RingBuff_t *rb)
{
	if(NULL == rb)
	{
		PRINT_ERR("ringbuff is null!");
		return 0;
	}

	return (rb->size - CanRead_RingBuff(rb));
}


/******************************** DEMO *****************************************************/
//	RingBuff_t ringbuff_handle;
//	
//	uint8_t rb[64];
//	uint8_t res[64];
//	Create_RingBuff(&ringbuff_handle, 
//								rb,
//								sizeof(rb));
//			Write_RingBuff(&ringbuff_handle,
//                     res, 
//                     datapack.data_length);
//			
//			PRINT_DEBUG("CanRead_RingBuff = %d!",CanRead_RingBuff(&ringbuff_handle));
//			PRINT_DEBUG("CanWrite_RingBuff = %d!",CanWrite_RingBuff(&ringbuff_handle));
//			
//			Read_RingBuff(&ringbuff_handle,
//                     res, 
//                     datapack.data_length);
/******************************** DEMO *****************************************************/


