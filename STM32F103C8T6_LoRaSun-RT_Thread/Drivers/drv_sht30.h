
#ifndef __DRV_SHT30_H__
#define __DRV_SHT30_H__


#include "drv_iic.h"


typedef struct
{
  u8 dev_addr;//器件地址,默认0x44;如果ADDR引脚上拉则是0x45
  
  I2cDriverStruct tag_iic;
  
  float temp_value,humi_value;
  
}Sht30WorkStruct;


void drv_sht30_init(Sht30WorkStruct *pSht30Work);
void drv_sht30_set_addr(Sht30WorkStruct *pSht30Work, u8 dev_addr);  
void drv_sht30_read_th(Sht30WorkStruct *pSht30Work);

#endif

