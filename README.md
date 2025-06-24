



 

# 高并发内存池项目

## 项目简介

本项目旨在实现一个高并发的内存池，其原型参考了`google`的开源项目[tcmalloc](https://github.com/google/tcmalloc)，[tcmalloc](https://gitee.com/mirrors/tcmalloc)全称为`Thread-Caching Malloc`，即线程缓存的`malloc`，它实现了高效的多线程内存管理，可用于替代系统的内存分配相关函数（如`malloc`、`free`）。本项目将`tcmalloc`最核心的框架进行简化，模拟实现了一个属于自己的高并发内存池，主要目的是学习`tcmalloc`的精华。

## 项目结构

### 主要文件及功能

1. CentralCache.cpp 和 CentralCache.h
   - `CentralCache`类为单例模式，负责管理中心缓存。
   - `FetchRangeObj`函数：从中心缓存获取指定数量和大小的内存块。
   - `GetOneSpan`函数：获取一个非空的`Span`。
   - `ReleaseListToSpans`函数：将从`ThreadCache`回收的内存块放回对应的`Span`中。
2. ThreadCache.cpp
   - `ThreadCache`类管理线程本地的缓存。
   - `FetchFromCentralCache`函数：当本地缓存不足时，从中心缓存获取内存块。
   - `Alloc`函数：分配内存。
   - `DeAlloc`函数：释放内存。
   - `ListTooLong`函数：当本地缓存中的某个自由链表内存过多时，将部分内存释放回中心缓存。
3. PageCache.cpp
   - `PageCache`类为单例模式，负责管理页缓存。
   - `NewSpan`函数：获取一个指定页数的`Span`。
   - `MapObjectToSpan`函数：根据对象地址查找其所属的`Span`。
   - `ReleaseSpanToPageCache`函数：将`Span`释放回页缓存，并尝试进行前后页的合并。
4. Benchmark.cpp
   - 包含性能测试函数`BenchmarkConcurrentMalloc`和`BenchmarkMalloc`，用于比较本内存池和系统`malloc`、`free`函数在多线程环境下的性能。
5. test.cpp
   - 包含多个测试函数，如`TestConcurrentAlloc1`、`TestConcurrentAlloc2`等，用于测试内存池的基本功能。
6. Common.h
   - 定义了一些通用的常量、函数和类，如`FreeList`、`SizeClass`、`Span`和`SpanList`等，这些是内存池实现的基础。
7. ObjectPool.h
   - 实现了一个对象池模板类`ObjectPool`，可用于高效地分配和释放对象。

### .gitignore 文件

该文件指定了在版本控制中需要忽略的文件和文件夹，包括编译生成的文件（如`.o`、`.exe`等）、日志文件（`.log`）和 JSON 文件（`.json`）等。

## 编译和运行

### 编译

本项目使用标准的 C++ 编译工具进行编译，确保你的编译器支持 C++11 及以上标准。可以使用以下命令进行编译：

```sh
g++ -std=c++11 -pthread Benchmark.cpp CentralCache.cpp ThreadCache.cpp PageCache.cpp -o benchmark
```

### 运行

编译成功后，运行生成的可执行文件：

```sh
./benchmark
```

## 性能测试

在Benchmark.cpp文件中，通过`BenchmarkConcurrentMalloc`和`BenchmarkMalloc`函数对本内存池和系统`malloc`、`free`函数进行了性能比较。可以根据需要调整测试参数（如`ntimes`、`nworks`、`rounds`）来测试不同场景下的性能。

## 注意事项

- 本项目在实现过程中使用了多线程和锁机制，确保在高并发环境下的线程安全。
- 代码中包含了一些调试信息和条件断点，可以在开发和调试过程中使用。
- 对于大内存的分配（超过 128 页），会直接调用系统的内存分配函数。

## 总结

通过本项目的实现，我们深入学习了`tcmalloc`的核心原理，实现了一个高并发的内存池，并通过性能测试验证了其在多线程环境下的优势。该内存池可以有效地减少内存碎片，提高内存分配和释放的效率。
