
#include "drv_oled96.h"
#include "oled_font.h"


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_init(DrvOLED96WorkStruct *pOLED96, GPIO_TypeDef* port_sda, u32 pin_sda, GPIO_TypeDef* port_scl, u32 pin_scl)
{
  I2cDriverStruct *pIIC=&pOLED96->tag_iic;
  pIIC->port_sda=port_sda;
  pIIC->pin_sda=pin_sda;
  pIIC->port_scl=port_scl;
  pIIC->pin_scl=pin_scl;
  
  IIC_GPIOInit(pIIC);    
  
	drv_oled96_write_byte(pOLED96,0xAE,OLED96_CMD);//--turn off oled panel
	drv_oled96_write_byte(pOLED96,0x00,OLED96_CMD);//---set low column address
	drv_oled96_write_byte(pOLED96,0x10,OLED96_CMD);//---set high column address
	drv_oled96_write_byte(pOLED96,0x40,OLED96_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	drv_oled96_write_byte(pOLED96,0x81,OLED96_CMD);//--set contrast control register
	drv_oled96_write_byte(pOLED96,0xCF,OLED96_CMD);// Set SEG Output Current Brightness
	drv_oled96_write_byte(pOLED96,0xA1,OLED96_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	drv_oled96_write_byte(pOLED96,0xC8,OLED96_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	drv_oled96_write_byte(pOLED96,0xA6,OLED96_CMD);//--set normal display
	drv_oled96_write_byte(pOLED96,0xA8,OLED96_CMD);//--set multiplex ratio(1 to 64)
	drv_oled96_write_byte(pOLED96,0x3f,OLED96_CMD);//--1/64 duty
	drv_oled96_write_byte(pOLED96,0xD3,OLED96_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	drv_oled96_write_byte(pOLED96,0x00,OLED96_CMD);//-not offset
	drv_oled96_write_byte(pOLED96,0xd5,OLED96_CMD);//--set display clock divide ratio/oscillator frequency
	drv_oled96_write_byte(pOLED96,0x80,OLED96_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	drv_oled96_write_byte(pOLED96,0xD9,OLED96_CMD);//--set pre-charge period
	drv_oled96_write_byte(pOLED96,0xF1,OLED96_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	drv_oled96_write_byte(pOLED96,0xDA,OLED96_CMD);//--set com pins hardware configuration
	drv_oled96_write_byte(pOLED96,0x12,OLED96_CMD);
	drv_oled96_write_byte(pOLED96,0xDB,OLED96_CMD);//--set vcomh
	drv_oled96_write_byte(pOLED96,0x40,OLED96_CMD);//Set VCOM Deselect Level
	drv_oled96_write_byte(pOLED96,0x20,OLED96_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	drv_oled96_write_byte(pOLED96,0x02,OLED96_CMD);//
	drv_oled96_write_byte(pOLED96,0x8D,OLED96_CMD);//--set Charge Pump enable/disable
	drv_oled96_write_byte(pOLED96,0x14,OLED96_CMD);//--set(0x10) disable
	drv_oled96_write_byte(pOLED96,0xA4,OLED96_CMD);// Disable Entire Display On (0xa4/0xa5)
	drv_oled96_write_byte(pOLED96,0xA6,OLED96_CMD);// Disable Inverse Display On (0xa6/a7) 
	drv_oled96_write_byte(pOLED96,0xAF,OLED96_CMD);
	drv_oled96_clear(pOLED96);  
  
}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_write_byte(DrvOLED96WorkStruct *pOLED96, u8 data, u8 mode)
{
  I2cDriverStruct *pIIC=&pOLED96->tag_iic;
  IIC_Start(pIIC);
  IIC_WriteByte(pIIC, 0x78);
  IIC_WaitAck(pIIC);
  if(mode)
    IIC_WriteByte(pIIC, 0x40);
  else
    IIC_WriteByte(pIIC, 0x00);
  IIC_WaitAck(pIIC);
  IIC_WriteByte(pIIC, data);
  IIC_WaitAck(pIIC);
  IIC_Stop(pIIC);
}



/*		
================================================================================
描述 : 开启屏幕
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_display_on(DrvOLED96WorkStruct *pOLED96)
{
  drv_oled96_write_byte(pOLED96, 0x8D, OLED96_CMD);//电荷泵使能
  drv_oled96_write_byte(pOLED96, 0x14, OLED96_CMD);//开启电荷泵
  drv_oled96_write_byte(pOLED96, 0xAF, OLED96_CMD);//点亮屏幕
}

/*		
================================================================================
描述 : 关闭屏幕
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_display_off(DrvOLED96WorkStruct *pOLED96)
{
  drv_oled96_write_byte(pOLED96, 0x8D, OLED96_CMD);//电荷泵使能
  drv_oled96_write_byte(pOLED96, 0x10, OLED96_CMD);//关闭电荷泵
  drv_oled96_write_byte(pOLED96, 0xAF, OLED96_CMD);//关闭屏幕
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_color_turn(DrvOLED96WorkStruct *pOLED96, u8 mode)
{
  if(mode==0)
  {
    drv_oled96_write_byte(pOLED96, 0xA6, OLED96_CMD);//正常显示
  }
  else
  {
    drv_oled96_write_byte(pOLED96, 0xA6, OLED96_CMD);//反色显示
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_display_turn(DrvOLED96WorkStruct *pOLED96, u8 mode)
{
  if(mode==0)
  {
    drv_oled96_write_byte(pOLED96, 0xC8, OLED96_CMD);//正常显示
    drv_oled96_write_byte(pOLED96, 0xA1, OLED96_CMD);
  }
  else
  {
    drv_oled96_write_byte(pOLED96, 0xC0, OLED96_CMD);//反转显示
    drv_oled96_write_byte(pOLED96, 0xA0, OLED96_CMD);
  }
}

/*		
================================================================================
描述 : 清屏
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_clear(DrvOLED96WorkStruct *pOLED96)
{
  for(u8 i=0; i<8; i++)
  {
    drv_oled96_write_byte(pOLED96, 0xb0+i, OLED96_CMD); 
    drv_oled96_write_byte(pOLED96, 0x00, OLED96_CMD);
    drv_oled96_write_byte(pOLED96, 0x10, OLED96_CMD);   
    for(u8 n=0; n<OLED96_WIDTH; n++)
    {
      drv_oled96_write_byte(pOLED96, 0, OLED96_DATA);
    }    
  }
}

/*		
================================================================================
描述 : 
输入 : x:0~128(F6X8字体最大是122, F8X16最大是120)  y:0~7(一个字节8个像素,高64像素,所以y有8格)
输出 : 
================================================================================
*/
void drv_oled96_set_pos(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y)
{
	drv_oled96_write_byte(pOLED96, 0xb0+y, OLED96_CMD); 
	drv_oled96_write_byte(pOLED96, ((x&0xf0)>>4)|0x10, OLED96_CMD);
	drv_oled96_write_byte(pOLED96, (x&0x0f)|0x01, OLED96_CMD);  
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_oled96_show_char(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y, char ch, u8 size)
{
  u8 offset=ch-' ';
  switch(size)
  {
    case OLEDFont6:
    {
      drv_oled96_set_pos(pOLED96, x, y);
      for(u8 i=0; i<6; i++)
      {
        u8 temp=F6x8[offset][i];
        drv_oled96_write_byte(pOLED96, temp, OLED96_DATA);
      }      
      break;
    }
    case OLEDFont16:
    {
      drv_oled96_set_pos(pOLED96, x, y);
      for(u8 i=0; i<8; i++)
      {
        u8 temp=F8X16[offset*16+i];
        drv_oled96_write_byte(pOLED96, temp, OLED96_DATA);
      }
      drv_oled96_set_pos(pOLED96, x, y+1);
      for(u8 i=0; i<8; i++)
      {
        u8 temp=F8X16[offset*16+i+8];
        drv_oled96_write_byte(pOLED96, temp, OLED96_DATA);
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
void drv_oled96_show_str_f6x8(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y, char *str)
{
	while((*str>=' ')&&(*str<='~'))//非法字符检查
	{
		drv_oled96_show_char(pOLED96, x, y, *str, OLEDFont6);
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
void drv_oled96_show_str_f8x16(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y, char *str)
{
	while((*str>=' ')&&(*str<='~'))//非法字符检查
	{
		drv_oled96_show_char(pOLED96, x, y, *str, OLEDFont16);
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
void drv_oled96_show_chinese(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y, u8 no)
{
  drv_oled96_set_pos(pOLED96, x, y);
  for(u8 i=0; i<16; i++)
  {
    u8 temp=Hzk[2*no][i];
    drv_oled96_write_byte(pOLED96, temp, OLED96_DATA);
  }  
  drv_oled96_set_pos(pOLED96, x, y+1);
  for(u8 i=0; i<16; i++)
  {
    u8 temp=Hzk[2*no+1][i];
    drv_oled96_write_byte(pOLED96, temp, OLED96_DATA);
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







