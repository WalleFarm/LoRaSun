#ifndef __DRV_AIR780E_H__
#define __DRV_AIR780E_H__

#include "drv_uart.h"

#define   AIR780_MAX_LINK_NUM    6

typedef enum
{
	AIR780_STATE_START=0,//起始状态
	AIR780_STAT_INIT, //初始化
	AIR780_STAT_CGREG,//网络注册
	AIR780_STAT_CGACT,//PDP上下文激活
	AIR780_STAT_CGATT0,//附着状态查询
	AIR780_STAT_CGATT1,//附着
	AIR780_STAT_CIICR,//激活移动场景,获取IP地址
	
	AIR780_STAT_OK,//入网成功

}Air780State;

typedef struct
{
  u8 sock_id;
  u8 conn_state;
  char type[5];//UDP   TCP
  char dst_addr[30];//目标IP或域名
  u16 dst_port;
  u16 heart_time;//心跳周期
  u32 keep_time;
}Air780ClientStruct;

typedef struct
{
	u8 state;//模块当前状态
  u8 rssi;//信号强度
	u8 imei_buff[16];
	u8 iccid_buff[20];  
	void (*fun_recv_parse)(u8 sock_id, u8 *buff, u16 len);//接收处理函数
	UART_Struct *pUART;
  float deg_sn, deg_we, hight;
  Air780ClientStruct client_list[AIR780_MAX_LINK_NUM];
  
}Air780WorkStruct;

void drv_air780_init(UART_Struct *pUART);
void drv_air780_set_client(u8 index, char *dst_addr, u16 port, char *type);
void drv_air780_fun_register(void (*fun_recv_parse)(u8 chn, u8 *buff, u16 len));
int drv_air780_send_data(u8 sock_id, u8 *buff, u16 len);
void drv_air780_main(void);

#endif
