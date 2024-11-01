
#include "user_app.h"
#include "app_node.h" 


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
			else if((pData=strstr(pBuff, "gw:"))!=NULL)
			{
				pData+=3;
				nwk_node_send2gateway((u8*)pData, strlen(pData));
			}
      else if((pData=strstr(pBuff, "join"))!=NULL)
			{
				nwk_node_req_join(0xC1011234);//请求入网
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











