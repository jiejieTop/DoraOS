#include <fifo.h>

static dos_uint32 _Dos_Find_Last_BitSet(dos_uint32 x)
{
    dos_uint32 r = 32;

    if (!x)
        return 0;
    if (!(x & 0xffff0000u)) 
    {
        x <<= 16;
        r -= 16;
    }
    if (!(x & 0xff000000u)) 
    {
        x <<= 8;
        r -= 8;
    }
    if (!(x & 0xf0000000u)) 
    {
        x <<= 4;
        r -= 4;
    }
    if (!(x & 0xc0000000u)) 
    {
        x <<= 2;
        r -= 2;
    }
    if (!(x & 0x80000000u)) 
    {
        x <<= 1;
        r -= 1;
    }
    return r;
}

static dos_uint32 _Dos_FifoAlign(dos_uint32 x)
{
	return (1 << (_Dos_Find_Last_BitSet(x-1)-1));	    //Memory down alignment
}

Dos_Fifo_t Dos_FifoCreate(dos_size size)
{
    Dos_Fifo_t fifo;

    if(0 == size)
    {
        return DOS_NULL;
    }

	if(size&(size - 1))
	{
		size = _Dos_FifoAlign(size);
	}

    fifo = (Dos_Fifo_t)Dos_MemAlloc(sizeof(struct Dos_Fifo));
    if(DOS_NULL != fifo)
    {
        fifo->Buffer = (dos_void *)Dos_MemAlloc(size);
        if(DOS_NULL == fifo->Buffer)
        {
            Dos_MemFree(fifo);  
            return DOS_NULL;
        }

        fifo->Mutex = Dos_MutexCreate();
        if(DOS_NULL == fifo->Mutex)
        {
            Dos_MemFree(fifo);  
            return DOS_NULL;
        }

        fifo->Size = size;
        fifo->In = 0;
        fifo->Out = 0;
    }
}

dos_uint32 Dos_FifoWrite(Dos_Fifo_t fifo, dos_void *buff, dos_uint32 len)
{
    
}


