/******************************************************************************
*
* Copyright (c) 2024 小易
* 本项目开源文件遵循GPL-v3协议
* 
* 文章专栏地址:https://blog.csdn.net/ypp240124016/category_12834955
* github主页:      https://github.com/WalleFarm
* LoRaSun开源地址: https://github.com/WalleFarm/LoRaSun
* M2M-IOT开源地址: https://github.com/WalleFarm/M2M-IOT
* 协议栈原理专利:CN110572843A (一种基于LoRa无线模块CAD模式的嗅探方法及系统)
*
* 测试套件采购地址:https://duandianwulian.taobao.com/
*
* 作者:小易
* 博客主页:https://blog.csdn.net/ypp240124016?type=blog
* 交流QQ群:701889554  (资料文件存放)
* 微信公众号:端点物联 (即时接收教程更新通知)
*
* 所有学习资源合集:https://blog.csdn.net/ypp240124016/article/details/143068017
*
* 免责声明:本项目所有资料仅限于学习和交流使用,请勿商用.
*
********************************************************************************/

#include "drv_encrypt.h"

/*		
================================================================================
描述 :AES-CBC模式加密
输入 : 
输出 : 
================================================================================
*/
int aes_encrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd)
{
  static aes_context AesContext;
  u8 iv[16];
  memset(iv, '0', 16);
  memset( AesContext.ksch, 0, sizeof(AesContext.ksch) );
  aes_set_key( passwd, 16, &AesContext );
  
  u16 loops=in_len/16;
  u16 remain_len=in_len%16;
  if( remain_len > 0  || in_len>out_size)
  {
    return 0;
  }  
  aes_cbc_encrypt(in_buff, out_buff, loops, iv, &AesContext);
  return in_len;
}

/*		
================================================================================
描述 :AES-CBC模式解密
输入 : 
输出 : 
================================================================================
*/
int aes_decrypt_buff(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd)
{
  u8 iv[16];
  memset(iv, '0', 16);  
  static aes_context AesContext;
  memset( AesContext.ksch, 0, sizeof(AesContext.ksch) );
  aes_set_key( passwd, 16, &AesContext );
  
  u16 loops=in_len/16;
  u16 remain_len=in_len%16;
  if( remain_len > 0  || in_len>out_size)
  {
    return 0;
  }  
  aes_cbc_decrypt(in_buff, out_buff, loops, iv, &AesContext);
  return in_len;
}


/*		
================================================================================
描述 :  TEA加密单元
输入 : 
输出 : 
================================================================================
*/
void EncryptTEA(u32 *firstChunk, u32 *secondChunk, u32* key)
{
	u32 y = *firstChunk;
	u32 z = *secondChunk;
	u32 sum = 0;
	u32 delta = 0x9e3779b9;

	for (int i = 0; i < 32; i++)//32轮运算(需要对应下面的解密核心函数的轮数一样)
	{
		sum += delta;
		y += ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
		z += ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
	}

	*firstChunk = y;
	*secondChunk = z;
 }

/*		
================================================================================
描述 : TEA解密单元
输入 : 
输出 : 
================================================================================
*/
void DecryptTEA(u32 *firstChunk, u32 *secondChunk, u32* key)
{
	u32  sum = 0;
	u32  y = *firstChunk;
	u32  z = *secondChunk;
	u32  delta = 0x9e3779b9;

	sum = delta << 5; //32轮运算，所以是2的5次方；16轮运算，所以是2的4次方；8轮运算，所以是2的3次方

	for (int i = 0; i < 32; i++) //32轮运算
	{
		z -= (y << 4) + key[2] ^ y + sum ^ (y >> 5) + key[3];
		y -= (z << 4) + key[0] ^ z + sum ^ (z >> 5) + key[1];
		sum -= delta;
	}

	*firstChunk = y;
	*secondChunk = z;
}


/*		
================================================================================
描述 :TEA数据加密函数
输入 : buff的长度必须是8的整数倍
输出 : 
================================================================================
*/
u16 tea_encrypt_buff(u8 *buff, u16 len, u32* key)
{
	u8 *p = buff; 
	u16 i,counts;

	if(len%8!=0)
	{
		printf("Encrypt buff len err!\n");
		return 0;
	}
	counts=len/8;	
	for(i=0;i<counts;i++)
	{
		EncryptTEA((u32 *)p, (u32 *)(p + 4), key);
		p+=8;
	}
	return len;
}

/*		
================================================================================
描述 : TEA数据解密函数
输入 : buff的长度必须是8的整数倍
输出 : 
================================================================================
*/
u16 tea_decrypt_buff(u8 *buff, u16 len, u32* key)  
{
	u8 *p = buff; 
	u16 i,counts;

	if(len%8!=0)
	{
		printf("Decryp buff len err!\n");
		return 0;
	}	
	counts=len/8;	
	for(i=0;i<counts;i++)
	{
		DecryptTEA((u32 *)p, (u32 *)(p + 4), key);
		p+=8;
	}
	
	return len;
}





