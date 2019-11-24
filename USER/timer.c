#include "timer.h"

void TIM3_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//开启时钟
	
	TIM_TimeBaseStruct.TIM_Period = arr - 1;//初值
	TIM_TimeBaseStruct.TIM_Prescaler = psc - 1;//预分频
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分割
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStruct);
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除更新中断标志
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//允许更新中断
	TIM_Cmd(TIM3,ENABLE);//使能定时器
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		Task_Remarks();
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}

