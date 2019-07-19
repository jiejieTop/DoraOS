/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
#include "include.h"
#include <task.h>
#include <queue.h>
#include <log.h>

/** Variable declaration */
DOS_TaskCB_t task = DOS_NULL;
DOS_TaskCB_t task1 = DOS_NULL;
Dos_Queue_t queue = DOS_NULL;


/** Macro definition */
#define QUEUE_LEN   10
#define QUEUE_SIZE  50

/** Function declaration */
static void BSP_Init(void);


void queue_read_task(void *Parameter)
{
    dos_uint32 res;
    dos_uint8 buff[QUEUE_SIZE];

    while(1)
    {
        DOS_LOG_INFO("start read buff, wait time is DOS_WAIT_FOREVER\n");
        res = Dos_QueueRead(queue, buff, QUEUE_SIZE, DOS_WAIT_FOREVER);
        if(res == DOS_OK)
        {
            DOS_LOG_INFO("read buff success: %s\n", buff);
        }
        else
        {
            DOS_LOG_WARN("read buff fail\n");
        }

        

    }
}
void queue_write_task(void *Parameter)
{
    dos_uint8 buff[] = "this is a queue test";
  
    while(1)
    {
        DOS_LOG_INFO("start write buff, wait time is 0\n");

        Dos_QueueWrite(queue, buff , QUEUE_SIZE, 0);

        Dos_TaskSleep(1000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    Dos_SystemInit();

    queue = Dos_QueueCreate(QUEUE_LEN, QUEUE_SIZE);

    task = Dos_TaskCreate( "queue_read_task",
                            &queue_read_task,
                            DOS_NULL,
                            1024,
                            2,
                            20);

    task1 = Dos_TaskCreate( "queue_write_task",
                            &queue_write_task,
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

