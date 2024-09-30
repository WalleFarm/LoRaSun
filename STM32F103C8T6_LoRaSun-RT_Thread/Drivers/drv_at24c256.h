
#ifndef __APP_AT24C256_H
#define __APP_AT24C256_H

#include "drv_iic.h"



#define			AT24C256_MAX_ADDR				(u32)(32768)


void at24c256_init(GPIO_TypeDef* port_sda, u16 pin_sda, GPIO_TypeDef* port_scl, u16 pin_scl);
u16 at24c256_read(u32 data_addr, u8 *out_buff, u16 len);
u16 at24c256_write(u32 data_addr, u8 *in_buff, u16 len);
void at24c256_earse(u32 earse_addr);
bool at24c256_is_free(u32 data_addr);













#endif








