#ifndef _SALOF_H_
#define _SALOF_H_

#include <dos_def.h>
#include <dos_config.h>

#define     DOS_USE_SALOF           (1U)
#define     DOS_SALOF_BUFF_SIZE     (256U)
#define     DOS_SALOF_FIFO_SIZE     (4096U)



dos_void Dos_SalofInit(dos_void);
dos_void Dos_Salof(const dos_char *fmt, ...);



#endif // !_SALOF_H_

