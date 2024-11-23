/******************************************************************************
*
* Copyright (c) 2024 艺大师
* 本项目开源文件遵循GPL-v3协议
* 
* 文章专栏地址:https://blog.csdn.net/ypp240124016/category_12834955
* 项目开源地址:https://github.com/WalleFarm/LoRaSun
* 协议栈原理专利:CN110572843A
*
* 测试套件采购地址:https://duandianwulian.taobao.com/
*
* 作者:艺大师
* 博客主页:https://blog.csdn.net/ypp240124016?type=blog
* 交流QQ群:701889554  (资料文件存放)
* 微信公众号:端点物联 (即时接收教程更新通知)
*
* 所有学习资源合集:https://blog.csdn.net/ypp240124016/article/details/143068017
*
* 免责声明:本项目所有资料仅限于学习和交流使用,请勿商用.
*
********************************************************************************/


#ifndef __DRV_ENCRYPT_H__
#define __DRV_ENCRYPT_H__


#include "mbedtls/aes.h"
#include "drv_common.h"


int aes_encrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);
int aes_decrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd);

u16 tea_encrypt_buff(u8 *buff, u16 len, u32* key);
u16 tea_decrypt_buff(u8 *buff, u16 len, u32* key);















#endif
