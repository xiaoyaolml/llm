// =============================================================================
// 精通现代 C++（C++17/20/23）— 零开销抽象与编译期优化 完全教程
// =============================================================================
//
// 编译：g++ -std=c++2b -O2 -o test12 test12.cpp
//       cl /std:c++latest /O2 /EHsc test12.cpp
//   (部分章节需要 C++20/23 编译器，已标注；可编译部分使用 C++17)
//
// 目录：
// ═══════════════════════════════════════════════════════════════
// 一、C++17 核心特性篇
//   1.  结构化绑定 (Structured Bindings)
//   2.  if constexpr — 编译期分支消除
//   3.  折叠表达式 (Fold Expressions)
//   4.  CTAD 类模板参数推导
//   5.  std::optional / std::variant / std::any
//   6.  std::string_view — 零拷贝字符串
//   7.  内联变量与嵌套命名空间
//   8.  std::invoke / std::apply — 通用调用
//
// 二、C++20 核心特性篇
//   9.  Concepts — 约束模板参数
//  10.  Ranges — 惰性管道式算法
//  11.  三路比较 <=> 与 operator==() = default
//  12.  consteval / constinit — 编译期强制
//  13.  协程 (Coroutines) 原理与 Generator
//  14.  std::format — 类型安全格式化
//  15.  Modules 概念与 import（概述）
//
// 三、C++23 新特性篇
//  16.  Deducing this — 显式对象参数
//  17.  std::expected — 错误处理新范式
//  18.  std::print / std::println
//  19.  if consteval / static operator()
//  20.  多维下标 operator[] 与 std::mdspan
//
// 四、零开销抽象篇
//  21.  CRTP — 静态多态（零虚表开销）
//  22.  Policy-Based Design — 策略组合
//  23.  Strong Types — 类型安全的零开销包装
//  24.  Type Erasure — 值语义多态
//  25.  std::variant 访问者模式 vs 虚函数
//
// 五、编译期计算篇
//  26.  constexpr 全面深入 (C++14→17→20→23)
//  27.  编译期容器与算法
//  28.  模板元编程：类型列表与编译期 Map
//  29.  编译期字符串处理
//  30.  编译期正则 / 状态机
//
// 六、高级模板技巧篇
//  31.  SFINAE / enable_if → Concepts 演进
//  32.  变参模板高级模式
//  33.  完美转发与万能引用陷阱
//  34.  Lambda 高级用法 (C++14→20→23)
//  35.  性能对比与最佳实践总结
// ═══════════════════════════════════════════════════════════════

#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <queue>
#include <tuple>
#include <optional>
#include <variant>
#include <any>
#include <functional>
#include <algorithm>
#include <numeric>
#include <memory>
#include <type_traits>
#include <utility>
#include <chrono>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <bitset>

// C++20 headers (MSVC / GCC 10+ / Clang 13+)
#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
  #define HAS_CPP20 1
  // #include <concepts>
  // #include <ranges>
  // #include <format>
  // #include <coroutine>
  // #include <compare>
#else
  #define HAS_CPP20 0
#endif

#if __cplusplus >= 202302L || _MSVC_LANG >= 202302L
  #define HAS_CPP23 1
  // #include <expected>
  // #include <print>
#else
  #define HAS_CPP23 0
#endif

using namespace std::string_literals;
using namespace std::string_view_literals;

// ─── 工具 ─────────────────────────────────────────────────────

static void print_header(const char* title) {
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║ " << std::left << std::setw(52) << title << " ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";
}
static void print_section(const char* title) {
    std::cout << "  ── " << title << " ──\n";
}

class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point start_ = clk::now();
public:
    double elapsed_us() const {
        return std::chrono::duration<double, std::micro>(clk::now()-start_).count();
    }
    double elapsed_ms() const { return elapsed_us() / 1000.0; }
    void reset() { start_ = clk::now(); }
};


// =============================================================================
// ██ 一、C++17 核心特性篇
// =============================================================================

// =============================================================================
// 第1章：结构化绑定 (Structured Bindings)
// =============================================================================

namespace ch1 {

// 结构化绑定可以绑定到：
// 1. 数组
// 2. tuple-like 类型 (std::pair, std::tuple, 自定义)
// 3. 聚合类 (public 数据成员)

struct Point { double x, y, z; };

// 自定义 tuple-like 绑定 (让任意类型支持结构化绑定)
class Color {
    uint8_t r_, g_, b_;
public:
    Color(uint8_t r, uint8_t g, uint8_t b) : r_(r), g_(g), b_(b) {}
    template<size_t I> auto get() const {
        if constexpr (I == 0) return r_;
        else if constexpr (I == 1) return g_;
        else return b_;
    }
};

} // namespace ch1

// 特化 tuple 接口 — 使 Color 支持结构化绑定
namespace std {
    template<> struct tuple_size<ch1::Color> : integral_constant<size_t, 3> {};
    template<size_t I> struct tuple_element<I, ch1::Color> { using type = uint8_t; };
}

namespace ch1 {

void demo() {
    print_section("结构化绑定 (Structured Bindings)");

    // 1. 绑定数组
    int arr[] = {10, 20, 30};
    auto [a, b, c] = arr;
    std::cout << "  数组: a=" << a << " b=" << b << " c=" << c << "\n";

    // 2. 绑定 pair/tuple
    auto [key, value] = std::make_pair("name"s, 42);
    std::cout << "  pair: " << key << " = " << value << "\n";

    auto [x, y, z] = std::make_tuple(1.0, 2.0, 3.0);
    std::cout << "  tuple: (" << x << ", " << y << ", " << z << ")\n";

    // 3. 绑定聚合类
    Point pt{1.5, 2.5, 3.5};
    auto& [px, py, pz] = pt; // 引用绑定，可修改原对象
    px = 100.0;
    std::cout << "  聚合: pt.x=" << pt.x << " (已被修改)\n";

    // 4. 绑定 map 迭代
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}};
    std::cout << "  map 遍历:\n";
    for (const auto& [name, score] : scores) {
        std::cout << "    " << name << ": " << score << "\n";
    }

    // 5. 自定义 tuple-like 绑定
    Color col(255, 128, 0);
    auto [r, g, bl] = col;
    std::cout << "  Color: R=" << (int)r << " G=" << (int)g << " B=" << (int)bl << "\n";

    // 6. 与 if 初始化语句结合 (C++17)
    if (auto [iter, inserted] = scores.insert({"Charlie", 92}); inserted) {
        std::cout << "  插入成功: " << iter->first << " = " << iter->second << "\n";
    }

    // 零开销: 结构化绑定在编译后完全消除，等同于直接成员访问
    std::cout << R"(
  零开销分析:
    auto [x, y] = pt;    →  编译为 x = pt.x; y = pt.y;
    auto& [x, y] = pt;   →  x 和 y 是 pt.x 和 pt.y 的引用
    无任何运行时开销，纯编译期语法糖
)";
}

} // namespace ch1


// =============================================================================
// 第2章：if constexpr — 编译期分支消除
// =============================================================================

namespace ch2 {

// if constexpr 在编译期求值，未选中的分支完全不编译 (不实例化)
// 这比传统 SFINAE / tag dispatch 简洁得多

// --- 类型分发 ---
template<typename T>
std::string type_name() {
    if constexpr (std::is_integral_v<T>)
        return "整数类型";
    else if constexpr (std::is_floating_point_v<T>)
        return "浮点类型";
    else if constexpr (std::is_pointer_v<T>)
        return "指针类型";
    else
        return "其他类型";
}

// --- 编译期递归展开 ---
template<typename T, typename... Ts>
void print_all(const T& first, const Ts&... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << ", ";
        print_all(rest...); // 只有 rest 非空时才展开
    }
}

// --- 条件性成员函数 ---
template<typename T>
class SmartContainer {
    std::vector<T> data_;
public:
    void add(const T& val) { data_.push_back(val); }

    // 只有算术类型才有 sum()
    auto sum() const {
        if constexpr (std::is_arithmetic_v<T>) {
            return std::accumulate(data_.begin(), data_.end(), T{0});
        } else {
            // 即使 T 没有 + 运算符，也不会报错，因为这个分支不会被实例化
            static_assert(std::is_arithmetic_v<T>, "sum() 仅支持算术类型");
            return T{};
        }
    }

    size_t size() const { return data_.size(); }
};

// --- 零开销序列化 ---
template<typename T>
void serialize(std::ostream& os, const T& val) {
    if constexpr (std::is_trivially_copyable_v<T>) {
        // POD 类型：直接内存拷贝（最快）
        os.write(reinterpret_cast<const char*>(&val), sizeof(T));
    } else if constexpr (std::is_same_v<T, std::string>) {
        uint32_t len = static_cast<uint32_t>(val.size());
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(val.data(), len);
    } else {
        // 编译期报错而非运行时失败
        static_assert(!std::is_same_v<T,T>, "不支持的序列化类型");
    }
}

void demo() {
    print_section("if constexpr — 编译期分支消除");

    std::cout << "  int  → " << type_name<int>() << "\n";
    std::cout << "  double → " << type_name<double>() << "\n";
    std::cout << "  int* → " << type_name<int*>() << "\n";

    std::cout << "  print_all: ";
    print_all(1, 2.5, "hello", 'A');
    std::cout << "\n";

    SmartContainer<int> c;
    c.add(10); c.add(20); c.add(30);
    std::cout << "  SmartContainer<int>.sum() = " << c.sum() << "\n";

    std::cout << R"(
  if constexpr vs 普通 if:
    普通 if:       两个分支都必须编译通过
    if constexpr:  未选中分支完全不实例化

  效果: 替代 SFINAE / tag dispatch / enable_if
    C++14: template<typename T, enable_if_t<is_integral_v<T>>* = nullptr>
    C++17: if constexpr (is_integral_v<T>) { ... }
    → 代码量减少 50%+, 可读性大幅提升
)";
}

} // namespace ch2


// =============================================================================
// 第3章：折叠表达式 (Fold Expressions)
// =============================================================================

namespace ch3 {

// C++17 折叠表达式: 对参数包执行运算的简洁方式
// 四种形式:
//   (pack op ...)       一元右折叠  a1 op (a2 op (a3 op a4))
//   (... op pack)       一元左折叠  ((a1 op a2) op a3) op a4
//   (pack op ... op init) 二元右折叠 a1 op (a2 op (a3 op init))
//   (init op ... op pack) 二元左折叠 ((init op a1) op a2) op a3

// 求和
template<typename... Ts>
auto sum(Ts... args) {
    return (args + ...); // 一元右折叠
}

// 全部为真
template<typename... Ts>
bool all_true(Ts... args) {
    return (args && ...);
}

// 任一为真
template<typename... Ts>
bool any_true(Ts... args) {
    return (args || ...);
}

// 打印（利用逗号运算符折叠）
template<typename... Ts>
void print_fold(const Ts&... args) {
    ((std::cout << args << " "), ...); // 一元左折叠 + 逗号
    std::cout << "\n";
}

// 编译期最大值
template<typename T, typename... Ts>
constexpr T max_of(T first, Ts... rest) {
    if constexpr (sizeof...(rest) == 0)
        return first;
    else {
        auto rest_max = max_of(rest...);
        return first > rest_max ? first : rest_max;
    }
}

// 折叠 + 类型检查: 编译期检查所有参数类型都是算术类型
template<typename... Ts>
constexpr bool all_arithmetic() {
    return (std::is_arithmetic_v<Ts> && ...);
}

// 折叠表达式用于函数调用链
template<typename T, typename... Funcs>
T compose(T val, Funcs... funcs) {
    return (funcs(val), ...); // 不对——逗号折叠丢弃中间结果
}

// 正确的管道式组合
template<typename T, typename... Funcs>
T pipeline(T val, Funcs... funcs) {
    ((val = funcs(val)), ...); // 每步更新 val
    return val;
}

void demo() {
    print_section("折叠表达式 (Fold Expressions)");

    std::cout << "  sum(1,2,3,4,5) = " << sum(1,2,3,4,5) << "\n";
    std::cout << "  all_true(1,1,1) = " << all_true(true, true, true) << "\n";
    std::cout << "  any_true(0,0,1) = " << any_true(false, false, true) << "\n";

    std::cout << "  print_fold: ";
    print_fold(42, 3.14, "hello", 'X');

    constexpr auto m = max_of(3, 7, 2, 9, 1);
    std::cout << "  constexpr max_of(3,7,2,9,1) = " << m << "\n";

    static_assert(all_arithmetic<int, double, float>());
    std::cout << "  all_arithmetic<int,double,float> = true ✓\n";

    // 管道式组合
    auto result = pipeline(10,
        [](int x) { return x * 2; },     // 20
        [](int x) { return x + 3; },     // 23
        [](int x) { return x * x; }      // 529
    );
    std::cout << "  pipeline(10, *2, +3, ^2) = " << result << "\n";

    std::cout << R"(
  折叠表达式 vs 递归模板:
    C++11: 递归展开 → 大量模板实例化 → 编译慢
    C++17: 折叠表达式 → 一次性展开 → 编译快 + 代码简洁
    生成的机器码相同 → 零开销
)";
}

} // namespace ch3


// =============================================================================
// 第4章：CTAD (Class Template Argument Deduction)
// =============================================================================

namespace ch4 {

// C++17: 构造函数可以推导模板参数

template<typename T>
class Wrapper {
    T value_;
public:
    Wrapper(T v) : value_(std::move(v)) {}
    const T& get() const { return value_; }
};

// 自定义推导指引 (Deduction Guide)
template<typename T>
Wrapper(T) -> Wrapper<T>;

// 字符串字面量 → string 推导指引
Wrapper(const char*) -> Wrapper<std::string>;

// 更复杂的推导指引
template<typename Iter>
class Range {
    Iter begin_, end_;
public:
    Range(Iter b, Iter e) : begin_(b), end_(e) {}
    Iter begin() const { return begin_; }
    Iter end() const { return end_; }
    auto size() const { return std::distance(begin_, end_); }
};

// 从容器推导
template<typename Container>
Range(Container&) -> Range<typename Container::iterator>;

// 从迭代器推导
template<typename Iter>
Range(Iter, Iter) -> Range<Iter>;

void demo() {
    print_section("CTAD 类模板参数推导");

    // C++17 之前：std::pair<int, double> p(1, 2.0);
    // C++17: 自动推导
    auto p = std::pair(1, 2.0);           // pair<int, double>
    auto t = std::tuple(1, "hello"s, 3.14); // tuple<int, string, double>
    auto v = std::vector{1, 2, 3, 4, 5};   // vector<int>
    auto o = std::optional(42);            // optional<int>

    std::cout << "  pair: (" << p.first << ", " << p.second << ")\n";
    std::cout << "  tuple: " << std::get<0>(t) << ", " << std::get<1>(t) << "\n";
    std::cout << "  vector: size=" << v.size() << "\n";
    std::cout << "  optional: " << *o << "\n";

    // 自定义 CTAD
    Wrapper w1(42);         // Wrapper<int>
    Wrapper w2(3.14);       // Wrapper<double>
    Wrapper w3("hello");    // Wrapper<string> (通过推导指引)

    std::cout << "  Wrapper(42): " << w1.get() << "\n";
    std::cout << "  Wrapper(3.14): " << w2.get() << "\n";
    std::cout << "  Wrapper(\"hello\"): " << w3.get() << "\n";

    // 容器推导指引
    std::vector vec{1, 2, 3};
    Range range(vec.begin(), vec.end());
    std::cout << "  Range size: " << range.size() << "\n";

    std::cout << R"(
  CTAD 规则:
    1. 编译器尝试用构造函数参数类型推导模板参数
    2. 如果有显式推导指引 (deduction guide)，优先使用
    3. 隐式推导指引从构造函数签名自动生成

  注意事项:
    std::vector v1{5};      →  vector<int>{5}       (一个元素5)
    std::vector v2(5, 0);   →  vector<int>(5, 0)    (五个0)
    → 花括号{} 和圆括号() 语义不同！
)";
}

} // namespace ch4


// =============================================================================
// 第5章：std::optional / std::variant / std::any
// =============================================================================

namespace ch5 {

// --- optional --- 安全的"可能有值"语义
struct UserRecord {
    std::string name;
    std::optional<std::string> email;    // 可能没有邮箱
    std::optional<int> age;              // 可能没有年龄信息
};

std::optional<int> safe_divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

// --- variant --- 类型安全的联合体 (tagged union)
using JsonValue = std::variant<
    std::nullptr_t,          // null
    bool,                    // boolean
    int,                     // integer
    double,                  // float
    std::string              // string
>;

// 访问者模式
struct JsonPrinter {
    void operator()(std::nullptr_t) const { std::cout << "null"; }
    void operator()(bool b) const { std::cout << (b ? "true" : "false"); }
    void operator()(int i) const { std::cout << i; }
    void operator()(double d) const { std::cout << d; }
    void operator()(const std::string& s) const { std::cout << '"' << s << '"'; }
};

// --- Overloaded 模式 (lambda 访问者) ---
template<typename... Ts>
struct Overloaded : Ts... { using Ts::operator()...; };
template<typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

void demo() {
    print_section("optional / variant / any");

    // optional
    auto r1 = safe_divide(10, 3);
    auto r2 = safe_divide(10, 0);
    std::cout << "  10/3 = " << r1.value_or(-1) << "\n";
    std::cout << "  10/0 = " << r2.value_or(-1) << " (无效)\n";

    // optional 链式操作
    std::optional<std::string> name = "Alice";
    auto len = name.has_value() ? std::optional(name->size()) : std::nullopt;
    std::cout << "  name长度: " << len.value_or(0) << "\n";

    // variant
    std::vector<JsonValue> json_array = {
        nullptr, true, 42, 3.14, "hello"s
    };

    std::cout << "  JSON 数组: [";
    for (size_t i = 0; i < json_array.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::visit(JsonPrinter{}, json_array[i]);
    }
    std::cout << "]\n";

    // Overloaded lambda 访问器
    JsonValue val = "modern C++"s;
    std::visit(Overloaded{
        [](auto arg) { std::cout << "  访问: " << arg << "\n"; },
        [](std::nullptr_t) { std::cout << "  访问: null\n"; }
    }, val);

    // any
    std::any a = 42;
    std::cout << "  any<int>: " << std::any_cast<int>(a) << "\n";
    a = "hello"s;
    std::cout << "  any<string>: " << std::any_cast<std::string>(a) << "\n";

    std::cout << R"(
  零开销分析:
    optional<T>:  sizeof = sizeof(T) + 1 (对齐后可能 + padding)
                  无堆分配，值直接内嵌
    variant<Ts>:  sizeof = max(sizeof(Ts)...) + 判别符 (通常 1-4 bytes)
                  无堆分配，无虚表
    any:          小对象优化 (SBO)，大对象可能堆分配
                  → 有运行时开销，慎用

  vs 传统方案:
    optional vs nullptr/magic value: 更安全，编译器检查
    variant vs union:  类型安全，自动析构
    variant vs 继承+虚函数: 无虚表，cache 友好，sizeof 更小
)";
}

} // namespace ch5


// =============================================================================
// 第6章：std::string_view — 零拷贝字符串引用
// =============================================================================

namespace ch6 {

// string_view 是一个轻量级的 {指针, 长度} 对, 不拥有内存
// sizeof(string_view) == 2 * sizeof(void*) = 16 bytes

// 零拷贝解析
std::vector<std::string_view> split_view(std::string_view sv, char delim) {
    std::vector<std::string_view> result;
    size_t start = 0;
    for (size_t i = 0; i <= sv.size(); ++i) {
        if (i == sv.size() || sv[i] == delim) {
            if (i > start)
                result.push_back(sv.substr(start, i - start));
            start = i + 1;
        }
    }
    return result;
}

// 编译期使用
constexpr bool starts_with_hello(std::string_view sv) {
    return sv.size() >= 5 && sv.substr(0, 5) == "hello";
}

void demo() {
    print_section("string_view — 零拷贝字符串引用");

    // 基本用法
    std::string_view sv = "Hello, World!";
    std::cout << "  sv = \"" << sv << "\", size=" << sv.size() << "\n";

    // 子串 (O(1), 不拷贝)
    auto sub = sv.substr(7, 5); // "World"
    std::cout << "  substr(7,5) = \"" << sub << "\"\n";

    // 零拷贝分割
    std::string data = "name:Alice:age:30:city:Beijing";
    auto parts = split_view(data, ':');
    std::cout << "  split_view 结果:";
    for (auto p : parts) std::cout << " [" << p << "]";
    std::cout << "\n";

    // constexpr 使用
    static_assert(starts_with_hello("hello world"));
    static_assert(!starts_with_hello("hi world"));
    std::cout << "  starts_with_hello(\"hello world\") = true ✓ (编译期)\n";

    // 性能对比
    std::string big_str(10000, 'A');
    constexpr int N = 100000;

    {
        Timer t;
        volatile size_t total = 0;
        for (int i = 0; i < N; ++i) {
            std::string sub_copy = big_str.substr(100, 500); // 拷贝
            total += sub_copy.size();
        }
        std::cout << "  string::substr ×" << N << ": "
                  << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << "ms\n";
    }
    {
        Timer t;
        std::string_view big_sv = big_str;
        volatile size_t total = 0;
        for (int i = 0; i < N; ++i) {
            auto sub_ref = big_sv.substr(100, 500); // 零拷贝
            total += sub_ref.size();
        }
        std::cout << "  string_view::substr ×" << N << ": "
                  << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << "ms\n";
    }

    std::cout << R"(
  ⚠️ string_view 注意事项:
    1. 不拥有内存 → 原始字符串被销毁后 sv 悬空
       ❌ string_view sv = get_temp_string(); // 悬空引用!
    2. 不保证 null-terminated → 不能传给 C API
       ✅ std::string s(sv); // 需要 null-terminated 时转换
    3. 用于函数参数(只读) → 最佳实践
       void process(string_view sv);  // 接受 string, char*, sv
)";
}

} // namespace ch6


// =============================================================================
// 第7章：内联变量与嵌套命名空间
// =============================================================================

namespace ch7 {

// C++17 inline 变量: 可以在头文件中定义变量，多个翻译单元不冲突
struct Config {
    static inline int max_connections = 100;      // C++17
    static inline const std::string version = "2.0"; // 头文件中定义
    static inline constexpr int buffer_size = 4096;  // constexpr 本身就是 inline
};

// 嵌套命名空间 (C++17)
// C++14: namespace A { namespace B { namespace C { ... } } }
// C++17:
namespace project::module::detail {
    int internal_func() { return 42; }
}

// C++17: 带属性的声明
// [[nodiscard]] 强制调用者使用返回值
[[nodiscard]] int compute_important_value() { return 42; }

// [[maybe_unused]] 抑制"未使用变量"警告
void demo() {
    print_section("内联变量与 C++17 小特性");

    std::cout << "  Config::max_connections = " << Config::max_connections << "\n";
    std::cout << "  Config::version = " << Config::version << "\n";
    std::cout << "  嵌套命名空间: " << project::module::detail::internal_func() << "\n";

    // [[maybe_unused]]
    [[maybe_unused]] int debug_counter = 0;

    // [[nodiscard]]
    auto val = compute_important_value(); // 必须使用返回值
    std::cout << "  [[nodiscard]] value = " << val << "\n";

    // init 语句用于 if/switch
    std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
    if (auto it = m.find("a"); it != m.end()) {
        std::cout << "  if-init: found " << it->first << "=" << it->second << "\n";
    }

    std::cout << R"(
  C++17 杂项:
    inline 变量:      解决头文件中全局变量的 ODR 问题
    嵌套命名空间:     语法糖
    [[nodiscard]]:    防止忽略重要返回值（错误码等）
    [[maybe_unused]]: 抑制编译器警告
    if-init:          限制变量作用域，减少名字污染
    std::byte:        类型安全的字节类型 (不是 char/unsigned char)
)";
}

} // namespace ch7


// =============================================================================
// 第8章：std::invoke / std::apply
// =============================================================================

namespace ch8 {

struct Printer {
    void print(int x) const { std::cout << "    Member: " << x << "\n"; }
    int value = 42;
};

void free_func(int x, int y) {
    std::cout << "    Free: " << x + y << "\n";
}

// 通用函数调用器
template<typename F, typename... Args>
auto call(F&& f, Args&&... args) {
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

void demo() {
    print_section("std::invoke / std::apply");

    Printer p;

    std::cout << "  std::invoke 统一调用:\n";
    // 1. 普通函数
    std::invoke(free_func, 10, 20);
    // 2. 成员函数
    std::invoke(&Printer::print, p, 100);
    // 3. Lambda
    std::invoke([](int x) { std::cout << "    Lambda: " << x << "\n"; }, 200);
    // 4. 成员变量
    int v = std::invoke(&Printer::value, p);
    std::cout << "    Member var: " << v << "\n";

    // std::apply — 将 tuple 展开为函数参数
    auto args = std::make_tuple(10, 20);
    std::cout << "  std::apply:\n";
    std::apply(free_func, args);

    // 结合 apply 实现通用的 tuple 打印
    auto tup = std::make_tuple(1, "hello"s, 3.14);
    std::apply([](const auto&... args) {
        std::cout << "    tuple: ";
        ((std::cout << args << " "), ...);
        std::cout << "\n";
    }, tup);

    std::cout << R"(
  invoke 统一了调用语法:
    普通函数:   invoke(f, args...)
    成员函数:   invoke(&C::f, obj, args...)
    成员变量:   invoke(&C::m, obj)
    函数对象:   invoke(functor, args...)
    Lambda:     invoke(lambda, args...)
  → 写泛型代码时不再需要 (obj.*f)(args...) 的特殊处理
)";
}

} // namespace ch8


// =============================================================================
// ██ 二、C++20 核心特性篇
// =============================================================================

// =============================================================================
// 第9章：Concepts — 约束模板参数
// =============================================================================

namespace ch9 {

// Concepts 用 C++17 模拟 (C++20 原生语法见注释)

// C++20 concept 语法:
// template<typename T>
// concept Addable = requires(T a, T b) { { a + b } -> std::same_as<T>; };
//
// template<Addable T>  // 直接约束
// T add(T a, T b) { return a + b; }
//
// auto add(auto a, auto b) requires Addable<decltype(a)>  // requires 子句

// C++17 模拟
template<typename T, typename = void>
struct is_addable : std::false_type {};

template<typename T>
struct is_addable<T, std::void_t<decltype(std::declval<T>() + std::declval<T>())>>
    : std::true_type {};

template<typename T>
constexpr bool is_addable_v = is_addable<T>::value;

// SFINAE 模拟 concept
template<typename T, std::enable_if_t<is_addable_v<T>, int> = 0>
T add_sfinae(T a, T b) { return a + b; }

// if constexpr 模拟 (更简洁)
template<typename T>
auto describe(const T& val) {
    if constexpr (std::is_integral_v<T>)
        return "整数: " + std::to_string(val);
    else if constexpr (std::is_floating_point_v<T>)
        return "浮点: " + std::to_string(val);
    else
        return "其他";
}

// has_xxx 检测模式
template<typename T, typename = void>
struct has_size : std::false_type {};

template<typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

template<typename T>
void print_info(const T& container) {
    if constexpr (has_size<T>::value) {
        std::cout << "    有 size(): " << container.size() << "\n";
    } else {
        std::cout << "    无 size()\n";
    }
}

void demo() {
    print_section("Concepts 约束 (C++17模拟 + C++20语法)");

    std::cout << "  is_addable<int>: " << is_addable_v<int> << "\n";
    std::cout << "  is_addable<string>: " << is_addable_v<std::string> << "\n";

    std::cout << "  add_sfinae(3,4) = " << add_sfinae(3, 4) << "\n";
    std::cout << "  " << describe(42) << "\n";
    std::cout << "  " << describe(3.14) << "\n";

    std::vector<int> v = {1, 2, 3};
    int arr[] = {1, 2, 3};
    std::cout << "  has_size:\n";
    print_info(v);

    std::cout << R"(
  C++20 Concepts 语法 (原生):
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

    // 使用方式 3: 缩写模板 (auto)
    void sort_it(Sortable auto& c);

  vs SFINAE:
    SFINAE:    error: no matching function (错误信息难读)
    Concepts:  error: constraint not satisfied (清晰指出哪个要求不满足)
    → Concepts 是 SFINAE 的上位替代
)";
}

} // namespace ch9


// =============================================================================
// 第10章：Ranges
// =============================================================================

namespace ch10 {

void demo() {
    print_section("Ranges — 惰性管道式算法 (C++20)");

    std::cout << R"(
  C++20 Ranges 是对 STL 算法的现代化重写：

  传统 STL:
    std::sort(v.begin(), v.end());
    std::copy_if(v.begin(), v.end(), out, pred);

  Ranges:
    std::ranges::sort(v);
    v | views::filter(pred) | views::transform(f) | views::take(5);

  关键组件:
  ┌───────────────────────────────────────────────────────────┐
  │ range        — 拥有 begin()/end() 的任何对象              │
  │ view         — 惰性求值、O(1) 拷贝的轻量级 range          │
  │ adaptor      — 将一个 range 转换为另一个 range(view)      │
  │ algorithm    — 接受 range 的算法                           │
  │ projection   — 算法中的投影函数                            │
  └───────────────────────────────────────────────────────────┘

  常用 views:
    views::filter(pred)     — 过滤
    views::transform(f)     — 变换
    views::take(n)          — 取前 n 个
    views::drop(n)          — 跳过前 n 个
    views::reverse          — 反转
    views::split(delim)     — 分割
    views::join             — 展平嵌套
    views::zip (C++23)      — 并行迭代多个 range
    views::enumerate (C++23)— 带索引迭代
    views::chunk (C++23)    — 分块
    views::stride (C++23)   — 步长迭代

  管道操作符 |:
    auto result = numbers
        | views::filter([](int n) { return n % 2 == 0; })
        | views::transform([](int n) { return n * n; })
        | views::take(5);
    // 惰性求值: 直到遍历 result 才真正计算

  零开销分析:
    views 是惰性的 → 不创建中间容器
    编译器内联 + 优化后 → 等同于手写循环
    实测: ranges 管道与手写循环性能差距 < 5%
)";

    // 用 C++17 模拟 ranges 的管道风格
    std::vector<int> nums = {1,2,3,4,5,6,7,8,9,10};

    // C++17 模拟: 过滤偶数 → 平方 → 取前3个
    std::vector<int> result;
    for (int n : nums) {
        if (n % 2 == 0) {
            result.push_back(n * n);
            if (result.size() >= 3) break; // 惰性语义
        }
    }

    std::cout << "  C++17 模拟: 偶数平方前3个 = ";
    for (int x : result) std::cout << x << " ";
    std::cout << "\n";

    std::cout << R"(
  C++20 写法:
    auto result = nums
        | views::filter([](int n) { return n % 2 == 0; })
        | views::transform([](int n) { return n * n; })
        | views::take(3);
    // result = {4, 16, 36}
)";
}

} // namespace ch10


// =============================================================================
// 第11章：三路比较 <=>
// =============================================================================

namespace ch11 {

// C++20 spaceship operator: 一个 <=> 自动生成 ==, !=, <, >, <=, >=
// C++17 模拟

struct Version {
    int major, minor, patch;

    // C++17: 手动实现所有比较运算符
    bool operator==(const Version& o) const {
        return std::tie(major, minor, patch) == std::tie(o.major, o.minor, o.patch);
    }
    bool operator!=(const Version& o) const { return !(*this == o); }
    bool operator<(const Version& o) const {
        return std::tie(major, minor, patch) < std::tie(o.major, o.minor, o.patch);
    }
    bool operator<=(const Version& o) const { return !(o < *this); }
    bool operator>(const Version& o) const { return o < *this; }
    bool operator>=(const Version& o) const { return !(*this < o); }

    // C++20: 只需一行
    // auto operator<=>(const Version&) const = default;
};

void demo() {
    print_section("三路比较 <=> (C++20)");

    Version v1{1, 2, 3}, v2{1, 3, 0}, v3{1, 2, 3};

    std::cout << "  v1 == v3: " << (v1 == v3) << "\n";
    std::cout << "  v1 < v2:  " << (v1 < v2) << "\n";
    std::cout << "  v1 > v2:  " << (v1 > v2) << "\n";

    std::cout << R"(
  C++20 三路比较:

    // 一行代替六个运算符
    auto operator<=>(const Version&) const = default;

    // 返回类型:
    //   strong_ordering:  a < b, a == b, a > b (整数比较)
    //   weak_ordering:    等价但不一定相等 (如大小写不敏感字符串)
    //   partial_ordering: 可能不可比较 (如 NaN)

    // 自定义:
    std::strong_ordering operator<=>(const Version& o) const {
        if (auto cmp = major <=> o.major; cmp != 0) return cmp;
        if (auto cmp = minor <=> o.minor; cmp != 0) return cmp;
        return patch <=> o.patch;
    }

  零开销: default <=> 编译后等同于手写的连续比较
)";
}

} // namespace ch11


// =============================================================================
// 第12章：consteval / constinit
// =============================================================================

namespace ch12 {

// constexpr: 可以在编译期或运行期求值
// consteval: 必须在编译期求值 (C++20) —— "立即函数"
// constinit: 变量必须在编译期初始化，但可在运行期修改 (C++20)

constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
}

// C++20: consteval
// consteval int must_compile_time(int n) {
//     return factorial(n);
// }
// int x = must_compile_time(10);  // ✅ 编译期
// int n = get_input();
// int y = must_compile_time(n);   // ❌ 编译错误! 参数非编译期常量

// C++20: constinit
// constinit int global_val = factorial(10);  // 编译期初始化，避免 SIOF
// global_val = 42;  // ✅ 运行时可修改

void demo() {
    print_section("consteval / constinit (C++20)");

    // constexpr 在编译期求值 (C++17 可用)
    constexpr int f10 = factorial(10);
    static_assert(f10 == 3628800);
    std::cout << "  constexpr factorial(10) = " << f10 << "\n";

    // 运行时也可以用
    int n = 5;
    int f5 = factorial(n); // 运行时求值
    std::cout << "  runtime factorial(5) = " << f5 << "\n";

    std::cout << R"(
  对比:
  ┌───────────┬──────────────────┬──────────────────┐
  │           │ 编译期可用?      │ 运行期可用?      │
  ├───────────┼──────────────────┼──────────────────┤
  │ constexpr │ ✅ (如果参数编译期)│ ✅               │
  │ consteval │ ✅ (强制)        │ ❌ (编译错误)    │
  │ constinit │ ✅ (初始化)      │ ✅ (修改/读取)   │
  │ const     │ ❌               │ ✅ (不可修改)    │
  └───────────┴──────────────────┴──────────────────┘

  consteval 用途:
    编译期查表、哈希、反射标记
    强制消除运行时开销

  constinit 用途:
    解决 SIOF (Static Initialization Order Fiasco)
    全局/线程局部变量保证在编译期初始化
    constinit thread_local int tls_val = 0;
)";
}

} // namespace ch12


// =============================================================================
// 第13章：协程 (Coroutines)
// =============================================================================

namespace ch13 {

void demo() {
    print_section("协程 (Coroutines) — C++20");

    std::cout << R"(
  C++20 协程是"可暂停、可恢复"的函数。
  通过 co_await / co_yield / co_return 关键字标识。

  ═══ 核心概念 ═══

  1. Promise Type — 控制协程行为
     struct promise_type {
         auto get_return_object();        // 返回协程句柄
         auto initial_suspend();          // 启动时是否暂停
         auto final_suspend() noexcept;   // 结束时是否暂停
         void return_void() / return_value(T); // co_return 语义
         void unhandled_exception();      // 异常处理
         auto yield_value(T);            // co_yield 语义
     };

  2. Coroutine Handle — 控制协程执行
     std::coroutine_handle<promise_type> handle;
     handle.resume();   // 恢复执行
     handle.done();     // 是否结束
     handle.destroy();  // 销毁

  3. Awaitable — co_await 的操作对象
     struct Awaitable {
         bool await_ready();              // true=不挂起, 直接继续
         void await_suspend(coroutine_handle<>); // 挂起时执行
         T await_resume();                // 恢复时返回值
     };

  ═══ Generator 示例 ═══

  Generator<int> fibonacci() {
      int a = 0, b = 1;
      while (true) {
          co_yield a;
          auto next = a + b;
          a = b;
          b = next;
      }
  }

  auto gen = fibonacci();
  for (int i = 0; i < 10; ++i) {
      gen.next();
      std::cout << gen.value() << " ";
  }
  // 输出: 0 1 1 2 3 5 8 13 21 34

  ═══ 异步 Task 示例 ═══

  Task<std::string> fetch_data(std::string url) {
      auto response = co_await async_http_get(url);
      auto json = co_await parse_json(response);
      co_return json["data"];
  }

  ═══ 零开销分析 ═══
  - 协程帧在堆上分配 (默认)，但编译器可以 HALO 优化掉
    (Heap Allocation eLision Optimization)
  - 无上下文切换的系统调用开销
  - 挂起/恢复 ≈ 几十纳秒 (vs 线程切换 ~微秒级)
  - 函数调用 < 协程 < 线程

  ═══ 协程 vs 其他方案 ═══
  ┌─────────────┬────────┬───────┬──────────┐
  │             │ 开销   │ 可扩展│ 编程模型 │
  ├─────────────┼────────┼───────┼──────────┤
  │ 回调        │ 零     │ 差    │ 复杂     │
  │ Future/Async│ 中     │ 中    │ 中       │
  │ 协程        │ 极低   │ 好    │ 直觉     │
  │ 线程        │ 高     │ 差    │ 直觉     │
  └─────────────┴────────┴───────┴──────────┘
)";
}

} // namespace ch13


// =============================================================================
// 第14章：std::format
// =============================================================================

namespace ch14 {

void demo() {
    print_section("std::format — 类型安全格式化 (C++20)");

    // C++20 std::format 模拟 (部分编译器已支持)
    // 这里用字符串流演示格式化概念

    std::cout << R"(
  C++20 std::format 语法:

  基本用法:
    std::format("Hello, {}!", "world")         → "Hello, world!"
    std::format("{} + {} = {}", 1, 2, 3)       → "1 + 2 = 3"

  位置参数:
    std::format("{1} {0}", "world", "hello")   → "hello world"

  格式说明符 {:spec}:
    {:d}    整数 (十进制)
    {:x}    十六进制
    {:o}    八进制
    {:b}    二进制
    {:f}    浮点 (fixed)
    {:e}    科学计数法
    {:>10}  右对齐, 宽度10
    {:<10}  左对齐
    {:^10}  居中
    {:*>10} 用 * 填充

  示例:
    std::format("{:08x}", 255)      → "000000ff"
    std::format("{:+.2f}", 3.14)    → "+3.14"
    std::format("{:>10}", "hi")     → "        hi"
    std::format("{:#b}", 42)        → "0b101010"

  vs printf / iostream:
    printf:   不类型安全, 格式串与参数不匹配 → UB
    iostream: 类型安全, 但格式化冗长 (setw/setprecision/...)
    format:   类型安全 + 简洁 + 编译期格式串检查

  C++23 std::print:
    std::print("Hello, {}!\n", "world");  // 直接输出，不返回 string
    std::println("value = {}", 42);       // 自动换行

  自定义类型格式化:
    template<>
    struct std::formatter<Point> {
        constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
        auto format(const Point& p, format_context& ctx) {
            return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
        }
    };

  性能: std::format 通常比 iostream 快 2-5x, 与 printf 相当
)";

    // C++17 模拟简单格式化
    auto fmt_hex = [](int val, int width) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(width) << val;
        return oss.str();
    };
    std::cout << "  C++17 模拟 hex: 0x" << fmt_hex(255, 4) << "\n";
}

} // namespace ch14


// =============================================================================
// 第15章：Modules
// =============================================================================

namespace ch15 {

void demo() {
    print_section("Modules — C++20 模块系统");

    std::cout << R"(
  Modules 替代 #include 的新机制:

  传统头文件问题:
    1. 编译慢: 每个 TU 都要重新解析头文件 (递归展开可达 MB 级别)
    2. 宏泄漏: #define 污染全局
    3. ODR 违反: 多重定义风险
    4. 顺序依赖: #include 顺序可能影响行为

  Module 语法:

    // math.cppm — 模块接口单元
    export module math;

    export int add(int a, int b) { return a + b; }

    export class Calculator {
    public:
        int compute(int a, int b);
    };

    // 非 export 的内容对导入者不可见
    int internal_helper() { return 42; }

    // main.cpp
    import math;  // 导入模块
    int main() {
        auto result = add(1, 2);       // ✅
        Calculator calc;               // ✅
        // internal_helper();          // ❌ 编译错误: 未导出
    }

  编译:
    g++ -std=c++20 -fmodules-ts -c math.cppm -o math.pcm
    g++ -std=c++20 -fmodules-ts main.cpp math.pcm -o main

  模块分区:
    // math_impl.cppm
    export module math:impl;    // math 的分区
    export int multiply(int a, int b) { return a * b; }

    // math.cppm
    export module math;
    export import :impl;        // 重新导出分区

  标准库模块 (C++23):
    import std;        // 导入整个标准库
    import std.compat; // 兼容 C 头文件

  性能提升:
    编译速度提升 2-10x (大型项目)
    模块预编译为二进制(.pcm/.ifc)，只需编译一次
)";
}

} // namespace ch15


// =============================================================================
// ██ 三、C++23 新特性篇
// =============================================================================

// =============================================================================
// 第16章：Deducing this
// =============================================================================

namespace ch16 {

void demo() {
    print_section("Deducing this — 显式对象参数 (C++23)");

    std::cout << R"(
  C++23 允许将 this 作为显式参数，支持推导其类型。

  传统方式(4个重载):
    class Widget {
        int& get() &       { return val_; }
        int& get() &&      { return val_; }
        const int& get() const &  { return val_; }
        const int& get() const && { return val_; }
    };

  C++23 deducing this(1个模板):
    class Widget {
        template<typename Self>
        auto& get(this Self&& self) {
            return std::forward<Self>(self).val_;
        }
    };

  应用场景:

  1. 消除 const/non-const 重载
     class Buffer {
         std::vector<char> data_;
     public:
         auto&& data(this auto&& self) {  // 一个函数搞定
             return std::forward<decltype(self)>(self).data_;
         }
     };

  2. 递归 Lambda (无需 Y-combinator)
     auto fib = [](this auto self, int n) -> int {
         return n < 2 ? n : self(n-1) + self(n-2);
     };
     fib(10); // 55

  3. CRTP 简化
     // 传统 CRTP:
     template<typename Derived>
     struct Base {
         void interface() { static_cast<Derived*>(this)->impl(); }
     };

     // C++23:
     struct Base {
         void interface(this auto& self) { self.impl(); }
     };

  4. Builder 模式
     class Builder {
         int width_, height_;
     public:
         auto&& set_width(this auto&& self, int w) {
             self.width_ = w;
             return std::forward<decltype(self)>(self);
         }
     };

  零开销: deducing this 在编译期解析，无虚表，与手写重载等价
)";
}

} // namespace ch16


// =============================================================================
// 第17章：std::expected
// =============================================================================

namespace ch17 {

// C++23 std::expected — 可以携带错误信息的返回值
// C++17 模拟

template<typename T, typename E>
class Expected {
    std::variant<T, E> data_;
    bool has_value_;
public:
    Expected(const T& val) : data_(val), has_value_(true) {}
    Expected(const E& err) : data_(err), has_value_(false) {}

    bool has_value() const { return has_value_; }
    explicit operator bool() const { return has_value_; }

    T& value() { return std::get<T>(data_); }
    const T& value() const { return std::get<T>(data_); }
    E& error() { return std::get<E>(data_); }
    const E& error() const { return std::get<E>(data_); }

    T value_or(const T& default_val) const {
        return has_value_ ? std::get<T>(data_) : default_val;
    }

    // 一元操作 (C++23 monadic interface)
    template<typename F>
    auto and_then(F&& f) const -> decltype(f(value())) {
        if (has_value_) return f(value());
        return decltype(f(value())){error()};
    }

    template<typename F>
    auto transform(F&& f) const -> Expected<decltype(f(value())), E> {
        if (has_value_) return f(value());
        return error();
    }
};

enum class ParseError { EmptyInput, InvalidFormat, Overflow };

std::string error_to_string(ParseError e) {
    switch (e) {
        case ParseError::EmptyInput:    return "空输入";
        case ParseError::InvalidFormat: return "格式错误";
        case ParseError::Overflow:      return "溢出";
    }
    return "未知";
}

Expected<int, ParseError> parse_int(std::string_view sv) {
    if (sv.empty()) return ParseError::EmptyInput;
    int result = 0;
    for (char c : sv) {
        if (c < '0' || c > '9') return ParseError::InvalidFormat;
        result = result * 10 + (c - '0');
        if (result > 99999) return ParseError::Overflow;
    }
    return result;
}

void demo() {
    print_section("std::expected — 错误处理新范式 (C++23)");

    auto r1 = parse_int("12345");
    auto r2 = parse_int("abc");
    auto r3 = parse_int("");

    std::cout << "  parse_int(\"12345\"): "
              << (r1 ? std::to_string(r1.value()) : error_to_string(r1.error())) << "\n";
    std::cout << "  parse_int(\"abc\"): "
              << (r2 ? std::to_string(r2.value()) : error_to_string(r2.error())) << "\n";
    std::cout << "  parse_int(\"\"): "
              << (r3 ? std::to_string(r3.value()) : error_to_string(r3.error())) << "\n";

    // 链式操作 (monadic)
    auto doubled = r1.transform([](int v) { return v * 2; });
    std::cout << "  transform(*2): " << doubled.value() << "\n";

    std::cout << R"(
  expected vs 其他错误处理:
  ┌──────────────┬──────────┬──────────┬──────────┬─────────┐
  │              │ 类型安全 │ 零开销   │ 信息     │ 可组合  │
  ├──────────────┼──────────┼──────────┼──────────┼─────────┤
  │ 返回错误码   │ ❌       │ ✅       │ 少       │ ❌      │
  │ 异常         │ ✅       │ ❌(抛出) │ 丰富     │ ❌      │
  │ optional     │ ✅       │ ✅       │ ❌(无)   │ ✅      │
  │ expected     │ ✅       │ ✅       │ ✅       │ ✅      │
  └──────────────┴──────────┴──────────┴──────────┴─────────┘

  C++23 monadic 接口:
    expected.and_then(f)  — 成功时链式调用 (flatMap)
    expected.transform(f) — 成功时转换值 (map)
    expected.or_else(f)   — 失败时恢复

    auto result = read_file(path)
        .and_then(parse_json)
        .transform(extract_value)
        .or_else(handle_error);
)";
}

} // namespace ch17


// =============================================================================
// 第18-20章：C++23 其他新特性
// =============================================================================

namespace ch18_20 {

void demo() {
    print_section("C++23 其他新特性");

    std::cout << R"(
  ═══ std::print / std::println ═══
    std::print("Hello {}!\n", "world");    // 直接输出
    std::println("value = {}", 42);        // 自动换行
    // 基于 std::format, 比 cout 快 2-5x

  ═══ if consteval ═══
    constexpr double my_sqrt(double x) {
        if consteval {
            // 编译期: 使用 constexpr 算法
            return constexpr_sqrt(x);
        } else {
            // 运行期: 使用硬件指令 (std::sqrt)
            return std::sqrt(x);
        }
    }

  ═══ static operator() ═══
    struct Add {
        static int operator()(int a, int b) { return a + b; }
    };
    // 无 this 指针 → 可以转换为普通函数指针
    // 无捕获 lambda 在 C++23 中也有 static operator()

  ═══ 多维下标 operator[] ═══
    class Matrix {
        double& operator[](size_t row, size_t col) {
            return data_[row * cols_ + col];
        }
    };
    mat[2, 3] = 1.0;  // 直接使用，不再需要 mat[2][3] 代理

  ═══ std::mdspan (多维数组视图) ═══
    std::vector<double> data(100);
    std::mdspan mat(data.data(), 10, 10);  // 10×10 矩阵视图
    mat[2, 3] = 42.0;  // 零拷贝, 零开销

  ═══ 其他 C++23 特性 ═══
    std::flat_map / flat_set  — 基于排序 vector 的有序容器 (cache 友好)
    std::stacktrace           — 运行时调用栈追踪
    std::generator            — 标准库协程 generator
    ranges::to<Container>()   — 将 range 物化为容器
    auto(x) / auto{x}        — 显式衰退拷贝 (decay copy)
    [[assume(expr)]]          — 编译器假设优化暗示
)";
}

} // namespace ch18_20


// =============================================================================
// ██ 四、零开销抽象篇
// =============================================================================

// =============================================================================
// 第21章：CRTP — 静态多态
// =============================================================================

namespace ch21 {

// CRTP = Curiously Recurring Template Pattern
// 用模板实现多态，编译期绑定，无虚表开销

template<typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
    void describe() const {
        std::cout << "    面积 = " << area() << "\n";
    }
};

class Circle : public Shape<Circle> {
    double radius_;
public:
    Circle(double r) : radius_(r) {}
    double area_impl() const { return 3.14159 * radius_ * radius_; }
};

class Rectangle : public Shape<Rectangle> {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area_impl() const { return w_ * h_; }
};

// CRTP mixin: 自动添加功能
template<typename Derived>
class Comparable {
public:
    bool operator!=(const Derived& o) const {
        return !(static_cast<const Derived&>(*this) == o);
    }
    bool operator>(const Derived& o) const {
        return o < static_cast<const Derived&>(*this);
    }
    bool operator<=(const Derived& o) const { return !(o < static_cast<const Derived&>(*this)); }
    bool operator>=(const Derived& o) const { return !(static_cast<const Derived&>(*this) < o); }
};

class Score : public Comparable<Score> {
    int val_;
public:
    Score(int v) : val_(v) {}
    bool operator==(const Score& o) const { return val_ == o.val_; }
    bool operator<(const Score& o) const { return val_ < o.val_; }
    int get() const { return val_; }
};

// CRTP 计数器：统计某类型实例数量
template<typename Derived>
class InstanceCounter {
    static inline int count_ = 0;
protected:
    InstanceCounter() { ++count_; }
    ~InstanceCounter() { --count_; }
    InstanceCounter(const InstanceCounter&) { ++count_; }
public:
    static int count() { return count_; }
};

class Widget : public InstanceCounter<Widget> {
    int id_;
public:
    Widget(int id) : id_(id) {}
};

void demo() {
    print_section("CRTP — 静态多态");

    Circle c(5.0);
    Rectangle r(3.0, 4.0);
    std::cout << "  Circle(5):    ";  c.describe();
    std::cout << "  Rectangle(3,4): "; r.describe();

    // Comparable mixin
    Score s1(85), s2(90), s3(85);
    std::cout << "  Score: 85==85? " << (s1 == s3)
              << ", 85<90? " << (s1 < s2)
              << ", 85>90? " << (s1 > s2) << "\n";

    // InstanceCounter
    {
        Widget w1(1), w2(2);
        std::cout << "  Widget 实例数: " << Widget::count() << "\n";
    }
    std::cout << "  Widget 实例数(销毁后): " << Widget::count() << "\n";

    std::cout << R"(
  CRTP vs 虚函数:
    虚函数:  运行时分发, 有虚表指针 (8 bytes), 间接调用 (cache miss)
    CRTP:    编译期分发, 无虚表, 可完全内联

    sizeof(虚函数类): sizeof(数据) + 8 (vptr)
    sizeof(CRTP类):   sizeof(数据)

    性能差距: CRTP 快 2-10x (取决于函数复杂度和内联程度)
)";
}

} // namespace ch21


// =============================================================================
// 第22章：Policy-Based Design
// =============================================================================

namespace ch22 {

// 策略模式: 通过模板参数组合行为，编译期绑定

// --- 日志策略 ---
struct ConsoleLogger {
    static void log(std::string_view msg) {
        std::cout << "[LOG] " << msg << "\n";
    }
};
struct NullLogger {
    static void log(std::string_view) {} // 编译后完全消除
};

// --- 线程策略 ---
struct SingleThread {
    struct Lock {
        Lock() = default;
    };
};
struct MultiThread {
    struct Lock {
        // 简化: 实际应使用 std::mutex
        Lock() { /* lock */ }
        ~Lock() { /* unlock */ }
    };
};

// --- 分配策略 ---
template<typename T>
struct HeapAllocator {
    static T* allocate(size_t n) { return new T[n]; }
    static void deallocate(T* p) { delete[] p; }
};
template<typename T>
struct PoolAllocator {
    static T* allocate(size_t n) { return new T[n]; /* 简化 */ }
    static void deallocate(T* p) { delete[] p; }
};

// --- 组合策略形成完整类 ---
template<
    typename T,
    typename LogPolicy = NullLogger,
    typename ThreadPolicy = SingleThread,
    template<typename> class AllocPolicy = HeapAllocator
>
class SmartBuffer {
    T* data_;
    size_t size_;
    size_t capacity_;

public:
    explicit SmartBuffer(size_t cap = 16) : size_(0), capacity_(cap) {
        data_ = AllocPolicy<T>::allocate(capacity_);
        LogPolicy::log("SmartBuffer 创建");
    }

    ~SmartBuffer() {
        AllocPolicy<T>::deallocate(data_);
        LogPolicy::log("SmartBuffer 销毁");
    }

    void push_back(const T& val) {
        typename ThreadPolicy::Lock lock;
        if (size_ >= capacity_) {
            LogPolicy::log("需要扩容");
            // 简化：不实现扩容
            return;
        }
        data_[size_++] = val;
    }

    size_t size() const { return size_; }
};

void demo() {
    print_section("Policy-Based Design — 策略组合");

    // 高性能版：无日志、单线程
    SmartBuffer<int> fast_buf(100);
    for (int i = 0; i < 10; ++i) fast_buf.push_back(i);
    std::cout << "  fast_buf size: " << fast_buf.size() << " (无日志, 单线程)\n";

    // 调试版：有日志
    SmartBuffer<int, ConsoleLogger> debug_buf(100);
    debug_buf.push_back(42);
    std::cout << "  debug_buf size: " << debug_buf.size() << "\n";

    // 多线程版
    SmartBuffer<int, NullLogger, MultiThread> mt_buf(100);
    mt_buf.push_back(1);

    std::cout << R"(
  核心思想:
    用模板参数注入策略 → 编译期确定行为 → 零运行时开销

    NullLogger::log() 是空函数 → 内联后完全消除
    SingleThread::Lock 构造/析构空 → 内联后完全消除
    → 高性能版本与手写裸代码生成完全相同的机器码!

  std::allocator_traits 就是策略模式的标准库实现。
  Alexandrescu《Modern C++ Design》的核心思想。
)";
}

} // namespace ch22


// =============================================================================
// 第23章：Strong Types
// =============================================================================

namespace ch23 {

// 强类型包装: 防止隐式转换错误

template<typename T, typename Tag>
class StrongType {
    T value_;
public:
    constexpr explicit StrongType(T val) : value_(std::move(val)) {}
    constexpr T& get() { return value_; }
    constexpr const T& get() const { return value_; }

    // 可选：启用算术运算
    constexpr StrongType operator+(const StrongType& o) const {
        return StrongType(value_ + o.value_);
    }
    constexpr bool operator==(const StrongType& o) const { return value_ == o.value_; }
    constexpr bool operator<(const StrongType& o) const { return value_ < o.value_; }
};

// 定义不同的"单位"类型
using Meters = StrongType<double, struct MetersTag>;
using Seconds = StrongType<double, struct SecondsTag>;
using Kilograms = StrongType<double, struct KilogramsTag>;
using UserId = StrongType<int, struct UserIdTag>;
using OrderId = StrongType<int, struct OrderIdTag>;

// 类型安全的函数
double compute_speed(Meters distance, Seconds time) {
    return distance.get() / time.get();
}

void demo() {
    print_section("Strong Types — 类型安全的零开销包装");

    Meters dist(100.0);
    Seconds time(9.58);
    // Meters wrong = Seconds(5.0); // ❌ 编译错误!
    // compute_speed(time, dist);   // ❌ 编译错误! 参数顺序

    double speed = compute_speed(dist, time);
    std::cout << "  速度 = " << speed << " m/s\n";

    UserId uid(1001);
    OrderId oid(2001);
    // if (uid == oid) {}  // ❌ 编译错误! 类型不同

    std::cout << "  UserId: " << uid.get() << "\n";
    std::cout << "  OrderId: " << oid.get() << "\n";

    // 编译期验证
    constexpr Meters m1(10.0), m2(20.0);
    constexpr auto m3 = m1 + m2;
    static_assert(m3.get() == 30.0);
    std::cout << "  constexpr Meters: " << m3.get() << "\n";

    std::cout << R"(
  零开销验证:
    sizeof(Meters) == sizeof(double) == 8 bytes
    StrongType<T, Tag> 在优化后完全等同于裸 T
    → 类型安全是编译期的, 运行时零开销

  实际应用:
    void transfer(UserId from, UserId to, Amount amount);
    // 不可能把 UserId 和 Amount 搞混!
    // 不可能把 from 和 to 搞反 (如果分别强类型化)

  库: NamedType (https://github.com/joboccara/NamedType)
)";
}

} // namespace ch23


// =============================================================================
// 第24章：Type Erasure
// =============================================================================

namespace ch24 {

// Type Erasure: 值语义的多态 (无继承要求)
// std::function 就是最经典的 type erasure 实现

// 手写 type erasure: 可调用任何有 draw() 方法的对象
class Drawable {
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw(std::ostream&) const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template<typename T>
    struct Model : Concept {
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

    Drawable(const Drawable& o) : pimpl_(o.pimpl_->clone()) {}
    Drawable(Drawable&&) = default;
    Drawable& operator=(Drawable o) { std::swap(pimpl_, o.pimpl_); return *this; }

    void draw(std::ostream& os) const { pimpl_->draw(os); }
};

// 无需继承, 只要有 draw() 方法
struct CircleShape {
    double radius;
    void draw(std::ostream& os) const { os << "○ r=" << radius; }
};

struct SquareShape {
    double side;
    void draw(std::ostream& os) const { os << "□ s=" << side; }
};

struct TextShape {
    std::string text;
    void draw(std::ostream& os) const { os << "T \"" << text << "\""; }
};

void demo() {
    print_section("Type Erasure — 值语义多态");

    // 不同类型的对象放入同一容器 (值语义!)
    std::vector<Drawable> shapes;
    shapes.emplace_back(CircleShape{5.0});
    shapes.emplace_back(SquareShape{3.0});
    shapes.emplace_back(TextShape{"Hello"});

    std::cout << "  所有形状:\n";
    for (const auto& s : shapes) {
        std::cout << "    ";
        s.draw(std::cout);
        std::cout << "\n";
    }

    // 拷贝语义
    auto shapes2 = shapes; // 深拷贝
    std::cout << "  拷贝后仍可用:\n    ";
    shapes2[0].draw(std::cout);
    std::cout << "\n";

    std::cout << R"(
  Type Erasure vs 继承:
  ┌──────────────┬──────────────────┬──────────────────┐
  │              │ 继承 + 虚函数    │ Type Erasure     │
  ├──────────────┼──────────────────┼──────────────────┤
  │ 侵入性       │ 必须继承基类     │ 无侵入          │
  │ 值/引用      │ 引用语义(指针)   │ 值语义          │
  │ 拷贝         │ 需要 clone       │ 自然拷贝        │
  │ 容器存储     │ unique_ptr       │ 直接存值        │
  │ 性能         │ 虚函数调用       │ 虚函数调用+堆   │
  │ 新类型要求   │ 继承基类         │ 只需有对应方法  │
  └──────────────┴──────────────────┴──────────────────┘

  标准库中的 Type Erasure:
    std::function<R(Args...)>  — 可调用对象
    std::any                    — 任意类型
    std::move_only_function    (C++23)
)";
}

} // namespace ch24


// =============================================================================
// 第25章：variant 访问者 vs 虚函数
// =============================================================================

namespace ch25 {

// 用 variant + visit 替代继承层次

// --- 传统虚函数方式 ---
struct ShapeBase {
    virtual ~ShapeBase() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};
struct VCircle : ShapeBase {
    double r;
    VCircle(double r) : r(r) {}
    double area() const override { return 3.14159 * r * r; }
    std::string name() const override { return "Circle"; }
};
struct VRect : ShapeBase {
    double w, h;
    VRect(double w, double h) : w(w), h(h) {}
    double area() const override { return w * h; }
    std::string name() const override { return "Rect"; }
};
struct VTri : ShapeBase {
    double b, h;
    VTri(double b, double h) : b(b), h(h) {}
    double area() const override { return 0.5 * b * h; }
    std::string name() const override { return "Triangle"; }
};

// --- variant 方式 ---
struct SCircle { double r; };
struct SRect   { double w, h; };
struct STri    { double b, h; };

using Shape = std::variant<SCircle, SRect, STri>;

struct AreaVisitor {
    double operator()(const SCircle& c) const { return 3.14159 * c.r * c.r; }
    double operator()(const SRect& r) const { return r.w * r.h; }
    double operator()(const STri& t) const { return 0.5 * t.b * t.h; }
};

void demo() {
    print_section("variant 访问者 vs 虚函数");

    // 虚函数方式
    std::vector<std::unique_ptr<ShapeBase>> v_shapes;
    v_shapes.push_back(std::make_unique<VCircle>(5.0));
    v_shapes.push_back(std::make_unique<VRect>(3.0, 4.0));
    v_shapes.push_back(std::make_unique<VTri>(6.0, 3.0));

    // variant 方式
    std::vector<Shape> s_shapes = {SCircle{5.0}, SRect{3.0, 4.0}, STri{6.0, 3.0}};

    std::cout << "  虚函数:\n";
    for (const auto& s : v_shapes)
        std::cout << "    " << s->name() << " → " << s->area() << "\n";

    std::cout << "  variant:\n";
    for (const auto& s : s_shapes)
        std::cout << "    area = " << std::visit(AreaVisitor{}, s) << "\n";

    // 性能对比
    constexpr int N = 1000000;
    {
        Timer t;
        volatile double total = 0;
        for (int i = 0; i < N; ++i)
            for (const auto& s : v_shapes) total += s->area();
        std::cout << "  虚函数 ×" << N << ": " << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << "ms\n";
    }
    {
        Timer t;
        volatile double total = 0;
        for (int i = 0; i < N; ++i)
            for (const auto& s : s_shapes) total += std::visit(AreaVisitor{}, s);
        std::cout << "  variant  ×" << N << ": " << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << "ms\n";
    }

    std::cout << R"(
  对比分析:
    虚函数: 堆分配 + 指针间接访问 → cache 不友好
    variant: 内联存储, 连续内存 → cache 友好, 通常更快

    sizeof(unique_ptr<ShapeBase>) = 8 bytes (指针)
      对象本身在堆上, 分散
    sizeof(Shape) = max(sizeof(SCircle), sizeof(SRect), sizeof(STri)) + 判别符
      对象连续排列, cache 友好

  选择指南:
    类型集合固定 → variant (编译期确定, 添加新类型需重编译)
    类型集合开放 → 虚函数/type erasure (运行时可扩展)
)";
}

} // namespace ch25


// =============================================================================
// ██ 五、编译期计算篇
// =============================================================================

// =============================================================================
// 第26章：constexpr 全面深入
// =============================================================================

namespace ch26 {

// C++14: constexpr 函数可以有循环、变量、if
// C++17: constexpr lambda, if constexpr
// C++20: constexpr new/delete, try-catch, virtual, std::vector
// C++23: constexpr 几乎无限制 (static, goto 除外)

// 编译期 sqrt (牛顿法)
constexpr double constexpr_sqrt(double x) {
    if (x < 0) return -1;
    if (x == 0) return 0;
    double guess = x;
    for (int i = 0; i < 100; ++i) {
        double next = 0.5 * (guess + x / guess);
        if (guess == next) break; // 收敛
        guess = next;
    }
    return guess;
}

// 编译期字符串哈希 (FNV-1a)
constexpr uint32_t fnv1a_hash(std::string_view sv) {
    uint32_t hash = 2166136261u;
    for (char c : sv) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

// 编译期排序
template<typename T, size_t N>
constexpr std::array<T, N> constexpr_sort(std::array<T, N> arr) {
    // 插入排序 (constexpr 友好)
    for (size_t i = 1; i < N; ++i) {
        T key = arr[i];
        size_t j = i;
        while (j > 0 && arr[j-1] > key) {
            arr[j] = arr[j-1];
            --j;
        }
        arr[j] = key;
    }
    return arr;
}

// 编译期查找表
constexpr auto make_sin_table() {
    std::array<double, 360> table{};
    for (int i = 0; i < 360; ++i) {
        double rad = i * 3.14159265358979 / 180.0;
        // constexpr sin (泰勒展开)
        double x = rad;
        double result = 0;
        double term = x;
        for (int n = 1; n <= 15; ++n) {
            result += term;
            term *= -x * x / ((2*n) * (2*n + 1));
        }
        table[i] = result;
    }
    return table;
}

void demo() {
    print_section("constexpr 全面深入");

    // 编译期 sqrt
    constexpr double sq = constexpr_sqrt(2.0);
    static_assert(sq > 1.414 && sq < 1.415);
    std::cout << "  constexpr sqrt(2) = " << std::setprecision(10) << sq << "\n";

    // 编译期哈希
    constexpr uint32_t h1 = fnv1a_hash("hello");
    constexpr uint32_t h2 = fnv1a_hash("world");
    static_assert(h1 != h2);
    std::cout << "  constexpr hash(\"hello\") = 0x" << std::hex << h1 << std::dec << "\n";

    // 编译期排序
    constexpr auto sorted = constexpr_sort(std::array{5, 3, 1, 4, 2});
    static_assert(sorted[0] == 1 && sorted[4] == 5);
    std::cout << "  constexpr sort: ";
    for (int v : sorted) std::cout << v << " ";
    std::cout << "\n";

    // 编译期查找表
    constexpr auto sin_table = make_sin_table();
    static_assert(sin_table[0] < 0.001);   // sin(0°) ≈ 0
    static_assert(sin_table[90] > 0.999);  // sin(90°) ≈ 1
    std::cout << "  sin(30°) = " << std::setprecision(6) << sin_table[30]
              << " (编译期查找表)\n";
    std::cout << "  sin(45°) = " << sin_table[45] << "\n";

    // 编译期 switch-case (哈希字符串)
    auto handle_command = [](std::string_view cmd) {
        switch (fnv1a_hash(cmd)) {
            case fnv1a_hash("start"):  return "启动";
            case fnv1a_hash("stop"):   return "停止";
            case fnv1a_hash("status"): return "查询";
            default:                    return "未知";
        }
    };
    std::cout << "  command(\"start\") → " << handle_command("start") << "\n";
    std::cout << "  command(\"stop\")  → " << handle_command("stop") << "\n";

    std::cout << R"(
  constexpr 演进:
    C++11: 单 return 语句
    C++14: 循环、变量、多语句
    C++17: if constexpr, constexpr lambda
    C++20: new/delete, virtual, dynamic_cast, std::vector/string
    C++23: 几乎无限制

  编译期计算的优势:
    1. 运行时零开销 (结果嵌入二进制)
    2. 编译期错误检测 (static_assert)
    3. 查找表不占运行时初始化时间
)";
}

} // namespace ch26


// =============================================================================
// 第27章：编译期容器与算法
// =============================================================================

namespace ch27 {

// 编译期 std::array 作为"容器"
template<typename T, size_t N>
constexpr bool constexpr_contains(const std::array<T, N>& arr, const T& val) {
    for (const auto& v : arr) {
        if (v == val) return true;
    }
    return false;
}

template<typename T, size_t N>
constexpr size_t constexpr_count_if(const std::array<T, N>& arr, T threshold) {
    size_t count = 0;
    for (const auto& v : arr) {
        if (v > threshold) ++count;
    }
    return count;
}

// 编译期 map (排序数组 + 二分查找)
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
        // 编译期排序
        for (size_t i = 1; i < N; ++i) {
            auto key = data_[i];
            size_t j = i;
            while (j > 0 && data_[j-1].first > key.first) {
                data_[j] = data_[j-1]; --j;
            }
            data_[j] = key;
        }
    }

    constexpr const V* find(const K& key) const {
        size_t idx = lower_bound(key);
        if (idx < N && data_[idx].first == key)
            return &data_[idx].second;
        return nullptr;
    }

    constexpr const V& at(const K& key) const {
        const V* p = find(key);
        // constexpr 中不能 throw，但可以触发编译错误
        return *p;
    }
};

void demo() {
    print_section("编译期容器与算法");

    constexpr std::array<int, 5> arr = {10, 30, 20, 50, 40};
    static_assert(constexpr_contains(arr, 30));
    static_assert(!constexpr_contains(arr, 99));
    static_assert(constexpr_count_if(arr, 25) == 3); // 30, 50, 40

    std::cout << "  contains(30) = true ✓ (编译期)\n";
    std::cout << "  count_if(>25) = 3 ✓ (编译期)\n";

    // 编译期 map
    constexpr ConstexprMap http_status(std::array{
        std::pair{200, "OK"},
        std::pair{404, "Not Found"},
        std::pair{500, "Internal Error"},
        std::pair{301, "Moved"},
        std::pair{403, "Forbidden"}
    });

    constexpr auto s200 = http_status.at(200);
    constexpr auto s404 = http_status.at(404);
    static_assert(std::string_view(s200) == "OK");

    std::cout << "  http_status[200] = " << s200 << " (编译期)\n";
    std::cout << "  http_status[404] = " << s404 << " (编译期)\n";

    std::cout << R"(
  编译期容器技巧:
    std::array → 编译期 "vector" (固定大小)
    排序 array + 二分 → 编译期 "map"
    constexpr 函数 → 编译期 "算法"

  C++20 可以用 constexpr std::vector 和 std::string
  但它们不能作为 constexpr 变量 (不能跨编译期-运行期边界)
  → std::array 仍是最常用的编译期容器
)";
}

} // namespace ch27


// =============================================================================
// 第28章：模板元编程 — 类型列表
// =============================================================================

namespace ch28 {

// 类型列表 (Type List)
template<typename... Ts>
struct TypeList {};

// 获取长度
template<typename TL> struct Size;
template<typename... Ts>
struct Size<TypeList<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};

// 获取第 N 个类型
template<size_t N, typename TL> struct TypeAt;
template<typename Head, typename... Tail>
struct TypeAt<0, TypeList<Head, Tail...>> { using type = Head; };
template<size_t N, typename Head, typename... Tail>
struct TypeAt<N, TypeList<Head, Tail...>> : TypeAt<N-1, TypeList<Tail...>> {};

template<size_t N, typename TL>
using TypeAt_t = typename TypeAt<N, TL>::type;

// 查找类型索引
template<typename T, typename TL> struct IndexOf;
template<typename T, typename... Ts>
struct IndexOf<T, TypeList<T, Ts...>> : std::integral_constant<size_t, 0> {};
template<typename T, typename Head, typename... Tail>
struct IndexOf<T, TypeList<Head, Tail...>>
    : std::integral_constant<size_t, 1 + IndexOf<T, TypeList<Tail...>>::value> {};

// 追加类型
template<typename TL, typename T> struct PushBack;
template<typename... Ts, typename T>
struct PushBack<TypeList<Ts...>, T> { using type = TypeList<Ts..., T>; };

// 前置类型
template<typename T, typename TL> struct PushFront;
template<typename T, typename... Ts>
struct PushFront<T, TypeList<Ts...>> { using type = TypeList<T, Ts...>; };

// 检查是否包含
template<typename T, typename TL> struct Contains;
template<typename T>
struct Contains<T, TypeList<>> : std::false_type {};
template<typename T, typename... Ts>
struct Contains<T, TypeList<T, Ts...>> : std::true_type {};
template<typename T, typename Head, typename... Tail>
struct Contains<T, TypeList<Head, Tail...>> : Contains<T, TypeList<Tail...>> {};

// 对每个类型应用函数
template<typename TL, template<typename> class F> struct Transform;
template<template<typename> class F, typename... Ts>
struct Transform<TypeList<Ts...>, F> {
    using type = TypeList<typename F<Ts>::type...>;
};

void demo() {
    print_section("模板元编程 — 类型列表");

    using MyTypes = TypeList<int, double, std::string, float>;

    // 编译期类型操作
    static_assert(Size<MyTypes>::value == 4);
    static_assert(std::is_same_v<TypeAt_t<0, MyTypes>, int>);
    static_assert(std::is_same_v<TypeAt_t<2, MyTypes>, std::string>);
    static_assert(IndexOf<double, MyTypes>::value == 1);
    static_assert(Contains<int, MyTypes>::value);
    static_assert(!Contains<char, MyTypes>::value);

    std::cout << "  TypeList<int, double, string, float>\n";
    std::cout << "    Size = " << Size<MyTypes>::value << "\n";
    std::cout << "    TypeAt<0> = int ✓\n";
    std::cout << "    IndexOf<double> = " << IndexOf<double, MyTypes>::value << "\n";
    std::cout << "    Contains<int> = true ✓\n";
    std::cout << "    Contains<char> = false ✓\n";

    // Transform: 对所有类型加 const
    using ConstTypes = typename Transform<TypeList<int, double>, std::add_const>::type;
    static_assert(std::is_same_v<TypeAt_t<0, ConstTypes>, const int>);
    std::cout << "    Transform<add_const>: TypeAt<0> = const int ✓\n";

    std::cout << R"(
  类型列表是编译期"容器":
    TypeList → 编译期 vector<type>
    Size     → .size()
    TypeAt   → operator[]
    Contains → find
    Transform→ transform/map

  应用: 序列化框架、反射系统、消息分发、变体类型生成
)";
}

} // namespace ch28


// =============================================================================
// 第29章：编译期字符串
// =============================================================================

namespace ch29 {

// 编译期固定长度字符串
template<size_t N>
struct FixedString {
    char data_[N] = {};
    constexpr FixedString() = default;
    constexpr FixedString(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i) data_[i] = str[i];
    }
    constexpr char operator[](size_t i) const { return data_[i]; }
    constexpr size_t size() const { return N - 1; } // 不含 null
    constexpr std::string_view view() const { return {data_, N - 1}; }

    constexpr bool operator==(const FixedString& o) const {
        for (size_t i = 0; i < N; ++i)
            if (data_[i] != o.data_[i]) return false;
        return true;
    }
};

// CTAD
template<size_t N>
FixedString(const char (&)[N]) -> FixedString<N>;

// 编译期字符串拼接
template<size_t N1, size_t N2>
constexpr auto concat(const FixedString<N1>& a, const FixedString<N2>& b) {
    FixedString<N1 + N2 - 1> result;
    for (size_t i = 0; i < N1 - 1; ++i) result.data_[i] = a[i];
    for (size_t i = 0; i < N2; ++i)     result.data_[N1 - 1 + i] = b[i];
    return result;
}

// 用作非类型模板参数 (C++20 NTTP)
// template<FixedString Name>
// struct NamedTag {
//     static constexpr auto name = Name;
// };
// using MyTag = NamedTag<"event_click">;

void demo() {
    print_section("编译期字符串处理");

    constexpr FixedString hello("Hello");
    constexpr FixedString world(" World!");
    constexpr auto greeting = concat(hello, world);

    static_assert(greeting.view() == "Hello World!");
    static_assert(greeting.size() == 12);

    std::cout << "  constexpr concat: \"" << greeting.view() << "\"\n";
    std::cout << "  size = " << greeting.size() << "\n";

    // 编译期大小写转换
    constexpr auto to_upper = [](auto str) constexpr {
        auto result = str;
        for (size_t i = 0; i < str.size(); ++i) {
            if (result.data_[i] >= 'a' && result.data_[i] <= 'z')
                result.data_[i] -= 32;
        }
        return result;
    };

    constexpr auto upper = to_upper(hello);
    static_assert(upper.view() == "HELLO");
    std::cout << "  constexpr to_upper: \"" << upper.view() << "\"\n";

    std::cout << R"(
  编译期字符串用途:
    1. 非类型模板参数 (C++20 NTTP)
       template<FixedString Name> struct Field;
       Field<"username"> user;

    2. 编译期 JSON/SQL 解析
    3. 编译期正则表达式 (如 CTRE 库)
    4. 编译期格式串检查 (std::format)
)";
}

} // namespace ch29


// =============================================================================
// 第30章：编译期状态机
// =============================================================================

namespace ch30 {

// 编译期状态机 — 在编译期验证状态转换的合法性

enum class State { Idle, Running, Paused, Stopped };
enum class Event { Start, Pause, Resume, Stop };

// 编译期状态转换表
constexpr std::optional<State> transition(State s, Event e) {
    // 返回 nullopt 表示非法转换
    if (s == State::Idle && e == Event::Start) return State::Running;
    if (s == State::Running && e == Event::Pause) return State::Paused;
    if (s == State::Running && e == Event::Stop) return State::Stopped;
    if (s == State::Paused && e == Event::Resume) return State::Running;
    if (s == State::Paused && e == Event::Stop) return State::Stopped;
    return std::nullopt; // 非法转换
}

constexpr const char* state_name(State s) {
    switch (s) {
        case State::Idle:    return "Idle";
        case State::Running: return "Running";
        case State::Paused:  return "Paused";
        case State::Stopped: return "Stopped";
    }
    return "?";
}

// 编译期验证状态序列
template<Event... events>
constexpr State run_state_machine() {
    State current = State::Idle;
    bool valid = true;
    // C++17 折叠表达式
    ((valid = valid && [&] {
        auto next = transition(current, events);
        if (next) { current = *next; return true; }
        return false;
    }()), ...);
    return valid ? current : State::Idle; // Idle 代表错误
}

void demo() {
    print_section("编译期状态机");

    // 编译期验证合法序列
    constexpr auto final1 = run_state_machine<
        Event::Start, Event::Pause, Event::Resume, Event::Stop>();
    static_assert(final1 == State::Stopped);

    std::cout << "  Start → Pause → Resume → Stop = "
              << state_name(final1) << " ✓\n";

    // 运行时使用（同一份代码）
    State s = State::Idle;
    Event events[] = {Event::Start, Event::Pause, Event::Resume, Event::Stop};
    std::cout << "  运行时状态序列: ";
    for (auto e : events) {
        auto next = transition(s, e);
        if (next) {
            s = *next;
            std::cout << state_name(s) << " → ";
        } else {
            std::cout << "(非法!) ";
        }
    }
    std::cout << "完成\n";

    std::cout << R"(
  编译期状态机优势:
    1. 非法状态转换在编译期就报错
    2. 同一份代码可在编译期和运行期使用
    3. 编译器可以将已知序列完全内联 (零开销)

  应用场景: 协议解析器、UI 状态管理、游戏 AI、硬件控制
)";
}

} // namespace ch30


// =============================================================================
// ██ 六、高级模板技巧篇
// =============================================================================

// =============================================================================
// 第31章：SFINAE → Concepts 演进
// =============================================================================

namespace ch31 {

// --- SFINAE (C++11) ---
// Substitution Failure Is Not An Error

// 检测是否有 .begin() 方法
template<typename T, typename = void>
struct is_iterable : std::false_type {};

template<typename T>
struct is_iterable<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())
>> : std::true_type {};

// --- enable_if (C++11) ---
template<typename T>
std::enable_if_t<std::is_integral_v<T>, std::string>
stringify_sfinae(T val) {
    return "int:" + std::to_string(val);
}

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, std::string>
stringify_sfinae(T val) {
    return "float:" + std::to_string(val);
}

// --- if constexpr (C++17, 更好) ---
template<typename T>
std::string stringify_constexpr(T val) {
    if constexpr (std::is_integral_v<T>)
        return "int:" + std::to_string(val);
    else if constexpr (std::is_floating_point_v<T>)
        return "float:" + std::to_string(val);
    else
        return "other";
}

void demo() {
    print_section("SFINAE → if constexpr → Concepts");

    static_assert(is_iterable<std::vector<int>>::value);
    static_assert(!is_iterable<int>::value);

    std::cout << "  is_iterable<vector>: true ✓\n";
    std::cout << "  is_iterable<int>:    false ✓\n";

    std::cout << "  SFINAE:      " << stringify_sfinae(42) << "\n";
    std::cout << "  if constexpr:" << stringify_constexpr(3.14) << "\n";

    std::cout << R"(
  演进路线:
    C++11 SFINAE + enable_if   → 难写、难读、难调试
    C++14 void_t / is_detected → 稍好, 但仍复杂
    C++17 if constexpr         → 简单, 但不能用于重载决议
    C++20 Concepts             → 最终解决方案

  C++20 等价代码:
    template<typename T>
    concept Iterable = requires(T t) {
        { t.begin() } -> std::input_or_output_iterator;
        { t.end() }   -> std::input_or_output_iterator;
    };

    void process(Iterable auto& container) { ... }

  →  错误信息从 "substitution failure in ..." 变为
     "constraint 'Iterable' not satisfied because ..."
)";
}

} // namespace ch31


// =============================================================================
// 第32章：变参模板高级模式
// =============================================================================

namespace ch32 {

// --- 索引序列 (index_sequence) ---
template<typename Tuple, size_t... Is>
void print_tuple_impl(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template<typename... Ts>
void print_tuple(const std::tuple<Ts...>& t) {
    std::cout << "(";
    print_tuple_impl(t, std::index_sequence_for<Ts...>{});
    std::cout << ")";
}

// --- 编译期 for_each ---
template<typename Tuple, typename F, size_t... Is>
void for_each_impl(Tuple& t, F&& f, std::index_sequence<Is...>) {
    (f(std::get<Is>(t)), ...);
}

template<typename... Ts, typename F>
void for_each_tuple(std::tuple<Ts...>& t, F&& f) {
    for_each_impl(t, std::forward<F>(f), std::index_sequence_for<Ts...>{});
}

// --- 变参类模板 ---
template<typename... Bases>
struct MultiInherit : Bases... {
    using Bases::operator()...;  // C++17: 继承所有 operator()
};
template<typename... Bases>
MultiInherit(Bases...) -> MultiInherit<Bases...>;

void demo() {
    print_section("变参模板高级模式");

    // 打印 tuple
    auto t = std::make_tuple(1, "hello", 3.14, 'A');
    std::cout << "  print_tuple: ";
    print_tuple(t);
    std::cout << "\n";

    // for_each_tuple
    auto t2 = std::make_tuple(10, 20.0, "world"s);
    std::cout << "  for_each_tuple (*2 for numbers):\n";
    for_each_tuple(t2, [](auto& val) {
        if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
            val *= 2;
        }
    });
    std::cout << "    ";
    print_tuple(t2);
    std::cout << "\n";

    // Overloaded (多继承变参模板)
    auto visitor = MultiInherit{
        [](int i) { std::cout << "    int: " << i << "\n"; },
        [](double d) { std::cout << "    double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "    string: " << s << "\n"; }
    };

    std::variant<int, double, std::string> v = 42;
    std::visit(visitor, v);
    v = 3.14;
    std::visit(visitor, v);
    v = "hello"s;
    std::visit(visitor, v);
}

} // namespace ch32


// =============================================================================
// 第33章：完美转发
// =============================================================================

namespace ch33 {

// 万能引用 (Universal Reference / Forwarding Reference)
// T&& 在模板中是万能引用，不是右值引用

void process(int& x)        { std::cout << "    左值引用: " << x << "\n"; }
void process(const int& x)  { std::cout << "    const左值: " << x << "\n"; }
void process(int&& x)       { std::cout << "    右值引用: " << x << "\n"; }

template<typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg)); // 完美转发
}

// 完美转发构造 (emplace 的实现原理)
template<typename T>
class Container {
    std::vector<T> data_;
public:
    template<typename... Args>
    T& emplace_back(Args&&... args) {
        data_.emplace_back(std::forward<Args>(args)...);
        return data_.back();
    }
};

// 避免万能引用的陷阱
struct Widget33 {
    std::string name_;

    // ❌ 陷阱: 这个构造函数会"吞掉"拷贝构造函数
    // template<typename T>
    // Widget33(T&& name) : name_(std::forward<T>(name)) {}

    // ✅ 正确: 使用 enable_if 排除自身
    template<typename T,
             std::enable_if_t<!std::is_same_v<std::decay_t<T>, Widget33>, int> = 0>
    Widget33(T&& name) : name_(std::forward<T>(name)) {}

    Widget33(const Widget33&) = default;
    Widget33(Widget33&&) = default;
};

void demo() {
    print_section("完美转发与万能引用");

    int x = 42;
    const int cx = 42;

    std::cout << "  wrapper(x): ";      wrapper(x);       // T = int& → 左值
    std::cout << "  wrapper(cx): ";     wrapper(cx);      // T = const int& → const左值
    std::cout << "  wrapper(42): ";     wrapper(42);      // T = int → 右值
    std::cout << "  wrapper(move(x)):"; wrapper(std::move(x)); // T = int → 右值

    std::cout << R"(
  引用折叠规则:
    T& &   → T&   (左值+左值 = 左值)
    T& &&  → T&   (左值+右值 = 左值)
    T&& &  → T&   (右值+左值 = 左值)
    T&& && → T&&  (右值+右值 = 右值)

  std::forward<T>(arg):
    T = X&  → forward 返回 X& (左值)
    T = X   → forward 返回 X&& (右值)

  陷阱:
    1. 万能引用只在模板函数中: template<typename T> void f(T&& x)
       auto&& x 也是万能引用 (auto 类似模板)
       Widget&& x 不是万能引用! 这是普通右值引用
    2. 万能引用构造函数会抢匹配 → 用 enable_if/concepts 排除
)";
}

} // namespace ch33


// =============================================================================
// 第34章：Lambda 高级用法
// =============================================================================

namespace ch34 {

void demo() {
    print_section("Lambda 高级用法 (C++14→20→23)");

    // C++14: 泛型 lambda
    auto add = [](auto a, auto b) { return a + b; };
    std::cout << "  泛型: add(1,2)=" << add(1,2) << " add(1.5,2.5)=" << add(1.5,2.5) << "\n";

    // C++14: init capture (移动捕获)
    auto p = std::make_unique<int>(42);
    auto f = [ptr = std::move(p)]() { return *ptr; };
    std::cout << "  移动捕获: " << f() << "\n";

    // C++17: constexpr lambda
    constexpr auto sq = [](int x) { return x * x; };
    static_assert(sq(5) == 25);
    std::cout << "  constexpr lambda: sq(5)=" << sq(5) << "\n";

    // 立即调用 lambda (IIFE)
    const auto config = [&] {
        // 复杂初始化逻辑
        std::map<std::string, int> cfg;
        cfg["timeout"] = 5000;
        cfg["retries"] = 3;
        return cfg;
    }();
    std::cout << "  IIFE: timeout=" << config.at("timeout") << "\n";

    // Lambda 用于模板参数
    std::vector<int> v = {5, 3, 1, 4, 2};
    std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
    std::cout << "  排序: ";
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";

    // Lambda 递归 (Y-combinator)
    auto fib = [](auto self, int n) -> int {
        return n < 2 ? n : self(self, n-1) + self(self, n-2);
    };
    std::cout << "  fib(10) = " << fib(fib, 10) << "\n";

    // 高阶函数
    auto make_adder = [](int n) {
        return [n](int x) { return x + n; };
    };
    auto add10 = make_adder(10);
    std::cout << "  make_adder(10)(5) = " << add10(5) << "\n";

    // Lambda overload set
    auto handler = Overloaded{
        [](int i) -> std::string { return "int:" + std::to_string(i); },
        [](double d) -> std::string { return "double:" + std::to_string(d); },
        [](const std::string& s) -> std::string { return "str:" + s; }
    };
    std::cout << "  overloaded: " << handler(42) << ", " << handler(3.14) << "\n";

    std::cout << R"(
  Lambda 演进:
    C++11: 基础 lambda, 值/引用捕获
    C++14: 泛型 lambda (auto), init capture
    C++17: constexpr lambda, *this 拷贝捕获
    C++20: 模板 lambda []<typename T>(T x){}, 无捕获用于 constexpr
    C++23: deducing this, static operator()

  C++20 模板 lambda:
    auto f = []<typename T>(std::vector<T>& v) { v.push_back(T{}); };
    // 可以在 lambda 中使用 T

  C++23 static lambda:
    auto f = [](int x) static { return x * 2; };
    // 可转换为普通函数指针, 无捕获开销
)";
}

// 5. variant 的 Overloaded 需要引用前面定义的
using ch5::Overloaded;

} // namespace ch34


// =============================================================================
// 第35章：性能对比与最佳实践
// =============================================================================

namespace ch35 {

void demo() {
    print_section("性能对比与最佳实践总结");

    // --- 虚函数 vs CRTP vs variant ---
    std::cout << "  抽象机制对比:\n";
    std::cout << R"(
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
)";

    std::cout << R"(
  ═══ 最佳实践总结 ═══

  1. 编译期优先:
     能 constexpr → constexpr
     能 if constexpr → if constexpr (而非 SFINAE)
     能编译期 → 不要运行时

  2. 值语义优先:
     能 variant → 不用 inheritance (封闭类型集)
     能 optional → 不用 nullptr
     能 expected → 不用 exception (性能敏感路径)

  3. 零拷贝优先:
     能 string_view → 不用 const string&
     能 span<T> → 不用 vector<T>&
     能 move → 不用 copy

  4. 类型安全优先:
     能 Strong Type → 不用 raw int/double
     能 enum class → 不用 int 常量
     能 concepts → 不用裸模板

  5. 模板注意事项:
     if constexpr > enable_if > tag dispatch > SFINAE
     Concepts (C++20) > 以上所有
     控制模板实例化数量 → 避免代码膨胀

  6. 内存布局:
     连续内存 (vector/array) > 链式结构 (list/map)
     SoA (struct of arrays) > AoS (array of structs) (SIMD 场景)
     避免 false sharing → alignas(64)

  ═══ 编译器探索工具 ═══
  Compiler Explorer: https://godbolt.org
  C++ Insights:      https://cppinsights.io
  Quick Bench:       https://quick-bench.com
)";
}

} // namespace ch35


// =============================================================================
// main
// =============================================================================

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "================================================================\n";
    std::cout << " 精通现代 C++ — 零开销抽象与编译期优化 完全教程\n";
    std::cout << "================================================================\n";

    // 一、C++17 核心
    print_header("一、C++17 核心特性篇");
    ch1::demo();
    ch2::demo();
    ch3::demo();
    ch4::demo();
    ch5::demo();
    ch6::demo();
    ch7::demo();
    ch8::demo();

    // 二、C++20 核心
    print_header("二、C++20 核心特性篇");
    ch9::demo();
    ch10::demo();
    ch11::demo();
    ch12::demo();
    ch13::demo();
    ch14::demo();
    ch15::demo();

    // 三、C++23 新特性
    print_header("三、C++23 新特性篇");
    ch16::demo();
    ch17::demo();
    ch18_20::demo();

    // 四、零开销抽象
    print_header("四、零开销抽象篇");
    ch21::demo();
    ch22::demo();
    ch23::demo();
    ch24::demo();
    ch25::demo();

    // 五、编译期计算
    print_header("五、编译期计算篇");
    ch26::demo();
    ch27::demo();
    ch28::demo();
    ch29::demo();
    ch30::demo();

    // 六、高级模板技巧
    print_header("六、高级模板技巧篇");
    ch31::demo();
    ch32::demo();
    ch33::demo();
    ch34::demo();
    ch35::demo();

    std::cout << "\n================================================================\n";
    std::cout << " 演示完成\n";
    std::cout << "================================================================\n";

    return 0;
}
