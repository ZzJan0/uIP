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

//һ���˳�������������ݾͻᱻ�½��յ����ݸ��ǣ���ˣ�Ҫôֱ�Ӵ����������ݣ�Ҫô���Ƶ������ط��Ժ���
void uIP_AppCall(void)
{
	/* ���ӱ������ж� */
	if (uip_aborted())
	{
		aborted();
	}
	/* ���ӳ�ʱ */
	if (uip_timedout())
	{
		timedout();
	}
	/* ���ӱ��ر� */
	if (uip_closed())
	{
		closed();
	}
	/* ���µ����ӽ��� */
	if (uip_connected())
	{
		connected();
	}
	/* ���������ݱ�Ӧ�� */
	if (uip_acked())
	{
		acked();
	}
	/* �ڵ�ǰ�������յ��µ����� */
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

