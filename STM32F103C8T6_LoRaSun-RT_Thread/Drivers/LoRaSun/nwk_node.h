
#ifndef __NWK_NODE_H__
#define __NWK_NODE_H__


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
  //与1268共用一套代码
  #define LORA_SX1268    
  #include "drv_sx1268.h"
  #define  LoRaDevStruct    DrvSx1268Struct

#endif


#define NWK_GW_NUM              3   //监听的网关最大数量
#define NWK_D2D_NODE_NUM        3   //D2D设备存储最大数量

//#define NWK_NODE_USE_AES        //是否启用AES加密,根据芯片能力和需求确定

#define NWK_NODE_USE_ENCRYPT_MODE     EncryptTEA  //节点使用的加密方法


typedef enum
{
  NwkNodeWorkIdel=0,//空闲
  NwkNodeWorkSearch,//搜索
  NwkNodeWorkRX,//接收
  NwkNodeWorkTXGw,//发送到网关
  NwkNodeWorkTXD2d,//发送到设备

}NwkNodeState;//节点状态

typedef enum
{
  NwkNodeSearchIdel=0,
  NwkNodeSearchCadInit,
  NwkNodeSearchCadCheck,
  NwkNodeSearchRxCheck,
  NwkNodeSearchExit,

}NwkNodeSearchState;//搜索状态

typedef enum
{
  NwkNodeRxIdel=0,
  NwkNodeRxInit,
  NwkNodeRxCadInit,
  NwkNodeRxCadCheck,
  NwkNodeRxSnCheck,
	NwkNodeRxAdrInit,
	NwkNodeRxAdrCadCheck,
	NwkNodeRxAppCheck,
  NwkNodeRxAppAck,
  
  NwkNodeRxExit=100,

}NwkNodeRxState;//接收状态

typedef enum
{
  NwkNodeTxGwIdel=0,
  NwkNodeTxGwInit,
  NwkNodeTxGwLBTInit,
  NwkNodeTxGwLBTCheck,
  NwkNodeTxGwSniffInit,//嗅探
  NwkNodeTxGwSniffCheck,
  NwkNodeTxGwRunning,
  NwkNodeTxGwAck,
	
  
  NwkNodeTxGwExit=100,

}NwkNodeTxGwState;//发送状态

typedef enum
{
  NwkNodeTxD2dIdel=0,
  NwkNodeTxD2dInit,
  
  NwkNodeTxD2dExit=100,

}NwkNodeTxD2dState;//发送D2D状态

typedef struct
{
  u32 gw_sn; //网关序列号
  u8 base_freq_ptr;//基础频率序号
  u8 wireless_num;//天线数量
  u8 payload_percent;//网关负载百分比
  u8 err_cnts;//出错次数
  s16 rssi;//信号强度
	s8 snr;
  u32 keep_rtc_time;//最近通信时间
	u8 app_key[16];//应用密码
	u8 join_state;//入网状态
	u8 up_pack_num, down_pack_num;
  u16 wait_join_time;//入网等待时间
	u32 last_join_time;//上次请求入网时间
}NwkParentWorkStrcut;//网关信息

typedef struct
{
  u8 search_state;//搜索状态
  u32 search_start_time;
  u32 alarm_rtc_time;//闹钟唤醒时间点
}NwkNodeSearchStruct;//广播搜索结构体

typedef struct
{
  u8 rx_state;
  u8 listen_cnts;
  u8 wait_cnts;
  u8 curr_sf, curr_bw;
	u8 will_len;
	u8 group_id;
  u32 freq;
  u32 start_rtc_time;
  u32 alarm_rtc_time;//闹钟唤醒时间点
  u8 recv_buff[255];
  u8 ack_buff[50];
  u8 ack_len;
}NwkNodeRxStruct;//接收结构体

typedef struct
{
  u8 tx_state;
  u8 sf, bw;
  u8 sniff_cnts;
	u8 cad_cnts;
  u8 wait_cnts, tx_len;
  u8 wireless_ptr;//选中的天线序号
  u8 group_id;
  u16 tx_total_cnts, tx_ok_cnts;
	u8 tx_step;
  u8 tx_cmd,try_cnts;
  u8 tx_buff[NWK_TRANSMIT_MAX_SIZE];//缓存应用数据
  u32 freq;
  u32 join_sn;//请求入网的网关SN
  u32 start_rtc_time;
  u32 alarm_rtc_time;//闹钟唤醒时间点
  NwkParentWorkStrcut *pGateWay;//要对接的网关
}NwkNodeTxGwStruct;//网关发送结构体

typedef struct
{
  u8 tx_state;
  u8 sf, bw;
  u8 sniff_cnts;
  u8 wait_cnts, tx_len;
  u16 wake_period;//
  u8 tx_buff[NWK_TRANSMIT_MAX_SIZE];//缓存应用数据
  u32 freq;
  u32 start_rtc_time;
  u32 alarm_rtc_time;//闹钟唤醒时间点
//  NwkD2DSaveStruct *pD2d;//要对接的网关
}NwkNodeTxD2dStruct;//D2D发送结构体
 
typedef struct
{
  u32 src_sn; //数据来源
  u8 app_data[256];//应用数据
  u16 data_len;//数据长度
  bool read_flag;//读取标志
}NwkNodeRecvFromStruct;//数据接收结构体

typedef struct
{
	u8 event;//事件  
  NwkNodeRecvFromStruct *pRecvFrom;
  u32 alarm_time;  //闹钟时间
}NowkNodeReturnStruct;
 
typedef struct
{
  u32 node_sn;
  NwkParentWorkStrcut parent_list[NWK_GW_NUM];//网关列表
  u16 wake_period;//唤醒周期,等于0表示节点无需休眠,时刻处于接收状态
  u8 work_state;//工作状态

  RfParamStruct rf_param;
  u32 alarm_rtc_time;//闹钟唤醒时间点
  LoRaDevStruct *pLoRaDev;
  NwkNodeRecvFromStruct recv_from;
  NowkNodeReturnStruct node_return;

  NwkNodeSearchStruct node_search;
  NwkNodeTxGwStruct node_tx_gw;
  NwkNodeTxD2dStruct node_tx_d2d;
  NwkNodeRxStruct node_rx;
  u8 root_key[16];//根密码,同一个网络中的设备保持一致
}NwkNodeWorkStruct;//节点工作信息


void nwk_node_set_sn(u32 node_sn);
void nwk_node_set_root_key(u8 *key);
void nwk_node_add_gw(u32 gw_sn, u8 base_freq, u8 wireless_num);
void nwk_node_del_gw(u32 gw_sn);
void nwk_node_set_wake_period(u16 period);
void nwk_node_set_lora_dev(LoRaDevStruct *pLoRaDev);
void nwk_node_set_led(bool state);//LED
void nwk_node_set_lora_param(u32 freq, u8 sf, u8 bw);
void nwk_node_device_init(void);
void nwk_node_sleep_init(void);
void nwk_node_cad_init(void);
void nwk_node_recv_init(void);
u8 nwk_node_cad_check(void);
u8 nwk_node_recv_check(u8 *buff, RfParamStruct *rf_param);
u32 nwk_node_calcu_air_time(u8 sf, u8 bw, u16 data_len);
void nwk_node_send_buff(u8 *buff, u16 len);
u8 nwk_node_send_check(void);
void nwk_node_send_sniff(u8 sf, u8 bw);

u8 nwk_node_make_send_buff(u8 opt, u32 dst_sn, u8 *key, u8 cmd_type, u8 pack_num, u8 *in_buff, u8 in_len, u8 *out_buff, u8 out_size);
void nwk_node_recv_parse(u8 *recv_buff, u8 recv_len);//接收解析
NwkParentWorkStrcut *nwk_node_search_gw(u32 gw_sn);
NwkParentWorkStrcut *nwk_node_select_gw(void);

void nwk_node_clear_tx(void);
u8 nwk_node_send2gateway(u8 *in_buff, u8 in_len);
void nwk_node_req_join(u32 gw_sn);
u8 nwk_node_send2node(u32 dst_node_sn, u8 *in_buff, u8 in_len);
void nwk_node_search_process(void);
void nwk_node_rx_process(void);
void nwk_node_tx_gw_process(void);
void nwk_node_tx_d2d_process(void);

void nwk_node_work_check(void);
NwkNodeRecvFromStruct *nwk_node_recv_from_check(void);
RfParamStruct *nwk_node_take_rf_param(void);
void nwk_node_tx_cnts(u16 *total_cnts, u16 *ok_cnts);
NowkNodeReturnStruct *nwk_node_main(void);


#endif












