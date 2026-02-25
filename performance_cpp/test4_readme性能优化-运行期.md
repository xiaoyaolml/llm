# 现代C++性能优化详细指导教程与代码示例教学

> The user wants a detailed tutorial on modern C++ performance optimization with code examples, to be written in test4.cpp. This should cover runtime performance optimization techniques (as opposed to the compile-time optimization tutorials already created). Let me create a comprehensive tutorial.

已在 test4.cpp 中创建完整的**现代 C++ 运行期性能优化教程**，共 **16 章**，每章都有详细讲解 + 可运行的基准测试。

---

### 教程目录

| 章 | 主题                         | 关键内容                                               |
| -- | ---------------------------- | ------------------------------------------------------ |
| 1  | **移动语义与完美转发** | noexcept 重要性、sink parameter、RVO/NRVO              |
| 2  | **内存布局与缓存**     | AoS vs SoA 基准测试、padding 优化、false sharing       |
| 3  | **避免不必要的拷贝**   | const&、emplace、auto& 陷阱                            |
| 4  | **字符串优化**         | SSO 检测、string_view 零拷贝、拼接优化                 |
| 5  | **容器选型**           | reserve、sorted vector vs set 基准、try_emplace        |
| 6  | **智能指针**           | unique_ptr 零开销证明、shared_ptr 开销分析、基准测试   |
| 7  | **内联与回调**         | FORCE_INLINE、函数指针 vs std::function vs 模板 lambda |
| 8  | **分支预测**           | 有序 vs 随机数据、无分支 abs/min/max、[[likely]]       |
| 9  | **SIMD 自动向量化**    | __restrict、对齐分配、循环展开                         |
| 10 | **内存池**             | 自定义 MemoryPool vs new/delete 基准测试               |
| 11 | **多线程无锁**         | false sharing 演示、SpinLock、无锁栈                   |
| 12 | **编译器优化提示**     | PGO 流程、全部编译选项速查                             |
| 13 | **虚函数替代**         | virtual vs CRTP vs variant 三方基准对比                |
| 14 | **I/O 优化**           | sync_with_stdio、批量输出、endl 陷阱                   |
| 15 | **性能测量**           | 统计定时器(中位数)、吞吐量计算、rdtsc                  |
| 16 | **SPSC 环形缓冲区**    | 无锁、2的幂优化、cache line 对齐                       |

### 编译

```bash
g++ -std=c++17 -O2 -pthread -o test4 test4.cpp && ./test4
cl /std:c++17 /O2 /EHsc test4.cpp
```

### 四个文件定位

| 文件                | 主题                                  |
| ------------------- | ------------------------------------- |
| test1.cpp           | 编译期优化基础                        |
| test2.cpp           | 编译期优化进阶 (C++20)                |
| test3.cpp           | C++17 编译期专项                      |
| **test4.cpp** | **运行期性能优化** (附基准测试) |
