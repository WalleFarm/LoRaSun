
#include "user_app.h"
#include "app_node.h" 
#include "app_oled96.h"

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void APP_Init(void)
{
	rt_thread_t tid=RT_NULL;	
	
	tid=RT_NULL;
	tid = rt_thread_create("uart", app_uart_thread_entry, RT_NULL,  
												 512, RT_THREAD_PRIORITY_MAX - 15, 20);
	RT_ASSERT(tid != RT_NULL);	
	rt_thread_startup(tid);		
		
	
	tid = rt_thread_create("node", app_node_thread_entry, RT_NULL,  
												 1024, RT_THREAD_PRIORITY_MAX - 15, 20);
	RT_ASSERT(tid != RT_NULL);	
	rt_thread_startup(tid);	
  
	tid = rt_thread_create("oled", app_oled96_thread_entry, RT_NULL,  
												 1024, RT_THREAD_PRIORITY_MAX - 12, 20);
	RT_ASSERT(tid != RT_NULL);	
	rt_thread_startup(tid);	  
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
u16 app_uart_send(u8 *buff, u16 len)
{
	UART2_Send(buff, len);
	return len;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void app_uart_thread_entry(void *parameter) 
{
	static UART_Struct *pUART=&g_sUART1; 
	u16 recv_len;
	char *pData=NULL;
	
	while(1)
	{
		if(pUART->iRecv>0)
		{
			recv_len=0;
			while(recv_len<pUART->iRecv)
			{
				recv_len=pUART->iRecv;
				delay_ms(5);
			}
      char *pBuff=(char*)pUART->pBuff;
			printf("debug recv=%s\n",pBuff);
      
      if(strstr(pBuff, "reset")!=NULL)
      {
        printf("*** reset!\n");
        drv_system_reset();
      }
			else if((pData=strstr(pBuff, "up:"))!=NULL)
			{
				pData+=3;
				nwk_node_send2gateway((u8*)pData, strlen(pData));
			}
      else if((pData=strstr(pBuff, "join"))!=NULL)
			{
				nwk_node_req_join(0xC1011234);//请求入网
			}
      else if((pData=strstr(pBuff, "sn:"))!=NULL)
      {
        pData+=3;
        u32 node_sn=atol(pData);
        app_node_set_sn(node_sn);
      }
      else if((pData=strstr(pBuff, "wake:"))!=NULL)
      {
        pData+=5;
        u16 wake_period=atol(pData);
        app_node_set_wake_period(wake_period);
      }		
      else if((pData=strstr(pBuff, "sniff:"))!=NULL)
      {
        pData+=strlen("sniff:");
        u8 group_id=atol(pData);
        pData=strstr(pData, ",");
        pData+=1;
        u16 cnts=atol(pData);
        
        u8 sf=0, bw=0;
        nwk_get_up_channel(group_id, &sf, &bw);
        printf("test sniff sf=%d, bw=%d, cnts=%d\n", sf, bw, cnts);
        u32 freq=NWK_GW_BASE_FREQ+0*1000000;
        nwk_node_set_lora_param(freq, sf, bw);
        for(u16 i=0; i<cnts; i++)
        {
          nwk_node_send_sniff(sf, bw);//发送嗅探帧
          nwk_node_cad_init(); 
        }
        printf("sniff ok!\n");
      }			      
			UART_Clear(pUART);
		}		

		delay_os(100);
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











