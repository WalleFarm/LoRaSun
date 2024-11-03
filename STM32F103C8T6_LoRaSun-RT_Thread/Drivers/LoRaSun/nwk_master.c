
#include "nwk_master.h"


NwkMasterWorkStruct g_sNwkMasterWork={0};


/*		
================================================================================
描述 : 添加传输接口
输入 :  
输出 : 
================================================================================
*/ 
void nwk_master_uart_send_register(u8 index, u8 slave_adddr, void (*fun_send)(u8 *buff, u16 len))
{
  if(index<NWK_GW_WIRELESS_NUM)
  {
    NwkSlaveTokenStruct *pSlaveToken=&g_sNwkMasterWork.slave_token_list[index];
    pSlaveToken->fun_send=fun_send;
    pSlaveToken->slave_addr=slave_adddr;
  }
  g_sNwkMasterWork.gw_sn=0xC1011234;//测试
}

/*		
================================================================================
描述 : 发送时长计算
输入 : 
输出 : 
================================================================================
*/ 
u32 nwk_master_calcu_air_time(u8 sf, u8 bw, u16 data_len)
{
  u32 tx_time=0;
#if defined(LORA_SX1278)  
	
	tx_time=drv_sx1278_calcu_air_time(sf, bw, data_len);
#elif defined(LORA_SX1268)
  tx_time=drv_sx1268_calcu_air_time(sf, bw, data_len);
#elif defined(LORA_LLCC68)
 
#endif	
return tx_time;  
}

/*		
================================================================================
描述 : 
输入 :  
输出 : 
================================================================================
*/ 
void nwk_master_uart_parse(u8 *recv_buff, u16 recv_len)
{
//  printf_hex("recv0=", recv_buff, recv_len);
  u8 head[2]={0xAA, 0x55}; 
  u8 *pData=nwk_find_head(recv_buff, recv_len, head, 2);
  if(pData)
  {
    u16 data_len=pData[2]<<8|pData[3];
    u16 crcValue=pData[data_len]<<8|pData[data_len+1];
    if(nwk_crc16(pData, data_len)==crcValue)
    {
      pData+=4;
      u8 slave_addr=pData[0];
      pData+=1;
      u8 cmd_type=pData[0];
      pData+=1;
      printf("slave_addr=%d\n", slave_addr);
      switch(cmd_type)
      {
        case MSCmdHeart://
        {
          
          break;
        }
        case MSCmdSetFreqPtr://从机请求设置基础频率
        {
          nwk_master_set_freq_ptr(slave_addr);
          break;
        }        
        case MSCmdRxData://接收数据
        {
          printf("MSCmdRxData ###\n");
          RfParamStruct rf;
          rf.rssi=pData[0]<<8|pData[1];
          pData+=2;
          rf.freq=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          rf.sf=pData[0];
          pData+=1;
          rf.bw=pData[0];
          pData+=1; 
          u8 data_len=pData[0];
          pData+=1;
//          printf_hex("rx=", pData, data_len);
          nwk_master_lora_parse(pData, data_len, slave_addr, &rf);          
          break;
        }        
      }
    }
  }
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void nwk_master_uart_send_level(u8 slave_addr, u8 cmd_type, u8 *in_buff, u16 in_len)
{
  static u8 make_buff[300]={0};
  if(in_len+10>sizeof(make_buff))
    return ;
  
  NwkSlaveTokenStruct *pSlaveToken=nwk_master_find_slave(slave_addr);
  if(pSlaveToken==NULL)
    return;
  u16 make_len=0;
  u16 data_len=in_len+6;
  make_buff[make_len++]=0xAA;
  make_buff[make_len++]=0x55;
  make_buff[make_len++]=data_len>>8;
  make_buff[make_len++]=data_len;
  make_buff[make_len++]=slave_addr;
  make_buff[make_len++]=cmd_type;
  memcpy(&make_buff[make_len], in_buff, in_len);
  make_len+=in_len;
  u16 crcValue=nwk_crc16(make_buff, make_len);
  make_buff[make_len++]=crcValue>>8;
  make_buff[make_len++]=crcValue;

  if(pSlaveToken->fun_send)
  {
    pSlaveToken->fun_send(make_buff, make_len);
  }
}

/*		
================================================================================
描述 : LoRa组合发送数据
输入 : 
输出 : 
================================================================================
*/ 
u8 nwk_master_make_lora_buff(u8 opt, u32 dst_sn, u8 *key, u8 cmd_type, u8 pack_num, u8 *in_buff, u8 in_len, u8 *out_buff, u8 out_size)
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
	u32 src_sn=g_sNwkMasterWork.gw_sn;
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
#ifdef	NWK_MASTER_USE_AES	//是否使用AES加密		
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
描述 : LoRa数据接收解析
输入 : 
输出 : 
================================================================================
*/ 
void nwk_master_lora_parse(u8 *recv_buff, u8 recv_len, u8 slave_addr, RfParamStruct *rf)
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
    printf("src_sn=0x%08X, payload_len=%d\n", src_sn, payload_len);
		if(payload_len>sizeof(union_buff))
		{
			printf("error payload_len>NWK_TRANSMIT_MAX_SIZE!\n");
			return;
		}
    if(role==NwkRoleGateWay)
    { 
      printf("error role==NwkRoleGateWay!\n");
      return;
    }
    NwkNodeTokenStruct *pNodeToken=nwk_master_find_node(src_sn);
//    if(pNodeToken==NULL)
//    {
//      printf("error pNodeToken==NULL!\n");
//      return;
//    }
    if(pNodeToken)
    {
      pNodeToken->rssi=rf->rssi;//更新信号强度
      pNodeToken->snr=rf->snr;      
    }
		u8 *pKey=g_sNwkMasterWork.root_key;//默认根密码
    if(key_type==KeyTypeApp)
    {
      if(pNodeToken==NULL)
      {
        printf("no found node_sn=0x%08X, key_type==KeyTypeApp error!\n", src_sn);
        return;
      }        
      pKey=pNodeToken->app_key;//应用密码
//      printf_hex("use app key=", pKey, 16);
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
#ifdef	NWK_MASTER_USE_AES	//是否使用AES加密		
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
				u32 dst_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
				pData+=4;
				u8 cmd_type=pData[0];
				pData+=1;
				u8 pack_num=pData[0];
				pData+=1;
        printf("dst_sn=0x%08X, cmd_type=%d, pack_num=%d\n", dst_sn, cmd_type, pack_num);
				if(dst_sn != g_sNwkMasterWork.gw_sn && dst_sn!=0xFFFFFFFF)
				{
					printf("dst_sn=0x%08X != local_sn=0x%08X\n", dst_sn, g_sNwkMasterWork.gw_sn);
					return;
				}
				if(pack_num==pNodeToken->down_pack_num)
				{
					printf("gw_sn=0x%08X have same down_pack_num=%d\n", src_sn, pack_num);
//					return;
				}
        u32 now_time=nwk_get_rtc_counter();
        if(pNodeToken)
          pNodeToken->keep_time=now_time;//更新连接时间
        NwkSlaveTokenStruct *pNwkSlaveToken=nwk_master_find_slave(slave_addr);
        if(pNwkSlaveToken)
        {
          pNwkSlaveToken->counts++;//各天线接收次数统计
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
              NwkNodeTokenStruct *pNwkNode=nwk_master_find_node(src_sn);
              if(pNwkNode)
              {
                memset(pNwkNode->down_buff, 0, sizeof(pNwkNode->down_buff));
                pNwkNode->down_len=0;
                printf("clear down buff!\n");
              }
            }
						break;
					}					
					case NwkCmdJoin://入网
					{
						printf("src_sn=0x%08X NwkCmdJoin!\n", src_sn);
            u16 wake_period=pData[0]<<8|pData[1];
            pData+=2;
            printf("wake_period=%ds\n", wake_period);
            if(pNodeToken==NULL)
            {
              pNodeToken=nwk_master_add_node(src_sn);
            }  
            if(pNodeToken)
            {
              pNodeToken->join_state=JoinStateOK;//入网成功
              pNodeToken->wake_period=wake_period;
            }
						u8 key_tmp[16]={0x45,0xEF,0x09,0x3E,0xA2,0xC8,0xB1,0x4A,0x90,0x75,0xD9,0x63,0x7B,0x3B,0x82,0x96};//解算密码,应用时注意混淆						
            u8 opt=NwkRoleGateWay | (encrypt_mode<<2) | (KeyTypeRoot<<4);//组合配置
            //组合LoRa回复包
            u8 lora_buff[50]={0};
            u8 lora_len=0;						
            u32 tx_time=nwk_master_calcu_air_time(rf->sf, rf->bw, 16);
            u8 det_sec=tx_time/1000;
            if(tx_time%1000>500)det_sec+=1;//四舍五入
            now_time+=det_sec;//增加延时
            printf("det_sec=%ds\n", det_sec);
            lora_buff[lora_len++]=JoinStateAccept;//默认接受入网
            lora_buff[lora_len++]=now_time>>24;
            lora_buff[lora_len++]=now_time>>16;
            lora_buff[lora_len++]=now_time>>8;
            lora_buff[lora_len++]=now_time;			
						u8 app_key[16]={0};
						for(u8 i=0; i<16; i++)//生成随机应用密码种子
						{
							app_key[i]=nwk_get_rand()%255;
						}			
            printf_hex("key in=", app_key, 16);
						memcpy(&lora_buff[lora_len], app_key, 16); 
						lora_len+=16;						
						nwk_tea_encrypt(app_key, 16, (u32 *)key_tmp);//把16字节随机数进行加密作为APP_KEY
            if(pNodeToken)
            {
              memcpy(pNodeToken->app_key, app_key, 16); 
            }
						printf_hex("key out=", app_key, 16);
						//组合从机数据
            u8 uart_buff[100]={0};
            u8 uart_len=0;   
            uart_buff[uart_len++]=rf->freq>>24;
            uart_buff[uart_len++]=rf->freq>>16;
            uart_buff[uart_len++]=rf->freq>>8;
            uart_buff[uart_len++]=rf->freq;
            uart_buff[uart_len++]=rf->sf;
            uart_buff[uart_len++]=rf->bw;
            u8 make_buff[80]={0};
            u8 make_len=nwk_master_make_lora_buff(opt, src_sn, pKey, NwkCmdJoin, ++pNodeToken->down_pack_num, lora_buff, lora_len, make_buff, sizeof(make_buff));
            if(make_len>0)
            {
              uart_buff[uart_len++]=make_len;
              memcpy(&uart_buff[uart_len], make_buff, make_len);
              uart_len+=make_len;
              nwk_master_uart_send_level(slave_addr, MSCmdAckRxData, uart_buff, uart_len);//发送到从机
            }						
						break;
					}
					case NwkCmdDataOnce://单包数据
					{
						printf("src_sn=0x%08X NwkCmdDataOnce!\n", src_sn);
            NwkMasterRecvFromStruct *pRecvFrom=&g_sNwkMasterWork.recv_from;
            pRecvFrom->data_len=union_len-7;//应用数据长度
            memcpy(pRecvFrom->app_data, pData, pRecvFrom->data_len);
            pRecvFrom->src_sn=src_sn;
            pRecvFrom->read_flag=true;//通知应用层读取
            memcpy(&pRecvFrom->rf_param, rf, sizeof(RfParamStruct));//复制无线参数
            
            printf_hex("app_buff=", pRecvFrom->app_data, pRecvFrom->data_len);
            //需要回复NwkCmdAck指令    
            u8 opt=NwkRoleGateWay | (encrypt_mode<<2) | (key_type<<4);//组合配置
            //组合回复包
            u8 lora_buff[50]={0}; 
            u8 lora_len=0;
            u32 tx_time=nwk_master_calcu_air_time(rf->sf, rf->bw, 16);
            u8 det_sec=tx_time/1000;
            if(tx_time%1000>500)det_sec+=1;//四舍五入
            now_time+=det_sec;//增加延时          
            printf("det_sec=%ds\n", det_sec);            
            lora_buff[lora_len++]=NwkCmdDataOnce;
            lora_buff[lora_len++]=now_time>>24;
            lora_buff[lora_len++]=now_time>>16;
            lora_buff[lora_len++]=now_time>>8;
            lora_buff[lora_len++]=now_time;	
            
            u8 uart_buff[100]={0};
            u8 uart_len=0;   
            uart_buff[uart_len++]=rf->freq>>24;
            uart_buff[uart_len++]=rf->freq>>16; 
            uart_buff[uart_len++]=rf->freq>>8;
            uart_buff[uart_len++]=rf->freq;
            uart_buff[uart_len++]=rf->sf;
            uart_buff[uart_len++]=rf->bw;
            u8 make_buff[80]={0};
            u8 make_len=nwk_master_make_lora_buff(opt, src_sn, pKey, NwkCmdAck, ++pNodeToken->down_pack_num, lora_buff, lora_len, make_buff, sizeof(make_buff));
            if(make_len>0)
            {
              uart_buff[uart_len++]=make_len;
              memcpy(&uart_buff[uart_len], make_buff, make_len);
              uart_len+=make_len;
              nwk_master_uart_send_level(slave_addr, MSCmdAckRxData, uart_buff, uart_len);//发送到从机
            }
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
描述 : 发送广播
输入 : 
输出 : 
================================================================================
*/
void nwk_master_send_broad(u8 slave_addr, u32 freq, u8 sf, u8 bw) 
{
	u8 uart_buff[100]={0};
	u8 uart_len=0; 	
	uart_buff[uart_len++]=freq>>24;
	uart_buff[uart_len++]=freq>>16;
	uart_buff[uart_len++]=freq>>8;
	uart_buff[uart_len++]=freq;
	uart_buff[uart_len++]=sf;
	uart_buff[uart_len++]=bw;	
		
	
	u8 make_buff[20]={0};
	u8 make_len=0;
	u32 gw_sn=g_sNwkMasterWork.gw_sn;
	make_buff[make_len++]=0xA5;
	make_buff[make_len++]=gw_sn>>24;
	make_buff[make_len++]=gw_sn>>16;
	make_buff[make_len++]=gw_sn>>8;
	make_buff[make_len++]=gw_sn;
	make_buff[make_len++]=g_sNwkMasterWork.freq_ptr;//起始频段
	make_buff[make_len++]=NWK_GW_WIRELESS_NUM;//天线数量
	make_buff[make_len++]=nwk_get_rand();//保留
	make_len=nwk_tea_encrypt(make_buff, make_len, (u32 *)g_sNwkMasterWork.root_key);//加密广播数据
	if(make_len>0)
	{
		uart_buff[uart_len++]=make_len;
		memcpy(&uart_buff[uart_len], make_buff, make_len);
		uart_len+=make_len;
		nwk_master_uart_send_level(slave_addr, MSCmdBroad, uart_buff, uart_len);//发送到从机
		printf("nwk_master_send_broad ###\n");
	}	

}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void nwk_master_send_freq_ptr(u8 slave_addr)
{
	u8 uart_buff[50]={0};
	u8 uart_len=0; 	
  uart_buff[uart_len++]=g_sNwkMasterWork.freq_ptr;
  nwk_master_uart_send_level(slave_addr, MSCmdSetFreqPtr, uart_buff, uart_len);//发送到从机	
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void nwk_master_send_down_pack(u32 dst_sn, u8 slave_addr, u8 *in_buff, u8 in_len, u8 flag)
{
  NwkNodeTokenStruct *pNodeToken=nwk_master_find_node(dst_sn);
  if(pNodeToken==NULL)
  {
    printf("no found node!\n");
    return;
  }
  u8 key_type=KeyTypeRoot;
  u8 *pKey=g_sNwkMasterWork.root_key;
  if(pNodeToken->join_state==JoinStateOK)
  {
//    printf("no joined!\n");
//    return;    
    key_type=KeyTypeApp;
    pKey=pNodeToken->app_key;
    printf("down use app key!\n");
  }
  u8 opt=NwkRoleGateWay | (EncryptTEA<<2) | (key_type<<4);//组合配置
  
  static u8 uart_buff[300]={0};
  u8 uart_len=0;   

  static u8 make_buff[250]={0};
  u8 make_len=nwk_master_make_lora_buff(opt, dst_sn, pKey, NwkCmdDataOnce, ++pNodeToken->down_pack_num, in_buff, in_len, make_buff, sizeof(make_buff));
  if(make_len>0)
  {
    printf_hex("lora buff=", make_buff, make_len);
    uart_buff[uart_len++]=dst_sn>>24;
    uart_buff[uart_len++]=dst_sn>>16;
    uart_buff[uart_len++]=dst_sn>>8;
    uart_buff[uart_len++]=dst_sn;
    uart_buff[uart_len++]=flag;
    uart_buff[uart_len++]=make_len;
    memcpy(&uart_buff[uart_len], make_buff, make_len);
    uart_len+=make_len;
    nwk_master_uart_send_level(slave_addr, MSCmdTxData, uart_buff, uart_len);//发送到从机
  } 
  
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
NwkSlaveTokenStruct *nwk_master_find_slave(u8 slave_addr)
{
  for(u8 i=0; i<NWK_GW_WIRELESS_NUM; i++)
  {
    NwkSlaveTokenStruct *pSlaveToken=&g_sNwkMasterWork.slave_token_list[i];
    if(pSlaveToken->slave_addr==slave_addr)
    {
      return pSlaveToken;
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
void nwk_master_set_root_key(u8 *key)
{
  memcpy(g_sNwkMasterWork.root_key, key, 16);
}

/*		
================================================================================
描述 : 设置基础频率序号
输入 : 
输出 : 
================================================================================
*/ 
void nwk_master_set_freq_ptr(u8 freq_ptr)
{
  g_sNwkMasterWork.freq_ptr=freq_ptr;
}

/*		
================================================================================
描述 : 添加节点记录
输入 : 
输出 : 
================================================================================
*/ 
NwkNodeTokenStruct *nwk_master_add_node(u32 node_sn)
{
  if(g_sNwkMasterWork.node_cnts<NWK_NODE_MAX_NUM)
  {
    NwkNodeTokenStruct *pNodeToken=(NwkNodeTokenStruct *)malloc(sizeof(NwkNodeTokenStruct));
    if(pNodeToken)
    {
      memset(pNodeToken, 0, sizeof(NwkNodeTokenStruct));
      pNodeToken->node_sn=node_sn;
      pNodeToken->next=NULL;
      if(g_sNwkMasterWork.pNodeHead==NULL)
      {
        g_sNwkMasterWork.pNodeHead=pNodeToken;
      }
      else
      {
        NwkNodeTokenStruct *pTemp=g_sNwkMasterWork.pNodeHead;
        while(pTemp->next)//寻找最后一个节点
        {
          pTemp=pTemp->next;
        }
        pTemp->next=pNodeToken;
      }
      g_sNwkMasterWork.node_cnts++;
      printf("add node token ok!\n");
      return pNodeToken;
    }
  }
  printf("add node token failed!\n");
  return NULL;
}

/*		
================================================================================
描述 : 查找节点记录
输入 : 
输出 : 
================================================================================
*/ 
NwkNodeTokenStruct *nwk_master_find_node(u32 node_sn)
{
  NwkNodeTokenStruct *pTemp=g_sNwkMasterWork.pNodeHead;
  while(pTemp)
  {
    if(pTemp->node_sn==node_sn)
    {
      return pTemp;
    }
    pTemp=pTemp->next;
  }
  return NULL;
}

/*		
================================================================================
描述 : 删除节点记录
输入 : 
输出 : 
================================================================================
*/ 
void nwk_master_del_node(u32 node_sn)
{
  NwkNodeTokenStruct *pTemp1=g_sNwkMasterWork.pNodeHead;
  NwkNodeTokenStruct *pTemp2=pTemp1;
  
  while(pTemp1)
  {   
    if(pTemp1->node_sn==node_sn)
    {
      if(pTemp1==g_sNwkMasterWork.pNodeHead)//链表头是否为删除点
      {
        g_sNwkMasterWork.pNodeHead=pTemp1->next;
      }
      else
      {
        pTemp2->next=pTemp1->next;//链表断开重连
      }
      g_sNwkMasterWork.node_cnts--;
      memset(pTemp1, 0, sizeof(NwkNodeTokenStruct));
      free(pTemp1);
      return;
    }
    pTemp2=pTemp1;
    pTemp1=pTemp1->next;
  }  
}


/*		
================================================================================
描述 : 添加下行包
输入 : 
输出 : 
================================================================================
*/ 
u8 nwk_master_add_down_pack(u32 dst_sn, u8 *in_buff, u8 in_len)
{
  NwkNodeTokenStruct *pNwkNode=nwk_master_find_node(dst_sn);
  if(pNwkNode)
  {
    if(pNwkNode->down_len==0)
    {
      memcpy(pNwkNode->down_buff, in_buff, in_len);
      pNwkNode->down_len=in_len;
      printf("add down pack ok!\n");
      return 1;
    }
    else
    {
      printf("have down buff!\n");
    }
  }
  else
  {
    printf("no found dst_sn=0x%08X\n", dst_sn);
  }
  return 0;
}

/*		
================================================================================
描述 : 下行包发送检测
输入 : 
输出 : 
================================================================================
*/
void nwk_master_check_down_pack(void)
{
  NwkNodeTokenStruct *pTemp=g_sNwkMasterWork.pNodeHead;
  u32 now_time=nwk_get_rtc_counter();
  while(pTemp)
  {
    if(pTemp->down_len>0 && now_time-pTemp->down_time>10)//避免连续发送
    {
      u16 period=pTemp->wake_period;
      bool send_flag=false;
      u8 awake_flag=1;//是否需要唤醒操作
      if(period==0)//不休眠,随时发送
      {
        send_flag=true;
        awake_flag=0;
      }
      else if(period==0xFFFF)//不唤醒,上行时才能发送
      {
        
      }
      else
      {
        if((now_time)%period==0)
        {
          send_flag=true;
        }
      }  
      if(send_flag)
      {
        pTemp->down_time=now_time;
        u8 slave_addr=nwk_get_rand()%NWK_GW_WIRELESS_NUM+1;
        printf(">>>down tx node_sn=0x%08X, slave_addr=%d\n", pTemp->node_sn, slave_addr);
        nwk_master_send_down_pack(pTemp->node_sn, slave_addr, pTemp->down_buff, pTemp->down_len, awake_flag);        
      }
    }

    pTemp=pTemp->next;
  }  
}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
NwkMasterRecvFromStruct *nwk_master_recv_from_check(void)
{
  NwkMasterRecvFromStruct *pRecvFrom=&g_sNwkMasterWork.recv_from;
  if(pRecvFrom->read_flag)
  {
    pRecvFrom->read_flag=false;
    return pRecvFrom;
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
void nwk_master_main(void)
{
  static u32 last_sec_time=0;
	u32 now_sec_time=nwk_get_sec_counter();
	nwk_master_check_down_pack();//下行数据包检测
	if(now_sec_time-last_sec_time>=10)
	{
//		nwk_master_send_broad(1, NWK_BROAD_BASE_FREQ, NWK_BROAD_SF, NWK_BROAD_BW);//广播
    u32 sum=1;
    for(u8 i=0; i<NWK_GW_WIRELESS_NUM; i++)
    {
      NwkSlaveTokenStruct *pNwkSlave=&g_sNwkMasterWork.slave_token_list[i];
      sum+=pNwkSlave->counts;
    }
    printf("sum counts=%d\n", sum);
    for(u8 i=0; i<NWK_GW_WIRELESS_NUM; i++)
    {
      NwkSlaveTokenStruct *pNwkSlave=&g_sNwkMasterWork.slave_token_list[i];
      printf("slave addr=%d, counts=%d, percent=%.1f%%\n", i+1, pNwkSlave->counts, pNwkSlave->counts*100.f/sum);
    }    
		last_sec_time=now_sec_time;
	}
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



/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 















