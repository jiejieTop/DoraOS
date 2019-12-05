/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-15 23:56:23
 * @LastEditTime: 2019-12-05 23:03:04
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _TASK_H_
#define _TASK_H_

#include "dos_def.h"
#include <list.h>
#include <port.h>
#include <sys.h>
#include <dos_config.h>

//#ifndef     DOS_MAX_PRIORITY_NUM
//#define     DOS_MAX_PRIORITY_NUM        32U
//#endif

#define     DOS_GET_STRUCT(ptr, type, member)   ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
#define     DOS_GET_TCB(node)    DOS_GET_STRUCT(node, struct dos_task, status_item)


#if DOS_MAX_PRIORITY_NUM > 32U
#define     DOS_PRIORITY_TAB  (((DOS_MAX_PRIORITY_NUM -1 )/32) + 1)
#define     DOS_PRIORITY_TAB_INDEX(PRI)  (((PRI -1 )/32))

/** set or reset the task ready priority */
#define     DOS_SET_TASK_PTIORITY(task)         (dos_task_priority[DOS_PRIORITY_TAB_INDEX(task->priority)]) |= (0x01 << (task->priority % 32))
#define     DOS_RESET_TASK_PTIORITY(task)       (dos_task_priority[DOS_PRIORITY_TAB_INDEX(task->priority)]) &= ~(0x01 << (task->priority % 32))
#else
/** set or reset the task ready priority */
#define     DOS_SET_TASK_PTIORITY(task)         (dos_task_priority) |= (0x01 << task->priority)
#define     DOS_RESET_TASK_PTIORITY(task)       (dos_task_priority) &= ~(0x01 << task->priority)
#endif

#define     DOS_SET_TASK_STATUS(task, status)     (task->task_status) |= (status)
#define     DOS_RESET_TASK_STATUS(task, status)   (task->task_status) &= ~(status)

#define     dos_interrupt_disable       Interrupt_Disable
#define     dos_interrupt_enable        Interrupt_Enable

/**
 * Task status
 */
#define   DOS_TASK_STATUS_MASK        0xFF
#define   DOS_TASK_STATUS_UNUSED      0x01    /** unused status */
#define   DOS_TASK_STATUS_READY       0x02    /** ready status */
#define   DOS_TASK_STATUS_RUNNING     0x04    /** running status */
#define   DOS_TASK_STATUS_DELAY       0x08    /** delay status */
#define   DOS_TASK_STATUS_TIMEOUT     0x10    /** timeout status */
#define   DOS_TASK_STATUS_SUSPEND     0x20    /** supend status */
#define   DOS_TASK_STATUS_EVENT       0x40    /** event status, reserved */
#define   DOS_TASK_STATUS_READ        0x80    /** read status, reserved */
//#define   DOS_TASK_STATUS_WRITE       0x80    /** write status, reserved */


#define   DOS_WAIT_FOREVER            0xFFFFFFFF    /** Define the timeout interval as LOS_WAIT_FOREVER. */


struct dos_task {
    dos_void                        *stack_point;              /** Task stack point            */
    dos_void                        *stack_addr;               /** Task stack point            */
    dos_uint16                      task_status;               /** Task status */
    dos_uint16                      priority;
    dos_uint32                      stack_size;                /** Task stack size             */
    dos_void                        *top_of_stack;              /** Task stack top              */
    dos_void                        *task_entry;               /** Task entrance function      */
    dos_void                        *parameter;               /** parameter                   */
    dos_char                        *task_name;                /** Task name                   */
    dos_uint32                      task_tick;                 /** task_tick                    */
    dos_uint32                      task_init_tick;             /** task_init_tick                */
    dos_uint32                      wait_event;                /** Task wait event             */
    dos_uint32                      wait_event_opt;              /** Task wait event options     */
    dos_uint32                      event_get;                 /** Task Get event              */
    dos_task_item_t                  status_item;                /** Task status item            */
    dos_task_item_t                  pend_item;                 /** Task pend item              */
};
typedef struct dos_task * dos_task_t;




void dos_task_init(void);
dos_task_t dos_task_create(const dos_char *name,
                           void (*task_entry)(void *param),
                           void * const param,
                           dos_uint32 stack_size,
                           dos_uint16 priority,
                           dos_uint32 tick);
dos_err dos_task_delete(dos_task_t task);
dos_uint32 dos_get_tick(void);
void dos_system_start_run( void );
void dos_task_sleep(dos_uint32 sleep_tick);
dos_char *dos_get_task_name(void);
dos_task_t dos_get_current_task(void);
dos_task_t dos_get_first_task(dos_task_list_t *list);
dos_task_t dos_get_next_task(dos_task_list_t *list);
dos_void dos_task_wait(dos_task_list_t *dos_list, dos_uint32 timeout);
dos_void dos_task_wake(dos_task_t task);
dos_void dos_set_task_priority(dos_task_t task, dos_uint16 prio);
dos_void dos_tick_update(dos_void);
dos_bool dos_check_task_priority(dos_void);
dos_void dos_choose_highest_priority_task(dos_void);
void dos_reset_tick(void);

#endif

