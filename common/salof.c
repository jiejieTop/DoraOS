/** synchronous asynchronous log output framework */

#include <salof.h>
#include <format.h>
#include <fifo.h>
#include <stdio.h>


Dos_Fifo_t Dos_Salof_Fifo = DOS_NULL;
dos_char buff[DOS_SALOF_BUFF_SIZE];

dos_void Dos_SalofInit(dos_void)
{
    Dos_Salof_Fifo = Dos_FifoCreate(DOS_SALOF_FIFO_SIZE);
}


dos_void Dos_Salof(const dos_char *fmt, ...)
{
    va_list args;
    dos_int32 len;
    // dos_char buff[DOS_SALOF_BUFF_SIZE];
    va_start(args, fmt);

    len = Dos_FormatNStr(buff, DOS_SALOF_BUFF_SIZE - 1, fmt, args);

    if(len > DOS_SALOF_BUFF_SIZE)
        len = DOS_SALOF_BUFF_SIZE - 1;

#if DOS_USE_SALOF
    Dos_FifoWrite(Dos_Salof_Fifo, buff, len, 100);
#else
    Dos_SalofOut(buff, len);
#endif

  va_end(args);
}

dos_int32 Dos_SalofOut(dos_char *buf, dos_int32 len)
{
    return SendBuff(buf, len);
}






