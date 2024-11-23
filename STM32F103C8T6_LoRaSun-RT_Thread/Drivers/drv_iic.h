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

#ifndef __DRV_IIC_H
#define __DRV_IIC_H

#include "drv_uart.h"

 
typedef struct
{
	GPIO_TypeDef* port_sda;
	u16 pin_sda;
	GPIO_TypeDef* port_scl;
	u16 pin_scl;		
  u16 delay_time;
}I2cDriverStruct;


#define			SDA_0						GPIO_ResetBits(pDriver->port_sda, pDriver->pin_sda)
#define			SDA_1						GPIO_SetBits(pDriver->port_sda, pDriver->pin_sda)

#define			SDA_READ()			GPIO_ReadInputDataBit(pDriver->port_sda, pDriver->pin_sda)


#define			SCL_0						GPIO_ResetBits(pDriver->port_scl, pDriver->pin_scl)
#define			SCL_1						GPIO_SetBits(pDriver->port_scl, pDriver->pin_scl)

#define			IIC_DELAY_TIME	5


void IIC_GPIOInit(I2cDriverStruct *pDriver);
void IIC_SdaInMode(I2cDriverStruct *pDriver);
void IIC_SdaOutMode(I2cDriverStruct *pDriver);
void IIC_SetDelayTime(I2cDriverStruct *pDriver, u16 time);

void IIC_Start(I2cDriverStruct *pDriver);//起始
void IIC_Stop(I2cDriverStruct *pDriver);//停止
void IIC_Ack(I2cDriverStruct *pDriver);//应答
void IIC_NAck(I2cDriverStruct *pDriver);//非应答
bool IIC_WaitAck(I2cDriverStruct *pDriver);//等待应答

u8 IIC_ReadByte(I2cDriverStruct *pDriver);//读一字节
void IIC_WriteByte(I2cDriverStruct *pDriver, u8 data);//写一字节



#endif


































