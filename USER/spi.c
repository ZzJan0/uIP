#include "spi.h"

void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;//SCLK��MISO��MOSI
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                      
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;                        
	GPIO_Init(GPIOA,&GPIO_InitStruct);

    SPI_InitStruct.SPI_Direction 	 = SPI_Direction_2Lines_FullDuplex;//˫��
    SPI_InitStruct.SPI_Mode 		 = SPI_Mode_Master;//����
    SPI_InitStruct.SPI_DataSize 	 = SPI_DataSize_8b;//���ݳ���
    SPI_InitStruct.SPI_CPOL 		 = SPI_CPOL_Low;//ʱ�Ӽ���
    SPI_InitStruct.SPI_CPHA 		 = SPI_CPHA_1Edge;//��λ
    SPI_InitStruct.SPI_NSS 			 = SPI_NSS_Soft;//�������
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//��Ƶ����
    SPI_InitStruct.SPI_FirstBit 	 = SPI_FirstBit_MSB;//��λ��ǰ
    SPI_InitStruct.SPI_CRCPolynomial = 7;//CRCУ��
    SPI_Init(SPI1,&SPI_InitStruct);
    SPI_Cmd(SPI1,ENABLE);
}

uint8_t SPI1_SendByte(uint8_t dat)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1,dat);
	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}
