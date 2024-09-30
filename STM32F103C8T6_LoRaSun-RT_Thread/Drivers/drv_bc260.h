#ifndef __DRV_BC26_H__
#define __DRV_BC26_H__

#include "drv_uart.h"

typedef enum
{
	BC260_STATE_START=0,//起始状态
	BC260_STAT_INIT, //初始化
	BC260_STAT_CEREG,//网络注册
  
  BC260_STAT_ONENET_INIT,//注册到服务器
  BC260_STAT_ONENET_CONNECT,//连接到服务器
  
  BC260_STAT_AEP_INIT,
  BC260_STAT_AEP_CONNECT,//连接到服务器

	BC260_STAT_OK,//入网成功

}BC260State;

typedef enum
{
  BC260_SLEEP_NO=0,
  BC260_SLEEP_DEEP,
  BC260_SLEEP_LIGHT,
}BC260SleepMode;

typedef enum
{
  BC260_SERVER_ONENET=0,
  BC260_SERVER_CHINANET_IOT,
  BC260_SERVER_CHINANET_AEP,
  BC260_SERVER_ALI,
  
}BC260ServerType;

//typedef struct
//{
//  u8 conn_state;
//  char type[5];//UDP   TCP
//  char dst_addr[30];//目标IP或域名
//  u16 dst_port;
//  u16 heart_time;//心跳周期
//  u32 keep_time;
//}BC260ClientStruct;


typedef struct
{
  u8 state;
  u8 server_type;
  u8 sleep_mode;
  u8 rssi;
	u8 imei_buff[16];
	u8 iccid_buff[21];  
	void (*fun_recv_parse)(u8 *buff, u16 len);//接收处理函数
	UART_Struct *pUART;
}BC260WorkStruct;


void drv_bc260_init(UART_Struct *pUART, u8 server_type, u8 sleep_mode);
void drv_bc260_main(void);
void drv_bc260_send_data(u8 *buff, u16 len);
void drv_bc260_copy_imei(u8 *imei_buff, u16 size);
void drv_bc260_copy_iccid(u8 *iccid_buff, u16 size);
u8 drv_bc260_get_rssi(void);
















#endif
