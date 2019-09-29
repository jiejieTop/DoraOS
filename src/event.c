#include <event.h>
#include <log.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>
#include <task.h>
#include <port.h>


static dos_bool _dos_check_event(dos_event_t event, dos_uint32 want_event, dos_uint32 op)
{
    dos_uint32 option;

    option = op & WAIT_EVENT_OP;

    switch (option)
    {
        case WAIT_ANY_EVENT:
            if((event->event_flag & want_event) != 0)
                goto got;
            break;

        case WAIT_ALL_EVENT:
            if((event->event_flag & want_event) == want_event)
                goto got;
            break;

        default:
            DOS_LOG_ERR("op is fail\n");
            break;
    }

    return DOS_FALSE;
got:
    if(!(op & NO_CLR_EVENT))
    {
        event->event_flag &= ~(want_event);
    }
    return DOS_TRUE;
}

/**
 * create a event 
 */
dos_event_t dos_event_create(void)
{
    dos_event_t event;

    event = (dos_event_t)dos_mem_alloc(sizeof(struct dos_event));
    if(event == DOS_NULL)
    {
        DOS_LOG_ERR("event is null\n");
        return DOS_NULL;
    }
    
    event->event_flag = 0;
    dos_task_list_init(&(event->event_pend_list));

    return event;
}


/**
 * delete a event
 * description: You need to set the event pointer to null after deleting the event
 */
dos_err dos_event_delete(dos_event_t event)
{
    if(event != DOS_NULL)
    {
        if(dos_task_list_is_empty(&(event->event_pend_list)))
        {
            memset(event,0,sizeof(struct dos_event));
            dos_mem_free(event);
            return DOS_OK;
        }
        else
        {
            DOS_LOG_WARN("there are tasks in the event pend list\n");
            return DOS_NOK;
        }
    }
    else
    {
        DOS_LOG_WARN("event is null\n");
        return DOS_NOK;
    }
}

/**
 * event wait function
 */
dos_uint32 dos_event_wait(dos_event_t event, dos_uint32 wait_event, dos_uint32 op, dos_uint32 timeout)
{
    dos_uint32 pri;
    dos_task_t task;
    
    pri = dos_interrupt_disable();

    if((event == DOS_NULL) ||(wait_event == 0))
    {
        DOS_LOG_WARN("event is null or the waiting event is 0\n");
        dos_interrupt_enable(pri);
        return 0;
    }

    if((op & WAIT_EVENT_OP) == WAIT_EVENT_OP)
    {
        DOS_LOG_WARN("waiting for event option is invalid\n");
        dos_interrupt_enable(pri);
        return 0;
    }
    
    if(_dos_check_event(event, wait_event, op) == DOS_TRUE)
    {
        dos_interrupt_enable(pri);
        return wait_event;  /** waiting for the event to succeed */
    }

    if((timeout == 0) || (dos_scheduler_is_lock()))  /** scheduler is lock */
    {
        dos_interrupt_enable(pri);
        return 0;
    }

    if(dos_context_is_interrupt())
    {
        DOS_LOG_ERR("event wait time is not 0, and the context is in an interrupt\n");
        dos_interrupt_enable(pri);
        return 0;
    }

    task = (dos_task_t)dos_get_current_task();
    task->wait_event = wait_event;
    task->wait_event_opt = op;

    dos_task_wait(&event->event_pend_list, timeout);
    dos_interrupt_enable(pri);
    dos_scheduler();

    /** Task resumes running */
    if(task->task_status & DOS_TASK_STATUS_TIMEOUT)
    {
        pri = dos_interrupt_disable();
        DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
        DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
        dos_task_item_del(&(task->pend_item));
        task->wait_event &= (~wait_event);
        task->wait_event_opt = 0;
        dos_interrupt_enable(pri);
        return 0;
    }

    return task->event_get;
}

/**
 * set a event flag
 */
dos_uint32 dos_event_set(dos_event_t event, dos_uint32 set_event)
{
    dos_uint32 pri;
    dos_uint32 value;
    dos_task_t task;
    
    pri = dos_interrupt_disable();


    if((event == DOS_NULL) ||(set_event == 0))
    {
        DOS_LOG_WARN("event is null or the setting event is 0\n");
        dos_interrupt_enable(pri);
        return 0;
    }

    event->event_flag |= set_event;
    dos_interrupt_enable(pri);

    if(!(dos_task_list_is_empty(&event->event_pend_list)))
    {
        dos_scheduler_lock();

        for(value = dos_get_task_list_value(&event->event_pend_list); value > 0; value--)
        {
            task = dos_get_next_task(&event->event_pend_list);

            if(_dos_check_event(event, task->wait_event, task->wait_event_opt) == DOS_TRUE)
            {
                task->event_get = task->wait_event;
                dos_task_wake(task);
                dos_scheduler();
            }
        }

        dos_scheduler_unlock();
    }

  return event->event_flag;
}
















