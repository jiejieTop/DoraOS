/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-15 23:56:23
 * @LastEditTime: 2019-12-05 23:25:31
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "include.h"
#include <task.h>
#include <log.h>
#include <memp.h>
#include "cmd.h"

/** Variable declaration */
dos_task_t task1 = DOS_NULL;
dos_task_t task2 = DOS_NULL;

/** Macro definition */


/** Function declaration */
static void BSP_Init(void);

void test_cmd(void)
{
    DOS_LOG_INFO("test_cmd\n");
}

REGISTER_CMD(test,test_cmd);


void test_task1(void *Parameter)
{
    while(1)
    {
        DOS_LOG_INFO("task1 running\n");
        cmd_parsing("test");
        dos_task_sleep(1000);
    }
}


void test_task2(void *Parameter)
{
    while(1)
    {
        DOS_LOG_INFO("task2 running\n");
        dos_task_sleep(1000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();
    cmd_init();
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



/**
 * This function is used to initialize all onboard hardware
 */ 
static void BSP_Init(void)
{
	USART_Config();
}



/********************************END OF FILE***************************************/

