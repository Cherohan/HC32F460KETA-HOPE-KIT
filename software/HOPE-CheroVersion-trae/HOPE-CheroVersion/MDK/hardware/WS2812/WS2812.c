#include "WS2812.h"
#include "delay.h"

__STATIC_INLINE void WS2812_SendBit(uint8_t bit)
{
    if(bit)
    {
        GPIO_SetPins(WS_PORT, WS_PIN);    /* 1 码高电平 800ns */
        delay_ns(800UL);
        GPIO_ResetPins(WS_PORT, WS_PIN);  /* 1 码低电平 450ns */
        delay_ns(450UL);
    }
    else
    {
        GPIO_SetPins(WS_PORT, WS_PIN);    /* 0 码高电平 400ns */
        delay_ns(400UL);
        GPIO_ResetPins(WS_PORT, WS_PIN);  /* 0 码低电平 850ns */
        delay_ns(850UL);
    }
}

void WS2812_SendByte(uint8_t dat)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        WS2812_SendBit(dat & 0x80);
        dat <<= 1;
    }
}

uint8_t WS_ScaleBright(uint8_t val)
{
    return ( (uint16_t)val * WS_BRIGHTNESS ) >> 8U;
}

void WS2812_SetRGB(uint8_t r, uint8_t g, uint8_t b)
{
    r = WS_ScaleBright(r);
    g = WS_ScaleBright(g);
    b = WS_ScaleBright(b);

    __disable_irq();
    WS2812_SendByte(g);
    WS2812_SendByte(r);
    WS2812_SendByte(b);
    __enable_irq();
}

void WS2812_Reset(void)
{
    GPIO_ResetPins(WS_PORT, WS_PIN);
    DDL_DelayUS(60U);
}

void WS2812_Test(void)
{
    // Reset: 拉低大于 50us
	GPIO_ResetPins (WS_PORT, WS_PIN);
	DDL_DelayUS (60U);
	WS2812_SetRGB (0xFF,0x00,0x00); // 红色
	DDL_DelayMS (200U);
	GPIO_ResetPins (WS_PORT, WS_PIN);
	DDL_DelayUS (60U);
	WS2812_SetRGB (0x00,0x00,0xFF); // 蓝色
	DDL_DelayMS (200U);

	GPIO_ResetPins (WS_PORT, WS_PIN);
	DDL_DelayUS (60U);
	WS2812_SetRGB (0x00,0xFF,0x00); // 绿色
	DDL_DelayMS (200U);
}

void WS2812_Ringle_Test(void)
{
	static uint16_t hue = 0;
    uint8_t r,g,b;
    uint8_t sector;
    uint16_t pos;
    uint8_t v = 200;   // 亮度 0~255
    uint8_t s = 180;   // 饱和度 0~255，越低越发白（混白效果）

    sector = hue / 60;
    pos = hue % 60;

    // 计算HSV基础分量
    uint8_t p = (uint16_t)v * (255 - s) / 255;
    uint8_t q = (uint16_t)v * (255 - (uint16_t)s * pos / 60U) / 255U;
    uint8_t t = (uint16_t)v * (255 - (uint16_t)s * (60U - pos) / 60U) / 255U;

    switch(sector)
    {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = 0; g = 0; b = 0; break;
    }

    GPIO_ResetPins(WS_PORT, WS_PIN);
    DDL_DelayUS(60U);
    WS2812_SetRGB(r, g, b);

    hue++;
    if(hue >= 360)
    {
        hue = 0;
    }
	
	DDL_DelayUS(3000U);
}
