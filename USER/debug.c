#include "debug.h"

void USART1_Init(uint32_t Bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate 	 = Bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits 	 = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity 	 = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode 	 	 = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口
#if 0	
	USART1_DMA_Init();
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#else
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif
	USART_Cmd(USART1, ENABLE);//使能串口
}

void USART1_IRQHandler(void)
{
	uint8_t Res = 0;
	
	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
	{
		Res = USART_ReceiveData(USART1);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
		USART_SendData(USART2,Res);
	}
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{    
	uint16_t cnt = 0;
	
	while((USART1->SR & 0X40)==0)
	{
		if(++cnt >= 30000)
			return ch;
	}
    USART1->DR = (uint8_t)ch;      
	return ch;
}

