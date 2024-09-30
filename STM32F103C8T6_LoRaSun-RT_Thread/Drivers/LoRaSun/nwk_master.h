
#ifndef __NWK_MASTER_H__
#define __NWK_MASTER_H__

#include "nwk_bsp.h"

#define NWK_GW_WIRELESS_NUM     4    //天线数量    
#define NWK_NODE_MAX_NUM        100  //节点最大数量,根据芯片RAM确定  

typedef struct NwkNodeTokenStruct
{
  u32 node_sn;
  u8 last_sf, last_bw;//最近一次通讯参数
  u8 join_state;
  u16 wake_period;//唤醒周期
  int16_t rssi;
  u8 up_pack_num,down_pack_num;
  u8 app_key[16];
  u32 keep_time;
  struct NwkNodeTokenStruct *next;
}NwkNodeTokenStruct;//节点记录

typedef struct
{
  u8 slave_addr;
  u32 keep_time;
  void (*fun_send)(u8 *buff, u16 len);//发送接口
}NwkSlaveTokenStruct;//从机天线管理

typedef struct
{
  u32 src_sn; //数据来源
  u8 *app_data;//应用数据指针
  u8 data_len;//数据长度
  bool read_flag;//读取标志
}NwkMasterRecvFromStruct;//数据接收结构体

typedef struct
{
  u32 gw_sn;
  u8 root_key[16];
  NwkNodeTokenStruct *pNodeHead;
  u16 node_cnts;
  u8 freq_ptr;//频段序号
  NwkSlaveTokenStruct slave_token_list[NWK_GW_WIRELESS_NUM];
  NwkMasterRecvFromStruct recv_from;
}NwkMasterWorkStruct;


void nwk_master_fun_send_register(u8 index, u8 slave_adddr, void (*fun_send)(u8 *buff, u16 len));
void nwk_master_uart_parse(u8 *recv_buff, u16 recv_len);
void nwk_master_uart_send_level(u8 index, u8 cmd_type, u8 *in_buff, u16 in_len);
u8 nwk_master_make_lora_buff(u8 opt, u32 dst_sn, u8 *key, u8 cmd_type, u8 pack_num, u8 *in_buff, u8 in_len, u8 *out_buff, u8 out_size);
void nwk_master_lora_parse(u8 *recv_buff, u8 recv_len, u8 slave_addr, RfParamStruct *rf);
NwkSlaveTokenStruct *nwk_master_find_slave(u8 slave_addr);


void nwk_master_set_root_key(u8 *key);
NwkNodeTokenStruct *nwk_master_add_token(u32 node_sn);
NwkNodeTokenStruct *nwk_master_find_token(u32 node_sn);
void nwk_master_del_token(u32 node_sn);

void nwk_master_main(void);

#endif
