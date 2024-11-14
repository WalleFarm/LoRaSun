
#include "app_master.h" 
#include "app_mqtt.h" 
#include "drv_server.h"

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
描述 : lora数据接收解析
输入 : 
输出 : 
================================================================================
*/
void app_master_recv_from_parse(void)
{
  NwkMasterRecvFromStruct *pRecvFrom=nwk_master_recv_from_check();
  if(pRecvFrom)
  {
    u8 *pData=pRecvFrom->app_data;
    u8 cmd_type=pData[0];
    pData+=1;
    printf("cmd_type=%d, rssi=%ddbm, snr=%ddbm\n", cmd_type,
            pRecvFrom->rf_param.rssi, pRecvFrom->rf_param.snr);
    switch(cmd_type)//本地串口打印
    {
      case 0x01:
      {
        u16 temp_val=pData[0]<<8 | pData[1];
        pData+=2;
        u32 node_time=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
        pData+=4;
        printf("temp_val=%.1fC\n", (temp_val-1000)/10.f);
        printf("node_time=%us, gw time=%us\n", node_time, nwk_get_rtc_counter());
        break;
      }
    }
    //同时转发到MQTT
    static u8 up_buff[300]={0};
    u16 up_len=0;
    u16 rssi=pRecvFrom->rf_param.rssi+1000;
    u16 snr=pRecvFrom->rf_param.snr+100;
    u32 freq=pRecvFrom->rf_param.freq;
    u8 sf=pRecvFrom->rf_param.sf;
    u8 bw=pRecvFrom->rf_param.bw;
    u32 node_sn=pRecvFrom->src_sn;
    if(pRecvFrom->data_len+5>sizeof(up_buff))
    {
      printf("error pRecvFrom->data_len+5>sizeof(up_buff)\n");
      return;
    }
    up_buff[up_len++]=node_sn>>24;
    up_buff[up_len++]=node_sn>>16;
    up_buff[up_len++]=node_sn>>8;
    up_buff[up_len++]=node_sn;    
    up_buff[up_len++]=rssi>>8;
    up_buff[up_len++]=rssi;
    up_buff[up_len++]=snr>>8;
    up_buff[up_len++]=snr;
    up_buff[up_len++]=freq>>24;
    up_buff[up_len++]=freq>>16;
    up_buff[up_len++]=freq>>8;
    up_buff[up_len++]=freq;      
    up_buff[up_len++]=sf;      
    up_buff[up_len++]=bw;      
    up_buff[up_len++]=pRecvFrom->up_pack_num;//每次重发都会自增包序号,
                                             //跟应用层的pack id作对比就可以知道该数据包重发次数以及通讯质量      
    
    memcpy(&up_buff[up_len], pRecvFrom->app_data, pRecvFrom->data_len);
    up_len+=pRecvFrom->data_len;
    drv_server_send_msg(GW01_CMD_NODE_DATA, up_buff, up_len);//发送到服务器(M2M协议)
//    app_mqtt_pub_data(pRecvFrom->app_data, pRecvFrom->data_len);
  }
}

/*		
================================================================================
描述 : lora数据接收解析
输入 : 
输出 : 
================================================================================
*/
void app_master_event_parse(void)
{
  NwkMasterEventStruct *pEvent=nwk_master_event_check();
  if(pEvent)//有事件发生
  {
    switch(pEvent->event)//事件处理
    {
      case NwkMasterEventDownAck://下发回复
      case NwkMasterDownResultTimeOut://下发超时
      {
        u8 *pData=pEvent->params;
        u8 param_len=5;//4字节的sn+1字节的result
        drv_server_send_msg(GW01_CMD_NODE_DOWN, pData, param_len);//发送到服务器(M2M协议)
        break;
      }
    }
    pEvent->event=NwkMasterEventNone;
  }
}

/*		
================================================================================
描述 : 服务端数据解析
输入 : 
输出 : 
================================================================================
*/
u16 app_server_recv_parse(u8 cmd_type, u8 *buff, u16 len)
{
  u8 ack_buff[20]={0};
  u8 ack_len=0;
  u8 *pData=buff;
  switch(cmd_type)
  {
    case GW01_CMD_DATA://心跳包
    {
      printf("GW01_CMD_DATA ###\n");
      u32 server_time=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
      pData+=4;
      u32 now_time=drv_get_rtc_counter();
      int det_time=server_time-now_time;
      printf("server_time=%ds\n", server_time);
      if(abs(det_time)>0)
      {
        drv_set_rtc_counter(server_time);
        printf("update rtc time!\n");
      }
      break;
    }
    case GW01_CMD_NODE_LIST://获取节点列表
    {
      
      break;
    }
    case GW01_CMD_NODE_DOWN:
    {
      printf("GW01_CMD_NODE_DOWN ###\n");
      u32 dst_sn=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
      pData+=4;
      u8 data_len=pData[0];
      pData+=1;
      u8 result=nwk_master_add_down_pack(dst_sn, pData, data_len);//发送
      ack_buff[ack_len++]=dst_sn>>24;
      ack_buff[ack_len++]=dst_sn>>16;
      ack_buff[ack_len++]=dst_sn>>8;
      ack_buff[ack_len++]=dst_sn;    
      ack_buff[ack_len++]=result;     
      drv_server_send_msg(GW01_CMD_NODE_DOWN, ack_buff, ack_len);//发送到服务器(M2M协议)
      break;
    }    
  }
  return 0;  
}

/*		
================================================================================
描述 : 网关数据发送
输入 : 
输出 : 
================================================================================
*/
void app_master_send_status(void)
{
  u8 up_buff[50]={0};
  u16 up_len=0;
  u32 now_time=drv_get_rtc_counter();
  up_buff[up_len++]=now_time>>24;
  up_buff[up_len++]=now_time>>16;
  up_buff[up_len++]=now_time>>8;
  up_buff[up_len++]=now_time;
  drv_server_send_msg(GW01_CMD_DATA, up_buff, up_len);//发送到服务器(M2M协议)
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
  
  u8 freq_ptr=13;
  nwk_master_set_config(freq_ptr, NwkRunModeDynamic);//设置配置信息
  
  u8 root_key[17]={"0123456789ABCDEF"};//根密码, 跟节点保持一致
  nwk_master_set_root_key(root_key);
  for(u8 i=0; i<NWK_GW_WIRELESS_NUM; i++)
  {
    u8 slave=i+1;
    nwk_master_send_slave_config(slave); 
    delay_os(100);
  }
  app_mqtt_init();
//  nwk_master_add_token(0x12345678);//测试
  while(1)
  {
    app_master_uart_recv_check();//串口接收检查
    app_master_recv_from_parse();//应用层接收解析
    app_master_event_parse();//事件处理
    nwk_master_main(); //协议层管理
    app_mqtt_main();//MQTT
    if(run_cnts++%100==0)//指示灯运行
    {
      led_state=!led_state; 
      app_master_led_set(led_state);
    } 
    
    if(run_cnts%1000==0)
    {
      app_master_send_status();
    }

    delay_os(10);
  }
}
















