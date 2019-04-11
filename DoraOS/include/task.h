#ifndef _TASK_H_
#define _TASK_H_

#include "dos_def.h"
#include <list.h>
#include <mem.h>
/* debug */
#include <debug.h>
#include <port.h>
#include <dos_config.h>


     
struct DOS_TaskCB
{
  dos_void                        *StackPoint;             /**< Task stack point          */
  dos_void                        *StackAddr;             /**< Task stack point          */
  dos_uint16                      TaskStatus;
  dos_uint16                      Priority;
  dos_uint32                      StackSize;                /**< Task stack size             */
  dos_void                        *TopOfStack;               /**< Task stack top              */
  dos_uint32                      uwTaskID;                   /**< Task ID                     */
  dos_void                        *TaskEntry;               /**< Task entrance function      */
  dos_void                        *TaskSem;                  /**< Task-held semaphore         */
  dos_void                        *TaskMux;                  /**< Task-held mutex             */
  dos_void                        *Parameter;                      /**< Parameter                   */
  dos_char                        *TaskName;                /**< Task name                   */
  Dos_TaskList_t                  ReadyList;
//  dos_void                        *TCB_Addr;

  Dos_TaskList_t                  PendList;
  //    LOS_DL_LIST                 stPendList;
//    LOS_DL_LIST                 stTimerList;
    dos_uint32                      uwIdxRollNum;
//    EVENT_CB_S                  uwEvent;
    dos_uint32                      uwEventMask;                /**< Event mask                  */
    dos_uint32                      uwEventMode;                /**< Event mode                  */
    dos_void                        *puwMsg;                    /**< Memory allocated to queues  */
};

typedef struct DOS_TaskCB * DOS_TaskCB_t;


void Dos_SystemInit(void);

DOS_TaskCB_t Dos_TaskCreate(const dos_char *dos_name,
                            void (*dos_task_entry)(void *dos_param),
                            void * const dos_param,
                            dos_uint32 dos_stack_size,
                            dos_uint16 dos_priority);
                            
void Dos_Start( void );
                            
#endif

