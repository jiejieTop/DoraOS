/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-18 23:05:52
 * @LastEditTime: 2019-12-05 23:08:28
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "include.h"
#include <task.h>
#include <sem.h>
#include <log.h>

/** Variable declaration */
dos_task_t task = DOS_NULL;
dos_task_t task1 = DOS_NULL;
dos_sem_t sem = DOS_NULL;


/** Macro definition */


/** Function declaration */
static void BSP_Init(void);


void sem_wait_task(void *parameter)
{
    dos_uint32 res;

    while (1) {
        DOS_LOG_INFO("start wait sem, wait time is DOS_WAIT_FOREVER\n");
        res = dos_sem_pend(sem, DOS_WAIT_FOREVER);
        if (res == DOS_OK) {
            DOS_LOG_INFO("wait sem success\n");
        } else {
            DOS_LOG_WARN("wait sem fail\n");
        }
    }
}


void sem_post_task(void *parameter)
{
    while (1) {
        DOS_LOG_INFO("start post sem\n");

        dos_sem_post(sem);

        dos_task_sleep(1000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    dos_system_init();

    sem = dos_binary_sem_create(0);

    task = dos_task_create( "sem_wait_task",
                            &sem_wait_task,
                            DOS_NULL,
                            1024,
                            2,
                            20);

    task1 = dos_task_create( "sem_post_task",
                             &sem_post_task,
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

