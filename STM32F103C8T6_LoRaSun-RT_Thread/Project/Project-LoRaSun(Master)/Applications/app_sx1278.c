
#include "app_sx1278.h"  
 
 
#ifdef  LORA_SX1278 
DrvSx1278Struct g_sDrvSx1278={0};
#endif

#ifdef  LORA_SX1268 
DrvSx1268Struct g_sDrvSx1268={0};
#endif

/*		
================================================================================
描述 : led初始化
输入 : 
输出 : 
================================================================================
*/
void app_led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  
  //PB15--绿灯
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
}

/*		
================================================================================
描述 : 绿灯
输入 : 
输出 : 
================================================================================
*/
void app_led_set_green(bool state)
{
  if(state)//亮灯
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
  }
  else
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
  }  
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_uart_master_send(u8 *buff, u16 len)
{
  UART_Send(2, buff, len);
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_uart_recv_check(void)
{
	static UART_Struct *pUART=&g_sUART2; 
	u16 recv_len;

  if(pUART->iRecv>0)
  {
    recv_len=0;
    while(recv_len<pUART->iRecv)
    {
      recv_len=pUART->iRecv;
      delay_ms(5);
    }
    nwk_master_uart_parse(pUART->pBuff, pUART->iRecv);
    UART_Clear(pUART);
  }   
}

/*		
================================================================================
描述 : sx1278线程
输入 : 
输出 : 
================================================================================
*/
void app_sx1278_thread_entry(void *parameter) 
{
  static u32 run_cnts=0;
  static bool led_state=false;

  app_led_init();
  nwk_master_uart_send_register(0, 1, app_uart_master_send);
  nwk_master_uart_send_register(1, 2, app_uart_master_send);
  nwk_master_uart_send_register(2, 3, app_uart_master_send);
  nwk_master_uart_send_register(3, 4, app_uart_master_send);
  while(1)
  {
    app_uart_recv_check();//串口接收检查
//    nwk_master_main(); 
    
    if(run_cnts++%100==0)//指示灯运行
    {
      led_state=!led_state;
      app_led_set_green(led_state);
    } 

    delay_os(10);
  }
}



/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/













