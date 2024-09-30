
#include "drv_sht30.h"



/*		
================================================================================
描述 : 器件引脚初始化
输入 : 
输出 : 
================================================================================
*/
void drv_sht30_init(Sht30WorkStruct *pSht30Work)
{
	IIC_GPIOInit(&pSht30Work->tag_iic);
	pSht30Work->dev_addr=0x44;//默认器件地址
}
	
/*		
================================================================================
描述 : 更新器件地址,硬件上ADDR引脚上拉时需要设置成0x45
输入 : 
输出 : 
================================================================================
*/
void drv_sht30_set_addr(Sht30WorkStruct *pSht30Work, u8 dev_addr)
{
	pSht30Work->dev_addr=dev_addr;
}


/*		
================================================================================
描述 : 读取温湿度数据
输入 : 
输出 : 
================================================================================
*/
void drv_sht30_read_th(Sht30WorkStruct *pSht30Work)
{
	u16 reg_addr=0x2C06;//温湿度的寄存器地址,由数据手册得来
	u8 dev_addr=pSht30Work->dev_addr;
	I2cDriverStruct *pIIC=&pSht30Work->tag_iic;
	
	IIC_Start(pIIC);
	IIC_WriteByte(pIIC, dev_addr<<1|0x00);//准备写入寄存器地址
	IIC_WaitAck(pIIC);
	
	IIC_WriteByte(pIIC, reg_addr>>8);//写入寄存器地址高8位
	IIC_WaitAck(pIIC);	
	IIC_WriteByte(pIIC, reg_addr&0xFF);//写入寄存器地址低8位
	IIC_WaitAck(pIIC);		
	IIC_Stop(pIIC);
	delay_ms(20);//这个延时要稍微长点20ms以上
	
	IIC_Start(pIIC);
	IIC_WriteByte(pIIC, dev_addr<<1|0x01);//准备读取数据
	IIC_WaitAck(pIIC);	
	
	u8 buff[10]={0};
	for(u8 i=0; i<6; i++)//读取温湿度和校验值状态
	{
		buff[i]=IIC_ReadByte(pIIC);
		if(i<5)IIC_Ack(pIIC);
		else IIC_NAck(pIIC);
	}
	IIC_Stop(pIIC);
	
	u16 temp=buff[0]<<8|buff[1];//温度寄存器值
	u16 humi=buff[3]<<8|buff[4];//湿度寄存器值
	
	pSht30Work->temp_value=175.f*(float)temp/65535.f-45.f ;//转换成温度-℃
	pSht30Work->humi_value=100.f*(float)humi/65535.f;//转换为湿度-%
	
	printf("temp=%.1f C, humi=%.1f%%\n", pSht30Work->temp_value, pSht30Work->humi_value);
}












