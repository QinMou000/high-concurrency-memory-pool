#include "PageCache.h"

PageCache PageCache::_sInst;

Span *PageCache::NewSpan(size_t K) // 获取一个有K页空间的span
{
    // 断言 K
    assert(K > 0 && K < NPAGES);
    // 查看当前位置有没有
    if (!_spanLists[K].Empty())
        return _spanLists[K].PopFront();

    // 查看后面位置有没有 切分
    for (int i = K + 1; i < NPAGES; ++i)
        if (!_spanLists[i].Empty()) // 如果第i个桶不为空
        {
            Span *nspan = _spanLists[i].PopFront();
            Span *kspan = new Span;

            // 对 nspan 进行切分
            kspan->_pageId = nspan->_pageId;
            kspan->_n = K; // 对 kspan 进行初始化

            nspan->_pageId += K;
            nspan->_n -= K; // 修改 nspan 删除不属于nspan的页号 和 页数

            _spanLists[nspan->_n].PushFront(nspan); // 将切分后剩下的 nspan 放入对应的桶里面

            // 将 nspan 中的第一页和最后一页 进行映射 方便 pagecache 回收内存时查找 span
            _idSpanMap[nspan->_pageId] = nspan;
            _idSpanMap[nspan->_pageId + nspan->_n - 1] = nspan;

            for (size_t i = 0; i < kspan->_n; i++) // 建立页号与Span间的映射 方便 centralcache 查找页号与 span 的关系
                _idSpanMap[kspan->_pageId + i] = kspan;

            return kspan;
        }
    // 向堆获取
    // 向系统申请 NPAGES - 1 页的内存 并将它挂到最后一个桶里面
    void *ptr = SysAlloc(NPAGES - 1); // 向系统申请
    Span *BigSpan = new Span;
    BigSpan->_pageId = (PAGE_ID)ptr >> PAGE_SHIFT; // 初始化 页号
    BigSpan->_n = NPAGES - 1;                      // 初始化页数

    _spanLists[BigSpan->_n].PushFront(BigSpan); // 将BigSpan插入最后一个桶

    // 这里设计得很巧妙
    return NewSpan(K); // 递归再调一次 这里的循环判断消耗很小 不用考虑性能问题
}

Span *PageCache::MapObjectToSpan(void *obj)
{
    PAGE_ID id = ((size_t)obj >> PAGE_SHIFT);
    auto it = _idSpanMap.find(id);
    if (it != _idSpanMap.end()) // 找到了
        return it->second;
    else
    {
        assert(false); // 不可能走到这里
        return nullptr;
    }
}
void PageCache::ReleaseSpanToPageCache(Span *span)
{
    // 向前合并
    while (1)
    {
        PAGE_ID prevId = span->_pageId - 1;
        auto ret = _idSpanMap.find(prevId);
        if (ret == _idSpanMap.end()) // 如果这个页没有被申请 就退出
            break;
        Span *prevSpan = ret->second;
        if (prevSpan->_isUse == true)             // 不能用 usecount == 0 表示没有被使用 prevSpan 可能刚申请出来 可能已经用了一段时间了
            break;                                // 因为当 span 刚刚申请好的时候 和 在 centralcache 内存块刚好返回完时 usecount 也是等于 0 的
        if (span->_n + prevSpan->_n > NPAGES - 1) // 如果合并后的页大于 128 没法管理 退出
            break;

        span->_pageId = prevSpan->_pageId;
        span->_n += prevSpan->_n;

        _spanLists[prevSpan->_n].Erase(prevSpan); // 将 prevSpan 从桶中删除
        delete prevSpan;                          // 释放对象 注意这里不是释放内存
    }
    // 向后和并
    while (1)
    {
        PAGE_ID nextId = span->_pageId + span->_n;
        auto ret = _idSpanMap.find(nextId);
        if (ret == _idSpanMap.end()) // 如果这个页没有被申请 就退出
            break;
        Span *nextSpan = ret->second;
        if (nextSpan->_isUse == true)             // 不能用 usecount == 0 表示没有被使用 prevSpan 可能刚申请出来 可能已经用了一段时间了
            break;                                // 因为当 span 刚刚申请好的时候 和 在 centralcache 内存块刚好返回完时 usecount 也是等于 0 的
        if (span->_n + nextSpan->_n > NPAGES - 1) // 如果合并后的页大于 128 没法管理 退出
            break;

        span->_n += nextSpan->_n;

        _spanLists[nextSpan->_n].Erase(nextSpan); // 将 nextSpan 从桶中删除
        delete nextSpan;                          // 释放对象 注意这里不是释放内存
    }

    // 将新 Span 插入
    _spanLists[span->_n].PushFront(span);
    span->_isUse = false;
    // 建立新的映射
    _idSpanMap[span->_pageId] = span;
    _idSpanMap[span->_pageId + span->_n - 1] = span;
}
