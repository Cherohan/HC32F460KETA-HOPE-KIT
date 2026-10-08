#ifndef __WS2812_H
#define __WS2812_H

#include "hc32_ll.h"

#define WS_PIN          GPIO_PIN_03
#define WS_PORT         GPIO_PORT_A
#define WS_BRIGHTNESS   2U

void WS2812_SendBit(uint8_t bit);
void WS2812_SendByte(uint8_t dat);
void WS2812_Reset(void);
void WS2812_SetRGB(uint8_t r, uint8_t g, uint8_t b);
void WS2812_Test(void);
void WS2812_Ringle_Test(void);

#endif
