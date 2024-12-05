#ifndef __USER_OPT_H__
#define __USER_OPT_H__

#define RT_HEAP_SIZE 1024  //RTT的堆栈空间  该数值*4

#define		USE_UART1
#define		USE_UART2
#define		USE_UART3

#define		APP_NAME						"LoRaSun-Node"
#define		APP_VER 						"1.0.1"  //应用软件版本

//#define		USE_WDOG   //是否启用看门狗

#define		USE_RTC    //是否启用RTC


#ifdef		USE_UART1

#define		UART_DEBUG		USART1  //调试用的串口

#endif

#define		UART_LEN			256
#define		UART1_LEN			UART_LEN
#define		UART2_LEN			UART_LEN 
#define		UART3_LEN			UART_LEN


#define		BAUD_UART1		115200
#define		BAUD_UART2		9600
#define		BAUD_UART3		9600






#endif



















