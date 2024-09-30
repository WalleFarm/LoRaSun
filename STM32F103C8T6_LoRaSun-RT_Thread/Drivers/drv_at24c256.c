

#include "drv_at24c256.h"


static I2cDriverStruct g_sAt24c256={0};

/*		
================================================================================
描述 :
输入 : 
输出 :  
================================================================================
*/
void at24c256_init(GPIO_TypeDef* port_sda, u16 pin_sda, GPIO_TypeDef* port_scl, u16 pin_scl)
{ 
	g_sAt24c256.port_sda=port_sda;
	g_sAt24c256.pin_sda=pin_sda;
	g_sAt24c256.port_scl=port_scl;
	g_sAt24c256.pin_scl=pin_scl;	
	
	IIC_GPIOInit(&g_sAt24c256);
//	log_at24c("AT24C1024_Init OK!\n");
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u8 at24c256_read_byte(u16 addr)
{
	const static u8 dev_addr=0xA0;
	u8 temp=0;
	
	IIC_Start(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, dev_addr );
	IIC_WaitAck(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, addr/256 );
	IIC_WaitAck(&g_sAt24c256);	
	IIC_WriteByte(&g_sAt24c256, addr%256 );
	IIC_WaitAck(&g_sAt24c256);
	
	IIC_Start(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, dev_addr|0x01 );
	IIC_WaitAck(&g_sAt24c256);
	temp=IIC_ReadByte(&g_sAt24c256);
	IIC_Stop(&g_sAt24c256);	
	
	return temp;
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void at24c256_write_byte(u16 addr, u8 data)
{
	const static u8 dev_addr=0xA0;
	
	IIC_Start(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, dev_addr);
	IIC_WaitAck(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, addr/256 );
	IIC_WaitAck(&g_sAt24c256);	
	IIC_WriteByte(&g_sAt24c256, addr%256 );
	IIC_WaitAck(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, data );
	IIC_WaitAck(&g_sAt24c256);	
	IIC_Stop(&g_sAt24c256);	
//	delay_ms(5);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u16 at24c256_read(u32 data_addr, u8 *out_buff, u16 len)
{
	const static u8 dev_addr=0xA0;
	u16 i=0;
	
	if(len==0 || data_addr+len>AT24C256_MAX_ADDR)
		return 0;
	
	IIC_Start(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, dev_addr );
	IIC_WaitAck(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, data_addr>>8 );
	IIC_WaitAck(&g_sAt24c256);	
	IIC_WriteByte(&g_sAt24c256, data_addr );
	IIC_WaitAck(&g_sAt24c256);
	
	IIC_Start(&g_sAt24c256);
	IIC_WriteByte(&g_sAt24c256, dev_addr|0x01 );
	IIC_WaitAck(&g_sAt24c256);
	for(i=0;i<len-1;i++)
	{
		out_buff[i]=IIC_ReadByte(&g_sAt24c256);
		IIC_Ack(&g_sAt24c256);		
	}		
	out_buff[i]=IIC_ReadByte(&g_sAt24c256);
	IIC_NAck(&g_sAt24c256);
	IIC_Stop(&g_sAt24c256);
	
	return len;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
bool at24c256_is_free(u32 data_addr)
{
	u8 read_buff[2]={0};
	at24c256_read(data_addr, read_buff, 2);
	if(read_buff[0]==0xFF && read_buff[1]==0xFF)
		return true;
	return false;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u16 at24c256_write(u32 data_addr, u8 *in_buff, u16 len)
{
	const static u8 dev_addr=0xA0;
	u8 *pData=in_buff;

	if(len==0 || data_addr+len>AT24C256_MAX_ADDR)
		return 0;
	
	while(len>0)
	{
		IIC_Start(&g_sAt24c256);
		IIC_WriteByte(&g_sAt24c256, dev_addr);
		IIC_WaitAck(&g_sAt24c256);
		IIC_WriteByte(&g_sAt24c256, data_addr>>8 );
		IIC_WaitAck(&g_sAt24c256);	
		IIC_WriteByte(&g_sAt24c256, data_addr );
		IIC_WaitAck(&g_sAt24c256);
		
		while(len>0)
		{
			IIC_WriteByte(&g_sAt24c256, *pData );
			IIC_WaitAck(&g_sAt24c256);
			len--;			
			pData++;
			data_addr++;
			if(data_addr%64==0)//写满一页,必须重新启动总线
			{
				break;
			}
		}

		IIC_Stop(&g_sAt24c256);		
		delay_ms(5);
	}
	
//	for(u16 i=0;i<len;i++)
//	{
//		at24c256_write_byte(data_addr+i, in_buff[i]); 
//	}	
	return len;
}

/*		
================================================================================
描述 : 擦除4字节
输入 : 
输出 : 
================================================================================
*/
void at24c256_earse(u32 earse_addr)
{
	u8 earse_buff[4]={0xFF, 0xFF, 0xFF, 0xFF};
//	for(u16 i=0;i<4;i++)
//	{
//		at24c256_write_byte(earse_addr+i, 0xFF); 
//	}	
	at24c256_write(earse_addr, earse_buff, 4);
}
































