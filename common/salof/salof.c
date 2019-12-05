/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-08-07 23:38:28
 * @LastEditTime: 2019-12-05 23:08:56
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include <salof.h>
#include <format.h>
#include <fifo.h>
#include <stdio.h>

#ifndef DOS_SALOF_BUFF_SIZE
    #define     DOS_SALOF_BUFF_SIZE     (256U)
#endif 
#ifndef DOS_SALOF_FIFO_SIZE
    #define     DOS_SALOF_FIFO_SIZE     (2048U)
#endif 

#if DOS_USE_SALOF
dos_fifo_t Dos_Salof_Fifo = DOS_NULL;
#endif

dos_char buff[DOS_SALOF_BUFF_SIZE];
 
dos_void dos_salof_init(dos_void)
{
#if DOS_USE_SALOF
    Dos_Salof_Fifo = dos_fifo_create(DOS_SALOF_FIFO_SIZE);
 #endif
}


dos_void dos_salof(const dos_char *fmt, ...)
{
    va_list args;
    dos_int32 len;
    // dos_char buff[DOS_SALOF_BUFF_SIZE];
    va_start(args, fmt);

    len = dos_format_nstr(buff, DOS_SALOF_BUFF_SIZE - 1, fmt, args);

    if(len > DOS_SALOF_BUFF_SIZE)
        len = DOS_SALOF_BUFF_SIZE - 1;

#if DOS_USE_SALOF
    dos_fifo_write(Dos_Salof_Fifo, buff, len, 100);
#else
    dos_salof_out(buff, len);
#endif

  va_end(args);
}

dos_int32 dos_salof_out(dos_char *buf, dos_int32 len)
{
    return send_buff(buf, len);
}






