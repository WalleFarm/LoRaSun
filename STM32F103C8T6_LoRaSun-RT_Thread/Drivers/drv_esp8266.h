#ifndef __DRV_ESP8266_H__
#define __DRV_ESP8266_H__

#include "drv_uart.h"

#define   ESP8266_MAX_LINK_NUM    5

typedef enum
{
	ESP8266_STATE_START=0,//起始状态
 	ESP8266_INIT,  //初始化
  ESP8266_WIFI_CONNECT,//连接WIFI
  ESP8266_NET_CFG,//网络配置

	ESP8266_STATE_OK,//状态成功
}Esp8266State;

typedef struct
{
  u8 sock_id;
  u8 conn_state;
  char type[5];//UDP   TCP
  char dst_addr[30];//目标IP或域名
  u16 dst_port;
  u16 heart_time;//心跳周期
  u32 keep_time;
}Esp8266ClientStruct;

typedef struct
{
	u8 state;//模块当前状态
	void (*fun_recv_parse)(u8 sock_id, u8 *buff, u16 len);//接收处理函数
	UART_Struct *pUART;
  
  char sta_ssid[40];
  char sta_passwd[40];
  char ap_ssid[40];
  char ap_passwd[20];  
  
  u16 listen_port;//为0时表示不开启服务器,大于0时开启服务器,但是前提模块开启了AP模式
  Esp8266ClientStruct client_list[ESP8266_MAX_LINK_NUM];
  
}Esp8266WorkStruct;





void drv_esp8266_init(UART_Struct *pUART, char *sta_ssid, char *sta_passwd);
void drv_esp8266_set_uart(UART_Struct *pUART);
void drv_esp8266_set_client(u8 index, char *dst_addr, u16 port, char *type);
void drv_esp8266_set_ap(char *ap_ssid, char *ap_passwd, u16 listen_port);
void drv_esp8266_uart_send(char *send_buff);
void drv_esp8266_fun_register(void (*fun_recv_parse)(u8 sock_id, u8 *buff, u16 len));
void drv_esp8266_reg_process(void);
void drv_esp8266_recv_process(void);
void drv_esp8266_connect_process(void);
void drv_esp8266_close(u8 sock_id);

int drv_esp8266_send_data(u8 sock_id, u8 *buff, u16 len);

void drv_esp8266_main(void);

//int printf_wifi(char const *const format, ...);











#endif

















