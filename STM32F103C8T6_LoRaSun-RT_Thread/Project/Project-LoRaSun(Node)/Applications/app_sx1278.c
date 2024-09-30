
#include "app_sx1278.h"  
DrvSx1278Struct g_sDrvSx1278={0};

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
	
	g_sDrvSx1278.tag_hal_sx1278.sx1278_reset = app_sx1278_reset;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_cs_0 = app_sx1278_cs0;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_cs_1 = app_sx1278_cs1;
	g_sDrvSx1278.tag_hal_sx1278.sx1278_spi_rw_byte = app_sx1278_spi_rw_byte;
	drv_sx1278_init(&g_sDrvSx1278);//初始化
	
	nwk_set_lora_dev(&g_sDrvSx1278);
	printf("app_sx1278_hal_init ok!\n");
	
  
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

  bsp_sx1278_set_param(434000000, 12, 9);//无线参数初始化
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
  app_sx1278_init();

  while(1)
  {
    nwk_node_main();
    NwkNodeRecvFromStruct *recv_from=nwk_node_recv_from_check();
    if(recv_from)
    {
      printf_hex("recv=", recv_from->app_data, recv_from->data_len);
    }

    delay_os(10);
  }
}

/*		
================================================================================
描述 : 设置无线参数
输入 : 
输出 : 
================================================================================
*/
void bsp_sx1278_set_param(u32 freq, u8 sf, u8 bw)
{
	drv_sx1278_set_rf_freq(&g_sDrvSx1278, freq);
	drv_sx1278_set_sf(&g_sDrvSx1278, sf);
	drv_sx1278_set_bw(&g_sDrvSx1278, bw);  
}



/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/













