#ifndef _TASK_H_
#define _TASK_H_

#include "dos_def.h"
#include <list.h>
#include <port.h>
#include <dos_config.h>

#ifndef     DOS_MAX_PRIORITY_NUM
#define     DOS_MAX_PRIORITY_NUM        32U
#endif

#define     DOS_GET_STRUCT(ptr, type, member)   ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
#define     DOS_GET_TCB(node)    DOS_GET_STRUCT(node, struct DOS_TaskCB, StateItem)


#if DOS_MAX_PRIORITY_NUM > 32U
#define   DOS_PRIORITY_TAB  (((DOS_MAX_PRIORITY_NUM -1 )/32) + 1)
#define   DOS_PRIORITY_TAB_INDEX(PRI)  (((PRI -1 )/32))

/** set or reset the task ready priority */
#define     DOS_SET_TASK_PTIORITY(task)         (Dos_Task_Priority[DOS_PRIORITY_TAB_INDEX(task->Priority)]) |= (0x01 << (task->Priority % 32))
#define     DOS_RESET_TASK_PTIORITY(task)       (Dos_Task_Priority[DOS_PRIORITY_TAB_INDEX(task->Priority)]) &= ~(0x01 << (task->Priority % 32))
#else 
/** set or reset the task ready priority */
#define     DOS_SET_TASK_PTIORITY(task)         (Dos_Task_Priority) |= (0x01 << task->Priority)
#define     DOS_RESET_TASK_PTIORITY(task)       (Dos_Task_Priority) &= ~(0x01 << task->Priority)
#endif

#define     DOS_SET_TASK_STATUS(task, status)     (task->TaskStatus) |= (status)
#define     DOS_RESET_TASK_STATUS(task, status)   (task->TaskStatus) &= ~(status)

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


struct DOS_TaskCB
{
  dos_void                        *StackPoint;              /** Task stack point            */
  dos_void                        *StackAddr;               /** Task stack point            */
  dos_uint16                      TaskStatus;               /** Task status */
  dos_uint16                      Priority;
  dos_uint32                      StackSize;                /** Task stack size             */
  dos_void                        *TopOfStack;              /** Task stack top              */
  dos_uint32                      TaskTick;                 /** TaskTick                    */
  dos_uint32                      TaskInitTick;             /** TaskInitTick                */ 
  dos_void                        *TaskEntry;               /** Task entrance function      */
  dos_void                        *TaskMutex;               /** Task-held mutex             */
  dos_void                        *Parameter;               /** Parameter                   */
  dos_char                        *TaskName;                /** Task name                   */
  Dos_TaskItem_t                  StateItem;
  Dos_TaskItem_t                  PendItem;
};
typedef struct DOS_TaskCB * DOS_TaskCB_t;




void Dos_SystemInit(void);
DOS_TaskCB_t Dos_TaskCreate(const dos_char *dos_name,
                            void (*dos_task_entry)(void *dos_param),
                            void * const dos_param,
                            dos_uint32 dos_stack_size,
                            dos_uint16 dos_priority);
dos_err Dos_TaskDelete(DOS_TaskCB_t dos_task);
dos_uint32 Dos_Get_Tick(void);         
void Dos_Start( void );
void Dos_TaskSleep(dos_uint32 dos_sleep_tick);
void Dos_Scheduler(void);
void Dos_Scheduler_Lock(void);
void Dos_Scheduler_Unlock(void);
dos_bool Dos_Scheduler_IsLock(void);
DOS_TaskCB_t Dos_Get_CurrentTCB(void);
DOS_TaskCB_t Dos_GetTCB(Dos_TaskList_t *list);
DOS_TaskCB_t Dos_Get_NextTCB(Dos_TaskList_t *list);
dos_void Dos_TaskWait(Dos_TaskList_t *dos_list, dos_uint32 timeout);
dos_void Dos_TaskWake(DOS_TaskCB_t task);
dos_void Dos_SetTaskPrio(DOS_TaskCB_t task, dos_uint16 prio);

#endif

