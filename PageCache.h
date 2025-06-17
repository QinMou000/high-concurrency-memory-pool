#pragma once

#include "Common.h"

class PageCache
{
public:
    static PageCache *GetInstance()
    {
        return &_sInst;
    }

    Span *NewSpan(size_t K); // 获取一个有K页空间的span

private:
    PageCache() {}
    PageCache(const PageCache &) = delete;

private:
    SpanList spanLists[NPAGES]; // 每一个span下面放的是对应序号的页大小 数组第一个元素span放的就是一页的内存大小

    static PageCache _sInst; // 单例
};