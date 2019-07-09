#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


#define     QUEUE_READ      0
#define     QUEUE_WRITE     1

struct Dos_Queue
{
  dos_uint16      QueueLen;
  dos_uint16      QueueSize;
  dos_uint8       *QueueHPtr;       /** head pointer  */
  dos_uint8       *QueueTPtr;       /** tail pointer  */
  dos_uint16      QueueRWCnt[2];    /** write count / read count */
  // dos_uint16      QueueRWLock[2];   /** write lock / read lock */
  dos_uint8       *QueueRWPtr[2];   /** write pointer / read pointer */
  Dos_TaskList_t  QueuePend[2];     /** task pend list, 28 byte *2 */
};
typedef struct Dos_Queue * Dos_Queue_t;

Dos_Queue_t Dos_QueueCreate(dos_uint16 len, dos_uint16 size);
dos_err Dos_QueueDelete(Dos_Queue_t queue);
dos_err Dos_QueueRead(Dos_Queue_t queue, void *buff, size_t size, dos_uint32 timeout);
dos_err Dos_QueueWrite(Dos_Queue_t queue, void *buff, size_t size, dos_uint32 timeout);



#endif
