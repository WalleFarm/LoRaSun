
#include "drv_bc260.h"

BC260WorkStruct g_sBc260Work={0};
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_init(UART_Struct *pUART, u8 server_type, u8 sleep_mode)
{
  g_sBc260Work.pUART=pUART;
  g_sBc260Work.server_type=server_type;
  g_sBc260Work.sleep_mode=sleep_mode;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_fun_register(void (*fun_recv_parse)(u8 *buff, u16 len))
{
  g_sBc260Work.fun_recv_parse=fun_recv_parse;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_uart_send(char *send_buff)
{
//	printf("***AT CMD:  %s", send_buff);
	if(g_sBc260Work.pUART!=NULL)	
	{
		UART_Send(g_sBc260Work.pUART->PortNum, (u8*)send_buff, strlen(send_buff));
	}
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_send_at(char *cmd_buff)
{
	static char send_buff[100]={0};
	if(strlen(cmd_buff)+10<sizeof(send_buff))
	{
		memset(send_buff, 0, sizeof(send_buff));
		sprintf(send_buff, "AT+%s\r\n", cmd_buff);
		drv_bc260_uart_send(send_buff);	
	}
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_send_data(u8 *buff, u16 len)
{
  if(g_sBc260Work.state!=BC260_STAT_OK)
    return;
  
	static char send_buff[200]={0};
	if(len*2+10<sizeof(send_buff))
	{
		memset(send_buff, 0, sizeof(send_buff));
		sprintf(send_buff, "AT+NMGS=%d,", len);
    for(u16 i=0; i<len; i++)
    {
      char temp_buff[5]={0};
      sprintf(temp_buff, "%02X", buff[i]);
      strcat(send_buff, temp_buff);
    }
    strcat(send_buff, "\r\n");
//    printf("send data=%s\n", send_buff);
		drv_bc260_uart_send(send_buff);	
	}  
}

/*		
================================================================================
描述 : 唤醒
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_awake(void)
{
  drv_bc260_send_at("QSCLK=0");
  delay_os(200);
  drv_bc260_send_at("QSCLK=0");
}

/*		
================================================================================
描述 : 注册流程
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_reg_process(void)
{
  static u32 last_sec_time=0, wait_time=2;
  u32 now_sec_time=drv_get_sec_counter();
  if(now_sec_time-last_sec_time>wait_time)
  {
    switch(g_sBc260Work.state)
    {
      case BC260_STATE_START:
      {
        drv_bc260_send_at("QRST=1");//复位模块  
        g_sBc260Work.state=BC260_STAT_INIT;
        wait_time=5;
        break;
      }
      case BC260_STAT_INIT:
      {
        drv_bc260_send_at("CSQ");
        delay_os(200);
        drv_bc260_send_at("QSCLK=0");//禁用休眠模式
        delay_os(200);
        drv_bc260_send_at("CSCON=1");//使能信令连接状态上报
        g_sBc260Work.state=BC260_STAT_CEREG;
        wait_time=2;
        break;
      }    
      case BC260_STAT_CEREG://网络注册状态
      { 
        drv_bc260_send_at("CEREG?");//查询
        wait_time=2;
        break;
      }       
      case BC260_STAT_AEP_INIT:
      { 
        drv_bc260_send_at("NNMI=1");//将数据接收模式设置为直吐模式
        delay_os(200);
        drv_bc260_send_at("NCFG=0,86400");//配置生命周期时间,秒
        g_sBc260Work.state=BC260_STAT_AEP_CONNECT;
        wait_time=2;
        break;
      }
      case BC260_STAT_AEP_CONNECT://连接到服务器
      { 
        drv_bc260_send_at("NCDPOPEN=\"221.229.214.202\",5683");
        wait_time=3;
        break;
      }     

      case BC260_STAT_OK:
      { 
        if(g_sBc260Work.imei_buff[0]==0)
        {
          drv_bc260_send_at("CGSN=1");
          delay_os(200);
        }
        else if(g_sBc260Work.iccid_buff[0]==0)
        {
          drv_bc260_send_at("QCCID");
          delay_os(200);          
        }
        else
        {
          drv_bc260_send_at("CSQ");
        }
        
        wait_time=5;
        break;
      }       
    }
    
    last_sec_time=drv_get_sec_counter();
  }
  
}

/*		
================================================================================
描述 : 十六进制字符串转字节数组
输入 : 
输出 : 
================================================================================
*/
void hex_str_to_array(char *hex_str, char *out_array, u16 array_len)
{
  for(u16 i=0; i<array_len; i++)
  {
    sscanf(hex_str+i*2, "%02hhX", &out_array[i]);
  }
}

/*		
================================================================================
描述 : 接收处理
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_recv_process(void)
{
	u16 recv_len;
	char *pData=NULL;
	
	if(g_sBc260Work.pUART->iRecv>0)
	{
		recv_len=0;
		while(recv_len<g_sBc260Work.pUART->iRecv)
		{
			recv_len=g_sBc260Work.pUART->iRecv;
			delay_ms(5);
		}
		char *pBuff=(char*)g_sBc260Work.pUART->pBuff;
		printf("bc260 recv=%s\n", g_sBc260Work.pUART->pBuff);
    
    if( (pData=strstr(pBuff, "+NNMI: "))!=NULL )//AEP接收到数据
    {
      pData+=strlen("+NNMI: ");
      u16 data_len=atoi(pData);
      if( (pData=strstr(pData, ","))!=NULL && data_len<100)
      {
        pData+=1;
        char data_buff[100]={0};
        hex_str_to_array(pData, data_buff, data_len);
        printf_hex("data=", (u8*)data_buff, data_len);
        if(g_sBc260Work.fun_recv_parse!=NULL)
        {
          g_sBc260Work.fun_recv_parse((u8*)data_buff, data_len);
        }
      }
    }
    else if((pData=strstr(pBuff, "+QLWEVTIND: "))!=NULL )
    {
      pData+=strlen("+QLWEVTIND: ");
      u8 type=atoi(pData);
      if(type==3)//AEP服务器订阅成功
      {
        g_sBc260Work.state=BC260_STAT_OK;
      }
    }
    else if((pData=strstr(pBuff, "+CEREG: "))!=NULL )
    {
      pData+=strlen("+CEREG: ");
      pData+=2;
      u8 state=atoi(pData);
      if(state==1 || state==5)//网络已注册
      {
        switch(g_sBc260Work.server_type)
        {
          case BC260_SERVER_ONENET://移动OneNet
          {
            
            break;
          }
          case BC260_SERVER_CHINANET_IOT://电信IOT
          {
            
            break;
          }
          case BC260_SERVER_CHINANET_AEP://电信AEP
          {
            g_sBc260Work.state=BC260_STAT_AEP_INIT;//进入AEP的初始化
            break;
          }
          case BC260_SERVER_ALI://阿里平台
          {
            
            break;
          }          
        }
        
      }
    }    
    else if((pData=strstr(pBuff, "+CSQ: "))!=NULL )
    {
      pData+=strlen("+CSQ: ");
      g_sBc260Work.rssi=atoi(pData);
      printf("***rssi=%d\n", g_sBc260Work.rssi);
    }    
    else if((pData=strstr(pBuff, "+CGSN: "))!=NULL )
    {
      pData+=strlen("+CGSN: ");
      memcpy(g_sBc260Work.imei_buff, pData, 15);
      printf("***imei=%s\n", g_sBc260Work.imei_buff);
    }      
    else if((pData=strstr(pBuff, "+QCCID: "))!=NULL )
    {
      pData+=strlen("+QCCID: ");
      memcpy(g_sBc260Work.iccid_buff, pData, 20);
      printf("***iccid=%s\n", g_sBc260Work.imei_buff);
    }  

    UART_Clear(g_sBc260Work.pUART);
  }

}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_main(void)
{
  drv_bc260_reg_process();
  drv_bc260_recv_process();
}

/*		
================================================================================
描述 : 获取IMEI
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_copy_imei(u8 *imei_buff, u16 size)
{
  if(size>=15)
  {
    memcpy(imei_buff, g_sBc260Work.imei_buff, 15);
  }
}

/*		
================================================================================
描述 : 获取ICCID
输入 : 
输出 : 
================================================================================
*/
void drv_bc260_copy_iccid(u8 *iccid_buff, u16 size)
{
  if(size>=20)
  {
    memcpy(iccid_buff, g_sBc260Work.iccid_buff, 20);
  }
}

/*		
================================================================================
描述 : 获取信号强度
输入 : 
输出 : 
================================================================================
*/
u8 drv_bc260_get_rssi(void)
{
  return g_sBc260Work.rssi;
}








