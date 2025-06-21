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
    PAGE_ID id = (int)obj >> PAGE_SHIFT;
    auto it = _idSpanMap.find(id);
    if (it != _idSpanMap.end()) // 找到了
        return it->second;
    else
    {
        assert(false); // 不可能走到这里
        return nullptr;
    }
}
void ReleaseSpanToPageCache(Span *span)
{
    // 向前合并

    // 向后和并
}
