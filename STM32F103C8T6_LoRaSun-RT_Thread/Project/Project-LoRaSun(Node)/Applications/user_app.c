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
      else if((pData=strstr(pBuff, "node_sn:"))!=NULL)
      {
        pData+=strlen("node_sn:");
        u32 node_sn=0;
        sscanf((char*)pData, "%08X", &node_sn);
        app_node_set_sn(node_sn);
      }
      else if((pData=strstr(pBuff, "d2d:"))!=NULL)
      {
        pData+=strlen("d2d:");
        u32 dst_sn=0;
        sscanf((char*)pData, "%08X", &dst_sn);
        pData+=8;
        char *pData2=strstr(pData, " ");
        pData2[0]='\0';
        pData2+=1;
        u16 period=atol(pData2);
        pData2=strstr(pData2, " ");
        pData2[0]='\0';
        pData2+=1;  
        u8 data_len=strlen(pData2);
        nwk_node_send2device(dst_sn, period, (u8*)pData2, data_len);
      }      
      else if((pData=strstr(pBuff, "wake:"))!=NULL)
      {
        pData+=strlen("wake:");
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











