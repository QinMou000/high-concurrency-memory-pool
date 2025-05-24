#include "ThreadCache.h"

void *ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
    return nullptr;
}

void *ThreadCache::Alloc(size_t size)
{
    // 确定对齐后的大小
    size_t AlignSize = SizeClass::RoundUp(size);
    // cout << "AlignSize: " << AlignSize << endl;
    // 确定在几号桶
    size_t index = SizeClass::Index(AlignSize);
    // cout << "index: " << index << endl;
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
    assert(ptr);
    assert(size <= MAX_BYTES);
    // 确定要回收到几号桶里面
    size_t index = SizeClass::Index(size);
    _FreeLists[index].push(ptr);
}
