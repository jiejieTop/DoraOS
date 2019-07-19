#ifndef _DOS_CONFIG_H_
#define _DOS_CONFIG_H_

#include <dos_def.h>
#include <debug.h>
#include <list.h>
#include <mem.h>
#include <task.h>

#include "stm32f10x.h"

#define         DOS_MAX_PRIORITY_NUM            (32U)

#define         DOS_LOGO_PRINT                  (0U)
#define         DOS_LOGO                        DOS_LOGO_BIG    /** Choose big logo or small logo */

#define         DOS_IDLE_TASK_PRIORITY          (DOS_MAX_PRIORITY_NUM - 1U)
#define         DOS_IDLE_TASK_SIZE              (512U)
#define         DOS_IDLE_TASK_TICK              (20U)

#define         DOS_SWTMR                       (0U)
#define         DOS_SWTMR_TASK_PRIORITY         (0U)
#define         DOS_SWTMR_TASK_SIZE             (1024U)
#define         DOS_SWTMR_TASK_TICK             (20U)


#define         DOS_SYSTEM_CLOCK_HZ             (SystemCoreClock)
#define         DOS_SYSTICK_CLOCK_HZ            (1000U)


#define         DOS_ALIGN_SIZE                  (8U)

#define         MEM_HEAP_SIZE                   (1024*30)

#define         DOS_IPC_QUEUQ                   (1U)

#define         DOS_LOG_LEVEL                   DOS_DEBUG_LEVEL
#define         DOS_LOG_COLOR                   (1U)
#define         DOS_LOG_TS                      (1U)
#define         DOS_LOG_TAR                     (1U)


#define         DOS_USE_SALOF                   (1U)
#define         DOS_SALOF_BUFF_SIZE             (256U)
#define         DOS_SALOF_FIFO_SIZE             (1024*4U)

#endif

