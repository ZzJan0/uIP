#include "tasks.h"

Task_St G_Task_St[] = 
{
	{1,0,1,10,10,Time_Base_10ms},
	{1,0,1,100,100,Time_Base_100ms},
	{1,0,1,1000,1000,Time_Base_1s},
};

void Task_Remarks(void)
{
	uint8_t i;
	
	for(i = 0;i < TASK_MAX;i++)
	{
		if(G_Task_St[i].En == 1)
		{
			if(G_Task_St[i].Timer)
			{
				G_Task_St[i].Timer--;
				if(G_Task_St[i].Timer == 0)
				{
					G_Task_St[i].Run = 1;
					if(G_Task_St[i].Load == 1)
					{
						G_Task_St[i].Timer = G_Task_St[i].ltvTime;
					}
				}
			}
		}
	}
}

void Task_Process(void)
{
	uint8_t i;
	
	for(i = 0;i < TASK_MAX;i++)
	{
		if(G_Task_St[i].Run == 1)
		{
			G_Task_St[i].Task_Hook();
			G_Task_St[i].Run = 0;
		}
	}
}
//10ms时基
void Time_Base_10ms(void)
{
	if(G_uIP_St.Periodic_Delay > 0)
		G_uIP_St.Periodic_Delay--;
}
//100ms时基
void Time_Base_100ms(void)
{
	if(G_Led_St.LED_Delay > 0)
		G_Led_St.LED_Delay--;
	
	if(G_uIP_St.Arp_Dealy > 0)
		G_uIP_St.Arp_Dealy--;
}

//1s时基
void Time_Base_1s(void)
{
	
}

