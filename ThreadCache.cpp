#include "ThreadCache.h"
#include "CentralCache.h"

void *ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
    // 一次从中心缓存取多少空间 控制一个批量 小对象多给 大对象少给 慢启动 <类似拥塞窗口>
    // 在每一个自由链表中维护一个 maxsize 每次取 maxsize 与 batch 中的小值 然后每一次++maxsize或者+=2
    size_t batchNum = std::min(SizeClass::NumMoveSize(size), _FreeLists[index].MaxSize());
    // 最开始不会向中心缓存要太多 如果需求大 maxsize会不断增长 直到上限
    if (_FreeLists[index].MaxSize() == batchNum)
        _FreeLists[index].MaxSize() += 1;

    // 获取单例 调用fetchrangeobj函数 但span里不一定有那么多 返回实际获取了多少
    void *start = nullptr;
    void *end = nullptr;
    size_t actualNum = CentralCache::GetInstance()->FetchRangeObj(start, end, batchNum, size);
    assert(actualNum > 0);
    if (actualNum == 1)
    {
        assert(start == end); // start和end应该是相同的
        return start;
    }
    else
    {
        // 插入当前线程的ThreadCache自由链表 并将第一个内存块返回
        _FreeLists[index].pushRange(NextObj(start), end, actualNum - 1);
        return start;
    }
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
        return FetchFromCentralCache(index, AlignSize); // 把已经对齐后的实际需要的大小传给另一个函数
    }
}

void ThreadCache::DeAlloc(void *ptr, size_t size)
{
    assert(ptr);
    assert(size <= MAX_BYTES);
    // 确定要回收到几号桶里面
    size_t index = SizeClass::Index(size);
    _FreeLists[index].push(ptr);

    // 如果自由链表中的内存大于 向 centralcache 申请的一个批量大小 就开始释放
    if (_FreeLists[index].Size() >= _FreeLists[index].MaxSize())
    {
        ListTooLong(_FreeLists[index], size);
    }
}

void ThreadCache::ListTooLong(FreeList &list, size_t size) // threadcache 中某个自由链表的内存太多了 回收一个批量的内存
{
    void *start = nullptr;
    void *end = nullptr;
    list.PopRange(start, end, list.MaxSize());

    if (start != end)
    {
        // 将内存交给 central cache 处理
        CentralCache::GetInstance()->ReleaseListToSpans(start, size); // 不需要知道 end 遍历到为空即为结束
    }
}
