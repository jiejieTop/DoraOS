#ifndef _FIFO_H_
#define _FIFO_H_
#include <dos_def.h>
#include <mutex.h>

#define			FIFO_READ		0
#define			FIFO_WRITE		1

struct Dos_Fifo
{
	dos_uint32      Size;       /* fifo size */
	dos_uint32      In;         /* Data input pointer (in % size) */
	dos_uint32      Out;        /* Data output pointer (out % size) */
	Dos_Mutex_t     Mutex[2];   /* Mutex */       
	dos_void        *Buffer;    /* data area */
};
typedef struct Dos_Fifo * Dos_Fifo_t;

Dos_Fifo_t Dos_FifoCreate(dos_size size);
dos_uint32 Dos_FifoWrite(Dos_Fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout);
dos_uint32 Dos_FifoRead(Dos_Fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout);
dos_uint32 Dos_Fifo_ReadAble(Dos_Fifo_t fifo);
dos_uint32 Dos_Fifo_WriteAble(Dos_Fifo_t fifo);

#endif // !_FIFO_H_

