
#include "drv_server.h"
#include "drv_encrypt.h"


ServerWorkStruct g_sServerWork={0};
ServerSaveStruct g_sServerSave={0};
/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_server_read(void)
{
  CONFIG_EEPROM_READ(SERVER_EEPROM_ADDR, (u8 *)&g_sServerSave, sizeof(g_sServerSave));
//  printf_hex("read=", (u8 *)&g_sServerSave, sizeof(g_sServerSave));
  
  if(g_sServerSave.crcValue!=drv_crc16((u8*)&g_sServerSave, sizeof(g_sServerSave)-2))
  {
    g_sServerSave.app_id=APP_ID_MIN;
    g_sServerSave.gw_sn=M2M_DEV_TYPE<<16;
    drv_server_write();
    printf("server read new!\n");
  }
//  printf("read app_id=%u, gw_sn=0x%08X\n", g_sServerSave.app_id, g_sServerSave.gw_sn);
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_server_write(void)
{
  g_sServerSave.crcValue=drv_crc16((u8*)&g_sServerSave, sizeof(g_sServerSave)-2);
  CONFIG_EEPROM_WRITE(SERVER_EEPROM_ADDR, (u8 *)&g_sServerSave, sizeof(g_sServerSave));
//  printf_hex("write=", (u8 *)&g_sServerSave, sizeof(g_sServerSave));
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_server_init(void)
{
  drv_server_read();
}


/*		
================================================================================
描述 :组合发送报文
输入 : 
输出 : 组合后的数据长度
================================================================================
*/
u16 drv_server_send_msg(u8 cmd_type, u8 *in_buff, u16 in_len)
{
	static u8 pack_num=0;
	static u8 data_buff[SERVER_PACK_SIZE]={0}, make_buff[SERVER_PACK_SIZE];
	static u8 to_server_pwd[16]={0};//密码
 	static u8 encrypt_mode=ENCRYPT_MODE_TEA;//加密模式--对于设备端来讲是固定的
	static u32 gw_sn=0, app_id=0;
	u16 data_len=0,union_len,remain_len,make_len,crcValue;
	ServerHeadStruct *pHead=(ServerHeadStruct *)make_buff;
	u8 *pData=&make_buff[16];//加密区起始地址
	int out_len;
	if(in_len+32>SERVER_PACK_SIZE)
	{
		printf("in len too long!\n");
		return 0;
	}
	
	if(gw_sn==0)
	{
		gw_sn=g_sServerSave.gw_sn;
	}
	
	if(app_id==0)
	{
		app_id=g_sServerSave.app_id;
	}
	
	pack_num++;
	

	memset(data_buff, 0, SERVER_PACK_SIZE);
	data_len=0;
	union_len=in_len+4;//数据单元长度
	data_buff[data_len++]=union_len>>8;//从此处开始加密
	data_buff[data_len++]=union_len;		
	data_buff[data_len++]=pack_num;
	data_buff[data_len++]=cmd_type;
	memcpy(&data_buff[data_len], in_buff, in_len);
	data_len+=in_len;
	crcValue=drv_crc16(data_buff, union_len);//数据单元校验
	data_buff[data_len++]=crcValue>>8;
	data_buff[data_len++]=crcValue;		
	remain_len=data_len%8;
	if(remain_len>0)
		data_len+=(8-remain_len);//8字节对齐,便于TEA加密 
	
  pHead->encrypt_index=drv_get_sec_counter()%SERVER_PASSWD_CNTS;//根据时间随机获取密码
  memcpy(to_server_pwd, g_sServerWork.passwd_table[pHead->encrypt_index], 16);//根据索引复制密码
	switch(encrypt_mode)
	{
		case ENCRYPT_MODE_DISABLE:
		{
			memcpy(pData, data_buff, data_len);
			out_len=data_len;
			break;
		}
		
		case ENCRYPT_MODE_TEA:
		{
			out_len=tea_encrypt_buff(data_buff, data_len, (u32*)to_server_pwd);
			if(out_len==data_len)
			{
				memcpy(pData, data_buff, data_len);
			}			
			else
			{
				printf("server tea error!\n");
				return 0;
			}
			break;
		}
#ifdef    USE_AES    //是否启用AES算法
		case ENCRYPT_MODE_AES:
		{
			out_len=aes_encrypt_buff(data_buff, data_len, pData, SERVER_PACK_SIZE-16, to_server_pwd);//aes加密
			if(out_len<16)
			{
				printf("server aes error!\n");
				return 0;
			}
			break;
		}
#endif    
		default:
			return 0;
	}
	data_len=out_len+8;//加上app_id和gw_sn的长度
	crcValue=drv_crc16(&make_buff[8], data_len);//总校验
	pHead->head[0]=0xAA;
	pHead->head[1]=0x55;
	pHead->version=SERVER_PROTOCOL_VER;
	pHead->crc_h=crcValue>>8;
	pHead->crc_l=crcValue;
	pHead->data_len_h=data_len>>8;
	pHead->data_len_l=data_len;
	pHead->app_id[0]=app_id>>24;
	pHead->app_id[1]=app_id>>16;
	pHead->app_id[2]=app_id>>8;
	pHead->app_id[3]=app_id;
	pHead->gw_sn[0]=gw_sn>>24;
	pHead->gw_sn[1]=gw_sn>>16;
	pHead->gw_sn[2]=gw_sn>>8;
	pHead->gw_sn[3]=gw_sn;	
	
	make_len=data_len+8;
	drv_server_send_level(make_buff, make_len);//发送
	return make_len;
}

/*		
================================================================================
描述 : 转发从机消息
输入 : 
输出 : 
================================================================================
*/
void drv_server_send_slave_msg(u32 node_sn, u8 *in_buff, u16 in_len)
{
  u8 make_buff[100]={0};
	u16 make_len=0;
  if(in_len+20>sizeof(make_buff))
  {
    return;
  }
	make_buff[make_len++]=node_sn>>24;
	make_buff[make_len++]=node_sn>>16;
	make_buff[make_len++]=node_sn>>8;
	make_buff[make_len++]=node_sn;
  u8 *pUnion=&make_buff[make_len];
  make_len+=2;//单元长度  
  memcpy(&make_buff[make_len], in_buff, in_len);
  make_len+=in_len;
  u16 union_len=make_len-4;//单元长度  
	pUnion[0]=union_len>>8;
	pUnion[1]=union_len;
  u16 crcValue=drv_crc16(pUnion, union_len);
	make_buff[make_len++]=crcValue>>8;  
	make_buff[make_len++]=crcValue;  
//  printf_hex("slave msg=", make_buff, make_len);
	drv_server_send_msg(SERVER_CMD_UP_DATA, make_buff, make_len);  //转发  
}

/*		
================================================================================
描述 : 服务端数据接收解析
输入 : 
输出 : 
================================================================================
*/
u16 drv_server_recv_parse(u8 *buff, u16 len, int (*fun_send)(u8 *buff, u16 len))
{
	static u8 head[2]={0xAA, 0x55}, out_buff[800]={0}, recv_pack_num=109;
	u8 to_server_pwd[16]={0}, encrypt_mode=0;//加密模式
	u8 *pBuff=buff, *pData=NULL, pack_num, cmd_type;
	u16 data_len=0, out_len=0, union_len=0, crcValue;
	static u32 local_app_id=0, local_gw_sn=0;
	u32 recv_gw_sn, recv_app_id;
	
	if(local_gw_sn==0)
	{
		local_gw_sn=g_sServerSave.gw_sn;
	}
	
	if(local_app_id==0)
	{
		local_app_id=g_sServerSave.app_id;
	}
	
//	printf_hex("drv_server_recv_parse: ", buff, len);
	
	if( (pBuff=memstr(buff, len, head, 2))!=NULL )
	{
		ServerHeadStruct *pHead = (ServerHeadStruct *)pBuff;
		data_len=pHead->data_len_h<<8|pHead->data_len_l;
		crcValue=pHead->crc_h<<8|pHead->crc_l;
		recv_app_id=pHead->app_id[0]<<24|pHead->app_id[1]<<16|pHead->app_id[2]<<8|pHead->app_id[3];
		recv_gw_sn=pHead->gw_sn[0]<<24|pHead->gw_sn[1]<<16|pHead->gw_sn[2]<<8|pHead->gw_sn[3];
		
    if(recv_app_id==0)
    {
      return 0;
    }
		if(recv_gw_sn!=local_gw_sn)
		{
			printf("drv_server_parse_recv error: recv_gw_sn(%u)!=local_gw_sn(%u)!", recv_gw_sn, local_gw_sn);
			return 0;
		}
		if(data_len<12 || data_len+20>sizeof(out_buff))
		{
			printf("drv_server_parse_recv error: data_len<12 || data_len+20>sizeof(out_buff)\n");
			return 0;
		}
    if(pHead->encrypt_index>=SERVER_PASSWD_CNTS)
    {
      return 0;
    }
    memcpy(to_server_pwd, g_sServerWork.passwd_table[pHead->encrypt_index], 16);//根据索引复制密码
    encrypt_mode=ENCRYPT_MODE_TEA;

		pData=pBuff+8;
		if(crcValue==drv_crc16(pData, data_len))
		{
			pData+=8;//app_id和gw_sn不加密
			data_len-=8;
			//解密
			switch(encrypt_mode)
			{
				case ENCRYPT_MODE_DISABLE:
				{
					memcpy(out_buff, pData, data_len);
					out_len=data_len;
					break;
				}
				
				case ENCRYPT_MODE_TEA:
				{
					out_len=tea_decrypt_buff(pData, data_len, (u32*)to_server_pwd);
					if(out_len==data_len)
					{
						memcpy(out_buff , pData, data_len);
					}			
					else
					{
						printf("server tea error!\n");
						return 0;
					}
					break;
				}
#ifdef    USE_AES    //是否启用AES算法
        case ENCRYPT_MODE_AES:
        {
          out_len=aes_decrypt_buff(pData, data_len, out_buff, sizeof(out_buff)-16, to_server_pwd);//aes
          if(out_len<16)
          {
            printf("server aes error!\n");
            return 0;
          }
          break;
        }
#endif 
				default:
					return 0;			
			}
		}
		else
		{
			printf("drv_server_parse_recv crc error 000!\n");
			return 0;
		}
		
		pData=out_buff;
		union_len=pData[0]<<8|pData[1];
		pData+=2;
		pack_num=pData[0];
		pData+=1;
		cmd_type=pData[0];
		pData+=1;
//		printf("union_len=%d, pack_num=%d, cmd_type=%d\n", union_len, pack_num, cmd_type);
		if(recv_pack_num==pack_num)//过滤相同的包序号
		{
			printf("recv_pack_num==pack_num\n");
			return 0;
		}
		recv_pack_num=pack_num;//更新包序号
		if(union_len<4 || union_len>sizeof(out_buff))
		{
			printf("drv_server_parse_recv error: union_len<4 || union_len>sizeof(out_buff)\n");
			return 0;		
		}
		crcValue=out_buff[union_len]<<8|out_buff[union_len+1];
		if(crcValue==drv_crc16(out_buff, union_len))//解密后校验
		{
			union_len-=4;
//			printf("cmd_type=%d\n", cmd_type);
			switch(cmd_type)
			{ 
				case SERVER_CMD_DOWN_DATA://数据转发
				{
					u32 recv_node_sn=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];//目标节点序列号
          drv_slave_cmd_parse(recv_node_sn, pData, union_len);//从机解析
					break;
				}
				case REG_CMD_SET_APP_ID://设置APP ID
				{
					u32 new_app_id=pData[0]<<24|pData[1]<<16|pData[2]<<8|pData[3];//新的应用ID
          pData+=4;
          if(new_app_id>=APP_ID_MIN)
          {
            drv_server_set_app_id(new_app_id);
            delay_os(100);
            drv_system_reset();//复位系统
          }
					break;
				}        
				default://其余命令交给应用层处理
				  drv_server_cmd_parse(cmd_type, pData, union_len);
			}		
			return 1;
		}
		else
		{
			printf("drv_server_parse_recv crc error 111!\n");
			return 0;
		}		
	}
	return 0;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
int drv_server_send_level(u8 *buff, u16 len)
{
  if(g_sServerWork.fun_send != NULL)
  {
    return g_sServerWork.fun_send(buff, len);
  }  

	return 0;
}

/*		
================================================================================
描述 :
输入 : 
输出 : 
================================================================================
*/
void drv_server_send_register(int (*fun_send)(u8 *buff, u16 len))
{
  g_sServerWork.fun_send=fun_send;
}

/*		
================================================================================
描述 : 服务端解析 
输入 : 
输出 : 
================================================================================
*/
u16 drv_server_cmd_parse(u8 cmd_type, u8 *in_buff, u16 in_len)
{
	u16 make_len=0; 
	if(g_sServerWork.fun_server_cmd_parse != NULL)
	{
		make_len=g_sServerWork.fun_server_cmd_parse(cmd_type, in_buff, in_len);
	}
	 
	return make_len;
}


/*		
================================================================================
描述 : 服务端解析函数注册
输入 : 
输出 : 
================================================================================
*/
void drv_server_cmd_parse_register(u16 (*fun_server_cmd_parse)(u8 cmd_type, u8 *in_buff, u16 in_len))
{
	g_sServerWork.fun_server_cmd_parse=fun_server_cmd_parse;
}


/*		
================================================================================
描述 : 从机端解析 
输入 : 
输出 : 
================================================================================
*/
u16 drv_slave_cmd_parse(u32 node_sn, u8 *in_buff, u16 in_len)
{
	u16 make_len=0; 
	if(g_sServerWork.fun_slave_cmd_parse != NULL)
	{
		make_len=g_sServerWork.fun_slave_cmd_parse(node_sn, in_buff, in_len);
	}
	 
	return make_len;
}


/*		
================================================================================
描述 : 从机端解析函数注册
输入 : 
输出 : 
================================================================================
*/
void drv_slave_cmd_parse_register(u16 (*fun_slave_cmd_parse)(u32 node_sn, u8 *in_buff, u16 in_len))
{
	g_sServerWork.fun_slave_cmd_parse=fun_slave_cmd_parse;
}


/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_server_set_app_id(u32 app_id)
{
  g_sServerSave.app_id=app_id;
  drv_server_write();
  printf("set app_id=%u\n", app_id);
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
u32 drv_server_get_app_id(void)
{
  return g_sServerSave.app_id;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_server_set_gw_sn(u32 gw_sn)
{
  g_sServerSave.gw_sn=gw_sn;
  drv_server_write(); 
  printf("set gw_sn=%08X\n", gw_sn);
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
u32 drv_server_get_gw_sn(void)
{
  return g_sServerSave.gw_sn;
}

/*		
================================================================================
描述 : 
输入 : 
输出 : 
================================================================================
*/
void drv_server_add_passwd(u8 index, u8 *passwd)
{
  if(index<SERVER_PASSWD_CNTS)
  {
    memcpy(g_sServerWork.passwd_table[index], passwd, 16);
  }
}










