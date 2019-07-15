#include <fifo.h>
#include <string.h>

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

        fifo->Mutex[FIFO_READ] = Dos_MutexCreate();
        if(DOS_NULL == fifo->Mutex[FIFO_READ])
        {
            Dos_MemFree(fifo);  
            return DOS_NULL;
        }

        fifo->Mutex[FIFO_WRITE] = Dos_MutexCreate();
        if(DOS_NULL == fifo->Mutex[FIFO_WRITE])
        {
            Dos_MemFree(fifo);
            Dos_MutexDelete(fifo->Mutex[FIFO_READ]);
            return DOS_NULL;
        }

        fifo->Size = size;
        fifo->In = 0;
        fifo->Out = 0;

        return fifo;
    }

    return DOS_NULL;
}

dos_uint32 Dos_FifoWrite(Dos_Fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout)
{
    dos_err err, l;

    if((!fifo) || (!buff) || (!len))
        return 0;

    err = Dos_MutexPend(fifo->Mutex[FIFO_WRITE], timeout);
    if(err == DOS_NOK)
        return 0;

    len = DOS_MIN(len, (fifo->Size - fifo->In + fifo->Out));

    l = DOS_MIN(len, (fifo->Size - (fifo->In & (fifo->Size -1))));
    memcpy(((dos_uint8 *)fifo->Buffer + (fifo->In & (fifo->Size -1))), buff, l);
    memcpy(fifo->Buffer, (dos_uint8 *)buff + l, len - l);

    fifo->In += len;

    Dos_MutexPost(fifo->Mutex[FIFO_WRITE]);

    return len;
}

dos_uint32 Dos_FifoRead(Dos_Fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout)
{
    dos_err err, l;

    if((!fifo) || (!buff) || (!len))
        return 0;

    err = Dos_MutexPend(fifo->Mutex[FIFO_READ], timeout);
    if(err == DOS_NOK)
        return 0;

    len = DOS_MIN(len, fifo->In - fifo->Out);

    l = DOS_MIN(len, (fifo->Size - (fifo->Out & (fifo->Size -1))));
    memcpy(buff, ((dos_uint8 *)fifo->Buffer + (fifo->Out & (fifo->Size -1))), l);
    memcpy((dos_uint8 *)buff + l, fifo->Buffer, len - l);

    fifo->Out += len;

    Dos_MutexPost(fifo->Mutex[FIFO_READ]);

    return len;
}

dos_uint32 Dos_Fifo_ReadAble(Dos_Fifo_t fifo)
{
    if(DOS_NULL == fifo)
        return 0;

    else if(fifo->In == fifo->Out)
        return 0;

    else if(fifo->In > fifo->Out)
        return (fifo->In - fifo->Out);
    
    return (fifo->Size - (fifo->Out - fifo->In));
}

dos_uint32 Dos_Fifo_WriteAble(Dos_Fifo_t fifo)
{
    return (fifo->Size - Dos_Fifo_ReadAble(fifo));
}
