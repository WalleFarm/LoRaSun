/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

//#include <sys/types.h>

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1 
#endif

#define INVALID_SOCKET SOCKET_ERROR

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "drv_eth.h"
#include "drv_air720.h"
#include "socket.h"
#include "transport.h"


/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/ 
static int mysock = 0;

void transport_setSocket(int sock)
{
	if(sock>7)
	 return;
	mysock=sock;
}

int transport_sendPacketBuffer(unsigned char* buf, int buflen)
{
	int rc = 0;
	rc = send(mysock, buf, buflen);
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{	
	u16 ret_len=0;
	
//	if(g_sNetInfo.n_socket[mysock].len>0)//缓冲区内有数据
//	{
//		if(g_sNetInfo.n_socket[mysock].len>=count)//长度检测
//		{
//			memcpy(buf, g_sNetInfo.n_socket[mysock].buff, count);
//			g_sNetInfo.n_socket[mysock].len-=count;//计算剩余长度
//			memcpy(g_sNetInfo.n_socket[mysock].buff, &g_sNetInfo.n_socket[mysock].buff[count], g_sNetInfo.n_socket[mysock].len);//将socket数据缓冲区前移
//			ret_len=count;
//		}
//		else
//		{
//			ret_len=count=g_sNetInfo.n_socket[mysock].len;
//			memcpy(buf, g_sNetInfo.n_socket[mysock].buff, count);
//			g_sNetInfo.n_socket[mysock].len=0;//数据已经取完
//		}
//	}
	
	return ret_len;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = recv(sock, buf, count);	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
	}
	return rc;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{

return 0;
}

int transport_close(int sock)
{
	int rc;
	rc = close(sock);
	return rc;
}
