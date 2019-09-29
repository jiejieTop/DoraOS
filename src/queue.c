#include <queue.h>
#include <log.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>


static dos_void _dos_queue_copy(dos_queue_t queue, dos_void *buff, dos_size size, dos_uint8 op)
{
    dos_uint32 pri;
    
    pri = dos_interrupt_disable();
    
    switch (op)
    {
    case QUEUE_READ:
        memcpy(buff, queue->queue_rw_ptr[op], size);
        break;

    case QUEUE_WRITE:
        memcpy(queue->queue_rw_ptr[op], buff, size);
        break;

    default:
        DOS_LOG_ERR("invalid queue operate type!\n");
        break;
    }

    queue->queue_rw_ptr[op] = (dos_uint8 *)queue->queue_rw_ptr[op] + queue->queue_size;
    if(queue->queue_rw_ptr[op] == queue->queue_tail_ptr)
    {
        queue->queue_rw_ptr[op] = queue->queue_head_ptr;
    }
    dos_interrupt_enable(pri);
}


/**
 * Generic message queue handler
 * queue : massages queue
 * buff  : data pointer
 * size  : queue data size
 * op    : 0 is read, 1 is wirte
 * timeout : tick
 */
static dos_err _dos_queue_handler(dos_queue_t queue, dos_void *buff, dos_size size, dos_uint8 op, dos_uint32 timeout)
{
    dos_uint32 pri;
    dos_task_t task;

    pri = dos_interrupt_disable();
    
    if((!queue) || (!buff) || (!size) || (op > QUEUE_WRITE))
    {
        DOS_LOG_WARN("queue does not satisfy the condition\n");
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }

    size = DOS_MIN(size, queue->queue_size);

    if(0 == queue->queue_rw_count[op])
    {
        if(0 == timeout)
        {
            dos_interrupt_enable(pri);
            return DOS_NOK;
        }

        if(dos_context_is_interrupt())
        {
            DOS_LOG_ERR("queue wait time is not 0, and the context is in an interrupt\n");
            dos_interrupt_enable(pri);
            return DOS_NOK;
        }

        if(dos_scheduler_is_lock())  /** scheduler is lock */
        {
            dos_interrupt_enable(pri);
            return DOS_NOK;
        }

        dos_task_wait(&queue->queue_pend_list[op], timeout);
        dos_interrupt_enable(pri);
        dos_scheduler();
        
        task = (dos_task_t)dos_get_current_task();
        /** Task resumes running */
        if(task->task_status & DOS_TASK_STATUS_TIMEOUT)
        {
            DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
            DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
            dos_task_item_del(&(task->pend_item));
            return DOS_NOK;
        }
    }
    else
    {
        queue->queue_rw_count[op]--;
        dos_interrupt_enable(pri);
    }
    
    _dos_queue_copy(queue, buff, size, op);
    
    if(!dos_task_list_is_empty(&(queue->queue_pend_list[1-op])))
    {
        pri = dos_interrupt_disable();
        task = dos_get_first_task(&(queue->queue_pend_list[1-op]));
        dos_task_wake(task);
        dos_interrupt_enable(pri);
        dos_scheduler();
    }
    else
    {
        queue->queue_rw_count[1-op]++;
    }
    
    return DOS_OK;
}


/**
 * create a queue
 * @param[in]  len: queue length
 * @param[in]  size: queue node size
 */
dos_queue_t dos_queue_create(dos_uint16 len, dos_uint16 size)
{
    dos_queue_t queue;
    dos_size queue_size;

    if((len <= 0) || (size <= 0))
    {
       DOS_LOG_WARN("queue len or size is 0\n");
        return DOS_NULL;
    }

    queue_size = (dos_size)(len * size);

    queue = (dos_queue_t)dos_mem_alloc(sizeof(struct dos_queue) + queue_size);
    if(queue == DOS_NULL)
    {
        DOS_LOG_ERR("unable to create queue\n");
        return DOS_NULL;
    }

    memset(queue,0,sizeof(struct dos_queue) + queue_size);

    queue->queue_len = len;
    queue->queue_size = size;

    queue->queue_head_ptr = (dos_uint8 *)queue + sizeof(struct dos_queue);
    queue->queue_tail_ptr = queue->queue_head_ptr + (queue->queue_len * queue->queue_size);

    queue->queue_rw_count[QUEUE_READ] = 0;
    queue->queue_rw_count[QUEUE_WRITE] = len;

//    queue->QueueRWLock[QUEUE_READ] = 0;
//    queue->QueueRWLock[QUEUE_WRITE] = 0;

    queue->queue_rw_ptr[QUEUE_READ] = queue->queue_head_ptr;
    queue->queue_rw_ptr[QUEUE_WRITE] = queue->queue_head_ptr;
    
    dos_task_list_init(&(queue->queue_pend_list[QUEUE_READ]));
    dos_task_list_init(&(queue->queue_pend_list[QUEUE_WRITE]));
    
    return queue;
}


/**
 * delete a queue
 * description: You need to set the semaphore pointer to null after deleting the semaphore
 */
dos_err dos_queue_delete(dos_queue_t queue)
{
    if(queue != DOS_NULL)
    {
        if((dos_task_list_is_empty(&(queue->queue_pend_list[QUEUE_READ]))) && (dos_task_list_is_empty(&(queue->queue_pend_list[QUEUE_WRITE]))))
        {
            memset(queue,0,sizeof(struct dos_queue) + (dos_size)(queue->queue_len * queue->queue_size));
            dos_mem_free(queue);
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
dos_err dos_queue_read(dos_queue_t queue, dos_void *buff, dos_size size, dos_uint32 timeout)
{
    return _dos_queue_handler(queue, buff, size, QUEUE_READ, timeout);
}

/**
 * Write data to the queue
 */
dos_err dos_queue_write(dos_queue_t queue, dos_void *buff, dos_size size, dos_uint32 timeout)
{
    return _dos_queue_handler(queue, buff, size, QUEUE_WRITE, timeout);
}



