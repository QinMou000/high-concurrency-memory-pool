#include "CentralCache.h"

CentralCache CentralCache::_sInst;

Span *CentralCache::GetOneSpan(SpanList &spanlist, size_t size) // 获取一个非空的Span
{
    return nullptr;
}

size_t CentralCache::FetchRangeObj(void *&start, void *&end, size_t batchNum, size_t size) // 从中心缓存获取batchNum数量的size大小的内存块
{
    // 获取这个大小的内存块应该在那个桶取 <下标>
    size_t index = SizeClass::Index(size);
    // 加锁
    _SpanLists[index]._mtx.lock();
    // 获取一个非空Span
    Span *span = GetOneSpan(_SpanLists[index], size);
    assert(span);                       // 保证span不为空
    assert(span->_freeList != nullptr); // 保证span里面有内存块
    // 从Span上截一段空间 返回实际取到的数量
    start = span->_freeList;
    end = start;
    size_t actualNum = 1; // _freelist不为空 我们实际上就是从1开始算起的 起码有一个内存块
    int i = 0;
    while (i < batchNum - 1 && NextObj(end) != nullptr) // 最多取batchNum个内存块 如果没有那么多 就有多少取多少
    {
        end = NextObj(end);
        ++i;
        ++actualNum;
    }
    span->_freeList = NextObj(end); // 将_freeList 指向 end的下一个位置
    NextObj(end) = nullptr;         // 将end的前4/8字节置空
    span->_usecount += actualNum;   // 使用计数加上 actualNum

    _SpanLists[index]._mtx.unlock();
    return actualNum;
}
