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


#ifndef __NWK_BSP_H__
#define __NWK_BSP_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//以下头文件是为了引入自定义的延时和加密等函数,根据自己的工程修改
#include "drv_common.h"
#include "drv_encrypt.h"

typedef unsigned          char u8;
typedef unsigned short     int u16;
typedef unsigned           int u32;

typedef   signed          char s8;
typedef   signed short     int s16;
typedef   signed           int s32;

//以下宏定义 在同一个网络系统中必须保持一致
#define   NWK_UP_CHANNEL_NUM        4   //上行通道数量
#define   NWK_DOWN_CHANNEL_NUM      4   //下行通道数量
#define   NWK_TRANSMIT_MAX_SIZE     200  //传输最大应用数据
#define   NWK_TIME_STAMP_THRESH     1726658962U //时间戳阈值
#define   NWK_MIN_FREQ              470000000U   //最低频段
#define   NWK_MAX_FREQ              500000000U   //最高频段
#define   NWK_GW_BASE_FREQ          470250000U  //网关天线监听基础频率,尽量与节点监听序列错开
#define   NWK_GW_SPACE_FREQ         2  //网关天线间的间隔频率,单位MHz
#define   NWK_BROAD_BASE_FREQ       505000000U  //广播发送基础频率
#define   NWK_BROAD_SF              11  //广播SF
#define   NWK_BROAD_BW              6   //广播BW
#define   NWK_WIRE_MAX_NUM          16   //天线最大数量

#define   NWK_LORASUN_VERSION       0x0101  //协议栈版本,主版本+子版本

typedef enum
{
  CadResultRunning=0,
  CadResultFailed,
  CadResultSuccess,
}CadResult;//CAD状态

typedef enum
{
  NwkRunModeStatic=0,//静态模式,每根天线接收监听固定RF通道
  NwkRunModeDynamic,//动态模型,每根天线CAD扫描全部通道
}NwkRunMode;

typedef enum
{
  NwkCmdHeart=0,//心跳
  NwkCmdAck,//回复指令
  NwkCmdJoin,//入网
  NwkCmdDataOnce,//单包数据
  NwkCmdDataMult,//连续数据
  
}NwkCmd;

typedef enum
{
  MSCmdHeart=0,
  MSCmdBroad,
  MSCmdRxData,
  MSCmdTxData,
  MSCmdAckRxData,//回复上行接收
  MSCmdSetSlaveCfg,//设置从机配置信息
}MSCmd;//网关主从机交互命令


typedef enum
{
  NwkRoleNode=0,//节点
  NwkRoleGateWay,//网关
}NwkRole;//本设备在网络中的角色

typedef enum
{
  JoinStateNone=0,
  JoinStateOK,
  JoinStateAccept,
  JoinStateRefuse,
}JoinState;//入网状态

typedef enum
{
  EncryptDisable=0,
  EncryptTEA,
  EncryptAES,
}EncryptMode;//加密类型

typedef enum
{
  KeyTypeRoot=0,
  KeyTypeApp,
}KeyType;//密钥类型

typedef struct
{
  u32 freq;
  u8 sf,bw;
  int16_t rssi;
  int8_t snr;
}RfParamStruct;//无线参数

u16 nwk_crc16(u8 *puchMsg,u16 usDataLen);
u8 *nwk_find_head(u8 *full_str, u16 full_len, u8 *sub_str, u16 sub_len);
void nwk_delay_ms(u32 dlyms);

int nwk_aes_encrypt(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);
int nwk_aes_decrypt(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);
u16 nwk_tea_encrypt(u8 *buff, u16 len, u32* key);
u16 nwk_tea_decrypt(u8 *buff, u16 len, u32* key);

u32 nwk_get_rtc_counter(void);
void nwk_set_rtc_counter(u32 time);
u32 nwk_get_sec_counter(void);

int nwk_get_rand(void);
void nwk_get_up_channel(u8 chn, u8 *sf, u8 *bw);
void nwk_get_down_channel(u8 chn, u8 *sf, u8 *bw);
void nwk_get_static_channel4(u8 chn, u8 *sf, u8 *bw);
u32 nwk_get_sn_freq(u32 node_sn);


#endif
