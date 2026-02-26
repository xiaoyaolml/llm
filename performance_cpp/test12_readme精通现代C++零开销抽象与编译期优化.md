# 精通现代 C++（C++17/20/23）— 零开销抽象与编译期优化

> **定位**：本教程对应 `test12.cpp`（3101 行，35 章），系统性覆盖 C++17/20/23 六大主题：核心语言特性、零开销抽象模式、编译期计算、高级模板技巧。每章均配有可编译运行的代码示例和性能分析。与 `test1-3.cpp`（编译期基础/进阶）聚焦编译器优化不同，本文件聚焦**语言层面的抽象设计**——如何用现代 C++ 写出既优雅又零开销的代码。

---

## 编译与运行

```bash
# GCC / Clang（C++17 即可编译全部可运行代码，C++20/23 部分以注释形式展示）
g++ -std=c++17 -O2 -o test12 test12.cpp && ./test12

# 使用 C++20/23 编译器可启用更多特性
g++ -std=c++2b -O2 -o test12 test12.cpp

# MSVC
cl /std:c++17 /O2 /EHsc test12.cpp && test12.exe
cl /std:c++latest /O2 /EHsc test12.cpp   # 启用 C++20/23
```

> C++20/23 特性通过 `HAS_CPP20` / `HAS_CPP23` 宏检测。核心代码均可在 C++17 下运行；C++20/23 原生语法以注释 + 原始字符串讲解形式呈现，确保教程完整性。

---

## 目录总览（35 章 × 6 篇）

| 篇 | 章 | 主题 | 核心技术 |
|---|---|---|---|
| **一、C++17 核心** | 1 | 结构化绑定 | 数组/tuple/聚合类/map, 自定义 tuple-like, 零开销 |
| | 2 | if constexpr | 编译期分支消除, 类型分发, 条件性成员函数 |
| | 3 | 折叠表达式 | 四种形式, sum/all/any, pipeline 管道组合 |
| | 4 | CTAD | 类模板参数推导, 自定义推导指引 |
| | 5 | optional/variant/any | 安全空值, 类型安全联合, Overloaded 模式 |
| | 6 | string_view | 零拷贝引用, split, constexpr, 性能基准 |
| | 7 | 内联变量与小特性 | inline var, [[nodiscard]], if-init |
| | 8 | invoke/apply | 统一调用接口, tuple 展开 |
| **二、C++20 核心** | 9 | Concepts | void_t/enable_if 模拟 → C++20 原生语法 |
| | 10 | Ranges | 惰性管道, views 适配器, 零开销分析 |
| | 11 | 三路比较 <=> | 一行替代六运算符, ordering 类型 |
| | 12 | consteval/constinit | 编译期强制, SIOF 防护 |
| | 13 | 协程 | promise_type/handle/awaitable, Generator/Task |
| | 14 | std::format | 类型安全格式化, 自定义 formatter |
| | 15 | Modules | export/import, 分区, 编译提速 2-10x |
| **三、C++23 新特性** | 16 | Deducing this | 显式对象参数, 消除重载, 递归 lambda |
| | 17 | std::expected | C++17 模拟, monadic 链式操作 |
| | 18-20 | C++23 杂项 | print, if consteval, static op(), mdspan, flat_map |
| **四、零开销抽象** | 21 | CRTP | 静态多态, Comparable mixin, InstanceCounter |
| | 22 | Policy-Based Design | 策略组合, NullLogger 零开销消除 |
| | 23 | Strong Types | 类型安全包装, sizeof == sizeof(T) |
| | 24 | Type Erasure | Concept/Model, 值语义多态, 无侵入 |
| | 25 | variant vs 虚函数 | 性能基准, cache 分析, 选型指南 |
| **五、编译期计算** | 26 | constexpr 深入 | sqrt/hash/sort/sin 查找表, C++11→23 |
| | 27 | 编译期容器 | ConstexprMap(排序+二分), HTTP 状态表 |
| | 28 | 类型列表 | TypeList, Size/TypeAt/IndexOf/Transform |
| | 29 | 编译期字符串 | FixedString, concat, NTTP |
| | 30 | 编译期状态机 | 转换表, 折叠验证, 双用 |
| **六、高级模板** | 31 | SFINAE → Concepts 演进 | void_t → enable_if → if constexpr → Concepts |
| | 32 | 变参模板 | index_sequence, for_each_tuple, MultiInherit |
| | 33 | 完美转发 | 万能引用, 引用折叠, 构造函数陷阱 |
| | 34 | Lambda 高级 | 泛型/constexpr/IIFE/Y-combinator/C++14→23 |
| | 35 | 最佳实践总结 | 机制对比表, 6 大原则, 工具链 |

---

## 文件关系与知识图谱

```
test1.cpp (编译期基础)     test2.cpp (编译期进阶)     test3.cpp (C++17编译期)
    │ constexpr/模板          │ 模板元/SFINAE             │ if constexpr/fold
    │                         │                           │
    └─────────────────────────┴───────────────────────────┘
                              │
                              ▼
                      test12.cpp (本文件)
              零开销抽象 × 编译期优化 × 现代 C++
                              │
          ┌───────────────────┼───────────────────┐
          │                   │                   │
          ▼                   ▼                   ▼
     test5.cpp           test11.cpp          test14.cpp
   (零开销抽象)         (无锁/内存序)       (CPU微架构)
     │ CRTP/variant       │ atomic/RCU        │ cache/pipeline
     │ type erasure       │ memory order      │ SIMD/branch
     └───────────────────┴───────────────────┘

知识层次:
┌─────────────────────────────────────────────────────────────┐
│  第六篇: 高级模板技巧 (Ch31-35)                              │
│    SFINAE → Concepts │ 变参模板 │ 完美转发 │ Lambda │ 总结   │
├─────────────────────────────────────────────────────────────┤
│  第五篇: 编译期计算 (Ch26-30)                                │
│    constexpr深入 │ 编译期容器 │ TypeList │ FixedString │ FSM │
├─────────────────────────────────────────────────────────────┤
│  第四篇: 零开销抽象 (Ch21-25)                                │
│    CRTP │ Policy │ StrongType │ TypeErasure │ variant比较   │
├─────────────────────────────────────────────────────────────┤
│  第三篇: C++23 (Ch16-20)                                    │
│    deducing this │ expected │ print │ mdspan │ flat_map      │
├─────────────────────────────────────────────────────────────┤
│  第二篇: C++20 (Ch9-15)                                     │
│    Concepts │ Ranges │ <=> │ consteval │ 协程 │ format │ 模 │
├─────────────────────────────────────────────────────────────┤
│  第一篇: C++17 (Ch1-8)                                      │
│    绑定 │ if constexpr │ fold │ CTAD │ variant │ sv │ invoke │
└─────────────────────────────────────────────────────────────┘
```

---

## 公共工具

```cpp
// 编译器版本检测
#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
  #define HAS_CPP20 1
#else
  #define HAS_CPP20 0
#endif
#if __cplusplus >= 202302L || _MSVC_LANG >= 202302L
  #define HAS_CPP23 1
#else
  #define HAS_CPP23 0
#endif

using namespace std::string_literals;       // "hello"s
using namespace std::string_view_literals;  // "hello"sv

static void print_header(const char* title) { /* 装饰性输出 */ }
static void print_section(const char* title) { /* 节标题 */ }

class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point start_ = clk::now();
public:
    double elapsed_us() const;
    double elapsed_ms() const { return elapsed_us() / 1000.0; }
    void reset() { start_ = clk::now(); }
};
```

---

# 一、C++17 核心特性篇

---

## 第1章：结构化绑定（Structured Bindings）

### 1.1 五种绑定场景

```cpp
// 1. 数组
int arr[] = {10, 20, 30};
auto [a, b, c] = arr;

// 2. pair / tuple
auto [key, value] = std::make_pair("name"s, 42);
auto [x, y, z] = std::make_tuple(1.0, 2.0, 3.0);

// 3. 聚合类 (public 成员)
struct Point { double x, y, z; };
Point pt{1.5, 2.5, 3.5};
auto& [px, py, pz] = pt;  // 引用绑定，可修改原对象
px = 100.0;                // pt.x == 100.0

// 4. map 迭代
for (const auto& [name, score] : scores) { ... }

// 5. if-init + 结构化绑定 (C++17)
if (auto [iter, inserted] = m.insert({"key", 42}); inserted) { ... }
```

### 1.2 自定义 tuple-like 绑定

```cpp
class Color {
    uint8_t r_, g_, b_;
public:
    template<size_t I> auto get() const {
        if constexpr (I == 0) return r_;
        else if constexpr (I == 1) return g_;
        else return b_;
    }
};

// 特化 std::tuple_size 和 std::tuple_element
namespace std {
    template<> struct tuple_size<Color> : integral_constant<size_t, 3> {};
    template<size_t I> struct tuple_element<I, Color> { using type = uint8_t; };
}

auto [r, g, b] = Color(255, 128, 0);  // 现在可以结构化绑定了
```

### 1.3 零开销分析

```
auto [x, y] = pt;    →  编译为 x = pt.x; y = pt.y;
auto& [x, y] = pt;   →  x 和 y 是 pt.x, pt.y 的引用
→ 纯编译期语法糖，零运行时开销
```

### 深入扩展

1. **C++20 结构化绑定 + lambda 捕获**：`[&x, &y] = pt; [x]() {...}` — C++20 允许 lambda 捕获结构化绑定变量
2. **结构化绑定与 `const` / `volatile` 限定**：`const auto& [a, b]` 正确传播 cv 限定
3. **与 `std::apply` 联动**：结构化绑定用于解包，`std::apply` 用于参数展开——互补使用
4. **反射扩展 (C++26 提案)**：未来可能自动为任意类型生成结构化绑定支持
5. **性能陷阱**：`auto [a, b] = pair` 是值拷贝；大对象务必用 `auto& [a, b]` 或 `const auto& [a, b]`

---

## 第2章：if constexpr — 编译期分支消除

### 2.1 核心原理

```
if constexpr: 在编译期求值条件
  ✓ 未选中分支完全不实例化（不编译）
  ✓ 替代 SFINAE / tag dispatch / enable_if
  ✓ 代码量减少 50%+
```

### 2.2 关键用法

```cpp
// 类型分发
template<typename T>
std::string type_name() {
    if constexpr (std::is_integral_v<T>)        return "整数类型";
    else if constexpr (std::is_floating_point_v<T>) return "浮点类型";
    else if constexpr (std::is_pointer_v<T>)      return "指针类型";
    else                                           return "其他类型";
}

// 编译期递归展开
template<typename T, typename... Ts>
void print_all(const T& first, const Ts&... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << ", ";
        print_all(rest...);  // 只有 rest 非空时才展开
    }
}

// 条件性成员函数
template<typename T>
class SmartContainer {
    std::vector<T> data_;
public:
    auto sum() const {
        if constexpr (std::is_arithmetic_v<T>) {
            return std::accumulate(data_.begin(), data_.end(), T{0});
        } else {
            static_assert(std::is_arithmetic_v<T>, "sum() 仅支持算术类型");
        }
    }
};

// 零开销序列化
template<typename T>
void serialize(std::ostream& os, const T& val) {
    if constexpr (std::is_trivially_copyable_v<T>) {
        os.write(reinterpret_cast<const char*>(&val), sizeof(T));  // 直接内存拷贝
    } else if constexpr (std::is_same_v<T, std::string>) {
        uint32_t len = val.size();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(val.data(), len);
    } else {
        static_assert(!std::is_same_v<T,T>, "不支持的序列化类型");
    }
}
```

### 2.3 if constexpr vs 普通 if

```
普通 if:        两个分支都必须编译通过
if constexpr:   未选中分支完全不实例化

C++14: template<typename T, enable_if_t<is_integral_v<T>>* = nullptr>
C++17: if constexpr (is_integral_v<T>) { ... }
→ 更简洁，可读性大幅提升
```

### 深入扩展

1. **if constexpr 嵌套与组合**：多层 if constexpr 实现复杂的类型分发树，替代大量 enable_if 重载
2. **与 `std::is_detected` 配合**：检测表达式是否有效 + if constexpr 条件编译
3. **`constexpr if` 与 `static_assert` 的交互**：未选中分支中的 `static_assert(false)` 仍会触发——必须用依赖模板参数的表达式
4. **C++23 `if consteval`**：编译期/运行期分别选择不同实现（consteval 上下文用编译期算法，否则用硬件指令）
5. **编译期条件下的 ABI 影响**：if constexpr 不同分支可能生成不同大小的类——影响二进制兼容性

---

## 第3章：折叠表达式（Fold Expressions）

### 3.1 四种折叠形式

```
(pack op ...)         一元右折叠  a1 op (a2 op (a3 op a4))
(... op pack)         一元左折叠  ((a1 op a2) op a3) op a4
(pack op ... op init) 二元右折叠  a1 op (a2 op (a3 op init))
(init op ... op pack) 二元左折叠  ((init op a1) op a2) op a3
```

### 3.2 典型应用

```cpp
// 求和
template<typename... Ts> auto sum(Ts... args) { return (args + ...); }

// 全部/任一为真
template<typename... Ts> bool all_true(Ts... args) { return (args && ...); }
template<typename... Ts> bool any_true(Ts... args) { return (args || ...); }

// 打印（逗号运算符折叠）
template<typename... Ts>
void print_fold(const Ts&... args) {
    ((std::cout << args << " "), ...);
}

// 编译期类型检查
template<typename... Ts>
constexpr bool all_arithmetic() { return (std::is_arithmetic_v<Ts> && ...); }

// 管道式组合 — 函数链
template<typename T, typename... Funcs>
T pipeline(T val, Funcs... funcs) {
    ((val = funcs(val)), ...);  // 每步更新 val
    return val;
}

auto result = pipeline(10,
    [](int x) { return x * 2; },   // 20
    [](int x) { return x + 3; },   // 23
    [](int x) { return x * x; });  // 529
```

### 3.3 折叠 vs 递归

```
C++11: 递归模板展开 → 大量模板实例化 → 编译慢
C++17: 折叠表达式   → 一次性展开 → 编译快 + 代码简洁
生成的机器码相同 → 零开销
```

### 深入扩展

1. **折叠 + `operator,` 实现副作用链**：`((func(args), 0), ...)` 保证求值顺序
2. **折叠 + 类型约束**：`(std::is_convertible_v<Ts, Target> && ...)` 编译期检查所有参数可转换
3. **`constexpr` 折叠**：`constexpr auto m = max_of(3, 7, 2, 9, 1)` 编译期选最大值
4. **嵌套折叠陷阱**：`(... + (args * args))` 语法合法性——需注意内外层参数包展开
5. **与 C++23 `std::ranges::fold_left/fold_right` 对比**：运行期折叠 vs 编译期折叠

---

## 第4章：CTAD（类模板参数推导）

### 4.1 标准库 CTAD

```cpp
// C++17 之前：必须显式指定模板参数
std::pair<int, double> p(1, 2.0);
// C++17：自动推导
auto p = std::pair(1, 2.0);             // pair<int, double>
auto t = std::tuple(1, "hello"s, 3.14); // tuple<int, string, double>
auto v = std::vector{1, 2, 3, 4, 5};    // vector<int>
auto o = std::optional(42);             // optional<int>
```

### 4.2 自定义推导指引

```cpp
template<typename T>
class Wrapper {
    T value_;
public:
    Wrapper(T v) : value_(std::move(v)) {}
    const T& get() const { return value_; }
};

// 隐式推导指引（从构造函数自动生成）
template<typename T> Wrapper(T) -> Wrapper<T>;
// 字符串字面量特化
Wrapper(const char*) -> Wrapper<std::string>;

Wrapper w1(42);       // Wrapper<int>
Wrapper w2(3.14);     // Wrapper<double>
Wrapper w3("hello");  // Wrapper<string>（通过推导指引）

// 迭代器推导指引
template<typename Iter> Range(Iter, Iter) -> Range<Iter>;
template<typename Container> Range(Container&) -> Range<typename Container::iterator>;
```

### 4.3 花括号 vs 圆括号

```
std::vector v1{5};      → vector<int>{5}      （一个元素 5）
std::vector v2(5, 0);   → vector<int>(5, 0)   （五个 0）
→ {} 和 () 语义不同！CTAD 时需特别注意
```

### 深入扩展

1. **聚合类 CTAD (C++20)**：聚合类也可以自动推导——`std::pair p = {1, 2.0}` 无需推导指引
2. **别名模板 CTAD (C++20)**：`template<typename T> using Vec = vector<T>; Vec v = {1,2,3};`
3. **CTAD 与继承**：推导指引在继承场景下可能失效——需要手动添加
4. **`std::to_array` (C++20)**：`auto arr = std::to_array({1,2,3})` 替代 C 数组推导
5. **推导指引优先级**：用户定义 > 编译器隐式生成，可以控制特定类型的推导行为

---

## 第5章：optional / variant / any

### 5.1 std::optional — 安全空值

```cpp
std::optional<int> safe_divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}
auto r = safe_divide(10, 3);  // has_value() == true
r.value_or(-1);               // 安全访问
```

### 5.2 std::variant — 类型安全联合体

```cpp
using JsonValue = std::variant<nullptr_t, bool, int, double, std::string>;

// 访问者模式
struct JsonPrinter {
    void operator()(nullptr_t)       const { std::cout << "null"; }
    void operator()(bool b)          const { std::cout << (b ? "true" : "false"); }
    void operator()(int i)           const { std::cout << i; }
    void operator()(double d)        const { std::cout << d; }
    void operator()(const string& s) const { std::cout << '"' << s << '"'; }
};

std::visit(JsonPrinter{}, json_val);

// Overloaded 模式（lambda 访问者）
template<typename... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<typename... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

std::visit(Overloaded{
    [](auto arg) { std::cout << arg; },
    [](nullptr_t) { std::cout << "null"; }
}, val);
```

### 5.3 零开销分析

```
optional<T>:  sizeof = sizeof(T) + 1 (+ 对齐 padding)
              无堆分配，值直接内嵌

variant<Ts>:  sizeof = max(sizeof(Ts)...) + 判别符 (1-4 bytes)
              无堆分配，无虚表
              连续内存，cache 友好

any:          小对象优化 (SBO)，大对象堆分配
              有运行时开销 → 慎用

vs 传统方案:
  optional  vs nullptr/magic value: 编译器检查，更安全
  variant   vs union:  类型安全，自动析构
  variant   vs 继承: 无虚表，cache 友好，sizeof 更小
```

### 深入扩展

1. **`std::optional` monadic (C++23)**：`.and_then()` / `.transform()` / `.or_else()` 链式操作
2. **`std::variant` 异常安全**：`valueless_by_exception()` — 赋值期间抛异常时的状态
3. **`std::visit` 性能**：编译器生成跳转表或分支链——少量类型时与手写 switch 等价
4. **`std::monostate`**：作为 variant 的默认类型（variant 第一个类型必须可默认构造）
5. **嵌套 variant / recursive variant**：需要 `std::unique_ptr` 打破递归——JSON AST 典型场景

---

## 第6章：string_view — 零拷贝字符串引用

### 6.1 核心特性

```cpp
// sizeof(string_view) == 2 * sizeof(void*) = 16 bytes
// 只是 {指针, 长度} 对，不拥有内存

// 零拷贝分割
std::vector<std::string_view> split_view(std::string_view sv, char delim) {
    std::vector<std::string_view> result;
    size_t start = 0;
    for (size_t i = 0; i <= sv.size(); ++i) {
        if (i == sv.size() || sv[i] == delim) {
            if (i > start)
                result.push_back(sv.substr(start, i - start));  // O(1)!
            start = i + 1;
        }
    }
    return result;
}

// 编译期使用
constexpr bool starts_with_hello(std::string_view sv) {
    return sv.size() >= 5 && sv.substr(0, 5) == "hello";
}
static_assert(starts_with_hello("hello world"));
```

### 6.2 性能基准（代码中实测）

```
string::substr      ×100000: ~15ms  （每次堆分配 + 拷贝）
string_view::substr ×100000: ~0.5ms （零拷贝，指针偏移）
→ 约 30 倍性能差距
```

### 6.3 注意事项

```
⚠️ 不拥有内存 → 原始字符串销毁后悬空
⚠️ 不保证 null-terminated → 不能直接传给 C API
✅ 最佳实践: 函数参数用 string_view（接受 string / char* / sv）
```

### 深入扩展

1. **`std::span<T>` (C++20)**：string_view 的泛化版——任意类型的非拥有视图
2. **`starts_with/ends_with` (C++20)**：`sv.starts_with("http")` 取代手动 substr 比较
3. **`contains` (C++23)**：`sv.contains("pattern")` 更直观的查找
4. **生命周期保证**：返回 string_view 时必须确保底层存储的生命周期——`[[lifetimebound]]` 属性提示
5. **与 `std::format` 结合**：`std::format("{}", sv)` 直接格式化，无需转换为 string

---

## 第7章：内联变量与 C++17 小特性

### 7.1 inline 变量

```cpp
struct Config {
    static inline int max_connections = 100;       // 头文件中定义 ✓
    static inline const std::string version = "2.0";
    static inline constexpr int buffer_size = 4096; // constexpr 本身 inline
};
```

### 7.2 其他 C++17 特性

| 特性 | 用途 | 示例 |
|---|---|---|
| 嵌套命名空间 | 语法简化 | `namespace A::B::C { ... }` |
| `[[nodiscard]]` | 防止忽略返回值 | `[[nodiscard]] int compute()` |
| `[[maybe_unused]]` | 抑制未使用警告 | `[[maybe_unused]] int x = 0` |
| if-init | 限制变量作用域 | `if (auto it = m.find(k); it != m.end())` |
| `std::byte` | 类型安全字节 | 不隐式转换为 int/char |

### 深入扩展

1. **`[[nodiscard("reason")]]` (C++20)**：带消息的 nodiscard，帮助理解为什么返回值重要
2. **`[[likely]]` / `[[unlikely]]` (C++20)**：分支预测提示
3. **`[[no_unique_address]]` (C++20)**：允许空类成员不占空间（EBO 的属性版）
4. **`[[assume(expr)]]` (C++23)**：告诉编译器某条件为真，指导优化
5. **`inline constexpr` vs `constexpr`**：在头文件中定义变量的 ODR 差异

---

## 第8章：std::invoke / std::apply

### 8.1 统一调用语法

```cpp
struct Printer {
    void print(int x) const { ... }
    int value = 42;
};

Printer p;
std::invoke(free_func, 10, 20);       // 普通函数
std::invoke(&Printer::print, p, 100); // 成员函数
std::invoke([](int x) {...}, 200);    // Lambda
std::invoke(&Printer::value, p);       // 成员变量 → 42
```

### 8.2 std::apply — tuple → 函数参数

```cpp
auto args = std::make_tuple(10, 20);
std::apply(free_func, args);  // free_func(10, 20)

// 通用 tuple 打印
std::apply([](const auto&... args) {
    ((std::cout << args << " "), ...);
}, tup);
```

### 深入扩展

1. **`std::invoke_r<R>` (C++23)**：指定返回类型的 invoke，用于隐式转换场景
2. **`std::bind_front / std::bind_back` (C++20/23)**：部分应用——比 `std::bind` 更安全高效
3. **泛型调度器实现**：`std::invoke` + `std::any` 实现类型擦除的通用命令模式
4. **`std::apply` + 结构化绑定**：tuple → 函数参数 → 结果解构——完整的函数式管道
5. **在协程中使用**：`co_await std::invoke(async_fn, args...)` 统一异步调用

---

# 二、C++20 核心特性篇

---

## 第9章：Concepts — 约束模板参数

### 9.1 C++17 模拟

```cpp
// void_t 检测是否有 + 运算符
template<typename T, typename = void>
struct is_addable : std::false_type {};
template<typename T>
struct is_addable<T, std::void_t<decltype(std::declval<T>() + std::declval<T>())>>
    : std::true_type {};

// enable_if 模拟 concept
template<typename T, std::enable_if_t<is_addable_v<T>, int> = 0>
T add_sfinae(T a, T b) { return a + b; }

// has_xxx 检测模式
template<typename T, typename = void>
struct has_size : std::false_type {};
template<typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};
```

### 9.2 C++20 原生语法

```cpp
// 定义 concept
template<typename T>
concept Sortable = requires(T& t) {
    { t.begin() } -> std::input_iterator;
    { t.end() }   -> std::input_iterator;
    requires std::totally_ordered<typename T::value_type>;
};

// 使用方式 1: 约束模板参数
template<Sortable T> void sort_it(T& c);

// 使用方式 2: requires 子句
template<typename T> void sort_it(T& c) requires Sortable<T>;

// 使用方式 3: 缩写模板
void sort_it(Sortable auto& c);
```

### 9.3 SFINAE vs Concepts

```
SFINAE 错误:  "no matching function for call to..."（难读）
Concepts 错误: "constraint 'Sortable' not satisfied because..."（清晰）
→ Concepts 是 SFINAE 的上位替代
```

### 深入扩展

1. **`requires requires` 表达式**：嵌套 requires 检测复合约束——语法奇特但强大
2. **Concept 偏序 (Subsumption)**：更约束的 concept 优先匹配——`std::integral` 优先于 `std::convertible_to<int>`
3. **标准库 Concepts**：`std::regular`, `std::movable`, `std::ranges::range`, `std::invocable` 等
4. **Concepts + CRTP**：约束 CRTP 派生类必须实现特定接口
5. **Concepts 与模板编译速度**：约束更早失败 → 减少无效实例化 → 编译更快

---

## 第10章：Ranges — 惰性管道式算法

### 10.1 核心设计

```
传统 STL:
  std::sort(v.begin(), v.end());
  std::copy_if(v.begin(), v.end(), out, pred);

Ranges (C++20):
  std::ranges::sort(v);
  v | views::filter(pred) | views::transform(f) | views::take(5);

组件:
  range    — 拥有 begin()/end() 的任何对象
  view     — 惰性求值、O(1) 拷贝的轻量 range
  adaptor  — range → range 转换
  algorithm— 接受 range 的算法
  projection— 算法中的投影函数
```

### 10.2 常用 views

| View | 功能 | 标准 |
|---|---|---|
| `views::filter(pred)` | 过滤 | C++20 |
| `views::transform(f)` | 变换 | C++20 |
| `views::take(n)` | 取前 n 个 | C++20 |
| `views::drop(n)` | 跳过前 n 个 | C++20 |
| `views::reverse` | 反转 | C++20 |
| `views::split(delim)` | 分割 | C++20 |
| `views::join` | 展平嵌套 | C++20 |
| `views::zip` | 并行迭代 | C++23 |
| `views::enumerate` | 带索引 | C++23 |
| `views::chunk(n)` | 分块 | C++23 |
| `views::stride(n)` | 步长访问 | C++23 |

### 10.3 零开销分析

```
views 是惰性的 → 不创建中间容器
编译器内联 + 优化后 → 等同于手写循环
实测: ranges 管道与手写循环性能差距 < 5%

C++17 模拟 (代码中实现):
  for (int n : nums) {
      if (n % 2 == 0) {
          result.push_back(n * n);
          if (result.size() >= 3) break;
      }
  }
C++20 等价:
  nums | views::filter(even) | views::transform(square) | views::take(3);
```

### 深入扩展

1. **`ranges::to<Container>()` (C++23)**：`v | filter(p) | ranges::to<vector>()` 物化为容器
2. **自定义 view adaptor**：继承 `ranges::view_interface<Derived>` 实现自定义惰性视图
3. **Ranges + 协程**：`co_yield` 生成惰性序列，与 ranges 管道无缝对接
4. **并行 Ranges (提案)**：`std::par | views::transform(f)` 并行惰性管道
5. **编译时间影响**：ranges 头文件较大，增量编译中 `<ranges>` 可能增加编译时间——考虑模块化

---

## 第11章：三路比较 <=>

### 11.1 一行替代六个运算符

```cpp
// C++17: 手动实现 6 个运算符
struct Version {
    int major, minor, patch;
    bool operator==(const Version& o) const {
        return std::tie(major, minor, patch) == std::tie(o.major, o.minor, o.patch);
    }
    bool operator<(const Version& o) const { return tie(...) < tie(...); }
    // ... 还有 !=, <=, >, >= 共 6 个
};

// C++20: 一行搞定
struct Version {
    int major, minor, patch;
    auto operator<=>(const Version&) const = default; // 自动生成全部 6 个
};
```

### 11.2 Ordering 类型

```
strong_ordering:   a < b, a == b, a > b  (整数等精确比较)
weak_ordering:     等价但不一定相等     (如大小写不敏感字符串)
partial_ordering:  可能不可比较         (如 NaN)
```

### 深入扩展

1. **自定义 <=> 返回类型**：手动控制比较语义——如忽略某些字段
2. **与标准库容器交互**：`std::set` / `std::map` 自动使用 <=> 生成的 `operator<`
3. **异构比较**：`strong_ordering operator<=>(int) const` 支持不同类型间比较
4. **`= default` 的语义**：逐成员按声明顺序比较——与手写 `std::tie` 等价
5. **性能**：`default <=>` 编译后完全等同于手写连续比较——零开销

---

## 第12章：consteval / constinit

### 12.1 三者对比

```
┌───────────┬─────────────────┬─────────────────┐
│           │ 编译期可用?      │ 运行期可用?      │
├───────────┼─────────────────┼─────────────────┤
│ constexpr │ ✅ (如果参数编译期)│ ✅               │
│ consteval │ ✅ (强制)        │ ❌ (编译错误)    │
│ constinit │ ✅ (初始化)      │ ✅ (修改/读取)   │
│ const     │ ❌               │ ✅ (不可修改)    │
└───────────┴─────────────────┴─────────────────┘

consteval — "立即函数"，调用处必须是编译期常量上下文
constinit — 变量初始化强制编译期，防止 SIOF
```

### 12.2 代码示例

```cpp
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
}

constexpr int f10 = factorial(10);   // ✅ 编译期
static_assert(f10 == 3628800);

int n = 5;
int f5 = factorial(n);               // ✅ 运行时也可以

// C++20:
// consteval int must_compile_time(int n) { return factorial(n); }
// int x = must_compile_time(10);    // ✅
// int y = must_compile_time(n);     // ❌ 编译错误!

// constinit int global = factorial(10); // 编译期初始化，避免 SIOF
// global = 42;                          // ✅ 运行时可修改
```

### 深入扩展

1. **consteval 函数作为编译期查表**：确保哈希/查表在编译期完成，零运行时开销
2. **SIOF 场景复现**：全局变量跨翻译单元初始化顺序未定义——constinit 彻底解决
3. **`consteval` + `if consteval` 组合**：编译期使用 consteval 函数，运行期 fallback 到标准库
4. **`constinit thread_local`**：线程局部变量也可以保证编译期初始化
5. **C++26 提案: constexpr placement new**：进一步放宽 constexpr 限制

---

## 第13章：协程（Coroutines）

### 13.1 核心组件

```
┌─── Promise Type ─── 控制协程行为 ──────────────────────┐
│ get_return_object()      → 返回协程句柄                 │
│ initial_suspend()        → 启动时暂停?                  │
│ final_suspend() noexcept → 结束时暂停?                  │
│ return_void/value()      → co_return 语义               │
│ yield_value()            → co_yield 语义                │
│ unhandled_exception()    → 异常处理                     │
├────────────────────────────────────────────────────────┤
│ Coroutine Handle         → 控制执行                     │
│ handle.resume()  / handle.done() / handle.destroy()    │
├────────────────────────────────────────────────────────┤
│ Awaitable               → co_await 操作对象              │
│ await_ready()  / await_suspend() / await_resume()      │
└────────────────────────────────────────────────────────┘
```

### 13.2 Generator 示例

```cpp
Generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        auto next = a + b; a = b; b = next;
    }
}
// 输出: 0 1 1 2 3 5 8 13 21 34
```

### 13.3 协程 vs 其他方案

```
┌─────────────┬────────┬───────┬──────────┐
│             │ 开销   │ 可扩展│ 编程模型 │
├─────────────┼────────┼───────┼──────────┤
│ 回调        │ 零     │ 差    │ 复杂     │
│ Future/Async│ 中     │ 中    │ 中       │
│ 协程        │ 极低   │ 好    │ 直觉     │
│ 线程        │ 高     │ 差    │ 直觉     │
└─────────────┴────────┴───────┴──────────┘

挂起/恢复 ≈ 几十纳秒 (vs 线程切换 ~微秒级)
HALO 优化: 编译器可消除堆上协程帧分配
```

### 深入扩展

1. **`std::generator` (C++23)**：标准库终于提供 Generator 类型，无需手写 promise_type
2. **协程 + io_uring**：`co_await` 一个 io_uring CQE，实现极简异步 I/O
3. **协程帧优化 (HALO)**：编译器检测协程生命周期 → 栈上分配帧 → 消除堆分配
4. **Structured Concurrency (提案)**：`co_await all(task1, task2, task3)` 安全并发
5. **C++26 `std::execution`**：Sender/Receiver 模型 + 协程 → 标准化异步框架

---

## 第14章：std::format

### 14.1 格式语法

```
基本:      std::format("Hello, {}!", "world")
位置:      std::format("{1} {0}", "world", "hello")
说明符:    {:d} {:x} {:o} {:b} {:f} {:e}
对齐:      {:>10} {:<10} {:^10}  {:*>10}
精度:      {:+.2f} → "+3.14"

vs printf / iostream:
  printf:   不类型安全，UB 风险
  iostream: 类型安全但冗长 (setw/setprecision)
  format:   类型安全 + 简洁 + 编译期格式串检查

C++23 std::print:
  std::println("value = {}", 42);  // 直接输出 + 自动换行
```

### 14.2 自定义 formatter

```cpp
template<>
struct std::formatter<Point> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Point& p, format_context& ctx) {
        return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};
```

### 深入扩展

1. **编译期格式串验证**：`std::format("{:d}", "str")` 在编译期就报错
2. **`std::format_to` + 预分配缓冲区**：避免内部分配，适合高性能路径
3. **`std::vformat` 动态格式串**：运行时格式串（失去编译期检查）
4. **日志库集成**：spdlog/fmtlib 已采用相同语法——迁移到 `std::format` 几乎零成本
5. **性能**：通常比 iostream 快 2-5x，与 `printf` 相当或更快

---

## 第15章：Modules

### 15.1 模块语法

```cpp
// math.cppm — 模块接口单元
export module math;
export int add(int a, int b) { return a + b; }
export class Calculator { ... };
int internal_helper() { return 42; } // 未 export，不可见

// main.cpp
import math;
auto r = add(1, 2);        // ✅
// internal_helper();       // ❌ 编译错误

// 模块分区
export module math:impl;
export import :impl;        // 重新导出分区

// C++23 标准库模块
import std;                 // 整个标准库
import std.compat;          // C 兼容
```

### 15.2 优势

```
传统头文件:          模块:
  递归展开可达 MB     编译一次为 .pcm/.ifc
  宏泄漏              无宏泄漏
  多重定义风险         严格可见性
  顺序依赖            无顺序问题
  → 编译速度提升 2-10x (大型项目)
```

### 深入扩展

1. **CMake 模块支持**：CMake 3.28+ `target_sources(... FILE_SET CXX_MODULES FILES ...)` 原生支持
2. **头文件单元 (Header Units)**：`import <vector>;` 渐进式迁移，不需要修改现有头文件
3. **模块与预编译头文件 (PCH) 对比**：模块更细粒度、更安全，PCH 是粗粒度加速
4. **编译器差异**：GCC/Clang/MSVC 对模块的支持程度和文件后缀不同
5. **模块与模板**：模板定义在接口模块中导出——实例化在导入侧发生

---

# 三、C++23 新特性篇

---

## 第16章：Deducing this — 显式对象参数

### 16.1 消除重复重载

```cpp
// 传统: 4 个重载 (const/non-const × &/&&)
class Widget {
    int& get() &;
    int& get() &&;
    const int& get() const &;
    const int& get() const &&;
};

// C++23: 1 个模板
class Widget {
    template<typename Self>
    auto& get(this Self&& self) {
        return std::forward<Self>(self).val_;
    }
};
```

### 16.2 更多应用

```cpp
// 递归 Lambda (无需 Y-combinator)
auto fib = [](this auto self, int n) -> int {
    return n < 2 ? n : self(n-1) + self(n-2);
};
fib(10);  // 55

// CRTP 简化
struct Base {
    void interface(this auto& self) { self.impl(); } // 无需模板参数
};

// Builder 模式
class Builder {
    auto&& set_width(this auto&& self, int w) {
        self.width_ = w;
        return std::forward<decltype(self)>(self);
    }
};
```

### 深入扩展

1. **与 `std::forward_like` (C++23) 配合**：通用的按相同值类别转发
2. **Deducing this + Concepts**：`void f(this Sortable auto& self)` 约束隐式对象参数
3. **取代虚函数的场景**：编译期已知类型时，deducing this 完全内联——零虚表
4. **链式调用返回类型**：`this auto&&` 保持值类别，左值调用返回左值引用，右值返回右值
5. **lambda 中的 this 推导**：`[](this auto self, ...) {}` 完整支持递归和移动语义

---

## 第17章：std::expected — 错误处理新范式

### 17.1 C++17 模拟实现

```cpp
template<typename T, typename E>
class Expected {
    std::variant<T, E> data_;
    bool has_value_;
public:
    Expected(const T& val) : data_(val), has_value_(true) {}
    Expected(const E& err) : data_(err), has_value_(false) {}

    bool has_value() const { return has_value_; }
    T& value() { return std::get<T>(data_); }
    E& error() { return std::get<E>(data_); }
    T value_or(const T& def) const { return has_value_ ? std::get<T>(data_) : def; }

    // Monadic 接口
    template<typename F> auto and_then(F&& f) const;   // flatMap
    template<typename F> auto transform(F&& f) const;  // map
};

Expected<int, ParseError> parse_int(std::string_view sv);
auto doubled = parse_int("12345").transform([](int v) { return v * 2; });
```

### 17.2 错误处理对比

```
┌──────────────┬──────────┬──────────┬──────────┬─────────┐
│              │ 类型安全 │ 零开销   │ 信息     │ 可组合  │
├──────────────┼──────────┼──────────┼──────────┼─────────┤
│ 返回错误码   │ ❌       │ ✅       │ 少       │ ❌      │
│ 异常         │ ✅       │ ❌(抛出) │ 丰富     │ ❌      │
│ optional     │ ✅       │ ✅       │ ❌(无)   │ ✅      │
│ expected     │ ✅       │ ✅       │ ✅       │ ✅      │
└──────────────┴──────────┴──────────┴──────────┴─────────┘

链式调用:
  read_file(path)
    .and_then(parse_json)
    .transform(extract_value)
    .or_else(handle_error);
```

### 深入扩展

1. **`std::expected<void, E>`**：仅返回成功/错误，不携带值——替代 `bool + errno`
2. **与 Coroutines 配合**：`co_await` expected 实现类似 Rust `?` 操作符的 early return
3. **错误类型设计**：`std::error_code` / 自定义枚举 / 自定义异常类——灵活选择
4. **性能**：expected 无堆分配，无异常机制开销——在 hot path 上比 try/catch 快 10-100x
5. **Boost.Outcome**：更早的库实现，与标准 expected 语义兼容

---

## 第18-20章：C++23 其他新特性

### 18.1 主要新特性速览

| 特性 | 说明 | 影响 |
|---|---|---|
| `std::print/println` | 直接输出，比 cout 快 2-5x | 日常输出 |
| `if consteval` | 编译期/运行期选择不同实现 | 性能优化 |
| `static operator()` | 无 this 指针，可转函数指针 | 零开销函子 |
| 多维 `operator[]` | `mat[2, 3]` 直接使用 | 矩阵/张量 |
| `std::mdspan` | 多维数组视图，零拷贝零开销 | 科学计算 |
| `std::flat_map/set` | 基于排序 vector，cache 友好 | 小集合 |
| `std::stacktrace` | 运行时调用栈追踪 | 调试 |
| `ranges::to<>()` | range 物化为容器 | Ranges |
| `[[assume(expr)]]` | 编译器优化暗示 | 性能 |

### 深入扩展

1. **`std::mdspan` 布局策略**：`layout_right` (行主序) / `layout_left` (列主序) / 自定义——与 Fortran/BLAS 互操作
2. **`std::flat_map` vs `std::map`**：小数据量 (< 1000) flat_map 通常更快（连续内存），大数据量 map（红黑树 O(log n) 插入）
3. **`std::generator` (C++23)**：标准库协程 Generator——终于不用手写 promise_type
4. **`auto(x)` decay copy**：`auto(arg)` 强制衰退拷贝，在泛型代码中消除引用/cv 限定
5. **`std::move_only_function`**：比 `std::function` 更高效，支持移动语义，不要求拷贝构造

---

# 四、零开销抽象篇

---

## 第21章：CRTP — 静态多态

### 21.1 核心实现

```cpp
template<typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
};

class Circle : public Shape<Circle> {
    double radius_;
public:
    double area_impl() const { return 3.14159 * radius_ * radius_; }
};

class Rectangle : public Shape<Rectangle> {
    double w_, h_;
public:
    double area_impl() const { return w_ * h_; }
};
```

### 21.2 CRTP Mixin

```cpp
// Comparable mixin: 只需实现 ==, < → 自动获得 !=, >, <=, >=
template<typename Derived>
class Comparable {
public:
    bool operator!=(const Derived& o) const { return !(self() == o); }
    bool operator>(const Derived& o) const  { return o < self(); }
    // ...
};

// InstanceCounter: 编译期注入实例计数
template<typename Derived>
class InstanceCounter {
    static inline int count_ = 0;
protected:
    InstanceCounter() { ++count_; }
    ~InstanceCounter() { --count_; }
public:
    static int count() { return count_; }
};

class Widget : public InstanceCounter<Widget> { ... };
```

### 21.3 CRTP vs 虚函数

```
虚函数:  运行时分发, 有虚表指针 (8 bytes), 间接调用 (cache miss)
CRTP:    编译期分发, 无虚表, 可完全内联

sizeof(虚函数类): sizeof(数据) + 8 (vptr)
sizeof(CRTP类):   sizeof(数据)

性能差距: CRTP 快 2-10x (取决于函数复杂度和内联程度)
```

### 深入扩展

1. **Deducing this 替代 CRTP (C++23)**：`void f(this auto& self)` 消除模板参数，更直观
2. **CRTP + Concepts**：`static_assert(requires { derived->impl(); })` 检查派生类接口
3. **多级 CRTP**：`class A : Base<A>`, `class B : A, Base<B>` — 需要仔细处理菱形继承
4. **CRTP 与 ABI**：CRTP 每个实例化生成独立代码——大量派生类可能导致代码膨胀
5. **CRTP 实际应用**：Eigen 矩阵库、Boost.Iterator、STL 容器适配器

---

## 第22章：Policy-Based Design — 策略组合

### 22.1 策略注入

```cpp
// 日志策略
struct ConsoleLogger { static void log(string_view msg) { cout << msg; } };
struct NullLogger     { static void log(string_view) {} }; // 编译后完全消除

// 线程策略
struct SingleThread { struct Lock { }; };                    // 空锁
struct MultiThread  { struct Lock { Lock() { /*lock*/ } ~Lock() { /*unlock*/ } }; };

// 分配策略
template<typename T> struct HeapAllocator { ... };
template<typename T> struct PoolAllocator { ... };

// 组合
template<typename T,
         typename LogPolicy = NullLogger,
         typename ThreadPolicy = SingleThread,
         template<typename> class AllocPolicy = HeapAllocator>
class SmartBuffer {
    void push_back(const T& val) {
        typename ThreadPolicy::Lock lock;  // 编译期决定是否真的加锁
        LogPolicy::log("push");            // NullLogger → 完全消除
        data_[size_++] = val;
    }
};

// 高性能版: 无日志、单线程 → 等同于手写裸代码
SmartBuffer<int> fast_buf(100);
// 调试版: 有日志
SmartBuffer<int, ConsoleLogger> debug_buf(100);
```

### 22.2 核心思想

```
模板参数注入策略 → 编译期确定行为 → 零运行时开销

NullLogger::log()  空函数 → 内联 → 消除
SingleThread::Lock 空构造/析构 → 消除
→ 高性能版本与裸代码生成完全相同的机器码!

典型实践: Alexandrescu《Modern C++ Design》, std::allocator_traits
```

### 深入扩展

1. **策略校验**：用 `static_assert` 或 Concepts 约束策略必须满足特定接口
2. **运行时策略切换**：Policy + Type Erasure 组合——编译期确定接口，运行时切换实现
3. **链式策略**：`Policy<SubPolicy<SubSubPolicy>>` 多层策略嵌套
4. **策略 vs 依赖注入**：策略在编译期绑定（零开销），依赖注入在运行时绑定（灵活）
5. **实战: 自定义分配器**：`std::pmr::polymorphic_allocator` 就是运行时策略的标准实现

---

## 第23章：Strong Types — 类型安全的零开销包装

### 23.1 实现

```cpp
template<typename T, typename Tag>
class StrongType {
    T value_;
public:
    constexpr explicit StrongType(T val) : value_(std::move(val)) {}
    constexpr T& get() { return value_; }
    constexpr const T& get() const { return value_; }
    constexpr StrongType operator+(const StrongType& o) const {
        return StrongType(value_ + o.value_);
    }
};

using Meters   = StrongType<double, struct MetersTag>;
using Seconds  = StrongType<double, struct SecondsTag>;
using UserId   = StrongType<int, struct UserIdTag>;
using OrderId  = StrongType<int, struct OrderIdTag>;

double compute_speed(Meters distance, Seconds time) {
    return distance.get() / time.get();
}

// compute_speed(Seconds(10), Meters(5));  ❌ 编译错误！参数类型不匹配
// if (uid == oid) {}                      ❌ 编译错误！不同类型
```

### 23.2 零开销验证

```
sizeof(Meters)   == sizeof(double) == 8 bytes
sizeof(UserId)   == sizeof(int)    == 4 bytes
→ 优化后完全等同于裸 T，类型安全是编译期的
```

### 深入扩展

1. **NamedType 库**：`github.com/joboccara/NamedType` — 更完整的强类型框架，支持运算符组合
2. **`std::chrono` 就是强类型**：`seconds`, `milliseconds`, `hours` 互不互通——标准库的典范
3. **物理单位库 (mp-units)**：C++23 推进中的物理量库——编译期单位检查
4. **Tag 类型设计**：空结构体 Tag 不增加任何开销，仅作为类型系统标记
5. **与 Concepts 结合**：`template<typename T> concept Distance = ...` 约束强类型的使用场景

---

## 第24章：Type Erasure — 值语义多态

### 24.1 Concept/Model 模式

```cpp
class Drawable {
    struct Concept {                              // 内部虚基类
        virtual ~Concept() = default;
        virtual void draw(std::ostream&) const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };
    template<typename T>
    struct Model : Concept {                      // 包装任意类型
        T obj_;
        Model(T obj) : obj_(std::move(obj)) {}
        void draw(std::ostream& os) const override { obj_.draw(os); }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(obj_);
        }
    };
    std::unique_ptr<Concept> pimpl_;
public:
    template<typename T>
    Drawable(T obj) : pimpl_(std::make_unique<Model<T>>(std::move(obj))) {}
    Drawable(const Drawable& o) : pimpl_(o.pimpl_->clone()) {} // 值语义拷贝
    void draw(std::ostream& os) const { pimpl_->draw(os); }
};

// 无需继承，只要有 draw() 方法
struct CircleShape  { double radius; void draw(ostream& os) const { ... } };
struct SquareShape  { double side;   void draw(ostream& os) const { ... } };

std::vector<Drawable> shapes;  // 异构容器，值语义
shapes.emplace_back(CircleShape{5.0});
shapes.emplace_back(SquareShape{3.0});
```

### 24.2 Type Erasure vs 继承

```
┌──────────────┬──────────────────┬──────────────────┐
│              │ 继承 + 虚函数    │ Type Erasure     │
├──────────────┼──────────────────┼──────────────────┤
│ 侵入性       │ 必须继承基类     │ 无侵入          │
│ 语义         │ 引用语义(指针)   │ 值语义          │
│ 拷贝         │ 需要 clone       │ 自然拷贝        │
│ 容器         │ unique_ptr       │ 直接存值        │
│ 新类型       │ 继承基类         │ 只需有对应方法  │
└──────────────┴──────────────────┴──────────────────┘

标准库: std::function, std::any, std::move_only_function (C++23)
```

### 深入扩展

1. **SBO (Small Buffer Optimization)**：小对象内嵌存储，避免堆分配——`std::function` 通常 16-32 字节 SBO
2. **`std::move_only_function` (C++23)**：不要求拷贝，支持 move-only 类型，开销更小
3. **多接口 Type Erasure**：同时擦除 `draw()` + `serialize()` + `clone()` 多个方法
4. **dyno 库**：基于 Concepts 的编译期 Type Erasure——避免虚函数表
5. **与 variant 对比**：Type Erasure 开放（新类型无需修改容器），variant 封闭（编译期确定）

---

## 第25章：variant 访问者 vs 虚函数性能

### 25.1 两种实现对比

```cpp
// 虚函数方式
struct ShapeBase { virtual double area() const = 0; };
struct VCircle : ShapeBase { double area() const override { ... } };
std::vector<std::unique_ptr<ShapeBase>> shapes;  // 堆，分散

// variant 方式
using Shape = std::variant<SCircle, SRect, STri>;
struct AreaVisitor { ... };
std::vector<Shape> shapes;  // 栈，连续
```

### 25.2 性能基准（代码中实测）

```
虚函数 ×1000000: ~Xms  (指针间接 + cache miss)
variant ×1000000: ~Yms  (内联 + 连续内存)

sizeof(unique_ptr<Base>) = 8 bytes (指针)，对象在堆上分散
sizeof(Shape) = max(sizeof(S...)) + 判别符，连续排列，cache 友好
→ variant 通常更快
```

### 25.3 选型指南

```
类型集合固定 (编译期已知所有类型) → variant
  优点: 编译期确定、cache 友好、无堆分配
  缺点: 添加新类型需重编译

类型集合开放 (运行时可扩展) → 虚函数 / Type Erasure
  优点: 插件式扩展、不需重编译
  缺点: 堆分配、间接调用
```

### 深入扩展

1. **`std::visit` 优化**：小型 variant (< 4 类型) 编译器生成 if-else 链；大型 variant 生成跳转表
2. **`variant` 与异常**：赋值期间类型构造抛异常 → `valueless_by_exception` 状态
3. **Double dispatch**：`std::visit(overloaded, v1, v2)` 同时访问两个 variant — 实现多分派
4. **variant + 递归 (JSON AST)**：需要通过 `unique_ptr` 打破递归——间接增加一层
5. **Benchmark 工具**：Google Benchmark + Compiler Explorer 精确对比两种实现的汇编

---

# 五、编译期计算篇

---

## 第26章：constexpr 全面深入

### 26.1 核心算法示例

```cpp
// 编译期 sqrt（牛顿法）
constexpr double constexpr_sqrt(double x) {
    double guess = x;
    for (int i = 0; i < 100; ++i) {
        double next = 0.5 * (guess + x / guess);
        if (guess == next) break;
        guess = next;
    }
    return guess;
}
static_assert(constexpr_sqrt(2.0) > 1.414 && constexpr_sqrt(2.0) < 1.415);

// 编译期哈希（FNV-1a）
constexpr uint32_t fnv1a_hash(std::string_view sv) {
    uint32_t hash = 2166136261u;
    for (char c : sv) { hash ^= c; hash *= 16777619u; }
    return hash;
}

// 编译期排序（插入排序）
template<typename T, size_t N>
constexpr std::array<T, N> constexpr_sort(std::array<T, N> arr) { ... }

// 编译期 sin 查找表（泰勒展开）
constexpr auto make_sin_table() {
    std::array<double, 360> table{};
    for (int i = 0; i < 360; ++i) { /* 泰勒展开 */ }
    return table;
}
constexpr auto sin_table = make_sin_table();
// sin_table[30] ≈ 0.5, sin_table[90] ≈ 1.0 — 零运行时开销

// 编译期 switch-case（哈希字符串）
switch (fnv1a_hash(cmd)) {
    case fnv1a_hash("start"):  return "启动";  // 编译期常量
    case fnv1a_hash("stop"):   return "停止";
}
```

### 26.2 constexpr 演进

```
C++11: 单 return 语句
C++14: 循环、变量、多语句
C++17: if constexpr, constexpr lambda
C++20: new/delete, virtual, dynamic_cast, std::vector/string
C++23: 几乎无限制 (static, goto 除外)
```

### 深入扩展

1. **`constexpr std::vector/string` (C++20)**：编译期可用动态容器——但不能跨编译期/运行期边界（不能是 constexpr 变量）
2. **编译期 JSON 解析**：constexpr 函数 + FixedString → 编译期解析 JSON 配置文件
3. **编译期正则匹配 (CTRE)**：`ctre::match<"^\\d{3}-\\d{4}$">(str)` 编译期优化正则
4. **编译期诊断**：`static_assert` + `constexpr` 函数 → 自定义编译错误消息
5. **constexpr 与 UB**：编译期执行中的 UB（如整数溢出）会被编译器捕获并报错——比运行时更安全

---

## 第27章：编译期容器与算法

### 27.1 ConstexprMap 实现

```cpp
template<typename K, typename V, size_t N>
class ConstexprMap {
    std::array<std::pair<K, V>, N> data_;

    constexpr size_t lower_bound(const K& key) const {
        size_t lo = 0, hi = N;
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (data_[mid].first < key) lo = mid + 1;
            else hi = mid;
        }
        return lo;
    }
public:
    constexpr ConstexprMap(std::array<std::pair<K, V>, N> data) : data_(data) {
        // 编译期排序（插入排序）
        for (size_t i = 1; i < N; ++i) { ... }
    }
    constexpr const V* find(const K& key) const {
        size_t idx = lower_bound(key);
        if (idx < N && data_[idx].first == key) return &data_[idx].second;
        return nullptr;
    }
};

constexpr ConstexprMap http_status(std::array{
    std::pair{200, "OK"}, std::pair{404, "Not Found"},
    std::pair{500, "Internal Error"}, std::pair{301, "Moved"}
});

constexpr auto s200 = http_status.at(200); // "OK" — 编译期查询
static_assert(std::string_view(s200) == "OK");
```

### 深入扩展

1. **`frozen::map` 库**：编译期完美哈希的 constexpr map——O(1) 查找
2. **编译期 set**：基于排序 array + 二分查找实现编译期集合
3. **C++20 `constexpr std::vector` 限制**：可以在 constexpr 函数中使用，但结果必须转为 array 输出
4. **编译期数据库**：ConstexprMap + FixedString 实现编译期键值存储
5. **constexpr 算法复杂度**：编译期排序/查找的时间复杂度不影响运行时——只影响编译速度

---

## 第28章：模板元编程 — 类型列表

### 28.1 TypeList 操作

```cpp
template<typename... Ts> struct TypeList {};

// 基础操作
Size<TypeList<int, double, string>>           → 3
TypeAt_t<0, TypeList<int, double, string>>    → int
IndexOf<double, TypeList<int, double, string>> → 1
Contains<char, TypeList<int, double>>          → false

// 变换
PushBack<TypeList<int>, double>  → TypeList<int, double>
PushFront<char, TypeList<int>>   → TypeList<char, int>
Transform<TypeList<int, double>, std::add_const>  → TypeList<const int, const double>
```

### 28.2 实际意义

```
TypeList → 编译期 vector<type>
Size     → .size()
TypeAt   → operator[]
Contains → find()
Transform→ ranges::transform()

应用: 序列化框架、反射系统、消息分发、变体类型生成
```

### 深入扩展

1. **Filter / Remove 操作**：编译期按条件过滤类型列表——`RemoveIf<is_pointer, TypeList<int, int*, double>>`
2. **Unique / Sort 操作**：编译期去重和排序类型列表——用于优化 variant
3. **TypeList → tuple / variant 转换**：`TypeListTo<std::variant, MyTypes>` 自动生成 variant 类型
4. **与反射结合 (C++26 提案)**：`meta::members_of(^MyClass)` 获取成员类型列表
5. **Boost.Mp11**：成熟的类型列表操作库，提供 100+ 元函数

---

## 第29章：编译期字符串

### 29.1 FixedString 实现

```cpp
template<size_t N>
struct FixedString {
    char data_[N] = {};
    constexpr FixedString(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i) data_[i] = str[i];
    }
    constexpr size_t size() const { return N - 1; }
    constexpr std::string_view view() const { return {data_, N - 1}; }
};
template<size_t N> FixedString(const char (&)[N]) -> FixedString<N>;

// 编译期拼接
template<size_t N1, size_t N2>
constexpr auto concat(const FixedString<N1>& a, const FixedString<N2>& b) {
    FixedString<N1 + N2 - 1> result;
    // ...
    return result;
}

constexpr auto greeting = concat(FixedString("Hello"), FixedString(" World!"));
static_assert(greeting.view() == "Hello World!");

// 编译期大小写
constexpr auto upper = to_upper(FixedString("hello"));
static_assert(upper.view() == "HELLO");
```

### 29.2 NTTP 应用 (C++20)

```cpp
template<FixedString Name>
struct NamedTag {
    static constexpr auto name = Name;
};
using MyTag = NamedTag<"event_click">;
// → 编译期字符串作为模板参数，实现类型级标签
```

### 深入扩展

1. **CTRE (Compile-Time Regular Expressions)**：`ctre::match<"\\d+">(str)` 正则在编译期编译为状态机
2. **编译期 JSON 键名**：`Field<"username">` 用 NTTP 字符串实现静态反射
3. **编译期格式串验证**：`std::format` 在编译期验证格式串与参数类型匹配
4. **`std::basic_fixed_string` (提案)**：标准化的固定长度字符串类型
5. **编译期 SQL 解析**：NTTP + constexpr 解析 SQL 字符串生成类型安全查询

---

## 第30章：编译期状态机

### 30.1 设计

```cpp
enum class State { Idle, Running, Paused, Stopped };
enum class Event { Start, Pause, Resume, Stop };

// 编译期转换表
constexpr std::optional<State> transition(State s, Event e) {
    if (s == State::Idle    && e == Event::Start)  return State::Running;
    if (s == State::Running && e == Event::Pause)  return State::Paused;
    if (s == State::Running && e == Event::Stop)   return State::Stopped;
    if (s == State::Paused  && e == Event::Resume) return State::Running;
    if (s == State::Paused  && e == Event::Stop)   return State::Stopped;
    return std::nullopt;  // 非法转换
}

// 编译期验证状态序列（折叠表达式）
template<Event... events>
constexpr State run_state_machine() {
    State current = State::Idle;
    bool valid = true;
    ((valid = valid && [&] {
        auto next = transition(current, events);
        if (next) { current = *next; return true; }
        return false;
    }()), ...);
    return valid ? current : State::Idle;
}

// 编译期验证合法
constexpr auto final = run_state_machine<
    Event::Start, Event::Pause, Event::Resume, Event::Stop>();
static_assert(final == State::Stopped);  // ✅

// 同一份代码运行期也可用
```

### 深入扩展

1. **Boost.SML (State Machine Library)**：零开销编译期状态机库——生产级实现
2. **状态机 + variant**：每个状态持有不同数据——`variant<Idle, Running, Paused, Stopped>`
3. **层次状态机 (HSM)**：状态嵌套子状态——UML statechart 的 C++ 实现
4. **状态机可视化**：从 constexpr 转换表自动生成 Mermaid/Graphviz 状态图
5. **协议验证**：编译期验证消息序列的合法性——如 TCP 握手顺序

---

# 六、高级模板技巧篇

---

## 第31章：SFINAE → Concepts 演进路线

### 31.1 四代方案

```
C++11  SFINAE + enable_if   → 难写、难读、难调试
C++14  void_t / is_detected → 稍好但仍复杂
C++17  if constexpr         → 简单, 但不能用于重载决议
C++20  Concepts             → 最终解决方案
```

### 31.2 代码对比

```cpp
// C++11 SFINAE
template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
std::string stringify(T val) { return "int:" + std::to_string(val); }

// C++17 if constexpr
template<typename T>
std::string stringify(T val) {
    if constexpr (std::is_integral_v<T>) return "int:" + std::to_string(val);
    else if constexpr (std::is_floating_point_v<T>) return "float:" + std::to_string(val);
    else return "other";
}

// C++20 Concepts
void process(std::integral auto x);     // 只接受整数类型
void process(std::floating_point auto x); // 只接受浮点类型
```

### 深入扩展

1. **错误消息质量**：SFINAE "substitution failure..." vs Concepts "constraint not satisfied because..." 
2. **Concepts 与重载决议**：更受约束的 concept 优先匹配——实现精确的重载控制
3. **`std::is_detected` (Library Fundamentals TS)**：标准化的检测习惯用法
4. **向后兼容策略**：`#if __cpp_concepts >= 202002L` 条件编译，逐步迁移到 Concepts
5. **Concepts 错误冒泡**：嵌套 concept 约束失败时会显示完整的约束链——调试更容易

---

## 第32章：变参模板高级模式

### 32.1 核心技巧

```cpp
// index_sequence — 编译期索引序列
template<typename Tuple, size_t... Is>
void print_tuple_impl(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

// for_each_tuple — 对 tuple 每个元素应用函数
template<typename... Ts, typename F>
void for_each_tuple(std::tuple<Ts...>& t, F&& f) {
    for_each_impl(t, std::forward<F>(f), std::index_sequence_for<Ts...>{});
}

// MultiInherit — 变参多继承 (Overloaded 模式)
template<typename... Bases>
struct MultiInherit : Bases... { using Bases::operator()...; };
template<typename... Bases>
MultiInherit(Bases...) -> MultiInherit<Bases...>;

auto visitor = MultiInherit{
    [](int i) { ... },
    [](double d) { ... },
    [](const std::string& s) { ... }
};
std::visit(visitor, variant_val);
```

### 深入扩展

1. **`std::index_sequence` 生成技巧**：`make_index_sequence<N>` 编译期 O(log N) 生成
2. **参数包展开的求值顺序**：C++17 折叠表达式保证从左到右——C++11 可能无序
3. **变参模板 + 递归继承**：`tuple` 的典型实现——每层继承持有一个元素
4. **`std::conjunction / disjunction / negation`**：短路求值的变参类型 trait
5. **Pack indexing (C++26 提案)**：`Ts...[I]` 直接索引参数包——取代 TypeAt 元函数

---

## 第33章：完美转发与万能引用

### 33.1 引用折叠规则

```
T& &   → T&    (左值 + 左值 = 左值)
T& &&  → T&    (左值 + 右值 = 左值)
T&& &  → T&    (右值 + 左值 = 左值)
T&& && → T&&   (右值 + 右值 = 右值)

std::forward<T>(arg):
  T = X&  → 返回 X&  (左值)
  T = X   → 返回 X&& (右值)
```

### 33.2 典型陷阱

```cpp
// ❌ 万能引用构造函数会"吞掉"拷贝构造函数
struct Widget {
    template<typename T>
    Widget(T&& name) : name_(std::forward<T>(name)) {}
    // Widget w2 = w1;  ← 调用万能引用版本，而非拷贝构造!
};

// ✅ 用 enable_if 排除自身
struct Widget {
    template<typename T,
             std::enable_if_t<!std::is_same_v<std::decay_t<T>, Widget>, int> = 0>
    Widget(T&& name) : name_(std::forward<T>(name)) {}
    Widget(const Widget&) = default;
};
```

### 33.3 关键区分

```
template<typename T> void f(T&& x)  → 万能引用 (T 待推导)
auto&& x = ...                      → 万能引用 (auto 待推导)
Widget&& x = ...                    → 普通右值引用 (类型已确定!)
```

### 深入扩展

1. **`std::forward_like` (C++23)**：按另一个值的值类别转发——`forward_like<Outer>(inner)`
2. **emplace 实现**：`vector::emplace_back(Args&&... args)` 就是完美转发到就地构造
3. **转发引用 + Concepts**：`template<std::integral T> void f(T&& x)` — T 是 `integral` 且是万能引用
4. **`decltype(auto)` 返回**：保留完美的值类别转发到返回值
5. **noexcept 转发**：`noexcept(noexcept(std::forward<T>(func)(args...)))` 条件 noexcept

---

## 第34章：Lambda 高级用法

### 34.1 C++14 → C++23 演进

```cpp
// C++14: 泛型 lambda + init capture
auto add = [](auto a, auto b) { return a + b; };
auto f = [ptr = std::make_unique<int>(42)]() { return *ptr; }; // 移动捕获

// C++17: constexpr lambda
constexpr auto sq = [](int x) { return x * x; };
static_assert(sq(5) == 25);

// IIFE (立即调用)
const auto config = [&] {
    std::map<std::string, int> cfg;
    cfg["timeout"] = 5000;
    return cfg;
}();

// Y-combinator (递归 lambda)
auto fib = [](auto self, int n) -> int {
    return n < 2 ? n : self(self, n-1) + self(self, n-2);
};
fib(fib, 10);  // 55

// 高阶函数
auto make_adder = [](int n) { return [n](int x) { return x + n; }; };
auto add10 = make_adder(10);

// Lambda 重载集
auto handler = Overloaded{
    [](int i) { ... }, [](double d) { ... }, [](const string& s) { ... }
};
```

### 34.2 C++20/23 新能力

```cpp
// C++20: 模板 lambda
auto f = []<typename T>(std::vector<T>& v) { v.push_back(T{}); };

// C++23: deducing this (递归 lambda 不需要 Y-combinator)
auto fib = [](this auto self, int n) -> int {
    return n < 2 ? n : self(n-1) + self(n-2);
};

// C++23: static lambda (无 this，可转函数指针)
auto f = [](int x) static { return x * 2; };
```

### 深入扩展

1. **Lambda 大小与 SBO**：无捕获 lambda 大小 = 1 byte；有捕获 = 捕获变量总大小——影响 `std::function` SBO
2. **`*this` 拷贝捕获 (C++17)**：`[*this]() { ... }` 拷贝整个对象，避免悬空 this
3. **Lambda 闭包与 ABI**：每个 lambda 表达式生成唯一类型——不同翻译单元的 lambda 不同类型
4. **Immediately Invoked Lambda (IIFE) 最佳实践**：初始化 const 变量时使用，限制临时变量作用域
5. **Coroutine lambda**：`auto coro = [](int n) -> Generator<int> { co_yield n; };`

---

## 第35章：性能对比与最佳实践总结

### 35.1 抽象机制对比表

```
┌──────────────┬──────────┬──────────┬──────────┬──────────┐
│ 机制         │ 分发方式 │ 内联     │ 扩展性   │ 缓存     │
├──────────────┼──────────┼──────────┼──────────┼──────────┤
│ 虚函数       │ 运行时   │ 困难     │ 开放     │ 差       │
│ CRTP         │ 编译期   │ 完全     │ 封闭     │ 好       │
│ variant+visit│ 运行时   │ 可内联   │ 封闭     │ 好       │
│ Type Erasure │ 运行时   │ 困难     │ 开放     │ 差       │
│ Concepts     │ 编译期   │ 完全     │ 开放     │ 好       │
│ if constexpr │ 编译期   │ 完全     │ 封闭     │ 好       │
└──────────────┴──────────┴──────────┴──────────┴──────────┘
```

### 35.2 六大原则

```
1. 编译期优先:
   constexpr → consteval → if constexpr → 查找表
   能编译期 → 不要运行期

2. 值语义优先:
   variant → optional → expected → strong types
   能值类型 → 不用指针/继承

3. 零拷贝优先:
   string_view → span → move semantics
   能引用 → 不要拷贝

4. 类型安全优先:
   strong types → concepts → enum class
   能类型检查 → 不要裸类型

5. 模板技巧选择:
   if constexpr > enable_if > tag dispatch > SFINAE
   Concepts (C++20) > 以上所有

6. 内存布局:
   连续 (vector/array) > 链式 (list/map)
   SoA > AoS (SIMD 场景)
   alignas(64) 避免 false sharing
```

### 35.3 工具链

| 工具 | 用途 | URL |
|---|---|---|
| Compiler Explorer | 查看汇编 | godbolt.org |
| C++ Insights | 查看编译器展开 | cppinsights.io |
| Quick Bench | 在线基准测试 | quick-bench.com |

---

## 附录 A：test12.cpp 扩展建议总结

| 章 | 扩展方向 | 优先级 | 关联 |
|---|---|---|---|
| Ch1 | C++20 lambda 捕获结构化绑定 / 反射 (C++26) | 中 | — |
| Ch2 | C++23 `if consteval` / `static_assert` 交互 | 中 | Ch12 |
| Ch3 | `constexpr` 折叠 / C++23 `ranges::fold` 对比 | 中 | Ch10 |
| Ch4 | 聚合类 CTAD (C++20) / 别名模板 CTAD | 低 | — |
| Ch5 | optional monadic (C++23) / recursive variant | 高 | Ch17 |
| Ch6 | `std::span` / `starts_with` (C++20) / 生命周期 | 高 | — |
| Ch7 | `[[likely/unlikely]]` / `[[no_unique_address]]` (C++20) | 中 | test14 |
| Ch8 | `std::bind_front/back` (C++20/23) / `invoke_r` | 中 | — |
| Ch9 | 标准库 Concepts 速查 / Concept 偏序 / 组合约束 | 高 | Ch31 |
| Ch10 | 自定义 view / `ranges::to` (C++23) / 并行提案 | 高 | — |
| Ch11 | 异构比较 / 与容器交互 | 低 | — |
| Ch12 | `consteval` + `if consteval` 组合 / constinit thread_local | 高 | Ch26 |
| Ch13 | `std::generator` (C++23) / io_uring 协程 / HALO 优化 | 高 | test9 |
| Ch14 | `format_to` 零分配 / 自定义 formatter 进阶 | 中 | — |
| Ch15 | CMake 模块支持 / Header Units / 编译器差异 | 高 | — |
| Ch16 | Deducing this + Concepts / CRTP 替代 | 高 | Ch21 |
| Ch17 | `expected<void, E>` / 协程配合 / Boost.Outcome | 高 | Ch13 |
| Ch18-20 | mdspan 布局策略 / flat_map 基准 / move_only_function | 中 | — |
| Ch21 | Deducing this 替代 / 多级 CRTP / 代码膨胀分析 | 高 | Ch16 |
| Ch22 | 运行时策略切换 / Concepts 校验策略 / pmr | 中 | — |
| Ch23 | mp-units 物理量库 / chrono 范例 | 高 | — |
| Ch24 | SBO 实现 / move_only type erasure / dyno 库 | 高 | — |
| Ch25 | 递归 variant (JSON AST) / double dispatch | 高 | — |
| Ch26 | constexpr std::vector (C++20) / CTRE / UB 检测 | 高 | Ch29 |
| Ch27 | frozen::map 完美哈希 / constexpr 算法库 | 中 | — |
| Ch28 | Filter/Unique/Sort 操作 / TypeList → variant | 中 | — |
| Ch29 | CTRE 正则 / 编译期 SQL / NTTP 进阶 | 高 | Ch26 |
| Ch30 | Boost.SML / variant 状态机 / 层次状态机 | 高 | — |
| Ch31 | Concepts 错误冒泡 / is_detected / 迁移策略 | 中 | Ch9 |
| Ch32 | Pack indexing (C++26) / conjunction/disjunction | 中 | — |
| Ch33 | `forward_like` (C++23) / noexcept 转发 | 中 | — |
| Ch34 | Coroutine lambda / Lambda ABI / `*this` 捕获场景 | 中 | Ch13 |
| Ch35 | 各机制汇编对比 (godbolt) / 实测基准数据 | 高 | test14 |

---

## 附录 B：典型输出示例

```
================================================================
 精通现代 C++ — 零开销抽象与编译期优化 完全教程
================================================================

╔══════════════════════════════════════════════════════╗
║ 一、C++17 核心特性篇                                  ║
╚══════════════════════════════════════════════════════╝

  ── 结构化绑定 (Structured Bindings) ──
  数组: a=10 b=20 c=30
  pair: name = 42
  tuple: (1, 2, 3)
  聚合: pt.x=100 (已被修改)
  map 遍历:
    Alice: 95
    Bob: 87
  Color: R=255 G=128 B=0
  插入成功: Charlie = 92

  ── if constexpr — 编译期分支消除 ──
  int  → 整数类型
  double → 浮点类型
  SmartContainer<int>.sum() = 60

  ── 折叠表达式 (Fold Expressions) ──
  sum(1,2,3,4,5) = 15
  constexpr max_of(3,7,2,9,1) = 9
  pipeline(10, *2, +3, ^2) = 529

  ── string_view — 零拷贝字符串引用 ──
  string::substr ×100000: 12.5ms
  string_view::substr ×100000: 0.3ms

╔══════════════════════════════════════════════════════╗
║ 四、零开销抽象篇                                      ║
╚══════════════════════════════════════════════════════╝

  ── CRTP — 静态多态 ──
  Circle(5):     面积 = 78.5398
  Rectangle(3,4): 面积 = 12
  Widget 实例数: 2
  Widget 实例数(销毁后): 0

  ── variant 访问者 vs 虚函数 ──
  虚函数 ×1000000: 18.5ms
  variant  ×1000000: 12.3ms

╔══════════════════════════════════════════════════════╗
║ 五、编译期计算篇                                      ║
╚══════════════════════════════════════════════════════╝

  ── constexpr 全面深入 ──
  constexpr sqrt(2) = 1.414213562
  constexpr hash("hello") = 0x4f9f2cab
  constexpr sort: 1 2 3 4 5
  sin(30°) = 0.500000 (编译期查找表)
  command("start") → 启动

  ── 编译期容器与算法 ──
  http_status[200] = OK (编译期)
  http_status[404] = Not Found (编译期)

================================================================
 演示完成
================================================================
```

---

## 附录 C：核心设计原则

> **零开销抽象 = 你不用的特性不付开销，你用的特性手写也不会更好。** 这是 C++ 的根本哲学。本教程的 35 章从三个维度实践了这一原则：**语言层** — if constexpr / fold / constexpr / Concepts 让编译器在编译期完成分发和计算；**设计层** — CRTP / Policy / Strong Types / Type Erasure 让抽象以零运行时代价存在；**工具层** — Compiler Explorer / C++ Insights 让你验证"零开销"确实为零。现代 C++ 的进化方向始终是：**把能在编译期做的事移到编译期，把能用类型系统保证的安全移到类型系统。**
