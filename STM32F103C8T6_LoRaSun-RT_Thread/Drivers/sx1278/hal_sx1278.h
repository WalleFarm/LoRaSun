#ifndef __HAL_SX1278B_H__
#define __HAL_SX1278B_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
typedef struct
{
	void (*sx1278_reset)(void);//复位函数
	u8 (*sx1278_spi_rw_byte)(u8 byte);//字节读写函数
	void (*sx1278_cs_0)(void);//片选0
	void (*sx1278_cs_1)(void);//片选1
}HalSx1278Struct;

 
void hal_sx1278_rst(HalSx1278Struct *psx1278); 
u8 hal_sx1278_spi_rw_byte(HalSx1278Struct *psx1278, u8 byte);
void hal_sx1278_write_buffer(HalSx1278Struct *psx1278, u8 addr, u8 *buff, u8 size);
void hal_sx1278_read_buffer(HalSx1278Struct *psx1278, u8 addr, u8 *buff, u8 size);

void hal_sx1278_write_addr(HalSx1278Struct *psx1278, u8 addr, u8 data);
u8 hal_sx1278_read_addr(HalSx1278Struct *psx1278, u8 addr);
void hal_sx1278_write_fifo(HalSx1278Struct *psx1278, u8 *buff, u8 size);
void hal_sx1278_read_fifo(HalSx1278Struct *psx1278, u8 *buff, u8 size);




#endif
