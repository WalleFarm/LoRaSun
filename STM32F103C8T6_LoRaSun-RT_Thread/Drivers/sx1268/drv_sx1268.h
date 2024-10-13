#ifndef __DRV_SX1268_H__
#define __DRV_SX1268_H__

#include "sx1268_reg.h"
#include "hal_sx1268.h"


#define SX1268_XTAL_FREQ              32000000UL
#define SX1268_PLL_STEP_SHIFT_AMOUNT   ( 14 )
#define SX1268_PLL_STEP_SCALED        ( SX1268_XTAL_FREQ >> ( 25 - SX1268_PLL_STEP_SHIFT_AMOUNT ) )
#define SX1268_MAX_LORA_SYMB_NUM_TIMEOUT       248


typedef struct
{
	HalSx1268Struct tag_hal_sx1268;
	
}DrvSx1268Struct;

typedef struct
{
  int8_t RssiPkt;                                //!< The RSSI of the last packet
  int8_t SnrPkt;                                 //!< The SNR of the last packet
  int8_t SignalRssiPkt;
}Sx1268PacketStatusStruct;



void drv_sx1268_init(DrvSx1268Struct *psx1268);
void drv_sx1268_set_standby(DrvSx1268Struct *psx1268, u8 standby_mode);
void drv_sx1268_set_sleep(DrvSx1268Struct *psx1268);
void drv_sx1268_cad_init(DrvSx1268Struct *psx1268);
void drv_sx1268_set_mode(DrvSx1268Struct *psx1268, u8 mode);
void drv_sx1268_set_freq(DrvSx1268Struct *psx1268, u32 freq);
void drv_sx1268_set_sf_bw(DrvSx1268Struct *psx1268, u8 sf, u8 bw);
void drv_sx1268_set_pack_params(DrvSx1268Struct *psx1268);
void drv_sx1268_set_dio2_switchctrl(DrvSx1268Struct *psx1268, u8 enable);
void drv_sx1268_set_regulator_mode(DrvSx1268Struct *psx1268, u8 mode);
void drv_sx1268_calibrate_image(DrvSx1268Struct *psx1268, u32 freq);
void drv_sx1268_set_pa(DrvSx1268Struct *psx1268, u8 level);
void drv_sx1268_set_power(DrvSx1268Struct *psx1268, int8_t power);
void drv_sx1268_set_irq_parsms(DrvSx1268Struct *psx1268, uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask);
u16 drv_sx1268_get_irq_status(DrvSx1268Struct *psx1268);
void drv_sx1268_clr_irq_status(DrvSx1268Struct *psx1268, u16 irq);
u32 drv_sx1268_convert_freqinhz2pllstep( u32 freqInHz);
void drv_sx1268_set_cad_params(DrvSx1268Struct *psx1268, u8 cadSymbolNum, u8 cadDetPeak, u8 cadDetMin, u8 cadExitMode, u32 cadTimeout);
void drv_sx1268_set_buff_base_addr(DrvSx1268Struct *psx1268, u8 tx_addr, u8 rx_addr);
void drv_sx1268_set_symbnum_timeout(DrvSx1268Struct *psx1268, u8 symbNum);
int8_t drv_sx1268_get_rssi_inst(DrvSx1268Struct *psx1268);
void drv_sx1268_get_pack_status(DrvSx1268Struct *psx1268, Sx1268PacketStatusStruct *pPacketStatus);
void drv_sx1268_get_rx_buff_status(DrvSx1268Struct *psx1268, uint8_t *payloadLength, uint8_t *rxStartBufferPointer);

void drv_sx1268_recv_init(DrvSx1268Struct *psx1268);
void drv_sx1268_send(DrvSx1268Struct *psx1268, u8 *buff, u16 len);
u8 drv_sx1268_recv_check(DrvSx1268Struct *psx1268, u8 *buff, u16 size);
u8 drv_sx1268_send_check(DrvSx1268Struct *psx1268);
u8 drv_sx1268_cad_check(DrvSx1268Struct *psx1268);

u32 drv_sx1268_calcu_air_time(u8 sf, u8 bw, u16 data_len);




#endif

