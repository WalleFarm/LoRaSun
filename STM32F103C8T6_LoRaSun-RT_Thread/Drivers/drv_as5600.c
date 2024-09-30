
#include "drv_as5600.h"



/*		
================================================================================
描述 : 初始化
输入 : 
输出 : 
================================================================================
*/ 
void drv_as5600_init(As5600WorkStruct *as5600, GPIO_TypeDef* port_sda, u32 pin_sda, GPIO_TypeDef* port_scl, u32 pin_scl)
{
  I2cDriverStruct *pIic=&as5600->tag_iic;
  pIic->port_sda=port_sda;
  pIic->pin_sda=pin_sda;
  pIic->port_scl=port_scl;
  pIic->pin_scl=pin_scl;
  
  IIC_GPIOInit(pIic);  
}


/*		
================================================================================
描述 : 字节读取
输入 : 
输出 : 
================================================================================
*/ 
u8 drv_as5600_read_byte(As5600WorkStruct *as5600, u8 dev_addr, u8 reg_addr)
{
	I2cDriverStruct *pIic=&as5600->tag_iic;
  
	IIC_Start(pIic);
	IIC_WriteByte(pIic, dev_addr);
	IIC_WaitAck(pIic);
	IIC_WriteByte(pIic, reg_addr );
	IIC_WaitAck(pIic);  
  
	IIC_Start(pIic);
	IIC_WriteByte(pIic, dev_addr|0x01); 
  IIC_WaitAck(pIic);  
	u8 temp=IIC_ReadByte(pIic);
	IIC_NAck(pIic);
	IIC_Stop(pIic);
  return temp;  
}

/*		
================================================================================
描述 : 角度读取
输入 : 
输出 : 
================================================================================
*/ 
u16 drv_as5600_read_angle(As5600WorkStruct *as5600)
{
  u16 value=0;
  u8 dev_addr=AS5600_ADDR;
  value=drv_as5600_read_byte(as5600, dev_addr, 0x0E);
  value<<=8;
  value|=drv_as5600_read_byte(as5600, dev_addr, 0x0F);
//  debug("as5600 value=%d\n", value);
  as5600->angle_value=value;
  if(value<1000 && as5600->last_angle>3000)
  {
    as5600->rounds++;
  }
  else if(value>3000 && as5600->last_angle<1000)
  {
    as5600->rounds--;
  }
  as5600->postion=as5600->rounds*4096+value;
  as5600->last_angle=value;
  return value;
}

/*		
================================================================================
描述 : 配置参数读取
输入 : 
输出 : 
================================================================================
*/ 
u16 drv_as5600_read_conf(As5600WorkStruct *as5600)
{
  u16 value=0;
  u8 dev_addr=AS5600_ADDR;
  value=drv_as5600_read_byte(as5600, dev_addr, 0x07);
  value<<=8;
  value|=drv_as5600_read_byte(as5600, dev_addr, 0x08);
  printf("as5600 conf=0x%04X\n", value);
  return value;
}








