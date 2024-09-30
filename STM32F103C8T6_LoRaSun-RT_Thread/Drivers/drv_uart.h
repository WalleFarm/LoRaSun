#ifndef __DRV_UART_H__
#define __DRV_UART_H__


#include "drv_common.h"

typedef struct
{
	USART_TypeDef* USARTx;
	u8 PortNum;//串口号
	u8 *pBuff;//数据指针
	u16 iRecv;//已接收长度
	u16 iDone;//已处理长度
	u16 total_len;//总长度
}UART_Struct;


extern UART_Struct g_sUART1;
extern UART_Struct g_sUART2;
extern UART_Struct g_sUART3;

void UART_Init(void);
void UART1_Init(void); 
void UART2_Init(void); 
void UART3_Init(void);

void UART_Send(u8 PortNum, u8 *buf, u16 len);
void UART_Clear(UART_Struct *pUART);

void UART1_Send(u8 *buf, u16 len);
void UART2_Send(u8 *buf, u16 len);
void UART3_Send(u8 *buf, u16 len);

#endif













