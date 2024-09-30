
#ifndef __DRV_AT24C64_H
#define __DRV_AT24C64_H

#include "drv_iic.h"

#define			AT24C64_MAX_ADDR				(u32)(0x2000)  //最大数据地址

typedef struct
{
  u8 dev_addr;//器件地址
  I2cDriverStruct tag_iic;//IIC总线定义
  
}At24c64WorkStruct;

void at24c64_init(GPIO_TypeDef* port_sda, u16 pin_sda, GPIO_TypeDef* port_scl, u16 pin_scl);
void at24c64_set_dev_addr(u8 dev_addr);
u16 at24c64_read(u32 data_addr, u8 *out_buff, u16 len);
u16 at24c64_write(u32 data_addr, u8 *in_buff, u16 len);

#endif








