#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"

#define USART_REC_LEN 200
#define EN_USART1_RX  1

extern uint8_t fputc_select_flag;   /* 0->USART1(调试) / 1->USART3(蓝牙) */

extern uint8_t  USART_RX_BUF[USART_REC_LEN];
extern uint16_t USART_RX_STA;

extern uint8_t  USART3_RX_BUF[32];
extern uint16_t USART3_RX_STA;

void uart_init(uint32_t bound);              /* USART1: PC10-TX / PC12-RX */
void USART1_SendChar(uint8_t data);
void USART1_SendBuf(uint8_t *data);

void USART3_Init(uint32_t bound);            /* USART3: PB3-TX / PB6-RX */
void USART3_SendChar(uint8_t data);
void USART3_SendBuf(uint8_t *data);

#endif
