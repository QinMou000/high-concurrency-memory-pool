#pragma once

#include "Common.h"
#include "PageCache.h"

class CentralCache // 单例
{
public:
    static CentralCache *GetInstance() // static 静态
    {
        return &_sInst;
    }
    size_t FetchRangeObj(void *&start, void *&end, size_t batchNum, size_t size); // 从中心缓存获取batchNum数量的size大小的内存块
    Span *GetOneSpan(SpanList &spanlist, size_t size);                            // 获取一个非空的Span

private:
    CentralCache() {}                            // 单例模式 把构造设置为私有
    CentralCache(const CentralCache &) = delete; // 拷贝构造 赋值重载都禁掉
    CentralCache operator=(const CentralCache &) = delete;

    SpanList _SpanLists[MAX_LIST]; // 挂的是span
public:
    static CentralCache _sInst; // 单例声明 定义在 cpp里面
};