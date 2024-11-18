#include "drv_mqtt.h"
 
MqttWorkStruct g_sMqttWork={0};
/*		
================================================================================
描述 : 获取缓冲区地址
输入 : 
输出 : 
================================================================================
*/
struct rt_ringbuffer *drv_mqtt_take_rb(u8 index)
{
  struct rt_ringbuffer *rb=NULL;
  if(index<MQTT_CONN_NUM)
  {
    MqttClientStruct *pClient=&g_sMqttWork.client_list[index];
    rb=pClient->rb;
  }
  return rb;
}

/*		
================================================================================
描述 : 每个连接的收发函数注册
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_fun_transmit_register(u8 index, int (*fun_send)(u8 *buff, u16 len), int (*fun_recv)(u8 *buff, int len))
{
  if(index<MQTT_CONN_NUM)
  {
    MqttClientStruct *pClient=&g_sMqttWork.client_list[index];
    pClient->mqtt_send=fun_send;
    pClient->mqtt_recv=fun_recv;
  }
}

/*		
================================================================================
描述 : MQTT应用层数据接收处理函数注册
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_fun_parse_register(void (*recv_parse)(u8 index, char *topic, u8 *buff, u16 len))
{
  g_sMqttWork.mqtt_recv_parse=recv_parse;
}


/*		
================================================================================
描述 : 初始化指定MQTT连接
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_init(u8 index, char *usr_name, char *passwd, char *client_id)
{
  if(index<MQTT_CONN_NUM)
  {
    MqttClientStruct *pClient=&g_sMqttWork.client_list[index];
    MQTTPacket_connectData connect_init = MQTTPacket_connectData_initializer;
    
    if((pClient->rb=rt_ringbuffer_create(MQTT_RING_BUFF_SIZE))!=NULL )   
    {
      memcpy(&pClient->condata, &connect_init, sizeof(connect_init));//复制连接初始化信息
      pClient->condata.keepAliveInterval=MQTT_KEEP_TIME;			
      pClient->condata.username.cstring=usr_name;//用户名
      pClient->condata.password.cstring=passwd;//密码
      pClient->condata.clientID.cstring=client_id;//客户ID      
      pClient->is_enable=true;
    }      

  }    
}

/*		
================================================================================
描述 : MQTT发布数据
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_publish(u8 index, u8 *msg_buff, u16 msg_len, char *topic)
{
	static u8 make_buff[MQTT_PUB_BUFF_SIZE]={0};
	static const int make_size=sizeof(make_buff);	
	u16 make_len=0;	  
  if(index<MQTT_CONN_NUM)
  {
    MqttClientStruct *pClient=&g_sMqttWork.client_list[index];
		if(pClient->is_connected==true)//已经连接
		{ 
			pClient->msg_id++;
			MQTTString topicString = MQTTString_initializer;
			topicString.cstring=topic; 
			make_len = MQTTSerialize_publish(make_buff, make_size, 0, 1,0, pClient->msg_id, topicString, msg_buff, msg_len);//组合发布报文
			if(pClient->mqtt_send!=NULL && make_len>0)
			{
				int ret=pClient->mqtt_send(make_buff, make_len);//发送
			}			
		}		    
  }
}

/*		
================================================================================
描述 : 连接和订阅
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_connect(void)
{
	static u32 last_sec_time=0;
	static u8 make_buff[200]={0};
	const int make_size=sizeof(make_buff);
	int make_len;	
	u32 now_sec_time=drv_get_sec_counter();
	
	if(now_sec_time-last_sec_time>=2)
  {
    static u8 conn_ptr=0;
    if(conn_ptr>=MQTT_CONN_NUM)
      conn_ptr=0;
    MqttClientStruct *pClient=&g_sMqttWork.client_list[conn_ptr];
    if(pClient->is_enable)
    {
      if(pClient->is_connected==false)
      {
        memset(make_buff, 0, make_size);
        make_len=MQTTSerialize_connect(make_buff, make_size, &pClient->condata);//组合连接请求包		
        if(pClient->mqtt_send!=NULL)
        {
//          printf("client=%d, mqtt send connect!  make_len=%d\n",conn_ptr, make_len); 
          pClient->mqtt_send(make_buff, make_len);//发送
        }		          
      }
      else
      {
        //订阅话题
        for(u8 i=0; i<MQTT_SUB_NUM; i++)
        {
          SubPackStruct *pSub=&pClient->sub_list[i];
          if(strlen(pSub->sub_topic)>0 && pSub->curr_state!=pSub->dst_state)
          {
            if(pSub->dst_state==TopicStateSub)//需要订阅
            {
              MQTTString topicString = MQTTString_initializer;
              int req_qos=1;					
              topicString.cstring=pSub->sub_topic;
              memset(make_buff, 0, make_size);
              make_len = MQTTSerialize_subscribe(make_buff, make_size, 0, pSub->base_msg_id, 1, &topicString, &req_qos);//组合订阅报文
              if(pClient->mqtt_send!=NULL)
              {
                printf("sub topic=%s\n", pSub->sub_topic);
                pClient->mqtt_send(make_buff, make_len);//发送
              }		              
            }
            else if(pSub->dst_state==TopicStateUnSub)//需要取消订阅
            {
              MQTTString topicString = MQTTString_initializer;			
              topicString.cstring=pSub->sub_topic;
              memset(make_buff, 0, make_size);
              make_len = MQTTSerialize_unsubscribe(make_buff, make_size, 0, pSub->base_msg_id, 1, &topicString);//组合取消订阅报文
              if(pClient->mqtt_send!=NULL)
              {
                printf("unsub topic=%s\n", pSub->sub_topic);
                pClient->mqtt_send(make_buff, make_len);//发送
              }		              
            }
	 
            break;//每次只订阅一个,避免堵塞
          }
        }
        
        //超时检测
        u32 det_time=now_sec_time-pClient->keep_time;
        if(det_time>=MQTT_KEEP_TIME)
        {
          printf("mqtt sock_id=%d timeout, close!\n", conn_ptr);
          drv_mqtt_close(pClient);//超时关闭	          
        }
        else if(det_time>=MQTT_KEEP_TIME-10)
        {
          //发送ping请求,保活
          memset(make_buff, 0, make_size);
          make_len=MQTTSerialize_pingreq(make_buff, make_size);//组合ping包		
          if(pClient->mqtt_send!=NULL)
          {
//            printf("sock=%d, mqtt send ping req! make_len=%d\n",conn_ptr,make_len); 
            pClient->mqtt_send(make_buff, make_len);//发送
          }		          
        }          
      }
    }
    conn_ptr++;
    last_sec_time=drv_get_sec_counter();
  }  
}  

/*		
================================================================================
描述 : 关闭连接
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_close(MqttClientStruct *pClient)
{
	pClient->is_connected=false;

  for(u8 i=0; i<MQTT_SUB_NUM; i++)
  {
    SubPackStruct *pSub=&pClient->sub_list[i];
    pSub->curr_state=TopicStateIdel;
//    pSub->subed_time=0;
  }
	pClient->msg_id=0;
	pClient->keep_time=0;
}


/*		
================================================================================
描述 : 接收检查
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_recv_check(void)
{
	static u8 make_buff[MQTT_SUB_BUFF_SIZE];
	const int make_size=sizeof(make_buff);
	int rc;

	u8 dup;
	int qos;
	u8 retained;
	u16 msgid;
	int payloadlen_in;
	u8 *payload_in;	
	MQTTString receivedTopic;	

	for(u8 i=0; i<MQTT_CONN_NUM; i++)
	{
		MqttClientStruct *pClient=&g_sMqttWork.client_list[i];
		if(pClient->is_enable==true)//启用
		{
			rc=MQTTPacket_read(make_buff, make_size, pClient->mqtt_recv);
			switch(rc)
			{
				case CONNACK://连接回复
				{
					printf("mqtt_id=%d CONNACK!\n", i);
					u8 sessionPresent, connack_rc;
					if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, make_buff, make_size) != 1 || connack_rc != 0)//解析收到的回复报文
					{
						drv_mqtt_close(pClient);
						printf("mqtt sock_id=%d  Unable to connect, return code %d\n",i, connack_rc); 
					}
					else
					{
						pClient->is_connected=true;
						pClient->keep_time=drv_get_sec_counter();//更新时间
						printf("mqtt sock_id=%d connect ok!\n", i);
					}						
					break;
				}		
				case PUBREC:
				case PUBACK: //发布回复
				{
//					debug("sock_id=%d PUBACK!\n", i);
					break;
				}			
				case PUBLISH://收到发布的消息
				{
					pClient->keep_time=drv_get_sec_counter();//更新时间

					printf("sock_id=%d PUBLISH!\n", i);
					rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_in, &payloadlen_in, make_buff, make_size);	
					char *pTopic=receivedTopic.lenstring.data;
          if(g_sMqttWork.mqtt_recv_parse!=NULL)
          {
            char topic[50]={0};
            int len=(char*)payload_in-pTopic;//topic 长度
            if(len>sizeof(topic))
            {
              len=sizeof(topic)-1;
            }
            memcpy(topic, pTopic, len);
            g_sMqttWork.mqtt_recv_parse(i, topic, payload_in, payloadlen_in);//应用层数据解析
          }
					break;
				}	
				case SUBACK://订阅回复
				{
//					debug("sock_id=%d SUBACK!\n", i);
//					printf_hex("sub buff=", make_buff, 30);
					int count, requestedQoSs[1];
					MQTTDeserialize_suback(&msgid, 1, &count, requestedQoSs, make_buff, make_size);
//					printf("$$$ sub msgid=0x%04X\n", msgid);
          for(u8 k=0; k<MQTT_SUB_NUM; k++)
          {
            SubPackStruct *pSub=&pClient->sub_list[k];
            if(pSub->base_msg_id==msgid)
            {
              printf("topic=%s sub ok!\n", pSub->sub_topic);
              pSub->curr_state=TopicStateSub;
//              pSub->subed_time=drv_get_sec_counter();
            }
          }
					break;
				}	              
        case UNSUBACK://取消订阅回复
        {
//					debug("sock_id=%d UNSUBACK!\n", i);
          MQTTDeserialize_unsuback(&msgid, make_buff, make_size);
//					debug("$$$ msgid=0x%04X\n", msgid); 
          for(u8 k=0; k<MQTT_SUB_NUM; k++)
          {
            SubPackStruct *pSub=&pClient->sub_list[k];
            if(pSub->base_msg_id==msgid)
            {
              printf("topic=%s unsub ok!\n", pSub->sub_topic);
              pSub->curr_state=TopicStateUnSub;
//              pSub->subed_time=drv_get_sec_counter();
            }
          }          
          break;
        }
				case PINGRESP://ping回复
				{
					pClient->keep_time=drv_get_sec_counter();//更新时间
//					debug("sock_id=%d PINGRESP!\n", i);
					break;
				}	
				case DISCONNECT://断开连接
				{
					printf("mqtt_id=%d DISCONNECT!\n", i);
					drv_mqtt_close(pClient);					
					break;
				}					
			}
		}	
	}
}

/*		
================================================================================
描述 : 设置话题信息
输入 : 
输出 :  
================================================================================
*/
void drv_mqtt_set_topic_info(u8 client_id, u8 sub_id, char *topic, u32 base_msg_id, u8 dst_state)
{
  if(client_id<MQTT_CONN_NUM) 
  { 
    MqttClientStruct *pClient=&g_sMqttWork.client_list[client_id];
    if(sub_id<MQTT_SUB_NUM)
    {
      SubPackStruct *pSub=&pClient->sub_list[sub_id];
      if(strlen(topic)<sizeof(pSub->sub_topic))
      {
        pSub->curr_state=TopicStateIdel;  
        pSub->dst_state=dst_state;
        pSub->base_msg_id=base_msg_id;
        strcpy(pSub->sub_topic, topic);        
      }
    }
  }
}

/*		
================================================================================
描述 : 设置话题订阅状态
输入 : 
输出 :  
================================================================================
*/
void drv_mqtt_set_topic_state(u8 client_id, u8 sub_id, u8 dst_state)
{
  if(client_id<MQTT_CONN_NUM) 
  { 
    MqttClientStruct *pClient=&g_sMqttWork.client_list[client_id];
    if(sub_id<MQTT_SUB_NUM)
    {
      SubPackStruct *pSub=&pClient->sub_list[sub_id];
      pSub->dst_state=dst_state;
    }
  }  
}

/*		
================================================================================
描述 : mqtt主循环
输入 : 
输出 : 
================================================================================
*/
void drv_mqtt_main(void)
{
	drv_mqtt_connect();
	drv_mqtt_recv_check();
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
















