
#include "drv_ds1302.h"

 
Ds1302DriverStruct g_sDs1302Drv={0};


/*		 
================================================================================
描述 : 硬件初始化
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_init(GPIO_TypeDef* port_clk, uint32_t pin_clk,
										 GPIO_TypeDef* port_io, uint32_t pin_io,
                     GPIO_TypeDef* port_rst, uint32_t pin_rst,
										Ds1302RtcStruct *rtc_initpara)
{
	g_sDs1302Drv.port_clk=port_clk;
	g_sDs1302Drv.pin_clk=pin_clk;
	g_sDs1302Drv.port_io=port_io;
	g_sDs1302Drv.pin_io=pin_io;
	g_sDs1302Drv.port_rst=port_rst;
	g_sDs1302Drv.pin_rst=pin_rst; 
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = pin_clk;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port_clk, &GPIO_InitStruct);	
  
	GPIO_InitStruct.GPIO_Pin = pin_io;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port_io, &GPIO_InitStruct);	

	GPIO_InitStruct.GPIO_Pin = pin_rst;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port_rst, &GPIO_InitStruct);	
  

	DS1302_RST_0; 	
	DS1302_CLK_0;
	delay_us(5);
	drv_ds1302_set_wp(false);
	drv_ds1302_write(0x90, 0xA7);//充电寄存器
	drv_ds1302_set_wp(true);
	
	drv_ds1302_set_hour_format(true);//24h制
	if(drv_ds1302_read(0x81)&0x80)//查询DS1302时钟是否启动,如果时钟停止走动：启动时钟+初始化时钟
	{		
		printf("DS1302 stop!\n");
		drv_ds1302_set_time(rtc_initpara);
		drv_ds1302_start();
	}

}

/*		 
================================================================================
描述 : 数据脚读写转换
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_set_io_mode(bool is_out)
{
	if(is_out)
	{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = g_sDs1302Drv.pin_io;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(g_sDs1302Drv.port_io, &GPIO_InitStruct);			
	}
	else
	{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = g_sDs1302Drv.pin_io;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(g_sDs1302Drv.port_io, &GPIO_InitStruct);
	}
}

/*		 
================================================================================
描述 : 写字节
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_write(u8 reg_addr, u8 reg_data)
{
	DS1302_RST_0;
	DS1302_CLK_0;
	delay_us(2);
	DS1302_RST_1;
	delay_us(5);
	for(u8 i=0; i<8; i++)
	{
		if(reg_addr&0x01)DS1302_IO_1;
		else DS1302_IO_0;
		DS1302_CLK_1;
		delay_us(5);
		DS1302_CLK_0;
		reg_addr=reg_addr>>1;
	}
	
	for(u8 i=0; i<8; i++)
	{
		if(reg_data&0x01)DS1302_IO_1;
		else DS1302_IO_0;
		DS1302_CLK_1;
		delay_us(5);
		DS1302_CLK_0;
		reg_data=reg_data>>1;
	}	
	DS1302_RST_0;
	delay_us(5);
}

/*		 
================================================================================
描述 : 读字节
输入 : 
输出 :  
================================================================================
*/
u8 drv_ds1302_read(u8 reg_addr)
{
	DS1302_RST_1;
	delay_us(5);
	for(u8 i=0; i<8; i++)
	{
		if(reg_addr&0x01)DS1302_IO_1;
		else DS1302_IO_0;
		DS1302_CLK_1;
		delay_us(5);
		DS1302_CLK_0;
		reg_addr=reg_addr>>1;
	}
	
	drv_ds1302_set_io_mode(0);
	u8 value=0;
	for(u8 i=0; i<8; i++)
	{
		value=value>>1;
		if(DS1302_IO_READ()>0)value|=0x80; 

		DS1302_CLK_1;
		delay_us(5);
		DS1302_CLK_0;
	}	
	drv_ds1302_set_io_mode(1);
	DS1302_RST_0;	
	delay_us(5);
	return value;	
}

/*		 
================================================================================
描述 : 时间设置
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_set_time(Ds1302RtcStruct *rtc_initpara)
{
	if(rtc_initpara==NULL)
		return;
	printf("drv_ds1302_set_time=20%02x-%02x-%02x  %02x:%02x:%02x\n", rtc_initpara->rtc_year, rtc_initpara->rtc_month, rtc_initpara->rtc_date,
																																rtc_initpara->rtc_hour, rtc_initpara->rtc_minute, rtc_initpara->rtc_second	);
	drv_ds1302_set_wp(false);
	drv_ds1302_write(DS1302_SEC_ADDR,rtc_initpara->rtc_second);
	drv_ds1302_write(DS1302_MIN_ADDR,rtc_initpara->rtc_minute);
	drv_ds1302_write(DS1302_HOUR_ADDR,rtc_initpara->rtc_hour);
	
	drv_ds1302_write(DS1302_DATE_ADDR,rtc_initpara->rtc_date);
	drv_ds1302_write(DS1302_MONTH_ADDR,rtc_initpara->rtc_month);
	drv_ds1302_write(DS1302_YEAR_ADDR,rtc_initpara->rtc_year);
	
	drv_ds1302_set_wp(true);
}

/*		 
================================================================================
描述 : 时间读取
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_get_time(Ds1302RtcStruct *rtc_initpara)
{
	if(rtc_initpara==NULL)
		return;
	rtc_initpara->rtc_second=drv_ds1302_read(0x81);
	rtc_initpara->rtc_minute=drv_ds1302_read(0x83);
	rtc_initpara->rtc_hour=drv_ds1302_read(0x85);
	rtc_initpara->rtc_date=drv_ds1302_read(0x87);
	rtc_initpara->rtc_month=drv_ds1302_read(0x89);
	rtc_initpara->rtc_year=drv_ds1302_read(0x8D);
//	printf("drv_ds1302_get_time=20%02x-%02x-%02x  %02x:%02x:%02x\n", rtc_initpara->rtc_year, rtc_initpara->rtc_month, rtc_initpara->rtc_date,
//																																rtc_initpara->rtc_hour, rtc_initpara->rtc_minute, rtc_initpara->rtc_second	);	
}

/*		 
================================================================================
描述 : 停止
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_stop(void)
{
	drv_ds1302_set_wp(false);
	u8 ret=drv_ds1302_read(0x81);
	ret=ret|0x80;
	drv_ds1302_write(0x80,ret);
	drv_ds1302_set_wp(true);
}


/*		 
================================================================================
描述 : 启动
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_start(void)
{
	drv_ds1302_set_wp(false);
	u8 ret=drv_ds1302_read(0x81);
	ret=ret&0x7F;
	drv_ds1302_write(0x80,ret);
	drv_ds1302_set_wp(true);
}

/*		 
================================================================================
描述 : 设置时间格式
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_set_hour_format(bool is_24h)
{
	drv_ds1302_set_wp(false);
	if(is_24h)
	{
		u8 ret=drv_ds1302_read(0x85);
		ret=ret&0x7F;
		drv_ds1302_write(0x84,ret);		
		printf("ds1302 set 24h\n");
	}
	else
	{
		u8 ret=drv_ds1302_read(0x85);
		ret=ret|0x80;
		drv_ds1302_write(0x84,ret);		
		printf("ds1302 set 12h\n");
	}
	drv_ds1302_set_wp(true);
}

/*		 
================================================================================
描述 :
输入 : 
输出 :  
================================================================================
*/
void drv_ds1302_set_wp(bool enable)
{
	if(enable)drv_ds1302_write(0x8e,0x80);	
	else drv_ds1302_write(0x8e,0x00);	
}










