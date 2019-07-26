#ifndef __DOS_DEF_H
#define __DOS_DEF_H

/************************************************************
  * @brief   dos_def.h
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    
  ***********************************************************/

//#include "include.h"

/* exact-width unsigned integer types */
typedef unsigned char                   dos_uint8;
typedef unsigned short                  dos_uint16;
typedef unsigned int                    dos_uint32;
typedef signed char                     dos_int8;
typedef signed short                    dos_int16;
typedef signed int                      dos_int32;

typedef unsigned int                    dos_size;

typedef float                           dos_float;
typedef double                          dos_double;
typedef char                            dos_char;

//typedef unsigned long                   dos_uint32;
//typedef signed long                     dos_int32;

typedef unsigned long long              dos_uint64;	/* 64-bit signed integer     */
typedef signed long long                dos_int64;

typedef void                            dos_void;

typedef unsigned int                    dos_bool;
typedef dos_int32              			    dos_err;       /**< Type for error number */

/* minimum values of exact-width signed integer types */
#define DOS_INT8_MIN                   -128
#define DOS_INT16_MIN                -32768
#define DOS_INT32_MIN          	(~0x7fffffff)   			/* -2147483648 is unsigned */
#define DOS_INT64_MIN  		dos_int64(~0x7fffffffffffffff) /* -9223372036854775808 is unsigned */

/* maximum values of exact-width signed integer types */
#define DOS_INT8_MAX                    127
#define DOS_INT16_MAX                32767
#define DOS_INT32_MAX            2147483647
#define DOS_INT64_MAX  		dos_int64(9223372036854775807)

/* maximum values of exact-width unsigned integer types */
#define DOS_UINT8_MAX                   255
#define DOS_UINT16_MAX                65535
#define DOS_UINT32_MAX           4294967295u
#define DOS_UINT64_MAX 		dos_uint64(18446744073709551615)

#define DOS_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define DOS_MIN(a,b) (((a) < (b)) ? (a) : (b))

#ifndef DOS_FALSE
#define DOS_FALSE                   ((dos_bool)0)
#endif

#ifndef DOS_TRUE
#define DOS_TRUE                    ((dos_bool)1)
#endif

#ifndef DOS_NULL
#define DOS_NULL                    ((dos_void *)0)
#endif

#ifdef DOS_YES
#undef DOS_YES
#endif
#define DOS_YES                     (1)

#ifdef  DOS_NO
#undef  DOS_NO
#endif
#define DOS_NO                      (0)


#ifndef DOS_OK
#define DOS_OK                      (0U)
#endif

#ifndef DOS_NOK
#define DOS_NOK                     (1U)
#endif

#define DOS_FAIL                    (1)
#define DOS_ERROR                   (UINT32)(-1)
#define DOS_INVALID                 (UINT32)(-1)

#define asm                         __asm
#ifdef typeof
#undef typeof
#endif
#define typeof                      __typeof__



#endif /* __REDEF_H */


/********************************END OF FILE***************************************/

