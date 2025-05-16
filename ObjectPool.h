#pragma once
#include "Common.h"

template <class T>
class ObjectPool
{
public:
    T *New()
    {
        T *obj = nullptr;
        if (_freeList) // 如果_freeList不为空，说明可以把回收的内存拿回来用
        {
            // 头删
            void *next = *(void **)_freeList; // 根据32/64位机器，将_freeList的前4/8个byte拿到
            obj = (T *)_freeList;
            _freeList = next;
        }
        else
        {
            if (_remainByte < sizeof(T)) // 如果剩余内存大小已经小于T所需内存大小，剩下的就不要了，直接重开一块内存
            {
                _remainByte = 8 * 1024; // 8KB
                // _memory = (char *)malloc(_remainByte);
                _memory = (char *)SysAlloc(_remainByte >> 13);
                if (_memory == nullptr)
                    throw std::bad_alloc(); // 抛出一个对象
            }
            // obj = (T*)_memory;
            // _memory += sizeof(T);
            // _remainByte -= sizeof(T);
            obj = (T *)_memory;
            size_t ObjSize = sizeof(T) < sizeof(void *) ? sizeof(void *) : sizeof(T); // 解决T大小 小于4/8字节的情况
            _memory += ObjSize;
            _remainByte -= ObjSize;
        }
        new (obj) T; // 定位new
        // new (place_address) Type;
        // new (place_address) Type(initializer-list);

        return obj;
    }
    void Delete(T *obj)
    {
        // 显式调用T的析构函数
        obj->~T();

        // 不管_freeList 为不为空，我们都进行头插
        *(void **)obj = _freeList;
        _freeList = obj;
    }

private:
    char *_memory = nullptr;   // 指向大的内存块的
    size_t _remainByte = 0;    // 大内存块还剩下多少字节的内存
    void *_freeList = nullptr; // 回收链表
};

struct TreeNode
{
    int _val;
    TreeNode *_left;
    TreeNode *_right;

    TreeNode()
        : _val(0), _left(nullptr), _right(nullptr)
    {
    }
};

void TestObjectPool()
{
    // 申请释放的轮次
    const size_t Rounds = 5;

    // 每轮申请释放多少次
    const size_t N = 1000000;

    std::vector<TreeNode *> v1;
    v1.reserve(N);

    size_t begin1 = clock();
    for (size_t j = 0; j < Rounds; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            v1.push_back(new TreeNode);
        }
        for (int i = 0; i < N; ++i)
        {
            delete v1[i];
        }
        v1.clear();
    }

    size_t end1 = clock();

    std::vector<TreeNode *> v2;
    v2.reserve(N);

    ObjectPool<TreeNode> TNPool;
    size_t begin2 = clock();
    for (size_t j = 0; j < Rounds; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            v2.push_back(TNPool.New());
        }
        for (int i = 0; i < N; ++i)
        {
            TNPool.Delete(v2[i]);
        }
        v2.clear();
    }
    size_t end2 = clock();

    cout << "new cost time:" << end1 - begin1 << endl;
    cout << "object pool cost time:" << end2 - begin2 << endl;
}