
/*
	组网协议中的节点部分

*/


#include "nwk_node.h" 

//NwkNodeSaveStruct g_sNwkNodeSave={0};
NwkNodeWorkStruct g_sNwkNodeWork={0};
/*		
================================================================================
描述 : 节点参数读取
输入 : 
输出 : 
================================================================================
*/ 
//void nwk_node_read(void)
//{
//	nwk_eeprom_read((u8*)&g_sNwkNodeSave, sizeof(g_sNwkNodeSave));
//	if(g_sNwkNodeSave.crc_value!=nwk_crc16((u8*)&g_sNwkNodeSave, sizeof(g_sNwkNodeSave)-2))
//	{
//		memset((u8*)&g_sNwkNodeSave, 0, sizeof(g_sNwkNodeSave));
//		nwk_node_save();
//	}
//}


/*		
================================================================================
描述 : 节点参数存储
输入 : 
输出 : 
================================================================================
*/ 
//void nwk_node_save(void)
//{
//	g_sNwkNodeSave.crc_value=nwk_crc16((u8*)&g_sNwkNodeSave, sizeof(g_sNwkNodeSave)-2);
//	nwk_eeprom_save((u8*)&g_sNwkNodeSave, sizeof(g_sNwkNodeSave));
//}


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
//	nwk_node_save();
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
描述 : 配网网关参数
输入 : 
输出 : 
================================================================================
*/ 
//void nwk_node_set_parent(NwkParentSaveStruct *parent)
//{
//	memcpy(&g_sNwkNodeSave.parent_save, parent, sizeof(NwkParentSaveStruct));
//	nwk_node_save();
//}

void nwk_node_add_gw(u32 gw_sn, u8 base_freq, u8 wireless_num)
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

#elif defined(LORA_LLCC68)

#endif	
	
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
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
		drv_sx1278_set_op_mode( g_sNwkNodeWork.pLoRaDev, RFLR_OPMODE_SLEEP );
	
#elif defined(LORA_SX1268)

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
u8 nwk_node_recv_check(u8 *buff, int16_t *rssi)
{
	u8 read_size=0;
	if(g_sNwkNodeWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	
	read_size=drv_sx1278_recv_check(g_sNwkNodeWork.pLoRaDev, buff); 
	if(read_size>0)
	{
		*rssi=drv_sx1278_read_rssi(g_sNwkNodeWork.pLoRaDev);
	}
#elif defined(LORA_SX1268)

#elif defined(LORA_LLCC68)

#endif		
	return read_size;
}

/*		
================================================================================
描述 : 发送时长计算
输入 : 
输出 : 
================================================================================
*/ 
u32 nwk_node_calcu_air_time(u8 sf, u8 bw, u16 data_len)
{
  u32 tx_time=0;
#if defined(LORA_SX1278)  
	
	tx_time=drv_sx1278_calcu_air_time(sf, bw, data_len);
#elif defined(LORA_SX1268)

#elif defined(LORA_LLCC68)

#endif	
return tx_time;  
}

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
  u32 tx_time=nwk_node_calcu_air_time(sf, bw, 1);
  u8 loops=(tx_time*0.1)/2+5;//计算循环次数
#if defined(LORA_SX1278)  
	drv_sx1278_send(g_sNwkNodeWork.pLoRaDev, buff, 1); 
	while(loops--)
  {
    nwk_delay_ms(2);
  }
#elif defined(LORA_SX1268)

#elif defined(LORA_LLCC68)

#endif	
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
			if(pGateWay && pGateWay->join_state==JoinStateOK && key_type==KeyTypeApp)
			{
				pKey=pGateWay->app_key;//入网后要用应用密码
        pGateWay->rssi=g_sNwkNodeWork.recv_rssi;//更新信号强度
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
			pData=union_buff;
			union_len=pData[0];
			u16 crcValue=pData[union_len]<<8|pData[union_len+1];
			if(nwk_crc16(union_buff, union_len)==crcValue)
			{
				pData+=1;
				u8 dst_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
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
				if(pGateWay && pack_num==pGateWay->down_pack_num)
				{
					printf("gw_sn=0x%08X have same down_pack_num=%d\n", src_sn, pack_num);
					return;
				}
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
            if(ack_cmd==NwkCmdDataOnce)//单包数据回复
            {
              //清理发送数据
              if(role==NwkRoleGateWay)//网关
              {
                NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
                memset(pNodeTxGw->tx_buff, 0, sizeof(pNodeTxGw->tx_buff));
                pNodeTxGw->tx_len=0;
                printf("clear tx gw buff!\n");
                //同时返回时间戳
                u32 ack_time=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
                pData+=4;
                if(ack_time>NWK_TIME_STAMP_THRESH && nwk_get_rtc_counter()!=ack_time)
                {
                  nwk_set_rtc_counter(ack_time);//更新RTC时间
                }
                u8 key_flag=pData[0];
                pData+=1;
                if(key_flag)//更新APP_KEY标志
                {
                  u8 key_tmp[16]={0x45,0xEF,0x09,0x3E,0xA2,0xC8,0xB1,0x4A,0x90,0x75,0xD9,0x63,0x7B,0x3B,0x82,0x96};//解算密码,应用时注意混淆
                  nwk_tea_encrypt(pData, 16, (u32 *)key_tmp);//把网关下发的16字节随机数进行加密作为APP_KEY
                  memcpy(pGateWay->app_key, pData, 16); 
                  pGateWay->join_state=JoinStateOK;//入网成功                  
                }
              }
              else if(role==NwkRoleNode)
              {
                NwkNodeTxD2dStruct *pNodeD2d=&g_sNwkNodeWork.node_tx_d2d;
                memset(pNodeD2d->tx_buff, 0, sizeof(pNodeD2d->tx_buff));
                pNodeD2d->tx_len=0;                
                printf("clear tx d2d buff!\n");
              }
            }
						break;
					}					
//					case NwkCmdJoin://入网
//					{
//						printf("src_sn=0x%08X NwkCmdJoin!\n", src_sn);
//            if(pGateWay)
//            {
//              u8 key_tmp[16]={0x45,0xEF,0x09,0x3E,0xA2,0xC8,0xB1,0x4A,0x90,0x75,0xD9,0x63,0x7B,0x3B,0x82,0x96};//解算密码,应用时注意混淆
//              nwk_tea_encrypt(pData, 16, (u32 *)key_tmp);//把网关下发的16字节随机数进行加密作为APP_KEY
//              memcpy(pGateWay->app_key, pData, 16); 
//              pGateWay->join_state=JoinStateOK;//入网成功
//            }
//						break;
//					}
					case NwkCmdDataOnce://单包数据
					{
						printf("src_sn=0x%08X NwkCmdDataOnce!\n", src_sn);
            NwkNodeRecvFromStruct *pRecvFrom=&g_sNwkNodeWork.recv_from;
            pRecvFrom->data_len=union_len-7;//应用数据长度
            pRecvFrom->app_data=pData;
            pRecvFrom->src_sn=src_sn;
            pRecvFrom->read_flag=true;//通知应用层读取
            
            //需要回复NwkCmdAck指令    
            NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
            u8 opt=NwkRoleNode | (encrypt_mode<<2) | (key_type<<4);//组合配置
            //组合回复包
            u8 tmp_buff[1]={NwkCmdDataOnce};
            pNodeRx->ack_len=nwk_node_make_send_buff(opt, src_sn, pKey, NwkCmdAck, nwk_get_rand()%255, tmp_buff, 1, pNodeRx->ack_buff, sizeof(pNodeRx->ack_buff));
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
	//网关质量:N信号*0.5+N(1-负载)*0.3+N(5-失败次数)*0.2   N表示归一化
  
  for(u8 i=0; i<NWK_GW_NUM; i++)
  {
    NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
    if(pGateWay->gw_sn>0)
    {
      return pGateWay;
    }
  }  
  
	return NULL;
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
    return 0;
  }
  
  memcpy(pNodeTxGw->tx_buff, in_buff, in_len);
  pNodeTxGw->tx_len=in_len;
  pNodeTxGw->tx_cmd=NwkCmdDataOnce;
  pNodeTxGw->try_cnts=0;
	printf("send to gw!\n");
  return 0;
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
  pNodeTxGw->try_cnts=0;
}

/*		
================================================================================
描述 : 把数据发送到节点(D2D)
输入 : 
输出 : 
================================================================================
*/
u8 nwk_node_send2node(u32 dst_node_sn, u8 *in_buff, u8 in_len)
{
  
  return 0;
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
  static u8 ptr=0;
  static const u32 base_freq=NWK_BEACON_BASE_FREQ;
  static const u8 sf=12, bw=NWK_MIN_BW;
  NwkNodeSearchStruct *pSearch=&g_sNwkNodeWork.node_search;
  switch(pSearch->search_state)
  {
    case NwkNodeSearchIdel: //空闲
    {
      
      break;
    }
    case NwkNodeSearchCadInit: //CAD初始化
    {
			u32 freq=base_freq+ptr*1000000;
//			printf("**search param (%d, %d, %d)\n", freq/1000000, sf, bw);
      nwk_node_set_lora_param(freq, sf, bw);
      nwk_node_cad_init();
      pSearch->search_state=NwkNodeSearchCadCheck;
      break;
    }   
    case NwkNodeSearchCadCheck: //CAD检测
    {
      u8 result=nwk_node_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
        ptr++;
        if(ptr>=4)//搜索4个频段        
        { 
          ptr=0;
        }
				u32 now_time=nwk_get_rtc_counter();
				if(now_time-pSearch->search_start_time>5)
				{
					printf("**** search exit!\n");
					pSearch->search_state=NwkNodeSearchIdel;//退出
				}
				else
				{
					pSearch->search_state=NwkNodeSearchCadInit;//进行下一个频段搜索
				}
        
      }
      else if(result==CadResultSuccess)//搜索成功 
      {
				printf("search into recv mode!\n");
        nwk_node_recv_init();//进入接收模式
        pSearch->search_state=NwkNodeSearchRxCheck;
      }
      break;
    }
    case NwkNodeSearchRxCheck: //接收检测
    {
      int16_t rssi;
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &rssi);
      if(recv_len>0)
      {
				printf("rssi=%ddbm\n", rssi);
				printf_hex("search recv=", g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        nwk_tea_decrypt(g_sNwkNodeWork.node_rx.recv_buff, recv_len, (u32*)g_sNwkNodeWork.root_key);//解密
        //数据解析,存储网关信息
        //A5 gw_sn freq  wireless
        u8 head[1]={0xA5};
        u8 *pData=nwk_find_head(g_sNwkNodeWork.node_rx.recv_buff, recv_len, head, 1);
        if(pData)
        {
          u32 gw_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          u8 base_freq=pData[0];
          pData+=1;
          u8 wireless_num=pData[0];
          pData+=1;
          
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
                pGateWay->rssi=rssi;
                printf("add gw_sn=0x%08X, base_freq=%d, wireless=%d\n", gw_sn, base_freq, wireless_num);
//                nwk_node_req_join(gw_sn);//请求入网
                break;
              }
            }            
          }
        }
        pSearch->search_state=NwkNodeSearchExit;
      }
      else
      {
        u32 now_time=nwk_get_rtc_counter();
        if(now_time-pSearch->search_start_time>11)//搜索时间到
        {
          pSearch->search_state=NwkNodeSearchIdel;
        }        
      }
      break;
    }    
    case NwkNodeSearchExit:
    {
      u32 now_time=nwk_get_rtc_counter();
      if(now_time-pSearch->search_start_time>11)//搜索时间到
      {
        pSearch->search_state=NwkNodeSearchIdel;
      }
      else
      {
        ptr++;
        if(ptr>=4)//搜索4个频段        
        { 
          ptr=0;
        }
        pSearch->search_state=NwkNodeSearchCadInit;//进行下一个频段搜索        
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
  NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
  switch(pNodeRx->rx_state)
  {
    case NwkNodeRxIdel:
    {
      
      break;
    }
    case NwkNodeRxInit:
    {
      u16 freq_cnts=(NWK_MAX_FREQ-NWK_MIN_FREQ)/1000000*2;
      pNodeRx->freq=(nwk_crc16((u8*)&g_sNwkNodeWork.node_sn, 4)%freq_cnts)*500000+NWK_MIN_FREQ;//根据序列号计算频段  
      pNodeRx->rx_state=NwkNodeRxCadInit;
      pNodeRx->listen_cnts=0;
      break;
    }
    case NwkNodeRxCadInit:
    {
      if(pNodeRx->listen_cnts==0 && pNodeRx->sf1>0 && pNodeRx->bw1>0)//上一次通讯参数
      {
        pNodeRx->curr_sf=pNodeRx->sf1;
        pNodeRx->curr_bw=pNodeRx->bw1;
        
      }
      else//最远通讯参数
      {
        pNodeRx->curr_sf=12;
        pNodeRx->curr_bw=NWK_MIN_BW;        
      }
//			printf("cad param=(%d, %d, %d)\n", pNodeRx->freq/1000000, pNodeRx->curr_sf, pNodeRx->curr_bw);
      nwk_node_set_lora_param(pNodeRx->freq, pNodeRx->curr_sf, pNodeRx->curr_bw);
      nwk_node_cad_init(); 
      pNodeRx->rx_state=NwkNodeRxCadCheck;
      break;
    }    
    case NwkNodeRxCadCheck:
    {
      u8 result=nwk_node_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
//        printf("rx no cad!\n");
        pNodeRx->listen_cnts++;
        if(pNodeRx->listen_cnts>=2)//两个通道监测结束
        {
          pNodeRx->rx_state=NwkNodeRxIdel;
        }
        else
        {
          pNodeRx->rx_state=NwkNodeRxCadInit;//进行12,6参数监听
        }
      }
      else if(result==CadResultSuccess)//搜索到
      {
//        printf("rx cad ok!\n");
        nwk_node_send_sniff(pNodeRx->curr_sf, pNodeRx->curr_bw);//发送嗅探帧
        nwk_node_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_node_calcu_air_time(pNodeRx->curr_sf, pNodeRx->curr_bw, NWK_TRANSMIT_MAX_SIZE);//接收等待时间
        pNodeRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeRx->wait_cnts=tx_time/1000+1;
        pNodeRx->rx_state=NwkNodeRxCheck;         
      }       
      break;
    }
    case NwkNodeRxCheck:
    {
      u32 now_time=nwk_get_rtc_counter();
      int16_t rssi;
      u8 recv_len=nwk_node_recv_check(pNodeRx->recv_buff, &rssi);
      if(recv_len>0)
      {
        //数据解析
        g_sNwkNodeWork.recv_rssi=rssi;
        nwk_node_recv_parse(pNodeRx->recv_buff, recv_len);
        if(pNodeRx->ack_len>0)//有回复包
        {
          nwk_node_send_buff(pNodeRx->ack_buff, pNodeRx->ack_len);//发送
          u16 tx_time=nwk_node_calcu_air_time(pNodeRx->curr_sf, pNodeRx->curr_bw, pNodeRx->ack_len);
          pNodeRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
          pNodeRx->wait_cnts=tx_time/1000+1;
          pNodeRx->rx_state=NwkNodeRxAck;
        }
        else
        {
          pNodeRx->rx_state=NwkNodeRxIdel;
        }
      }   
      else if(now_time-pNodeRx->start_rtc_time>=pNodeRx->wait_cnts)//超时
      {
        pNodeRx->rx_state=NwkNodeRxIdel;
      }       
      break;
    }
    case NwkNodeRxAck://回复发送检测
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成
      {
        pNodeRx->rx_state=NwkNodeRxIdel;//结束本回合
      }
      else if(now_time-pNodeRx->start_rtc_time>=pNodeRx->wait_cnts)//发送超时
      {
        pNodeRx->rx_state=NwkNodeRxIdel;//结束本回合  
      }      
      break;
    }
  }
}  


/*		
================================================================================
描述 : 节点主动发送到网关进程
输入 : 
输出 : 
================================================================================
*/ 
void nwk_node_tx_gw_process(void)
{
  NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
  static u8 *pMakeBuff=g_sNwkNodeWork.node_rx.recv_buff;  //接收缓冲区复用
  static const u8 make_size=(u8)sizeof(g_sNwkNodeWork.node_rx.recv_buff);
  static u8 make_len=0;
  switch(pNodeTxGw->tx_state)
  {
    case NwkNodeTxGwIdel:
    {
      break;
    }
    case NwkNodeTxGwInit://参数初始化
    {
      //1.选择最优网关; 2.根据网关的RSSI大致确定通道参数
      NwkParentWorkStrcut *pGateWay=NULL;
      if(pNodeTxGw->tx_cmd==NwkCmdJoin)
      {
        pGateWay=nwk_node_search_gw(pNodeTxGw->join_sn);//选择要入网的网关
      }
      else
      {
        pGateWay=nwk_node_select_gw();//选择最优网关
      }
      
      if(pGateWay)
      {
				pNodeTxGw->pGateWay=pGateWay;
        u8 key_type=KeyTypeRoot;
        u8 *pKey=g_sNwkNodeWork.root_key;
        if(pGateWay->join_state==JoinStateOK)
        {
          key_type=KeyTypeApp;
          pKey=pGateWay->app_key;//有应用密码尽量用应用密码
        }
        u8 encrypt_mode=NWK_NODE_USE_ENCRYPT_MODE;
        u8 opt=NwkRoleNode | (encrypt_mode<<2) | (key_type<<4);//组合配置
        
        //组合发送数据
        make_len=nwk_node_make_send_buff(opt, pGateWay->gw_sn, pKey, pNodeTxGw->tx_cmd, pGateWay->up_pack_num, pNodeTxGw->tx_buff, pNodeTxGw->tx_len, pMakeBuff, make_size);
				printf_hex("make buff=", pMakeBuff, make_len);        
        pNodeTxGw->chn_ptr=0;//通道组,从0开始,也可以根据信号强度选择较高的通道组
        pNodeTxGw->wireless_ptr=0;
        if(pNodeTxGw->pGateWay->wireless_num>0)
        {
          pNodeTxGw->wireless_ptr=nwk_get_rand()%pNodeTxGw->pGateWay->wireless_num;//随机选择天线
					printf("wireless_ptr=%d\n", pNodeTxGw->wireless_ptr);
        }
        pNodeTxGw->tx_state=NwkNodeTxGwLBTInit;//下一步      
      }
      else
      {
        printf("no found GateWay!\n");
        pNodeTxGw->tx_state=NwkNodeTxGwExit;
      }            
      break;
    }    
    case NwkNodeTxGwLBTInit://传输前监听
    { 
      if(pNodeTxGw->chn_ptr>=NWK_RF_GROUP_NUM)//所有通道都无法到达 NWK_RF_GROUP_NUM
      {
        pNodeTxGw->pGateWay->err_cnts++;
        pNodeTxGw->tx_state=NwkNodeTxGwExit;//退出,重新开始
        printf("gw_sn=%08X, err_cnts=%d\n", pNodeTxGw->pGateWay->gw_sn, pNodeTxGw->pGateWay->err_cnts);
      }
      else
      {
        u8 freq_ptr=pNodeTxGw->pGateWay->base_freq_ptr + pNodeTxGw->wireless_ptr*3+pNodeTxGw->chn_ptr;//计算频率序号
        u8 sf=0, bw=0;
        nwk_get_channel(pNodeTxGw->chn_ptr*3, &sf, &bw);
        pNodeTxGw->freq=NWK_GW_BASE_FREQ+freq_ptr*500000;//目标天线监听频率,间隔2M
        nwk_node_set_lora_param(pNodeTxGw->freq, sf, bw);
//        nwk_node_cad_init(); 
//        pNodeTxGw->tx_state=NwkNodeTxGwLBTCheck;
				pNodeTxGw->sniff_cnts=0;
				pNodeTxGw->tx_state=NwkNodeTxGwSniffInit;//进行嗅探
        printf("tx LBT init!\n");        
      }
      break;
    }
    case NwkNodeTxGwLBTCheck://监听检测
    {
      u8 result=nwk_node_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
        pNodeTxGw->sniff_cnts=0;
        pNodeTxGw->tx_state=NwkNodeTxGwSniffInit;//进行嗅探
        printf("no LBT!\n");
      }
      else if(result==CadResultSuccess)//搜索到,有设备正在传输
      {
        //退出,随机延时,下次再嗅探
        printf("have LBT, exit!\n");
        pNodeTxGw->tx_state=NwkNodeTxGwExit;
      }      
      break;
    }
    case NwkNodeTxGwSniffInit://嗅探
    {
      u8 sf=0, bw=0;
      nwk_get_channel(pNodeTxGw->chn_ptr*3, &sf, &bw); //嗅探参数
      nwk_node_set_lora_param(pNodeTxGw->freq, sf, bw);
			 
			for(u8 i=0; i<8-pNodeTxGw->chn_ptr; i++)
			{
//				printf("sniff_%d 000\n", i);	
				nwk_node_send_sniff(sf, bw);//发送嗅探帧
				nwk_node_cad_init();//状态切换
				delay_ms(5);
//				printf("sniff_%d 111\n", i);
			}
      pNodeTxGw->sniff_cnts++;
      printf("***group id=%d, send sniff (%.2f, %d, %d), cnts=%d\n", pNodeTxGw->chn_ptr, pNodeTxGw->freq/1000000.0, sf, bw, pNodeTxGw->sniff_cnts);
      nwk_get_channel(pNodeTxGw->chn_ptr*3+1, &sf, &bw);//以本通道组的第二个参数作为CAD监听参数
      nwk_node_set_lora_param(pNodeTxGw->freq, sf, bw);
      nwk_node_cad_init(); //开始监听返回
			pNodeTxGw->cad_cnts=0;
      pNodeTxGw->sf=sf;
      pNodeTxGw->bw=bw; //记录监听参数,发送时候再次使用      
      pNodeTxGw->tx_state=NwkNodeTxGwSniffCheck;
      break;
    }    
    case NwkNodeTxGwSniffCheck://嗅探返回检测
    {
      u8 result=nwk_node_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
        pNodeTxGw->cad_cnts++;
				if(pNodeTxGw->cad_cnts<50-pNodeTxGw->chn_ptr*5)
				{
					nwk_node_cad_init();//继续监听
				}
        else if(pNodeTxGw->sniff_cnts<10-pNodeTxGw->chn_ptr)//同一组参数嗅探多次,根据通道不同,次数不同
        {
          pNodeTxGw->tx_state=NwkNodeTxGwSniffInit;//继续嗅探
        }
        else
        {
          pNodeTxGw->chn_ptr++;//换下一组参数 
          pNodeTxGw->tx_state=NwkNodeTxGwLBTInit;//继续嗅探          
        }
      }
      else if(result==CadResultSuccess)//搜索成功 
      {
				printf("************cad ack!\n");
        nwk_delay_ms(pNodeTxGw->chn_ptr*150+400);//适当延时,等待对方准备好
				printf("tx len=%d\n", make_len);
        nwk_node_send_buff(pMakeBuff, make_len);//发送数据包
        u32 tx_time=nwk_node_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, make_len)*1.2;//发送时间,冗余
        pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeTxGw->wait_cnts=tx_time/1000+2;//等待秒数
        pNodeTxGw->tx_state=NwkNodeTxGwRunning;
      }      
      break;
    }
    case NwkNodeTxGwRunning:
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_node_send_check();//发送完成检测
      if(result)//发送完成
      {
				printf("tx ok!\n");
        nwk_node_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_node_calcu_air_time(pNodeTxGw->sf, pNodeTxGw->bw, 20);//接收回复包等待时间
        pNodeTxGw->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pNodeTxGw->wait_cnts=tx_time/1000+1;
        pNodeTxGw->tx_state=NwkNodeTxGwAck;
      }
      else if(now_time-pNodeTxGw->start_rtc_time>=pNodeTxGw->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pNodeTxGw->wait_cnts);
        pNodeTxGw->tx_state=NwkNodeTxGwExit;  
      }
      break;
    }
    case NwkNodeTxGwAck:
    {
      u32 now_time=nwk_get_rtc_counter();
      int16_t rssi;
      u8 recv_len=nwk_node_recv_check(g_sNwkNodeWork.node_rx.recv_buff, &rssi);
      if(recv_len>0)
      {
        //数据解析
        g_sNwkNodeWork.recv_rssi=rssi;
        nwk_node_recv_parse(g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
        pNodeRx->sf1=pNodeTxGw->sf;
        pNodeRx->bw1=pNodeTxGw->bw;//记录最优参数,接收时候使用
        
        pNodeTxGw->pGateWay->err_cnts=0;
        pNodeTxGw->tx_state=NwkNodeTxGwIdel;
      }   
      else if(now_time-pNodeTxGw->start_rtc_time>=pNodeTxGw->wait_cnts)//超时
      {
        pNodeTxGw->chn_ptr++;//换下一组参数 
        pNodeTxGw->tx_state=NwkNodeTxGwLBTInit;//继续嗅探   
      }      
      break;
    }
    
    case NwkNodeTxGwExit://退出
    {
      pNodeTxGw->try_cnts++;
        printf("try_cnts=%d\n", pNodeTxGw->try_cnts);        
      if(pNodeTxGw->try_cnts>=3)//结束发送
      {
        memset(pNodeTxGw->tx_buff, 0, sizeof(pNodeTxGw->tx_buff));
        pNodeTxGw->tx_len=0; 
        pNodeTxGw->try_cnts=0;        
        pNodeTxGw->alarm_rtc_time=0xFFFFFFFF;//不唤醒
        printf("clear tx buff!\n");        
      }
      else
      {
        u32 now_time=nwk_get_rtc_counter();
        pNodeTxGw->wait_cnts=nwk_get_rand()%20+3;//随机延时,再次尝试
        pNodeTxGw->start_rtc_time=now_time;
        pNodeTxGw->alarm_rtc_time=now_time+pNodeTxGw->wait_cnts;//闹钟时间
        printf("tx wait time=%ds\n", pNodeTxGw->wait_cnts);        
      }
      pNodeTxGw->tx_state=NwkNodeTxGwIdel;//空闲等待
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
  NwkNodeTxD2dStruct *pNodeTxD2d=&g_sNwkNodeWork.node_tx_d2d;
  switch(pNodeTxD2d->tx_state)
  {
    case NwkNodeTxD2dIdel: 
    {

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
  NwkNodeTxGwStruct *pNodeTxGw=&g_sNwkNodeWork.node_tx_gw;
  NwkNodeTxD2dStruct *pNodeD2d=&g_sNwkNodeWork.node_tx_d2d;  
  NwkNodeRxStruct *pNodeRx=&g_sNwkNodeWork.node_rx;
  NwkNodeSearchStruct *pNodeSearch=&g_sNwkNodeWork.node_search;
  
	switch(g_sNwkNodeWork.work_state)
	{
		case NwkNodeWorkIdel://空闲 
		{
      //网关数据检查
      if(pNodeTxGw->tx_len>0)//是否有网关数据需要发送
      {
        if(pNodeTxGw->wait_cnts>0)//有延时
        {
          u32 now_time=nwk_get_rtc_counter();
          if(now_time-pNodeTxGw->start_rtc_time>=pNodeTxGw->wait_cnts)//随机延时结束
          {
            printf("tx gw wait ok!\n");
            pNodeTxGw->alarm_rtc_time=0;
            pNodeTxGw->tx_state=NwkNodeTxGwInit;//重新开始
            g_sNwkNodeWork.work_state=NwkNodeWorkTXGw;
          }      
        }
        else//首次发送
        {
          pNodeTxGw->alarm_rtc_time=0;
          pNodeTxGw->tx_state=NwkNodeTxGwInit;//开始
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
          if(now_time%pNodeD2d->wake_period==0)//唤醒周期到
          {
            if(pNodeD2d->wait_cnts>0)//有延时
            {
              if(now_time-pNodeD2d->start_rtc_time>=pNodeD2d->wait_cnts)//随机延时结束
              {
                printf("tx d2d wait ok!\n");
                pNodeD2d->alarm_rtc_time=0;
                pNodeD2d->tx_state=NwkNodeTxD2dInit;//重新开始
                g_sNwkNodeWork.work_state=NwkNodeWorkTXGw;                
              }
            }
            else//首次发送
            {
              pNodeD2d->alarm_rtc_time=0;
              pNodeD2d->tx_state=NwkNodeTxD2dInit;//开始
              g_sNwkNodeWork.work_state=NwkNodeWorkTXGw;
            }            
          }
        }
      }
      //搜索检查
      if(g_sNwkNodeWork.work_state==NwkNodeWorkIdel)//仍旧空闲--搜索
      {
        //不具备有效网关就要搜索
        u8 gw_cnts=0;
        for(u8 i=0; i<NWK_GW_NUM; i++)
        {
          NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
          if(pGateWay->gw_sn>0)
          {
            if(pGateWay->rssi<0 && pGateWay->rssi>-140)
            {
              
            }
            gw_cnts++;
          }
        }
        u32 now_time=nwk_get_rtc_counter();
        static u32 wait_time=20;
        int det_time=now_time-pNodeSearch->search_start_time;
//				printf("det_time=%d\n", det_time);
        if(gw_cnts==0)// && det_time>wait_time
        {
          wait_time*=2;
          if(wait_time>86400)
          {
            wait_time=86400;
          }
          pNodeSearch->alarm_rtc_time=now_time+wait_time;
          pNodeSearch->search_start_time=now_time;
          pNodeSearch->search_state=NwkNodeSearchCadInit;
          g_sNwkNodeWork.work_state=NwkNodeWorkSearch;//进入搜索状态
          printf("*****start search!\n");
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
            pNodeRx->rx_state=NwkNodeRxInit;
      			g_sNwkNodeWork.work_state=NwkNodeWorkRX;//进入接收监听  
            last_wake_time=now_time;
          }
        }        
      }
//      if(g_sNwkNodeWork.work_state==NwkNodeWorkIdel)//仍旧空闲--入网检查
//      {
//        for(u8 i=0; i<NWK_GW_NUM; i++)
//        {
//          NwkParentWorkStrcut *pGateWay=&g_sNwkNodeWork.parent_list[i];
//          if(pGateWay->gw_sn>0 && pGateWay->join_state==JoinStateNone)
//          {
//            nwk_node_req_join(pGateWay->gw_sn);//请求入网
//            return;
//          }
//        }        
//      }
      //睡眠检查
      if(g_sNwkNodeWork.work_state==NwkNodeWorkIdel)//仍旧空闲--睡眠
      {
//        g_sNwkNodeWork.work_state=NwkNodeWorkSleep;//进入休眠
        //睡眠时间更新
        //1.网关数据延时发送 2.D2D发送  3.自身定时唤醒监听
        nwk_node_sleep_init();//
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
      if(pNodeD2d->tx_state==NwkNodeTxD2dIdel)//单回合结束
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
描述 : 节点运行主函数
输入 : 
输出 : //0~不休眠  time~唤醒节点 FFFFFFFF~全休眠
================================================================================
*/ 
NowkNodeReturnStruct *nwk_node_main(void)
{
  u32 min_alarm_time=0xFFFFFFFF;  
  NowkNodeReturnStruct *pReturn=&g_sNwkNodeWork.node_return;
	nwk_node_work_check();
  pReturn->pRecvFrom=nwk_node_recv_from_check();
  
  if(g_sNwkNodeWork.work_state!=NwkNodeWorkIdel)//有任务在运行
  {
    pReturn->alarm_time=0;
    return pReturn;//不休眠
  }
  if(g_sNwkNodeWork.wake_period==0)
  {
    pReturn->alarm_time=0;
    return pReturn;//不休眠
  }
  u32 curr_alarm_time=g_sNwkNodeWork.node_tx_gw.alarm_rtc_time;
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }
  
  curr_alarm_time=g_sNwkNodeWork.node_tx_d2d.alarm_rtc_time;
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }  

  curr_alarm_time=g_sNwkNodeWork.node_search.alarm_rtc_time;
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }

  curr_alarm_time=g_sNwkNodeWork.node_rx.alarm_rtc_time;
  if(curr_alarm_time>0 && curr_alarm_time<min_alarm_time)
  {
    min_alarm_time=curr_alarm_time;
  }  
  
  g_sNwkNodeWork.alarm_rtc_time=min_alarm_time;
  pReturn->alarm_time=min_alarm_time;
  
  return pReturn;
}











