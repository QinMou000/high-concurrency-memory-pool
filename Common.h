#pragma once
#include <assert.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <time.h>
#include <windows.h>

using std::cout;
using std::endl;

static const size_t MAX_LIST = 208;
static const size_t MAX_BYTES = 256 * 1024;

#ifdef _WIN64
typedef unsigned long long PAGE_ID;
#elif _WIN32
typedef size_t PAGE_ID;
#else
// Linux TODO
#endif // _WIN32

static void *SysAlloc(size_t kpage)
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
    static inline size_t _RoundUp(size_t bytes, size_t align /*对齐数*/)
    {
        //      计算对齐掩码：~(align - 1)
        // 要保证 align 是 2 的幂，例如 2、4、8、16 等。以 8 为例，align - 1 就是 7（二进制为 00000111）。
        // 对 align - 1 取反后，得到的掩码是高位全为 1，低 log2(align) 位全为 0。就拿 8 来说，取反后掩码为 11111000。
        //      向上舍入：(bytes + align - 1) & 掩码
        // 先将 bytes 加上 align - 1，这样就能确保结果向上舍入到下一个对齐边界。
        // 然后通过与掩码进行按位与运算，把低 log2(align) 位清零，从而得到对齐后的值。
        return (bytes + align - 1) & ~(align - 1); // 很巧妙的糕手写法
    }
    // 整体控制在最多10%左右的内碎片浪费
    // [1,128]              8byte对齐       freelist[0,16)
    // [128+1,1024]         16byte对齐      freelist[16,72)
    // [1024+1,8*1024]      128byte对齐     freelist[72,128)
    // [8*1024+1,64*1024]   1024byte对齐    freelist[128,184)
    // [64*1024+1,256*1024] 8*1024byte对齐  freelist[184,208)

    static inline size_t RoundUp(size_t bytes) // 给我你需要的内存大小，返回给你实际开辟的内存大小
    {
        assert(bytes <= MAX_BYTES);
        if (bytes <= 128)
        {
            return _RoundUp(bytes, 8);
        }
        else if (bytes <= 1024)
        {
            return _RoundUp(bytes, 16);
        }
        else if (bytes <= 8 * 1024)
        {
            return _RoundUp(bytes, 128);
        }
        else if (bytes <= 64 * 1024)
        {
            return _RoundUp(bytes, 1024);
        }
        else if (bytes <= 256 * 1024)
        {
            return _RoundUp(bytes, 8 * 1024);
        }
        else
        {
            return 0;
        }
    }

    static inline size_t _Index(size_t bytes, size_t aligan_shift /*对齐数对应的2的几次方*/)
    {
        //      计算对齐掩码：(1 << align_shift) - 1
        // 1 << align_shift 会生成一个值，这个值是 2 的 align_shift 次幂，也就是 2^align_shift。
        // 用这个结果减 1，得到的掩码在低 align_shift 位全为 1，其余高位全为 0。
        // 举个例子，若 align_shift 为 3，那么 1 << 3 是 8（二进制为 1000），减 1 后得到 7（二进制为 0111）。
        //      向上舍入到对齐边界：(bytes + mask) >> align_shift
        // 把 bytes 与掩码相加。要是 bytes 并非 2^align_shift 的整数倍，相加后就会进到下一个对齐边界。
        // 右移 align_shift 位，这相当于把结果除以 2^align_shift，最终得到的是向上舍入后的对齐值。
        //      减 1 操作：最终结果要减去 1。

        return ((bytes + (1 << aligan_shift) - 1) >> aligan_shift) - 1; // 很巧妙的糕手写法
    }

    static inline size_t Index(size_t bytes) // 给我你要开辟的内存大小，返回给你应该在几号桶取空间
    {
        assert(bytes <= MAX_BYTES);
        // 列出每个对齐区间有多少个桶
        size_t arr[5] = {16, 56, 56, 56};
        // 将当前的结果加上之前桶的下标
        if (bytes <= 128)
        {
            return _Index(bytes, 3);
        }
        else if (bytes <= 1024)
        {
            return _Index(bytes, 4) + arr[0];
        }
        else if (bytes <= 8 * 1024)
        {
            return _Index(bytes, 7) + arr[0] + arr[1];
        }
        else if (bytes <= 64 * 1024)
        {
            return _Index(bytes, 10) + arr[0] + arr[1] + arr[2];
        }
        else if (bytes <= 256 * 1024)
        {
            return _Index(bytes, 13) + arr[0] + arr[1] + arr[2] + arr[3];
        }
        else
        {
            return MAX_LIST;
        }
    }
};

// 管理多个连续页的大块内存跨度结构
struct Span
{
    PAGE_ID _pageId = 0; // 页号
    size_t _n = 0;       // 页数

    Span *_prev = nullptr; // 双向链表指针
    Span *_next = nullptr; // 双向链表指针

    size_t _usecount = 0; // 被切成的小块内存被分配给ThreadCache的计数

    void *_freeList = nullptr; // 切好的小块内存自由链表
};

class SpanList // 带头双向循环链表
{
public:
    SpanList()
    {
        _head = new Span;
        _head->_next = _head;
        _head->_prev = _head;
    }

    void Insert(Span *pos, Span *newSpan)
    {
        assert(pos);
        assert(newSpan);
        Span *prev = pos->_prev;

        // prev <-> newspan <-> pos
        prev->_next = newSpan;
        newSpan->_prev = prev;
        newSpan->_next = pos;
        pos->_prev = newSpan;
    }
    void Erase(Span *pos)
    {
        assert(pos);
        assert(pos != _head); // 不能删除哨兵位的头节点

        Span *prev = pos->_prev;
        Span *next = pos->_next;

        // prev - pos - next
        prev->_next = next;
        next->_prev = prev;
    }

private:
    Span *_head;
    std::mutex _mtx; // 每一个Span里面都有一把锁 叫做桶锁 支持多线程访问不同的桶
};