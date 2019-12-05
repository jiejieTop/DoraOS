/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-08-07 23:38:28
 * @LastEditTime: 2019-12-05 23:25:05
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <salof.h>

#define DOS_PRINT_DEBUG_ENABLE		1		
#define DOS_PRINT_ERR_ENABLE			0 	
#define DOS_PRINT_INFO_ENABLE			0		


#if DOS_PRINT_DEBUG_ENABLE
#define DOS_PRINT_DEBUG(fmt, args...)   do{(dos_salof("\n[DEBUG] >> "), dos_salof(fmt, ##args));}while(0)
#else
#define DOS_PRINT_DEBUG(fmt, ...)	     
#endif

#if DOS_PRINT_ERR_ENABLE
#define DOS_PRINT_ERR(fmt, ...)     do{(dos_salof("\n[ERR] >> "), dos_salof(fmt, ##args));}while(0)     
#else
#define DOS_PRINT_ERR(fmt, ...)	       
#endif

#if DOS_PRINT_INFO_ENABLE
#define DOS_PRINT_INFO(fmt, ...)    do{(dos_salof("\n[INFO] >> "), dos_salof(fmt, ##args));}while(0)  
#else
#define DOS_PRINT_INFO(fmt, ...)	       
#endif

/**@} */

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
#endif


typedef enum 
{
	ASSERT_ERR = 0,								
	ASSERT_SUCCESS = !ASSERT_ERR	
} Assert_ErrorStatus;

typedef enum 
{
	FALSE = 0,		
	TRUE = !FALSE	
}bool;

#endif /* __DEBUG_H_ */

