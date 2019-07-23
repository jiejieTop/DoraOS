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
/** Variable declaration */
DOS_TaskCB_t task1 = DOS_NULL;
DOS_TaskCB_t task2 = DOS_NULL;

/** Macro definition */


/** Function declaration */
extern void BSP_Init(void);

void test_task1(void *Parameter)
{
    dos_uint32 pri;
    while(1)
    {
        pri = Dos_Interrupt_Disable();
        DOS_LOG_INFO("task1\n");
        Dos_Interrupt_Enable(pri);
        Dos_TaskSleep(1000);
        
    }
}


void test_task2(void *Parameter)
{
    dos_uint32 pri;
    while(1)
    {
        pri = Dos_Interrupt_Disable();
        DOS_LOG_INFO("task2\n");
        Dos_Interrupt_Enable(pri);
        Dos_TaskSleep(1000);
        
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    Dos_SystemInit();
    
//    DOS_LOG_INFO("sysclk:%d\n", HAL_RCC_GetSysClockFreq());
//    DOS_LOG_INFO("hclk:%d\n", HAL_RCC_GetHCLKFreq());
//    DOS_LOG_INFO("pclk1:%d\n", HAL_RCC_GetPCLK1Freq());
//    DOS_LOG_INFO("pclk2:%d\n", HAL_RCC_GetPCLK2Freq());
    
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




/********************************END OF FILE***************************************/

