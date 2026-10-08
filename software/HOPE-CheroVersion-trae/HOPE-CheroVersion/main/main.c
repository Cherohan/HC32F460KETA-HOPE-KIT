/**
 *******************************************************************************
 * @file  main.c
 * @brief Main program.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2026-09-21       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
/* USER CODE BEGIN Header */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "WS2812.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* Configures TimerA. */
static void App_TimerACfg(void);
/* Configures USARTx. */
static void App_USARTxCfg(void);
/* Configures SPIx. */
static void App_SPIxCfg(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

//Clock Config
static void App_ClkCfg(void)
{
    /* Set bus clock div. */
    CLK_SetClockDiv(CLK_BUS_CLK_ALL, (CLK_HCLK_DIV1 | CLK_EXCLK_DIV2 | CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | \
                                   CLK_PCLK2_DIV4 | CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2));
    /* sram init include read/write wait cycle setting */
    SRAM_SetWaitCycle(SRAM_SRAM_ALL, SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);
    /* flash read wait cycle setting */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE5);
    /* XTAL config */
    stc_clock_xtal_init_t stcXtalInit;
    (void)CLK_XtalStructInit(&stcXtalInit);
    stcXtalInit.u8State = CLK_XTAL_ON;
    stcXtalInit.u8Drv = CLK_XTAL_DRV_HIGH;
    stcXtalInit.u8Mode = CLK_XTAL_MD_OSC;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);
    /* MPLL config */
    stc_clock_pll_init_t stcMPLLInit;
    (void)CLK_PLLStructInit(&stcMPLLInit);
    stcMPLLInit.PLLCFGR = 0UL;
    stcMPLLInit.PLLCFGR_f.PLLM = (3UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLN = (50UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLP = (2UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLQ = (2UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLR = (2UL - 1UL);
    stcMPLLInit.u8PLLState = CLK_PLL_ON;
    stcMPLLInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
    (void)CLK_PLLInit(&stcMPLLInit);
    /* 3 cycles for 126MHz ~ 200MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT3);
    /* Switch driver ability */
    PWC_HighSpeedToHighPerformance();
    /* Set the system clock source */
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);
}

//Port Config
static void App_PortCfg(void)
{
    /* GPIO initialize */
    stc_gpio_init_t stcGpioInit;
    /* PH1 set to XTAL-IN */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_H, GPIO_PIN_01, &stcGpioInit);

    /* PH0 set to XTAL-EXT/XTAL-OUT */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_H, GPIO_PIN_00, &stcGpioInit);

    /* PC2 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_02, &stcGpioInit);

    /* PC3 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_03, &stcGpioInit);

    /* PA15 set to GPIO-Input */
    GPIO_SetDebugPort(GPIO_PIN_TDI, DISABLE);
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_15, &stcGpioInit);

    /* PA10 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_10, &stcGpioInit);

    /* PA9 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_09, &stcGpioInit);

    /* PA8 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_08, &stcGpioInit);

    /* PC9 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_09, &stcGpioInit);

    /* PC8 set to GPIO-Input */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_08, &stcGpioInit);

    /* PC7 set to GPIO-Input */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_07, &stcGpioInit);

    /* PA3 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_03, &stcGpioInit);

    /* PA5 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_05, &stcGpioInit);

    /* PA4 set to GPIO-Input */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_04, &stcGpioInit);

    /* PC4 set to GPIO-Input */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_04, &stcGpioInit);

    /* PC5 set to GPIO-Input */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_05, &stcGpioInit);

    GPIO_SetFunc(GPIO_PORT_C,GPIO_PIN_01,GPIO_FUNC_42);//SPI1-SS0
    
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_00,GPIO_FUNC_40);//SPI1-MOSI
    
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_01,GPIO_FUNC_41);//SPI1-MISO
    
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_02,GPIO_FUNC_43);//SPI1-SCK
    
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_06,GPIO_FUNC_5);//TIMA-3-PWM1/TIMA-3-CLKA
    
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_07,GPIO_FUNC_5);//TIMA-3-PWM2/TIMA-3-CLKB
    
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_11,GPIO_FUNC_10);//USBFS-DM
    
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_12,GPIO_FUNC_10);//USBFS-DP
    
    GPIO_SetFunc(GPIO_PORT_C,GPIO_PIN_10,GPIO_FUNC_32);//USART1-TX
    
    GPIO_SetFunc(GPIO_PORT_C,GPIO_PIN_12,GPIO_FUNC_33);//USART1-RX
    
    GPIO_SetDebugPort(GPIO_PIN_SWO, DISABLE);
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_03,GPIO_FUNC_32);//USART3-TX
    
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_06,GPIO_FUNC_33);//USART3-RX
    
}


/**
 * @brief  Main function of the project
 * @param  None
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* USER CODE BEGIN Main0 */

    /* USER CODE END Main0 */
    /* Register write unprotected for some required peripherals. */
    LL_PERIPH_WE(LL_PERIPH_ALL);
    //Clock Config
    App_ClkCfg();
    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */
    //Port Config
    App_PortCfg();
    //TimerA Config
    App_TimerACfg();
    //USARTx Config
    App_USARTxCfg();
    //SPIx Config
    App_SPIxCfg();
    /* Register write protected for some required peripherals. */
    LL_PERIPH_WP(LL_PERIPH_ALL);
    /* USER CODE BEGIN Main1 */

    /* USER CODE END Main1 */
    for (;;) 
    {
        /* USER CODE BEGIN FOR */
		WS2812_Test();
        /* USER CODE END FOR */
    }
}


//TimerA Config
static void App_TimerACfg(void)
{
    /* USER CODE BEGIN TimerA 0 */

    /* USER CODE END TimerA 0 */
    stc_tmra_init_t stcTmraInit;

    /* Enable TMRA_3 peripheral clock */
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_3, ENABLE);

    /************************* Configure TMRA_3 counter *************************/
    (void)TMRA_StructInit(&stcTmraInit);
    /* Config hardware count */
    stcTmraInit.u8CountSrc = TMRA_CNT_SRC_HW;
    stcTmraInit.hw_count.u16CountUpCond = TMRA_CNT_UP_COND_CLKB_LOW_CLKA_RISING;
    stcTmraInit.hw_count.u16CountDownCond = TMRA_CNT_DOWN_COND_CLKB_HIGH_CLKA_RISING;
    stcTmraInit.u32PeriodValue = 0xFFFFU;
    (void)TMRA_Init(CM_TMRA_3, &stcTmraInit);

    /* Start timerA */
    TMRA_Start(CM_TMRA_3);

    /* USER CODE BEGIN TimerA 1 */

    /* USER CODE END TimerA 1 */
}

//USARTx Config
static void App_USARTxCfg(void)
{
    /* USER CODE BEGIN USARTx 0 */

    /* USER CODE END USARTx 0 */
    stc_usart_uart_init_t stcUartInit;

    /* Enable USART1 clock */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART1, ENABLE);
    /************************* Configure USART1***************************/
    USART_DeInit(CM_USART1);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 9600UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_RTS;
    USART_UART_Init(CM_USART1, &stcUartInit, NULL);
    /* Enable USART_TX | USART_RX function */
    USART_FuncCmd(CM_USART1, (USART_TX | USART_RX), ENABLE);

    /* Enable USART3 clock */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART3, ENABLE);
    /************************* Configure USART3***************************/
    USART_DeInit(CM_USART3);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 9600UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_RTS;
    USART_UART_Init(CM_USART3, &stcUartInit, NULL);
    /* Enable USART_TX | USART_RX function */
    USART_FuncCmd(CM_USART3, (USART_TX | USART_RX), ENABLE);
    /* USER CODE BEGIN USARTx 1 */

    /* USER CODE END USARTx 1 */
}

//SPIx Config
static void App_SPIxCfg(void)
{
    /* USER CODE BEGIN SPIx 0 */

    /* USER CODE END SPIx 0 */
    stc_spi_init_t stcSpiInit;
    stc_spi_delay_t stcSpiDelay;

    /* Enable SPI1 clock */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_SPI1, ENABLE);
    /************************* Configure SPI1***************************/
    SPI_StructInit(&stcSpiInit);
    stcSpiInit.u32WireMode = SPI_4_WIRE;
    stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave = SPI_MASTER;
    stcSpiInit.u32Parity = SPI_PARITY_INVD;
    stcSpiInit.u32SpiMode = SPI_MD_0;
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV128;
    stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
    stcSpiInit.u32SuspendMode = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32FrameLevel = SPI_1_FRAME;
    (void)SPI_Init(CM_SPI1, &stcSpiInit);

    SPI_DelayStructInit(&stcSpiDelay);
    stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_1SCK;
    stcSpiDelay.u32ReleaseDelay = SPI_RELEASE_TIME_1SCK;
    stcSpiDelay.u32SetupDelay = SPI_SETUP_TIME_1SCK;
    (void)SPI_DelayTimeConfig(CM_SPI1, &stcSpiDelay);

    /* SPI loopback function configuration */
    SPI_SetLoopbackMode(CM_SPI1, SPI_LOOPBACK_INVD);
    /* SPI parity check error self diagnosis configuration */
    SPI_ParityCheckCmd(CM_SPI1, DISABLE);
    /* SPI valid SS signal configuration */
    SPI_SSPinSelect(CM_SPI1, SPI_PIN_SS0);
    /* SPI SS signal valid level configuration */
    SPI_SetSSValidLevel(CM_SPI1, SPI_PIN_SS0, SPI_SS_VALID_LVL_LOW);
    /* Enable SPI1 */
    SPI_Cmd(CM_SPI1, ENABLE);
    /* USER CODE BEGIN SPIx 1 */

    /* USER CODE END SPIx 1 */
}

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
