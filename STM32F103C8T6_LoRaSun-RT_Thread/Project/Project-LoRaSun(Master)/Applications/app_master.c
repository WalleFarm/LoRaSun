
#include "app_master.h" 

#include "app_mqtt.h" 


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


void app_master_recv_parse(void)
{
  NwkMasterRecvFromStruct *pRecvFrom=nwk_master_recv_from_check();
  if(pRecvFrom)
  {
    u8 *pData=pRecvFrom->app_data;
    u8 cmd_type=pData[0];
    pData+=1;
    printf("cmd_type=%d, rssi=%ddbm, snr=%ddbm\n", cmd_type,
            pRecvFrom->rf_param.rssi, pRecvFrom->rf_param.snr);
    switch(cmd_type)
    {
      case 0x01:
      {
        u16 temp_val=pData[0]<<8|pData[1];
        pData+=2;
        u32 node_time=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
        pData+=4;
        printf("temp_val=%.1fC\n", (temp_val-1000)/10.f);
        printf("node_time=%us, gw time=%us\n", node_time, nwk_get_rtc_counter());
        break;
      }
    }
    //同时转发到MQTT
    app_mqtt_pub_data(pRecvFrom->app_data, pRecvFrom->data_len);
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
  nwk_master_uart_send_register(wireless++, slave_addr++, app_master_uart_send);//添加从机模块
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
  app_mqtt_init();
//  nwk_master_add_token(0x12345678);//测试
  while(1)
  {
    app_master_uart_recv_check();//串口接收检查
    app_master_recv_parse();//应用层接收解析
    nwk_master_main(); 
    app_mqtt_main();//MQTT
    if(run_cnts++%100==0)//指示灯运行
    {
      led_state=!led_state;
      app_master_led_set(led_state);
    } 

    delay_os(10);
  }
}
















