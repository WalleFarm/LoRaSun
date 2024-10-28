
#ifndef __DRV_SX1278_H__
#define __DRV_SX1278_H__

#include "sx1278_reg.h"
#include "hal_sx1278.h"

#define RSSI_OFFSET_LF             -164
#define RSSI_OFFSET_HF             -157 //计算接收强度

#define LORA_VERSION                  0x12  //器件版本号


//参数配置表
#define LR_SET_RF_FREQ              434000000      // 基本频率
#define LR_SET_RF_PWR               20              // 发射功率
#define LR_SET_RF_BW                6   // 带宽 SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                                        // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
                                        
#define LR_SET_SF                   12  // 扩频因子 SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
#define LR_SET_CR                   1   // 编码率 ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LR_SET_CRC_ON               true  // CRC校验开关 CrcOn [0: OFF, 1: ON]
#define LR_SET_IM_ON                false  //显示报文头开关  ImplicitHeaderOn [0: OFF, 1: ON]
#define LR_SET_PAY_LEN              25  //负载长度  PayloadLength (used for implicit header mode)


typedef struct
{
	HalSx1278Struct tag_hal_sx1278;
	
}DrvSx1278Struct;




void drv_sx1278_init(DrvSx1278Struct *psx1278);
void drv_sx1278_set_default_param(DrvSx1278Struct *psx1278 );
void drv_sx1278_set_op_mode(DrvSx1278Struct *psx1278, u8 opMode);
void drv_sx1278_set_on(DrvSx1278Struct *psx1278);
void drv_sx1278_set_rf_freq( DrvSx1278Struct *psx1278, uint32_t freq );
uint32_t drv_sx1278_get_rf_freq( DrvSx1278Struct *psx1278 );
void drv_sx1278_set_sf( DrvSx1278Struct *psx1278, uint8_t factor );
int16_t drv_sx1278_read_rssi( DrvSx1278Struct *psx1278 );
int8_t drv_sx1278_read_snr( DrvSx1278Struct *psx1278 );
void drv_sx1278_set_rf_power( DrvSx1278Struct *psx1278, int8_t power );
void drv_sx1278_set_bw( DrvSx1278Struct *psx1278, uint8_t bw );
void drv_sx1278_set_error_coding( DrvSx1278Struct *psx1278, uint8_t value );
void drv_sx1278_set_packet_crc_on( DrvSx1278Struct *psx1278, bool enable );
void drv_sx1278_set_preamble_length( DrvSx1278Struct *psx1278, uint16_t value );
void drv_sx1278_set_implicit_header_on( DrvSx1278Struct *psx1278, bool enable );
void drv_sx1278_set_hop_period( DrvSx1278Struct *psx1278, uint8_t value );
void drv_sx1278_set_payload_length( DrvSx1278Struct *psx1278, uint8_t value );
void drv_sx1278_set_pa20dBm( DrvSx1278Struct *psx1278, bool enale );
void drv_sx1278_set_pa_output( DrvSx1278Struct *psx1278, uint8_t outputPin );
void drv_sx1278_set_symb_timeout( DrvSx1278Struct *psx1278, uint16_t value );
void drv_sx1278_set_lowdatarateoptimize( DrvSx1278Struct *psx1278, bool enable );
void drv_sx1278_set_nb_trig_peaks( DrvSx1278Struct *psx1278, uint8_t value );
uint8_t drv_sx1278_get_version( DrvSx1278Struct *psx1278 );

void drv_sx1278_recv_init(DrvSx1278Struct *psx1278);
void drv_sx1278_send(DrvSx1278Struct *psx1278, u8 *buff, u16 len);
void drv_sx1278_cad_init(DrvSx1278Struct *psx1278);

u8 drv_sx1278_recv_check(DrvSx1278Struct *psx1278, u8 *buff);
u8 drv_sx1278_send_check(DrvSx1278Struct *psx1278);
u8 drv_sx1278_cad_check(DrvSx1278Struct *psx1278);

u32 drv_sx1278_calcu_air_time(u8 sf, u8 bw, u16 data_len);

#endif





















