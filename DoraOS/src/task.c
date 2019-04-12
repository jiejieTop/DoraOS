#include <dos_def.h>
#include <task.h>
#include "include.h"

#ifndef     DOS_MAX_PRIORITY_NUM
#define     DOS_MAX_PRIORITY_NUM        32U
#endif

#if DOS_MAX_PRIORITY_NUM > 32
#define   DOS_PRIORITY_TAB  (((DOS_MAX_PRIORITY_NUM -1 )/32) + 1)

#define   DOS_PRIORITY_TAB_INDEX(PRI)  (((PRI -1 )/32))

static dos_uint32 Dos_Task_Priority[DOS_PRIORITY_TAB];
#else
static dos_uint32 Dos_Task_Priority;
#endif

Dos_TaskList_t Dos_TaskPriority_List[DOS_MAX_PRIORITY_NUM];

Dos_TaskList_t Dos_TaskSleep_List;

DOS_TaskCB_t volatile Dos_CurrentTCB = DOS_NULL;

DOS_TaskCB_t volatile Dos_IdleTCB = DOS_NULL;
  
dos_uint32 Dos_TickCount = 0U;

const dos_uint8 Dos_BitMap[] =
{
  0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 00 */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 10 */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 20 */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 30 */
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 40 */ 
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 50 */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 60 */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 70 */
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 80 */ 
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* 90 */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* A0 */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* B0 */
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* C0 */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* D0 */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    /* E0 */ 
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0     /* F0 */
};

static void _Dos_Create_IdleTask(void);

dos_uint32 Dos_Get_Highest_Priority(dos_uint32 pri)
{
    if (pri == 0) return 0;

    if (pri & 0xff)
        return Dos_BitMap[pri & 0xff];

    if (pri & 0xff00)
        return Dos_BitMap[(pri & 0xff00) >> 8] + 8;

    if (pri & 0xff0000)
        return Dos_BitMap[(pri & 0xff0000) >> 16] + 16;

    return Dos_BitMap[(pri & 0xff000000) >> 24] + 24;
}



static void _Dos_TaskPriority_List_Init(void)
{
  dos_uint32 i;
#if DOS_MAX_PRIORITY_NUM > 32
  for(i = 0; i < DOS_PRIORITY_TAB; i++)
    Dos_Task_Priority[i] = 0;
#else
  Dos_Task_Priority = 0;
#endif
  for(i  =0; i < DOS_MAX_PRIORITY_NUM; i++)
  {
    Dos_TaskList_Init(&Dos_TaskPriority_List[i]); 
  }
}
static void _Dos_TaskSleep_List_Init(void)
{
  Dos_TaskList_Init(&Dos_TaskSleep_List);
}


static void _Dos_Task_List_Init(void)
{
  _Dos_TaskPriority_List_Init();
  
  _Dos_TaskSleep_List_Init();
}

static void _Dos_Inser_TaskPriority_List(DOS_TaskCB_t dos_taskcb)
{
  /* update priority  */
#if DOS_MAX_PRIORITY_NUM > 32
  Dos_Task_Priority[DOS_PRIORITY_TAB_INDEX(dos_taskcb->Priority)] |= (0x01 << (dos_taskcb->Priority % 32));
#else
  Dos_Task_Priority |= (0x01 << dos_taskcb->Priority);
  DOS_PRINT_DEBUG("Dos_Task_Priority = %#x",Dos_Task_Priority);
#endif
  /* init task list,the list will pend in readylist or pendlist  */
  Dos_TaskList_Init(&(dos_taskcb->StateList));
  /* inser priority list */
  Dos_DListInser(&Dos_TaskPriority_List[dos_taskcb->Priority].TaskDList,&dos_taskcb->StateList.TaskDList);
  Dos_TaskPriority_List[dos_taskcb->Priority].TCB_Addr = (dos_void*)dos_taskcb;
}

void _Dos_Inser_TaskSleep_List(DOS_TaskCB_t dos_taskcb)
{
  dos_uint8 i;
  
  const DOS_DList_t *dos_plist = &(dos_taskcb->StateList.TaskDList);
  while(dos_plist->Next != &(dos_taskcb->StateList.TaskDList))
  {
    dos_plist = dos_plist->Next;
    i++;
  }

  Dos_DListInser(&Dos_TaskSleep_List.TaskDList, &(dos_taskcb->StateList.TaskDList));
}


/**
 * @brief       NULL  
 * @param[in]   NULL
 * @param[out]  NULL
 * @return      NULL
 * @author      jiejie
 * @github      https://github.com/jiejieTop
 * @date        2018-xx-xx
 * @version     v1.0
 * @note        nados system init
 */
void Dos_SystemInit(void)
{
  /* system memheap init */
  Dos_MemHeap_Init();
  
  /* init task list */
  _Dos_Task_List_Init();

  _Dos_Create_IdleTask();

}


static void _Dos_InitTask(DOS_TaskCB_t dos_taskcb)
{
	/* 获取栈顶地址 */
	dos_taskcb->TopOfStack = (dos_void *)((dos_uint32)dos_taskcb->StackAddr + (dos_uint32)(dos_taskcb->StackSize - 1));
	
  /* 向下做8字节对齐 */
	dos_taskcb->TopOfStack = (dos_void *)((( uint32_t)dos_taskcb->TopOfStack) & (~((dos_uint32 )0x0007)));	
  
  dos_taskcb->StackPoint = Dos_StackInit( dos_taskcb->TopOfStack,
                                          dos_taskcb->TaskEntry,
                                          dos_taskcb->Parameter);
}


/**
 * This function will create a task
 * @param[in]  task       the task to be created
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  pri        the prio of task
 * @param[in]  ticks      the time slice if there are same prio task
 * @param[in]  stack_buf  the start address of task stack
 * @param[in]  stack      the size of thread stack
 * @param[in]  entry      the entry function of task
 * @param[in]  autorun    the autorunning flag of task
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
DOS_TaskCB_t Dos_TaskCreate(const dos_char *dos_name,
                            void (*dos_task_entry)(void *dos_param),
                            void * const dos_param,
                            dos_uint32 dos_stack_size,
                            dos_uint16 dos_priority)
{
  DOS_TaskCB_t dos_taskcb;
  dos_void *dos_stack;
  
  dos_taskcb = (DOS_TaskCB_t)Dos_MemAlloc(sizeof(struct DOS_TaskCB));
  if(dos_taskcb != DOS_NULL)
  {
    dos_stack = (dos_void *)Dos_MemAlloc(dos_stack_size);
    if(DOS_NULL == dos_stack)
    {
      DOS_PRINT_ERR("system mem DOS_NULL");
      /* 此处应释放申请到的内存 */
      return DOS_NULL;
    }
//    dos_taskcb->ReadyList.TCB_Addr = (dos_void *)dos_taskcb;
    dos_taskcb->StackAddr = dos_stack;
    dos_taskcb->StackSize = dos_stack_size;
  }

  dos_taskcb->TaskEntry = (void *)dos_task_entry;
  dos_taskcb->Parameter = dos_param;
  dos_taskcb->Priority = dos_priority;
  dos_taskcb->TaskName = (dos_char *)dos_name;

  _Dos_InitTask(dos_taskcb);       
  
  _Dos_Inser_TaskPriority_List(dos_taskcb);
  
  return dos_taskcb;
}


void Dos_TaskSleep(dos_uint32 dos_sleep_tick)
{
  if(Dos_IdleTCB == Dos_CurrentTCB)
  {
    DOS_PRINT_ERR("Idle tasks are not allowed to sleep!\n");
    return;
  }
  if(0 == dos_sleep_tick)
  {
    DOS_TASK_YIELD();
  }
  Dos_DListDel(&(Dos_CurrentTCB->StateList.TaskDList));

}



void _Dos_IdleTask(void *Parameter)
{
  while(1)
  {
    ;
  }
}

static void _Dos_Create_IdleTask(void)
{
 Dos_IdleTCB = Dos_TaskCreate( "IdleTask",
                                &_Dos_IdleTask,
                                DOS_NULL,
                                DOS_IDLE_TASK_SIZE,
                                DOS_IDLE_TASK_PRIORITY);
  if(DOS_NULL == Dos_IdleTCB)
  {
    DOS_PRINT_ERR("Dos_IdleTCB is NULL!\n");
  }
}
 

void Dos_Start( void )
{
  dos_uint32 pri;
  /* 手动指定第一个运行的任务 */
#if DOS_MAX_PRIORITY_NUM > 32
  dos_uint32 i;
  for(i = 0; i < DOS_PRIORITY_TAB; i++)
  {
    if(Dos_Task_Priority[i] & 0xFFFFFFFF)
      break;
  }
  pri = Dos_Get_Highest_Priority(Dos_Task_Priority[i]) + 32 * i;
#else
  pri = Dos_Get_Highest_Priority(Dos_Task_Priority);
#endif
  DOS_PRINT_DEBUG("PRI = %d \n",pri);
  
  Dos_CurrentTCB = (DOS_TaskCB_t)Dos_TaskPriority_List[pri].TCB_Addr;
  
  Dos_TickCount = 0U;
  /* 启动调度器 */
  if( Dos_StartScheduler() != 0 )
  {
      /* 调度器启动成功，则不会返回，即不会来到这里 */
  }
}

void Dos_SwitchTask( void )
{    
  dos_uint32 pri;
#if DOS_MAX_PRIORITY_NUM > 32
  dos_uint32 i;
  for(i = 0; i < DOS_PRIORITY_TAB; i++)
  {
    if(Dos_Task_Priority[i] & 0xFFFFFFFF)
      break;
  }
  pri = Dos_Get_Highest_Priority(Dos_Task_Priority[i]) + 32 * i;
#else
  pri = Dos_Get_Highest_Priority(Dos_Task_Priority);
#endif
  
  Dos_CurrentTCB = (DOS_TaskCB_t)Dos_TaskPriority_List[pri].TCB_Addr;
  
//    /* 两个任务轮流切换 */
//    if( Dos_CurrentTCB == task )
//    {
//        Dos_CurrentTCB = task1;
//    }
//    else
//    {
//        Dos_CurrentTCB = task;
//    }
  
  
  
}






