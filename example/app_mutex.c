/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-18 23:05:52
 * @LastEditTime: 2019-12-05 23:08:21
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "include.h"
#include <task.h>
#include <mutex.h>
#include <log.h>

/** Variable declaration */
dos_task_t task = DOS_NULL;
dos_task_t task1 = DOS_NULL;
dos_mutex_t mutex = DOS_NULL;


/** Macro definition */


/** Function declaration */
static void BSP_Init(void);


void mutex_task(void *parameter)
{
    dos_uint32 res;

    while (1) {
        DOS_LOG_INFO("start pend mutex, wait time is DOS_WAIT_FOREVER\n");
        res = dos_mutex_pend(mutex, DOS_WAIT_FOREVER);
        if (res == DOS_OK) {
            DOS_LOG_INFO("pend mutex success\n");
        } else {
            DOS_LOG_WARN("wait mutex fail\n");
            return;
        }
        DOS_LOG_INFO("start post mutex\n");
        dos_mutex_post(mutex);
        dos_task_sleep(1000);
    }
}


void mutex_delete_task(void *parameter)
{
    dos_uint32 res;
    dos_task_sleep(4000);

    res =dos_mutex_delete(mutex);
    if (res == DOS_OK) {
        mutex = DOS_NULL;
        DOS_LOG_INFO("delete mutex success\n");
    } else {
        DOS_LOG_WARN("delete mutex fail\n");
    }

    while (1) {
        DOS_LOG_INFO("mutex_delete_task running\n");

        dos_task_sleep(1000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    dos_system_init();

    mutex = dos_mutex_create();

    task = dos_task_create( "mutex_task",
                            &mutex_task,
                            DOS_NULL,
                            1024,
                            3,
                            20);

    task1 = dos_task_create( "mutex_delete_task",
                             &mutex_delete_task,
                             DOS_NULL,
                             1024,
                             2,
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

