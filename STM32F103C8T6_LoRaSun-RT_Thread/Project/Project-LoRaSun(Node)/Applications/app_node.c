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

#include "app_node.h" 

#ifdef  LORA_SX1278 
DrvSx1278Struct g_sDrvSx1278={0};
#endif

#ifdef  LORA_SX1268 
DrvSx1268Struct g_sDrvSx1268={0};
#endif

AppNodeSaveStruct g_sAppNodeSave={0};
AppNodeWorkStruct g_sAppNodeWork={0};
extern AppOLEDShowNodeStruct g_sOLEDShowNode;//OLED节点显示参数
/*		
================================================================================
描述 : 硬件复位
输入 : 
输出 : 
================================================================================
*/
static void app_node_lora_reset(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	delay_ms(10);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	delay_ms(10);	

}

/*		
================================================================================
描述 : 片选0
输入 : 
输出 : 
================================================================================
*/
static void app_node_lora_cs0(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

/*		
================================================================================
描述 : 片选1
输入 : 
输出 : 
================================================================================
*/
static void app_node_lora_cs1(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

/*		
================================================================================
描述 : SPI字节读写
输入 : 
输出 : 
================================================================================
*/
static u8 app_node_lora_spi_rw_byte(u8 byte)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,byte);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);  
}

/*		
================================================================================
描述 : SX1268忙检测
输入 : 
输出 : 
================================================================================
*/
void app_node_wait_on_busy(void)
{
	u32 counts=0;
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)>0)
	{
		if(counts++>1000)
		{
			printf("busy line wait time out!\n");
			return;
		}
		delay_ms(1);
	}
}

/*		
================================================================================
描述 : led初始化
输入 : 
输出 : 
================================================================================
*/
void app_node_led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  
  //PB15--绿灯
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

  //PA3--蓝灯
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
}

/*		
================================================================================
描述 : 蓝灯开关
输入 : 
输出 : 
================================================================================
*/
void app_node_led_set_blue(bool state)
{
  if(state)//亮灯
  {
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
  }
  else
  {
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
  }  
}

/*		
================================================================================
描述 : 绿灯开关
输入 : 
输出 : 
================================================================================
*/
void app_node_led_set_green(bool state)
{
  if(state)//亮灯
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
  }
  else
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
  }  
}

/*		
================================================================================
描述 : 按键初始化
输入 : 
输出 : 
================================================================================
*/
void app_node_key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  
  //PB9--按键输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*		
================================================================================
描述 : 按键检测
输入 : 
输出 : 
================================================================================
*/
void app_node_key_check(void)
{
  static u16 key_cnts=0;
  u8 val=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
  if(val==0)//按下
  {
    key_cnts++;
    if(key_cnts==6)//外部循环周期是5ms,这里大概是30ms
    {
      printf("key trigger!\n");
      app_node_send_status();
    }
    if(key_cnts>500)
    {
      drv_system_reset();//长按放开复位
    }    
  }
  else//放开
  {
    key_cnts=0;
  }
}

/*		
================================================================================
描述 : 温度ADC初始化
输入 : 
输出 : 
================================================================================
*/
void app_node_temp_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef   ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1, ENABLE ); //使能ADC1通道时钟 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1); //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC 工作模式:独立模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; //AD 多通道模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //AD 连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC 数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;//顺序进行规则转换的 ADC 通道的数目 
  ADC_Init(ADC1, &ADC_InitStructure);	//根据指定的参数初始化外设 ADCx  
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );  
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );  
		
	ADC_Cmd(ADC1, ENABLE);  //使能指定的 ADC1
	ADC_ResetCalibration(ADC1);//执行复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));  //等待复位校准结束
	ADC_StartCalibration(ADC1);    //开始指定 ADC1 的校准状态
	while(ADC_GetCalibrationStatus(ADC1));    //等待 AD 校准结束
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能指定的 ADC1 的软件转换--开始连续转换
  
}

/*		
================================================================================
描述 : 温度获取
输入 : 
输出 : 
================================================================================
*/
u16 app_node_temp_update(void)
{
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	u16 adc_value=ADC_GetConversionValue(ADC1);  
  float res_value=0.f;//电阻阻值  
  float volt=adc_value/4096.f*3.3f;
  res_value=10000.f*volt/(3.3f-volt);//计算NTC阻值
  
  float Rp=10000.f;
  float T2=273.15f+25.f;
  float Bx=3450.f;
  float Ka=273.15f;	
  float temp_value=1.f/(1.f/T2+log(res_value/Rp)/Bx)-Ka+0.5f;
//	printf("@@@@@@ ave_value =%u, volt=%.1fV, res_value=%.1f ohm, temp_value=%.2fC\n", ave_value, volt, res_value, temp_value);
//  printf("temp_value=%.2f C\n", temp_value);
  u16 temp_u16=temp_value*10+1000;    
  return temp_u16;
}


/*		
================================================================================
描述 :应用层sx1278初始化,注册
输入 : 
输出 : 
================================================================================
*/
static void app_node_lora_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	

	//复位引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//SPI初始化	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//SPI
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);		//CS

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);   //CLK   MOSI

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);  //MISO

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);

#ifdef  LORA_SX1278 
	g_sDrvSx1278.tag_hal_sx1278.sx1278_reset = app_node_lora_reset;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_cs_0 = app_node_lora_cs0;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_cs_1 = app_node_lora_cs1;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_spi_rw_byte = app_node_lora_spi_rw_byte;
	g_sDrvSx1278.tag_hal_sx1278.set_led = app_node_led_set_blue;
	drv_sx1278_init(&g_sDrvSx1278);//初始化
	
	nwk_node_set_lora_dev(&g_sDrvSx1278);//往协议栈传递设备指针
	printf("app_sx1278_hal_init ok!\n");
#endif

#ifdef  LORA_SX1268 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//BUSY
	
	g_sDrvSx1268.tag_hal_sx1268.sx1268_reset = app_node_lora_reset;
	g_sDrvSx1268.tag_hal_sx1268.sx1268_cs_0 = app_node_lora_cs0;
	g_sDrvSx1268.tag_hal_sx1268.sx1268_cs_1 = app_node_lora_cs1;
	g_sDrvSx1268.tag_hal_sx1268.sx1268_spi_rw_byte = app_node_lora_spi_rw_byte;
	g_sDrvSx1268.tag_hal_sx1268.wait_on_busy = app_node_wait_on_busy;
  g_sDrvSx1268.tag_hal_sx1268.delay_ms = delay_ms;
  g_sDrvSx1268.tag_hal_sx1268.set_led = app_node_led_set_blue;
	drv_sx1268_init(&g_sDrvSx1268);//初始化
	 
  nwk_node_set_lora_dev(&g_sDrvSx1268); //往协议栈传递设备指针
	printf("app_sx12768_hal_init ok!\n");
#endif  
  
  /*   接线
  *    PB14--复位reset
  *    PA4--片选NSS 
  *    PA5--时钟SCK
  *    PA6--MISO
  *    PA7--MOSI
  *    3.3V--电源
  *    GND --GND
  */
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/

/*		
================================================================================
描述 : 保存配置信息
输入 : 
输出 : 
================================================================================
*/
void app_node_write_config(void)
{
  g_sAppNodeSave.crcValue=nwk_crc16((u8*)&g_sAppNodeSave, sizeof(g_sAppNodeSave)-2);
  EEPROM_Write(50, (u8*)&g_sAppNodeSave, sizeof(g_sAppNodeSave));
}

/*		
================================================================================
描述 : 读取配置信息
输入 : 
输出 : 
================================================================================
*/
void app_node_read_config(void)
{
  EEPROM_Read(50, (u8*)&g_sAppNodeSave, sizeof(g_sAppNodeSave));
  if(g_sAppNodeSave.crcValue!=nwk_crc16((u8*)&g_sAppNodeSave, sizeof(g_sAppNodeSave)-2))
  {
    memset(&g_sAppNodeSave, 0, sizeof(g_sAppNodeSave));
    g_sAppNodeSave.node_sn=0x11223300;
    g_sAppNodeSave.wake_period=10;
    app_node_write_config();
    printf("app_node_read_config new!\n");
  }
  printf("read node_sn=0x%08X, period=%ds\n", g_sAppNodeSave.node_sn, g_sAppNodeSave.wake_period);
  
//  u8 base_freq_ptr=18;
//	nwk_node_add_gw(0xC1012202, base_freq_ptr, 4, NwkRunModeStatic);//手动添加目标网关 NwkRunModeStatic NwkRunModeDynamic
	nwk_node_set_sn(g_sAppNodeSave.node_sn);//设置节点SN
	nwk_node_set_wake_period(g_sAppNodeSave.wake_period);  //设置节点唤醒周期
  nwk_node_set_search_time(120*1, 10);//网关搜索周期 要进行并发测试的时候周期设置长一点,避免影响测试效果
                                    //网关默认设置是2分钟广播一次
  u8 root_key[17]={"0123456789ABCDEF"};//根密码,跟网关保持一致
  nwk_node_set_root_key(root_key);
  
  g_sOLEDShowNode.node_sn=g_sAppNodeSave.node_sn;
  g_sOLEDShowNode.wake_period=g_sAppNodeSave.wake_period;
  
  srand(g_sOLEDShowNode.node_sn*nwk_get_rtc_counter());//随机数种子
}

/*		
================================================================================
描述 : 设置并保存节点SN
输入 : 
输出 : 
================================================================================
*/
void app_node_set_sn(u32 node_sn)
{
  printf("set node sn=0x%08X\n", node_sn);
  g_sAppNodeSave.node_sn=node_sn;
  app_node_write_config();
  g_sOLEDShowNode.node_sn=g_sAppNodeSave.node_sn;
}

/*		
================================================================================
描述 : 设置并保存唤醒周期
输入 : 
输出 : 
================================================================================
*/
void app_node_set_wake_period(u16 wake_period)
{
  printf("set wake period=%ds\n", wake_period);
  g_sAppNodeSave.wake_period=wake_period;
  app_node_write_config();
  nwk_node_set_wake_period(wake_period);
  g_sOLEDShowNode.wake_period=g_sAppNodeSave.wake_period;
}

/*		
================================================================================
描述 : 发送节点状态信息
输入 : 
输出 : 
================================================================================
*/
void app_node_send_status(void)
{
  static u16 pack_id=0;
	printf("app_node_send_status  ###\n");
	u8 make_buff[30]={0};
	u8 make_len=0;
	u16 temp_val=app_node_temp_update();//温度更新
	u32 now_time=nwk_get_rtc_counter();
	make_buff[make_len++]=0x01;//状态  命令
	make_buff[make_len++]=temp_val>>8;
	make_buff[make_len++]=temp_val;
	make_buff[make_len++]=now_time>>24;
 
	make_buff[make_len++]=now_time>>16;
	make_buff[make_len++]=now_time>>8;
	make_buff[make_len++]=now_time;
	make_buff[make_len++]=pack_id>>8;
	make_buff[make_len++]=pack_id;  
  pack_id++;
	nwk_node_send2gateway(make_buff, make_len);
}

/*		
================================================================================
描述 : 休眠/唤醒检测
输入 : 
输出 : 
================================================================================
*/
void app_node_sleep_check(void)
{
  static u32 last_time=0;
  u32 alarm_time=nwk_node_calcu_alarm_time();
  if(last_time!=alarm_time)
  {
    last_time=alarm_time;
    printf("******alarm_time=%us\n", alarm_time);
    if(alarm_time>0)//可以休眠
    {
      //这里只进行LoRa模块的低功耗设置,没有针对整体
      nwk_node_sleep_init();
    }    
    else//唤醒
    {
      nwk_node_device_init();
    }
  }

}


/*		
================================================================================
描述 : 协议层事件处理
输入 : 
输出 : 
================================================================================
*/
void app_node_event_parse(void)
{
  NwkNodeEventStruct *pEvent=nwk_node_event_check();
  if(pEvent)//有事件发生
  {
    switch(pEvent->event)//事件处理
    {
      case NwkNodeEventTxGwOK:
      {
        printf("### NwkNodeEventTxGwOK\n");
        break;
      }
      case NwkNodeEventTxGwFailed:
      {
        printf("### NwkNodeEventTxGwFailed\n");      
        printf_oled("tx gw failed!");
        break;
      }
      case NwkNodeEventTxD2DOK:
      {
        printf("### NwkNodeEventTxD2DOK\n");
        break;
      }
      case NwkNodeEventTxD2DFailed:
      {
        printf("### NwkNodeEventTxD2DFailed\n");
        printf_oled("tx d2d failed!");
        break;
      }      
      case NwkNodeEventJoinResult:
      {
        u8 *pData=pEvent->params;
        u8 join_state=pData[0];
        pData+=1;
        u32 gw_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
        pData+=4;
        printf("join %08X =%d!\n", gw_sn, join_state);
        printf_oled("join %08X ok!", gw_sn);
        break;
      }
    }
    pEvent->event=NwkNodeEventNone;
  }
}


/*		
================================================================================
描述 : 节点线程
输入 : 
输出 : 
================================================================================
*/
void app_node_thread_entry(void *parameter) 
{
  static u32 run_cnts=0;
  static bool led_state=false;  

  printf("LoRaSun Nwk version=V%d.%d\n", NWK_LORASUN_VERSION>>8&0xFF, NWK_LORASUN_VERSION&0xFF);

  
  app_node_read_config();//读取SN等参数
  
  app_node_lora_init();//lora初始化
  app_node_led_init();//指示灯初始化
  
  app_node_key_init();//按键初始化
  app_node_temp_init();//温度头初始化
  
  //指示灯测试
  app_node_led_set_blue(true);
  app_node_led_set_green(true);
  delay_os(500);
  app_node_led_set_blue(false);
  app_node_led_set_green(false);

  while(1)
  {
    nwk_node_main_process();
    NwkNodeRecvFromStruct *recv_from=nwk_node_recv_from_check();
    if(recv_from)
    {
      printf_hex("app buff=", recv_from->app_data, recv_from->data_len);//用户数据
      printf_oled("rx=%s", recv_from->app_data);
    }
    app_node_event_parse();//协议栈内部事件处理
    app_node_sleep_check();//休眠检测

    app_node_key_check();//按键检测
    if(run_cnts++%200==0)//大概1秒
    {
      led_state=!led_state;
      app_node_led_set_green(led_state);//运行指示灯
//      app_node_temp_update();//温度更新
      
      //显示参数复制更新
      RfParamStruct *rf_param=nwk_node_take_rf_param();//获取最近一次接收信号
      g_sOLEDShowNode.rssi=rf_param->rssi;
      g_sOLEDShowNode.snr=rf_param->snr;
      
      u16 total_cnts=0, ok_cnts=0;
      nwk_node_take_tx_cnts(&total_cnts, &ok_cnts);//总计发送次数和成功发送次数
      g_sOLEDShowNode.total_cnts=total_cnts;
      g_sOLEDShowNode.ok_cnts=ok_cnts;
      
      //网关信号
      extern NwkNodeWorkStruct g_sNwkNodeWork;
      static u8 ptr=0;
      for(u8 i=0; i<NWK_GW_NUM; i++)
      {
        if(ptr>=NWK_GW_NUM)ptr=0;
        NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[ptr];
        ptr++;
        if(pGateWay->gw_sn>0)
        {
          g_sOLEDShowNode.gw_sn=pGateWay->gw_sn;
          g_sOLEDShowNode.freq_ptr=pGateWay->base_freq_ptr;
          g_sOLEDShowNode.run_mode=pGateWay->run_mode;
          g_sOLEDShowNode.join_state=pGateWay->join_state;
          break;
        }
      }       
      
    }
    if(0)//并发数测试
    {
      static u32 send_time=0;
      u32 now_time=nwk_get_rtc_counter();
      if(now_time>10)
      {
        if(now_time%60==0 && send_time==0)
        {
          u16 wait_time=nwk_get_rand()%45;
          send_time=now_time+wait_time;
          printf("\n\n$$$$ net test trigger! $$$$\nwait_time=%ds\n", wait_time);          
        }
        else if(now_time==send_time)
        {
          send_time=0;
          app_node_send_status();
        }        
      }
    }
		
    delay_os(5);
  }
}



/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/



































