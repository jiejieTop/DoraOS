#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


struct Dos_Mutex
{
    dos_uint16      MutexCnt;     /** pend mutex count */
    dos_uint16      Priority;       /** the initial priority of the tasks */
    DOS_TaskCB_t    MutexOwner;     /** mutex owner */
    Dos_TaskList_t  MutexPend;        /** task pend list, 28 byte */
};
typedef struct Dos_Mutex * Dos_Mutex_t;


Dos_Mutex_t Dos_MutexCreate(void);
dos_err Dos_MutexDelete(Dos_Mutex_t mutex);
dos_err Dos_MutexPend(Dos_Mutex_t mutex, dos_uint32 timeout);
dos_err Dos_MutexPost(Dos_Mutex_t mutex);

#endif // _MUTEX_H_
