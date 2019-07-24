#include <queue.h>
#include <log.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>


static dos_void _Dos_QueueCopy(Dos_Queue_t queue, dos_void *buff, dos_size size, dos_uint8 op)
{
    dos_uint32 pri;
    
    pri = Dos_Interrupt_Disable();
    
    switch (op)
    {
    case QUEUE_READ:
        memcpy(buff, queue->QueueRWPtr[op], size);
        break;

    case QUEUE_WRITE:
        memcpy(queue->QueueRWPtr[op], buff, size);
        break;

    default:
        DOS_LOG_ERR("invalid queue operate type!\n");
        break;
    }

    queue->QueueRWPtr[op] = (dos_uint8 *)queue->QueueRWPtr[op] + queue->QueueSize;
    if(queue->QueueRWPtr[op] == queue->QueueTPtr)
    {
        queue->QueueRWPtr[op] = queue->QueueHPtr;
    }
    Dos_Interrupt_Enable(pri);
}


/**
 * Generic message queue handler
 * queue : massages queue
 * buff  : data pointer
 * size  : queue data size
 * op    : 0 is read, 1 is wirte
 * timeout : tick
 */
static dos_err _Dos_Queuehandler(Dos_Queue_t queue, dos_void *buff, dos_size size, dos_uint8 op, dos_uint32 timeout)
{
    dos_uint32 pri;
    DOS_TaskCB_t task;

    pri = Dos_Interrupt_Disable();
    
    if((!queue) || (!buff) || (!size) || (op > QUEUE_WRITE))
    {
        DOS_LOG_WARN("queue does not satisfy the condition\n");
        Dos_Interrupt_Enable(pri);
        return DOS_NOK;
    }

    size = DOS_MIN(size, queue->QueueSize);

    if(0 == queue->QueueRWCnt[op])
    {
        if(0 == timeout)
        {
            Dos_Interrupt_Enable(pri);
            return DOS_NOK;
        }

        if(Dos_ContextIsInt())
        {
            DOS_LOG_ERR("queue wait time is not 0, and the context is in an interrupt\n");
            Dos_Interrupt_Enable(pri);
            return DOS_NOK;
        }

        if(Dos_Scheduler_IsLock())  /** scheduler is lock */
        {
            Dos_Interrupt_Enable(pri);
            return DOS_NOK;
        }

        Dos_TaskWait(&queue->QueuePend[op], timeout);
        Dos_Interrupt_Enable(pri);
        Dos_Scheduler();
        
        task = (DOS_TaskCB_t)Dos_Get_CurrentTCB();
        /** Task resumes running */
        if(task->TaskStatus & DOS_TASK_STATUS_TIMEOUT)
        {
            DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
            DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
            Dos_TaskItem_Del(&(task->PendItem));
            DOS_LOG_INFO("waiting for queue timeout\n");
            return DOS_NOK;
        }
    }
    else
    {
        queue->QueueRWCnt[op]--;
        Dos_Interrupt_Enable(pri);
    }
    
    _Dos_QueueCopy(queue, buff, size, op);
    
    if(!Dos_TaskList_IsEmpty(&(queue->QueuePend[1-op])))
    {
        pri = Dos_Interrupt_Disable();
        task = Dos_GetTCB(&(queue->QueuePend[1-op]));
        Dos_TaskWake(task);
        Dos_Interrupt_Enable(pri);
        Dos_Scheduler();
    }
    else
    {
        queue->QueueRWCnt[1-op]++;
    }
    
    return DOS_OK;
}


/**
 * create a queue
 * @param[in]  len: queue length
 * @param[in]  size: queue node size
 */
Dos_Queue_t Dos_QueueCreate(dos_uint16 len, dos_uint16 size)
{
    Dos_Queue_t queue;
    dos_size queue_size;

    if((len <= 0) || (size <= 0))
    {
       DOS_LOG_WARN("queue len or size is 0\n");
        return DOS_NULL;
    }

    queue_size = (dos_size)(len * size);

    queue = (Dos_Queue_t)Dos_MemAlloc(sizeof(struct Dos_Queue) + queue_size);
    if(queue == DOS_NULL)
    {
        DOS_LOG_ERR("unable to create queue\n");
        return DOS_NULL;
    }

    memset(queue,0,sizeof(struct Dos_Queue) + queue_size);

    queue->QueueLen = len;
    queue->QueueSize = size;

    queue->QueueHPtr = (dos_uint8 *)queue + sizeof(struct Dos_Queue);
    queue->QueueTPtr = queue->QueueHPtr + (queue->QueueLen * queue->QueueSize);

    queue->QueueRWCnt[QUEUE_READ] = 0;
    queue->QueueRWCnt[QUEUE_WRITE] = len;

//    queue->QueueRWLock[QUEUE_READ] = 0;
//    queue->QueueRWLock[QUEUE_WRITE] = 0;

    queue->QueueRWPtr[QUEUE_READ] = queue->QueueHPtr;
    queue->QueueRWPtr[QUEUE_WRITE] = queue->QueueHPtr;
    
    Dos_TaskList_Init(&(queue->QueuePend[QUEUE_READ]));
    Dos_TaskList_Init(&(queue->QueuePend[QUEUE_WRITE]));
    
    return queue;
}


/**
 * delete a queue
 * description: You need to set the semaphore pointer to null after deleting the semaphore
 */
dos_err Dos_QueueDelete(Dos_Queue_t queue)
{
    if(queue != DOS_NULL)
    {
        if((Dos_TaskList_IsEmpty(&(queue->QueuePend[QUEUE_READ]))) && (Dos_TaskList_IsEmpty(&(queue->QueuePend[QUEUE_WRITE]))))
        {
            memset(queue,0,sizeof(struct Dos_Queue) + (dos_size)(queue->QueueLen * queue->QueueSize));
            Dos_MemFree(queue);
            return DOS_OK;
        }
        else
        {
            DOS_LOG_WARN("there are tasks in the queue pend list\n");
            return DOS_NOK;
        }
    }
    else
    {
        DOS_LOG_WARN("queue is null\n");
        return DOS_NOK;
    }
}

/**
 * Read data from the queue
 */
dos_err Dos_QueueRead(Dos_Queue_t queue, dos_void *buff, dos_size size, dos_uint32 timeout)
{
    return _Dos_Queuehandler(queue, buff, size, QUEUE_READ, timeout);
}

/**
 * Write data to the queue
 */
dos_err Dos_QueueWrite(Dos_Queue_t queue, dos_void *buff, dos_size size, dos_uint32 timeout)
{
    return _Dos_Queuehandler(queue, buff, size, QUEUE_WRITE, timeout);
}



