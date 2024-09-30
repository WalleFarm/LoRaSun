
#include "drv_modbus.h"


/*		
================================================================================
描述 :modbus 0x01的报文组合
输入 : 
输出 :  
================================================================================
*/
u16 drv_modbus_send_fun01(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size)
{
  if(make_size<20)
  {
    return 0;
  }
	u16 make_len=0;
	u16 crcValue;
	
	make_buff[make_len++]=slave_addr;
	make_buff[make_len++]=0x01;
	make_buff[make_len++]=reg_start>>8;
	make_buff[make_len++]=reg_start;
	make_buff[make_len++]=reg_num>>8;
	make_buff[make_len++]=reg_num;
	crcValue=drv_crc16(make_buff, make_len);
	make_buff[make_len++]=crcValue>>8;
	make_buff[make_len++]=crcValue;
	return make_len;
}

/*		
================================================================================
描述 :modbus 0x02的报文组合
输入 : 
输出 :  
================================================================================
*/
u16 drv_modbus_send_fun02(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size)
{
  if(make_size<20)
  {
    return 0;
  }
	u16 make_len=0;
	u16 crcValue;
	
	make_buff[make_len++]=slave_addr;
	make_buff[make_len++]=0x02;
	make_buff[make_len++]=reg_start>>8;
	make_buff[make_len++]=reg_start;
	make_buff[make_len++]=reg_num>>8;
	make_buff[make_len++]=reg_num;
	crcValue=drv_crc16(make_buff, make_len);
	make_buff[make_len++]=crcValue>>8;
	make_buff[make_len++]=crcValue;
	return make_len;
}

/*		
================================================================================
描述 :modbus 0x03的报文组合
输入 : 
输出 :  
================================================================================
*/
u16 drv_modbus_send_fun03(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size)
{
  if(make_size<20)
  {
    return 0;
  }
	u16 make_len=0;
	u16 crcValue;
	
	make_buff[make_len++]=slave_addr;
	make_buff[make_len++]=0x03;
	make_buff[make_len++]=reg_start>>8;
	make_buff[make_len++]=reg_start;
	make_buff[make_len++]=reg_num>>8;
	make_buff[make_len++]=reg_num;
	crcValue=drv_crc16(make_buff, make_len);
	make_buff[make_len++]=crcValue>>8;
	make_buff[make_len++]=crcValue;
	return make_len;
}

/*		
================================================================================
描述 :modbus 0x04的报文组合
输入 : 
输出 :  
================================================================================
*/
u16 drv_modbus_send_fun04(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *make_buff, u16 make_size)
{
  if(make_size<20)
  {
    return 0;
  }
	u16 make_len=0;
	u16 crcValue;
	
	make_buff[make_len++]=slave_addr;
	make_buff[make_len++]=0x04;
	make_buff[make_len++]=reg_start>>8;
	make_buff[make_len++]=reg_start;
	make_buff[make_len++]=reg_num>>8;
	make_buff[make_len++]=reg_num;
	crcValue=drv_crc16(make_buff, make_len);
	make_buff[make_len++]=crcValue>>8;
	make_buff[make_len++]=crcValue;
	return make_len;
}

/*		
================================================================================
描述 :modbus 0x05的报文组合
输入 : 
输出 :  
================================================================================
*/
u16 drv_modbus_send_fun05(u8 slave_addr, u16 reg_start, u16 reg_value, u8 *make_buff, u16 make_size)
{
  if(make_size<20)
  {
    return 0;
  }
	u16 make_len=0;
	u16 crcValue;
	
	make_buff[make_len++]=slave_addr;
	make_buff[make_len++]=0x05;
	make_buff[make_len++]=reg_start>>8;
	make_buff[make_len++]=reg_start;
	make_buff[make_len++]=reg_value>>8;
	make_buff[make_len++]=reg_value;	
	crcValue=drv_crc16(make_buff, make_len);
	make_buff[make_len++]=crcValue>>8;
	make_buff[make_len++]=crcValue;
	return make_len;
}


/*		
================================================================================
描述 :modbus 0x06的报文组合
输入 : 
输出 :  
================================================================================
*/
u16 drv_modbus_send_fun06(u8 slave_addr, u16 reg_start, u16 reg_value, u8 *make_buff, u16 make_size)
{
  if(make_size<20)
  {
    return 0;
  }
	u16 make_len=0;
	u16 crcValue;
	
	make_buff[make_len++]=slave_addr;
	make_buff[make_len++]=0x06;
	make_buff[make_len++]=reg_start>>8;
	make_buff[make_len++]=reg_start;
	make_buff[make_len++]=reg_value>>8;
	make_buff[make_len++]=reg_value;	
	crcValue=drv_crc16(make_buff, make_len);
	make_buff[make_len++]=crcValue>>8;
	make_buff[make_len++]=crcValue;
	return make_len;
}

/*		
================================================================================
描述 : modbus 0x10的报文组合
输入 : 
输出 : 
================================================================================
*/
u16 drv_modbus_send_fun16(u8 slave_addr, u16 reg_start, u16 reg_num, u8 *reg_data, u8 *make_buff, u16 make_size)
{
	u16 make_len=0;
	u16 crcValue;
	u8 	data_len=reg_num*2;

	if(make_size<10+data_len)
		return 0;
	make_buff[make_len++]=slave_addr;
	make_buff[make_len++]=0x10;
	make_buff[make_len++]=reg_start>>8;
	make_buff[make_len++]=reg_start; //寄存器起始地址
	make_buff[make_len++]=reg_num>>8;
	make_buff[make_len++]=reg_num;	//寄存器数量
	make_buff[make_len++]=data_len;//数据区长度
	memcpy(&make_buff[make_len], reg_data, data_len);//数据区
	make_len+=data_len;
	crcValue=drv_crc16(make_buff, make_len);
	make_buff[make_len++]=crcValue>>8;
	make_buff[make_len++]=crcValue;
	
	return make_len;		
}


/*		
================================================================================
描述 : modbus 基础数据解析
输入 : 
输出 : 
================================================================================
*/
u16 drv_modbus_parse_base(u8 slave_addr, u8 fun_code, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size)
{
  u16 recv_len=in_len, crcValue;
  u8 *pData=in_buff;
  u8 data_len=0;
  if(recv_len<4 || recv_len>250)
    return 0;
  for(u8 i=0;i+4<recv_len;i++,pData++)
  {
    if(pData[0]==slave_addr && pData[1]==fun_code)//比较地址和功能码
    {
      data_len=pData[2];
      crcValue=pData[data_len+3]<<8|pData[data_len+4];
      if(crcValue==drv_crc16(pData, data_len+3))
      {
        if(data_len<out_size)
        {
          memcpy(out_buff, &pData[3], data_len);
        }
        else
        {
          data_len=0;
        }          
        break;
      }						

    }				
  }
  return data_len;
}

/*		
================================================================================
描述 : modbus 0x01 数据解析
输入 : 
输出 : 
================================================================================
*/
u16 drv_modbus_parse_fun01(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size)
{
  return drv_modbus_parse_base(slave_addr, 0x01, in_buff, in_len, out_buff, out_size);
}


/*		
================================================================================
描述 : modbus 0x02 数据解析
输入 : 
输出 : 
================================================================================
*/
u16 drv_modbus_parse_fun02(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size)
{
  return drv_modbus_parse_base(slave_addr, 0x02, in_buff, in_len, out_buff, out_size);
}

/*		
================================================================================
描述 : modbus 0x03 数据解析
输入 : 
输出 : 
================================================================================
*/
u16 drv_modbus_parse_fun03(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size)
{
  return drv_modbus_parse_base(slave_addr, 0x03, in_buff, in_len, out_buff, out_size);
}

/*		
================================================================================
描述 : modbus 0x04 数据解析
输入 : 
输出 : 
================================================================================
*/
u16 drv_modbus_parse_fun04(u8 slave_addr, u8 *in_buff, u16 in_len, u8 *out_buff, u16 out_size)
{
  return drv_modbus_parse_base(slave_addr, 0x04, in_buff, in_len, out_buff, out_size);
}













