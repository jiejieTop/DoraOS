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
Dos_Sem_t Dos_SemCreate(dos_uint32 cnt, dos_uint32 max_cnt)
{
    Dos_Sem_t sem;

    if(cnt > max_cnt)
    {
        DOS_LOG_WARN("the cnt of the semaphore cannot exceed the max cnt\n");
    }

    sem = (Dos_Sem_t)Dos_MemAlloc(sizeof(struct Dos_Sem));
    if(sem == DOS_NULL)
    {
        DOS_LOG_ERR("unable to create sem\n");
        return DOS_NULL;
    }

    memset(sem, 0, sizeof(struct Dos_Sem));

    sem->SemCnt = cnt;
    sem->SemMaxCnt = max_cnt;
    
    /** Initialize semaphores pend list */
    Dos_TaskList_Init(&(sem->SemPend));

    return sem;
}

/**
 * semaphores delete
 * description: You need to set the semaphore pointer to null after deleting the semaphore
 */
dos_err Dos_SemDelete(Dos_Sem_t sem)
{
    if(sem != DOS_NULL)
    {
        if(Dos_TaskList_IsEmpty(&(sem->SemPend)))
        {
            memset(sem,0,sizeof(struct Dos_Sem));
            Dos_MemFree(sem);
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
dos_err Dos_SemWait(Dos_Sem_t sem, dos_uint32 timeout)
{
    dos_uint32 pri;
    DOS_TaskCB_t task;
    
    pri = Dos_Interrupt_Disable();
    
    if(sem == DOS_NULL)
    {
        DOS_LOG_WARN("sem is null\n");
        Dos_Interrupt_Enable(pri);
        return DOS_NOK;
    }

    if(sem->SemCnt > 0)
    {
        sem->SemCnt--;
        Dos_Interrupt_Enable(pri);
        return DOS_OK;
    }

    if((timeout == 0) || (Dos_Scheduler_IsLock()))  /** scheduler is lock */
    {
        Dos_Interrupt_Enable(pri);
        return DOS_NOK;
    }

    if(Dos_ContextIsInt())
    {
        DOS_LOG_ERR("sem wait time is not 0, and the context is in an interrupt\n");
        Dos_Interrupt_Enable(pri);
        return DOS_NOK;
    }

    Dos_TaskWait(&sem->SemPend, timeout);
    Dos_Interrupt_Enable(pri);
    Dos_Scheduler();
    
    task = (DOS_TaskCB_t)Dos_Get_CurrentTCB();
    /** Task resumes running */
    if(task->TaskStatus & DOS_TASK_STATUS_TIMEOUT)
    {
        pri = Dos_Interrupt_Disable();
        DOS_RESET_TASK_STATUS(task, (DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
        DOS_SET_TASK_STATUS(task, DOS_TASK_STATUS_READY);
        Dos_TaskItem_Del(&(task->PendItem));
        DOS_LOG_INFO("waiting for sem timeout\n");
        Dos_Interrupt_Enable(pri);
        return DOS_NOK;
    }

    return DOS_OK;
}

/**
 * semaphores post
 */
dos_err Dos_SemPost(Dos_Sem_t sem)
{
    dos_uint32 pri;
    DOS_TaskCB_t task;
    
    pri = Dos_Interrupt_Disable();

    if(sem == DOS_NULL)
    {
        DOS_LOG_WARN("sem is null\n");
        Dos_Interrupt_Enable(pri);
        return DOS_NOK;
    }
    
    if(sem->SemCnt == sem->SemMaxCnt)
    {
        Dos_Interrupt_Enable(pri);
        return DOS_NOK; /** overflow */
    }

    /** No task is waiting for the semaphore, the number of semaphores ++ */
    if(Dos_TaskList_IsEmpty(&(sem->SemPend)))
    {
        sem->SemCnt++;  
        Dos_Interrupt_Enable(pri);
    }
    else
    {
        task = Dos_GetTCB(&(sem->SemPend));
        Dos_TaskWake(task);
        Dos_Interrupt_Enable(pri);
        Dos_Scheduler();
    }

    return DOS_OK;
}


















