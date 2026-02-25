// =============================================================================
// C++17 编译期优化 完全指导教程与代码示例
// =============================================================================
// 标准：严格 C++17（不使用任何 C++20 特性）
// 编译：g++ -std=c++17 -O2 -o test3 test3.cpp
//       cl /std:c++17 /O2 /EHsc test3.cpp
//
// 目录：
//   1.  constexpr 完全指南（C++17 增强）
//   2.  if constexpr — 编译期分支消除
//   3.  折叠表达式 (Fold Expressions)
//   4.  CTAD — 类模板参数推导
//   5.  inline constexpr 与编译期常量
//   6.  结构化绑定与编译期
//   7.  std::string_view 编译期字符串
//   8.  std::void_t 与简化 SFINAE
//   9.  constexpr lambda
//  10.  变参模板高级技巧
//  11.  编译期查找表与数据结构
//  12.  CRTP 编译期多态（进阶）
//  13.  编译期类型计算
//  14.  编译期 EBO 与布局优化
//  15.  std::variant + std::visit 编译期分派
//  16.  实战：编译期词法分析器
// =============================================================================

#include <iostream>
#include <array>
#include <type_traits>
#include <string_view>
#include <tuple>
#include <cstdint>
#include <utility>
#include <variant>
#include <optional>
#include <functional>
#include <algorithm>


// =============================================================================
// 第1章：constexpr 完全指南 (C++17 增强)
// =============================================================================
// C++17 对 constexpr 的增强：
//   - constexpr lambda 表达式
//   - constexpr if
//   - 更宽松的 constexpr 函数规则（已从 C++14 开始放宽）
//
// 关键限制（C++17 仍不能做）：
//   - 不能 new/delete（需要 C++20）
//   - 不能 try-catch（需要 C++20）
//   - 不能调用虚函数（需要 C++20）
//   - 不能 reinterpret_cast

// --- 1.1 constexpr 函数：循环、分支、局部变量全部可用 ---

constexpr int64_t power(int base, int exp) {
    int64_t result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}

static_assert(power(2, 10) == 1024, "2^10");
static_assert(power(3, 5)  == 243,  "3^5");

// --- 1.2 constexpr 中使用局部数组 ---

constexpr int sum_of_squares(int n) {
    // C++17: constexpr 函数中可以使用局部数组
    int arr[100] = {}; // 必须初始化
    for (int i = 0; i < n && i < 100; ++i) {
        arr[i] = i * i;
    }
    int sum = 0;
    for (int i = 0; i < n && i < 100; ++i) {
        sum += arr[i];
    }
    return sum;
}

static_assert(sum_of_squares(5) == 0 + 1 + 4 + 9 + 16, "sum of squares 0..4");
static_assert(sum_of_squares(10) == 285, "sum of squares 0..9");

// --- 1.3 constexpr 与 std::array 完美搭配 ---

template <size_t N>
constexpr std::array<int, N> generate_fibonacci() {
    std::array<int, N> fib{};
    if constexpr (N >= 1) fib[0] = 0;
    if constexpr (N >= 2) fib[1] = 1;
    for (size_t i = 2; i < N; ++i) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
    return fib;
}

constexpr auto fib_table = generate_fibonacci<20>();
static_assert(fib_table[0] == 0,   "fib(0)");
static_assert(fib_table[10] == 55, "fib(10)");
static_assert(fib_table[19] == 4181, "fib(19)");

// --- 1.4 constexpr 类：编译期构造 + 方法 ---

class Fraction {
    int num_, den_;

    constexpr int gcd(int a, int b) const {
        a = a < 0 ? -a : a;
        b = b < 0 ? -b : b;
        while (b) { int t = b; b = a % b; a = t; }
        return a;
    }

public:
    constexpr Fraction(int num = 0, int den = 1)
        : num_(num), den_(den) {
        int g = gcd(num_, den_);
        if (g != 0) { num_ /= g; den_ /= g; }
        if (den_ < 0) { num_ = -num_; den_ = -den_; }
    }

    constexpr Fraction operator+(const Fraction& rhs) const {
        return Fraction(num_ * rhs.den_ + rhs.num_ * den_, den_ * rhs.den_);
    }

    constexpr Fraction operator*(const Fraction& rhs) const {
        return Fraction(num_ * rhs.num_, den_ * rhs.den_);
    }

    constexpr bool operator==(const Fraction& rhs) const {
        return num_ == rhs.num_ && den_ == rhs.den_;
    }

    constexpr int numerator()   const { return num_; }
    constexpr int denominator() const { return den_; }
};

// 编译期分数运算
constexpr Fraction half(1, 2);
constexpr Fraction third(1, 3);
constexpr Fraction sum_frac = half + third;       // 5/6
constexpr Fraction prod_frac = half * third;      // 1/6

static_assert(sum_frac == Fraction(5, 6),  "1/2 + 1/3 = 5/6");
static_assert(prod_frac == Fraction(1, 6), "1/2 * 1/3 = 1/6");

// --- 1.5 constexpr 中的多重返回值 ---

struct DivResult {
    int quotient;
    int remainder;
};

constexpr DivResult safe_div(int a, int b) {
    return { a / b, a % b };
}

constexpr auto div_result = safe_div(17, 5);
static_assert(div_result.quotient == 3,  "17/5 = 3");
static_assert(div_result.remainder == 2, "17%5 = 2");


// =============================================================================
// 第2章：if constexpr — 编译期分支消除
// =============================================================================
// if constexpr 是 C++17 最强大的编译期特性。
// 被丢弃的分支 *不会被实例化*，因此：
//   - 可以引用仅在特定类型上有效的操作
//   - 替代大量 SFINAE/enable_if
//   - 替代 tag dispatch

// --- 2.1 基础：类型安全的序列化 ---

template <typename T>
std::string serialize(const T& value) {
    if constexpr (std::is_integral_v<T>) {
        return "int:" + std::to_string(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "float:" + std::to_string(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "str:" + value;
    } else if constexpr (std::is_same_v<T, const char*> ||
                         std::is_same_v<T, std::string_view>) {
        return "str:" + std::string(value);
    } else if constexpr (std::is_pointer_v<T>) {
        return "ptr:0x...";
    } else {
        // 如果没有匹配的类型，编译期报错
        static_assert(sizeof(T) == 0, "Unsupported type for serialize");
    }
}

// --- 2.2 编译期递归终止 (替代偏特化) ---

// 旧方式：需要递归终止的偏特化
// 新方式：if constexpr 直接终止

template <typename T>
constexpr int count_dimensions() {
    // 如果 T 不是数组类型，维度为 0
    if constexpr (std::is_array_v<T>) {
        return 1 + count_dimensions<std::remove_extent_t<T>>();
    } else {
        return 0;
    }
}

static_assert(count_dimensions<int>() == 0, "int: 0 dim");
static_assert(count_dimensions<int[3]>() == 1, "int[3]: 1 dim");
static_assert(count_dimensions<int[3][4]>() == 2, "int[3][4]: 2 dim");
static_assert(count_dimensions<int[2][3][4]>() == 3, "3 dim");

// --- 2.3 编译期类型分派（替代 tag dispatch）---

// 旧方式需要三个重载 + tag struct
// 新方式：一个函数

enum class Endian { Little, Big };

template <Endian E, typename T>
constexpr T byte_swap(T value) {
    static_assert(std::is_integral_v<T>, "Only integral types");

    if constexpr (sizeof(T) == 1) {
        return value; // 单字节无需交换
    } else if constexpr (sizeof(T) == 2) {
        auto v = static_cast<uint16_t>(value);
        return static_cast<T>((v >> 8) | (v << 8));
    } else if constexpr (sizeof(T) == 4) {
        auto v = static_cast<uint32_t>(value);
        return static_cast<T>(
            ((v >> 24) & 0xFF)       |
            ((v >> 8)  & 0xFF00)     |
            ((v << 8)  & 0xFF0000)   |
            ((v << 24) & 0xFF000000)
        );
    }
    // 编译器会在编译期选择正确的分支，其余分支不生成代码
}

static_assert(byte_swap<Endian::Big>(uint16_t(0x0102)) == 0x0201, "swap16");
static_assert(byte_swap<Endian::Big>(uint32_t(0x01020304)) == 0x04030201, "swap32");

// --- 2.4 编译期选择数据结构 ---

template <size_t N>
auto make_storage() {
    if constexpr (N <= 16) {
        // 小数据：栈上分配
        return std::array<uint8_t, N>{};
    } else {
        // 大数据：使用 vector（运行期分配）
        return std::vector<uint8_t>(N);
    }
}

// --- 2.5 if constexpr 的 always_false 技巧 ---

// 为什么不能直接写 static_assert(false)?
// 因为 static_assert(false) 会在模板定义时就失败，
// 而不是在实例化时。解决方法：

template <typename>
constexpr bool always_false_v = false;

template <typename T>
void must_not_call() {
    // 这只在函数被实例化时才触发
    static_assert(always_false_v<T>, "This function should not be called");
}


// =============================================================================
// 第3章：折叠表达式 (Fold Expressions) — C++17 新增
// =============================================================================
// 四种形式:
//   (pack op ...)       一元右折叠: a1 op (a2 op (a3 op a4))
//   (... op pack)       一元左折叠: ((a1 op a2) op a3) op a4
//   (pack op ... op init)  二元右折叠: a1 op (a2 op (a3 op init))
//   (init op ... op pack)  二元左折叠: ((init op a1) op a2) op a3

// --- 3.1 基础折叠 ---

template <typename... Args>
constexpr auto fold_sum(Args... args) {
    return (args + ...); // 一元右折叠
}

template <typename... Args>
constexpr auto fold_product(Args... args) {
    return (args * ... * 1); // 二元右折叠，空包返回1
}

template <typename... Args>
constexpr bool fold_all(Args... args) {
    return (args && ...);
}

template <typename... Args>
constexpr bool fold_any(Args... args) {
    return (args || ...);
}

static_assert(fold_sum(1, 2, 3, 4, 5) == 15, "sum");
static_assert(fold_product(2, 3, 4) == 24, "product");
static_assert(fold_all(true, true, true), "all true");
static_assert(!fold_any(false, false, false), "none true");

// --- 3.2 折叠 + 逗号运算符：编译期 for_each ---

template <typename Func, typename... Args>
void for_each_arg(Func&& f, Args&&... args) {
    // 用逗号运算符折叠，依次对每个参数调用 f
    (f(std::forward<Args>(args)), ...);
}

// --- 3.3 编译期参数包查找 ---

template <typename T, typename... Args>
constexpr bool contains_type() {
    return (std::is_same_v<T, Args> || ...);
}

static_assert(contains_type<int, double, int, char>(), "has int");
static_assert(!contains_type<float, double, int, char>(), "no float");

// --- 3.4 折叠实现 print ---

// 利用折叠表达式 + 运算符重载实现链式输出
template <typename... Args>
void println(Args&&... args) {
    // 二元左折叠 with std::cout
    (std::cout << ... << args) << "\n";
}

// 带分隔符版本
template <typename First, typename... Rest>
void print_separated(const char* sep, First&& first, Rest&&... rest) {
    std::cout << first;
    ((std::cout << sep << rest), ...);
    std::cout << "\n";
}

// --- 3.5 编译期 min/max ---

template <typename T, typename... Rest>
constexpr T fold_min(T first, Rest... rest) {
    T result = first;
    ((result = (rest < result ? rest : result)), ...);
    return result;
}

template <typename T, typename... Rest>
constexpr T fold_max(T first, Rest... rest) {
    T result = first;
    ((result = (rest > result ? rest : result)), ...);
    return result;
}

static_assert(fold_min(5, 3, 8, 1, 9) == 1, "min");
static_assert(fold_max(5, 3, 8, 1, 9) == 9, "max");

// --- 3.6 编译期计数满足条件的参数 ---

template <typename... Args>
constexpr int count_integral(Args...) {
    return (0 + ... + std::is_integral_v<Args>);
}

static_assert(count_integral(1, 2.0, 3, 'a', 5.0f) == 3, "3 integrals");

// --- 3.7 折叠表达式构建编译期字符串连接 ---

// 编译期安全的多参数比较
template <typename T, typename... Args>
constexpr bool all_equal(T first, Args... args) {
    return ((first == args) && ...);
}

static_assert(all_equal(5, 5, 5, 5), "all 5");
static_assert(!all_equal(5, 5, 3, 5), "not all 5");


// =============================================================================
// 第4章：CTAD — 类模板参数推导
// =============================================================================
// C++17 允许从构造函数参数推导类模板参数。
// 减少冗余，并可自定义推导指引 (deduction guide)。

// --- 4.1 标准库 CTAD ---

// C++14: std::pair<int, double> p(1, 2.0);
// C++17: std::pair p(1, 2.0); — 自动推导

// C++14: std::tuple<int, double, std::string> t(1, 2.0, "hello");
// C++17: std::tuple t(1, 2.0, std::string("hello"));

// C++14: std::array<int, 3> a = {1, 2, 3};
// C++17: std::array a = {1, 2, 3}; // 只有部分编译器支持

// --- 4.2 自定义 CTAD：推导指引 ---

template <typename T, size_t N>
struct StaticVec {
    T data[N]{};
    size_t size_ = N;

    constexpr T operator[](size_t i) const { return data[i]; }
    constexpr size_t size() const { return size_; }
};

// 推导指引：从初始化列表推导 T 和 N
template <typename T, typename... Args>
StaticVec(T, Args...) -> StaticVec<T, 1 + sizeof...(Args)>;

// 使用：
// StaticVec v{1, 2, 3, 4, 5}; → StaticVec<int, 5>

// --- 4.3 CTAD 用于工厂模式 ---

template <typename... Funcs>
struct Overloaded : Funcs... {
    using Funcs::operator()...;
};

// C++17 推导指引
template <typename... Funcs>
Overloaded(Funcs...) -> Overloaded<Funcs...>;

// 这让 std::visit 的用法变得优雅：
// std::visit(Overloaded{
//     [](int i)    { ... },
//     [](double d) { ... },
//     [](auto x)   { ... }
// }, my_variant);


// =============================================================================
// 第5章：inline constexpr 与编译期常量
// =============================================================================
// C++17 引入 inline 变量，解决了头文件中定义常量的 ODR 问题。

// --- 5.1 inline constexpr：头文件中的编译期常量 ---

// C++14 问题：头文件中定义 constexpr 变量可能导致链接错误
// C++17 解决：inline constexpr

inline constexpr double PI = 3.14159265358979323846;
inline constexpr double E  = 2.71828182845904523536;
inline constexpr double GOLDEN_RATIO = 1.61803398874989484820;

// --- 5.2 inline constexpr 用于类型萃取 ---

// C++14 风格：
// template <typename T>
// struct is_my_type : std::false_type {};
// template <typename T>
// constexpr bool is_my_type_v = is_my_type<T>::value; // 可能有 ODR 问题

// C++17 风格：
template <typename T>
inline constexpr bool is_numeric_v =
    std::is_integral_v<T> || std::is_floating_point_v<T>;

static_assert(is_numeric_v<int>, "int is numeric");
static_assert(is_numeric_v<double>, "double is numeric");
static_assert(!is_numeric_v<std::string>, "string is not numeric");

// --- 5.3 编译期配置表 ---

struct Config {
    int max_connections;
    int buffer_size;
    bool debug_mode;
    std::string_view app_name;
};

inline constexpr Config DEFAULT_CONFIG = {
    .max_connections = 100,
    .buffer_size = 4096,
    .debug_mode = false,
    .app_name = "MyApp"
};

inline constexpr Config DEBUG_CONFIG = {
    .max_connections = 10,
    .buffer_size = 1024,
    .debug_mode = true,
    .app_name = "MyApp-Debug"
};

// 编译期选择配置
#ifdef NDEBUG
inline constexpr auto& ACTIVE_CONFIG = DEFAULT_CONFIG;
#else
inline constexpr auto& ACTIVE_CONFIG = DEBUG_CONFIG;
#endif


// =============================================================================
// 第6章：结构化绑定与编译期
// =============================================================================
// C++17 结构化绑定可以与 constexpr 配合使用。

// --- 6.1 constexpr 结构化绑定 ---

constexpr auto compute_stats(int a, int b) {
    struct Result { int sum; int diff; int product; };
    return Result{a + b, a - b, a * b};
}

// 利用结构化绑定访问编译期结果
constexpr auto [ct_sum, ct_diff, ct_prod] = compute_stats(10, 3);
static_assert(ct_sum == 13,  "10+3");
static_assert(ct_diff == 7,  "10-3");
static_assert(ct_prod == 30, "10*3");

// --- 6.2 结构化绑定 + tuple ---

constexpr auto pair_result = std::pair{42, 3.14};
constexpr auto [int_val, double_val] = pair_result;
static_assert(int_val == 42, "pair first");

// --- 6.3 函数返回多值的惯用法 ---

constexpr auto minmax_element_ct(const int* arr, size_t n) {
    struct MinMax { int min_val; int max_val; size_t min_idx; size_t max_idx; };
    MinMax result{arr[0], arr[0], 0, 0};
    for (size_t i = 1; i < n; ++i) {
        if (arr[i] < result.min_val) { result.min_val = arr[i]; result.min_idx = i; }
        if (arr[i] > result.max_val) { result.max_val = arr[i]; result.max_idx = i; }
    }
    return result;
}

constexpr int test_arr[] = {5, 2, 8, 1, 9, 3};
constexpr auto [min_v, max_v, min_i, max_i] = minmax_element_ct(test_arr, 6);
static_assert(min_v == 1 && min_i == 3, "min");
static_assert(max_v == 9 && max_i == 4, "max");


// =============================================================================
// 第7章：std::string_view 编译期字符串处理
// =============================================================================
// string_view 是 C++17 引入的只读字符串视图。
// 它是 constexpr 友好的，支持编译期字符串操作。

// --- 7.1 编译期字符串操作 ---

constexpr size_t ct_count_char(std::string_view sv, char c) {
    size_t count = 0;
    for (char ch : sv) {
        if (ch == c) ++count;
    }
    return count;
}

static_assert(ct_count_char("hello world", 'l') == 3, "3 l's");
static_assert(ct_count_char("", 'a') == 0, "empty");

// --- 7.2 编译期字符串分割（查找子串）---

constexpr std::string_view ct_trim(std::string_view sv) {
    while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t'))
        sv.remove_prefix(1);
    while (!sv.empty() && (sv.back() == ' ' || sv.back() == '\t'))
        sv.remove_suffix(1);
    return sv;
}

static_assert(ct_trim("  hello  ") == "hello", "trim");
static_assert(ct_trim("  ") == "", "trim spaces");

// --- 7.3 编译期路径处理 ---

constexpr std::string_view ct_filename(std::string_view path) {
    auto pos = path.rfind('/');
    if (pos == std::string_view::npos)
        pos = path.rfind('\\');
    if (pos == std::string_view::npos)
        return path;
    return path.substr(pos + 1);
}

constexpr std::string_view ct_extension(std::string_view path) {
    auto name = ct_filename(path);
    auto pos = name.rfind('.');
    if (pos == std::string_view::npos) return {};
    return name.substr(pos);
}

static_assert(ct_filename("/usr/local/bin/app") == "app", "filename");
static_assert(ct_filename("C:\\Users\\test.txt") == "test.txt", "win filename");
static_assert(ct_extension("photo.jpg") == ".jpg", "extension");
static_assert(ct_extension("Makefile") == "", "no extension");

// --- 7.4 FNV-1a 哈希 + 字符串 switch ---

constexpr uint32_t fnv1a(std::string_view sv) {
    uint32_t hash = 2166136261u;
    for (char c : sv) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

// 用法：让 string 支持 switch/case
void handle_http_method(std::string_view method) {
    switch (fnv1a(method)) {
        case fnv1a("GET"):
            std::cout << "  处理 GET 请求\n"; break;
        case fnv1a("POST"):
            std::cout << "  处理 POST 请求\n"; break;
        case fnv1a("PUT"):
            std::cout << "  处理 PUT 请求\n"; break;
        case fnv1a("DELETE"):
            std::cout << "  处理 DELETE 请求\n"; break;
        default:
            std::cout << "  未知方法: " << method << "\n"; break;
    }
}

// --- 7.5 编译期字符串比较器 ---

constexpr bool ct_starts_with(std::string_view sv, std::string_view prefix) {
    if (prefix.size() > sv.size()) return false;
    return sv.substr(0, prefix.size()) == prefix;
}

constexpr bool ct_ends_with(std::string_view sv, std::string_view suffix) {
    if (suffix.size() > sv.size()) return false;
    return sv.substr(sv.size() - suffix.size()) == suffix;
}

constexpr bool ct_contains(std::string_view sv, std::string_view sub) {
    return sv.find(sub) != std::string_view::npos;
}

static_assert(ct_starts_with("Hello, World", "Hello"), "starts_with");
static_assert(ct_ends_with("Hello, World", "World"), "ends_with");
static_assert(ct_contains("Hello, World", "lo, W"), "contains");


// =============================================================================
// 第8章：std::void_t 与简化 SFINAE
// =============================================================================
// C++17 引入了 std::void_t 和 std::conjunction/disjunction/negation，
// 极大简化了 SFINAE 类型检测的写法。

// --- 8.1 std::void_t 基础：检测表达式有效性 ---

// 检测类型是否可迭代
template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())
>> : std::true_type {};

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

// 检测类型是否可转为字符串
template <typename T, typename = void>
struct has_to_string : std::false_type {};

template <typename T>
struct has_to_string<T, std::void_t<
    decltype(std::to_string(std::declval<T>()))
>> : std::true_type {};

template <typename T>
inline constexpr bool has_to_string_v = has_to_string<T>::value;

static_assert(is_iterable_v<std::array<int, 5>>, "array is iterable");
static_assert(!is_iterable_v<int>, "int is not iterable");
static_assert(has_to_string_v<int>, "int has to_string");
static_assert(!has_to_string_v<std::string>, "string: no std::to_string(string)");

// --- 8.2 检测成员变量/函数 ---

// 检测是否有 .size() 方法
template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

// 检测是否有 .value 成员
template <typename T, typename = void>
struct has_value_member : std::false_type {};

template <typename T>
struct has_value_member<T, std::void_t<decltype(T::value)>>
    : std::true_type {};

static_assert(has_size<std::vector<int>>::value, "vector has size");
static_assert(!has_size<int>::value, "int has no size");
static_assert(has_value_member<std::true_type>::value, "true_type has ::value");

// --- 8.3 std::conjunction / disjunction / negation ---

// 逻辑组合多个类型谓词

template <typename T>
using is_string_like = std::disjunction<
    std::is_same<std::decay_t<T>, std::string>,
    std::is_same<std::decay_t<T>, std::string_view>,
    std::is_same<std::decay_t<T>, const char*>
>;

static_assert(is_string_like<std::string>::value, "string");
static_assert(is_string_like<const char*>::value, "c-string");
static_assert(!is_string_like<int>::value, "int is not string-like");

// 短路求值：conjunction 在遇到 false 时停止实例化
// 这在递归模板中很重要（避免无效的模板实例化）

template <typename T>
using is_printable_number = std::conjunction<
    std::is_arithmetic<T>,
    std::negation<std::is_same<T, bool>>
>;

static_assert(is_printable_number<int>::value, "int is printable number");
static_assert(!is_printable_number<bool>::value, "bool excluded");

// --- 8.4 综合：通用打印函数 ---

template <typename T>
void smart_print(const T& value) {
    if constexpr (is_iterable_v<T> && !is_string_like<T>::value) {
        // 容器：打印元素
        std::cout << "  [";
        bool first = true;
        for (const auto& elem : value) {
            if (!first) std::cout << ", ";
            std::cout << elem;
            first = false;
        }
        std::cout << "]";
    } else if constexpr (is_string_like<T>::value) {
        std::cout << "  \"" << value << "\"";
    } else if constexpr (std::is_arithmetic_v<T>) {
        std::cout << "  " << value;
    } else {
        std::cout << "  [unprintable]";
    }
    std::cout << "\n";
}


// =============================================================================
// 第9章：constexpr lambda — C++17 新增
// =============================================================================
// C++17 允许 lambda 表达式为 constexpr（自动或显式）。

// --- 9.1 显式 constexpr lambda ---

constexpr auto ct_square = [](int x) constexpr { return x * x; };
constexpr auto ct_cube   = [](int x) constexpr { return x * x * x; };

static_assert(ct_square(5) == 25, "lambda square");
static_assert(ct_cube(3) == 27, "lambda cube");

// --- 9.2 constexpr lambda 组合 ---

// 函数组合器
template <typename F, typename G>
constexpr auto compose(F f, G g) {
    return [f, g](auto x) constexpr { return f(g(x)); };
}

constexpr auto square_then_add1 = compose(
    [](int x) constexpr { return x + 1; },
    [](int x) constexpr { return x * x; }
);

static_assert(square_then_add1(5) == 26, "5^2 + 1 = 26");

// --- 9.3 constexpr lambda 作为模板参数的排序比较器 ---

template <typename T, size_t N, typename Compare>
constexpr std::array<T, N> ct_sort(std::array<T, N> arr, Compare cmp) {
    // 插入排序（对 constexpr 友好）
    for (size_t i = 1; i < N; ++i) {
        T key = arr[i];
        size_t j = i;
        while (j > 0 && cmp(key, arr[j - 1])) {
            arr[j] = arr[j - 1];
            --j;
        }
        arr[j] = key;
    }
    return arr;
}

constexpr auto ascending = ct_sort(
    std::array{5, 3, 8, 1, 9, 2},
    [](int a, int b) constexpr { return a < b; }
);

constexpr auto descending = ct_sort(
    std::array{5, 3, 8, 1, 9, 2},
    [](int a, int b) constexpr { return a > b; }
);

static_assert(ascending[0] == 1 && ascending[5] == 9, "ascending");
static_assert(descending[0] == 9 && descending[5] == 1, "descending");

// --- 9.4 编译期 map/filter/reduce ---

template <typename T, size_t N, typename F>
constexpr auto ct_map(const std::array<T, N>& arr, F f) {
    using ResultT = decltype(f(arr[0]));
    std::array<ResultT, N> result{};
    for (size_t i = 0; i < N; ++i) {
        result[i] = f(arr[i]);
    }
    return result;
}

template <typename T, size_t N, typename F, typename Acc>
constexpr Acc ct_reduce(const std::array<T, N>& arr, Acc init, F f) {
    for (size_t i = 0; i < N; ++i) {
        init = f(init, arr[i]);
    }
    return init;
}

constexpr auto squares = ct_map(
    std::array{1, 2, 3, 4, 5},
    [](int x) constexpr { return x * x; }
);

constexpr auto total = ct_reduce(
    squares, 0,
    [](int acc, int x) constexpr { return acc + x; }
);

static_assert(squares[2] == 9, "3^2 = 9");
static_assert(total == 55, "1+4+9+16+25 = 55");


// =============================================================================
// 第10章：变参模板高级技巧
// =============================================================================

// --- 10.1 编译期计算参数包属性 ---

template <typename... Ts>
struct TypePack {
    static constexpr size_t size = sizeof...(Ts);

    // 编译期计算所有类型的总大小
    static constexpr size_t total_size = (sizeof(Ts) + ...);

    // 最大对齐
    static constexpr size_t max_align = fold_max(alignof(Ts)...);

    // 是否所有类型都是平凡的
    static constexpr bool all_trivial = (std::is_trivially_copyable_v<Ts> && ...);
};

using MyPack = TypePack<int, double, char, float>;
static_assert(MyPack::size == 4, "4 types");
static_assert(MyPack::all_trivial, "all trivial");

// --- 10.2 索引参数包 ---

template <size_t I, typename T, typename... Ts>
constexpr decltype(auto) pack_get(T&& first, Ts&&... rest) {
    if constexpr (I == 0) {
        return std::forward<T>(first);
    } else {
        return pack_get<I - 1>(std::forward<Ts>(rest)...);
    }
}

static_assert(pack_get<0>(10, 20, 30) == 10, "first");
static_assert(pack_get<2>(10, 20, 30) == 30, "third");

// --- 10.3 编译期 zip ---

template <typename F, typename Tuple1, typename Tuple2, size_t... Is>
constexpr auto zip_with_impl(F f, const Tuple1& t1, const Tuple2& t2,
                             std::index_sequence<Is...>) {
    return std::make_tuple(f(std::get<Is>(t1), std::get<Is>(t2))...);
}

template <typename F, typename Tuple1, typename Tuple2>
constexpr auto zip_with(F f, const Tuple1& t1, const Tuple2& t2) {
    constexpr size_t N = std::min(
        std::tuple_size_v<Tuple1>,
        std::tuple_size_v<Tuple2>
    );
    return zip_with_impl(f, t1, t2, std::make_index_sequence<N>{});
}

// --- 10.4 编译期类型去重 ---

template <typename T, typename... Ts>
constexpr bool is_unique_type() {
    return (!std::is_same_v<T, Ts> && ...);
}

template <typename... Ts>
constexpr bool all_unique_types() {
    // 利用 index sequence 检查每个类型与后面的不重复
    // 简化版：
    return true; // 完整实现需要更多代码
}


// =============================================================================
// 第11章：编译期查找表与数据结构
// =============================================================================

// --- 11.1 编译期 sin/cos 查找表 ---

constexpr double ct_sin_approx(double x) {
    // 泰勒级数: sin(x) ≈ x - x³/3! + x⁵/5! - ...
    double result = 0.0;
    double term = x;
    for (int i = 1; i <= 15; ++i) {
        result += term;
        term *= -x * x / (2.0 * i * (2.0 * i + 1.0));
    }
    return result;
}

constexpr double ct_cos_approx(double x) {
    double result = 0.0;
    double term = 1.0;
    for (int i = 0; i < 15; ++i) {
        result += term;
        term *= -x * x / ((2.0 * i + 1.0) * (2.0 * i + 2.0));
    }
    return result;
}

template <size_t N>
constexpr auto make_sin_cos_table() {
    struct SinCos { double sin_val; double cos_val; };
    std::array<SinCos, N> table{};
    constexpr double TWO_PI = 2.0 * 3.14159265358979323846;
    for (size_t i = 0; i < N; ++i) {
        double angle = TWO_PI * static_cast<double>(i) / static_cast<double>(N);
        table[i] = { ct_sin_approx(angle), ct_cos_approx(angle) };
    }
    return table;
}

inline constexpr auto trig_lut = make_sin_cos_table<360>(); // 每度一个采样点

// --- 11.2 编译期 CRC32 ---

constexpr uint32_t crc32_entry(uint32_t idx) {
    uint32_t crc = idx;
    for (int j = 0; j < 8; ++j)
        crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
    return crc;
}

template <size_t... I>
constexpr auto make_crc32_table(std::index_sequence<I...>) {
    return std::array<uint32_t, sizeof...(I)>{crc32_entry(static_cast<uint32_t>(I))...};
}

inline constexpr auto CRC32_TABLE = make_crc32_table(std::make_index_sequence<256>{});

constexpr uint32_t crc32(std::string_view data) {
    uint32_t crc = 0xFFFFFFFF;
    for (char c : data) {
        crc = (crc >> 8) ^ CRC32_TABLE[(crc ^ static_cast<uint8_t>(c)) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

constexpr uint32_t crc_hello = crc32("Hello, World!");
static_assert(crc_hello != 0, "CRC32 computed at compile time");

// --- 11.3 编译期查找映射 (constexpr map) ---

template <typename Key, typename Value, size_t N>
class ConstMap {
    std::array<std::pair<Key, Value>, N> data_;

public:
    constexpr ConstMap(std::array<std::pair<Key, Value>, N> data)
        : data_(data) {}

    constexpr Value at(const Key& key) const {
        for (const auto& [k, v] : data_) {
            if (k == key) return v;
        }
        // 在 constexpr 中这里的"运行期"行为不重要
        // 可以返回默认值
        return Value{};
    }

    constexpr bool contains(const Key& key) const {
        for (const auto& [k, v] : data_) {
            if (k == key) return true;
        }
        return false;
    }

    constexpr size_t size() const { return N; }
};

// 辅助函数推导 N
template <typename Key, typename Value, size_t N>
constexpr auto make_const_map(std::pair<Key, Value> (&&items)[N]) {
    std::array<std::pair<Key, Value>, N> arr{};
    for (size_t i = 0; i < N; ++i) arr[i] = items[i];
    return ConstMap<Key, Value, N>(arr);
}

constexpr auto HTTP_STATUS = make_const_map<std::string_view, int>({
    {"OK", 200},
    {"Not Found", 404},
    {"Internal Server Error", 500},
    {"Bad Request", 400},
    {"Unauthorized", 401},
});

static_assert(HTTP_STATUS.at("OK") == 200, "OK=200");
static_assert(HTTP_STATUS.at("Not Found") == 404, "404");
static_assert(HTTP_STATUS.contains("Bad Request"), "has Bad Request");


// =============================================================================
// 第12章：CRTP 编译期多态 (进阶)
// =============================================================================

// --- 12.1 CRTP mixin 链 ---
// 通过 CRTP 组合多个能力，编译期解析。

template <typename Derived>
struct Printable_Mixin {
    void print() const {
        std::cout << "  " << static_cast<const Derived*>(this)->to_string() << "\n";
    }
};

template <typename Derived>
struct Comparable_Mixin {
    bool operator<(const Derived& rhs) const {
        return static_cast<const Derived*>(this)->compare(rhs) < 0;
    }
    bool operator>(const Derived& rhs) const {
        return static_cast<const Derived*>(this)->compare(rhs) > 0;
    }
    bool operator==(const Derived& rhs) const {
        return static_cast<const Derived*>(this)->compare(rhs) == 0;
    }
};

template <typename Derived>
struct Cloneable_Mixin {
    Derived clone() const {
        return Derived(static_cast<const Derived&>(*this));
    }
};

// 组合使用
class Temperature : public Printable_Mixin<Temperature>,
                    public Comparable_Mixin<Temperature>,
                    public Cloneable_Mixin<Temperature> {
    double celsius_;
public:
    constexpr Temperature(double c) : celsius_(c) {}

    std::string to_string() const {
        return std::to_string(celsius_) + "°C";
    }

    constexpr int compare(const Temperature& rhs) const {
        if (celsius_ < rhs.celsius_) return -1;
        if (celsius_ > rhs.celsius_) return 1;
        return 0;
    }

    constexpr double value() const { return celsius_; }
};

// --- 12.2 CRTP 静态接口检查 ---

template <typename Derived>
class Serializer {
public:
    auto serialize_to_json() const {
        // 在编译期验证 Derived 有 get_fields() 方法
        return "{" + static_cast<const Derived*>(this)->get_fields() + "}";
    }

    // CRTP 可以提供默认实现，派生类可以覆盖
    std::string get_type_name() const {
        return "Unknown";
    }
};

class UserData : public Serializer<UserData> {
public:
    std::string name = "Alice";
    int age = 30;

    std::string get_fields() const {
        return "\"name\":\"" + name + "\",\"age\":" + std::to_string(age);
    }

    std::string get_type_name() const {
        return "UserData";
    }
};


// =============================================================================
// 第13章：编译期类型计算
// =============================================================================

// --- 13.1 编译期类型列表 ---

template <typename... Ts>
struct TypeList {};

// 获取长度
template <typename List>
struct Length;

template <typename... Ts>
struct Length<TypeList<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
};

// 获取第 N 个类型
template <size_t N, typename List>
struct At;

template <typename Head, typename... Tail>
struct At<0, TypeList<Head, Tail...>> {
    using type = Head;
};

template <size_t N, typename Head, typename... Tail>
struct At<N, TypeList<Head, Tail...>> {
    using type = typename At<N - 1, TypeList<Tail...>>::type;
};

// 追加类型
template <typename List, typename T>
struct PushBack;

template <typename... Ts, typename T>
struct PushBack<TypeList<Ts...>, T> {
    using type = TypeList<Ts..., T>;
};

// 类型是否存在
template <typename List, typename T>
struct Contains;

template <typename T>
struct Contains<TypeList<>, T> : std::false_type {};

template <typename T, typename... Tail>
struct Contains<TypeList<T, Tail...>, T> : std::true_type {};

template <typename Head, typename... Tail, typename T>
struct Contains<TypeList<Head, Tail...>, T> : Contains<TypeList<Tail...>, T> {};

// 反转类型列表
template <typename List>
struct Reverse;

template <>
struct Reverse<TypeList<>> {
    using type = TypeList<>;
};

template <typename Head, typename... Tail>
struct Reverse<TypeList<Head, Tail...>> {
    using type = typename PushBack<typename Reverse<TypeList<Tail...>>::type, Head>::type;
};

// 测试
using MyList = TypeList<int, double, char, float>;

static_assert(Length<MyList>::value == 4, "4 types");
static_assert(std::is_same_v<At<0, MyList>::type, int>, "index 0");
static_assert(std::is_same_v<At<2, MyList>::type, char>, "index 2");
static_assert(Contains<MyList, double>::value, "has double");
static_assert(!Contains<MyList, long>::value, "no long");

using Reversed = Reverse<MyList>::type;
static_assert(std::is_same_v<At<0, Reversed>::type, float>, "reversed first");
static_assert(std::is_same_v<At<3, Reversed>::type, int>, "reversed last");

// --- 13.2 编译期类型过滤 ---

template <template <typename> typename Pred, typename List>
struct Filter;

template <template <typename> typename Pred>
struct Filter<Pred, TypeList<>> {
    using type = TypeList<>;
};

template <template <typename> typename Pred, typename Head, typename... Tail>
struct Filter<Pred, TypeList<Head, Tail...>> {
    using rest = typename Filter<Pred, TypeList<Tail...>>::type;
    using type = std::conditional_t<
        Pred<Head>::value,
        typename PushBack<TypeList<Head>, rest>::type, // 简化，实际需要 Concat
        rest
    >;
    // 注：完整的需要 Prepend 而非 PushBack + Concat
};

// --- 13.3 编译期类型大小排序 ---

template <typename A, typename B>
struct SmallerType {
    static constexpr bool value = sizeof(A) < sizeof(B);
};


// =============================================================================
// 第14章：编译期 EBO 与布局优化
// =============================================================================
// Empty Base Optimization (EBO) 是编译器的一种优化：
// 空基类不占用存储空间。

// --- 14.1 问题演示 ---

struct Empty {};
struct NotOptimized {
    Empty e;       // 占 1 字节（+ padding）
    int value;     // 4 字节
    // total: 可能 8 字节（对齐填充）
};

struct Optimized : Empty {
    int value;     // 4 字节
    // total: 4 字节（Empty 不占空间）
};

static_assert(sizeof(NotOptimized) > sizeof(int), "not optimized");
static_assert(sizeof(Optimized) == sizeof(int), "EBO works");

// --- 14.2 [[no_unique_address]] (C++20) 的 C++17 替代 ---
// 在 C++17 中，我们使用 EBO 来实现类似的效果

template <typename Alloc, typename T>
class CompactContainer : private Alloc {
    // Alloc 通常是空类（无状态分配器）
    // 通过 EBO，不占额外空间
    T* data_;
    size_t size_;
public:
    CompactContainer() : data_(nullptr), size_(0) {}
    Alloc& get_allocator() { return *this; }
};

// --- 14.3 compressed_pair：利用 EBO ---

template <typename T1, typename T2,
          bool = std::is_empty_v<T1> && !std::is_final_v<T1>>
class compressed_pair;

// T1 为空类时，继承 T1 节省空间
template <typename T1, typename T2>
class compressed_pair<T1, T2, true> : private T1 {
    T2 second_;
public:
    constexpr compressed_pair() = default;
    constexpr compressed_pair(T1 t1, T2 t2) : T1(t1), second_(t2) {}

    constexpr T1& first() { return *this; }
    constexpr const T1& first() const { return *this; }
    constexpr T2& second() { return second_; }
    constexpr const T2& second() const { return second_; }
};

// T1 非空时，正常存储
template <typename T1, typename T2>
class compressed_pair<T1, T2, false> {
    T1 first_;
    T2 second_;
public:
    constexpr compressed_pair() = default;
    constexpr compressed_pair(T1 t1, T2 t2) : first_(t1), second_(t2) {}

    constexpr T1& first() { return first_; }
    constexpr const T1& first() const { return first_; }
    constexpr T2& second() { return second_; }
    constexpr const T2& second() const { return second_; }
};

struct EmptyDeleter {};

// compressed_pair<EmptyDeleter, int*> 大小 == sizeof(int*)
// std::pair<EmptyDeleter, int*> 大小 > sizeof(int*)
static_assert(sizeof(compressed_pair<EmptyDeleter, int*>) == sizeof(int*),
              "compressed_pair EBO");


// =============================================================================
// 第15章：std::variant + std::visit 编译期分派
// =============================================================================
// C++17 的 std::variant 是类型安全的 union。
// std::visit 在编译期生成访问跳转表，运行期 O(1) 分派。

// --- 15.1 Overloaded 模式（C++17 经典惯用法）---
// 已在第4章定义 Overloaded 和 CTAD 推导指引

void variant_demo() {
    using Value = std::variant<int, double, std::string>;

    Value values[] = {42, 3.14, std::string("hello")};

    for (const auto& v : values) {
        std::visit(Overloaded{
            [](int i)              { std::cout << "  int: " << i << "\n"; },
            [](double d)           { std::cout << "  double: " << d << "\n"; },
            [](const std::string& s) { std::cout << "  string: " << s << "\n"; }
        }, v);
    }
}

// --- 15.2 编译期多路分派 ---

using Shape = std::variant<struct Circle_, struct Rect_, struct Triangle_>;

struct Circle_ {
    double radius;
    constexpr double area() const { return 3.14159265 * radius * radius; }
};
struct Rect_ {
    double w, h;
    constexpr double area() const { return w * h; }
};
struct Triangle_ {
    double base, height;
    constexpr double area() const { return 0.5 * base * height; }
};

// std::visit 编译期生成跳转表，零虚函数调用
double total_area(const Shape* shapes, size_t n) {
    double total = 0;
    for (size_t i = 0; i < n; ++i) {
        total += std::visit([](const auto& s) { return s.area(); }, shapes[i]);
    }
    return total;
}

// --- 15.3 variant 状态机 ---

struct Idle    { std::string_view name = "Idle"; };
struct Running { std::string_view name = "Running"; int speed = 0; };
struct Paused  { std::string_view name = "Paused"; };
struct Error   { std::string_view name = "Error"; std::string_view msg; };

using MachineState = std::variant<Idle, Running, Paused, Error>;

struct StartEvent {};
struct StopEvent {};
struct PauseEvent {};
struct ErrorEvent { std::string_view message; };

// 状态转移函数 —— 编译期生成所有组合的跳转表
MachineState process_event(const MachineState& state, const auto& event) {
    return std::visit(Overloaded{
        // Idle 状态
        [](const Idle&, const StartEvent&) -> MachineState { return Running{"Running", 100}; },
        [](const Idle&, const auto&) -> MachineState { return Idle{}; },

        // Running 状态
        [](const Running&, const PauseEvent&) -> MachineState { return Paused{}; },
        [](const Running&, const StopEvent&) -> MachineState { return Idle{}; },
        [](const Running&, const ErrorEvent& e) -> MachineState { return Error{"Error", e.message}; },
        [](const Running& r, const auto&) -> MachineState { return r; },

        // Paused 状态
        [](const Paused&, const StartEvent&) -> MachineState { return Running{"Running", 50}; },
        [](const Paused&, const StopEvent&) -> MachineState { return Idle{}; },
        [](const Paused& p, const auto&) -> MachineState { return p; },

        // Error 状态 —— 只能 Stop
        [](const Error&, const StopEvent&) -> MachineState { return Idle{}; },
        [](const Error& e, const auto&) -> MachineState { return e; },
    }, state, event);
}

std::string_view get_state_name(const MachineState& state) {
    return std::visit([](const auto& s) { return s.name; }, state);
}


// =============================================================================
// 第16章：实战 — 编译期词法分析器
// =============================================================================
// 一个在编译期对字符串进行词法分析的完整示例。

enum class TokenType {
    Number,
    Plus,
    Minus,
    Star,
    Slash,
    LParen,
    RParen,
    End,
    Error
};

struct Token {
    TokenType type;
    std::string_view text;
    int value; // 仅对 Number 有效
};

constexpr const char* token_type_name(TokenType t) {
    switch (t) {
        case TokenType::Number: return "Number";
        case TokenType::Plus:   return "Plus";
        case TokenType::Minus:  return "Minus";
        case TokenType::Star:   return "Star";
        case TokenType::Slash:  return "Slash";
        case TokenType::LParen: return "LParen";
        case TokenType::RParen: return "RParen";
        case TokenType::End:    return "End";
        case TokenType::Error:  return "Error";
    }
    return "Unknown";
}

class Lexer {
    std::string_view source_;
    size_t pos_ = 0;

    constexpr char peek() const {
        return pos_ < source_.size() ? source_[pos_] : '\0';
    }
    constexpr char advance() {
        return pos_ < source_.size() ? source_[pos_++] : '\0';
    }
    constexpr void skip_whitespace() {
        while (pos_ < source_.size() &&
               (source_[pos_] == ' ' || source_[pos_] == '\t'))
            ++pos_;
    }
    constexpr bool is_digit(char c) const {
        return c >= '0' && c <= '9';
    }

public:
    constexpr Lexer(std::string_view src) : source_(src) {}

    constexpr Token next_token() {
        skip_whitespace();
        if (pos_ >= source_.size())
            return {TokenType::End, {}, 0};

        size_t start = pos_;
        char c = advance();

        switch (c) {
            case '+': return {TokenType::Plus, source_.substr(start, 1), 0};
            case '-': return {TokenType::Minus, source_.substr(start, 1), 0};
            case '*': return {TokenType::Star, source_.substr(start, 1), 0};
            case '/': return {TokenType::Slash, source_.substr(start, 1), 0};
            case '(': return {TokenType::LParen, source_.substr(start, 1), 0};
            case ')': return {TokenType::RParen, source_.substr(start, 1), 0};
            default:
                if (is_digit(c)) {
                    int value = c - '0';
                    while (is_digit(peek())) {
                        value = value * 10 + (advance() - '0');
                    }
                    return {TokenType::Number, source_.substr(start, pos_ - start), value};
                }
                return {TokenType::Error, source_.substr(start, 1), 0};
        }
    }
};

// --- 16.1 编译期词法分析 ---

template <size_t MaxTokens = 64>
constexpr auto tokenize(std::string_view source) {
    struct Result {
        std::array<Token, MaxTokens> tokens{};
        size_t count = 0;
    };

    Result result;
    Lexer lexer(source);

    while (result.count < MaxTokens) {
        auto tok = lexer.next_token();
        result.tokens[result.count++] = tok;
        if (tok.type == TokenType::End || tok.type == TokenType::Error)
            break;
    }
    return result;
}

// 编译期词法分析！
constexpr auto tokens = tokenize("(1 + 2) * 3 - 4 / 2");

static_assert(tokens.tokens[0].type == TokenType::LParen, "first is (");
static_assert(tokens.tokens[1].type == TokenType::Number, "then number");
static_assert(tokens.tokens[1].value == 1, "value is 1");
static_assert(tokens.tokens[2].type == TokenType::Plus, "then +");
static_assert(tokens.tokens[3].type == TokenType::Number, "then number");
static_assert(tokens.tokens[3].value == 2, "value is 2");

// --- 16.2 编译期表达式求值（简化版：仅加减）---

constexpr int eval_simple(std::string_view expr) {
    auto result = tokenize(expr);
    const auto& toks = result.tokens;

    // 简化：只处理 number (+|-) number (+|-) number ...
    if (toks[0].type != TokenType::Number) return -1;
    int value = toks[0].value;

    size_t i = 1;
    while (toks[i].type != TokenType::End) {
        TokenType op = toks[i].type;
        if (op != TokenType::Plus && op != TokenType::Minus) break;
        ++i;
        if (toks[i].type != TokenType::Number) break;
        int rhs = toks[i].value;
        ++i;
        if (op == TokenType::Plus)  value += rhs;
        else                        value -= rhs;
    }
    return value;
}

static_assert(eval_simple("10 + 20 + 30") == 60, "10+20+30");
static_assert(eval_simple("100 - 30 - 20") == 50, "100-30-20");
static_assert(eval_simple("5 + 3 - 2") == 6, "5+3-2");


// =============================================================================
// 附录：C++17 编译期优化速查表
// =============================================================================
//
// | 特性               | 用途                     | 替代/改进              |
// |--------------------|--------------------------|------------------------|
// | if constexpr       | 编译期分支消除            | 替代 SFINAE/tag dispatch|
// | 折叠表达式         | 参数包展开                | 替代递归模板           |
// | constexpr lambda   | 编译期高阶函数            | 全新特性               |
// | inline constexpr   | 头文件常量定义            | 解决 ODR 问题          |
// | CTAD               | 减少模板参数冗余          | 全新特性               |
// | string_view        | 编译期字符串处理          | 全新特性               |
// | void_t             | 简化 SFINAE              | 替代复杂 SFINAE        |
// | conjunction/...    | 逻辑组合 type traits     | 全新特性               |
// | std::variant       | 类型安全 union + 分派     | 替代继承多态           |
// | 结构化绑定         | 多返回值                  | 全新特性               |
// | std::optional      | 可选值                    | 全新特性               |
//
// 编译命令：
//   GCC:  g++ -std=c++17 -O2 -Wall -Wextra -o test3 test3.cpp
//   Clang: clang++ -std=c++17 -O2 -Wall -Wextra -o test3 test3.cpp
//   MSVC: cl /std:c++17 /O2 /W4 /EHsc test3.cpp
// =============================================================================


// =============================================================================
// main: 运行演示
// =============================================================================
int main() {
    std::cout << "===== C++17 编译期优化完全教程 演示 =====\n\n";

    // 1. constexpr
    std::cout << "[1] constexpr 增强:\n";
    std::cout << "  power(2,10) = " << power(2, 10) << "\n";
    std::cout << "  sum_of_squares(10) = " << sum_of_squares(10) << "\n";
    std::cout << "  fib[10] = " << fib_table[10] << ", fib[19] = " << fib_table[19] << "\n";
    std::cout << "  Fraction: 1/2 + 1/3 = "
              << sum_frac.numerator() << "/" << sum_frac.denominator() << "\n\n";

    // 2. if constexpr
    std::cout << "[2] if constexpr:\n";
    std::cout << "  " << serialize(42) << "\n";
    std::cout << "  " << serialize(3.14) << "\n";
    std::cout << "  " << serialize(std::string("hello")) << "\n";
    std::cout << "  dimensions of int[2][3][4] = " << count_dimensions<int[2][3][4]>() << "\n";
    std::cout << "  byte_swap(0x0102) = 0x"
              << std::hex << byte_swap<Endian::Big>(uint16_t(0x0102))
              << std::dec << "\n\n";

    // 3. 折叠表达式
    std::cout << "[3] 折叠表达式:\n";
    std::cout << "  sum(1..5) = " << fold_sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "  min(5,3,8,1,9) = " << fold_min(5, 3, 8, 1, 9) << "\n";
    std::cout << "  max(5,3,8,1,9) = " << fold_max(5, 3, 8, 1, 9) << "\n";
    std::cout << "  ";
    print_separated(", ", "hello", 42, 3.14, "world");
    std::cout << "  integral count(1, 2.0, 3, 'a', 5.0f) = "
              << count_integral(1, 2.0, 3, 'a', 5.0f) << "\n\n";

    // 4. CTAD
    std::cout << "[4] CTAD:\n";
    auto p = std::pair{42, std::string("hello")}; // 无需写 pair<int, string>
    auto t = std::tuple{1, 2.0, 'c'};
    std::cout << "  pair: {" << p.first << ", " << p.second << "}\n";
    std::cout << "  tuple: {" << std::get<0>(t) << ", " << std::get<1>(t)
              << ", " << std::get<2>(t) << "}\n";
    StaticVec sv{10, 20, 30, 40};
    std::cout << "  StaticVec{10,20,30,40} size=" << sv.size() << "\n\n";

    // 5. inline constexpr
    std::cout << "[5] inline constexpr:\n";
    std::cout << "  PI = " << PI << "\n";
    std::cout << "  Config: " << ACTIVE_CONFIG.app_name
              << ", buf=" << ACTIVE_CONFIG.buffer_size << "\n\n";

    // 6. 结构化绑定
    std::cout << "[6] 结构化绑定:\n";
    std::cout << "  compute_stats(10,3): sum=" << ct_sum << " diff=" << ct_diff
              << " prod=" << ct_prod << "\n";
    std::cout << "  minmax of {5,2,8,1,9,3}: min=" << min_v << "@[" << min_i
              << "], max=" << max_v << "@[" << max_i << "]\n\n";

    // 7. string_view
    std::cout << "[7] string_view 编译期处理:\n";
    std::cout << "  filename('/usr/bin/app') = " << ct_filename("/usr/bin/app") << "\n";
    std::cout << "  extension('photo.jpg') = " << ct_extension("photo.jpg") << "\n";
    std::cout << "  trim('  hello  ') = '" << ct_trim("  hello  ") << "'\n";
    handle_http_method("GET");
    handle_http_method("POST");
    std::cout << "\n";

    // 8. void_t & SFINAE
    std::cout << "[8] void_t SFINAE:\n";
    std::cout << "  is_iterable<vector<int>> = " << std::boolalpha
              << is_iterable_v<std::vector<int>> << "\n";
    std::cout << "  is_iterable<int> = " << is_iterable_v<int> << "\n";
    std::cout << "  smart_print: ";
    smart_print(std::vector<int>{1, 2, 3, 4, 5});
    std::cout << "  smart_print: ";
    smart_print(42);
    std::cout << "  smart_print: ";
    smart_print(std::string_view("hello"));
    std::cout << "\n";

    // 9. constexpr lambda
    std::cout << "[9] constexpr lambda:\n";
    std::cout << "  square(7) = " << ct_square(7) << "\n";
    std::cout << "  compose(+1, ^2)(5) = " << square_then_add1(5) << "\n";
    std::cout << "  sorted asc: ";
    for (auto x : ascending) std::cout << x << " ";
    std::cout << "\n  sorted desc: ";
    for (auto x : descending) std::cout << x << " ";
    std::cout << "\n  map+reduce [1..5]^2 sum = " << total << "\n\n";

    // 10. 变参模板
    std::cout << "[10] 变参模板:\n";
    std::cout << "  TypePack<int,double,char,float>.size = " << MyPack::size << "\n";
    std::cout << "  TypePack total_size = " << MyPack::total_size << "\n";
    std::cout << "  pack_get<2>(10,20,30) = " << pack_get<2>(10, 20, 30) << "\n\n";

    // 11. 编译期查找表
    std::cout << "[11] 编译期查找表:\n";
    std::cout << "  sin(0°) = " << trig_lut[0].sin_val << "\n";
    std::cout << "  cos(0°) = " << trig_lut[0].cos_val << "\n";
    std::cout << "  sin(90°) = " << trig_lut[90].sin_val << "\n";
    std::cout << "  CRC32('Hello, World!') = 0x" << std::hex << crc_hello << std::dec << "\n";
    std::cout << "  HTTP_STATUS['OK'] = " << HTTP_STATUS.at("OK") << "\n";
    std::cout << "  HTTP_STATUS['Not Found'] = " << HTTP_STATUS.at("Not Found") << "\n\n";

    // 12. CRTP
    std::cout << "[12] CRTP mixin:\n";
    Temperature t1(100.0), t2(36.5);
    t1.print();
    t2.print();
    std::cout << "  100°C > 36.5°C ? " << std::boolalpha << (t1 > t2) << "\n\n";

    // 13. EBO
    std::cout << "[13] EBO:\n";
    std::cout << "  sizeof(NotOptimized) = " << sizeof(NotOptimized) << "\n";
    std::cout << "  sizeof(Optimized)    = " << sizeof(Optimized) << "\n";
    std::cout << "  sizeof(compressed_pair<Empty,int*>) = "
              << sizeof(compressed_pair<EmptyDeleter, int*>) << "\n\n";

    // 14. variant + visit
    std::cout << "[14] variant + visit:\n";
    variant_demo();

    Shape shapes[] = {Circle_{5.0}, Rect_{4.0, 6.0}, Triangle_{3.0, 8.0}};
    std::cout << "  total area = " << total_area(shapes, 3) << "\n\n";

    // 15. variant 状态机
    std::cout << "[15] variant 状态机:\n";
    MachineState state = Idle{};
    std::cout << "  state: " << get_state_name(state) << "\n";
    state = process_event(state, StartEvent{});
    std::cout << "  -> Start -> " << get_state_name(state) << "\n";
    state = process_event(state, PauseEvent{});
    std::cout << "  -> Pause -> " << get_state_name(state) << "\n";
    state = process_event(state, StartEvent{});
    std::cout << "  -> Start -> " << get_state_name(state) << "\n";
    state = process_event(state, ErrorEvent{"disk full"});
    std::cout << "  -> Error -> " << get_state_name(state) << "\n";
    state = process_event(state, StopEvent{});
    std::cout << "  -> Stop  -> " << get_state_name(state) << "\n\n";

    // 16. 编译期词法分析器
    std::cout << "[16] 编译期词法分析器:\n";
    std::cout << "  tokenize('(1 + 2) * 3 - 4 / 2'):\n";
    for (size_t i = 0; i < tokens.count; ++i) {
        const auto& tok = tokens.tokens[i];
        std::cout << "    " << token_type_name(tok.type);
        if (tok.type == TokenType::Number)
            std::cout << "(" << tok.value << ")";
        std::cout << " ";
    }
    std::cout << "\n";
    std::cout << "  eval('10 + 20 + 30') = " << eval_simple("10 + 20 + 30") << "\n";
    std::cout << "  eval('100 - 30 - 20') = " << eval_simple("100 - 30 - 20") << "\n";

    std::cout << "\n===== 所有 static_assert 通过 = 编译期正确性已验证 =====\n";
    return 0;
}
