
#ifndef __APP_AIR_H__
#define __APP_AIR_H__

#include "app_mqtt.h" 
#include "drv_sht30.h"
#include "drv_at24c64.h" 
#include "drv_bc260.h"

typedef enum
{
  AIR_CMD_HEART=0,//心跳包
  AIR_CMD_DATA,//数据包
  
  AIR_CMD_SET_SPEED=100,//设置风扇转速
  AIR_CMD_SET_SWITCH, //设置开关
}AirCmd;

typedef struct
{
  Sht30WorkStruct tag_sht30;
  u8 switch_state;//开关状态
  u8 fan_speed;//风速等级0~255
  u16 pm25_value;
  u32 device_sn;//设备识别码
}AirWorkStruct;

void app_air_thread_entry(void *parameter); 
u32 app_air_take_sn(void);

void app_sht30_init(void);
void app_motor_init(void);
void app_motor_set_speed(u8 speed);
void app_pm25_recv_check(void);
void app_air_send_status(void);
void app_air_recv_parse(u8 *buff, u16 len);


#endif
