
#ifndef __APP_NODE_H__
#define __APP_NODE_H__
#include "nwk_node.h" 
#include "drv_uart.h"
#include "app_oled96.h"

typedef struct
{
  u32 gw_sn;
  u8 base_freq_ptr;
  u8 wireless_num;
}AppNodeGwSaveStruct;

typedef struct
{
  u32 node_sn;
  AppNodeGwSaveStruct gw_save_list[NWK_GW_NUM];
  u16 wake_period;
  u16 crcValue;
}AppNodeSaveStruct;

typedef struct
{
  u8 temp;
//  DrvOLED96WorkStruct oled96_work;
}AppNodeWorkStruct;


void app_node_set_sn(u32 node_sn);
void app_node_set_wake_period(u16 wake_period);

void app_node_send_status(void);
void app_node_thread_entry(void *parameter); 

#endif


















