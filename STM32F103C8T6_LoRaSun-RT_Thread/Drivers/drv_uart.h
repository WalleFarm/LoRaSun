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

#ifndef __DRV_UART_H__
#define __DRV_UART_H__


#include "drv_common.h"

typedef struct
{
	USART_TypeDef* USARTx;
	u8 PortNum;//串口号
	u8 *pBuff;//数据指针
	u16 iRecv;//已接收长度
	u16 iDone;//已处理长度
	u16 total_len;//总长度
}UART_Struct;


extern UART_Struct g_sUART1;
extern UART_Struct g_sUART2;
extern UART_Struct g_sUART3;

void UART_Init(void);
void UART1_Init(void); 
void UART2_Init(void); 
void UART3_Init(void);

void UART_Send(u8 PortNum, u8 *buf, u16 len);
void UART_Clear(UART_Struct *pUART);

void UART1_Send(u8 *buf, u16 len);
void UART2_Send(u8 *buf, u16 len);
void UART3_Send(u8 *buf, u16 len);

#endif













