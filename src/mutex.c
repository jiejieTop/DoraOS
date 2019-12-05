/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-15 23:56:23
 * @LastEditTime: 2019-12-05 23:03:45
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <mutex.h>
#include <log.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>
#include <task.h>
#include <port.h>


/**
 * create a mutex
 */
dos_mutex_t dos_mutex_create(void)
{
    dos_mutex_t mutex;

    mutex = dos_mem_alloc(sizeof(struct dos_mutex));
    if (mutex == DOS_NULL) {
        DOS_LOG_ERR("mutex is null\n");
        return DOS_NULL;
    }

    memset(mutex, 0, sizeof(struct dos_mutex));

    mutex->mutex_count = 0;
    mutex->mutex_owner = DOS_NULL;
    mutex->mutex_priority = 0;

    /** Initialize mutex pend list */
    dos_task_list_init(&(mutex->mutex_pend_list));

    return mutex;
}

/**
 * delete a mutex
 */
dos_err dos_mutex_delete(dos_mutex_t mutex)
{
    if (mutex != DOS_NULL) {
        if (dos_task_list_is_empty(&(mutex->mutex_pend_list))) {
            memset(mutex,0,sizeof(struct dos_mutex));
            dos_mem_free(mutex);
            return DOS_OK;
        } else {
            DOS_LOG_WARN("there are tasks in the mutex pend list\n");
            return DOS_NOK;
        }
    } else {
        DOS_LOG_WARN("the mutex to be deleted is null\n");
        return DOS_NOK;
    }
}

/**
 * the mutex P operation, pending mutex
 */
dos_err dos_mutex_pend(dos_mutex_t mutex, dos_uint32 timeout)
{
    dos_task_t task;
    dos_uint32 pri;

    if ((mutex == DOS_NULL) || (dos_context_is_interrupt())) {
        DOS_LOG_WARN("unable to continue to pend the mutex, the mutex is null or is currently in the interrupt context\n");
        return DOS_NOK;
    }

    task = (dos_task_t)dos_get_current_task();

    pri = dos_interrupt_disable();
    /** can pend the mutex */
    if (mutex->mutex_count == 0) {
        mutex->mutex_count++;
        mutex->mutex_priority = task->priority;
        mutex->mutex_owner = task;
        dos_interrupt_enable(pri);
        return DOS_OK;
    }

    if (mutex->mutex_owner == task) {
        mutex->mutex_count++;
        dos_interrupt_enable(pri);
        return DOS_OK;
    }

    /** time is be set 0 or scheduler is lock */
    if ((timeout == 0) || (dos_scheduler_is_lock())) {
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }

    /** current task has a higher priority, priority inheritance is required */
    if (mutex->mutex_priority > task->priority) {
        dos_set_task_priority(mutex->mutex_owner, task->priority);
    }

    dos_task_wait(&mutex->mutex_pend_list, timeout);
    dos_interrupt_enable(pri);
    dos_scheduler();

    /** Task resumes running */
    if (task->task_status & DOS_TASK_STATUS_TIMEOUT) {
        pri = dos_interrupt_disable();
        DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
        DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
        dos_task_item_del(&(task->pend_item));
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }

    return DOS_OK;
}


dos_err dos_mutex_post(dos_mutex_t mutex)
{
    dos_task_t task;
    dos_uint32 pri;

    if ((mutex == DOS_NULL) || (dos_context_is_interrupt())) {
        DOS_LOG_WARN("unable to continue to post the mutex, the mutex is null or is currently in the interrupt context\n");
        return DOS_NOK;
    }

    task = (dos_task_t)dos_get_current_task();

    pri = dos_interrupt_disable();

    if ((mutex->mutex_count == 0) || (mutex->mutex_owner != task)) {
        DOS_LOG_WARN("unable to continue to post the mutex, the mutex is not be held, or the owner is not the current task\n");
        dos_interrupt_enable(pri);
        return DOS_NOK;
    }


    if (--(mutex->mutex_count) != 0) {
        dos_interrupt_enable(pri);
        return DOS_OK;
    }

    /** Determine if priority inheritance occurs ? */
    if (mutex->mutex_owner->priority != mutex->mutex_priority) {
        dos_set_task_priority(mutex->mutex_owner, mutex->mutex_priority);
    }

    if (!dos_task_list_is_empty(&(mutex->mutex_pend_list))) {
        task = dos_get_first_task(&(mutex->mutex_pend_list));
        mutex->mutex_priority = task->priority;
        mutex->mutex_count++;
        mutex->mutex_owner = task;
        dos_task_wake(task);
        dos_interrupt_enable(pri);
        dos_scheduler();
    } else {
        mutex->mutex_priority = 0;
        mutex->mutex_owner = DOS_NULL;
    }

    dos_interrupt_enable(pri);

    return DOS_OK;
}




