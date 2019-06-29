#include <queue.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>


static void _Dos_QueueCopy(Dos_Queue_t queue, void *buff, size_t size, dos_uint8 op)
{
    switch (op)
    {
    case QUEUE_READ:
        memcpy(buff, queue->QueueRWPtr[op], size);
        break;

    case QUEUE_WRITE:
        memcpy(queue->QueueRWPtr[op], buff, size);
        break;

    default:
        DOS_PRINT_ERR("invalid queue operate type!\n");
        break;
    }

    queue->QueueRWPtr[op] = (uint8_t *)queue->QueueRWPtr[op] + queue->QueueSize;
    if(queue->QueueRWPtr[op] == queue->QueueTPtr)
    {
        queue->QueueRWPtr[op] = queue->QueueHPtr;
    }

}


/**
 * Generic message queue handler
 * queue : massages queue
 * buff  : data pointer
 * size  : queue data size
 * op    : 0 is read, 1 is wirte
 * timeout : tick
 */
static dos_err _Dos_Queuehandler(Dos_Queue_t queue, void *buff, size_t size, dos_uint8 op, dos_uint32 timeout)
{
   DOS_TaskCB_t task;

    if((!queue) || (!buff) || (!size) || (op > QUEUE_WRITE))
    {
        return DOS_NOK;
    }

    size = DOS_MIN(size, queue->QueueSize);

    if(0 == queue->QueueRWCnt[op])
    {
        if(0 == timeout)
        {
            return DOS_NOK;
        }

        if(Dos_Scheduler_IsLock())  /** scheduler is lock */
        {
            return DOS_NOK;
        }

        Dos_TaskWait(&queue->QueuePend[op], timeout);
        
        task = (DOS_TaskCB_t)Dos_Get_CurrentTCB();
        /** Task resumes running */
        if(task->TaskStatus & DOS_TASK_STATUS_TIMEOUT)
        {
            task->TaskStatus &= (~DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND);
            DOS_PRINT_DEBUG("QUEUE TIMEOUT\n");
            return DOS_NOK;
        }

    }
    else
    {
        queue->QueueRWCnt[op]--;
    }

    _Dos_QueueCopy(queue, buff, size, op);

    if(!Dos_TaskList_IsEmpty(&(queue->QueuePend[1-op])))
    {
        task = Dos_GetTCB(&(queue->QueuePend[1-op]));
        Dos_TaskWake(task);
    }
    else
    {
        queue->QueueRWCnt[1-op]++;
    }

    return DOS_OK;
}



Dos_Queue_t Dos_QueueCreate(dos_uint16 len, dos_uint16 size)
{
    Dos_Queue_t queue;
    dos_size queue_size;

    if((len <= 0) || (size <= 0))
    {
       DOS_PRINT_ERR("queue len or size is 0\n");
        return DOS_NULL;
    }

    queue_size = (dos_size)(len * size);

    queue = (Dos_Queue_t)Dos_MemAlloc(sizeof(Dos_Queue_t) + queue_size);
    if(queue == DOS_NULL)
    {
       DOS_PRINT_ERR("queue is null\n");
        return DOS_NULL;
    }

    memset(queue,0,sizeof(Dos_Queue_t) + queue_size);

    queue->QueueLen = len;
    queue->QueueSize = size;

    queue->QueueHPtr = (dos_uint8 *)queue + sizeof(Dos_Queue_t);
    queue->QueueTPtr = queue->QueueHPtr + (queue->QueueLen * queue->QueueSize);

    queue->QueueRWCnt[QUEUE_READ] = 0;
    queue->QueueRWCnt[QUEUE_WRITE] = len;

    queue->QueueRWLock[QUEUE_READ] = 0;
    queue->QueueRWLock[QUEUE_WRITE] = 0;

    queue->QueueRWPtr[QUEUE_READ] = queue->QueueHPtr;
    queue->QueueRWPtr[QUEUE_WRITE] = queue->QueueHPtr;
    
    Dos_TaskList_Init(&(queue->QueuePend[QUEUE_READ]));
    Dos_TaskList_Init(&(queue->QueuePend[QUEUE_WRITE]));
    
    return queue;
}


dos_err Dos_QueueRead(Dos_Queue_t queue, void *buff, size_t size, dos_uint32 timeout)
{
    return _Dos_Queuehandler(queue, buff, size, QUEUE_READ, timeout);
}

dos_err Dos_QueueWrite(Dos_Queue_t queue, void *buff, size_t size, dos_uint32 timeout)
{
    return _Dos_Queuehandler(queue, buff, size, QUEUE_WRITE, timeout);
}



