
#ifndef __DRV_IAP_H__
#define __DRV_IAP_H__


#include "drv_common.h"
#include "drv_uart.h"



#define 			IAP_EEPROM_ADDR    	((uint32_t)0x200)			    //存放BootLoader参数的EEPROM地址

#define 			APP_START_ADDR    	((uint32_t)0x08006000)			//APP起始地址
#define  		  FLASH_PAGE_SIZE			(u32)(1024*1)									//flash页大小
#define 			IAP_PACK_SIZE    	  ((u32)400)								//升级包大小
#define				MAX_BIN_SIZE				((u32)(40*1024))					//升级程序最大占用空间
#define				RAM_SIZE						((u32)(20*1024))					//芯片RAM空间

#define 			IAP_FLAG		    	   5


typedef enum 
{
	IAP_CMD_INTO_BOOT=0,
	IAP_CMD_REQ_UPDATE,
	IAP_CMD_START_FRAM,
	IAP_CMD_UPDATING,
	IAP_CMD_ACK,
	IAP_CMD_KEEP,
	IAP_CMD_UPDATE_OK,
	
	IAP_CMD_ERR=0x80,
	
}iap_cmd;

typedef enum
{
	IAP_ERR_UNKNOW,	
	IAP_ERR_JMP,
	IAP_ERR_STACK,
	IAP_ERR_FLASH,
	IAP_ERR_CRC,	
	IAP_ERR_FLAG,	
}iap_err;
/*==================================
	IAP升级通讯协议定义
	帧头：AA 77      2字节
	数据区长度高位、低位：2字节
	CRC校验高位、低位：2字节
	数据区：第一字节--保留
					第二字节--命令类型 
					
					命令类型：

==================================*/


typedef  void (*iapfun)(void);


typedef struct
{
	u16 total_pack;
	u16 pack_num;	
	u8 jum_result;//跳转结果
	bool b_update_flag;//升级标志,升级过程中禁止其他通讯接口通讯
}IAPMangerStruct;


typedef struct
{
	u8 boot_flag;	//IAP跳转次数，这个标志会在app中被置零，如果APP程序有问题，那么超过IAP_FLAG次就进入IAP模式
	u8 app_ver;//app版本号,复位前保存
	u8 app_name[12];//app名称,复位前保存
	u16 crcValue;
}IAPBootStruct;


extern IAPMangerStruct g_sIapManger;
extern IAPBootStruct g_sIapBoot;
//////////////////

u8 IAP_ParsePack(u8 *buff, u16 len);

void IAP_BootFlagSave(void);
u16 IAP_MakeAckMsg(u8 *make_buff);
void IAP_JmpFlagCheck(void);
u8 IAP_LoadApp(u32 appxaddr);
u8 version_to_num(const char *ver_str);
u8 get_app_version(void);


#endif

