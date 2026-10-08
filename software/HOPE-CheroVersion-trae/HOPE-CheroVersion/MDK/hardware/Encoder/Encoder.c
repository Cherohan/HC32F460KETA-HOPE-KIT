#include "Encoder.h"
#include "BEEPER.h"
#include "hc32_ll_fcg.h"
#include "hc32_ll_tmra.h"
#include "hc32_ll_gpio.h"

/* 编码器：TMRA_3 硬件正交计数，CLKA=PA6，CLKB=PA7（官方例程写法：只 SetFunc，不 GPIO_Init） */
#define ENCODER_TMRA    (CM_TMRA_3)
#define ENCODER_FCG     (FCG2_PERIPH_TMRA_3)
#define ENCODER_BASE    (32768U)

/* X1 正交：一个 detent（格）对应 1 个计数 */
#define ENCODER_THRESHOLD (1)

void Encoder_Init(void)
{
    stc_tmra_init_t stcTmraInit;

    FCG_Fcg2PeriphClockCmd(ENCODER_FCG, ENABLE);

    (void)TMRA_StructInit(&stcTmraInit);
    stcTmraInit.u8CountSrc = TMRA_CNT_SRC_HW;
    /* 官方对称 X1：Up/Down 条件互相对称，能正确区分顺/逆时针 */
    stcTmraInit.hw_count.u16CountUpCond   = TMRA_CNT_UP_COND_CLKB_HIGH_CLKA_RISING;
    stcTmraInit.hw_count.u16CountDownCond = TMRA_CNT_DOWN_COND_CLKA_HIGH_CLKB_RISING;
    (void)TMRA_Init(ENCODER_TMRA, &stcTmraInit);

    /* 引脚复用为 TMRA_3 CLKA/CLKB（官方例程：只 SetFunc，不 GPIO_Init/上下拉） */
    GPIO_SetFunc(GPIO_PORT_A, GPIO_PIN_06, GPIO_FUNC_5);
    GPIO_SetFunc(GPIO_PORT_A, GPIO_PIN_07, GPIO_FUNC_5);

    /* CLKA/CLKB 硬件数字滤波去抖（DIV64，滤除机械编码器弹跳毛刺） */
    TMRA_SetFilterClockDiv(ENCODER_TMRA, TMRA_PIN_CLKA, TMRA_FILTER_CLK_DIV64);
    TMRA_SetFilterClockDiv(ENCODER_TMRA, TMRA_PIN_CLKB, TMRA_FILTER_CLK_DIV64);
    TMRA_FilterCmd(ENCODER_TMRA, TMRA_PIN_CLKA, ENABLE);
    TMRA_FilterCmd(ENCODER_TMRA, TMRA_PIN_CLKB, ENABLE);

    TMRA_SetCountValue(ENCODER_TMRA, ENCODER_BASE);
    TMRA_Start(ENCODER_TMRA);
}

/* 读取编码器增量：用上次计数值做 16 位差值（自动处理回绕），不写回计数器 */
static int16_t Encoder_ReadValue(void)
{
    static uint16_t s_u16Last = 0U;
    static uint8_t  s_u8Init  = 0U;
    uint16_t u16Now;
    int16_t  s16Diff;

    u16Now = (uint16_t)TMRA_GetCountValue(ENCODER_TMRA);

    if (0U == s_u8Init) {
        s_u16Last = u16Now;
        s_u8Init  = 1U;
        return 0;
    }

    s16Diff   = (int16_t)(u16Now - s_u16Last);
    s_u16Last = u16Now;
    return s16Diff;
}

/* 净累计去抖：正反抖动互相抵消，累计净变化超过阈值才确认方向 */
static int8_t Encoder_GetDirection(void)
{
    static int16_t s16Accum = 0;

    s16Accum += Encoder_ReadValue();

    if (s16Accum >= (int16_t)ENCODER_THRESHOLD) {
        s16Accum = 0;
        return 1;
    }
    if (s16Accum <= -(int16_t)ENCODER_THRESHOLD) {
        s16Accum = 0;
        return -1;
    }
    return 0;
}

extern uint8_t EncoderNum;
void Encoder_Foreward_CallBack(void);
void Encoder_Reverse_CallBack(void);

/* 应用层：正转/反转触发回调 */
void Encoder_Handler(void)
{
    int8_t i8Dir = Encoder_GetDirection();
    if (i8Dir > 0) {
        Encoder_Foreward_CallBack();
    } else if (i8Dir < 0) {
        Encoder_Reverse_CallBack();
    }
}

void Encoder_Foreward_CallBack(void)
{
    EncoderNum = 1; /* 正转 -> 上一个菜单 */
    Beeper_Perform(BEEPER_KEYPRESS);
}

void Encoder_Reverse_CallBack(void)
{
    EncoderNum = 2; /* 反转 -> 下一个菜单 */
    Beeper_Perform(BEEPER_KEYPRESS);
}
