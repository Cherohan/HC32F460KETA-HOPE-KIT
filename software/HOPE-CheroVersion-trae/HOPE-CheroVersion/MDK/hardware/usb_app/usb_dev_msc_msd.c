/**
 *******************************************************************************
 * @file  usb_dev_msc_msd.c
 * @brief user MSC application layer (HOPE-CheroVersion: 16MB W25Q128)
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_dev_msc_msd.h"
#include "usb_dev_msc_mem.h"
#include <string.h>

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
int8_t msc_init(uint8_t lun);
int8_t msc_getcapacity(uint8_t lun, uint32_t *block_num, uint32_t *block_size);
int8_t msc_ifready(uint8_t lun);
int8_t msc_ifwrprotected(uint8_t lun);
int8_t msc_read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t msc_write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t msc_getmaxlun(void);

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define MSC_BLOCK_SIZE      (512U)
#define MSC_BLOCK_NUM       (32768U)   /* 16MB / 512B (W25Q128) */

/*******************************************************************************
 * Global variable definitions
 ******************************************************************************/
/* Variable for Storage operation status */
__IO static uint8_t USB_STATUS_REG = 0U;

/* USB Mass storage inquiry data (36 bytes for each lun) */
const int8_t msc_inquirydata[] = {
    /* LUN 0 */
    0x00,
    0x80,
    0x02,
    0x02,
    (USB_DEV_INQUIRY_LENGTH - 4U),
    0x00,
    0x00,
    0x00,
    /* Vendor Identification */
    'X', 'H', 'S', 'C', ' ', 'M', 'C', 'U', ' ',    /* 9 bytes */
    /* Product Identification */
    'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',    /* 15 bytes */
    ' ', 'D', 'i', 's', 'k', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' ',                             /* 4 bytes */
};

static USB_DEV_MSC_cbk_TypeDef flash_fops = {
    &msc_init,
    &msc_getcapacity,
    &msc_getmaxlun,
    &msc_ifready,
    &msc_read,
    &msc_write,
    &msc_ifwrprotected,
    (int8_t *)msc_inquirydata
};

/* Pointer to flash_fops */
USB_DEV_MSC_cbk_TypeDef *msc_fops = &flash_fops;

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

/**
 * @brief  initialize storage
 * @param  [in] lun          logic number
 * @retval status
 */
int8_t msc_init(uint8_t lun)
{
    (void)lun;
    return LL_OK;
}

/**
 * @brief  Get Storage capacity
 * @param  [in] lun          logic number
 * @param  [in] block_num    sector number
 * @param  [in] block_size   sector size
 * @retval status
 */
int8_t msc_getcapacity(uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
    (void)lun;
    *block_size = MSC_BLOCK_SIZE;
    *block_num  = MSC_BLOCK_NUM;
    return LL_OK;
}

/**
 * @brief  Check if storage is ready
 * @param  [in] lun          logic number
 * @retval status
 */
int8_t msc_ifready(uint8_t lun)
{
    (void)lun;
    USB_STATUS_REG |= (uint8_t)0X10;
    return LL_OK;
}

/**
 * @brief  Check if storage is write protected
 * @param  [in] lun          logic number
 * @retval status
 */
int8_t msc_ifwrprotected(uint8_t lun)
{
    (void)lun;
    return LL_OK;
}

/**
 * @brief  read data from storage devices
 * @param  [in] lun          logic number
 * @param  [in] buf          data buffer be read
 * @param  [in] blk_addr     sector address
 * @param  [in] blk_len      sector count
 * @retval status
 */
int8_t msc_read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    (void)lun;
    (void)blk_addr;
    /* 无实际存储介质：返回全 0（枚举测试用途） */
    (void)memset(buf, 0, (size_t)blk_len * MSC_BLOCK_SIZE);
    return LL_OK;
}

/**
 * @brief  Write data to storage devices
 * @param  [in] lun          logic number
 * @param  [in] buf          data buffer be written
 * @param  [in] blk_addr     sector address
 * @param  [in] blk_len      sector count
 * @retval status
 */
int8_t msc_write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    (void)lun;
    (void)buf;
    (void)blk_addr;
    (void)blk_len;
    return LL_OK;
}

/**
 * @brief  Get supported logic number
 * @param  None
 * @retval 0 (single LUN)
 */
int8_t msc_getmaxlun(void)
{
    return 0;
}
