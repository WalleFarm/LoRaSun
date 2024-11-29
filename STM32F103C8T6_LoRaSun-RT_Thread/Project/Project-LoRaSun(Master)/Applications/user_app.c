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
#include "app_master.h" 
#include "app_mqtt.h" 



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
		
	
	tid = rt_thread_create("master", app_master_thread_entry, RT_NULL,  
												 1200, RT_THREAD_PRIORITY_MAX - 15, 20);
	RT_ASSERT(tid != RT_NULL);	
	rt_thread_startup(tid);	
  
	tid = rt_thread_create("mqtt", app_mqtt_thread_entry, RT_NULL,  
												 1200, RT_THREAD_PRIORITY_MAX - 15, 20);
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
			else if((pData=strstr(pBuff, "app_id:"))!=NULL)
			{
				pData+=strlen("app id:");
        u32 app_id=atol(pData); 
        drv_server_set_app_id(app_id);//设置应用ID
        drv_system_reset();
			}  
			else if((pData=strstr(pBuff, "gw_sn:"))!=NULL)
			{
        u32 gw_sn=0; 
				pData+=strlen("gw_sn:");
        sscanf(pData, "%08X", &gw_sn);
        drv_server_set_gw_sn(gw_sn);//设置网关SN
        drv_system_reset();
			}          
			else if((pData=strstr(pBuff, "down:"))!=NULL)
			{
				pData+=strlen("down:");
        u16 len=strlen(pData);
        nwk_master_add_down_pack(0x12345678, (u8*)pData, len);//发送测试
			}
 			else if((pData=strstr(pBuff, "time:"))!=NULL)
			{
				pData+=strlen("time:");
        u32 time=atol(pData);
        printf("set time=%us\n", time);
        nwk_set_rtc_counter(time);
			}     
 			else if((pData=strstr(pBuff, "offset:"))!=NULL)
			{
				pData+=strlen("offset:");
        u16 offset=atol(pData);
        app_master_set_offset(offset);
			}        
 			else if((pData=strstr(pBuff, "broad:"))!=NULL)
			{
        pData+=strlen("broad:");
        u8 slave_addr=atoi(pData);
				nwk_master_send_broad(slave_addr);
			}        
      else if((pData=strstr(pBuff, "mode:"))!=NULL)
      {
        pData+=strlen("mode:");
        u8 freq_ptr=atoi(pData);
        if(freq_ptr<10)
          pData+=2;
        else
          pData+=3;
        u8 run_mode=atoi(pData);
        printf("set freq_ptr=%d, run_mode=%d\n", freq_ptr, run_mode);
        
        nwk_master_set_config(freq_ptr, run_mode);//设置配置信息       
      }
      else if((pData=strstr(pBuff, "wifi:"))!=NULL)
      {
        pData+=strlen("wifi:");
        char *pData2=strstr(pData, " ");
        pData2[0]='\0';
        pData2+=1;
        app_esp8266_set_ssid_pwd(pData, pData2);
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











