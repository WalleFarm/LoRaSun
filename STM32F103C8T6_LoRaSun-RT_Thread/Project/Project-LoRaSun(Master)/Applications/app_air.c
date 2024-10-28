

#include "app_air.h" 

AirWorkStruct g_sAirWork={0};
/*		
================================================================================
描述 : 整体初始化
输入 : 
输出 : 
================================================================================
*/
void app_air_init(void)
{
  g_sAirWork.device_sn=0x11223345;//设备识别码
  app_mqtt_init();
  
  app_sht30_init();
  app_motor_init();
} 

/*		
================================================================================
描述 : AT24C64初始化
输入 : 
输出 : 
================================================================================
*/
void app_at24c64_init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //开启时钟 
  at24c64_init(GPIOB, GPIO_Pin_8, GPIOB, GPIO_Pin_9); //引脚定义初始化
  
  u8 temp_buff[10]={"123456789"};
  at24c64_write(0x1000, temp_buff, 10);//写入
  memset(temp_buff, 0, sizeof(temp_buff));//清零
  at24c64_read(0x1000, temp_buff, 10);//读取
}



/*		
================================================================================
描述 : 获取设备识别码
输入 : 
输出 : 
================================================================================
*/
u32 app_air_take_sn(void)
{
  return g_sAirWork.device_sn;
}


/*		
================================================================================
描述 : 底层通讯协议组合
输入 : 
输出 : 
================================================================================
*/
void app_air_send_level(u8 cmd_type, u8 *buff, u16 len)
{
  u8 make_buff[100]={0};
  u16 make_len=0;
  u16 total_len=0;
  u32 device_sn=g_sAirWork.device_sn;
  
  if(len+10>sizeof(make_buff))
    return;
  
  make_buff[make_len++]=0xAA;
  make_buff[make_len++]=0x55;
  make_buff[make_len++]=total_len>>8;
  make_buff[make_len++]=total_len;   //数据长度,暂存
  make_buff[make_len++]=device_sn>>24;
  make_buff[make_len++]=device_sn>>16;
  make_buff[make_len++]=device_sn>>8;
  make_buff[make_len++]=device_sn;  
  make_buff[make_len++]=cmd_type;  //命令
  memcpy(&make_buff[make_len], buff, len);
  make_len+=len;
  make_buff[2]=make_len>>8;
  make_buff[3]=make_len; //更新数据长度
  u16 crcValue=drv_crc16(make_buff, make_len);
  make_buff[make_len++]=crcValue>>8;
  make_buff[make_len++]=crcValue;  
  app_mqtt_pub_data(make_buff, make_len);
}

/*		
================================================================================
描述 : 发送设备的状态数据
输入 : 
输出 : 
================================================================================
*/
void app_air_send_status(void)
{
  u8 cmd_buff[20]={0};
  u16 cmd_len=0;
  u16 tmp_u16=0;
  
  tmp_u16=g_sAirWork.tag_sht30.temp_value*10+1000;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //温度
  tmp_u16=g_sAirWork.tag_sht30.humi_value*10;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //湿度
  tmp_u16=g_sAirWork.pm25_value;
  cmd_buff[cmd_len++]=tmp_u16>>8;
  cmd_buff[cmd_len++]=tmp_u16;  //PM2.5  
  cmd_buff[cmd_len++]=g_sAirWork.fan_speed;   //风速等级
  cmd_buff[cmd_len++]=g_sAirWork.switch_state;  
//  printf_hex("status= ", cmd_buff, cmd_len);
  app_air_send_level(AIR_CMD_DATA, cmd_buff, cmd_len);
  
}


/*		
================================================================================
描述 : 设备解析服务器下发的数据
输入 : 
输出 : 
================================================================================
*/
void app_air_recv_parse(u8 *buff, u16 len)
{
  u8 head[2]={0xAA, 0x55};
  u8 *pData=memstr(buff, len, head, 2);
  if(pData!=NULL)
  {
    u16 total_len=pData[2]<<8 | pData[3];
    u16 crcValue=pData[total_len]<<8 | pData[total_len+1];
    if(crcValue==drv_crc16(pData, total_len))
    {
      pData+=4;
      u32 device_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
      pData+=4;
      if(device_sn!=g_sAirWork.device_sn)//识别码确认
        return;
      u8 cmd_type=pData[0];
      pData++;
      switch(cmd_type)
      {
        case AIR_CMD_HEART://心跳包
        {
          
          break;
        }
        case AIR_CMD_DATA://数据包
        {
          
          break;
        }
        case AIR_CMD_SET_SPEED://设置风速
        {
          u8 speed=pData[0];
          pData+=1;
          app_motor_set_speed(speed);
          break;
        }        
        case AIR_CMD_SET_SWITCH://设置开关
        {
          u8 state=pData[0];
          pData+=1;
          g_sAirWork.switch_state=state;
          if(state>0)
          {
            app_motor_set_speed(100);//启动风扇
          }
          else
          {
            app_motor_set_speed(0);//停止风扇
          }
          app_air_send_status();
          break;
        }
      }
    }
  }
}

/*		
================================================================================
描述 : 净化器总任务线程
输入 : 
输出 : 
================================================================================
*/
void app_air_thread_entry(void *parameter) 
{
  u16 run_cnts=0;
  printf("app_air_thread_entry start ****\n");
  delay_os(1000);
//  app_air_init();//初始化
  drv_bc260_init(&g_sUART2, BC260_SERVER_CHINANET_AEP, BC260_SLEEP_NO);
  while(1)
  {
//    app_mqtt_main(); //MQTT主程序
    drv_bc260_main();//NB BC260 主程序
    
    delay_os(20);//延时,每个任务线程都要添加,才不会阻塞,最小延时5ms, 即delay_os(5);
    
    if(run_cnts%500==0)//20*200=4000ms 执行一次
    {
//      drv_sht30_read_th(&g_sAirWork.tag_sht30);//读取温湿度值
//      app_pm25_recv_check();//解析PM2.5值
//      
//      app_air_send_status();//上报数据
      
//      u8 cmd_buff[10]={"123456789"};
//      drv_bc260_send_data(cmd_buff, 9);
    }
    run_cnts++;
  }
  
}

/*		
================================================================================
描述 : SHT30温湿度初始化
输入 : 
输出 : 
================================================================================
*/
void app_sht30_init(void)
{
  //SDA--PB8   SCL--PB9
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  I2cDriverStruct *pIIC=&g_sAirWork.tag_sht30.tag_iic;
  
  pIIC->pin_sda=GPIO_Pin_8;
  pIIC->port_sda=GPIOB;
  
  pIIC->pin_scl=GPIO_Pin_9;
  pIIC->port_scl=GPIOB;  
  
  drv_sht30_init(&g_sAirWork.tag_sht30);
}


#define PWMPeriodValue  1000 //PWM信号周期值

/*		
================================================================================
描述 : 风扇电机初始化
输入 : 
输出 : 
================================================================================
*/
void app_motor_init(void)
{
// 使能TIM1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
    // 设置GPIOA_8为复用功能推挽输出
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    // 初始化TIM1 PWM模式
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_Period = PWMPeriodValue-1; // 周期为10k
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1; // 预分频器设置为7199，确保计数器的频率为1MHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;	
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//选择PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出比较极性选择
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出使能
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);//初始化 TIM1 OC1
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);//使能CCR1自动重装
    TIM_ARRPreloadConfig(TIM1,ENABLE);//开启预装载  
    TIM_CtrlPWMOutputs(TIM1, ENABLE);//手动开启,防止与串口1冲突后不启动


    // 使能TIM1的输出比较预装载寄存器
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
 
    // 使能TIM1
    TIM_Cmd(TIM1, ENABLE);  
    
    
    printf("app_motor_init ok!\n");
}

/*		
================================================================================
描述 : 设置速度
输入 : speed 0~255,速度分为256级
输出 : 
================================================================================
*/
void app_motor_set_speed(u8 speed)
{
  g_sAirWork.fan_speed=speed;
  u16 value=speed*PWMPeriodValue/255*0.6;//系数主要是防止电机电流过大影响设备稳定
  printf("pwm val=%d\n", value);
  TIM_SetCompare1(TIM1, value);
}


/*		
================================================================================
描述 : PM2.5数据解析
输入 : 
输出 : 
================================================================================
*/
void app_pm25_recv_check(void)
{
  static UART_Struct *pUART=&g_sUART3;

  if(pUART->iRecv>=4)
  {
    u8 head[1]={0xA5};
    u8 *pData=pUART->pBuff;
//    printf_hex("pm25 recv=", pData, pUART->iRecv);
    if( (pData=memstr(pData, pUART->iRecv, head, 1))!=NULL )//查找数据头
    {
      u8 sum0=drv_check_sum(pData, 3) & 0x7F; //校验码
      u8 sum1=pData[3];
      if(sum0==sum1)
      {
        u16 pm25=(pData[1]&0x7F)*128+(pData[2]&0x7F);//根据协议组合数据
        pm25*=0.04;//以小米净化器的数值为基准,将灰尘度数值乘以系数来获取PM2.5数值
        g_sAirWork.pm25_value=pm25;
        printf("pm25=%d ug/m3\n", pm25);
      }
    }
    
    UART_Clear(pUART);//清理数据
  }
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
描述 :
输入 : 
输出 : 
================================================================================
*/










