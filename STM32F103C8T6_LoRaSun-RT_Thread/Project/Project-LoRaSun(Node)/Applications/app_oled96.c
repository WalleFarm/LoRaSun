

#include "app_oled96.h"

AppOLEDWorkStruct g_sAppOLEDWork={0};

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_oled96_init(void)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  
  drv_oled96_init(pOLED96, GPIOB, GPIO_Pin_7, GPIOB, GPIO_Pin_6);//SDA--PB7  SCL--PB6

  app_oled96_show_main();
  delay_os(1000);
  drv_oled96_clear(pOLED96);
  app_oled96_show_signal(-129, 48);
  app_oled96_show_tx_total(12345, 12321);

//  printf_oled("==debug 1234657890123456 time=%us", drv_get_rtc_counter());
  
}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_main(void)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  for(u8 i=0; i<4; i++)
  {
    drv_oled96_show_chinese(pOLED96, 30+i*16, 2, i);//汉字显示
  }

  drv_oled96_show_str_f8x16(pOLED96, 20, 4,">>LoRaSun<<");     
}

/*		
================================================================================
描述 : 显示节点信息
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_node(u32 node_sn, u16 period)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  char buff[30]={0};
  sprintf(buff, "1.SN:%08X, %ds", node_sn, period);
  
  drv_oled96_clear_line(pOLED96, 0);//清理
  drv_oled96_show_str_f6x8(pOLED96, 0, 0, buff);   
}

/*		
================================================================================
描述 : 显示接收信号质量
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_signal(s16 rssi, s8 snr)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  char buff[30]={0};
  sprintf(buff, "2.SI:%ddbm, %ddbm", rssi, snr);
  drv_oled96_clear_line(pOLED96, 1);//清理
  drv_oled96_show_str_f6x8(pOLED96, 0, 1, buff);  
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_time(void)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  char buff[30]={0};
  sprintf(buff, "3.RTC:%us", drv_get_rtc_counter());
//  drv_oled96_clear_line(pOLED96, 2);//清理
  drv_oled96_show_str_f6x8(pOLED96, 0, 2, buff);   
}

/*		
================================================================================
描述 : 显示发送统计
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_tx_total(u16 total_cnts, u16 ok_cnts)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  char buff[30]={0};
  float percent=100.f*ok_cnts/total_cnts;
//  printf("percent=%.1f%%\n", percent);
  sprintf(buff, "4.%d/%d=%.1f%%", ok_cnts, total_cnts, percent);
  drv_oled96_clear_line(pOLED96, 3);//清理
  drv_oled96_show_str_f6x8(pOLED96, 0, 3, buff);   
}

/*		
================================================================================
描述 : 显示调试信息,单行,不要太多内容
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_debug(char *str)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  drv_oled96_clear_line(pOLED96, 4);//清理
  drv_oled96_clear_line(pOLED96, 5);//清理
  drv_oled96_clear_line(pOLED96, 6);//清理
  drv_oled96_clear_line(pOLED96, 7);//清理
  drv_oled96_show_str_f6x8(pOLED96, 0, 4, str); 
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void printf_oled(char const *const format, ...)
{
	static char show_buff[200];
  memset(show_buff, 0, sizeof(show_buff));

	__va_list  arglist;

	va_start(arglist, format);
	vsnprintf(show_buff, sizeof(show_buff), format, arglist);
	va_end(arglist); 
  app_oled96_show_debug(show_buff);
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_recv(u8 *recv_buff, u16 recv_len)
{
  DrvOLED96WorkStruct *pOLED96=&g_sAppOLEDWork.oled96_work;
  if(recv_len>50)recv_len=50;
  static char show_buff[200]={0};
  memset(show_buff, 0, sizeof(show_buff));
  for(u16 i=0; i<recv_len; i++)
  {
    char temp_buff[6]={0};
    sprintf(temp_buff, "%02X ", recv_buff[i]);
    strcat(show_buff, temp_buff);
  }
  drv_oled96_show_str_f6x8(pOLED96, 0, 5, show_buff); 
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_oled96_main_process(void)
{
  static u32 last_time=0;
  u32 now_time=drv_get_rtc_counter();
  if(now_time-last_time>0)
  {
    app_oled96_show_time();
    last_time=now_time;
  }
}







  






