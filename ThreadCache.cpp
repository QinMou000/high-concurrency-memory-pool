#include "ThreadCache.h"

void *ThreadCache::Alloc(size_t size)
{
    // 确定对齐后的大小
    size_t AlignSize = SizeClass::RoundUp(size);
    // 确定在几号桶
    size_t index = SizeClass::Index(AlignSize);
    if (!_FreeLists[index].IsEmpty())
    {
        return _FreeLists[index].pop();
    }
    else
    {
        return FetchFromCentralCache(index, AlignSize); // 把已经对齐后的实际需要的大小传给另一个模块
    }
}

void ThreadCache::DeAlloc(void *ptr, size_t size)
{
}