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
	
	tapdev_init();//网卡初始化
	
	uip_arp_init();//ARP初始化
	uip_init();//uIP协议栈初始化
	
	//IP设置
	uip_ipaddr(ipaddr, 192,168,199,100);
	uip_sethostaddr(ipaddr);
	//网关设置
	uip_ipaddr(ipaddr, 192,168,199,1);
	uip_setdraddr(ipaddr);
	//子掩码设置
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);
}

//10ms时基
void uIP_Periodic_Task(uint8_t ms)
{
	if(G_uIP_St.Periodic_Delay > 0)
		return;
	
	/* 查询并处理所有UDP连接*/
	for(uint8_t i = 0; i < UIP_CONNS; i++) 
	{
		uip_periodic(i);//周期处理uIP内部事件，
		if(uip_len > 0) 
		{
			uip_arp_out();
			tapdev_send();
		}
	}
	
#if UIP_UDP
	/* 查询并处理所有UDP连接*/
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
//100ms时基
void uIP_Arp_Task(uint8_t ms)
{
	if(G_uIP_St.Arp_Dealy > 0)
		return;
	
	uip_arp_timer();//定时清除过期的表项
	
	G_uIP_St.Arp_Dealy = ms;
}

void uIP_Task(void)
{
	//查询是否接收到数据
	uip_len = tapdev_read();
	if(uip_len > 0) 
	{
		//IP数据数据包
		if(BUF->type == htons(UIP_ETHTYPE_IP)) 
		{
			//去除以太网头结构，跟新ARP表
			uip_arp_ipin();
			//处理IP数据包
			uip_input();
			//当uip_input返回时，uip_len > 0表明有数据要发送
			if(uip_len > 0) 
			{
				//加以太网头结构
				uip_arp_out();
				//发送数据到以太网中
				tapdev_send();
			}
		}
		//ARP数据包
		else if(BUF->type == htons(UIP_ETHTYPE_ARP)) 
		{
			//如果是ARP回应就更新ARP表，如果是请求就构造回应数据
			uip_arp_arpin();
			//是ARP请求，要发送响应
			if(uip_len > 0) 
			{
				//发送数据到以太网中
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
