#pragma once

#include "Common.h"
#include "ObjectPool.h"

class PageCache
{
public:
    static PageCache *GetInstance()
    {
        return &_sInst;
    }

    Span *NewSpan(size_t K);                 // 获取一个有K页空间的span
    Span *MapObjectToSpan(void *obj);        // 查询这个 obj 内存块是哪一个 span 里面的
    void ReleaseSpanToPageCache(Span *span); // 释放空闲的 span 回到 PageCache 并考虑合并 span 中内存页中的前后页 缓解内存碎片问题
private:
    PageCache() {}
    PageCache(const PageCache &) = delete;

    ObjectPool<Span> _SpanPool;                     // 专门为 new span 创建的单对象内存池
    SpanList _spanLists[NPAGES];                    // 每一个span下面放的是对应序号的页大小 数组第二个元素的span放的就是一页的内存大小
    std::unordered_map<PAGE_ID, Span *> _idSpanMap; // 将 页号 与页号内存块与 span 相映射
    static PageCache _sInst;                        // 单例

public:
    std::mutex _pageMutex;
};