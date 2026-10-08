#include "SW_I2C_OLED.h"

void OLED_SW_I2C_W_SCL(uint8_t BitValue)
{
	switch(BitValue)
	{
		case 0:
			GPIO_ResetPins (OLED_SW_I2C_SCL_PORT, OLED_SW_I2C_SCL_PIN);
			break;
		case 1:
			GPIO_SetPins (OLED_SW_I2C_SCL_PORT, OLED_SW_I2C_SCL_PIN);
			break;
		default:
			break;
	}
}

void OLED_SW_I2C_W_SDA(uint8_t BitValue)
{
	switch(BitValue)
	{
		case 0:
			GPIO_ResetPins (OLED_SW_I2C_SDA_PORT, OLED_SW_I2C_SDA_PIN);
			break;
		case 1:
			GPIO_SetPins (OLED_SW_I2C_SDA_PORT, OLED_SW_I2C_SDA_PIN);
			break;
		default:
			break;
	}
}


void OLED_SW_I2C_Init(void)
{
	OLED_SW_I2C_W_SCL(1);
	OLED_SW_I2C_W_SDA(1);
}

void OLED_SW_I2C_Start(void)
{
	OLED_SW_I2C_W_SDA(1);
	OLED_SW_I2C_W_SCL(1);
	OLED_SW_I2C_W_SDA(0);
	OLED_SW_I2C_W_SCL(0);
}

void OLED_SW_I2C_Stop(void)
{
	OLED_SW_I2C_W_SDA(0);
	OLED_SW_I2C_W_SCL(1);
	OLED_SW_I2C_W_SDA(1);
}

void OLED_SW_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_SW_I2C_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_SW_I2C_W_SCL(1);
		OLED_SW_I2C_W_SCL(0);
	}
	OLED_SW_I2C_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_SW_I2C_W_SCL(0);
}

void OLED_SW_I2C_WriteCommand(uint8_t Command)
{
	OLED_SW_I2C_Start();
	OLED_SW_I2C_SendByte(0x78);		//从机地址
	OLED_SW_I2C_SendByte(0x00);		//写命令
	OLED_SW_I2C_SendByte(Command); 
	OLED_SW_I2C_Stop();
}

void OLED_SW_I2C_WriteData(uint8_t Data)
{
	OLED_SW_I2C_Start();
	OLED_SW_I2C_SendByte(0x78);		//从机地址
	OLED_SW_I2C_SendByte(0x40);		//写数据
	OLED_SW_I2C_SendByte(Data);
	OLED_SW_I2C_Stop();
}

// 设置光标位置


// 反色
void OLED_ColorTurn(uint8_t color)
{
	if(color == 0)
		OLED_SW_I2C_WriteCommand(0xA6);
	if(color == 1)
		OLED_SW_I2C_WriteCommand(0xA7);
}

// 屏幕旋转180度
void OLED_DisplayTurn(uint8_t direction)
{
	if(direction == 0)
	{
		OLED_SW_I2C_WriteCommand(0xC8);
		OLED_SW_I2C_WriteCommand(0xA1);
	}
	if(direction == 1)
	{
		OLED_SW_I2C_WriteCommand(0xC0);
		OLED_SW_I2C_WriteCommand(0xA0);
	}
}

// 开启显示
void OLED_DisPlayOn(void)
{
	OLED_SW_I2C_WriteCommand(0x8D);
	OLED_SW_I2C_WriteCommand(0x14);
	OLED_SW_I2C_WriteCommand(0xAF);
}

// 关闭显示
void OLED_DisPlayOff(void)
{
	OLED_SW_I2C_WriteCommand(0x8D);
	OLED_SW_I2C_WriteCommand(0x10);
	OLED_SW_I2C_WriteCommand(0xAE);
}
