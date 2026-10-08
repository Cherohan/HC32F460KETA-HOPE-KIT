#include "sys.h"
#include "usart.h"
#include "hc32_ll_usart.h"
#include "hc32_ll_fcg.h"

/* ================= printf 重定向（AC5，不使用 MicroLIB） ================= */
#if 1
#pragma import(__use_no_semihosting)
struct __FILE { int handle; };
FILE __stdout;

void _sys_exit(int x) { x = x; }

uint8_t fputc_select_flag = 0;

int fputc(int ch, FILE *f)
{
    (void)f;
    if (fputc_select_flag == 0U) {
        while (USART_GetStatus(CM_USART1, USART_FLAG_TX_EMPTY) == RESET) {}
        USART_WriteData(CM_USART1, (uint16_t)ch);
    } else {
        while (USART_GetStatus(CM_USART3, USART_FLAG_TX_EMPTY) == RESET) {}
        USART_WriteData(CM_USART3, (uint16_t)ch);
    }
    return ch;
}
#endif

uint8_t  USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA = 0U;

uint8_t  USART3_RX_BUF[32];
uint16_t USART3_RX_STA = 0U;

/* ATK 0x0d 0x0a 协议接收状态机 */
static void USART_RxStaHandle(uint8_t res, uint16_t *pSta, uint8_t *pBuf)
{
    if ((*pSta & 0x8000U) == 0U) {
        if ((*pSta & 0x4000U) != 0U) {
            if (res != 0x0aU) { *pSta = 0U; }
            else              { *pSta |= 0x8000U; }
        } else {
            if (res == 0x0dU) { *pSta |= 0x4000U; }
            else {
                pBuf[*pSta & 0x3FFFU] = res;
                (*pSta)++;
                if (*pSta > (USART_REC_LEN - 1U)) { *pSta = 0U; }
            }
        }
    }
}

static void USART1_RxIrqCallback(void)
{
    if (USART_GetStatus(CM_USART1, USART_FLAG_RX_FULL) == SET) {
        USART_RxStaHandle((uint8_t)USART_ReadData(CM_USART1), &USART_RX_STA, USART_RX_BUF);
    }
}

static void USART3_RxIrqCallback(void)
{
    if (USART_GetStatus(CM_USART3, USART_FLAG_RX_FULL) == SET) {
        USART_RxStaHandle((uint8_t)USART_ReadData(CM_USART3), &USART3_RX_STA, USART3_RX_BUF);
    }
}

static void USART_Config(CM_USART_TypeDef *USARTx, uint32_t u32Baudrate)
{
    stc_usart_uart_init_t stcUartInit;
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc     = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv     = USART_CLK_DIV1;
    stcUartInit.u32Baudrate     = u32Baudrate;
    stcUartInit.u32DataWidth    = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit      = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity       = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    (void)USART_UART_Init(USARTx, &stcUartInit, NULL);
    USART_FuncCmd(USARTx, (USART_TX | USART_RX), ENABLE);
}

static void USART_RxIrqEnable(CM_USART_TypeDef *USARTx, IRQn_Type enIRQn,
                              en_int_src_t enIntSrc, func_ptr_t pfnCallback)
{
    stc_irq_signin_config_t stcIrqSign;
    stcIrqSign.enIRQn      = enIRQn;
    stcIrqSign.enIntSrc    = enIntSrc;
    stcIrqSign.pfnCallback = pfnCallback;
    (void)INTC_IrqSignIn(&stcIrqSign);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_SetPriority(enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(enIRQn);
    USART_FuncCmd(USARTx, USART_INT_RX, ENABLE);
}

void uart_init(uint32_t bound)
{
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_10, GPIO_FUNC_32);  /* USART1_TX */
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_12, GPIO_FUNC_33);  /* USART1_RX */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART1, ENABLE);
    USART_DeInit(CM_USART1);
    USART_Config(CM_USART1, bound);
#if EN_USART1_RX
    USART_RxIrqEnable(CM_USART1, INT004_IRQn, INT_SRC_USART1_RI, &USART1_RxIrqCallback);
#endif
}

void USART3_Init(uint32_t bound)
{
    GPIO_SetFunc(GPIO_PORT_B, GPIO_PIN_03, GPIO_FUNC_32);  /* USART3_TX */
    GPIO_SetFunc(GPIO_PORT_B, GPIO_PIN_06, GPIO_FUNC_33);  /* USART3_RX */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART3, ENABLE);
    USART_DeInit(CM_USART3);
    USART_Config(CM_USART3, bound);
    USART_RxIrqEnable(CM_USART3, INT005_IRQn, INT_SRC_USART3_RI, &USART3_RxIrqCallback);
}

void USART1_SendChar(uint8_t data)
{
    while (USART_GetStatus(CM_USART1, USART_FLAG_TX_EMPTY) == RESET) {}
    USART_WriteData(CM_USART1, (uint16_t)data);
}

void USART1_SendBuf(uint8_t *data)
{
    while (*data != '\0') { USART1_SendChar(*data++); }
}

void USART3_SendChar(uint8_t data)
{
    while (USART_GetStatus(CM_USART3, USART_FLAG_TX_EMPTY) == RESET) {}
    USART_WriteData(CM_USART3, (uint16_t)data);
}

void USART3_SendBuf(uint8_t *data)
{
    while (*data != '\0') { USART3_SendChar(*data++); }
}
