
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


