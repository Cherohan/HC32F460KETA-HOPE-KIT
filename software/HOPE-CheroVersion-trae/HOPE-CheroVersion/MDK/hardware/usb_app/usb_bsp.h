/**
 *******************************************************************************
 * @file  usb_bsp.h
 * @brief Head file for usb_bsp.c
 *******************************************************************************
 */
#ifndef __USB_BSP_H__
#define __USB_BSP_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_lib.h"

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
void usb_bsp_init(usb_core_instance *pdev, stc_usb_port_identify *pstcPortIdentify);
void usb_udelay(const uint32_t usec);
void usb_mdelay(const uint32_t msec);
void usb_bsp_nvicconfig(usb_core_instance *pdev);

#ifdef __cplusplus
}
#endif

#endif /* __USB_BSP_H__ */
