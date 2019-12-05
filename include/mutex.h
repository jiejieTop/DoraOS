/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-15 23:56:23
 * @LastEditTime: 2019-12-05 23:04:53
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


struct dos_mutex {
    dos_uint16          mutex_count;     /** pend mutex count */
    dos_uint16          mutex_priority;       /** the initial priority of the tasks */
    dos_task_t          mutex_owner;     /** mutex owner */
    dos_task_list_t     mutex_pend_list;        /** task pend list, 28 byte */
};
typedef struct dos_mutex * dos_mutex_t;


dos_mutex_t dos_mutex_create(void);
dos_err dos_mutex_delete(dos_mutex_t mutex);
dos_err dos_mutex_pend(dos_mutex_t mutex, dos_uint32 timeout);
dos_err dos_mutex_post(dos_mutex_t mutex);

#endif // _MUTEX_H_
