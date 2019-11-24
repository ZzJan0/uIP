#include "enc28j60.h"

static uint8_t Enc28J60Bank = 0;
static uint16_t NextPacketPtr = RXSTART_INIT;

void ENC28J60_Init(uint8_t *pmac)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	//��̫���ж�
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;//INT
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 	
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU; 
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//��ֹ��̫��Ƭѡ
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7;//CS       
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	ENC28J60_CS = 1;
	//��ֹ������Ƭѡ
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4;//CS       
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	LCDTOUCH_CS = 1;
	//��ֹFLASHƬѡ
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6;//CS       
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	SST25VF016B_CS = 1;

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;//RST
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOE, &GPIO_InitStruct);   
	ENC28J60_RST = 1;
	
	SPI1_Init();
	
	//�����λ
	ENC28J60_WriteOp(ENC28J60_SOFT_RESET,0,ENC28J60_SOFT_RESET);
	//��λ�ϵ�����ѯCLKRDYλ
	while(!(ENC28J60_Read(ESTAT) & ESTAT_CLKRDY));
	//��ʼ�����ջ�������ʼ��ַ
	NextPacketPtr = RXSTART_INIT;
	//���ý��ջ�������ʼָ��
	ENC28J60_Write(ERXSTL,RXSTART_INIT & 0xFF);
	ENC28J60_Write(ERXSTH,RXSTART_INIT >> 8);
	//���ý��ջ�������ָ��
	ENC28J60_Write(ERXRDPTL,RXSTART_INIT & 0xFF);
	ENC28J60_Write(ERXRDPTH,RXSTART_INIT >> 8);
	//���ý��ջ���������ָ��
	ENC28J60_Write(ERXNDL,RXSTOP_INIT & 0xFF);
	ENC28J60_Write(ERXNDH,RXSTOP_INIT >> 8);
	
	//���÷��ͻ�������ʼָ��
	ENC28J60_Write(ETXSTL,TXSTART_INIT & 0xFF);
	ENC28J60_Write(ETXSTH,TXSTART_INIT >> 8);
	//���÷��ͻ���������ָ��
	ENC28J60_Write(ETXNDL,TXSTOP_INIT & 0xFF);
	ENC28J60_Write(ETXNDH,TXSTOP_INIT >> 8);
	
	//ʹ�ܵ������� CRCУ�� ��ʽƥ��
	ENC28J60_Write(ERXFCON,ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
	ENC28J60_Write(EPMM0,0x3F);
	ENC28J60_Write(EPMM1,0x30);
	ENC28J60_Write(EPMCSL,0xF9);
	ENC28J60_Write(EPMCSH,0xF7);
	
	//ʹ��MAC���� ����MAC���ͺͽ�����ͣ����֡
	ENC28J60_Write(MACON1,MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	ENC28J60_Write(MACON2,0x00);
	
	//��0������ж�֡��60�ֽڣ���׷��CRC ����CRCʹ�� ֡����У�� ǿ��ȫ˫������֧���Զ�Э�̣�
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
	//ȫ˫����Ĭ��ֵ
	ENC28J60_Write(MAIPGL,0x12);
	ENC28J60_Write(MAIPGH,0x0C);
	ENC28J60_Write(MABBIPG,0x15);
	//���֡����
	ENC28J60_Write(MAMXFLL,MAX_FRAMELEN & 0xFF);
	ENC28J60_Write(MAMXFLH,MAX_FRAMELEN >> 8);
	//д��MAC��ַ
	ENC28J60_Write(MAADR5,pmac[0]);
	ENC28J60_Write(MAADR4,pmac[1]);
	ENC28J60_Write(MAADR3,pmac[2]);
	ENC28J60_Write(MAADR2,pmac[3]);
	ENC28J60_Write(MAADR1,pmac[4]);
	ENC28J60_Write(MAADR0,pmac[5]);
	//����PHYΪȫ˫����LEDBΪ������
	ENC28J60_WritePHY(PHCON1,PHCON1_PDPXMD);
	//LED״̬
	ENC28J60_WritePHY(PHLCON,0x0476);
	//��˫���ػ���ֹ
	ENC28J60_WritePHY(PHCON2,PHCON2_HDLDIS);
	//����BANK0
	ENC28J60_SetBank(ECON1);
	//ʹ���ж� ȫ���ж� �����ж� ���մ����ж�
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,EIE,EIE_INTIE | EIE_PKTIE | EIE_RXERIE);
	//����ʹ��
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_RXEN);
}

void ENC28J60_Rest(void)
{
	ENC28J60_RST = 0;
	delay_ms(500);
	ENC28J60_RST = 1;
}

/* ������ */
uint8_t ENC28J60_ReadOp(uint8_t op,uint8_t addr)
{
	uint8_t dat = 0;
	
	ENC28J60_CS = 0;//ʹ��
	
	dat = op | (addr & ADDR_MASK);//������͵�ַ
	
	SPI1_SendByte(dat);//д����
	
	dat = SPI1_SendByte(0xFF);//������
	
	//�����MAC��MII�Ĵ�������һ���ֽ���Ч
	if(addr & 0x80)
	{
		dat = SPI1_SendByte(0xFF);//������
	}
	
	ENC28J60_CS = 1;//��ֹ
	
	return dat;
}

/* д���� */
void ENC28J60_WriteOp(uint8_t op,uint8_t addr,uint8_t data)
{
	uint8_t dat = 0;
	
	ENC28J60_CS = 0;//ʹ��
	
	dat = op | (addr & ADDR_MASK);//������͵�ַ
	
	SPI1_SendByte(dat);//д����
	
	SPI1_SendByte(data);//д����
	
	ENC28J60_CS = 1;//��ֹ
}

/* ����Bank�� */
void ENC28J60_SetBank(uint8_t addr)
{
	//�Ƿ���ϴ���ͬ
	if((addr & BANK_MASK) != Enc28J60Bank)
	{
		//���������д��
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR,ECON1,(ECON1_BSEL0 | ECON1_BSEL1));
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON1,(addr & BANK_MASK) >> 5);
		Enc28J60Bank = (addr & BANK_MASK);
	}
}

/* ���Ĵ��� */
uint8_t ENC28J60_Read(uint8_t addr)
{
	ENC28J60_SetBank(addr);//�����üĴ�������
	return ENC28J60_ReadOp(ENC28J60_READ_CTRL_REG,addr);
}

/* д�Ĵ��� */
void ENC28J60_Write(uint8_t addr,uint8_t data)
{
	ENC28J60_SetBank(addr);//�����üĴ�������
	ENC28J60_WriteOp(ENC28J60_WRITE_CTRL_REG,addr,data);
}

/* �������� */
void ENC28J60_ReadBuffer(uint16_t len,uint8_t *pdata)
{
	ENC28J60_CS = 0;//ʹ��
	
	SPI1_SendByte(ENC28J60_READ_BUF_MEM);//���Ͷ�����������
	
	while(len)
	{
		*pdata = SPI1_SendByte(0x00);
		pdata++;
		len--;
	}
	
	ENC28J60_CS = 1;//��ֹ
}

/* д������ */
void ENC28J60_WriteBuffer(uint16_t len,uint8_t *pdata)
{
	ENC28J60_CS = 0;//ʹ��
	
	SPI1_SendByte(ENC28J60_WRITE_BUF_MEM);//���Ͷ�����������
	
	while(len)
	{
		SPI1_SendByte(*pdata);
		pdata++;
		len--;
	}
	
	ENC28J60_CS = 1;//��ֹ
}

/* дPHY�Ĵ��� */
void ENC28J60_WritePHY(uint8_t addr,uint16_t data)
{
	ENC28J60_Write(MIREGADR,addr);//����MIREGADRд���ַ
	ENC28J60_Write(MIWRL,data);//д���8λ
	ENC28J60_Write(MIWRH,data >> 8);//д���8λ
	while(ENC28J60_Read(MISTAT) & MISTAT_BUSY);//�ȴ�д���
}

/* �������� */
void ENC28J60_Packet_Send(uint16_t len,uint8_t *packet)
{
	//��ѯ�����߼���λ
	while((ENC28J60_Read(ECON1) & ECON1_TXRTS) != 0);
	//���÷��ͻ�������ʼ��ַ
	ENC28J60_Write(EWRPTL,TXSTART_INIT & 0xFF);
	ENC28J60_Write(EWRPTH,TXSTART_INIT >> 8);
	//���÷��ͻ�����������ַ
	ENC28J60_Write(ETXNDL,(TXSTART_INIT + len) & 0xFF);
	ENC28J60_Write(ETXNDH,(TXSTART_INIT + len) >> 8);
	//���Ϳ��ư���ʽ��
	ENC28J60_WriteOp(ENC28J60_WRITE_BUF_MEM,0,0x00);
	//�������ݰ�
	ENC28J60_WriteBuffer(len,packet);
	//��ʼ����
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_TXRTS);
	//��λ�����߼�����
	if(ENC28J60_Read(EIR) & EIR_TXERIF)
	{
		ENC28J60_SetBank(ECON1);
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR,ECON1,ECON1_TXRTS);
	}
}

/* ��ȡ���� */
uint16_t ENC28J60_Packet_Receive(uint16_t maxlen,uint8_t *packet)
{
	uint16_t rxstatus = 0;
	uint16_t len = 0;
	
	//�Ƿ���յ�����
	if(ENC28J60_Read(EPKTCNT) == 0)
		return 0;
	
	//���ý��ջ�������ָ��
	ENC28J60_Write(ERDPTL,NextPacketPtr & 0xFF);
	ENC28J60_Write(ERDPTH,NextPacketPtr >> 8);
	
	//����һ������ָ��
	NextPacketPtr  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0);
	NextPacketPtr |= (ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0) << 8);
	//��������
	len  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0);
	len |= (ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0) << 8);
	//ȥ��CRCУ��
	len -= 4;
	//������״̬
	rxstatus  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0);
	rxstatus |= (ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0) << 8);
	//���ƽ��ճ���
	if(len > maxlen - 1)
	{
		len = maxlen - 1;
	}
	//���CRC�ͷ��ϴ���
	if((rxstatus & 0x80) == 0)
	{
		len = 0;//��Ч
	}
	else
	{
		ENC28J60_ReadBuffer(len,packet);//�ӻ�������������
	}
	//�ƶ���������ָ��
	ENC28J60_Write(ERXRDPTL,NextPacketPtr & 0xFF);
	ENC28J60_Write(ERXRDPTH,NextPacketPtr >> 8);
	//���ݰ��������ݼ��������Ѿ���ȡ�����
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON2,ECON2_PKTDEC);
	
	return len;
}

