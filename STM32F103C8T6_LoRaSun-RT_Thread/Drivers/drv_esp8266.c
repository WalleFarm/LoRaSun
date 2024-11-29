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


#include "drv_esp8266.h" 

Esp8266WorkStruct g_sEsp8266Work={0};

 
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_init(UART_Struct *pUART, char *sta_ssid, char *sta_passwd)
{
	g_sEsp8266Work.pUART=pUART;
	g_sEsp8266Work.state=ESP8266_STATE_START;
  g_sEsp8266Work.listen_port=333;
  
  u16 len=strlen(sta_ssid);
  if(len>1 && len<sizeof(g_sEsp8266Work.sta_ssid))
  {
    strcpy(g_sEsp8266Work.sta_ssid, sta_ssid);
  }
  
  len=strlen(sta_passwd);
  if(len>1 && len<sizeof(g_sEsp8266Work.sta_passwd))
  {
    strcpy(g_sEsp8266Work.sta_passwd, sta_passwd);
  }  
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_set_uart(UART_Struct *pUART)
{
	g_sEsp8266Work.pUART=pUART;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_set_client(u8 index, char *dst_addr, u16 port, char *type)
{
  if(index<ESP8266_MAX_LINK_NUM)
  {
    Esp8266ClientStruct *pClient=&g_sEsp8266Work.client_list[index];
    if(strlen(dst_addr)<sizeof(pClient->dst_addr))
    {
      strcpy(pClient->dst_addr, dst_addr);
      pClient->sock_id=index;
      pClient->dst_port=port;
      strcpy(pClient->type, type);
      pClient->conn_state=0;       
      pClient->heart_time=60;      
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
void drv_esp8266_set_ap(char *ap_ssid, char *ap_passwd, u16 listen_port)
{
  u16 len=strlen(ap_ssid);
  if(len>1 && len<sizeof(g_sEsp8266Work.ap_ssid))
  {
    strcpy(g_sEsp8266Work.ap_ssid, ap_ssid);
  }
  
  len=strlen(ap_passwd);
  if(len>1 && len<sizeof(g_sEsp8266Work.ap_passwd))
  {
    strcpy(g_sEsp8266Work.ap_passwd, ap_passwd);
  }   
  g_sEsp8266Work.listen_port=listen_port;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_fun_register(void (*fun_recv_parse)(u8 chn, u8 *buff, u16 len))
{
	g_sEsp8266Work.fun_recv_parse=fun_recv_parse; 
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_uart_send(char *send_buff)
{
//	printf("AT CMD:  %s", send_buff);
	if(g_sEsp8266Work.pUART!=NULL)	
	{
		UART_Send(g_sEsp8266Work.pUART->PortNum, (u8*)send_buff, strlen(send_buff));
	}
}
 
 
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_send_at(char *cmd_buff)
{
	static char send_buff[100]={0};
	if(strlen(cmd_buff)+10<sizeof(send_buff))
	{
		memset(send_buff, 0, sizeof(send_buff));
		sprintf(send_buff, "AT+%s\r\n", cmd_buff);
		drv_esp8266_uart_send(send_buff);	
	}
}

/*		
================================================================================
描述 : 数据发送
输入 : 
输出 : 
================================================================================
*/
int drv_esp8266_send_data(u8 sock_id, u8 *buff, u16 len)
{
	char cmd_buff[30]={0};
  if(sock_id<ESP8266_MAX_LINK_NUM)
  {
    if(g_sEsp8266Work.client_list[sock_id].conn_state>0)
    {
      sprintf(cmd_buff, "AT+CIPSEND=%u,%u\r\n", sock_id,len);
      drv_esp8266_uart_send(cmd_buff);
      if(g_sEsp8266Work.pUART!=NULL)	
      {
        UART_Send(g_sEsp8266Work.pUART->PortNum, buff, len);
        return len;
      }	      
    }
  }
	return 0;
}

/*		
================================================================================
描述 : 客户端连接
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_client_connect(u8 sock_id, char * type, char *dst_addr, u16 dst_port)
{
  static char cmd_buff[100]={0};  
  
  sprintf(cmd_buff, "CIPSTART=%d,\"%s\",\"%s\",%d",sock_id, type, dst_addr, dst_port);
  drv_esp8266_send_at(cmd_buff);
}

/*		
================================================================================
描述 : 客户端断开
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_close(u8 sock_id)
{
  if(sock_id>ESP8266_MAX_LINK_NUM)
    return;
  char cmd_buff[30]={0};  
  sprintf(cmd_buff, "CIPCLOSE=%d", sock_id);
  drv_esp8266_send_at(cmd_buff);
  if(sock_id==ESP8266_MAX_LINK_NUM)//全部关闭
  {
    for(u8 i=0; i<ESP8266_MAX_LINK_NUM; i++)
    {
      g_sEsp8266Work.client_list[i].conn_state=0;
    }
  }
  else
  {
    g_sEsp8266Work.client_list[sock_id].conn_state=0;
  }
}

/*		
================================================================================
描述 : 客户端连接管理任务
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_connect_process(void)
{
  u32 now_sec_time=drv_get_sec_counter();
  for(u8 i=0; i<ESP8266_MAX_LINK_NUM; i++)
  {
    Esp8266ClientStruct *pClient=&g_sEsp8266Work.client_list[i];
    if(pClient->dst_port>0)//有连接需求
    {
      if(pClient->conn_state==0)
      {
        drv_esp8266_client_connect(pClient->sock_id, pClient->type, pClient->dst_addr, pClient->dst_port);
        delay_os(1000);
        char *pData=(char*)g_sEsp8266Work.pUART->pBuff;  
//        printf("***8266 recv=%s\n", pData);        
        if(strstr(pData, "ALREADY CONNECTED")!=NULL)
        {
          printf("sock_id=%d, already connected!\n", i);
          pClient->conn_state=1;
          pClient->keep_time=now_sec_time;
          UART_Clear(g_sEsp8266Work.pUART);//清理串口数据     
        }
        else if(strstr(pData, ",CONNECT")!=NULL)
        {
          printf("sock_id=%d, new connected!\n", i);
          pClient->conn_state=1;
          pClient->keep_time=now_sec_time;
          UART_Clear(g_sEsp8266Work.pUART);//清理串口数据             
        }
        else if(strstr(pData, ",CLOSED")!=NULL)
        {
          printf("sock_id=%d, error close!\n", i);
          pClient->conn_state=0;
          UART_Clear(g_sEsp8266Work.pUART);//清理串口数据             
        }        
      }
      else
      {
        int det_time=now_sec_time-pClient->keep_time;
        if(det_time>pClient->heart_time)//心跳超时
        {
          printf("sock_id=%d, heart time out!\n", i);
          drv_esp8266_close(pClient->sock_id);
        }
      }
    } 
  } 

}  

/*		
================================================================================
描述 : 网络注册函数
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_reg_process(void)
{
	static u32 last_sec_time=0, wait_time=2;
  static char cmd_buff[100]={0};
	u32 now_sec_time=drv_get_sec_counter();
	if(now_sec_time-last_sec_time>wait_time)
	{
		switch(g_sEsp8266Work.state)
		{
			case ESP8266_STATE_START:
			{
        delay_os(2000);
				drv_esp8266_uart_send("ATE0\r\n");
				delay_os(200);         
        
				drv_esp8266_send_at("CWMODE_DEF=3");//WiFi模式  STA+AP
				delay_os(200); 
				drv_esp8266_send_at("CWAUTOCONN=1");//上电自动连接
				delay_os(200);   

        if(strlen(g_sEsp8266Work.sta_ssid)>0)
        {
          sprintf(cmd_buff, "CWJAP_DEF=\"%s\",\"%s\"", g_sEsp8266Work.sta_ssid, g_sEsp8266Work.sta_passwd);
//          sprintf(cmd_buff, "CWJAP=\"%s\",\"%s\"", g_sEsp8266Work.sta_ssid, g_sEsp8266Work.sta_passwd);
          drv_esp8266_send_at(cmd_buff);
          delay_os(1000);        
          printf("set wifi ssid=%s, pwd=%s\n", g_sEsp8266Work.sta_ssid, g_sEsp8266Work.sta_passwd);
        }                   
        drv_esp8266_send_at("RST");//复位模块   
//        delay_os(200);          
//        drv_esp8266_send_at("RST");//复位模块   
				g_sEsp8266Work.state=ESP8266_INIT;		
        wait_time=3;			
				break;
			}
			case ESP8266_INIT:
			{
				drv_esp8266_uart_send("ATE0\r\n");
				delay_os(200);     
        if(strlen(g_sEsp8266Work.ap_ssid)>0)
        {
          sprintf(cmd_buff, "CWSAP_DEF=\"%s\",\"%s\",5,3,4,0", g_sEsp8266Work.ap_ssid, g_sEsp8266Work.ap_passwd);
          drv_esp8266_send_at(cmd_buff);
          delay_os(1000);           
        }       
        wait_time=3;
				g_sEsp8266Work.state=ESP8266_WIFI_CONNECT;			
				break;
			}          
			case ESP8266_WIFI_CONNECT://等待WIFI连接成功
			{
        drv_esp8266_send_at("CIPSTATUS");//查询网络连接信息
				wait_time=5;
        break;
			}
			case ESP8266_NET_CFG://网络配置
			{
        printf("### ESP8266_NET_CFG\n");
        drv_esp8266_send_at("CIPMODE=0");//非透传模式
        delay_os(200);    
        
				drv_esp8266_send_at("CIPMUX=1");//使能多连接
				delay_os(200);
        if(g_sEsp8266Work.listen_port>0)
        {
          sprintf(cmd_buff, "CIPSERVER=1,%d", g_sEsp8266Work.listen_port);
          drv_esp8266_send_at(cmd_buff);	 //建立TCP服务器
          delay_os(200);             
        }       
     
        g_sEsp8266Work.state=ESP8266_STATE_OK;	
				wait_time=2;
        break;
			}      
			case ESP8266_STATE_OK:
			{
        drv_esp8266_connect_process();
        drv_esp8266_send_at("CIPSTATUS");//查询网络连接信息
				wait_time=5;
        break;
			}      
		}
		last_sec_time=drv_get_sec_counter();
	}
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_recv_process(void)
{
	u16 recv_len;
	char *pData=NULL;
	
	if(g_sEsp8266Work.pUART->iRecv>0)
	{
		recv_len=0;
		while(recv_len<g_sEsp8266Work.pUART->iRecv)
		{
			recv_len=g_sEsp8266Work.pUART->iRecv;
			delay_ms(5);
		}
		char *pBuff=(char*)g_sEsp8266Work.pUART->pBuff;
//		printf("esp8266 recv=%s\n", g_sEsp8266Work.pUART->pBuff);
		if( (pData=strstr(pBuff, "+IPD,"))!=NULL )//接收到数据
		{
      pData=pBuff;
      while((pData=strstr(pData, "+IPD,"))!=NULL)
      {
        pData+=strlen("+IPD,");
        u8 sock_id=atoi(pData);
//        printf("sock_id=%d\n", sock_id);
        if(sock_id<ESP8266_MAX_LINK_NUM && (pData=strstr(pData, ","))!=NULL )
        {
          pData+=1;
          u16 data_len=atoi(pData);
          if( (pData=strstr(pData, ":"))!=NULL )
          {
            pData+=1;
            g_sEsp8266Work.client_list[sock_id].keep_time=drv_get_sec_counter();
//            printf_hex("esp8266 recv buff= ", (u8*)pData, data_len);
            if(g_sEsp8266Work.fun_recv_parse!=NULL)
            {
              g_sEsp8266Work.fun_recv_parse(sock_id, (u8*)pData, data_len);//接收处理
            }
            pData+=data_len;
          }
        }		        
      }
	
		}		
    else if( (pData=strstr(pBuff, "WIFI GOT IP"))!=NULL )//模块连接上路由器
    {
      if(g_sEsp8266Work.state==ESP8266_WIFI_CONNECT)
      {
        g_sEsp8266Work.state=ESP8266_NET_CFG;//路由器连接成功,进行TCP/UDP连接 
      }
    }
    else if( (pData=strstr(pBuff, ",CONNECT"))!=NULL )//网络连接
    {
      pData=pBuff;
      while((pData=strstr(pData, ",CONNECT"))!=NULL)
      {
        u8 sock_id=atoi(pData-1);
        pData+=strlen(",CONNECT");
        printf("sock_id=%d connected!\n",sock_id);
        if(sock_id<ESP8266_MAX_LINK_NUM)
        {
          g_sEsp8266Work.client_list[sock_id].conn_state=1;
          g_sEsp8266Work.client_list[sock_id].keep_time=drv_get_sec_counter();
        }        
      }
    }    
    else if((pData=strstr(pBuff, ",CLOSED"))!=NULL)//网络断开
    {
      pData=pBuff;
      while((pData=strstr(pData, ",CLOSED"))!=NULL)
      {
        u8 sock_id=atoi(pData-1);
        pData+=strlen(",CLOSED");
        printf("sock_id=%d close!\n",sock_id);
        if(sock_id<ESP8266_MAX_LINK_NUM)
        {
          drv_esp8266_close(sock_id);
        }        
      }      
    } 
    else if( (pData=strstr(pBuff, "STATUS:"))!=NULL )//模块连接上路由器
    {
      pData+=strlen("STATUS:");
      u8 state=atoi(pData);
//      printf("state=%d\n", state);
      if(state==2 || state==3)
      {
        if(g_sEsp8266Work.state==ESP8266_WIFI_CONNECT)
        {
          g_sEsp8266Work.state=ESP8266_NET_CFG;//路由器连接成功,进行TCP/UDP连接 
        }
      }
      else if(state==5)//未连接AP
      {
        drv_esp8266_close(ESP8266_MAX_LINK_NUM);//网络已经断开,全部关闭
        g_sEsp8266Work.state=ESP8266_WIFI_CONNECT;//返回网络连接状态
      }
    }        
		UART_Clear(g_sEsp8266Work.pUART);
	}
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_esp8266_main(void)
{
  drv_esp8266_reg_process();
  drv_esp8266_recv_process();
}















