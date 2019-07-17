/** synchronous asynchronous log output framework */

#include <salof.h>
#include <format.h>
#include <fifo.h>
#include <stdio.h>

Dos_Fifo_t Dos_Salof_Fifo = DOS_NULL;


dos_void Dos_SalofInit(dos_void)
{
    Dos_Salof_Fifo = Dos_FifoCreate(DOS_SALOF_FIFO_SIZE);
}


dos_void Dos_Salof(const dos_char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
#if DOS_USE_SALOF
    dos_int32 len;
    dos_char buff[DOS_SALOF_BUFF_SIZE];

    len = Dos_FormatNStr(buff, DOS_SALOF_BUFF_SIZE - 1, fmt, args);

    if(len > DOS_SALOF_BUFF_SIZE)
        len = DOS_SALOF_BUFF_SIZE - 1;
    
    Dos_FifoWrite(Dos_Salof_Fifo, buff, len, 100);
#else
    Dos_FormatNStr(DOS_NULL, 0, fmt, args);
#endif

  va_end(args);
}






