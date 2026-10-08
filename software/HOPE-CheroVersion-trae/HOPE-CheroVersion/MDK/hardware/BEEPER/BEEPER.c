#include "BEEPER.h"
#include "hc32_ll_clk.h"
#include "hc32_ll_fcg.h"
#include "hc32_ll_tmra.h"

/* 蜂鸣器定时器：TMRA_2 PWM1，PA5 */
#define BEEPER_TMRA        (CM_TMRA_2)
#define BEEPER_FCG         (FCG2_PERIPH_TMRA_2)
#define BEEPER_CH          (TMRA_CH1)
#define BEEPER_PORT        (GPIO_PORT_A)
#define BEEPER_PIN         (GPIO_PIN_05)
#define BEEPER_PIN_FUNC    (GPIO_FUNC_4)
#define BEEPER_CLK_DIV     (TMRA_CLK_DIV8)

/* 计数时钟频率（初始化时由 PCLK1 分频后计算得到，TIMERA 时钟源为 PCLK1） */
static uint32_t s_u32BeepCntFreq = 1000000UL;

/*创建Beeper的handle*/
BEEPER_Tag Beeper0;

/*C4-B8的音调对应的频率大小*/
const uint16_t MusicNoteFrequency[] = {
    0,      /* rest_note */
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,   /* C4~B4 */
    523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988,   /* C5~B5 */
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, /* C6~B6 */
    2093, 2218, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, /* C7~B7 */
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, /* C8~B8 */
    0,      /* check_note */
};

/*全局TONE结构体指针，用于定时器中断函数中*/
TONE *MySound;

/*乐曲*/
TONE const BEEPER_KEYPRESS[] = {
    {NOTE_C6, 7},
    {CHECK_NOTE, 0},
};

TONE const BEEPER_TRITONE[] = {
    {NOTE_B5, 7},
    {REST_NOTE, 2},
    {NOTE_D6, 6},
    {REST_NOTE, 2},
    {NOTE_F6, 6},
    {CHECK_NOTE, 0},
};

TONE const BEEPER_WARNING[] = {
    {NOTE_F4, 5},
    {REST_NOTE, 2},
    {NOTE_F4, 5},
    {CHECK_NOTE, 0},
};

TONE const BEEP1[] = {
    {NOTE_C5, 11}, {REST_NOTE, 2}, {NOTE_C5, 11}, {REST_NOTE, 2},
    {NOTE_G5, 11}, {REST_NOTE, 2}, {NOTE_G5, 11}, {REST_NOTE, 2},
    {NOTE_A5, 11}, {REST_NOTE, 2}, {NOTE_A5, 11}, {REST_NOTE, 2},
    {NOTE_G5, 22}, {REST_NOTE, 2},
    {NOTE_F6, 11}, {REST_NOTE, 2}, {NOTE_F6, 11}, {REST_NOTE, 2},
    {NOTE_E7, 11}, {REST_NOTE, 2}, {NOTE_E7, 11}, {REST_NOTE, 2},
    {NOTE_D8, 11}, {REST_NOTE, 2}, {NOTE_D8, 11}, {REST_NOTE, 2},
    {NOTE_C5, 11},
    {CHECK_NOTE, 0},
};

TONE const BEEP2[] = {
    {REST_NOTE, 20}, {REST_NOTE, 20}, {REST_NOTE, 20},
    {NOTE_C5, 10}, {NOTE_B4, 10},
    {NOTE_A4, 20}, {NOTE_E5, 40}, {NOTE_C5, 10}, {NOTE_A4, 10},
    {NOTE_B4, 20}, {NOTE_F5, 20}, {NOTE_E5, 10}, {NOTE_D5, 30},
    {NOTE_C5, 10}, {NOTE_D5, 10}, {NOTE_C5, 10}, {NOTE_D5, 10},
    {NOTE_E5, 20}, {NOTE_C5, 10}, {NOTE_B4, 10},
    {NOTE_A4, 20}, {NOTE_D5, 20}, {NOTE_C5, 10}, {NOTE_B4, 10},
    {REST_NOTE, 10}, {NOTE_A4, 5}, {NOTE_B4, 5},
    {NOTE_C5, 20}, {NOTE_A4, 20}, {NOTE_E5, 20}, {NOTE_C5, 20},
    {NOTE_D5, 20}, {NOTE_A5, 20}, {NOTE_G5, 20},
    {NOTE_F5, 10}, {NOTE_E5, 5}, {NOTE_D5, 5},
    {NOTE_E5, 80},
    {CHECK_NOTE, 0},
};

void Beeper_PWM_Init(uint16_t arr)
{
    stc_tmra_init_t stcTmraInit;
    stc_tmra_pwm_init_t stcPwmInit;

    /* 使能 TMRA_2 时钟 */
    FCG_Fcg2PeriphClockCmd(BEEPER_FCG, ENABLE);

    /* 初始化 TMRA_2 计数（软件计数，锯齿波，向上） */
    (void)TMRA_StructInit(&stcTmraInit);
    stcTmraInit.u8CountSrc = TMRA_CNT_SRC_SW;
    stcTmraInit.sw_count.u8ClockDiv = BEEPER_CLK_DIV;
    stcTmraInit.sw_count.u8CountMode = TMRA_MD_SAWTOOTH;
    stcTmraInit.sw_count.u8CountDir = TMRA_DIR_UP;
    stcTmraInit.u32PeriodValue = (uint32_t)(arr - 1U);
    (void)TMRA_Init(BEEPER_TMRA, &stcTmraInit);

    /* 初始化 PWM 通道 */
    (void)TMRA_PWM_StructInit(&stcPwmInit);
    stcPwmInit.u32CompareValue = (uint32_t)(arr / 2U);  /* 占空比 50% */
    stcPwmInit.u16StartPolarity = TMRA_PWM_HIGH;
    stcPwmInit.u16StopPolarity = TMRA_PWM_LOW;
    stcPwmInit.u16CompareMatchPolarity = TMRA_PWM_INVT;
    stcPwmInit.u16PeriodMatchPolarity = TMRA_PWM_INVT;
    GPIO_SetFunc(BEEPER_PORT, BEEPER_PIN, BEEPER_PIN_FUNC);
    (void)TMRA_PWM_Init(BEEPER_TMRA, BEEPER_CH, &stcPwmInit);
    TMRA_PWM_OutputCmd(BEEPER_TMRA, BEEPER_CH, ENABLE);

    /* 先停止，防止蜂鸣器怪叫 */
    TMRA_Stop(BEEPER_TMRA);
}

void Beeper_Init(void)
{
    stc_clock_freq_t stcFreq;

    /* 读取 PCLK1 频率并计算计数时钟频率（TIMERA 时钟源为 PCLK1） */
    (void)CLK_GetClockFreq(&stcFreq);
    s_u32BeepCntFreq = stcFreq.u32Pclk1Freq / 8U;  /* BEEPER_CLK_DIV8，计数频率 = PCLK1/8 */

    /* 初始化定时器 PWM */
    Beeper_PWM_Init(1000);

    /* BEEPER 使能标志位 */
    Beeper0.Beeper_Enable = 1;
    Beeper0.Beeper_Continue_Flag = 0;
    Beeper0.Sound_Loud = 20;
}

/* 计算周期值（计数频率 / 音调频率） */
uint16_t Set_Musical_Note(uint16_t frq)
{
    if (frq == 0U) {
        return 0U;
    }
    return (uint16_t)(s_u32BeepCntFreq / (uint32_t)frq);
}

void Beeper_Set_Musical_Tone(uint16_t frq)
{
    if (frq == 0U) {
        TMRA_Stop(BEEPER_TMRA);
        return;
    }
    TMRA_Start(BEEPER_TMRA);
    TMRA_SetPeriodValue(BEEPER_TMRA, Set_Musical_Note(frq));
}

/* 应用层：播放一段乐曲 */
void Beeper_Perform(const TONE *Sound)
{
    uint16_t Note_Length;

    TMRA_Stop(BEEPER_TMRA);

    MySound = (TONE *)Sound;

    /* 通过检查位 CHECK_NOTE 计算乐曲长度 */
    for (Note_Length = 0; MySound[Note_Length].Note != CHECK_NOTE; Note_Length++) {
        ;
    }

    Beeper0.Sound_Size = Note_Length;
    Beeper0.Beep_Play_Schedule = 0;

    Beeper0.Beeper_Continue_Flag = 1;
    Beeper0.Beeper_Count = 0;
}

/* 用于 10ms 定时器中断进行循环 */
void Beeper_Proc(void)
{
    uint16_t u16Period;

    if (Beeper0.Beeper_Continue_Flag && Beeper0.Beeper_Enable) {
        if (Beeper0.Beep_Play_Schedule <= Beeper0.Sound_Size) {
            Beeper0.Beeper_Count--;
            /* count 溢出到 65535 表示当前音符延时结束 */
            if (!(Beeper0.Beeper_Count < 65535U)) {
                u16Period = Set_Musical_Note(
                    MusicNoteFrequency[MySound[Beeper0.Beep_Play_Schedule].Note]);
                TMRA_SetPeriodValue(BEEPER_TMRA, (uint32_t)u16Period);
                /* 占空比 = 周期 * 响度 / 100，控制音量 */
                TMRA_SetCompareValue(BEEPER_TMRA, BEEPER_CH,
                    (uint32_t)((uint32_t)u16Period * Beeper0.Sound_Loud / 100U));
                Beeper0.Beeper_Count = MySound[Beeper0.Beep_Play_Schedule].Delay;
                Beeper0.Beep_Play_Schedule++;
                TMRA_Start(BEEPER_TMRA);
            }
        } else {
            /* 播放完毕 */
            TMRA_Stop(BEEPER_TMRA);
            Beeper0.Beeper_Continue_Flag = 0;
        }
    } else {
        TMRA_Stop(BEEPER_TMRA);
        Beeper0.Beeper_Continue_Flag = 0;
    }
}
