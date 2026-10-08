#include "SW_I2C_SEN.h"

void SEN_SW_I2C_W_SCL(uint8_t BitValue)
{
	switch(BitValue)
	{
		case 0:
			GPIO_ResetPins (SEN_SW_I2C_SCL_PORT, SEN_SW_I2C_SCL_PIN);
			break;
		case 1:
			GPIO_SetPins (SEN_SW_I2C_SCL_PORT, SEN_SW_I2C_SCL_PIN);
			break;
		default:
			break;
	}
	
	DDL_DelayUS (5U);
}

void SEN_SW_I2C_W_SDA(uint8_t BitValue)
{
	switch(BitValue)
	{
		case 0:
			GPIO_ResetPins (SEN_SW_I2C_SDA_PORT, SEN_SW_I2C_SDA_PIN);
			break;
		case 1:
			GPIO_SetPins (SEN_SW_I2C_SDA_PORT, SEN_SW_I2C_SDA_PIN);
			break;
		default:
			break;
	}
	
	DDL_DelayUS (5U);
}

uint8_t SEN_SW_I2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputPins(SEN_SW_I2C_SDA_PORT, SEN_SW_I2C_SDA_PIN);
	DDL_DelayUS (5U);
	return BitValue;
}

void SEN_SW_I2C_Init(void)
{
	GPIO_SetPins (SEN_SW_I2C_SDA_PORT, SEN_SW_I2C_SDA_PIN);
	GPIO_SetPins (SEN_SW_I2C_SCL_PORT, SEN_SW_I2C_SCL_PIN);
}

void SEN_SW_I2C_Start(void)
{
	SEN_SW_I2C_W_SDA(1);
	SEN_SW_I2C_W_SCL(1);
	SEN_SW_I2C_W_SDA(0);
	SEN_SW_I2C_W_SCL(0);
}

void SEN_SW_I2C_Stop(void)
{
	SEN_SW_I2C_W_SDA(0);
	SEN_SW_I2C_W_SCL(1);
	SEN_SW_I2C_W_SDA(1);
}

void SEN_SW_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	
	for (i = 0; i < 8; i++)
	{
		SEN_SW_I2C_W_SDA(!!(Byte & (0x80 >> i)));
		SEN_SW_I2C_W_SCL(1);
		SEN_SW_I2C_W_SCL(0);
	}
}

uint8_t SEN_SW_I2C_ReceiveByte(uint8_t ack)
{
	uint8_t i, Byte = 0x00;
	SEN_SW_I2C_W_SDA(1);
	for (i = 0; i < 8; i++)
	{
		SEN_SW_I2C_W_SCL(1);
		if (SEN_SW_I2C_R_SDA() == 1)
		{
			Byte |= (0x80 >> i);
		}
		SEN_SW_I2C_W_SCL(0);
	}
	if (!ack)
		SEN_SW_I2C_SendAck(1); // 发送nACK
	else
		SEN_SW_I2C_SendAck(0); // 发送ACK
	return Byte;
}

void SEN_SW_I2C_SendAck(uint8_t AckBit)
{
	SEN_SW_I2C_W_SDA(AckBit);
	SEN_SW_I2C_W_SCL(1);
	SEN_SW_I2C_W_SCL(0);
}

uint8_t SEN_SW_I2C_ReceiveAck(void)
{
	uint8_t AckBit;
	SEN_SW_I2C_W_SDA(1);
	SEN_SW_I2C_W_SCL(1);
	AckBit = SEN_SW_I2C_R_SDA();
	SEN_SW_I2C_W_SCL(0);
	return AckBit;
}

uint8_t SEN_SW_I2C_Write_SingleByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
	SEN_SW_I2C_Start();
	SEN_SW_I2C_SendByte((SlaveAddress << 1) | 0);   // 发送器件地址+写命令
	if (SEN_SW_I2C_ReceiveAck())					 // 等待应答
	{
		SEN_SW_I2C_Stop();
		return 1;
	}
	SEN_SW_I2C_SendByte(REG_Address);               // 写寄存器地址
	SEN_SW_I2C_ReceiveAck();			             // 等待应答
	SEN_SW_I2C_SendByte(REG_data);	                 // 发送数据
	if (SEN_SW_I2C_ReceiveAck())		             // 等待ACK
	{
		SEN_SW_I2C_Stop();
		return 1;
	}
	SEN_SW_I2C_Stop();
	return 0;
}

uint8_t SEN_SW_I2C_Read_SingleByte(uint8_t SlaveAddress, uint8_t REG_Address)
{
	uint8_t res;
	SEN_SW_I2C_Start();
	SEN_SW_I2C_SendByte((SlaveAddress << 1) | 0);   // 发送器件地址+写命令
	SEN_SW_I2C_ReceiveAck();						 // 等待应答
	SEN_SW_I2C_SendByte(REG_Address);			     // 写寄存器地址
	SEN_SW_I2C_ReceiveAck();						 // 等待应答
	SEN_SW_I2C_Start();
	SEN_SW_I2C_SendByte((SlaveAddress << 1) | 1);   // 发送器件地址+读命令
	SEN_SW_I2C_ReceiveAck();						 // 等待应答
	res = SEN_SW_I2C_ReceiveByte(0);				 // 读取数据,发送nACK
	SEN_SW_I2C_Stop();							     // 产生一个停止条件
	return res;
}

uint8_t SEN_SW_I2C_Write_MultiBytes(uint8_t DeviceAddr, uint8_t REG_Address, uint8_t BytesNum, uint8_t *buf)
{
	uint8_t i;
	SEN_SW_I2C_Start();
	SEN_SW_I2C_SendByte((DeviceAddr << 1) | 0); // 发送器件地址+写命令
	if (SEN_SW_I2C_ReceiveAck())				   // 等待应答
	{
		SEN_SW_I2C_Stop();
		return 1;
	}
	SEN_SW_I2C_SendByte(REG_Address); // 写寄存器地址
	SEN_SW_I2C_ReceiveAck();			 // 等待应答
	for (i = 0; i < BytesNum; i++)
	{
		SEN_SW_I2C_SendByte(buf[i]); // 发送数据
		if (SEN_SW_I2C_ReceiveAck()) // 等待ACK
		{
			SEN_SW_I2C_Stop();
			return 1;
		}
	}
	SEN_SW_I2C_Stop();
	return 0;
}

uint8_t SEN_SW_I2C_Read_MultiBytes(uint8_t DeviceAddr, uint8_t REG_Address, uint8_t BytesNum, uint8_t *buf)
{
	SEN_SW_I2C_Start();
	SEN_SW_I2C_SendByte((DeviceAddr << 1) | 0); // 发送器件地址+写命令
	if (SEN_SW_I2C_ReceiveAck())				   // 等待应答
	{
		SEN_SW_I2C_Stop();
		return 1;
	}
	SEN_SW_I2C_SendByte(REG_Address); // 写寄存器地址
	SEN_SW_I2C_ReceiveAck();			 // 等待应答
	SEN_SW_I2C_Start();
	SEN_SW_I2C_SendByte((DeviceAddr << 1) | 1); // 发送器件地址+读命令
	SEN_SW_I2C_ReceiveAck();					   // 等待应答
	while (BytesNum)
	{
		if (BytesNum == 1)
			*buf = SEN_SW_I2C_ReceiveByte(0); // 读数据,发送nACK
		else
			*buf = SEN_SW_I2C_ReceiveByte(1); // 读数据,发送ACK
		BytesNum--;
		buf++;
	}
	SEN_SW_I2C_Stop(); // 产生一个停止条件
	return 0;
}
