#include "malloc.h"

// 内存池(32字节对齐)
__align(32) uint8_t mem1base[MEM1_MAX_SIZE]; // 内部SRAM内存池
// 内存管理表
uint16_t mem1mapbase[MEM1_ALLOC_TABLE_SIZE]; // 内部SRAM内存池MAP
// 内存管理参数
const uint32_t memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE}; // 内存表大小
const uint32_t memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE};       // 内存分块大小
const uint32_t memsize[SRAMBANK] = {MEM1_MAX_SIZE};            // 内存总大小

// 内存管理控制器
struct _m_mallco_dev mallco_dev =
    {
        my_mem_init,       // 内存初始化
        my_mem_perused,    // 内存使用率
        mem1base,          // 内存池
        mem1mapbase,       // 内存管理状态表
        0,                 // 内存管理未就绪
};

// 复制内存
void mymemcpy(void *des, void *src, uint32_t n)
{
    uint8_t *xdes = des;
    uint8_t *xsrc = src;
    while (n--)
        *xdes++ = *xsrc++;
}
// 设置内存
void mymemset(void *s, uint8_t c, uint32_t count)
{
    uint8_t *xs = s;
    while (count--)
        *xs++ = c;
}
// 内存管理初始化
void my_mem_init(uint8_t memx)
{
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 2); // 内存状态表数据清零
    mymemset(mallco_dev.membase[memx], 0, memsize[memx]);       // 内存池所有数据清零
    mallco_dev.memrdy[memx] = 1;                                // 内存管理初始化OK
}
// 获取内存使用率
uint8_t my_mem_perused(uint8_t memx)
{
    uint32_t used = 0;
    uint32_t i;
    for (i = 0; i < memtblsize[memx]; i++)
    {
        if (mallco_dev.memmap[memx][i])
            used++;
    }
    return (used * 100) / (memtblsize[memx]);
}
// 内存分配(内部调用)
uint32_t my_mem_malloc(uint8_t memx, uint32_t size)
{
    signed long offset = 0;
    uint32_t nmemb;     // 需要的内存块数
    uint32_t cmemb = 0; // 连续空内存块数
    uint32_t i;
    if (!mallco_dev.memrdy[memx])
        mallco_dev.init(memx); // 未初始化,先执行初始化
    if (size == 0)
        return 0XFFFFFFFF;           // 不需要分配
    nmemb = size / memblksize[memx]; // 获取需要分配的连续内存块数
    if (size % memblksize[memx])
        nmemb++;
    for (offset = memtblsize[memx] - 1; offset >= 0; offset--) // 搜索整个内存控制区
    {
        if (!mallco_dev.memmap[memx][offset])
            cmemb++; // 连续空内存块数增加
        else
            cmemb = 0;      // 连续内存块清零
        if (cmemb == nmemb) // 找到了连续nmemb个空内存块
        {
            for (i = 0; i < nmemb; i++) // 标注内存块非空
            {
                mallco_dev.memmap[memx][offset + i] = nmemb;
            }
            return (offset * memblksize[memx]); // 返回偏移地址
        }
    }
    return 0XFFFFFFFF; // 未找到符合分配条件的内存块
}
// 释放内存(内部调用)
uint8_t my_mem_free(uint8_t memx, uint32_t offset)
{
    int i;
    if (!mallco_dev.memrdy[memx]) // 未初始化,先执行初始化
    {
        mallco_dev.init(memx);
        return 1; // 未初始化
    }
    if (offset < memsize[memx]) // 偏移在内存池内.
    {
        int index = offset / memblksize[memx];      // 偏移所在内存块号码
        int nmemb = mallco_dev.memmap[memx][index]; // 内存块数量
        for (i = 0; i < nmemb; i++)                 // 内存块清零
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }
        return 0;
    }
    else
        return 2; // 偏移超区了.
}
// 释放内存(外部调用)
void myfree(uint8_t memx, void *ptr)
{
    uint32_t offset;
    if (ptr == NULL)
        return; // 地址为0.
    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];
    my_mem_free(memx, offset); // 释放内存
}
// 分配内存(外部调用)
void *mymalloc(uint8_t memx, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);
    if (offset == 0XFFFFFFFF)
        return NULL;
    else
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
}
// 重新分配内存(外部调用)
void *myrealloc(uint8_t memx, void *ptr, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);
    if (offset == 0XFFFFFFFF)
        return NULL;
    else
    {
        mymemcpy((void *)((uint32_t)mallco_dev.membase[memx] + offset), ptr, size); // 拷贝旧内存内容到新内存
        myfree(memx, ptr);                                                     // 释放旧内存
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);          // 返回新内存首地址
    }
}
