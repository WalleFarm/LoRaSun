#include "hal_sx1278.h"
 
 
/*		
================================================================================
描述 :复位LORA设备
输入 : 
输出 : 
================================================================================
*/
void hal_sx1278_rst(HalSx1278Struct *psx1278) 
{
	psx1278->sx1278_reset();      
}


/*		
================================================================================
描述 : SPI字节读写
输入 : 
输出 : 
================================================================================
*/
u8 hal_sx1278_spi_rw_byte(HalSx1278Struct *psx1278, u8 byte)
{
	uint32_t dat;
	
	dat=psx1278->sx1278_spi_rw_byte(byte);
	return dat;  
}


/*		  
================================================================================
描述 : SPI写数据流
输入 : 
输出 : 
================================================================================
*/
void hal_sx1278_write_buffer(HalSx1278Struct *psx1278, u8 addr, u8 *buff, u8 size)
{
	u8 i;

	psx1278->sx1278_cs_0();//选中	  
	addr |= 0x80;
	hal_sx1278_spi_rw_byte(psx1278, addr);
	for(i=0;i<size;i++)
	{
		hal_sx1278_spi_rw_byte(psx1278, buff[i]);
	}		
	psx1278->sx1278_cs_1();//取消选择	
}

/*		
================================================================================
描述 : SPI读数据流
输入 : 
输出 : 
================================================================================
*/
void hal_sx1278_read_buffer(HalSx1278Struct *psx1278, u8 addr, u8 *buff, u8 size)
{
	u8 i;
	psx1278->sx1278_cs_0();//选中
	
	addr &= 0x7F;
	hal_sx1278_spi_rw_byte(psx1278, addr);
	for(i=0;i<size;i++)
	{
		buff[i]=hal_sx1278_spi_rw_byte(psx1278, 0);
	}	
	
	psx1278->sx1278_cs_1();//取消选择
}


/*		
================================================================================
描述 :往指定寄存器地址写入数据
输入 : 
输出 : 
================================================================================
*/
void hal_sx1278_write_addr(HalSx1278Struct *psx1278, u8 addr, u8 data)
{
	hal_sx1278_write_buffer(psx1278, addr, &data, 1);
}


/*		
================================================================================
描述 :读取指定寄存器地址的数据
输入 : 
输出 : 
================================================================================
*/
u8 hal_sx1278_read_addr(HalSx1278Struct *psx1278, u8 addr)
{
	u8 data=0;
	hal_sx1278_read_buffer(psx1278, addr, &data, 1);
	return data;  	
}

/*		
================================================================================
描述 :往LORA芯片FIFO写入数据
输入 : 
输出 : 
================================================================================
*/
void hal_sx1278_write_fifo(HalSx1278Struct *psx1278, u8 *buff, u8 size)
{
	hal_sx1278_write_buffer(psx1278, 0, buff, size);
}

/*		
================================================================================
描述 :读取LORA芯片FIFO的数据
输入 : 
输出 : 
================================================================================
*/
void hal_sx1278_read_fifo(HalSx1278Struct *psx1278, u8 *buff, u8 size)
{
	hal_sx1278_read_buffer(psx1278, 0, buff, size);
}


/*		
================================================================================
描述 : LED状态设置
输入 : 
输出 : 
================================================================================
*/
void hal_sx1278_set_led(HalSx1278Struct *psx1278, bool state)
{
  if(psx1278->set_led)
  {
    psx1278->set_led(state);
  }
}


































