# C++编译期优化详细指导教程与代码示例教学

>
> The user wants a C++ compile-time optimization tutorial with detailed guidance and code examples, this time in test2.cpp. I already created a comprehensive tutorial in test.cpp previously. Let me create a different, complementary tutorial in test2.cpp that covers more advanced topics or different aspects to avoid duplication.
>
> Let me check what's in test2.cpp first.


已在 test2.cpp 中创建完整的 **C++ 编译期优化进阶教程**，与 test.cpp 互补，覆盖 **12 个高级主题**。

---

### 教程目录

| 章节 | 主题                                 | 核心技术                                                         |
| ---- | ------------------------------------ | ---------------------------------------------------------------- |
| 1    | **Concepts (C++20)**           | 概念定义、约束语法、组合概念、重载决议                           |
| 2    | **编译期反射**                 | `__PRETTY_FUNCTION__` / `__FUNCSIG__` 获取类型名、类型ID哈希 |
| 3    | **编译期模式匹配**             | `overloaded` 模式、类型偏特化匹配                              |
| 4    | **Tuple 编译期操作**           | 遍历、变换(map)、过滤、反转、类型查找                            |
| 5    | **编译期排序**                 | constexpr 冒泡排序、快速排序、二分查找                           |
| 6    | **SFINAE 深入**                | `enable_if`、`void_t` 检测成员/运算符、与 Concepts 对比      |
| 7    | **表达式模板**                 | CRTP + 惰性求值，消除临时对象（线性代数库核心技术）              |
| 8    | **编译期位操作**               | popcount、clz、log2、2的幂判断、类型安全 Bitflags                |
| 9    | **constexpr 动态分配 (C++20)** | 编译期 new/delete、编译期 vector、编译期链表                     |
| 10   | **Policy-Based Design**        | 日志策略、分配策略、零开销组合                                   |
| 11   | **Tag Dispatch**               | 迭代器分类、序列化标签分派                                       |
| 12   | **编译期依赖注入**             | CRTP 工厂、模板注入服务                                          |

### 编译方式

```bash
# GCC/Clang (需 C++20)
g++ -std=c++20 -O2 -o test2 test2.cpp && ./test2

# MSVC
cl /std:c++20 /O2 /EHsc test2.cpp
```

### 两个文件的关系

- **test.cpp** — 基础篇：constexpr、TMP、if constexpr、折叠表达式、CRTP、查找表、状态机
- **test2.cpp** — 进阶篇：Concepts、表达式模板、编译期排序/位操作/动态分配、策略模式、标签分派
