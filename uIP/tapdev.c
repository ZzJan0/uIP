#include "uip.h"
#include "tapdev.h"
#include "uip_arp.h"
#include "enc28j60.h"

struct uip_eth_addr uip_mac;
static uint8_t ethernet_mac[6] = {0x48,0x45,0x4B,0x31,0x12,0x18};

/* µ×²ã³õÊ¼»¯ */
void tapdev_init(void)
{
	uint8_t i = 0;
	
	ENC28J60_Init(ethernet_mac);
	for(i = 0;i < 6;i++)
	{
		uip_mac.addr[i] = ethernet_mac[i];
	}
	//MACÉèÖÃ
	uip_setethaddr(uip_mac);
	
	printf("ENC28J60 MAC: ");
	printf("%02X ",ENC28J60_Read(MAADR5));
	printf("%02X ",ENC28J60_Read(MAADR4));
	printf("%02X ",ENC28J60_Read(MAADR3));
	printf("%02X ",ENC28J60_Read(MAADR2));
	printf("%02X ",ENC28J60_Read(MAADR1));
	printf("%02X ",ENC28J60_Read(MAADR0));
	printf("\r\n");
}

/* µ×²ã¶Áº¯Êý */
unsigned short int tapdev_read(void)
{
	return ENC28J60_Packet_Receive(MAX_FRAMELEN,uip_buf);
}

/* µ×²ã·¢ËÍº¯Êý */
void tapdev_send(void)
{
	ENC28J60_Packet_Send(uip_len,uip_buf);
}
