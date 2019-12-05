/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-08-07 23:38:28
 * @LastEditTime: 2019-12-05 23:02:44
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _FIFO_H_
#define _FIFO_H_
#include <dos_def.h>
#include <mutex.h>

#define     FIFO_READ   0
#define     FIFO_WRITE    1

struct dos_fifo {
    dos_uint32      size;       /* fifo size */
    dos_uint32      in;         /* data input pointer (in % size) */
    dos_uint32      out;        /* data output pointer (out % size) */
    dos_mutex_t     mutex[2];   /* mutex */
    dos_void        *buffer;    /* data area */
};
typedef struct dos_fifo * dos_fifo_t;

dos_fifo_t dos_fifo_create(dos_size size);
dos_uint32 dos_fifo_write(dos_fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout);
dos_uint32 dos_fifo_read(dos_fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout);
dos_uint32 dos_fifo_read_able(dos_fifo_t fifo);
dos_uint32 dos_fifo_write_able(dos_fifo_t fifo);

#endif // !_FIFO_H_

