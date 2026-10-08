#ifndef __W25QXX_H
#define __W25QXX_H

#include "hc32_ll.h"

#define W25Q_CS_PORT        GPIO_PORT_C
#define W25Q_CS_PIN         GPIO_PIN_01

#define W25Q_CMD_JEDEC_ID   0x9FU

void     W25Q_Init(void);
void     W25Q_ReadJEDECID(uint8_t *pu8Mfr, uint8_t *pu8Type, uint8_t *pu8Cap);
uint32_t W25Q_GetCapacity(void);

#endif
