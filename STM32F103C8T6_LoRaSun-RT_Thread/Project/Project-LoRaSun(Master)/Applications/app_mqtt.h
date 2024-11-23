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

#ifndef __APP_MQTT_H__
#define __APP_MQTT_H__


#include "drv_esp8266.h"  
#include "drv_mqtt.h"
#include "drv_server.h"

void app_mqtt_init(void);
void app_mqtt_put_data(u8 index, u8 *buff, u16 len);
int app_mqtt_send0(u8 *buff, u16 len);
int app_mqtt_recv0(u8 *buff, int len);
int app_mqtt_send1(u8 *buff, u16 len);
int app_mqtt_recv1(u8 *buff, int len);

void app_esp8266_recv(u8 sock_id, u8 *buff, u16 len);
void app_mqtt_recv_parse(u8 index, char *topic, u8 *buff, u16 len);

int app_mqtt_pub_data_m2m(u8 *buff, u16 len); 
int app_mqtt_pub_data_aep(char *topic, u8 *buff, u16 len);


void app_mqtt_thread_entry(void *parameter); 

void app_esp8266_set_ssid_pwd(char *ssid, char *pwd);








#endif

