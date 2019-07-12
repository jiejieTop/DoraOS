#include <swtmr.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>
#include <task.h>
#include <port.h>
#include <queue.h>


static Dos_Queue_t _Dos_SwtmrQueue = DOS_NULL;
static DOS_TaskCB_t _Dos_SwtmrTCB = DOS_NULL;

static struct Dos_Swtmr _Dos_Swtmr_Start1;
static struct Dos_Swtmr _Dos_Swtmr_Start2;
static struct Dos_Swtmr *_Dos_Swtmr_OverFlowPtr;
static struct Dos_Swtmr *_Dos_SwtmrPtr;



static void _Dos_SwtmrStart(Dos_Swtmr_t swtmr)
{
    Dos_Swtmr_t swtmr_item;
    dos_uint32 cur_time;

    cur_time = Dos_Get_Tick();

    swtmr->WakeTime = cur_time + swtmr->Timeout;

    if(swtmr->WakeTime < cur_time)  /** overflow */
    {
        swtmr_item = _Dos_Swtmr_OverFlowPtr;
    }

    else if(swtmr->WakeTime == 0xFFFFFFFF)  /** Insert the start of the overflow list */
    {
        swtmr->WakeTime = 0;
        swtmr_item = _Dos_Swtmr_OverFlowPtr;
    }

    else    /** at normal value */
    {
        for(swtmr_item = _Dos_SwtmrPtr; 
            (swtmr_item->Next->WakeTime) <= swtmr->WakeTime; 
            swtmr_item = swtmr_item->Next);
    }

    swtmr->Next = swtmr_item->Next;
    swtmr->Next->Prev = swtmr;
    swtmr->Prev = swtmr_item;
    swtmr_item->Next = swtmr;

    swtmr->Status = DOS_SWTMR_STATUS_RUNNING;
}

static void _Dos_SwtmrStop(Dos_Swtmr_t swtmr)
{
    if(_Dos_SwtmrPtr->Next == swtmr)
    {
        _Dos_SwtmrPtr->Next = swtmr->Next;
    }

    swtmr->Prev->Next = swtmr->Next;
    swtmr->Next->Prev = swtmr->Prev;

    swtmr->Status = DOS_SWTMR_STATUS_STOP;
    swtmr->Prev = swtmr;
    swtmr->Next = swtmr;
}


dos_uint32 _Dos_GetSwtmr_WakeTime(void)
{
    return _Dos_SwtmrPtr->Next->WakeTime;
}

Dos_Swtmr_t _Dos_GetSwtmr(void)
{
    return _Dos_SwtmrPtr->Next;
}

dos_void _Dos_SwtmrTimeout_Handle(Dos_Swtmr_t swtmr)
{
    if(swtmr->CallBacke != DOS_NULL)
    {
        swtmr->CallBacke(swtmr->Parameter);
        _Dos_SwtmrStop(swtmr);
        if(swtmr->Mode == Dos_Swtmr_PeriodMode)
        {
            _Dos_SwtmrStart(swtmr);
        }
    }
}


dos_void Swtmr_CallBacke1(dos_void *Parameter)
{
    printf("abc1\n");
}
Dos_Swtmr_t swtmr1;

dos_void Swtmr_CallBacke2(dos_void *Parameter)
{
    printf("abc2\n");
}
Dos_Swtmr_t swtmr2;

dos_void Swtmr_CallBacke3(dos_void *Parameter)
{
    printf("abc3\n");
}
Dos_Swtmr_t swtmr3;


dos_err _Dos_Swtmr_Wait(void)
{
    dos_err err;
    Dos_Swtmr_t wake_swtmr;
    dos_uint32 cur_time, wait_time;
    struct Dos_SwtmrMsg swtmr_msg;
  
    wake_swtmr = _Dos_GetSwtmr();
    cur_time = Dos_Get_Tick();

    if(wake_swtmr == _Dos_SwtmrPtr)
    {
        wait_time = DOS_WAIT_FOREVER;
    }
    else
    {
        wait_time = wake_swtmr->WakeTime-cur_time;
    }

    err = Dos_QueueRead(_Dos_SwtmrQueue, &swtmr_msg, sizeof(struct Dos_SwtmrMsg), wait_time);
    return err;
}


static void _Dos_SwtmrTask(void *Parameter)
{
//    Dos_TaskSleep(100);
    swtmr1 = Dos_SwtmrCreate(1000,Dos_Swtmr_PeriodMode,Swtmr_CallBacke1,DOS_NULL);
    swtmr2 = Dos_SwtmrCreate(2000,Dos_Swtmr_PeriodMode,Swtmr_CallBacke2,DOS_NULL);
    swtmr3 = Dos_SwtmrCreate(500,Dos_Swtmr_PeriodMode,Swtmr_CallBacke3,DOS_NULL);
    _Dos_SwtmrStart(swtmr1);
    // _Dos_SwtmrStart(swtmr2);
    // _Dos_SwtmrStart(swtmr3);
    dos_err err;
    Dos_Swtmr_t wake_swtmr;
    dos_uint32 cur_time;

    for( ; ; )
    {
        err = _Dos_Swtmr_Wait();
        if(err == DOS_NOK)  /** software timer timeout */
        {
            cur_time = Dos_Get_Tick();
            wake_swtmr = _Dos_GetSwtmr();
            while (cur_time >= wake_swtmr->WakeTime)
            {
                _Dos_SwtmrTimeout_Handle(wake_swtmr);
                wake_swtmr = _Dos_GetSwtmr();
                Dos_TaskSleep(20);
            }
        }
        else
        {
            /* code */
        }
        


//        DOS_PRINT_DEBUG("swtmr task running\n");
//        Dos_TaskSleep(1000);
    }
}


dos_void _Dos_Swtmr_ItemInit(Dos_Swtmr_t swtmr)
{
    memset(swtmr, 0, sizeof(struct Dos_Swtmr));
    swtmr->Status = DOS_SWTMR_STATUS_UNUSED;
    swtmr->Prev = (Dos_Swtmr_t)(swtmr);
    swtmr->Next = (Dos_Swtmr_t)(swtmr);
    swtmr->WakeTime = 0xFFFFFFFF;
}

dos_err Dos_SwtmrInit(void)
{
#ifndef DOS_IPC_QUEUQ
    #error "message queue must be turned on when using software timer"
#endif // !DOS_IPC_QUEUQ

    _Dos_SwtmrQueue = Dos_QueueCreate(3, sizeof(struct Dos_SwtmrMsg));
    if(_Dos_SwtmrQueue == DOS_NULL)
    {
        DOS_PRINT_DEBUG("swtmr queue is null\n");
        return DOS_NOK;
    }

    _Dos_SwtmrTCB = Dos_TaskCreate("Swtmr_Task", &_Dos_SwtmrTask, DOS_NULL, 512, 0);
    if(_Dos_SwtmrTCB == DOS_NULL)
    {
        DOS_PRINT_DEBUG("swtmr task is null\n");
        return DOS_NOK;
    }

    _Dos_Swtmr_ItemInit(&_Dos_Swtmr_Start1);
    _Dos_Swtmr_ItemInit(&_Dos_Swtmr_Start2);

    _Dos_SwtmrPtr = &_Dos_Swtmr_Start1;
    _Dos_Swtmr_OverFlowPtr = &_Dos_Swtmr_Start2;

    return DOS_OK;
}



Dos_Swtmr_t Dos_SwtmrCreate(dos_uint32 timeout, dos_uint16 mode, Swtmr_CallBacke cb, dos_void *param)
{
    Dos_Swtmr_t swtmr;

    swtmr = (Dos_Swtmr_t)Dos_MemAlloc(sizeof(struct Dos_Swtmr));
    if(swtmr == DOS_NULL)
    {
        DOS_PRINT_DEBUG("not enough memory to create a software timer\n");
        return DOS_NULL;
    }

    if((cb == DOS_NULL) || (timeout == 0))
    {
        DOS_PRINT_DEBUG("parameter is is invalid\n");
        return DOS_NULL;
    }

    if((!(mode & Dos_Swtmr_ModeMask)) || ((mode & Dos_Swtmr_ModeMask) == Dos_Swtmr_ModeMask))
    {
        DOS_PRINT_DEBUG("parameter is is invalid\n");
        return DOS_NULL;
    }

    swtmr->Status = DOS_SWTMR_STATUS_CREATE;
    swtmr->Prev = swtmr;
    swtmr->Next = swtmr;
    swtmr->WakeTime = 0;
    swtmr->Timeout = timeout;
    swtmr->Mode = mode;
    swtmr->CallBacke = cb;
    swtmr->Parameter = param;

    return swtmr;
}


dos_err Dos_SwtmrDelete(Dos_Swtmr_t swtmr)
{
   dos_err err = DOS_NOK;

   if(DOS_NULL == swtmr)
   {
       DOS_PRINT_ERR("unable to delete, swtmr is null\n");
       return DOS_NOK;
   }

   switch (swtmr->Status)
   {
       case DOS_SWTMR_STATUS_UNUSED:
//            goto out;
           break;
           
       case DOS_SWTMR_STATUS_CREATE:
       case DOS_SWTMR_STATUS_STOP:
//            goto delete;
           break;

       default:
           break;
   }

//stop:
////    _Dos_SwtmrStop(swtmr);

//delete:


//out:
   return err;
}












