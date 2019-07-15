#ifndef _FIFO_H_
#define _FIFO_H_
#include <dos_def.h>
#include <mutex.h>

struct Dos_Fifo
{
	dos_uint32      Size;       /* fifo size */
	dos_uint32      In;         /* Data input pointer (in % size) */
	dos_uint32      Out;        /* Data output pointer (out % size) */
    Dos_Mutex_t     Mutex;      /* Mutex */       
	dos_void        *Buffer;    /* data area */
};
typedef struct Dos_Fifo * Dos_Fifo_t;

Dos_Fifo_t Dos_FifoCreate(dos_size size);



#endif // !_FIFO_H_