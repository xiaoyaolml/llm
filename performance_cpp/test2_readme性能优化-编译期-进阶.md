# C++ 编译期优化进阶教程

> **对应源码**: `test2.cpp` （本教程是 `test1.cpp` 基础教程的延续，聚焦更高级的编译期技术）
>
> **前置知识**: 建议先阅读 `test1_readme性能优化-编译期-基础.md`

---

## 编译方式

```bash
# GCC / Clang (需 C++20)
g++ -std=c++20 -O2 -o test2 test2.cpp && ./test2

# MSVC
cl /std:c++20 /O2 /EHsc test2.cpp
```

---

## 教程目录

| 章节 | 主题 | 核心技术 |
|------|------|----------|
| 1 | **Concepts (C++20)** | 概念定义、约束语法（3种写法）、组合概念、重载决议 |
| 2 | **编译期反射与类型名获取** | `__PRETTY_FUNCTION__` / `__FUNCSIG__`、FNV-1a 类型ID |
| 3 | **编译期模式匹配** | `overloaded` 模式、类型偏特化匹配 |
| 4 | **std::tuple 编译期操作** | 遍历、变换(map)、过滤、反转、类型查找 |
| 5 | **编译期排序** | constexpr 冒泡排序、快速排序、二分查找 |
| 6 | **SFINAE 与 enable_if 深入** | `enable_if`、`void_t` 检测成员/运算符、与 Concepts 对比 |
| 7 | **表达式模板 (Expression Templates)** | CRTP + 惰性求值，消除临时对象（线性代数库核心技术） |
| 8 | **编译期位操作与位域** | popcount、clz、log2、2的幂判断、类型安全 Bitflags |
| 9 | **constexpr 动态内存分配 (C++20)** | 编译期 new/delete、编译期 vector、编译期链表 |
| 10 | **Policy-Based Design** | 日志策略、分配策略、零开销策略组合 |
| 11 | **Tag Dispatch (标签分派)** | 迭代器分类分派、序列化标签分派 |
| 12 | **编译期依赖注入与工厂** | CRTP 工厂、模板注入服务 |
| 附录 | **编译期优化最佳实践** | 7 条实战建议 |

### 两个文件的关系

| | 基础篇 (test1.cpp) | 进阶篇 (test2.cpp) |
|---|---|---|
| constexpr | 基础函数、类、向量运算 | constexpr 动态分配、排序、位操作 |
| 模板 | TMP 阶乘、素数、整数序列 | 表达式模板、Tuple 操作、类型列表 |
| 分支 | if constexpr 基础 | Concepts、SFINAE 深入、Tag Dispatch |
| 多态 | CRTP 基础 | Policy-Based Design、编译期工厂 |
| 应用 | 查找表、状态机 | 模式匹配、依赖注入 |

---

## 第1章：Concepts — C++20 概念约束

Concepts 是 C++20 最重要的特性之一，为模板参数提供**声明式的约束语义**，替代了晦涩的 SFINAE。

**优点**：
- 错误信息清晰可读
- 代码意图一目了然
- 支持组合与重载决议

### 1.1 基础概念定义

```cpp
// 定义一个概念：类型 T 必须支持算术运算和比较
template <typename T>
concept Arithmetic = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    { a < b } -> std::convertible_to<bool>;
};

// 定义一个概念：可调用并返回指定类型
template <typename F, typename R, typename... Args>
concept CallableReturning = requires(F f, Args... args) {
    { f(args...) } -> std::convertible_to<R>;
};
```

### 1.2 使用概念约束函数模板（3种写法）

```cpp
// 方式1：requires 子句
template <typename T>
    requires Arithmetic<T>
constexpr T safe_add(T a, T b) {
    return a + b;
}

// 方式2：简写形式 (terse syntax)
constexpr auto safe_multiply(Arithmetic auto a, Arithmetic auto b) {
    return a * b;
}

// 方式3：trailing requires
template <typename T>
constexpr T safe_divide(T a, T b) requires Arithmetic<T> {
    return a / b;
}

static_assert(safe_add(3, 4) == 7, "concept add");
static_assert(safe_multiply(3, 5) == 15, "concept multiply");
```

### 1.3 组合概念

概念可以通过逻辑运算符组合：

```cpp
template <typename T>
concept Printable = requires(std::ostream& os, T val) {
    { os << val } -> std::same_as<std::ostream&>;
};

template <typename T>
concept ArithmeticAndPrintable = Arithmetic<T> && Printable<T>;

template <ArithmeticAndPrintable T>
void compute_and_print(T a, T b) {
    std::cout << a << " + " << b << " = " << (a + b) << "\n";
    std::cout << a << " * " << b << " = " << (a * b) << "\n";
}
```

### 1.4 概念用于重载决议

更特化的概念**优先匹配**，编译器自动选择最佳匹配：

```cpp
template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <Integral T>
constexpr T smart_abs(T x) { return x < 0 ? -x : x; }

template <FloatingPoint T>
constexpr T smart_abs(T x) { return x < 0.0 ? -x : x; }

static_assert(smart_abs(-5) == 5,      "integral abs");
static_assert(smart_abs(-3.14) == 3.14, "floating abs");
```

### 1.5 概念约束容器

```cpp
template <typename C>
concept Container = requires(C c) {
    typename C::value_type;
    typename C::iterator;
    { c.begin() } -> std::same_as<typename C::iterator>;
    { c.end() }   -> std::same_as<typename C::iterator>;
    { c.size() }  -> std::convertible_to<std::size_t>;
};

template <Container C>
void print_container(const C& c) {
    std::cout << "[";
    bool first = true;
    for (const auto& elem : c) {
        if (!first) std::cout << ", ";
        std::cout << elem;
        first = false;
    }
    std::cout << "]\n";
}
```

---

## 第2章：编译期反射与类型名获取

C++ 目前没有正式的反射机制，但可以利用编译器特性在编译期获取类型名。

### 2.1 利用编译器内置宏获取类型名

```cpp
template <typename T>
constexpr std::string_view type_name() {
    #if defined(__clang__)
        constexpr std::string_view prefix = "[T = ";
        constexpr std::string_view suffix = "]";
        constexpr std::string_view function = __PRETTY_FUNCTION__;
    #elif defined(__GNUC__)
        constexpr std::string_view prefix = "with T = ";
        constexpr std::string_view suffix = "]";
        constexpr std::string_view function = __PRETTY_FUNCTION__;
    #elif defined(_MSC_VER)
        constexpr std::string_view prefix = "type_name<";
        constexpr std::string_view suffix = ">(void)";
        constexpr std::string_view function = __FUNCSIG__;
    #endif

    constexpr auto start = function.find(prefix) + prefix.size();
    constexpr auto end = function.rfind(suffix);
    return function.substr(start, end - start);
}

// 调用: type_name<int>() → "int"
// 调用: type_name<std::string>() → "std::basic_string<char>"
```

### 2.2 编译期类型 ID（哈希）

利用类型名生成唯一的编译期哈希 ID：

```cpp
template <typename T>
constexpr uint32_t type_id() {
    constexpr std::string_view name = type_name<T>();
    uint32_t hash = 2166136261u; // FNV-1a
    for (char c : name) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

// 不同类型生成不同的编译期 ID
// type_id<int>() != type_id<double>()
```

> **应用场景**：编译期类型注册表、简易 RTTI 替代方案。

> **NOTE (补充说明)**:
> - **局限性**: 这种基于函数签名字符串解析的方法是一种“黑科技”，并非标准反射。它对复杂模板类型、匿名命名空间中的类型或别名（using/typedef）的处理可能不一致或不优雅。例如，`std::string` 可能会显示为 `std::basic_string<char, ...>`。
> - **稳定性**: 这种方法的输出完全依赖于特定编译器的实现，并且可能随着编译器版本的更新而改变。在需要跨编译器或长期保持稳定性的场景下，应谨慎使用。

---

## 第3章：编译期模式匹配

虽然 C++ 还没有原生的模式匹配（P2688R0 提案中），但可以用模板技术实现类似功能。

### 3.1 overloaded 模式 — 模拟 Variant 访问

```cpp
// 重载集合工具（C++17 经典技巧）
template <typename... Fs>
struct overloaded : Fs... {
    using Fs::operator()...;
};

// C++17 CTAD 推导指引
template <typename... Fs>
overloaded(Fs...) -> overloaded<Fs...>;

// 使用示例
void pattern_match_demo() {
    auto matcher = overloaded{
        [](int i)    { std::cout << "int: " << i << "\n"; },
        [](double d) { std::cout << "double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "string: " << s << "\n"; },
        [](auto x)   { std::cout << "other: " << x << "\n"; }
    };

    matcher(42);             // → int: 42
    matcher(3.14);           // → double: 3.14
    matcher(std::string("hello")); // → string: hello
    matcher('A');            // → other: A
}
```

> **原理**：`overloaded` 继承所有 lambda，`using Fs::operator()...` 让所有 `operator()` 参与重载决议。

### 3.2 编译期递归模式匹配

对类型进行"模式匹配"——通过模板特化实现：

```cpp
template <typename T> struct match_type {
    static constexpr const char* name = "unknown";
};
template <> struct match_type<int> {
    static constexpr const char* name = "integer";
};
template <> struct match_type<double> {
    static constexpr const char* name = "double";
};
template <> struct match_type<std::string> {
    static constexpr const char* name = "string";
};
// 偏特化：匹配所有 vector<T>
template <typename T> struct match_type<std::vector<T>> {
    static constexpr const char* name = "vector";
};

static_assert(std::string_view(match_type<int>::name) == "integer");
static_assert(std::string_view(match_type<std::vector<int>>::name) == "vector");
```

---

## 第4章：std::tuple 编译期操作

`tuple` 是最重要的异构容器，其所有操作都发生在编译期。

### 4.1 编译期遍历 tuple

```cpp
template <typename Tuple, typename Func, size_t... Is>
void tuple_for_each_impl(const Tuple& t, Func&& f, std::index_sequence<Is...>) {
    (f(std::get<Is>(t)), ...);  // 折叠表达式展开
}

template <typename Tuple, typename Func>
void tuple_for_each(const Tuple& t, Func&& f) {
    tuple_for_each_impl(t, std::forward<Func>(f),
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

// 使用
auto my_tuple = std::make_tuple(1, 2.5, "hello", 42);
tuple_for_each(my_tuple, [](const auto& elem) {
    std::cout << elem << " ";
});
// 输出: 1 2.5 hello 42
```

### 4.2 编译期 tuple 变换 (map)

对 tuple 的每个元素应用变换函数，返回新 tuple：

```cpp
template <typename Tuple, typename Func, size_t... Is>
auto tuple_transform_impl(const Tuple& t, Func&& f, std::index_sequence<Is...>) {
    return std::make_tuple(f(std::get<Is>(t))...);
}

template <typename Tuple, typename Func>
auto tuple_transform(const Tuple& t, Func&& f) {
    return tuple_transform_impl(t, std::forward<Func>(f),
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

// 使用: 将 (1, 2, 3) 变换为 (2, 4, 6)
auto doubled = tuple_transform(std::make_tuple(1, 2, 3),
                               [](auto x) { return x * 2; });
```

### 4.3 编译期 tuple 过滤

```cpp
template <typename Pred, typename Tuple, size_t... Is>
auto tuple_filter_impl(const Tuple& t, std::index_sequence<Is...>) {
    return std::tuple_cat(
        [&]() {
            if constexpr (Pred{}(std::get<Is>(t))) {
                return std::make_tuple(std::get<Is>(t));
            } else {
                return std::tuple<>{};
            }
        }()...
    );
}
```

### 4.4 编译期 tuple 反转

```cpp
template <typename Tuple, size_t... Is>
auto tuple_reverse_impl(const Tuple& t, std::index_sequence<Is...>) {
    constexpr size_t N = sizeof...(Is);
    return std::make_tuple(std::get<N - 1 - Is>(t)...);
}

template <typename Tuple>
auto tuple_reverse(const Tuple& t) {
    return tuple_reverse_impl(t,
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

// std::make_tuple(1, 2, 3, 4) → std::make_tuple(4, 3, 2, 1)
```

### 4.5 编译期 tuple 类型查找

```cpp
template <typename T, typename Tuple>
struct tuple_index;

template <typename T, typename Head, typename... Tail>
struct tuple_index<T, std::tuple<Head, Tail...>> {
    static constexpr size_t value = 1 + tuple_index<T, std::tuple<Tail...>>::value;
};

template <typename T, typename... Tail>
struct tuple_index<T, std::tuple<T, Tail...>> {
    static constexpr size_t value = 0;
};

static_assert(tuple_index<double, std::tuple<int, double, char>>::value == 1,
              "double is at index 1");
```

---

## 第5章：编译期排序

在编译期对 `constexpr` 数组进行排序 — 运行期直接访问已排序的数据，**零运行时排序开销**。

### 5.1 编译期冒泡排序

```cpp
template <typename T, size_t N>
constexpr std::array<T, N> ct_bubble_sort(std::array<T, N> arr) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                T tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
    return arr;
}

constexpr std::array<int, 8> unsorted = {64, 34, 25, 12, 22, 11, 90, 1};
constexpr auto sorted_arr = ct_bubble_sort(unsorted);

static_assert(sorted_arr[0] == 1,  "min element");
static_assert(sorted_arr[7] == 90, "max element");
static_assert(sorted_arr[3] == 22, "element at index 3");
```

### 5.2 编译期快速排序

使用栈模拟递归，避免编译期递归深度问题：

```cpp
template <typename T, size_t N>
constexpr std::array<T, N> ct_quick_sort(std::array<T, N> arr) {
    auto partition = [](std::array<T, N>& a, int low, int high) constexpr -> int {
        T pivot = a[high];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (a[j] <= pivot) {
                ++i;
                T tmp = a[i]; a[i] = a[j]; a[j] = tmp;
            }
        }
        T tmp = a[i + 1]; a[i + 1] = a[high]; a[high] = tmp;
        return i + 1;
    };

    // 用栈模拟递归
    int stack[64] = {};
    int top = -1;
    stack[++top] = 0;
    stack[++top] = static_cast<int>(N) - 1;

    while (top >= 0) {
        int high = stack[top--];
        int low  = stack[top--];
        int pi = partition(arr, low, high);
        if (pi - 1 > low) { stack[++top] = low; stack[++top] = pi - 1; }
        if (pi + 1 < high) { stack[++top] = pi + 1; stack[++top] = high; }
    }
    return arr;
}

constexpr std::array<int, 10> unsorted2 = {39, 27, 43, 3, 9, 82, 10, 55, 1, 99};
constexpr auto qsorted = ct_quick_sort(unsorted2);
static_assert(qsorted[0] == 1,  "qsort min");
static_assert(qsorted[9] == 99, "qsort max");
```

### 5.3 编译期二分查找

```cpp
template <typename T, size_t N>
constexpr int ct_binary_search(const std::array<T, N>& arr, T target) {
    int low = 0, high = static_cast<int>(N) - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1; // 未找到
}

static_assert(ct_binary_search(sorted_arr, 25) == 3,  "find 25");
static_assert(ct_binary_search(sorted_arr, 100) == -1, "100 not found");
```

---

## 第6章：SFINAE 与 enable_if 深入

SFINAE (Substitution Failure Is Not An Error) — 替换失败不是错误。这是 C++20 Concepts 出现之前控制重载决议的核心技术，理解它对于**阅读旧代码**至关重要。

### 6.1 基础 SFINAE: enable_if

```cpp
// 仅当 T 是整数类型时启用
template <typename T>
typename std::enable_if<std::is_integral_v<T>, T>::type
sfinae_double_value(T val) {
    return val * 2;
}

// 仅当 T 是浮点类型时启用
template <typename T>
typename std::enable_if<std::is_floating_point_v<T>, T>::type
sfinae_double_value(T val) {
    return val * 2.0;
}
```

### 6.2 检测成员函数是否存在（经典 SFINAE 技巧）

```cpp
// 检测类型 T 是否有 .serialize() 方法
template <typename T, typename = void>
struct has_serialize : std::false_type {};

template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>>
    : std::true_type {};

template <typename T>
constexpr bool has_serialize_v = has_serialize<T>::value;

struct Serializable { std::string serialize() const { return "data"; } };
struct NotSerializable { int value = 42; };

static_assert(has_serialize_v<Serializable>,    "has serialize");
static_assert(!has_serialize_v<NotSerializable>, "no serialize");
static_assert(!has_serialize_v<int>,             "int has no serialize");
```

### 6.3 SFINAE vs Concepts 对比

```cpp
// 旧方式 (SFINAE) — 晦涩:
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr T old_style_abs(T x) { return x < 0 ? -x : x; }

// 新方式 (Concepts) — 清晰:
template <std::integral T>
constexpr T new_style_abs(T x) { return x < 0 ? -x : x; }
```

> **结论**：新代码优先用 Concepts，但理解 SFINAE 对阅读旧库代码必不可少。

### 6.4 检测运算符是否存在

```cpp
template <typename T, typename U, typename = void>
struct has_addition : std::false_type {};

template <typename T, typename U>
struct has_addition<T, U, std::void_t<
    decltype(std::declval<T>() + std::declval<U>())
>> : std::true_type {};

static_assert(has_addition<int, double>::value,     "int + double exists");
static_assert(!has_addition<std::string, int>::value, "string + int doesn't");
```

---

## 第7章：表达式模板 (Expression Templates)

表达式模板是一种编译期优化技术，**避免中间临时对象的创建**。典型用途：线性代数库 (Eigen, Blaze 等)。

### 7.1 问题：朴素向量加法的性能问题

```
a + b + c
```

朴素实现会产生 **两个临时对象**：`temp1 = a+b`，`temp2 = temp1+c`。每次都分配内存、遍历数组。

表达式模板可以将整个表达式**延迟到赋值时一次性求值**，只遍历一次数组。

### 7.2 表达式模板实现

核心思想：运算符 `+` 不直接计算，而是返回一个**表达式节点**（轻量代理对象），实际计算在赋值时触发。

```cpp
// CRTP 表达式基类
template <typename E>
class VecExpr {
public:
    double operator[](size_t i) const {
        return static_cast<const E&>(*this)[i];
    }
    size_t size() const {
        return static_cast<const E&>(*this).size();
    }
};

// 表达式节点：加法（不做实际计算，只存引用）
template <typename L, typename R>
class VecAdd : public VecExpr<VecAdd<L, R>> {
    const L& lhs_;
    const R& rhs_;
public:
    VecAdd(const L& l, const R& r) : lhs_(l), rhs_(r) {}
    double operator[](size_t i) const { return lhs_[i] + rhs_[i]; }
    size_t size() const { return lhs_.size(); }
};

// 表达式节点：标量乘法
template <typename E>
class VecScale : public VecExpr<VecScale<E>> {
    const E& expr_;
    double scalar_;
public:
    VecScale(const E& e, double s) : expr_(e), scalar_(s) {}
    double operator[](size_t i) const { return expr_[i] * scalar_; }
    size_t size() const { return expr_.size(); }
};

// 运算符重载
template <typename L, typename R>
VecAdd<L, R> operator+(const VecExpr<L>& l, const VecExpr<R>& r) {
    return VecAdd<L, R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template <typename E>
VecScale<E> operator*(const VecExpr<E>& e, double s) {
    return VecScale<E>(static_cast<const E&>(e), s);
}
```

### 7.3 使用方式

```cpp
SmallVec a = {1.0, 2.0, 3.0, 4.0};
SmallVec b = {10.0, 20.0, 30.0, 40.0};
SmallVec c = wrap(a) + wrap(b);           // 无中间临时对象！
SmallVec d = (wrap(a) + wrap(b)) * 2.0;  // 融合计算
// c = {11, 22, 33, 44}
// d = {22, 44, 66, 88}
```

> **关键**：`wrap(a) + wrap(b)` 返回的是 `VecAdd` 代理对象（只有两个引用，无内存分配），赋值给 `SmallVec c` 时才遍历一次数组完成计算。

> **NOTE (补充说明)**:
> - **优点**: 表达式模板的核心优势是**消除临时对象**和**融合循环(Loop Fusion)**，将多个操作（如 `a+b+c`）合并到一次迭代中完成，极大地提高了缓存利用率和计算效率。
> - **缺点**:
>     - **编译时间**: 表达式模板会生成大量复杂的类型，显著增加编译时间。
>     - **调试难度**: 调试时看到的类型（如 `VecAdd<VecAdd<...>, ...>`）非常复杂，难以理解。
>     - **实现复杂**: 正确实现表达式模板需要深厚的模板元编程知识。
> - **`auto` 陷阱**: 如果使用 `auto result = wrap(a) + wrap(b);`，`result` 的类型是 `VecAdd` 代理对象，它持有对 `a` 和 `b` 的引用。如果 `a` 或 `b` 的生命周期结束，`result` 将持有悬垂引用，导致未定义行为。这是表达式模板最常见的错误来源。

---

## 第8章：编译期位操作与位域

位操作在嵌入式/系统编程中极为重要，许多位运算可以在编译期完成。

### 8.1 编译期位计数 (popcount)

```cpp
constexpr int ct_popcount(uint32_t x) {
    int count = 0;
    while (x) {
        count += (x & 1);
        x >>= 1;
    }
    return count;
}

static_assert(ct_popcount(0b10110101) == 5, "popcount");
static_assert(ct_popcount(0) == 0,          "popcount 0");
static_assert(ct_popcount(0xFFFFFFFF) == 32, "popcount all ones");
```

### 8.2 编译期前导零计数 (CLZ)

```cpp
constexpr int ct_clz(uint32_t x) {
    if (x == 0) return 32;
    int n = 0;
    if ((x & 0xFFFF0000) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF000000) == 0) { n += 8;  x <<= 8;  }
    if ((x & 0xF0000000) == 0) { n += 4;  x <<= 4;  }
    if ((x & 0xC0000000) == 0) { n += 2;  x <<= 2;  }
    if ((x & 0x80000000) == 0) { n += 1; }
    return n;
}

static_assert(ct_clz(0) == 32, "clz 0");
static_assert(ct_clz(1) == 31, "clz 1");
static_assert(ct_clz(0x80000000) == 0, "clz top bit");
```

### 8.3 编译期 log2

```cpp
constexpr int ct_log2(uint32_t x) {
    return 31 - ct_clz(x);
}

static_assert(ct_log2(1) == 0,     "log2(1)");
static_assert(ct_log2(8) == 3,     "log2(8)");
static_assert(ct_log2(1024) == 10, "log2(1024)");
```

### 8.4 编译期判断2的幂 & 向上取整

```cpp
constexpr bool ct_is_power_of_two(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

constexpr uint32_t ct_next_power_of_two(uint32_t x) {
    if (x == 0) return 1;
    --x;
    x |= x >> 1;  x |= x >> 2;
    x |= x >> 4;  x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

static_assert(ct_is_power_of_two(256), "256 is 2^8");
static_assert(!ct_is_power_of_two(100), "100 is not 2^n");
static_assert(ct_next_power_of_two(100) == 128, "next pow2 of 100");
```

### 8.5 编译期位反转

```cpp
constexpr uint32_t ct_reverse_bits(uint32_t x) {
    uint32_t result = 0;
    for (int i = 0; i < 32; ++i) {
        result = (result << 1) | (x & 1);
        x >>= 1;
    }
    return result;
}

static_assert(ct_reverse_bits(0x80000000) == 0x00000001, "reverse bits");
```

### 8.6 类型安全的 Bitflags

利用 `enum class` 和 Concepts 实现类型安全的位标志操作：

```cpp
template <typename EnumT>
    requires std::is_enum_v<EnumT>
class Bitflags {
    using UT = std::underlying_type_t<EnumT>;
    UT flags_ = 0;
public:
    constexpr Bitflags() = default;
    constexpr Bitflags(EnumT e) : flags_(static_cast<UT>(e)) {}

    constexpr Bitflags& set(EnumT e)    { flags_ |= static_cast<UT>(e); return *this; }
    constexpr Bitflags& clear(EnumT e)  { flags_ &= ~static_cast<UT>(e); return *this; }
    constexpr Bitflags& toggle(EnumT e) { flags_ ^= static_cast<UT>(e); return *this; }
    constexpr bool test(EnumT e) const  { return (flags_ & static_cast<UT>(e)) != 0; }
    constexpr UT raw() const { return flags_; }
};

enum class Permission : uint8_t {
    Read    = 1 << 0,
    Write   = 1 << 1,
    Execute = 1 << 2,
};

constexpr auto perms = Bitflags<Permission>{}
    .set(Permission::Read)
    .set(Permission::Write);

static_assert(perms.test(Permission::Read),     "has read");
static_assert(perms.test(Permission::Write),    "has write");
static_assert(!perms.test(Permission::Execute), "no execute");
```

---

## 第9章：constexpr 动态内存分配 (C++20)

C++20 允许在 `constexpr` 函数中使用 `new/delete`！但有一个限制：**编译期分配的内存必须在编译期释放**（不能"泄漏"到运行期）。

### 9.1 编译期 vector 式结构

```cpp
template <typename T>
class ConstexprVec {
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t cap_ = 0;
public:
    constexpr ConstexprVec() = default;
    constexpr ~ConstexprVec() { delete[] data_; }

    constexpr void push_back(const T& val) {
        if (size_ >= cap_) {
            size_t new_cap = cap_ == 0 ? 4 : cap_ * 2;
            T* new_data = new T[new_cap];
            for (size_t i = 0; i < size_; ++i)
                new_data[i] = data_[i];
            delete[] data_;
            data_ = new_data;
            cap_ = new_cap;
        }
        data_[size_++] = val;
    }

    constexpr size_t size() const { return size_; }
    constexpr T operator[](size_t i) const { return data_[i]; }
};

// 编译期使用动态数组！
constexpr int test_constexpr_vec() {
    ConstexprVec<int> v;
    for (int i = 0; i < 10; ++i) v.push_back(i * i);
    return static_cast<int>(v.size()) + v[5]; // 10 + 25 = 35
}

static_assert(test_constexpr_vec() == 35, "constexpr dynamic alloc");
```

### 9.2 编译期链表

```cpp
constexpr int test_constexpr_linked_list() {
    struct Node {
        int value;
        Node* next = nullptr;
    };

    Node* head = nullptr;
    for (int i = 5; i >= 1; --i) {
        Node* n = new Node{i, head};
        head = n;
    }

    // 编译期求和
    int sum = 0;
    Node* curr = head;
    while (curr) { sum += curr->value; curr = curr->next; }

    // 编译期释放（必须！）
    curr = head;
    while (curr) {
        Node* next = curr->next;
        delete curr;
        curr = next;
    }
    return sum;
}

static_assert(test_constexpr_linked_list() == 15, "1+2+3+4+5=15 at compile time");
```

> **重要**：编译期 `new` 的内存必须在同一 `constexpr` 求值过程中 `delete`，否则编译失败。

> **NOTE (补充说明)**:
> - **核心约束**: 这个特性被称为 "Transient `constexpr` allocations"。其核心思想是，`constexpr` 函数内部可以像普通函数一样使用动态内存，但当函数求值结束时，所有分配的内存必须已经释放。任何试图将编译期分配的指针返回到运行时的行为都会导致编译错误。
> - **用途**: 它使得在编译期实现更复杂的数据结构和算法成为可能，例如编译期 `std::vector` 和 `std::string`（C++20已支持）。这对于解析配置文件、生成复杂数据结构等场景非常有用。
> - **性能**: 编译期内存分配和释放的开销在运行时为零，但会增加编译时间。

---

## 第10章：Policy-Based Design (策略模式的编译期实现)

将运行期多态（虚函数）替换为编译期策略选择。**零虚函数调用开销**，编译器可内联所有代码。

### 10.1 日志策略

```cpp
struct ConsoleLogger {
    static void log(std::string_view msg) {
        std::cout << "[CONSOLE] " << msg << "\n";
    }
};

struct SilentLogger {
    static void log(std::string_view /*msg*/) {
        // 什么都不做 —— 编译器会完全优化掉
    }
};

struct PrefixLogger {
    static void log(std::string_view msg) {
        std::cout << "[PREFIX] >>> " << msg << " <<<\n";
    }
};
```

### 10.2 分配策略

```cpp
struct HeapAllocator {
    template <typename T>
    static T* allocate(size_t n) { return new T[n]; }
    template <typename T>
    static void deallocate(T* p) { delete[] p; }
};

struct StackAllocator {
    static constexpr size_t BUFFER_SIZE = 4096;
    inline static thread_local char buffer[BUFFER_SIZE];
    inline static thread_local size_t offset = 0;

    template <typename T>
    static T* allocate(size_t n) {
        size_t bytes = n * sizeof(T);
        if (offset + bytes > BUFFER_SIZE) return nullptr;
        T* ptr = reinterpret_cast<T*>(buffer + offset);
        offset += bytes;
        return ptr;
    }
    template <typename T>
    static void deallocate(T*) { /* 栈分配器不单独释放 */ }
};
```

### 10.3 策略组合：通过模板参数注入

```cpp
template <typename LogPolicy = ConsoleLogger,
          typename AllocPolicy = HeapAllocator>
class DataProcessor {
public:
    void process(std::string_view data) {
        LogPolicy::log("Processing started");
        auto* buffer = AllocPolicy::template allocate<char>(data.size());
        if (buffer) {
            for (size_t i = 0; i < data.size(); ++i)
                buffer[i] = data[i];
            LogPolicy::log("Data copied to buffer");
            AllocPolicy::template deallocate<char>(buffer);
        }
        LogPolicy::log("Processing complete");
    }
};

// 不同配置，编译期确定，零运行时开销：
using VerboseProcessor = DataProcessor<ConsoleLogger, HeapAllocator>;
using SilentProcessor  = DataProcessor<SilentLogger, HeapAllocator>;
using FastProcessor    = DataProcessor<PrefixLogger, StackAllocator>;
```

> **经典案例**：STL 的 `allocator` 就是 Policy-Based Design 的标准应用。

---

## 第11章：Tag Dispatch (标签分派)

利用空的标签类型在编译期选择函数重载。比 `enable_if` 更直观，比 `if constexpr` 更早出现。

### 11.1 迭代器分类标签分派

```cpp
namespace my {

struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

// 不同迭代器类型用不同算法实现 advance
template <typename Iter>
void advance_impl(Iter& it, int n, input_iterator_tag) {
    // 单向迭代器：只能逐步前进 O(n)
    for (int i = 0; i < n; ++i) ++it;
}

template <typename Iter>
void advance_impl(Iter& it, int n, bidirectional_iterator_tag) {
    // 双向迭代器：可前进也可后退 O(n)
    if (n >= 0) { for (int i = 0; i < n; ++i) ++it; }
    else        { for (int i = 0; i > n; --i) --it; }
}

template <typename Iter>
void advance_impl(Iter& it, int n, random_access_iterator_tag) {
    // 随机访问迭代器：O(1) 直接跳转
    it += n;
}

template <typename Iter, typename Tag>
void my_advance(Iter& it, int n, Tag tag) {
    advance_impl(it, n, tag);
}

} // namespace my
```

### 11.2 标签分派用于类型区分

```cpp
struct serialize_as_text {};
struct serialize_as_binary {};

template <typename T>
void serialize(const T& obj, serialize_as_text) {
    std::cout << "Text serialization: " << obj << "\n";
}

template <typename T>
void serialize(const T& obj, serialize_as_binary) {
    std::cout << "Binary serialization of " << sizeof(T) << " bytes\n";
> **NOTE (补充说明)**:
> - **可扩展性**: Tag Dispatch 的一个主要优点是其可扩展性。用户可以为自己的迭代器类型提供特化，并定义其所属的迭代器类别标签，从而无缝地融入到 `my_advance` 这样的算法中，而无需修改算法本身。
> - **与Concepts的结合**: 在现代C++中，Tag Dispatch常常与Concepts结合使用，以提供更清晰的约束和更好的错误信息。

}
```

> **Tag Dispatch vs if constexpr**：Tag Dispatch 基于类型继承层级进行分派，适用于像迭代器分类这样的**层级关系**；`if constexpr` 更适合**平级的条件分支**。

---

## 第12章：编译期依赖注入与工厂

利用模板在编译期注入依赖，实现零开销的依赖反转。

### 12.1 编译期工厂

```cpp
// 产品（通过 CRTP 实现编译期多态）
template <typename Derived>
struct Widget {
    void render() const {
        static_cast<const Derived*>(this)->render_impl();
    }
    std::string name() const {
        return static_cast<const Derived*>(this)->name_impl();
    }
};

struct Button  : Widget<Button>  {
    void render_impl() const { std::cout << "[Button rendered]\n"; }
    std::string name_impl() const { return "Button"; }
};
struct TextBox : Widget<TextBox> {
    void render_impl() const { std::cout << "[TextBox rendered]\n"; }
    std::string name_impl() const { return "TextBox"; }
};
struct Slider  : Widget<Slider>  {
    void render_impl() const { std::cout << "[Slider rendered]\n"; }
    std::string name_impl() const { return "Slider"; }
};

// 编译期工厂：根据枚举选择创建的类型
enum class WidgetType { Button, TextBox, Slider };

template <WidgetType W>
auto create_widget() {
    if constexpr (W == WidgetType::Button)  return Button{};
    else if constexpr (W == WidgetType::TextBox) return TextBox{};
    else if constexpr (W == WidgetType::Slider)  return Slider{};
}
```

### 12.2 编译期依赖注入框架

通过模板参数注入服务实现，消除虚函数开销：

```cpp
struct ConsoleOutput {
    static void write(std::string_view msg) { std::cout << msg; }
};
struct FileOutput {
    static void write(std::string_view msg) { std::cout << "[FILE] " << msg; }
};

// 通过模板注入依赖
template <typename OutputService>
class Application {
public:
    void run() {
        OutputService::write("Application started\n");
        OutputService::write("Doing work...\n");
        OutputService::write("Application finished\n");
    }
};

using ConsoleApp = Application<ConsoleOutput>;
using FileApp    = Application<FileOutput>;
```

---

## 附录：编译期优化最佳实践

| # | 实践 | 说明 |
|---|------|------|
| 1 | **优先 constexpr/consteval 而非 TMP** | 更可读、可调试、编译速度更快 |
| 2 | **优先 Concepts 而非 SFINAE** | 错误信息清晰，代码更声明式 |
| 3 | **用 if constexpr 替代 tag dispatch** | 代码更集中；但 tag dispatch 在层级分派中仍有优势 |
| 4 | **注意编译期计算的权衡** | 过多编译期计算增加编译时间；用 `-ftemplate-depth=N` 调整递归限制 |
| 5 | **用 static_assert 做编译期测试** | 编译通过 = 测试通过，比运行期测试更可靠 |
| 6 | **表达式模板用于性能关键的数学库** | 消除临时对象、融合循环，但增加代码复杂度 |
| 7 | **Policy-Based Design 用于高性能库** | 编译期选择策略、零运行时开销的灵活性（STL allocator 就是此模式） |

---

## 运行输出示例

```
===== C++ 编译期优化进阶教程 演示 =====

[1] Concepts 概念约束:
  safe_add(3, 4) = 7
  smart_abs(-42) = 42
  smart_abs(-2.71) = 2.71
  10 + 20 = 30
  10 * 20 = 200

[2] 编译期类型名:
  type_name<int>()    = int
  type_name<double>() = double

[3] 编译期模式匹配:
  int: 42
  double: 3.14
  string: hello
  other: A

[4] Tuple 编译期操作:
  遍历: 1 2.5 hello 42
  变换 (x2): 2, 4, 6
  反转: 4, 3, 2, 1

[5] 编译期排序:
  原始: 64 34 25 12 22 11 90 1
  冒泡: 1 11 12 22 25 34 64 90
  快排: 1 3 9 10 27 39 43 55 82 99
  binary_search(25) 位于索引 3

[6] SFINAE:
  sfinae_double(21) = 42
  sfinae_double(1.5) = 3
  has_serialize<Serializable> = true
  has_serialize<int> = false

[7] 表达式模板:
  a   = [1, 2, 3, 4]
  b   = [10, 20, 30, 40]
  a+b = [11, 22, 33, 44]
  (a+b)*2 = [22, 44, 66, 88]

[8] 编译期位操作:
  popcount(0b10110101)   = 5
  clz(1)                 = 31
  log2(1024)             = 10
  is_power_of_two(256)   = true
  next_power_of_two(100) = 128

[9] constexpr 动态分配:
  constexpr vec test     = 35 (编译期计算)
  constexpr linked list  = 15 (编译期计算)

[10] 策略模式:
  Verbose: [CONSOLE] Processing started / complete
  Silent:  (无输出)
  Fast:    [PREFIX] >>> Processing started <<<

[11] 标签分派:
  [random_access] jumping 3 positions → *it = 4
  Text serialization: 42
  Binary serialization of 8 bytes

[12] 编译期工厂:
  [Button rendered]
  [TextBox rendered]
  [Slider rendered]
  ConsoleApp / FileApp 输出

===== 所有 static_assert 通过 = 编译期正确性已验证 =====
```

> **核心理念**：所有 `static_assert` 在编译时验证正确性——如果编译通过，即证明所有编译期计算结果正确。运行期代码仅用于输出展示。
