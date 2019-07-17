#ifndef _DOS_LIB_H_
#define _DOS_LIB_H_

#include <dos_def.h>

dos_void *memset(dos_void* dst,dos_int32 val, dos_size count);
dos_void *memcpy(dos_void *dst, const dos_void *src, dos_size len);
 
#endif // !_DOS_LIB_H_


