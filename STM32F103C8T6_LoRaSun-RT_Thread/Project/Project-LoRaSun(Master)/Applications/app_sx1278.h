#ifndef __APP_SX1278_H__
#define __APP_SX1278_H__
//#include "nwk_node.h" 
//#include "nwk_slave.h" 
#include "nwk_master.h" 
#include "drv_uart.h"



void app_sx1278_init(void);
void app_sx1278_thread_entry(void *parameter); 

void bsp_sx1278_set_param(u32 freq, u8 sf, u8 bw);

#endif


