/******************************************************************************
*
* Copyright (c) 2024 小易
* 本项目开源文件遵循GPL-v3协议
* 
* 文章专栏地址:https://blog.csdn.net/ypp240124016/category_12834955
* github主页:      https://github.com/WalleFarm
* LoRaSun开源地址: https://github.com/WalleFarm/LoRaSun
* M2M-IOT开源地址: https://github.com/WalleFarm/M2M-IOT
* 协议栈原理专利:CN110572843A (一种基于LoRa无线模块CAD模式的嗅探方法及系统)
*
* 测试套件采购地址:https://duandianwulian.taobao.com/
*
* 作者:小易
* 博客主页:https://blog.csdn.net/ypp240124016?type=blog
* 交流QQ群:701889554  (资料文件存放)
* 微信公众号:端点物联 (即时接收教程更新通知)
*
* 所有学习资源合集:https://blog.csdn.net/ypp240124016/article/details/143068017
*
* 免责声明:本项目所有资料仅限于学习和交流使用,请勿商用.
*
********************************************************************************/



#include "drv_uart.h"



UART_Struct g_sUART1={0},g_sUART2={0},g_sUART3={0};


/*		
================================================================================
描述 :串口初始化程序
输入 : 
输出 : 
================================================================================
*/
void UART_Init(void)
{
#ifdef	USE_UART1
	UART1_Init();
#endif	
#ifdef	USE_UART2
	UART2_Init();
#endif

#ifdef	USE_UART3
	UART3_Init();
#endif

	
	printf("UART Init ok!\n");
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void UART_Clear(UART_Struct *pUART)
{
	if(pUART==NULL)
	{
		return;
	}
	memset(pUART->pBuff, 0, pUART->total_len);
	pUART->iRecv=0;
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
#ifdef	USE_UART1
u8 g_u8UART1_Buff[UART1_LEN];//缓冲区

void UART1_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;// GPIO_Mode_AF_OD
	GPIO_Init(GPIOA, &GPIO_InitStructure);    //TX

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    //RX	
	

	USART_InitStructure.USART_BaudRate = BAUD_UART1;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收和发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //使能串口中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //主优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //初始化NVIC
 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//接收中断
	USART_Cmd(USART1, ENABLE);                    //使能串口	
	
	g_sUART1.USARTx=USART1;//串口地址
	g_sUART1.PortNum=1;//串口号
	g_sUART1.pBuff=g_u8UART1_Buff;//把数据指针指向接收缓冲区
	g_sUART1.total_len=UART1_LEN;//缓冲区长度
}

/*		
================================================================================
描述 :串口中断函数
输入 : 
输出 : 
================================================================================
*/
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)//获取接收状态
	{
		g_sUART1.pBuff[g_sUART1.iRecv]=USART_ReceiveData(USART1);//缓存接收字节		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清理中断位
		USART_ClearFlag(USART1, USART_FLAG_RXNE);//清理标志位
		g_sUART1.iRecv++;//接收长度加1
		if(g_sUART1.iRecv>=UART1_LEN)
			g_sUART1.iRecv=0;
	}
	else if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//接收错误
	{
		USART_ReceiveData(USART1);
//		USART_ClearITPendingBit(USART1, USART_IT_ORE);
		USART_ClearFlag(USART1, USART_FLAG_ORE);
	}	
}


/*		
================================================================================
描述 :串口1发送
输入 : 
输出 : 
================================================================================
*/
void UART1_Send(u8 *buf, u16 len)
{
	u16 i;
 for(i=0;i<len;i++)
 {
	 while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//发送完成
	 USART_SendData(USART1,buf[i]);
 }
 while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//发送完成		
 delay_ms(2); 
}

#endif
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
#ifdef	USE_UART2
u8 g_u8UART2_Buff[UART2_LEN];

void UART2_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    //TX

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    //RX	
	
 
	USART_InitStructure.USART_BaudRate = BAUD_UART2;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收和发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //使能串口中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //主优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //初始化NVIC
 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//接收中断
	USART_Cmd(USART2, ENABLE);                    //使能串口		
	

	g_sUART2.USARTx=USART2;
	g_sUART2.PortNum=2;
	g_sUART2.pBuff=g_u8UART2_Buff;//把数据指针指向接收缓冲区 
	g_sUART2.total_len=UART2_LEN;
}
/*		
================================================================================
描述 :串口中断函数
输入 : 
输出 : 
================================================================================
*/
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		g_sUART2.pBuff[g_sUART2.iRecv]=USART_ReceiveData(USART2);		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
		g_sUART2.iRecv++;
		if(g_sUART2.iRecv>=UART2_LEN)
			g_sUART2.iRecv=0;
	} 
	else if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2, USART_IT_ORE);
		USART_ClearFlag(USART2, USART_FLAG_ORE);
	}	
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void UART2_Send(u8 *buf, u16 len)
{
	u16 i;
 for(i=0;i<len;i++)
 {
	 while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//发送完成
	 USART_SendData(USART2,buf[i]); 
 }
 while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//发送完成		
 delay_ms(2);  
}

#endif

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
#ifdef	USE_UART3
u8 g_u8UART3_Buff[UART3_LEN];

void UART3_Init(void)  
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//
	GPIO_Init(GPIOB, &GPIO_InitStructure);    //TX

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//
	GPIO_Init(GPIOB, &GPIO_InitStructure);    //RX	
	
 
	USART_InitStructure.USART_BaudRate = BAUD_UART3;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收和发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //使能串口中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //主优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //初始化NVIC
 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//接收中断
	USART_Cmd(USART3, ENABLE);                    //使能串口		

	g_sUART3.USARTx=USART3;
	g_sUART3.PortNum=3;
	g_sUART3.pBuff=g_u8UART3_Buff;//把数据指针指向接收缓冲区 
	g_sUART3.total_len=UART3_LEN;
}
/*		
================================================================================
描述 :串口中断函数
输入 : 
输出 : 
================================================================================
*/
void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		g_sUART3.pBuff[g_sUART3.iRecv]=USART_ReceiveData(USART3);		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		g_sUART3.iRecv++;
		if(g_sUART3.iRecv>=UART3_LEN)
			g_sUART3.iRecv=0;
	} 
	else if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART3);
//		USART_ClearITPendingBit(USART3, USART_IT_ORE);
		USART_ClearFlag(USART3, USART_FLAG_ORE);
	}
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void UART3_Send(u8 *buf, u16 len)
{
	u16 i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	//发送完成
		USART_SendData(USART3,buf[i]);
	}
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	//发送完成	
	delay_ms(2); 
}
#endif




/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void UART_Send(u8 PortNum, u8 *buf, u16 len)
{
	UART_Struct *pUART;
	u16 i;
 if(0)
 {
 }
#ifdef	USE_UART1 
 else  if(PortNum==1)//如果是串口1
 {
	 pUART=&g_sUART1;
 }
#endif
#ifdef	USE_UART2 
 else  if(PortNum==2)//如果是串口2
 {
   pUART=&g_sUART2;
 }
#endif
#ifdef	USE_UART3 
 else  if(PortNum==3)//如果是串口3
 {
   pUART=&g_sUART3;
 }
#endif 
#ifdef	USE_UART4 
 else  if(PortNum==4)//如果是串口4
 {
   pUART=&g_sUART4;
 }
#endif
 
#ifdef	USE_UART5 
 else  if(PortNum==5)//如果是串口5
 {
   pUART=&g_sUART5;
 }
#endif 
 else
 {
		return;
 }	
 for(i=0;i<len;i++)
 {
	 while(USART_GetFlagStatus(pUART->USARTx, USART_FLAG_TC) == RESET);	//发送完成
	 USART_SendData(pUART->USARTx,buf[i]);//pUART->USARTx
 }
 while(USART_GetFlagStatus(pUART->USARTx, USART_FLAG_TC) == RESET);	//发送完成
 delay_ms(2);
}



/*		
================================================================================
描述 : printf重定义
输入 : 
输出 : 
================================================================================
*/
int fputc(int ch, FILE *f)
{ 
#ifdef UART_DEBUG
   USART_SendData(UART_DEBUG, (u8) ch); 
   while(!(USART_GetFlagStatus(UART_DEBUG, USART_FLAG_TXE) == SET))
   {
   }
#endif
   return ch;
}






