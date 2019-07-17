/**
 * format: printf("\033[background color; font color m string \033[0m" );
 * E.g:
 *  printf("\033[47;31mThis is a color test.\033[0m");
 * 
 * Partial color code:
 * background color: 40--49
 *      40: Black
 *      41: Red                   
 *      42: Green                 
 *      43: Yellow                
 *      44: Blue                  
 *      45: Purple                
 *      46: Dark Green            
 *      47: White                 
 * font color: 30--39
 *      30: Black
 *      31: Red
 *      32: Green
 *      33: Yellow
 *      34: Blue
 *      35: Purple
 *      36: Dark Green
 *      37: White
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <salof.h>
#include <dos_config.h>

//#define     BGC_BLACK   40      /** background color */
//#define     BGC_RED     41
//#define     BGC_GREEN   42
//#define     BGC_YELLOW  43
//#define     BGC_BLUE    44
//#define     BGC_PURPLE  45
//#define     BGC_DARK    46
//#define     BGC_WHITE   47

#define     FC_BLACK    30       /** font color */
#define     FC_RED      31
#define     FC_GREEN    32
#define     FC_YELLOW   33
#define     FC_BLUE     34
#define     FC_PURPLE   35
#define     FC_DARK     36
#define     FC_WHITE    37


#ifndef DOS_LOG_COLOR
    #define DOS_LOG_COLOR  (0U)
#endif 

#if DOS_LOG_COLOR
#define DOS_LOG_S(l, c) Dos_Salof("\033\n["#c"m["#l"] >> ")
#define DOS_LOG_E   Dos_Salof("\033[0m")  
#else
#define DOS_LOG_S(l, c) Dos_Salof("\n["#l"] >> ")
#define DOS_LOG_E       
#endif

#define DOS_LOG_LINE(l, c, fmt, ...)        \
    do                                      \
    {                                       \
        DOS_LOG_S(l, c);                    \
        Dos_Salof(fmt, ##__VA_ARGS__);      \
        DOS_LOG_E;                          \
    }                                       \
    while (0)


#define DOS_BASE_LEVEL  (0)

#define DOS_ERR_LEVEL   (DOS_BASE_LEVEL + 1)

#define DOS_WARN_LEVEL  (DOS_ERR_LEVEL + 1)

#define DOS_INFO_LEVEL  (DOS_WARN_LEVEL + 1)

#define DOS_DEBUG_LEVEL (DOS_INFO_LEVEL + 1)

#ifndef DOS_LOG_LEVEL
    #define DOS_LOG_LEVEL   DOS_DEBUG_LEVEL
#endif // !DOS_LOG_LEVEL

#if DOS_LOG_LEVEL < DOS_DEBUG_LEVEL
#define DOS_LOG_DEBUG(fmt, ...)
#else
#define DOS_LOG_DEBUG(fmt, ...)     DOS_LOG_LINE(DEBUG, 0, fmt, ##__VA_ARGS__)
#endif

#if DOS_LOG_LEVEL < DOS_INFO_LEVEL
#define DOS_LOG_INFO(fmt, ...)
#else
#define DOS_LOG_INFO(fmt, ...)      DOS_LOG_LINE(INFO, FC_GREEN, fmt, ##__VA_ARGS__)
#endif

#if DOS_LOG_LEVEL < DOS_WARN_LEVEL
#define DOS_LOG_WARN(fmt, ...)
#else
#define DOS_LOG_WARN(fmt, ...)      DOS_LOG_LINE(WARN, FC_YELLOW, fmt, ##__VA_ARGS__)
#endif

#if DOS_LOG_LEVEL < DOS_ERR_LEVEL
#define DOS_LOG_ERR(fmt, ...)
#else
#define DOS_LOG_ERR(fmt, ...)       DOS_LOG_LINE(ERR, FC_RED, fmt, ##__VA_ARGS__)
#endif

#if DOS_LOG_LEVEL < DOS_BASE_LEVEL
#define DOS_LOG(fmt, ...)
#else
#define DOS_LOG(fmt, ...)           Dos_Salof(fmt, ##__VA_ARGS__)
#endif





#endif // !_LOG_H_

