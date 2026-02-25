// =============================================================================
// 现代 C++ 零开销抽象 完全指导教程与代码示例
// =============================================================================
// "What you don't use, you don't pay for.
//  And further: What you do use, you couldn't hand code any better."
//                                       — Bjarne Stroustrup
//
// 零开销抽象 (Zero-Cost Abstraction) 是 C++ 的核心设计哲学：
//   1. 你不使用的特性不会产生任何运行时开销
//   2. 你使用的特性，手写底层代码也不会更快
//
// 标准：C++17/20
// 编译：g++ -std=c++17 -O2 -o test5 test5.cpp -lpthread
//       cl /std:c++17 /O2 /EHsc test5.cpp
//
// 目录：
//   1.  零开销抽象的定义与原则
//   2.  RAII — 资源管理的零开销
//   3.  迭代器抽象 — 与指针等效的性能
//   4.  模板 — 零开销泛型
//   5.  constexpr — 编译期零运行时开销
//   6.  strong typedef — 类型安全零开销
//   7.  std::optional — 替代指针/哨兵值
//   8.  std::variant — 替代虚函数的零堆分配
//   9.  std::span (C++20 风格) — 零拷贝视图
//  10.  CRTP — 编译期多态零虚表
//  11.  Policy-Based Design — 可组合的零开销策略
//  12.  Expression Templates — 消除临时对象
//  13.  Proxy/Handle 模式 — 延迟求值
//  14.  编译期状态检查 — 类型系统做守卫
//  15.  零开销异常替代方案
//  16.  实战：零开销矩阵库
//  17.  实战：零开销单位系统 (Units Library)
//  18.  实战：零开销有限状态机
//  19.  汇编验证：证明零开销
// =============================================================================

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <memory>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <utility>
#include <type_traits>
#include <functional>
#include <variant>
#include <optional>
#include <cassert>
#include <tuple>


// =============================================================================
// 工具
// =============================================================================
class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    Timer(const char* label) : start_(Clock::now()), label_(label) {}
    ~Timer() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
            Clock::now() - start_).count();
        std::cout << "  [" << label_ << "] " << us << " us\n";
    }
};

template <typename T>
void do_not_optimize(T&& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}


// =============================================================================
// 第1章：零开销抽象的定义与原则
// =============================================================================
//
// 零开销抽象意味着：
//
// (1) 编译后与手写 C 代码 _完全相同_ 的机器码
//
//   例如：std::sort       vs  手写快排         → 同样快（甚至更快）
//         std::unique_ptr vs  裸指针 + delete  → 同样的汇编
//         range-for       vs  index loop       → 同样的汇编
//
// (2) 不是 "低开销"，而是 "零开销"
//
//   Java 的泛型在运行时做类型擦除 → 开销
//   C++ 的模板在编译期完全具体化 → 零开销
//
// (3) 并非所有 C++ 特性都是零开销的
//
//   ❌ 虚函数:     vtable 间接跳转 + 无法内联
//   ❌ std::function: 类型擦除 + 可能堆分配
//   ❌ 异常:       栈展开表（空间开销，即使不抛出）
//   ❌ RTTI:       typeinfo 结构体（空间开销）
//
//   ✅ 模板、constexpr、RAII、迭代器、CRTP、
//      strong typedef、optional、variant、span → 零开销
//
// 本教程的每一章都将证明抽象的 "零开销" 特性。


// =============================================================================
// 第2章：RAII — 资源管理的零开销
// =============================================================================
// RAII (Resource Acquisition Is Initialization):
//   构造时获取资源，析构时释放资源。
//   编译器在确定的时机调用析构函数 → 零运行时判断开销。

namespace ch2 {

// --- 2.1 手写 C 风格 vs RAII ---

// C 风格：容易忘记释放，异常不安全
void c_style() {
    int* data = (int*)malloc(100 * sizeof(int));
    if (!data) return;

    // ... 使用 data ...
    // 如果这里抛异常或提前 return → 内存泄漏！

    free(data);
}

// RAII 风格：零开销 + 异常安全
void raii_style() {
    auto data = std::make_unique<int[]>(100);
    // 编译器保证在函数退出时调用析构函数
    // 异常、提前return、正常退出 → 全部安全释放
    // 与 C 风格生成的析构代码完全相同
}

// --- 2.2 自定义 RAII 封装 ---

// 文件句柄的零开销 RAII 封装
class FileHandle {
    FILE* fp_ = nullptr;
public:
    explicit FileHandle(const char* filename, const char* mode)
        : fp_(fopen(filename, mode)) {}

    ~FileHandle() {
        if (fp_) fclose(fp_);
    }

    // 禁止拷贝
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // 允许移动
    FileHandle(FileHandle&& other) noexcept : fp_(other.fp_) {
        other.fp_ = nullptr;
    }
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            if (fp_) fclose(fp_);
            fp_ = other.fp_;
            other.fp_ = nullptr;
        }
        return *this;
    }

    FILE* get() const { return fp_; }
    explicit operator bool() const { return fp_ != nullptr; }
};

// 编译后，FileHandle 与裸 FILE* 使用完全相同的机器码，
// 只是编译器自动插入了 fclose 调用。

// --- 2.3 ScopeGuard — 通用 RAII ---

template <typename Func>
class ScopeGuard {
    Func func_;
    bool active_ = true;
public:
    explicit ScopeGuard(Func f) : func_(std::move(f)) {}
    ~ScopeGuard() { if (active_) func_(); }

    ScopeGuard(ScopeGuard&& other) noexcept
        : func_(std::move(other.func_)), active_(other.active_) {
        other.dismiss();
    }

    void dismiss() { active_ = false; }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

template <typename Func>
ScopeGuard<Func> make_scope_guard(Func f) {
    return ScopeGuard<Func>(std::move(f));
}

// 使用示例：
void demo_scope_guard() {
    int* raw = new int[100];

    auto guard = make_scope_guard([&]() {
        delete[] raw;
        std::cout << "  ScopeGuard: 资源已释放\n";
    });

    // ... 使用 raw ...
    // 无论如何退出，guard 的析构函数都会调用 lambda
}

// --- 2.4 零开销证明：unique_ptr vs 裸指针 ---

void demo_unique_ptr_zero_cost() {
    constexpr int N = 10000000;

    // 裸指针
    {
        Timer t("裸指针 new/delete");
        for (int i = 0; i < N; ++i) {
            int* p = new int(i);
            do_not_optimize(*p);
            delete p;
        }
    }

    // unique_ptr
    {
        Timer t("unique_ptr");
        for (int i = 0; i < N; ++i) {
            auto p = std::make_unique<int>(i);
            do_not_optimize(*p);
        }
    }

    // 大小也相同
    static_assert(sizeof(std::unique_ptr<int>) == sizeof(int*),
                  "unique_ptr is same size as raw pointer");
    std::cout << "  sizeof(int*)             = " << sizeof(int*) << "\n";
    std::cout << "  sizeof(unique_ptr<int>)  = " << sizeof(std::unique_ptr<int>) << "\n";
}

} // namespace ch2


// =============================================================================
// 第3章：迭代器抽象 — 与指针等效的性能
// =============================================================================

namespace ch3 {

// --- 3.1 range-for vs 索引循环 vs 指针循环 ---
// 三者生成 _完全相同_ 的机器码（-O2）。

void demo_iterator_zero_cost() {
    constexpr int N = 10000000;
    std::vector<int> data(N);
    std::iota(data.begin(), data.end(), 0);

    // (a) 索引循环
    {
        Timer t("索引循环");
        long sum = 0;
        for (size_t i = 0; i < data.size(); ++i)
            sum += data[i];
        do_not_optimize(sum);
    }

    // (b) 迭代器
    {
        Timer t("迭代器");
        long sum = 0;
        for (auto it = data.begin(); it != data.end(); ++it)
            sum += *it;
        do_not_optimize(sum);
    }

    // (c) range-for
    {
        Timer t("range-for");
        long sum = 0;
        for (int x : data)
            sum += x;
        do_not_optimize(sum);
    }

    // (d) std::accumulate
    {
        Timer t("std::accumulate");
        long sum = std::accumulate(data.begin(), data.end(), 0L);
        do_not_optimize(sum);
    }

    // 四者时间应该完全相同（在 -O2 下）
}

// --- 3.2 自定义迭代器 — 同样零开销 ---

// 不拥有内存的轻量视图
template <typename T>
class ArrayView {
    const T* data_;
    size_t size_;

public:
    constexpr ArrayView(const T* data, size_t size)
        : data_(data), size_(size) {}

    constexpr ArrayView(const std::vector<T>& vec)
        : data_(vec.data()), size_(vec.size()) {}

    template <size_t N>
    constexpr ArrayView(const std::array<T, N>& arr)
        : data_(arr.data()), size_(N) {}

    // 迭代器就是指针 — 零开销
    constexpr const T* begin() const { return data_; }
    constexpr const T* end()   const { return data_ + size_; }
    constexpr size_t size()    const { return size_; }
    constexpr const T& operator[](size_t i) const { return data_[i]; }

    constexpr ArrayView subview(size_t offset, size_t count) const {
        return ArrayView(data_ + offset, count);
    }
};

// ArrayView 的大小 = 1个指针 + 1个 size_t — 与手写结构完全相同
static_assert(sizeof(ArrayView<int>) == sizeof(int*) + sizeof(size_t),
              "ArrayView is minimal");

void demo_array_view() {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    ArrayView<int> view(data);

    std::cout << "  ArrayView: ";
    for (int x : view) std::cout << x << " ";
    std::cout << "\n";

    auto sub = view.subview(3, 4);
    std::cout << "  subview(3,4): ";
    for (int x : sub) std::cout << x << " ";
    std::cout << "\n";
}

} // namespace ch3


// =============================================================================
// 第4章：模板 — 零开销泛型
// =============================================================================
// C++ 模板在编译期完全具体化（monomorphization）。
// 与 Java 泛型（类型擦除）、Go 接口（间接调用）不同，
// C++ 模板生成的代码与手写特定类型版本完全相同。

namespace ch4 {

// --- 4.1 模板函数 vs 手写特定版本 ---

// 模板版本
template <typename T>
T generic_max(T a, T b) {
    return a > b ? a : b;
}

// 手写版本
int int_max(int a, int b) {
    return a > b ? a : b;
}

double double_max(double a, double b) {
    return a > b ? a : b;
}

// generic_max<int> 编译后的汇编与 int_max 完全相同。
// 这就是零开销泛型。

// --- 4.2 模板 vs 虚函数 vs std::function 性能对比 ---

// 策略接口：比较两个 int
struct LessThan {
    bool operator()(int a, int b) const { return a < b; }
};

struct GreaterThan {
    bool operator()(int a, int b) const { return a > b; }
};

// (a) 模板版本 —— 零开销（编译器内联比较器）
template <typename Compare>
void sort_template(int* data, int n, Compare cmp) {
    // 简化的插入排序
    for (int i = 1; i < n; ++i) {
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && cmp(data[j], key)) {
            data[j + 1] = data[j];
            --j;
        }
        data[j + 1] = key;
    }
}

// (b) 虚函数版本 —— 有开销（间接调用）
struct ICompare {
    virtual bool compare(int a, int b) const = 0;
    virtual ~ICompare() = default;
};

struct LessVirtual : ICompare {
    bool compare(int a, int b) const override { return a < b; }
};

void sort_virtual(int* data, int n, const ICompare& cmp) {
    for (int i = 1; i < n; ++i) {
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && cmp.compare(data[j], key)) {
            data[j + 1] = data[j];
            --j;
        }
        data[j + 1] = key;
    }
}

// (c) std::function 版本 —— 有开销（类型擦除 + 间接调用）
void sort_function(int* data, int n, std::function<bool(int, int)> cmp) {
    for (int i = 1; i < n; ++i) {
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && cmp(data[j], key)) {
            data[j + 1] = data[j];
            --j;
        }
        data[j + 1] = key;
    }
}

void demo_template_zero_cost() {
    constexpr int N = 10000;
    constexpr int ITERS = 100;

    auto make_data = [&]() {
        std::vector<int> v(N);
        for (int i = 0; i < N; ++i) v[i] = N - i;
        return v;
    };

    // 模板
    {
        Timer t("模板 sort (零开销)");
        for (int iter = 0; iter < ITERS; ++iter) {
            auto data = make_data();
            sort_template(data.data(), N, GreaterThan{});
            do_not_optimize(data[0]);
        }
    }

    // 虚函数
    {
        Timer t("虚函数 sort");
        LessVirtual cmp;
        for (int iter = 0; iter < ITERS; ++iter) {
            auto data = make_data();
            sort_virtual(data.data(), N, cmp);
            do_not_optimize(data[0]);
        }
    }

    // std::function
    {
        Timer t("std::function sort");
        for (int iter = 0; iter < ITERS; ++iter) {
            auto data = make_data();
            sort_function(data.data(), N, [](int a, int b) { return a > b; });
            do_not_optimize(data[0]);
        }
    }
}

} // namespace ch4


// =============================================================================
// 第5章：constexpr — 编译期零运行时开销
// =============================================================================
// constexpr 计算在编译期完成，运行期开销为零。
// 详见 test.cpp / test3.cpp — 这里只强调零开销特性。

namespace ch5 {

// 编译期查找表：运行期只需数组访问
constexpr auto make_square_table() {
    std::array<int, 256> table{};
    for (int i = 0; i < 256; ++i)
        table[i] = i * i;
    return table;
}

inline constexpr auto SQUARE_TABLE = make_square_table();

// 运行期：直接查表，O(1)，与手写常量数组完全相同
int fast_square(uint8_t x) {
    return SQUARE_TABLE[x];
}

// 编译期哈希：switch/case 的标签在编译期计算
constexpr uint32_t ct_hash(std::string_view sv) {
    uint32_t h = 2166136261u;
    for (char c : sv) { h ^= c; h *= 16777619u; }
    return h;
}

// 这个 switch 与用整数常量做 case 完全相同
void dispatch(std::string_view cmd) {
    switch (ct_hash(cmd)) {
        case ct_hash("open"):  std::cout << "  open\n"; break;
        case ct_hash("close"): std::cout << "  close\n"; break;
        case ct_hash("read"):  std::cout << "  read\n"; break;
        default: std::cout << "  unknown\n"; break;
    }
}

} // namespace ch5


// =============================================================================
// 第6章：Strong Typedef — 类型安全零开销
// =============================================================================
// 用不同类型包装同一底层类型，获得编译期类型检查，
// 但运行期与裸类型完全相同。

namespace ch6 {

// --- 6.1 问题：类型混淆 ---

// 危险：所有参数都是 double，容易传错
// double compute_force(double mass, double acceleration, double velocity);

// --- 6.2 Strong Typedef 实现 ---

template <typename Tag, typename T = double>
class StrongType {
    T value_;
public:
    constexpr explicit StrongType(T val) : value_(val) {}
    constexpr T get() const { return value_; }

    // 同类型可以运算
    constexpr StrongType operator+(StrongType rhs) const {
        return StrongType(value_ + rhs.value_);
    }
    constexpr StrongType operator-(StrongType rhs) const {
        return StrongType(value_ - rhs.value_);
    }
    constexpr StrongType operator*(T scalar) const {
        return StrongType(value_ * scalar);
    }
    constexpr bool operator<(StrongType rhs) const {
        return value_ < rhs.value_;
    }
    constexpr bool operator==(StrongType rhs) const {
        return value_ == rhs.value_;
    }
};

// 定义具体类型
using Meters     = StrongType<struct MetersTag>;
using Seconds    = StrongType<struct SecondsTag>;
using Kilograms  = StrongType<struct KilogramsTag>;
using MetersPerSec = StrongType<struct VelocityTag>;

// 类型安全的函数
constexpr MetersPerSec compute_velocity(Meters distance, Seconds time) {
    return MetersPerSec(distance.get() / time.get());
}

// 零开销证明：
static_assert(sizeof(Meters) == sizeof(double),
              "StrongType is same size as underlying type");
static_assert(sizeof(Seconds) == sizeof(double), "zero overhead");

// 编译期验证
constexpr Meters dist(100.0);
constexpr Seconds time_val(9.58);
constexpr MetersPerSec speed = compute_velocity(dist, time_val);

// 以下代码无法编译 — 类型安全！
// Meters wrong = Meters(1.0) + Seconds(2.0);  // ❌ 编译错误
// auto bad = compute_velocity(time_val, dist); // ❌ 参数类型不匹配

void demo_strong_type() {
    std::cout << "  100m / 9.58s = " << speed.get() << " m/s\n";

    Meters a(10.0), b(20.0);
    Meters c = a + b;                  // ✅ 同类型可以加
    std::cout << "  10m + 20m = " << c.get() << "m\n";

    // 编译器生成的代码与直接操作 double 完全相同
}

// --- 6.3 更完整的 Strong Typedef：支持混合运算 ---

// 物理量运算规则（编译期检查维度）
struct Meters2Tag {};
using SquareMeters = StrongType<Meters2Tag>;

constexpr SquareMeters compute_area(Meters length, Meters width) {
    return SquareMeters(length.get() * width.get());
}

constexpr auto area = compute_area(Meters(5.0), Meters(3.0));
static_assert(area.get() == 15.0, "5m * 3m = 15m^2");

} // namespace ch6


// =============================================================================
// 第7章：std::optional — 替代指针/哨兵值的零堆分配
// =============================================================================

namespace ch7 {

// --- 7.1 optional 的零开销特性 ---
// optional<T> 内部就是 T + bool，无堆分配。
// 与手写 struct { T value; bool has_value; } 完全相同。

static_assert(sizeof(std::optional<int>) == sizeof(int) + sizeof(int),
              "optional<int> = 8 bytes (int + padding for bool)");

// --- 7.2 替代空指针 ---

// ❌ 旧方式：返回指针表示"可能没有值"
int* find_bad(std::vector<int>& data, int target) {
    for (auto& x : data)
        if (x == target) return &x;
    return nullptr;
}

// ✅ 零开销方式：optional 不需要堆分配
std::optional<int> find_good(const std::vector<int>& data, int target) {
    for (int x : data)
        if (x == target) return x;
    return std::nullopt;
}

// --- 7.3 替代哨兵值 ---

// ❌ 旧方式：用 -1 表示"未找到"
constexpr int find_index_bad(const int* arr, int n, int target) {
    for (int i = 0; i < n; ++i)
        if (arr[i] == target) return i;
    return -1; // 魔法数字
}

// ✅ 零开销方式：类型系统表达"可能没有"
constexpr std::optional<int> find_index_good(const int* arr, int n, int target) {
    for (int i = 0; i < n; ++i)
        if (arr[i] == target) return i;
    return std::nullopt;
}

// --- 7.4 optional 链式调用 ---

struct User {
    std::string name;
    std::optional<std::string> email;
    std::optional<int> age;
};

// 安全的链式访问
std::optional<size_t> get_email_length(const User& user) {
    if (user.email.has_value())
        return user.email->size();
    return std::nullopt;
}

// --- 7.5 optional 性能基准 ---

void demo_optional_zero_cost() {
    constexpr int N = 10000000;
    std::vector<int> data(N);
    std::iota(data.begin(), data.end(), 0);

    {
        Timer t("指针方式查找");
        for (int i = 0; i < 100; ++i) {
            auto* result = find_bad(data, N / 2);
            do_not_optimize(result);
        }
    }

    {
        Timer t("optional 方式查找");
        for (int i = 0; i < 100; ++i) {
            auto result = find_good(data, N / 2);
            do_not_optimize(result);
        }
    }
}

} // namespace ch7


// =============================================================================
// 第8章：std::variant — 替代虚函数的零堆分配
// =============================================================================

namespace ch8 {

// variant 的零开销特性：
// 1. 大小 = max(各类型大小) + 类型索引 — 无堆分配
// 2. std::visit 编译期生成跳转表 — 与 switch 等效
// 3. 数据在栈上 — 缓存友好

// --- 8.1 大小对比 ---

struct Circle { double r; };
struct Rect   { double w, h; };
struct Triangle { double a, b, c; };

using Shape = std::variant<Circle, Rect, Triangle>;

// variant 大小 = 最大成员 + 对齐
// Triangle 有 3 个 double = 24 bytes，加上 type index
void demo_variant_size() {
    std::cout << "  sizeof(Circle)  = " << sizeof(Circle)  << "\n";
    std::cout << "  sizeof(Rect)    = " << sizeof(Rect)    << "\n";
    std::cout << "  sizeof(Triangle)= " << sizeof(Triangle)<< "\n";
    std::cout << "  sizeof(variant) = " << sizeof(Shape)   << "\n";
    // variant 在栈上，与 union + enum 完全等效，但类型安全
}

// --- 8.2 Overloaded 模式 ---

template <typename... Fs>
struct Overloaded : Fs... { using Fs::operator()...; };
template <typename... Fs>
Overloaded(Fs...) -> Overloaded<Fs...>;

double area(const Shape& s) {
    return std::visit(Overloaded{
        [](const Circle& c)   { return 3.14159265 * c.r * c.r; },
        [](const Rect& r)     { return r.w * r.h; },
        [](const Triangle& t) {
            // 海伦公式
            double s = (t.a + t.b + t.c) / 2.0;
            return std::sqrt(s * (s - t.a) * (s - t.b) * (s - t.c));
        }
    }, s);
}

// --- 8.3 variant vs 虚函数性能对比 ---

struct IShape {
    virtual double area() const = 0;
    virtual ~IShape() = default;
};

struct CircleV : IShape {
    double r;
    CircleV(double r) : r(r) {}
    double area() const override { return 3.14159265 * r * r; }
};

struct RectV : IShape {
    double w, h;
    RectV(double w, double h) : w(w), h(h) {}
    double area() const override { return w * h; }
};

void demo_variant_vs_virtual() {
    constexpr int N = 5000000;

    // variant: 栈上，连续内存
    {
        std::vector<Shape> shapes;
        shapes.reserve(N);
        for (int i = 0; i < N; ++i)
            shapes.emplace_back(i % 2 ? Shape(Circle{1.0}) : Shape(Rect{2.0, 3.0}));

        Timer t("variant visit (连续内存)");
        double total = 0;
        for (const auto& s : shapes)
            total += area(s);
        do_not_optimize(total);
    }

    // 虚函数: 堆上，指针追踪
    {
        std::vector<std::unique_ptr<IShape>> shapes;
        shapes.reserve(N);
        for (int i = 0; i < N; ++i) {
            if (i % 2) shapes.push_back(std::make_unique<CircleV>(1.0));
            else       shapes.push_back(std::make_unique<RectV>(2.0, 3.0));
        }

        Timer t("虚函数 (堆分配, 指针追踪)");
        double total = 0;
        for (const auto& s : shapes)
            total += s->area();
        do_not_optimize(total);
    }
}

} // namespace ch8


// =============================================================================
// 第9章：零拷贝视图 — string_view 与 ArrayView
// =============================================================================

namespace ch9 {

// --- 9.1 string_view: 零拷贝字符串参数 ---

// ❌ 需要拷贝
size_t count_spaces_bad(std::string text) {
    return std::count(text.begin(), text.end(), ' ');
}

// ✅ 零拷贝
size_t count_spaces_good(std::string_view text) {
    return std::count(text.begin(), text.end(), ' ');
}

// string_view = 指针 + 长度，与 C 风格 (const char*, size_t) 完全等效
static_assert(sizeof(std::string_view) == sizeof(const char*) + sizeof(size_t),
              "string_view is zero overhead");

// --- 9.2 零拷贝解析 ---

// 解析 CSV 行：不分配任何堆内存
constexpr size_t MAX_FIELDS = 32;

struct CSVRow {
    std::string_view fields[MAX_FIELDS];
    size_t count = 0;
};

constexpr CSVRow parse_csv(std::string_view line) {
    CSVRow row;
    while (!line.empty() && row.count < MAX_FIELDS) {
        auto comma = line.find(',');
        if (comma == std::string_view::npos) {
            row.fields[row.count++] = line;
            break;
        }
        row.fields[row.count++] = line.substr(0, comma);
        line.remove_prefix(comma + 1);
    }
    return row;
}

// 编译期解析！
constexpr auto csv = parse_csv("Alice,30,Engineer,NYC");
static_assert(csv.count == 4, "4 fields");
static_assert(csv.fields[0] == "Alice", "first field");
static_assert(csv.fields[2] == "Engineer", "third field");

void demo_csv_parse() {
    auto row = parse_csv("hello,world,foo,bar,baz");
    std::cout << "  CSV 解析 (" << row.count << " 个字段): ";
    for (size_t i = 0; i < row.count; ++i) {
        if (i > 0) std::cout << " | ";
        std::cout << row.fields[i];
    }
    std::cout << "\n";
}

// --- 9.3 string_view 性能对比 ---

void demo_string_view_perf() {
    std::string long_str(10000, 'x');
    constexpr int N = 1000000;

    {
        Timer t("string 子串 (堆分配)");
        for (int i = 0; i < N; ++i) {
            std::string sub = long_str.substr(100, 200);
            do_not_optimize(sub);
        }
    }
    {
        Timer t("string_view 子串 (零拷贝)");
        std::string_view sv(long_str);
        for (int i = 0; i < N; ++i) {
            std::string_view sub = sv.substr(100, 200);
            do_not_optimize(sub);
        }
    }
}

} // namespace ch9


// =============================================================================
// 第10章：CRTP — 编译期多态零虚表
// =============================================================================

namespace ch10 {

// CRTP 实现编译期多态：
// - 无 vtable（节省内存 + 避免间接跳转）
// - 编译器可以内联所有调用
// - 与手写特定类型版本生成相同机器码

template <typename Derived>
class Serializable {
public:
    std::string to_json() const {
        return "{" + static_cast<const Derived*>(this)->fields_json() + "}";
    }

    void print_json() const {
        std::cout << "  " << to_json() << "\n";
    }
};

template <typename Derived>
class Comparable {
public:
    bool operator>(const Derived& rhs) const {
        return static_cast<const Derived*>(this)->compare_value() >
               rhs.compare_value();
    }
    bool operator<(const Derived& rhs) const {
        return static_cast<const Derived*>(this)->compare_value() <
               rhs.compare_value();
    }
    bool operator==(const Derived& rhs) const {
        return static_cast<const Derived*>(this)->compare_value() ==
               rhs.compare_value();
    }
};

class Product : public Serializable<Product>,
                public Comparable<Product> {
    std::string name_;
    double price_;
public:
    Product(std::string name, double price)
        : name_(std::move(name)), price_(price) {}

    std::string fields_json() const {
        return "\"name\":\"" + name_ + "\",\"price\":" + std::to_string(price_);
    }

    double compare_value() const { return price_; }
};

// Product 没有 vtable！
// sizeof(Product) = sizeof(string) + sizeof(double)
// 继承 Serializable 和 Comparable 不增加任何内存

void demo_crtp() {
    Product a("Widget", 9.99);
    Product b("Gadget", 19.99);

    a.print_json();
    b.print_json();

    std::cout << "  Widget < Gadget? " << std::boolalpha << (a < b) << "\n";

    // 零大小证明：CRTP 基类不增加大小
    std::cout << "  sizeof(Product) = " << sizeof(Product) << "\n";
    std::cout << "  sizeof(string) + sizeof(double) = "
              << sizeof(std::string) + sizeof(double) << "\n";
}

} // namespace ch10


// =============================================================================
// 第11章：Policy-Based Design — 可组合的零开销策略
// =============================================================================

namespace ch11 {

// 策略通过模板参数注入，编译期确定，被完全内联。
// 与同等功能的虚函数版本相比：零间接调用开销。

// --- 11.1 策略定义 ---

// 线程安全策略
struct SingleThreaded {
    struct Lock {};
    static Lock lock() { return {}; }
    // 完全空操作 — 编译器会优化掉
};

struct MultiThreaded {
    // 实际项目中这里用 std::mutex
    struct Lock {
        Lock() { /* mutex.lock() */ }
        ~Lock() { /* mutex.unlock() */ }
    };
    static Lock lock() { return Lock{}; }
};

// 边界检查策略
struct BoundsChecked {
    static void check(size_t index, size_t size) {
        if (index >= size) {
            throw std::out_of_range("Index out of bounds");
        }
    }
};

struct NoBoundsCheck {
    static void check(size_t, size_t) {
        // 空操作 — 编译器完全优化掉
    }
};

// 存储策略
template <typename T, size_t N>
struct StackStorage {
    T data[N];
    static constexpr size_t capacity() { return N; }
};

template <typename T, size_t InitCap = 16>
struct HeapStorage {
    std::vector<T> data;
    HeapStorage() { data.reserve(InitCap); }
    size_t capacity() const { return data.capacity(); }
};

// --- 11.2 策略组合 ---

template <typename T,
          typename ThreadPolicy = SingleThreaded,
          typename BoundsPolicy = NoBoundsCheck>
class SmartArray {
    std::vector<T> data_;

public:
    SmartArray() = default;
    SmartArray(std::initializer_list<T> init) : data_(init) {}

    void push_back(const T& val) {
        auto lock = ThreadPolicy::lock(); // 单线程时被优化掉
        data_.push_back(val);
    }

    T& operator[](size_t index) {
        BoundsPolicy::check(index, data_.size()); // 无检查时被优化掉
        return data_[index];
    }

    const T& operator[](size_t index) const {
        BoundsPolicy::check(index, data_.size());
        return data_[index];
    }

    size_t size() const { return data_.size(); }
};

// 不同配置 — 编译期确定，零运行时开销
using FastArray = SmartArray<int, SingleThreaded, NoBoundsCheck>;
using SafeArray = SmartArray<int, SingleThreaded, BoundsChecked>;
using ConcurrentArray = SmartArray<int, MultiThreaded, NoBoundsCheck>;

void demo_policy() {
    FastArray fast = {1, 2, 3, 4, 5};
    SafeArray safe = {1, 2, 3, 4, 5};

    // FastArray 访问 — 与裸数组相同的机器码
    std::cout << "  FastArray[2] = " << fast[2] << "\n";

    // SafeArray 访问 — 带边界检查
    try {
        std::cout << "  SafeArray[2] = " << safe[2] << "\n";
        // safe[10]; // 抛出 out_of_range
    } catch (const std::out_of_range& e) {
        std::cout << "  异常: " << e.what() << "\n";
    }

    // 同一份代码，策略配置不同，性能特征不同
    // Debug 模式用 SafeArray，Release 用 FastArray
}

} // namespace ch11


// =============================================================================
// 第12章：Expression Templates — 消除临时对象
// =============================================================================

namespace ch12 {

// 表达式模板将计算延迟到赋值时，一次性求值。
// a + b + c 不创建 2 个临时 vector，而是直接逐元素计算。
// 编译后与手写循环完全相同 — 零开销。

// 表达式基类 (CRTP)
template <typename E>
struct VecExpr {
    double operator[](size_t i) const {
        return static_cast<const E&>(*this)[i];
    }
    size_t size() const {
        return static_cast<const E&>(*this).size();
    }
};

// 实际向量
class Vec : public VecExpr<Vec> {
    std::vector<double> data_;
public:
    Vec(size_t n, double val = 0) : data_(n, val) {}

    // 从表达式构造 —— 延迟求值在此发生
    template <typename E>
    Vec(const VecExpr<E>& expr) : data_(expr.size()) {
        for (size_t i = 0; i < data_.size(); ++i)
            data_[i] = expr[i];
        // 整个表达式树在一次循环中求值！
    }

    template <typename E>
    Vec& operator=(const VecExpr<E>& expr) {
        data_.resize(expr.size());
        for (size_t i = 0; i < data_.size(); ++i)
            data_[i] = expr[i];
        return *this;
    }

    double  operator[](size_t i) const { return data_[i]; }
    double& operator[](size_t i)       { return data_[i]; }
    size_t  size() const { return data_.size(); }
};

// 加法表达式节点
template <typename L, typename R>
struct VecAdd : public VecExpr<VecAdd<L, R>> {
    const L& lhs;
    const R& rhs;
    VecAdd(const L& l, const R& r) : lhs(l), rhs(r) {}
    double operator[](size_t i) const { return lhs[i] + rhs[i]; }
    size_t size() const { return lhs.size(); }
};

// 乘法表达式节点
template <typename L, typename R>
struct VecMul : public VecExpr<VecMul<L, R>> {
    const L& lhs;
    const R& rhs;
    VecMul(const L& l, const R& r) : lhs(l), rhs(r) {}
    double operator[](size_t i) const { return lhs[i] * rhs[i]; }
    size_t size() const { return lhs.size(); }
};

// 标量乘法表达式节点
template <typename E>
struct VecScale : public VecExpr<VecScale<E>> {
    const E& expr;
    double scalar;
    VecScale(const E& e, double s) : expr(e), scalar(s) {}
    double operator[](size_t i) const { return expr[i] * scalar; }
    size_t size() const { return expr.size(); }
};

// 运算符
template <typename L, typename R>
VecAdd<L, R> operator+(const VecExpr<L>& l, const VecExpr<R>& r) {
    return {static_cast<const L&>(l), static_cast<const R&>(r)};
}

template <typename L, typename R>
VecMul<L, R> operator*(const VecExpr<L>& l, const VecExpr<R>& r) {
    return {static_cast<const L&>(l), static_cast<const R&>(r)};
}

template <typename E>
VecScale<E> operator*(const VecExpr<E>& e, double s) {
    return {static_cast<const E&>(e), s};
}

void demo_expression_template() {
    constexpr size_t N = 1000000;

    Vec a(N, 1.0), b(N, 2.0), c(N, 3.0);

    // 朴素方式（如果不用表达式模板）
    // Vec temp1 = a + b;     // 分配临时 vector
    // Vec temp2 = temp1 + c; // 再分配临时 vector
    // Vec result = temp2;

    // 表达式模板方式：
    // a + b + c 构建类型 VecAdd<VecAdd<Vec, Vec>, Vec>
    // 赋值时一次性求值：result[i] = a[i] + b[i] + c[i]

    {
        Timer t("朴素方式（临时对象）");
        Vec temp1(N);
        for (size_t i = 0; i < N; ++i) temp1[i] = a[i] + b[i];
        Vec result(N);
        for (size_t i = 0; i < N; ++i) result[i] = temp1[i] + c[i];
        do_not_optimize(result[0]);
    }

    {
        Timer t("表达式模板（零临时对象）");
        Vec result = a + b + c; // 一次循环，零临时分配
        do_not_optimize(result[0]);
    }

    // 更复杂的表达式也是一次循环
    {
        Timer t("复杂表达式模板");
        Vec result = (a + b) * c * 2.0; // 一次循环
        do_not_optimize(result[0]);
    }
}

} // namespace ch12


// =============================================================================
// 第13章：Proxy/Handle 模式 — 延迟求值
// =============================================================================

namespace ch13 {

// Proxy 对象允许在 "假装" 做某事时实际延迟操作，
// 减少不必要的工作。经典案例：operator[] 的读写分离。

class SmartString {
    std::string data_;

    // Proxy: 区分读和写
    class CharProxy {
        SmartString& str_;
        size_t index_;
    public:
        CharProxy(SmartString& str, size_t i) : str_(str), index_(i) {}

        // 读取（隐式转换为 char）
        operator char() const {
            std::cout << "    [read] index " << index_ << "\n";
            return str_.data_[index_];
        }

        // 写入（赋值运算符）
        CharProxy& operator=(char c) {
            std::cout << "    [write] index " << index_ << " = '" << c << "'\n";
            str_.data_[index_] = c;
            return *this;
        }
    };

public:
    SmartString(std::string s) : data_(std::move(s)) {}

    CharProxy operator[](size_t i) {
        return CharProxy(*this, i);
    }

    char operator[](size_t i) const {
        return data_[i];
    }

    const std::string& str() const { return data_; }
};

void demo_proxy() {
    SmartString s("Hello");

    std::cout << "  读操作:\n";
    char c = s[1]; // 触发 read
    std::cout << "    got: '" << c << "'\n";

    std::cout << "  写操作:\n";
    s[0] = 'h'; // 触发 write
    std::cout << "    result: " << s.str() << "\n";
}

} // namespace ch13


// =============================================================================
// 第14章：编译期状态检查 — 类型系统做守卫
// =============================================================================
// 利用类型系统在编译期防止非法状态转换，
// 运行期无任何检查开销。

namespace ch14 {

// --- 14.1 Phantom Types: 编译期状态标记 ---

// 状态标签（空类型，不占空间）
struct Locked {};
struct Unlocked {};

template <typename State>
class Door {
public:
    // 只有 Unlocked 的门可以打开
    // 只有 Locked 的门可以关闭

    // 这些方法只在特定状态下存在
};

// 特化：Unlocked 状态的门
template <>
class Door<Unlocked> {
public:
    Door<Locked> lock() const {
        std::cout << "  门已锁定\n";
        return Door<Locked>{};
    }

    void enter() const {
        std::cout << "  进门\n";
    }
};

// 特化：Locked 状态的门
template <>
class Door<Locked> {
public:
    Door<Unlocked> unlock() const {
        std::cout << "  门已解锁\n";
        return Door<Unlocked>{};
    }

    // 没有 enter() 方法 — 锁着的门无法进入！
};

void demo_phantom_types() {
    Door<Locked> locked_door;

    // locked_door.enter(); // ❌ 编译错误！锁着的门不能进

    auto unlocked = locked_door.unlock(); // Locked → Unlocked
    unlocked.enter();                      // ✅ OK

    auto relocked = unlocked.lock();       // Unlocked → Locked

    // unlocked.enter(); // 如果尝试使用已移动的对象，逻辑上应该避免
    // relocked.enter(); // ❌ 编译错误！

    // 零运行时开销：所有检查在编译期完成
    // Door<Locked> 和 Door<Unlocked> 都是空类
    static_assert(sizeof(Door<Locked>) == 1, "empty class");
    static_assert(sizeof(Door<Unlocked>) == 1, "empty class");
}

// --- 14.2 Builder Pattern: 编译期强制字段设置 ---

struct BuilderEmpty {};
struct BuilderHasName {};
struct BuilderHasAge {};
struct BuilderComplete {};

template <typename State>
class UserBuilder;

struct UserInfo {
    std::string name;
    int age = 0;
};

template <>
class UserBuilder<BuilderEmpty> {
public:
    UserBuilder<BuilderHasName> set_name(std::string name) {
        UserBuilder<BuilderHasName> b;
        b.info.name = std::move(name);
        return b;
    }
    // 不能 build() — 字段未设置
    friend class UserBuilder<BuilderHasName>;
};

template <>
class UserBuilder<BuilderHasName> {
public:
    UserInfo info;

    UserBuilder<BuilderComplete> set_age(int age) {
        UserBuilder<BuilderComplete> b;
        b.info = info;
        b.info.age = age;
        return b;
    }
    // 仍然不能 build() — age 未设置
    friend class UserBuilder<BuilderComplete>;
};

template <>
class UserBuilder<BuilderComplete> {
public:
    UserInfo info;

    UserInfo build() {
        return info; // ✅ 所有必填字段已设置
    }
};

void demo_builder() {
    auto user = UserBuilder<BuilderEmpty>{}
        .set_name("Alice")
        .set_age(30)
        .build();

    std::cout << "  User: " << user.name << ", age " << user.age << "\n";

    // 以下编译错误 — 跳过必填字段：
    // UserBuilder<BuilderEmpty>{}.set_name("Bob").build(); // ❌ 没有 set_age
    // UserBuilder<BuilderEmpty>{}.set_age(25);             // ❌ 没有 set_name
}

} // namespace ch14


// =============================================================================
// 第15章：零开销异常替代方案
// =============================================================================

namespace ch15 {

// C++ 异常的开销：
//   - 正常路径：几乎为零（表驱动实现）
//   - 抛出路径：非常昂贵（栈展开、RTTI 匹配）
//   - 二进制膨胀：展开表占空间
//
// 在嵌入式/游戏引擎/高频交易系统中，常常禁用异常。
// 以下是零开销的替代方案。

// --- 15.1 std::optional 替代异常 ---

std::optional<int> parse_int(std::string_view str) {
    if (str.empty()) return std::nullopt;
    int result = 0;
    int sign = 1;
    size_t start = 0;
    if (str[0] == '-') { sign = -1; start = 1; }
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') return std::nullopt;
        result = result * 10 + (str[i] - '0');
    }
    return result * sign;
}

// --- 15.2 Expected<T, E> — Rust 风格的 Result ---

template <typename T, typename E>
class Expected {
    std::variant<T, E> data_;

public:
    // 成功
    Expected(T value) : data_(std::move(value)) {}

    // 失败
    struct Error { E error; };
    Expected(Error err) : data_(std::move(err.error)) {}

    bool has_value() const { return data_.index() == 0; }
    explicit operator bool() const { return has_value(); }

    T& value() { return std::get<0>(data_); }
    const T& value() const { return std::get<0>(data_); }

    E& error() { return std::get<1>(data_); }
    const E& error() const { return std::get<1>(data_); }

    // 函数式链式调用
    template <typename Func>
    auto and_then(Func f) const -> decltype(f(value())) {
        if (has_value()) return f(value());
        return typename decltype(f(value()))::Error{error()};
    }

    template <typename Func>
    auto map(Func f) const -> Expected<decltype(f(value())), E> {
        if (has_value()) return f(value());
        return typename Expected<decltype(f(value())), E>::Error{error()};
    }
};

// 辅助函数
template <typename T, typename E>
Expected<T, E> make_expected(T val) { return val; }

template <typename T, typename E>
Expected<T, E> make_error(E err) {
    return typename Expected<T, E>::Error{std::move(err)};
}

// --- 15.3 使用示例 ---

enum class ParseError { Empty, InvalidChar, Overflow };

Expected<int, ParseError> safe_parse(std::string_view str) {
    if (str.empty())
        return Expected<int, ParseError>::Error{ParseError::Empty};

    int result = 0;
    for (char c : str) {
        if (c < '0' || c > '9')
            return Expected<int, ParseError>::Error{ParseError::InvalidChar};
        result = result * 10 + (c - '0');
    }
    return result;
}

const char* error_to_string(ParseError e) {
    switch (e) {
        case ParseError::Empty: return "empty input";
        case ParseError::InvalidChar: return "invalid character";
        case ParseError::Overflow: return "overflow";
    }
    return "unknown";
}

void demo_expected() {
    auto result1 = safe_parse("42");
    auto result2 = safe_parse("abc");

    if (result1) std::cout << "  parse('42')  = " << result1.value() << "\n";
    if (!result2) std::cout << "  parse('abc') = error: "
                            << error_to_string(result2.error()) << "\n";

    // 链式调用
    auto doubled = result1.map([](int x) { return x * 2; });
    if (doubled) std::cout << "  42 * 2 = " << doubled.value() << "\n";

    // Expected 大小与 variant 相同 — 无堆分配
    std::cout << "  sizeof(Expected<int, ParseError>) = "
              << sizeof(Expected<int, ParseError>) << "\n";
}

} // namespace ch15


// =============================================================================
// 第16章：实战 — 零开销矩阵库
// =============================================================================

namespace ch16 {

// 固定大小矩阵，全部在栈上，编译期确定大小。
// 模板参数传递维度 — 零运行时维度检查开销。

template <typename T, size_t Rows, size_t Cols>
class Matrix {
    std::array<T, Rows * Cols> data_{};

public:
    constexpr Matrix() = default;

    constexpr Matrix(std::initializer_list<T> init) {
        size_t i = 0;
        for (T val : init) {
            if (i < Rows * Cols) data_[i++] = val;
        }
    }

    constexpr T& operator()(size_t r, size_t c) {
        return data_[r * Cols + c];
    }
    constexpr const T& operator()(size_t r, size_t c) const {
        return data_[r * Cols + c];
    }

    static constexpr size_t rows() { return Rows; }
    static constexpr size_t cols() { return Cols; }

    // 矩阵加法：编译期检查维度匹配
    constexpr Matrix operator+(const Matrix& rhs) const {
        Matrix result;
        for (size_t i = 0; i < Rows * Cols; ++i)
            result.data_[i] = data_[i] + rhs.data_[i];
        return result;
    }

    // 矩阵乘法：编译期检查维度兼容性
    template <size_t OtherCols>
    constexpr Matrix<T, Rows, OtherCols> operator*(
        const Matrix<T, Cols, OtherCols>& rhs) const {
        Matrix<T, Rows, OtherCols> result;
        for (size_t i = 0; i < Rows; ++i) {
            for (size_t j = 0; j < OtherCols; ++j) {
                T sum = T{};
                for (size_t k = 0; k < Cols; ++k)
                    sum += (*this)(i, k) * rhs(k, j);
                result(i, j) = sum;
            }
        }
        return result;
    }

    // 转置
    constexpr Matrix<T, Cols, Rows> transpose() const {
        Matrix<T, Cols, Rows> result;
        for (size_t i = 0; i < Rows; ++i)
            for (size_t j = 0; j < Cols; ++j)
                result(j, i) = (*this)(i, j);
        return result;
    }

    // 单位矩阵
    static constexpr Matrix identity() {
        static_assert(Rows == Cols, "Identity matrix must be square");
        Matrix m;
        for (size_t i = 0; i < Rows; ++i) m(i, i) = T{1};
        return m;
    }

    void print(const char* label = nullptr) const {
        if (label) std::cout << "  " << label << ":\n";
        for (size_t i = 0; i < Rows; ++i) {
            std::cout << "    [ ";
            for (size_t j = 0; j < Cols; ++j)
                std::cout << (*this)(i, j) << " ";
            std::cout << "]\n";
        }
    }
};

// 编译期矩阵运算！
constexpr Matrix<int, 2, 3> A = {
    1, 2, 3,
    4, 5, 6
};

constexpr Matrix<int, 3, 2> B = {
    7,  8,
    9,  10,
    11, 12
};

constexpr auto C = A * B; // 2x3 * 3x2 = 2x2，编译期完成

static_assert(C(0, 0) == 58,  "1*7+2*9+3*11");
static_assert(C(0, 1) == 64,  "1*8+2*10+3*12");
static_assert(C(1, 0) == 139, "4*7+5*9+6*11");
static_assert(C(1, 1) == 154, "4*8+5*10+6*12");

// 维度不匹配会编译错误：
// auto bad = A * A; // ❌ 2x3 * 2x3 不兼容

void demo_matrix() {
    A.print("A (2x3)");
    B.print("B (3x2)");
    C.print("A * B (2x2)");

    constexpr auto I = Matrix<double, 3, 3>::identity();
    I.print("I (3x3)");

    constexpr auto AT = A.transpose();
    AT.print("A^T (3x2)");
}

} // namespace ch16


// =============================================================================
// 第17章：实战 — 零开销单位系统 (Units Library)
// =============================================================================

namespace ch17 {

// 量纲分析在编译期完成，运行期只有裸浮点运算。
// 单位错误在编译期报告 — 不可能出现火星飞船级别的单位错误。

// 量纲用编译期整数指数表示：
// [Length^L, Time^T, Mass^M]

template <int L, int T, int M>
struct Dimension {
    static constexpr int length = L;
    static constexpr int time   = T;
    static constexpr int mass   = M;
};

// 量纲运算
template <typename D1, typename D2>
using DimMultiply = Dimension<
    D1::length + D2::length,
    D1::time   + D2::time,
    D1::mass   + D2::mass
>;

template <typename D1, typename D2>
using DimDivide = Dimension<
    D1::length - D2::length,
    D1::time   - D2::time,
    D1::mass   - D2::mass
>;

// 物理量 = 数值 + 量纲标签
template <typename Dim>
class Quantity {
    double value_;
public:
    constexpr explicit Quantity(double v) : value_(v) {}
    constexpr double get() const { return value_; }

    // 同量纲可加减
    constexpr Quantity operator+(Quantity rhs) const {
        return Quantity(value_ + rhs.value_);
    }
    constexpr Quantity operator-(Quantity rhs) const {
        return Quantity(value_ - rhs.value_);
    }

    // 标量乘
    constexpr Quantity operator*(double s) const {
        return Quantity(value_ * s);
    }

    constexpr bool operator<(Quantity rhs) const { return value_ < rhs.value_; }
    constexpr bool operator>(Quantity rhs) const { return value_ > rhs.value_; }
};

// 不同量纲的乘除 — 返回新量纲
template <typename D1, typename D2>
constexpr Quantity<DimMultiply<D1, D2>> operator*(Quantity<D1> a, Quantity<D2> b) {
    return Quantity<DimMultiply<D1, D2>>(a.get() * b.get());
}

template <typename D1, typename D2>
constexpr Quantity<DimDivide<D1, D2>> operator/(Quantity<D1> a, Quantity<D2> b) {
    return Quantity<DimDivide<D1, D2>>(a.get() / b.get());
}

// 预定义量纲
using Dimensionless = Dimension<0, 0, 0>;
using Length        = Dimension<1, 0, 0>;
using Time          = Dimension<0, 1, 0>;
using Mass          = Dimension<0, 0, 1>;
using Velocity      = Dimension<1, -1, 0>;  // m/s
using Acceleration  = Dimension<1, -2, 0>;  // m/s^2
using Force         = Dimension<1, -2, 1>;  // kg*m/s^2 = Newton

// 便利的类型别名
using Meter   = Quantity<Length>;
using Second  = Quantity<Time>;
using Kg      = Quantity<Mass>;
using MPerS   = Quantity<Velocity>;
using MPerS2  = Quantity<Acceleration>;
using Newton  = Quantity<Force>;

// 零开销证明
static_assert(sizeof(Meter)  == sizeof(double), "无额外开销");
static_assert(sizeof(Newton) == sizeof(double), "无额外开销");

// 编译期物理计算
constexpr Meter distance(100.0);
constexpr Second time_val(9.58);
constexpr auto velocity = distance / time_val;  // 自动推导为 Velocity

constexpr Kg mass(70.0);
constexpr auto acceleration = velocity / time_val; // Acceleration
constexpr auto force = mass * acceleration;         // Force = Newton!

// 以下代码无法编译 — 量纲不对！
// auto bad = distance + time_val;  // ❌ 米 + 秒 无意义
// auto bad2 = distance + force;    // ❌ 米 + 牛顿 无意义
// auto bad3 = distance * distance + force; // ❌ m² + N

void demo_units() {
    std::cout << "  距离: " << distance.get() << " m\n";
    std::cout << "  时间: " << time_val.get() << " s\n";
    std::cout << "  速度: " << velocity.get() << " m/s\n";
    std::cout << "  质量: " << mass.get() << " kg\n";
    std::cout << "  加速度: " << acceleration.get() << " m/s^2\n";
    std::cout << "  力: " << force.get() << " N\n";

    std::cout << "\n  sizeof(Newton) = " << sizeof(Newton) << " bytes"
              << " (与 double 相同: " << sizeof(double) << ")\n";
}

} // namespace ch17


// =============================================================================
// 第18章：实战 — 零开销有限状态机
// =============================================================================

namespace ch18 {

// 利用 variant + visit 实现零虚函数、零堆分配的 FSM。
// 所有状态转换在编译期类型检查。

// 状态
struct Off      { constexpr static std::string_view name = "Off"; };
struct Starting { constexpr static std::string_view name = "Starting"; int progress = 0; };
struct Running  { constexpr static std::string_view name = "Running"; double speed = 0; };
struct Stopping { constexpr static std::string_view name = "Stopping"; };
struct Error_   { constexpr static std::string_view name = "Error"; std::string_view msg; };

using State = std::variant<Off, Starting, Running, Stopping, Error_>;

// 事件
struct PowerOn    {};
struct Initialized {};
struct SetSpeed   { double speed; };
struct PowerOff   {};
struct Fault      { std::string_view message; };

// 转换：每种 (State, Event) 组合返回新 State
// 未定义的组合保持当前状态

template <typename S, typename E>
State on_event(const S& state, const E&) {
    return state; // 默认：无效转换，保持当前状态
}

// 有效转换
State on_event(const Off&, const PowerOn&)          { return Starting{0}; }
State on_event(const Starting&, const Initialized&) { return Running{0.0}; }
State on_event(const Running&, const SetSpeed& e)   { return Running{e.speed}; }
State on_event(const Running&, const PowerOff&)     { return Stopping{}; }
State on_event(const Stopping&, const Initialized&) { return Off{}; }
// 任何状态遇到 Fault 都转到 Error
State on_event(const auto&, const Fault& f)         { return Error_{f.message}; }
State on_event(const Error_&, const PowerOff&)      { return Off{}; }

// FSM 类
class Engine {
    State state_ = Off{};

public:
    template <typename Event>
    void process(const Event& event) {
        state_ = std::visit([&](const auto& s) -> State {
            return on_event(s, event);
        }, state_);
    }

    std::string_view current_state() const {
        return std::visit([](const auto& s) { return s.name; }, state_);
    }

    void print_state() const {
        std::cout << "  State: " << current_state();
        if (auto* r = std::get_if<Running>(&state_))
            std::cout << " (speed=" << r->speed << ")";
        if (auto* e = std::get_if<Error_>(&state_))
            std::cout << " (" << e->msg << ")";
        std::cout << "\n";
    }
};

void demo_fsm() {
    Engine engine;

    engine.print_state();                         // Off
    engine.process(PowerOn{});                    // → Starting
    engine.print_state();
    engine.process(Initialized{});                // → Running
    engine.print_state();
    engine.process(SetSpeed{100.0});              // → Running(100)
    engine.print_state();
    engine.process(Fault{"overheating"});          // → Error
    engine.print_state();
    engine.process(PowerOff{});                   // → Off
    engine.print_state();

    std::cout << "\n  sizeof(State) = " << sizeof(State) << " (栈上, 无堆分配)\n";
}

} // namespace ch18


// =============================================================================
// 第19章：汇编验证 — 证明零开销
// =============================================================================
//
// 以下命令可以查看生成的汇编，验证零开销：
//
// GCC:
//   g++ -std=c++17 -O2 -S -o test5.s test5.cpp
//   grep -A 20 "function_name" test5.s
//
// Clang:
//   clang++ -std=c++17 -O2 -S -emit-llvm -o test5.ll test5.cpp
//
// Compiler Explorer (推荐):
//   https://godbolt.org/
//   粘贴代码，选择 -O2，对比汇编输出
//
// 验证清单：
//
// | 抽象                | 应该等效的手写代码         | 验证方法            |
// |---------------------|--------------------------|---------------------|
// | unique_ptr          | 裸指针 + delete           | 相同汇编            |
// | range-for           | 指针循环                   | 相同汇编            |
// | StrongType<Tag, T>  | 裸 T                      | 相同汇编            |
// | optional<T>         | T + bool                  | 相同大小+汇编       |
// | constexpr 查找表    | 硬编码 const 数组          | 完全相同            |
// | CRTP                | 非虚成员函数               | 被内联              |
// | 空策略类            | 什么都不做                 | 被编译器消除        |
// | 表达式模板          | 手写单循环                 | 相同汇编            |
// | Quantity<Dim>       | 裸 double                 | 相同汇编            |
//
// 最有力的证明：在 godbolt.org 对比两个版本的汇编输出。


// =============================================================================
// 附录：零开销抽象速查表
// =============================================================================
//
// ✅ 零开销特性          替代方案                何时用
// ─────────────────────────────────────────────────────────────────
// unique_ptr            裸指针                   总是用
// array                 C 数组                   固定大小时
// string_view           const char* + size       只读字符串
// optional              指针/哨兵值              可能为空的值
// variant               union + enum             有限类型集合
// span (C++20)          指针 + 大小              数组视图
// constexpr             宏/硬编码                编译期计算
// CRTP                  虚函数                   编译期多态
// 模板策略              虚函数策略               可配置行为
// StrongType            裸类型                   防止类型混淆
// 表达式模板            手写循环                 数学库
// Quantity<Dim>         裸数值                   物理量计算
//
// ❌ 非零开销特性        开销来源                何时考虑
// ─────────────────────────────────────────────────────────────────
// virtual               vtable + 间接跳转        需要运行时多态
// std::function         类型擦除 + 堆分配        需要存储可调用对象
// 异常                  展开表 + 栈展开          错误不频繁时
// RTTI                  typeinfo 结构体          dynamic_cast
// shared_ptr            引用计数原子操作          共享所有权
// =============================================================================


// =============================================================================
// main: 运行演示
// =============================================================================
int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "===== 现代 C++ 零开销抽象教程 演示 =====\n\n";

    // 2. RAII
    std::cout << "[2] RAII 零开销:\n";
    ch2::demo_scope_guard();
    ch2::demo_unique_ptr_zero_cost();
    std::cout << "\n";

    // 3. 迭代器
    std::cout << "[3] 迭代器零开销:\n";
    ch3::demo_iterator_zero_cost();
    ch3::demo_array_view();
    std::cout << "\n";

    // 4. 模板泛型
    std::cout << "[4] 模板零开销泛型:\n";
    ch4::demo_template_zero_cost();
    std::cout << "\n";

    // 5. constexpr
    std::cout << "[5] constexpr:\n";
    ch5::dispatch("open");
    ch5::dispatch("close");
    ch5::dispatch("read");
    std::cout << "\n";

    // 6. Strong Typedef
    std::cout << "[6] Strong Typedef 类型安全:\n";
    ch6::demo_strong_type();
    std::cout << "\n";

    // 7. optional
    std::cout << "[7] std::optional:\n";
    ch7::demo_optional_zero_cost();
    std::cout << "\n";

    // 8. variant
    std::cout << "[8] std::variant vs 虚函数:\n";
    ch8::demo_variant_size();
    ch8::demo_variant_vs_virtual();
    std::cout << "\n";

    // 9. string_view
    std::cout << "[9] 零拷贝视图:\n";
    ch9::demo_csv_parse();
    ch9::demo_string_view_perf();
    std::cout << "\n";

    // 10. CRTP
    std::cout << "[10] CRTP 编译期多态:\n";
    ch10::demo_crtp();
    std::cout << "\n";

    // 11. Policy-Based Design
    std::cout << "[11] 策略模式:\n";
    ch11::demo_policy();
    std::cout << "\n";

    // 12. Expression Templates
    std::cout << "[12] 表达式模板:\n";
    ch12::demo_expression_template();
    std::cout << "\n";

    // 13. Proxy
    std::cout << "[13] Proxy 延迟求值:\n";
    ch13::demo_proxy();
    std::cout << "\n";

    // 14. 编译期状态检查
    std::cout << "[14] Phantom Types:\n";
    ch14::demo_phantom_types();
    ch14::demo_builder();
    std::cout << "\n";

    // 15. Expected
    std::cout << "[15] Expected (异常替代):\n";
    ch15::demo_expected();
    std::cout << "\n";

    // 16. 矩阵库
    std::cout << "[16] 零开销矩阵库:\n";
    ch16::demo_matrix();
    std::cout << "\n";

    // 17. 单位系统
    std::cout << "[17] 零开销单位系统:\n";
    ch17::demo_units();
    std::cout << "\n";

    // 18. 有限状态机
    std::cout << "[18] 零开销状态机:\n";
    ch18::demo_fsm();

    std::cout << "\n===== 演示完成 =====\n";
    return 0;
}
