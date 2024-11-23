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

#ifndef __HAL_SX1278B_H__
#define __HAL_SX1278B_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
typedef struct
{
	void (*sx1278_reset)(void);//复位函数
	u8 (*sx1278_spi_rw_byte)(u8 byte);//字节读写函数
	void (*sx1278_cs_0)(void);//片选0
	void (*sx1278_cs_1)(void);//片选1
  void (*set_led)(bool state);
}HalSx1278Struct;

 
void hal_sx1278_rst(HalSx1278Struct *psx1278); 
u8 hal_sx1278_spi_rw_byte(HalSx1278Struct *psx1278, u8 byte);
void hal_sx1278_write_buffer(HalSx1278Struct *psx1278, u8 addr, u8 *buff, u8 size);
void hal_sx1278_read_buffer(HalSx1278Struct *psx1278, u8 addr, u8 *buff, u8 size);

void hal_sx1278_write_addr(HalSx1278Struct *psx1278, u8 addr, u8 data);
u8 hal_sx1278_read_addr(HalSx1278Struct *psx1278, u8 addr);
void hal_sx1278_write_fifo(HalSx1278Struct *psx1278, u8 *buff, u8 size);
void hal_sx1278_read_fifo(HalSx1278Struct *psx1278, u8 *buff, u8 size);

void hal_sx1278_set_led(HalSx1278Struct *psx1278, bool state);



#endif
