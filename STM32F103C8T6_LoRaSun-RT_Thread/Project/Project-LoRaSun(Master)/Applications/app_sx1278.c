
#include "app_sx1278.h"  
 
 
#ifdef  LORA_SX1278 
DrvSx1278Struct g_sDrvSx1278={0};
#endif

#ifdef  LORA_SX1268 
DrvSx1268Struct g_sDrvSx1268={0};
#endif
/*		
================================================================================
描述 : 硬件复位
输入 : 
输出 : 
================================================================================
*/
static void app_sx1278_reset(void)
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
static void app_sx1278_cs0(void)
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
static void app_sx1278_cs1(void)
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
static u8 app_sx1278_spi_rw_byte(u8 byte)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,byte);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);  
}

/*		
================================================================================
描述 : led初始化
输入 : 
输出 : 
================================================================================
*/
void app_led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  
  //PB15--绿灯
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
}

/*		
================================================================================
描述 : 绿灯
输入 : 
输出 : 
================================================================================
*/
void app_led_set_green(bool state)
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
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_uart_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  
  //PB2--CTS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
}

/*		
================================================================================
描述 : 设置CTS模式
输入 : 
输出 : 
================================================================================
*/
void app_uart_cts_set_mode(u8 mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  if(mode==0)//输入模式
  {
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
  }
  else//输出-拉低模式
  {
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
    GPIO_ResetBits(GPIOB, GPIO_Pin_2);
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_uart_slave_send(u8 *buff, u16 len)
{
  u8 state=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2);
  srand(drv_get_rtc_counter());
  while(state==0)
  {
    u8 rand_num=(u8)rand();
    delay_os(rand_num);
    state=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2);
  }
  app_uart_cts_set_mode(1);//拉低
  UART_Send(2, buff, len);
  app_uart_cts_set_mode(0);//释放
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void app_uart_recv_check(void)
{
	static UART_Struct *pUART=&g_sUART2; 
	u16 recv_len;

	while(1)
	{
		if(pUART->iRecv>0)
		{
			recv_len=0;
			while(recv_len<pUART->iRecv)
			{
				recv_len=pUART->iRecv;
				delay_ms(5);
			}
      nwk_slave_uart_parse(pUART->pBuff, pUART->iRecv);
      UART_Clear(pUART);
    }
  }    
}

/*		
================================================================================
描述 :应用层sx1278初始化,注册
输入 : 
输出 : 
================================================================================
*/
static void app_sx1278_hal_init(void)
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
	g_sDrvSx1278.tag_hal_sx1278.sx1278_reset = app_sx1278_reset;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_cs_0 = app_sx1278_cs0;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_cs_1 = app_sx1278_cs1;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_spi_rw_byte = app_sx1278_spi_rw_byte;
	drv_sx1278_init(&g_sDrvSx1278);//初始化
	
	nwk_slave_set_lora_dev(&g_sDrvSx1278);
	printf("app_sx1278_hal_init ok!\n");
#endif

#ifdef  LORA_SX1268 
	g_sDrvSx1268.tag_hal_sx1268.sx1268_reset = app_sx1278_reset;
	g_sDrvSx1268.tag_hal_sx1268.sx1268_cs_0 = app_sx1278_cs0;
	g_sDrvSx1268.tag_hal_sx1268.sx1268_cs_1 = app_sx1278_cs1;
	g_sDrvSx1268.tag_hal_sx1268.sx1268_spi_rw_byte = app_sx1278_spi_rw_byte;
  g_sDrvSx1268.tag_hal_sx1268.delay_ms=delay_ms;
	drv_sx1268_init(&g_sDrvSx1268);//初始化
	 
  nwk_slave_set_lora_dev(&g_sDrvSx1268); 
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
void app_sx1278_init(void)
{
	app_sx1278_hal_init();

}

/*		
================================================================================
描述 : sx1278线程
输入 : 
输出 : 
================================================================================
*/
void app_sx1278_thread_entry(void *parameter) 
{
  static u32 run_cnts=0;
  static bool led_state=false;
  app_sx1278_init();
  app_led_init();
  app_uart_init();
  nwk_slave_uart_send_register(app_uart_slave_send);
  while(1)
  {
    app_uart_recv_check();//串口接收检查
    nwk_slave_main(); 
    if(run_cnts++%100==0)//指示灯运行
    {
      led_state=!led_state;
      app_led_set_green(led_state);
    } 

    delay_os(10);
  }
}



/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/













