#include <sem.h>
#include <log.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>
#include <task.h>
#include <port.h>

/**
 * semaphores create
 * @param[in]  cnt : Initialize the number of semaphores
 * @param[in]  cnt : Maximum number of semaphores
 */
dos_sem_t dos_sem_create(dos_uint32 cnt, dos_uint32 max_cnt)
{
    dos_sem_t sem;

    if(cnt > max_cnt)
    {
        DOS_LOG_WARN("the cnt of the semaphore cannot exceed the max cnt\n");
    }

    sem = (dos_sem_t)dos_mem_alloc(sizeof(struct dos_sem));
    if(sem == DOS_NULL)
    {
        DOS_LOG_ERR("unable to create sem\n");
        return DOS_NULL;
    }

    memset(sem, 0, sizeof(struct dos_sem));

    sem->sem_count = cnt;
    sem->sem_max_count = max_cnt;
    
    /** Initialize semaphores pend list */
    dos_task_list_init(&(sem->sem_pend_list));

    return sem;
}

/**
 * semaphores delete
 * description: You need to set the semaphore pointer to null after deleting the semaphore
 */
dos_err dos_sem_delete(dos_sem_t sem)
{
    if(sem != DOS_NULL)
    {
        if(dos_task_list_is_empty(&(sem->sem_pend_list)))
        {
            memset(sem,0,sizeof(struct dos_sem));
            dos_mem_free(sem);
            return DOS_OK;
        }
        else
        {
            DOS_LOG_WARN("there are tasks in the semaphore pend list\n");
            return DOS_NOK;
        }
    }
    else
    {
        DOS_LOG_WARN("sem is null\n");
        return DOS_NOK;
    }
}

/**
 * semaphores wait function
 */
dos_err dos_sem_pend(dos_sem_t sem, dos_uint32 timeout)
{
    dos_uint32 pri;
    dos_task_t task;
    
    pri = dos_interrupt_disable();
    
    if(sem == DOS_NULL)
    {
        DOS_LOG_WARN("sem is null\n");
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }

    if(sem->sem_count > 0)
    {
        sem->sem_count--;
        dos_interrupt_enable(pri);
        return DOS_OK;
    }

    if((timeout == 0) || (dos_scheduler_is_lock()))  /** scheduler is lock */
    {
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }

    if(dos_context_is_interrupt())
    {
        DOS_LOG_ERR("sem wait time is not 0, and the context is in an interrupt\n");
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }

    dos_task_wait(&sem->sem_pend_list, timeout);
    dos_interrupt_enable(pri);
    dos_scheduler();
    
    task = (dos_task_t)dos_get_current_task();
    /** Task resumes running */
    if(task->task_status & DOS_TASK_STATUS_TIMEOUT)
    {
        pri = dos_interrupt_disable();
        DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
        DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
        dos_task_item_del(&(task->pend_item));
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }

    return DOS_OK;
}

/**
 * semaphores post
 */
dos_err dos_sem_post(dos_sem_t sem)
{
    dos_uint32 pri;
    dos_task_t task;
    
    pri = dos_interrupt_disable();

    if(sem == DOS_NULL)
    {
        DOS_LOG_WARN("sem is null\n");
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }
    
    if(sem->sem_count == sem->sem_max_count)
    {
        dos_interrupt_enable(pri);
        return DOS_NOK; /** overflow */
    }

    /** No task is waiting for the semaphore, the number of semaphores ++ */
    if(dos_task_list_is_empty(&(sem->sem_pend_list)))
    {
        sem->sem_count++;  
        dos_interrupt_enable(pri);
    }
    else
    {
        task = dos_get_first_task(&(sem->sem_pend_list));
        dos_task_wake(task);
        dos_interrupt_enable(pri);
        dos_scheduler();
    }

    return DOS_OK;
}


















