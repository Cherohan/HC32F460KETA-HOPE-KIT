#ifndef __SYS_H
#define __SYS_H
#include "hc32_ll.h"

/* 位带操作（HC32F460 GPIO 基址 0x40053800 位于位带区） */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)        *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

/* HC32F460 GPIO 输入/输出数据寄存器偏移（CM_GPIO_BASE = 0x40053800） */
#define GPIOA_IDR_Addr (CM_GPIO_BASE + 0x00U)
#define GPIOA_ODR_Addr (CM_GPIO_BASE + 0x04U)
#define GPIOB_IDR_Addr (CM_GPIO_BASE + 0x10U)
#define GPIOB_ODR_Addr (CM_GPIO_BASE + 0x14U)
#define GPIOC_IDR_Addr (CM_GPIO_BASE + 0x20U)
#define GPIOC_ODR_Addr (CM_GPIO_BASE + 0x24U)
#define GPIOD_IDR_Addr (CM_GPIO_BASE + 0x30U)
#define GPIOD_ODR_Addr (CM_GPIO_BASE + 0x34U)
#define GPIOE_IDR_Addr (CM_GPIO_BASE + 0x40U)
#define GPIOE_ODR_Addr (CM_GPIO_BASE + 0x44U)

#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n)
#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n)
#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n)
#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n)
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n)
#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n)
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n)
#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n)
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n)

#define CHECK_FLAG(flag) (*(flag) ? (*(flag) = 0, 1) : 0)

#endif
