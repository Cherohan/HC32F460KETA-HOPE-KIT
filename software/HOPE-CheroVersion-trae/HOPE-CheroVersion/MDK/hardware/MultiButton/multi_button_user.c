#include "multi_button_user.h"
#include "multi_button.h"
#include "sys.h"
#include "usart.h"
#include "BEEPER.h"

#define ENCODER_MODE 1

struct Button key0;
struct Button key1;
struct Button key2;

extern uint8_t KeyNum;
extern uint8_t EncoderNum;
uint8_t EncoderMode_flag = 1;

void user_keyBSP_init(void)
{
    stc_gpio_init_t stcGpioInit;

#if ENCODER_MODE == 0
    /* key0=PC0, key1=PC1（此分支当前未使用） */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_00, &stcGpioInit);
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_01, &stcGpioInit);

    button_init(&key0, read_key0_gpio, 0, 0);
    button_init(&key1, read_key1_gpio, 0, 1);

    button_attach(&key0, PRESS_DOWN, key0_press_down_Handler);
    button_attach(&key0, PRESS_UP, key0_press_up_Handler);
    button_attach(&key0, LONG_PRESS_START, key0_long_press_start_Handler);
    button_attach(&key0, SINGLE_CLICK, key0_single_click_Handler);

    button_attach(&key1, PRESS_DOWN, key1_press_down_Handler);
    button_attach(&key1, PRESS_UP, key1_press_up_Handler);
    button_attach(&key1, LONG_PRESS_START, key1_long_press_start_Handler);
    button_attach(&key1, SINGLE_CLICK, key1_single_click_Handler);

    button_start(&key0);
    button_start(&key1);
#else
    /* key2 = PC8（BUT1，ECK 最终固定此引脚），上拉输入 */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_08, &stcGpioInit);

    button_init(&key2, read_key2_gpio, 0, 0);
    button_attach(&key2, PRESS_DOWN, key2_press_down_Handler);
    button_attach(&key2, PRESS_UP, key2_press_up_Handler);
    button_attach(&key2, LONG_PRESS_START, key2_long_press_start_Handler);
    button_attach(&key2, SINGLE_CLICK, key2_single_click_Handler);
    button_start(&key2);
#endif
}

uint8_t read_key0_gpio(uint8_t button_id)
{
    (void)button_id;
    return (GPIO_ReadInputPins(GPIO_PORT_C, GPIO_PIN_00) == PIN_SET) ? 1U : 0U;
}

uint8_t read_key1_gpio(uint8_t button_id)
{
    (void)button_id;
    return (GPIO_ReadInputPins(GPIO_PORT_C, GPIO_PIN_01) == PIN_SET) ? 1U : 0U;
}

uint8_t read_key2_gpio(uint8_t button_id)
{
    (void)button_id;
    return (GPIO_ReadInputPins(GPIO_PORT_C, GPIO_PIN_08) == PIN_SET) ? 1U : 0U;
}

void key0_press_down_Handler(void *btn)  { (void)btn; printf("---> key0 press down! <---\r\n"); Beeper_Perform(BEEPER_KEYPRESS); }
void key0_press_up_Handler(void *btn)    { (void)btn; printf("***> key0 press up! <***\r\n"); }
void key0_single_click_Handler(void *btn){ (void)btn; EncoderNum = 1; }
void key0_long_press_start_Handler(void *btn){ (void)btn; KeyNum = 1; Beeper_Perform(BEEPER_TRITONE); }

void key1_press_down_Handler(void *btn)  { (void)btn; printf("---> key1 press down! <---\r\n"); Beeper_Perform(BEEPER_KEYPRESS); }
void key1_press_up_Handler(void *btn)    { (void)btn; printf("***> key1 press up! <***\r\n"); }
void key1_single_click_Handler(void *btn){ (void)btn; EncoderNum = 2; }
void key1_long_press_start_Handler(void *btn){ (void)btn; KeyNum = 2; Beeper_Perform(BEEPER_WARNING); }

void key2_press_down_Handler(void *btn)       { (void)btn; Beeper_Perform(BEEPER_KEYPRESS); }
void key2_press_up_Handler(void *btn)         { (void)btn; }
void key2_single_click_Handler(void *btn)     { (void)btn; KeyNum = 1; Beeper_Perform(BEEPER_TRITONE); }
void key2_long_press_start_Handler(void *btn) { (void)btn; KeyNum = 2; Beeper_Perform(BEEPER_WARNING); }
