
#include "drv_air780e.h"

Air780WorkStruct g_sAir780Work={0};


/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_air780_init(UART_Struct *pUART)
{
  g_sAir780Work.pUART=pUART;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_air780_set_client(u8 index, char *dst_addr, u16 port, char *type)
{
  if(index<AIR780_MAX_LINK_NUM)
  {
    Air780ClientStruct *pClient=&g_sAir780Work.client_list[index];
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
void drv_air780_fun_register(void (*fun_recv_parse)(u8 chn, u8 *buff, u16 len))
{
	g_sAir780Work.fun_recv_parse=fun_recv_parse; 
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_air780_uart_send(char *send_buff)
{
//	printf("***AT CMD:  %s", send_buff);
	if(g_sAir780Work.pUART!=NULL)	
	{
		UART_Send(g_sAir780Work.pUART->PortNum, (u8*)send_buff, strlen(send_buff));
	}
}
 
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_air780_send_at(char *cmd_buff)
{
	static char send_buff[100]={0};
	if(strlen(cmd_buff)+10<sizeof(send_buff))
	{
		memset(send_buff, 0, sizeof(send_buff));
		sprintf(send_buff, "AT+%s\r\n", cmd_buff);
		drv_air780_uart_send(send_buff);	
	}
}

/*		
================================================================================
描述 : 数据发送
输入 : 
输出 : 
================================================================================
*/
int drv_air780_send_data(u8 sock_id, u8 *buff, u16 len)
{
	char cmd_buff[30]={0};
  if(sock_id<AIR780_MAX_LINK_NUM)
  {
    if(g_sAir780Work.client_list[sock_id].conn_state>0)
    {
      sprintf(cmd_buff, "AT+CIPSEND=%u,%u\r\n", sock_id,len);
      drv_air780_uart_send(cmd_buff);
      if(g_sAir780Work.pUART!=NULL)	
      {
        UART_Send(g_sAir780Work.pUART->PortNum, buff, len);
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
void drv_air780_client_connect(u8 sock_id, char * type, char *dst_addr, u16 dst_port)
{
  static char cmd_buff[100]={0};  
  
  sprintf(cmd_buff, "CIPSTART=%d,\"%s\",\"%s\",%d",sock_id, type, dst_addr, dst_port);
  drv_air780_send_at(cmd_buff);
//  printf("connect=%s\n", cmd_buff);
}

/*		
================================================================================
描述 : 客户端断开
输入 : 
输出 : 
================================================================================
*/
void drv_air780_close(u8 sock_id)
{
  if(sock_id>AIR780_MAX_LINK_NUM)
    return;
  char cmd_buff[30]={0};  
  sprintf(cmd_buff, "CIPCLOSE=%d", sock_id);
  drv_air780_send_at(cmd_buff);
  if(sock_id==AIR780_MAX_LINK_NUM)//全部关闭
  {
    for(u8 i=0; i<AIR780_MAX_LINK_NUM; i++)
    {
      g_sAir780Work.client_list[i].conn_state=0;
    }
  }
  else
  {
    g_sAir780Work.client_list[sock_id].conn_state=0;
  }
}


/*		
================================================================================
描述 : 客户端连接管理任务
输入 : 
输出 : 
================================================================================
*/
void drv_air780_connect_process(void)
{
  u32 now_sec_time=drv_get_sec_counter();
  for(u8 i=0; i<AIR780_MAX_LINK_NUM; i++)
  {
    Air780ClientStruct *pClient=&g_sAir780Work.client_list[i];
    if(pClient->dst_port>0)//有连接需求
    {
      if(pClient->conn_state==0)
      {
        drv_air780_client_connect(pClient->sock_id, pClient->type, pClient->dst_addr, pClient->dst_port);     
      }
      else
      {
        int det_time=now_sec_time-pClient->keep_time;
        if(det_time>pClient->heart_time)//心跳超时
        {
          printf("sock_id=%d, heart time out!\n", i);
          drv_air780_close(pClient->sock_id);
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
void drv_air780_reg_process(void)
{
	static u32 last_sec_time=0, wait_time=2;

	u32 now_sec_time=drv_get_sec_counter();
	if(now_sec_time-last_sec_time>wait_time)
	{
		switch(g_sAir780Work.state)
		{
			case AIR780_STATE_START:
			{
				drv_air780_uart_send("ATE0\r\n");
				delay_os(200);         
        

        drv_air780_send_at("RESET");//复位模块        
				g_sAir780Work.state=AIR780_STAT_INIT;		
        wait_time=5;			
				break;
			}
			case AIR780_STAT_INIT:
			{
				drv_air780_uart_send("ATE0\r\n");
				delay_os(200);     
        drv_air780_send_at("CSQ");//查询信号强度
				delay_os(200);     
        drv_air780_send_at("CGNSPWR=1");//打开GPS       
				delay_os(200);     
        drv_air780_send_at("CGNSAID=31,1,1,1");//使能位置辅助定位           
        g_sAir780Work.state=AIR780_STAT_CGREG;
				break;
			}          
			case AIR780_STAT_CGREG://查询网络注册状态
			{
        drv_air780_send_at("CGREG?");//查询网络连接信息
				wait_time=3;
        break;
			}
			case AIR780_STAT_CGACT://查询PDP状态
			{
        drv_air780_send_at("CGACT?");//查询网络连接信息
				wait_time=3;
        break;
			}      
			case AIR780_STAT_CGATT0://查询附着状态
			{
        drv_air780_send_at("CGATT?");//查询网络连接信息
				wait_time=3;
        break;
			}   
			case AIR780_STAT_CGATT1://已附着
			{
        drv_air780_send_at("CIPMUX=1");//多路连接
        delay_os(200);
        drv_air780_send_at("CIPQSEND=0");//快发模式
        delay_os(200);
        drv_air780_send_at("CSTT");//设置APN      =\"cmnet\",\"\",\"\"     
				wait_time=2;
        g_sAir780Work.state=AIR780_STAT_CIICR;
        break;
			}    
			case AIR780_STAT_CIICR:
			{
        drv_air780_send_at("CIICR");////激活移动场景,获取IP地址
        delay_os(1000);
        drv_air780_send_at("CIFSR");//查询IP地址           
        g_sAir780Work.state=AIR780_STAT_OK;
				wait_time=3;
        break;
			}   
			case AIR780_STAT_OK://入网成功
			{
        static u8 counts=0;
        if(counts++%2==0)
        {
          if(g_sAir780Work.imei_buff[0]==0)
          {
            drv_air780_send_at("CGSN");
          }
          else if(g_sAir780Work.iccid_buff[0]==0)
          {
            drv_air780_send_at("ICCID");
          }
          else
          {
            static u8 flag=0;
            if(flag++%2==0)
            {
              drv_air780_send_at("CSQ");//查询信号强度
            }
            else
            {
              drv_air780_send_at("CGNSINF");//查询GPS信息
            }
          }
          
        }
        else
        {
          drv_air780_connect_process();
        }
				wait_time=3;
        break;
			}         
		}
		last_sec_time=drv_get_sec_counter();
	}
}



/*		
================================================================================
描述 : 接收处理
输入 : 
输出 : 
================================================================================
*/
void drv_air780_recv_process(void)
{
	u16 recv_len;
	char *pData=NULL;
	
	if(g_sAir780Work.pUART->iRecv>0)
	{
		recv_len=0;
		while(recv_len<g_sAir780Work.pUART->iRecv)
		{
			recv_len=g_sAir780Work.pUART->iRecv;
			delay_ms(5);
		}
		char *pBuff=(char*)g_sAir780Work.pUART->pBuff;
		printf("air780 recv=%s\n", g_sAir780Work.pUART->pBuff);
		if( (pData=strstr(pBuff, "+RECEIVE,"))!=NULL )//接收到数据
		{
      pData=pBuff;
      while((pData=strstr(pData, "+RECEIVE,"))!=NULL)
      {
        pData+=strlen("+RECEIVE,");
        u8 sock_id=atoi(pData);
//        printf("sock_id=%d\n", sock_id);
        if(sock_id<AIR780_MAX_LINK_NUM && (pData=strstr(pData, ","))!=NULL )
        {
          pData+=1;
          u16 data_len=atoi(pData);
          if( (pData=strstr(pData, ":"))!=NULL )
          {
            pData+=3;//: 0D 0A
            g_sAir780Work.client_list[sock_id].keep_time=drv_get_sec_counter();
            printf_hex("air780 data buff= ", (u8*)pData, data_len);
            if(g_sAir780Work.fun_recv_parse!=NULL)
            {
              g_sAir780Work.fun_recv_parse(sock_id, (u8*)pData, data_len);//接收处理
            }
            pData+=data_len;
          }
        }		        
      }
	
		}		
    else if( (pData=strstr(pBuff, "+CSQ: "))!=NULL )//信号强度
    {
      pData+=strlen("+CSQ: ");
      g_sAir780Work.rssi=atoi(pData);
      printf("***rssi=%d\n", g_sAir780Work.rssi);
    }
    else if( (pData=strstr(pBuff, "+CGNSINF:"))!=NULL )//定位信息
    {
      pData+=strlen("+CGNSINF:");
//      printf("GPS=%s", pBuff);
      for(u8 i=0; i<5; i++)
      {
        if( (pData=strstr(pData, ","))!=NULL )
        {
          pData+=1;
          switch(i)
          {
            case 2:
            {
              g_sAir780Work.deg_sn=atof(pData);//纬度
              break;
            }
            case 3:
            {
              g_sAir780Work.deg_we=atof(pData);//经度
              break;
            }    
            case 4:
            {
              g_sAir780Work.hight=atof(pData);//海拔
              break;
            }            
          } 
        }
        else
        {
          break;
        }
      }
      printf("GPS: WE=%.3f deg, SN=%.3f deg, H=%.3f m\n", g_sAir780Work.deg_we, g_sAir780Work.deg_sn, g_sAir780Work.hight);
    }    
		else if((pData=strstr((char*)pBuff, "+CGEV: "))!=NULL)//PDP变化
		{
			pData+=strlen("+CGEV: ");
			if(strstr(pData, "DEACT")!=NULL)//移动场景去活
			{
						
        drv_air780_send_at("CIPSHUT");//去激活
				delay_os(100);
				g_sAir780Work.state=AIR780_STATE_START;
				printf("===RDY net_state=AIR780_STATE_START\n");			
			}
			else if(strstr(pData, "ACT")!=NULL)//移动场景激活
			{
				if(g_sAir780Work.state==AIR780_STAT_CIICR)
				{
					g_sAir780Work.state=AIR780_STAT_OK;
				}
			}						
		}	    
		else if((pData=strstr((char*)pBuff, "+CGREG:"))!=NULL)//注册查询
		{
			pData+=strlen("+CGREG:");
			if((pData=strstr((char*)pBuff, ",1"))!=NULL || (pData=strstr((char*)pBuff, ",5"))!=NULL)//注册成功
			{
				g_sAir780Work.state=AIR780_STAT_CGACT;
				printf("=== +CGREG:1\n");
			}
		}			
		else if((pData=strstr((char*)pBuff, "+CGACT: "))!=NULL && (pData=strstr((char*)pBuff, ",1"))!=NULL)//PDP查询
		{
			g_sAir780Work.state=AIR780_STAT_CGATT0;
			printf("=== AIR780_STAT_CGATT0\n");
		} 
		else if((pData=strstr((char*)pBuff, "+CGATT: 1"))!=NULL)//附着查询
		{
			g_sAir780Work.state=AIR780_STAT_CGATT1;
			printf("=== +CGATT: 1\n");
		}	    
		else if((pData=strstr((char*)pBuff, ", CONNECT OK"))!=NULL)//连接检测  || (pData=strstr((char*)pBuff, ", ALREADY CONNECT"))!=NULL
		{
			u8 sock_id=0;
			pData--;
			printf("### pData=%s\n", pData);
			if(pData[0]>='0' && pData[0]<='9')
			{
				sock_id=pData[0]-'0';
				printf("=== air780 sock_id=%d connect ok!\n", sock_id);
				if(sock_id<AIR780_MAX_LINK_NUM)
				{
					g_sAir780Work.client_list[sock_id].conn_state=true;
					g_sAir780Work.client_list[sock_id].keep_time=drv_get_sec_counter();
				}
			}
			
		}		
		else if((pData=strstr((char*)pBuff, ",CLOSE OK"))!=NULL)//关闭
		{
			u8 sock_id=0;
			pData--;
			if(pData[0]>='0' && pData[0]<='9')
			{
				sock_id=pData[0]-'0';
				printf("=== sock_id=%d close ok!\n", sock_id);
				if(sock_id<AIR780_MAX_LINK_NUM)
				{
					g_sAir780Work.client_list[sock_id].conn_state=false;
				}
			}
		}		
		else if((pData=strstr((char*)pBuff, "+PDP: DEACT"))!=NULL)//PDP失效
		{		
      drv_air780_send_at("CIPSHUT");//去激活
			delay_os(100);
			g_sAir780Work.state=AIR780_STATE_START;
			printf("===RDY net_state=AIR780_STATE_START\n");			
		}	 
		else if(pBuff[2]>='0' && pBuff[2]<='9' && g_sAir780Work.imei_buff[0]==0)//IMEI检测
		{
			bool is_number=true;
			pBuff+=2;
			for(u8 i=0; i<15; i++)
			{
				if(pBuff[i]<'0' || pBuff[i]>'9')
				{
					is_number=false;
					break;
				}
			}
			if(is_number==true)
			{
				memset(g_sAir780Work.imei_buff, 0, sizeof(g_sAir780Work.imei_buff));
				memcpy(g_sAir780Work.imei_buff, pBuff, 15);
				printf("imei=%s\n", g_sAir780Work.imei_buff);
			}
		}
		else if((pData=strstr((char*)pBuff, "+ICCID: "))!=NULL)//ICCID检测
		{
			pData+=strlen("+ICCID: ");
			memset(g_sAir780Work.iccid_buff, 0, sizeof(g_sAir780Work.iccid_buff));
			memcpy(g_sAir780Work.iccid_buff, pData, 20);
			printf("iccid=%s\n", g_sAir780Work.iccid_buff);
		}    
		UART_Clear(g_sAir780Work.pUART);
	}
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_air780_main(void)
{
  drv_air780_reg_process();
  drv_air780_recv_process();
}




