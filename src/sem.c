#include <sem.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>
#include <task.h>
#include <port.h>

Dos_Sem_t Dos_SemCreate(dos_uint32 cnt, dos_uint32 max_cnt)
{
    Dos_Sem_t sem;

    if(cnt > max_cnt)
    {
        DOS_PRINT_ERR("the cnt of the semaphore cannot exceed the max cnt\n");
    }

    sem = (Dos_Sem_t)Dos_MemAlloc(sizeof(struct Dos_Sem));
    if(sem == DOS_NULL)
    {
        DOS_PRINT_ERR("sem is null\n");
        return DOS_NULL;
    }

    memset(sem,0,sizeof(struct Dos_Sem));

    sem->SemCnt = cnt;
    sem->SemMaxCnt = max_cnt;
    
    Dos_TaskList_Init(&(sem->SemPend));

    return sem;
}

dos_err Dos_SemWait(Dos_Sem_t sem, dos_uint32 timeout)
{
    DOS_TaskCB_t task;

    if(sem->SemCnt > 0)
    {
        sem->SemCnt--;
        return DOS_OK;
    }

    if(timeout == 0)
    {
        return DOS_NOK;
    }

    if(Dos_ContextIsInt())
    {
        DOS_PRINT_ERR("sem wait time is not 0, and the context is in an interrupt\n");
        return DOS_NOK;
    }

    Dos_TaskWait(&sem->SemPend, timeout);

    task = (DOS_TaskCB_t)Dos_Get_CurrentTCB();
    /** Task resumes running */
    if(task->TaskStatus & DOS_TASK_STATUS_TIMEOUT)
    {
        task->TaskStatus &= (~(DOS_TASK_STATUS_TIMEOUT | DOS_TASK_STATUS_SUSPEND));
        task->TaskStatus |= DOS_TASK_STATUS_READY;
        Dos_TaskItem_Del(&(task->PendItem));
        DOS_PRINT_DEBUG("SEM TIMEOUT\n");
        return DOS_NOK;
    }

    return DOS_OK;
}


dos_err Dos_SemPost(Dos_Sem_t sem)
{
    DOS_TaskCB_t task;
    
    if(sem->SemCnt == sem->SemMaxCnt)
    {
        return DOS_NOK; /** overflow */
    }

    /** No task is waiting for the semaphore, the number of semaphores ++ */
    if(Dos_TaskList_IsEmpty(&(sem->SemPend)))
    {
        sem->SemCnt++;  
    }
    else
    {
        task = Dos_GetTCB(&(sem->SemPend));
        Dos_TaskWake(task);
    }

    return DOS_OK;
}


















