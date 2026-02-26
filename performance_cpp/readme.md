# 🚀 C++ 高性能编程完全教程

> **57,876 行代码 · 15 个实战教程 · 31 个文件 · 覆盖从编译期优化到内核旁路网络的全栈性能工程**

本项目是一套系统性的 C++ 高性能编程教程，以 **可编译运行的 C++ 代码 + 配套 Markdown 文档** 的形式组织，涵盖编译期优化、运行期优化、零开销抽象、多线程与高并发、无锁编程、Linux 内核机制、CPU 微架构、低延迟系统设计、网络编程到 DPDK/RDMA 高性能网络开发等领域。

---

## 📋 目录

- [🚀 C++ 高性能编程完全教程](#-c-高性能编程完全教程)
  - [📋 目录](#-目录)
  - [项目概览](#项目概览)
  - [文件规模统计](#文件规模统计)
    - [源码文件](#源码文件)
    - [文档文件](#文档文件)
  - [教程系列详解](#教程系列详解)
    - [test1 — 编译期优化基础](#test1--编译期优化基础)
    - [test2 — 编译期优化进阶](#test2--编译期优化进阶)
    - [test3 — 编译期优化进阶 (C++17)](#test3--编译期优化进阶-c17)
    - [test4 — 运行期性能优化](#test4--运行期性能优化)
    - [test5 — 零开销抽象](#test5--零开销抽象)
    - [test6 — 多线程编程](#test6--多线程编程)
    - [test7 — 网络编程](#test7--网络编程)
    - [test8 — STL 完全教程](#test8--stl-完全教程)
    - [test9 — 高并发编程](#test9--高并发编程)
    - [test10 — Linux 内核机制](#test10--linux-内核机制)
    - [test11 — 无锁编程与内存顺序](#test11--无锁编程与内存顺序)
    - [test12 — 精通现代 C++ 零开销抽象](#test12--精通现代-c-零开销抽象)
    - [test13 — 微秒低延迟系统](#test13--微秒低延迟系统)
    - [test14 — CPU 微架构深度解析](#test14--cpu-微架构深度解析)
    - [test15 — 高性能网络开发](#test15--高性能网络开发)
  - [学习路线推荐](#学习路线推荐)
    - [路线一：由浅入深全面学习](#路线一由浅入深全面学习)
    - [路线二：面向量化/HFT 快速路径](#路线二面向量化hft-快速路径)
    - [路线三：现代 C++ 语言精通](#路线三现代-c-语言精通)
    - [路线四：系统编程与并发](#路线四系统编程与并发)
  - [主题索引](#主题索引)
  - [编译与运行](#编译与运行)
    - [Linux (GCC / Clang)](#linux-gcc--clang)
    - [Windows (MSVC)](#windows-msvc)
    - [Windows (MinGW / MSYS2)](#windows-mingw--msys2)
  - [适合人群](#适合人群)
  - [项目结构](#项目结构)

---

## 项目概览

每个教程由一对文件组成：

| 文件类型                | 命名规则             | 说明                                       |
| ----------------------- | -------------------- | ------------------------------------------ |
| **C++ 源码**      | `testN.cpp`        | 可独立编译运行的完整示例，包含详细中文注释 |
| **Markdown 文档** | `testN_readme*.md` | 对应的原理讲解、图示说明、知识总结         |

所有代码以 **C++17** 为基准标准，部分教程包含 C++20/23 扩展内容（通过宏进行特性检测）。每个 `.cpp` 文件均可独立编译、独立运行，输出结构化的章节标题和演示结果。

---

## 文件规模统计

### 源码文件

| 序号 | 文件           |             行数 |    章节数    | 主题                   |
| :--: | -------------- | ---------------: | :-----------: | ---------------------- |
|  1  | test1.cpp      |              739 |      10      | 编译期优化基础         |
|  2  | test2.cpp      |            1,215 |      12      | 编译期优化进阶         |
|  3  | test3.cpp      |            1,735 |      16      | 编译期优化进阶 (C++17) |
|  4  | test4.cpp      |            1,877 |      16      | 运行期性能优化         |
|  5  | test5.cpp      |            2,040 |      19      | 零开销抽象             |
|  6  | test6.cpp      |            2,373 |      18      | 多线程编程             |
|  7  | test7.cpp      |            2,221 |      19      | 网络编程               |
|  8  | test8.cpp      |            1,973 |      20      | STL 完全教程           |
|  9  | test9.cpp      |            2,698 |      18      | 高并发编程             |
|  10  | test10.cpp     |            2,277 |      24      | Linux 内核机制         |
|  11  | test11.cpp     |            2,928 |      25      | 无锁编程与内存顺序     |
|  12  | test12.cpp     |            3,100 |      35      | 精通现代 C++           |
|  13  | test13.cpp     |            3,047 |      38      | 微秒低延迟系统         |
|  14  | test14.cpp     |            3,578 |      46      | CPU 微架构深度解析     |
|  15  | test15.cpp     |            2,697 |      35      | 高性能网络开发         |
|      | **合计** | **34,498** | **351** |                        |

### 文档文件

| 序号 | 文件                                              |             行数 | 主题             |
| :--: | ------------------------------------------------- | ---------------: | ---------------- |
|  1  | test1_readme性能优化-编译期-基础.md               |              815 | 编译期优化基础   |
|  2  | test2_readme性能优化-编译期-进阶.md               |            1,155 | 编译期优化进阶   |
|  3  | test3_readme性能优化-编译期-进阶C++17.md          |            1,537 | 编译期优化 C++17 |
|  4  | test4_readme性能优化-运行期.md                    |            1,389 | 运行期优化       |
|  5  | test5_readme零开销抽象.md                         |            1,254 | 零开销抽象       |
|  6  | test6_readme多线程.md                             |            1,433 | 多线程           |
|  7  | test7_readme网络编程.md                           |            1,789 | 网络编程         |
|  8  | test8_readmeSTL.md                                |            1,591 | STL              |
|  9  | test9_readme高并发.md                             |            2,799 | 高并发           |
|  10  | test10_linux内核机制.md                           |            1,810 | Linux 内核       |
|  11  | test11_readme无锁编程内存顺序等.md                |            2,284 | 无锁编程         |
|  12  | test12_readme精通现代C++零开销抽象与编译期优化.md |            2,047 | 现代 C++         |
|  13  | test13_readme微秒低延迟系统.md                    |            1,837 | 低延迟系统       |
|  14  | test14_readmeCPU微架构深度解析.md                 |              672 | CPU 微架构       |
|  15  | test15_readme高性能网络开发完全教程.md            |              539 | 高性能网络       |
|  16  | test15_快速参考卡片.md                            |              427 | 网络技术速查     |
|      | **合计**                                    | **23,378** |                  |

> **总计：57,876 行**，覆盖 351 个章节主题

---

## 教程系列详解

---

### test1 — 编译期优化基础

**源码:** test1.cpp (739 行, 10 章) | **文档:** test1_readme性能优化-编译期-基础.md (815 行)

从零开始掌握 C++ 编译期计算的核心技术。

| 章节 | 主题                          | 核心内容                                     |
| :--: | ----------------------------- | -------------------------------------------- |
|  1  | constexpr 基础与进阶          | constexpr 变量/函数/类，编译期 vs 运行期求值 |
|  2  | 模板元编程 (TMP)              | 递归模板计算、类型列表、编译期阶乘/斐波那契  |
|  3  | if constexpr — 编译期分支    | 条件编译、分支消除、替代 SFINAE              |
|  4  | consteval / constinit (C++20) | 强制编译期求值、静态初始化顺序控制           |
|  5  | 编译期字符串与数组处理        | 编译期字符串哈希、静态数组变换               |
|  6  | 变参模板与折叠表达式          | 参数包展开、折叠求和/打印/类型检查           |
|  7  | CRTP (奇异递归模板模式)       | 编译期多态、静态接口、零虚表开销             |
|  8  | 编译期类型萃取 (Type Traits)  | is_same、decay、conditional、自定义 traits   |
|  9  | 编译期查找表 (LUT)            | constexpr 数组初始化、O(1) 编译期查表        |
|  10  | 实战：编译期状态机            | 基于模板的有限状态机、编译期状态转移验证     |

**关键技术：** constexpr, TMP, if constexpr, fold expressions, CRTP, type traits

---

### test2 — 编译期优化进阶

**源码:** test2.cpp (1,215 行, 12 章) | **文档:** test2_readme性能优化-编译期-进阶.md (1,155 行)

在 test1 基础上深入 C++20 编译期高级技术。

| 章节 | 主题                              | 核心内容                                      |
| :--: | --------------------------------- | --------------------------------------------- |
|  1  | Concepts (C++20)                  | 概念约束、requires 表达式、替代 SFINAE        |
|  2  | 编译期反射与类型名获取            | `__PRETTY_FUNCTION__` 提取类型名            |
|  3  | 编译期正则/模式匹配               | overloaded 模式、编译期字符串匹配             |
|  4  | std::tuple 编译期操作             | tuple 遍历、map、filter、zip                  |
|  5  | 编译期排序                        | constexpr 冒泡/快排、编译期数组排序           |
|  6  | SFINAE 与 enable_if 深入          | SFINAE 原理、enable_if 技巧、与 Concepts 对比 |
|  7  | 表达式模板 (Expression Templates) | 惰性求值、消除临时对象、矩阵运算优化          |
|  8  | 编译期位操作与位域                | constexpr 位运算、编译期 popcount             |
|  9  | constexpr 内存分配 (C++20)        | 编译期 new/delete、transient allocation       |
|  10  | Policy-Based Design               | 策略模式编译期实现、组合优于继承              |
|  11  | Tag Dispatch (标签分派)           | 基于类型标签的重载选择                        |
|  12  | 编译期依赖注入与工厂              | 编译期工厂模式、零运行时开销的依赖注入        |

**关键技术：** C++20 Concepts, expression templates, Policy-Based Design, Tag Dispatch, constexpr allocation

---

### test3 — 编译期优化进阶 (C++17)

**源码:** test3.cpp (1,735 行, 16 章) | **文档:** test3_readme性能优化-编译期-进阶C++17.md (1,537 行)

严格限定 C++17 标准，不依赖 C++20，适合需要广泛编译器兼容性的项目。

| 章节 | 主题                           | 核心内容                                             |
| :--: | ------------------------------ | ---------------------------------------------------- |
|  1  | constexpr 完全指南 (C++17)     | C++17 constexpr 增强：constexpr if、constexpr lambda |
|  2  | if constexpr — 编译期分支消除 | 替代 tag dispatch 和 SFINAE 的现代方式               |
|  3  | 折叠表达式 (Fold Expressions)  | 一元/二元折叠、逗号折叠、实用模式                    |
|  4  | CTAD — 类模板参数推导         | 推导指引 (deduction guide)、简化模板使用             |
|  5  | inline constexpr 与编译期常量  | 头文件常量管理、ODR 安全                             |
|  6  | 结构化绑定与编译期             | auto [a, b, c] 解构、与 constexpr 结合               |
|  7  | std::string_view 编译期字符串  | 零拷贝字符串视图、编译期字符串处理                   |
|  8  | std::void_t 与简化 SFINAE      | 检测表达式有效性、简洁的 SFINAE 模式                 |
|  9  | constexpr lambda               | Lambda 在编译期的使用、立即调用                      |
|  10  | 变参模板高级技巧               | 递归展开、index_sequence、编译期 for_each            |
|  11  | 编译期查找表与数据结构         | constexpr std::array、编译期二分查找                 |
|  12  | CRTP 编译期多态（进阶）        | 多层 CRTP、Mixin 模式、静态接口                      |
|  13  | 编译期类型计算                 | 类型列表、类型过滤、类型去重                         |
|  14  | 编译期 EBO 与布局优化          | 空基类优化、`[[no_unique_address]]`、内存布局      |
|  15  | std::variant + std::visit 分派 | 编译期分派、多 variant 访问、状态机                  |
|  16  | 实战：编译期词法分析器         | Token 定义、constexpr 分词器                         |

**关键技术：** 纯 C++17 实现。CTAD, string_view, void_t, variant + visit, constexpr lambda, EBO

---

### test4 — 运行期性能优化

**源码:** test4.cpp (1,877 行, 16 章) | **文档:** test4_readme性能优化-运行期.md (1,389 行)

运行时性能优化的核心技术与实战。

| 章节 | 主题                          | 核心内容                                          |
| :--: | ----------------------------- | ------------------------------------------------- |
|  1  | 移动语义与完美转发            | 右值引用、std::move、universal reference          |
|  2  | 内存布局与缓存友好性          | SoA vs AoS、cache line padding、数据对齐          |
|  3  | 避免不必要的拷贝              | RVO/NRVO、引用传参、sink arguments                |
|  4  | 字符串优化 (SSO, string_view) | 小字符串优化、string_view 零拷贝                  |
|  5  | 容器选型与使用优化            | vector vs list vs deque、reserve、emplace         |
|  6  | 智能指针的性能考量            | unique_ptr 零开销、shared_ptr 引用计数成本        |
|  7  | 内联与函数调用开销            | inline 提示、`__attribute__((always_inline))`   |
|  8  | 分支预测与无分支编程          | `[[likely]]`/`[[unlikely]]`、CMOV、branchless |
|  9  | SIMD 风格优化与自动向量化     | 编译器自动向量化条件、循环展开                    |
|  10  | 内存分配优化                  | 自定义分配器、内存池、arena                       |
|  11  | 多线程与无锁编程基础          | 原子操作基础、CAS、简单自旋锁                     |
|  12  | 编译器优化提示                | `[[likely]]`, restrict, PGO 配置文件引导优化    |
|  13  | 虚函数开销与替代方案          | 虚函数表代价、CRTP/variant 替代                   |
|  14  | I/O 优化                      | 缓冲 I/O、mmap、预读取                            |
|  15  | 实战：高性能定时器            | rdtsc/chrono、Benchmark 基础设施                  |
|  16  | 实战：高性能环形缓冲区        | 无锁 ring buffer、power-of-2 mask                 |

**关键技术：** 移动语义, cache-friendly layout, SSO, 分支预测, SIMD, 内存池, PGO, ring buffer

---

### test5 — 零开销抽象

**源码:** test5.cpp (2,040 行, 19 章) | **文档:** test5_readme零开销抽象.md (1,254 行)

实践 Bjarne Stroustrup 的零开销抽象原则——"不用的不付费，用了的不可能手写得更好"。

| 章节 | 主题                             | 核心内容                              |
| :--: | -------------------------------- | ------------------------------------- |
|  1  | 零开销抽象的定义与原则           | Stroustrup 哲学、C++ 设计理念         |
|  2  | RAII — 资源管理零开销           | 自动资源管理、与 C 手动管理等价的汇编 |
|  3  | 迭代器抽象                       | 与原始指针等效的性能验证              |
|  4  | 模板 — 零开销泛型               | 模板实例化 vs 虚函数、单态化          |
|  5  | constexpr — 编译期零运行时开销  | 计算完全在编译期完成                  |
|  6  | strong typedef — 类型安全零开销 | 强类型别名、防止单位混淆              |
|  7  | std::optional                    | 替代指针/哨兵值，无堆分配             |
|  8  | std::variant                     | 替代虚函数的零堆分配方案              |
|  9  | std::span (C++20 风格)           | 零拷贝数组视图                        |
|  10  | CRTP — 编译期多态零虚表         | 静态多态 vs 动态多态的性能对比        |
|  11  | Policy-Based Design              | 可组合的零开销策略模式                |
|  12  | Expression Templates             | 消除矩阵运算临时对象                  |
|  13  | Proxy/Handle 模式                | 延迟求值、写时拷贝                    |
|  14  | 编译期状态检查                   | 类型系统做守卫、静态断言              |
|  15  | 零开销异常替代方案               | expected、error code、noexcept        |
|  16  | 实战：零开销矩阵库               | Expression Templates 矩阵运算         |
|  17  | 实战：零开销单位系统             | 编译期量纲分析                        |
|  18  | 实战：零开销有限状态机           | 类型安全 FSM                          |
|  19  | 汇编验证：证明零开销             | 对比 C++ 抽象与手写 C 的汇编输出      |

**关键技术：** RAII, strong typedef, optional/variant/span, CRTP, expression templates, 汇编级验证

---

### test6 — 多线程编程

**源码:** test6.cpp (2,373 行, 18 章) | **文档:** test6_readme多线程.md (1,433 行)

C++ 标准库多线程编程从入门到实战。

| 章节 | 主题                        | 核心内容                              |
| :--: | --------------------------- | ------------------------------------- |
|  1  | std::thread 基础            | 线程创建、join、detach                |
|  2  | 线程传参与返回值            | 值传递、引用传递、std::ref            |
|  3  | mutex 家族                  | mutex、timed_mutex、recursive_mutex   |
|  4  | 锁管理                      | lock_guard、unique_lock、scoped_lock  |
|  5  | 死锁与避免策略              | 固定加锁顺序、std::lock、层次锁       |
|  6  | condition_variable          | 等待/通知机制、虚假唤醒处理           |
|  7  | 生产者-消费者模式           | 有界/无界队列、多生产者多消费者       |
|  8  | 异步编程                    | future、promise、async、packaged_task |
|  9  | std::atomic                 | 原子操作基础、CAS 操作                |
|  10  | memory_order 详解           | 六种内存序、happens-before 关系       |
|  11  | 线程安全单例                | call_once、Meyer's Singleton、双检锁  |
|  12  | shared_mutex 读写锁         | 读多写少场景优化                      |
|  13  | 线程池实现                  | 任务队列、工作线程、优雅关停          |
|  14  | 并行算法 (C++17)            | execution policy、par/par_unseq       |
|  15  | 无锁编程 — Lock-Free Queue | 基于 CAS 的无锁队列                   |
|  16  | 实战：并发 Web 爬虫         | 多线程 URL 抓取模型                   |
|  17  | 实战：Map-Reduce 框架       | 并行映射、分区归约                    |
|  18  | 常见陷阱与调试              | 数据竞争、死锁检测、TSan              |

**关键技术：** std::thread, mutex, condition_variable, future/promise, atomic, 线程池, parallel algorithms

---

### test7 — 网络编程

**源码:** test7.cpp (2,221 行, 19 章) | **文档:** test7_readme网络编程.md (1,789 行)

跨平台 (Windows Winsock / Linux POSIX) 的网络编程完全指南。

| 章节 | 主题                     | 核心内容                   |
| :--: | ------------------------ | -------------------------- |
|  1  | 平台抽象层               | 统一 Winsock / POSIX API   |
|  2  | Socket 基础 — RAII 封装 | Socket 生命周期管理        |
|  3  | 地址封装                 | IPv4 / IPv6 统一处理       |
|  4  | TCP 客户端               | 连接、收发、协议处理       |
|  5  | TCP 服务器               | 监听、accept、并发处理     |
|  6  | UDP 通信                 | 无连接数据报、广播/组播    |
|  7  | 非阻塞 I/O 与 select     | 多路复用基础               |
|  8  | Buffer 管理              | 零拷贝缓冲设计             |
|  9  | HTTP 客户端              | 手写 HTTP/1.1 请求解析     |
|  10  | HTTP 服务器              | 简易 Web Server 实现       |
|  11  | 协议序列化               | 二进制协议编解码、TLV 格式 |
|  12  | 网络字节序               | 类型安全转换               |
|  13  | DNS 解析                 | getaddrinfo 使用           |
|  14  | 超时与错误处理           | 超时重试、错误码处理       |
|  15  | 多线程服务器             | Thread-per-Connection 模型 |
|  16  | IO 多路复用服务器        | select/poll 事件驱动       |
|  17  | 实战：简易聊天室         | 多人实时消息               |
|  18  | 实战：Redis 协议客户端   | RESP 协议解析与命令执行    |
|  19  | 网络编程最佳实践         | 调试技巧、性能优化         |

**关键技术：** 跨平台 socket, RAII, TCP/UDP, HTTP, select/poll, 二进制协议, Redis RESP

---

### test8 — STL 完全教程

**源码:** test8.cpp (1,973 行, 20 章) | **文档:** test8_readmeSTL.md (1,591 行)

C++ 标准模板库的全面覆盖，按容器→迭代器→算法→函数对象→实战的脉络组织。

| 篇章                 | 章节 | 主题                                                    |
| -------------------- | :--: | ------------------------------------------------------- |
| **容器篇**     |  1  | 序列容器：vector / array / deque / list / forward_list  |
|                      |  2  | 关联容器：set / map / multiset / multimap               |
|                      |  3  | 无序容器：unordered_set / unordered_map                 |
|                      |  4  | 容器适配器：stack / queue / priority_queue              |
|                      |  5  | 特殊容器：string / string_view / bitset / valarray      |
|                      |  6  | C++17 更新：节点操作、try_emplace、insert_or_assign     |
| **迭代器篇**   |  7  | 迭代器分类与特征                                        |
|                      |  8  | 迭代器适配器：reverse / insert / stream / move          |
| **算法篇**     |  9  | 非修改算法：find / count / search / mismatch            |
|                      |  10  | 修改算法：copy / transform / replace / remove           |
|                      |  11  | 排序与搜索：sort / nth_element / binary_search          |
|                      |  12  | 集合算法：set_union / set_intersection / set_difference |
|                      |  13  | 数值算法：accumulate / inner_product / partial_sum      |
|                      |  14  | 最值与排列：min/max_element / next_permutation          |
| **函数对象篇** |  15  | std::function / std::bind / Lambda                      |
|                      |  16  | std::optional / std::variant / std::any                 |
|                      |  17  | std::tuple 与结构化绑定                                 |
|                      |  18  | 智能指针：unique_ptr / shared_ptr / weak_ptr            |
| **实战篇**     |  19  | STL 组合拳解决实际问题                                  |
|                      |  20  | STL 性能陷阱与最佳实践                                  |

**关键技术：** 全部 STL 容器/迭代器/算法/函数对象, C++17 新特性, 智能指针, 性能最佳实践

---

### test9 — 高并发编程

**源码:** test9.cpp (2,698 行, 18 章) | **文档:** test9_readme高并发.md (2,799 行)

在 test6 (多线程基础) 之上，聚焦生产级高并发数据结构与架构模式。

| 章节 | 主题                     | 核心内容                                  |
| :--: | ------------------------ | ----------------------------------------- |
|  1  | 并发基础设施             | CAS / 内存序 / 缓存行填充 / 对齐          |
|  2  | 无锁栈 (Lock-Free Stack) | Treiber Stack、ABA 问题处理               |
|  3  | 无锁 MPMC 有界队列       | Vyukov MPMC 算法                          |
|  4  | 并发哈希表               | 分段锁 (Striped Lock) HashMap             |
|  5  | 高性能线程池             | 任务窃取 (Work-Stealing)                  |
|  6  | Future/Promise 链式调用  | 异步组合、then/when_all                   |
|  7  | Actor 模型               | 消息传递、隔离状态                        |
|  8  | Pipeline 并行流水线      | 多阶段流水线处理                          |
|  9  | Fork-Join 并行分治       | 递归分治、任务合并                        |
|  10  | 并行 MapReduce           | 分布式计算框架简化版                      |
|  11  | 背压与限流               | Backpressure、Token Bucket / Leaky Bucket |
|  12  | 读写锁优化               | SeqLock / 无锁读计数器                    |
|  13  | 定时器轮 (Timer Wheel)   | O(1) 定时器管理                           |
|  14  | 对象池 / 连接池          | 资源复用、避免频繁分配                    |
|  15  | 并发 LRU Cache           | 线程安全的缓存淘汰                        |
|  16  | 优雅关停                 | Graceful Shutdown 模式                    |
|  17  | 协程式并发 (C++20)       | Coroutines 概念入门                       |
|  18  | 高并发最佳实践           | 性能调优检查清单                          |

**关键技术：** 无锁栈/队列, work-stealing, Actor, Pipeline, Fork-Join, 背压, Timer Wheel, 对象池, LRU Cache

---

### test10 — Linux 内核机制

**源码:** test10.cpp (2,277 行, 24 章) | **文档:** test10_linux内核机制.md (1,810 行)

从用户态视角深入理解 Linux 内核核心子系统，用 C++ 模拟内核算法。

| 篇章                        | 章节 | 主题                                         |
| --------------------------- | :--: | -------------------------------------------- |
| **进程调度篇**        |  1  | CFS 完全公平调度器原理与模拟                 |
|                             |  2  | 实时调度策略 (SCHED_FIFO / RR / DEADLINE)    |
|                             |  3  | CPU 亲和性与 NUMA 感知                       |
|                             |  4  | cgroups CPU 限制原理                         |
| **内存管理篇**        |  5  | 虚拟内存与分页机制                           |
|                             |  6  | 伙伴系统 (Buddy System) 模拟                 |
|                             |  7  | SLAB/SLUB 分配器原理                         |
|                             |  8  | mmap 与零拷贝 (Zero-Copy)                    |
|                             |  9  | 内存映射 I/O 与 DMA 概念                     |
|                             |  10  | OOM Killer 与内存压力处理                    |
| **文件系统与 I/O 篇** |  11  | VFS 虚拟文件系统抽象                         |
|                             |  12  | Page Cache 与 I/O 调度                       |
|                             |  13  | io_uring 异步 I/O (Linux 5.1+)               |
| **网络栈篇**          |  14  | TCP/IP 协议栈分层架构                        |
|                             |  15  | Socket 缓冲区 (sk_buff) 原理                 |
|                             |  16  | Netfilter / iptables 钩子链                  |
|                             |  17  | epoll 内核实现原理                           |
|                             |  18  | 零拷贝网络：sendfile / splice                |
| **设备驱动篇**        |  19  | Linux 设备模型 (字符/块/网络设备)            |
|                             |  20  | 中断处理：上半部/下半部                      |
| **内核同步篇**        |  21  | 内核同步原语：spinlock, rwlock, RCU, seqlock |
| **综合实战篇**        |  22  | /proc 与 /sys 信息采集                       |
|                             |  23  | 性能分析工具链 (perf, ftrace, bpf)           |
|                             |  24  | 内核调优参数速查手册                         |

**关键技术：** CFS 模拟, Buddy System, SLAB/SLUB, io_uring, epoll 内部原理, RCU, perf/ftrace/bpf

---

### test11 — 无锁编程与内存顺序

**源码:** test11.cpp (2,928 行, 25 章) | **文档:** test11_readme无锁编程内存顺序等.md (2,284 行)

C++ 无锁编程的终极深入教程，从原子操作到工业级并发数据结构。

| 篇章                         | 章节 | 主题                                          |
| ---------------------------- | :--: | --------------------------------------------- |
| **原子操作基础篇**     |  1  | std::atomic 基础：load/store/exchange/CAS     |
|                              |  2  | 原子类型完全手册（整型/指针/浮点/atomic_ref） |
|                              |  3  | fetch_add / fetch_or 等 RMW 操作              |
|                              |  4  | CAS 循环模式与 ABA 问题                       |
| **内存顺序篇**         |  5  | CPU 指令重排与编译器重排                      |
|                              |  6  | 六种 memory_order 逐一精讲                    |
|                              |  7  | Acquire-Release：生产者-消费者详解            |
|                              |  8  | Release Sequence 与传递性                     |
|                              |  9  | memory_order_consume 与数据依赖               |
|                              |  10  | volatile vs atomic 本质区别                   |
|                              |  11  | fence 内存屏障独立使用                        |
| **无锁算法篇**         |  12  | 自旋锁三种实现（TAS / TTAS / Ticket Lock）    |
|                              |  13  | 无锁栈 (Treiber Stack)                        |
|                              |  14  | 无锁队列 (Michael-Scott Queue)                |
|                              |  15  | 无锁哈希表（分桶 + 原子链表）                 |
|                              |  16  | 无锁环形缓冲区 (SPSC / MPMC)                  |
| **内存回收篇**         |  17  | Hazard Pointer 详解与实现                     |
|                              |  18  | Epoch-Based Reclamation (EBR)                 |
|                              |  19  | 引用计数回收 (split reference count)          |
| **高级并发数据结构篇** |  20  | SeqLock 序列锁完整实现                        |
|                              |  21  | Read-Copy-Update (RCU) 用户态模拟             |
|                              |  22  | 并发跳表 (Concurrent Skip List)               |
| **实战与调试篇**       |  23  | 常见 Bug 模式与修复                           |
|                              |  24  | 性能基准：锁 vs 无锁 vs 分片                  |
|                              |  25  | 工具链：TSan / ASAN / Relacy / CDSChecker     |

**关键技术：** 全部 6 种 memory_order, Treiber Stack, Michael-Scott Queue, Hazard Pointer, EBR, RCU, 并发跳表

---

### test12 — 精通现代 C++ 零开销抽象

**源码:** test12.cpp (3,100 行, 35 章) | **文档:** test12_readme精通现代C++零开销抽象与编译期优化.md (2,047 行)

横跨 C++17/20/23 三代标准，系统梳理现代 C++ 语言特性与零开销设计哲学。

| 篇章                   | 章节范围 | 主题                                                                                                     |
| ---------------------- | :------: | -------------------------------------------------------------------------------------------------------- |
| **C++17 核心**   |   1-8   | 结构化绑定, if constexpr, 折叠表达式, CTAD, optional/variant/any, string_view, inline 变量, invoke/apply |
| **C++20 核心**   |   9-15   | Concepts, Ranges,`<=>`, consteval/constinit, Coroutines/Generator, std::format, Modules                |
| **C++23 新特性** |  16-20  | Deducing this, std::expected, std::print, if consteval, mdspan                                           |
| **零开销抽象**   |  21-25  | CRTP, Policy-Based Design, Strong Types, Type Erasure, variant vs virtual                                |
| **编译期计算**   |  26-30  | constexpr deep dive, 编译期容器, type list / 编译期 Map, 编译期字符串, 编译期正则/状态机                 |
| **高级模板**     |  31-35  | SFINAE→Concepts 演进, 变参模板高级, 完美转发陷阱, Lambda C++14→23 演进, 性能对比与最佳实践             |

> 使用 `HAS_CPP20` / `HAS_CPP23` 宏进行特性检测，向下兼容 C++17。

**关键技术：** C++17/20/23 全特性, Concepts, Ranges, Coroutines, Modules, Type Erasure, expected, mdspan

---

### test13 — 微秒低延迟系统

**源码:** test13.cpp (3,047 行, 38 章) | **文档:** test13_readme微秒低延迟系统.md (1,837 行)

面向高频交易 (HFT) / 量化交易场景的极致低延迟工程实践。

| 篇章                   | 章节范围 | 主题                                                                       |
| ---------------------- | :------: | -------------------------------------------------------------------------- |
| **延迟测量**     |   1-3   | rdtsc/rdtscp 精确计时、延迟直方图、热路径识别                              |
| **内存子系统**   |   4-9   | Cache line 对齐、prefetch、huge pages、对象池、arena allocator、SoA vs AoS |
| **CPU 与调度**   |  10-14  | Core pinning、busy wait、自旋锁退避、NUMA 感知、likely/unlikely/branchless |
| **无锁数据结构** |  15-18  | 生产级 SPSC 队列、MPSC 队列、SeqLock、无锁内存池                           |
| **网络 I/O**     |  19-23  | 内核旁路概述、raw socket、TCP_NODELAY、UDP 组播、NIC 硬件时间戳            |
| **编译器优化**   |  24-28  | Compiler barrier、SIMD SSE/AVX、内联汇编、PGO、LTO                         |
| **系统级调优**   |  29-33  | SCHED_FIFO、IRQ 隔离、sysctl 调优、CPU 频率锁定、BIOS 调优                 |
| **架构设计**     |  34-38  | 单线程事件循环、流水线架构、冷热路径分离、预计算/LUT、全系统集成           |

> 编译推荐：`-fno-exceptions -fno-rtti -march=native -O3`

**关键技术：** rdtsc, huge pages, SPSC queue, kernel bypass, SIMD, PGO/LTO, SCHED_FIFO, IRQ affinity, 事件驱动架构

---

### test14 — CPU 微架构深度解析

**源码:** test14.cpp (3,578 行, 46 章) ⭐ 最大文件 | **文档:** test14_readmeCPU微架构深度解析.md (672 行)

最硬核的一篇——从 CPU 流水线到乱序执行引擎，深入处理器微架构。

| 篇章                     | 章节范围 | 主题                                                                                                        |
| ------------------------ | :------: | ----------------------------------------------------------------------------------------------------------- |
| **流水线基础**     |   1-4   | 5 级流水线、超标量/多发射、流水线冒险、气泡测量                                                             |
| **缓存层级**       |   5-13   | L1/L2/L3 延迟、cache line、组相联、替换策略、MESI/MOESI、false sharing 深度分析、缓存友好数据结构、缓存抖动 |
| **TLB 与虚拟内存** |  14-16  | TLB 层级、huge pages TLB 影响、PCID                                                                         |
| **分支预测**       |  17-24  | BHT/BTB、静态/动态预测、2-bit 计数器、TAGE 预测器、间接跳转、误预测代价测量、branchless 编码、likely/PGO    |
| **乱序执行**       |  25-32  | OoO 引擎、寄存器重命名/RAT、保留站、ROB、store buffer、执行端口、ILP 最大化、循环依赖                       |
| **前端优化**       |  33-36  | I-cache、μop cache/DSB、LSD、代码对齐                                                                      |
| **性能分析**       |  37-40  | PMU 硬件计数器、perf Top-Down 分析、VTune/μProf、优化检查清单                                              |
| **扩展专题**       |  41-46  | Spectre/Meltdown、SMT/超线程、Apple Silicon、硬件预取、SIMD/AVX、NUMA                                       |

> 包含 Intel / AMD / Apple Silicon 三大平台的实际延迟数据。

**关键技术：** 流水线, cache hierarchy, 分支预测器模型, OoO 引擎(ROB/保留站/store buffer), μop cache, perf Top-Down, Spectre

---

### test15 — 高性能网络开发

**源码:** test15.cpp (2,697 行, 35 章) | **文档:** test15_readme高性能网络开发完全教程.md (539 行) + test15_快速参考卡片.md (427 行)

面向量化交易和超低延迟场景的高性能网络技术栈。

| 篇章                        | 章节范围 | 主题                                                                                  |
| --------------------------- | :------: | ------------------------------------------------------------------------------------- |
| **网络性能基础**      |   1-5   | 传统协议栈延迟分析、内核旁路对比、零拷贝原则、DMA/ring buffer、轮询 vs 中断           |
| **DPDK 核心**         |   6-14   | DPDK 架构、环境初始化、Mempool、Mbuf、PMD 驱动、RX/TX、多核并行、RSS、Flow Director   |
| **Solarflare/Xilinx** |  15-19  | OpenOnload、ef_vi 零拷贝 API、Onload 加速、PTP 时间戳、TCPDirect                      |
| **RDMA**              |  20-25  | RDMA 概念、InfiniBand/RoCE/iWARP、Verbs API、单边 WRITE/READ、双边 SEND/RECV、RDMA CM |
| **高级网络**          |  26-30  | XDP、AF_XDP、io_uring 网络、SmartNIC/DPU、FPGA NIC 加速                               |
| **性能优化**          |  31-35  | 延迟测量、多队列 NIC、CPU 绑核/IRQ 亲和、内存池/零分配设计、生产调优清单              |

**快速参考卡片** 提供延迟对比表：

| 技术        | 延迟        | 吞吐量      |
| ----------- | ----------- | ----------- |
| 传统 Socket | 5-15 μs    | 1-2 Mpps    |
| OpenOnload  | 2-5 μs     | 5-10 Mpps   |
| DPDK        | 1-3 μs     | 10-100 Mpps |
| RDMA        | 1-2 μs     | 10-200 Mpps |
| FPGA        | 0.1-0.5 μs | 10-100 Mpps |

> 支持无 DPDK 库时的模拟模式编译。

**关键技术：** DPDK, OpenOnload/ef_vi, RDMA Verbs, XDP/AF_XDP, io_uring, SmartNIC/DPU, FPGA

---

## 学习路线推荐

### 路线一：由浅入深全面学习

```
test1 (编译期基础) → test2 (编译期进阶) → test3 (C++17 编译期)
    → test4 (运行期优化) → test5 (零开销抽象)
    → test8 (STL) → test12 (现代 C++ 总览)
    → test6 (多线程) → test9 (高并发) → test11 (无锁编程)
    → test7 (网络编程) → test10 (Linux 内核)
    → test14 (CPU 微架构) → test13 (低延迟系统) → test15 (高性能网络)
```

### 路线二：面向量化/HFT 快速路径

```
test4 (运行期优化) → test14 (CPU 微架构) → test13 (低延迟系统)
    → test11 (无锁编程) → test9 (高并发) → test15 (高性能网络)
```

### 路线三：现代 C++ 语言精通

```
test1 (编译期基础) → test3 (C++17) → test2 (C++20)
    → test12 (C++17/20/23 全览) → test5 (零开销抽象) → test8 (STL)
```

### 路线四：系统编程与并发

```
test6 (多线程) → test9 (高并发) → test11 (无锁编程)
    → test10 (Linux 内核) → test7 (网络编程) → test15 (高性能网络)
```

---

## 主题索引

快速查找特定技术主题所在的教程：

| 主题                                      | 教程                               |
| ----------------------------------------- | ---------------------------------- |
| **constexpr / 编译期计算**          | test1, test2, test3, test5, test12 |
| **模板元编程 (TMP)**                | test1, test2, test3, test12        |
| **Concepts (C++20)**                | test2, test12                      |
| **CRTP**                            | test1, test3, test5, test12        |
| **Expression Templates**            | test2, test5                       |
| **移动语义 / 完美转发**             | test4, test12                      |
| **内存布局 / Cache 优化**           | test4, test13, test14              |
| **分支预测 / Branchless**           | test4, test13, test14              |
| **SIMD / AVX**                      | test4, test13, test14              |
| **RAII**                            | test5, test7                       |
| **std::optional / variant / span**  | test5, test8, test12               |
| **std::thread / mutex**             | test6                              |
| **future / promise / async**        | test6, test9                       |
| **线程池**                          | test6, test9                       |
| **无锁编程 (Lock-Free)**            | test6, test9, test11, test13       |
| **原子操作 / memory_order**         | test6, test9, test11               |
| **Hazard Pointer / EBR / RCU**      | test11                             |
| **自旋锁 / SeqLock**                | test9, test11, test13              |
| **Socket / TCP / UDP**              | test7                              |
| **HTTP 实现**                       | test7                              |
| **select / poll / epoll**           | test7, test10                      |
| **STL 容器 / 算法**                 | test8                              |
| **智能指针**                        | test6, test8                       |
| **Producer-Consumer**               | test6, test9                       |
| **Actor / Pipeline / Fork-Join**    | test9                              |
| **背压 / 限流**                     | test9                              |
| **CFS / 进程调度**                  | test10                             |
| **虚拟内存 / Buddy System**         | test10                             |
| **io_uring**                        | test10, test15                     |
| **epoll 内核实现**                  | test10                             |
| **perf / ftrace / bpf**             | test10, test14                     |
| **Coroutines (C++20)**              | test9, test12                      |
| **Ranges (C++20)**                  | test12                             |
| **C++23 (expected, mdspan)**        | test12                             |
| **rdtsc / 纳秒计时**                | test13, test14                     |
| **Huge Pages**                      | test13, test14                     |
| **PGO / LTO**                       | test4, test13, test14              |
| **CPU 流水线 / 乱序执行**           | test14                             |
| **Cache Line / MESI 协议**          | test14                             |
| **ROB / 保留站 / Store Buffer**     | test14                             |
| **μop Cache / DSB / LSD**          | test14                             |
| **Spectre / Meltdown**              | test14                             |
| **DPDK**                            | test15                             |
| **Solarflare / OpenOnload / ef_vi** | test15                             |
| **RDMA / InfiniBand / RoCE**        | test15                             |
| **XDP / AF_XDP**                    | test15                             |
| **SmartNIC / DPU / FPGA**           | test15                             |

---

## 编译与运行

### Linux (GCC / Clang)

```bash
# 通用编译命令（适用于 test1-test12, test14）
g++ -std=c++17 -O2 -march=native -pthread testN.cpp -o testN
./testN

# 低延迟编译（test13 推荐）
g++ -std=c++17 -O3 -march=native -fno-exceptions -fno-rtti -pthread test13.cpp -o test13

# 网络编程（test7）
g++ -std=c++17 -O2 -pthread test7.cpp -o test7

# 高性能网络（test15，模拟模式，无需 DPDK 库）
g++ -std=c++17 -O2 -march=native -pthread test15.cpp -o test15
```

### Windows (MSVC)

```cmd
cl /std:c++17 /O2 /EHsc /arch:AVX2 testN.cpp
testN.exe
```

### Windows (MinGW / MSYS2)

```bash
g++ -std=c++17 -O2 -march=native -pthread -lws2_32 testN.cpp -o testN.exe
```

> **注意：**
>
> - test10 (Linux 内核机制) 中部分章节使用 Linux-only API，Windows 下提供 fallback stub
> - test15 (DPDK/RDMA) 在无对应库时以模拟模式编译运行
> - 所有文件均可独立编译，无跨文件依赖

---

## 适合人群

| 人群                                  | 推荐教程                             |
| ------------------------------------- | ------------------------------------ |
| **C++ 初中级开发者**            | test1 → test4 → test8 → test6     |
| **想系统学习现代 C++ 的开发者** | test1 → test3 → test12 → test5    |
| **后端/服务端开发工程师**       | test6 → test9 → test7 → test10    |
| **量化/HFT 工程师**             | test13 → test14 → test11 → test15 |
| **系统/内核方向工程师**         | test10 → test14 → test11 → test13 |
| **对无锁编程感兴趣的开发者**    | test6 → test9 → test11             |
| **网络开发工程师**              | test7 → test10 → test15            |

---

## 项目结构

```
performance_cpp/
├── readme.md                                          ← 本文件
├── test1.cpp  / test1_readme性能优化-编译期-基础.md          ← 编译期优化基础
├── test2.cpp  / test2_readme性能优化-编译期-进阶.md          ← 编译期优化进阶
├── test3.cpp  / test3_readme性能优化-编译期-进阶C++17.md     ← 编译期优化 C++17
├── test4.cpp  / test4_readme性能优化-运行期.md               ← 运行期优化
├── test5.cpp  / test5_readme零开销抽象.md                    ← 零开销抽象
├── test6.cpp  / test6_readme多线程.md                        ← 多线程
├── test7.cpp  / test7_readme网络编程.md                      ← 网络编程
├── test8.cpp  / test8_readmeSTL.md                           ← STL 完全教程
├── test9.cpp  / test9_readme高并发.md                        ← 高并发
├── test10.cpp / test10_linux内核机制.md                      ← Linux 内核机制
├── test11.cpp / test11_readme无锁编程内存顺序等.md           ← 无锁编程
├── test12.cpp / test12_readme精通现代C++零开销抽象与编译期优化.md ← 现代 C++
├── test13.cpp / test13_readme微秒低延迟系统.md           ← 低延迟系统
├── test14.cpp / test14_readmeCPU微架构深度解析.md            ← CPU 微架构
├── test15.cpp / test15_readme高性能网络开发完全教程.md       ← 高性能网络
└── test15_快速参考卡片.md                                    ← 网络技术速查卡
```

---

> **本教程系列持续更新中。** 所有代码均经过编译验证，包含丰富的中文注释和运行输出，适合边读边跑、边学边练。
