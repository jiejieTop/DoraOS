#include <event.h>
#include <log.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>
#include <task.h>
#include <port.h>


static dos_bool _Dos_CheckEvent(Dos_Event_t event, dos_uint32 want_event, dos_uint32 op)
{
    dos_uint32 option;

    option = op & WAIT_EVENT_OP;

    switch (option)
    {
        case WAIT_ANY_EVENT:
            if((event->EventSet & want_event) != 0)
                goto got;
            break;

        case WAIT_ALL_EVENT:
            if((event->EventSet & want_event) == want_event)
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
        event->EventSet &= ~(want_event);
    }
    return DOS_TRUE;
}

/**
 * create a event set
 */
Dos_Event_t Dos_EventCreate(void)
{
    Dos_Event_t event;

    event = (Dos_Event_t)Dos_MemAlloc(sizeof(struct Dos_Event));
    if(event == DOS_NULL)
    {
        DOS_LOG_ERR("event is null\n");
        return DOS_NULL;
    }
    
    event->EventSet = 0;
    Dos_TaskList_Init(&(event->EventPend));

    return event;
}


/**
 * delete a event set
 * description: You need to set the event pointer to null after deleting the event set
 */
dos_err Dos_EventDelete(Dos_Event_t event)
{
    if(event != DOS_NULL)
    {
        if(Dos_TaskList_IsEmpty(&(event->EventPend)))
        {
            memset(event,0,sizeof(struct Dos_Event));
            Dos_MemFree(event);
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
dos_uint32 Dos_EventWait(Dos_Event_t event, dos_uint32 wait_event, dos_uint32 op, dos_uint32 timeout)
{
    DOS_TaskCB_t task;

    if((event == DOS_NULL) ||(wait_event == 0))
    {
        DOS_LOG_WARN("event is null or the waiting event is 0\n");
        return 0;
    }

    if((op & WAIT_EVENT_OP) == WAIT_EVENT_OP)
    {
        DOS_LOG_WARN("waiting for event option is invalid\n");
        return 0;
    }
    
    if(_Dos_CheckEvent(event, wait_event, op) == DOS_TRUE)
    {
        return wait_event;  /** waiting for the event to succeed */
    }

    if((timeout == 0) || (Dos_Scheduler_IsLock()))  /** scheduler is lock */
    {
        return 0;
    }

    if(Dos_ContextIsInt())
    {
        DOS_LOG_ERR("event wait time is not 0, and the context is in an interrupt\n");
        return 0;
    }

    task = (DOS_TaskCB_t)Dos_Get_CurrentTCB();
    task->WaitEvent = wait_event;
    task->WaitEventOp = op;

    Dos_TaskWait(&event->EventPend, timeout);

    /** Task resumes running */
    if(task->TaskStatus & DOS_TASK_STATUS_TIMEOUT)
    {
        DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
        DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
        Dos_TaskItem_Del(&(task->PendItem));
        DOS_LOG_INFO("waiting for event timeout\n");
        task->WaitEvent &= (~wait_event);
        task->WaitEventOp = 0;
        return 0;
    }

    return task->EventGet;
}

/**
 * set a event set
 */
dos_uint32 Dos_EventSet(Dos_Event_t event, dos_uint32 set_event)
{
    dos_uint32 value;
    DOS_TaskCB_t task;

    if((event == DOS_NULL) ||(set_event == 0))
    {
        DOS_LOG_WARN("event is null or the setting event is 0\n");
        return 0;
    }

    event->EventSet |= set_event;

    if(!(Dos_TaskList_IsEmpty(&event->EventPend)))
    {
        Dos_Scheduler_Lock();

        for(value = Dos_Get_TaskListValue(&event->EventPend); value > 0; value--)
        {
            task = Dos_Get_NextTCB(&event->EventPend);

            if(_Dos_CheckEvent(event, task->WaitEvent, task->WaitEventOp) == DOS_TRUE)
            {
                task->EventGet = task->WaitEvent;
                Dos_TaskWake(task);
            }
        }

        Dos_Scheduler_Unlock();
    }

  return event->EventSet;
}
















