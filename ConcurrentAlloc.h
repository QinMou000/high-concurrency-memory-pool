#pragma once

#include "Common.h"
#include "ThreadCache.h"
#include "PageCache.h"
#include "ObjectPool.h"
#include <thread>

void *ConcurrentAlloc(size_t size) // Concurrent:并发
{
    // 1、当申请的字节数 大于 256 K
    //      a、大于 256 K/32 页 小于 128页 128*8 K
    //      b、大于 128 页 128*8*1024 byte
    if (size > MAX_BYTES)
    {
        // 算对齐后的大小
        size_t align = SizeClass::RoundUp(size);
        // 算出需要多少页 直接向 PageCache申请
        size_t npage = align >> PAGE_SHIFT;
        // 调用 NewSpan 申请一个 npage 页的 span
        PageCache::GetInstance()->_pageMutex.lock();
        Span *span = PageCache::GetInstance()->NewSpan(npage); // 如果大于 128 页 走系统调用 大于 256k 小于 128页 从 PageCache 拿
        span->_objSize = size;
        PageCache::GetInstance()->_pageMutex.unlock();
        // 将span申请的页的起始地址返回
        void *ptr = (void *)(span->_pageId << PAGE_SHIFT);
        return ptr;
    }
    else
    {
        // 2、当申请的字节数 小于 128 页
        if (pTLSThreadCache == nullptr) // 第一次过来为空，就new一个新的对象
        {
            // pTLSThreadCache = new ThreadCache;
            static ObjectPool<ThreadCache> ThreadCachePool; // 全局唯一的一个 ThreadCache 对象内存池
            pTLSThreadCache = ThreadCachePool.New();
        }
        // cout << std::this_thread::get_id() << ":" << pTLSThreadCache << endl;
        return pTLSThreadCache->Alloc(size); // 调用ThreadCache的Alloc
    }
}
void ConcurrentFree(void *ptr)
{
    assert(ptr);
    Span *span = PageCache::GetInstance()->MapObjectToSpan(ptr); // 获取到 当前内存块所对应的 span
    size_t size = span->_objSize;                                // 再从 span 中获取 单个内存块大小

    if (size > MAX_BYTES) // 大于 256 K
    {
        PageCache::GetInstance()->_pageMutex.lock();
        PageCache::GetInstance()->ReleaseSpanToPageCache(span); // 将该 span 直接回收到 PageCache
        PageCache::GetInstance()->_pageMutex.unlock();
    }
    else
    {
        assert(pTLSThreadCache);             // 这个指针肯定不为空了
        pTLSThreadCache->DeAlloc(ptr, size); // 这个size后面要做处理的
    }
}