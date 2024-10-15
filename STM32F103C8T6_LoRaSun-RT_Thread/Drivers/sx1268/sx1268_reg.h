#ifndef __SX1268_REG_H__
#define __SX1268_REG_H__

#define REG_SX1268_SYNCH_TIMEOUT                        0x0706

typedef enum
{
    SX1268_GET_STATUS                        = 0xC0,
    SX1268_WRITE_REGISTER                    = 0x0D,
    SX1268_READ_REGISTER                     = 0x1D,
    SX1268_WRITE_BUFFER                      = 0x0E,
    SX1268_READ_BUFFER                       = 0x1E,
    SX1268_SET_SLEEP                         = 0x84,
    SX1268_SET_STANDBY                       = 0x80,
    SX1268_SET_FS                            = 0xC1,
    SX1268_SET_TX                            = 0x83,
    SX1268_SET_RX                            = 0x82,
    SX1268_SET_RXDUTYCYCLE                   = 0x94,
    SX1268_SET_CAD                           = 0xC5,
    SX1268_SET_TXCONTINUOUSWAVE              = 0xD1,
    SX1268_SET_TXCONTINUOUSPREAMBLE          = 0xD2,
    SX1268_SET_PACKETTYPE                    = 0x8A,
    SX1268_GET_PACKETTYPE                    = 0x11,
    SX1268_SET_RFFREQUENCY                   = 0x86,
    SX1268_SET_TXPARAMS                      = 0x8E,
    SX1268_SET_PACONFIG                      = 0x95,
    SX1268_SET_CADPARAMS                     = 0x88,
    SX1268_SET_BUFFERBASEADDRESS             = 0x8F,
    SX1268_SET_MODULATIONPARAMS              = 0x8B,
    SX1268_SET_PACKETPARAMS                  = 0x8C,
    SX1268_GET_RXBUFFERSTATUS                = 0x13,
    SX1268_GET_PACKETSTATUS                  = 0x14,
    SX1268_GET_RSSIINST                      = 0x15,
    SX1268_GET_STATS                         = 0x10,
    SX1268_RESET_STATS                       = 0x00,
    SX1268_CFG_DIOIRQ                        = 0x08,
    SX1268_GET_IRQSTATUS                     = 0x12,
    SX1268_CLR_IRQSTATUS                     = 0x02,
    SX1268_CALIBRATE                         = 0x89,
    SX1268_CALIBRATEIMAGE                    = 0x98,
    SX1268_SET_REGULATORMODE                 = 0x96,
    SX1268_GET_ERROR                         = 0x17,
    SX1268_CLR_ERROR                         = 0x07,
    SX1268_SET_TCXOMODE                      = 0x97,
    SX1268_SET_TXFALLBACKMODE                = 0x93,
    SX1268_SET_RFSWITCHMODE                  = 0x9D,
    SX1268_SET_STOPRXTIMERONPREAMBLE         = 0x9F,
    SX1268_SET_LORASYMBTIMEOUT               = 0xA0,
}Sx1268CmdType;//SX1268命令

typedef enum
{
    SX1268_MODEM_FSK = 0,
    SX1268_MODEM_LORA,
}Sx1268ModemType;

typedef enum
{
    SX1268_STDBY_RC                                = 0x00,
    SX1268_STDBY_XOSC                              = 0x01,
}Sx1268StandbyMode;

typedef enum
{
    SX1268_CR_4_5                             = 0x01,
    SX1268_CR_4_6                             = 0x02,
    SX1268_CR_4_7                             = 0x03,
    SX1268_CR_4_8                             = 0x04,
}Sx1268CodingRate;

typedef enum
{
    SX1268_CAD_01_SYMBOL                      = 0x00,
    SX1268_CAD_02_SYMBOL                      = 0x01,
    SX1268_CAD_04_SYMBOL                      = 0x02,
    SX1268_CAD_08_SYMBOL                      = 0x03,
    SX1268_CAD_16_SYMBOL                      = 0x04,
}Sx1278CadSymbols;//CAD检测符号数

typedef enum
{
    SX1268_CAD_ONLY                           = 0x00,
    SX1268_CAD_RX                             = 0x01,
    SX1268_CAD_LBT                            = 0x10,
}Sx1268CadExitModes;//CAD退出模式

typedef enum
{
    SX1268_RAMP_10_US                        = 0x00,
    SX1268_RAMP_20_US                        = 0x01,
    SX1268_RAMP_40_US                        = 0x02,
    SX1268_RAMP_80_US                        = 0x03,
    SX1268_RAMP_200_US                       = 0x04,
    SX1268_RAMP_800_US                       = 0x05,
    SX1268_RAMP_1700_US                      = 0x06,
    SX1268_RAMP_3400_US                      = 0x07,
}Sx1268RampTimes;//PA爬坡时间

typedef enum
{
    SX1268_IRQ_RADIO_NONE                          = 0x0000,
    SX1268_IRQ_TX_DONE                             = 0x0001,
    SX1268_IRQ_RX_DONE                             = 0x0002,
    SX1268_IRQ_PREAMBLE_DETECTED                   = 0x0004,
    SX1268_IRQ_SYNCWORD_VALID                      = 0x0008,
    SX1268_IRQ_HEADER_VALID                        = 0x0010,
    SX1268_IRQ_HEADER_ERROR                        = 0x0020,
    SX1268_IRQ_CRC_ERROR                           = 0x0040,
    SX1268_IRQ_CAD_DONE                            = 0x0080,
    SX1268_IRQ_CAD_ACTIVITY_DETECTED               = 0x0100,
    SX1268_IRQ_RX_TX_TIMEOUT                       = 0x0200,
    SX1268_IRQ_RADIO_ALL                           = 0xFFFF,
}Sx1268IrqMasks;

typedef enum
{
    SX1268_USE_LDO                                 = 0x00, // default
    SX1268_USE_DCDC                                = 0x01,
}Sx1268RegulatorMode;




#endif
