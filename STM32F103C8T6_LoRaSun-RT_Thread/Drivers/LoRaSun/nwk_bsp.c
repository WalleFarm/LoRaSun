
#include "nwk_bsp.h"


static const u8 up_table[][2]={  //SF,BW  无线参数表,可根据自己的需求配置,独立网络中一致即可
  9, 9,  
  10, 9, 
  11, 9,  
  11, 8,  
  11, 7,  
  11, 6,  

};

static const u8 down_table[][2]={  //SF,BW  无线参数表,可根据自己的需求配置,独立网络中一致即可  
  11, 9,   
  11, 8,  
  11, 7,  
  11, 6,  

};

static const u8 static_table4[][2]={  //SF,BW  4天线静态模式参数表
  11, 9,  
  11, 8,  
  11, 7,  
  11, 6,  

};

/*		
================================================================================
描述 : crc16校验
输入 : 
输出 : 
================================================================================
*/ 
u16 nwk_crc16(u8 *puchMsg,u16 usDataLen)
{
  return drv_crc16(puchMsg, usDataLen);
}

/*		
================================================================================
描述 : 在一个长数组中匹配短数组
输入 : 
输出 : 
================================================================================
*/ 
u8 *nwk_find_head(u8 *full_str, u16 full_len, u8 *sub_str, u16 sub_len)
{
	int last_pos=full_len-sub_len+1;
	u16 i;
	
	if(last_pos<0)
	{
		return NULL;
	}
	for(i=0;i<last_pos;i++)
	{
		if(full_str[i]==sub_str[0])//先对比第一个字节
		{
			if( memcmp(&full_str[i], sub_str, sub_len)==0 )
			{
				return &full_str[i];
			}		
		}		
	}
	return NULL;	
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void nwk_delay_ms(u32 dlyms)
{
  delay_ms(dlyms);
}

/*		
================================================================================
描述 : AES-CBC模式加密
输入 : 
输出 : 
================================================================================
*/ 
int nwk_aes_encrypt(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd)
{
  return aes_encrypt_buff(in_buff, in_len, out_buff, out_size, passwd);
}

/*		
================================================================================
描述 : AES-CBC模式解密
输入 : 
输出 : 
================================================================================
*/ 
int nwk_aes_decrypt(u8 *in_buff, u16 in_len,u8 *out_buff, u16 out_size,u8 *passwd)
{
  return aes_decrypt_buff(in_buff, in_len, out_buff, out_size, passwd);
}

/*		
================================================================================
描述 : TEA数据加密函数
输入 : 
输出 : 
================================================================================
*/ 
u16 nwk_tea_encrypt(u8 *buff, u16 len, u32* key)
{
  return tea_encrypt_buff(buff, len, key);
}

/*		
================================================================================
描述 : TEA数据解密函数
输入 : 
输出 : 
================================================================================
*/
u16 nwk_tea_decrypt(u8 *buff, u16 len, u32* key)
{
  return tea_decrypt_buff(buff, len, key);
}

/*		
================================================================================
描述 : 获取RTC时间
输入 : 
输出 : 
================================================================================
*/
u32 nwk_get_rtc_counter(void)
{
  return drv_get_rtc_counter();
}

/*		
================================================================================
描述 : 设置RTC时间
输入 : 
输出 : 
================================================================================
*/
void nwk_set_rtc_counter(u32 time)
{
  drv_set_rtc_counter(time);
}

/*		
================================================================================
描述 : 获取sec时间
输入 : 
输出 : 
================================================================================
*/
u32 nwk_get_sec_counter(void)
{
	return drv_get_sec_counter();
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/


int nwk_get_rand(void)
{
  static u32 srand_num=0;
  if(srand_num==0)
  {
    srand_num=nwk_get_rtc_counter();
    srand(srand_num);
  }
  
  int rand_num=rand();
//  srand_num=rand_num;
  return rand_num;
}


/*		
================================================================================
描述 : 上行参数表
输入 : 
输出 : 
================================================================================
*/
void nwk_get_up_channel(u8 chn, u8 *sf, u8 *bw)
{
  *sf=*bw=0;
  if(chn<NWK_UP_CHANNEL_NUM) 
  {
    *sf=up_table[chn][0];
    *bw=up_table[chn][1];
  }
}

/*		
================================================================================
描述 : 下行参数表(动态)
输入 : 
输出 : 
================================================================================
*/
void nwk_get_down_channel(u8 chn, u8 *sf, u8 *bw)
{
  *sf=*bw=0;
  if(chn<NWK_DOWN_CHANNEL_NUM) 
  {
    *sf=down_table[chn][0];
    *bw=down_table[chn][1];
  }
}

/*		
================================================================================
描述 : 下行参数表(动态、4天线)
输入 : 
输出 : 
================================================================================
*/
void nwk_get_static_channel4(u8 chn, u8 *sf, u8 *bw)
{
  *sf=*bw=0;
  if(chn<NWK_DOWN_CHANNEL_NUM) 
  {
    *sf=static_table4[chn][0];
    *bw=static_table4[chn][1];
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
u16 nwk_cacul_sniff_time(u8 sf, u8 bw)
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
  tx_time=t_s*1;  
  return tx_time;
}

/*		
================================================================================
描述 : 根据节点SN计算频率
输入 : 
输出 : 
================================================================================
*/
u32 nwk_get_sn_freq(u32 node_sn)
{
  u16 freq_cnts=(NWK_MAX_FREQ-NWK_MIN_FREQ)/500000;//频点数量,0.5M间隔
  u32 freq=(nwk_crc16((u8*)&node_sn, 4)%freq_cnts)*500000+NWK_MIN_FREQ;//根据序列号计算频段    
  return freq;
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










