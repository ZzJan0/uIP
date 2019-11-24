#include "timer.h"

void TIM3_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//����ʱ��
	
	TIM_TimeBaseStruct.TIM_Period = arr - 1;//��ֵ
	TIM_TimeBaseStruct.TIM_Prescaler = psc - 1;//Ԥ��Ƶ
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷָ�
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStruct);
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//��������жϱ�־
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//��������ж�
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܶ�ʱ��
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		Task_Remarks();
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}

