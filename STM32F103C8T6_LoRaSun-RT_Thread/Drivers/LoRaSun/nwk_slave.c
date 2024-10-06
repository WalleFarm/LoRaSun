
#include "nwk_slave.h"

NwkSlaveSaveStruct g_sNwkSlaveSave={0};
NwkSlaveWorkStruct g_sNwkSlaveWork={0};

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_uart_parse(u8 *recv_buff, u16 recv_len)
{
  u8 head[2]={0xAA, 0x55};
  u8 *pData=nwk_find_head(recv_buff, recv_len, head, 2);
  if(pData)
  {
    u16 data_len=pData[2]<<8|pData[3];
    u16 crcValue=pData[data_len]<<8|pData[data_len+1];
    if(nwk_crc16(pData, data_len)==crcValue || true)
    {
      pData+=4;
      u8 slave_addr=pData[0];
      pData+=1;
      u8 cmd_type=pData[0];
      pData+=1;
      if(slave_addr!=g_sNwkSlaveSave.slave_addr && slave_addr!=0xFF)
      {
        return;
      }
      switch(cmd_type)
      {
        case MSCmdHeart://主机下发的同步信号,根据地址码间隔(slave-1)*10ms发送数据
        {
          g_sNwkSlaveWork.ack_tickets=0;
          break;
        }
        case MSCmdBroad://设置广播信息
        {
          NwkSlaveBroadStruct *pSlaveBroad=&g_sNwkSlaveWork.slave_broad;
          pSlaveBroad->freq=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          pSlaveBroad->sf=pData[0];
          pData+=1;
          pSlaveBroad->bw=pData[0];
          pData+=1;          
          u8 broad_len=pData[0];
					pData+=1;
          if(broad_len>sizeof(pSlaveBroad->broad_buff))
          {
            printf("error broad_len=%d\n", broad_len);
            return;
          }
          memcpy(pSlaveBroad->broad_buff, pData, broad_len);
          pSlaveBroad->broad_len=broad_len;
					printf_hex("broad_buff=", pSlaveBroad->broad_buff, broad_len);
          break;
        }
        case MSCmdAckData://确认数据已经收到
        {
          memset(g_sNwkSlaveWork.ack_buff, 0, sizeof(g_sNwkSlaveWork.ack_buff));
          g_sNwkSlaveWork.ack_len=0;
          break;
        }
        case MSCmdTxData://发送数据
        {
          NwkSlaveTxStruct *pSlaveTx=&g_sNwkSlaveWork.slave_tx;
          u32 dst_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          pSlaveTx->try_cnts=pData[0];
          if(pSlaveTx->try_cnts>2)
          {
            printf("error try_cnts=%d\n", pSlaveTx->try_cnts);
            return;
          }
          pData+=1;
          for(u8 i=0; i<pSlaveTx->try_cnts; i++)
          {
            pSlaveTx->sfs[i]=pData[0];
            pData+=1;   
            pSlaveTx->bws[i]=pData[0];
            pData+=1;            
          }
          u8 tx_len=pData[0];
          pData+=1;
          if(tx_len<sizeof(pSlaveTx->tx_buff))
          {  
            if(pSlaveTx->tx_len==0)
            {
              pSlaveTx->tx_len=tx_len;
              memcpy(pSlaveTx->tx_buff, pData, tx_len);
              u16 freq_cnts=(NWK_MAX_FREQ-NWK_MIN_FREQ)/1000000*2;
              pSlaveTx->freq=(nwk_crc16((u8*)&dst_sn, 4)%freq_cnts)*500000+NWK_MIN_FREQ;//根据序列号计算频段  
              printf("dst_sn=0x%08X, (%.2f, %d, %d)\n", dst_sn, pSlaveTx->freq/1000000.f, pSlaveTx->sfs[0], pSlaveTx->bws[0]);              
            }
            else
            {
              printf("Buffer is full!");
            }
          }
          else
          {
            printf("tx_len=%d too long!\n", tx_len);
          }
          break;
        } 
        case MSCmdRxData://回复LoRa接收数据
        {
          RfParamStruct rf;
          rf.freq=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          rf.sf=pData[0];
          pData+=1;
          rf.bw=pData[0];
          pData+=1;
          u8 data_len=pData[0];
          pData+=1;
          nwk_slave_set_lora_param(rf.freq, rf.sf, rf.bw);
          nwk_slave_send_buff(pData, data_len);
          NwkSlaveRxStruct *pSlaveRx=&g_sNwkSlaveWork.slave_rx;
          u32 tx_time=nwk_slave_calcu_air_time(rf.sf, rf.bw, data_len)*1.2;//发送时间,冗余
          pSlaveRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
          pSlaveRx->wait_cnts=tx_time/1000+1;//等待秒数
          pSlaveRx->rx_state=NwkSlaveRxAckCheck;     
          g_sNwkSlaveWork.work_state=NwkSlaveWorkRX;
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
void nwk_slave_uart_send_level(u8 cmd_type, u8 *in_buff, u16 in_len)
{
  static u8 make_buff[300]={0};
  if(in_len+10>sizeof(make_buff))
    return ;
  u16 make_len=0;
  u16 data_len=in_len+6;
  make_buff[make_len++]=0xAA;
  make_buff[make_len++]=0x55;
  make_buff[make_len++]=data_len>>8;
  make_buff[make_len++]=data_len;
  make_buff[make_len++]=g_sNwkSlaveSave.slave_addr;
  make_buff[make_len++]=cmd_type;
  memcpy(&make_buff[make_len], in_buff, in_len);
  make_len+=in_len;
  u16 crcValue=nwk_crc16(make_buff, make_len);
  make_buff[make_len++]=crcValue>>8;
  make_buff[make_len++]=crcValue;

  if(g_sNwkSlaveWork.fun_send)
  {
    g_sNwkSlaveWork.fun_send(make_buff, make_len);
  }
}

/*		
================================================================================
描述 : 给主机发送心跳包
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_send_heart(void)
{
  nwk_slave_uart_send_level(MSCmdHeart, NULL, 0);
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_send_rx(u8 *buff, u8 len, RfParamStruct *rf)
{
  u8 in_buff[300]={0};
  u16 in_len=0;
  
  in_buff[in_len++]=rf->rssi>>8;
  in_buff[in_len++]=rf->rssi; 
  in_buff[in_len++]=rf->freq>>24;
  in_buff[in_len++]=rf->freq>>16;
  in_buff[in_len++]=rf->freq>>8;
  in_buff[in_len++]=rf->freq;  
  in_buff[in_len++]=rf->sf;  
  in_buff[in_len++]=rf->bw;  
  in_buff[in_len++]=len;
  memcpy(in_buff, buff, len);
  in_len+=len;
  nwk_slave_uart_send_level(MSCmdRxData, in_buff, in_len);
}

/*		
================================================================================
描述 : 从机参数读取
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_read(void)
{
	nwk_eeprom_read((u8*)&g_sNwkSlaveSave, sizeof(g_sNwkSlaveSave));
	if(g_sNwkSlaveSave.crc_value!=nwk_crc16((u8*)&g_sNwkSlaveSave, sizeof(g_sNwkSlaveSave)-2))
	{
		memset((u8*)&g_sNwkSlaveSave, 0, sizeof(g_sNwkSlaveSave));
		nwk_slave_save();
	}
}


/*		
================================================================================
描述 : 从机参数存储
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_save(void)
{
	g_sNwkSlaveSave.crc_value=nwk_crc16((u8*)&g_sNwkSlaveSave, sizeof(g_sNwkSlaveSave)-2);
	nwk_eeprom_save((u8*)&g_sNwkSlaveSave, sizeof(g_sNwkSlaveSave));
}


/*		
================================================================================
描述 : 设置从机地址码
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_set_addr(u8 slave_addr)
{
	g_sNwkSlaveSave.slave_addr=slave_addr;
	nwk_slave_save();
}


/*		
================================================================================
描述 : 传递LoRa模块应用指针
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_set_lora_dev(LoRaDevStruct *pLoRaDev)
{
	g_sNwkSlaveWork.pLoRaDev=pLoRaDev;
}


/*		
================================================================================
描述 : 设置LoRa参数
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_set_lora_param(u32 freq, u8 sf, u8 bw)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
		drv_sx1278_set_rf_freq(g_sNwkSlaveWork.pLoRaDev, freq);
		drv_sx1278_set_sf(g_sNwkSlaveWork.pLoRaDev, sf);
		drv_sx1278_set_bw(g_sNwkSlaveWork.pLoRaDev, bw);

#elif defined(LORA_SX1268)

#elif defined(LORA_LLCC68)

#endif	
	
}

/*		
================================================================================
描述 : 发送函数注册
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_fun_send_register(void (*fun_send)(u8 *buff, u16 len))
{
  g_sNwkSlaveWork.fun_send=fun_send; 
}

/*		
================================================================================
描述 : CAD初始化
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_cad_init(void)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;	
	
#if defined(LORA_SX1278)  
	drv_sx1278_cad_init(g_sNwkSlaveWork.pLoRaDev);
	
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
void nwk_slave_recv_init(void)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;	
	
#if defined(LORA_SX1278)  
	drv_sx1278_recv_init(g_sNwkSlaveWork.pLoRaDev); 
	
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
u8 nwk_slave_cad_check(void)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	return drv_sx1278_cad_check(g_sNwkSlaveWork.pLoRaDev);  
	
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
u8 nwk_slave_recv_check(u8 *buff, int16_t *rssi)
{
	u8 read_size=0;
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	
	read_size=drv_sx1278_recv_check(g_sNwkSlaveWork.pLoRaDev, buff); 
	if(read_size>0)
	{
		*rssi=drv_sx1278_read_rssi(g_sNwkSlaveWork.pLoRaDev);
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
u32 nwk_slave_calcu_air_time(u8 sf, u8 bw, u16 data_len)
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
void nwk_slave_send_buff(u8 *buff, u16 len)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;	

#if defined(LORA_SX1278)  
	drv_sx1278_send(g_sNwkSlaveWork.pLoRaDev, buff, len); 
	
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
u8 nwk_slave_send_check(void)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	return drv_sx1278_send_check(g_sNwkSlaveWork.pLoRaDev); 
	
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
void nwk_slave_send_sniff(u8 sf, u8 bw)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;	
  u8 buff[1]={0x01};
  u32 tx_time=nwk_slave_calcu_air_time(sf, bw, 1);
  u8 loops=(tx_time*0.1)/2+5;//计算循环次数
//  u8 loops=2;//计算循环次数
#if defined(LORA_SX1278)  
	drv_sx1278_send(g_sNwkSlaveWork.pLoRaDev, buff, 1); 
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
描述 : 广播进程
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_broad_process(void)
{
  NwkSlaveBroadStruct *pSlaveBroad=&g_sNwkSlaveWork.slave_broad;
  switch(pSlaveBroad->broad_state)
  {
    case NwkSlaveBroadIdel:
    {
      
      break;
    }
    case NwkSlaveBroadInit:
    {
      nwk_slave_set_lora_param(pSlaveBroad->freq, pSlaveBroad->sf, pSlaveBroad->bw);
			printf("broad param (%d, %d, %d)\n", pSlaveBroad->freq/1000000, pSlaveBroad->sf, pSlaveBroad->bw);
			pSlaveBroad->sniff_cnts=0;
			pSlaveBroad->broad_state=NwkSlaveBroadSniff;       
      break;
    }
		case NwkSlaveBroadSniff:
		{
			if(pSlaveBroad->sniff_cnts<5)
			{
				nwk_slave_send_sniff(pSlaveBroad->sf, pSlaveBroad->bw);
				pSlaveBroad->sniff_cnts++;
			}
			else
			{
				nwk_slave_recv_init();//状态切换,清除发送状态位
				nwk_slave_set_lora_param(pSlaveBroad->freq, pSlaveBroad->sf, pSlaveBroad->bw);
				nwk_slave_send_buff(pSlaveBroad->broad_buff, pSlaveBroad->broad_len);//发送数据包
				u32 tx_time=nwk_slave_calcu_air_time(pSlaveBroad->sf, pSlaveBroad->bw, pSlaveBroad->broad_len)*1.2;//发送时间,冗余
				pSlaveBroad->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
				pSlaveBroad->wait_cnts=tx_time/1000+1;//等待秒数
				printf("tx_time=%d ms, wait_cnts=%d\n", tx_time, pSlaveBroad->wait_cnts);
				pSlaveBroad->broad_state=NwkSlaveBroadTxCheck;  				
			}
			break;
		}
    case NwkSlaveBroadTxCheck:
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_slave_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("broad send ok!\n");
				pSlaveBroad->broad_len=0;
        pSlaveBroad->broad_state=NwkSlaveBroadIdel; 
      }
      else if(now_time-pSlaveBroad->start_rtc_time>pSlaveBroad->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pSlaveBroad->wait_cnts);
				pSlaveBroad->broad_len=0;
        pSlaveBroad->broad_state=NwkSlaveBroadIdel; 
      }      
      break;
    }
  
  }
}

/*		
================================================================================
描述 : 接收进程
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_rx_process(void)
{
  NwkSlaveRxStruct *pSlaveRx=&g_sNwkSlaveWork.slave_rx;
  switch(pSlaveRx->rx_state)
  {
    case NwkSlaveRxIdel:
    {
      
      break;
    }
    case NwkSlaveRxInit:
    {
//      pSlaveRx->freq=NWK_GW_BASE_FREQ+pSlaveRx->freq_ptr*2000000;
      pSlaveRx->rx_state=NwkSlaveRxCadInit;
      pSlaveRx->group_id=0;
//      printf("###### NwkSlaveRxInit!\n");        
      break;
    }
    case NwkSlaveRxCadInit:
    {
      u8 sf=0, bw=0;
      pSlaveRx->freq=NWK_GW_BASE_FREQ+pSlaveRx->group_id*500000;
      nwk_get_channel(pSlaveRx->group_id*2, &sf, &bw);
      pSlaveRx->curr_sf=sf;
      pSlaveRx->curr_bw=bw;      
//			printf("cad param(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, sf, bw);
      nwk_slave_set_lora_param(pSlaveRx->freq, sf, bw);
      nwk_slave_cad_init();  
      pSlaveRx->rx_state=NwkSlaveRxCadCheck;      
      break;
    }
    case NwkSlaveRxCadCheck:
    {
      u8 result=nwk_slave_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
        pSlaveRx->group_id++;
        if(pSlaveRx->group_id>=NWK_RF_GROUP_NUM)//NWK_RF_GROUP_NUM
        {
          pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
        } 
				else
				{
					pSlaveRx->rx_state=NwkSlaveRxCadInit;//下一组参数
				}
      }
      else if(result==CadResultSuccess)//搜索到
      {
        printf("cad OK***(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, pSlaveRx->curr_sf, pSlaveRx->curr_bw);
        u8 sf=0, bw=0;
        nwk_get_channel(pSlaveRx->group_id*2+1, &sf, &bw);//以本通道组的第二个参数作为CAD监听参数
        pSlaveRx->curr_sf=sf;
        pSlaveRx->curr_bw=bw;
        printf("group id=%d, rx sniff param(%.2f, %d, %d)\n", pSlaveRx->group_id, pSlaveRx->freq/1000000.0, sf, bw);
        
        nwk_slave_set_lora_param(pSlaveRx->freq, sf, bw);  
        for(u8 i=0; i<3+NWK_RF_GROUP_NUM-pSlaveRx->group_id; i++)//8-pSlaveRx->group_id
        {
          nwk_slave_send_sniff(sf, bw);//返回嗅探帧
          nwk_slave_cad_init();//状态切换
        }
        printf("into recv mode!\n"); 
        nwk_slave_recv_init();//进入接收
        u32 tx_time=nwk_slave_calcu_air_time(sf, bw, 4)*1.2;//接收等待时间
        pSlaveRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=tx_time/1000+1;             
        pSlaveRx->rx_state=NwkSlaveRxCheck; 
        printf("will wait time=%ds\n", pSlaveRx->wait_cnts);  
      }        
      break;
    }     
    case NwkSlaveRxCheck:
    {
      u32 now_time=nwk_get_rtc_counter();
      int16_t rssi;
      u8 recv_len=nwk_slave_recv_check(g_sNwkSlaveWork.slave_rx.recv_buff, &rssi); 
      if(recv_len>0)
      {
        printf("recv rssi=%ddbm\n", rssi);
        u8 *pBuff=g_sNwkSlaveWork.slave_rx.recv_buff;
        printf_hex("recv=", pBuff, recv_len);
        
        //查看是否为应用数据长度
        u8 head[1]={0xA8};
        u8 *pData=nwk_find_head(pBuff, recv_len, head, 1);
        if(pData)
        {
          u16 crcValue=pData[2]<<8|pData[3];
          if(crcValue==nwk_crc16(pData, 2))
          {
            u8 will_len=pData[1];//待接收的数据长度
            printf("app len=%d\n", will_len);
            u32 tx_time=nwk_slave_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, will_len)*1.2;//接收等待时间
            pSlaveRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
            pSlaveRx->wait_cnts=tx_time/1000+2; 
            printf("app wait time=%ds\n", pSlaveRx->wait_cnts);
            return;            
          }
        }
        
        //应用数据解析
        g_sNwkSlaveWork.recv_rssi=rssi; 
        RfParamStruct rf;
        rf.rssi=rssi;
        rf.freq=pSlaveRx->freq;
        rf.sf=pSlaveRx->curr_sf;
        rf.bw=pSlaveRx->curr_bw;
        nwk_slave_send_rx(pBuff, recv_len, &rf);//无线数据发送到主机
        pSlaveRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=2;         
        pSlaveRx->rx_state=NwkSlaveRxAckWait;
        
        //测试
        u8 ack_buff[]={0xAA, 0x55};
        delay_ms(300);
        nwk_slave_send_buff(ack_buff, sizeof(ack_buff)); 
        u32 tx_time=nwk_slave_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, 2)*1.2;//接收等待时间
        pSlaveRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=tx_time/1000+1;             
        pSlaveRx->rx_state=NwkSlaveRxAckCheck;//测试, 回复包发送检测          
      }   
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//超时
      {
        printf("recv wait time out!\n");
        pSlaveRx->group_id++;//换下一组参数 
        if(pSlaveRx->group_id>=NWK_RF_GROUP_NUM)
        {
          pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
        } 
        else
        {
          pSlaveRx->rx_state=NwkSlaveRxCadInit;//继续监听
        }
      }       
      break;
    }    
    case NwkSlaveRxAckWait://等待主机的回复包
    {
      u32 now_time=nwk_get_rtc_counter();
      if(now_time-pSlaveRx->start_rtc_time>=pSlaveRx->wait_cnts)//超时
      {
        printf("master ack time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }
      break;
    }
    case NwkSlaveRxAckCheck:
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_slave_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("ack send ok!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//发送超时
      {
        printf("ack send time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }    
      break;
    }
  }
}


/*		
================================================================================
描述 : 发送进程
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_tx_process(void)
{
  NwkSlaveTxStruct *pSlaveTx=&g_sNwkSlaveWork.slave_tx;
  switch(pSlaveTx->tx_state)
  {
    case NwkSlaveTxIdel:
    {
      
      break;
    }
    case NwkSlaveTxInit:
    {
      pSlaveTx->curr_cnts=0;
      if(pSlaveTx->try_cnts>2)
        pSlaveTx->try_cnts=2;
      pSlaveTx->start_rtc_time=nwk_get_rtc_counter();
      pSlaveTx->tx_state=NwkSlaveTxLBTInit;
      break;
    }
    case NwkSlaveTxLBTInit:
    {
      u32 now_time=nwk_get_rtc_counter();
      if(now_time-pSlaveTx->start_rtc_time>=4)
      {
        //退出
        pSlaveTx->tx_len=0;
        printf("tx time out, exit!\n");
        pSlaveTx->tx_state=NwkSlaveTxIdel; 
        return;
      }
      else if(pSlaveTx->curr_cnts>=pSlaveTx->try_cnts)
      {
        pSlaveTx->curr_cnts=0;//循环嗅探唤醒
      }

//      pSlaveTx->curr_sf=pSlaveTx->sfs[pSlaveTx->curr_cnts];
//      pSlaveTx->curr_bw=pSlaveTx->bws[pSlaveTx->curr_cnts];
      pSlaveTx->curr_sf=11;
      pSlaveTx->curr_bw=6;      
      nwk_slave_set_lora_param(pSlaveTx->freq, pSlaveTx->curr_sf, pSlaveTx->curr_bw);
//      nwk_slave_cad_init(); 
//      pSlaveTx->tx_state=NwkSlaveTxLBTCheck;     

      pSlaveTx->sniff_cnts=0; 
      pSlaveTx->tx_state=NwkSlaveTxSniffInit;//进行嗅探      
      break;
    }
    case NwkSlaveTxLBTCheck:
    {
      u8 result=nwk_slave_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
        pSlaveTx->sniff_cnts=0; 
        pSlaveTx->tx_state=NwkSlaveTxSniffInit;//进行嗅探
        printf("no LBT!\n");
      }
      else if(result==CadResultSuccess)//搜索到,有设备正在传输
      {
        //退出
        printf("have LBT, exit!\n");
        pSlaveTx->tx_state=NwkSlaveTxIdel;
      }       
      break;
    }
    case NwkSlaveTxSniffInit:
    {
      for(u8 i=0; i<20; i++)
      {
        nwk_slave_send_sniff(pSlaveTx->curr_sf, pSlaveTx->curr_bw);//发送嗅探帧
      }
      nwk_slave_cad_init(); //开始监听返回
      pSlaveTx->cad_cnts=0;
      printf("send sniff (%.2f, %d, %d), cnts=%d\n", pSlaveTx->freq/1000000.f, pSlaveTx->curr_sf, pSlaveTx->curr_bw, pSlaveTx->sniff_cnts+1);     
      pSlaveTx->tx_state=NwkSlaveTxSniffCheck;      
      break;
    }
    case NwkSlaveTxSniffCheck:
    {
      u8 result=nwk_slave_cad_check();
      if(result==CadResultFailed)//没搜索到
      {
        printf("sniff no cad!\n");
        pSlaveTx->cad_cnts++;
        if(pSlaveTx->cad_cnts<=3)
        {
          nwk_slave_cad_init();//继续监听
        }
        else
        {
          pSlaveTx->curr_cnts++;
          pSlaveTx->tx_state=NwkSlaveTxLBTInit;  //继续嗅探           
        }
      }
      else if(result==CadResultSuccess)//搜索成功 
      {
        printf("**** CAD OK!\n");
        nwk_delay_ms(5);//适当延时,等待对方准备好 
        nwk_slave_send_buff(pSlaveTx->tx_buff, pSlaveTx->tx_len);//发送数据包
        u32 tx_time=nwk_slave_calcu_air_time(pSlaveTx->curr_sf, pSlaveTx->curr_bw, pSlaveTx->tx_len)*1.2;//发送时间,冗余
        pSlaveTx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pSlaveTx->wait_cnts=tx_time/1000+2;//等待秒数
        pSlaveTx->tx_state=NwkSlaveTxRunning;
      }       
      break;
    } 
    case NwkSlaveTxRunning:
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_slave_send_check();//发送完成检测
      if(result)//发送完成
      {
        nwk_slave_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_slave_calcu_air_time(pSlaveTx->curr_sf, pSlaveTx->curr_bw, 20);//接收回复包等待时间
        pSlaveTx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pSlaveTx->wait_cnts=tx_time/1000+1;
        pSlaveTx->tx_state=NwkSlaveTxAck;
      }
      else if(now_time-pSlaveTx->start_rtc_time>=pSlaveTx->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pSlaveTx->wait_cnts);
        pSlaveTx->tx_state=NwkSlaveTxIdel;  
      }      
      break;
    }
    case NwkSlaveTxAck:
    {
      u32 now_time=nwk_get_rtc_counter();
      int16_t rssi;
      u8 recv_len=nwk_slave_recv_check(g_sNwkSlaveWork.slave_rx.recv_buff, &rssi);
      if(recv_len>0)
      {
        //数据解析
        g_sNwkSlaveWork.recv_rssi=rssi;
        RfParamStruct rf;
        rf.freq=pSlaveTx->freq;
        rf.sf=pSlaveTx->curr_sf;
        rf.bw=pSlaveTx->curr_bw;
        rf.rssi=rssi;
        nwk_slave_send_rx(g_sNwkSlaveWork.slave_rx.recv_buff, recv_len, &rf);//无线数据发送到主机
        pSlaveTx->tx_state=NwkSlaveTxIdel;
      }   
      else if(now_time-pSlaveTx->start_rtc_time>=pSlaveTx->wait_cnts)//超时
      {
        pSlaveTx->tx_state=NwkSlaveTxIdel; 
      }        
      break;
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
void nwk_slave_work_state_check(void)
{
  NwkSlaveBroadStruct *pSlaveBroad=&g_sNwkSlaveWork.slave_broad;
  NwkSlaveRxStruct *pSlaveRx=&g_sNwkSlaveWork.slave_rx;
  NwkSlaveTxStruct *pSlaveTx=&g_sNwkSlaveWork.slave_tx;
  
  switch(g_sNwkSlaveWork.work_state)
  {
    case NwkSlaveWorkIdel://空闲
    {
      if(pSlaveTx->tx_len>0)
      {
        pSlaveTx->tx_state=NwkSlaveTxInit;
        g_sNwkSlaveWork.work_state=NwkSlaveWorkTX;
      }
      else if(pSlaveBroad->broad_len>0)
      {
        pSlaveBroad->broad_state=NwkSlaveBroadInit;
        g_sNwkSlaveWork.work_state=NwkSlaveWorkBroad;
      }
      else
      {
        pSlaveRx->rx_state=NwkSlaveRxInit;
        g_sNwkSlaveWork.work_state=NwkSlaveWorkRX;
      }
      break;
    }
    case NwkSlaveWorkBroad://广播
    {
      nwk_slave_broad_process();
      if(pSlaveBroad->broad_state==NwkSlaveBroadIdel)
      {
        g_sNwkSlaveWork.work_state=NwkSlaveWorkIdel;
      }
      break;
    }
    case NwkSlaveWorkRX://接收
    {
      nwk_slave_rx_process();
      if(pSlaveRx->rx_state==NwkSlaveRxIdel)
      {
        g_sNwkSlaveWork.work_state=NwkSlaveWorkIdel;
      }      
      break;
    }
    case NwkSlaveWorkTX://发送
    {
      nwk_slave_tx_process();
      if(pSlaveTx->tx_state==NwkSlaveTxIdel)
      {
        g_sNwkSlaveWork.work_state=NwkSlaveWorkIdel;
      }      
      break;
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
void nwk_slave_main(void)
{
  nwk_slave_work_state_check();
  if(g_sNwkSlaveWork.ack_tickets==(g_sNwkSlaveSave.slave_addr-1)*10)
  {
    
  }
  g_sNwkSlaveWork.ack_tickets++;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 




