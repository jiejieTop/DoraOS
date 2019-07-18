/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
#include "include.h"
#include <task.h>
#include <event.h>
#include <log.h>

/** Variable declaration */
DOS_TaskCB_t task = DOS_NULL;
DOS_TaskCB_t task1 = DOS_NULL;
Dos_Event_t event = DOS_NULL;

/** Macro definition */
#define EVENT1 (0x01<<1)
#define EVENT2 (0x01<<2)
#define EVENT3 (0x01<<3)
#define EVENT4 (0x01<<4)

/** Function declaration */
  
static void BSP_Init(void);


void event_wait_task(void *Parameter)
{
  dos_uint32 res;

  while(1)
  {
    DOS_LOG_DEBUG("wait event 1 or 2 5000 tick, and do not clear events\n");
    res = Dos_EventWait(event, EVENT1 | EVENT2, WAIT_ANY_EVENT | NO_CLR_EVENT, 5000);
    if((res & EVENT1) || (res & EVENT2))
      DOS_LOG_DEBUG("wait event 1 or 2 success");

    Dos_TaskSleep(1000);

    DOS_LOG_DEBUG("wait event 1 or 2 forever, and do not clear events\n");
    res = Dos_EventWait(event, EVENT1 | EVENT2, WAIT_ANY_EVENT | NO_CLR_EVENT, DOS_WAIT_FOREVER);
    if((res & EVENT1) || (res & EVENT2))
      DOS_LOG_DEBUG("wait event 1 or 2 success");


    DOS_LOG_DEBUG("wait event 1 and 2 500 tick\n");
    res = Dos_EventWait(event, EVENT1 | EVENT2, WAIT_ALL_EVENT, 500);
    if(res &= (EVENT1 | EVENT2))
      DOS_LOG_DEBUG("wait event 1 and 2 success");

    DOS_LOG_DEBUG("wait event 3 and 4 1000 tick\n");
    res = Dos_EventWait(event, EVENT1 | EVENT2, WAIT_ALL_EVENT, DOS_WAIT_FOREVER);
    if(res &= (EVENT1 | EVENT2))
      DOS_LOG_DEBUG("wait event 1 and 2 success");
  }
}
void event_set_task(void *Parameter)
{
  while(1)
  {
    DOS_LOG_DEBUG("set event 1\n");
    Dos_EventSet(event, EVENT1);

    Dos_TaskSleep(1000);

    DOS_LOG_DEBUG("set event 2\n");
    Dos_EventSet(event, EVENT2);
    Dos_TaskSleep(1000);
  }
}

/** main function */

int main(void)
{
  BSP_Init();
  
  Dos_SystemInit();
  
  event = Dos_EventCreate();

  task = Dos_TaskCreate( "event_wait",
                          &event_wait_task,
                          DOS_NULL,
                          1024,
                          2,
                          20);

  task1 = Dos_TaskCreate( "event_set",
                          &event_set_task,
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
	/* ´®¿Ú³õÊ¼»¯ */
	USART_Config();
}



/********************************END OF FILE***************************************/

