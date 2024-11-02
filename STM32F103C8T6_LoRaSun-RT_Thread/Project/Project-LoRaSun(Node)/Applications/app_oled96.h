
#ifndef __APP_OLED96_H__
#define __APP_OLED96_H__

#include "drv_oled96.h"

typedef struct
{
	DrvOLED96WorkStruct oled96_work;

}AppOLEDWorkStruct;

void app_oled96_init(void);

void app_oled96_main_process(void);


void app_oled96_show_main(void);
void app_oled96_show_node(u32 node_sn, u16 period);
void app_oled96_show_signal(s16 rssi, s8 snr);
void app_oled96_show_time(void);
void app_oled96_show_tx_total(u16 tx_cnts, u16 ok_cnts);
void app_oled96_show_debug(char *str);
void printf_oled(char const *const format, ...);
void app_oled96_show_recv(u8 *recv_buff, u16 recv_len);



#endif



