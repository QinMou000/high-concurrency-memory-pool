当前项目是实现⼀个高并发的内存池，它原型是`google`的⼀个开源项目[tcmalloc](https://github.com/google/tcmalloc)，[tcmalloc](https://gitee.com/mirrors/tcmalloc)全称`Thread-Caching Malloc`，即线程缓存的`malloc`，实现了高效的多线程内存管理，用于替代系统的内存分配相关的函数（malloc、free）。把`tcmalloc`最核⼼的框架简化后拿出来，模拟实现出⼀个自己的高并发内存池，目的就是学习`tcamlloc`的精华



 
