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
//#define   NWK_RF_CHANNEL_NUM        8  //SF,BW通道数量,节点与网关要保持一致
//#define   NWK_RF_GROUP_NUM          4   //通道数量组数
#define   NWK_UP_CHANNEL_NUM        10   //上行通道数量
#define   NWK_DOWN_CHANNEL_NUM      4   //下行通道数量
#define   NWK_TRANSMIT_MAX_SIZE     200  //传输最大应用数据
#define   NWK_TIME_STAMP_THRESH     1726658962U //时间戳阈值
#define   NWK_MIN_FREQ              470000000U   //最低频段
#define   NWK_MAX_FREQ              500000000U   //最高频段
#define   NWK_GW_BASE_FREQ          470250000U  //网关天线监听基础频率,尽量与节点监听序列错开
#define   NWK_BROAD_BASE_FREQ      505000000U  //广播发送基础频率
#define   NWK_BROAD_SF             11  //广播SF
#define   NWK_BROAD_BW             6   //广播BW

typedef enum
{
  CadResultRunning=0,
  CadResultFailed,
  CadResultSuccess,
}CadResult;

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
  MSCmdSetFreqPtr,//设置基础频段
}MSCmd;


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
}JoinState;

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
}KeyType;

typedef struct
{
  u32 freq;
  u8 sf,bw;
  int16_t rssi;
  int8_t snr;
}RfParamStruct;

u16 nwk_crc16(u8 *puchMsg,u16 usDataLen);
u8 *nwk_find_head(u8 *full_str, u16 full_len, u8 *sub_str, u16 sub_len);
void nwk_delay_ms(u32 dlyms);
//void nwk_delay_us(u32 dlyus);

int nwk_aes_encrypt(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);
int nwk_aes_decrypt(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);
u16 nwk_tea_encrypt(u8 *buff, u16 len, u32* key);
u16 nwk_tea_decrypt(u8 *buff, u16 len, u32* key);

u32 nwk_get_rtc_counter(void);
void nwk_set_rtc_counter(u32 time);
u32 nwk_get_sec_counter(void);

//void nwk_eeprom_read(u8 *out_buff, u32 size);
//void nwk_eeprom_save(u8 *buff, u32 size);

int nwk_get_rand(void);
//void nwk_get_channel(u8 chn, u8 *sf, u8 *bw);
void nwk_get_up_channel(u8 chn, u8 *sf, u8 *bw);
void nwk_get_down_channel(u8 chn, u8 *sf, u8 *bw);
u16 nwk_cacul_sniff_time(u8 sf, u8 bw);


#endif
