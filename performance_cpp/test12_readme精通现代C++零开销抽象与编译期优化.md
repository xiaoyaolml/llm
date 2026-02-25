# 精通现代 C++（C++17/20/23）— 零开销抽象与编译期优化

## 编译

```bash
# GCC/Clang
g++ -std=c++17 -O2 -o test12 test12.cpp

# MSVC
cl /std:c++17 /O2 /EHsc test12.cpp
```

## 目录 (35 章)

### 一、C++17 核心特性篇

| 章 | 主题 | 关键内容 |
|---|------|----------|
| 1 | 结构化绑定 | 数组/tuple/聚合类/map迭代, 自定义tuple-like绑定, 零开销分析 |
| 2 | if constexpr | 编译期分支消除, 类型分发, 条件性成员函数, 零开销序列化 |
| 3 | 折叠表达式 | 四种折叠形式, sum/all/any, 管道式组合(pipeline), vs递归模板 |
| 4 | CTAD | 类模板参数推导, 自定义推导指引(Deduction Guide), 花括号vs圆括号 |
| 5 | optional/variant/any | 安全空值/类型安全联合体/任意类型, Overloaded模式, 零开销分析 |
| 6 | string_view | 零拷贝字符串引用, 零拷贝split, constexpr使用, 性能对比 |
| 7 | 内联变量 | inline 变量/嵌套命名空间/\[\[nodiscard\]\]/\[\[maybe_unused\]\]/if-init |
| 8 | invoke/apply | std::invoke统一调用, std::apply展开tuple, 通用tuple遍历 |

### 二、C++20 核心特性篇

| 章 | 主题 | 关键内容 |
|---|------|----------|
| 9 | Concepts | C++17模拟(void_t/enable_if), C++20原生语法, requires子句, 缩写模板 |
| 10 | Ranges | 惰性管道式算法, views适配器, 管道操作符\|, C++23新views, 零开销 |
| 11 | 三路比较 | spaceship <=> 自动生成6个运算符, strong/weak/partial ordering |
| 12 | consteval/constinit | 编译期强制求值, 避免SIOF, constexpr vs consteval vs constinit |
| 13 | 协程 | promise_type/coroutine_handle/awaitable, Generator, 异步Task |
| 14 | std::format | 类型安全格式化, 格式说明符, 自定义formatter, vs printf/iostream |
| 15 | Modules | export module, 模块分区, import std(C++23), 编译速度2-10x提升 |

### 三、C++23 新特性篇

| 章 | 主题 | 关键内容 |
|---|------|----------|
| 16 | Deducing this | 显式对象参数, 消除const重载, 递归lambda, CRTP简化, Builder模式 |
| 17 | std::expected | 错误处理新范式, C++17模拟实现, monadic接口(and_then/transform/or_else) |
| 18-20 | C++23杂项 | print/println, if consteval, static operator(), 多维下标, mdspan, flat_map |

### 四、零开销抽象篇

| 章 | 主题 | 关键内容 |
|---|------|----------|
| 21 | CRTP | 静态多态, Comparable mixin, InstanceCounter, vs虚函数(快2-10x) |
| 22 | Policy-Based Design | 策略组合(日志/线程/分配), 编译期绑定, NullLogger零开销消除 |
| 23 | Strong Types | 类型安全包装(Meters/Seconds/UserId), constexpr, sizeof==sizeof(T) |
| 24 | Type Erasure | 值语义多态(Concept/Model), 无侵入, vs继承对比 |
| 25 | variant vs 虚函数 | 性能基准对比, cache友好分析, 封闭vs开放类型集选择指南 |

### 五、编译期计算篇

| 章 | 主题 | 关键内容 |
|---|------|----------|
| 26 | constexpr深入 | 编译期sqrt/hash/sort/sin查找表, C++11→23演进, switch-case哈希 |
| 27 | 编译期容器 | constexpr array算法, ConstexprMap(排序+二分), 编译期HTTP状态码表 |
| 28 | 类型列表 | TypeList, Size/TypeAt/IndexOf/Contains/Transform, 编译期容器 |
| 29 | 编译期字符串 | FixedString, constexpr拼接/大写, NTTP(C++20), CTRE |
| 30 | 编译期状态机 | 状态转换表, 折叠表达式验证序列, 编译期+运行期双用 |

### 六、高级模板技巧篇

| 章 | 主题 | 关键内容 |
|---|------|----------|
| 31 | SFINAE→Concepts | void_t/enable_if → if constexpr → Concepts 演进路线 |
| 32 | 变参模板 | index_sequence, for_each_tuple, MultiInherit, Overloaded |
| 33 | 完美转发 | 万能引用/引用折叠/std::forward, 构造函数陷阱, enable_if排除 |
| 34 | Lambda高级 | 泛型/init-capture/constexpr/IIFE/Y-combinator, C++14→23演进 |
| 35 | 最佳实践 | 抽象机制对比表, 6大原则, 编译器探索工具(godbolt/cppinsights) |

## 核心理念

```
零开销抽象 = 你不用的特性不付开销, 你用的特性手写也不会更好
```

- **编译期优先**: constexpr → consteval → if constexpr → 查找表
- **值语义优先**: variant → optional → expected → strong types
- **零拷贝优先**: string_view → span → move semantics
- **类型安全优先**: strong types → concepts → enum class
