#include "FileSystem.h"

extern uint8_t ui_Key_num, ui_Encoder_num;

/* 无 FATFS：本阶段不挂载 Flash 文件系统，仅提供占位实现，
   Flash 驱动(W25Q128)已在 main.c 中单独初始化。 */

void FileSystem_Init(void)
{
    /* 不挂载文件系统，什么都不做 */
}

/* 用于保存数据的事件函数（占位） */
void EventSaveSettingConfig(void)
{
    static uint8_t SaveFinish_flag = 1;

    if (SaveFinish_flag)
    {
        Oled_u8g2_ShowStr(0, FONT_HEIGHT, "No FATFS!");
        Oled_u8g2_ShowStr(0, FONT_HEIGHT * 2, "Config Not Saved");
        SaveFinish_flag = 0;
    }
    else
    {
        Oled_u8g2_ShowStr((SCREEN_WIDTH - Oled_u8g2_Get_UTF8_ASCII_PixLen("Saved Success!")) / 2,
                          SCREEN_HEIGHT / 2 + 3, "Saved Success!");
    }

    if (ui_Key_num == 2)
    {
        SaveFinish_flag = 1;
    }
}

/* 用于上电载入数据的事件函数（占位） */
void EventLoadSettingConfig(void)
{
    /* 无 FATFS，不载入配置 */
}

/* 文件页只加入一个 Exit 项，避免出现空页 */
uint8_t HugoUIPageFilesAddItems(HugoUIPage_t *thispage, uint8_t *path)
{
    (void)path;

    if (lastPage == NULL)
        thispage->AddItem(thispage, "Exit", ITEM_JUMP_PAGE)->SetJumpId(0, 0);
    else
        thispage->AddItem(thispage, "Exit", ITEM_JUMP_PAGE)->SetJumpId(lastPage->pageId, 0);

    return 0;
}

void HugoUIPageFilesCallBack(void)
{
    /* 无 FATFS，暂不处理文件页回调 */
}
