#ifndef _SALOF_H_
#define _SALOF_H_

#include <dos_def.h>
#include <dos_config.h>


#ifndef DOS_SALOF_BUFF_SIZE
    #define     DOS_SALOF_BUFF_SIZE     (256U)
#endif 
#ifndef DOS_SALOF_FIFO_SIZE
    #define     DOS_SALOF_FIFO_SIZE     (4096U)
#endif 

extern int SendBuff(char *buf, int len);

dos_void Dos_SalofInit(dos_void);
dos_void Dos_Salof(const dos_char *fmt, ...);
dos_int32 Dos_SalofOut(dos_char *buf, dos_int32 len);


#endif // !_SALOF_H_

