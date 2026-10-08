#include "W25Qxx.h"

static void W25Q_CS_Low(void)
{
    GPIO_ResetPins(W25Q_CS_PORT, W25Q_CS_PIN);
}

static void W25Q_CS_High(void)
{
    GPIO_SetPins(W25Q_CS_PORT, W25Q_CS_PIN);
}

static uint8_t W25Q_SPI_ReadWriteByte(uint8_t u8Dat)
{
    uint8_t u8Rx = 0xFFU;
    (void)SPI_TransReceive(CM_SPI1, &u8Dat, &u8Rx, 1U, 1000U);
    return u8Rx;
}

void W25Q_Init(void)
{
    W25Q_CS_High();
}

void W25Q_ReadJEDECID(uint8_t *pu8Mfr, uint8_t *pu8Type, uint8_t *pu8Cap)
{
    W25Q_CS_Low();
    (void)W25Q_SPI_ReadWriteByte(W25Q_CMD_JEDEC_ID);
    *pu8Mfr  = W25Q_SPI_ReadWriteByte(0xFFU);
    *pu8Type = W25Q_SPI_ReadWriteByte(0xFFU);
    *pu8Cap  = W25Q_SPI_ReadWriteByte(0xFFU);
    W25Q_CS_High();
}

uint32_t W25Q_GetCapacity(void)
{
    uint8_t u8Mfr, u8Type, u8Cap;

    W25Q_ReadJEDECID(&u8Mfr, &u8Type, &u8Cap);

    if ((u8Mfr == 0x00U) || (u8Mfr == 0xFFU))
    {
        return 0UL;
    }
    return (1UL << u8Cap);   /* 2^cap 字节 */
}
