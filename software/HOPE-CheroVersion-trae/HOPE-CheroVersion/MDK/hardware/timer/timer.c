#include "timer.h"

#include <stdlib.h>

#include "BEEPER.h"
#include "Encoder.h"
#include "multi_button.h"

void HugoUI_Ticks(void);

uint32_t Time_ms = 0U;

static uint8_t  s_u8KeyCount     = 0U;
static uint8_t  s_u8EncoderCount = 0U;
static uint16_t s_u16BeeperCount = 0U;
static uint16_t s_u16NETCount    = 0U;

uint8_t NET_flag = 0U;

void Timer_Init(void)
{
    (void)SysTick_Init(1000U);
}

void SysTick_Handler(void)
{
    SysTick_IncTick();
    __DSB();

    Time_ms++;

    HugoUI_Ticks();

    if (++s_u8KeyCount >= 5U)
    {
        button_ticks();
        s_u8KeyCount = 0U;
    }

    if (++s_u8EncoderCount >= 10U)
    {
        Encoder_Handler();
        s_u8EncoderCount = 0U;
    }

    if (++s_u16BeeperCount >= 10U)
    {
        Beeper_Proc();
        s_u16BeeperCount = 0U;
    }

    if (++s_u16NETCount >= 5000U)
    {
        NET_flag = 1U;
        s_u16NETCount = 0U;
    }
}

uint32_t RandomCreate(void)
{
    srand(Time_ms);
    return ((uint32_t)rand() % 1000U);
}
