/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
#include "include.h"
#include <task.h>
#include <mutex.h>
#include <log.h>

/** Variable declaration */
DOS_TaskCB_t task = DOS_NULL;
DOS_TaskCB_t task1 = DOS_NULL;
Dos_Mutex_t mutex = DOS_NULL;


/** Macro definition */


/** Function declaration */
static void BSP_Init(void);


void mutex_task(void *Parameter)
{
    dos_uint32 res;

    while(1)
    {
        DOS_LOG_INFO("start pend mutex, wait time is DOS_WAIT_FOREVER\n");
        res = Dos_MutexPend(mutex, DOS_WAIT_FOREVER);
        if(res == DOS_OK)
        {
            DOS_LOG_INFO("pend mutex success\n");
        }
        else
        {
            DOS_LOG_WARN("wait mutex fail\n");
            return;
        }
        DOS_LOG_INFO("start post mutex\n");
        Dos_MutexPost(mutex);
        Dos_TaskSleep(1000);
    }
}


void mutex_delete_task(void *Parameter)
{
    dos_uint32 res;
    Dos_TaskSleep(4000);

    res =Dos_MutexDelete(mutex);
    if(res == DOS_OK)
    {
        mutex = DOS_NULL;
        DOS_LOG_INFO("delete mutex success\n");
    }
    else
    {
        DOS_LOG_WARN("delete mutex fail\n");
    }

    while(1)
    {
        DOS_LOG_INFO("mutex_delete_task running\n");

        Dos_TaskSleep(1000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    Dos_SystemInit();

    mutex = Dos_MutexCreate();

    task = Dos_TaskCreate( "mutex_task",
                            &mutex_task,
                            DOS_NULL,
                            1024,
                            3,
                            20);

    task1 = Dos_TaskCreate( "mutex_delete_task",
                            &mutex_delete_task,
                            DOS_NULL,
                            1024,
                            2,
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

