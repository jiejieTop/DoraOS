/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-13 10:48:52
 * @LastEditTime: 2019-12-13 11:52:23
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _CMD_H_
#define _CMD_H_

#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  __attribute__((section(x)))
    #define CMD_USED                    __attribute__((used))

#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  @ x
    #define CMD_USED                    __root

#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define SECTION(x)                  __attribute__((section(x)))
    #define CMD_USED                    __attribute__((used))
#else
    #error "not supported tool chain..."
#endif


typedef void (*cmd_handler)(void);

typedef struct cmd {
 	const char*     cmd;
	cmd_handler     handler;
} cmd_t;

#define REGISTER_CMD(cmd, handler)                             \
    const char _register_##cmd##_handler[] = #cmd;              \
    CMD_USED const cmd_t _register_##cmd SECTION("CMDS")=       \
    {                                                           \
        _register_##cmd##_handler,                              \
        (cmd_handler)&handler                                   \
    };

void cmd_init(void);
void cmd_parsing(char *str);

#endif

