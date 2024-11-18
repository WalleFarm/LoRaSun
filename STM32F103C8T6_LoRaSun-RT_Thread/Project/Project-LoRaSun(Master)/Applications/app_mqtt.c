
#include "app_mqtt.h" 
#include "app_master.h"  

#define ESP8266_M2M_LINK_ID    3 //WIFI连接ID
#define MQTT_M2M_CONN_ID       0 //MQTT连接ID

#define ESP8266_AEP_LINK_ID    4 //WIFI连接ID
#define MQTT_AEP_CONN_ID       1 //MQTT连接ID
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
//  drv_esp8266_init(&g_sUART3, "YPP001", "12345678");
//  drv_esp8266_init(&g_sUART3, "CMCC-2.4G", "YHS11223344%");
//  drv_esp8266_init(&g_sUART3, "LoRaSun", "123456789");
  drv_esp8266_init(&g_sUART3, "", "");//密码第一次设置后会保存,之后用这个可以避免重复设置,保护flash并节省时间
    
  drv_esp8266_set_client(ESP8266_M2M_LINK_ID, "broker.emqx.io", 1883, "TCP");
  drv_esp8266_set_client(ESP8266_AEP_LINK_ID, "10414623.non-nb.ctwing.cn", 1883, "TCP");//根据自己的AEP产品信息修改目标地址,同一类型一般是一样的
  
  drv_esp8266_fun_register(app_esp8266_recv);//注册ESP8266 接收处理函数(将数据放入MQTT缓冲区)
  
  //MQTT 参数配置

  
  if(1)//M2M协议MQTT平台
  {
    drv_server_init();  
    u32 app_id=drv_server_get_app_id();
    u32 gw_sn=drv_server_get_gw_sn();  
    nwk_master_set_gw_sn(gw_sn);    
    
    static char *usr_name="usr_dev";
    static char *passwd="123456789";
    static char client_id[20]={0};
    sprintf(client_id,"C%08X_1", gw_sn);//根据序列号创建MQTT客户端ID
    
    drv_mqtt_init(MQTT_M2M_CONN_ID, usr_name, passwd, client_id);//参数初始化
    drv_mqtt_fun_transmit_register(MQTT_M2M_CONN_ID, app_mqtt_send0, app_mqtt_recv0);//收发函数注册  
   
    //话题订阅
    static char sub_topic[30]={0};//
    sprintf(sub_topic, "%sdev/sub/data/%u/%08X", TOPIC_HEAD, app_id, gw_sn);
    u32 base_msg_id=0xBB01;
    drv_mqtt_set_topic_info(MQTT_M2M_CONN_ID, 0, sub_topic, base_msg_id, TopicStateSub); 

    drv_server_send_register(app_mqtt_pub_data_m2m);//服务端发送函数注册 
    drv_server_cmd_parse_register(app_server_recv_parse);//服务端应用层命令解析函数注册       
  }
  
  if(1)//AEP MQTT平台
  {
    static char *usr_name="LoRaSun";//这个可以不改,理论上是自己的平台登录用户名
    static char *passwd="h5eDXbZAxZPUkUIdlLJOb0ZcrSlbFxP7A9rm8ipmLeM";//特征串
    static char *client_id="17166408C1011234";    //设备ID    以上AEP信息根据自己设备信息更改

    drv_mqtt_init(MQTT_AEP_CONN_ID, usr_name, passwd, client_id);//参数初始化
    drv_mqtt_fun_transmit_register(MQTT_AEP_CONN_ID, app_mqtt_send1, app_mqtt_recv1);//收发函数注册    

    //话题订阅
    static char sub_topic[30]={"node_down"};//
    u32 base_msg_id=0xBB02;
    drv_mqtt_set_topic_info(MQTT_AEP_CONN_ID, 0, sub_topic, base_msg_id, TopicStateSub);     
  }


  drv_mqtt_fun_parse_register(app_mqtt_recv_parse);//MQTT应用解析函数注册

  //QT C++模型 ModelGw01里的密码,实际使用可以做个混淆
  u8 passwd_table[5][16]={
    0x9D, 0x53, 0x09, 0xBF, 0x75, 0x28, 0xDE, 0x94, 0x4A, 0xFD, 0xB3, 0x69, 0x1F, 0xD2, 0x88, 0x3E, 
    0xF4, 0xAA, 0x5D, 0x13, 0xC9, 0x7F, 0x31, 0xE7, 0x9D, 0x53, 0x06, 0xBC, 0x72, 0x28, 0xDB, 0x91,
    0x47, 0xFD, 0xB3, 0x66, 0x1C, 0xD2, 0x88, 0x3B, 0xF1, 0xB5, 0x75, 0x39, 0xFA, 0xBE, 0x7E, 0x42,
    0x03, 0xC7, 0x88, 0x4B, 0x0C, 0xD0, 0x91, 0x54, 0x15, 0xD9, 0x9A, 0x5E, 0x21, 0xE2, 0xA6, 0x67, 
    0x2A, 0xEB, 0xAF, 0x70, 0x34, 0xF4, 0xB8, 0x79, 0x3D, 0xFD, 0xC1, 0x82, 0x46, 0x06, 0xCA, 0x8B, 
  };   
  for(u8 i=0; i<5; i++)
  {
    drv_server_add_passwd(i, passwd_table[i]);//添加密码
  }
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
	drv_esp8266_send_data(ESP8266_M2M_LINK_ID, buff, len);//WIFI发送
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
	struct rt_ringbuffer *rb=drv_mqtt_take_rb(MQTT_M2M_CONN_ID);
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
	drv_esp8266_send_data(ESP8266_AEP_LINK_ID, buff, len);//WIFI发送
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
	struct rt_ringbuffer *rb=drv_mqtt_take_rb(MQTT_AEP_CONN_ID);
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
    case ESP8266_M2M_LINK_ID:
    {
      app_mqtt_put_data(MQTT_M2M_CONN_ID, buff, len);
      break;
    }  
    case ESP8266_AEP_LINK_ID:
    {
      app_mqtt_put_data(MQTT_AEP_CONN_ID, buff, len);
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
  printf_hex("recv=", buff, len);
  switch(index)
  {
    case MQTT_M2M_CONN_ID:
    {
      drv_server_recv_parse(buff, len, NULL);//进入M2M协议解析
      break;
    }
    case MQTT_AEP_CONN_ID:
    {
      printf("MQTT_AEP_CONN_ID ####\n");     //进入AEP协议解析
      app_aep_recv_parse(topic, buff, len);
      break;
    }    
  }

  
}

/*		
================================================================================
描述 : 发送M2M消息数据
输入 : 
输出 : 
================================================================================
*/
int app_mqtt_pub_data_m2m(u8 *buff, u16 len)
{
  u32 app_id=drv_server_get_app_id();
  char pub_topic[30]={0};
  sprintf(pub_topic, "%sdev/pub/data/%u", TOPIC_HEAD, app_id);
  drv_mqtt_publish(MQTT_M2M_CONN_ID, buff, len, pub_topic);
  return len;
}

/*		
================================================================================
描述 : 发送AEP消息数据
输入 : 
输出 : 
================================================================================
*/
int app_mqtt_pub_data_aep(char *topic, u8 *buff, u16 len)
{
  drv_mqtt_publish(MQTT_AEP_CONN_ID, buff, len, topic);
  return len;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void app_mqtt_thread_entry(void *parameter) 
{
  delay_os(500);
  app_mqtt_init();
  while(1)
  {
    drv_esp8266_main();
    drv_mqtt_main();     
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








