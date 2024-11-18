
#ifndef __NWK_MASTER_H__
#define __NWK_MASTER_H__

#include "nwk_bsp.h"

#define NWK_GW_WIRELESS_NUM     4    //天线数量    
#define NWK_NODE_MAX_NUM        100  //节点最大数量,根据芯片RAM确定  
//#define NWK_MASTER_USE_AES        //是否启用AES加密,根据芯片能力和需求确定

typedef enum
{
  NwkMasterEventNone,
  NwkMasterEventDownAck,//下发回复
}NwkMasterEvent;//事件类型

typedef enum
{
  NwkMasterDownResultAddOK=0,//添加成功
  NwkMasterDownResultFull, //缓冲区已满
  NwkMasterDownResultErrSn, //设备不存在
  NwkMasterDownResultTimeOut,//超时,失败
  NwkMasterDownResultSuccess,//下发成功
  
  NwkMasterDownResultErrUnknow=100,//未知错误
}NwkMasterDownResult;

typedef struct NwkNodeTokenStruct
{
  u32 node_sn;
  u8 down_buff[256];//下行缓冲区
  u8 down_len; 
  u8 down_cnts;
  u8 join_state;
  u16 wake_period;//唤醒周期
  s16 rssi;
  s8 snr;
  u8 up_pack_num,down_pack_num;
  u8 app_key[16];
  u32 keep_time;
  u32 down_time;
  struct NwkNodeTokenStruct *next;
}NwkNodeTokenStruct;//节点记录

typedef struct
{
  u8 slave_addr;
  u32 keep_time;
  u32 counts;
  void (*fun_send)(u8 *buff, u16 len);//发送接口
}NwkSlaveTokenStruct;//从机天线管理

typedef struct
{
  u32 src_sn; //数据来源
  u8 app_data[300];//应用数据指针
  u16 data_len;//数据长度
  bool read_flag;//读取标志
  u8 up_pack_num;//上行包序号
  RfParamStruct rf_param;
}NwkMasterRecvFromStruct;//数据接收结构体

typedef struct
{
  u8 event;
  u8 params[50];
}NwkMasterEventStruct;//事件



typedef struct
{
  u32 gw_sn;
  u8 root_key[16];
  NwkNodeTokenStruct *pNodeHead;
  u16 node_cnts;
  u8 freq_ptr;//频段序号
  u8 run_mode;
  NwkSlaveTokenStruct slave_token_list[NWK_GW_WIRELESS_NUM];
  NwkMasterRecvFromStruct recv_from;
  NwkMasterEventStruct event;
}NwkMasterWorkStruct;


void nwk_master_uart_send_register(u8 index, u8 slave_adddr, void (*fun_send)(u8 *buff, u16 len));
void nwk_master_uart_parse(u8 *recv_buff, u16 recv_len);
void nwk_master_uart_send_level(u8 index, u8 cmd_type, u8 *in_buff, u16 in_len);

u8 nwk_master_make_lora_buff(u8 opt, u32 dst_sn, u8 *key, u8 cmd_type, u8 pack_num, u8 *in_buff, u8 in_len, u8 *out_buff, u8 out_size);
void nwk_master_lora_parse(u8 *recv_buff, u8 recv_len, u8 slave_addr, RfParamStruct *rf);
void nwk_master_send_broad(u8 slave_addr); 
void nwk_master_send_slave_config(u8 slave_addr);
void nwk_master_send_down_pack(u32 dst_sn, u8 slave_addr, u8 *in_buff, u8 in_len, u8 flag);

NwkSlaveTokenStruct *nwk_master_find_slave(u8 slave_addr);
void nwk_master_set_root_key(u8 *key);
void nwk_master_set_config(u8 freq_ptr, u8 run_mode);
void nwk_master_get_config(u8 *freq_ptr, u8 *run_mode);
NwkNodeTokenStruct *nwk_master_add_node(u32 node_sn);
NwkNodeTokenStruct *nwk_master_find_node(u32 node_sn);
void nwk_master_del_node(u32 node_sn);

void nwk_master_set_gw_sn(u32 gw_sn);
u32 nwk_master_get_gw_sn(void);

u8 nwk_master_add_down_pack(u32 dst_sn, u8 *in_buff, u8 in_len);
void nwk_master_check_down_pack(void);

NwkMasterRecvFromStruct *nwk_master_recv_from_check(void);
NwkMasterEventStruct *nwk_master_event_check(void);
void nwk_master_main(void);

#endif
