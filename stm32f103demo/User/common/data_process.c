/*
	***********************************************************
  * @brief   data_process.c
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    此文件用于处理数据转换
  ***********************************************************
	*/

/* 头文件 */
#include "include.h"

static int32_t isspace(int32_t x);
static int32_t isdigit(int32_t x);
/************************************************************
  * @brief   Str2Int
  * @param   nptr:输入的字符串
  * @return  int32_t:返回一个整形数（int32）
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    将字符串转换为整形数
  ***********************************************************/
int32_t Str2Int(const char *nptr)
{
    int32_t c;             /* 当前字符 */
    int32_t total;         /* 目前的总和 */
    int32_t sign;          /* 如果'-'，则为负数，否则为正数 */

    /* 跳过空白 */
    while ( isspace((int32_t)(unsigned char)*nptr) )
        ++nptr;

    c = (int32_t)(unsigned char)*nptr++;
    sign = c;          			 /* 保存标志指示 */
    if (c == '-' || c == '+')
        c = (int32_t)(unsigned char)*nptr++;    /* 跳过符号 */

    total = 0;

    while (isdigit(c)) /* 字符串在数字范围 */
    {
        total = 10 * total + (c - '0');     /* 积累数字 */
        c = (int32_t)(unsigned char)*nptr++;   /* 获取下一个字符 */
    }

    if (sign == '-')
        return -total;
    else
        return total;   /* 返回结果 */
}
/************************************************************
  * @brief
  * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    将整形数转换为字符
  ***********************************************************/
void Int2Str(int32_t num,char *ptr)
{
  int i,j,sign;
  char str[7];/* uint32的数字最大为5位，加上正负号与'\0' */
  if((sign = num) < 0)    /* 记录符号 */
    num = -num;           /* 使n成为正数 */
  i=0;
  do {
      str[i++] = num%10 + '0';  /* 取下一个数字 */
  } while((num/=10) > 0);     /* 循环相除 */

  if(sign<0)
    str[i++] = '-'; /* 负数 */
  
  str[i] = '\0';  /* 添加结束'\0' */
  
  for(j=i-1; j>=0; j--) /* 生成的数字是逆序的，要逆序输出 */
    *ptr++ = str[j];
}

char *StrnCopy(char *dst, const char *src, uint32_t n)
{
  if (n != 0)
  {
    char *d = dst;
    const char *s = src;
    do
    {
        if ((*d++ = *s++) == 0)
        {
            while (--n != 0)
                *d++ = 0;
            break;
        }
    } while (--n != 0);
  }
  return (dst);
}


/************************************************************
                以下是内部调用函数
  ***********************************************************/
static int32_t isspace(int32_t x)
{
    if(x==' '||x=='\t'||x=='\n'||x=='\f'||x=='\b'||x=='\r')
        return 1;
    else
        return 0;
}

static int32_t isdigit(int32_t x)
{
    if(x<='9'&&x>='0')
        return 1;
    else
        return 0;
}


