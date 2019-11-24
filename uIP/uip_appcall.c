#include "uip_appcall.h"
#include "uip.h"
#include <stdio.h>
#include <string.h>


static void aborted(void)
{
	printf("Connected aborted\r\n");
}

static void timedout(void)
{
	printf("Connected timeout\r\n");
}

static void closed(void)
{
	printf("Closed IP: %d.%d.%d.%d:%d\r\n",
			(uip_conn->ripaddr[0] & 0xFF),
			(uip_conn->ripaddr[0] >> 8),
			(uip_conn->ripaddr[1] & 0xFF),
			(uip_conn->ripaddr[1] >> 8),HTONS(uip_conn->rport));
}

static void connected(void)
{
	printf("Connected IP: %d.%d.%d.%d:%d\r\n",
			(uip_conn->ripaddr[0] & 0xFF),
			(uip_conn->ripaddr[0] >> 8),
			(uip_conn->ripaddr[1] & 0xFF),
			(uip_conn->ripaddr[1] >> 8),HTONS(uip_conn->rport));
}

static void newdata(void)
{
	struct tcp_appstate *p = &uip_conn->appstate;
	
	((char *)uip_appdata)[uip_len] = 0;
	printf("%s",(char *)uip_appdata);
	if(strstr((char *)uip_appdata,"Hello uIP"))
	{
		p->ptxdat = (uint8_t *)"Hello PC\r\n";
		p->txlen = strlen((char *)p->ptxdat);
	}
}

static void acked(void)
{
	//printf("uIP Send Successfully\r\n");
}

static void senddata(void)
{
	struct tcp_appstate *p = &uip_conn->appstate;
	
	if(p->txlen > 0)
	{
		uip_send(p->ptxdat,p->txlen);
		p->txlen = 0;
	}
}

//一旦退出这个函数，数据就会被新接收的数据覆盖，因此，要么直接处理到来的数据，要么复制到其它地方稍后处理
void uIP_AppCall(void)
{
	/* 连接被意外中断 */
	if (uip_aborted())
	{
		aborted();
	}
	/* 连接超时 */
	if (uip_timedout())
	{
		timedout();
	}
	/* 连接被关闭 */
	if (uip_closed())
	{
		closed();
	}
	/* 有新的连接建立 */
	if (uip_connected())
	{
		connected();
	}
	/* 发出的数据被应答 */
	if (uip_acked())
	{
		acked();
	}
	/* 在当前连接上收到新的数据 */
	if (uip_newdata())
	{
		newdata();
	}

	if (uip_rexmit() ||	uip_newdata() || uip_acked() ||	uip_connected() || uip_poll())
	{
		senddata();
	}
}

void uip_log(char *msg)
{
	printf("%s\r\n",msg);
}

