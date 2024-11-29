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

#ifndef __DRV_OLED96_H__
#define __DRV_OLED96_H__

#include "drv_iic.h" 

#define			OLED96_WIDTH			128
#define			OLED96_HEIGHT			64
#define			OLED96_CMD				0   //写命令
#define			OLED96_DATA			  1   //写数据

typedef enum
{
	OLEDFont6=6,
	OLEDFont16=16,
	OLEDFont24=24,
}OLEDFontSize;

typedef struct
{
//	I2cDriverStruct tag_iic;
//	u8 show_ram[144][8];
  void (*write_byte)(u8 data, u8 mode);
}DrvOLED96WorkStruct;
 

void drv_oled96_init(void);
void drv_oled96_write_byte_register(void (*write_byte)(u8 data, u8 mode));
void drv_oled96_write_byte(u8 data, u8 mode);
void drv_oled96_display_on(void);
void drv_oled96_display_off(void);
void drv_oled96_color_turn(u8 mode);
void drv_oled96_display_turn(u8 mode);
void drv_oled96_clear(void);
void drv_oled96_clear_line(u8 line);

void drv_oled96_set_pos(u8 x, u8 y);
void drv_oled96_show_char(u8 x,u8 y,char ch,u8 size);
void drv_oled96_show_str_f6x8(u8 x, u8 y, char *str);
void drv_oled96_show_str_f8x16(u8 x, u8 y, char *str);
void drv_oled96_show_chinese(u8 x, u8 y, u8 no);




#endif


