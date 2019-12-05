/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-08-07 23:38:28
 * @LastEditTime: 2019-12-05 23:01:35
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <fifo.h>
#include <string.h>

static dos_uint32 _dos_find_last_bit_set(dos_uint32 x)
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

static dos_uint32 _dos_fifo_align(dos_uint32 x)
{
	return (1 << (_dos_find_last_bit_set(x-1)-1));	    //Memory down alignment
}

dos_fifo_t dos_fifo_create(dos_size size)
{
    dos_fifo_t fifo;

    if(0 == size)
    {
        return DOS_NULL;
    }

	if(size&(size - 1))
	{
		size = _dos_fifo_align(size);
	}

    fifo = (dos_fifo_t)dos_mem_alloc((sizeof(struct dos_fifo) + size));
    if(DOS_NULL != fifo)
    {
        fifo->buffer = (dos_uint8 *)fifo + sizeof(struct dos_fifo);

        fifo->mutex[FIFO_READ] = dos_mutex_create();
        if(DOS_NULL == fifo->mutex[FIFO_READ])
        {
            dos_mem_free(fifo);  
            return DOS_NULL;
        }

        fifo->mutex[FIFO_WRITE] = dos_mutex_create();
        if(DOS_NULL == fifo->mutex[FIFO_WRITE])
        {
            dos_mem_free(fifo);
            dos_mutex_delete(fifo->mutex[FIFO_READ]);
            return DOS_NULL;
        }

        fifo->size = size;
        fifo->in = 0;
        fifo->out = 0;

        return fifo;
    }

    return DOS_NULL;
}

dos_uint32 dos_fifo_write(dos_fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout)
{
    dos_err err, l;

    if((!fifo) || (!buff) || (!len))
        return 0;

    err = dos_mutex_pend(fifo->mutex[FIFO_WRITE], timeout);
    if(err == DOS_NOK)
        return 0;

    len = DOS_MIN(len, (fifo->size - fifo->in + fifo->out));

    l = DOS_MIN(len, (fifo->size - (fifo->in & (fifo->size -1))));
    memcpy(((dos_uint8 *)fifo->buffer + (fifo->in & (fifo->size -1))), buff, l);
    memcpy(fifo->buffer, (dos_uint8 *)buff + l, len - l);

    fifo->in += len;

    dos_mutex_post(fifo->mutex[FIFO_WRITE]);

    return len;
}

dos_uint32 dos_fifo_read(dos_fifo_t fifo, dos_void *buff, dos_uint32 len, dos_uint32 timeout)
{
    dos_err err, l;

    if((!fifo) || (!buff) || (!len))
        return 0;

    err = dos_mutex_pend(fifo->mutex[FIFO_READ], timeout);
    if(err == DOS_NOK)
        return 0;

    len = DOS_MIN(len, fifo->in - fifo->out);

    l = DOS_MIN(len, (fifo->size - (fifo->out & (fifo->size -1))));
    memcpy(buff, ((dos_uint8 *)fifo->buffer + (fifo->out & (fifo->size -1))), l);
    memcpy((dos_uint8 *)buff + l, fifo->buffer, len - l);

    fifo->out += len;

    dos_mutex_post(fifo->mutex[FIFO_READ]);

    return len;
}

dos_uint32 dos_fifo_read_able(dos_fifo_t fifo)
{
    if(DOS_NULL == fifo)
        return 0;

    else if(fifo->in == fifo->out)
        return 0;

    else if(fifo->in > fifo->out)
        return (fifo->in - fifo->out);
    
    return (fifo->size - (fifo->out - fifo->in));
}

dos_uint32 dos_fifo_write_able(dos_fifo_t fifo)
{
    return (fifo->size - dos_fifo_read_able(fifo));
}

