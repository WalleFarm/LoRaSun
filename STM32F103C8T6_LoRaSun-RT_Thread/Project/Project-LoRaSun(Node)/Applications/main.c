 
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
