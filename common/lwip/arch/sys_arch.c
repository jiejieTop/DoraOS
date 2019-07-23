/*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Simon Goldschmidt
 *
 */
#include "debug.h"

#include <lwip/opt.h>
#include <lwip/arch.h>

#include "tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/sio.h"
#include "ethernetif.h"

#if !NO_SYS
#include "sys_arch.h"
#endif
#include <lwip/stats.h>
#include <lwip/debug.h>
#include <lwip/sys.h>
#include "lwip/dhcp.h"
#include <string.h>

int errno;


u32_t lwip_sys_now;

struct sys_timeouts {
  struct sys_timeo *next;
};

struct timeoutlist
{
	struct sys_timeouts timeouts;
	DOS_TaskCB_t pid;
};

#define SYS_THREAD_MAX 4

static struct timeoutlist s_timeoutlist[SYS_THREAD_MAX];

static u16_t s_nextthread = 0;

u32_t
sys_jiffies(void)
{
  lwip_sys_now = Dos_Get_Tick();
  return lwip_sys_now;
}

u32_t
sys_now(void)
{
  lwip_sys_now = Dos_Get_Tick();
  return lwip_sys_now;
}

void
sys_init(void)
{
	int i;
	// Initialize the the per-thread sys_timeouts structures
	// make sure there are no valid pids in the list
	for(i = 0; i < SYS_THREAD_MAX; i++)
	{
		s_timeoutlist[i].pid = 0;
		s_timeoutlist[i].timeouts.next = NULL;
	}
	// keep track of how many threads have been created
	s_nextthread = 0;
}

struct sys_timeouts *sys_arch_timeouts(void)
{
	int i;
	DOS_TaskCB_t pid;
	struct timeoutlist *tl;
	pid = Dos_Get_CurrentTCB();
	for(i = 0; i < s_nextthread; i++)
	{
		tl = &(s_timeoutlist[i]);
		if(tl->pid == pid)
		{
			return &(tl->timeouts);
		}
	}
	return NULL;
}

sys_prot_t sys_arch_protect(void)
{
    return Dos_Interrupt_Disable();
}

void sys_arch_unprotect(sys_prot_t pval)
{
	Dos_Interrupt_Enable(pval);
}

#if !NO_SYS


err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
  /* 创建 sem */
  if(count <= 1)
  {    
    *sem = Dos_BinarySem_Create(count);
    if(count == 1)
    {
      Dos_SemPost(*sem);
    }
  }
  else
    *sem = Dos_SemCreate(count,count);
  
#if SYS_STATS
	++lwip_stats.sys.sem.used;
 	if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used) {
		lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
	}
#endif /* SYS_STATS */
  
  if(*sem != SYS_SEM_NULL)
    return ERR_OK;
  else
  {
#if SYS_STATS
    ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
    printf("[sys_arch]:new sem fail!\n");
    return ERR_MEM;
  }
}

void
sys_sem_free(sys_sem_t *sem)
{
#if SYS_STATS
   --lwip_stats.sys.sem.used;
#endif /* SYS_STATS */
  /* 删除 sem */
  Dos_SemDelete(*sem);
  *sem = SYS_SEM_NULL;
}


int sys_sem_valid(sys_sem_t *sem)                                               
{
  return (*sem != SYS_SEM_NULL);                                    
}


void
sys_sem_set_invalid(sys_sem_t *sem)
{
  *sem = SYS_SEM_NULL;
}

/* 
 如果timeout参数不为零，则返回值为
 等待信号量所花费的毫秒数。如果
 信号量未在指定时间内发出信号，返回值为
 SYS_ARCH_TIMEOUT。如果线程不必等待信号量
 该函数返回零。 */
u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u32_t wait_tick = 0;
  u32_t start_tick = 0 ;
  
  //看看信号量是否有效
  if(*sem == SYS_SEM_NULL)
    return SYS_ARCH_TIMEOUT;
  
  //首先获取开始等待信号量的时钟节拍
  start_tick = Dos_Get_Tick();
  
  //timeout != 0，需要将ms换成系统的时钟节拍
  if(timeout != 0)
  {
    //将ms转换成时钟节拍
    wait_tick = timeout / DOS_TICK_PERIOD_MS;
    if (wait_tick == 0)
      wait_tick = 1;
  }
  else
    wait_tick = DOS_WAIT_FOREVER;  //一直阻塞
  
  //等待成功，计算等待的时间，否则就表示等待超时
  if(Dos_SemWait(*sem, wait_tick) == DOS_OK)
    return ((Dos_Get_Tick()-start_tick) * DOS_TICK_PERIOD_MS);
  else
    return SYS_ARCH_TIMEOUT;
}

void
sys_sem_signal(sys_sem_t *sem)
{
  if(Dos_SemPost( *sem ) != DOS_OK)
    DOS_LOG_WARN("[sys_arch]:sem signal fail!\n");
}

err_t
sys_mutex_new(sys_mutex_t *mutex)
{
  /* 创建 sem */   
  *mutex = Dos_MutexCreate();
  if(*mutex != SYS_MRTEX_NULL)
    return ERR_OK;
  else
  {
    printf("[sys_arch]:new mutex fail!\n");
    return ERR_MEM;
  }
}

void
sys_mutex_free(sys_mutex_t *mutex)
{
  Dos_MutexDelete(*mutex);
}

void
sys_mutex_set_invalid(sys_mutex_t *mutex)
{
  *mutex = SYS_MRTEX_NULL;
}

void
sys_mutex_lock(sys_mutex_t *mutex)
{
  Dos_MutexPend(*mutex,/* 互斥量句柄 */
                 DOS_WAIT_FOREVER); /* 等待时间 */
}

void
sys_mutex_unlock(sys_mutex_t *mutex)
{
  Dos_MutexPost( *mutex );//给出互斥量
}


sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
  sys_thread_t handle = NULL;
  /* 创建MidPriority_Task任务 */
  handle = Dos_TaskCreate(name, function, arg, stacksize, prio, 40);
  if(handle == DOS_NULL)
  {
    DOS_LOG_WARN("[sys_arch]:create task fail!\n");
    return NULL;
  }
  return handle;
}

err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
    /* 创建Test_Queue */
    *mbox = Dos_QueueCreate( size, sizeof(void *));
    if(NULL == *mbox)
        return ERR_MEM;
    return ERR_OK;
}

void
sys_mbox_free(sys_mbox_t *mbox)
{
    Dos_QueueDelete(*mbox);
}

int sys_mbox_valid(sys_mbox_t *mbox)          
{      
  if (*mbox == SYS_MBOX_NULL) 
    return 0;
  else
    return 1;
}   

void
sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  *mbox = SYS_MBOX_NULL; 
}

void
sys_mbox_post(sys_mbox_t *q, void *msg)
{
  while(Dos_QueueWrite( *q, /* 消息队列的句柄 */
                    &msg,/* 发送的消息内容 */
                    sizeof(void *),
                    DOS_WAIT_FOREVER) != DOS_OK); /* 等待时间 */
}

err_t
sys_mbox_trypost(sys_mbox_t *q, void *msg)
{
  if(Dos_QueueWrite(*q,&msg,sizeof(void *),0) == DOS_OK)  
    return ERR_OK;
  else
    return ERR_MEM;
}

err_t
sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
  return sys_mbox_trypost(q, msg);
}

u32_t
sys_arch_mbox_fetch(sys_mbox_t *q, void **msg, u32_t timeout)
{
  void *dummyptr;
  u32_t wait_tick = 0;
  u32_t start_tick = 0 ;
  
  if ( msg == NULL )  //看看存储消息的地方是否有效
		msg = &dummyptr;
  
  //首先获取开始等待信号量的时钟节拍
  start_tick = sys_now();
  
  //timeout != 0，需要将ms换成系统的时钟节拍
  if(timeout != 0)
  {
    //将ms转换成时钟节拍
    wait_tick = timeout / DOS_TICK_PERIOD_MS;
    if (wait_tick == 0)
      wait_tick = 1;
  }
  //一直阻塞
  else
    wait_tick = DOS_WAIT_FOREVER;
  
  //等待成功，计算等待的时间，否则就表示等待超时
  if(Dos_QueueRead(*q,&(*msg),sizeof(void *), wait_tick) == DOS_OK)
    return ((sys_now() - start_tick)*DOS_TICK_PERIOD_MS);
  else
  {
    *msg = NULL;
    return SYS_ARCH_TIMEOUT;
  }
}

u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *q, void **msg)
{
	void *dummyptr;
	if ( msg == NULL )
		msg = &dummyptr;
  
  //等待成功，计算等待的时间
  if(Dos_QueueRead(*q,&(*msg),sizeof(void *), 0) == DOS_OK)
    return ERR_OK;
  else
    return SYS_MBOX_EMPTY;
}

#if LWIP_NETCONN_SEM_PER_THREAD
#error LWIP_NETCONN_SEM_PER_THREAD==1 not supported
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

#endif /* !NO_SYS */

/* Variables Initialization */
struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];

void TCPIP_Init(void)
{
  tcpip_init(NULL, NULL);
  
  /* IP addresses initialization */
  /* USER CODE BEGIN 0 */
#if LWIP_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP4_ADDR(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP4_ADDR(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP4_ADDR(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* USE_DHCP */
  /* USER CODE END 0 */
  /* Initilialize the LwIP stack without RTOS */
  /* add the network interface (IPv4/IPv6) without RTOS */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /* Registers the default network interface */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }
  
#if LWIP_DHCP	   			//若使用了DHCP
  int err;
  /*  Creates a new DHCP client for this interface on the first call.
  Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
  the predefined regular intervals after starting the client.
  You can peek in the netif->dhcp struct for the actual DHCP status.*/
  
  printf("本例程将使用DHCP动态分配IP地址,如果不需要则在lwipopts.h中将LWIP_DHCP定义为0\n\n");
  
  err = dhcp_start(&gnetif);      //开启dhcp
  if(err == ERR_OK)
    printf("lwip dhcp init success...\n\n");
  else
    printf("lwip dhcp init fail...\n\n");
  while(ip_addr_cmp(&(gnetif.ip_addr),&ipaddr))   //等待dhcp分配的ip有效
  {
    vTaskDelay(1);
  } 
#endif
  printf("本地IP地址是:%d.%d.%d.%d\n\n",  \
        ((gnetif.ip_addr.addr)&0x000000ff),       \
        (((gnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((gnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((gnetif.ip_addr.addr)&0xff000000)>>24);
}









