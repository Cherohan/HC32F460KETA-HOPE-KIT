/**
 *******************************************************************************
 * @file  usb_bsp.c
 * @brief BSP function for USB example (adapted for HOPE-CheroVersion)
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_bsp.h"
#include "hc32_ll.h"
#include "usb_dev_int.h"

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE               (LL_PERIPH_GPIO | LL_PERIPH_FCG | \
                                         LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)

#define USB_DP_PORT                     (GPIO_PORT_A)
#define USB_DP_PIN                      (GPIO_PIN_12)
#define USB_DM_PORT                     (GPIO_PORT_A)
#define USB_DM_PIN                      (GPIO_PIN_11)
#define USB_VBUS_PORT                   (GPIO_PORT_A)
#define USB_VBUS_PIN                    (GPIO_PIN_09)
#define USB_SOF_PORT                    (GPIO_PORT_A)
#define USB_SOF_PIN                     (GPIO_PIN_08)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
extern usb_core_instance usb_dev;

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  handle the USB interrupt
 * @param  None
 * @retval None
 */
static void USB_IRQ_Handler(void)
{
    usb_isr_handler(&usb_dev);
}

/**
 * @brief  USB clock initial (24MHz XTAL -> 48MHz USB clock via UPLL)
 * @param  None
 * @retval None
 */
static void UsbClockInit(void)
{
    stc_clock_pllx_init_t stcUpllInit;

    (void)CLK_PLLxStructInit(&stcUpllInit);
    stcUpllInit.u8PLLState = CLK_PLLX_ON;
    stcUpllInit.PLLCFGR = 0UL;
    stcUpllInit.PLLCFGR_f.PLLM = (6UL  - 1UL);  /* /6 : 24MHz -> 4MHz */
    stcUpllInit.PLLCFGR_f.PLLN = (84UL - 1UL);  /* *84 : VCO 336MHz    */
    stcUpllInit.PLLCFGR_f.PLLR = (7UL  - 1UL);
    stcUpllInit.PLLCFGR_f.PLLQ = (7UL  - 1UL);
    stcUpllInit.PLLCFGR_f.PLLP = (7UL  - 1UL);  /* /7 : 48MHz          */
    (void)CLK_PLLxInit(&stcUpllInit);

    /* Set USB clock source */
    CLK_SetUSBClockSrc(CLK_USBCLK_PLLXP);
}

/**
 * @brief  initialize configurations for the BSP
 * @param  [in] pdev                device instance
 * @param  [in] pstcPortIdentify    usb core and phy select
 * @retval None
 */
void usb_bsp_init(usb_core_instance *pdev, stc_usb_port_identify *pstcPortIdentify)
{
    stc_gpio_init_t stcGpioCfg;

    /* Unlock peripherals or registers */
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);

    /* USB clock source configure */
    UsbClockInit();

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(USB_DM_PORT, USB_DM_PIN, &stcGpioCfg);
    (void)GPIO_Init(USB_DP_PORT, USB_DP_PIN, &stcGpioCfg);
    /* 不使用 VBUS 检测：PA9 保留给 OLED SCL */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USBFS, ENABLE);
    /* 无物理 VBUS：强制 VBUS 有效(VBUSOVEN+VBUSVAL)，否则 USB 设备无法被主机枚举 */
    SET_REG32_BIT(CM_USBFS->GVBUSCFG, (USBFS_GVBUSCFG_VBUSOVEN | USBFS_GVBUSCFG_VBUSVAL));
}

/**
 * @brief  configure the NVIC of USB
 * @param  [in] pdev                    device instance
 * @retval None
 */
void usb_bsp_nvicconfig(usb_core_instance *pdev)
{
    stc_irq_signin_config_t stcIrqRegiConf;
    /* 软复位(usb_initusbcore)会清除 GVBUSCFG，必须在此重新强制 VBUS 有效，
       否则设备处于 session invalid 状态，无法接收 SOF/SETUP 数据包 */
    SET_REG32_BIT(CM_USBFS->GVBUSCFG, (USBFS_GVBUSCFG_VBUSOVEN | USBFS_GVBUSCFG_VBUSVAL));
    /* Register INT_SRC_USBFS_GLB Int to Vect.No.030 */
    stcIrqRegiConf.enIRQn = INT030_IRQn;
    /* Select interrupt function */
    stcIrqRegiConf.enIntSrc = INT_SRC_USBFS_GLB;
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &USB_IRQ_Handler;
    /* Registration IRQ */
    (void)INTC_IrqSignIn(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIO_15);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
}

/**
 * @brief  This function provides delay time in micro sec
 * @param  [in] usec         Value of delay required in micro sec
 * @retval None
 */
void usb_udelay(const uint32_t usec)
{
    __IO uint32_t i;
    uint32_t j;
    j = (HCLK_VALUE + 1000000UL - 1UL) / 1000000UL * usec;
    for (i = 0UL; i < j; i++) {
    }
}

/**
 * @brief  This function provides delay time in milli sec
 * @param  [in] msec         Value of delay required in milli sec
 * @retval None
 */
void usb_mdelay(const uint32_t msec)
{
    usb_udelay(msec * 1000UL);
}
