
#include "app_master.h" 



/*		
================================================================================
描述 : led初始化
输入 : 
输出 : 
================================================================================
*/
void app_master_led_init(void)
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
描述 : 指示灯
输入 : 
输出 : 
================================================================================
*/
void app_master_led_set(bool state)
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
void app_master_uart_send(u8 *buff, u16 len)
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
void app_master_uart_recv_check(void)
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
描述 : master线程
输入 : 
输出 : 
================================================================================
*/
void app_master_thread_entry(void *parameter) 
{
  static u32 run_cnts=0;
  static bool led_state=false;

  app_master_led_init();
  u8 wireless=0, slave_addr=1;
  nwk_master_uart_send_register(wireless++, slave_addr++, app_master_uart_send);
  nwk_master_uart_send_register(wireless++, slave_addr++, app_master_uart_send);
  nwk_master_uart_send_register(wireless++, slave_addr++, app_master_uart_send);
  nwk_master_uart_send_register(wireless++, slave_addr++, app_master_uart_send);
  
  u8 freq_ptr=0;
  nwk_master_set_freq_ptr(freq_ptr);//设置基础频段
  for(u8 i=0; i<4; i++)
  {
    u8 slave=i+1;
    nwk_master_send_freq_ptr(slave);
    delay_os(100);
  }
  nwk_master_add_token(0x12345678);//测试
  while(1)
  {
    app_master_uart_recv_check();//串口接收检查
    nwk_master_main(); 
    
    if(run_cnts++%100==0)//指示灯运行
    {
      led_state=!led_state;
      app_master_led_set(led_state);
    } 

    delay_os(10);
  }
}
















