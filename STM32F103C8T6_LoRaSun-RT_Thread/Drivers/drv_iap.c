
#include "drv_iap.h"
#include "user_app.h"

IAPBootStruct g_sIapBoot={0};
IAPMangerStruct g_sIapManger={0};

iapfun jump2app; 

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void IAP_FlashWritePack(u32 *buff, u32 start_addr, u32 size)
{
	u32 i,k=0;
	for(i=0; i<size; i+=4)
	{
		if(FLASH_ProgramWord(start_addr+i, buff[k++])!=FLASH_COMPLETE)
		{
			printf("FLASH_ProgramWord error\n");
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
u8 IAP_ParsePack(u8 *buff, u16 len)
{
	static u8 head[3]={0xAA,0x77};
	u8 *pData=buff;
	u16 rema_len=len, data_len, crcValue,recv_pack_num;
	u32 flash_size,flash_page;
	u8 tea_key[17]={0},app_name_size=0;
	
	if( (pData=memstr(pData, rema_len, head, 2))!=NULL )
	{
		data_len=pData[2]<<8|pData[3];
		crcValue=pData[4]<<8|pData[5];
//		printf("data_len=%d, crcValue=0x%02X\n",data_len,crcValue);
		if(crcValue==drv_crc16(&pData[6], data_len))
		{
			u8 cmd_type=pData[7];

			switch(cmd_type)
			{				
				case IAP_CMD_INTO_BOOT://进入升级模式
					g_sIapBoot.boot_flag=IAP_FLAG;
					app_name_size=strlen(APP_NAME);
					printf("app_name_size=%d\n",app_name_size);
				
#ifndef IAP_BOOT_LOADER //应用程序内才进行以下操作				
					g_sIapBoot.app_ver=version_to_num(APP_VER);//APP版本 					
					if(app_name_size>sizeof(g_sIapBoot.app_name))
					{
						app_name_size=sizeof(g_sIapBoot.app_name);
					}
					memset(g_sIapBoot.app_name, 0, sizeof(g_sIapBoot.app_name));
					memcpy(g_sIapBoot.app_name, APP_NAME, app_name_size);//APP名称
					drv_system_reset();
#endif				
					
					IAP_BootFlagSave();	
					printf("into boot mode!\n");

					break;
				case IAP_CMD_START_FRAM://升级起始帧
									
					g_sIapManger.total_pack=pData[8]<<8|pData[9];//总包个数
					g_sIapManger.pack_num=65535;
					flash_size=(u32)g_sIapManger.total_pack*IAP_PACK_SIZE;
					printf("total_pack=%d, pack_size=%d\n",g_sIapManger.total_pack,IAP_PACK_SIZE);
					if(flash_size>MAX_BIN_SIZE)
					{ 
						printf("bin size error!\n");
						memset(&g_sIapManger, 0, sizeof(g_sIapManger));
						return IAP_CMD_ERR;
					} 
					FLASH_Unlock();//解锁
					flash_page=flash_size/FLASH_PAGE_SIZE;//需要擦除的页数
					if(flash_size%FLASH_PAGE_SIZE>0)  
						flash_page+=1;
					 
					printf("flash_page=%d\n",flash_page);
					for(int i=0; i<flash_page; i++)//擦除存放区域的所有FLASH
					{
						printf("erase page=%d, addr=0x%08X\n",i,APP_START_ADDR+FLASH_PAGE_SIZE*i);
						FLASH_ErasePage(APP_START_ADDR+(u32)(FLASH_PAGE_SIZE*i)); 
					}
					printf("FLASH_ErasePage=%d ok!\n",flash_page);
					g_sIapManger.b_update_flag=true;
					break;

				case IAP_CMD_UPDATING://升级进行包
					recv_pack_num=pData[8]<<8|pData[9];//包序号,从0开始计算
					printf("recv_pack_num=%d\n",recv_pack_num);
					if(recv_pack_num!=g_sIapManger.pack_num)//过滤相同的数据包
					{
						drv_get_rand_key(25, 13, tea_key); 
						tea_decrypt_buff(&pData[10], IAP_PACK_SIZE, (u32*)tea_key);//解密
						IAP_FlashWritePack((u32*)&pData[10], APP_START_ADDR+recv_pack_num*IAP_PACK_SIZE, IAP_PACK_SIZE);//写入FLASH	
						g_sIapManger.pack_num=recv_pack_num;				
					}					
					if(g_sIapManger.pack_num+1==g_sIapManger.total_pack)
					{
						g_sIapBoot.boot_flag=0;
						IAP_BootFlagSave();
						return IAP_CMD_UPDATE_OK;
					}
					break;		
						
				default:
					return IAP_CMD_ERR;
			}	
			
			return cmd_type;
		}
		else
		{
			printf("iap crc error!\n");
		}
	}
	else
	{
//		printf("iap no found head!\n");
	}
	return IAP_CMD_ERR;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void IAP_BootFlagSave(void)
{ 
	g_sIapBoot.crcValue=drv_crc16((u8*)&g_sIapBoot, sizeof(g_sIapBoot)-2);
	EEPROM_Write(IAP_EEPROM_ADDR, (u8*)&g_sIapBoot, sizeof(g_sIapBoot)); 
}



/*		
================================================================================
描述 : 组合回复报文
输入 : 
输出 : 
================================================================================
*/ 
u16 IAP_MakeAckMsg(u8 *make_buff)
{
	u8 *pBuff=make_buff, *pData=&make_buff[6];
	u16 len=0;
	u16 crcValue;
	
	pData[len++]=0xFF;//保留
	pData[len++]=IAP_CMD_ACK;//命令类型
	pData[len++]=g_sIapManger.pack_num>>8;
	pData[len++]=g_sIapManger.pack_num;
	
	crcValue=drv_crc16(pData, len);
	pBuff[0]=0xAA;    
	pBuff[1]=0x77;
	pBuff[2]=len>>8;
	pBuff[3]=len;
	pBuff[4]=crcValue>>8;
	pBuff[5]=crcValue;

	return len+6;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
u8 IAP_LoadApp(u32 appxaddr)
{
	u32 stack_addr=*(vu32*)(appxaddr);//栈顶地址

	if( stack_addr>=0x20000000 && stack_addr<0x20000000+RAM_SIZE ) //与 值根据RAM空间大小计算
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字节为程序开始地址
		__set_MSP(*(vu32*)(appxaddr));					//初始化APP堆栈指针，用户区的第一个字用于存放栈顶地址
    
		jump2app();									//跳转到app
		return IAP_ERR_JMP;
	}
	else if( stack_addr<0x20000000 || stack_addr>=0x20000000+RAM_SIZE )//栈顶地址不对
	  return IAP_ERR_STACK;
  else
    return IAP_ERR_UNKNOW;	
}		 

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
void IAP_JmpFlagCheck(void)
{

	EEPROM_Read(IAP_EEPROM_ADDR, (u8*)&g_sIapBoot, sizeof(g_sIapBoot)); 
	
	if(g_sIapBoot.crcValue==drv_crc16((u8*)&g_sIapBoot, sizeof(g_sIapBoot)-2))
	{
		if(g_sIapBoot.boot_flag<IAP_FLAG)//APP中会把这个标志清零,如果多次没有清零说明APP有问题,则进入升级模式
		{
			g_sIapBoot.boot_flag++;//标志自增,在应用程序内置零
			IAP_BootFlagSave();
			g_sIapManger.jum_result=IAP_LoadApp(APP_START_ADDR);//跳转		
		}		
		else
		{
			g_sIapManger.jum_result=IAP_ERR_FLAG;
		}
	}
	else
	{
		g_sIapManger.jum_result=IAP_ERR_CRC;	
		strcpy((char*)g_sIapBoot.app_name, "app_boot");
		g_sIapBoot.app_ver=10;
		g_sIapBoot.boot_flag=IAP_FLAG;
		IAP_BootFlagSave();
	}
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
u8 version_to_num(const char *ver_str)
{
	u8 i,ver_num=0;
	for(i=0;i<4;i++)
	{
		if(ver_str[i]>='0' && ver_str[i]<='9')
		{
			ver_num=ver_num*10+ver_str[i]-'0';
		}
		else if(ver_str[i]=='.')
		{
		
		}
		else
		{
			break;
		}
	}
	return ver_num;
}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/ 
u8 get_app_version(void)
{
	return version_to_num(APP_VER);
}

















