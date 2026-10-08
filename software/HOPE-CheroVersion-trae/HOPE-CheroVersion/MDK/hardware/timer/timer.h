#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

extern uint32_t Time_ms;
extern uint8_t  NET_flag;

void Timer_Init(void);

uint32_t RandomCreate(void);

#endif
