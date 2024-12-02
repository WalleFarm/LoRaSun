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

#include "drv_common.h"
#include "drv_uart.h"

u32 g_u32SecCounter=0;
u32 tickets_us=5,tickets_ms=5538;
bool bWatchDog=true;
static u8 g_u8BackUp[EEPROM_SIZE]={0};
 
//////////////高位表///////////////////
static const u8 auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

//////////////低位表///////////////////
static const u8 auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

/*		
================================================================================
描述 : crc16校验
输入 : 
输出 : 
================================================================================
*/ 
u16 drv_crc16(u8 *puchMsg,u16 usDataLen) 	
{ 											
	u8 uchCRCHi = 0xFF ; 				
	u8 uchCRCLo = 0xFF ; 				
	u16 uIndex ; 							
	while (usDataLen--) 					
	{
		uIndex = uchCRCHi ^ *puchMsg++ ; 	
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
		uchCRCLo = auchCRCLo[uIndex] ;
	}
	return (((u16)(uchCRCHi) << 8) | uchCRCLo) ;
}

/*		
================================================================================
描述 : 累加和校验
输入 : 
输出 : 
================================================================================
*/ 
u8 drv_check_sum(u8 *buff, u16 len)
{
	u8 sum=0;
	u16 i=0;
	for(i=0;i<len;i++)
	{
		sum+=buff[i];
	}
	return sum;
}

/*		
================================================================================
描述 : 芯片基本外设初始化
输入 : 
输出 : 
================================================================================
*/
void drv_common_init(void)
{
	RCC_ClocksTypeDef rcc;
	
	RCC_GetClocksFreq(&rcc);	
	tickets_us=rcc.SYSCLK_Frequency/1000000/13;
	tickets_ms=rcc.SYSCLK_Frequency/1000/13;
	if(tickets_us==0)
		tickets_us=1;
	UART_Init();//串口初始化
	drv_rtc_init();//RTC初始化
	
	printf("SYSCLK_Frequency=%dHZ\ntickets_us=%d,tickets_ms=%d",rcc.SYSCLK_Frequency,tickets_us,tickets_ms);
}

/*		
================================================================================
描述 : 异或校验
输入 : 
输出 : 
================================================================================
*/
u8 drv_xor_cal(u8 *buf, u16 len)
	{
		unsigned char ret;
		unsigned int i;
		ret = *buf;
		for (i = 1; i < len; i++)
		{
			ret = ret ^ (*(++buf));
		}
		return ret;
	}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u8 *memstr(u8 *full_str, u16 full_len, u8 *sub_str, u16 sub_len)
{
	int last_pos=full_len-sub_len+1;
	u16 i;
	
	if(last_pos<0)
	{
		return NULL;
	}
	for(i=0;i<last_pos;i++)
	{
		if(full_str[i]==sub_str[0])//先对比第一个字节
		{
			if( memcmp(&full_str[i], sub_str, sub_len)==0 )
			{
				return &full_str[i];
			}		
		}		
	}
	return NULL;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void printf_hex(char *str , u8 *hex, u16 len)
{
	u16 i;
	printf("%s",str);
	for(i=0;i<len;i++)
	{
		printf("%02X ",hex[i]);
	}
	printf("\n");
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void delay_ms(u32 dlyms)
{
	u32 i,j;
	for(i=0;i<dlyms;i++)
		for(j=0;j<tickets_ms;j++);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void delay_us(u32 dlyus)
{
	u32 i,j;
	for(i=0;i<dlyus;i++)
		for(j=0;j<tickets_us;j++);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void delay_os(u32 dlyms)
{
	const static u32 period_ms=1000/RT_TIMER_TICK_PER_SECOND;//系统滴答时钟的周期（ms）
	if(dlyms<period_ms)
	{
		dlyms=period_ms;
	}
	
	rt_thread_delay(dlyms/period_ms);
}
/*		
================================================================================
描述 :RTC初始化
输入 : 
输出 : 
================================================================================
*/
void drv_rtc_init(void)
{
#ifdef	USE_RTC	
	
	u8 count=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
  RCC_LSICmd(ENABLE);
	if(0 || BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)//检测是否断电过
	{
		BKP_DeInit();
		RCC_LSEConfig(RCC_LSE_ON);
		do
		{
			delay_ms(50);
			count++;
		}while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&count<100); 
		
		if(count<100)
		{
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
			printf("use LSE\n");
		}
		else
		{
			RCC_LSEConfig(RCC_LSE_OFF);
			RCC_LSICmd(ENABLE);
			while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);  //使用内部晶振		
			printf("use LSI\n");
		}
		
		
		RCC_RTCCLKCmd(ENABLE);		

		//Output
		BKP_TamperPinCmd(DISABLE);  
		//BKP_RTCOutputConfig(ENABLE);
		BKP_RTCOutputConfig(DISABLE);

		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
				
		//设置时间 
		RTC_SetCounter(0);
		RTC_WaitForLastTask();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);    
		printf("RTC restart!\n");
	}
	else
	{
//		RTC_SetCounter(0);
//		RTC_WaitForLastTask();

		printf("RTC continue!\n");
	}
#endif	
}



/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void drv_get_chip_uid(u8 *id_buff)
{	
	for(u8 i=0;i<12;i++)
	{
		id_buff[i]=*(u8*)(UID_ADDR+i);
	}
}



/*		
================================================================================
描述 : 读取模拟eeprom中的数据
输入 : start_addr--模拟的EEPROM地址,从0开始
输出 : 
================================================================================
*/ 
void EEPROM_Read(u32 start_addr, u8 *out_buff, u32 size)
{
//	static u8 back_up_buff[EEPROM_SIZE]={0};
	u32 i,read_data;
	if(start_addr+size>EEPROM_SIZE)//范围检查
	{
		return;
	} 
	for(i=0; i<EEPROM_SIZE; i+=4)
	{
		read_data=*(vu32*)(EEPROM_START_ADDR+i);
		memcpy(&g_u8BackUp[i], &read_data, 4);
//		memcpy(&back_up_buff[i], &read_data, 4);
	}	
	
	memcpy(out_buff, &g_u8BackUp[start_addr], size);
//	printf_hex(back_up_buff, sizeof(back_up_buff));
} 
	

/*		
================================================================================
描述 : 往模拟EEPROM中写数据
输入 : start_addr--模拟的EEPROM地址,从0开始
输出 : 
================================================================================
*/
void EEPROM_Write(u32 start_addr, u8 *buff, u32 size)
{
	u32 i;
	if(start_addr+size>EEPROM_SIZE)//范围检查
	{
		return;
	}	
	FLASH_Unlock();//解锁	
	EEPROM_Read(0, g_u8BackUp, EEPROM_SIZE);//备份	
	memcpy(&g_u8BackUp[start_addr], buff, size);//复制替换
	FLASH_ErasePage(EEPROM_START_ADDR);//擦除
	for(i=0; i<EEPROM_SIZE; i+=4)
	{
		FLASH_ProgramWord(EEPROM_START_ADDR+i, *(vu32*)&g_u8BackUp[i]);
	}	
//	printf_hex(g_u8BackUp, sizeof(g_u8BackUp));
	FLASH_Lock();//锁定
}
	
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_wdog_init(void)
{
#ifdef		USE_WDOG	
	RCC_LSICmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(0x800);//256  800约12s
	IWDG_ReloadCounter();
	IWDG_Enable();
#endif
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_wdog_feed(void)
{
#ifdef		USE_WDOG	
	if(bWatchDog==true)
		IWDG_ReloadCounter(); //喂狗
#endif
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_system_reset(void)
{
	bWatchDog=false;
	printf("system reset!\n");
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);    /*WWDG时钟使能*/
	WWDG_SetPrescaler(WWDG_Prescaler_8); /*设置IWDG预分频值*/
	WWDG_SetWindowValue(80);    /*设置窗口值*/
	WWDG_Enable(127);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u32 drv_get_sec_counter(void)
{
	return g_u32SecCounter;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_set_sec_counter(u32 time)
{
	g_u32SecCounter=time;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u32 drv_get_rtc_counter(void)
{
	return RTC_GetCounter();
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_set_rtc_counter(u32 time)
{
	RTC_SetCounter(time);
}

/*		
================================================================================
描述 :十进制字符串转为数值
输入 : 
输出 : 
================================================================================
*/
u32 drv_dec_string_to_num(char *str) 
{
  u32 i=0,value=0;
	while(str[i]>='0' && str[i]<='9' && i<10)
	{
		value=value*10+str[i]-'0';
		i++;
	}		
  return value;
}



