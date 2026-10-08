#ifndef __SW_I2C_SEN_H
#define __SW_I2C_SEN_H

#include "hc32_ll.h"

#define SEN_SW_I2C_SDA_PORT    GPIO_PORT_A
#define SEN_SW_I2C_SDA_PIN     GPIO_PIN_08
#define SEN_SW_I2C_SCL_PORT    GPIO_PORT_C
#define SEN_SW_I2C_SCL_PIN     GPIO_PIN_09

void SEN_SW_I2C_W_SCL(uint8_t BitValue);
void SEN_SW_I2C_W_SDA(uint8_t BitValue);
uint8_t SEN_SW_I2C_R_SDA(void);
void SEN_SW_I2C_Init(void);
void SEN_SW_I2C_Start(void);
void SEN_SW_I2C_Stop(void);
void SEN_SW_I2C_SendByte(uint8_t Byte);
uint8_t SEN_SW_I2C_ReceiveByte(uint8_t ack);
void SEN_SW_I2C_SendAck(uint8_t AckBit);
uint8_t SEN_SW_I2C_ReceiveAck(void);
uint8_t SEN_SW_I2C_Write_SingleByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data);
uint8_t SEN_SW_I2C_Read_SingleByte(uint8_t SlaveAddress, uint8_t REG_Address);
uint8_t SEN_SW_I2C_Write_MultiBytes(uint8_t DeviceAddr, uint8_t REG_Address, uint8_t BytesNum, uint8_t *buf);
uint8_t SEN_SW_I2C_Read_MultiBytes(uint8_t DeviceAddr, uint8_t REG_Address, uint8_t BytesNum, uint8_t *buf);

#endif
