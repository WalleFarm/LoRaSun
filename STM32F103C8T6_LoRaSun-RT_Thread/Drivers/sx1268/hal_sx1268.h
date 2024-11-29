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

#ifndef __HAL_SX1268_H__
#define __HAL_SX1268_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
typedef struct
{
	void (*sx1268_reset)(void);//复位函数
	u8 (*sx1268_spi_rw_byte)(u8 byte);//字节读写函数
	void (*sx1268_cs_0)(void);//片选0
	void (*sx1268_cs_1)(void);//片选1
  void (*delay_ms)(u32 dlyms);//延时
	void (*wait_on_busy)(void);//等待空闲
  void (*set_led)(bool state);
}HalSx1268Struct;

 
void hal_sx1268_rst(HalSx1268Struct *psx1268); 
u8 hal_sx1268_spi_rw_byte(HalSx1268Struct *psx1268, u8 byte);

void hal_sx1268_write_cmd(HalSx1268Struct *psx1268, u8 cmd, u8 *buff, u8 size);
u8 hal_sx1268_read_cmd(HalSx1268Struct *psx1268, u8 cmd, u8 *buff, u8 size);
void hal_sx1268_write_regs(HalSx1268Struct *psx1268, u16 addr, u8 *buff, u8 size);
void hal_sx1268_read_regs(HalSx1268Struct *psx1268, u16 addr, u8 *buff, u8 size);

void hal_sx1268_write_reg(HalSx1268Struct *psx1268, u16 addr, u8 data);
u8 hal_sx1268_read_reg(HalSx1268Struct *psx1268, u16 addr);

void hal_sx1268_write_fifo(HalSx1268Struct *psx1268, u8 *buff, u8 size);
void hal_sx1268_read_fifo(HalSx1268Struct *psx1268, u8 offset, u8 *buff, u8 size);
void hal_sx1268_set_led(HalSx1268Struct *psx1268, bool state);


#endif
