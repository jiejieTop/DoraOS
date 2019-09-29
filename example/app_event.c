/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
//#include "include.h"
#include <task.h>
#include <event.h>
#include <log.h>

/** Variable declaration */
dos_task_t task = DOS_NULL;
dos_task_t task1 = DOS_NULL;
dos_event_t event = DOS_NULL;

/** Macro definition */
#define EVENT1 (0x01<<1)
#define EVENT2 (0x01<<2)
#define EVENT3 (0x01<<3)
#define EVENT4 (0x01<<4)

/** Function declaration */
  
static void BSP_Init(void);


void event_wait_task(void *parameter)
{
  dos_uint32 res;

  while(1)
  {
    DOS_LOG_DEBUG("wait event 1 or 2 5000 tick, and do not clear events\n");
    res = dos_event_wait(event, EVENT1 | EVENT2, WAIT_ANY_EVENT | NO_CLR_EVENT, 5000);
    if((res & EVENT1) || (res & EVENT2))
      DOS_LOG_DEBUG("wait event 1 or 2 success");

    dos_task_sleep(1000);

    DOS_LOG_DEBUG("wait event 1 or 2 forever, and do not clear events\n");
    res = dos_event_wait(event, EVENT1 | EVENT2, WAIT_ANY_EVENT | NO_CLR_EVENT, DOS_WAIT_FOREVER);
    if((res & EVENT1) || (res & EVENT2))
      DOS_LOG_DEBUG("wait event 1 or 2 success");


    DOS_LOG_DEBUG("wait event 1 and 2 500 tick\n");
    res = dos_event_wait(event, EVENT1 | EVENT2, WAIT_ALL_EVENT, 500);
    if(res &= (EVENT1 | EVENT2))
      DOS_LOG_DEBUG("wait event 1 and 2 success");

    DOS_LOG_DEBUG("wait event 1 and 2 1000 tick\n");
    res = dos_event_wait(event, EVENT1 | EVENT2, WAIT_ALL_EVENT, DOS_WAIT_FOREVER);
    if(res &= (EVENT1 | EVENT2))
      DOS_LOG_DEBUG("wait event 1 and 2 success");
  }
}
void event_set_task(void *parameter)
{
  while(1)
  {
    DOS_LOG_DEBUG("set event 1\n");
    dos_event_set(event, EVENT1);

    dos_task_sleep(1000);

    DOS_LOG_DEBUG("set event 2\n");
    dos_event_set(event, EVENT2);
    dos_task_sleep(1000);
  }
}

/** main function */

int main(void)
{
  BSP_Init();
  
  dos_system_init();
  
  event = dos_event_create();

  task = dos_task_create( "event_wait",
                          &event_wait_task,
                          DOS_NULL,
                          1024,
                          2,
                          20);

  task1 = dos_task_create( "event_set",
                          &event_set_task,
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

