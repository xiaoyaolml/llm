# C++ 编译期优化详细指导教程

> **对应源码**: `test1.cpp`
>
> 编译期优化的核心思想：将尽可能多的计算从运行期移到编译期完成，从而获得**零开销抽象 (Zero-Cost Abstraction)**。
>
> 本教程覆盖 10 个章节 + 编译器选项附录，所有代码均可编译运行。

---

## 编译方式

```bash
# GCC / Clang (需 C++20)
g++ -std=c++20 -O2 -o test1 test1.cpp && ./test1

# MSVC
cl /std:c++20 /O2 /EHsc test1.cpp
```

---

## 教程目录

| 章节 | 主题 | 关键技术 |
|------|------|----------|
| 1 | **constexpr 基础与进阶** | constexpr 函数、constexpr 类、编译期向量运算 |
| 2 | **模板元编程 (TMP)** | 递归模板、编译期素数判断、整数序列 |
| 3 | **if constexpr — 编译期分支** | 类型安全转换、编译期策略选择 |
| 4 | **consteval / constinit (C++20)** | 强制编译期求值、解决 SIOF 问题 |
| 5 | **编译期字符串与数组处理** | FNV-1a 哈希、字符串 switch、编译期数组生成 |
| 6 | **变参模板与折叠表达式** | 四种折叠形式、编译期 max、类型检查 |
| 7 | **CRTP — 奇异递归模板模式** | 编译期多态、零开销替代虚函数 |
| 8 | **编译期类型萃取 (Type Traits)** | 自定义 trait、条件类型、编译期类型列表 |
| 9 | **编译期查找表 (LUT)** | sin 查找表、CRC32 表（全部编译期生成） |
| 10 | **实战：编译期状态机** | FSM + 折叠表达式 + static_assert 验证 |
| 附录 | **编译器优化选项速查** | GCC/Clang/MSVC 编译优化参数 |

---

## 第1章：constexpr 基础与进阶

`constexpr` 让函数和变量可以在编译期求值。C++11 对 `constexpr` 函数限制严格（只能有一个 return 语句）；C++14 放宽了限制，允许循环、局部变量等；C++17/20 进一步放宽，支持 try-catch、虚函数等。

### 1.1 基础：编译期阶乘

```cpp
constexpr uint64_t factorial(int n) {
    uint64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// 编译期验证 — 结果直接嵌入二进制，零运行时开销
static_assert(factorial(0) == 1,       "0! should be 1");
static_assert(factorial(5) == 120,     "5! should be 120");
static_assert(factorial(10) == 3628800, "10! should be 3628800");
```

### 1.2 constexpr 斐波那契（迭代版，高效）

```cpp
constexpr uint64_t fibonacci(int n) {
    if (n <= 1) return n;
    uint64_t a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        uint64_t tmp = a + b;
        a = b;
        b = tmp;
    }
    return b;
}

static_assert(fibonacci(0) == 0,     "fib(0)");
static_assert(fibonacci(1) == 1,     "fib(1)");
static_assert(fibonacci(10) == 55,   "fib(10)");
static_assert(fibonacci(20) == 6765, "fib(20)");
```

> **注意**：与递归版相比，迭代版 `fibonacci` 不会导致编译期递归深度爆炸。

### 1.3 constexpr 类 — 编译期向量运算

`constexpr` 不仅用于函数，也可以用于构造函数和类方法，实现编译期面向对象：

```cpp
class Vec3 {
public:
    double x, y, z;

    constexpr Vec3(double x = 0, double y = 0, double z = 0)
        : x(x), y(y), z(z) {}

    constexpr Vec3 operator+(const Vec3& rhs) const {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    constexpr Vec3 operator*(double s) const {
        return Vec3(x * s, y * s, z * s);
    }

    constexpr double dot(const Vec3& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    constexpr double length_squared() const {
        return dot(*this);
    }
};

// 编译期向量运算 — 所有计算在编译期完成
constexpr Vec3 a(1.0, 2.0, 3.0);
constexpr Vec3 b(4.0, 5.0, 6.0);
constexpr Vec3 c = a + b;
constexpr double dot_ab = a.dot(b);

static_assert(c.x == 5.0 && c.y == 7.0 && c.z == 9.0, "vec add");
static_assert(dot_ab == 32.0, "dot product"); // 1*4 + 2*5 + 3*6 = 32
```

---

## 第2章：模板元编程 (Template Metaprogramming, TMP)

TMP 利用模板实例化在编译期完成计算。虽然 `constexpr` 更现代，但 TMP 依然是理解编译期计算的重要基础。

### 2.1 经典 TMP 阶乘

```cpp
template <int N>
struct Factorial {
    static constexpr uint64_t value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr uint64_t value = 1;
};

static_assert(Factorial<5>::value == 120,     "TMP Factorial");
static_assert(Factorial<10>::value == 3628800, "TMP Factorial 10");
```

### 2.2 编译期判断素数

```cpp
namespace detail {
    constexpr bool is_prime_helper(int n, int d) {
        if (d * d > n) return true;
        if (n % d == 0) return false;
        return is_prime_helper(n, d + 1);
    }
}

constexpr bool is_prime(int n) {
    if (n < 2) return false;
    return detail::is_prime_helper(n, 2);
}

static_assert(is_prime(2),    "2 is prime");
static_assert(is_prime(17),   "17 is prime");
static_assert(!is_prime(15),  "15 is not prime");
static_assert(is_prime(997),  "997 is prime");
```

### 2.3 编译期整数序列 (Integer Sequence)

`std::integer_sequence` 与 `std::index_sequence` 是 TMP 的重要工具，配合 C++17 折叠表达式实现编译期求和：

```cpp
template <typename T, T... Ints>
constexpr T compile_time_sum(std::integer_sequence<T, Ints...>) {
    return (Ints + ...); // 折叠表达式 C++17
}

static_assert(
    compile_time_sum(std::integer_sequence<int, 1, 2, 3, 4, 5>{}) == 15,
    "sum 1..5"
);
```

---

## 第3章：if constexpr — 编译期分支

`if constexpr` (C++17) 让你在编译期选择代码路径。被丢弃的分支**不会被实例化**，可用于替代 SFINAE 和 `enable_if`。

### 3.1 类型安全的 to_string

```cpp
template <typename T>
std::string to_string_safe(const T& value) {
    if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return value;
    } else if constexpr (std::is_same_v<T, const char*>) {
        return std::string(value);
    } else {
        static_assert(std::is_same_v<T, void>,
                      "Unsupported type for to_string_safe");
        return "";
    }
}
```

> **关键**：`if constexpr` 在编译期求值，被丢弃的分支中的代码即使对当前类型不合法也不会报错。

### 3.2 编译期选择算法实现

通过模板参数在编译期决定使用哪种排序算法，无运行时分支开销：

```cpp
enum class SortStrategy { Bubble, Quick };

template <SortStrategy S>
void sort_impl(int* arr, int n) {
    if constexpr (S == SortStrategy::Bubble) {
        // 编译期确定使用冒泡排序 — 适合小数组
        for (int i = 0; i < n - 1; ++i)
            for (int j = 0; j < n - i - 1; ++j)
                if (arr[j] > arr[j + 1])
                    std::swap(arr[j], arr[j + 1]);
    } else if constexpr (S == SortStrategy::Quick) {
        // 简化的快速排序
        if (n <= 1) return;
        int pivot = arr[n / 2];
        int i = 0, j = n - 1;
        while (i <= j) {
            while (arr[i] < pivot) ++i;
            while (arr[j] > pivot) --j;
            if (i <= j) { std::swap(arr[i], arr[j]); ++i; --j; }
        }
        if (j > 0) sort_impl<S>(arr, j + 1);
        if (i < n) sort_impl<S>(arr + i, n - i);
    }
}
```

---

## 第4章：consteval 与 constinit (C++20)

- **`consteval`** (立即函数)：调用必须产生编译期常量，否则**编译错误**
- **`constinit`**：保证变量在编译期初始化，但变量本身不一定是 `const`

### 4.1 consteval — 强制编译期求值

```cpp
consteval int square(int n) {
    return n * n;
}

constexpr int sq5 = square(5);  // OK: 编译期
static_assert(sq5 == 25, "5^2 = 25");

// 以下会编译错误（取消注释可验证）：
// int runtime_val = 3;
// int result = square(runtime_val); // ERROR: not a constant expression
```

### 4.2 constinit — 编译期初始化，运行期可修改

`constinit` 解决 **Static Initialization Order Fiasco (SIOF)** 问题：

```cpp
constinit int global_counter = factorial(5); // 编译期初始化为120

// global_counter 可以在运行期修改:
// void increment() { global_counter++; }  // OK
```

> **`consteval` vs `constexpr` vs `constinit` 区别**：
> - `constexpr`：可以在编译期或运行期求值
> - `consteval`：**必须**在编译期求值
> - `constinit`：保证编译期**初始化**，但运行期可以修改值

---

## 第5章：编译期字符串与数组处理

### 5.1 编译期字符串长度

```cpp
constexpr size_t ct_strlen(const char* s) {
    size_t len = 0;
    while (s[len] != '\0') ++len;
    return len;
}

static_assert(ct_strlen("Hello") == 5, "strlen");
static_assert(ct_strlen("") == 0,      "empty string");
```

### 5.2 编译期字符串哈希 (FNV-1a)

FNV-1a 是一种简单高效的哈希算法，非常适合编译期计算：

```cpp
constexpr uint32_t fnv1a_hash(std::string_view sv) {
    uint32_t hash = 2166136261u;
    for (char c : sv) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

constexpr uint32_t hash_hello = fnv1a_hash("hello");
constexpr uint32_t hash_world = fnv1a_hash("world");
static_assert(hash_hello != hash_world, "different hashes");
```

### 5.3 基于哈希的编译期字符串 switch

C++ 的 `switch` 不支持字符串，但通过编译期哈希可以实现类似功能：

```cpp
void process_command(std::string_view cmd) {
    switch (fnv1a_hash(cmd)) {
        case fnv1a_hash("start"):
            std::cout << "Starting...\n"; break;
        case fnv1a_hash("stop"):
            std::cout << "Stopping...\n"; break;
        case fnv1a_hash("status"):
            std::cout << "Status OK\n"; break;
        default:
            std::cout << "Unknown command: " << cmd << "\n"; break;
    }
}
```

> **原理**：`case` 标签中的 `fnv1a_hash("start")` 是编译期常量，运行期只做一次哈希计算 + 跳转表查找。

### 5.4 编译期生成数组

```cpp
template <size_t N>
constexpr std::array<int, N> generate_squares() {
    std::array<int, N> arr{};
    for (size_t i = 0; i < N; ++i) {
        arr[i] = static_cast<int>(i * i);
    }
    return arr;
}

constexpr auto squares_10 = generate_squares<10>();
static_assert(squares_10[0] == 0,  "0^2");
static_assert(squares_10[3] == 9,  "3^2");
static_assert(squares_10[9] == 81, "9^2");
```

---

## 第6章：变参模板与折叠表达式

C++11 引入变参模板，C++17 引入折叠表达式，两者结合可以在编译期处理任意数量的参数。

### 6.1 折叠表达式基础

四种折叠形式：

| 形式 | 语法 | 展开方式 |
|------|------|----------|
| 一元右折叠 | `(pack op ...)` | `a₁ op (a₂ op (a₃ op a₄))` |
| 一元左折叠 | `(... op pack)` | `((a₁ op a₂) op a₃) op a₄` |
| 二元右折叠 | `(pack op ... op init)` | `a₁ op (a₂ op (a₃ op init))` |
| 二元左折叠 | `(init op ... op pack)` | `((init op a₁) op a₂) op a₃` |

```cpp
template <typename... Args>
constexpr auto sum(Args... args) {
    return (args + ...); // 一元右折叠
}

template <typename... Args>
constexpr bool all_true(Args... args) {
    return (args && ...); // 一元右折叠
}

template <typename... Args>
constexpr auto product(Args... args) {
    return (args * ... * 1); // 二元右折叠，初始值1处理空参数包
}

static_assert(sum(1, 2, 3, 4, 5) == 15, "sum fold");
static_assert(all_true(true, true, true), "all true");
static_assert(!all_true(true, false, true), "not all true");
static_assert(product(2, 3, 4) == 24, "product fold");
```

### 6.2 编译期类型打印（利用折叠表达式）

```cpp
template <typename... Args>
void print_all(const Args&... args) {
    // 折叠表达式 + 逗号运算符: 依次输出每个参数
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}
```

### 6.3 编译期参数包类型检查

```cpp
template <typename... Args>
constexpr bool all_integral() {
    return (std::is_integral_v<Args> && ...);
}

static_assert(all_integral<int, long, short>(),    "all integral");
static_assert(!all_integral<int, double, short>(), "not all integral");
```

### 6.4 编译期 max

```cpp
template <typename T>
constexpr T ct_max(T a, T b) {
    return a > b ? a : b;
}

template <typename T, typename... Rest>
constexpr T ct_max(T first, Rest... rest) {
    return ct_max(first, ct_max(rest...));
}

static_assert(ct_max(3, 7, 2, 9, 1) == 9, "max of pack");
```

---

## 第7章：CRTP — 奇异递归模板模式

CRTP 实现**编译期多态**，避免虚函数调用的运行时开销。原理：基类以派生类作为模板参数。

### 7.1 CRTP 实现编译期多态

```cpp
template <typename Derived>
class Shape {
public:
    // 编译期分派到派生类的实现
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
    double perimeter() const {
        return static_cast<const Derived*>(this)->perimeter_impl();
    }
    void print_info() const {
        std::cout << "Area: " << area()
                  << ", Perimeter: " << perimeter() << "\n";
    }
};

class Circle : public Shape<Circle> {
    double radius_;
public:
    constexpr Circle(double r) : radius_(r) {}
    constexpr double area_impl() const {
        return 3.14159265358979 * radius_ * radius_;
    }
    constexpr double perimeter_impl() const {
        return 2.0 * 3.14159265358979 * radius_;
    }
};

class Rectangle : public Shape<Rectangle> {
    double w_, h_;
public:
    constexpr Rectangle(double w, double h) : w_(w), h_(h) {}
    constexpr double area_impl() const { return w_ * h_; }
    constexpr double perimeter_impl() const { return 2.0 * (w_ + h_); }
};
```

> **CRTP vs 虚函数**：CRTP 无虚函数表，编译器可内联所有调用，实现**零开销多态**。

> **NOTE (补充说明)**:
> - **性能权衡**: 虽然CRTP避免了虚函数调用的间接跳转开销，但其优势在频繁、小函数的调用中最为显著。对于复杂的、耗时较长的函数，虚函数调用的开销占比可能微不足道。
> - **代码大小**: CRTP会为每个派生类生成独立的基类实例，可能导致代码膨胀（Code Bloat），而虚函数实现则共享基类代码。
> - **灵活性**: 虚函数支持在运行期处理任意派生类对象的集合（例如 `std::vector<Shape*>`），而CRTP的类型在编译期固定，无法将不同派生类的对象放入同一个同质容器中。
> - **总结**: CRTP是实现静态多态的强大工具，适用于性能极其敏感且类型在编译期已知的场景。虚函数则提供了更高的运行期灵活性。

---

## 第8章：编译期类型萃取 (Type Traits)

类型萃取是对类型进行编译期查询和转换的核心工具。

### 8.1 自定义 type trait — 判断是否是容器

利用 `std::void_t` (C++17) 进行 SFINAE 检测：

```cpp
template <typename T, typename = void>
struct is_container : std::false_type {};

template <typename T>
struct is_container<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end()),
    decltype(std::declval<T>().size())
>> : std::true_type {};

template <typename T>
constexpr bool is_container_v = is_container<T>::value;

static_assert(is_container_v<std::array<int, 5>>, "array is container");
static_assert(!is_container_v<int>,                "int is not container");
```

### 8.2 条件类型选择

```cpp
template <bool Cond, typename TrueType, typename FalseType>
struct my_conditional {
    using type = TrueType;
};

template <typename TrueType, typename FalseType>
struct my_conditional<false, TrueType, FalseType> {
    using type = FalseType;
};

// 根据平台选择不同的整数类型
using fast_int = my_conditional<(sizeof(void*) == 8), int64_t, int32_t>::type;
static_assert(sizeof(fast_int) == 8, "64-bit platform uses int64_t");
```

### 8.3 编译期类型列表操作

```cpp
template <typename... Ts>
struct TypeList {
    static constexpr size_t size = sizeof...(Ts);
};

// 获取第 N 个类型
template <size_t N, typename List>
struct TypeAt;

template <size_t N, typename Head, typename... Tail>
struct TypeAt<N, TypeList<Head, Tail...>> {
    using type = typename TypeAt<N - 1, TypeList<Tail...>>::type;
};

template <typename Head, typename... Tail>
struct TypeAt<0, TypeList<Head, Tail...>> {
    using type = Head;
};

using MyTypes = TypeList<int, double, char, float>;
static_assert(std::is_same_v<TypeAt<0, MyTypes>::type, int>,  "index 0");
static_assert(std::is_same_v<TypeAt<2, MyTypes>::type, char>, "index 2");
static_assert(MyTypes::size == 4, "size 4");
```

---

## 第9章：编译期查找表 (Look-Up Table, LUT)

将运行期查表转为编译期生成，常用于数学函数、CRC 校验等场景。

### 9.1 编译期 sin 查找表

利用泰勒级数在编译期计算 sin 值，生成 256 个采样点的查找表：

```cpp
constexpr double ct_sin(double x) {
    double result = 0.0;
    double term = x;
    for (int i = 1; i <= 15; ++i) {
        result += term;
        term *= -x * x / ((2.0 * i) * (2.0 * i + 1.0));
    }
    return result;
}

template <size_t N>
constexpr std::array<double, N> generate_sin_table() {
    std::array<double, N> table{};
    constexpr double PI = 3.14159265358979323846;
    for (size_t i = 0; i < N; ++i) {
        double angle = 2.0 * PI * static_cast<double>(i) / static_cast<double>(N);
        table[i] = ct_sin(angle);
    }
    return table;
}

// 256 个采样点，完全在编译期生成
constexpr auto sin_lut = generate_sin_table<256>();

// 查表函数：运行期只做数组访问，O(1)
double fast_sin(double angle) {
    constexpr double TWO_PI = 2.0 * 3.14159265358979323846;
    while (angle < 0) angle += TWO_PI;
    while (angle >= TWO_PI) angle -= TWO_PI;
    size_t index = static_cast<size_t>(angle / TWO_PI * 256) % 256;
    return sin_lut[index];
}
```

> **NOTE (补充说明)**:
> 当前的 `fast_sin` 实现是“最近邻”采样，精度较低。在性能要求允许的情况下，可以通过**线性插值**来大幅提高精度：
> ```cpp
> double fast_sin_interpolated(double angle) {
>     // ... 角度规范化
>     double pos = angle / TWO_PI * 256;
>     size_t index = static_cast<size_t>(pos);
>     double frac = pos - index;
>     // 在两个相邻的LUT表项之间进行线性插值
>     double y1 = sin_lut[index];
>     double y2 = sin_lut[(index + 1) % 256]; // 环形处理
>     return y1 + frac * (y2 - y1);
> }
> ```
> 这种方法在增加少量计算（一次乘法和一次加法）的代价下，可以获得远超直接查表的精度。

### 9.2 编译期 CRC32 查找表

```cpp
constexpr uint32_t crc32_table_entry(uint32_t index) {
    uint32_t crc = index;
    for (int j = 0; j < 8; ++j) {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xEDB88320;
        else
            crc >>= 1;
    }
    return crc;
}

template <size_t... I>
constexpr auto make_crc32_table(std::index_sequence<I...>) {
    return std::array<uint32_t, sizeof...(I)>{ crc32_table_entry(I)... };
}

constexpr auto crc32_table = make_crc32_table(std::make_index_sequence<256>{});

// 编译期 CRC32 计算
constexpr uint32_t crc32(std::string_view data) {
    uint32_t crc = 0xFFFFFFFF;
    for (char c : data) {
        uint8_t index = (crc ^ static_cast<uint8_t>(c)) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    return crc ^ 0xFFFFFFFF;
}

constexpr uint32_t test_crc = crc32("Hello, World!");
// CRC32 值在编译期计算完成，运行期直接使用常量
```

---

## 第10章：实战 — 编译期状态机

一个完全在编译期定义和验证的**有限状态机 (FSM)**。

### 10.1 状态与事件定义

```cpp
enum class State { Idle, Running, Paused, Stopped };
enum class Event { Start, Pause, Resume, Stop };
```

### 10.2 编译期状态转换表

```cpp
constexpr State transition(State current, Event event) {
    switch (current) {
        case State::Idle:
            switch (event) {
                case Event::Start: return State::Running;
                default: return current;
            }
        case State::Running:
            switch (event) {
                case Event::Pause: return State::Paused;
                case Event::Stop:  return State::Stopped;
                default: return current;
            }
        case State::Paused:
            switch (event) {
                case Event::Resume: return State::Running;
                case Event::Stop:   return State::Stopped;
                default: return current;
            }
        case State::Stopped:
            return State::Stopped; // 终态
    }
    return current;
}
```

### 10.3 编译期执行事件序列

利用折叠表达式驱动状态机：

```cpp
template <Event... Events>
constexpr State run_fsm(State initial) {
    State s = initial;
    ((s = transition(s, Events)), ...); // 折叠表达式
    return s;
}
```

### 10.4 编译期验证状态机行为

所有验证在编译期完成 — 如果编译通过，即证明状态机转换逻辑正确：

```cpp
static_assert(
    run_fsm<Event::Start, Event::Pause, Event::Resume, Event::Stop>(State::Idle)
        == State::Stopped,
    "FSM: Idle -> Start -> Pause -> Resume -> Stop = Stopped"
);

static_assert(
    run_fsm<Event::Start, Event::Pause>(State::Idle)
        == State::Paused,
    "FSM: Idle -> Start -> Pause = Paused"
);

static_assert(
    run_fsm<Event::Stop>(State::Idle)
        == State::Idle,
    "FSM: Idle -> Stop = Idle (invalid transition)"
);
```

状态机 FSM 转换图：

```
  Idle ──Start──▶ Running ──Pause──▶ Paused
                    │  ▲              │  │
                    │  └──Resume──────┘  │
                    │                    │
                    └──Stop──▶ Stopped ◀─┘
```

---

## 附录：编译器优化选项速查
| `-fPIC` / `-fPIE` | 生成位置无关代码，用于动态库和地址空间布局随机化（ASLR）的程序 |

### MSVC

| 选项 | 说明 |
|------|------|
| `/O1` | 最小化大小 |
| `/O2` | 最大化速度 |
| `/Ox` | 全部优化 |
| `/GL` | 全程序优化（类似 LTO） |
| `/LTCG` | 链接时代码生成 |
| `/arch:AVX2` | 使用 AVX2 指令集 |

### 建议

```
开发阶段:  -O0 -g            (调试)
测试阶段:  -O2 -g            (带调试信息的优化)
发布阶段:  -O2 -DNDEBUG -flto (O3 需要性能测试确认)
```

> **NOTE (补充说明)**:
> - **`-march=native`**: 这个选项非常强大，但编译出的二进制文件可能无法在其他（较旧的）CPU上运行。如果需要分发二进制程序，应选择一个兼容性更广的架构（如 `x86-64-v2`）。
> - **`-fPIC`**: 在编译动态库（`.so` 文件）时，必须使用此选项。它会产生位置无关代码，虽然相比静态代码会引入微小的性能开销（额外的寄存器寻址），但这是动态链接所必需的。
| 选项 | 说明 |
|------|------|
| `/O1` | 最小化大小 |
| `/O2` | 最大化速度 |
| `/Ox` | 全部优化 |
| `/GL` | 全程序优化（类似 LTO） |
| `/LTCG` | 链接时代码生成 |
| `/arch:AVX2` | 使用 AVX2 指令集 |

### 建议

```
开发阶段:  -O0 -g            (调试)
测试阶段:  -O2 -g            (带调试信息的优化)
发布阶段:  -O2 -DNDEBUG -flto (O3 需要性能测试确认)
```

---

## 运行输出示例

```
===== C++ 编译期优化教程 演示 =====

[1] constexpr 计算:
  factorial(10)  = 3628800
  fibonacci(20)  = 6765
  Vec3 dot(a,b)  = 32

[2] 模板元编程:
  Factorial<10>  = 3628800
  is_prime(997)  = true

[3] if constexpr:
  to_string(42)      = 42
  to_string(3.14)    = 3.140000
  to_string("hello") = hello

[4] consteval:
  square(5) = 25 (guaranteed compile-time)

[5] 编译期字符串:
  hash("hello") = 1335831723
  Starting...
  Status OK

[6] 折叠表达式:
  sum(1..5)      = 15
  product(2,3,4) = 24

[7] CRTP 编译期多态:
  Circle(5):   Area: 78.5398, Perimeter: 31.4159
  Rect(4x6):   Area: 24, Perimeter: 20

[8] 编译期查找表:
  sin_lut[0]   = 0 (≈ sin 0)
  sin_lut[64]  = 1 (≈ sin π/2)
  CRC32("Hello, World!") = 0xec4ac3d0

[9] 编译期状态机:
  Idle->Start->Pause->Resume->Stop = Stopped

[10] 编译期选择排序策略:
  Bubble sort: 1 2 3 5 8
  Quick sort:  1 4 6 7 9

===== 所有 static_assert 通过 = 编译期正确性已验证 =====
```

> **核心理念**：所有 `static_assert` 在编译时验证正确性——如果编译通过，即证明所有编译期计算结果正确。运行期代码仅用于输出展示。
