#pragma once
#include <assert.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <windows.h>

using std::cout;
using std::endl;


void *SysAlloc(size_t kpage)
{
#if defined(_WIN32)
    // WIN32
    void *ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    // Linux
    // ...
#endif

    if (ptr == nullptr)
        exit(-1);
    return ptr;
}

// 根据32/64位机器，将obj的前4/8个byte拿到
#define NextObj(obj) (*(void **)(obj))

class FreeList // 自由链表
{
public:
    void push(void *obj)
    {
        assert(obj);
        // 头插
        obj = NextObj(_freeList);
        _freeList = obj;
    }
    void *pop()
    {
        // 头删并返回该内存块
        assert(_freeList);

        void *obj = _freeList;
        _freeList = NextObj(obj);
        return obj;
    }

private:
    void *_freeList = nullptr;
};
