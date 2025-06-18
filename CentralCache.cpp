#include "CentralCache.h"

CentralCache CentralCache::_sInst;

Span *CentralCache::GetOneSpan(SpanList &spanlist, size_t size) // 获取一个非空的Span
{
    // 现在当前 _SpanLists 里找有没有有空间的Span
    Span *it = spanlist.Begin();
    while (it != spanlist.End())
    {
        if (it->_freeList != nullptr)
            return it; // 如果 _freeList 不为空证明有空间直接返回 it
        else
            it = it->_next; // it 迭代到下一个 Span
    }
    // 走到下面 表明当前的 _SpanLists 已经没有有空间的 Span 了 要向下一层 PageCache 要
    Span *newspan = PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(size)); // 将算出需要多少页内存给给下一层

    // 将得到的大内存块切分成size大小的小内存

    char *start = (char *)(newspan->_pageId << PAGE_SHIFT); // 根据页号算出内存起始地址
    size_t bytes = newspan->_n >> PAGE_SHIFT;               // 根据有页数算出有多少字节
    char *end = start + bytes;                              // 算出末尾地址

    newspan->_freeList = start;                // 先将整个内存块的 start 给给 _freelist 指针
    start += size;                             // 逻辑上就是将前size个字节切走
    char *tail = (char *)(newspan->_freeList); // 留一个尾 方便尾插
    while (start != end)
    {
        NextObj(tail) = start;
        tail = (char *)NextObj(tail);
        start += size;
    }
    spanlist.PushFront(newspan); // 将切好内存的 Span 链入 _SpanLists 中

    return newspan;
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
