
#ifndef __APP_OLED96_H__
#define __APP_OLED96_H__

#include "drv_oled96.h"

typedef struct
{
  u32 node_sn;
  u16 wake_period;
  s16 rssi;
  s8 snr;
  u16 total_cnts, ok_cnts;
  
  u32 gw_sn;
  u8 freq_ptr, run_mode;
  char debug_str[200];
}AppOLEDShowNodeStruct;//要显示的信息

void app_oled96_init(void);
 
void app_oled96_thread_entry(void *parameter);


void app_oled96_show_start(void);
void app_oled96_show_node(u32 node_sn, u16 period);
void app_oled96_show_signal(s16 rssi, s8 snr);
void app_oled96_show_time(void);
void app_oled96_show_tx_total(u16 total_cnts, u16 ok_cnts);
void app_oled96_show_gw_info(u32 gw_sn, u8 freq_ptr, u8 run_mode);
void app_oled96_show_version(void);
void app_oled96_show_debug(char *str);
void printf_oled(char const *const format, ...);

void app_oled96_thread_entry(void *parameter);


#endif



