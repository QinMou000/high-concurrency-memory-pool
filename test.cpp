// #include "ObjectPool.h"
// #include "ThreadCache.h"
// #include "ConcurrentAlloc.h"
// #include <thread>
// #include <atomic>
// #include <chrono>
// #include <cstdlib>
// #include <stdexcept>

// // void alloc1()
// // {
// //     for (size_t i = 0; i < 5; ++i)
// //     {
// //         void *ptr = ConcurrentAlloc(2);
// //     }
// // }

// // void alloc2()
// // {
// //     for (size_t i = 0; i < 5; ++i)
// //     {
// //         void *ptr = ConcurrentAlloc(23);
// //     }
// // }

// void TestConcurrentAlloc1()
// {
//     void *ptr1 = ConcurrentAlloc(6);
//     void *ptr2 = ConcurrentAlloc(1);
//     void *ptr3 = ConcurrentAlloc(5);
//     void *ptr4 = ConcurrentAlloc(7);
//     void *ptr5 = ConcurrentAlloc(1);
//     void *ptr6 = ConcurrentAlloc(2);
//     void *ptr7 = ConcurrentAlloc(2);

//     cout << ptr1 << endl;
//     cout << ptr2 << endl;
//     cout << ptr3 << endl;
//     cout << ptr4 << endl;
//     cout << ptr5 << endl;
//     cout << ptr6 << endl;
//     cout << ptr7 << endl;

//     ConcurrentFree(ptr1);
//     ConcurrentFree(ptr2);
//     ConcurrentFree(ptr3);
//     ConcurrentFree(ptr4);
//     ConcurrentFree(ptr5);
//     ConcurrentFree(ptr6);
//     ConcurrentFree(ptr7);
// }
// void TestConcurrentAlloc2()
// {
//     for (int i = 0; i < 1024; i++)
//     {
//         void *ptr1 = ConcurrentAlloc(6);
//         cout << ptr1 << endl;
//     }
//     void *ptr2 = ConcurrentAlloc(6);
//     cout << ptr2 << endl;
// }

// void MuitlThreadAlloc1()
// {
//     std::vector<void *> v;
//     for (size_t i = 0; i < 10; ++i)
//     {
//         void *ptr = ConcurrentAlloc(2);
//         v.push_back(ptr);
//     }
//     for (auto e : v)
//     {
//         ConcurrentFree(e);
//     }
// }

// void MuitlThreadAlloc2()
// {
//     std::vector<void *> v;
//     for (size_t i = 0; i < 10; ++i)
//     {
//         void *ptr = ConcurrentAlloc(15);
//         v.push_back(ptr);
//     }
//     for (auto e : v)
//     {
//         ConcurrentFree(e);
//     }
// }

// void BigAlloc()
// {
//     void *p1 = ConcurrentAlloc(257 * 1024);
//     void *p2 = ConcurrentAlloc(129 * 8 * 1024);

//     ConcurrentFree(p1);
//     ConcurrentFree(p2);
// }
// // int main()
// // {
//     // BigAlloc();

//     // TestConcurrentAlloc1();

//     // std::thread thread1(MuitlThreadAlloc1);
//     // std::thread thread2(MuitlThreadAlloc2);
//     // thread1.join();
//     // thread2.join();

//     // std::thread thread1(alloc1);
//     // thread1.join();
//     // std::thread thread2(alloc2);
//     // thread2.join();

//     // std::cout << sizeof(PAGE_ID) << std::endl;

//     // TestObjectPool();
//     // cout << SizeClass::RoundUp(2) << endl;
//     // cout << SizeClass::RoundUp(5) << endl;
//     // cout << SizeClass::RoundUp(9) << endl;
//     // cout << SizeClass::RoundUp(129) << endl;
//     // cout << SizeClass::RoundUp(1025) << endl;

//     // cout << SizeClass::Index(2) << endl;
//     // cout << SizeClass::Index(5) << endl;
//     // cout << SizeClass::Index(9) << endl;
//     // cout << SizeClass::Index(129) << endl;
//     // cout << SizeClass::Index(1025) << endl;

// //     return 0;
// // }