/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-08-07 23:38:28
 * @LastEditTime: 2019-12-05 23:07:49
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _SALOF_H_
#define _SALOF_H_

#include <dos_def.h>
#include <dos_config.h>

extern int send_buff(char *buf, int len);

dos_void dos_salof_init(dos_void);
dos_void dos_salof(const dos_char *fmt, ...);
dos_int32 dos_salof_out(dos_char *buf, dos_int32 len);


#endif // !_SALOF_H_

