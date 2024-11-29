/******************************************************************************
*
* Copyright (c) 2024 小易
* 本项目开源文件遵循GPL-v3协议
* 
* 文章专栏地址:https://blog.csdn.net/ypp240124016/category_12834955
* github主页:      https://github.com/WalleFarm
* LoRaSun开源地址: https://github.com/WalleFarm/LoRaSun
* M2M-IOT开源地址: https://github.com/WalleFarm/M2M-IOT
* 协议栈原理专利:CN110572843A (一种基于LoRa无线模块CAD模式的嗅探方法及系统)
*
* 测试套件采购地址:https://duandianwulian.taobao.com/
*
* 作者:小易
* 博客主页:https://blog.csdn.net/ypp240124016?type=blog
* 交流QQ群:701889554  (资料文件存放)
* 微信公众号:端点物联 (即时接收教程更新通知)
*
* 所有学习资源合集:https://blog.csdn.net/ypp240124016/article/details/143068017
*
* 免责声明:本项目所有资料仅限于学习和交流使用,请勿商用.
*
********************************************************************************/

#ifndef __APP_NODE_H__
#define __APP_NODE_H__
#include "nwk_node.h" 
#include "drv_uart.h"
#include "app_oled96.h"

typedef struct
{
  u32 gw_sn;
  u8 base_freq_ptr;
  u8 wireless_num;
}AppNodeGwSaveStruct;

typedef struct
{
  u32 node_sn;
  AppNodeGwSaveStruct gw_save_list[NWK_GW_NUM];
  u16 wake_period;
  u16 crcValue;
}AppNodeSaveStruct;

typedef struct
{
  u8 temp;
//  DrvOLED96WorkStruct oled96_work;
}AppNodeWorkStruct;



void app_node_set_sn(u32 node_sn);
void app_node_set_wake_period(u16 wake_period);

void app_node_send_status(void);
void app_node_thread_entry(void *parameter); 

#endif


















