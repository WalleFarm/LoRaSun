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

#include "drv_oled96.h"
#include "oled_font.h"

DrvOLED96WorkStruct g_sDrvOLED96Work={0};
/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_init(void)
{  
  
	drv_oled96_write_byte(0xAE,OLED96_CMD);//--turn off oled panel
	drv_oled96_write_byte(0x00,OLED96_CMD);//---set low column address
	drv_oled96_write_byte(0x10,OLED96_CMD);//---set high column address
	drv_oled96_write_byte(0x40,OLED96_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	drv_oled96_write_byte(0x81,OLED96_CMD);//--set contrast control register
	drv_oled96_write_byte(0xCF,OLED96_CMD);// Set SEG Output Current Brightness
	drv_oled96_write_byte(0xA1,OLED96_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	drv_oled96_write_byte(0xC8,OLED96_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	drv_oled96_write_byte(0xA6,OLED96_CMD);//--set normal display
	drv_oled96_write_byte(0xA8,OLED96_CMD);//--set multiplex ratio(1 to 64)
	drv_oled96_write_byte(0x3f,OLED96_CMD);//--1/64 duty
	drv_oled96_write_byte(0xD3,OLED96_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	drv_oled96_write_byte(0x00,OLED96_CMD);//-not offset
	drv_oled96_write_byte(0xd5,OLED96_CMD);//--set display clock divide ratio/oscillator frequency
	drv_oled96_write_byte(0x80,OLED96_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	drv_oled96_write_byte(0xD9,OLED96_CMD);//--set pre-charge period
	drv_oled96_write_byte(0xF1,OLED96_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	drv_oled96_write_byte(0xDA,OLED96_CMD);//--set com pins hardware configuration
	drv_oled96_write_byte(0x12,OLED96_CMD);
	drv_oled96_write_byte(0xDB,OLED96_CMD);//--set vcomh
	drv_oled96_write_byte(0x40,OLED96_CMD);//Set VCOM Deselect Level
	drv_oled96_write_byte(0x20,OLED96_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	drv_oled96_write_byte(0x02,OLED96_CMD);//
	drv_oled96_write_byte(0x8D,OLED96_CMD);//--set Charge Pump enable/disable
	drv_oled96_write_byte(0x14,OLED96_CMD);//--set(0x10) disable
	drv_oled96_write_byte(0xA4,OLED96_CMD);// Disable Entire Display On (0xa4/0xa5)
	drv_oled96_write_byte(0xA6,OLED96_CMD);// Disable Inverse Display On (0xa6/a7) 
	drv_oled96_write_byte(0xAF,OLED96_CMD);
	drv_oled96_clear();  
  
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_write_byte_register(void (*write_byte)(u8 data, u8 mode))
{
  g_sDrvOLED96Work.write_byte=write_byte;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_write_byte(u8 data, u8 mode)
{
  if(g_sDrvOLED96Work.write_byte)
  {
    g_sDrvOLED96Work.write_byte(data, mode);
  }
}



/*		
================================================================================
描述 : 开启屏幕
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_display_on(void)
{
  drv_oled96_write_byte( 0x8D, OLED96_CMD);//电荷泵使能
  drv_oled96_write_byte( 0x14, OLED96_CMD);//开启电荷泵
  drv_oled96_write_byte( 0xAF, OLED96_CMD);//点亮屏幕
}

/*		
================================================================================
描述 : 关闭屏幕
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_display_off(void)
{
  drv_oled96_write_byte( 0x8D, OLED96_CMD);//电荷泵使能
  drv_oled96_write_byte( 0x10, OLED96_CMD);//关闭电荷泵
  drv_oled96_write_byte( 0xAF, OLED96_CMD);//关闭屏幕
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_color_turn(u8 mode)
{
  if(mode==0)
  {
    drv_oled96_write_byte( 0xA6, OLED96_CMD);//正常显示
  }
  else
  {
    drv_oled96_write_byte( 0xA6, OLED96_CMD);//反色显示
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_display_turn(u8 mode)
{
  if(mode==0)
  {
    drv_oled96_write_byte( 0xC8, OLED96_CMD);//正常显示
    drv_oled96_write_byte( 0xA1, OLED96_CMD);
  }
  else 
  {
    drv_oled96_write_byte( 0xC0, OLED96_CMD);//反转显示
    drv_oled96_write_byte( 0xA0, OLED96_CMD);
  }
}

/*		
================================================================================
描述 : 清屏
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_clear(void)
{
  for(u8 i=0; i<8; i++)
  {
    drv_oled96_write_byte( 0xb0+i, OLED96_CMD); 
    drv_oled96_write_byte( 0x00, OLED96_CMD);
    drv_oled96_write_byte( 0x10, OLED96_CMD);   
    for(u8 n=0; n<OLED96_WIDTH; n++)
    {
      drv_oled96_write_byte( 0, OLED96_DATA);
    }    
  }
}

/*		
================================================================================
描述 : 清理单行
输入 : line:0~7
输出 : 
================================================================================
*/
void drv_oled96_clear_line(u8 line)
{
    drv_oled96_write_byte( 0xb0+line, OLED96_CMD); 
    drv_oled96_write_byte( 0x00, OLED96_CMD);
    drv_oled96_write_byte( 0x10, OLED96_CMD);   
    for(u8 n=0; n<OLED96_WIDTH; n++)
    {
      drv_oled96_write_byte( 0, OLED96_DATA);
    }  
}

/*		
================================================================================
描述 : 
输入 : x:0~128(F6X8字体最大是122, F8X16最大是120)  y:0~7(一个字节8个像素,高64像素,所以y有8格)
输出 : 
================================================================================
*/
void drv_oled96_set_pos(u8 x, u8 y)
{
	drv_oled96_write_byte( 0xb0+y, OLED96_CMD); 
	drv_oled96_write_byte( ((x&0xf0)>>4)|0x10, OLED96_CMD);
	drv_oled96_write_byte( (x&0x0f)|0x01, OLED96_CMD);  
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_show_char(u8 x, u8 y, char ch, u8 size)
{
  u8 offset=ch-' ';
  switch(size)
  {
    case OLEDFont6:
    {
      drv_oled96_set_pos( x, y);
      for(u8 i=0; i<6; i++)
      {
        u8 temp=F6x8[offset][i];
        drv_oled96_write_byte( temp, OLED96_DATA);
      }      
      break;
    }
    case OLEDFont16:
    {
      drv_oled96_set_pos( x, y);
      for(u8 i=0; i<8; i++)
      {
        u8 temp=F8X16[offset*16+i];
        drv_oled96_write_byte( temp, OLED96_DATA);
      }
      drv_oled96_set_pos( x, y+1);
      for(u8 i=0; i<8; i++)
      {
        u8 temp=F8X16[offset*16+i+8];
        drv_oled96_write_byte( temp, OLED96_DATA);
      }      
      break;
    }    
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_show_str_f6x8(u8 x, u8 y, char *str)
{
	while((*str>=' ')&&(*str<='~'))//非法字符检查
	{
		drv_oled96_show_char( x, y, *str, OLEDFont6);
		x+=6;
		if(x>OLED96_WIDTH-6)  //换行
		{
			x=0;
			y+=1;
    }
		str++;
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_show_str_f8x16(u8 x, u8 y, char *str)
{
	while((*str>=' ')&&(*str<='~'))//非法字符检查
	{
		drv_oled96_show_char( x, y, *str, OLEDFont16);
		x+=8;
		if(x>OLED96_WIDTH-8)  //换行
		{
			x=0;
			y+=2;
    }
		str++;
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_show_chinese(u8 x, u8 y, u8 no)
{
  drv_oled96_set_pos( x, y);
  for(u8 i=0; i<16; i++)
  {
    u8 temp=Hzk[2*no][i];
    drv_oled96_write_byte( temp, OLED96_DATA);
  }  
  drv_oled96_set_pos( x, y+1);
  for(u8 i=0; i<16; i++)
  {
    u8 temp=Hzk[2*no+1][i];
    drv_oled96_write_byte( temp, OLED96_DATA);
  }    
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/







