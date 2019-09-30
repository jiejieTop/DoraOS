#include <task.h>
#include <log.h>

#if DOS_MAX_PRIORITY_NUM > 32U
static dos_uint32 dos_task_priority[DOS_PRIORITY_TAB];
#else
static dos_uint32 dos_task_priority;
#endif

/**
 * task priority list
 */
dos_task_list_t dos_task_priority_list[DOS_MAX_PRIORITY_NUM];

/**
 * Time dependent list
 */
static dos_task_list_t _dos_sleep_list1;
static dos_task_list_t _dos_sleep_list2;
static dos_task_list_t *_dos_task_sleep_list;
static dos_task_list_t *_dos_task_sleep_overflow_list;

static dos_task_list_t _dos_recycle_list;

static dos_uint32 dos_next_wake_tick = DOS_UINT32_MAX;

/**
 * task TCB
 */
dos_task_t volatile dos_current_task = DOS_NULL;
static dos_task_t volatile dos_idle_task = DOS_NULL;

/**
 * global variable
 */
static dos_uint32 dos_tick_count = 0U;      /** system time (tick) */
static dos_uint32 dos_cur_priority = 0;     /** the highest priority the system is currently in the ready state */

#if !DOS_HW_CLZ_SUPT
/**
 * bit map
 */
static const dos_uint8 dos_bit_map[] =
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
#endif

/**
 * Function declaration
 */
static dos_void _dos_create_idle_task(dos_void);

/**
 * task priority list initialization
 */
static dos_void _dos_task_priority_list_init(dos_void)
{
    dos_uint32 i;
#if DOS_MAX_PRIORITY_NUM > 32
    for(i = 0; i < DOS_PRIORITY_TAB; i++)
        dos_task_priority[i] = 0;
#else
    dos_task_priority = 0;
#endif
    for(i  =0; i < DOS_MAX_PRIORITY_NUM; i++)
    {
        dos_task_list_init(&dos_task_priority_list[i]); 
    }
}

/**
 * task sleep list initialization
 */
static dos_void _dos_task_sleep_list_init(dos_void)
{
    _dos_task_sleep_list = &_dos_sleep_list1;
    _dos_task_sleep_overflow_list = &_dos_sleep_list2;

    dos_task_list_init(_dos_task_sleep_list);
    dos_task_list_init(_dos_task_sleep_overflow_list);
}

/**
 * task priority/sleep list initialization
 */
static dos_void _dos_task_list_init(dos_void)
{
    dos_task_list_init(&_dos_recycle_list);
    _dos_task_priority_list_init();
    _dos_task_sleep_list_init();
}

/**
 * Task initialization
 */
static dos_void _dos_task_init(dos_task_t task)
{
    /** Get the top address of the task stack */
    task->top_of_stack = (dos_void *)((dos_uint32)task->stack_addr + (dos_uint32)(task->stack_size - 1));

    /** 8-byte alignment */
    task->top_of_stack = (dos_void *)((( uint32_t)task->top_of_stack) & (~((dos_uint32 )0x0007)));

    /** Task stack initialization */
    task->stack_point = dos_stack_init( task->top_of_stack,
                                    task->task_entry,
                                    task->parameter);
}


/** 
 * get Highest priority by bit map
 */
static dos_uint32 _dos_get_highest_priority(dos_uint32 pri)
{
    if (pri == 0) return 0;
#if DOS_HW_CLZ_SUPT
    return hard_ware_clz(pri);
#else
    if (pri & 0xff)
        return dos_bit_map[pri & 0xff];

    if (pri & 0xff00)
        return dos_bit_map[(pri & 0xff00) >> 8] + 8;

    if (pri & 0xff0000)
        return dos_bit_map[(pri & 0xff0000) >> 16] + 16;

    return dos_bit_map[(pri & 0xff000000) >> 24] + 24;
#endif
}


/**
 * insert 
 */
static dos_void _dos_insert_task_priority_list(dos_task_t task)
{
    /* update priority  */
    DOS_SET_TASK_PTIORITY(task);

    DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);

    /* init task list,the list will pend in readylist or pendlist  */
    /* insert priority list */
    dos_task_item_insert(&dos_task_priority_list[task->priority],&task->status_item);
}

static dos_void _dos_insert_task_sleep_list(dos_uint32 sleep_tick)
{
    dos_task_t cur_task = dos_current_task;

    if(dos_idle_task == cur_task)
    {
        DOS_LOG_ERR("Idle tasks are not allowed to sleep!\n");
        return;
    }

    /** Change task status */
    DOS_RESET_TASK_STATUS(cur_task, DOS_TASK_STATUS_READY);
    DOS_SET_TASK_STATUS(cur_task, DOS_TASK_STATUS_DELAY);

    /** Remove a task from the corresponding status list */
    if(dos_task_item_del(&(cur_task->status_item)) == 0)
    {
        /** If there are no more tasks under the current task priority, the bit corresponding to dos_task_priority will be canceled. */
        if(dos_task_list_is_empty(&dos_task_priority_list[cur_task->priority]) == DOS_TRUE)
        {
            DOS_RESET_TASK_PTIORITY(cur_task);
            // dos_task_priority &= ~(0x01 << cur_task->priority); 
        }
    }
  /** Calculate the time of wake up */
    cur_task->status_item.dos_task_value = dos_tick_count + sleep_tick;

    /**If the time overflows, insert the _dos_task_sleep_overflow_list list */
    if(cur_task->status_item.dos_task_value < dos_tick_count)   //time overflow
    {
        dos_task_item_insert(_dos_task_sleep_overflow_list, &(cur_task->status_item));
    }
    else
    {
        /** Time does not overflow, insert _dos_task_sleep_list list */
        dos_task_item_insert(_dos_task_sleep_list, &(cur_task->status_item)); //time no overflow
        if(dos_next_wake_tick >= cur_task->status_item.dos_task_value)
        {
            /**  */
            dos_next_wake_tick = cur_task->status_item.dos_task_value;
        }
    }
}


/**
 * idle task function
 */
static dos_void _dos_idle_task(dos_void *parameter)
{
    dos_task_t task;
#if DOS_USE_SALOF
    #include <fifo.h>
    #include <format.h>
    #include <string.h>
    extern Dos_Fifo_t Dos_Salof_Fifo;
    dos_int32 len;
    dos_uint8 buff[256];
#endif
    while(1)
    {
#if DOS_USE_SALOF
        len = Dos_FifoRead(Dos_Salof_Fifo, buff, sizeof(buff), 0);
        if(len > 0)
        {
            Dos_SalofOut((dos_char *)buff, len);
            memset(buff, 0, len);
        }
#endif
        if(dos_task_list_is_empty(&_dos_recycle_list) != DOS_TRUE)
        {
            task = dos_get_first_task(&_dos_recycle_list);
            dos_task_item_del(&(task->status_item));
            dos_mem_free(task);
            dos_mem_free(task->stack_addr);
        }
    }
}

/** 
 * create idle task
 */
static dos_void _dos_create_idle_task(dos_void)
{
 dos_idle_task = dos_task_create( "IdleTask",
                                &_dos_idle_task,
                                DOS_NULL,
                                DOS_IDLE_TASK_SIZE,
                                DOS_IDLE_TASK_PRIORITY,
                                DOS_IDLE_TASK_TICK);
    if(DOS_NULL == dos_idle_task)
    {
        DOS_LOG_ERR("dos_idle_task is NULL!\n");
    }
}

/**
 * switch sleep list
 * switch _dos_task_sleep_overflow_list when time overflows. 
 * Otherwise choose _dos_task_sleep_list
 */
static dos_void _dos_switch_sleep_list(dos_void)
{
    dos_task_list_t *list;
    dos_task_t task;

    /** When switching _dos_task_sleep_overflow_list, judge whether the current list is empty. */
    if(dos_task_list_is_empty(_dos_task_sleep_list) == DOS_TRUE)
    {
        /** switch sleep list */
        list = _dos_task_sleep_list;
        _dos_task_sleep_list = _dos_task_sleep_overflow_list;
        _dos_task_sleep_overflow_list = list;

        /** Whether the list is empty after switching sleep list  */
        if(dos_task_list_is_empty(_dos_task_sleep_list) == DOS_TRUE)
        {
            /** No wake up time required */
            dos_next_wake_tick = DOS_UINT32_MAX;
        }
        else
        {
            /** Get the first task of the sleep list, the task value is next wake time */
            task = dos_get_first_task(_dos_task_sleep_list);
            dos_next_wake_tick = task->status_item.dos_task_value;
        }
    }
    else
        DOS_LOG_DEBUG("Task sleep list is not empty!\n");
}


/***********************************************************************************************************************/

/**
 * system init
 */
dos_void dos_task_init(dos_void)
{
    /* init task list */
    _dos_task_list_init();

    /** create idle task */
    _dos_create_idle_task();

}

/**
 * This function will create a task
 * @param[in]  name         the name of task, which shall be unique
 * @param[in]  task_entry   the parameter of task enter function
 * @param[in]  param        the param of task enter 
 * @param[in]  stack_size   the start address of task stack
 * @param[in]  priority     the time slice if there are same prio task
 * @return     Task control block pointer
 */
dos_task_t dos_task_create(const dos_char *name,
                            dos_void (*task_entry)(dos_void *param),
                            dos_void * const param,
                            dos_uint32 stack_size,
                            dos_uint16 priority,
                            dos_uint32 tick)
{
    dos_task_t task;
    dos_void *stack;

    if(priority > DOS_MAX_PRIORITY_NUM)
    {
        DOS_LOG_ERR("unable to create task, task priority value is greater than maximum priority value\n");
        return DOS_NULL;
    }

    /** alloc for task control block memory  */
    task = (dos_task_t)dos_mem_alloc(sizeof(struct dos_task));
    if(DOS_NULL != task)
    {
        /** alloc for task stack memory  */
        stack = (dos_void *)dos_mem_alloc(stack_size);
        if(DOS_NULL == stack)
        {
            DOS_LOG_ERR("not enough memory to create a task\n");

            /** Failed to alloc memory */
            dos_mem_free(task);  
            return DOS_NULL;
        }

        if(tick == 0)
        {
            tick = 1;
        }
        /** Initialize list item and task control block information */
        dos_task_item_init(&task->status_item);
        dos_task_item_init(&task->pend_item);
        task->status_item.dos_task = (dos_void *)task;
        task->pend_item.dos_task = (dos_void *)task;
        task->stack_addr = stack;
        task->stack_size = stack_size;
        /** Insert in ascending order of priority in the pend list */
        task->pend_item.dos_task_value = priority;  
        task->task_init_tick = tick;
        task->task_tick = tick;
        task->wait_event = 0;
        task->wait_event_opt = 0;
    }
    else
    {
        return DOS_NULL;
    }
  
    /** Initialize task control block information */
    task->task_entry = (dos_void *)task_entry;
    task->parameter = param;
    task->priority = priority;
    task->task_name = (dos_char *)name;

    /** Task initialization */
    _dos_task_init(task);       

    /** Insert tasks into the ready list to allow task scheduling */
    _dos_insert_task_priority_list(task);

    return task;
}


/**
 * delete task function
 * @param[in]  dos_task         Task control block pointer
 */
dos_err dos_task_delete(dos_task_t task)
{
    dos_uint16 status;
    dos_uint32 pri;

    if(DOS_NULL == task)
    {
        DOS_LOG_ERR("unable to delete, task is null\n");
        return DOS_NOK;
    }
  
    status = task->task_status;

    if(DOS_TASK_STATUS_UNUSED == status)  /** task is unused status */
    {
        DOS_LOG_ERR("task status is unused\n");
        return DOS_NOK;
    }

    if(dos_scheduler_is_lock())
    {
        DOS_LOG_ERR("scheduler is lock\n");
        return DOS_NOK;
    }

    /** task is used status */
    if((DOS_TASK_STATUS_DELAY & status) || (DOS_TASK_STATUS_READY & status))  
    {
        pri = dos_interrupt_disable();

        /**Remove a task from the corresponding status list */
        if(dos_task_item_del(&(task->status_item)) == 0)
        {
            /** If there are no more tasks under the current task priority, the bit corresponding to dos_task_priority will be canceled. */
            if(dos_task_list_is_empty(&dos_task_priority_list[task->priority]) == DOS_TRUE)
            {
                DOS_RESET_TASK_PTIORITY(task);
            }
        }
        dos_interrupt_enable(pri);
    }
  
    /** If the task is in a suspended state, remove it from the pending list */
    if(DOS_TASK_STATUS_SUSPEND & status)
    {
        dos_task_item_del(&(task->pend_item));
    }
  
    /** set task status is unused */
    DOS_RESET_TASK_STATUS(task, DOS_TASK_STATUS_MASK);
    DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_UNUSED);

    if(task != dos_current_task)
    {
        /**If the deleted task is not the current task, then release the task control block and the task stack */
        dos_mem_free(task);
        dos_mem_free(task->stack_addr);
        return DOS_OK;
    }
    else
    {
        /* Insert recycle list, Reserved not implemented*/
        dos_task_item_insert(&_dos_recycle_list, &(task->status_item));
        return DOS_OK;
    }

}

/**
 * task sleep (tick)
 */
dos_void dos_task_sleep(dos_uint32 sleep_tick)
{
    dos_uint32 pri;

    if(0 == sleep_tick)
    {
        dos_scheduler();
        return ;
    }

    pri = dos_interrupt_disable();

    /** insert Task Sleep List */
    _dos_insert_task_sleep_list(sleep_tick);

    /** After the task sleeps, perform a task scheduling */
    dos_scheduler();

    dos_interrupt_enable(pri);
}


/**
 * get current task control block 
 */
dos_task_t dos_get_current_task(dos_void)
{
    dos_uint32 pri;
    dos_task_t task;
    
    pri = dos_interrupt_disable();
    task = dos_current_task;
    dos_interrupt_enable(pri);
    return task;
}

/**
 * get current task name
 */
dos_char *dos_get_task_name(dos_void)
{
    return dos_current_task->task_name;
}


/**
 * get the first task control block from the list
 */
dos_task_t dos_get_first_task(dos_task_list_t *list)
{
    list->task_item = list->task_end_item.next;
    if((dos_void*)(list)->task_item == (dos_void*)&((list)->task_end_item))
    {
        list->task_item = list->task_item->next;
    }
    return list->task_item->dos_task;
}


/**
 * get the task control block from the list, each time the function is called, the task item will move backwards once. 
 */
dos_task_t dos_get_next_task(dos_task_list_t *list)
{
    /** Support time slice rotation scheduling. Each time the function is called, the task item will move backwards once. */
    list->task_item = list->task_item->next;    

    if((dos_void*)(list)->task_item == (dos_void*)&((list)->task_end_item))
    {
        list->task_item = list->task_item->next;
    }
    return list->task_item->dos_task;
}


/**
 * get system tick
 */
dos_uint32 dos_get_tick(dos_void)
{
    return dos_tick_count;
}

/**
 * Tasks are waiting for message queues, events, semaphores, mutex locks etc.
 * set task status is suspend, insert pend list and sleep list.
 */
dos_void dos_task_wait(dos_task_list_t *list, dos_uint32 timeout)
{
    dos_task_t task = dos_current_task;

    /** set task status is suspend, insert pend list */
    DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_SUSPEND);
    dos_task_item_insert(list, &task->pend_item);

    if(timeout != DOS_WAIT_FOREVER)
    {
        /** Insert a task into the sleep list based on the wait timeout */
        _dos_insert_task_sleep_list(timeout);
        
        /** After the task sleeps, perform a task scheduling */
    }
    else
    {
        /** Permanent waiting */
        DOS_RESET_TASK_STATUS(task, DOS_TASK_STATUS_READY);

        /** Remove a task from the corresponding status list */
        if(dos_task_item_del(&(task->status_item)) == 0)
        {
            /** If there are no more tasks under the current task priority, the bit corresponding to dos_task_priority will be canceled. */
            if(dos_task_list_is_empty(&dos_task_priority_list[task->priority]) == DOS_TRUE)
            {
                DOS_RESET_TASK_PTIORITY(task);
            }
        }
    }

}

/**
 * wake task from pend list and sleep list.
 * and insert task priority list, task can be scheduled
 */
dos_void dos_task_wake(dos_task_t task)
{
    dos_task_item_del(&(task->pend_item));
    dos_task_item_del(&(task->status_item));

    DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_SUSPEND | DOS_TASK_STATUS_DELAY));

    /** insert task priority list, and set the priority position corresponding to the dos_task_priority variable is 1*/
    _dos_insert_task_priority_list(task);
}


dos_void dos_set_task_priority(dos_task_t task, dos_uint16 prio)
{
    if(task->task_status & DOS_TASK_STATUS_READY)
    {
        /** Remove a task from the corresponding status list */
        if(dos_task_item_del(&(task->status_item)) == 0)
        {
            /** If there are no more tasks under the current task priority, the bit corresponding to dos_task_priority will be canceled. */
            if(dos_task_list_is_empty(&dos_task_priority_list[task->priority]) == DOS_TRUE)
            {
                DOS_RESET_TASK_PTIORITY(task);
            }
        }

        DOS_RESET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
        task->priority = prio;
        _dos_insert_task_priority_list(task);
    }
    else
    {
        task->priority = prio;
    }
}


/**
 * task exit function
 */
dos_void dos_task_exit(dos_void)
{
    DOS_LOG_WARN("task exit\n");
    dos_task_delete(dos_get_current_task());
    dos_scheduler();
}


/**
 * Choose the right task to run
 */
dos_void dos_choose_task( dos_void )
{  
    /** Get the control block for the highest priority task  */
    dos_current_task = dos_get_next_task(&dos_task_priority_list[dos_cur_priority]);
}

/**
 * Choose the highest priority task to run
 */
dos_void dos_choose_highest_priority_task( dos_void )
{  
    /** Get the control block for the highest priority task  */
    dos_current_task = dos_get_first_task(&dos_task_priority_list[dos_cur_priority]);
}

dos_bool dos_check_task_priority(dos_void)
{
#if DOS_MAX_PRIORITY_NUM > 32
    dos_uint32 i;
    for(i = 0; i < DOS_PRIORITY_TAB; i++)
    {
        if(dos_task_priority[i] & 0xFFFFFFFF)
            break;
    }
    dos_cur_priority = _dos_get_highest_priority(dos_task_priority[i]) + 32 * i;
#else
    dos_cur_priority = _dos_get_highest_priority(dos_task_priority); /** get current task proority */
#endif
  
    /** Task scheduling when there are higher priority tasks, except for idle tasks. or the current task is not in the ready state */
    if((dos_cur_priority < dos_current_task->priority) || (!(dos_current_task->task_status & DOS_TASK_STATUS_READY)))
        return DOS_TRUE;
  
  /** there are more tasks in the current task priority list */
    else if(dos_get_task_list_value(&dos_task_priority_list[dos_current_task->priority]) > 1)
    {
        if(--dos_current_task->task_tick == 0)
        {
            dos_current_task->task_tick = dos_current_task->task_init_tick;
            return DOS_TRUE;
        }
        else
        {
            return DOS_FALSE;
        }
    }
  /** No task schedul */
  else
    return DOS_FALSE;
}

void dos_reset_tick(void)
{
    dos_tick_count = 0;
}

/**
 * update system tick
 */
extern dos_err dos_swtmr_overflow(dos_void);
dos_void dos_tick_update(dos_void)
{
    dos_task_t task;
    dos_uint32 tick;

    dos_tick_count++;

    if(dos_tick_count ==0)   //overflow
    {
        /** When time overflows, switch list */
        _dos_switch_sleep_list();
#if DOS_SWTMR
        /** Notify the software timer of overflow */
        dos_swtmr_overflow(); 
#endif
    }
  
    /** When a timeout event occurs, such as sleep timeout, waiting for message queue, semaphore, mutex, event timeout ect */
    if(dos_tick_count >= dos_next_wake_tick)
    {
        /** Handle all timeout events in the loop, ensuring that each timeout is handled */
        for(;;)
        {
            /** When the timeout list is empty, the task wakeup time is not set. */
            if(dos_task_list_is_empty(_dos_task_sleep_list) == DOS_TRUE)
            {
                dos_next_wake_tick = DOS_UINT32_MAX;
                break;
            }
            else
            {
                /** get task contorl block and set task wake time */
                task = dos_get_first_task(_dos_task_sleep_list);
                tick = task->status_item.dos_task_value;
                if(tick > dos_next_wake_tick)
                {
                dos_next_wake_tick = tick;
                break;
                }
            
                /** swtmr_timeout has occurred, remove the task from the sleep list, and set the task status to (~DOS_TASK_STATUS_DELAY) */
                dos_task_item_del(&task->status_item);
                DOS_RESET_TASK_STATUS(task, DOS_TASK_STATUS_DELAY);

                if(task->task_status & DOS_TASK_STATUS_SUSPEND)
                {
                    /** If the task status is waiting for a message queue, semaphore, mutex, event, etc.
                      *  when the wait has timed out, set its status to DOS_TASK_STATUS_TIMEOUT 
                      */
                    DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_TIMEOUT);
                }

                /** Insert the task into the ready list and set the task status to aaa */
                _dos_insert_task_priority_list(task);
            }
        } 
    }
    
    dos_scheduler();
}





