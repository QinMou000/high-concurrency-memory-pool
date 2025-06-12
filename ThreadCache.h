#pragma once

#include "Common.h"

class ThreadCache
{
public:
    void *Alloc(size_t size);
    void DeAlloc(void *ptr, size_t size);

    // 从中心缓存取空间
    void *FetchFromCentralCache(size_t index, size_t size);

private:
    FreeList _FreeLists[MAX_LIST]; // 哈希桶，下面挂不同大小的内存块
};

// 用TLS实现对象线程隔离
// 每个线程首次访问 pTLSThreadCache 时，会创建一个独立的指针副本（初始为 nullptr）。
// 线程A修改自己的副本不会影响线程B的副本。
static thread_local ThreadCache *pTLSThreadCache = nullptr;