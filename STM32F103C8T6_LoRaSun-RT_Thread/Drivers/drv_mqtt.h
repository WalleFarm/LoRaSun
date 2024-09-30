
#ifndef __DRV_MQTT_H__
#define __DRV_MQTT_H__

#include "drv_uart.h"
#include "MQTTPacket.h" 
#include "ringbuffer.h"

#ifndef   MQTT_CONN_NUM
#define		MQTT_CONN_NUM				1   //MQTT连接数量
#endif

#ifndef   MQTT_SUB_NUM
#define		MQTT_SUB_NUM				3   //每个连接订阅数量
#endif

#ifndef   MQTT_KEEP_TIME
#define		MQTT_KEEP_TIME			60  //保活时间60S
#endif

#ifndef   MQTT_RING_BUFF_SIZE
#define		MQTT_RING_BUFF_SIZE				500  //缓刑缓冲区大小
#endif

#ifndef   MQTT_PUB_BUFF_SIZE
#define		MQTT_PUB_BUFF_SIZE				150  //每次发布消息最大长度
#endif

#ifndef   MQTT_SUB_BUFF_SIZE
#define		MQTT_SUB_BUFF_SIZE				150 //每次接收消息最大长度
#endif

typedef enum
{
  TopicStateIdel=0,//空闲,无效
  TopicStateSub, //订阅
  TopicStateUnSub,//取消订阅
}TopicState;//话题状态

typedef struct
{
	char sub_topic[40];
	u8 curr_state, dst_state;
  u32 base_msg_id;//用来标记订阅消息
}SubPackStruct; //订阅话题结构体


typedef struct
{
  bool is_connected;//连接状态
  bool is_enable;//使能状态
  u16 msg_id;//消息ID	
	u32 keep_time;  //保活时间
  char *usr_name;//用户名
  char *passwd;//密码
  char *client_id;//客户端ID
  MQTTPacket_connectData condata;//初始化信息
 	int (*mqtt_send)(u8 *buff, u16 len); //发送函数指针
	int (*mqtt_recv)(u8 *buff, int len); //接收函数指针
  struct rt_ringbuffer *rb; //ringbuffer 缓冲区指针
  SubPackStruct sub_list[MQTT_SUB_NUM];//订阅话题
}MqttClientStruct;//MQTT客户端结构体


typedef struct
{
  MqttClientStruct client_list[MQTT_CONN_NUM];//客户端列表
  void (*mqtt_recv_parse)(u8 index, char *topic, u8 *buff, u16 len);//应用层解析函数指针
}MqttWorkStruct;//MQTT总体工作结构体




struct rt_ringbuffer *drv_mqtt_take_rb(u8 index);

void drv_mqtt_fun_transmit_register(u8 index, int (*fun_send)(u8 *buff, u16 len), int (*fun_recv)(u8 *buff, int len));
void drv_mqtt_fun_parse_register(void (*recv_parse)(u8 index, char *topic, u8 *buff, u16 len));
void drv_mqtt_init(u8 index, char *usr_name, char *passwd, char *client_id);
void drv_mqtt_publish(u8 index, u8 *msg_buff, u16 msg_len, char *topic);
void drv_mqtt_connect(void);
void drv_mqtt_close(MqttClientStruct *pClient);
void drv_mqtt_set_topic_info(u8 client_id, u8 sub_id, char *topic, u32 base_msg_id, u8 dst_state);
void drv_mqtt_set_topic_state(u8 client_id, u8 sub_id, u8 dst_state);
void drv_mqtt_main(void);







#endif
