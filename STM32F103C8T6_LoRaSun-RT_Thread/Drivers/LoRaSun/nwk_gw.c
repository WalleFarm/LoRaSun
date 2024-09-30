
#include "nwk_gw.h"


NwkCoreWorkStruct g_sNwkCoreWork={0};



/*		
================================================================================
描述 : 设置LoRa参数
输入 : 
输出 : 
================================================================================
*/ 
void nwk_gw_set_lora_param(NwkGwWorkStruct *pNwkGw, u32 freq, u8 sf, u8 bw)
{
	if(pNwkGw->pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
		drv_sx1278_set_rf_freq(pNwkGw->pLoRaDev, freq);
		drv_sx1278_set_sf(pNwkGw->pLoRaDev, sf);
		drv_sx1278_set_bw(pNwkGw->pLoRaDev, bw);

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
void nwk_gw_cad_init(NwkGwWorkStruct *pNwkGw)
{
	if(pNwkGw->pLoRaDev==NULL)
		return;	
	
#if defined(LORA_SX1278)  
	drv_sx1278_cad_init(pNwkGw->pLoRaDev);
	
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
void nwk_gw_recv_init(NwkGwWorkStruct *pNwkGw)
{
	if(pNwkGw->pLoRaDev==NULL)
		return;	
	
#if defined(LORA_SX1278)  
	drv_sx1278_recv_init(pNwkGw->pLoRaDev); 
	
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
u8 nwk_gw_cad_check(NwkGwWorkStruct *pNwkGw)
{
	if(pNwkGw->pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	return drv_sx1278_cad_check(pNwkGw->pLoRaDev);  
	
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
u8 nwk_gw_recv_check(NwkGwWorkStruct *pNwkGw, u8 *buff, int16_t *rssi)
{
	u8 read_size=0;
	if(pNwkGw->pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	
	read_size=drv_sx1278_recv_check(pNwkGw->pLoRaDev, buff); 
	if(read_size>0)
	{
		*rssi=drv_sx1278_read_rssi(pNwkGw->pLoRaDev);
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
u32 nwk_gw_calcu_air_time(u8 sf, u8 bw, u16 data_len)
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
void nwk_gw_send_buff(NwkGwWorkStruct *pNwkGw, u8 *buff, u16 len)
{
	if(pNwkGw->pLoRaDev==NULL)
		return;	

#if defined(LORA_SX1278)  
	drv_sx1278_send(pNwkGw->pLoRaDev, buff, len); 
	
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
u8 nwk_gw_send_check(NwkGwWorkStruct *pNwkGw)
{
	if(pNwkGw->pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	return drv_sx1278_send_check(pNwkGw->pLoRaDev); 
	
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
void nwk_gw_send_sniff(NwkGwWorkStruct *pNwkGw, u8 sf, u8 bw)
{
	if(pNwkGw->pLoRaDev==NULL)
		return;	
  u8 buff[1]={0x01};
  u32 tx_time=nwk_gw_calcu_air_time(sf, bw, 1);
  u8 loops=(tx_time*0.5)/2+1;//计算循环次数
#if defined(LORA_SX1278)  
	drv_sx1278_send(pNwkGw->pLoRaDev, buff, 1); 
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
void nwk_gw_broad_process(NwkGwWorkStruct *pNwkGw)
{
  NwkGwBroadStruct *pGwBroad=&pNwkGw->gw_broad;
  switch(pGwBroad->broad_state)
  {
    case NwkGwBroadIdel:
    {
      
      break;
    }
    case NwkGwBroadInit:
    {
      nwk_gw_set_lora_param(pNwkGw, pGwBroad->freq, pGwBroad->sf, pGwBroad->bw);
      nwk_gw_send_buff(pNwkGw, pGwBroad->broad_buff, pGwBroad->broad_len);//发送数据包
      u32 tx_time=nwk_gw_calcu_air_time(pGwBroad->sf, pGwBroad->bw, pGwBroad->broad_len)*1.2;//发送时间,冗余
      pGwBroad->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
      pGwBroad->wait_cnts=tx_time/1000+2;//等待秒数
      pGwBroad->broad_state=NwkGwBroadTxCheck;       
      break;
    }
    case NwkGwBroadTxCheck:
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_gw_send_check(pNwkGw);//发送完成检测 
      if(result)//发送完成
      {
        pGwBroad->broad_state=NwkGwBroadIdel; 
      }
      else if(now_time-pGwBroad->start_rtc_time>=pGwBroad->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pGwBroad->wait_cnts);
        pGwBroad->broad_state=NwkGwBroadIdel; 
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
void nwk_gw_rx_process(NwkGwWorkStruct *pNwkGw)
{
  NwkGwRxStruct *pGwRx=&pNwkGw->gw_rx;
  switch(pGwRx->rx_state)
  {
    case NwkGwRxIdel:
    {
      
      break;
    }
    case NwkGwRxInit:
    {
      pGwRx->freq=NWK_GW_BASE_FREQ+pGwRx->freq_ptr*2000000;
      pGwRx->rx_state=NwkGwRxCadInit;
      pGwRx->chn_ptr=0;
      printf("NwkGwRxInit!\n");        
      break;
    }
    case NwkGwRxCadInit:
    {
      u8 sf=0, bw=0;
      nwk_get_channel(pGwRx->chn_ptr*3, &sf, &bw);
      nwk_gw_set_lora_param(pNwkGw, pGwRx->freq, sf, bw);
      nwk_gw_cad_init(pNwkGw);  
      pGwRx->rx_state=NwkGwRxCadCheck;      
      break;
    }
    case NwkGwRxCadCheck:
    {
      u8 result=nwk_gw_cad_check(pNwkGw);
      if(result==CadResultFailed)//没搜索到
      {
        pGwRx->chn_ptr++;
        pGwRx->rx_state=NwkGwRxCadInit;//下一组参数
      }
      else if(result==CadResultSuccess)//搜索到
      {
        pGwRx->rx_state=NwkGwRxSniff;        
      }        
      break;
    }
    case NwkGwRxSniff:
    {
      u8 sf=0, bw=0;
      nwk_get_channel(pGwRx->chn_ptr*3+1, &sf, &bw);//以本通道组的第二个参数作为CAD监听参数
      nwk_gw_set_lora_param(pNwkGw, pGwRx->freq, sf, bw);  
      nwk_gw_send_sniff(pNwkGw, sf, bw);//发送嗅探帧 
      nwk_gw_recv_init(pNwkGw);//进入接收
      u32 tx_time=nwk_gw_calcu_air_time(sf, bw, NWK_TRANSMIT_MAX_SIZE);//接收等待时间
      pGwRx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
      pGwRx->wait_cnts=tx_time/1000+1;             
      pGwRx->rx_state=NwkGwRxCheck;   
      break;
    }     
    case NwkGwRxCheck:
    {
      u32 now_time=nwk_get_rtc_counter();
      int16_t rssi;
      u8 recv_len=nwk_gw_recv_check(pNwkGw, pGwRx->recv_buff, &rssi); 
      if(recv_len>0)
      {
        //数据解析
        pNwkGw->recv_rssi=rssi; 
//        nwk_gw_recv_parse(g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        pGwRx->rx_state=NwkGwRxIdel;
      }   
      else if(now_time-pGwRx->start_rtc_time>=pGwRx->wait_cnts)//超时
      {
        pGwRx->chn_ptr++;//换下一组参数 
        if(pGwRx->chn_ptr>=NWK_RF_GROUP_NUM)
        {
          pGwRx->rx_state=NwkGwRxIdel;//结束本回合
        } 
        else
        {
          pGwRx->rx_state=NwkGwRxCadInit;//继续监听
        }
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
void nwk_gw_tx_process(NwkGwWorkStruct *pNwkGw)
{
  NwkGwTxStruct *pGwTx=&pNwkGw->gw_tx;
  switch(pGwTx->tx_state)
  {
    case NwkGwTxIdel:
    {
      
      break;
    }
    case NwkGwTxInit:
    {
      pGwTx->curr_cnts=0;
      if(pGwTx->try_cnts>2)
        pGwTx->try_cnts=2;
      pGwTx->start_rtc_time=nwk_get_rtc_counter();
      break;
    }
    case NwkGwTxLBTInit:
    {
      u32 now_time=nwk_get_rtc_counter();
      if(now_time-pGwTx->start_rtc_time>=2)
      {
        //退出
        pGwTx->tx_state=NwkGwTxIdel;        
      }
      else if(pGwTx->curr_cnts>=pGwTx->try_cnts)
      {
        pGwTx->curr_cnts=0;//循环嗅探唤醒
      }

      pGwTx->curr_sf=pGwTx->sfs[pGwTx->curr_cnts];
      pGwTx->curr_bw=pGwTx->bws[pGwTx->curr_cnts];
      nwk_gw_set_lora_param(pNwkGw, pGwTx->freq, pGwTx->curr_sf, pGwTx->curr_bw);
      nwk_gw_cad_init(pNwkGw); 
      pGwTx->tx_state=NwkGwTxLBTCheck;        
      break;
    }
    case NwkGwTxLBTCheck:
    {
      u8 result=nwk_gw_cad_check(pNwkGw);
      if(result==CadResultFailed)//没搜索到
      {
        pGwTx->sniff_cnts=0; 
        pGwTx->tx_state=NwkGwTxSniffInit;//进行嗅探
        printf("no LBT!\n");
      }
      else if(result==CadResultSuccess)//搜索到,有设备正在传输
      {
        //退出
        printf("have LBT, exit!\n");
        pGwTx->tx_state=NwkGwTxIdel;
      }       
      break;
    }
    case NwkGwTxSniffInit:
    {
      nwk_gw_send_sniff(pNwkGw, pGwTx->curr_sf, pGwTx->curr_bw);//发送嗅探帧
      nwk_gw_cad_init(pNwkGw); //开始监听返回
      printf("send sniff sf=%d, bw=%d, cnts=%d\n", pGwTx->curr_sf, pGwTx->curr_bw, pGwTx->sniff_cnts+1);     
      pGwTx->tx_state=NwkGwTxSniffCheck;      
      break;
    }
    case NwkGwTxSniffCheck:
    {
      u8 result=nwk_gw_cad_check(pNwkGw);
      if(result==CadResultFailed)//没搜索到
      {
        pGwTx->curr_cnts++;
        pGwTx->tx_state=NwkGwTxLBTInit;  
      }
      else if(result==CadResultSuccess)//搜索成功 
      {
        nwk_delay_ms(5);//适当延时,等待对方准备好 
        nwk_gw_send_buff(pNwkGw, pGwTx->tx_buff, pGwTx->tx_len);//发送数据包
        u32 tx_time=nwk_gw_calcu_air_time(pGwTx->curr_sf, pGwTx->curr_bw, pGwTx->tx_len)*1.2;//发送时间,冗余
        pGwTx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pGwTx->wait_cnts=tx_time/1000+2;//等待秒数
        pGwTx->tx_state=NwkGwTxRunning;
      }       
      break;
    } 
    case NwkGwTxRunning:
    {
      u32 now_time=nwk_get_rtc_counter();
      u8 result=nwk_gw_send_check(pNwkGw);//发送完成检测
      if(result)//发送完成
      {
        nwk_gw_recv_init(pNwkGw);//进入接收,等待回复
        u32 tx_time=nwk_gw_calcu_air_time(pGwTx->curr_sf, pGwTx->curr_bw, 20);//接收回复包等待时间
        pGwTx->start_rtc_time=nwk_get_rtc_counter();//记录当前时间,防止超时
        pGwTx->wait_cnts=tx_time/1000+1;
        pGwTx->tx_state=NwkGwTxAck;
      }
      else if(now_time-pGwTx->start_rtc_time>=pGwTx->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pGwTx->wait_cnts);
        pGwTx->tx_state=NwkGwTxIdel;  
      }      
      break;
    }
    case NwkGwTxAck:
    {
      u32 now_time=nwk_get_rtc_counter();
      int16_t rssi;
      u8 recv_len=nwk_gw_recv_check(pNwkGw, pNwkGw->gw_rx.recv_buff, &rssi);
      if(recv_len>0)
      {
        //数据解析
        pNwkGw->recv_rssi=rssi;
//        nwk_node_recv_parse(g_sNwkNodeWork.node_rx.recv_buff, recv_len);
        pGwTx->tx_state=NwkGwTxIdel;
      }   
      else if(now_time-pGwTx->start_rtc_time>=pGwTx->wait_cnts)//超时
      {
        pGwTx->tx_state=NwkGwTxIdel; 
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
void nwk_gw_work_state_check(NwkGwWorkStruct *pNwkGw)
{
  NwkGwBroadStruct *pGwBroad=&pNwkGw->gw_broad;
  NwkGwRxStruct *pGwRx=&pNwkGw->gw_rx;
  NwkGwTxStruct *pGwTx=&pNwkGw->gw_tx;
  
  switch(pNwkGw->work_state)
  {
    case NwkGwWorkIdel:
    {
      if(pGwTx->tx_len>0)
      {
        pGwTx->tx_state=NwkGwTxInit;
        pNwkGw->work_state=NwkGwWorkTX;
      }
      else if(pGwBroad->broad_len>0)
      {
        pGwBroad->broad_state=NwkGwBroadInit;
        pNwkGw->work_state=NwkGwWorkBroad;
      }
      else
      {
        pGwRx->rx_state=NwkGwRxInit;
        pNwkGw->work_state=NwkGwWorkRX;
      }
      break;
    }
    case NwkGwWorkBroad:
    {
      nwk_gw_broad_process(pNwkGw);
      if(pGwBroad->broad_state==NwkGwBroadIdel)
      {
        pNwkGw->work_state=NwkGwWorkIdel;
      }
      break;
    }
    case NwkGwWorkRX:
    {
      nwk_gw_rx_process(pNwkGw);
      if(pGwRx->rx_state==NwkGwRxIdel)
      {
        pNwkGw->work_state=NwkGwWorkIdel;
      }      
      break;
    }
    case NwkGwWorkTX:
    {
      nwk_gw_tx_process(pNwkGw);
      if(pGwTx->tx_state==NwkGwTxIdel)
      {
        pNwkGw->work_state=NwkGwWorkIdel;
      }      
      break;
    }    
  }
  
}



/*
*         以下是网络核心计算
*/
 
/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void nwk_core_set_root_key(u8 *key)
{
  memcpy(g_sNwkCoreWork.root_key, key, 16);
}

/*		
================================================================================
描述 : 初始化天线硬件
输入 : 
输出 : 
================================================================================
*/ 
NwkGwWorkStruct *nwk_core_init_gw(u8 index, LoRaDevStruct *pLoRaDev)
{
  if(index<NWK_GW_WIRELESS_NUM)
  {
    NwkGwWorkStruct *pNwkGwWork=&g_sNwkCoreWork.gw_wireless_list[index];
    pNwkGwWork->pLoRaDev=pLoRaDev;
    pNwkGwWork->index=index;
    return pNwkGwWork;
  }
  return NULL;
}

/*		
================================================================================
描述 : 添加节点记录
输入 : 
输出 : 
================================================================================
*/ 
NwkNodeTokenStruct *nwk_core_add_token(u32 node_sn)
{
  if(g_sNwkCoreWork.node_cnts<NWK_NODE_MAX_NUM)
  {
    NwkNodeTokenStruct *pNodeToken=(NwkNodeTokenStruct *)malloc(sizeof(NwkNodeTokenStruct));
    if(pNodeToken)
    {
      memset(pNodeToken, 0, sizeof(NwkNodeTokenStruct));
      pNodeToken->node_sn=node_sn;
      pNodeToken->next=NULL;
      if(g_sNwkCoreWork.pNodeHead==NULL)
      {
        g_sNwkCoreWork.pNodeHead=pNodeToken;
      }
      else
      {
        NwkNodeTokenStruct *pTemp=g_sNwkCoreWork.pNodeHead;
        while(pTemp->next)//寻找最后一个节点
        {
          pTemp=pTemp->next;
        }
        pTemp->next=pNodeToken;
      }
      g_sNwkCoreWork.node_cnts++;
      return pNodeToken;
    }
  }
  return NULL;
}

/*		
================================================================================
描述 : 查找节点记录
输入 : 
输出 : 
================================================================================
*/ 
NwkNodeTokenStruct *nwk_core_find_token(u32 node_sn)
{
  NwkNodeTokenStruct *pTemp=g_sNwkCoreWork.pNodeHead;
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
void nwk_core_del_token(u32 node_sn)
{
  NwkNodeTokenStruct *pTemp1=g_sNwkCoreWork.pNodeHead;
  NwkNodeTokenStruct *pTemp2=pTemp1;
  
  while(pTemp1)
  {   
    if(pTemp1->node_sn==node_sn)
    {
      if(pTemp1==g_sNwkCoreWork.pNodeHead)//链表头是否为删除点
      {
        g_sNwkCoreWork.pNodeHead=pTemp1->next;
      }
      else
      {
        pTemp2->next=pTemp1->next;//链表断开重连
      }
      g_sNwkCoreWork.node_cnts--;
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
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void nwk_core_main(void)
{
  for(u8 i=0; i<NWK_GW_WIRELESS_NUM; i++)
  {
    NwkGwWorkStruct *pGwWork=&g_sNwkCoreWork.gw_wireless_list[i];
    nwk_gw_work_state_check(pGwWork);
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












