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
	
	//以太网中断
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;//INT
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 	
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU; 
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//禁止以太网片选
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7;//CS       
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	ENC28J60_CS = 1;
	//禁止触摸屏片选
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4;//CS       
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	LCDTOUCH_CS = 1;
	//禁止FLASH片选
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
	
	//软件复位
	ENC28J60_WriteOp(ENC28J60_SOFT_RESET,0,ENC28J60_SOFT_RESET);
	//复位上电必须查询CLKRDY位
	while(!(ENC28J60_Read(ESTAT) & ESTAT_CLKRDY));
	//初始化接收缓冲区起始地址
	NextPacketPtr = RXSTART_INIT;
	//设置接收缓冲区起始指针
	ENC28J60_Write(ERXSTL,RXSTART_INIT & 0xFF);
	ENC28J60_Write(ERXSTH,RXSTART_INIT >> 8);
	//设置接收缓冲区读指针
	ENC28J60_Write(ERXRDPTL,RXSTART_INIT & 0xFF);
	ENC28J60_Write(ERXRDPTH,RXSTART_INIT >> 8);
	//设置接收缓冲区结束指针
	ENC28J60_Write(ERXNDL,RXSTOP_INIT & 0xFF);
	ENC28J60_Write(ERXNDH,RXSTOP_INIT >> 8);
	
	//设置发送缓冲区起始指针
	ENC28J60_Write(ETXSTL,TXSTART_INIT & 0xFF);
	ENC28J60_Write(ETXSTH,TXSTART_INIT >> 8);
	//设置发送缓冲区结束指针
	ENC28J60_Write(ETXNDL,TXSTOP_INIT & 0xFF);
	ENC28J60_Write(ETXNDH,TXSTOP_INIT >> 8);
	
	//使能单播过滤 CRC校验 格式匹配
	ENC28J60_Write(ERXFCON,ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
	ENC28J60_Write(EPMM0,0x3F);
	ENC28J60_Write(EPMM1,0x30);
	ENC28J60_Write(EPMCSL,0xF9);
	ENC28J60_Write(EPMCSH,0xF7);
	
	//使能MAC接收 允许MAC发送和接收暂停控制帧
	ENC28J60_Write(MACON1,MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	ENC28J60_Write(MACON2,0x00);
	
	//用0填充所有短帧至60字节，并追加CRC 发送CRC使能 帧长度校验 强制全双工（不支持自动协商）
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
	//全双工，默认值
	ENC28J60_Write(MAIPGL,0x12);
	ENC28J60_Write(MAIPGH,0x0C);
	ENC28J60_Write(MABBIPG,0x15);
	//最大帧长度
	ENC28J60_Write(MAMXFLL,MAX_FRAMELEN & 0xFF);
	ENC28J60_Write(MAMXFLH,MAX_FRAMELEN >> 8);
	//写入MAC地址
	ENC28J60_Write(MAADR5,pmac[0]);
	ENC28J60_Write(MAADR4,pmac[1]);
	ENC28J60_Write(MAADR3,pmac[2]);
	ENC28J60_Write(MAADR2,pmac[3]);
	ENC28J60_Write(MAADR1,pmac[4]);
	ENC28J60_Write(MAADR0,pmac[5]);
	//设置PHY为全双工，LEDB为拉电流
	ENC28J60_WritePHY(PHCON1,PHCON1_PDPXMD);
	//LED状态
	ENC28J60_WritePHY(PHLCON,0x0476);
	//半双工回环禁止
	ENC28J60_WritePHY(PHCON2,PHCON2_HDLDIS);
	//返回BANK0
	ENC28J60_SetBank(ECON1);
	//使能中断 全局中断 接收中断 接收错误中断
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,EIE,EIE_INTIE | EIE_PKTIE | EIE_RXERIE);
	//接收使能
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_RXEN);
}

void ENC28J60_Rest(void)
{
	ENC28J60_RST = 0;
	delay_ms(500);
	ENC28J60_RST = 1;
}

/* 读操作 */
uint8_t ENC28J60_ReadOp(uint8_t op,uint8_t addr)
{
	uint8_t dat = 0;
	
	ENC28J60_CS = 0;//使能
	
	dat = op | (addr & ADDR_MASK);//操作码和地址
	
	SPI1_SendByte(dat);//写数据
	
	dat = SPI1_SendByte(0xFF);//读数据
	
	//如果是MAC和MII寄存器，第一个字节无效
	if(addr & 0x80)
	{
		dat = SPI1_SendByte(0xFF);//读数据
	}
	
	ENC28J60_CS = 1;//禁止
	
	return dat;
}

/* 写操作 */
void ENC28J60_WriteOp(uint8_t op,uint8_t addr,uint8_t data)
{
	uint8_t dat = 0;
	
	ENC28J60_CS = 0;//使能
	
	dat = op | (addr & ADDR_MASK);//操作码和地址
	
	SPI1_SendByte(dat);//写数据
	
	SPI1_SendByte(data);//写数据
	
	ENC28J60_CS = 1;//禁止
}

/* 设置Bank号 */
void ENC28J60_SetBank(uint8_t addr)
{
	//是否和上次相同
	if((addr & BANK_MASK) != Enc28J60Bank)
	{
		//先清除，再写入
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR,ECON1,(ECON1_BSEL0 | ECON1_BSEL1));
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON1,(addr & BANK_MASK) >> 5);
		Enc28J60Bank = (addr & BANK_MASK);
	}
}

/* 读寄存器 */
uint8_t ENC28J60_Read(uint8_t addr)
{
	ENC28J60_SetBank(addr);//先设置寄存器区域
	return ENC28J60_ReadOp(ENC28J60_READ_CTRL_REG,addr);
}

/* 写寄存器 */
void ENC28J60_Write(uint8_t addr,uint8_t data)
{
	ENC28J60_SetBank(addr);//先设置寄存器区域
	ENC28J60_WriteOp(ENC28J60_WRITE_CTRL_REG,addr,data);
}

/* 读缓冲区 */
void ENC28J60_ReadBuffer(uint16_t len,uint8_t *pdata)
{
	ENC28J60_CS = 0;//使能
	
	SPI1_SendByte(ENC28J60_READ_BUF_MEM);//发送读缓冲区命令
	
	while(len)
	{
		*pdata = SPI1_SendByte(0x00);
		pdata++;
		len--;
	}
	
	ENC28J60_CS = 1;//禁止
}

/* 写缓冲区 */
void ENC28J60_WriteBuffer(uint16_t len,uint8_t *pdata)
{
	ENC28J60_CS = 0;//使能
	
	SPI1_SendByte(ENC28J60_WRITE_BUF_MEM);//发送读缓冲区命令
	
	while(len)
	{
		SPI1_SendByte(*pdata);
		pdata++;
		len--;
	}
	
	ENC28J60_CS = 1;//禁止
}

/* 写PHY寄存器 */
void ENC28J60_WritePHY(uint8_t addr,uint16_t data)
{
	ENC28J60_Write(MIREGADR,addr);//先向MIREGADR写入地址
	ENC28J60_Write(MIWRL,data);//写入低8位
	ENC28J60_Write(MIWRH,data >> 8);//写入高8位
	while(ENC28J60_Read(MISTAT) & MISTAT_BUSY);//等待写完成
}

/* 发送数据 */
void ENC28J60_Packet_Send(uint16_t len,uint8_t *packet)
{
	//查询发送逻辑复位
	while((ENC28J60_Read(ECON1) & ECON1_TXRTS) != 0);
	//设置发送缓冲区起始地址
	ENC28J60_Write(EWRPTL,TXSTART_INIT & 0xFF);
	ENC28J60_Write(EWRPTH,TXSTART_INIT >> 8);
	//设置发送缓冲区结束地址
	ENC28J60_Write(ETXNDL,(TXSTART_INIT + len) & 0xFF);
	ENC28J60_Write(ETXNDH,(TXSTART_INIT + len) >> 8);
	//发送控制包格式字
	ENC28J60_WriteOp(ENC28J60_WRITE_BUF_MEM,0,0x00);
	//发送数据包
	ENC28J60_WriteBuffer(len,packet);
	//开始发送
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_TXRTS);
	//复位发送逻辑问题
	if(ENC28J60_Read(EIR) & EIR_TXERIF)
	{
		ENC28J60_SetBank(ECON1);
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR,ECON1,ECON1_TXRTS);
	}
}

/* 读取数据 */
uint16_t ENC28J60_Packet_Receive(uint16_t maxlen,uint8_t *packet)
{
	uint16_t rxstatus = 0;
	uint16_t len = 0;
	
	//是否接收到数据
	if(ENC28J60_Read(EPKTCNT) == 0)
		return 0;
	
	//设置接收缓冲区读指针
	ENC28J60_Write(ERDPTL,NextPacketPtr & 0xFF);
	ENC28J60_Write(ERDPTH,NextPacketPtr >> 8);
	
	//读下一个包的指针
	NextPacketPtr  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0);
	NextPacketPtr |= (ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0) << 8);
	//读包长度
	len  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0);
	len |= (ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0) << 8);
	//去掉CRC校验
	len -= 4;
	//读接收状态
	rxstatus  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0);
	rxstatus |= (ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM,0) << 8);
	//限制接收长度
	if(len > maxlen - 1)
	{
		len = maxlen - 1;
	}
	//检测CRC和符合错误
	if((rxstatus & 0x80) == 0)
	{
		len = 0;//无效
	}
	else
	{
		ENC28J60_ReadBuffer(len,packet);//从缓冲区拷贝数据
	}
	//移动缓冲区读指针
	ENC28J60_Write(ERXRDPTL,NextPacketPtr & 0xFF);
	ENC28J60_Write(ERXRDPTH,NextPacketPtr >> 8);
	//数据包计数器递减，我们已经获取这个包
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET,ECON2,ECON2_PKTDEC);
	
	return len;
}

