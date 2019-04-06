#ifndef _DOS_CONFIG_H_
#define _DOS_CONFIG_H_

#include "dos_def.h"
/* debug */
#include <debug.h>

#ifndef     MAX_PRIORITY_NUM
#define     MAX_PRIORITY_NUM        32U
#endif

#define     IDLE_TASK_PRIORITY      (MAX_PRIORITY_NUM - 1U)
#define     IDLE_TASK_SIZE          128U

#define SYSTEM_CLOCK_HZ					    (SystemCoreClock)
#define DOS_SYSTICK_CLOCK_HZ        1000U


                            
#endif

