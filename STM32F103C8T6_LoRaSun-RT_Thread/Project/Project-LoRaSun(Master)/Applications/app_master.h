
/******************************************************************************
*
* Copyright (c) 2024 艺大师
* 本项目开源文件遵循GPL-v3协议
* 
* 文章专栏地址:https://blog.csdn.net/ypp240124016/category_12834955
* 项目开源地址:https://github.com/WalleFarm/LoRaSun
* 协议栈原理专利:CN110572843A
*
* 测试套件采购地址:https://duandianwulian.taobao.com/
*
* 作者:艺大师
* 博客主页:https://blog.csdn.net/ypp240124016?type=blog
* 交流QQ群:701889554  (资料文件存放)
* 微信公众号:端点物联 (即时接收教程更新通知)
*
* 所有学习资源合集:https://blog.csdn.net/ypp240124016/article/details/143068017
*
* 免责声明:本项目所有资料仅限于学习和交流使用,请勿商用.
*
********************************************************************************/

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
  GW01_CMD_SET_CFG,//配置起始频段和运行模式
  GW01_CMD_SET_BROAD,//手动触发广播

}Gw01Cmd;

void app_master_thread_entry(void *parameter); 

u16 app_server_recv_parse(u8 cmd_type, u8 *buff, u16 len);
void app_aep_recv_parse(char *topic, u8 *in_buff, u16 in_len);
void app_master_send_status(void);






#endif
