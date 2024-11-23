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


#include "drv_iic.h"

/*		
================================================================================
描述 : IIC引脚初始化
输入 : 
输出 : 
================================================================================
*/
void IIC_GPIOInit(I2cDriverStruct *pDriver)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = pDriver->pin_sda;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(pDriver->port_sda, &GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin = pDriver->pin_scl;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(pDriver->port_scl, &GPIO_InitStruct);		
  
  pDriver->delay_time=IIC_DELAY_TIME;
}


/*		
================================================================================
描述 : SDA设置成输入模式
输入 : 
输出 : 
================================================================================
*/
void IIC_SdaInMode(I2cDriverStruct *pDriver)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = pDriver->pin_sda;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(pDriver->port_sda, &GPIO_InitStruct);
}

/*		
================================================================================
描述 : SDA设置成输出模式
输入 : 
输出 : 
================================================================================
*/
void IIC_SdaOutMode(I2cDriverStruct *pDriver)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = pDriver->pin_sda;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(pDriver->port_sda, &GPIO_InitStruct);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void IIC_SetDelayTime(I2cDriverStruct *pDriver, u16 time)
{
  pDriver->delay_time=time;
}


/*		
================================================================================
描述 : 起始信号
输入 : 
输出 : 
================================================================================
*/
void IIC_Start(I2cDriverStruct *pDriver)
{
	SCL_1;
	SDA_1;
	delay_us(pDriver->delay_time);
	SDA_0;
	delay_us(pDriver->delay_time);
	SCL_0;
	delay_us(pDriver->delay_time);
}


/*		
================================================================================
描述 : 停止信号
输入 : 
输出 : 
================================================================================
*/
void IIC_Stop(I2cDriverStruct *pDriver)
{
	SDA_0;
	SCL_1;
	delay_us(pDriver->delay_time);
	SDA_1;
	delay_us(pDriver->delay_time);
	SCL_0;
}


/*		
================================================================================
描述 : 应答
输入 : 
输出 : 
================================================================================
*/
void IIC_Ack(I2cDriverStruct *pDriver)
{
	SDA_0;
	delay_us(pDriver->delay_time);
	SCL_1;
	delay_us(pDriver->delay_time);
	SCL_0;
	delay_us(pDriver->delay_time);
	SDA_1;
	delay_us(pDriver->delay_time);
}

/*		
================================================================================
描述 : 非应答
输入 : 
输出 : 
================================================================================
*/
void IIC_NAck(I2cDriverStruct *pDriver)
{
	SCL_0;
	delay_us(pDriver->delay_time);
	SDA_1;
	SCL_1;
	delay_us(pDriver->delay_time);
	
}

/*		
================================================================================
描述 : 等待回复
输入 : 
输出 : 
================================================================================
*/
bool IIC_WaitAck(I2cDriverStruct *pDriver)
{
	u32 wait_tickets=0;
	
	SCL_0;
	IIC_SdaInMode(pDriver);
	delay_us(pDriver->delay_time);
	
	while(SDA_READ()>0)
	{
		wait_tickets++;
		if(wait_tickets>250)
		{
			IIC_SdaOutMode(pDriver);
			delay_us(pDriver->delay_time);
			IIC_Stop(pDriver);
			return false;
		}
		delay_us(1);
	}
	SCL_1;
	delay_us(pDriver->delay_time);
	SCL_0;
	delay_us(pDriver->delay_time);
	IIC_SdaOutMode(pDriver);
	return true;
}
/*		
================================================================================
描述 : 读取一个字节
输入 : 
输出 : 
================================================================================
*/
u8 IIC_ReadByte(I2cDriverStruct *pDriver)
{
	u8 i, data=0;
	
	IIC_SdaInMode(pDriver);
	for(i=0;i<8;i++)
	{
		SCL_0;
		delay_us(pDriver->delay_time);
		SCL_1;
		delay_us(pDriver->delay_time);
		data=data<<1;//左移，高位先读取
		if(SDA_READ()>0)
		{
			data|=0x01;
		}
	}
	SCL_0;
	IIC_SdaOutMode(pDriver);
	delay_us(pDriver->delay_time);
	return data;
}
/*		
================================================================================
描述 : 写入一个字节
输入 : 
输出 : 
================================================================================
*/
void IIC_WriteByte(I2cDriverStruct *pDriver, u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		SCL_0;
		if(data&0x80)//高位先写
		{
			SDA_1;			
		}
		else
		{
			SDA_0;
		}
		delay_us(pDriver->delay_time);
		SCL_1;
		delay_us(pDriver->delay_time);
		data=data<<1;  
	}

}

































