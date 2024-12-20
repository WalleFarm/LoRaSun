/******************************************************************************
*
* Copyright (c) 2024 小易
* 本项目开源文件遵循GPL-v3协议
* 
* 文章专栏地址:https://blog.csdn.net/ypp240124016/category_12834955
* github主页:      https://github.com/WalleFarm
* LoRaSun开源地址: https://github.com/WalleFarm/LoRaSun
* M2M-IOT开源地址: https://github.com/WalleFarm/M2M-IOT
* 协议栈原理专利:CN110572843A (一种基于LoRa无线模块CAD模式的嗅探方法及系统)
*
* 测试套件采购地址:https://duandianwulian.taobao.com/
*
* 作者:小易
* 博客主页:https://blog.csdn.net/ypp240124016?type=blog
* 交流QQ群:701889554  (资料文件存放)
* 微信公众号:端点物联 (即时接收教程更新通知)
*
* 所有学习资源合集:https://blog.csdn.net/ypp240124016/article/details/143068017
*
* 免责声明:本项目所有资料仅限于学习和交流使用,请勿商用.
*
********************************************************************************/

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
//	psx1268->wait_on_busy();
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
//	psx1268->wait_on_busy();
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
//	psx1268->wait_on_busy();
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
//	psx1268->wait_on_busy();
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
//	psx1268->wait_on_busy();
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
//	psx1268->wait_on_busy();
  
}

/*		
================================================================================
描述 : LED状态设置
输入 : 
输出 : 
================================================================================
*/
void hal_sx1268_set_led(HalSx1268Struct *psx1268, bool state)
{
  if(psx1268->set_led)
  {
    psx1268->set_led(state);
  }
}

































