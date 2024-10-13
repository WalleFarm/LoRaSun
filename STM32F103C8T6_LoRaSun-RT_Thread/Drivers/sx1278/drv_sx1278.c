
#include "drv_sx1278.h"
 

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_init(DrvSx1278Struct *psx1278)
{
	hal_sx1278_rst(&psx1278->tag_hal_sx1278);//复位 
	drv_sx1278_set_on(psx1278);
	drv_sx1278_set_default_param(psx1278);
	printf("drv_sx1278 version=0x%02X\n", drv_sx1278_get_version(psx1278));
}


/*		
================================================================================
描述 :设置lora的默认参数
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_default_param(DrvSx1278Struct *psx1278 )
{
    hal_sx1278_write_addr( &psx1278->tag_hal_sx1278, REG_LR_LNA, RFLR_LNA_GAIN_G1);

    drv_sx1278_set_rf_freq( psx1278, LR_SET_RF_FREQ );     // 设置载波频率
    drv_sx1278_set_sf( psx1278, LR_SET_SF );      // 设置扩频因子 SF6 only operates in implicit header mode.
    drv_sx1278_set_error_coding( psx1278, LR_SET_CR );          // 设置纠错编码率
    drv_sx1278_set_packet_crc_on( psx1278, LR_SET_CRC_ON );      // CRC开关
    drv_sx1278_set_bw( psx1278, LR_SET_RF_BW );    // 设置带宽
    drv_sx1278_set_implicit_header_on( psx1278, LR_SET_IM_ON);    // 设置显式/隐式报头模式
	
    drv_sx1278_set_symb_timeout( psx1278, 0x3FF );//设置RX超时
    drv_sx1278_set_payload_length(psx1278, LR_SET_PAY_LEN);//负载字节长度
    drv_sx1278_set_lowdatarateoptimize( psx1278, true );       // 低速率优化模式,符号长度超过 16ms时必须打开

    drv_sx1278_set_pa_output( psx1278, RFLR_PACONFIG_PASELECT_PABOOST );//选择PA输出引脚
    drv_sx1278_set_rf_power( psx1278, LR_SET_RF_PWR ); 
    
		hal_sx1278_write_addr( &psx1278->tag_hal_sx1278, REG_LR_OCP, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_OCP)& RFLR_OCP_TRIM_MASK) | RFLR_OCP_TRIM_180_MA );//为PA开启过流保护 180mA        

    drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_STANDBY );//进入待机模式
} 

/*		
================================================================================
描述 :设置模块的工作模式
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_op_mode(DrvSx1278Struct *psx1278, u8 opMode)
{	
	hal_sx1278_write_addr( &psx1278->tag_hal_sx1278, REG_LR_OPMODE, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_OPMODE)& RFLR_OPMODE_MASK) | opMode );
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_on(DrvSx1278Struct *psx1278)
{
	drv_sx1278_set_op_mode(psx1278, RFLR_OPMODE_SLEEP );//进入睡眠模式
	hal_sx1278_write_addr( &psx1278->tag_hal_sx1278, REG_LR_OPMODE, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_ON );//开启Lora模式
	drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_STANDBY );//进入待机模式

	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING1, 0);
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING2, 0);
}

/*		
================================================================================
描述 :设置载波频率
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_rf_freq( DrvSx1278Struct *psx1278, uint32_t freq )
{
	freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FRFMSB, ( freq >> 16 ) & 0xFF);
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FRFMID, ( freq >> 8 ) & 0xFF);
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FRFLSB, ( freq >> 0 ) & 0xFF);
}

/*		
================================================================================
描述 :获取载波频率
输入 : 
输出 : 
================================================================================
*/
uint32_t drv_sx1278_get_rf_freq( DrvSx1278Struct *psx1278 )
{
	uint32_t RFFrequency = 0;
	RFFrequency = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_FRFMSB);
	RFFrequency <<= 8;
	RFFrequency |= hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_FRFMID);
	RFFrequency <<= 8;
	RFFrequency |= hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_FRFLSB);

	RFFrequency *= XTAL_FREQ;
	RFFrequency >>= 4;

	RFFrequency *= 100;
	RFFrequency >>= 6;
	
	RFFrequency *= 100;
	RFFrequency >>= 6;
	
	RFFrequency *= 100;
	RFFrequency >>= 3;

	return RFFrequency;
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_sf( DrvSx1278Struct *psx1278, uint8_t factor )
{
	if( factor > 12 )
	{
		factor = 12;
	}
	else if( factor < 6 )
	{
		factor = 6;
	}

	if( factor == 6 )
	{
		drv_sx1278_set_nb_trig_peaks( psx1278, 5 );
		hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DETECTIONTHRESHOLD, RFLR_DETECTIONTHRESH_SF6);        
	}
	else
	{
		drv_sx1278_set_nb_trig_peaks( psx1278, 3 );
		hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DETECTIONTHRESHOLD, RFLR_DETECTIONTHRESH_SF7_TO_SF12);        
	}

	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG2, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG2)& RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 ) );
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
int16_t drv_sx1278_read_rssi( DrvSx1278Struct *psx1278 )
{
	uint8_t RegPktRssiValue,rxSnrEstimate;
	int8_t RxPacketSnrEstimate;
	int16_t RxPacketRssiValue;

	// Reads the RSSI value
	RegPktRssiValue = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_PKTRSSIVALUE );

	rxSnrEstimate = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_PKTSNRVALUE );
	RxPacketSnrEstimate = ((int8_t)rxSnrEstimate ) >> 2;

	if( drv_sx1278_get_rf_freq(psx1278) <= RF_MID_BAND_THRESH )  // LF
	{
		if(RxPacketSnrEstimate<0)
		{
			RxPacketRssiValue = RSSI_OFFSET_LF + RegPktRssiValue + (RxPacketSnrEstimate>>2);
		}
		else
		{
			RxPacketRssiValue = RSSI_OFFSET_LF + RegPktRssiValue;
		}
				 
	}
	else
	{
		if(RxPacketSnrEstimate<0)
		{
			RxPacketRssiValue = RSSI_OFFSET_HF + RegPktRssiValue + (RxPacketSnrEstimate>>2);
		}
		else
		{
			RxPacketRssiValue = RSSI_OFFSET_HF + RegPktRssiValue;
		}        
	}
	return RxPacketRssiValue;
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_rf_power( DrvSx1278Struct *psx1278, int8_t power )
{
	uint8_t RegPaConfig;
	
	RegPaConfig = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_PACONFIG );
	
	if( ( RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
	{
		if( power < 5 )
		{
				power = 5;
		}
		
		if( power > 20 )
		{
				power = 20;                
		}

		if ( power >= 20 )
		{
				drv_sx1278_set_pa20dBm( psx1278, true );                
				RegPaConfig = (RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
				printf("20dbm\n");
		}
		else
		{
				RegPaConfig = ( RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
		}
		RegPaConfig = (RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
	}
	else
	{
			if( power < -1 )
			{
					power = -1;
			}
			
			if( power > 14 )
			{
					power = 14;
			}
			RegPaConfig = ( RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
			RegPaConfig = ( RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
	}
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_PACONFIG, RegPaConfig );
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_bw( DrvSx1278Struct *psx1278, uint8_t bw )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG1, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG1)& RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 ) );
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_error_coding( DrvSx1278Struct *psx1278, uint8_t value )
{
   hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG1, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG1)& RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 ) );
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_packet_crc_on( DrvSx1278Struct *psx1278, bool enable )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG2, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG2)& RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 ) );
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_preamble_length( DrvSx1278Struct *psx1278, uint16_t value )
{
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_PREAMBLEMSB, ( value >> 8 ) & 0x00FF);
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_PREAMBLELSB, ( value ) & 0x00FF);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_implicit_header_on( DrvSx1278Struct *psx1278, bool enable )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG1, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG1)& RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable) );
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_hop_period( DrvSx1278Struct *psx1278, uint8_t value )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_HOPPERIOD, value );
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_payload_length( DrvSx1278Struct *psx1278, uint8_t value )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_PAYLOADLENGTH, value);
}
 

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_pa20dBm( DrvSx1278Struct *psx1278, bool enale )
{
	uint8_t RegPaConfig;
	uint8_t RegPaDac;

	RegPaConfig = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_PACONFIG );
	RegPaDac = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_PADAC );
 
	if( ( RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
	{    
		if( enale == true )
		{
				RegPaDac = 0x87;
		}
	}
	else
	{
		RegPaDac = 0x84;
	}
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_PADAC, RegPaDac );
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_pa_output( DrvSx1278Struct *psx1278, uint8_t outputPin )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_PACONFIG, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_PACONFIG)& RFLR_PACONFIG_PASELECT_MASK ) | outputPin );
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_symb_timeout( DrvSx1278Struct *psx1278, uint16_t value )
{
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG2, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG2)& RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK )| ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ));
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_SYMBTIMEOUTLSB, value & 0xFF);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_lowdatarateoptimize( DrvSx1278Struct *psx1278, bool enable )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG3, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_MODEMCONFIG3)& RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK )| ( enable << 3 ));
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_set_nb_trig_peaks( DrvSx1278Struct *psx1278, uint8_t value )
{
  hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_LORADETECTOPTIMIZE, (hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_LORADETECTOPTIMIZE)& RFLR_DETECTIONOPTIMIZE_MASK )| ( value ));
}


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
uint8_t drv_sx1278_get_version( DrvSx1278Struct *psx1278 )
{
  return hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_VERSION);
}
 

//********************************************//

/*		
================================================================================
描述 : 接收模式初始化
输入 : 
输出 : 
================================================================================
*/ 
void drv_sx1278_recv_init(DrvSx1278Struct *psx1278) 
{
	u8 RegIrqFlagsMask,RegDioMapping1,RegDioMapping2;
	
	drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_STANDBY );
	RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
								//RFLR_IRQFLAGS_RXDONE |
								//RFLR_IRQFLAGS_PAYLOADCRCERROR |
								RFLR_IRQFLAGS_VALIDHEADER |
								RFLR_IRQFLAGS_TXDONE |
								RFLR_IRQFLAGS_CADDONE |
								//RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
								RFLR_IRQFLAGS_CADDETECTED;
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGSMASK, RegIrqFlagsMask );

	RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
								// CadDetected               ModeReady
	RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING1, RegDioMapping1);
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING2, RegDioMapping2);
	
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFORXBASEADDR, 0x00 );
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFOADDRPTR, 0x00 );   //指定接收基地址         
	drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_RECEIVER );//连续接收	
}

/*		
================================================================================
描述 :用户层Lora数据发送 
输入 : 
输出 : 
================================================================================
*/
void drv_sx1278_send(DrvSx1278Struct *psx1278, u8 *buff, u16 len)
{
	u8 RegIrqFlagsMask=0,RegFifoTxBaseAddr=0,RegDioMapping1=0,RegDioMapping2=0;
	drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_STANDBY );//待机模式
	
//	printf("send start!\n");
	RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
								RFLR_IRQFLAGS_RXDONE |
								RFLR_IRQFLAGS_PAYLOADCRCERROR |
								RFLR_IRQFLAGS_VALIDHEADER |
								//RFLR_IRQFLAGS_TXDONE |
								RFLR_IRQFLAGS_CADDONE |
								RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
								RFLR_IRQFLAGS_CADDETECTED;//置位表示该位中断屏蔽

	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGSMASK, RegIrqFlagsMask );

	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_PAYLOADLENGTH, len);//设置数据长度
	
	RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFOTXBASEADDR, RegFifoTxBaseAddr );//设置待发送数据 写入的RAM起始地址，芯片内部有256字节RAM，要跟FIFO区别
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFOADDRPTR, RegFifoTxBaseAddr);
		   
	hal_sx1278_write_fifo(&psx1278->tag_hal_sx1278, (uint8_t*)buff, len);//向FIFO写入数据
		
									// TxDone               RxTimeout                   FhssChangeChannel          ValidHeader         
	RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;//将DIO_0配置成发送完成中断
									// PllLock              Mode Ready
	RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING1, RegDioMapping1 );//写入配置
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING2, RegDioMapping2 );

	drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_TRANSMITTER );//设置成发送模式
	
}

/*		
================================================================================
描述 : CAD模式初始化
输入 : 
输出 : 
================================================================================
*/ 
void drv_sx1278_cad_init(DrvSx1278Struct *psx1278) 
{
	u8 RegIrqFlagsMask,RegDioMapping1,RegDioMapping2;
	
	drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_STANDBY );
	RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
								RFLR_IRQFLAGS_RXDONE |
								RFLR_IRQFLAGS_PAYLOADCRCERROR |
								RFLR_IRQFLAGS_VALIDHEADER |
								RFLR_IRQFLAGS_TXDONE |
								//RFLR_IRQFLAGS_CADDONE |
								RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL;
								//RFLR_IRQFLAGS_CADDETECTED;
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGSMASK, RegIrqFlagsMask );

	RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
								// CadDetected               ModeReady
	RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING1, RegDioMapping1);
	hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_DIOMAPPING2, RegDioMapping2);
	       
	drv_sx1278_set_op_mode( psx1278, RFLR_OPMODE_CAD );//CAD模式	
}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
u8 drv_sx1278_recv_check(DrvSx1278Struct *psx1278, u8 *buff)
{
	u8 RegFifoRxCurrentAddr;
	u8 RxPacketSize=0; 
  

	if(( hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_RXDONE ) == RFLR_IRQFLAGS_RXDONE)		
	{
		hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE );//一次写操作清除 IRQ	
		RegFifoRxCurrentAddr = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFORXCURRENTADDR );//接收到最后一个数据包的起始地址（256B的数据缓冲区中）		
		RxPacketSize = hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_NBRXBYTES);//读取 接收到的数据字节数
		hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFOADDRPTR, RegFifoRxCurrentAddr);//设置要读取的起始地址
		hal_sx1278_read_fifo( &psx1278->tag_hal_sx1278, buff, RxPacketSize );//读取数据
		hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFORXBASEADDR, 0x00 );
		hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_FIFOADDRPTR, 0x00 );   //指定接收基地址 
	}
	return RxPacketSize;
}

/*		
================================================================================
描述 : CAD结果检测 
输入 : 
输出 : 
================================================================================
*/ 
u8 drv_sx1278_cad_check(DrvSx1278Struct *psx1278)
{
  u8 result=0;
  if(( hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_CADDONE ) == RFLR_IRQFLAGS_CADDONE)//CAD检测完成
  {
//    printf("cad done!\n");
    hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE );//一次写操作清除 IRQ	
    if(( hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_CADDETECTED ) == RFLR_IRQFLAGS_CADDETECTED)//信道检测结果
    {
      hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED );//一次写操作清除 IRQ
//      printf("cad success!\n");
      result=2;
    }
    else
    {
//      printf("cad failed!\n");
      result=1;
    }
  }

	return result;//返回,如果还需要CAD检测,则需要重新调用drv_sx1278_cad_init()初始化
}

/*		
================================================================================
描述 : 发送完成检测
输入 : 
输出 : 
================================================================================
*/ 
u8 drv_sx1278_send_check(DrvSx1278Struct *psx1278)
{

	if(( hal_sx1278_read_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_TXDONE ) == RFLR_IRQFLAGS_TXDONE)		
	{
		hal_sx1278_write_addr(&psx1278->tag_hal_sx1278, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE );//一次写操作清除 IRQ	
		return true;
	}
	return false;
}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
//static u32 bsp_pow2(u8 n)
//{
////    u8 i;
//    u32 sum=1;
////    for(i=0;i<n;i++)
////        sum=sum*2;
//    return sum<<n;
//}


/*		
================================================================================
描述 : 根据射频参数计算发送时间
输入 : 
输出 : 发送时间,返回0表示无效,单位：ms
================================================================================
*/ 
u32 drv_sx1278_calcu_air_time(u8 sf, u8 bw, u16 data_len)
{
	float bw_value=0.f, t_s;
	u32 tx_time=0;
	
	switch(bw)
	{
		case 0:
			bw_value=7.8;
			break;
		case 1:
			bw_value=10.4;
			break;
		case 2:
			bw_value=15.6;
			break;
		case 3:
			bw_value=20.8;
			break;
		case 4:
			bw_value=31.25;
			break;
		case 5:
			bw_value=41.6;
			break;
		case 6:
			bw_value=62.5;
			break;
		case 7:
			bw_value=125;
			break;
		case 8:
			bw_value=250;
			break;
		case 9:
			bw_value=500;
			break;
		default: return 0;	
	}
	
	if(sf<7 || sf>12)
	{
		return 0;
	}
	t_s=1.f*(1<<sf)/bw_value;
	
	int payload_nb=0;
	int k1=8*data_len-4*sf+24;
	int k2=4*(sf-2);
	
	payload_nb=k1/k2;
	if(payload_nb<0)
	{
		payload_nb=0;
	}
	else if(k1%k2>0)
	{
		payload_nb++;
	}
	
	payload_nb=payload_nb*5+8;
		
	tx_time=(u32)(payload_nb+12.5)*t_s;
	if(tx_time==0)
	{
		tx_time=1;
	}
	return tx_time;
}
















