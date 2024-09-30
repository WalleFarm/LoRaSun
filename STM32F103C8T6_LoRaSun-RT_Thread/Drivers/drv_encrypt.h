
#ifndef __DRV_ENCRYPT_H__
#define __DRV_ENCRYPT_H__


#include "mbedtls/aes.h"
#include "drv_common.h"


int aes_encrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);
int aes_decrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);

u16 tea_encrypt_buff(u8 *buff, u16 len, u32* key);
u16 tea_decrypt_buff(u8 *buff, u16 len, u32* key);















#endif
