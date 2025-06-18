#include "ObjectPool.h"
#include "ThreadCache.h"
#include "ConcurrentAlloc.h"
#include <thread>

void alloc1()
{
    for (size_t i = 0; i < 5; ++i)
    {
        void *ptr = ConcurrentAlloc(2);
    }
}

void alloc2()
{
    for (size_t i = 0; i < 5; ++i)
    {
        void *ptr = ConcurrentAlloc(23);
    }
}

int main()
{
    // std::thread thread1(alloc1);
    // thread1.join();
    // std::thread thread2(alloc2);
    // thread2.join();

    std::cout << sizeof(PAGE_ID) << std::endl;

    // TestObjectPool();
    // cout << SizeClass::RoundUp(2) << endl;
    // cout << SizeClass::RoundUp(5) << endl;
    // cout << SizeClass::RoundUp(9) << endl;
    // cout << SizeClass::RoundUp(129) << endl;
    // cout << SizeClass::RoundUp(1025) << endl;

    // cout << SizeClass::Index(2) << endl;
    // cout << SizeClass::Index(5) << endl;
    // cout << SizeClass::Index(9) << endl;
    // cout << SizeClass::Index(129) << endl;
    // cout << SizeClass::Index(1025) << endl;

    return 0;
}