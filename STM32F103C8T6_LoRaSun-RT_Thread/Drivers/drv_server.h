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

#ifndef __DRV_SERVER_H
#define __DRV_SERVER_H

#include "drv_common.h"  


#ifndef				SERVER_PACK_SIZE
#define				SERVER_PACK_SIZE	        256		//数据包最大值
#endif

#ifndef				SERVER_PROTOCOL_VER
#define				SERVER_PROTOCOL_VER	      1   //协议版本
#endif

#ifndef				SERVER_PASSWD_CNTS
#define				SERVER_PASSWD_CNTS	      5   //密码组数
#endif

#ifndef				SERVER_EEPROM_ADDR
#define				SERVER_EEPROM_ADDR         (0x0050)//存储地址
#endif

#ifndef			CONFIG_EEPROM_READ			
#define			CONFIG_EEPROM_READ		EEPROM_Read  //配置读取函数
#endif

#ifndef			CONFIG_EEPROM_WRITE			
#define			CONFIG_EEPROM_WRITE		EEPROM_Write  //配置保存函数
#endif

#define         APP_ID_MIN                       (u32)(123000)
#define     USE_AES
typedef enum
{
  ENCRYPT_MODE_DISABLE=0,
  ENCRYPT_MODE_TEA,
  ENCRYPT_MODE_AES,
}encryptMode;

typedef enum
{

	SERVER_CMD_UP_DATA=100,
	SERVER_CMD_DOWN_DATA=200,
  
  REG_CMD_REPORT_APP_ID=210,
  REG_CMD_SET_APP_ID,
  
  UPDATE_CMD_IAP=220,//远程升级总命令
  UPDATE_CMD_INTO_BOOT,//使设备进入升级状态
  UPDATE_CMD_KEEP,//保持连接  
  
  SERVER_CMD_LORA=230,//LORA总命令
}ServerCmdType;

typedef struct
{
	u8 head[2];
	u8 version;
	u8 encrypt_index;//密码索引
	u8 crc_h;
	u8 crc_l;
	u8 data_len_h;
	u8 data_len_l;
	u8 app_id[4];
	u8 gw_sn[4];
}ServerHeadStruct;

typedef struct
{
  u32 app_id;//应用ID
  u32 gw_sn;//设备SN
  u16 reserved;
  u16 crcValue;
}ServerSaveStruct;

typedef struct
{	

  u8 passwd_table[SERVER_PASSWD_CNTS][16];//密码表,要跟用户端的模型对应

	int (*fun_send)(u8 *buff, u16 len);
	u16 (*fun_server_cmd_parse)(u8 cmd_type, u8 *in_buff, u16 in_len);	
	u16 (*fun_slave_cmd_parse)(u32 node_sn, u8 *in_buff, u16 in_len);	
   
}ServerWorkStruct;


void drv_server_read(void);
void drv_server_write(void);
void drv_server_init(void);


u16 drv_server_send_msg(u8 cmd_type, u8 *in_buff, u16 in_len);
void drv_server_send_slave_msg(u32 node_sn, u8 *in_buff, u16 in_len);
int drv_server_send_level(u8 *buff, u16 len);
void drv_server_send_register(int (*fun_send)(u8 *buff, u16 len));

u16 drv_server_recv_parse(u8 *buff, u16 len, int (*fun_send)(u8 *buff, u16 len));
u16 drv_server_cmd_parse(u8 cmd_type, u8 *in_buff, u16 in_len);
void drv_server_cmd_parse_register(u16 (*fun_server_cmd_parse)(u8 cmd_type, u8 *in_buff, u16 in_len));

u16 drv_slave_cmd_parse(u32 node_sn, u8 *in_buff, u16 in_len);
void drv_slave_cmd_parse_register(u16 (*fun_slave_cmd_parse)(u32 node_sn, u8 *in_buff, u16 in_len));

void drv_server_set_app_id(u32 app_id);
u32 drv_server_get_app_id(void);
void drv_server_set_gw_sn(u32 gw_sn);
u32 drv_server_get_gw_sn(void);
void drv_server_add_passwd(u8 index, u8 *passwd);

#endif




