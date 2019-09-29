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
#include <memp.h>
#include <log.h>

/** Variable declaration */
dos_task_t task = DOS_NULL;
dos_task_t task1 = DOS_NULL;
dos_queue_t queue = DOS_NULL;
dos_memp_t  memp = DOS_NULL;

/** Macro definition */
#define QUEUE_LEN   10
#define QUEUE_SIZE  40

#define MEMP_SIZE   (QUEUE_SIZE * QUEUE_LEN)
#define MEMP_BLOCK_SIZE  QUEUE_SIZE

#if (DOS_HEAP_TO_MEMP == 0U)
dos_char arr[MEMP_SIZE];
#endif 

/** Function declaration */
static void BSP_Init(void);


void queue_read_task(void *parameter)
{
    dos_uint32 res;
    dos_char *p;
    while(1)
    {
        res = dos_queue_read(queue, &p, 4, DOS_WAIT_FOREVER);
        if(res == DOS_OK)
        {
            DOS_LOG_INFO("read buff success: %s\n", p);
            
            DOS_LOG_INFO("free memory addr: %p\n", p);
            dos_memp_free(p);
        }
        else
        {
            DOS_LOG_WARN("read buff fail\n");
        }

        dos_task_sleep(1000);

    }
}
void queue_write_task(void *parameter)
{
    dos_uint8 buff[] = "this is a queue test";
    dos_char *p;
    while(1)
    {
        DOS_LOG_INFO("start alloc memory, wait time is 0\n");

        p = (dos_char *)dos_memp_alloc(memp);
        if(p != DOS_NULL)
        {
            DOS_LOG_INFO("memory addr is %p\n", p);
            memcpy(p, buff, sizeof(buff));
            dos_queue_write(queue, &p , 4, 0);
        }
        else
        {
            DOS_LOG_WARN("alloc memory fail\n");
        }

        dos_task_sleep(1000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    dos_system_init();
    
#if (DOS_HEAP_TO_MEMP == 0U)
    memp = dos_memp_create(arr, MEMP_SIZE, MEMP_BLOCK_SIZE);
#else   
    memp = dos_memp_create(MEMP_SIZE, MEMP_BLOCK_SIZE);
#endif
    
    queue = dos_queue_create(QUEUE_LEN, QUEUE_SIZE);

    task = dos_task_create( "queue_read_task",
                            &queue_read_task,
                            DOS_NULL,
                            1024,
                            2,
                            20);

    task1 = dos_task_create( "queue_write_task",
                            &queue_write_task,
                            DOS_NULL,
                            1024,
                            3,
                            0);
                
    dos_system_start_run();
  
}



/**
 * This function is used to initialize all onboard hardware
 */ 
static void BSP_Init(void)
{
	USART_Config();
}



/********************************END OF FILE***************************************/

