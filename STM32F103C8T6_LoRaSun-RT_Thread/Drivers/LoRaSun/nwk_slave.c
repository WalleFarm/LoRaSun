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



#include "nwk_slave.h"

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
  printf_hex("master buff= ", recv_buff, recv_len);
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
      if(slave_addr!=g_sNwkSlaveWork.slave_addr && slave_addr!=0xFF)
      {
        return;
      }
      switch(cmd_type)
      {
        case MSCmdHeart://主机下发的同步信号,根据地址码间隔(slave-1)*10ms发送数据
        {

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
          g_sNwkSlaveWork.work_state=NwkSlaveWorkIdel; //优先广播
          break;
        } 
        case MSCmdAckRxData://回复上行接收
        {
          printf("MSCmdAckRxData ###\n");
          NwkSlaveRxStruct *pSlaveRx=&g_sNwkSlaveWork.slave_rx;          
          u32 freq=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          u8 sf=pData[0]; 
          pData+=1;
          u8 bw=pData[0];
          pData+=1;    
          u8 lora_len=pData[0];
          pData+=1;   
          u8 *lora_buff=pData;
          nwk_slave_set_lora_param(freq, sf, bw);//设置通讯参数(也可以不设置)
//          nwk_delay_ms(100);
          nwk_slave_send_buff(lora_buff, lora_len); 
          u32 tx_time=nwk_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, lora_len);//接收等待时间
          pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
          pSlaveRx->wait_cnts=tx_time/1000+1;             
          pSlaveRx->rx_state=NwkSlaveRxAckCheck;//回复包发送检测  
          pData+=lora_len;
          pSlaveRx->down_len=pData[0];//判断是否有下行数据
          pData+=1;
//          printf("down_len=%d\n", pSlaveRx->down_len);
          if(pSlaveRx->down_len>0)
          {
            memcpy(pSlaveRx->down_buff, pData, pSlaveRx->down_len);
//            printf_hex("down buff=", pSlaveRx->down_buff, pSlaveRx->down_len);
          }
          break;
        }
        case MSCmdTxData://下行数据
        {
          NwkSlaveTxStruct *pSlaveTx=&g_sNwkSlaveWork.slave_tx;
          u32 dst_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          u8 awake_flag=pData[0];
          pData+=1;
          u8 tx_len=pData[0];
          pData+=1;
          if(tx_len<sizeof(pSlaveTx->tx_buff))
          {  
            if(pSlaveTx->tx_len==0)
            {
              pSlaveTx->dst_sn=dst_sn;
              pSlaveTx->awake_flag=awake_flag;//唤醒标志,供电设备无需唤醒,节省时间
              pSlaveTx->tx_len=tx_len;
              memcpy(pSlaveTx->tx_buff, pData, tx_len);
              pSlaveTx->freq=nwk_get_sn_freq(dst_sn);//根据序列号计算频段  
              printf("dst_sn=0x%08X, freq=%.2fMHZ, tx_len=%d\n", dst_sn, pSlaveTx->freq/1000000.f, tx_len);    
              g_sNwkSlaveWork.work_state=NwkSlaveWorkIdel; //优先下行发送,否则时间点会错过             
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
        case MSCmdSetSlaveCfg:
        {
          g_sNwkSlaveWork.freq_ptr=pData[0];
          pData+=1;
          g_sNwkSlaveWork.slave_rx.run_mode=pData[0];
          pData+=1;
          printf("MSCmdSetFreqPtr=%d, run_mode=%d\n", g_sNwkSlaveWork.freq_ptr, g_sNwkSlaveWork.slave_rx.run_mode);
          g_sNwkSlaveWork.work_state=NwkSlaveWorkIdel;//进行模式切换
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
  make_buff[make_len++]=g_sNwkSlaveWork.slave_addr;
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
void nwk_slave_uart_send_heart(void)
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
void nwk_slave_uart_req_config(void)
{
  nwk_slave_uart_send_level(MSCmdSetSlaveCfg, NULL, 0);
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_uart_send_rx(u8 *buff, u8 len, RfParamStruct *rf)
{
  u8 in_buff[300]={0};
  u16 in_len=0;
  
  in_buff[in_len++]=rf->rssi>>8;
  in_buff[in_len++]=rf->rssi; 
  in_buff[in_len++]=rf->snr;   
  in_buff[in_len++]=rf->freq>>24;
  in_buff[in_len++]=rf->freq>>16;
  in_buff[in_len++]=rf->freq>>8;
  in_buff[in_len++]=rf->freq;  
  in_buff[in_len++]=rf->sf;  
  in_buff[in_len++]=rf->bw;  
  in_buff[in_len++]=len;
  memcpy(&in_buff[in_len], buff, len);
  in_len+=len;
  nwk_slave_uart_send_level(MSCmdRxData, in_buff, in_len);
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
  printf("set slave_addr=%d\n", slave_addr);
	g_sNwkSlaveWork.slave_addr=slave_addr;
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
		drv_sx1268_set_freq(g_sNwkSlaveWork.pLoRaDev, freq);
		drv_sx1268_set_sf_bw(g_sNwkSlaveWork.pLoRaDev, sf, bw);
#elif defined(LORA_LLCC68)

#endif	
	
}

/*		
================================================================================
描述 : 串口发送函数注册
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_uart_send_register(void (*fun_send)(u8 *buff, u16 len))
{
  g_sNwkSlaveWork.fun_send=fun_send; 
}


/*		
================================================================================
描述 : 设备初始化
输入 : 
输出 : 
================================================================================
*/ 
void nwk_slave_device_init(void)
{
	printf("nwk_slave_device_init ### \n");
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;
#if defined(LORA_SX1278)  
		drv_sx1278_init( g_sNwkSlaveWork.pLoRaDev);
	
#elif defined(LORA_SX1268)
		drv_sx1268_init(g_sNwkSlaveWork.pLoRaDev);
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
void nwk_slave_cad_init(void)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;	
	
#if defined(LORA_SX1278)  
	drv_sx1278_cad_init(g_sNwkSlaveWork.pLoRaDev);
	
#elif defined(LORA_SX1268)
  drv_sx1268_cad_init(g_sNwkSlaveWork.pLoRaDev);
  
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
  drv_sx1268_recv_init(g_sNwkSlaveWork.pLoRaDev); 

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
  return drv_sx1268_cad_check(g_sNwkSlaveWork.pLoRaDev);

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
u8 nwk_slave_recv_check(u8 *buff, RfParamStruct *rf_param)
{
	u8 read_size=0;
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return 0;	
	
#if defined(LORA_SX1278)  
	
	read_size=drv_sx1278_recv_check(g_sNwkSlaveWork.pLoRaDev, buff); 
	if(read_size>0)
	{
		rf_param->rssi=drv_sx1278_read_rssi(g_sNwkSlaveWork.pLoRaDev);
    rf_param->snr=drv_sx1278_read_snr(g_sNwkSlaveWork.pLoRaDev);
	}
#elif defined(LORA_SX1268)
	read_size=drv_sx1268_recv_check(g_sNwkSlaveWork.pLoRaDev, buff, 256); 
	if(read_size>0)
	{
    Sx1268PacketStatusStruct status;
    drv_sx1268_get_pack_status(g_sNwkSlaveWork.pLoRaDev, &status);
    rf_param->snr=status.SnrPkt;
		rf_param->rssi=drv_sx1268_get_rssi_inst(g_sNwkSlaveWork.pLoRaDev);    
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
// 
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
void nwk_slave_send_buff(u8 *buff, u16 len)
{
	if(g_sNwkSlaveWork.pLoRaDev==NULL)
		return;	

#if defined(LORA_SX1278)  
	drv_sx1278_send(g_sNwkSlaveWork.pLoRaDev, buff, len); 
	
#elif defined(LORA_SX1268)
  drv_sx1268_send(g_sNwkSlaveWork.pLoRaDev, buff, len); 

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
  return drv_sx1268_send_check(g_sNwkSlaveWork.pLoRaDev); 

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

#if defined(LORA_SX1278)  
	drv_sx1278_send(g_sNwkSlaveWork.pLoRaDev, buff, 1); 

#elif defined(LORA_SX1268)
  drv_sx1268_send(g_sNwkSlaveWork.pLoRaDev, buff, 1);

#elif defined(LORA_LLCC68)

#endif
  nwk_delay_ms(5);
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
				u32 tx_time=nwk_calcu_air_time(pSlaveBroad->sf, pSlaveBroad->bw, pSlaveBroad->broad_len);//发送时间,冗余
				pSlaveBroad->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
				pSlaveBroad->wait_cnts=tx_time/1000+2;//等待秒数
				printf("tx_time=%d ms, wait_cnts=%d\n", tx_time, pSlaveBroad->wait_cnts);
				pSlaveBroad->broad_state=NwkSlaveBroadTxCheck;  				
			}
			break;
		}
    case NwkSlaveBroadTxCheck:
    {
      u32 now_time=nwk_get_sec_counter();
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
      if(pSlaveRx->run_mode==NwkRunModeStatic)//静态模式
      {
        pSlaveRx->rx_state=NwkSlaveRxStaticInit;
      }
      else//动态模式
      {
        pSlaveRx->group_id=0;
        pSlaveRx->cad_cnts=0;
        pSlaveRx->rx_state=NwkSlaveRxCadInit;        
      }

//      printf("###### NwkSlaveRxInit!\n");        
      break;
    }
    case NwkSlaveRxStaticInit://静态模式初始化
    {
      u8 sf=0, bw=0;
      u8 wireless_ptr=g_sNwkSlaveWork.slave_addr-1;
      pSlaveRx->freq=NWK_GW_BASE_FREQ+(g_sNwkSlaveWork.freq_ptr+(g_sNwkSlaveWork.slave_addr-1)*NWK_GW_SPACE_FREQ)*1000000;//计算监听频率
      nwk_get_static_channel4(wireless_ptr, &sf, &bw);
      pSlaveRx->curr_sf=sf;
      pSlaveRx->curr_bw=bw;       
      nwk_slave_set_lora_param(pSlaveRx->freq, sf, bw);
      nwk_slave_recv_init();  

      pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
      pSlaveRx->wait_cnts=60;            
      pSlaveRx->rx_state=NwkSlaveRxStaticFirstCheck;  
      printf("start listen, P(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, sf, bw);
      break;
    }
    case NwkSlaveRxStaticFirstCheck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 recv_len=nwk_slave_recv_check(g_sNwkSlaveWork.slave_rx.recv_buff, &g_sNwkSlaveWork.rf_param); 
      if(recv_len>0)
      {
        u8 *pBuff=g_sNwkSlaveWork.slave_rx.recv_buff;
        u8 head[1]={0xA7};
        u8 *pData=nwk_find_head(pBuff, recv_len, head, 1);
        if(pData)
        {
          pData+=1;
          u32 src_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];
          pData+=4;
          u8 will_len=pData[0];
          pData+=1;
          printf("static first src_sn=0x%08X, will_len=%d\n", src_sn, will_len);
          pSlaveRx->will_len=will_len;
          pSlaveRx->freq=nwk_get_sn_freq(src_sn);//使用节点的频段
          printf("send ack, P(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, pSlaveRx->curr_sf, pSlaveRx->curr_bw);
          nwk_slave_set_lora_param(pSlaveRx->freq, pSlaveRx->curr_sf, pSlaveRx->curr_bw);
          u8 ack_buff[10]={0};
          u8 ack_len=0;
          ack_buff[ack_len++]=0xA7;
          ack_buff[ack_len++]=src_sn>>24;
          ack_buff[ack_len++]=src_sn>>16;
          ack_buff[ack_len++]=src_sn>>8;
          ack_buff[ack_len++]=src_sn;
          nwk_slave_send_buff(ack_buff, ack_len);//回复
          u32 tx_time=nwk_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, ack_len);//接收等待时间
          pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
          pSlaveRx->wait_cnts=tx_time/1000+1;             
          pSlaveRx->rx_state=NwkSlaveRxStaticAckCheck;//回复包发送检测           
        }
      }   
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//没收到数据
      {
        printf("listen time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }
      break;
    }
    case NwkSlaveRxStaticAckCheck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 result=nwk_slave_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("static ack send ok!\ninto recv mode!\n");
        nwk_slave_recv_init();  
        u32 tx_time=nwk_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, pSlaveRx->will_len);//接收等待时间
        pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=tx_time/1000+2;            
        pSlaveRx->rx_state=NwkSlaveRxStaticAppCheck;  
      }
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//发送超时
      {
        printf("static ack send time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }        
      break;
    }
    case NwkSlaveRxStaticAppCheck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 recv_len=nwk_slave_recv_check(g_sNwkSlaveWork.slave_rx.recv_buff, &g_sNwkSlaveWork.rf_param); 
      if(recv_len>0)
      {
        u8 *pBuff=g_sNwkSlaveWork.slave_rx.recv_buff;
        printf_hex("recv=", pBuff, recv_len);
        g_sNwkSlaveWork.rf_param.freq=pSlaveRx->freq;
        g_sNwkSlaveWork.rf_param.sf=pSlaveRx->curr_sf;
        g_sNwkSlaveWork.rf_param.bw=pSlaveRx->curr_bw;
        
        nwk_slave_uart_send_rx(pBuff, recv_len, &g_sNwkSlaveWork.rf_param);//无线数据发送到主机
        pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=2;         
        pSlaveRx->rx_state=NwkSlaveRxAckWait;//等待主机回复        
      } 
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//超时
      {
        printf("static recv wait time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }       
      break;
    }
    
    //动态模式
    case NwkSlaveRxCadInit:
    {
      u8 sf=0, bw=0;
      pSlaveRx->freq=NWK_GW_BASE_FREQ+(g_sNwkSlaveWork.freq_ptr+(g_sNwkSlaveWork.slave_addr-1)*NWK_GW_SPACE_FREQ)*1000000;//计算监听频率
      nwk_get_up_channel(pSlaveRx->group_id, &sf, &bw);
      pSlaveRx->curr_sf=sf;
      pSlaveRx->curr_bw=bw;      
//			printf("cad1 param(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, sf, bw);
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
        static u8 loops=0;
        pSlaveRx->group_id++;
        if(pSlaveRx->group_id>=NWK_UP_CHANNEL_NUM)
        {
          loops++;
          pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
        } 
        else if(loops%2==1 && pSlaveRx->group_id>=NWK_UP_CHANNEL_NUM-1)//比较费时的参数两轮轮一次
        {
          loops++;
          pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合          
        }
				else
				{
          u8 sf=0, bw=0;
          nwk_get_up_channel(pSlaveRx->group_id, &sf, &bw);
          pSlaveRx->curr_sf=sf;
          pSlaveRx->curr_bw=bw;      
//          printf("cad2 param(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, sf, bw);
          nwk_slave_set_lora_param(pSlaveRx->freq, sf, bw);
          nwk_slave_cad_init();          
				}
      }
      else if(result==CadResultSuccess)//搜索到
      {
        printf("cad OK***(%.2f, %d, %d)\n", pSlaveRx->freq/1000000.0, pSlaveRx->curr_sf, pSlaveRx->curr_bw);
        u8 sf=0, bw=0;
        nwk_get_up_channel(pSlaveRx->group_id, &sf, &bw);
        pSlaveRx->curr_sf=sf;
        pSlaveRx->curr_bw=bw;
        pSlaveRx->freq+=1000000;
        printf("group id=%d, rx sniff param(%.2f, %d, %d)\n", pSlaveRx->group_id, pSlaveRx->freq/1000000.0, sf, bw);
//        nwk_delay_ms(20); 
        nwk_slave_set_lora_param(pSlaveRx->freq, sf, bw);  
        for(u16 i=0; i<40; i++)//
        {
          nwk_slave_send_sniff(sf, bw);//返回嗅探帧
        }
        
        printf("into recv mode!\n"); 
        nwk_slave_recv_init();//进入接收
        u32 tx_time=nwk_calcu_air_time(sf, bw, 4);//接收等待时间
        pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=tx_time/1000+2;             
        pSlaveRx->rx_state=NwkSlaveRxCheck; 
        printf("will wait time=%ds\n", pSlaveRx->wait_cnts);  
      }        
      break;
    }     
    case NwkSlaveRxCheck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 recv_len=nwk_slave_recv_check(g_sNwkSlaveWork.slave_rx.recv_buff, &g_sNwkSlaveWork.rf_param); 
      if(recv_len>0)
      {
        u8 *pBuff=g_sNwkSlaveWork.slave_rx.recv_buff;
        printf_hex("recv=", pBuff, recv_len);
        
        //查看是否为应用数据长度(探测报文)
        u8 head[1]={0xA8};
        u8 *pData=nwk_find_head(pBuff, recv_len, head, 1);
        if(pData)
        {
          u16 crcValue=pData[2]<<8|pData[3];
          if(crcValue==nwk_crc16(pData, 2))
          {
            u8 will_len=pData[1];//待接收的数据长度
            printf("app len=%d\n", will_len);
            u32 tx_time=nwk_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, will_len);//接收等待时间
            pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
            pSlaveRx->wait_cnts=tx_time/1000+2; 
            printf("app wait time=%ds\n", pSlaveRx->wait_cnts);
            return;            
          }
        }
        
        //应用数据解析
        
        g_sNwkSlaveWork.rf_param.freq=pSlaveRx->freq;
        g_sNwkSlaveWork.rf_param.sf=pSlaveRx->curr_sf;
        g_sNwkSlaveWork.rf_param.bw=pSlaveRx->curr_bw;
        
        nwk_slave_uart_send_rx(pBuff, recv_len, &g_sNwkSlaveWork.rf_param);//无线数据发送到主机
        pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=2;         
        pSlaveRx->rx_state=NwkSlaveRxAckWait;//等待主机回复
                
      }   
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//超时
      {
        printf("recv wait time out!\n");
        pSlaveRx->group_id++;//换下一组参数 
        if(pSlaveRx->group_id>=NWK_UP_CHANNEL_NUM)
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
      u32 now_time=nwk_get_sec_counter();
      if(now_time-pSlaveRx->start_rtc_time>=pSlaveRx->wait_cnts)//超时
      {
        printf("master ack time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }
      break;
    }
    case NwkSlaveRxAckCheck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 result=nwk_slave_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("ack send ok!\n");
        if(pSlaveRx->down_len>0)
        {
          nwk_slave_send_buff(pSlaveRx->down_buff, pSlaveRx->down_len); 
          u32 tx_time=nwk_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, pSlaveRx->down_len);//接收等待时间
          pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
          pSlaveRx->wait_cnts=tx_time/1000+1;             
          pSlaveRx->rx_state=NwkSlaveRxDownCheck;//回复包发送检测            
        }
        else
        {
          pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
        }
      }
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//发送超时
      {
        printf("ack send time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }    
      break;
    }
    case NwkSlaveRxDownCheck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 result=nwk_slave_send_check();//发送完成检测   
      if(result)//发送完成
      {
        printf("down send ok!\n");
        printf("into recv mode!\n"); 
        nwk_slave_recv_init();//进入接收
        u32 tx_time=nwk_calcu_air_time(pSlaveRx->curr_sf, pSlaveRx->curr_bw, 20);//接收等待时间
        pSlaveRx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
        pSlaveRx->wait_cnts=tx_time/1000+2;             
        pSlaveRx->rx_state=NwkSlaveRxDownWait; 
        printf("down ack wait time=%ds\n", pSlaveRx->wait_cnts);         
      }
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//发送超时
      {
        printf("down send time out!\n");
        pSlaveRx->rx_state=NwkSlaveRxIdel;//结束本回合
      }       
      break;
    }
    case NwkSlaveRxDownWait:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 recv_len=nwk_slave_recv_check(g_sNwkSlaveWork.slave_rx.recv_buff, &g_sNwkSlaveWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
        printf_hex("down ack=", g_sNwkSlaveWork.slave_rx.recv_buff, recv_len);
        printf("clear down buff!\n");
        pSlaveRx->down_len=0;
        
        g_sNwkSlaveWork.rf_param.freq=pSlaveRx->freq;
        g_sNwkSlaveWork.rf_param.sf=pSlaveRx->curr_sf;
        g_sNwkSlaveWork.rf_param.bw=pSlaveRx->curr_bw;

        nwk_slave_uart_send_rx(g_sNwkSlaveWork.slave_rx.recv_buff, recv_len, &g_sNwkSlaveWork.rf_param);//无线数据发送到主机
        pSlaveRx->rx_state=NwkSlaveRxIdel; //结束本回合
      }   
      else if(now_time-pSlaveRx->start_rtc_time>pSlaveRx->wait_cnts)//超时
      {
        printf("down ack time out!\n");
        printf("clear down buff!\n");
        pSlaveRx->down_len=0;        
        pSlaveRx->rx_state=NwkSlaveRxIdel; //结束本回合
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
      pSlaveTx->curr_sf=NWK_BROAD_SF;
      pSlaveTx->curr_bw=NWK_BROAD_BW;      
      nwk_slave_set_lora_param(pSlaveTx->freq, pSlaveTx->curr_sf, pSlaveTx->curr_bw);

      printf("***set wake param (%.2f, %d, %d)\n", pSlaveTx->freq/1000000.0, pSlaveTx->curr_sf, pSlaveTx->curr_bw);
      pSlaveTx->sniff_cnts=0; 
      pSlaveTx->tx_state=NwkSlaveTxWake;//进行唤醒 
      
      break;
    }
    case NwkSlaveTxWake:
    {
      if(pSlaveTx->awake_flag)
      {
        printf("send wake!\n");
        for(u16 i=0; i<100; i++)
        {
          nwk_slave_send_sniff(pSlaveTx->curr_sf, pSlaveTx->curr_bw);//发送嗅探帧,唤醒设备
        }        
      }
      u8 test_buff[10]={0};
      u8 test_len=0;
      u32 dst_sn=pSlaveTx->dst_sn; 
      test_buff[test_len++]=0xA9;
      test_buff[test_len++]=dst_sn>>24;
      test_buff[test_len++]=dst_sn>>16;
      test_buff[test_len++]=dst_sn>>8;
      test_buff[test_len++]=dst_sn;
      test_buff[test_len++]=pSlaveTx->tx_len;
      u16 crcValue=nwk_crc16(test_buff, test_len);
      test_buff[test_len++]=crcValue>>8;
      test_buff[test_len++]=crcValue;
      
      nwk_slave_cad_init();//状态切换
//      nwk_delay_ms(1000);
      printf("send sn buff!\n");
      nwk_slave_send_buff(test_buff, test_len);//发送SN匹配包
      u32 tx_time=nwk_calcu_air_time(pSlaveTx->curr_sf, pSlaveTx->curr_bw, pSlaveTx->tx_len);//发送时间,冗余
      pSlaveTx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
      pSlaveTx->wait_cnts=tx_time/1000+2;//等待秒数
      pSlaveTx->tx_state=NwkSlaveTxSnCheck;     
      break;
    }
    case NwkSlaveTxSnCheck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 result=nwk_slave_send_check();//发送完成检测
      if(result)//发送完成,进入ADR模式
      {
        printf("send sn ok!\n");
        pSlaveTx->sniff_cnts=0;
        pSlaveTx->group_id=0;
        pSlaveTx->tx_state=NwkSlaveTxAdrSniffInit; 
      }
      else if(now_time-pSlaveTx->start_rtc_time>pSlaveTx->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pSlaveTx->wait_cnts);
        pSlaveTx->tx_state=NwkSlaveTxIdel;  
      }      
      break;
    }
    case NwkSlaveTxAdrSniffInit:
    {
      if(pSlaveTx->group_id>=NWK_DOWN_CHANNEL_NUM)
      {
        pSlaveTx->tx_len=0;
        printf("clear buff, tx failed!\n");
        pSlaveTx->tx_state=NwkSlaveTxIdel;  //结束
        return;
      }
      u8 sf=0, bw=0;
      u32 freq=pSlaveTx->freq;//+pSlaveTx->group_id*500000;
      nwk_get_down_channel(pSlaveTx->group_id, &sf, &bw); //嗅探参数
      nwk_slave_set_lora_param(freq, sf, bw);
			 
			for(u8 i=0; i<5; i++)//
			{
				nwk_slave_send_sniff(sf, bw);//发送嗅探帧
			}
      pSlaveTx->sniff_cnts++;
      printf("***group id=%d, send sniff (%.2f, %d, %d), cnts=%d\n", pSlaveTx->group_id, freq/1000000.f, sf, bw, pSlaveTx->sniff_cnts);
//      nwk_get_down_channel(pSlaveTx->group_id, &sf, &bw);//以本通道组的第二个参数作为CAD监听参数
      
      freq+=1000000;
      nwk_slave_set_lora_param(freq, sf, bw);
      nwk_slave_cad_init(); //开始监听返回
      printf("cad ack P(%.2f, %d, %d)\n", freq/1000000.f, sf, bw);
			pSlaveTx->cad_cnts=0;
      pSlaveTx->curr_sf=sf;
      pSlaveTx->curr_bw=bw; //记录监听参数,发送时候再次使用      
//      pSlaveTx->tx_state=NwkSlaveTxAdrSniffCheck;      
      for(u8 i=0; i<6; )//CAD回复检测次数
      {
        u8 result=nwk_slave_cad_check();
        if(result==CadResultFailed)//没搜索到
        {
          nwk_slave_cad_init();//继续监听
//          printf("111 cad init\n");
          i++;
        }
        else if(result==CadResultSuccess)//搜索成功
        {
          printf("**** CAD OK!\n");
          nwk_delay_ms(200);//适当延时,等待对方嗅探帧发送完
          printf("send app!\n");
          nwk_slave_send_buff(pSlaveTx->tx_buff, pSlaveTx->tx_len);//发送数据包
          u32 tx_time=nwk_calcu_air_time(pSlaveTx->curr_sf, pSlaveTx->curr_bw, pSlaveTx->tx_len);//发送时间,冗余
          pSlaveTx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
          pSlaveTx->wait_cnts=tx_time/1000+2;//等待秒数
          pSlaveTx->tx_state=NwkSlaveTxRunning;     
          return;          
        }        
      }
      //没有检测到CAD
      if(pSlaveTx->sniff_cnts<5)//同一组参数嗅探多次
      {
        pSlaveTx->tx_state=NwkSlaveTxAdrSniffInit;//继续嗅探
      }
      else
      {
        pSlaveTx->group_id++;//换下一组参数 
        pSlaveTx->sniff_cnts=0;
        pSlaveTx->tx_state=NwkSlaveTxAdrSniffInit;//继续嗅探          
      }       
      break;
    }
    
//    case NwkSlaveTxAdrSniffCheck:
//    {
//      u8 result=nwk_slave_cad_check();
//      if(result==CadResultFailed)//没搜索到
//      {
////        printf("sniff no cad!\n");
//        pSlaveTx->cad_cnts++;
//        if(pSlaveTx->cad_cnts<5)
//        {
//          nwk_slave_cad_init();//继续监听
//        }
//        else if(pSlaveTx->sniff_cnts<3)
//        {
//          pSlaveTx->tx_state=NwkSlaveTxAdrSniffInit;//继续嗅探  
//        }
//        else
//        {
//          pSlaveTx->group_id++;
//          pSlaveTx->sniff_cnts=0;
//          pSlaveTx->tx_state=NwkSlaveTxAdrSniffInit;  //继续嗅探           
//        }
//      }
//      else if(result==CadResultSuccess)//搜索成功 
//      {
//        printf("**** CAD OK!\n");
//        nwk_delay_ms(pSlaveTx->group_id*50+300);//适当延时,等待对方嗅探帧发送完
//        nwk_slave_send_buff(pSlaveTx->tx_buff, pSlaveTx->tx_len);//发送数据包
//        u32 tx_time=nwk_calcu_air_time(pSlaveTx->curr_sf, pSlaveTx->curr_bw, pSlaveTx->tx_len);//发送时间,冗余
//        pSlaveTx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
//        pSlaveTx->wait_cnts=tx_time/1000+2;//等待秒数
//        pSlaveTx->tx_state=NwkSlaveTxRunning;
//      }       
//      break;
//    } 
    case NwkSlaveTxRunning:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 result=nwk_slave_send_check();//发送完成检测
      if(result)//发送完成
      {
        printf("send app ok!   \nwait ack!\n");
        nwk_slave_recv_init();//进入接收,等待回复
        u32 tx_time=nwk_calcu_air_time(pSlaveTx->curr_sf, pSlaveTx->curr_bw, 20);//接收回复包等待时间
        pSlaveTx->start_rtc_time=nwk_get_sec_counter();//记录当前时间,防止超时
        pSlaveTx->wait_cnts=tx_time/1000+1;
        pSlaveTx->tx_state=NwkSlaveTxWaitAck;
      }
      else if(now_time-pSlaveTx->start_rtc_time>pSlaveTx->wait_cnts)//发送超时
      {
        //这里要加入故障跟踪,经常超时说明硬件有问题
        printf("tx buff time out! wait time=%ds\n", pSlaveTx->wait_cnts);
        pSlaveTx->tx_state=NwkSlaveTxIdel;  
      }      
      break;
    }
    case NwkSlaveTxWaitAck:
    {
      u32 now_time=nwk_get_sec_counter();
      u8 recv_len=nwk_slave_recv_check(g_sNwkSlaveWork.slave_rx.recv_buff, &g_sNwkSlaveWork.rf_param);
      if(recv_len>0)
      {
        //数据解析
        printf_hex("ack=", g_sNwkSlaveWork.slave_rx.recv_buff, recv_len);
        printf("clear tx buff!\n");
        pSlaveTx->tx_len=0;
        
        g_sNwkSlaveWork.rf_param.freq=pSlaveTx->freq;
        g_sNwkSlaveWork.rf_param.sf=pSlaveTx->curr_sf;
        g_sNwkSlaveWork.rf_param.bw=pSlaveTx->curr_bw;

        nwk_slave_uart_send_rx(g_sNwkSlaveWork.slave_rx.recv_buff, recv_len, &g_sNwkSlaveWork.rf_param);//无线数据发送到主机
        pSlaveTx->tx_state=NwkSlaveTxIdel;
      }   
      else if(now_time-pSlaveTx->start_rtc_time>pSlaveTx->wait_cnts)//超时
      {
        printf("wait ack time out!\n");
        printf("clear tx buff!\n");
        pSlaveTx->tx_len=0;        
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
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 




