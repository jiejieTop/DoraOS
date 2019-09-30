#ifndef _SYS_H_
#define _SYS_H_

#include "dos_def.h"
#include <list.h>
#include <port.h>
#include <dos_config.h>

/** 

 oooooooooo.                                      .oooooo.     .ooooooo.   \r\n \
 `888'   `Y8b                                    d8P'  `Y8b   d8P'    `Y8  \r\n \
  888      888   .ooooo.   oooo d8b  .coooo.    888      888  Y88bo.       \r\n \
  888      888  d88' '88b  `888''8P  `P  )88b   888      888   'Yl8888o.   \r\n \
  888      888  888   888   888       .oP''88   888      888       'Y88lb  \r\n \
  888     d88'  888   888   888      d8(   88   `88b    d88'  oo     .d8P  \r\n \
 o888bood8P'    `Y8bod8P'  d888b     `Y888''8o.   'Y8bood8P'   '8888888P'  \r\n ";


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

*/


void dos_system_init(void);
void dos_scheduler(void);
void dos_scheduler_lock(void);
void dos_scheduler_unlock(void);
dos_bool dos_scheduler_is_lock(void);
dos_bool dos_system_is_running(void);
void dos_set_system_status(dos_uint8 status);
dos_bool dos_is_in_interrupt(dos_void);
void dos_interrupt_enter(void);
void dos_interrupt_leave(void);

#endif // !_SYS_H_

