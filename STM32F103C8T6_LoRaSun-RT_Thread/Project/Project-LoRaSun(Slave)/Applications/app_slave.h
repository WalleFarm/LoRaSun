
#ifndef __APP_SLAVE_H__
#define __APP_SLAVE_H__
#include "nwk_slave.h" 
#include "drv_uart.h"


typedef struct
{
  u8 slave_addr;
  u8 reserved;
  u16 crcValue;
}AppSlaveSaveStruct;

void app_slave_thread_entry(void *parameter); 

void app_slave_write_addr(u8 slave_addr);



#endif

