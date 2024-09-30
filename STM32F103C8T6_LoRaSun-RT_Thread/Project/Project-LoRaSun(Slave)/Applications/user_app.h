#ifndef __USER_APP_H__
#define __USER_APP_H__

#include "drv_uart.h"


#define		APP_NAME						"app_air"
#define		APP_VER 						"1.1"

/*
* 版本说明
*	V1.0 
*
*
*
*/

void APP_Init(void);

void app_uart_thread_entry(void *parameter);


#endif

