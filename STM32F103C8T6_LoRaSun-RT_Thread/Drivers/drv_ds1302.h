

#ifndef __DRV_DS1302_H
#define __DRV_DS1302_H

#include "drv_uart.h"

typedef struct
{
  uint8_t rtc_year;                /*!< RTC year value: 0x0 - 0x99(BCD format) */
  uint8_t rtc_month;               /*!< RTC month value */
  uint8_t rtc_date;                /*!< RTC date value: 0x1 - 0x31(BCD format) */
  uint8_t rtc_day_of_week;         /*!< RTC weekday value */
  uint8_t rtc_hour;                /*!< RTC hour value */
  uint8_t rtc_minute;              /*!< RTC minute value: 0x0 - 0x59(BCD format) */
  uint8_t rtc_second;              /*!< RTC second value: 0x0 - 0x59(BCD format) */
  uint32_t rtc_am_pm;              /*!< RTC AM/PM value */
  uint32_t rtc_display_format;     /*!< RTC time notation */  
}Ds1302RtcStruct;

typedef struct
{
	GPIO_TypeDef* port_clk;
	uint32_t pin_clk;
	GPIO_TypeDef* port_io;
	uint32_t pin_io;	
	GPIO_TypeDef* port_rst;
	uint32_t pin_rst;		
}Ds1302DriverStruct;

#define	 DS1302_RST_1		GPIO_SetBits(g_sDs1302Drv.port_rst, g_sDs1302Drv.pin_rst)
#define	 DS1302_RST_0		GPIO_ResetBits(g_sDs1302Drv.port_rst, g_sDs1302Drv.pin_rst)

#define	 DS1302_IO_1		GPIO_SetBits(g_sDs1302Drv.port_io, g_sDs1302Drv.pin_io)
#define	 DS1302_IO_0		GPIO_ResetBits(g_sDs1302Drv.port_io, g_sDs1302Drv.pin_io)

#define	 DS1302_CLK_1		GPIO_SetBits(g_sDs1302Drv.port_clk, g_sDs1302Drv.pin_clk)
#define	 DS1302_CLK_0		GPIO_ResetBits(g_sDs1302Drv.port_clk, g_sDs1302Drv.pin_clk)

#define	 DS1302_IO_READ()		GPIO_ReadInputDataBit(g_sDs1302Drv.port_io, g_sDs1302Drv.pin_io)


//DS1302寄存器操作指令定义时间地址
#define DS1302_SEC_ADDR   0x80       //秒 
#define DS1302_MIN_ADDR   0x82       //分
#define DS1302_HOUR_ADDR  0x84       //时
#define DS1302_DATE_ADDR  0x86       //日
#define DS1302_MONTH_ADDR 0x88       //月
#define DS1302_YEAR_ADDR  0x8c       //年 


void drv_ds1302_init(GPIO_TypeDef* port_clk, uint32_t pin_clk,
										 GPIO_TypeDef* port_io, uint32_t pin_io,
                     GPIO_TypeDef* port_rst, uint32_t pin_rst,
										 Ds1302RtcStruct *rtc_initpara);
void drv_ds1302_set_io_mode(bool is_out);

void drv_ds1302_write(u8 reg_addr, u8 reg_data);
u8 drv_ds1302_read(u8 reg_addr);
void drv_ds1302_set_time(Ds1302RtcStruct *rtc_initpara);
void drv_ds1302_get_time(Ds1302RtcStruct *rtc_initpara);
void drv_ds1302_stop(void);
void drv_ds1302_start(void);
void drv_ds1302_set_hour_format(bool is_24h);
void drv_ds1302_set_wp(bool enable);














#endif


