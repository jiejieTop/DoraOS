/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-18 23:05:51
 * @LastEditTime: 2019-12-05 23:08:32
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "include.h"
#include <task.h>
#include <swtmr.h>
#include <log.h>

/** Variable declaration */
dos_task_t task1 = DOS_NULL;
dos_task_t task2 = DOS_NULL;

dos_swtmr_t swtmr3 = DOS_NULL;
dos_swtmr_t swtmr2 = DOS_NULL;
dos_swtmr_t swtmr1 = DOS_NULL;


/** Macro definition */


/** Function declaration */
static void BSP_Init(void);

dos_void Swtmr_CallBacke1(dos_void *parameter)
{
    DOS_LOG_INFO("Swtmr1 CallBack, tick = %d\n",dos_get_tick());
}

dos_void Swtmr_CallBacke2(dos_void *parameter)
{
    DOS_LOG_INFO("Swtmr2 CallBack, tick = %d\n",dos_get_tick());
}

dos_void Swtmr_CallBacke3(dos_void *parameter)
{
    DOS_LOG_INFO("Swtmr3 CallBack, tick = %d\n",dos_get_tick());
}

void swtmr_stop_task1(void *parameter)
{
    DOS_LOG_WARN("start swtmr 1 2\n");
    dos_swtmr_start(swtmr1);
    dos_swtmr_start(swtmr2);


    dos_task_sleep(10000);
    DOS_LOG_WARN("stop swtmr 1\n");
    dos_swtmr_stop(swtmr1);

    while (1) {
        DOS_LOG_INFO("task1 running\n");

        dos_task_sleep(5000);
    }
}


void swtmr_stop_task2(void *parameter)
{
    dos_task_sleep(20000);
    DOS_LOG_WARN("stop swtmr 2 \n");
    dos_swtmr_stop(swtmr2);

    DOS_LOG_WARN("start swtmr 3\n");
    dos_swtmr_start(swtmr3);
    while (1) {
        DOS_LOG_INFO("task2 running\n");

        dos_task_sleep(5000);
    }
}

/** main function */

int main(void)
{
    BSP_Init();

    dos_system_init();

    swtmr1 = dos_swtmr_create(1000, dos_swtmr_mode_period, Swtmr_CallBacke1, DOS_NULL);
    swtmr2 = dos_swtmr_create(2000, dos_swtmr_mode_period, Swtmr_CallBacke2, DOS_NULL);
    swtmr3 = dos_swtmr_create(10000, dos_swtmr_mode_one, Swtmr_CallBacke3, DOS_NULL);

    task1 = dos_task_create( "swtmr_stop_task1",
                             &swtmr_stop_task1,
                             DOS_NULL,
                             1024,
                             2,
                             20);

    task2 = dos_task_create( "swtmr_stop_task2",
                             &swtmr_stop_task2,
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

