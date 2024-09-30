
#ifndef __DRV_COMMON_H__
#define __DRV_COMMON_H__


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include <math.h>

#include <rthw.h>
#include <rtthread.h>

#include "user_opt.h"

#define		UID_ADDR							0x1FFFF7E8			

#define		TIME_STAMP_THRESH			(u32)1595224982  

#define 	EEPROM_START_ADDR    ((uint32_t)0x0800FC00)			//  模拟EEPROM的起始FLASH地址
#define		EEPROM_SIZE						1024		


void drv_common_init(void);
u16 drv_crc16(u8 *puchMsg,u16 usDataLen);
u8 drv_xor_cal(u8 *buff, u16 len);
u8 drv_check_sum(u8 *buff, u16 len);

void delay_os(u32 dlyms);
void delay_us(u32 dlyms);
void delay_ms(u32 dlyms);

u8 *memstr(u8 *full_str, u16 full_len, u8 *sub_str, u16 sub_len);
void printf_hex(char *str , u8 *hex, u16 len);
int printf_null(char const *const format, ...);

void drv_get_chip_uid(u8 *id_buff);


void EEPROM_Read(u32 start_addr, u8 *out_buff, u32 size);
void EEPROM_Write(u32 start_addr, u8 *buff, u32 size);

void drv_wdog_init(void);
void drv_wdog_feed(void);
void drv_system_reset(void);

u32 drv_get_sec_counter(void);
u32 drv_get_rtc_counter(void);
void drv_set_sec_counter(u32 time);
void drv_set_rtc_counter(u32 time);


void drv_rtc_init(void);
u16 drv_pow2(u8 n);
u32 drv_dec_string_to_num(char *str);

#endif

