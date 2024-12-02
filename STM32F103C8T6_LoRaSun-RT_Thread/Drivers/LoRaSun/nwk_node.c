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



#include "nwk_node.h" 

NwkNodeWorkStruct g_sNwkNodeWork={0};


extern void printf_oled(char const *const format, ...);//OLED打印输出,可以注释
/*		
================================================================================
描述 : 节点SN存储
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_set_sn(u32 node_sn)
{
	g_sNwkNodeWork.node_sn=node_sn;
}

/*		
================================================================================
描述 : 配置根密码
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_set_root_key(u8 *key)
{
  memcpy(g_sNwkNodeWork.root_key, key, 16);
}

/*		
================================================================================
描述 : 添加网关
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_add_gw(u32 gw_sn, u8 base_freq, u8 wireless_num, u8 run_mode)
{
  for(u8 i=0; i<NWK_GW_NUM; i++) 
  {
    NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
    if(pGateWay->gw_sn==0)
    {
      memset(pGateWay, 0, sizeof(NwkParentWorkStrcut));
			pGateWay->gw_sn=gw_sn;
			pGateWay->base_freq_ptr=base_freq;
			pGateWay->wireless_num=wireless_num;
      pGateWay->run_mode=run_mode;
      break;
    }
  }  	
}

/*		
================================================================================
描述 : 删除网关
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_del_gw(u32 gw_sn)
{
  for(u8 i=0; i<NWK_GW_NUM; i++)
  {
    NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
    if(pGateWay->gw_sn==gw_sn)
    {
      memset(pGateWay, 0, sizeof(NwkParentWorkStrcut));
    }
  }   
}

/*		
================================================================================
描述 : 唤醒周期,由应用层设置
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_set_wake_period(u16 period)
{
	g_sNwkNodeWork.wake_period=period;
}

/*		
================================================================================
描述 : 设置搜索周期和时长
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_set_search_time(u32 period, u32 wait_time)
{
  NwkNodeSearchStruct *pSearch=&g_sNwkNodeWork.node_search;
  pSearch->period=period;
  pSearch->wait_time=wait_time;
}

/*		
================================================================================
描述 : 传递LoRa模块应用指针
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_set_lora_dev(LoRaDevStruct *pLoRaDev)
{
	g_sNwkNodeWork.pLoRaDev=pLoRaDev;
}

/*		
================================================================================
描述 : 设置LED
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_set_led(bool state)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
    hal_sx1278_set_led(&g_sNwkNodeWork.pLoRaDev->tag_hal_sx1278, state);

#elif defined(LORA_SX1268)
    hal_sx1268_set_led(&g_sNwkNodeWork.pLoRaDev->tag_hal_sx1268, state);
#elif defined(LORA_LLCC68)

#endif	
	
}

/*		
================================================================================
描述 : 设置LoRa参数
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_set_lora_param(u32 freq, u8 sf, u8 bw)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
		drv_sx1278_set_rf_freq(g_sNwkNodeWork.pLoRaDev, freq);
		drv_sx1278_set_sf(g_sNwkNodeWork.pLoRaDev, sf);
		drv_sx1278_set_bw(g_sNwkNodeWork.pLoRaDev, bw);

#elif defined(LORA_SX1268)
		drv_sx1268_set_freq(g_sNwkNodeWork.pLoRaDev, freq);
		drv_sx1268_set_sf_bw(g_sNwkNodeWork.pLoRaDev, sf, bw);
#elif defined(LORA_LLCC68)

#endif	
	
}

/*		
================================================================================
描述 : 设备初始化
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_device_init(void)
{
	printf("nwk_node_device_init ### \n");
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
		drv_sx1278_init( g_sNwkNodeWork.pLoRaDev);
	
#elif defined(LORA_SX1268)
		drv_sx1268_init(g_sNwkNodeWork.pLoRaDev);
#elif defined(LORA_LLCC68)

#endif	
    nwk_delay_ms(5);
}

/*		
================================================================================
描述 : 进入睡眠
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_sleep_init(void)
{
	printf("nwk_node_sleep_init ### \n");
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
		drv_sx1278_set_op_mode( g_sNwkNodeWork.pLoRaDev, RFLR_OPMODE_SLEEP );
	
#elif defined(LORA_SX1268)
		drv_sx1268_set_sleep(g_sNwkNodeWork.pLoRaDev);
#elif defined(LORA_LLCC68)

#endif		
}


/*		
================================================================================
描述 : CAD初始化
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_cad_init(void)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;	
	
#if defined(LORA_SX1278)  
	drv_sx1278_cad_init(g_sNwkNodeWork.pLoRaDev);
	
#elif defined(LORA_SX1268)
	drv_sx1268_cad_init(g_sNwkNodeWork.pLoRaDev);

#elif defined(LORA_LLCC68)

#endif	
}

/*		
================================================================================
描述 : 接收初始化
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_recv_init(void)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;	
	
#if defined(LORA_SX1278)  
	drv_sx1278_recv_init(g_sNwkNodeWork.pLoRaDev); 
	
#elif defined(LORA_SX1268)
  drv_sx1268_recv_init(g_sNwkNodeWork.pLoRaDev); 
#elif defined(LORA_LLCC68)

#endif	
}

/*		
================================================================================
描述 : CAD检测
输入 : 
输出 : 
================================================================================
*/ 
u8 nwk_node_cad_check(void)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	return drv_sx1278_cad_check(g_sNwkNodeWork.pLoRaDev);  
	
#elif defined(LORA_SX1268)
  return drv_sx1268_cad_check(g_sNwkNodeWork.pLoRaDev);  
#elif defined(LORA_LLCC68)

#endif	
  
}

/*		
================================================================================
描述 : 接收检查,同时获取RSSI
输入 : 
输出 : 
================================================================================
*/ 
u8 nwk_node_recv_check(u8 *buff, RfParamStruct *rf_param)
{
	u8 read_size=0;
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	
	read_size=drv_sx1278_recv_check(g_sNwkNodeWork.pLoRaDev, buff); 
	if(read_size>0)
	{
		rf_param->rssi=drv_sx1278_read_rssi(g_sNwkNodeWork.pLoRaDev);
    rf_param->snr=drv_sx1278_read_snr(g_sNwkNodeWork.pLoRaDev);
	}
#elif defined(LORA_SX1268)
	read_size=drv_sx1268_recv_check(g_sNwkNodeWork.pLoRaDev, buff, 255); 
	if(read_size>0)
	{
    Sx1268PacketStatusStruct status;
    drv_sx1268_get_pack_status(g_sNwkNodeWork.pLoRaDev, &status);
//    rf_param->rssi=status.SignalRssiPkt;
    rf_param->snr=status.SnrPkt;
		rf_param->rssi=drv_sx1268_get_rssi_inst(g_sNwkNodeWork.pLoRaDev);
	}
#elif defined(LORA_LLCC68)

#endif		
	return read_size;
}

///*		
//================================================================================
//描述 : 发送时长计算
//输入 : 
//输出 : 
//================================================================================
//*/ 
//u32 nwk_calcu_air_time(u8 sf, u8 bw, u16 data_len)
//{
//  u32 tx_time=0;
//#if defined(LORA_SX1278)  
//	
//	tx_time=drv_sx1278_calcu_air_time(sf, bw, data_len);
//#elif defined(LORA_SX1268)
//  tx_time=drv_sx1268_calcu_air_time(sf, bw, data_len);
//#elif defined(LORA_LLCC68)

//#endif	
//return tx_time;  
//}

/*		
================================================================================
描述 : 发送数据包
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_send_buff(u8 *buff, u16 len)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;	

#if defined(LORA_SX1278)  
	drv_sx1278_send(g_sNwkNodeWork.pLoRaDev, buff, len); 
	
#elif defined(LORA_SX1268)
  drv_sx1268_send(g_sNwkNodeWork.pLoRaDev, buff, len); 
#elif defined(LORA_LLCC68)

#endif	
}


/*		
================================================================================
描述 : 发送完成检测
输入 : 
输出 : 
================================================================================
*/ 
u8 nwk_node_send_check(void)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	return drv_sx1278_send_check(g_sNwkNodeWork.pLoRaDev); 
	
#elif defined(LORA_SX1268)
  return drv_sx1268_send_check(g_sNwkNodeWork.pLoRaDev); 
#elif defined(LORA_LLCC68)

#endif	
}

/*		
================================================================================
描述 : 发送嗅探包
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_send_sniff(u8 sf, u8 bw)
{
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;	
  u8 buff[1]={0x01};
#if defined(LORA_SX1278)  
	drv_sx1278_send(g_sNwkNodeWork.pLoRaDev, buff, 1); 

#elif defined(LORA_SX1268)
  drv_sx1268_send(g_sNwkNodeWork.pLoRaDev, buff, 1); 
#elif defined(LORA_LLCC68)

#endif	
	nwk_delay_ms(5);
}

/*		
================================================================================
描述 : 组合发送数据
输入 : 
输出 : 
================================================================================
*/ 
u8 nwk_node_make_send_buff(u8 opt, u32 dst_sn, u8 *key, u8 cmd_type, u8 pack_num, u8 *in_buff, u8 in_len, u8 *out_buff, u8 out_size)
{
	//A5 ~ opt ~ src_sn ~ data_len ~ payload
	//opt: bit[0:1]=网络角色; bit[2:3]=加密方式;bit[4:4]=密码类型;bit[5:6]=保留;bit[7:7]=扩展位
	//payload: union_len ~dst_sn ~ cmd_type ~ pack_num ~ data ~ crc16
	if(in_len+20>out_size || in_len>NWK_TRANSMIT_MAX_SIZE)
	{
		return 0; 
	}
	
	//加密单元整合
	u8 union_buff[NWK_TRANSMIT_MAX_SIZE+16]={0};
	u8 union_len=0;
	union_buff[union_len++]=0;
	u32 src_sn=g_sNwkNodeWork.node_sn;
	union_buff[union_len++]=dst_sn>>24; 
	union_buff[union_len++]=dst_sn>>16;
	union_buff[union_len++]=dst_sn>>8;
	union_buff[union_len++]=dst_sn;
	union_buff[union_len++]=cmd_type;
	union_buff[union_len++]=pack_num;
	memcpy(&union_buff[union_len], in_buff, in_len);
	union_len+=in_len;
	union_buff[0]=union_len;//单元实际长度
	u16 crcValue=nwk_crc16(union_buff, union_len);
	union_buff[union_len++]=crcValue>>8;	
	union_buff[union_len++]=crcValue;	
	
	u8 encrypt_mode=(opt>>2)&0x03;//加密模式
	u8 *pEncrypt=out_buff+7;//加密区缓存地址
	switch(encrypt_mode)
	{
		case EncryptDisable:
		{ 
			memcpy(pEncrypt, union_buff, union_len);
			break;
		}
		case EncryptTEA:
		{
			u8 remain_len=union_len%8;
			if(remain_len>0)
				union_len+=(8-remain_len);//8字节对齐,便于TEA加密			
			nwk_tea_encrypt(union_buff, union_len, (u32*)key);
			memcpy(pEncrypt, union_buff, union_len);
			break;
		}
		case EncryptAES:
		{
#ifdef	NWK_NODE_USE_AES	//是否使用AES加密		
			u8 remain_len=union_len%16;
			if(remain_len>0)
				union_len+=(16-remain_len);//16字节对齐,便于TEA加密		      
			int out_len=nwk_aes_encrypt(union_buff, union_len, pEncrypt, NWK_TRANSMIT_MAX_SIZE, key);//aes加密
			if(out_len<16)
			{
				return 0;
			}
			union_len=out_len;
#else
			union_len=0;//无法加密
#endif			
			break;
		}
		default:union_len=0;
	}	

	u8 *pData=out_buff;
	u8 data_len=0;	
	if(union_len>0)
	{
		//整合明文区
		pData[data_len++]=0xA5;
		pData[data_len++]=opt;
		pData[data_len++]=src_sn>>24;
		pData[data_len++]=src_sn>>16;
		pData[data_len++]=src_sn>>8;
		pData[data_len++]=src_sn;
		pData[data_len++]=union_len;
		data_len+=union_len;	
	}		
	
	return data_len;
}

/*		
================================================================================
描述 : 数据接收解析
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_recv_parse(u8 *recv_buff, u8 recv_len)
{
	u8 head[1]={0xA5};
	u8 union_buff[NWK_TRANSMIT_MAX_SIZE+16]={0};
	u8 *pData=nwk_find_head(recv_buff, recv_len, head, 1);
	
	if(pData)
	{
		pData+=1;
		u8 opt=pData[0];
		pData+=1;
		u8 role=opt&0x03;//网络角色
		u8 encrypt_mode=(opt>>2)&0x03;//加密模式
		u8 key_type=(opt>>4)&0x01;//密码类型
		u8 ext=(opt>>7)&0x01;//扩展位
		if(ext)
		{
			u8 opt1=pData[0];
			printf("opt1=0x%02X\n", opt1);
			pData+=1;
		}
		u32 src_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
		pData+=4;
		u8 payload_len=pData[0];
		pData+=1;
		if(payload_len>sizeof(union_buff))
		{
			printf("error payload_len>NWK_TRANSMIT_MAX_SIZE!\n");
			return;
		}
		u8 *pKey=g_sNwkNodeWork.root_key;//默认根密码
		NwkParentWorkStrcut *pGateWay=NULL; 
		if(role==NwkRoleGateWay)//是否为网关设备
		{
			pGateWay=nwk_node_search_gw(src_sn);//查询网关
			if(pGateWay && key_type==KeyTypeApp)
			{
				pKey=pGateWay->app_key;//应用密码
        pGateWay->rssi=g_sNwkNodeWork.rf_param.rssi;//更新信号强度
        pGateWay->snr=g_sNwkNodeWork.rf_param.snr;
			}			
		}
		
		int union_len=0;
		switch(encrypt_mode)
		{
			case EncryptDisable:
			{ 
				memcpy(union_buff, pData, payload_len);
				union_len=payload_len;
				break;
			}	
			case EncryptTEA:
			{ 
				union_len=nwk_tea_decrypt(pData, payload_len, (u32*)pKey);
				memcpy(union_buff, pData, payload_len);
				break;
			}
			case EncryptAES:
			{ 
#ifdef	NWK_NODE_USE_AES	//是否使用AES加密		
				union_len=nwk_aes_decrypt(pData, payload_len, union_buff, sizeof(union_buff), pKey); 
#else
				union_len=0;
#endif				
				break;
			}			
		}
		if(union_len>0)
		{
			printf_hex("out buff=", union_buff, union_len);
			pData=union_buff;
			union_len=pData[0];
			u16 crcValue=pData[union_len]<<8|pData[union_len+1];
			if(nwk_crc16(union_buff, union_len)==crcValue)
			{
				pData+=1;
				u32 dst_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
				pData+=4;
				u8 cmd_type=pData[0];
				pData+=1;
				u8 pack_num=pData[0];
				pData+=1;
				if(dst_sn != g_sNwkNodeWork.node_sn)
				{
					printf("dst_sn=0x%08X != local_sn=0x%08X\n", dst_sn, g_sNwkNodeWork.node_sn);
					return;
				}
//				if(pGateWay && pack_num==pGateWay->down_pack_num)
//				{
//					printf("gw_sn=0x%08X have same down_pack_num=%d\n", src_sn, pack_num);
//					return;
//				}
        printf("down pack_num=%d\n", pack_num);
				switch(cmd_type)
				{
					case NwkCmdHeart:
					{
						printf("src_sn=0x%08X NwkCmdHeart!\n", src_sn);
						break;
					}
					case NwkCmdAck://回复
					{
						printf("src_sn=0x%08X NwkCmdAck!\n", src_sn);
            u8 ack_cmd=pData[0];
            pData+=1;
            if(ack_cmd==NwkCmdDataOnce)//单包数据回复,说明发送成功
            {
              //清理发送数据
              if(role==NwkRoleGateWay)//网关
              {
                nwk_node_clear_tx();
                NwkNodeEventStruct *pEvent=&g_sNwkNodeWork.event;
                pEvent->event=NwkNodeEventTxGwOK;                 
                NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
                pNodeTxGw->tx_ok_cnts++;//发送成功计数
                //同时返回时间戳
								u32 now_time=nwk_get_rtc_counter();
                u32 ack_time=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
                pData+=4;
								printf("ack_time=%us, now_time=%us\n", ack_time, now_time);
                if(ack_time>NWK_TIME_STAMP_THRESH && now_time!=ack_time)
                {
//                  printf("update rtc time=%us\n", ack_time);
//                  nwk_set_rtc_counter(ack_time);//更新RTC时间
                }
                u8 down_len=pData[0];
                pData+=1;
                if(down_len>0)//有下行数据,继续等待
                {
                  printf("have down len=%d, continue recv!\n", down_len);
                  nwk_node_recv_init();//继续接收
                  u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, down_len);//等待时间
                  pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
                  pNodeTxGw->wait_cnts=tx_time/1000+1;             
                  pNodeTxGw->tx_state=NwkNodeTxGwWaitDownCheck;//下行包接收检测                     
                }
              }
              else if(role==NwkRoleNode)
              {
                nwk_node_clear_d2d();
                NwkNodeEventStruct *pEvent=&g_sNwkNodeWork.event;
                pEvent->event=NwkNodeEventTxD2DOK;                 
              }
            }
						break;
					}					
					case NwkCmdJoin://入网
					{
						printf("src_sn=0x%08X NwkCmdJoin!\n", src_sn);
            if(pGateWay)
            {
              u8 join_state=pData[0];
              pData+=1;
              printf("join_state=%d\n", join_state);//默认接受入网
              //同时返回时间戳
              u32 ack_time=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
              pData+=4;
              if(ack_time>NWK_TIME_STAMP_THRESH && nwk_get_rtc_counter()!=ack_time)
              {
//                printf("update rtc time=%us\n", ack_time);
//                nwk_set_rtc_counter(ack_time);//更新RTC时间
              }              
              
              u8 key_tmp[16]={0x45,0xEF,0x09,0x3E,0xA2,0xC8,0xB1,0x4A,0x90,0x75,0xD9,0x63,0x7B,0x3B,0x82,0x96};//解算密码,应用时注意混淆
              printf_hex("key in=", pData, 16);
							nwk_tea_encrypt(pData, 16, (u32 *)key_tmp);//把网关下发的16字节随机数进行加密作为APP_KEY
              printf_hex("key out=", pData, 16);
							memcpy(pGateWay->app_key, pData, 16); 
              pGateWay->join_state=JoinStateOK;//入网成功
							printf("pGateWay->join_state=%d\n", pGateWay->join_state);
              printf_oled("join ok!");
							nwk_node_clear_tx();			
              NwkNodeEventStruct *pEvent=&g_sNwkNodeWork.event;
              pEvent->event=NwkNodeEventJoinResult;    
              u8 *pData=pEvent->params;
              pData[0]=join_state;
              pData[1]=src_sn>>24;
              pData[2]=src_sn>>16;
              pData[3]=src_sn>>8;
              pData[4]=src_sn;
            }
						break;
					}
					case NwkCmdDataOnce://单包数据
					{
						printf("src_sn=0x%08X NwkCmdDataOnce!\n", src_sn);
            NwkNodeRecvFromStruct *pRecvFrom=&g_sNwkNodeWork.recv_from;
            pRecvFrom->data_len=union_len-7;//应用数据长度
            memset(pRecvFrom->app_data, 0, sizeof(pRecvFrom->app_data));
            memcpy(pRecvFrom->app_data, pData, pRecvFrom->data_len);
            pRecvFrom->src_sn=src_sn;
            pRecvFrom->read_flag=true;//通知应用层读取

            //需要回复NwkCmdAck指令    
            NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
            
            u8 opt=NwkRoleNode | (encrypt_mode<<2) | (key_type<<4);//组合配置
            //组合回复包
            u8 tmp_buff[1]={cmd_type};
            u8 pack_num=nwk_get_rand()%255;
            pNodeRx->ack_len=nwk_node_make_send_buff(opt, src_sn, pKey, NwkCmdAck, pack_num, tmp_buff, 1, pNodeRx->ack_buff, sizeof(pNodeRx->ack_buff));
            break;
					}
					case NwkCmdDataMult://连续数据
					{
						printf("src_sn=0x%08X NwkCmdDataMult!\n", src_sn);
						break;
					}					
				}
			}
		}
	}
	
}

/*		
================================================================================
描述 : 搜索网关列表
输入 : 
输出 : 
================================================================================
*/ 
NwkParentWorkStrcut *nwk_node_search_gw(u32 gw_sn)
{
	for(u8 i=0; i<NWK_GW_NUM; i++)
	{
		NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
		if(pGateWay->gw_sn==gw_sn)
		{
			return pGateWay;
		}
	}
	return NULL;
}



/*		
================================================================================
描述 : 根据各个网关的数据选择合适的网关作为数据发送对象
输入 : 
输出 : 
================================================================================
*/ 
NwkParentWorkStrcut *nwk_node_select_gw(void)
{
  u8 ptr=nwk_get_rand()%NWK_GW_NUM;
  for(u8 i=0; i<NWK_GW_NUM; i++)
  {
    if(ptr>=NWK_GW_NUM)ptr=0;
    NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[ptr];
    if(pGateWay->gw_sn>0 && pGateWay->join_state==JoinStateOK)
    {
      printf("select gw=0x%08X\n", pGateWay->gw_sn);
      return pGateWay;
    }
    ptr++;
  }  
  
	return NULL;
}

/*		
================================================================================
描述 : 清除发送数据
输入 : 
输出 : 
================================================================================
*/
void nwk_node_clear_tx(void)
{
  NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
  memset(pNodeTxGw->tx_buff, 0, sizeof(pNodeTxGw->tx_buff));
  pNodeTxGw->tx_len=0; 
  pNodeTxGw->try_cnts=0;       
  pNodeTxGw->tx_state=NwkNodeTxGwIdel;//回合结束  
  printf("clear tx buff!\n");   
}

/*		
================================================================================
描述 : 清除D2D发送数据
输入 : 
输出 : 
================================================================================
*/
void nwk_node_clear_d2d(void)
{
  NwkNodeTxD2dStruct *pNodeTxD2d=&g_sNwkNodeWork.node_tx_d2d;
  memset(pNodeTxD2d->tx_buff, 0, sizeof(pNodeTxD2d->tx_buff));
  pNodeTxD2d->tx_len=0; 
  pNodeTxD2d->try_cnts=0;       
  pNodeTxD2d->d2d_state=NwkNodeTxD2dIdel;//回合结束  
  printf("clear d2d buff!\n");   
}

/*		
================================================================================
描述 : 把数据发送到网关
输入 : 
输出 : 
================================================================================
*/ 
u8 nwk_node_send2gateway(u8 *in_buff, u8 in_len)
{
  NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
  if(pNodeTxGw->tx_len>0 || in_len>NWK_TRANSMIT_MAX_SIZE)
  {
    printf("tx task is busy!\n");
    return 0;
  }

  memcpy(pNodeTxGw->tx_buff, in_buff, in_len);
  pNodeTxGw->tx_len=in_len;
  pNodeTxGw->tx_cmd=NwkCmdDataOnce;
  pNodeTxGw->try_cnts=0;
  pNodeTxGw->tx_total_cnts++;//统计发送次数

  pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();
  pNodeTxGw->wait_cnts=0;
  
  printf("***send to gw!\n");

  return 1;
}

/*		
================================================================================
描述 : 请求入网
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_req_join(u32 gw_sn)
{
  NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
  pNodeTxGw->join_sn=gw_sn;
  pNodeTxGw->tx_buff[0]=g_sNwkNodeWork.wake_period>>8;
  pNodeTxGw->tx_buff[1]=g_sNwkNodeWork.wake_period;//唤醒周期
  pNodeTxGw->tx_len=2;
  pNodeTxGw->tx_cmd=NwkCmdJoin;
  pNodeTxGw->wait_cnts=0;
  pNodeTxGw->try_cnts=0;
	printf("***req_join!\n");  
  printf_oled("***req_join!");
}

/*		
================================================================================
描述 : 把数据发送到节点(D2D)
输入 : 
输出 : 
================================================================================
*/
u8 nwk_node_send2device(u32 dst_sn, u16 period, u8 *in_buff, u8 in_len)
{
  printf("d2d dst_sn=0x%08X, period=%ds\n", dst_sn, period);
  printf_hex("buff=", in_buff, in_len);
  NwkNodeTxD2dStruct *pNodeTxD2d=&g_sNwkNodeWork.node_tx_d2d;
  if(pNodeTxD2d->tx_len>0 || in_len>NWK_TRANSMIT_MAX_SIZE)
  {
    printf("d2d task is busy!\n");
    return 0;
  }
  if(period==0xFFFF)//长休眠设备无法发送D2D数据
  {
    printf("period error!\n");
    return 0;
  }
  u32 now_time=nwk_get_rtc_counter();
  if(period>0 && now_time<NWK_TIME_STAMP_THRESH)//有休眠, 时间未同步,不能发送D2D数据
  {
    printf("now_time=%us error!\n", now_time);
    return 0;
  }
  pNodeTxD2d->dst_sn=dst_sn;
  pNodeTxD2d->wake_period=period;
  memcpy(pNodeTxD2d->tx_buff, in_buff, in_len);
  pNodeTxD2d->tx_len=in_len;
  pNodeTxD2d->try_cnts=0;
  
  pNodeTxD2d->start_rtc_time=nwk_get_rtc_counter();
  pNodeTxD2d->wait_cnts=0;
  
  printf("***send to d2d sn=0x%08X!\n", dst_sn);  
  return 1;
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
描述 : 搜索广播进程
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_search_process(void)
{
  static const u32 base_freq=NWK_BROAD_BASE_FREQ;
  static const u8 sf=NWK_BROAD_SF, bw=NWK_BROAD_BW;
  NwkNodeSearchStruct *pSearch=&g_sNwkNodeWork.node_search;
  switch(pSearch->search_state)
  {
    case NwkNodeSearchIdel: //空闲
    {
      
      break;
    }
    case NwkNodeSearchInit: //初始化
    {
      printf("*****start search!\n");
      printf_oled("**start search! %us", pSearch->search_start_time);       
			u32 freq=base_freq;
			printf("**search param (%d, %d, %d)\n", freq/1000000, sf, bw);
      nwk_node_set_lora_param(freq, sf, bw);
			nwk_node_recv_init();//进入接收模式
			pSearch->search_state=NwkNodeSearchRxCheck;			
      break;
    }   
    case NwkNodeSearchRxCheck: //接收检测
    {
      RfParamStruct *rf=&g_sNwkNodeWork.rf_param;
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, rf);
      if(recv_len>0)
      {
				printf("rssi=%ddbm, snr=%ddbm\n", rf->rssi, rf->snr);
				printf_hex("search recv=", g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        nwk_tea_decrypt(g_sNwkNodeWork.node_rx.recv_buff, recv_len, (u32*)g_sNwkNodeWork.root_key);//解密
				printf_hex("search out=", g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        //数据解析,存储网关信息
        //A5 gw_sn freq  wireless
        u8 head[1]={0xA5};
        u8 *pData=nwk_find_head(g_sNwkNodeWork.node_rx.recv_buff, recv_len, head, 1);
        if(pData)
        {
					pData+=1;
          u32 gw_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          u8 base_freq=pData[0];
          pData+=1;
          u8 wireless_num=pData[0];
          pData+=1;
          u8 run_mode=base_freq>>7;
          pData+=1;
          u32 rtc_time=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          base_freq&=0x7F;
          printf_oled("search W:%d/%d,mode=%d, F=%d, %ddbm, %ddbm", wireless_num>>4&0x0F, wireless_num&0x0F, 
                      run_mode, base_freq, rf->rssi, rf->snr);
          wireless_num&=0x0F;
          u32 now_time=nwk_get_rtc_counter();
          printf("now_time=%us, recv rtc_time=%us\n", now_time, rtc_time);
          nwk_set_rtc_counter(rtc_time);
          srand(rf->rssi);
          NwkParentWorkStrcut *pGateWay=nwk_node_search_gw(gw_sn);
          if(pGateWay==NULL)
          {
            for(u8 i=0; i<NWK_GW_NUM; i++)
            {
              NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
              if(pGateWay->gw_sn==0)
              {
                pGateWay->gw_sn=gw_sn;
                pGateWay->base_freq_ptr=base_freq;
                pGateWay->wireless_num=wireless_num;
                pGateWay->run_mode=run_mode;
                printf("add gw_sn=0x%08X, base_freq=%d, wireless=%d, run_mode=%d\n", 
                        gw_sn, base_freq, wireless_num, run_mode);
                break;
              }
            }            
          }
          else//更新该网关信息
          {
            pGateWay->base_freq_ptr=base_freq;
            pGateWay->wireless_num=wireless_num;
            pGateWay->run_mode=run_mode;     
            printf("update gw_sn=0x%08X, base_freq=%d, wireless=%d, run_mode=%d\n", 
                    gw_sn, base_freq, wireless_num, run_mode);            
          }
        }
      } //end of recv
      else
      {
        u32 now_time=nwk_get_rtc_counter();
        if(now_time-pSearch->search_start_time>pSearch->wait_time)//搜索时间到
        {
          pSearch->alarm_rtc_time=now_time+pSearch->wait_time;
          pSearch->search_start_time=now_time;
          printf("exit search!\nalarm time=%us\n", pSearch->alarm_rtc_time);
          printf_oled("exit search!");
          pSearch->search_state=NwkNodeSearchIdel;
        }        
      }
      break;
    }    
  }
}

/*		
================================================================================
描述 : 节点接收进程
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_rx_process(void)
{
  static bool led_state=false;
  nwk_node_set_led(led_state);
  led_state=!led_state;
  
  NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
  switch(pNodeRx->rx_state)
  {
    case NwkNodeRxIdel:
    {
      
      break;
    }
    case NwkNodeRxInit:
    {
      pNodeRx->freq=nwk_get_sn_freq(g_sNwkNodeWork.node_sn);//根据序列号计算频段  
//			printf("node freq=%.2fMHz\n", pNodeRx->freq/1000000.f);
      pNodeRx->rx_state=NwkNodeRxCadInit;
      pNodeRx->listen_cnts=0;
      break;
    }
    case NwkNodeRxCadInit:
    {
			pNodeRx->curr_sf=NWK_BROAD_SF;
			pNodeRx->curr_bw=NWK_BROAD_BW; 
			printf("rx cad param=(%.2f, %d, %d)\n", pNodeRx->freq/1000000.f, pNodeRx->curr_sf, pNodeRx->curr_bw);
      nwk_node_set_lora_param(pNodeRx->freq, pNodeRx->curr_sf, pNodeRx->curr_bw);
      if(g_sNwkNodeWork.wake_period==0)
      {
        nwk_node_recv_init();//进入接收,等待回复
        pNodeRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeRx->wait_cnts=10;
        pNodeRx->rx_state=NwkNodeRxSnCheck; //地址匹配  
        printf("period=0, wait rx sn!\n");        
      }
      else
      {
        nwk_node_cad_init(); 
        pNodeRx->rx_state=NwkNodeRxCadCheck;        
      }
      break;
    }    
    case NwkNodeRxCadCheck:
    {
      u8 result=nwk_node_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
//        printf("rx no cad!\n");
        pNodeRx->listen_cnts++;
        if(pNodeRx->listen_cnts>=2)//监测结束
        {
          nwk_node_set_led(false);
          pNodeRx->rx_state=NwkNodeRxIdel;
        }
        else
        {
					nwk_node_cad_init(); //继续监听
        }
      }
      else if(result==CadResultSuccess)//搜索到
      {
        printf("***rx cad ok! awake!\n");
        nwk_node_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_calcu_air_time(pNodeRx->curr_sf, pNodeRx->curr_bw, 8);//接收等待时间
        pNodeRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeRx->wait_cnts=tx_time/1000+4;
        pNodeRx->rx_state=NwkNodeRxSnCheck; //地址匹配        
      }       
      break;
    }
    case NwkNodeRxSnCheck:
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 recv_len=nwk_node_recv_check(pNodeRx->recv_buff, &g_sNwkNodeWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
				printf_hex("recv=", pNodeRx->recv_buff, recv_len);
				
				u8 head[]={0xA9};
				u8 *pData=nwk_find_head(pNodeRx->recv_buff, recv_len, head, 1);
				if(pData)
				{
					pData+=1;
					u32 dst_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
					pData+=4;
					pNodeRx->will_len=pData[0];
					pData+=1;
					if(dst_sn==0xFFFFFFFF)//广播
					{
						//继续接收状态
					}
					else if(dst_sn==g_sNwkNodeWork.node_sn)
					{
						//速率匹配
						pNodeRx->group_id=0;
						pNodeRx->listen_cnts=0;
						pNodeRx->rx_state=NwkNodeRxAdrInit;
						printf("sn OK!\n");
					}
					else
					{
						printf("dst_sn=0x%08X error!\n", dst_sn);
						pNodeRx->rx_state=NwkNodeRxIdel;//结束
					}
				} 				
      }   
      else if(now_time-pNodeRx->start_rtc_time>pNodeRx->wait_cnts)//超时
      {
				printf("wait sn time out!\n");
        pNodeRx->rx_state=NwkNodeRxIdel;
      }       
      break;
    }
		case NwkNodeRxAdrInit:
		{
			u8 sf, bw;
			u32 freq=pNodeRx->freq;
      nwk_get_down_channel(pNodeRx->group_id, &sf, &bw);
      pNodeRx->curr_sf=sf;
      pNodeRx->curr_bw=bw;      
//			printf("cad param(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, sf, bw);
      nwk_node_set_lora_param(freq, sf, bw);
      nwk_node_cad_init();  
      pNodeRx->rx_state=NwkNodeRxAdrCadCheck;			
			break;
		}
		case NwkNodeRxAdrCadCheck:
		{
      u8 result=nwk_node_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
        pNodeRx->group_id++;
        if(pNodeRx->group_id>=NWK_DOWN_CHANNEL_NUM)
        {
					pNodeRx->listen_cnts++;
					if(pNodeRx->listen_cnts<30)//扫描轮次
					{
						pNodeRx->group_id=0;
						pNodeRx->rx_state=NwkNodeRxAdrInit;//继续监听	
					}
					else
					{
						printf("adr failed!\n");
						pNodeRx->rx_state=NwkNodeRxIdel;//结束本回合
					}
        } 
				else
				{
					pNodeRx->rx_state=NwkNodeRxAdrInit;//下一组参数
				}
      }
      else if(result==CadResultSuccess)//搜索到
			{
        printf("cad OK***(%.2f, %d, %d)\n", pNodeRx->freq/1000000.0, pNodeRx->curr_sf, pNodeRx->curr_bw);
				u32 freq=pNodeRx->freq+1000000;//
        u8 sf=pNodeRx->curr_sf;
        u8 bw=pNodeRx->curr_bw;
        printf("group id=%d, rx sniff param(%.2f, %d, %d)\n", pNodeRx->group_id, freq/1000000.0, sf, bw);
        
        nwk_node_set_lora_param(freq, sf, bw);  
        for(u8 i=0; i<10; i++)
        {
          nwk_node_send_sniff(sf, bw);//返回嗅探帧
        }
        printf("into recv mode!\n"); 
        nwk_node_recv_init();//进入接收
        u32 tx_time=nwk_calcu_air_time(sf, bw, pNodeRx->will_len);//接收等待时间
        pNodeRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeRx->wait_cnts=tx_time/1000+2;             
        pNodeRx->rx_state=NwkNodeRxAppCheck; 
        printf("will wait time=%ds\n", pNodeRx->wait_cnts); 				
			}
				
			break;
		}			
		case NwkNodeRxAppCheck://应用数据接收检查
		{
      u32 now_time=nwk_get_rtc_counter();
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &g_sNwkNodeWork.rf_param); 
      if(recv_len>0)
      {
        printf("rx recv rssi=%ddbm, snr=%ddbm\n", g_sNwkNodeWork.rf_param.rssi, g_sNwkNodeWork.rf_param.snr);
        srand(g_sNwkNodeWork.rf_param.rssi);
        u8 *pBuff=g_sNwkNodeWork.node_rx.recv_buff;
        printf_hex("recv=", pBuff, recv_len);
        pNodeRx->ack_len=0;
				nwk_node_recv_parse(pBuff, recv_len);
        if(pNodeRx->ack_len>0)
        {
          //发送回复包
          nwk_node_send_buff(pNodeRx->ack_buff, pNodeRx->ack_len); 
          u32 tx_time=nwk_calcu_air_time(pNodeRx->curr_sf, pNodeRx->curr_bw, pNodeRx->ack_len);//接收等待时间
          pNodeRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
          pNodeRx->wait_cnts=tx_time/1000+1;             
          pNodeRx->rx_state=NwkNodeRxAppAck;//回复包发送检测           
        }
        else
        {
          printf("ack none, exit!\n");
          pNodeRx->rx_state=NwkNodeRxIdel;//结束本回合 
        }				
			}	
      else if(now_time-pNodeRx->start_rtc_time>pNodeRx->wait_cnts)//超时
      {
        printf("recv wait time out!\n");
        pNodeRx->rx_state=NwkNodeRxIdel;//结束本回合
      } 			
			break;
		}
    case NwkNodeRxAppAck://回复发送检测
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成
      {
				printf("app ack send ok!\n");
        pNodeRx->rx_state=NwkNodeRxIdel;//结束本回合
      }
      else if(now_time-pNodeRx->start_rtc_time>pNodeRx->wait_cnts)//发送超时
      {
				printf("app ack send time out!\n");
        pNodeRx->rx_state=NwkNodeRxIdel;//结束本回合  
      }      
      break;
    }
  }
}  

/*		
================================================================================
描述 : 节点主动发送到网关
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_tx_gw_process(void)
{
  static bool led_state=false;
  nwk_node_set_led(led_state);
  led_state=!led_state;
  
  NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
  static u8 make_buff[255]={0};  //缓冲区
  static const u8 make_size=(u8)sizeof(make_buff);
  static u8 make_len=0;
  switch(pNodeTxGw->tx_state)
  {
    case NwkNodeTxGwIdel:
    {
      break;
    }
    case NwkNodeTxGwInit://参数初始化
    {
      NwkParentWorkStrcut *pGateWay=NULL;
      if(pNodeTxGw->try_cnts==0)//首次发送
      {
        pNodeTxGw->pGateWay=NULL;
        if(pNodeTxGw->tx_cmd==NwkCmdJoin)//请求入网发送
        {
          printf("req join gw_sn=0x%08X###\n", pNodeTxGw->join_sn);
          pGateWay=nwk_node_search_gw(pNodeTxGw->join_sn);//选择要入网的网关
        }
        else
        {
          pGateWay=nwk_node_select_gw();//选择最优网关
        }     
        pNodeTxGw->pGateWay=pGateWay;
      }
      else
      {
        pGateWay=pNodeTxGw->pGateWay;
      }
      
      if(pGateWay)
      {
        nwk_node_device_init();//初始化设备
        u8 key_type=KeyTypeRoot;
        u8 *pKey=g_sNwkNodeWork.root_key;
        if(pGateWay->join_state==JoinStateOK && pNodeTxGw->tx_cmd!=NwkCmdJoin)//入网用根密码,入网后应用数据用应用密码
        {
          key_type=KeyTypeApp;
          pKey=pGateWay->app_key;//有应用密码尽量用应用密码
					printf_hex("use app key=", pKey, 16);
        }
        u8 encrypt_mode=NWK_NODE_USE_ENCRYPT_MODE;//加密模式一般根据芯片能力直接固定即可
        u8 opt=NwkRoleNode | (encrypt_mode<<2) | (key_type<<4);//组合配置
        
        //组合发送数据
        make_len=nwk_node_make_send_buff(opt, pGateWay->gw_sn, pKey, pNodeTxGw->tx_cmd, ++pGateWay->up_pack_num, pNodeTxGw->tx_buff, pNodeTxGw->tx_len, make_buff, make_size);
				printf_hex("make buff=", make_buff, make_len);   
        
        if(pGateWay->run_mode==NwkRunModeStatic)//静态模式
        {
          pNodeTxGw->wireless_ptr=0;
          if(pGateWay->wireless_num>0)
          {
            pNodeTxGw->wireless_ptr=nwk_get_rand()%pGateWay->wireless_num;//随机选择天线
//            printf("wireless_ptr=%d\n", pNodeTxGw->wireless_ptr);
            
          }             
          printf("wireless_ptr=%d\n", pNodeTxGw->wireless_ptr);
          printf_oled("wireless_ptr=%d\n", pNodeTxGw->wireless_ptr);
          pNodeTxGw->tx_state=NwkNodeTxStaticInit;//下一步,静态参数初始化   
        }
        else//动态模式
        {
          if(pNodeTxGw->try_cnts==0)//首次发送
          {
            pNodeTxGw->group_id=nwk_get_rand()%3;
            printf("gw_sn=0x%08X in dynamic mode! start group_id=%d\n", pGateWay->gw_sn, pNodeTxGw->group_id);
          }
          pNodeTxGw->wireless_ptr=0;
          if(pGateWay->wireless_num>0)
          {
            pNodeTxGw->wireless_ptr=nwk_get_rand()%pGateWay->wireless_num;//随机选择天线
            printf("wireless_ptr=%d\n", pNodeTxGw->wireless_ptr);
          }
          printf_oled("wireless_ptr=%d\n", pNodeTxGw->wireless_ptr);
          pNodeTxGw->tx_state=NwkNodeTxGwFreqInit;//下一步            
        }
      }
      else
      {
        printf("no found GateWay!\n");
        pNodeTxGw->tx_state=NwkNodeTxGwExit;
      }            
      break;
    }    
    case NwkNodeTxStaticInit://静态参数初始化
    {
      u8 sf=0, bw=0;
      nwk_get_static_channel4(pNodeTxGw->wireless_ptr, &sf, &bw);
      u8 freq_ptr=pNodeTxGw->pGateWay->base_freq_ptr + pNodeTxGw->wireless_ptr*NWK_GW_SPACE_FREQ;//计算频率序号,每根天线间隔2MHz
      pNodeTxGw->freq=NWK_GW_BASE_FREQ+freq_ptr*1000000;//目标天线监听频率   
      pNodeTxGw->sf=sf;      
      pNodeTxGw->bw=bw;      
      nwk_node_set_lora_param(pNodeTxGw->freq, sf, bw);
      printf("NwkNodeTxStaticInit P(%.2f, %d, %d)\n", pNodeTxGw->freq/1000000.f, pNodeTxGw->sf, pNodeTxGw->bw);
      u8 first_buff[20]={0};
      u8 first_len=0;
      first_buff[first_len++]=0xA7;
      first_buff[first_len++]=g_sNwkNodeWork.node_sn>>24;
      first_buff[first_len++]=g_sNwkNodeWork.node_sn>>16;
      first_buff[first_len++]=g_sNwkNodeWork.node_sn>>8;
      first_buff[first_len++]=g_sNwkNodeWork.node_sn;
      first_buff[first_len++]=pNodeTxGw->tx_len;//发送长度
      first_buff[first_len++]=nwk_get_rand();//随机数1
      first_buff[first_len++]=nwk_get_rand();//随机数2  这里暂且明文发送
      nwk_node_send_buff(first_buff, first_len);//发送抢占包
      u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, first_len);//发送时间,冗余
      pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
      pNodeTxGw->wait_cnts=tx_time/1000+1;//等待秒数
      pNodeTxGw->tx_state=NwkNodeTxStaticFirstCheck;
      printf("send first buff! tx_time=%ums\n", tx_time);         
      break;
    }
    case NwkNodeTxStaticFirstCheck://发送抢占包检测
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("tx_first ok!\n");
        pNodeTxGw->freq=nwk_get_sn_freq(g_sNwkNodeWork.node_sn);//根据序列号计算频段
        nwk_node_set_lora_param(pNodeTxGw->freq, pNodeTxGw->sf, pNodeTxGw->bw);
        printf("into recv, wait ack, P(%.2f, %d, %d)\n", pNodeTxGw->freq/1000000.f, pNodeTxGw->sf, pNodeTxGw->bw);
        nwk_node_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, 20);//接收回复包等待时间
        pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeTxGw->wait_cnts=tx_time/1000+2;
        pNodeTxGw->tx_state=NwkNodeTxStaticFirstAck;		
      }
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//发送超时
      {
        printf("tx first time out! wait time=%ds\n", pNodeTxGw->wait_cnts);
        pNodeTxGw->tx_state=NwkNodeTxStaticExit;  
      }      
      break;
    }
    case NwkNodeTxStaticFirstAck://抢占包回复检测
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &g_sNwkNodeWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
        u8 *pBuff=g_sNwkNodeWork.node_rx.recv_buff;
				printf("first ack rssi=%ddbm, snr=%ddbm\n", g_sNwkNodeWork.rf_param.rssi, g_sNwkNodeWork.rf_param.snr);
				printf_hex("ack=", pBuff, recv_len);
        u8 head[1]={0xA7};
        u8 *pData=nwk_find_head(pBuff, recv_len, head, 1);
        if(pData)
        {
          pData+=1;
          u32 dst_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          printf("dst_sn=0x%08X\n", dst_sn);
          if(dst_sn==g_sNwkNodeWork.node_sn)
          {
            nwk_node_send_buff(make_buff, make_len);//发送数据包
            u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, make_len);//发送时间,冗余
            pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
            pNodeTxGw->wait_cnts=tx_time/1000+2;//等待秒数	
            pNodeTxGw->tx_state=NwkNodeTxStaticAppCheck; 
            printf("send app buff\n");
            printf_oled("tx app wire=%d\n", pNodeTxGw->wireless_ptr);		            
          }
          else
          {
            printf("sn error!\n");
            pNodeTxGw->tx_state=NwkNodeTxStaticExit;             
          }
        }
      }   
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//超时,可能没有抢占到
      {
				printf("wait first ack time out!\n");  
        pNodeTxGw->tx_state=NwkNodeTxStaticExit;
      }       
      break;
    }
    case NwkNodeTxStaticAppCheck://发送数据包检测
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("tx app ok, recv ack!\n");
        nwk_node_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, 20);//接收回复包等待时间
        pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeTxGw->wait_cnts=tx_time/1000+2;
        pNodeTxGw->tx_state=NwkNodeTxStaticAppAck;		
      }
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//发送超时
      {
        printf("tx app time out! wait time=%ds\n", pNodeTxGw->wait_cnts);
        pNodeTxGw->tx_state=NwkNodeTxStaticExit;  
      }      
      break;
    }
    case NwkNodeTxStaticAppAck://等待网关回复确认
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &g_sNwkNodeWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
				printf("tx ack rssi=%ddbm, snr=%ddbm\n", g_sNwkNodeWork.rf_param.rssi, g_sNwkNodeWork.rf_param.snr);
        srand(g_sNwkNodeWork.rf_param.rssi);
				printf_hex("ack=", g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        nwk_node_recv_parse(g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        printf_oled("*tx ok! wire=%d", pNodeTxGw->wireless_ptr);
      }   
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//超时
      {
				printf("wait ack time out!\n");  
        pNodeTxGw->tx_state=NwkNodeTxGwExit;
      }      
      break;
    }
    case NwkNodeTxStaticExit://静态退出
    {
      pNodeTxGw->try_cnts++;
        printf("static try_cnts=%d\n", pNodeTxGw->try_cnts);        
      if(pNodeTxGw->try_cnts>=3)//结束发送
      {
        nwk_node_clear_tx();       
        pNodeTxGw->alarm_rtc_time=0xFFFFFFFF;//可以进入休眠
      }
      else
      {
        u32 now_time=nwk_get_rtc_counter();
        pNodeTxGw->wait_cnts=nwk_get_rand()%10;//随机延时,再次尝试
        pNodeTxGw->start_rtc_time=now_time;
        pNodeTxGw->alarm_rtc_time=now_time+pNodeTxGw->wait_cnts;//闹钟时间        
        if(pNodeTxGw->wireless_ptr>=pNodeTxGw->pGateWay->wireless_num)//结束发送
        {
          nwk_node_clear_tx();       
          pNodeTxGw->alarm_rtc_time=0xFFFFFFFF;//可以进入休眠
          printf_oled("static tx failed!");
        }
        else
        {
          printf("tx wait time=%ds\n", pNodeTxGw->wait_cnts);        
          printf_oled("tx wait time=%ds\n", pNodeTxGw->wait_cnts);             
        }
        printf("static alarm time=%us\n", pNodeTxGw->alarm_rtc_time);
      }
      nwk_node_set_led(false);//指示灯灭
      pNodeTxGw->tx_state=NwkNodeTxGwIdel;//回合结束
      break;
    }
    
    //动态步骤
    case NwkNodeTxGwFreqInit://频段初始化
    { 
      if(pNodeTxGw->group_id>=NWK_UP_CHANNEL_NUM)//所有通道都无法到达 NWK_RF_GROUP_NUM
      {
        pNodeTxGw->pGateWay->err_cnts++;
        pNodeTxGw->tx_state=NwkNodeTxGwExit;//退出,重新开始
        printf("gw_sn=%08X, err_cnts=%d\n", pNodeTxGw->pGateWay->gw_sn, pNodeTxGw->pGateWay->err_cnts);
      }
      else
      {
        u8 freq_ptr=pNodeTxGw->pGateWay->base_freq_ptr + pNodeTxGw->wireless_ptr*NWK_GW_SPACE_FREQ;//计算频率序号,每根天线间隔2MHz
        pNodeTxGw->freq=NWK_GW_BASE_FREQ+freq_ptr*1000000;//目标天线监听频率
				pNodeTxGw->sniff_cnts=0;
				pNodeTxGw->tx_state=NwkNodeTxGwSniff;//进行嗅探       
      }
      break;
    }
    case NwkNodeTxGwSniff://嗅探
    {
      u8 sf=0, bw=0;
      nwk_get_up_channel(pNodeTxGw->group_id, &sf, &bw);//嗅探参数
      nwk_node_set_lora_param(pNodeTxGw->freq, sf, bw);
			printf("start sniff!\n");
			for(u8 i=0; i<10; i++)
			{
				nwk_node_send_sniff(sf, bw);//发送嗅探帧
			}
      nwk_node_cad_init();//状态切换,停止嗅探
      pNodeTxGw->sniff_cnts++;
      printf("***group id=%d, send sniff (%.2f, %d, %d), cnts=%d\n", pNodeTxGw->group_id, pNodeTxGw->freq/1000000.0, sf, bw, pNodeTxGw->sniff_cnts);
      u32 freq=pNodeTxGw->freq+1000000;
      nwk_node_set_lora_param(freq, sf, bw);
      nwk_node_cad_init(); //开始监听返回
			pNodeTxGw->cad_cnts=0;
      pNodeTxGw->sf=sf;
      pNodeTxGw->bw=bw; //记录监听参数,发送时候再次使用   
      printf("***wait cad ack (%.2f, %d, %d)\n", freq/1000000.0, sf, bw);
      for(u8 i=0; i<10; )//CAD回复检测次数
      {
        u8 result=nwk_node_cad_check();
        if(result==CadResultFailed)//没搜索到
        {
          nwk_node_cad_init();//继续监听
//          printf("111 cad init\n");
          i++;
        }
        else if(result==CadResultSuccess)//搜索成功
        {
          printf("************cad ack!\n");
          nwk_delay_ms(300);//适当延时,等待对方嗅探帧发送完
          pNodeTxGw->tx_step=0;
          u8 will_buff[5]={0};
          u8 will_len=0;
          will_buff[will_len++]=0xA8;
          will_buff[will_len++]=make_len;
          u16 crcValue=nwk_crc16(will_buff, 2);
          will_buff[will_len++]=crcValue>>8;
          will_buff[will_len++]=crcValue;//这里暂且明文发送
          
          nwk_node_send_buff(will_buff, will_len);//发送长度包
          u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, will_len);//发送时间,冗余
          pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
          pNodeTxGw->wait_cnts=tx_time/1000+1;//等待秒数
          pNodeTxGw->tx_state=NwkNodeTxGwRunning;
          printf("send will buff\n");       
          return;          
        }
      }
      //没有检测到CAD
      if(pNodeTxGw->sniff_cnts<10-pNodeTxGw->group_id)//同一组参数嗅探多次
      {
        pNodeTxGw->tx_state=NwkNodeTxGwSniff;//继续嗅探
      }
      else//超时
      { 
        pNodeTxGw->sniff_cnts=0;
        pNodeTxGw->tx_state=NwkNodeTxGwExit;//退出        
      }      
      break;
    }    
    case NwkNodeTxGwRunning://发送任务
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result>0)//发送完成
      {
        printf("tx_step=%d tx ok!\n", pNodeTxGw->tx_step);
				if(pNodeTxGw->tx_step>0)
				{
          nwk_node_recv_init();//进入接收,等待回复
          printf("into recv, wait ack, P(%.2f, %d, %d)\n", pNodeTxGw->freq/1000000.f+1, pNodeTxGw->sf, pNodeTxGw->bw);
					u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, 20);//接收回复包等待时间
					pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
					pNodeTxGw->wait_cnts=tx_time/1000+2;
					pNodeTxGw->tx_state=NwkNodeTxGwAck;	          
				}
				else//准备发送正式数据包
				{
					pNodeTxGw->tx_step++;
					nwk_node_send_buff(make_buff, make_len);//发送数据包
					u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, make_len);//发送时间,冗余
					pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
					pNodeTxGw->wait_cnts=tx_time/1000+3;//等待秒数	
					printf("send app buff\n");
          printf_oled("tx app group_id=%d\n", pNodeTxGw->group_id);					
				}
      }
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pNodeTxGw->wait_cnts);
        pNodeTxGw->tx_state=NwkNodeTxGwExit;  
      }
      break;
    }
    case NwkNodeTxGwAck://等待网关回复确认
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &g_sNwkNodeWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
				printf("tx ack rssi=%ddbm, snr=%ddbm\n", g_sNwkNodeWork.rf_param.rssi, g_sNwkNodeWork.rf_param.snr);
        srand(g_sNwkNodeWork.rf_param.rssi);
				printf_hex("ack=", g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        nwk_node_recv_parse(g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        printf_oled("*tx ok! id=%d,w=%d", pNodeTxGw->group_id, pNodeTxGw->wireless_ptr);
      }   
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//超时
      {
				printf("wait ack time out!\n");  
        pNodeTxGw->tx_state=NwkNodeTxGwExit;
      }      
      break;
    }
    case NwkNodeTxGwExit://动态退出
    {
      pNodeTxGw->try_cnts++;
        printf("try_cnts=%d\n", pNodeTxGw->try_cnts);        
      if(pNodeTxGw->try_cnts>=3)//结束发送
      {
        nwk_node_clear_tx();       
        pNodeTxGw->alarm_rtc_time=0xFFFFFFFF;//可以进入休眠
        
        NwkNodeEventStruct *pEvent=&g_sNwkNodeWork.event;
        pEvent->event=NwkNodeEventTxGwFailed; 
      }
      else
      {
        u32 now_time=nwk_get_rtc_counter();
        pNodeTxGw->wait_cnts=nwk_get_rand()%10;//随机延时,再次尝试
        pNodeTxGw->start_rtc_time=now_time;
        pNodeTxGw->alarm_rtc_time=now_time+pNodeTxGw->wait_cnts;//闹钟时间
        pNodeTxGw->group_id+=nwk_get_rand()%3+1;
        if(pNodeTxGw->group_id>=NWK_UP_CHANNEL_NUM-1)
        {
          pNodeTxGw->group_id=NWK_UP_CHANNEL_NUM-1;
        }
        printf("tx wait time=%ds\n", pNodeTxGw->wait_cnts);        
        printf_oled("tx wait time=%ds\n", pNodeTxGw->wait_cnts);  
        printf("dynamic alarm time=%us\n", pNodeTxGw->alarm_rtc_time);
      }
      nwk_node_set_led(false);//指示灯灭
      pNodeTxGw->tx_state=NwkNodeTxGwIdel;//回合结束
      break;
    }
    case NwkNodeTxGwWaitDownCheck://下行数据包接收
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &g_sNwkNodeWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
				printf("down rssi=%ddbm, snr=%ddbm\n", g_sNwkNodeWork.rf_param.rssi, g_sNwkNodeWork.rf_param.snr);
        srand(g_sNwkNodeWork.rf_param.rssi);
				printf_hex("down=", g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        nwk_node_recv_parse(g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
        if(pNodeRx->ack_len>0)
        {
          //发送回复包
          nwk_node_send_buff(pNodeRx->ack_buff, pNodeRx->ack_len); 
          u32 tx_time=nwk_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, pNodeRx->ack_len);//等待时间
          pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
          pNodeTxGw->wait_cnts=tx_time/1000+1;             
          pNodeTxGw->tx_state=NwkNodeTxGwAckDownCheck;//回复包发送检测           
        }
        else
        {
          printf("down none, exit!\n");
          pNodeRx->rx_state=NwkNodeTxGwIdel;//结束本回合 
        }	        
      }   
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//超时
      {
				printf("wait down time out!\n");  
        pNodeTxGw->tx_state=NwkNodeTxGwIdel;//回合结束
      }       
      break;
    }
    case NwkNodeTxGwAckDownCheck://下行数据包回复
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成
      {
				printf("app ack send ok!\n");
        pNodeTxGw->tx_state=NwkNodeTxGwIdel;//结束本回合
      }
      else if(now_time-pNodeTxGw->start_rtc_time>pNodeTxGw->wait_cnts)//发送超时
      {
				printf("app ack send time out!\n");
        pNodeTxGw->tx_state=NwkNodeTxGwIdel;//结束本回合  
      }         
      break;
    }    
  }
  
}

/*		
================================================================================
描述 : 节点发送到D2D进程
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_tx_d2d_process(void)
{
  static bool led_state=false;
  nwk_node_set_led(led_state);
  led_state=!led_state;
  
  NwkNodeTxD2dStruct *pNodeTxD2d=&g_sNwkNodeWork.node_tx_d2d;
  static u8 make_buff[255]={0};  //缓冲区
  static const u8 make_size=(u8)sizeof(make_buff);
  static u8 make_len=0;
  
  switch(pNodeTxD2d->d2d_state)
  {
    case NwkNodeTxD2dIdel: 
    {

      break;
    }
    case NwkNodeTxD2dInit: 
    {
      u8 key_type=KeyTypeRoot;
      u8 *pKey=g_sNwkNodeWork.root_key;
      u8 encrypt_mode=EncryptTEA;
      u8 opt=NwkRoleNode | (encrypt_mode<<2) | (key_type<<4);//组合配置
      
      //组合发送数据
      u8 pack_num=nwk_get_rand()%255;
      make_len=nwk_node_make_send_buff(opt, pNodeTxD2d->dst_sn, pKey, NwkCmdDataOnce, pack_num, pNodeTxD2d->tx_buff, pNodeTxD2d->tx_len, make_buff, make_size);
      printf_hex("make buff=", make_buff, make_len);       
      
      pNodeTxD2d->curr_sf=NWK_BROAD_SF;
      pNodeTxD2d->curr_bw=NWK_BROAD_BW;   
      pNodeTxD2d->freq =nwk_get_sn_freq(pNodeTxD2d->dst_sn);     
      nwk_node_set_lora_param(pNodeTxD2d->freq, pNodeTxD2d->curr_sf, pNodeTxD2d->curr_bw);

      printf("***set wake param (%.2f, %d, %d)\n", pNodeTxD2d->freq/1000000.0, pNodeTxD2d->curr_sf, pNodeTxD2d->curr_bw);
      pNodeTxD2d->sniff_cnts=0; 
      pNodeTxD2d->d2d_state=NwkNodeTxD2dWake;//进行唤醒 
      break;
    }
    case NwkNodeTxD2dWake: 
    {
      if(pNodeTxD2d->wake_period>0)
      {
        printf("d2d send wake!\n");
        for(u16 i=0; i<100; i++)
        {
          nwk_node_send_sniff(pNodeTxD2d->curr_sf, pNodeTxD2d->curr_bw);//发送嗅探帧,唤醒设备
        }        
      }
      u8 test_buff[10]={0};
      u8 test_len=0;
      u32 dst_sn=pNodeTxD2d->dst_sn; 
      test_buff[test_len++]=0xA9;
      test_buff[test_len++]=dst_sn>>24;
      test_buff[test_len++]=dst_sn>>16;
      test_buff[test_len++]=dst_sn>>8;
      test_buff[test_len++]=dst_sn;
      test_buff[test_len++]=pNodeTxD2d->tx_len;
      u16 crcValue=nwk_crc16(test_buff, test_len);
      test_buff[test_len++]=crcValue>>8;
      test_buff[test_len++]=crcValue;
      
      nwk_node_cad_init();//状态切换
      printf("d2d send sn buff!\n");
      nwk_node_send_buff(test_buff, test_len);//发送SN匹配包
      u32 tx_time=nwk_calcu_air_time(pNodeTxD2d->curr_sf, pNodeTxD2d->curr_bw, pNodeTxD2d->tx_len);//发送时间,冗余
      pNodeTxD2d->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
      pNodeTxD2d->wait_cnts=tx_time/1000+2;//等待秒数
      pNodeTxD2d->d2d_state=NwkNodeTxD2dSnCheck;  
      break;
    }
    case NwkNodeTxD2dSnCheck: 
    {
      u32 now_time=nwk_get_sec_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成,进入ADR模式
      {
        printf("d2d send sn ok!\n");
        pNodeTxD2d->sniff_cnts=0;
        if(pNodeTxD2d->try_cnts==0)
        {
          pNodeTxD2d->group_id=nwk_get_rand()%2;
        }
        else
        {
          pNodeTxD2d->group_id+=(nwk_get_rand()%2+1);
          u8 max_group=NWK_DOWN_CHANNEL_NUM-1;
          if(pNodeTxD2d->group_id>=max_group)
          {
            pNodeTxD2d->group_id=max_group;
          }
        }
        
        pNodeTxD2d->d2d_state=NwkNodeTxD2dAdrSniffInit; 
      }
      else if(now_time-pNodeTxD2d->start_rtc_time>pNodeTxD2d->wait_cnts)//发送超时
      {
        printf("d2d tx sn time out! wait time=%ds\n", pNodeTxD2d->wait_cnts);
        pNodeTxD2d->d2d_state=NwkNodeTxD2dExit;  
      }  
      break;
    }
    case NwkNodeTxD2dAdrSniffInit: 
    { 
      if(pNodeTxD2d->group_id>=NWK_DOWN_CHANNEL_NUM)
      {
        printf("d2d exit!\n");
        pNodeTxD2d->d2d_state=NwkNodeTxD2dExit;  //退出
        return;
      }
      u8 sf=0, bw=0;
      u32 freq=pNodeTxD2d->freq;
      nwk_get_down_channel(pNodeTxD2d->group_id, &sf, &bw); //嗅探参数
      nwk_node_set_lora_param(freq, sf, bw);
			 
			for(u8 i=0; i<5; i++)//
			{
				nwk_node_send_sniff(sf, bw);//发送嗅探帧
			}
      pNodeTxD2d->sniff_cnts++;
      printf("***d2d group id=%d, send sniff (%.2f, %d, %d), cnts=%d\n", pNodeTxD2d->group_id, freq/1000000.f, sf, bw, pNodeTxD2d->sniff_cnts);
//      nwk_get_down_channel(pSlaveTx->group_id, &sf, &bw);//以本通道组的第二个参数作为CAD监听参数
      
      freq+=1000000;
      nwk_node_set_lora_param(freq, sf, bw);
      nwk_node_cad_init(); //开始监听返回
      printf("d2d cad ack P(%.2f, %d, %d)\n", freq/1000000.f, sf, bw);
			pNodeTxD2d->cad_cnts=0;
      pNodeTxD2d->curr_sf=sf;
      pNodeTxD2d->curr_bw=bw; //记录监听参数,发送时候再次使用            
      for(u8 i=0; i<6; )//CAD回复检测次数
      {
        u8 result=nwk_node_cad_check(); 
        if(result==CadResultFailed)//没搜索到
        {
          nwk_node_cad_init();//继续监听
//          printf("111 cad init\n");
          i++;
        }
        else if(result==CadResultSuccess)//搜索成功
        {
          printf("****d2d CAD OK!\n");
          nwk_delay_ms(200);//适当延时,等待对方嗅探帧发送完
          printf("d2d send app!\n");
          nwk_node_send_buff(make_buff, make_len);//发送数据包
          u32 tx_time=nwk_calcu_air_time(pNodeTxD2d->curr_sf, pNodeTxD2d->curr_bw, make_len);//发送时间,冗余
          pNodeTxD2d->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
          pNodeTxD2d->wait_cnts=tx_time/1000+2;//等待秒数
          pNodeTxD2d->d2d_state=NwkNodeTxD2dRunning;     
          return;          
        }        
      }
      //没有检测到CAD
      if(pNodeTxD2d->sniff_cnts<5)//同一组参数嗅探多次
      {
        pNodeTxD2d->d2d_state=NwkNodeTxD2dAdrSniffInit;//继续嗅探
      }
      else
      {
        pNodeTxD2d->group_id++;//换下一组参数 
        pNodeTxD2d->sniff_cnts=0;
        pNodeTxD2d->d2d_state=NwkNodeTxD2dAdrSniffInit;//继续嗅探          
      }
      break;
    } 
    case NwkNodeTxD2dRunning: 
    {
      u32 now_time=nwk_get_sec_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("d2d send app ok!   \nwait ack!\n");
        nwk_node_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_calcu_air_time(pNodeTxD2d->curr_sf, pNodeTxD2d->curr_bw, 20);//接收回复包等待时间
        pNodeTxD2d->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
        pNodeTxD2d->wait_cnts=tx_time/1000+1;
        pNodeTxD2d->d2d_state=NwkNodeTxD2dWaitAck;
      }
      else if(now_time-pNodeTxD2d->start_rtc_time>pNodeTxD2d->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("d2d tx buff time out! wait time=%ds\n", pNodeTxD2d->wait_cnts);
        pNodeTxD2d->d2d_state=NwkNodeTxD2dExit;  
      }   
      break;
    }  
    case NwkNodeTxD2dWaitAck: 
    {
      u32 now_time=nwk_get_sec_counter();
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &g_sNwkNodeWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
        srand(g_sNwkNodeWork.rf_param.rssi);
        printf_hex("d2d ack=", g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        nwk_node_recv_parse(g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        printf_oled("*d2d tx ok! id=%d", pNodeTxD2d->group_id);        
        
      }   
      else if(now_time-pNodeTxD2d->start_rtc_time>pNodeTxD2d->wait_cnts)//超时
      {
        printf("d2d wait ack time out!\n");
        
        pNodeTxD2d->d2d_state=NwkNodeTxD2dExit; 
      }
      break;
    }  
    case NwkNodeTxD2dExit: 
    {
      pNodeTxD2d->try_cnts++;
        printf("try_cnts=%d\n", pNodeTxD2d->try_cnts);        
      if(pNodeTxD2d->try_cnts>=3)//结束发送
      {
        nwk_node_clear_d2d();       
        pNodeTxD2d->alarm_rtc_time=0xFFFFFFFF;//可以进入休眠
        NwkNodeEventStruct *pEvent=&g_sNwkNodeWork.event;
        pEvent->event=NwkNodeEventTxD2DFailed;         
      }
      nwk_node_set_led(false);//指示灯灭
      pNodeTxD2d->d2d_state=NwkNodeTxD2dIdel;//回合结束      
      break;
    }
  }
}

/*		
================================================================================
描述 : 睡眠进程
输入 : 
输出 : 
================================================================================
*/ 


/*		
================================================================================
描述 : 工作状态检测
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_work_check(void)
{
  static NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
  static NwkNodeTxD2dStruct *pNodeD2d=&g_sNwkNodeWork.node_tx_d2d;  
  static NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
  static NwkNodeSearchStruct *pNodeSearch=&g_sNwkNodeWork.node_search;
  
	switch(g_sNwkNodeWork.work_state)
	{
		case NwkNodeWorkIdel://空闲 
		{
      //网关数据检查
      if(pNodeTxGw->tx_len>0)//是否有网关数据需要发送
      {
        u32 now_time=nwk_get_rtc_counter();
        if(now_time-pNodeTxGw->start_rtc_time>=pNodeTxGw->wait_cnts)//随机延时结束
        {
          printf("tx gw wait ok!  time=%us\n", now_time);
          pNodeTxGw->alarm_rtc_time=0;
          pNodeTxGw->tx_state=NwkNodeTxGwInit;//重新开始
          g_sNwkNodeWork.work_state=NwkNodeWorkTXGw;
        }         
      }			
      //D2D数据检查
      if(g_sNwkNodeWork.work_state==NwkNodeWorkIdel)//仍旧空闲--D2D
      {
        if(pNodeD2d->tx_len>0)//是否有D2D数据需要发送
        {
          u32 now_time=nwk_get_rtc_counter();
          if(pNodeD2d->wake_period==0)
            pNodeD2d->wake_period=1;
          if((now_time+1)%pNodeD2d->wake_period==0)//唤醒周期到 提前1秒
          {
            printf("tx d2d wait ok! time=%us\n", now_time);
            pNodeD2d->alarm_rtc_time=0;
            pNodeD2d->d2d_state=NwkNodeTxD2dInit;//开始
            g_sNwkNodeWork.work_state=NwkNodeWorkTXD2d;         
          }
        }
      }
      if(g_sNwkNodeWork.work_state==NwkNodeWorkIdel)//仍旧空闲--入网检查
      {
				u32 now_time=nwk_get_rtc_counter();
        for(u8 i=0; i<NWK_GW_NUM; i++)
        {
          NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
          if(pGateWay->gw_sn>0)
          {
            if(pGateWay->wait_join_time==0)
            {
              pGateWay->wait_join_time=nwk_get_rand()%10+3;//首次入网等待时间
              pGateWay->last_join_time=now_time;
            }          
            if(pGateWay->join_state==JoinStateNone && 
               now_time-pGateWay->last_join_time>pGateWay->wait_join_time)
            {
              printf("pGateWay=0x%08X, join_state=%d\n", pGateWay->gw_sn, pGateWay->join_state);          
              pGateWay->wait_join_time=nwk_get_rand()%60+60;
              pGateWay->last_join_time=now_time;
              nwk_node_req_join(pGateWay->gw_sn);//请求入网
              return;
            }            
          }
        }        
      }      
      //搜索检查
      if(g_sNwkNodeWork.work_state==NwkNodeWorkIdel)//仍旧空闲--搜索
      {
        u32 now_time=nwk_get_rtc_counter();
        if(pNodeSearch->search_start_time==0  || //起始
           now_time<TIME_STAMP_THRESH ||  //时间未同步
          (pNodeSearch->period>0 && now_time%pNodeSearch->period==0) )
        {
          pNodeSearch->search_start_time=now_time;
          pNodeSearch->search_state=NwkNodeSearchInit;
          g_sNwkNodeWork.work_state=NwkNodeWorkSearch;//进入搜索状态             
        }
        
      } 
      //接收检查
      if(g_sNwkNodeWork.work_state==NwkNodeWorkIdel)//仍旧空闲--接收
      {
        if(g_sNwkNodeWork.wake_period==0)//无需休眠
        {
          pNodeRx->alarm_rtc_time=0;//不休眠
          pNodeRx->rx_state=NwkNodeRxInit;
          g_sNwkNodeWork.work_state=NwkNodeWorkRX;  //进入接收监听      
        }
        else if(g_sNwkNodeWork.wake_period==0xFFFF)//无需监听
        {
          pNodeRx->alarm_rtc_time=0xFFFFFFFF;//不唤醒
        }
        else
        {
          static u32 last_wake_time=0;//上次唤醒时间,避免单次周期内重复唤醒
          u32 now_time=nwk_get_rtc_counter();
          if(now_time%g_sNwkNodeWork.wake_period==0 && now_time!=last_wake_time)
          {
						printf("wake!!!  time=%us\n", now_time);
//						printf_oled("wake!!!  time=%us\n", now_time);
            pNodeRx->rx_state=NwkNodeRxInit;
            pNodeRx->alarm_rtc_time=now_time+g_sNwkNodeWork.wake_period;//下一次唤醒周期
      			g_sNwkNodeWork.work_state=NwkNodeWorkRX;//进入接收监听  
            last_wake_time=now_time;
          }
        }        
      }

    
			break;
		}
    
    /******************************/
		case NwkNodeWorkSearch://搜索  1
		{
			nwk_node_search_process();
      if(pNodeSearch->search_state==NwkNodeSearchIdel)//单回合结束
      {
        g_sNwkNodeWork.work_state=NwkNodeWorkIdel;
      }       
			break;
		}
		case NwkNodeWorkRX://接收   2
		{
			nwk_node_rx_process();
      if(pNodeRx->rx_state==NwkNodeRxIdel)//单回合结束
      {
        g_sNwkNodeWork.work_state=NwkNodeWorkIdel;
      }      
			break;
		}
		case NwkNodeWorkTXGw://发送到网关  3
		{
			nwk_node_tx_gw_process();
      if(pNodeTxGw->tx_state==NwkNodeTxGwIdel)//单回合结束
      {
        g_sNwkNodeWork.work_state=NwkNodeWorkIdel;
      }
			break;
		}    
		case NwkNodeWorkTXD2d://发送到设备   4
		{
			nwk_node_tx_d2d_process();
      if(pNodeD2d->d2d_state==NwkNodeTxD2dIdel)//单回合结束
      {
        g_sNwkNodeWork.work_state=NwkNodeWorkIdel;  
      }
			break;
		}       
	}
}


/*		
================================================================================
描述 : 应用层查询是否有数据
输入 : 
输出 : 
================================================================================
*/ 
NwkNodeRecvFromStruct *nwk_node_recv_from_check(void)
{
  NwkNodeRecvFromStruct *pRecvFrom=&g_sNwkNodeWork.recv_from;
  if(pRecvFrom->read_flag==true)
  {
    pRecvFrom->read_flag=false;
    return pRecvFrom;
  }
  return NULL;
}

/*		
================================================================================
描述 : 事件检测
输入 : 
输出 : 
================================================================================
*/ 
NwkNodeEventStruct *nwk_node_event_check(void)
{
  NwkNodeEventStruct *pEvent=&g_sNwkNodeWork.event;
  if(pEvent->event>0)
  {
    return pEvent;
  }
  return NULL;
}


/*		
================================================================================
描述 : 获取RF参数
输入 : 
输出 : 
================================================================================
*/ 
RfParamStruct *nwk_node_take_rf_param(void)
{
  return &g_sNwkNodeWork.rf_param;
}

/*		
================================================================================
描述 : 获取TX参数
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_take_tx_cnts(u16 *total_cnts, u16 *ok_cnts)
{
  *total_cnts=g_sNwkNodeWork.node_tx_gw.tx_total_cnts;
  *ok_cnts=g_sNwkNodeWork.node_tx_gw.tx_ok_cnts;
}

/*		
================================================================================
描述 : 节点唤醒时间计算
输入 : 
输出 : //0~不休眠  time~唤醒时间点 FFFFFFFF~全休眠
================================================================================
*/ 
u32 nwk_node_calcu_alarm_time(void)
{
  u32 min_alarm_time=0xFFFFFFFF;  //全休眠
  if(g_sNwkNodeWork.work_state!=NwkNodeWorkIdel)//有任务在运行
  {
    return 0;//不休眠
  }
  if(g_sNwkNodeWork.wake_period==0)
  {
    return 0;//不休眠
  }
  u32 curr_alarm_time=g_sNwkNodeWork.node_tx_gw.alarm_rtc_time;//发送线程闹钟
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }
  
  curr_alarm_time=g_sNwkNodeWork.node_tx_d2d.alarm_rtc_time;//D2D线程闹钟
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }  

  curr_alarm_time=g_sNwkNodeWork.node_search.alarm_rtc_time;//搜索线程闹钟
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }

  curr_alarm_time=g_sNwkNodeWork.node_rx.alarm_rtc_time;//接收线程闹钟
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }    
  return min_alarm_time;
}


/*		
================================================================================
描述 : 节点运行主函数
输入 : 
输出 :
================================================================================
*/ 
void nwk_node_main_process(void)
{
	nwk_node_work_check();
}











