#ifndef	_LED_H
#define _LED_H

#include "main.h"

#define LED3 	PDout(3)
#define LED2 	PDout(6)
#define LED1	PBout(5)

typedef struct _Led_St
{
	uint8_t LED_Delay;
	
}Led_St;

extern Led_St G_Led_St;

void LED_Init(void);
void LED_Task(uint8_t ms);

#endif
