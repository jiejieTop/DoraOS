/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
#include "include.h"
#include <task.h>
#include <sem.h>
#include <log.h>

/** Variable declaration */
DOS_TaskCB_t task = DOS_NULL;
DOS_TaskCB_t task1 = DOS_NULL;
Dos_Sem_t sem = DOS_NULL;


/** Macro definition */


/** Function declaration */
static void BSP_Init(void);


void sem_wait_task(void *Parameter)
{
    dos_uint32 res;

    while(1)
    {
        DOS_LOG_INFO("start wait sem, wait time is DOS_WAIT_FOREVER\n");
        res = Dos_SemWait(sem, DOS_WAIT_FOREVER);
        if(res == DOS_OK)
        {
            DOS_LOG_INFO("wait sem success\n");
        }
        else
        {
            DOS_LOG_WARN("wait sem fail\n");
        }
    }
}


void sem_post_task(void *Parameter)
{
    while(1)
    {
        DOS_LOG_INFO("start post sem\n");

        Dos_SemPost(sem);

        Dos_TaskSleep(1000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    Dos_SystemInit();

    sem = Dos_BinarySem_Create(0);

    task = Dos_TaskCreate( "sem_wait_task",
                            &sem_wait_task,
                            DOS_NULL,
                            1024,
                            2,
                            20);

    task1 = Dos_TaskCreate( "sem_post_task",
                            &sem_post_task,
                            DOS_NULL,
                            1024,
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

