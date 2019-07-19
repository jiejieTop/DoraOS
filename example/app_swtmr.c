/**
  * @file    app_event.c
  * @author  jiejie
  * @version V1.0
  * @date    2019
  * @brief   this is an event api example
  */ 
#include "include.h"
#include <task.h>
#include <swtmr.h>
#include <log.h>

/** Variable declaration */
DOS_TaskCB_t task1 = DOS_NULL;
DOS_TaskCB_t task2 = DOS_NULL;

Dos_Swtmr_t swtmr3 = DOS_NULL;
Dos_Swtmr_t swtmr2 = DOS_NULL;
Dos_Swtmr_t swtmr1 = DOS_NULL;


/** Macro definition */


/** Function declaration */
static void BSP_Init(void);

dos_void Swtmr_CallBacke1(dos_void *Parameter)
{
    DOS_LOG_INFO("Swtmr1 CallBack, tick = %d\n",Dos_Get_Tick());
}

dos_void Swtmr_CallBacke2(dos_void *Parameter)
{
    DOS_LOG_INFO("Swtmr2 CallBack, tick = %d\n",Dos_Get_Tick());
}

dos_void Swtmr_CallBacke3(dos_void *Parameter)
{
    DOS_LOG_INFO("Swtmr3 CallBack, tick = %d\n",Dos_Get_Tick());
}

void swtmr_stop_task1(void *Parameter)
{
    DOS_LOG_WARN("start swtmr1 2\n");
    Dos_SwtmrStart(swtmr1);
    Dos_SwtmrStart(swtmr2);


    Dos_TaskSleep(10000);
    DOS_LOG_WARN("stop swtmr1\n");
    Dos_SwtmrStop(swtmr1);

    while(1)
    {
        DOS_LOG_INFO("task1 running\n");

        Dos_TaskSleep(5000);
    }
}


void swtmr_stop_task2(void *Parameter)
{
    Dos_TaskSleep(20000);
    DOS_LOG_WARN("stop swtmr2 \n");
    Dos_SwtmrStop(swtmr2);
    
    DOS_LOG_WARN("start swtmr3\n");
    Dos_SwtmrStart(swtmr3);
    while(1)
    {
        DOS_LOG_INFO("task2 running\n");

        Dos_TaskSleep(5000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    Dos_SystemInit();

    swtmr1 = Dos_SwtmrCreate(1000, Dos_Swtmr_PeriodMode, Swtmr_CallBacke1, DOS_NULL);
    swtmr2 = Dos_SwtmrCreate(2000, Dos_Swtmr_PeriodMode, Swtmr_CallBacke2, DOS_NULL);
    swtmr3 = Dos_SwtmrCreate(10000, Dos_Swtmr_OnceMode, Swtmr_CallBacke3, DOS_NULL);

    task1 = Dos_TaskCreate( "swtmr_stop_task1",
                            &swtmr_stop_task1,
                            DOS_NULL,
                            1024,
                            2,
                            20);

    task2 = Dos_TaskCreate( "swtmr_stop_task2",
                            &swtmr_stop_task2,
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

