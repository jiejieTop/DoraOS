#ifndef _SEM_H_
#define _SEM_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>

struct Dos_Sem
{
    dos_uint32      SemCnt;         /** sem count */
    dos_uint32      SemMaxCnt;      /** sem max count */
    Dos_TaskList_t  SemPend;        /** task pend list, 28 byte */
};
typedef struct Dos_Sem * Dos_Sem_t;


Dos_Sem_t Dos_SemCreate(dos_uint32 cnt, dos_uint32 max_cnt);
dos_err Dos_SemWait(Dos_Sem_t sem, dos_uint32 timeout);
dos_err Dos_SemPost(Dos_Sem_t sem);

#endif
