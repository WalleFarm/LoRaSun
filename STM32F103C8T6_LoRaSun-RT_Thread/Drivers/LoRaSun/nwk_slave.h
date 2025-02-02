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




#ifndef __NWK_SLAVE_H__
#define __NWK_SLAVE_H__


#include "nwk_bsp.h"

//定义所使用的LoRa芯片模块,只能选一种,其余注释
//#define LORA_SX1278    
#define LORA_SX1268    
//#define LORA_LLCC68    


#if defined(LORA_SX1278)  
  #include "drv_sx1278.h"
  #define  LoRaDevStruct    DrvSx1278Struct

#elif defined(LORA_SX1268)
  #include "drv_sx1268.h"
  #define  LoRaDevStruct    DrvSx1268Struct

#elif defined(LORA_LLCC68)


#endif



typedef enum
{
  NwkSlaveWorkIdel=0,//空闲
  NwkSlaveWorkBroad,//广播
  NwkSlaveWorkRX,//接收
  NwkSlaveWorkTX,//发送

}NwkSlaveState;//从机工作状态

typedef enum
{
  NwkSlaveBroadIdel=0,
  NwkSlaveBroadInit,
  NwkSlaveBroadSniff,
  NwkSlaveBroadTxCheck,
  NwkSlaveBroadExit,

}NwkSlaveBroadState;//从机广播状态


typedef enum
{
  NwkSlaveRxIdel=0,
  NwkSlaveRxInit,
  NwkSlaveRxCadInit,
  NwkSlaveRxCadCheck,
  NwkSlaveRxCheck,
  NwkSlaveRxAckWait,//等待主机的回复包
  NwkSlaveRxAckCheck,//回复节点
  NwkSlaveRxDownCheck,//下行包发送检测
  NwkSlaveRxDownWait,
  
  NwkSlaveRxStaticInit=50,
  NwkSlaveRxStaticFirstCheck,
  NwkSlaveRxStaticAckCheck,
  NwkSlaveRxStaticAppCheck,

}NwkSlaveRxState;//从机接收状态

typedef enum
{
  NwkSlaveTxIdel=0,
  NwkSlaveTxInit,
  NwkSlaveTxWake,
  NwkSlaveTxSnCheck,
  
  NwkSlaveTxAdrSniffInit,//嗅探
  NwkSlaveTxAdrSniffCheck,
  
  NwkSlaveTxRunning,
  NwkSlaveTxWaitAck,
	
  
  NwkSlaveTxExit=100,  

}NwkSlaveTxState;//从机发送状态

typedef struct
{
  u8 broad_state;
	u8 sniff_cnts;
  u8 broad_buff[50];
  u8 broad_len;
  u8 sf,bw;
  u16 wait_cnts;
  u32 freq;
  u32 start_rtc_time;
}NwkSlaveBroadStruct;//从机广播结构体

typedef struct
{
  u8 rx_state;
  u8 recv_buff[255];
  u8 down_buff[255];
  u8 recv_len;
  u8 down_len;
  u8 will_len;
  u8 run_mode;
  u8 group_id;
  u8 cad_cnts;
  u16 wait_cnts;
  u32 freq;
  u8 curr_sf, curr_bw;
  u32 start_rtc_time;
}NwkSlaveRxStruct;//从机接收结构体

typedef struct
{
  u8 tx_state;
  u8 tx_buff[255];
  u8 tx_len;
  u8 sniff_cnts;
  u8 group_id;
  u8 cad_cnts;
  u8 curr_sf, curr_bw;
  u8 awake_flag;
  u16 wait_cnts;
  u32 dst_sn;
  u32 freq;
  u32 start_rtc_time;
}NwkSlaveTxStruct;//从机发送结构体

typedef struct
{
	u8 slave_addr;
  u8 freq_ptr;
  u8 work_state;
  int recv_rssi;
  LoRaDevStruct *pLoRaDev;
  RfParamStruct rf_param;

  NwkSlaveBroadStruct slave_broad;
  NwkSlaveRxStruct slave_rx;
  NwkSlaveTxStruct slave_tx;
  void (*fun_send)(u8 *buff, u16 len);
}NwkSlaveWorkStruct;

void nwk_slave_set_lora_dev(LoRaDevStruct *pLoRaDev);
void nwk_slave_set_lora_param(u32 freq, u8 sf, u8 bw);
void nwk_slave_uart_send_register(void (*fun_send)(u8 *buff, u16 len));
void nwk_slave_device_init(void);
void nwk_slave_cad_init(void);
void nwk_slave_recv_init(void);
u8 nwk_slave_cad_check(void);
u8 nwk_slave_recv_check(u8 *buff, RfParamStruct *rf_param);
//u32 nwk_slave_calcu_air_time(u8 sf, u8 bw, u16 data_len);
void nwk_slave_send_buff(u8 *buff, u16 len);
u8 nwk_slave_send_check(void);
void nwk_slave_send_sniff(u8 sf, u8 bw);

void nwk_slave_uart_parse(u8 *recv_buff, u16 recv_len);
void nwk_slave_uart_send_level(u8 cmd_type, u8 *in_buff, u16 in_len);
void nwk_slave_uart_send_heart(void);
void nwk_slave_uart_req_config(void);
void nwk_slave_uart_send_rx(u8 *buff, u8 len, RfParamStruct *rf);
void nwk_slave_set_addr(u8 slave_addr);

void nwk_slave_broad_process(void);
void nwk_slave_rx_process(void);
void nwk_slave_tx_process(void);
void nwk_slave_work_state_check(void);

void nwk_slave_main(void);


#endif



