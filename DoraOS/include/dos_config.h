#ifndef _DOS_CONFIG_H_
#define _DOS_CONFIG_H_

#include "dos_def.h"
/* debug */
#include <debug.h>

#define     DOS_MAX_PRIORITY_NUM        32U


#define     DOS_IDLE_TASK_PRIORITY      (DOS_MAX_PRIORITY_NUM - 1U)
#define     DOS_IDLE_TASK_SIZE          128U

#define DOS_SYSTEM_CLOCK_HZ					    (SystemCoreClock)
#define DOS_SYSTICK_CLOCK_HZ        1000U


                            
#endif

