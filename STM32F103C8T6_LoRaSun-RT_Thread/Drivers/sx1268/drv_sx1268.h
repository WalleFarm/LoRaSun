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





void drv_sx1268_set_standby(DrvSx1268Struct *psx1268, u8 standby_mode);
void drv_sx1268_set_sleep(DrvSx1268Struct *psx1268);
void drv_sx1268_cad_init(DrvSx1268Struct *psx1268);
void drv_sx1268_set_mode(DrvSx1268Struct *psx1268, u8 mode);
void drv_sx1268_set_freq(DrvSx1268Struct *psx1268, u32 freq);
void drv_sx1268_set_sf_bw(DrvSx1268Struct *psx1268, u8 sf, u8 bw);
void drv_sx1268_set_pack_params(DrvSx1268Struct *psx1268);
void drv_sx1268_set_dio2_switchctrl(DrvSx1268Struct *psx1268, u8 enable);
void drv_sx1268_calibrate_image(DrvSx1268Struct *psx1268, u32 freq);
void drv_sx1268_set_pa(DrvSx1268Struct *psx1268, u8 level);
u16 drv_sx1268_get_irq_status(DrvSx1268Struct *psx1268);
void drv_sx1268_clr_irq_status(DrvSx1268Struct *psx1268, u16 irq);
u32 drv_sx1268_convert_freqinhz2pllstep( u32 freqInHz);






#endif

