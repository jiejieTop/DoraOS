#include <dos_def.h>
#include <task.h>
#include "include.h"

#ifndef DOS_MAX_PRIORITY
#define DOS_MAX_PRIORITY		32
#endif 

static dos_uint32 Dos_Task_Priority;
DOS_DList_t Dos_TaskPriority_List[DOS_MAX_PRIORITY];



DOS_TaskCB_t volatile Dos_CurrentTCB = NULL;

dos_uint32 Dos_TickCount = 0U;

static void Dos_Create_IdleTask(void);


static void Dos_TaskPriority_List_Init(void)
{
  dos_uint8 i;
  Dos_Task_Priority = 0;
  for(i=0; i<DOS_MAX_PRIORITY; i++)
  {
    Dos_DListInit(&Dos_TaskPriority_List[i]); 
  }
}

static void Dos_Inser_TaskPriority_List(DOS_TaskCB_t dos_taskcb)
{
  Dos_Task_Priority |= (0x01 << dos_taskcb->Priority);
  printf("Dos_Task_Priority = %#x\n",Dos_Task_Priority);
  Dos_DListInser(&Dos_TaskPriority_List[dos_taskcb->Priority],&dos_taskcb->TaskList);
}


/**
 * @brief       NULL  
 * @param[in]   NULL
 * @param[out]  NULL
 * @return      NULL
 * @author      jiejie
 * @github      https://github.com/jiejieTop
 * @date        2018-xx-xx
 * @version     v1.0
 * @note        nados system init
 */
void Dos_SystemInit(void)
{
  /* system memheap init */
  Dos_MemHeap_Init();
  
  /* init task priority list */
  Dos_TaskPriority_List_Init();
  
  Dos_Create_IdleTask();

}


void prvInitialiseNewTask(DOS_TaskCB_t dos_taskcb)
{
	/* 获取栈顶地址 */
	dos_taskcb->TopOfStack = (dos_void *)((dos_uint32)dos_taskcb->StackAddr + (dos_uint32)(dos_taskcb->StackSize - 1));
	
  /* 向下做8字节对齐 */
	dos_taskcb->TopOfStack = (dos_void *)((( uint32_t)dos_taskcb->TopOfStack) & (~((dos_uint32 )0x0007)));	
  
  dos_taskcb->StackPoint = Dos_StackInit( dos_taskcb->TopOfStack,
                                          dos_taskcb->TaskEntry,
                                          dos_taskcb->Parameter);
}


/**
 * This function will create a task
 * @param[in]  task       the task to be created
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  pri        the prio of task
 * @param[in]  ticks      the time slice if there are same prio task
 * @param[in]  stack_buf  the start address of task stack
 * @param[in]  stack      the size of thread stack
 * @param[in]  entry      the entry function of task
 * @param[in]  autorun    the autorunning flag of task
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
DOS_TaskCB_t Dos_TaskCreate(const dos_char *dos_name,
                            void (*dos_task_entry)(void *dos_param),
                            void * const dos_param,
                            dos_uint32 dos_stack_size,
                            dos_uint16 dos_priority)
{
  DOS_TaskCB_t dos_taskcb;
  dos_void *dos_stack;
  
  dos_taskcb = (DOS_TaskCB_t)Dos_MemAlloc(sizeof(struct DOS_TaskCB));
  if(dos_taskcb != DOS_NULL)
  {
    
    dos_stack = (dos_void *)Dos_MemAlloc(dos_stack_size);
    if(DOS_NULL == dos_stack)
    {
      DOS_PRINT_ERR("system mem DOS_NULL");
      /* 此处应释放申请到的内存 */
      return DOS_NULL;
    }
    dos_taskcb->StackAddr = dos_stack;
    dos_taskcb->StackSize = dos_stack_size;
  }

  dos_taskcb->TaskEntry = (void *)dos_task_entry;
  dos_taskcb->Parameter = dos_param;
  dos_taskcb->Priority = dos_priority;
  dos_taskcb->TaskName = (dos_char *)dos_name;

  prvInitialiseNewTask(dos_taskcb);       
  
  /* init task list,the list will pend in readylist or pendlist  */
  Dos_DListInit(&(dos_taskcb->TaskList));

  Dos_Inser_TaskPriority_List(dos_taskcb);
  
  return dos_taskcb;
}

void IdleTask(void *Parameter)
{
  while(1)
  {
    ;
  }
}

static void Dos_Create_IdleTask(void)
{
  Dos_TaskCreate( "IdleTask",
                  &IdleTask,
                  DOS_NULL,
                  IDLE_TASK_SIZE,
                  IDLE_TASK_PRIORITY);

}


extern DOS_TaskCB_t task;
extern DOS_TaskCB_t task1;
void Dos_Start( void )
{
    /* 手动指定第一个运行的任务 */
    Dos_CurrentTCB = task;
    Dos_TickCount = 0U;
    /* 启动调度器 */
    if( Dos_StartScheduler() != 0 )
    {
        /* 调度器启动成功，则不会返回，即不会来到这里 */
    }
}

void vTaskSwitchContext( void )
{    
    /* 两个任务轮流切换 */
    if( Dos_CurrentTCB == task )
    {
        Dos_CurrentTCB = task1;
    }
    else
    {
        Dos_CurrentTCB = task;
    }
}






