
#ifndef __APP_MQTT_H__
#define __APP_MQTT_H__


#include "drv_air780e.h" 
#include "drv_mqtt.h"

void app_mqtt_init(void);
void app_mqtt_put_data(u8 index, u8 *buff, u16 len);
int app_mqtt_send0(u8 *buff, u16 len);
int app_mqtt_recv0(u8 *buff, int len);

int app_mqtt_send1(u8 *buff, u16 len);
int app_mqtt_recv1(u8 *buff, int len);

void app_esp8266_recv(u8 sock_id, u8 *buff, u16 len);
void app_mqtt_recv_parse(u8 index, char *topic, u8 *buff, u16 len);


void app_mqtt_main(void);

void app_mqtt_pub_data(u8 *buff, u16 len); 











#endif

