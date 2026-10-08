/**
 *******************************************************************************
 * @file  usb/usb_dev_msc/source/usb_app_conf.h
 * @brief low level driver configuration
 *******************************************************************************
 */
#ifndef __USB_APP_CONF_H__
#define __USB_APP_CONF_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/

/* USB MODE CONFIGURATION */
#define USB_FS_MODE
#define USE_DEVICE_MODE

#ifndef USB_FS_MODE
#define USB_FS_MODE
#endif

#ifndef USE_DEVICE_MODE
#ifndef USE_HOST_MODE
#error  "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
#endif
#endif

/* USB DEVICE ENDPOINT CONFIGURATION */
#define MSC_IN_EP                       (0x81U)
#define MSC_OUT_EP                      (0x01U)

/* USB FIFO CONFIGURATION */
#define RX_FIFO_FS_SIZE                 (128U)
#define TX0_FIFO_FS_SIZE                (128U)
#define TX1_FIFO_FS_SIZE                (64U)
#define TX2_FIFO_FS_SIZE                (0U)
#define TX3_FIFO_FS_SIZE                (0U)
#define TX4_FIFO_FS_SIZE                (0U)
#define TX5_FIFO_FS_SIZE                (0U)

#if ((RX_FIFO_FS_SIZE + TX0_FIFO_FS_SIZE + TX1_FIFO_FS_SIZE + TX2_FIFO_FS_SIZE + TX3_FIFO_FS_SIZE + TX4_FIFO_FS_SIZE + \
      TX5_FIFO_FS_SIZE) > 320U)
#error  "The USB max FIFO size is 320 x 4 Bytes!"
#endif

/* FUNCTION CONFIGURATION */
#define DEV_MAX_CFG_NUM                 (1U)
#define USBD_ITF_MAX_NUM                (1U)
#define USB_MAX_STR_DESC_SIZ            (128U)

/* VBUS 检测关闭：PA9 留给 OLED SCL 使用，USB 使用软件软连接 */
//#define VBUS_SENSING_ENABLED
//#define SELF_POWER

/* CONFIGURATION FOR MSC */
#define MSC_MEDIA_PACKET                (12UL * 1024UL)
#define MSC_MAX_PACKET                  (64U)

#ifdef __cplusplus
}
#endif

#endif /* __USB_APP_CONF_H__ */
