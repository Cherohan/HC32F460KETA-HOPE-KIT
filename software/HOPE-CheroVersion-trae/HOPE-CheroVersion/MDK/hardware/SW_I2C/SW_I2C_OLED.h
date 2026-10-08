#ifndef __SW_I2C_OLED_H
#define __SW_I2C_OLED_H

#include "hc32_ll.h"

#define OLED_SW_I2C_SDA_PORT    GPIO_PORT_A
#define OLED_SW_I2C_SDA_PIN     GPIO_PIN_10
#define OLED_SW_I2C_SCL_PORT    GPIO_PORT_A
#define OLED_SW_I2C_SCL_PIN     GPIO_PIN_09

void OLED_SW_I2C_W_SCL(uint8_t BitValue);
void OLED_SW_I2C_W_SDA(uint8_t BitValue);
void OLED_SW_I2C_Init(void);
void OLED_SW_I2C_Start(void);
void OLED_SW_I2C_Stop(void);
void OLED_SW_I2C_SendByte(uint8_t Byte);
void OLED_SW_I2C_WriteCommand(uint8_t Command);
void OLED_SW_I2C_WriteData(uint8_t Data);

#endif
