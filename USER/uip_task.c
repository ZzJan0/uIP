#include "uip_task.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
	
uIP_St G_uIP_St = 
{
	.Periodic_Delay = 50,//500ms
	.Arp_Dealy = 10,//1s
};
	
void uIP_Init(void)
{
	uip_ipaddr_t ipaddr;
	
	tapdev_init();//������ʼ��
	
	uip_arp_init();//ARP��ʼ��
	uip_init();//uIPЭ��ջ��ʼ��
	
	//IP����
	uip_ipaddr(ipaddr, 192,168,199,100);
	uip_sethostaddr(ipaddr);
	//��������
	uip_ipaddr(ipaddr, 192,168,199,1);
	uip_setdraddr(ipaddr);
	//����������
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);
}

//10msʱ��
void uIP_Periodic_Task(uint8_t ms)
{
	if(G_uIP_St.Periodic_Delay > 0)
		return;
	
	/* ��ѯ����������UDP����*/
	for(uint8_t i = 0; i < UIP_CONNS; i++) 
	{
		uip_periodic(i);//���ڴ���uIP�ڲ��¼���
		if(uip_len > 0) 
		{
			uip_arp_out();
			tapdev_send();
		}
	}
	
#if UIP_UDP
	/* ��ѯ����������UDP����*/
	for(uint8_t i = 0; i < UIP_UDP_CONNS; i++)
	{
		uip_udp_periodic(i);
		if(uip_len > 0)
		{
			uip_arp_out();
			tapdev_send();
		}
	}
#endif
	
	G_uIP_St.Periodic_Delay = ms;
}
//100msʱ��
void uIP_Arp_Task(uint8_t ms)
{
	if(G_uIP_St.Arp_Dealy > 0)
		return;
	
	uip_arp_timer();//��ʱ������ڵı���
	
	G_uIP_St.Arp_Dealy = ms;
}

void uIP_Task(void)
{
	//��ѯ�Ƿ���յ�����
	uip_len = tapdev_read();
	if(uip_len > 0) 
	{
		//IP�������ݰ�
		if(BUF->type == htons(UIP_ETHTYPE_IP)) 
		{
			//ȥ����̫��ͷ�ṹ������ARP��
			uip_arp_ipin();
			//����IP���ݰ�
			uip_input();
			//��uip_input����ʱ��uip_len > 0����������Ҫ����
			if(uip_len > 0) 
			{
				//����̫��ͷ�ṹ
				uip_arp_out();
				//�������ݵ���̫����
				tapdev_send();
			}
		}
		//ARP���ݰ�
		else if(BUF->type == htons(UIP_ETHTYPE_ARP)) 
		{
			//�����ARP��Ӧ�͸���ARP�����������͹����Ӧ����
			uip_arp_arpin();
			//��ARP����Ҫ������Ӧ
			if(uip_len > 0) 
			{
				//�������ݵ���̫����
				tapdev_send();
			}
		}
	}
	//500ms
	uIP_Periodic_Task(50);
	//1s
	uIP_Arp_Task(10);
}

void TCPSever_Init(void)
{
	uip_listen(HTONS(6000));
}

void TCPClient_Init(void)
{
	uip_ipaddr_t ipaddr;
	
	uip_ipaddr(ipaddr,192,168,199,151);
	uip_connect(&ipaddr,HTONS(80));
}
