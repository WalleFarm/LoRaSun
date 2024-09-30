

#ifndef __DRV_AS5600_H__
#define __DRV_AS5600_H__

#include "drv_iic.h"


#define		AS5600_ADDR		(0x36<<1)   //器件地址

typedef struct
{
  I2cDriverStruct tag_iic;
  u16 angle_value,last_angle;
  int32_t rounds;
  int32_t postion;
}As5600WorkStruct;



void drv_as5600_init(As5600WorkStruct *as5600, GPIO_TypeDef* port_sda, u32 pin_sda, GPIO_TypeDef* port_scl, u32 pin_scl);
u8 drv_as5600_read_byte(As5600WorkStruct *as5600, u8 dev_addr, u8 reg_addr);
u16 drv_as5600_read_angle(As5600WorkStruct *as5600);
u16 drv_as5600_read_conf(As5600WorkStruct *as5600);


#endif

