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


#include "app_oled96.h"
#include "drv_iic.h"
#include "nwk_bsp.h"


AppOLEDShowNodeStruct g_sOLEDShowNode={0};

I2cDriverStruct g_sI2cDrv={0};
/*		
================================================================================
描述 : 硬件IIC初始化
输入 : 
输出 : 
================================================================================
*/
void app_oled96_iic_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //开启GPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);    //开启I2C1时钟  ​
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;            //设置开漏复用模式
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;    //I2C1_SCL <--> PB6  I2C1_SDA <--> PB7
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  I2C_DeInit(I2C1);           //将I2C1外设寄存器初始化为其默认重置值  ​
  I2C_InitStructure.I2C_Ack                   = I2C_Ack_Enable;               //使能应答
  I2C_InitStructure.I2C_AcknowledgedAddress   = I2C_AcknowledgedAddress_7bit; //设置寻址地址为7位地址模式
  I2C_InitStructure.I2C_ClockSpeed            = 400000;                       //设置I2C速率为最大值400kHz
  I2C_InitStructure.I2C_DutyCycle             = I2C_DutyCycle_2;              //设置低比高电平时间为2:1
  I2C_InitStructure.I2C_Mode                  = I2C_Mode_I2C;                 //设置为I2C模式
  I2C_InitStructure.I2C_OwnAddress1           = 0x30;                         //设置自身设备地址为0x30
  I2C_Init(I2C1, &I2C_InitStructure);
  I2C_Cmd(I2C1, ENABLE);                                                      //使能I2C1  
}

/*		
================================================================================
描述 : OLED写字节函数
输入 : 
输出 : 
================================================================================
*/
void app_oled96_write_byte(u8 data, u8 mode)
{
  while( I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) );//等待总线空闲
  I2C_GenerateSTART(I2C1, ENABLE);
  while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR );//检测EV5事件
  I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
  while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR );//检测EV6事件
  if( mode )
     I2C_SendData(I2C1, 0x40);//写数据
  else
     I2C_SendData(I2C1, 0x00);//写命令
  while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR );//检测EV8_2事件
  I2C_SendData(I2C1, data);
  while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR );//检测EV8_2事件
  I2C_GenerateSTOP(I2C1, ENABLE);  
}

/*		
================================================================================
描述 : OLED初始化
输入 : 
输出 : 
================================================================================
*/
void app_oled96_init(void) 
{

  app_oled96_iic_init();//IIC初始化
  drv_oled96_write_byte_register(app_oled96_write_byte);//写函数注册
  drv_oled96_init();
  app_oled96_show_start();//显示主页面
  delay_os(1000);
  drv_oled96_clear();
  app_oled96_show_signal(-129, 18);
  app_oled96_show_tx_total(100, 99);

//  printf_oled("==debug 1234657890123456 time=%us", drv_get_rtc_counter());
  
}
 

/*		
================================================================================
描述 : 显示启动页面
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_start(void)
{
  for(u8 i=0; i<4; i++)
  {
    drv_oled96_show_chinese( 30+i*16, 2, i);//汉字显示
  }

  drv_oled96_show_str_f8x16( 20, 4,">>LoRaSun<<");     
}

/*		
================================================================================
描述 : 显示节点信息
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_node(u32 node_sn, u16 period)
{
  char buff[30]={0};
  sprintf(buff, "1.SN:%08X, %ds", node_sn, period);
  
  drv_oled96_clear_line( 0);//清理
  drv_oled96_show_str_f6x8( 0, 0, buff);   
}

/*		
================================================================================
描述 : 显示接收信号质量
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_signal(s16 rssi, s8 snr)
{
  char buff[30]={0};
  sprintf(buff, "2.SI:%ddbm, %ddbm", rssi, snr);
  drv_oled96_clear_line( 1);//清理
  drv_oled96_show_str_f6x8( 0, 1, buff);  
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_time(void)
{
  static s8 lse_flag=-1;
  char buff[30]={0};
  if(lse_flag<0)
  {
    lse_flag=RCC_GetFlagStatus(RCC_FLAG_LSERDY);
  }
  sprintf(buff, "3.RTC:%us", drv_get_rtc_counter());
  if(!lse_flag)
  {
    strcat(buff, " *");
  }
//  drv_oled96_clear_line( 2);//清理
  drv_oled96_show_str_f6x8( 0, 2, buff);   
}

/*		
================================================================================
描述 : 显示发送统计
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_tx_total(u16 total_cnts, u16 ok_cnts)
{
  char buff[30]={0};
  float percent=100.f*ok_cnts/total_cnts;
  sprintf(buff, "4.%d/%d=%.1f%%", ok_cnts, total_cnts, percent);
  drv_oled96_clear_line( 3);//清理
  drv_oled96_show_str_f6x8( 0, 3, buff);   
}

/*		
================================================================================
描述 : 显示网关信息
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_gw_info(u32 gw_sn, u8 freq_ptr, u8 run_mode, u8 join_state)
{
  char buff[30]={0};
  char ch=join_state==JoinStateOK ? ':' : ',';
  sprintf(buff, "5.%08X%c F%d,M%d", gw_sn, ch, freq_ptr, run_mode);
//  printf("gw=%s\n", buff);
  drv_oled96_clear_line(4);//清理
  drv_oled96_show_str_f6x8(0, 4, buff);   
}

/*		
================================================================================
描述 : 显示版本信息
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_version(void)
{
  char buff[30]={0};
  sprintf(buff, " Sv%s, Nv%d.%d", APP_VER, NWK_LORASUN_VERSION>>8&0xFF, NWK_LORASUN_VERSION&0xFF);
//  printf("gw=%s\n", buff);
  drv_oled96_clear_line(5);//清理
  drv_oled96_show_str_f6x8(0, 5, buff);   
}


/*		
================================================================================
描述 : 显示调试信息,不要太多内容
输入 : 
输出 : 
================================================================================
*/
void app_oled96_show_debug(char *str)
{
  drv_oled96_clear_line(6);//清理
  drv_oled96_clear_line(7);//清理
  drv_oled96_show_str_f6x8(0, 6, str); 
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void printf_oled(char const *const format, ...)
{
	static char show_buff[200];
  memset(show_buff, 0, sizeof(show_buff));

	__va_list  arglist;

	va_start(arglist, format);
	vsnprintf(show_buff, sizeof(show_buff), format, arglist);
	va_end(arglist); 
  strcpy(g_sOLEDShowNode.debug_str, show_buff);
//  app_oled96_show_debug(show_buff);
}

/*		
================================================================================
描述 : OLED线程
输入 : 
输出 : 
================================================================================
*/
void app_oled96_thread_entry(void *parameter)
{
  u8 run_cnts=0;
  app_oled96_init();
  app_oled96_show_node(g_sOLEDShowNode.node_sn, g_sOLEDShowNode.wake_period);//显示节点信息
  app_oled96_show_version();
  printf("app_oled96_thread_entry statt ###\n");
  while(1)
  {
    if(run_cnts++%5==0)
    {
      app_oled96_show_time();
      app_oled96_show_signal(g_sOLEDShowNode.rssi, g_sOLEDShowNode.snr);//更新信号显示
      app_oled96_show_tx_total(g_sOLEDShowNode.total_cnts, g_sOLEDShowNode.ok_cnts);//显示发送次数
      app_oled96_show_gw_info(g_sOLEDShowNode.gw_sn, g_sOLEDShowNode.freq_ptr, g_sOLEDShowNode.run_mode, g_sOLEDShowNode.join_state);
    }
    if(g_sOLEDShowNode.debug_str[0]>0)
    {
      app_oled96_show_debug(g_sOLEDShowNode.debug_str);
      g_sOLEDShowNode.debug_str[0]=0;
    }
    delay_os(200);
  }
  

}







  






