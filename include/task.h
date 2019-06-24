#ifndef _TASK_H_
#define _TASK_H_

#include "dos_def.h"
#include <list.h>
#include <mem.h>
/* debug */
#include <debug.h>
#include <port.h>
#include <dos_config.h>

#ifndef     DOS_MAX_PRIORITY_NUM
#define     DOS_MAX_PRIORITY_NUM        32U
#endif

#define     DOS_GET_STRUCT(ptr, type, member)   ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
#define     DOS_GET_TCB(node)    DOS_GET_STRUCT(node, struct DOS_TaskCB, StateItem)

#define     DOS_TASKYIELD               (INT_CTRL_REG = PENDSVSET_BIT)

/**
 * Task status
 */
#define   DOS_TASK_STATUS_MASK        0xFF
#define   DOS_TASK_STATUS_UNUSED      0x01    /** unused status */
#define   DOS_TASK_STATUS_READY       0x02    /** ready status */    
#define   DOS_TASK_STATUS_RUNNING     0x03    /** running status */
#define   DOS_TASK_STATUS_DELAY       0x08    /** delay status */
#define   DOS_TASK_STATUS_TIMEOUT     0x10    /** timeout status */
#define   DOS_TASK_STATUS_SUSPEND     0x20    /** supend status */
#define   DOS_TASK_STATUS_EVENT       0x40    /** event status, reserved */
#define   DOS_TASK_STATUS_READ        0x80    /** read status, reserved */
//#define   DOS_TASK_STATUS_WRITE       0x80    /** write status, reserved */


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
  dos_void                        *TaskSem;                 /** Task-held semaphore         */
  dos_void                        *TaskMux;                 /** Task-held mutex             */
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
                            
#endif

