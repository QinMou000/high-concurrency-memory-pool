#include "ObjectPool.h"
#include "ThreadCache.h"
#include "ConcurrentAlloc.h"
#include <thread>
#include <atomic>

// void alloc1()
// {
//     for (size_t i = 0; i < 5; ++i)
//     {
//         void *ptr = ConcurrentAlloc(2);
//     }
// }

// void alloc2()
// {
//     for (size_t i = 0; i < 5; ++i)
//     {
//         void *ptr = ConcurrentAlloc(23);
//     }
// }

void TestConcurrentAlloc1()
{
    void *ptr1 = ConcurrentAlloc(6);
    void *ptr2 = ConcurrentAlloc(1);
    void *ptr3 = ConcurrentAlloc(5);
    void *ptr4 = ConcurrentAlloc(7);
    void *ptr5 = ConcurrentAlloc(1);
    void *ptr6 = ConcurrentAlloc(2);
    void *ptr7 = ConcurrentAlloc(2);

    cout << ptr1 << endl;
    cout << ptr2 << endl;
    cout << ptr3 << endl;
    cout << ptr4 << endl;
    cout << ptr5 << endl;
    cout << ptr6 << endl;
    cout << ptr7 << endl;

    ConcurrentFree(ptr1);
    ConcurrentFree(ptr2);
    ConcurrentFree(ptr3);
    ConcurrentFree(ptr4);
    ConcurrentFree(ptr5);
    ConcurrentFree(ptr6);
    ConcurrentFree(ptr7);
}
void TestConcurrentAlloc2()
{
    for (int i = 0; i < 1024; i++)
    {
        void *ptr1 = ConcurrentAlloc(6);
        cout << ptr1 << endl;
    }
    void *ptr2 = ConcurrentAlloc(6);
    cout << ptr2 << endl;
}

void MuitlThreadAlloc1()
{
    std::vector<void *> v;
    for (size_t i = 0; i < 10; ++i)
    {
        void *ptr = ConcurrentAlloc(2);
        v.push_back(ptr);
    }
    for (auto e : v)
    {
        ConcurrentFree(e);
    }
}

void MuitlThreadAlloc2()
{
    std::vector<void *> v;
    for (size_t i = 0; i < 10; ++i)
    {
        void *ptr = ConcurrentAlloc(15);
        v.push_back(ptr);
    }
    for (auto e : v)
    {
        ConcurrentFree(e);
    }
}
// 单轮次申请释放次数 线程数 轮次
void BenchmarkConcurrentMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
    std::vector<std::thread> vthread(nworks);
    std::atomic<size_t> malloc_costtime(0);
    std::atomic<size_t> free_costtime(0);

    for (size_t k = 0; k < nworks; ++k)
    {
        vthread[k] = std::thread([&]()
                                 {
			std::vector<void*> v;
			v.reserve(ntimes);

			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					v.push_back(ConcurrentAlloc(16));
					//v.push_back(ConcurrentAlloc((16 + i) % 8192 + 1));
				}
				size_t end1 = clock();

				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					ConcurrentFree(v[i]);
				}
				size_t end2 = clock();
				v.clear();

				malloc_costtime += (end1 - begin1);
				free_costtime += (end2 - begin2);
			} });
    }

    for (auto &t : vthread)
    {
        t.join();
    }

    printf("%u个线程并发执行%u轮次，每轮次concurrent alloc %u次: 花费：%u ms\n",
           nworks, rounds, ntimes, std::ref(malloc_costtime));

    printf("%u个线程并发执行%u轮次，每轮次concurrent dealloc %u次: 花费：%u ms\n",
           nworks, rounds, ntimes, std::ref(free_costtime));

    printf("%u个线程并发concurrent alloc&dealloc %u次，总计花费：%u ms\n",
           nworks, nworks * rounds * ntimes, malloc_costtime + free_costtime);
}

// ntimes 一轮申请和释放内存的次数
// rounds 轮次
void BenchmarkMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
    std::vector<std::thread> vthread(nworks);
    std::atomic<size_t> malloc_costtime(0);
    std::atomic<size_t> free_costtime(0);

    for (size_t k = 0; k < nworks; ++k)
    {
        vthread[k] = std::thread([&, k]()
                                 {
			std::vector<void*> v;
			v.reserve(ntimes);

			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					//v.push_back(malloc(16));
					v.push_back(malloc((16 + i) % 8192 + 1));
				}
				size_t end1 = clock();

				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					free(v[i]);
				}
				size_t end2 = clock();
				v.clear();

				malloc_costtime += (end1 - begin1);
				free_costtime += (end2 - begin2);
			} });
    }

    for (auto &t : vthread)
    {
        t.join();
    }

    printf("%u个线程并发执行%u轮次，每轮次malloc %u次: 花费：%u ms\n",
           nworks, rounds, ntimes, std::ref(malloc_costtime));

    printf("%u个线程并发执行%u轮次，每轮次free %u次: 花费：%u ms\n",
           nworks, rounds, ntimes, std::ref(free_costtime));

    printf("%u个线程并发malloc&free %u次，总计花费：%u ms\n",
           nworks, nworks * rounds * ntimes, malloc_costtime + free_costtime);
}

int main()
{

    // TestConcurrentAlloc1();

    // std::thread thread1(MuitlThreadAlloc1);
    // std::thread thread2(MuitlThreadAlloc2);
    // thread1.join();
    // thread2.join();

    size_t n = 1000;
    cout << "==========================================================" << endl;
    BenchmarkConcurrentMalloc(n, 4, 10);
    cout << endl
         << endl;

    BenchmarkMalloc(n, 4, 10);
    cout << "==========================================================" << endl;

    // std::thread thread1(alloc1);
    // thread1.join();
    // std::thread thread2(alloc2);
    // thread2.join();

    // std::cout << sizeof(PAGE_ID) << std::endl;

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