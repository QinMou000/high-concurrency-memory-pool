#include "ObjectPool.h"
#include "ThreadCache.h"
#include "ConcurrentAlloc.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <stdexcept>
// 单轮次申请释放次数 线程数 轮次
void BenchmarkConcurrentMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
    std::vector<std::thread> vthread(nworks);
    std::atomic<size_t> malloc_costtime{0};
    std::atomic<size_t> free_costtime{0};

    for (size_t k = 0; k < nworks; ++k)
    {
        vthread[k] = std::thread([&]()
                                 {
            std::vector<void*> v;
            v.reserve(ntimes);

            for (size_t j = 0; j < rounds; ++j) {
                // 使用高精度时钟测量实际耗时
                auto begin1 = std::chrono::high_resolution_clock::now();
                
                for (size_t i = 0; i < ntimes; i++) {

                    // void* ptr = ConcurrentAlloc(16);
                    void* ptr = ConcurrentAlloc((16 + i) % 8192 + 1);

                    if (!ptr) {
                        throw std::bad_alloc(); // 处理分配失败
                    }
                    v.push_back(ptr);
                }
                
                auto end1 = std::chrono::high_resolution_clock::now();
                auto begin2 = std::chrono::high_resolution_clock::now();
                
                for (size_t i = 0; i < ntimes; i++) {
                    ConcurrentFree(v[i]);
                }
                
                auto end2 = std::chrono::high_resolution_clock::now();
                
                // 完全释放向量内存
                v.clear();
                v.shrink_to_fit();

                // 累加耗时（毫秒）
                malloc_costtime += std::chrono::duration_cast<std::chrono::milliseconds>(end1 - begin1).count();
                free_costtime += std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count();
            } });
    }
    // 确保所有线程完成
    for (auto &t : vthread)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    // 使用类型安全的输出
    std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次concurrent alloc "
              << ntimes << "次: 花费：" << malloc_costtime << " ms" << std::endl;

    std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次concurrent dealloc "
              << ntimes << "次: 花费：" << free_costtime << " ms" << std::endl;

    std::cout << nworks << "个线程并发 concurrent alloc & dealloc " << nworks * rounds * ntimes
              << "次，总计花费：" << (malloc_costtime + free_costtime) << " ms" << std::endl;
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
        vthread[k] = std::thread([&]()
                                 {
            std::vector<void*> v;
            v.reserve(ntimes);

            for (size_t j = 0; j < rounds; ++j)
            {
                // 使用高精度时钟
                auto begin1 = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < ntimes; i++)
                {

                    // void* ptr = malloc(16);
                    void* ptr = malloc((16 + i) % 8192 + 1);

                    if (ptr == nullptr) {
                        // 处理内存分配失败
                        throw std::bad_alloc();
                    }
                    v.push_back(ptr);
                }
                auto end1 = std::chrono::high_resolution_clock::now();

                auto begin2 = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < ntimes; i++)
                {
                    free(v[i]);
                }
                auto end2 = std::chrono::high_resolution_clock::now();
                
                // 清空向量并释放内存
                v.clear();
                v.shrink_to_fit();

                // 计算耗时（毫秒）
                malloc_costtime += std::chrono::duration_cast<std::chrono::milliseconds>(end1 - begin1).count();
                free_costtime += std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count();
            } });
    }
    // 等待所有线程完成
    for (auto &t : vthread)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    // 修正输出格式
    std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次malloc "
              << ntimes << "次: 花费：" << malloc_costtime << " ms" << std::endl;

    std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次free "
              << ntimes << "次: 花费：" << free_costtime << " ms" << std::endl;

    std::cout << nworks << "个线程并发 malloc & free " << nworks * rounds * ntimes
              << "次，总计花费：" << (malloc_costtime + free_costtime) << " ms" << std::endl;
}
int main()
{
    size_t n = 10000;
    cout << "==========================================================" << endl;
    BenchmarkConcurrentMalloc(n, 4, 10);
    cout << endl;

    BenchmarkMalloc(n, 4, 10);
    cout << "==========================================================" << endl;

    void *ptr;
    cout << sizeof(ptr) << endl;  

    return 0;
}
