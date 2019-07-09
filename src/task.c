#include <task.h>


#if DOS_MAX_PRIORITY_NUM > 32U
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
 * Task initialization
 */
static void _Dos_InitTask(DOS_TaskCB_t dos_taskcb)
{
	/** Get the top address of the task stack */
	dos_taskcb->TopOfStack = (dos_void *)((dos_uint32)dos_taskcb->StackAddr + (dos_uint32)(dos_taskcb->StackSize - 1));
	
  /** 8-byte alignment */
	dos_taskcb->TopOfStack = (dos_void *)((( uint32_t)dos_taskcb->TopOfStack) & (~((dos_uint32 )0x0007)));	
  
  /** Task stack initialization */
  dos_taskcb->StackPoint = Dos_StackInit( dos_taskcb->TopOfStack,
                                          dos_taskcb->TaskEntry,
                                          dos_taskcb->Parameter);
}


/** 
 * get Highest Priority by bit map
 */
static dos_uint32 _Dos_Get_Highest_Priority(dos_uint32 pri)
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
 * insert 
 */
static void _Dos_insert_TaskPriority_List(DOS_TaskCB_t dos_taskcb)
{
  /* update priority  */
  DOS_SET_TASK_PTIORITY(dos_taskcb);

  DOS_SET_TASK_STATUS(dos_taskcb, DOS_TASK_STATUS_READY);

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
  
  /** Change task status */
  DOS_RESET_TASK_STATUS(cur_task, DOS_TASK_STATUS_READY);
  DOS_SET_TASK_STATUS(cur_task, DOS_TASK_STATUS_DELAY);

  /** Remove a task from the corresponding status list */
  if(Dos_TaskItem_Del(&(cur_task->StateItem)) == 0)
  {
    /** If there are no more tasks under the current task priority, the bit corresponding to Dos_Task_Priority will be canceled. */
    if(Dos_TaskList_IsEmpty(&Dos_TaskPriority_List[cur_task->Priority]) == DOS_TRUE)
    {
      DOS_RESET_TASK_PTIORITY(cur_task);
      // Dos_Task_Priority &= ~(0x01 << cur_task->Priority); 
//      Dos_Scheduler();
    }
  }
  /** Calculate the time of wake up */
  cur_task->StateItem.Dos_TaskValue = Dos_TickCount + dos_sleep_tick;
  
  /**If the time overflows, insert the _Dos_TaskSleep_OverFlow_List list */
  if(cur_task->StateItem.Dos_TaskValue < Dos_TickCount)   //time overflow
  {
    Dos_TaskItem_insert(_Dos_TaskSleep_OverFlow_List, &(cur_task->StateItem));
  }
  else
  {
    /** Time does not overflow, insert _Dos_TaskSleep_List list */
    Dos_TaskItem_insert(_Dos_TaskSleep_List, &(cur_task->StateItem)); //time no overflow
    if(Dos_NextWake_Tick >= cur_task->StateItem.Dos_TaskValue)
    {
      /**  */
      Dos_NextWake_Tick = cur_task->StateItem.Dos_TaskValue;
    } 
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
  Dos_CurPriority = _Dos_Get_Highest_Priority(Dos_Task_Priority[i]) + 32 * i;
#else
  Dos_CurPriority = _Dos_Get_Highest_Priority(Dos_Task_Priority); /** get current task proority */
#endif
  
  /** Task scheduling when there are higher priority tasks, except for idle tasks. or the current task is not in the ready state */
  if((Dos_CurPriority < Dos_CurrentTCB->Priority) || (!(Dos_CurrentTCB->TaskStatus & DOS_TASK_STATUS_READY)))
    return DOS_TRUE;
  
  /** there are more tasks in the current task priority list */
  else if(Dos_Get_TaskListValue(&Dos_TaskPriority_List[Dos_CurrentTCB->Priority]) > 1)
    return DOS_TRUE;

  /** No task schedul */
  else
    return DOS_FALSE;
}

/**
 * idle task function
 */
static void _Dos_IdleTask(void *Parameter)
{
  while(1)
  {
    ;
  }
}

/** 
 * create idle task
 */
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

/**
 * switch sleep list
 * switch _Dos_TaskSleep_OverFlow_List when time overflows. 
 * Otherwise choose _Dos_TaskSleep_List
 */
static void _Dos_Switch_SleepList(void)
{
  Dos_TaskList_t *dos_list;
  DOS_TaskCB_t dos_task;

  /** When switching _Dos_TaskSleep_OverFlow_List, judge whether the current list is empty. */
  if(Dos_TaskList_IsEmpty(_Dos_TaskSleep_List) == DOS_TRUE)
  {
    /** switch sleep list */
    dos_list = _Dos_TaskSleep_List;
    _Dos_TaskSleep_List = _Dos_TaskSleep_OverFlow_List;
    _Dos_TaskSleep_OverFlow_List = dos_list;

    /** Whether the list is empty after switching sleep list  */
    if(Dos_TaskList_IsEmpty(_Dos_TaskSleep_List) == DOS_TRUE)
    {
      /** No wake up time required */
      Dos_NextWake_Tick = DOS_UINT32_MAX;
    }
    else
    {
      /** Get the first task of the sleep list, the task value is next wake time */
      dos_task = Dos_GetTCB(_Dos_TaskSleep_List);
      Dos_NextWake_Tick = dos_task->StateItem.Dos_TaskValue;
    }
  }
  else
    DOS_PRINT_ERR("Task sleep list is not empty!\n");
}

/**
 * task scheduler(Internally used function)
 */
static dos_bool _Dos_Scheduler(void)
{
  if((_Dos_Cheek_TaskPriority() != DOS_FALSE) && (DOS_YES == Dos_IsRun))
  {
    return DOS_TRUE;
  }
  return DOS_FALSE;
}

/***********************************************************************************************************************/

/**
 * system init
 */
void Dos_SystemInit(void)
{
  /* system memheap init */
  Dos_MemHeap_Init();
  
  /* init task list */
  _Dos_Task_List_Init();

  /** create idle task */
  _Dos_Create_IdleTask();

}

/**
 * This function will create a task
 * @param[in]  dos_name         the name of task, which shall be unique
 * @param[in]  dos_task_entry   the parameter of task enter function
 * @param[in]  dos_param        the param of task enter 
 * @param[in]  dos_stack_size   the start address of task stack
 * @param[in]  dos_priority     the time slice if there are same prio task
 * @return     Task control block pointer
 */
DOS_TaskCB_t Dos_TaskCreate(const dos_char *dos_name,
                            void (*dos_task_entry)(void *dos_param),
                            void * const dos_param,
                            dos_uint32 dos_stack_size,
                            dos_uint16 dos_priority)
{
  DOS_TaskCB_t dos_taskcb;
  dos_void *dos_stack;
  
  DOS_ASSERT(dos_priority <= DOS_MAX_PRIORITY_NUM);
  
  /** alloc for task control block memory  */
  dos_taskcb = (DOS_TaskCB_t)Dos_MemAlloc(sizeof(struct DOS_TaskCB));
  if(DOS_NULL != dos_taskcb)
  {
    /** alloc for task stack memory  */
    dos_stack = (dos_void *)Dos_MemAlloc(dos_stack_size);
    if(DOS_NULL == dos_stack)
    {
      DOS_PRINT_DEBUG("system mem DOS_NULL");

      /** Failed to alloc memory */
      Dos_MemFree(dos_taskcb);  
      return DOS_NULL;
    }

    /** Initialize list item and task control block information */
    Dos_TaskItem_Init(&dos_taskcb->StateItem);
    Dos_TaskItem_Init(&dos_taskcb->PendItem);
    dos_taskcb->StateItem.Dos_TCB = (dos_void *)dos_taskcb;
    dos_taskcb->PendItem.Dos_TCB = (dos_void *)dos_taskcb;
    dos_taskcb->StackAddr = dos_stack;
    dos_taskcb->StackSize = dos_stack_size;
  }
  else
  {
    return DOS_NULL;
  }
  
  /** Initialize task control block information */
  dos_taskcb->TaskEntry = (void *)dos_task_entry;
  dos_taskcb->Parameter = dos_param;
  dos_taskcb->Priority = dos_priority;
  dos_taskcb->TaskName = (dos_char *)dos_name;
  
  /** Task initialization */
  _Dos_InitTask(dos_taskcb);       
  
  /** Insert tasks into the ready list to allow task scheduling */
  _Dos_insert_TaskPriority_List(dos_taskcb);
  
  return dos_taskcb;
}


/**
 * delete task function
 * @param[in]  dos_task         Task control block pointer
 */
dos_err Dos_TaskDelete(DOS_TaskCB_t dos_task)
{
  dos_uint16 dos_status;
  dos_uint32 pri;

  if(DOS_NULL == dos_task)
  {
    DOS_PRINT_ERR("delete task is null\n");
    return DOS_NOK;
  }
  
  dos_status = dos_task->TaskStatus;

  if(DOS_TASK_STATUS_UNUSED == dos_status)  /** task is unused status */
  {
    DOS_PRINT_ERR("task status is unused\n");
    return DOS_NOK;
  }

  if(Dos_SchedulerLock != 0)
  {
    DOS_PRINT_ERR("scheduler is lock\n");
    return DOS_NOK;
  }

  /** task is used status */
  if((DOS_TASK_STATUS_DELAY & dos_status) || (DOS_TASK_STATUS_READY & dos_status))  
  {
    pri = Dos_Interrupt_Disable();

    /**Remove a task from the corresponding status list */
    if(Dos_TaskItem_Del(&(dos_task->StateItem)) == 0)
    {
      /** If there are no more tasks under the current task priority, the bit corresponding to Dos_Task_Priority will be canceled. */
      if(Dos_TaskList_IsEmpty(&Dos_TaskPriority_List[dos_task->Priority]) == DOS_TRUE)
      {
        DOS_RESET_TASK_PTIORITY(dos_task);
        // Dos_Task_Priority &= ~(0x01 << dos_task->Priority); 
        DOS_TASK_YIELD();
//        Dos_Scheduler();
      }
    }
    Dos_Interrupt_Enable(pri);
  }
  
  /** If the task is in a suspended state, remove it from the pending list */
  if(DOS_TASK_STATUS_SUSPEND & dos_status)
  {
    Dos_TaskItem_Del(&(dos_task->PendItem));
  }
  
  /** set task status is unused */
  DOS_RESET_TASK_STATUS(dos_task, DOS_TASK_STATUS_MASK);
  DOS_SET_TASK_STATUS(dos_task, DOS_TASK_STATUS_UNUSED);

  if(dos_task != Dos_CurrentTCB)
  {
    /**If the deleted task is not the current task, then release the task control block and the task stack */
    Dos_MemFree(dos_task);
    Dos_MemFree(dos_task->StackAddr);
    return DOS_OK;
  }
  else
  {
    /* Insert recycle list, Reserved not implemented*/

    return DOS_OK;
  }

}

/**
 * task sleep (tick)
 */
void Dos_TaskSleep(dos_uint32 dos_sleep_tick)
{
  dos_uint32 pri;

  if(0 == dos_sleep_tick)
  {
    Dos_Scheduler();
    return ;
  }

  pri = Dos_Interrupt_Disable();

  /** insert Task Sleep List */
  _Dos_insert_TaskSleep_List(dos_sleep_tick);

  /** After the task sleeps, perform a task scheduling */
  Dos_Scheduler();

  Dos_Interrupt_Enable(pri);
}


/**
 * get current task control block 
 */
DOS_TaskCB_t Dos_Get_CurrentTCB(void)
{
  return Dos_CurrentTCB;
}


/**
 * get the first task control block from the list
 */
DOS_TaskCB_t Dos_GetTCB(Dos_TaskList_t *list)
{
  list->Dos_TaskItem = list->Task_EndItem.Next;
  if((void*)(list)->Dos_TaskItem == (void*)&((list)->Task_EndItem))
  {
    list->Dos_TaskItem = list->Dos_TaskItem->Next;
  }
  return list->Dos_TaskItem->Dos_TCB;
}


/**
 * get the task control block from the list, each time the function is called, the task item will move backwards once. 
 */
DOS_TaskCB_t Dos_Get_NextTCB(Dos_TaskList_t *list)
{
  /** Support time slice rotation scheduling. Each time the function is called, the task item will move backwards once. */
  list->Dos_TaskItem = list->Dos_TaskItem->Next;    

  if((void*)(list)->Dos_TaskItem == (void*)&((list)->Task_EndItem))
  {
    list->Dos_TaskItem = list->Dos_TaskItem->Next;
  }
  return list->Dos_TaskItem->Dos_TCB;
}


/**
 * get system tick
 */
dos_uint32 Dos_Get_Tick(void)
{
  dos_uint32 dos_cur_tick;
  dos_uint32 pri;

  pri = Dos_Interrupt_Disable();
  
  dos_cur_tick = Dos_TickCount;
  
  Dos_Interrupt_Enable(pri);
  
  return dos_cur_tick;
}

/**
 * Tasks are waiting for message queues, events, semaphores, mutex locks etc.
 * set task status is suspend, insert pend list and sleep list.
 */
dos_void Dos_TaskWait(Dos_TaskList_t *dos_list, dos_uint32 timeout)
{
  DOS_TaskCB_t task = Dos_CurrentTCB;
  dos_uint32 pri;

  /** set task status is suspend, insert pend list */
  DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_SUSPEND);
  Dos_TaskItem_insert(dos_list, &task->PendItem);

  if(timeout != DOS_WAIT_FOREVER)
  {
    pri = Dos_Interrupt_Disable();

    /** Insert a task into the sleep list based on the wait timeout */
    _Dos_insert_TaskSleep_List(timeout);

    /** After the task sleeps, perform a task scheduling */
    Dos_Scheduler();

    Dos_Interrupt_Enable(pri);
  }
  else
  {
    /** Permanent waiting */
    DOS_RESET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
    
    /** Remove a task from the corresponding status list */
    if(Dos_TaskItem_Del(&(task->StateItem)) == 0)
    {
      /** If there are no more tasks under the current task priority, the bit corresponding to Dos_Task_Priority will be canceled. */
      if(Dos_TaskList_IsEmpty(&Dos_TaskPriority_List[task->Priority]) == DOS_TRUE)
      {
        DOS_RESET_TASK_PTIORITY(task);
        // Dos_Task_Priority &= ~(0x01 << task->Priority); 
        Dos_Scheduler();
      }
    }
  }
  
}

/**
 * wake task from pend list and sleep list.
 * and insert task priority list, task can be scheduled
 */
dos_void Dos_TaskWake(DOS_TaskCB_t task)
{
  dos_uint32 pri;
  pri = Dos_Interrupt_Disable();

  Dos_TaskItem_Del(&(task->PendItem));
  Dos_TaskItem_Del(&(task->StateItem));
  
  DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_SUSPEND | DOS_TASK_STATUS_DELAY));

  /** insert task priority list, and set the priority position corresponding to the Dos_Task_Priority variable is 1*/
  _Dos_insert_TaskPriority_List(task);

  Dos_Scheduler();

  Dos_Interrupt_Enable(pri);
}


/**
 * task exit function
 */
void Dos_TaskExit(void)
{
  DOS_ASSERT(0);
  while(1);
}



/**
 * system scheduler
 */
void Dos_Scheduler(void)
{
  if(_Dos_Scheduler() == DOS_TRUE)
  {
    DOS_TASK_YIELD(); 
  }
}

/**
 * start run system
 */
void Dos_Start( void )
{
  
  _Dos_Cheek_TaskPriority();
  
  Dos_CurrentTCB = Dos_GetTCB(&Dos_TaskPriority_List[Dos_CurPriority]);

  DOS_PRINT_DEBUG("TaskPriority = %d\n",Dos_CurPriority);
  
  /** Update Dos_TickCount and Dos_IsRun to indicate that the system starts to boot */
  Dos_TickCount = 0U;
  Dos_IsRun = DOS_YES;

  if( Dos_StartScheduler() != 0 )
  {
      /* Will not be executed here after the task is started */
  }
}


/**
 * Choose the right task to run
 */
void Dos_SwitchTask( void )
{    
//  _Dos_Cheek_TaskPriority();

  /** Get the control block for the highest priority task  */
  Dos_CurrentTCB = Dos_Get_NextTCB(&Dos_TaskPriority_List[Dos_CurPriority]);
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

    /** When the scheduler is completely unlocked */
    if(0 == Dos_SchedulerLock)
    {
       Dos_Interrupt_Enable(pri);

       /** Perform a task scheduling */
       Dos_Scheduler();
       return;
    }
  }

  Dos_Interrupt_Enable(pri);
}

/**
 * Determine if the system scheduler is locked
 */
dos_bool Dos_Scheduler_IsLock(void)
{
  return (0 != Dos_SchedulerLock);
}

/**
 * update system tick
 */
void Dos_Update_Tick(void)
{
  DOS_TaskCB_t dos_task;
  dos_uint32 dos_tick;
  
  Dos_TickCount++;
  
  if(Dos_TickCount ==0)   //overflow
  {
    /** When time overflows, switch list */
    _Dos_Switch_SleepList();
  }
  
  /** When a timeout event occurs, such as sleep timeout, waiting for message queue, semaphore, mutex, event timeout ect */
  if(Dos_TickCount >= Dos_NextWake_Tick)
  {
    /** Handle all timeout events in the loop, ensuring that each timeout is handled */
    for(;;)
    {
      /** When the timeout list is empty, the task wakeup time is not set. */
      if(Dos_TaskList_IsEmpty(_Dos_TaskSleep_List) == DOS_TRUE)
      {
        Dos_NextWake_Tick = DOS_UINT32_MAX;
        break;
      }
      else
      {
        /** get task contorl block and set task wake time */
        dos_task = Dos_GetTCB(_Dos_TaskSleep_List);
        dos_tick = dos_task->StateItem.Dos_TaskValue;
        if(dos_tick > Dos_NextWake_Tick)
        {
          Dos_NextWake_Tick = dos_tick;
          break;
        }
        
        /** Timeout has occurred, remove the task from the sleep list, and set the task status to (~DOS_TASK_STATUS_DELAY) */
        Dos_TaskItem_Del(&dos_task->StateItem);
        DOS_RESET_TASK_STATUS(dos_task, DOS_TASK_STATUS_DELAY);

        if(dos_task->TaskStatus & DOS_TASK_STATUS_SUSPEND)
        {
          /** If the task status is waiting for a message queue, semaphore, mutex, event, etc.
           *  when the wait has timed out, set its status to DOS_TASK_STATUS_TIMEOUT */
          DOS_SET_TASK_STATUS(dos_task, DOS_TASK_STATUS_TIMEOUT);
        }

        /** Insert the task into the ready list and set the task status to aaa */
        _Dos_insert_TaskPriority_List(dos_task);
        // dos_task->TaskStatus |= DOS_TASK_STATUS_READY;
        // Dos_TaskItem_insert(&Dos_TaskPriority_List[dos_task->Priority], &dos_task->StateItem);
        // Dos_Task_Priority |= (0x01 << dos_task->Priority);
        
        /** Determine the priority of the task and decide whether task scheduling is required. */
        if(dos_task->Priority < Dos_CurrentTCB->Priority)
        {
          DOS_TASK_YIELD();
        }
      }
    } 
  }
}


/**
 * system tick handler
 */
void SysTick_Handler(void)
{
  dos_uint32 pri; 
  pri = Interrupt_Disable();
  
  /** update system tick */
  Dos_Update_Tick();

  Dos_Scheduler();
  
  Interrupt_Enable(pri);
}


/*************************************************************************************************************************/

/**
 * Check if the tick of the task has arrived, Reserved!
 */
dos_bool Dos_CheekTaskTick(Dos_TaskList_t *list)
{
  DOS_TaskCB_t taskcb = (DOS_TaskCB_t)&(list->Dos_TaskItem->Dos_TCB);
  
  if(taskcb->TaskTick >= Dos_TickCount)   
  {
    taskcb->TaskTick += taskcb->TaskInitTick; 
    return DOS_TRUE;
  }
  else
  {
    taskcb->TaskTick++;
    if(taskcb->TaskTick == 0)   
    {
      
    }
  }
  return DOS_FALSE;
}



