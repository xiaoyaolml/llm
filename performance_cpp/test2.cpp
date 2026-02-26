// =============================================================================
// C++ 编译期优化 进阶教程与代码示例 (第二部分)
// =============================================================================
// 本教程是 test1.cpp 基础教程的延续，聚焦更高级的编译期技术。
//
// 本教程覆盖：
//   1.  Concepts (C++20 概念约束)
//   2.  编译期反射与类型名获取
//   3.  编译期正则/模式匹配
//   4.  std::tuple 编译期操作
//   5.  编译期排序
//   6.  SFINAE 与 enable_if 深入
//   7.  编译期表达式模板 (Expression Templates)
//   8.  编译期位操作与位域
//   9.  constexpr 内存分配 (C++20)
//  10.  Policy-Based Design (策略模式的编译期实现)
//  11.  Tag Dispatch (标签分派)
//  12.  编译期依赖注入与工厂
// =============================================================================

#include <iostream>
#include <array>
#include <type_traits>
#include <string_view>
#include <tuple>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <numeric>
#include <concepts>
#include <vector>
#include <memory>
#include <functional>
#include <cmath>

// =============================================================================
// 第1章：Concepts — C++20 概念约束
// =============================================================================
// Concepts 是 C++20 最重要的特性之一，它为模板参数提供了
// 声明式的约束语义，替代了晦涩的 SFINAE。
// 优点：
//   - 错误信息清晰可读
//   - 代码意图一目了然
//   - 支持组合与重载决议

// --- 1.1 基础概念定义 ---

// 定义一个概念：类型 T 必须支持 +、- 运算和比较
template <typename T>
concept Arithmetic = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    { a < b } -> std::convertible_to<bool>;
};

// 定义一个概念：类型 T 必须是可调用的，返回 R
template <typename F, typename R, typename... Args>
concept CallableReturning = requires(F f, Args... args) {
    { f(args...) } -> std::convertible_to<R>;
};

// --- 1.2 使用概念约束函数模板 ---

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
    return a / b; // 调用者负责 b != 0
}

static_assert(safe_add(3, 4) == 7, "concept add");
static_assert(safe_multiply(3, 5) == 15, "concept multiply");

// --- 1.3 组合概念 ---

template <typename T>
concept Printable = requires(std::ostream& os, T val) {
    { os << val } -> std::same_as<std::ostream&>;
};

template <typename T>
concept ArithmeticAndPrintable = Arithmetic<T> && Printable<T>;

template <ArithmeticAndPrintable T>
void compute_and_print(T a, T b) {
    std::cout << "  " << a << " + " << b << " = " << (a + b) << "\n";
    std::cout << "  " << a << " * " << b << " = " << (a * b) << "\n";
}

// --- 1.4 概念用于重载决议 ---

// 更特化的概念优先匹配
template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <Integral T>
constexpr T smart_abs(T x) { return x < 0 ? -x : x; }

template <FloatingPoint T>
constexpr T smart_abs(T x) { return x < 0.0 ? -x : x; }

static_assert(smart_abs(-5) == 5, "integral abs");
static_assert(smart_abs(-3.14) == 3.14, "floating abs");

// --- 1.5 概念与容器 ---

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
    std::cout << "  [";
    bool first = true;
    for (const auto& elem : c) {
        if (!first) std::cout << ", ";
        std::cout << elem;
        first = false;
    }
    std::cout << "]\n";
}


// =============================================================================
// 第2章：编译期反射与类型名获取
// =============================================================================
// C++ 目前没有正式的反射机制，但可以利用编译器特性在编译期获取类型名。

// --- 2.1 利用 __PRETTY_FUNCTION__ / __FUNCSIG__ 获取类型名 ---

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

// 编译期获取类型名（编译器相关，但主流编译器都支持）
// 调用方式: type_name<int>() → "int"

// --- 2.2 编译期类型ID（哈希） ---
template <typename T>
constexpr uint32_t type_id() {
    // 利用类型名生成唯一 hash
    constexpr std::string_view name = type_name<T>();
    uint32_t hash = 2166136261u;
    for (char c : name) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

// 不同类型生成不同的 ID（编译期完成）
// static_assert(type_id<int>() != type_id<double>(), "different type ids");


// =============================================================================
// 第3章：编译期模式匹配
// =============================================================================
// 虽然 C++ 还没有原生的模式匹配（P2688R0 提案中），
// 但可以用模板技术实现类似功能。

// --- 3.1 编译期 Variant 访问（模拟 match） ---

// 重载集合工具
template <typename... Fs>
struct overloaded : Fs... {
    using Fs::operator()...;
};

// C++17 CTAD 推导指引
template <typename... Fs>
overloaded(Fs...) -> overloaded<Fs...>;

// 使用示例：
void pattern_match_demo() {
    auto matcher = overloaded{
        [](int i)    { std::cout << "  int: " << i << "\n"; },
        [](double d) { std::cout << "  double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "  string: " << s << "\n"; },
        [](auto x)   { std::cout << "  other: " << x << "\n"; }
    };
    
    matcher(42);
    matcher(3.14);
    matcher(std::string("hello"));
    matcher('A');
}

// --- 3.2 编译期递归模式匹配 ---
// 对类型列表进行 "模式匹配"

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
template <typename T> struct match_type<std::vector<T>> {
    static constexpr const char* name = "vector";
};

static_assert(std::string_view(match_type<int>::name) == "integer",
              "match int");
static_assert(std::string_view(match_type<std::vector<int>>::name) == "vector",
              "match vector");


// =============================================================================
// 第4章：std::tuple 编译期操作
// =============================================================================
// tuple 是异构容器，其所有操作都发生在编译期。

// --- 4.1 编译期遍历 tuple ---

template <typename Tuple, typename Func, size_t... Is>
void tuple_for_each_impl(const Tuple& t, Func&& f, std::index_sequence<Is...>) {
    (f(std::get<Is>(t)), ...);
}

template <typename Tuple, typename Func>
void tuple_for_each(const Tuple& t, Func&& f) {
    tuple_for_each_impl(t, std::forward<Func>(f),
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

// --- 4.2 编译期 tuple 变换 (map) ---

template <typename Tuple, typename Func, size_t... Is>
auto tuple_transform_impl(const Tuple& t, Func&& f, std::index_sequence<Is...>) {
    return std::make_tuple(f(std::get<Is>(t))...);
}

template <typename Tuple, typename Func>
auto tuple_transform(const Tuple& t, Func&& f) {
    return tuple_transform_impl(t, std::forward<Func>(f),
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

// --- 4.3 编译期 tuple 过滤 ---

// 过滤出满足条件的元素
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

// --- 4.4 编译期 tuple 拼接与反转 ---

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

// --- 4.5 编译期 tuple 查找 ---

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


// =============================================================================
// 第5章：编译期排序
// =============================================================================
// 在编译期对 constexpr 数组进行排序 — 运行期访问已排序的数据。

// --- 5.1 编译期冒泡排序 ---
template <typename T, size_t N>
constexpr std::array<T, N> ct_bubble_sort(std::array<T, N> arr) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                // constexpr swap
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

// --- 5.2 编译期快速排序 ---
template <typename T, size_t N>
constexpr std::array<T, N> ct_quick_sort(std::array<T, N> arr) {
    // 使用非递归方式（避免编译期递归深度问题）
    // 简化版本：使用 constexpr 兼容的 partition
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

    // 使用栈模拟递归
    int stack[64] = {};
    int top = -1;

    stack[++top] = 0;
    stack[++top] = static_cast<int>(N) - 1;

    while (top >= 0) {
        int high = stack[top--];
        int low  = stack[top--];

        int pi = partition(arr, low, high);

        if (pi - 1 > low) {
            stack[++top] = low;
            stack[++top] = pi - 1;
        }
        if (pi + 1 < high) {
            stack[++top] = pi + 1;
            stack[++top] = high;
        }
    }
    return arr;
}

constexpr std::array<int, 10> unsorted2 = {39, 27, 43, 3, 9, 82, 10, 55, 1, 99};
constexpr auto qsorted = ct_quick_sort(unsorted2);
static_assert(qsorted[0] == 1,  "qsort min");
static_assert(qsorted[9] == 99, "qsort max");

// --- 5.3 编译期二分查找 ---
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

static_assert(ct_binary_search(sorted_arr, 25) == 3, "find 25");
static_assert(ct_binary_search(sorted_arr, 100) == -1, "100 not found");


// =============================================================================
// 第6章：SFINAE 与 enable_if 深入
// =============================================================================
// SFINAE (Substitution Failure Is Not An Error) —— 替换失败不是错误。
// 这是 C++20 Concepts 出现之前控制重载决议的核心技术，
// 理解它对于阅读旧代码至关重要。

// --- 6.1 基础 SFINAE: enable_if ---

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

// --- 6.2 检测成员函数是否存在 (经典 SFINAE 技巧) ---

// 检测类型 T 是否有 .serialize() 方法
template <typename T, typename = void>
struct has_serialize : std::false_type {};

template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>>
    : std::true_type {};

template <typename T>
constexpr bool has_serialize_v = has_serialize<T>::value;

// 测试类
struct Serializable {
    std::string serialize() const { return "data"; }
};
struct NotSerializable {
    int value = 42;
};

static_assert(has_serialize_v<Serializable>, "has serialize");
static_assert(!has_serialize_v<NotSerializable>, "no serialize");
static_assert(!has_serialize_v<int>, "int has no serialize");

// --- 6.3 SFINAE vs Concepts 对比 ---

// 旧方式 (SFINAE):
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr T old_style_abs(T x) { return x < 0 ? -x : x; }

// 新方式 (Concepts) —— 更清晰:
// template <std::integral T>
// constexpr T new_style_abs(T x) { return x < 0 ? -x : x; }

// --- 6.4 检测运算符是否存在 ---

template <typename T, typename U, typename = void>
struct has_addition : std::false_type {};

template <typename T, typename U>
struct has_addition<T, U, std::void_t<
    decltype(std::declval<T>() + std::declval<U>())
>> : std::true_type {};

static_assert(has_addition<int, double>::value, "int + double exists");
static_assert(!has_addition<std::string, int>::value, "string + int doesn't exist");


// =============================================================================
// 第7章：表达式模板 (Expression Templates)
// =============================================================================
// 表达式模板是一种编译期优化技术，避免中间临时对象的创建。
// 典型用途：线性代数库 (Eigen, Blaze 等)。

// --- 7.1 问题：朴素向量加法的性能问题 ---
// a + b + c 会产生两个临时对象，分别存储 a+b 和 (a+b)+c
// 表达式模板可以将整个表达式延迟到赋值时一次性求值。

// --- 7.2 表达式模板实现 ---

// 前向声明
template <typename E>
class VecExpr;

// 实际存储数据的向量
class SmallVec {
    std::array<double, 4> data_{};
    size_t size_ = 0;
public:
    SmallVec() = default;
    SmallVec(std::initializer_list<double> init) : size_(init.size()) {
        size_t i = 0;
        for (double v : init) {
            if (i < 4) data_[i++] = v;
        }
    }

    // 从表达式构造（延迟求值发生在此处）
    template <typename E>
    SmallVec(const VecExpr<E>& expr) : size_(expr.size()) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = expr[i];
        }
    }

    double operator[](size_t i) const { return data_[i]; }
    double& operator[](size_t i) { return data_[i]; }
    size_t size() const { return size_; }

    void print(const char* label) const {
        std::cout << "  " << label << " = [";
        for (size_t i = 0; i < size_; ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data_[i];
        }
        std::cout << "]\n";
    }
};

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

// 表达式节点：加法
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

// SmallVec 也要继承 VecExpr
// 注意：为了简化，我们让 SmallVec 独立，通过包装器桥接

class VecRef : public VecExpr<VecRef> {
    const SmallVec& vec_;
public:
    VecRef(const SmallVec& v) : vec_(v) {}
    double operator[](size_t i) const { return vec_[i]; }
    size_t size() const { return vec_.size(); }
};

VecRef wrap(const SmallVec& v) { return VecRef(v); }

// 使用方式:
// SmallVec a = {1, 2, 3, 4};
// SmallVec b = {5, 6, 7, 8};
// SmallVec c = wrap(a) + wrap(b);  // 无中间临时对象！
// SmallVec d = (wrap(a) + wrap(b)) * 2.0;


// =============================================================================
// 第8章：编译期位操作与位域
// =============================================================================
// 位操作在嵌入式/系统编程中极为重要，
// 许多位运算可以在编译期完成。

// --- 8.1 编译期位计数 (popcount) ---
constexpr int ct_popcount(uint32_t x) {
    int count = 0;
    while (x) {
        count += (x & 1);
        x >>= 1;
    }
    return count;
}

static_assert(ct_popcount(0b10110101) == 5, "popcount");
static_assert(ct_popcount(0) == 0, "popcount 0");
static_assert(ct_popcount(0xFFFFFFFF) == 32, "popcount all ones");

// --- 8.2 编译期前导零计数 (clz) ---
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

// --- 8.3 编译期 log2 ---
constexpr int ct_log2(uint32_t x) {
    return 31 - ct_clz(x);
}

static_assert(ct_log2(1) == 0, "log2(1)");
static_assert(ct_log2(8) == 3, "log2(8)");
static_assert(ct_log2(1024) == 10, "log2(1024)");

// --- 8.4 编译期判断2的幂 ---
constexpr bool ct_is_power_of_two(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

static_assert(ct_is_power_of_two(1), "1 is 2^0");
static_assert(ct_is_power_of_two(256), "256 is 2^8");
static_assert(!ct_is_power_of_two(100), "100 is not 2^n");

// --- 8.5 编译期向上取整到2的幂 ---
constexpr uint32_t ct_next_power_of_two(uint32_t x) {
    if (x == 0) return 1;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

static_assert(ct_next_power_of_two(5) == 8, "next pow2 of 5");
static_assert(ct_next_power_of_two(16) == 16, "16 is already pow2");
static_assert(ct_next_power_of_two(100) == 128, "next pow2 of 100");

// --- 8.6 编译期位反转 ---
constexpr uint32_t ct_reverse_bits(uint32_t x) {
    uint32_t result = 0;
    for (int i = 0; i < 32; ++i) {
        result = (result << 1) | (x & 1);
        x >>= 1;
    }
    return result;
}

static_assert(ct_reverse_bits(0x80000000) == 0x00000001, "reverse bits");

// --- 8.7 编译期设置/清除/翻转位的类型安全封装 ---
template <typename EnumT>
    requires std::is_enum_v<EnumT>
class Bitflags {
    using UT = std::underlying_type_t<EnumT>;
    UT flags_ = 0;
public:
    constexpr Bitflags() = default;
    constexpr Bitflags(EnumT e) : flags_(static_cast<UT>(e)) {}

    constexpr Bitflags& set(EnumT e) {
        flags_ |= static_cast<UT>(e);
        return *this;
    }
    constexpr Bitflags& clear(EnumT e) {
        flags_ &= ~static_cast<UT>(e);
        return *this;
    }
    constexpr Bitflags& toggle(EnumT e) {
        flags_ ^= static_cast<UT>(e);
        return *this;
    }
    constexpr bool test(EnumT e) const {
        return (flags_ & static_cast<UT>(e)) != 0;
    }
    constexpr UT raw() const { return flags_; }
};

// 示例用法
enum class Permission : uint8_t {
    Read    = 1 << 0,
    Write   = 1 << 1,
    Execute = 1 << 2,
};

constexpr auto perms = Bitflags<Permission>{}
    .set(Permission::Read)
    .set(Permission::Write);

static_assert(perms.test(Permission::Read), "has read");
static_assert(perms.test(Permission::Write), "has write");
static_assert(!perms.test(Permission::Execute), "no execute");


// =============================================================================
// 第9章：constexpr 动态内存分配 (C++20)
// =============================================================================
// C++20 允许在 constexpr 函数中使用 new/delete！
// 但有一个限制：编译期分配的内存必须在编译期释放（不能"泄漏"到运行期）。

// --- 9.1 编译期 vector 式结构 ---

// 简化的编译期动态数组
template <typename T>
class ConstexprVec {
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t cap_ = 0;

public:
    constexpr ConstexprVec() = default;

    constexpr ~ConstexprVec() {
        delete[] data_;
    }

    constexpr void push_back(const T& val) {
        if (size_ >= cap_) {
            size_t new_cap = cap_ == 0 ? 4 : cap_ * 2;
            T* new_data = new T[new_cap];
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
            cap_ = new_cap;
        }
        data_[size_++] = val;
    }

    constexpr size_t size() const { return size_; }
    constexpr T operator[](size_t i) const { return data_[i]; }

    constexpr T back() const { return data_[size_ - 1]; }
};

// 编译期使用动态数组！
constexpr int test_constexpr_vec() {
    ConstexprVec<int> v;
    for (int i = 0; i < 10; ++i) {
        v.push_back(i * i);
    }
    // 内存在 constexpr 上下文结束时释放
    return static_cast<int>(v.size()) + v[5]; // size=10, v[5]=25 → 35
}

static_assert(test_constexpr_vec() == 35, "constexpr dynamic alloc");

// --- 9.2 编译期链表 ---
constexpr int test_constexpr_linked_list() {
    struct Node {
        int value;
        Node* next = nullptr;
    };

    Node* head = nullptr;
    // 编译期构建链表 1->2->3->4->5
    for (int i = 5; i >= 1; --i) {
        Node* n = new Node{i, head};
        head = n;
    }

    // 编译期求和
    int sum = 0;
    Node* curr = head;
    while (curr) {
        sum += curr->value;
        curr = curr->next;
    }

    // 编译期释放
    curr = head;
    while (curr) {
        Node* next = curr->next;
        delete curr;
        curr = next;
    }

    return sum;
}

static_assert(test_constexpr_linked_list() == 15, "1+2+3+4+5=15 at compile time");


// =============================================================================
// 第10章：Policy-Based Design (策略模式的编译期实现)
// =============================================================================
// 将运行期多态（虚函数）替换为编译期策略选择。
// 优点：零虚函数调用开销，编译器可内联所有代码。

// --- 10.1 日志策略 ---

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

// --- 10.2 分配策略 ---

struct HeapAllocator {
    template <typename T>
    static T* allocate(size_t n) {
        return new T[n];
    }
    template <typename T>
    static void deallocate(T* p) {
        delete[] p;
    }
};

struct StackAllocator {
    // 简化版：使用线程本地缓冲区
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
    static void deallocate(T*) {
        // 栈分配器不单独释放
    }
};

// --- 10.3 策略组合：通过模板参数注入 ---

template <typename LogPolicy = ConsoleLogger,
          typename AllocPolicy = HeapAllocator>
class DataProcessor {
public:
    void process(std::string_view data) {
        LogPolicy::log("Processing started");

        // 分配工作缓冲区
        auto* buffer = AllocPolicy::template allocate<char>(data.size());
        if (buffer) {
            for (size_t i = 0; i < data.size(); ++i) {
                buffer[i] = data[i];
            }
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


// =============================================================================
// 第11章：Tag Dispatch (标签分派)
// =============================================================================
// 利用空的标签类型在编译期选择函数重载。
// 比 enable_if 更直观，比 if constexpr 更早出现。

// --- 11.1 迭代器分类标签分派 ---

namespace my {

struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

// 不同类型迭代器用不同算法实现 advance
template <typename Iter>
void advance_impl(Iter& it, int n, input_iterator_tag) {
    // 单向迭代器：只能逐步前进
    std::cout << "  [input] advancing " << n << " steps one by one\n";
    for (int i = 0; i < n; ++i) ++it;
}

template <typename Iter>
void advance_impl(Iter& it, int n, bidirectional_iterator_tag) {
    // 双向迭代器：可以前进也可以后退
    std::cout << "  [bidirectional] advancing " << n << " steps\n";
    if (n >= 0) { for (int i = 0; i < n; ++i) ++it; }
    else        { for (int i = 0; i > n; --i) --it; }
}

template <typename Iter>
void advance_impl(Iter& it, int n, random_access_iterator_tag) {
    // 随机访问迭代器：O(1) 直接跳转
    std::cout << "  [random_access] jumping " << n << " positions\n";
    it += n;
}

// 通用接口：根据迭代器类别自动分派
template <typename Iter, typename Tag>
void my_advance(Iter& it, int n, Tag tag) {
    advance_impl(it, n, tag);
}

} // namespace my

// --- 11.2 标签分派用于类型区分 ---

struct serialize_as_text {};
struct serialize_as_binary {};

template <typename T>
void serialize(const T& obj, serialize_as_text) {
    std::cout << "  Text serialization: " << obj << "\n";
}

template <typename T>
void serialize(const T& obj, serialize_as_binary) {
    std::cout << "  Binary serialization of " << sizeof(T) << " bytes\n";
    (void)obj;
}


// =============================================================================
// 第12章：编译期依赖注入与工厂
// =============================================================================
// 利用模板在编译期注入依赖，实现零开销的依赖反转。

// --- 12.1 编译期工厂 ---

// 产品接口（通过 CRTP 实现）
template <typename Derived>
struct Widget {
    void render() const {
        static_cast<const Derived*>(this)->render_impl();
    }
    std::string name() const {
        return static_cast<const Derived*>(this)->name_impl();
    }
};

struct Button : Widget<Button> {
    void render_impl() const { std::cout << "    [Button rendered]\n"; }
    std::string name_impl() const { return "Button"; }
};

struct TextBox : Widget<TextBox> {
    void render_impl() const { std::cout << "    [TextBox rendered]\n"; }
    std::string name_impl() const { return "TextBox"; }
};

struct Slider : Widget<Slider> {
    void render_impl() const { std::cout << "    [Slider rendered]\n"; }
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

// --- 12.2 编译期依赖注入框架 ---

// 服务接口
struct ConsoleOutput {
    static void write(std::string_view msg) {
        std::cout << msg;
    }
};

struct FileOutput {
    static void write(std::string_view msg) {
        // 实际中写文件，这里简化
        std::cout << "[FILE] " << msg;
    }
};

// 通过模板注入依赖
template <typename OutputService>
class Application {
public:
    void run() {
        OutputService::write("  Application started\n");
        OutputService::write("  Doing work...\n");
        OutputService::write("  Application finished\n");
    }
};

using ConsoleApp = Application<ConsoleOutput>;
using FileApp    = Application<FileOutput>;


// =============================================================================
// 附录：编译期优化的最佳实践
// =============================================================================
//
// 1. 优先使用 constexpr/consteval 而非 TMP
//    - constexpr 更可读、可调试
//    - 编译速度更快
//
// 2. 优先使用 Concepts 而非 SFINAE
//    - 错误信息更清晰
//    - 代码更声明式
//
// 3. 使用 if constexpr 替代 tag dispatch
//    - 代码更集中，不需要多个重载
//    - 但 tag dispatch 在某些场景仍有优势
//
// 4. 编译期计算的权衡
//    - 过多的编译期计算会增加编译时间
//    - 深度递归模板可能超出编译器限制
//    - 使用 -ftemplate-depth=N 调整限制
//
// 5. 利用 static_assert 进行编译期测试
//    - 比运行期测试更可靠
//    - 编译通过 = 测试通过
//
// 6. 表达式模板用于性能关键的数学库
//    - 消除临时对象
//    - 融合循环
//    - 但增加了代码复杂度
//
// 7. Policy-Based Design 用于高性能库
//    - 编译期选择实现策略
//    - 零运行时开销的灵活性
//    - STL 的 allocator 就是这种模式
// =============================================================================


// =============================================================================
// main: 运行演示
// =============================================================================
int main() {
    std::cout << "===== C++ 编译期优化进阶教程 演示 =====\n\n";

    // 1. Concepts
    std::cout << "[1] Concepts 概念约束:\n";
    std::cout << "  safe_add(3, 4) = " << safe_add(3, 4) << "\n";
    std::cout << "  smart_abs(-42) = " << smart_abs(-42) << "\n";
    std::cout << "  smart_abs(-2.71) = " << smart_abs(-2.71) << "\n";
    compute_and_print(10, 20);
    std::cout << "\n";

    // 2. 类型名反射
    std::cout << "[2] 编译期类型名:\n";
    std::cout << "  type_name<int>()    = " << type_name<int>() << "\n";
    std::cout << "  type_name<double>() = " << type_name<double>() << "\n";
    std::cout << "  type_name<Vec3>()   = " << type_name<std::string>() << "\n";
    std::cout << "\n";

    // 3. 模式匹配
    std::cout << "[3] 编译期模式匹配:\n";
    pattern_match_demo();
    std::cout << "\n";

    // 4. Tuple 操作
    std::cout << "[4] Tuple 编译期操作:\n";
    auto my_tuple = std::make_tuple(1, 2.5, std::string("hello"), 42);
    std::cout << "  遍历: ";
    tuple_for_each(my_tuple, [](const auto& elem) {
        std::cout << elem << " ";
    });
    std::cout << "\n";

    auto doubled = tuple_transform(std::make_tuple(1, 2, 3), [](auto x) { return x * 2; });
    std::cout << "  变换 (x2): " << std::get<0>(doubled) << ", "
              << std::get<1>(doubled) << ", " << std::get<2>(doubled) << "\n";

    auto reversed = tuple_reverse(std::make_tuple(1, 2, 3, 4));
    std::cout << "  反转: " << std::get<0>(reversed) << ", "
              << std::get<1>(reversed) << ", " << std::get<2>(reversed) << ", "
              << std::get<3>(reversed) << "\n\n";

    // 5. 编译期排序
    std::cout << "[5] 编译期排序:\n";
    std::cout << "  原始: ";
    for (auto x : unsorted) std::cout << x << " ";
    std::cout << "\n  冒泡: ";
    for (auto x : sorted_arr) std::cout << x << " ";
    std::cout << "\n  快排: ";
    for (auto x : qsorted) std::cout << x << " ";
    std::cout << "\n  binary_search(25) 位于索引 " << ct_binary_search(sorted_arr, 25) << "\n\n";

    // 6. SFINAE
    std::cout << "[6] SFINAE:\n";
    std::cout << "  sfinae_double(21) = " << sfinae_double_value(21) << "\n";
    std::cout << "  sfinae_double(1.5) = " << sfinae_double_value(1.5) << "\n";
    std::cout << "  has_serialize<Serializable> = " << std::boolalpha
              << has_serialize_v<Serializable> << "\n";
    std::cout << "  has_serialize<int> = " << has_serialize_v<int> << "\n\n";

    // 7. 表达式模板
    std::cout << "[7] 表达式模板:\n";
    SmallVec va = {1.0, 2.0, 3.0, 4.0};
    SmallVec vb = {10.0, 20.0, 30.0, 40.0};
    SmallVec vc = wrap(va) + wrap(vb);                   // 无临时对象
    SmallVec vd = (wrap(va) + wrap(vb)) * 2.0;          // 融合计算
    va.print("a");
    vb.print("b");
    vc.print("a+b");
    vd.print("(a+b)*2");
    std::cout << "\n";

    // 8. 编译期位操作
    std::cout << "[8] 编译期位操作:\n";
    std::cout << "  popcount(0b10110101)  = " << ct_popcount(0b10110101) << "\n";
    std::cout << "  clz(1)               = " << ct_clz(1) << "\n";
    std::cout << "  log2(1024)            = " << ct_log2(1024) << "\n";
    std::cout << "  is_power_of_two(256)  = " << std::boolalpha << ct_is_power_of_two(256) << "\n";
    std::cout << "  next_power_of_two(100)= " << ct_next_power_of_two(100) << "\n\n";

    // 9. constexpr 动态分配
    std::cout << "[9] constexpr 动态分配:\n";
    std::cout << "  constexpr vec test    = " << test_constexpr_vec() << " (编译期计算)\n";
    std::cout << "  constexpr linked list = " << test_constexpr_linked_list() << " (编译期计算)\n\n";

    // 10. Policy-Based Design
    std::cout << "[10] 策略模式:\n";
    std::cout << "  Verbose Processor:\n";
    VerboseProcessor{}.process("Hello");
    std::cout << "  Silent Processor: (无输出)\n";
    SilentProcessor{}.process("Hello");
    std::cout << "  Fast Processor:\n";
    FastProcessor{}.process("Hello");
    std::cout << "\n";

    // 11. Tag Dispatch
    std::cout << "[11] 标签分派:\n";
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin();
    my::my_advance(it, 3, my::random_access_iterator_tag{});
    std::cout << "  after advance(3): *it = " << *it << "\n";

    serialize(42, serialize_as_text{});
    serialize(3.14, serialize_as_binary{});
    std::cout << "\n";

    // 12. 编译期工厂
    std::cout << "[12] 编译期工厂:\n";
    auto btn = create_widget<WidgetType::Button>();
    auto txt = create_widget<WidgetType::TextBox>();
    auto sld = create_widget<WidgetType::Slider>();
    btn.render();
    txt.render();
    sld.render();
    std::cout << "\n";

    std::cout << "  ConsoleApp:\n";
    ConsoleApp{}.run();
    std::cout << "  FileApp:\n";
    FileApp{}.run();

    std::cout << "\n===== 所有 static_assert 通过 = 编译期正确性已验证 =====\n";
    return 0;
}
