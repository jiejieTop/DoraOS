/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
#include "include.h"
#include <task.h>
#include <log.h>
#include <memp.h>
/** Variable declaration */
DOS_TaskCB_t task1 = DOS_NULL;
DOS_TaskCB_t task2 = DOS_NULL;
Dos_Memp_t  testmemp = DOS_NULL;

/** Macro definition */


/** Function declaration */
static void BSP_Init(void);


void test_task1(void *Parameter)
{
    while(1)
    {
        DOS_LOG_INFO("task1 running\n");

        Dos_TaskSleep(1000);
    }
}


void test_task2(void *Parameter)
{
    dos_uint8 *p;
    while(1)
    {
        DOS_LOG_INFO("task2 running\n");
        p = (dos_uint8 *)Dos_MempAlloc(testmemp);
        DOS_LOG_DEBUG("memp addr is %p\n", p);
        Dos_TaskSleep(1000);
    }
}

/** main function */

int main(void)
{
    dos_uint8 *p1;
    dos_uint8 *p2;
    dos_uint8 *p3;
    dos_uint8 *p4;
    dos_uint8 *p5;
    dos_uint8 *p6;
    int i;
    BSP_Init();

    Dos_SystemInit();
    testmemp = Dos_MempCreate(160,16);
    
    p1 = (dos_uint8 *)Dos_MempAlloc(testmemp);
    DOS_LOG_DEBUG("memp addr is %p\n", p1);
    
    p2 = (dos_uint8 *)Dos_MempAlloc(testmemp);
    DOS_LOG_DEBUG("memp addr is %p\n", p2);
    
    p3 = (dos_uint8 *)Dos_MempAlloc(testmemp);
    DOS_LOG_DEBUG("memp addr is %p\n", p3);

//    Dos_MempFree(p1);
//    Dos_MempFree(p2);
//    Dos_MempFree(p3);
    
    p4 = (dos_uint8 *)Dos_MempAlloc(testmemp);
    DOS_LOG_DEBUG("memp addr is %p\n", p4);
    
    p5 = (dos_uint8 *)Dos_MempAlloc(testmemp);
    DOS_LOG_DEBUG("memp addr is %p\n", p5);
    
    p6 = (dos_uint8 *)Dos_MempAlloc(testmemp);
    DOS_LOG_DEBUG("memp addr is %p\n", p6);
    
    
    task1 = Dos_TaskCreate( "test_task1",
                            &test_task1,
                            DOS_NULL,
                            512,
                            2,
                            20);

    task2 = Dos_TaskCreate( "test_task2",
                            &test_task2,
                            DOS_NULL,
                            512,
                            3,
                            0);
                
    Dos_Start();
  
}



/**
 * This function is used to initialize all onboard hardware
 */ 
static void BSP_Init(void)
{
	USART_Config();
}



/********************************END OF FILE***************************************/

