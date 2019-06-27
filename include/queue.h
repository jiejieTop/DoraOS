#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "dos_def.h"
#include <mem.h>
/* debug */
#include <debug.h>

struct Dos_Queue
{
  dos_uint16    QueueLen;
  dos_uint16    QueueSize;
  dos_uint16    QueueRWCnt[2];
  
};

typedef struct Dos_Queue * Dos_Queue_t;






#endif
