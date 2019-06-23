#include <dos_def.h>
#include <task.h>
#include "include.h"

#if DOS_MAX_PRIORITY_NUM > 32
#define   DOS_PRIORITY_TAB  (((DOS_MAX_PRIORITY_NUM -1 )/32) + 1)
#define   DOS_PRIORITY_TAB_INDEX(PRI)  (((PRI -1 )/32))

static dos_uint32 Dos_Task_Priority[DOS_PRIORITY_TAB];
#else
static dos_uint32 Dos_Task_Priority;
#endif

/**
 * task priority list
 */
Dos_TaskList_t Dos_TaskPriority_List[DOS_MAX_PRIORITY_NUM];

/**
 * Time dependent list
 */
static Dos_TaskList_t _Dos_Sleep_List1;
static Dos_TaskList_t _Dos_Sleep_List2;
static Dos_TaskList_t *_Dos_TaskSleep_List;
static Dos_TaskList_t *_Dos_TaskSleep_OverFlow_List;

static dos_uint32 Dos_NextWake_Tick = DOS_UINT32_MAX;

/**
 * task TCB
 */
DOS_TaskCB_t volatile Dos_CurrentTCB = DOS_NULL;
DOS_TaskCB_t volatile Dos_IdleTCB = DOS_NULL;

/**
 * global variable
 */
dos_uint8 Dos_IsRun = DOS_NO;       /** system run flag */
dos_uint32 Dos_SchedulerLock = 0;   /** scheduler lock number of times*/
dos_uint32 Dos_TickCount = 0U;      /** system time (tick) */
dos_uint32 Dos_CurPriority = 0;     /** the highest priority the system is currently in the ready state */

/**
 * bit map
 */
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


/**
 * Function declaration
 */
static void _Dos_Create_IdleTask(void);
static dos_bool _Dos_Cheek_TaskPriority(void);

/** 
 * get Highest Priority by bit map
 */
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

/**
 * task priority list initialization
 */
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

/**
 * task sleep list initialization
 */
static void _Dos_TaskSleep_List_Init(void)
{
  _Dos_TaskSleep_List = &_Dos_Sleep_List1;
  _Dos_TaskSleep_OverFlow_List = &_Dos_Sleep_List2;
  
  Dos_TaskList_Init(_Dos_TaskSleep_List);
  Dos_TaskList_Init(_Dos_TaskSleep_OverFlow_List);
}

/**
 * task priority/sleep list initialization
 */
static void _Dos_Task_List_Init(void)
{
  _Dos_TaskPriority_List_Init();
  
  _Dos_TaskSleep_List_Init();
}


/**
 * insert 
 */
static void _Dos_insert_TaskPriority_List(DOS_TaskCB_t dos_taskcb)
{
  /* update priority  */
#if DOS_MAX_PRIORITY_NUM > 32
  Dos_Task_Priority[DOS_PRIORITY_TAB_INDEX(dos_taskcb->Priority)] |= (0x01 << (dos_taskcb->Priority % 32));
#else
  Dos_Task_Priority |= (0x01 << dos_taskcb->Priority);
  DOS_PRINT_DEBUG("Dos_Task_Priority = %#x",Dos_Task_Priority);
#endif
  /* init task list,the list will pend in readylist or pendlist  */
  /* insert priority list */
  Dos_TaskItem_insert(&Dos_TaskPriority_List[dos_taskcb->Priority],&dos_taskcb->StateItem);
}

static void _Dos_insert_TaskSleep_List(dos_uint32 dos_sleep_tick)
{
  DOS_TaskCB_t cur_task = Dos_CurrentTCB;

  if(Dos_IdleTCB == cur_task)
  {
    DOS_PRINT_ERR("Idle tasks are not allowed to sleep!\n");
    return;
  }
  
  if(Dos_TaskItem_Del(&(cur_task->StateItem)) == 0)
  {
    if(Dos_TaskList_IsEmpty(&Dos_TaskPriority_List[cur_task->Priority]) == DOS_TRUE)
    {
      Dos_Task_Priority &= ~(0x01 << cur_task->Priority); 
      
      INT_CTRL_REG = PENDSVSET_BIT;
    }
  }
  
  cur_task->StateItem.Dos_TaskValue = Dos_TickCount + dos_sleep_tick;
  
  if(cur_task->StateItem.Dos_TaskValue < Dos_TickCount)   //overflow
  {
    Dos_TaskItem_insert(_Dos_TaskSleep_OverFlow_List, &(cur_task->StateItem));
  }
  else
  {
    Dos_TaskItem_insert(_Dos_TaskSleep_List, &(cur_task->StateItem));
    if(Dos_NextWake_Tick >= cur_task->StateItem.Dos_TaskValue)
    {
      Dos_NextWake_Tick = cur_task->StateItem.Dos_TaskValue;
    } 
  }
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
      DOS_PRINT_DEBUG("system mem DOS_NULL");
      /* 此处应释放申请到的内存 */
      return DOS_NULL;
    }
    Dos_TaskItem_Init(&dos_taskcb->StateItem);
    dos_taskcb->StateItem.Dos_TCB = (dos_void *)dos_taskcb;
    dos_taskcb->StackAddr = dos_stack;
    dos_taskcb->StackSize = dos_stack_size;
  }
  else
  {
    return DOS_NULL;
  }
  

  dos_taskcb->TaskEntry = (void *)dos_task_entry;
  dos_taskcb->Parameter = dos_param;
  dos_taskcb->Priority = dos_priority;
  dos_taskcb->TaskName = (dos_char *)dos_name;

  
  
  _Dos_InitTask(dos_taskcb);       
  
  _Dos_insert_TaskPriority_List(dos_taskcb);
  
  return dos_taskcb;
}


void Dos_TaskSleep(dos_uint32 dos_sleep_tick)
{
  dos_uint32 pri;

  if(0 == dos_sleep_tick)
  {
    DOS_TASK_YIELD();
  }
  pri = Dos_Interrupt_Disable();

  _Dos_insert_TaskSleep_List(dos_sleep_tick);

  Dos_Scheduler();

  Dos_Interrupt_Enable(pri);
}

dos_uint32 Dos_Get_Tick(void)
{
  dos_uint32 dos_cur_tick;
  dos_uint32 pri;

  pri = Dos_Interrupt_Disable();
  
  dos_cur_tick = Dos_TickCount;
  
  Dos_Interrupt_Enable(pri);
  
  return dos_cur_tick;
}

static void _Dos_IdleTask(void *Parameter)
{
  while(1)
  {
    ;
  }
}


void Dos_TaskExit(void)
{
  DOS_ASSERT(0);
  while(1);
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
 

static dos_bool _Dos_Cheek_TaskPriority(void)
{
#if DOS_MAX_PRIORITY_NUM > 32
  dos_uint32 i;
  for(i = 0; i < DOS_PRIORITY_TAB; i++)
  {
    if(Dos_Task_Priority[i] & 0xFFFFFFFF)
      break;
  }
  Dos_CurPriority = Dos_Get_Highest_Priority(Dos_Task_Priority[i]) + 32 * i;
#else
  Dos_CurPriority = Dos_Get_Highest_Priority(Dos_Task_Priority);
#endif
  
  if(Dos_CurPriority <= Dos_CurrentTCB->Priority)
    return DOS_TRUE;
  else
    return DOS_FALSE;
}


DOS_TaskCB_t Dos_GetTCB(Dos_TaskList_t *list)
{
  list->Dos_TaskItem = list->Dos_TaskItem->Next;
  if((void*)(list)->Dos_TaskItem == (void*)&((list)->Task_EndItem))
  {
    list->Dos_TaskItem = list->Dos_TaskItem->Next;
  }
  return list->Dos_TaskItem->Dos_TCB;
}

dos_bool Dos_CheekTaskTick(Dos_TaskList_t *list)
{
  DOS_TaskCB_t taskcb = (DOS_TaskCB_t)&(list->Dos_TaskItem->Dos_TCB);
  
  if(taskcb->TaskTick >= Dos_TickCount)   //时间片到了
  {
    taskcb->TaskTick += taskcb->TaskInitTick; 
    return DOS_TRUE;
  }
  else
  {
    taskcb->TaskTick++;
    if(taskcb->TaskTick == 0)   //溢出处理
    {
      
    }
  }
  return DOS_FALSE;
}

dos_bool _Dos_Scheduler(void)
{
  dos_bool dos_res = DOS_FALSE;
  
  if((_Dos_Cheek_TaskPriority() != dos_res) && (DOS_YES == Dos_IsRun))
  {
    return DOS_TRUE;
  }
  return DOS_FALSE;
}

void Dos_Scheduler(void)
{
  if(_Dos_Scheduler() == DOS_TRUE)
  {
    INT_CTRL_REG = PENDSVSET_BIT;   //如果当前优先级列表下有任务并且时间片到达了，或者有更高优先级的任务就绪了，那么需要切换任务
  }
}

void Dos_Start( void )
{
  
  _Dos_Cheek_TaskPriority();
  
  Dos_CurrentTCB = Dos_GetTCB(&Dos_TaskPriority_List[Dos_CurPriority]);

  DOS_PRINT_DEBUG("TaskPriority = %d",Dos_CurPriority);
  
  Dos_TickCount = 0U;
  Dos_IsRun = DOS_YES;
  /* 启动调度器 */
  if( Dos_StartScheduler() != 0 )
  {
      /* 调度器启动成功，则不会返回，即不会来到这里 */
  }
}

static void _Dos_Switch_SleepList(void)
{
  Dos_TaskList_t *dos_list;
  DOS_TaskCB_t dos_task;
  if(Dos_TaskList_IsEmpty(_Dos_TaskSleep_List) == DOS_TRUE)
  {
    dos_list = _Dos_TaskSleep_List;
    _Dos_TaskSleep_List = _Dos_TaskSleep_OverFlow_List;
    _Dos_TaskSleep_OverFlow_List = dos_list;
    if(Dos_TaskList_IsEmpty(_Dos_TaskSleep_List) == DOS_TRUE)
    {
      Dos_NextWake_Tick = DOS_UINT32_MAX;
    }
    else
    {
      dos_task = Dos_GetTCB(_Dos_TaskSleep_List);
      Dos_NextWake_Tick = dos_task->StateItem.Dos_TaskValue;
    }
  }
  else
    DOS_PRINT_ERR("Task sleep list is not empty!\n");
}

void Dos_SwitchTask( void )
{    
  _Dos_Cheek_TaskPriority();
  Dos_CurrentTCB = Dos_GetTCB(&Dos_TaskPriority_List[Dos_CurPriority]);
}


/**
 * Scheduler lock
 */
void Dos_Scheduler_Lock(void)
{
  dos_uint32 pri;

  pri = Dos_Interrupt_Disable();

  Dos_SchedulerLock ++ ;

  Dos_Interrupt_Enable(pri);
}

/**
 * Scheduler unlock
 */
void Dos_Scheduler_Unlock(void)
{
  dos_uint32 pri;

  pri = Dos_Interrupt_Disable();

  if(Dos_SchedulerLock > 0)
  {
    Dos_SchedulerLock -- ;
    if(0 == Dos_SchedulerLock)
    {
       Dos_Interrupt_Enable(pri);
       Dos_Scheduler();
       return;
    }
  }

  Dos_Interrupt_Enable(pri);
}




void Dos_Updata_Tick(void)
{
  DOS_TaskCB_t dos_task;
  dos_uint32 dos_tick;
  
  Dos_TickCount++;
  
  if(Dos_TickCount ==0)   //overflow
  {
    _Dos_Switch_SleepList();
  }
  
  if(Dos_TickCount >= Dos_NextWake_Tick)
  {
    for(;;)
    {
      if(Dos_TaskList_IsEmpty(_Dos_TaskSleep_List) == DOS_TRUE)
      {
        Dos_NextWake_Tick = DOS_UINT32_MAX;
        break;
      }
      else
      {
        dos_task = Dos_GetTCB(_Dos_TaskSleep_List);
        dos_tick = dos_task->StateItem.Dos_TaskValue;
        if(dos_tick > Dos_NextWake_Tick)
        {
          Dos_NextWake_Tick = dos_tick;
          break;
        }
        
        Dos_TaskItem_Del(&dos_task->StateItem);

        Dos_TaskItem_insert(&Dos_TaskPriority_List[dos_task->Priority], &dos_task->StateItem);
        Dos_Task_Priority |= (0x01 << dos_task->Priority);
        
        if(dos_task->Priority < Dos_CurrentTCB->Priority)
        {
          INT_CTRL_REG = PENDSVSET_BIT;
        }
      }
    } 
  }
}

void SysTick_Handler(void)
{
  dos_uint32 pri; 
  pri = Interrupt_Disable();
  
  Dos_Updata_Tick();
  
  Dos_Scheduler();
  
  Interrupt_Enable(pri);
}

