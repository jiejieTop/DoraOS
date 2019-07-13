#ifndef _DEBUG_H_
#define _DEBUG_H_
/************************************************************
  * @brief   debug.h
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    printf debug/err/information
  ***********************************************************/
/**
* @name Debug print 
* @{
*/
 #include <stdio.h>
 
#define DOS_PRINT_DEBUG_ENABLE		0		
#define DOS_PRINT_ERR_ENABLE			1 	
#define DOS_PRINT_INFO_ENABLE			1		


#if DOS_PRINT_DEBUG_ENABLE
#define DOS_PRINT_DEBUG(fmt, args...)   do{(printf("\n[DEBUG] >> "), printf(fmt, ##args));}while(0)
#else
#define DOS_PRINT_DEBUG(fmt, args...)	     
#endif

#if DOS_PRINT_ERR_ENABLE
#define DOS_PRINT_ERR(fmt, args...)     do{(printf("\n[ERR] >> "), printf(fmt, ##args));}while(0)     
#else
#define DOS_PRINT_ERR(fmt, args...)	       
#endif

#if DOS_PRINT_INFO_ENABLE
#define DOS_PRINT_INFO(fmt, args...)    do{(printf("\n[INFO] >> "), printf(fmt, ##args));}while(0)  
#else
#define DOS_PRINT_INFO(fmt, args...)	       
#endif

/**@} */

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
#endif

/* Assert */
#define Dos_AssertCalled(char,int) 	printf("\nError:%s,%d\r\n",char,int)
#define DOS_ASSERT(x)   if((x)==0)  Dos_AssertCalled(__FILE__,__LINE__)
  
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

