#include "delay.h"
#include "hc32_ll_utility.h"

void delay_init(uint8_t SYSCLK)
{
    (void)SYSCLK;
}

void delay_us(uint32_t nus)
{
    DDL_DelayUS(nus);
}

void delay_ms(uint16_t nms)
{
    DDL_DelayMS((uint32_t)nms);
}

/* 纳秒级高精度延时，基于 DWT CYCCNT 周期计数器（1 tick = 5ns @200MHz）。
 * 用于 WS2812 等需要亚微秒分辨率的单线协议场景。 */
void delay_ns(uint32_t nns)
{
    uint32_t u32Ticks;
    uint32_t u32Start;

    /* 惰性初始化 DWT（避免依赖外部初始化顺序） */
    if (0UL == (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk))
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0UL;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }

    u32Ticks = (SystemCoreClock / 1000000UL) * nns / 1000UL;
    if (0UL == u32Ticks)
    {
        u32Ticks = 1UL;
    }

    u32Start = DWT->CYCCNT;
    while ((uint32_t)(DWT->CYCCNT - u32Start) < u32Ticks)
    {
        ;
    }
}
