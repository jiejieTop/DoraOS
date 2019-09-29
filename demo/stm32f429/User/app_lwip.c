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
dos_task_t task1 = DOS_NULL;
dos_task_t task2 = DOS_NULL;

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

    printf("��������ʾ�����巢�����ݵ�������\n\n");

    printf("��������ģ�����£�\n\t ����<--����-->·��<--����-->������\n\n");

    printf("ʵ����ʹ��TCPЭ�鴫�����ݣ�������ΪTCP Server����������ΪTCP Client\n\n");

    printf("�����̵�IP��ַ����User/arch/sys_arch.h�ļ����޸�\n\n");

    printf("�����̲ο�<<LwIPӦ��ʵս����ָ��>>��15�� ʹ�� NETCONN �ӿڱ��\n\n");
    
    while(1)
    {
//        pri = dos_interrupt_disable();
////        printf("task1\n");
//        dos_interrupt_enable(pri);
        dos_task_sleep(100000);
        
    }
}


void test_task2(void *Parameter)
{
    dos_uint32 pri;
    while(1)
    {
        pri = dos_interrupt_disable();
        printf("mem:%d\n",dos_get_mem_heap_info());
        dos_interrupt_enable(pri);
        dos_task_sleep(1000);
        
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    dos_system_init();
    
   
    task1 = dos_task_create( "test_task1",
                            &test_task1,
                            DOS_NULL,
                            2048,
                            6,
                            20);

    task2 = dos_task_create( "test_task2",
                            &test_task2,
                            DOS_NULL,
                            2048,
                            7,
                            0);
                
    dos_system_start_run();
  
}




/********************************END OF FILE***************************************/

