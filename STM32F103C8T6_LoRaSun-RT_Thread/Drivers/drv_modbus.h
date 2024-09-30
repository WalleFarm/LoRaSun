
#ifndef __DRV_MODBUS_H__
#define __DRV_MODBUS_H__

#include "drv_common.h"   

u16 drv_modbus_send_fun01(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size);
u16 drv_modbus_send_fun02(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size);
u16 drv_modbus_send_fun03(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size);
u16 drv_modbus_send_fun04(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size);
u16 drv_modbus_send_fun05(u8 slave_addr, u16 reg_start, u16 reg_value, u8 *make_buff, u16 make_size);
u16 drv_modbus_send_fun06(u8 slave_addr, u16 reg_start, u16 reg_value, u8 *make_buff, u16 make_size);
u16 drv_modbus_send_fun16(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *reg_data, u8 *make_buff, u16 make_size);



u16 drv_modbus_parse_fun01(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size);
u16 drv_modbus_parse_fun02(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size);
u16 drv_modbus_parse_fun03(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size);
u16 drv_modbus_parse_fun04(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size);





#endif

