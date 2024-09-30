

#include "drv_at24c64.h"


At24c64WorkStruct g_sAt24c64Work={0};
/*		 
================================================================================
描述 :初始化
输入 : 
输出 :  
================================================================================
*/
void at24c64_init(GPIO_TypeDef* port_sda, u16 pin_sda, GPIO_TypeDef* port_scl, u16 pin_scl)
{ 
  I2cDriverStruct *pIIC=&g_sAt24c64Work.tag_iic;
	pIIC->port_sda=port_sda; 
	pIIC->pin_sda=pin_sda;
	pIIC->port_scl=port_scl;
	pIIC->pin_scl=pin_scl;	
	
  g_sAt24c64Work.dev_addr=0xA0;//默认器件地址
	IIC_GPIOInit(pIIC);

}

/*		
================================================================================
描述 : 设置器件地址
输入 : 
输出 : 
================================================================================
*/
void at24c64_set_dev_addr(u8 dev_addr)
{
  g_sAt24c64Work.dev_addr=dev_addr;
}

/*		
================================================================================
描述 : 读数据
输入 : 
输出 : 
================================================================================
*/
u16 at24c64_read(u32 data_addr, u8 *out_buff, u16 len)
{
	u8 dev_addr=g_sAt24c64Work.dev_addr;//器件地址
	u16 i=0;
	
	if(len==0 || data_addr+len>AT24C64_MAX_ADDR)//数据范围检测
		return 0;
  
	I2cDriverStruct *pIIC=&g_sAt24c64Work.tag_iic;
  
	IIC_Start(pIIC);
	IIC_WriteByte(pIIC, dev_addr );//写器件地址
	IIC_WaitAck(pIIC);
	IIC_WriteByte(pIIC, data_addr>>8 );
	IIC_WaitAck(pIIC);	
	IIC_WriteByte(pIIC, data_addr ); //写数据地址
	IIC_WaitAck(pIIC);
	
	IIC_Start(pIIC);
	IIC_WriteByte(pIIC, dev_addr|0x01 );//准备读
	IIC_WaitAck(pIIC);
	for(i=0;i<len-1;i++)//循环读取,少一个字节
	{
		out_buff[i]=IIC_ReadByte(pIIC);
		IIC_Ack(pIIC);//ACK
	}		
	out_buff[i]=IIC_ReadByte(pIIC);//读取最后一个字节
	IIC_NAck(pIIC);//NACK
	IIC_Stop(pIIC);
	
	return len;
}

/*		
================================================================================
描述 :写数据
输入 : 
输出 : 
================================================================================
*/
u16 at24c64_write(u32 data_addr, u8 *in_buff, u16 len)
{
	u8 dev_addr=g_sAt24c64Work.dev_addr;//器件地址
	u8 *pData=in_buff;

	if(len==0 || data_addr+len>AT24C64_MAX_ADDR)//数据范围检测
		return 0;
	
  I2cDriverStruct *pIIC=&g_sAt24c64Work.tag_iic;
  
	while(len>0)
	{
		IIC_Start(pIIC);
		IIC_WriteByte(pIIC, dev_addr);//写器件地址
		IIC_WaitAck(pIIC);
		IIC_WriteByte(pIIC, data_addr>>8 );
		IIC_WaitAck(pIIC);	
		IIC_WriteByte(pIIC, data_addr );//写数据地址
		IIC_WaitAck(pIIC);
		
    //继续写
		while(len>0)
		{
			IIC_WriteByte(pIIC, *pData );
			IIC_WaitAck(pIIC);
			len--;			
			pData++;
			data_addr++;
			if(data_addr%32==0)//写满一页,必须重新启动总线
			{
				break;
			}
		}
		IIC_Stop(pIIC);		
		delay_ms(5);//适当延时
	}
	return len;
}































