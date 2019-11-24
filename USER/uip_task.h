#ifndef _UIP_TASK_H
#define _UIP_TASK_H

#include "main.h"

typedef struct _uIP_St
{
	uint8_t Periodic_Delay;
	uint8_t Arp_Dealy;
	
}uIP_St;

extern uIP_St G_uIP_St;

void uIP_Init(void);
void uIP_Task(void);
void TCPSever_Init(void);
void TCPClient_Init(void);

#endif

