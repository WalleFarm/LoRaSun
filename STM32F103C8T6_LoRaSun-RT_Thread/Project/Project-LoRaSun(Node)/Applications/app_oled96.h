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

#ifndef __APP_OLED96_H__
#define __APP_OLED96_H__

#include "drv_oled96.h"

typedef struct
{
  u32 node_sn;
  u16 wake_period;
  s16 rssi;
  s8 snr;
  u16 total_cnts, ok_cnts;
  
  u32 gw_sn;
  u8 freq_ptr, run_mode;
  u8 join_state;
  char debug_str[200];
}AppOLEDShowNodeStruct;//要显示的信息

void app_oled96_init(void);
 
void app_oled96_thread_entry(void *parameter);


void app_oled96_show_start(void);
void app_oled96_show_node(u32 node_sn, u16 period);
void app_oled96_show_signal(s16 rssi, s8 snr);
void app_oled96_show_time(void);
void app_oled96_show_tx_total(u16 total_cnts, u16 ok_cnts);
void app_oled96_show_gw_info(u32 gw_sn, u8 freq_ptr, u8 run_mode, u8 join_state);
void app_oled96_show_version(void);
void app_oled96_show_debug(char *str);
void printf_oled(char const *const format, ...);

void app_oled96_thread_entry(void *parameter);


#endif



