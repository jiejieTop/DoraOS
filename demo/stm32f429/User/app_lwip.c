/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
#include <task.h>
#include <log.h>
#include <memp.h>
#include "iperf.h"
//#include "client.h"
/** Variable declaration */
DOS_TaskCB_t task1 = DOS_NULL;
DOS_TaskCB_t task2 = DOS_NULL;

/** Macro definition */


/** Function declaration */
extern void BSP_Init(void);
extern void TCPIP_Init(void);

void test_task1(void *Parameter)
{
//    dos_uint32 pri;
    
    TCPIP_Init();

    iperf_server_init();
//    client_init();

    printf("本例程演示开发板发送数据到服务器\n\n");

    printf("网络连接模型如下：\n\t 电脑<--网线-->路由<--网线-->开发板\n\n");

    printf("实验中使用TCP协议传输数据，电脑作为TCP Server，开发板作为TCP Client\n\n");

    printf("本例程的IP地址均在User/arch/sys_arch.h文件中修改\n\n");

    printf("本例程参考<<LwIP应用实战开发指南>>第15章 使用 NETCONN 接口编程\n\n");
    
    while(1)
    {
//        pri = Dos_Interrupt_Disable();
////        printf("task1\n");
//        Dos_Interrupt_Enable(pri);
        Dos_TaskSleep(100000);
        
    }
}


void test_task2(void *Parameter)
{
    dos_uint32 pri;
    while(1)
    {
        pri = Dos_Interrupt_Disable();
        printf("mem:%d\n",Dos_MemInfoGet());
        Dos_Interrupt_Enable(pri);
        Dos_TaskSleep(1000);
        
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    Dos_SystemInit();
    
   
    task1 = Dos_TaskCreate( "test_task1",
                            &test_task1,
                            DOS_NULL,
                            2048,
                            6,
                            20);

    task2 = Dos_TaskCreate( "test_task2",
                            &test_task2,
                            DOS_NULL,
                            2048,
                            7,
                            0);
                
    Dos_Start();
  
}




/********************************END OF FILE***************************************/

