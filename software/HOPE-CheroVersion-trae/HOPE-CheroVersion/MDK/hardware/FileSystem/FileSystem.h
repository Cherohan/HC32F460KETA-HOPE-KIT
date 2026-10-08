#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H
#include "Hugo_UI.h"

void FileSystem_Init(void);

void EventSaveSettingConfig(void);

void EventLoadSettingConfig(void);

uint8_t HugoUIPageFilesAddItems(HugoUIPage_t *thispage, uint8_t *path);
void HugoUIPageFilesCallBack(void);
#endif
