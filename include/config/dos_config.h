#ifndef _DOS_CONFIG_H_
#define _DOS_CONFIG_H_

#include <dos_def.h>
#include <debug.h>
#include <list.h>
#include <mem.h>
#include <task.h>

#include "stm32f10x.h"

#define         DOS_MAX_PRIORITY_NUM            32U

#define         DOS_LOGO                        DOS_LOGO_BIG    /** Choose big logo or small logo */

#define         DOS_IDLE_TASK_PRIORITY          (DOS_MAX_PRIORITY_NUM - 1U)
#define         DOS_IDLE_TASK_SIZE              128U

#define         DOS_SYSTEM_CLOCK_HZ             (SystemCoreClock)
#define         DOS_SYSTICK_CLOCK_HZ            1000U


#define         DOS_ALIGN_SIZE                  (8)

#define         MEM_HEAP_SIZE                   1024*30

#define         DOS_IPC_QUEUQ                   1


#endif

