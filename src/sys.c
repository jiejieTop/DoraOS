#include <sys.h>
#include <task.h>
#include <swtmr.h>
#include <salof.h>
#include <dos_config.h>

#if DOS_LOGO_PRINT
#define DOS_LOGO_BIG    0
#define DOS_LOGO_SMALL  1

#ifndef DOS_LOGO
#define DOS_LOGO    DOS_LOGO_BIG
#endif // !DOS_LOGO

#if (DOS_LOGO == DOS_LOGO_BIG)
/* This logo contains 2097 characters */
static dos_char _DoraOS_Logo[] = "\r\n \
  Welcome to DoraOS real-time operating system -- jiejie \r\n\n \
  DDDDDDDDDDDDD                                                                     OOOOOOOOO         SSSSSSSSSSSSSSS   \r\n \
  D::::::::::::DDD                                                                OO:::::::::OO     SS:::::::::::::::S  \r\n \
  D:::::::::::::::DD                                                            OO:::::::::::::OO  S:::::SSSSSS::::::S  \r\n \
  DDD:::::DDDDD:::::D                                                          O:::::::OOO:::::::O S:::::S     SSSSSSS  \r\n \
    D:::::D    D:::::D     ooooooooooo    rrrrr   rrrrrrrrr    aaaaaaaaaaaaa   O::::::O   O::::::O S:::::S              \r\n \
    D:::::D     D:::::D  oo:::::::::::oo  r::::rrr:::::::::r   a::::::::::::a  O:::::O     O:::::O S:::::S              \r\n \
    D:::::D     D:::::D o:::::::::::::::o r:::::::::::::::::r  aaaaaaaaa:::::a O:::::O     O:::::O  S::::SSSS           \r\n \
    D:::::D     D:::::D o:::::ooooo:::::o rr::::::rrrrr::::::r          a::::a O:::::O     O:::::O   SS::::::SSSSS      \r\n \
    D:::::D     D:::::D o::::o     o::::o  r:::::r     r:::::r   aaaaaaa:::::a O:::::O     O:::::O     SSS::::::::SS    \r\n \
    D:::::D     D:::::D o::::o     o::::o  r:::::r     rrrrrrr aa::::::::::::a O:::::O     O:::::O        SSSSSS::::S   \r\n \
    D:::::D     D:::::D o::::o     o::::o  r:::::r            a::::aaaa::::::a O:::::O     O:::::O             S:::::S  \r\n \
    D:::::D    D:::::D  o::::o     o::::o  r:::::r           a::::a    a:::::a O::::::O   O::::::O             S:::::S  \r\n \
  DDD:::::DDDDD:::::D   o:::::ooooo:::::o  r:::::r           a::::a    a:::::a O:::::::OOO:::::::O SSSSSSS     S:::::S  \r\n \
  D:::::::::::::::DD    o:::::::::::::::o  r:::::r           a:::::aaaa::::::a  OO:::::::::::::OO  S::::::SSSSSS:::::S  \r\n \
  D::::::::::::DDD       oo:::::::::::oo   r:::::r            a::::::::::aa:::a   OO:::::::::OO    S:::::::::::::::SS   \r\n \
  DDDDDDDDDDDDD            ooooooooooo     rrrrrrr             aaaaaaaaaa  aaaa     OOOOOOOOO       SSSSSSSSSSSSSSS     \r\n ";
#else
/* This logo contains 573 characters */
static dos_char _DoraOS_Logo[] = "\r\n \
 Welcome to DoraOS real-time operating system -- jiejie \r\n\n \
 oooooooooo.                                      .oooooo.     .ooooooo.   \r\n \
 `888'   `Y8b                                    d8P'  `Y8b   d8P'    `Y8  \r\n \
  888      888   .ooooo.   oooo d8b  .coooo.    888      888  Y88bo.       \r\n \
  888      888  d88' '88b  `888''8P  `P  )88b   888      888   'Yl8888o.   \r\n \
  888      888  888   888   888       .oP''88   888      888       'Y88lb  \r\n \
  888     d88'  888   888   888      d8(   88   `88b    d88'  oo     .d8P  \r\n \
 o888bood8P'    `Y8bod8P'  d888b     `Y888''8o.  'Y8bood8P'   '8888888P'  \r\n ";
#endif

#ifdef DOS_LOGO
static void _Dos_PrintfLogo(void)
{
    Dos_SalofOut(_DoraOS_Logo,  sizeof(_DoraOS_Logo));
}
#endif 

#endif

void Dos_SystemInit(void)
{
#if DOS_LOGO_PRINT
    /** printf DoraOS logo */
    _Dos_PrintfLogo();
#endif
  
    /* system memheap init */
    Dos_MemHeap_Init();

    Dos_SalofInit();

    Dos_TaskInit();
  
#if DOS_SWTMR
    Dos_SwtmrInit();
#endif
}


