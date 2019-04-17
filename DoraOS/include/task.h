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
  dos_uint32                      TaskTick;                   /**< TaskTick                    */
  dos_uint32                      TaskInitTick;              /**< TaskInitTick                     */ 
  dos_void                        *TaskEntry;               /**< Task entrance function      */
  dos_void                        *TaskSem;                  /**< Task-held semaphore         */
  dos_void                        *TaskMux;                  /**< Task-held mutex             */
  dos_void                        *Parameter;                      /**< Parameter                   */
  dos_char                        *TaskName;                /**< Task name                   */
  Dos_TaskItem_t                  StateItem;
//  dos_void                        *TCB_Addr;
//  Dos_TaskList_t                  SleepList;
  Dos_TaskItem_t                  PendItem;
  //    LOS_DL_LIST                 stPendList;
//    LOS_DL_LIST                 stTimerList;
    dos_uint32                      uwIdxRollNum;
//    EVENT_CB_S                  uwEvent;
    dos_uint32                      uwEventMask;                /**< Event mask                  */
    dos_uint32                      uwEventMode;                /**< Event mode                  */
    dos_void                        *puwMsg;                    /**< Memory allocated to queues  */
};

typedef struct DOS_TaskCB * DOS_TaskCB_t;



#define DOS_GET_STRUCT(ptr, type, member)   ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
          
#define DOS_GET_TCB(node)    DOS_GET_STRUCT(node, struct DOS_TaskCB, StateItem)
  
//#define rt_container_of(ptr, type, member) \ 
//        ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
//          
//#define rt_list_entry(node, type, member) \ 
//        rt_container_of(node, type, member)
        
void Dos_SystemInit(void);

DOS_TaskCB_t Dos_TaskCreate(const dos_char *dos_name,
                            void (*dos_task_entry)(void *dos_param),
                            void * const dos_param,
                            dos_uint32 dos_stack_size,
                            dos_uint16 dos_priority);
                            
dos_uint32 Dos_Get_Tick(void);         
void Dos_Start( void );
void Dos_TaskSleep(dos_uint32 dos_sleep_tick);
                            
                            
#endif

