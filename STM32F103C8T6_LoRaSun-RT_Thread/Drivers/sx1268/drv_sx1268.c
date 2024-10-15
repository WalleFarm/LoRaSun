#include "drv_sx1268.h"
#include "drv_uart.h"



/*		 
================================================================================
描述 : 模块初始化
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_init(DrvSx1268Struct *psx1268)
{
  hal_sx1268_rst(&psx1268->tag_hal_sx1268);

//  drv_sx1268_wakeup(psx1268);
  
  drv_sx1268_set_standby(psx1268, SX1268_STDBY_RC);
  drv_sx1268_set_dio2_switchctrl(psx1268, true);//射频自动切换

  drv_sx1268_set_standby(psx1268, SX1268_STDBY_RC);
  drv_sx1268_set_regulator_mode(psx1268, SX1268_USE_DCDC);  
  drv_sx1268_set_buff_base_addr(psx1268, 0, 0);//FIFO基地址初始化
  
  drv_sx1268_set_irq_parsms(psx1268, 
                           SX1268_IRQ_RADIO_ALL, 
                           SX1268_IRQ_RADIO_ALL, 
                           SX1268_IRQ_RADIO_NONE,
                           SX1268_IRQ_RADIO_NONE );  
  u32 freq=475000000;
  u8 sf=12, bw=9;
  drv_sx1268_calibrate_image(psx1268, freq);//470M频段校准
  drv_sx1268_set_freq(psx1268, freq);
  
  drv_sx1268_set_standby(psx1268, SX1268_STDBY_RC);
  drv_sx1268_set_mode(psx1268, SX1268_MODEM_LORA);

  drv_sx1268_set_sf_bw(psx1268, sf, bw);
  drv_sx1268_set_pack_params(psx1268, 25);//包格式初始化,前导码等  
//  drv_sx1268_set_cad_params(psx1268, SX1268_CAD_08_SYMBOL, 24, 20, SX1268_CAD_ONLY, 100);  
  
  u8 reg=hal_sx1268_read_reg(&psx1268->tag_hal_sx1268, 0x0889);
  hal_sx1268_write_reg(&psx1268->tag_hal_sx1268, 0x0889, reg | ( 1 << 2 ));
  drv_sx1268_set_power(psx1268, 22);//22dbm输出  
  
  u8 status=hal_sx1268_read_cmd(&psx1268->tag_hal_sx1268, SX1268_GET_STATUS, NULL, 0);
    
  printf("status=0x%0X\n", status);

  
}

/*		
================================================================================
描述 : 唤醒
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_wakeup(DrvSx1268Struct *psx1268)
{
	psx1268->tag_hal_sx1268.sx1268_cs_0();//选中	  
	hal_sx1268_read_cmd(&psx1268->tag_hal_sx1268, SX1268_GET_STATUS, NULL, 0);
	hal_sx1268_spi_rw_byte(&psx1268->tag_hal_sx1268, 0);
  delay_ms(10);
	psx1268->tag_hal_sx1268.sx1268_cs_1();//取消选择	  
}

/*		
================================================================================
描述 : 设置待机模式下振荡器类型,
输入 : RC或者XOSC
输出 : 
================================================================================
*/
void drv_sx1268_set_standby(DrvSx1268Struct *psx1268, u8 standby_mode)
{
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_STANDBY, (u8*)&standby_mode, 1);
}


/*		
================================================================================
描述 : 进入休眠模式
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_sleep(DrvSx1268Struct *psx1268)
{
  u8 value=0;
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_SLEEP, (u8*)&value, 1);
}

/*		
================================================================================
描述 : 设置运行模式, FSK或LORA
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_mode(DrvSx1268Struct *psx1268, u8 mode)
{
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_PACKETTYPE, (u8*)&mode, 1);  
}

/*		
================================================================================
描述 : 设置载波频率
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_freq(DrvSx1268Struct *psx1268, u32 freq)
{
  u8 buff[4]={0};  
  u32 freqInPllSteps = 0;
//  drv_sx1268_calibrate_image(psx1268, freq);
  freqInPllSteps = drv_sx1268_convert_freqinhz2pllstep( freq );
//  printf("in freq=%uHz, freqInPllSteps=%uHz\n", freq, freqInPllSteps);
  buff[0]=freqInPllSteps>>24;
  buff[1]=freqInPllSteps>>16;
  buff[2]=freqInPllSteps>>8;
  buff[3]=freqInPllSteps;
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_RFFREQUENCY, buff, 4);  
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_sf_bw(DrvSx1268Struct *psx1268, u8 sf, u8 bw)
{
  u8 buff[4]={0};
  buff[0]=sf;
  buff[1]=bw-3;//为了与SX1278保持一致,SX1268: 3~4~5~6
  buff[2]=SX1268_CR_4_5;//编码率
  buff[3]=true;//低速率优化
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_MODULATIONPARAMS, buff, 4); 
  
}

/*		
================================================================================
描述 : 设置数据包格式参数
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_pack_params(DrvSx1268Struct *psx1268, u8 payload_len)
{
  u8 buff[10]={0};
  u16 PreambleLength=12;//前导码长度
  buff[0]=PreambleLength>>8;
  buff[1]=PreambleLength;
  buff[2]=0;//可变长度数据包（显式报头）
  buff[3]=payload_len;//负载长度
  buff[4]=0x01;//CRC ON
  buff[5]=0;//标准IQ极性
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_PACKETPARAMS, buff, 6); 

  u8 reg_val=hal_sx1268_read_reg(&psx1268->tag_hal_sx1268, 0x0736);
  hal_sx1268_write_reg(&psx1268->tag_hal_sx1268, 0x0736, reg_val | ( 1 << 2 ) );//使用标准IQ极性是第2位置1  
}

/*		
================================================================================
描述 : DIO2设置为射频自动切换开关
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_dio2_switchctrl(DrvSx1268Struct *psx1268, u8 enable)
{
  u8 buff[1]={0};
  buff[0]=enable;
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_RFSWITCHMODE, buff, 1); 
}

/*		
================================================================================
描述 : 电源模式
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_regulator_mode(DrvSx1268Struct *psx1268, u8 mode)
{
  u8 buff[1]={0};
  buff[0]=mode;
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_REGULATORMODE, buff, 1); 
}

/*		
================================================================================
描述 : 频段校准
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_calibrate_image(DrvSx1268Struct *psx1268, u32 freq)
{
    uint8_t calFreq[2];

    if( freq > 900000000 )
    {
        calFreq[0] = 0xE1;
        calFreq[1] = 0xE9;
    }
    else if( freq > 850000000 )
    {
        calFreq[0] = 0xD7;
        calFreq[1] = 0xDB;
    }
    else if( freq > 770000000 )
    {
        calFreq[0] = 0xC1;
        calFreq[1] = 0xC5;
    }
    else if( freq > 460000000 )
    {
        calFreq[0] = 0x75;
        calFreq[1] = 0x81;
    }
    else if( freq > 425000000 )
    {
        calFreq[0] = 0x6B;
        calFreq[1] = 0x6F;
    }
    hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_CALIBRATEIMAGE, calFreq, 2); 
}

/*		
================================================================================
描述 : PA输出等级
输入 : level:0~4对应10-14-17-20-22dbm
输出 : 
================================================================================
*/
void drv_sx1268_set_pa(DrvSx1268Struct *psx1268, u8 level)
{
  u8 pa_buff[5][4]={
    0x00, 0x03, 0x00, 0x01,//10dbm
    0x04, 0x06, 0x00, 0x01,//14dbm
    0x02, 0x03, 0x00, 0x01,//17dbm
    0x03, 0x05, 0x00, 0x01,//20dbm
    0x04, 0x07, 0x00, 0x01,//22dbm
  };
  if(level>4)level=4;
  
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_PACONFIG, &pa_buff[level][0], 4); 
}

/*		
================================================================================
描述 : 设置发送功率
输入 : -9~22dbm
输出 : 
================================================================================
*/
void drv_sx1268_set_power(DrvSx1268Struct *psx1268, int8_t power)
{
  u8 buff[2]={0};
  u8 reg_val=hal_sx1268_read_reg(&psx1268->tag_hal_sx1268, 0x08D8);
  hal_sx1268_write_reg(&psx1268->tag_hal_sx1268, 0x08D8, reg_val | ( 0x0F << 1 ) );
  
  drv_sx1268_set_pa(psx1268, 4);
  if( power > 22 )
  {
    power = 22;
  }
  else if( power < -9 )
  {
    power = -9;
  }  
  buff[0] = power;
  buff[1] = SX1268_RAMP_40_US;  //PA爬坡时间
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_TXPARAMS, buff, 2); 
}

/*		
================================================================================
描述 : 设置中断触发参数
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_irq_parsms(DrvSx1268Struct *psx1268, uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask)
{
    uint8_t buf[8];

    buf[0] = ( uint8_t )( ( irqMask >> 8 ) & 0x00FF );
    buf[1] = ( uint8_t )( irqMask & 0x00FF );
    buf[2] = ( uint8_t )( ( dio1Mask >> 8 ) & 0x00FF );
    buf[3] = ( uint8_t )( dio1Mask & 0x00FF );
    buf[4] = ( uint8_t )( ( dio2Mask >> 8 ) & 0x00FF );
    buf[5] = ( uint8_t )( dio2Mask & 0x00FF );
    buf[6] = ( uint8_t )( ( dio3Mask >> 8 ) & 0x00FF );
    buf[7] = ( uint8_t )( dio3Mask & 0x00FF );
    hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_CFG_DIOIRQ, buf, 8 );  
}

/*		
================================================================================
描述 : 获取中断标志
输入 : 
输出 : 
================================================================================
*/
u16 drv_sx1268_get_irq_status(DrvSx1268Struct *psx1268)
{
  u8 irqStatus[2];

  hal_sx1268_read_cmd(&psx1268->tag_hal_sx1268, SX1268_GET_IRQSTATUS, irqStatus, 2); 
  return ( irqStatus[0] << 8 ) | irqStatus[1];  
}

/*		
================================================================================
描述 : 清理中断标志
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_clr_irq_status(DrvSx1268Struct *psx1268, u16 irq)
{
  u8 buff[2]={0};
  buff[0] = ( uint8_t )( ( ( uint16_t )irq >> 8 ) & 0x00FF );
  buff[1] = ( uint8_t )( ( uint16_t )irq & 0x00FF );  
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_CLR_IRQSTATUS, buff, 2);   
}


/*		
================================================================================
描述 : 频率数值转化为寄存器格式数值
输入 : 
输出 : 
================================================================================
*/
u32 drv_sx1268_convert_freqinhz2pllstep( u32 freqInHz)
{
    uint32_t stepsInt;
    uint32_t stepsFrac;

    // pllSteps = freqInHz / (SX126X_XTAL_FREQ / 2^19 )
    // Get integer and fractional parts of the frequency computed with a PLL step scaled value
    stepsInt = freqInHz / SX1268_PLL_STEP_SCALED;
    stepsFrac = freqInHz - ( stepsInt * SX1268_PLL_STEP_SCALED );
    
    // Apply the scaling factor to retrieve a frequency in Hz (+ ceiling)
    return ( stepsInt << SX1268_PLL_STEP_SHIFT_AMOUNT ) + 
           ( ( ( stepsFrac << SX1268_PLL_STEP_SHIFT_AMOUNT ) + ( SX1268_PLL_STEP_SCALED >> 1 ) ) /
             SX1268_PLL_STEP_SCALED );  
}

/*		
================================================================================
描述 : 设置CAD参数
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_cad_params(DrvSx1268Struct *psx1268, u8 cadSymbolNum, u8 cadDetPeak, u8 cadDetMin, u8 cadExitMode, u32 cadTimeout)
{
  u8 buff[10]={0};
  buff[0] = cadSymbolNum;
  buff[1] = cadDetPeak;  
  buff[2] = cadDetMin;  
  buff[3] = cadExitMode;  
  buff[4] = cadTimeout>>16;  
  buff[5] = cadTimeout>>8;  
  buff[6] = cadTimeout;  
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_CADPARAMS, buff, 7);    
}

/*		
================================================================================
描述 : 设置收发缓存的基地址
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_buff_base_addr(DrvSx1268Struct *psx1268, u8 tx_addr, u8 rx_addr)
{
  u8 buff[2]={0};
  buff[0] = tx_addr;
  buff[1] = rx_addr;  
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_BUFFERBASEADDRESS, buff, 2);   
}

/*		
================================================================================
描述 : 设置验证成功接收的符号数
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_set_symbnum_timeout(DrvSx1268Struct *psx1268, u8 symbNum)
{
  uint8_t mant = ( ( ( symbNum > SX1268_MAX_LORA_SYMB_NUM_TIMEOUT ) ?
                     SX1268_MAX_LORA_SYMB_NUM_TIMEOUT : 
                     symbNum ) + 1 ) >> 1;
  uint8_t exp  = 0;
  uint8_t reg  = 0;

  while( mant > 31 )
  {
    mant = ( mant + 3 ) >> 2;
    exp++;
  }

  reg = mant << ( 2 * exp + 1 );
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_LORASYMBTIMEOUT, &reg, 1 );

  if( symbNum != 0 )
  {
    reg = exp + ( mant << 3 );
    hal_sx1268_write_reg(&psx1268->tag_hal_sx1268, REG_SX1268_SYNCH_TIMEOUT, reg );
  }  
}


/*		
================================================================================
描述 : 获取信号强度值(瞬时)
输入 : 
输出 : 
================================================================================
*/
int8_t drv_sx1268_get_rssi_inst(DrvSx1268Struct *psx1268)
{
  u8 buff[2]={0};
  
  hal_sx1268_read_cmd(&psx1268->tag_hal_sx1268, SX1268_GET_RSSIINST, buff, 1);   
  int8_t rssi = -buff[0] >> 1;
  return rssi;
}

/*		
================================================================================
描述 : 获取数据包信号状态
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_get_pack_status(DrvSx1268Struct *psx1268, Sx1268PacketStatusStruct *pPacketStatus)
{
  uint8_t status[3];
  hal_sx1268_read_cmd(&psx1268->tag_hal_sx1268, SX1268_GET_PACKETSTATUS, status, 3); 
  
  pPacketStatus->RssiPkt = -status[0] >> 1;
  pPacketStatus->SnrPkt = ( ( ( int8_t )status[1] ) + 2 ) >> 2;
  pPacketStatus->SignalRssiPkt = -status[2] >> 1;
}

/*		
================================================================================
描述 : 获取接收数据长度和指针
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_get_rx_buff_status(DrvSx1268Struct *psx1268, uint8_t *payloadLength, uint8_t *rxStartBufferPointer)
{
  uint8_t status[2];

  hal_sx1268_read_cmd(&psx1268->tag_hal_sx1268, SX1268_GET_RXBUFFERSTATUS, status, 2 );
  *payloadLength = status[0];
  *rxStartBufferPointer = status[1];
}

void drv_sx1268_set_stop_rxtimer_onpreamble_detect(DrvSx1268Struct *psx1268, bool enable)
{
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_STOPRXTIMERONPREAMBLE, ( uint8_t* )&enable, 1 );
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_recv_init(DrvSx1268Struct *psx1268)
{
  u8 timeout[3]={0xFF, 0xFF, 0xFF}; 
  drv_sx1268_set_standby(psx1268, SX1268_STDBY_RC);
  drv_sx1268_set_stop_rxtimer_onpreamble_detect(psx1268, false);
  drv_sx1268_set_irq_parsms(psx1268, 
                           SX1268_IRQ_RX_DONE, 
                           SX1268_IRQ_RX_DONE, 
                           SX1268_IRQ_RADIO_NONE,
                           SX1268_IRQ_RADIO_NONE );
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_RX, timeout, 3);//连续接收
  
  u8 reg=hal_sx1268_read_reg(&psx1268->tag_hal_sx1268, 0x0736);
  hal_sx1268_write_reg(&psx1268->tag_hal_sx1268, 0x0736, reg | ( 1 << 2 ));  
}

/*		
================================================================================
描述 : CAD初始化
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_cad_init(DrvSx1268Struct *psx1268)
{
  drv_sx1268_set_standby(psx1268, SX1268_STDBY_RC);
  drv_sx1268_set_irq_parsms(psx1268, 
                           SX1268_IRQ_CAD_DONE | SX1268_IRQ_CAD_ACTIVITY_DETECTED, 
                           SX1268_IRQ_CAD_DONE | SX1268_IRQ_CAD_ACTIVITY_DETECTED, 
                           SX1268_IRQ_RADIO_NONE,
                           SX1268_IRQ_RADIO_NONE );  
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_CAD, NULL, 0);
}

/*		
================================================================================
描述 : 发送数据包
输入 : 
输出 : 
================================================================================
*/
void drv_sx1268_send(DrvSx1268Struct *psx1268, u8 *buff, u16 len)
{
  u8 timeout[3]={0};//不设置超时
  drv_sx1268_set_standby(psx1268, SX1268_STDBY_RC);
  drv_sx1268_set_irq_parsms(psx1268, 
                           SX1268_IRQ_TX_DONE | SX1268_IRQ_RX_TX_TIMEOUT, 
                           SX1268_IRQ_TX_DONE | SX1268_IRQ_RX_TX_TIMEOUT, 
                           SX1268_IRQ_RADIO_NONE,
                           SX1268_IRQ_RADIO_NONE );  
  drv_sx1268_set_pack_params(psx1268, len);//设置负载长度 
  hal_sx1268_write_fifo(&psx1268->tag_hal_sx1268, buff, len);
  
  hal_sx1268_write_cmd(&psx1268->tag_hal_sx1268, SX1268_SET_TX, timeout, 3);//启动发送
  
}


/*		
================================================================================
描述 : 接收检查
输入 : 
输出 : 
================================================================================
*/
u8 drv_sx1268_recv_check(DrvSx1268Struct *psx1268, u8 *buff, u16 size)
{
  u16 irqRegs=drv_sx1268_get_irq_status(psx1268);
  drv_sx1268_clr_irq_status(psx1268, irqRegs);//清除标志
  if( ( irqRegs & SX1268_IRQ_RX_DONE ) == SX1268_IRQ_RX_DONE )
  {
    u8 payload_len=0, offset=0;
    drv_sx1268_get_rx_buff_status(psx1268, &payload_len, &offset);
//    printf("offset=%d, payload_len=%d\n", offset, payload_len);
    if(payload_len>size)
    {
      printf("error payload_len>size!\n");
      return 0;
    }
    hal_sx1268_read_fifo(&psx1268->tag_hal_sx1268, offset, buff, payload_len);
    return payload_len;
  }
  return 0;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
u8 drv_sx1268_send_check(DrvSx1268Struct *psx1268)
{
  u16 irqRegs=drv_sx1268_get_irq_status(psx1268);
  drv_sx1268_clr_irq_status(psx1268, irqRegs);//清除标志
  if( ( irqRegs & SX1268_IRQ_TX_DONE ) == SX1268_IRQ_TX_DONE )
  {
//    printf("sx1268 send ok!\n");
    return 1;
  }    
  return 0;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
u8 drv_sx1268_cad_check(DrvSx1268Struct *psx1268)
{
//  u8 result=0;
  u16 irqRegs=drv_sx1268_get_irq_status(psx1268);
//  printf("cad reg=0x%04X\n", irqRegs);
  drv_sx1268_clr_irq_status(psx1268, irqRegs);//清除标志
  if((irqRegs & SX1268_IRQ_CAD_DONE) == SX1268_IRQ_CAD_DONE)
  {
    if((irqRegs & SX1268_IRQ_CAD_ACTIVITY_DETECTED) == SX1268_IRQ_CAD_ACTIVITY_DETECTED)
    {
      return 2;
    }
    return 1;
  }    
  return 0;
}

/*		
================================================================================
描述 : 根据射频参数计算发送时间
输入 : 
输出 : 发送时间,返回0表示无效,单位：ms
================================================================================
*/ 
u32 drv_sx1268_calcu_air_time(u8 sf, u8 bw, u16 data_len)
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







