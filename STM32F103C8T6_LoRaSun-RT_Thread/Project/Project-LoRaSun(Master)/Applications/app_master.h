
#ifndef __APP_MASTER_H__
#define __APP_MASTER_H__

#include "nwk_master.h" 
#include "drv_uart.h"

typedef enum
{
  GW01_CMD_DATA=0,//数据包

  GW01_CMD_NODE_DATA=10,//节点数据,类似于透传
  GW01_CMD_NODE_LIST,//该网关下的节点列表

  
  GW01_CMD_NODE_DOWN=128,//节点下行数据

}Gw01Cmd;

void app_master_thread_entry(void *parameter); 

u16 app_server_recv_parse(u8 cmd_type, u8 *buff, u16 len);






#endif
