#include "led.h"

Led_St G_Led_St = {
	
	.LED_Delay = 5,
};

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;			    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
	LED1 = 1;
	LED2 = 1;
	LED3 = 1;	
}

void LED_Task(uint8_t ms)
{
	if(G_Led_St.LED_Delay > 0)
		return;
	
	LED1 = !LED1;
	
	G_Led_St.LED_Delay = ms;
}
