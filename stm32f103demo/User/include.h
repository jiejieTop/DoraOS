#ifndef __INCLUDE_H
#define __INCLUDE_H

/* stm32必要文件 */
#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"

/* c 标准库 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* 重定义头文件 */
#include "redef.h"

/* debug头文件 */
#include "debug.h"

/* 板级头文件 */
#include "./exti/bsp_exti.h"

#include "./key/bsp_key.h" 

#include "./led/bsp_led.h" 

#include "./usart/bsp_usart.h"

#include "./crc/bsp_crc.h"

#include "./SysTick/bsp_SysTick.h"

#include "./dwt_delay/core_delay.h" 

#include "./i2c/i2c.h"

/* 包含通用处理相关文件 */
#include "./common/data_pack.h"
#include "./common/data_process.h"
#include "./common/ringbuff.h"
#include "./common/button.h"
#include "./common/PID_Control.h"

#endif /* __INCLUDE_H */

/********************************END OF FILE***************************************/

