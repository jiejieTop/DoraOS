#ifndef _DOS_CONFIG_H_
#define _DOS_CONFIG_H_

#include <dos_def.h>
#include <debug.h>
#include <list.h>
#include <mem.h>
#include <task.h>

#include "stm32f4xx.h"

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    extern uint32_t SystemCoreClock;
#endif

#define         DOS_MAX_PRIORITY_NUM            (32U)

#define         DOS_LOGO_PRINT                  (1U)
#define         DOS_LOGO                        DOS_LOGO_BIG    /** Choose big logo or small logo */

#define         DOS_IDLE_TASK_PRIORITY          (DOS_MAX_PRIORITY_NUM - 1U)
#define         DOS_IDLE_TASK_SIZE              (512U)
#define         DOS_IDLE_TASK_TICK              (20U)

#define         DOS_SWTMR                       (1U)
#define         DOS_SWTMR_QUEUE_SIZE            (4U)
#define         DOS_SWTMR_TASK_PRIORITY         (0U)
#define         DOS_SWTMR_TASK_SIZE             (1024U)
#define         DOS_SWTMR_TASK_TICK             (20U)


#define         DOS_HW_CLZ_SUPT                 (0U)

#define         DOS_SYSTEM_CLOCK_HZ             (SystemCoreClock)
#define         DOS_SYSTICK_CLOCK_HZ            (1000U)


#define         DOS_ALIGN_SIZE                  (4U)

#define         DOS_MEM_HEAP_SIZE               (1024*30)
#define         DOS_HEAP_TO_MEMP                (1U)    /** memory heap to create memory pool */

#define         DOS_IPC_QUEUQ                   (1U)

#define         DOS_LOG_LEVEL                   DOS_DEBUG_LEVEL
#define         DOS_LOG_COLOR                   (0U)
#define         DOS_LOG_TS                      (1U)
#define         DOS_LOG_TAR                     (1U)

#define         DOS_TICK_PERIOD_MS			    (1000U / DOS_SYSTICK_CLOCK_HZ )

#define         DOS_USE_SALOF                   (0U)
#define         DOS_SALOF_BUFF_SIZE             (256U)
#define         DOS_SALOF_FIFO_SIZE             (1024*4U)

#endif

