#pragma once
#include <assert.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <windows.h>

using std::cout;
using std::endl;

static const size_t MAX_LIST = 208;

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

    bool IsEmpty()
    {
        return _freeList == nullptr;
    }

private:
    void *_freeList = nullptr;
};

class SizeClass
{
public:
    // 整体控制在最多10%左右的内碎片浪费
    // [1,128]              8byte对齐       freelist[0,16)
    // [128+1,1024]         16byte对齐      freelist[16,72)
    // [1024+1,81024]       128byte对齐     freelist[72,128)
    // [8*1024+1,641024]    1024byte对齐    freelist[128,184)
    // [64*1024+1,256*1024] 8*1024byte对齐  freelist[184,208)

    static inline size_t RoundUp(size_t bytes) // 给我你需要的内存大小，返回给你实际开辟的内存大小
    {
        // TODO
    }
    static inline size_t Index(size_t bytes) // 给我你要开辟的内存大小，返回给你应该在几号桶取空间
    {
        // TODO
    }
};