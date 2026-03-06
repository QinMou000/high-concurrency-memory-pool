cmake_minimum_required(VERSION 3.16)
project(high_concurrency_memory_pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(benchmark
    Benchmark.cpp
    CentralCache.cpp
    ThreadCache.cpp
    PageCache.cpp
)

target_include_directories(benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(benchmark PRIVATE Threads::Threads)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(benchmark PRIVATE
        -finput-charset=UTF-8
        -fexec-charset=GBK
    )
endif()