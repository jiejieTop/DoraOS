/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-15 23:56:23
 * @LastEditTime: 2019-12-05 23:04:57
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


#define     QUEUE_READ      0
#define     QUEUE_WRITE     1

struct dos_queue {
    dos_uint32      queue_len;
    dos_size        queue_size;
    dos_uint8       *queue_head_ptr;       /** head pointer  */
    dos_uint8       *queue_tail_ptr;       /** tail pointer  */
    dos_uint16      queue_rw_count[2];    /** write count / read count */
    dos_uint8       *queue_rw_ptr[2];   /** write pointer / read pointer */
    dos_task_list_t  queue_pend_list[2];     /** task pend list, 28 byte *2 */
};
typedef struct dos_queue * dos_queue_t;

dos_queue_t dos_queue_create(dos_uint16 len, dos_uint16 size);
dos_err dos_queue_delete(dos_queue_t queue);
dos_err dos_queue_read(dos_queue_t queue, dos_void *buff, dos_size size, dos_uint32 timeout);
dos_err dos_queue_write(dos_queue_t queue, dos_void *buff, dos_size size, dos_uint32 timeout);



#endif
