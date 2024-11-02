
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
	I2cDriverStruct tag_iic;
//	u8 show_ram[144][8];
}DrvOLED96WorkStruct;


void drv_oled96_init(DrvOLED96WorkStruct *pOLED96, GPIO_TypeDef* port_sda, u32 pin_sda, GPIO_TypeDef* port_scl, u32 pin_scl);
void drv_oled96_write_byte(DrvOLED96WorkStruct *pOLED96, u8 data, u8 mode);
void drv_oled96_display_on(DrvOLED96WorkStruct *pOLED96);
void drv_oled96_display_off(DrvOLED96WorkStruct *pOLED96);
void drv_oled96_color_turn(DrvOLED96WorkStruct *pOLED96, u8 mode);
void drv_oled96_display_turn(DrvOLED96WorkStruct *pOLED96, u8 mode);
void drv_oled96_clear(DrvOLED96WorkStruct *pOLED96);

void drv_oled96_set_pos(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y);
void drv_oled96_show_char(DrvOLED96WorkStruct *pOLED96, u8 x,u8 y,char ch,u8 size);
void drv_oled96_show_str_f6x8(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y, char *str);
void drv_oled96_show_str_f8x16(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y, char *str);
void drv_oled96_show_chinese(DrvOLED96WorkStruct *pOLED96, u8 x, u8 y, u8 no);




#endif


