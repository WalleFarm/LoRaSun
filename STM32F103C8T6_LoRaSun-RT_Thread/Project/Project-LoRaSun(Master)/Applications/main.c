/******************************************************************************
*
* Copyright (c) 2024 艺大师
* 本项目开源文件遵循GPL-v3协议
* 
* 文章专栏地址:https://blog.csdn.net/ypp240124016/category_12834955
* 项目开源地址:https://github.com/WalleFarm/LoRaSun
* 协议栈原理专利:CN110572843A
*
* 测试套件采购地址:https://duandianwulian.taobao.com/
*
* 作者:艺大师
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
#include <rtthread.h>
#include "drv_uart.h"
#include "drv_iap.h"
 
int main(void)
{
	
	printf("\n\n****Into main****\n");
	printf("app name=%s\nversion=%s\n",APP_NAME,APP_VER);

	APP_Init();//app初始化 
	printf("\n======App Init ok!======\n\n");
	
	while(1)
	{
		drv_wdog_feed();//喂狗
		printf("app sec counter=%ds\n", drv_get_sec_counter());
		printf("app rtc counter=%ds\n", drv_get_rtc_counter());
		delay_os(5000);

	}
}
