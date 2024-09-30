

#ifndef __NWK_GW_H__
#define __NWK_GW_H__


#include "nwk_bsp.h"

//定义所使用的LoRa芯片模块,只能选一种,其余注释
#define LORA_SX1278    
//#define LORA_SX1268    
//#define LORA_LLCC68    


#if defined(LORA_SX1278)  
  #include "drv_sx1278.h"
  #define  LoRaDevStruct    DrvSx1278Struct

#elif defined(LORA_SX1268)


#elif defined(LORA_LLCC68)


#endif


#define NWK_GW_WIRELESS_NUM     4    //天线数量    
#define NWK_NODE_MAX_NUM        100  //节点最大数量,根据芯片RAM确定    



typedef enum
{
  NwkGwWorkIdel=0,//空闲
  NwkGwWorkBroad,//广播
  NwkGwWorkRX,//接收
  NwkGwWorkTX,//发送

}NwkGwState;//网关工作状态

typedef enum
{
  NwkGwBroadIdel=0,
  NwkGwBroadInit,
  NwkGwBroadTxCheck,
  NwkGwBroadExit,

}NwkGwBroadState;//网关广播状态


typedef enum
{
  NwkGwRxIdel=0,
  NwkGwRxInit,
  NwkGwRxCadInit,
  NwkGwRxCadCheck,
  NwkGwRxSniff,
  NwkGwRxCheck,
  NwkGwRxExit,

}NwkGwRxState;//网关接收状态

typedef enum
{
  NwkGwTxIdel=0,
  NwkGwTxInit,
  NwkGwTxLBTInit,
  NwkGwTxLBTCheck,
  NwkGwTxSniffInit,//嗅探
  NwkGwTxSniffCheck,
  NwkGwTxRunning,
  NwkGwTxAck,
	
  
  NwkGwTxExit=100,  

}NwkGwTxState;//网关发送状态

typedef struct
{
  u8 broad_state;
  u8 broad_buff[50];
  u8 broad_len;
  u8 sf,bw;
  u16 wait_cnts;
  u32 freq;
  u32 start_rtc_time;
}NwkGwBroadStruct;//网关广播结构体

typedef struct
{
  u8 rx_state;
  u8 recv_buff[255];
  u8 recv_len;
  u8 freq_ptr;
  u8 chn_ptr;
  u16 wait_cnts;
  u32 freq;
  u32 start_rtc_time;
}NwkGwRxStruct;//网关接收结构体

typedef struct
{
  u8 tx_state;
  u8 tx_buff[NWK_TRANSMIT_MAX_SIZE+30];
  u8 tx_len;
  u8 sniff_cnts;
  u8 try_cnts, curr_cnts;
  u8 sfs[2],bws[2];
  u8 curr_sf, curr_bw;
  u16 wait_cnts;
  u32 freq;
  u32 start_rtc_time;
}NwkGwTxStruct;//网关发送结构体


typedef struct
{
  u8 index;
  u8 work_state;
  int recv_rssi;
  LoRaDevStruct *pLoRaDev;

  NwkGwBroadStruct gw_broad;
  NwkGwRxStruct gw_rx;
  NwkGwTxStruct gw_tx;

}NwkGwWorkStruct;//网关天线工作节点


typedef struct NwkNodeTokenStruct
{
  u32 node_sn;
  u8 last_sf, last_bw;//最近一次通讯参数
  u8 join_state;
  u16 wake_period;
  u8 app_key[16];
  u32 keep_time;
  struct NwkNodeTokenStruct *next;
}NwkNodeTokenStruct;//节点记录

typedef struct
{
  u8 root_key[16];
  NwkNodeTokenStruct *pNodeHead;
  u16 node_cnts;
  u8 freq_ptr;//频段序号
  NwkGwWorkStruct gw_wireless_list[NWK_GW_WIRELESS_NUM];//网关天线列表
}NwkCoreWorkStruct;//网络核心


void nwk_gw_set_lora_param(NwkGwWorkStruct *pNwkGw, u32 freq, u8 sf, u8 bw);
void nwk_gw_cad_init(NwkGwWorkStruct *pNwkGw);
void nwk_gw_recv_init(NwkGwWorkStruct *pNwkGw);
u8 nwk_gw_cad_check(NwkGwWorkStruct *pNwkGw);
u8 nwk_gw_recv_check(NwkGwWorkStruct *pNwkGw, u8 *buff, int16_t *rssi);
u32 nwk_gw_calcu_air_time(u8 sf, u8 bw, u16 data_len);
void nwk_gw_send_buff(NwkGwWorkStruct *pNwkGw, u8 *buff, u16 len);
u8 nwk_gw_send_check(NwkGwWorkStruct *pNwkGw);
void nwk_gw_send_sniff(NwkGwWorkStruct *pNwkGw, u8 sf, u8 bw);
void nwk_gw_broad_process(NwkGwWorkStruct *pNwkGw);
void nwk_gw_rx_process(NwkGwWorkStruct *pNwkGw);
void nwk_gw_tx_process(NwkGwWorkStruct *pNwkGw);
void nwk_gw_work_state_check(NwkGwWorkStruct *pNwkGw);

/*************/
void nwk_core_set_root_key(u8 *key);
NwkGwWorkStruct *nwk_core_init_gw(u8 index, LoRaDevStruct *pLoRaDev);
NwkNodeTokenStruct *nwk_core_add_token(u32 node_sn);
NwkNodeTokenStruct *nwk_core_find_token(u32 node_sn);
void nwk_core_del_token(u32 node_sn);
void nwk_core_main(void);



#endif

