#ifndef __DATA_PROCESS_H
#define __DATA_PROCESS_H

/************************************************************
  * @brief   __DATA_PROCESS_H
  * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    这个文件是一些处理数据的函数头文件
  ***********************************************************/

#include "include.h"

int32_t Str2Int(const char *nptr);
void Int2Str(int32_t num,char *ptr);
char *StrnCopy(char *dst, const char *src, uint32_t n);

#endif /* __DATA_PROCESS_H */
