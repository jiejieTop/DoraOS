/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "client.h"

#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/api.h"


static void client(void *thread_param)
{
  struct netconn *conn;
  int ret;
  ip4_addr_t ipaddr;
  
  uint8_t send_buf[]= "This is a TCP Client test...\n";
  
  printf("目地IP地址:%d.%d.%d.%d \t 端口号:%d\n\n",      \
          DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3,DEST_PORT);
  
  printf("请将电脑上位机设置为TCP Server.在User/arch/sys_arch.h文件中将目标IP地址修改为您电脑上的IP地址\n\n");
  
  printf("修改对应的宏定义:DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3,DEST_PORT\n\n");
  
  while(1)
  {
    conn = netconn_new(NETCONN_TCP);
    if (conn == NULL)
    {
      PRINT_DEBUG("create conn failed!\n");
      vTaskDelay(10);
      continue;
    }
    
    IP4_ADDR(&ipaddr,DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3);

    ret = netconn_connect(conn,&ipaddr,DEST_PORT);
    if (ret == -1)
    {
        PRINT_DEBUG("Connect failed!\n");
        netconn_close(conn);
        vTaskDelay(10);
        continue;
    }

    PRINT_DEBUG("Connect to server successful!\n");
     
    while (1)
    {
      ret = netconn_write(conn,send_buf,sizeof(send_buf),0);
   
      vTaskDelay(1000);
    }
  }

}

void
client_init(void)
{
  sys_thread_new("client", client, NULL, 512, 4);
}
