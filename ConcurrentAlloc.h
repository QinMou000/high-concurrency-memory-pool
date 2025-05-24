#pragma once

#include "Common.h"
#include "ThreadCache.h"
#include <thread>

void *ConcurrentAlloc(size_t size)
{
    if (pTLSThreadCache == nullptr) // 第一次过来为空，就new一个新的对象
    {
        pTLSThreadCache = new ThreadCache;
    }
    // cout << std::this_thread::get_id() << ":" << pTLSThreadCache << endl;
    return pTLSThreadCache->Alloc(size); // 调用ThreadCache的Alloc
}
void ConcurrentFree(void *ptr, size_t size /*TODO*/)
{
    assert(ptr);
    assert(pTLSThreadCache);             // 这个指针肯定不为空了
    pTLSThreadCache->DeAlloc(ptr, size); // 这个size后面要做处理的TODO
}