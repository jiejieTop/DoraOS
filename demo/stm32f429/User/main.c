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
dos_task_t task1 = DOS_NULL;
dos_task_t task2 = DOS_NULL;

/** Macro definition */


/** Function declaration */
extern void BSP_Init(void);

void test_task1(void *Parameter)
{
    dos_uint32 pri;
    while(1)
    {
        pri = dos_interrupt_disable();
        DOS_LOG_INFO("task1\n");
        dos_interrupt_enable(pri);
        dos_task_sleep(1000);
        
    }
}


void test_task2(void *Parameter)
{
    dos_uint32 pri;
    while(1)
    {
        pri = dos_interrupt_disable();
        DOS_LOG_INFO("task2\n");
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
                            512,
                            2,
                            20);

    task2 = dos_task_create( "test_task2",
                            &test_task2,
                            DOS_NULL,
                            512,
                            3,
                            0);
                
    dos_system_start_run();
  
}




/********************************END OF FILE***************************************/

