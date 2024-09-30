
#include "app_mqtt.h" 

#include "app_air.h" 

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_mqtt_init(void)
{
  //4G 参数配置
  drv_air780_init(&g_sUART2);
    
  drv_air780_set_client(3, "broker.emqx.io", 1883, "TCP");//EMQ官方测试服务器
  
  drv_air780_fun_register(app_esp8266_recv);//注册ESP8266 接收处理函数
  
  //MQTT 参数配置
  u32 device_sn=app_air_take_sn();
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
  sprintf(data_topic1, "air/dev/sub/%08X", device_sn);
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
	drv_air780_send_data(0, buff, len);
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
	struct rt_ringbuffer *rb=drv_mqtt_take_rb(0);
	if(rb==NULL)
	{
		return 0;
	}
	return rt_ringbuffer_get(rb, buff, len);  
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
int app_mqtt_send1(u8 *buff, u16 len)
{
	drv_air780_send_data(1, buff, len);
  return len;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/ 
int app_mqtt_recv1(u8 *buff, int len)
{
	struct rt_ringbuffer *rb=drv_mqtt_take_rb(1);
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
    case 3://AIR780 link_id=3
    {
      u8 conn_id=0;
      app_mqtt_put_data(conn_id, buff, len);
      break;
    }
//    case 4://ESP8266 link_id=4
//    {
//      u8 conn_id=1;
//      app_mqtt_put_data(conn_id, buff, len);
//      break;
//    }    
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
  
  app_air_recv_parse(buff, len);
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
  static char *pub_topic="air/dev/pub/data";
  drv_mqtt_publish(0, buff, len, pub_topic);
//  delay_os(200);
//  drv_mqtt_publish(1, buff, len, pub_topic);
  
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
  drv_air780_main();
  drv_mqtt_main();
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/








