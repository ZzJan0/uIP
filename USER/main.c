#include "main.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_Init();
	LED_Init();
	USART1_Init(115200);
	TIM3_Init(1000,72);//¶¨Ê±0.001s
	uIP_Init();
	
	TCPSever_Init();
	
	TCPClient_Init();
	
	while(1)
	{
		Task_Process();
		
		LED_Task(5);
		
		uIP_Task();
	}
}
