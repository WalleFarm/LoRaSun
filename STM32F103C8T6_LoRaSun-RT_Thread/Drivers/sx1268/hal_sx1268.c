#include "hal_sx1268.h"
#include "sx1268_reg.h"
/*		
================================================================================
描述 :复位LORA设备
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_rst(HalSx1268Struct *psx1268) 
{
	psx1268->sx1268_reset();      
}


/*		
================================================================================
描述 : SPI字节读写
输入 : 
输出 : 
================================================================================
*/
u8 hal_sx1268_spi_rw_byte(HalSx1268Struct *psx1268, u8 byte)
{
	uint32_t dat;
	
	dat=psx1268->sx1268_spi_rw_byte(byte);
	return dat;  
}


/*		  
================================================================================
描述 : 写命令
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_write_cmd(HalSx1268Struct *psx1268, u8 cmd, u8 *buff, u8 size)
{
	u8 i;

	psx1268->sx1268_cs_0();//选中	  
	hal_sx1268_spi_rw_byte(psx1268, cmd);
	for(i=0;i<size;i++)
	{
		hal_sx1268_spi_rw_byte(psx1268, buff[i]);
	}		
	psx1268->sx1268_cs_1();//取消选择	
  psx1268->delay_ms(1);//这个延时是必须的,等待操作成功
}

/*		
================================================================================
描述 : 读命令
输入 : 
输出 : 
================================================================================
*/
u8 hal_sx1268_read_cmd(HalSx1268Struct *psx1268, u8 cmd, u8 *buff, u8 size)
{
	u8 i;
	psx1268->sx1268_cs_0();//选中
	hal_sx1268_spi_rw_byte(psx1268, cmd);
  u8 status=hal_sx1268_spi_rw_byte(psx1268, 0);
	for(i=0;i<size;i++)
	{
		buff[i]=hal_sx1268_spi_rw_byte(psx1268, 0);
	}	
	
	psx1268->sx1268_cs_1();//取消选择
  psx1268->delay_ms(1);
  return status;
}

/*		  
================================================================================
描述 : 写多个寄存器
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_write_regs(HalSx1268Struct *psx1268, u16 addr, u8 *buff, u8 size)
{
	u8 i;
	psx1268->sx1268_cs_0();//选中	  
	hal_sx1268_spi_rw_byte(psx1268, SX1268_WRITE_REGISTER);
	hal_sx1268_spi_rw_byte(psx1268, addr>>8);
	hal_sx1268_spi_rw_byte(psx1268, addr);
	for(i=0;i<size;i++)
	{
		hal_sx1268_spi_rw_byte(psx1268, buff[i]);
	}		
	psx1268->sx1268_cs_1();//取消选择	
  psx1268->delay_ms(1);
}

/*		
================================================================================
描述 : 读多个寄存器
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_read_regs(HalSx1268Struct *psx1268, u16 addr, u8 *buff, u8 size)
{
	u8 i;
	psx1268->sx1268_cs_0();//选中
	
	hal_sx1268_spi_rw_byte(psx1268, SX1268_READ_REGISTER);
	hal_sx1268_spi_rw_byte(psx1268, addr>>8);
	hal_sx1268_spi_rw_byte(psx1268, addr); 
  hal_sx1268_spi_rw_byte(psx1268, 0);
	for(i=0;i<size;i++)
	{
		buff[i]=hal_sx1268_spi_rw_byte(psx1268, 0);
	}	
	
	psx1268->sx1268_cs_1();//取消选择
  psx1268->delay_ms(1);
}




/*		
================================================================================
描述 :往指定寄存器地址写入数据
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_write_reg(HalSx1268Struct *psx1268, u16 addr, u8 data)
{
	hal_sx1268_write_regs(psx1268, addr, &data, 1);
}


/*		
================================================================================
描述 :读取指定寄存器地址的数据
输入 : 
输出 : 
================================================================================
*/
u8 hal_sx1268_read_reg(HalSx1268Struct *psx1268, u16 addr)
{
	u8 data=0;
	hal_sx1268_read_regs(psx1268, addr, &data, 1);
	return data;  	
}

/*		
================================================================================
描述 :往LORA芯片FIFO写入数据
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_write_fifo(HalSx1268Struct *psx1268, u8 *buff, u8 size)
{
	u8 i,offset=0;
	psx1268->sx1268_cs_0();//选中	  
	hal_sx1268_spi_rw_byte(psx1268, SX1268_WRITE_BUFFER);
	hal_sx1268_spi_rw_byte(psx1268, offset);
	for(i=0;i<size;i++)
	{
		hal_sx1268_spi_rw_byte(psx1268, buff[i]);
	}		
	psx1268->sx1268_cs_1();//取消选择	
  psx1268->delay_ms(1);
}

/*		
================================================================================
描述 :读取LORA芯片FIFO的数据
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_read_fifo(HalSx1268Struct *psx1268, u8 offset, u8 *buff, u8 size)
{
	psx1268->sx1268_cs_0();//选中	  
	hal_sx1268_spi_rw_byte(psx1268, SX1268_READ_BUFFER);
	hal_sx1268_spi_rw_byte(psx1268, offset);
	hal_sx1268_spi_rw_byte(psx1268, 0);
	for(u8 i=0;i<size;i++)
	{
		buff[i]=hal_sx1268_spi_rw_byte(psx1268, 0);
	}		
	psx1268->sx1268_cs_1();//取消选择	
  psx1268->delay_ms(1);
  
}




































