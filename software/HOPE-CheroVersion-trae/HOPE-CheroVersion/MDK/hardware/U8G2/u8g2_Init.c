#include "hc32_ll.h"
#include "u8g2.h"
#include "u8g2_Init.h"
#include "SW_I2C_OLED.h"
#include "delay.h"

/* ssd1312 128*64 库，软件 I2C 接到目标 OLED 的 PA9(SCL)/PA10(SDA) */

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
        __NOP();
        break;
    case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
        for (uint16_t n = 0; n < 320; n++)
        {
            __NOP();
        }
        break;
    case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
        delay_ms(1);
        break;
    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        delay_us(5);
        break;
    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: low, arg_int=1: high
        OLED_SW_I2C_W_SCL(arg_int);
        break;
    case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: low, arg_int=1: high
        OLED_SW_I2C_W_SDA(arg_int);
        break;
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
        break;

    case U8X8_MSG_BYTE_INIT:
        /* i2c 已由 OLED_SW_I2C_Init() 初始化 */
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:
        if (buf_idx <= 0)
            break;

        OLED_SW_I2C_Start();
        OLED_SW_I2C_SendByte(u8x8_GetI2CAddress(u8x8)); // 0x78
        for (uint8_t i = 0; i < buf_idx; i++)
        {
            OLED_SW_I2C_SendByte(buffer[i]);
        }
        OLED_SW_I2C_Stop();
        break;

    default:
        return 0;
    }
    return 1;
}

void u8g2Init(u8g2_t *u8g2)
{
    u8g2_Setup_ssd1312_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay); // 初始化u8g2结构体
    u8g2_InitDisplay(u8g2);                                                                       // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
    u8g2_SetPowerSave(u8g2, 0);                                                                   // 打开显示器
    u8g2_ClearBuffer(u8g2);
}
