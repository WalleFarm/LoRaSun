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


#ifndef __NWK_MASTER_H__
#define __NWK_MASTER_H__

#include "nwk_bsp.h"

#define NWK_GW_WIRELESS_NUM     4    //天线数量    
#define NWK_NODE_MAX_NUM        100  //节点最大数量,根据芯片RAM确定  
#define NWK_MASTER_USE_AES        //是否启用AES加密,根据芯片能力和需求确定

typedef enum
{
  NwkMasterEventNone,
  NwkMasterEventDownAck,//下发回复
}NwkMasterEvent;//事件类型

typedef enum
{
  NwkMasterDownResultAddOK=0,//添加成功
  NwkMasterDownResultFull, //缓冲区已满
  NwkMasterDownResultLong, //数据太长
  NwkMasterDownResultErrSn, //设备不存在
  NwkMasterDownResultTimeOut,//超时,失败
  NwkMasterDownResultSuccess,//下发成功
  
  NwkMasterDownResultErrUnknow=100,//未知错误
}NwkMasterDownResult;

typedef struct NwkNodeTokenStruct
{
  u32 node_sn;
  u8 down_buff[50];//下行缓冲区,STM32F103C8T6 RAM较小,不宜设置过大,测试50字节够用了
  u8 down_len; 
  u8 down_cnts;
  u16 wake_period;//唤醒周期
  s16 rssi;
  u8 join_state;
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
  u16 broad_offset;//广播偏移
  NwkSlaveTokenStruct slave_token_list[NWK_GW_WIRELESS_NUM];
  NwkMasterRecvFromStruct recv_from;
  NwkMasterEventStruct event;
}NwkMasterWorkStruct;


void nwk_master_uart_send_register(u8 index, u8 slave_adddr, void (*fun_send)(u8 *buff, u16 len));
void nwk_master_uart_parse(u8 *recv_buff, u16 recv_len);
void nwk_master_uart_send_level(u8 index, u8 cmd_type, u8 *in_buff, u16 in_len);

u8 nwk_master_make_lora_buff(u8 opt, u32 dst_sn, u8 *key, u8 cmd_type, u8 pack_num, u8 *in_buff, u8 in_len, u8 *out_buff, u16 out_size);
void nwk_master_lora_parse(u8 *recv_buff, u8 recv_len, u8 slave_addr, RfParamStruct *rf);
void nwk_master_send_broad(u8 slave_addr); 
void nwk_master_send_slave_config(u8 slave_addr);
void nwk_master_send_down_pack(u32 dst_sn, u8 slave_addr, u8 *in_buff, u8 in_len, u8 flag);

NwkSlaveTokenStruct *nwk_master_find_slave(u8 slave_addr);
void nwk_master_set_offset(u16 offset);
void nwk_master_set_root_key(u8 *key);
void nwk_master_set_config(u8 freq_ptr, u8 run_mode);
void nwk_master_get_config(u8 *freq_ptr, u8 *run_mode);
NwkNodeTokenStruct *nwk_master_add_node(u32 node_sn);
NwkNodeTokenStruct *nwk_master_find_node(u32 node_sn);
void nwk_master_del_node(u32 node_sn);

void nwk_master_set_gw_sn(u32 gw_sn);
u32 nwk_master_get_gw_sn(void);

u8 nwk_master_add_down_pack(u32 dst_sn, u8 *in_buff, u16 in_len);
void nwk_master_check_down_pack(void);
void nwk_master_clear_down_buff(NwkNodeTokenStruct *pNwkNode);

NwkMasterRecvFromStruct *nwk_master_recv_from_check(void);
NwkMasterEventStruct *nwk_master_event_check(void);
void nwk_master_main(void);

#endif
