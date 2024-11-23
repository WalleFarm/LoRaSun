#ifndef __USER_APP_H__
#define __USER_APP_H__

#include "drv_uart.h"

void APP_Init(void);

void app_uart_thread_entry(void *parameter);


#endif

