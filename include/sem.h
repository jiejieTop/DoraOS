#ifndef _SEM_H_
#define _SEM_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


#define dos_binary_sem_create(cnt)  dos_sem_create(cnt,1)

struct dos_sem {
    dos_uint32      sem_count;         /** sem count */
    dos_uint32      sem_max_count;      /** sem max count */
    dos_task_list_t  sem_pend_list;        /** task pend list, 28 byte */
};
typedef struct dos_sem * dos_sem_t;


dos_sem_t dos_sem_create(dos_uint32 cnt, dos_uint32 max_cnt);
dos_err dos_sem_delete(dos_sem_t sem);
dos_err dos_sem_pend(dos_sem_t sem, dos_uint32 timeout);
dos_err dos_sem_post(dos_sem_t sem);

#endif

