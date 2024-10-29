#ifndef __HAL_SX1268_H__
#define __HAL_SX1268_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
typedef struct
{
	void (*sx1268_reset)(void);//复位函数
	u8 (*sx1268_spi_rw_byte)(u8 byte);//字节读写函数
	void (*sx1268_cs_0)(void);//片选0
	void (*sx1268_cs_1)(void);//片选1
  void (*delay_ms)(u32 dlyms);//延时
	void (*wait_on_busy)(void);
}HalSx1268Struct;

 
void hal_sx1268_rst(HalSx1268Struct *psx1268); 
u8 hal_sx1268_spi_rw_byte(HalSx1268Struct *psx1268, u8 byte);

void hal_sx1268_write_cmd(HalSx1268Struct *psx1268, u8 cmd, u8 *buff, u8 size);
u8 hal_sx1268_read_cmd(HalSx1268Struct *psx1268, u8 cmd, u8 *buff, u8 size);
void hal_sx1268_write_regs(HalSx1268Struct *psx1268, u16 addr, u8 *buff, u8 size);
void hal_sx1268_read_regs(HalSx1268Struct *psx1268, u16 addr, u8 *buff, u8 size);

void hal_sx1268_write_reg(HalSx1268Struct *psx1268, u16 addr, u8 data);
u8 hal_sx1268_read_reg(HalSx1268Struct *psx1268, u16 addr);

void hal_sx1268_write_fifo(HalSx1268Struct *psx1268, u8 *buff, u8 size);
void hal_sx1268_read_fifo(HalSx1268Struct *psx1268, u8 offset, u8 *buff, u8 size);


#endif
