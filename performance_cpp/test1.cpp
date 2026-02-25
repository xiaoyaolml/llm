// =============================================================================
// C++ 编译期优化 详细指导教程与代码示例
// =============================================================================
// 编译期优化的核心思想：将尽可能多的计算从运行期移到编译期完成，
// 从而获得零开销抽象 (Zero-Cost Abstraction)。
//
// 本教程覆盖：
//   1. constexpr 基础与进阶
//   2. 模板元编程 (TMP)
//   3. if constexpr (编译期分支)
//   4. consteval / constinit (C++20)
//   5. 编译期字符串与数组处理
//   6. 变参模板与折叠表达式
//   7. CRTP (奇异递归模板模式)
//   8. 编译期类型萃取 (Type Traits)
//   9. 编译期查找表 (LUT)
//  10. 实战：编译期状态机
// =============================================================================

#include <iostream>
#include <array>
#include <type_traits>
#include <string_view>
#include <tuple>
#include <cstdint>
#include <utility>

// =============================================================================
// 第1章：constexpr 基础与进阶
// =============================================================================
// constexpr 让函数和变量可以在编译期求值。
// C++11 对 constexpr 函数限制严格（只能有一个 return 语句）；
// C++14 放宽了限制，允许循环、局部变量等；
// C++17/20 进一步放宽，支持 try-catch、虚函数等。

// --- 1.1 基础：编译期阶乘 ---
constexpr uint64_t factorial(int n) {
    uint64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// 编译期验证
static_assert(factorial(0) == 1,   "0! should be 1");
static_assert(factorial(5) == 120, "5! should be 120");
static_assert(factorial(10) == 3628800, "10! should be 3628800");

// --- 1.2 constexpr 斐波那契（迭代版，高效） ---
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

static_assert(fibonacci(0) == 0,  "fib(0)");
static_assert(fibonacci(1) == 1,  "fib(1)");
static_assert(fibonacci(10) == 55, "fib(10)");
static_assert(fibonacci(20) == 6765, "fib(20)");

// --- 1.3 constexpr 类 ---
// constexpr 不仅用于函数，也可以用于构造函数和类方法

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

// 编译期向量运算
constexpr Vec3 a(1.0, 2.0, 3.0);
constexpr Vec3 b(4.0, 5.0, 6.0);
constexpr Vec3 c = a + b;
constexpr double dot_ab = a.dot(b);

static_assert(c.x == 5.0 && c.y == 7.0 && c.z == 9.0, "vec add");
static_assert(dot_ab == 32.0, "dot product"); // 1*4 + 2*5 + 3*6 = 32


// =============================================================================
// 第2章：模板元编程 (Template Metaprogramming, TMP)
// =============================================================================
// TMP 利用模板实例化在编译期完成计算。
// 虽然 constexpr 更现代，但 TMP 依然是理解编译期计算的重要基础。

// --- 2.1 经典 TMP 阶乘 ---
template <int N>
struct Factorial {
    static constexpr uint64_t value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr uint64_t value = 1;
};

static_assert(Factorial<5>::value == 120, "TMP Factorial");
static_assert(Factorial<10>::value == 3628800, "TMP Factorial 10");

// --- 2.2 编译期判断素数 ---
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

// --- 2.3 编译期整数序列 (Integer Sequence) ---
// std::integer_sequence 与 std::index_sequence 是TMP的重要工具

template <typename T, T... Ints>
constexpr T compile_time_sum(std::integer_sequence<T, Ints...>) {
    return (Ints + ...); // 折叠表达式 C++17
}

static_assert(compile_time_sum(std::integer_sequence<int, 1, 2, 3, 4, 5>{}) == 15,
              "sum 1..5");


// =============================================================================
// 第3章：if constexpr — 编译期分支
// =============================================================================
// if constexpr (C++17) 让你在编译期选择代码路径。
// 被丢弃的分支不会被实例化，可用于替代 SFINAE 和 enable_if。

// --- 3.1 类型安全的 to_string ---
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

// --- 3.2 编译期选择算法实现 ---
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
            if (i <= j) {
                std::swap(arr[i], arr[j]);
                ++i; --j;
            }
        }
        if (j > 0) sort_impl<S>(arr, j + 1);
        if (i < n) sort_impl<S>(arr + i, n - i);
    }
}


// =============================================================================
// 第4章：consteval 与 constinit (C++20)
// =============================================================================
// consteval: 立即函数，调用必须产生编译期常量，否则编译错误
// constinit: 保证变量在编译期初始化，但变量本身不一定是 const

// --- 4.1 consteval: 强制编译期求值 ---
consteval int square(int n) {
    return n * n;
}

// 以下在编译期完成：
constexpr int sq5 = square(5);  // OK: 编译期
static_assert(sq5 == 25, "5^2 = 25");

// 以下会编译错误（取消注释可验证）：
// int runtime_val = 3;
// int result = square(runtime_val); // ERROR: not a constant expression

// --- 4.2 constinit: 编译期初始化，运行期可修改 ---
// 解决 Static Initialization Order Fiasco (SIOF)

constinit int global_counter = factorial(5); // 编译期初始化为120

// global_counter 可以在运行期修改：
// void increment() { global_counter++; }  // OK


// =============================================================================
// 第5章：编译期字符串与数组处理
// =============================================================================

// --- 5.1 编译期字符串长度 ---
constexpr size_t ct_strlen(const char* s) {
    size_t len = 0;
    while (s[len] != '\0') ++len;
    return len;
}

static_assert(ct_strlen("Hello") == 5, "strlen");
static_assert(ct_strlen("") == 0, "empty string");

// --- 5.2 编译期字符串哈希 (FNV-1a) ---
constexpr uint32_t fnv1a_hash(std::string_view sv) {
    uint32_t hash = 2166136261u;
    for (char c : sv) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

// 可用于编译期字符串比较/switch
constexpr uint32_t hash_hello = fnv1a_hash("hello");
constexpr uint32_t hash_world = fnv1a_hash("world");
static_assert(hash_hello != hash_world, "different hashes");

// --- 5.3 基于哈希的编译期字符串 switch ---
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

// --- 5.4 编译期生成数组 ---
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


// =============================================================================
// 第6章：变参模板与折叠表达式
// =============================================================================
// C++11 引入变参模板，C++17 引入折叠表达式，
// 两者结合可以在编译期处理任意数量的参数。

// --- 6.1 折叠表达式基础 ---
// 四种折叠形式：
// (pack op ...)    → 一元右折叠
// (... op pack)    → 一元左折叠
// (pack op ... op init) → 二元右折叠
// (init op ... op pack) → 二元左折叠

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

// --- 6.2 编译期类型打印（利用折叠表达式） ---
template <typename... Args>
void print_all(const Args&... args) {
    // 折叠表达式 + 逗号运算符: 依次输出每个参数
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

// --- 6.3 编译期参数包大小与类型检查 ---
template <typename... Args>
constexpr bool all_integral() {
    return (std::is_integral_v<Args> && ...);
}

static_assert(all_integral<int, long, short>(), "all integral");
static_assert(!all_integral<int, double, short>(), "not all integral");

// --- 6.4 编译期 max ---
template <typename T>
constexpr T ct_max(T a, T b) {
    return a > b ? a : b;
}

template <typename T, typename... Rest>
constexpr T ct_max(T first, Rest... rest) {
    return ct_max(first, ct_max(rest...));
}

static_assert(ct_max(3, 7, 2, 9, 1) == 9, "max of pack");


// =============================================================================
// 第7章：CRTP — 奇异递归模板模式
// =============================================================================
// CRTP 实现编译期多态，避免虚函数调用的运行时开销。
// 原理：基类以派生类作为模板参数。

// --- 7.1 CRTP 实现编译期多态 ---
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

    // 通用功能：输出信息
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

// 无虚函数表，编译器可内联所有调用
// 与虚函数版本相比，CRTP 零开销


// =============================================================================
// 第8章：编译期类型萃取 (Type Traits)
// =============================================================================
// 类型萃取是对类型进行编译期查询和转换的核心工具。

// --- 8.1 自定义 type trait: 判断是否是容器 ---
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

// 验证
static_assert(is_container_v<std::array<int, 5>>, "array is container");
static_assert(!is_container_v<int>, "int is not container");

// --- 8.2 条件类型选择 ---
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

// --- 8.3 编译期类型列表操作 ---
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
static_assert(std::is_same_v<TypeAt<0, MyTypes>::type, int>, "index 0");
static_assert(std::is_same_v<TypeAt<2, MyTypes>::type, char>, "index 2");
static_assert(MyTypes::size == 4, "size 4");


// =============================================================================
// 第9章：编译期查找表 (Look-Up Table, LUT)
// =============================================================================
// 将运行期查表转为编译期生成，常用于数学函数、CRC校验等场景。

// --- 9.1 编译期 sin 查找表 ---
constexpr double ct_abs(double x) { return x < 0 ? -x : x; }

// 编译期 sin 近似（泰勒级数）
constexpr double ct_sin(double x) {
    // 将 x 规范化到 [-π, π] 区间（简化版）
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

// 256 个采样点的 sin 查找表，完全在编译期生成
constexpr auto sin_lut = generate_sin_table<256>();

// 查表函数：运行期只做数组访问，O(1)
double fast_sin(double angle) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    // 规范化角度
    while (angle < 0) angle += TWO_PI;
    while (angle >= TWO_PI) angle -= TWO_PI;
    // 映射到索引
    size_t index = static_cast<size_t>(angle / TWO_PI * 256) % 256;
    return sin_lut[index];
}

// --- 9.2 编译期 CRC32 查找表 ---
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
// CRC32 值在编译期计算完成


// =============================================================================
// 第10章：实战 — 编译期状态机
// =============================================================================
// 一个完全在编译期定义的有限状态机 (FSM)

// 状态定义
enum class State { Idle, Running, Paused, Stopped };
enum class Event { Start, Pause, Resume, Stop };

// 编译期状态转换表
constexpr State transition(State current, Event event) {
    // 使用 switch 实现状态转换（编译期求值）
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

// 编译期执行一系列事件
template <Event... Events>
constexpr State run_fsm(State initial) {
    State s = initial;
    ((s = transition(s, Events)), ...); // 折叠表达式
    return s;
}

// 编译期验证状态机行为
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

// 状态名称（运行期输出用）
constexpr const char* state_name(State s) {
    switch (s) {
        case State::Idle:    return "Idle";
        case State::Running: return "Running";
        case State::Paused:  return "Paused";
        case State::Stopped: return "Stopped";
    }
    return "Unknown";
}


// =============================================================================
// 附录：编译优化编译器选项速查
// =============================================================================
//
// GCC / Clang:
//   -O0          无优化 (默认)
//   -O1          基本优化
//   -O2          推荐的生产级优化
//   -O3          最大优化 (可能增大二进制)
//   -Os          优化大小
//   -Ofast       -O3 + 违反标准的激进优化
//   -flto        链接时优化 (Link-Time Optimization)
//   -march=native 使用当前CPU架构的全部指令集
//   -DNDEBUG     关闭 assert
//   -fno-exceptions    禁用异常（嵌入式常用）
//   -fno-rtti          禁用 RTTI
//
// MSVC:
//   /O1          最小化大小
//   /O2          最大化速度
//   /Ox          全部优化
//   /GL          全程序优化（类似 LTO）
//   /LTCG        链接时代码生成
//   /arch:AVX2   使用 AVX2 指令集
//
// 建议：
//   开发阶段: -O0 -g (调试)
//   测试阶段: -O2 -g (带调试信息的优化)
//   发布阶段: -O2 -DNDEBUG -flto (-O3 需要性能测试确认)
// =============================================================================


// =============================================================================
// main: 运行演示
// =============================================================================
int main() {
    std::cout << "===== C++ 编译期优化教程 演示 =====\n\n";

    // 1. constexpr 结果直接使用（零运行时开销）
    std::cout << "[1] constexpr 计算:\n";
    std::cout << "  factorial(10)  = " << factorial(10) << "\n";
    std::cout << "  fibonacci(20)  = " << fibonacci(20) << "\n";
    std::cout << "  Vec3 dot(a,b)  = " << dot_ab << "\n\n";

    // 2. TMP
    std::cout << "[2] 模板元编程:\n";
    std::cout << "  Factorial<10>  = " << Factorial<10>::value << "\n";
    std::cout << "  is_prime(997)  = " << std::boolalpha << is_prime(997) << "\n\n";

    // 3. if constexpr
    std::cout << "[3] if constexpr:\n";
    std::cout << "  to_string(42)      = " << to_string_safe(42) << "\n";
    std::cout << "  to_string(3.14)    = " << to_string_safe(3.14) << "\n";
    std::cout << "  to_string(\"hello\") = " << to_string_safe("hello") << "\n\n";

    // 4. consteval
    std::cout << "[4] consteval:\n";
    std::cout << "  square(5) = " << sq5 << " (guaranteed compile-time)\n\n";

    // 5. 编译期字符串处理
    std::cout << "[5] 编译期字符串:\n";
    std::cout << "  hash(\"hello\") = " << hash_hello << "\n";
    process_command("start");
    process_command("status");

    // 6. 折叠表达式
    std::cout << "\n[6] 折叠表达式:\n";
    std::cout << "  sum(1..5)    = " << sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "  product(2,3,4) = " << product(2, 3, 4) << "\n";
    std::cout << "  print_all:   ";
    print_all(42, " hello ", 3.14, " world");
    std::cout << "\n";

    // 7. CRTP
    std::cout << "[7] CRTP 编译期多态:\n";
    Circle circle(5.0);
    Rectangle rect(4.0, 6.0);
    std::cout << "  Circle(5):   ";  circle.print_info();
    std::cout << "  Rect(4x6):   ";  rect.print_info();
    std::cout << "\n";

    // 8. 编译期查找表
    std::cout << "[8] 编译期查找表:\n";
    std::cout << "  sin_lut[0]   = " << sin_lut[0] << " (≈ sin 0)\n";
    std::cout << "  sin_lut[64]  = " << sin_lut[64] << " (≈ sin π/2)\n";
    std::cout << "  fast_sin(1.5708) = " << fast_sin(1.5708) << "\n";
    std::cout << "  CRC32(\"Hello, World!\") = 0x" << std::hex << test_crc << std::dec << "\n\n";

    // 9. 编译期状态机
    std::cout << "[9] 编译期状态机:\n";
    constexpr State final_state =
        run_fsm<Event::Start, Event::Pause, Event::Resume, Event::Stop>(State::Idle);
    std::cout << "  Idle->Start->Pause->Resume->Stop = "
              << state_name(final_state) << "\n\n";

    // 10. 排序演示
    std::cout << "[10] 编译期选择排序策略:\n";
    int arr1[] = {5, 3, 8, 1, 2};
    sort_impl<SortStrategy::Bubble>(arr1, 5);
    std::cout << "  Bubble sort: ";
    for (int x : arr1) std::cout << x << " ";
    std::cout << "\n";

    int arr2[] = {9, 7, 4, 6, 1};
    sort_impl<SortStrategy::Quick>(arr2, 5);
    std::cout << "  Quick sort:  ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "\n===== 所有 static_assert 通过 = 编译期正确性已验证 =====\n";
    return 0;
}
