
#include "app_mqtt.h" 
#include "nwk_master.h" 

#define ESP8266_LINK_ID    3 //WIFI连接ID
#define MQTT_CONN_ID       0 //MQTT连接ID
/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_mqtt_init(void)
{
  //WIFI 参数配置
  drv_esp8266_init(&g_sUART3, "LoRaSun", "123456789");
//  drv_esp8266_init(&g_sUART3, "", "");
    
  drv_esp8266_set_client(ESP8266_LINK_ID, "broker.emqx.io", 1883, "TCP");
  
  drv_esp8266_fun_register(app_esp8266_recv);//注册ESP8266 接收处理函数(将数据放入MQTT缓冲区)
  
  //MQTT 参数配置
  u32 device_sn=nwk_master_get_gw_sn();
  static char *usr_name="usr_dev";
  static char *passwd="123456789";
  static char client_id01[20]={0};
  sprintf(client_id01,"C%08X_1", device_sn);//根据序列号创建MQTT客户端ID
//  static char *client_id02="c002";
  
  drv_mqtt_init(0, usr_name, passwd, client_id01);//参数初始化
  drv_mqtt_fun_transmit_register(0, app_mqtt_send0, app_mqtt_recv0);//收发函数注册  

//  drv_mqtt_init(1, usr_name, passwd, client_id02);//参数初始化
//  drv_mqtt_fun_transmit_register(1, app_mqtt_send1, app_mqtt_recv1);//收发函数注册  
  
  drv_mqtt_fun_parse_register(app_mqtt_recv_parse);//应用解析函数注册
  
  //话题订阅
  static char data_topic1[30]={0};//
  sprintf(data_topic1, "lora/dev/sub/%08X", device_sn);
//  static char *data_topic2="air/dev/sub/002";
  u32 base_msg_id=0xBB01;

  drv_mqtt_set_topic_info(0, 0, data_topic1, base_msg_id++, TopicStateSub); 


//  drv_mqtt_set_topic_info(1, 0, data_topic1, base_msg_id++, TopicStateSub);
//  drv_mqtt_set_topic_info(1, 1, data_topic2, base_msg_id++, TopicStateSub); 

}


/*		
================================================================================
描述 : 将收到的数据压入指定的MQTT 环形缓冲区内
输入 : 
输出 : 
================================================================================
*/
void app_mqtt_put_data(u8 index, u8 *buff, u16 len)
{
  struct rt_ringbuffer *rb=drv_mqtt_take_rb(index);//获取对应MQTT的缓冲区
  if(rb==NULL)
  {
    printf("index=%d, rb=null\n", index);
    return;
  }
  u16 size = rt_ringbuffer_space_len(rb); //剩余空间           
  if(len<size)
  { 
    
    rt_ringbuffer_put(rb, buff, len);//放入ringbuffer
  }  
  else
  {
    printf("data_len>ringbuffer size\n");
  }     
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
int app_mqtt_send0(u8 *buff, u16 len)
{
	drv_esp8266_send_data(ESP8266_LINK_ID, buff, len);//WIFI发送
  return len;
}
 
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/ 
int app_mqtt_recv0(u8 *buff, int len)
{
	struct rt_ringbuffer *rb=drv_mqtt_take_rb(MQTT_CONN_ID);
	if(rb==NULL)
	{
		return 0;
	}
	return rt_ringbuffer_get(rb, buff, len);  
}

/*		
================================================================================
描述 : ESP8266应用层接收处理
输入 : 
输出 : 
================================================================================
*/
void app_esp8266_recv(u8 sock_id, u8 *buff, u16 len)
{
  switch(sock_id)
  {
    case ESP8266_LINK_ID:
    {
      app_mqtt_put_data(MQTT_CONN_ID, buff, len);
      break;
    }  
  }
}

/*		
================================================================================
描述 : 应用层数据接收处理
输入 : 
输出 : 
================================================================================
*/
void app_mqtt_recv_parse(u8 index, char *topic, u8 *buff, u16 len)
{
  printf("mqtt conn id=%d, topic=%s\n", index, topic);
  printf("recv size=%d, buff=%s\n", len, buff);
  

}

/*		
================================================================================
描述 : 发送消息数据
输入 : 
输出 : 
================================================================================
*/
void app_mqtt_pub_data(u8 *buff, u16 len)
{
  static char *pub_topic="lora/dev/pub/data";
  drv_mqtt_publish(MQTT_CONN_ID, buff, len, pub_topic);
  
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void app_mqtt_main(void)
{
  drv_esp8266_main();
  drv_mqtt_main(); 
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/








