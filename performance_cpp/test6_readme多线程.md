# 现代C++多线程详细指导教程与代码示例教学


已创建 test6.cpp，共 18 章，完整覆盖现代 C++ 多线程编程：

| 章节 | 主题          | 核心内容                                                                     |
| ---- | ------------- | ---------------------------------------------------------------------------- |
| 1    | 线程基础      | `std::thread` 创建、join/detach、RAII JoinThread                           |
| 2    | 线程传参      | 值传递、`std::ref` 引用、移动语义、返回值                                  |
| 3    | 互斥量        | `mutex` / `timed_mutex` / `recursive_mutex`、数据竞争演示              |
| 4    | 锁管理        | `lock_guard` / `unique_lock` / `scoped_lock` (C++17)                   |
| 5    | 死锁与避免    | 固定顺序、`scoped_lock`、`try_lock`、层级锁                              |
| 6    | 条件变量      | `condition_variable`、虚假唤醒、`wait_for`、`notify_all`               |
| 7    | 生产者-消费者 | 线程安全队列 `ThreadSafeQueue<T>` 完整实现                                 |
| 8    | 异步编程      | `async` / `promise` / `future` / `packaged_task` / `shared_future` |
| 9    | 原子操作      | `atomic` 全部操作、atomic vs mutex 性能、SpinLock                          |
| 10   | 内存序        | `relaxed` / `acquire-release` / `seq_cst` 详解与示例                   |
| 11   | 单例          | `call_once`、Meyer's Singleton                                             |
| 12   | 读写锁        | `shared_mutex` + `ConcurrentMap` 实现                                    |
| 13   | 线程池        | 完整 `ThreadPool`，支持 `submit()` 返回 `future`                       |
| 14   | 并行算法      | 手写 `parallel_for_each` / `parallel_reduce` / `parallel_sort`         |
| 15   | 无锁编程      | SPSC 队列 + MPMC 队列（基于 CAS）                                            |
| 16   | 并发管道      | 多阶段 Pipeline 模式                                                         |
| 17   | Map-Reduce    | 并行 Map-Reduce 框架 + 词频统计                                              |
| 18   | 陷阱与调试    | false sharing、thread_local、async 陷阱、TSan/Helgrind                       |

编译：`g++ -std=c++17 -O2 -pthread -o test6 test6.cpp`
