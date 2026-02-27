// =============================================================================
// 现代 C++ 性能优化 完全指导教程与代码示例
// =============================================================================
// 标准：C++17/20
// 编译：g++ -std=c++17 -O2 -o test4 test4.cpp -lpthread
//       cl /std:c++17 /O2 /EHsc test4.cpp
//
// 目录：
//   1.  移动语义与完美转发
//   2.  内存布局与缓存友好性
//   3.  避免不必要的拷贝
//   4.  字符串优化 (SSO, string_view)
//   5.  容器选型与使用优化
//   6.  智能指针的性能考量
//   7.  内联与函数调用开销
//   8.  分支预测与无分支编程
//   9.  SIMD 风格优化与自动向量化
//  10.  内存分配优化
//  11.  多线程与无锁编程基础
//  12.  编译器优化提示 ([[likely]], restrict, PGO)
//  13.  虚函数开销与替代方案
//  14.  I/O 优化
//  15.  实战：高性能定时器与性能测量
//  16.  实战：高性能环形缓冲区
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
#include <utility>
#include <type_traits>
#include <functional>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <list>
#include <deque>
#include <cassert>
#include <atomic>
#include <thread>
#include <mutex>
#include <new>          // std::hardware_destructive_interference_size
#include <set>          // NOTE: Added for std::set in ch5

#include "portability.h" // NOTE: Added for cross-platform support

// =============================================================================
// 工具：高精度计时器
// =============================================================================
class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    Timer(const char* label) : start_(Clock::now()), label_(label) {}
    ~Timer() {
        auto end = Clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << "  [" << label_ << "] " << us << " μs\n";
    }
};

// NOTE: Added to handle errors from memory allocation
void handle_error(const char* msg) {
    std::cerr << "Error: " << msg << std::endl;
    std::terminate();
}


// =============================================================================
// 第1章：移动语义与完美转发
// =============================================================================
// 移动语义是 C++11 引入的最重要的性能特性。
// 核心思想：转移资源所有权而非拷贝。
//
// 关键规则：
// 1. 移动后的对象处于"有效但未指定"状态
// 2. 移动构造/赋值应为 noexcept（STL 容器依赖此保证）
// 3. 完美转发保持值类别 (lvalue/rvalue)

namespace ch1 {

// --- 1.1 有无移动语义的性能差异 ---

class HeavyObject {
    std::vector<double> data_;
    std::string name_;
public:
    // 构造
    explicit HeavyObject(size_t n, std::string name)
        : data_(n, 1.0), name_(std::move(name)) {}

    // 拷贝构造（昂贵）
    HeavyObject(const HeavyObject& other)
        : data_(other.data_), name_(other.name_) {
        // 拷贝 N 个 double + 字符串
    }

    // 移动构造（廉价） —— 必须 noexcept!
    HeavyObject(HeavyObject&& other) noexcept
        : data_(std::move(other.data_)),
          name_(std::move(other.name_)) {
        // O(1): 仅转移指针
    }

    // 拷贝赋值
    HeavyObject& operator=(const HeavyObject& other) {
        if (this != &other) {
            data_ = other.data_;
            name_ = other.name_;
        }
        return *this;
    }

    // 移动赋值 —— 必须 noexcept!
    HeavyObject& operator=(HeavyObject&& other) noexcept {
        data_ = std::move(other.data_);
        name_ = std::move(other.name_);
        return *this;
    }

    size_t size() const { return data_.size(); }
};

// --- 1.2 noexcept 的重要性 ---
// std::vector 在扩容时：
//   - 如果元素有 noexcept 移动 → 移动（快）
//   - 如果没有 noexcept 移动 → 拷贝（慢！保证异常安全）

void demo_noexcept_importance() {
    std::cout << "  HeavyObject move noexcept? "
              << std::boolalpha
              << std::is_nothrow_move_constructible_v<HeavyObject>
              << "\n";
    // vector 扩容时用移动而非拷贝
    std::vector<HeavyObject> vec;
    vec.reserve(2);
    vec.emplace_back(1000, "obj1");
    vec.emplace_back(1000, "obj2");
    // 下一次 push_back 触发扩容，会移动而非拷贝
    vec.emplace_back(1000, "obj3");
}

// --- 1.3 完美转发：避免中间拷贝 ---

// 工厂函数：完美转发参数到构造函数
template <typename T, typename... Args>
T make_object(Args&&... args) {
    return T(std::forward<Args>(args)...);
    // std::forward 保持参数的值类别：
    // - 左值引用 → 左值引用
    // - 右值引用 → 右值引用
}

// --- 1.4 sink parameter 模式 ---

class Logger {
    std::string prefix_;
public:
    // ❌ 旧方式：两个重载
    // void set_prefix(const std::string& p) { prefix_ = p; }
    // void set_prefix(std::string&& p) { prefix_ = std::move(p); }

    // ✅ 现代方式：按值传递 + 移动
    // 左值：1次拷贝 + 1次移动
    // 右值：2次移动（几乎免费）
    void set_prefix(std::string p) {
        prefix_ = std::move(p);
    }
};

// --- 1.5 返回值优化 (RVO/NRVO) ---

HeavyObject create_object() {
    HeavyObject obj(10000, "created");
    // NRVO: 编译器直接在调用者栈帧构造，无拷贝无移动
    return obj;
    // 不要写 return std::move(obj); —— 这会阻止 NRVO!
}

void demo_move_semantics() {
    std::cout << "\n  -- 移动 vs 拷贝 --\n";

    HeavyObject a(100000, "original");

    {
        Timer t("拷贝 100K doubles");
        HeavyObject b = a; // 拷贝
        // do_not_optimize(b);
    }

    {
        Timer t("移动 100K doubles");
        HeavyObject c = std::move(a); // 移动
        // do_not_optimize(c);
    }
}

} // namespace ch1


// =============================================================================
// 第2章：内存布局与缓存友好性
// =============================================================================
// 现代 CPU 的内存访问模式：
//   L1 cache: ~1 ns (32-64 KB)
//   L2 cache: ~3-5 ns (256 KB - 1 MB)
//   L3 cache: ~10-20 ns (数 MB)
//   主内存:   ~60-100 ns
//
// 缓存行 (cache line) 通常为 64 字节。
// 连续内存访问 >> 随机内存访问。

namespace ch2 {

// --- 2.1 AoS vs SoA (Array of Structs vs Struct of Arrays) ---

// 方式1：AoS (Array of Structs) —— 对象粒度
struct Particle_AoS {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;
    int type;
    // 32 bytes —— 一个缓存行放 2 个
};

// 方式2：SoA (Struct of Arrays) —— 属性粒度
struct Particles_SoA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;
    std::vector<int> type;

    void resize(size_t n) {
        x.resize(n); y.resize(n); z.resize(n);
        vx.resize(n); vy.resize(n); vz.resize(n);
        mass.resize(n); type.resize(n);
    }
};

// 更新位置：只访问 x,y,z 和 vx,vy,vz
// SoA 方式下，每个数组在内存中是连续的，缓存命中率高

void update_positions_aos(std::vector<Particle_AoS>& particles, float dt) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

void update_positions_soa(Particles_SoA& p, float dt, size_t n) {
    // 连续内存访问，对 SIMD 和预取友好
    for (size_t i = 0; i < n; ++i) p.x[i] += p.vx[i] * dt;
    for (size_t i = 0; i < n; ++i) p.y[i] += p.vy[i] * dt;
    for (size_t i = 0; i < n; ++i) p.z[i] += p.vz[i] * dt;
}

void demo_aos_vs_soa() {
    constexpr size_t N = 1'000'000;

    // AoS
    std::vector<Particle_AoS> aos(N);
    for (size_t i = 0; i < N; ++i) {
        aos[i] = {float(i), float(i), float(i),
                  1.0f, 1.0f, 1.0f, 1.0f, 0};
    }

    // SoA
    Particles_SoA soa;
    soa.resize(N);
    for (size_t i = 0; i < N; ++i) {
        soa.x[i] = soa.y[i] = soa.z[i] = float(i);
        soa.vx[i] = soa.vy[i] = soa.vz[i] = 1.0f;
    }

    {
        Timer t("AoS 更新 100万粒子");
        for (int iter = 0; iter < 10; ++iter)
            update_positions_aos(aos, 0.016f);
    }
    {
        Timer t("SoA 更新 100万粒子");
        for (int iter = 0; iter < 10; ++iter)
            update_positions_soa(soa, 0.016f, N);
    }
}

// --- 2.2 数据对齐与 padding ---

// ❌ 差：字段顺序导致大量 padding
struct BadLayout {
    char a;      // 1 byte + 7 padding
    double b;    // 8 bytes
    char c;      // 1 byte + 3 padding
    int d;       // 4 bytes
    char e;      // 1 byte + 7 padding
    // Total: 可能 32 bytes
};

// ✅ 好：按大小降序排列，减少 padding
struct GoodLayout {
    double b;    // 8 bytes
    int d;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    char e;      // 1 byte + 1 padding
    // Total: 可能 16 bytes
};

// --- 2.3 缓存行对齐，避免 false sharing ---

// False sharing: 两个线程修改同一缓存行的不同变量
// 解决方法：让变量在不同的缓存行上

struct alignas(64) CacheLineAligned {
    std::atomic<int> counter{0};
    // 64 字节对齐 → 独占一个缓存行
};

// C++17 方式：
// constexpr size_t CACHE_LINE = std::hardware_destructive_interference_size;
constexpr size_t CACHE_LINE = std::hardware_destructive_interference_size; // NOTE: Use standard constant

struct PaddedCounter {
    alignas(CACHE_LINE) std::atomic<int> counter{0};
    char padding[CACHE_LINE - sizeof(std::atomic<int>)]; // 填充到整个缓存行
};

void demo_layout() {
    std::cout << "  sizeof(BadLayout)  = " << sizeof(BadLayout) << " bytes\n";
    std::cout << "  sizeof(GoodLayout) = " << sizeof(GoodLayout) << " bytes\n";
    std::cout << "  节省 " << (sizeof(BadLayout) - sizeof(GoodLayout)) << " bytes/object\n";
}

} // namespace ch2


// =============================================================================
// 第3章：避免不必要的拷贝
// =============================================================================

namespace ch3 {

// --- 3.1 引用传参 vs 值传参 ---

// ❌ 差：按值传递大对象
void process_bad(std::vector<int> data) {
    // 每次调用拷贝整个 vector
    long sum = 0;
    for (int x : data) sum += x;
    // do_not_optimize(sum);
}

// ✅ 好：const 引用
void process_good(const std::vector<int>& data) {
    long sum = 0;
    for (int x : data) sum += x;
    // do_not_optimize(sum);
}

// ✅ 更好：span 或迭代器（编译期确定范围）
// C++20: void process_best(std::span<const int> data);

// --- 3.2 循环中的临时对象 ---

void loop_optimization_demo() {
    std::vector<std::string> names = {"Alice", "Bob", "Charlie", "David"};

    // ❌ 差：每次循环创建 string 临时对象
    // for (int i = 0; i < 1000; ++i) {
    //     std::string greeting = "Hello, " + names[i % 4] + "!";
    //     do_not_optimize(greeting);
    // }

    // ✅ 好：预分配 + 复用
    std::string greeting;
    greeting.reserve(64);
    for (int i = 0; i < 1000; ++i) {
        greeting.clear();
        greeting += "Hello, ";
        greeting += names[i % 4];
        greeting += "!";
        // do_not_optimize(greeting);
    }
}

// --- 3.3 emplace vs push ---

void emplace_demo() {
    struct Widget {
        std::string name;
        int value;
        Widget(std::string n, int v) : name(std::move(n)), value(v) {}
    };

    std::vector<Widget> widgets;
    widgets.reserve(100);

    // ❌ push_back: 构造临时对象 → 移动到容器
    widgets.push_back(Widget("item1", 1));

    // ✅ emplace_back: 直接在容器内存上构造
    widgets.emplace_back("item2", 2);
    // 省去了一次移动构造
}

// --- 3.4 auto& 捕获陷阱 ---

void auto_traps() {
    std::unordered_map<std::string, int> map = {
        {"one", 1}, {"two", 2}, {"three", 3}
    };

    // ❌ 差：pair 的 key 被拷贝！
    // for (auto elem : map) { ... }  // 拷贝每个 pair

    // ❌ 隐蔽的拷贝：类型不匹配导致隐式转换
    // for (const std::pair<std::string, int>& elem : map) { ... }
    // 注意：map 的 value_type 是 pair<const string, int>
    // 这里 pair<string, int> ≠ pair<const string, int>，会创建临时对象！

    // ✅ 正确：
    for (const auto& [key, value] : map) {
        // do_not_optimize(value);
    }
}

} // namespace ch3


// =============================================================================
// 第4章：字符串优化
// =============================================================================

namespace ch4 {

// --- 4.1 SSO (Small String Optimization) ---
// 大多数 std::string 实现对短字符串不分配堆内存。
// 典型阈值：15-22 字节（取决于实现）。

void demo_sso() {
    // 短字符串：存在栈上的内联缓冲区
    std::string short_str = "hello";           // SSO: 无堆分配
    std::string long_str(100, 'x');            // 堆分配

    std::cout << "  SSO threshold test:\n";
    for (int len = 1; len <= 32; ++len) {
        std::string s(len, 'a');
        // 简易检测：如果数据指针在对象自身范围内，则是 SSO
        const void* obj_start = &s;
        const void* obj_end = reinterpret_cast<const char*>(&s) + sizeof(s);
        const void* data = s.data();
        bool is_sso = (data >= obj_start && data < obj_end);
        if (len <= 22 || !is_sso) {
            std::cout << "    len=" << len << " SSO=" << std::boolalpha << is_sso << "\n";
        }
    }
}

// --- 4.2 string_view 避免拷贝 ---

// ❌ 差：创建子串需要分配内存
std::string get_extension_bad(const std::string& filename) {
    auto pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos); // 分配新字符串
}

// ✅ 好：string_view 零拷贝
std::string_view get_extension_good(std::string_view filename) {
    auto pos = filename.rfind('.');
    if (pos == std::string_view::npos) return {};
    return filename.substr(pos); // 仅调整指针和长度
}

// --- 4.3 字符串拼接优化 ---

std::string build_path_bad(const std::string& dir, const std::string& file) {
    // ❌ 每次 + 运算都可能分配内存
    return dir + "/" + file;
}

std::string build_path_good(std::string_view dir, std::string_view file) {
    // ✅ 预分配 + 追加
    std::string result;
    result.reserve(dir.size() + 1 + file.size());
    result += dir;
    result += '/';
    result += file;
    return result;
}

void demo_string_perf() {
    constexpr int N = 100000;

    {
        Timer t("string substr (拷贝)");
        for (int i = 0; i < N; ++i) {
            auto ext = get_extension_bad("document.txt");
            // do_not_optimize(ext);
        }
    }
    {
        Timer t("string_view substr (零拷贝)");
        for (int i = 0; i < N; ++i) {
            auto ext = get_extension_good("document.txt");
            // do_not_optimize(ext);
        }
    }
}

} // namespace ch4


// =============================================================================
// 第5章：容器选型与使用优化
// =============================================================================

namespace ch5 {

// --- 5.1 容器性能特征速查 ---
//
// | 容器          | 随机访问 | 插入/删除 | 缓存友好 | 备注              |
// |---------------|----------|-----------|----------|-------------------|
// | vector        | O(1)     | O(n)*     | ★★★★★  | 默认首选          |
// | array         | O(1)     | N/A       | ★★★★★  | 固定大小          |
// | deque         | O(1)     | O(1) 头尾 | ★★★☆☆  | 分段连续          |
// | list          | O(n)     | O(1)      | ★☆☆☆☆  | 几乎不用          |
// | map           | O(log n) | O(log n)  | ★★☆☆☆  | 有序需求时用      |
// | unordered_map | O(1)     | O(1)      | ★★★☆☆  | 默认哈希表选择    |
// | flat_map(C++23)| O(log n)| O(n)     | ★★★★☆  | 小数据集最快      |

// --- 5.2 vector 使用优化 ---

void vector_optimizations() {
    // (a) reserve 预分配 —— 避免多次重新分配
    {
        Timer t("无 reserve");
        std::vector<int> v;
        for (int i = 0; i < 1000000; ++i)
            v.push_back(i);
        // do_not_optimize(v);
    }
    {
        Timer t("有 reserve");
        std::vector<int> v;
        v.reserve(1000000);
        for (int i = 0; i < 1000000; ++i)
            v.push_back(i);
        // do_not_optimize(v);
    }

    // (b) shrink_to_fit 释放多余内存
    std::vector<int> v(1000);
    v.clear();
    std::cout << "  clear 后 capacity = " << v.capacity() << "\n";
    v.shrink_to_fit();
    std::cout << "  shrink 后 capacity = " << v.capacity() << "\n";
}

// --- 5.3 vector<bool> 陷阱 ---

// vector<bool> 是特化版本，每个 bool 占 1 bit！
// 问题：不能取元素地址，proxy 对象导致性能问题
// 替代方案：vector<char> 或 bitset

void vector_bool_trap() {
    std::vector<bool> vb(1000, true);
    // auto& ref = vb[0]; // ❌ 编译错误！返回的是 proxy，不是引用

    // 替代方案：
    std::vector<char> vc(1000, 1); // 每个 char 占 1 byte，但行为正常
    auto& ref = vc[0]; // ✅ OK
    // do_not_optimize(ref);
}

// --- 5.4 erase-remove 惯用法 ---

void erase_remove_demo() {
    std::vector<int> v = {1, 2, 3, 2, 5, 2, 7};

    // C++17 方式：
    v.erase(std::remove(v.begin(), v.end(), 2), v.end());
    // 结果：{1, 3, 5, 7}

    // C++20 更简洁：
    // std::erase(v, 2); // 或 std::erase_if(v, pred);

    std::cout << "  after erase 2: ";
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";
}

// --- 5.5 unordered_map 优化 ---

void map_optimization() {
    constexpr int N = 100000;

    // (a) 预设 bucket 数量
    std::unordered_map<int, int> map;
    map.reserve(N); // 减少 rehash

    for (int i = 0; i < N; ++i)
        map[i] = i * i;

    // (b) 查找避免二次查找
    auto it = map.find(42);
    if (it != map.end()) {
        // ✅ 直接用迭代器，避免 map[42] 的二次 hash
        std::cout << "  map[42] = " << it->second << "\n";
    }

    // (c) C++17: try_emplace 避免不必要的构造
    map.try_emplace(99999, 42); // 如果 key 存在，不构造 value
}

// --- 5.6 排序 vector vs map/set ---

void sorted_vector_vs_set() {
    constexpr int N = 10000;
    constexpr int LOOKUPS = 100000;

    // 方式1: std::set (红黑树)
    std::set<int> s;
    for (int i = 0; i < N; ++i) s.insert(i);

    // 方式2: 排序 vector + binary_search
    std::vector<int> v(N);
    std::iota(v.begin(), v.end(), 0);
    std::sort(v.begin(), v.end()); // 已排序

    {
        Timer t("set 查找 10万次");
        int found = 0;
        for (int i = 0; i < LOOKUPS; ++i)
            found += s.count(i % N);
        do_not_optimize(found);
    }
    {
        Timer t("sorted vector 二分查找 10万次");
        int found = 0;
        for (int i = 0; i < LOOKUPS; ++i)
            found += std::binary_search(v.begin(), v.end(), i % N);
        do_not_optimize(found);
    }
}

} // namespace ch5


// =============================================================================
// 第6章：智能指针的性能考量
// =============================================================================

namespace ch6 {

// --- 6.1 unique_ptr: 零开销 ---

struct Resource {
    int data[256];
    ~Resource() {}
};

void unique_ptr_demo() {
    // unique_ptr 与裸指针大小相同（无自定义 deleter 时）
    static_assert(sizeof(std::unique_ptr<Resource>) == sizeof(Resource*),
                  "unique_ptr is zero overhead");

    // 但有自定义 deleter 时可能增大
    auto custom_deleter = [](Resource* p) { delete p; };
    // lambda deleter: 空类，EBO → 仍然零开销
    // function pointer deleter: +1 指针大小
}

// --- 6.2 shared_ptr: 引用计数开销 ---

void shared_ptr_demo() {
    // shared_ptr 开销:
    // - 控制块 (16-32 bytes)：引用计数 + 弱引用计数 + deleter
    // - 每次拷贝/销毁：原子操作
    // - 额外间接寻址

    std::cout << "  sizeof(shared_ptr<int>)  = " << sizeof(std::shared_ptr<int>) << "\n";
    std::cout << "  sizeof(unique_ptr<int>)  = " << sizeof(std::unique_ptr<int>) << "\n";
    std::cout << "  sizeof(int*)             = " << sizeof(int*) << "\n";

    // ✅ make_shared: 一次分配（对象 + 控制块在同一内存块）
    auto sp1 = std::make_shared<Resource>(); // 1 次分配

    // ❌ 两次分配：
    // auto sp2 = std::shared_ptr<Resource>(new Resource());
    // 先 new Resource，再分配控制块

    // ✅ 传引用避免原子操作
    // void func(const std::shared_ptr<Resource>& sp);  // 引用，无原子操作
    // void func(std::shared_ptr<Resource> sp);          // 拷贝，2次原子操作
}

// --- 6.3 性能对比 ---

void smart_ptr_benchmark() {
    constexpr int N = 1000000;

    {
        Timer t("裸指针 创建+销毁 100万次");
        for (int i = 0; i < N; ++i) {
            auto* p = new int(i);
            do_not_optimize(*p);
            delete p;
        }
    }
    {
        Timer t("unique_ptr 创建+销毁 100万次");
        for (int i = 0; i < N; ++i) {
            auto p = std::make_unique<int>(i);
            do_not_optimize(*p);
        }
    }
    {
        Timer t("shared_ptr 创建+销毁 100万次");
        for (int i = 0; i < N; ++i) {
            auto p = std::make_shared<int>(i);
            do_not_optimize(*p);
        }
    }
}

} // namespace ch6


// =============================================================================
// 第7章：内联与函数调用开销
// =============================================================================

namespace ch7 {

// --- 7.1 函数调用的成本 ---
// 函数调用开销：
//   - 保存/恢复寄存器
//   - 参数传递 (寄存器/栈)
//   - 跳转 + 返回
//   - 可能的 cache miss
// 热路径上的小函数应该被内联。

// 强制内联
#if defined(_MSC_VER)
    #define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

// 目标函数
int target_func(int x) {
    return x * x;
}

// 禁止内联（用于基准测试或调试）
#if defined(__GNUC__) || defined(__clang__)
    #define NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define NO_INLINE __declspec(noinline)
#else
    #define NO_INLINE
#endif

FORCE_INLINE int fast_abs(int x) {
    return x >= 0 ? x : -x;
}

NO_INLINE int slow_abs(int x) {
    return x >= 0 ? x : -x;
}

void demo_inline() {
    constexpr int N = 10000000;
    volatile int result = 0;

    {
        Timer t("内联 abs 1千万次");
        for (int i = -N/2; i < N/2; ++i)
            result = fast_abs(i);
    }
    {
        Timer t("非内联 abs 1千万次");
        for (int i = -N/2; i < N/2; ++i)
            result = slow_abs(i);
    }
    do_not_optimize(result);
}

// --- 7.2 LTO (Link-Time Optimization) ---
// 编译命令：g++ -O2 -flto file1.cpp file2.cpp
// 效果：跨翻译单元内联

// --- 7.3 函数指针/std::function 的开销 ---

void demo_callback_overhead() {
    constexpr int N = 10000000;
    int result = 0;

    // 直接调用
    {
        Timer t("直接调用");
        for (int i = 0; i < N; ++i)
            result += fast_abs(i - N/2);
    }

    // 函数指针
    {
        Timer t("函数指针");
        int (*fn_ptr)(int) = slow_abs;
        for (int i = 0; i < N; ++i)
            result += fn_ptr(i - N/2);
    }

    // std::function（最慢：类型擦除 + 可能的堆分配）
    {
        Timer t("std::function");
        std::function<int(int)> fn = [](int x) { return x >= 0 ? x : -x; };
        for (int i = 0; i < N; ++i)
            result += fn(i - N/2);
    }

    // 模板（零开销：编译期确定）
    {
        Timer t("模板 lambda");
        auto fn = [](int x) { return x >= 0 ? x : -x; };
        for (int i = 0; i < N; ++i)
            result += fn(i - N/2);
    }
    do_not_optimize(result);
}

} // namespace ch7


// =============================================================================
// 第8章：分支预测与无分支编程
// =============================================================================

namespace ch8 {

// --- 8.1 分支预测基础 ---
// CPU 的分支预测器会猜测分支方向。
// 如果猜错，流水线被清空（惩罚约 15-20 个时钟周期）。
// 有序数据的条件分支几乎 100% 预测正确。
// 随机数据的条件分支预测准确率约 50%。

void demo_branch_prediction() {
    constexpr int N = 1000000;
    std::vector<int> sorted_data(N), random_data(N);

    for (int i = 0; i < N; ++i) {
        sorted_data[i] = i;
        random_data[i] = i;
    }
    // 打乱 random_data
    for (int i = N - 1; i > 0; --i) {
        int j = (i * 2654435761u) % (i + 1); // 简单伪随机
        std::swap(random_data[i], random_data[j]);
    }

    // 条件求和：只加 > N/2 的值
    auto conditional_sum = [](const std::vector<int>& data, int threshold) {
        long sum = 0;
        for (int x : data) {
            if (x > threshold) // 分支
                sum += x;
        }
        return sum;
    };

    {
        Timer t("有序数据 条件求和（预测友好）");
        auto s = conditional_sum(sorted_data, N / 2);
        do_not_optimize(s);
    }
    {
        Timer t("随机数据 条件求和（预测不友好）");
        auto s = conditional_sum(random_data, N / 2);
        do_not_optimize(s);
    }
}

// --- 8.2 无分支技巧 ---

// 无分支 abs
constexpr int branchless_abs(int x) {
    int mask = x >> 31; // 全0或全1
    return (x ^ mask) - mask;
}

// 无分支 min/max
constexpr int branchless_min(int a, int b) {
    return b + ((a - b) & ((a - b) >> 31));
}

constexpr int branchless_max(int a, int b) {
    return a - ((a - b) & ((a - b) >> 31));
}

// 无分支条件累加
long branchless_conditional_sum(const std::vector<int>& data, int threshold) {
    long sum = 0;
    for (int x : data) {
        // 无分支：用算术替代 if
        int mask = -(x > threshold); // x > threshold ? 0xFFFFFFFF : 0
        sum += (x & mask);
    }
    return sum;
}

void demo_branchless() {
    constexpr int N = 1000000;
    std::vector<int> data(N);
    for (int i = N - 1; i > 0; --i) {
        data[i] = i;
        int j = (i * 2654435761u) % (i + 1);
        std::swap(data[i], data[j]);
    }

    {
        Timer t("有分支 条件求和");
        long sum = 0;
        for (int x : data)
            if (x > N / 2) sum += x;
        do_not_optimize(sum);
    }
    {
        Timer t("无分支 条件求和");
        auto sum = branchless_conditional_sum(data, N / 2);
        do_not_optimize(sum);
    }
}

// --- 8.3 [[likely]] / [[unlikely]] (C++20) ---

int process_with_hint(int value) {
    if (value > 0) [[likely]] {
        // 大多数情况走这里
        return value * 2;
    } else [[unlikely]] {
        // 错误路径
        std::cerr << "Negative value!\n";
        return 0;
    }
}

// --- 8.4 查表替代分支 ---

// ❌ 多层 if-else
int categorize_branchy(int value) {
    if (value < 10)  return 0;
    if (value < 20)  return 1;
    if (value < 50)  return 2;
    if (value < 100) return 3;
    return 4;
}

// ✅ 查表
int categorize_table(int value) {
    // 预计算的阈值表
    static constexpr int thresholds[] = {10, 20, 50, 100};
    int category = 0;
    for (int t : thresholds) {
        category += (value >= t); // 无分支累加
    }
    return category;
}

} // namespace ch8


// =============================================================================
// 第9章：SIMD 风格优化与自动向量化
// =============================================================================

namespace ch9 {

// --- 9.1 帮助编译器自动向量化的技巧 ---

// (a) 使用简单循环，避免复杂控制流
// (b) 确保内存对齐
// (c) 使用 restrict 语义（C++中用指针参数）
// (d) 避免循环依赖

// ❌ 难以向量化
void add_arrays_bad(float* a, float* b, float* c, int n) {
    for (int i = 0; i < n; ++i) {
        if (a[i] > 0) // 条件阻碍向量化
            c[i] = a[i] + b[i];
        else
            c[i] = a[i] - b[i];
    }
}

// ✅ 易于向量化
void add_arrays_good(float* __restrict a,
                     float* __restrict b,
                     float* __restrict c, int n) {
    // __restrict: 告诉编译器指针不重叠
    for (int i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// --- 9.2 编译器提示：__restrict ---
// 告诉编译器指针指向的内存不重叠

// NOTE: Use cross-platform RESTRICT macro from portability.h
void sum_restrict(float* RESTRICT a, const float* RESTRICT b, const float* RESTRICT c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}

// --- 9.3 循环展开 ---

// 手动循环展开（编译器通常自动做）
float dot_product_unrolled(const float* a, const float* b, int n) {
    float sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    int i = 0;
    // 4路展开 → 减少循环开销 + 利用 CPU 超标量
    for (; i + 3 < n; i += 4) {
        sum0 += a[i]   * b[i];
        sum1 += a[i+1] * b[i+1];
        sum2 += a[i+2] * b[i+2];
        sum3 += a[i+3] * b[i+3];
    }
    // 处理剩余
    for (; i < n; ++i)
        sum0 += a[i] * b[i];

    return sum0 + sum1 + sum2 + sum3;
}

} // namespace ch9


// =============================================================================
// 第10章：内存分配优化
// =============================================================================

namespace ch10 {

// --- 10.1 内存池 (Memory Pool) ---
// 频繁的小对象 new/delete 效率低下。
// 内存池预先分配大块内存，快速分配/释放。

template <typename T, size_t BlockSize = 4096>
class MemoryPool {
    struct Block {
        alignas(T) char data[sizeof(T)];
    };

    std::vector<Block*> chunks_;
    char* current_chunk_ = nullptr;
    size_t current_offset_ = 0;

public:
    MemoryPool(size_t chunk_size, size_t alignment = alignof(std::max_align_t))
        : chunk_size_(chunk_size), alignment_(alignment) {}

    ~MemoryPool() {
        for (void* chunk : chunks_) {
            portable_aligned_free(chunk); // NOTE: Use portable free
        }
    }

    void* allocate(size_t size) {
        size_t aligned_size = (size + alignment_ - 1) & ~(alignment_ - 1);

        if (current_chunk_ == nullptr || current_offset_ + aligned_size > chunk_size_) {
            // NOTE: Use portable aligned allocation
            void* new_chunk = portable_aligned_alloc(alignment_, chunk_size_);
            if (!new_chunk) {
                throw std::bad_alloc();
            }
            chunks_.push_back(new_chunk);
            current_chunk_ = static_cast<char*>(new_chunk);
            current_offset_ = 0;
        }

        void* ptr = current_chunk_ + current_offset_;
        current_offset_ += aligned_size;
        return ptr;
    }

    // 不支持单独释放，只能整体销毁
    void deallocate(void* ptr, size_t size) {
        // No-op
    }
};


void demo_memory_pool() {
    constexpr int N = 1000000;
    struct SmallObj { int data[4]; };

    {
        Timer t("标准 new/delete 100万次");
        std::vector<SmallObj*> ptrs(N);
        for (int i = 0; i < N; ++i)
            ptrs[i] = new SmallObj{{i, i, i, i}};
        for (int i = 0; i < N; ++i)
            delete ptrs[i];
    }

    {
        Timer t("内存池 分配/释放 100万次");
        MemoryPool<SmallObj> pool;
        std::vector<SmallObj*> ptrs(N);
        for (int i = 0; i < N; ++i)
            ptrs[i] = pool.construct(SmallObj{{i, i, i, i}});
        for (int i = 0; i < N; ++i)
            pool.destroy(ptrs[i]);
    }
    // ...existing code...
    } catch (const std::bad_alloc&) {
        handle_error("Memory pool allocation failed"); // NOTE: Use defined error handler
    }
}

// --- 10.2 栈上分配 (alloca / VLA 替代) ---

// 小数组用 std::array 栈分配
// 大小未知但有上限时用固定大小的栈缓冲
template <size_t MaxN>
void process_small_data(const int* input, size_t n) {
    assert(n <= MaxN);
    int buffer[MaxN]; // 栈上分配
    for (size_t i = 0; i < n; ++i)
        buffer[i] = input[i] * 2;
    do_not_optimize(buffer[0]);
}

} // namespace ch10


// =============================================================================
// 第11章：多线程与无锁编程基础
// =============================================================================

namespace ch11 {

// --- 11.1 false sharing 演示 ---

struct NaiveCounters {
    std::atomic<int> counter1{0};
    std::atomic<int> counter2{0};
    // 两个 counter 在同一个缓存行上 → false sharing
};

struct PaddedCounters {
    alignas(64) std::atomic<int> counter1{0};
    alignas(64) std::atomic<int> counter2{0};
    // 每个 counter 独占一个缓存行
};

void demo_false_sharing() {
    false_sharing_test<NaiveCounters>("false sharing (共享缓存行)");
    false_sharing_test<PaddedCounters>("无 false sharing (独立缓存行)");
}

// --- 11.2 原子操作的内存序 ---

// memory_order 从宽松到严格：
// - relaxed:  仅保证原子性
// - acquire:  读操作。之后的读写不能被重排到此之前
// - release:  写操作。之前的读写不能被重排到此之后
// - acq_rel:  读修改写。同时 acquire + release
// - seq_cst:  全序一致（默认，最严格，最慢）

class SpinLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 自旋等待
            // 可以加 _mm_pause() 或 yield 减少功耗
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};

// --- 11.3 无锁栈 (Lock-Free Stack) ---

template <typename T>
class LockFreeStack {
    struct Node {
        T value;
        Node* next;
    };
    std::atomic<Node*> head_{nullptr};

public:
    void push(T value) {
        Node* new_node = new Node{std::move(value), nullptr};
        new_node->next = head_.load(std::memory_order_relaxed);
        // CAS 循环
        while (!head_.compare_exchange_weak(
            new_node->next, new_node,
            std::memory_order_release,
            std::memory_order_relaxed))
        {
            // 失败时 new_node->next 已被更新为最新的 head
        }
    }

    std::optional<T> pop() {
        Node* old_head = head_.load(std::memory_order_relaxed);
        while (old_head &&
               !head_.compare_exchange_weak(
                   old_head, old_head->next,
                   std::memory_order_acquire,
                   std::memory_order_relaxed))
        {
            // 重试
        }
        if (old_head) {
            T value = std::move(old_head->value);
            delete old_head; // 注意：生产环境需要防 ABA 问题
            return value;
        }
        return std::nullopt;
    }
};

void demo_lock_free() {
    LockFreeStack<int> stack;
    constexpr int N = 100000;

    // 多线程 push
    std::thread producer([&]() {
        for (int i = 0; i < N; ++i)
            stack.push(i);
    });

    // 多线程 pop
    std::thread consumer([&]() {
        int count = 0;
        while (count < N) {
            if (stack.pop().has_value())
                ++count;
        }
    });

    producer.join();
    consumer.join();
    std::cout << "  无锁栈: " << N << " 次 push/pop 完成\n";
}

} // namespace ch11


// =============================================================================
// 第12章：编译器优化提示
// =============================================================================

namespace ch12 {

// --- 12.1 __builtin_expect (GCC/Clang) ---

#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

int process_with_likelihood(int value) {
    if (LIKELY(value > 0)) {
        return value * 2;        // 热路径
    } else {
        return handle_error();   // 冷路径
    }
}

static int handle_error() { return -1; }

// --- 12.2 __restrict 指针不重叠保证 ---

// 告诉编译器 dst 和 src 不重叠 → 允许更激进的优化
void fast_copy(float* __restrict dst,
               const float* __restrict src, int n) {
    for (int i = 0; i < n; ++i)
        dst[i] = src[i];
}

// --- 12.3 PGO (Profile-Guided Optimization) ---
//
// 步骤1: 插桩编译
//   g++ -fprofile-generate -O2 -o app app.cpp
//
// 步骤2: 运行程序（生成 .gcda 文件）
//   ./app < typical_input.txt
//
// 步骤3: 用收集的数据重新编译
//   g++ -fprofile-use -O2 -o app app.cpp
//
// 效果：编译器根据实际运行数据优化分支预测、内联决策等。
// 典型提升：5-15%

// --- 12.4 常用编译选项 ---
//
// 基础优化:
//   -O2              推荐的生产级优化
//   -O3              更激进 (循环展开、向量化)
//   -Os              优化大小
//
// 高级优化:
//   -flto             链接时优化 (跨文件内联)
//   -march=native     使用当前 CPU 的全部指令集
//   -ffast-math       放宽浮点精度 (可能改变结果!)
//   -funroll-loops    循环展开
//   -fomit-frame-pointer  省去帧指针(多一个寄存器)
//
// 向量化:
//   -ftree-vectorize  自动向量化 (-O2 默认开启)
//   -mavx2            使用 AVX2 指令集
//   -mfma             使用 FMA 指令
//
// 诊断:
//   -fopt-info-vec    报告向量化结果
//   -fopt-info-inline 报告内联决策
//   -Rpass=loop-vectorize (Clang) 向量化报告

} // namespace ch12


// =============================================================================
// 第13章：虚函数开销与替代方案
// =============================================================================

namespace ch13 {

// --- 13.1 虚函数的开销分析 ---
// 虚函数调用的成本：
//   1. vtable 间接跳转（1次内存访问）
//   2. 无法内联（编译器看不到实际调用目标）
//   3. 分支预测困难（多态调用目标不固定）
//
// 在紧密循环中，这些开销可能很显著。

// (a) 虚函数版本
struct Shape_Virtual {
    virtual double area() const = 0;
    virtual ~Shape_Virtual() = default;
};

struct Circle_V : Shape_Virtual {
    double r;
    Circle_V(double r) : r(r) {}
    double area() const override { return 3.14159265 * r * r; }
};

struct Rect_V : Shape_Virtual {
    double w, h;
    Rect_V(double w, double h) : w(w), h(h) {}
    double area() const override { return w * h; }
};

// (b) CRTP 替代
template <typename Derived>
struct Shape_CRTP {
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
};

struct Circle_C : Shape_CRTP<Circle_C> {
    double r;
    Circle_C(double r) : r(r) {}
    double area_impl() const { return 3.14159265 * r * r; }
};

struct Rect_C : Shape_CRTP<Rect_C> {
    double w, h;
    Rect_C(double w, double h) : w(w), h(h) {}
    double area_impl() const { return w * h; }
};

// (c) std::variant 替代
struct Circle_Var { double r; };
struct Rect_Var   { double w, h; };
using Shape_Variant = std::variant<Circle_Var, Rect_Var>;

double area_variant(const Shape_Variant& s) {
    return std::visit([](const auto& shape) -> double {
        using T = std::decay_t<decltype(shape)>;
        if constexpr (std::is_same_v<T, Circle_Var>)
            return 3.14159265 * shape.r * shape.r;
        else
            return shape.w * shape.h;
    }, s);
}

void demo_virtual_vs_alternatives() {
    constexpr int N = 10000000;

    // 虚函数
    {
        std::vector<std::unique_ptr<Shape_Virtual>> shapes;
        shapes.reserve(N);
        for (int i = 0; i < N; ++i) {
            if (i % 2)
                shapes.push_back(std::make_unique<Circle_V>(1.0));
            else
                shapes.push_back(std::make_unique<Rect_V>(1.0, 2.0));
        }
        Timer t("虚函数 area() 1千万次");
        double total = 0;
        for (const auto& s : shapes)
            total += s->area();
        do_not_optimize(total);
    }

    // variant
    {
        std::vector<Shape_Variant> shapes;
        shapes.reserve(N);
        for (int i = 0; i < N; ++i) {
            if (i % 2)
                shapes.emplace_back(Circle_Var{1.0});
            else
                shapes.emplace_back(Rect_Var{1.0, 2.0});
        }
        Timer t("variant area() 1千万次");
        double total = 0;
        for (const auto& s : shapes)
            total += area_variant(s);
        do_not_optimize(total);
    }

    // CRTP（同类型循环，编译器可内联）
    {
        std::vector<Circle_C> circles(N, Circle_C(1.0));
        Timer t("CRTP area() 1千万次 (同类型)");
        double total = 0;
        for (const auto& c : circles)
            total += c.area();
        do_not_optimize(total);
    }
}

} // namespace ch13


// =============================================================================
// 第14章：I/O 优化
// =============================================================================

namespace ch14 {

// --- 14.1 关闭 stdio 同步 ---

void fast_io_setup() {
    // 取消 C++ 流与 C stdio 的同步
    std::ios_base::sync_with_stdio(false);
    // 取消 cin 与 cout 的绑定
    std::cin.tie(nullptr);
    // 典型提升：2-10x
}

// --- 14.2 批量输出 ---

void demo_io_optimization() {
    constexpr int N = 100000;

    // ❌ 慢：逐行输出
    {
        Timer t("逐行 cout");
        for (int i = 0; i < N; ++i) {
            // std::cout << i << "\n"; // 每次调用可能刷新
        }
    }

    // ✅ 快：先构建字符串再输出
    {
        Timer t("批量 string 输出");
        std::string buf;
        buf.reserve(N * 8);
        for (int i = 0; i < N; ++i) {
            buf += std::to_string(i);
            buf += '\n';
        }
        // 一次性输出
        // std::cout << buf;
        do_not_optimize(buf);
    }

    // ✅ 更快：使用 printf / write
    // printf 通常比 cout 快，因为不需要处理 locale

    // --- 14.3 '\n' vs std::endl ---
    // std::endl = '\n' + flush
    // 在循环中永远不要用 std::endl！
    // ❌ std::cout << data << std::endl;
    // ✅ std::cout << data << '\n';
}

} // namespace ch14


// =============================================================================
// 第15章：实战 — 高性能定时器与性能测量
// =============================================================================

namespace ch15 {

// --- 15.1 RAII 定时器 ---
// 已在文件顶部定义 Timer 类

// --- 15.2 统计定时器：多次运行取中位数 ---

template <typename Func>
double benchmark(Func func, int iterations = 100) {
    std::vector<double> times;
    times.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        double us = std::chrono::duration<double, std::micro>(end - start).count();
        times.push_back(us);
    }

    // 取中位数（比平均值更稳定）
    std::sort(times.begin(), times.end());
    return times[iterations / 2];
}

// --- 15.3 吞吐量计算 ---

void throughput_demo() {
    constexpr size_t DATA_SIZE = 100'000'000; // 100M elements
    std::vector<int> data(DATA_SIZE);
    std::iota(data.begin(), data.end(), 0);

    auto median_us = benchmark([&]() {
        long sum = 0;
        for (int x : data) sum += x;
        do_not_optimize(sum);
    }, 20);

    double bytes_per_sec = (DATA_SIZE * sizeof(int)) / (median_us * 1e-6);
    double gb_per_sec = bytes_per_sec / (1024.0 * 1024.0 * 1024.0);

    std::cout << "  遍历 100M int: " << median_us << " μs\n";
    std::cout << "  吞吐量: " << gb_per_sec << " GB/s\n";
}

// --- 15.4 CPU 周期计数器 (x86) ---

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
inline uint64_t rdtsc() {
    unsigned int lo, hi;
    #if defined(_MSC_VER)
        return __rdtsc();
    #else
        __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
        return (static_cast<uint64_t>(hi) << 32) | lo;
    #endif
}
#endif

} // namespace ch15


// =============================================================================
// 第16章：实战 — 高性能环形缓冲区
// =============================================================================

namespace ch16 {

// 单生产者-单消费者 (SPSC) 无锁环形缓冲区
// 利用 2 的幂大小 + modulo 优化 + cache line 对齐

template <typename T, size_t N>
class SPSCRingBuffer {
    static_assert((N & (N - 1)) == 0, "N must be power of 2");

    alignas(64) std::array<T, N> buffer_{};
    alignas(64) std::atomic<size_t> write_pos_{0};
    alignas(64) std::atomic<size_t> read_pos_{0};

    static constexpr size_t MASK = N - 1;

public:
    bool try_push(const T& value) {
        size_t write = write_pos_.load(std::memory_order_relaxed);
        size_t next  = (write + 1) & MASK;
        if (next == read_pos_.load(std::memory_order_acquire))
            return false; // 满

        buffer_[write] = value;
        write_pos_.store(next, std::memory_order_release);
        return true;
    }

    bool try_pop(T& value) {
        size_t read = read_pos_.load(std::memory_order_relaxed);
        if (read == write_pos_.load(std::memory_order_acquire))
            return false; // 空

        value = buffer_[read];
        read_pos_.store((read + 1) & MASK, std::memory_order_release);
        return true;
    }

    size_t size() const {
        size_t w = write_pos_.load(std::memory_order_relaxed);
        size_t r = read_pos_.load(std::memory_order_relaxed);
        return (w - r) & MASK;
    }

    bool empty() const { return size() == 0; }
    static constexpr size_t capacity() { return N - 1; }
};

void demo_ring_buffer() {
    constexpr int N = 1000000;
    SPSCRingBuffer<int, 1024> rb;

    std::atomic<bool> done{false};
    long consumer_sum = 0;

    // 生产者线程
    std::thread producer([&]() {
        for (int i = 0; i < N; ++i) {
            while (!rb.try_push(i)) {
                // 自旋等待
            }
        }
        done.store(true, std::memory_order_release);
    });

    // 消费者线程
    std::thread consumer([&]() {
        int value;
        int count = 0;
        while (count < N) {
            if (rb.try_pop(value)) {
                consumer_sum += value;
                ++count;
            }
        }
    });

    Timer t("SPSC 环形缓冲区 100万次 push/pop");
    producer.join();
    consumer.join();

    long expected = static_cast<long>(N) * (N - 1) / 2;
    std::cout << "  sum = " << consumer_sum
              << " (expected " << expected << ")\n";
}

} // namespace ch16


// =============================================================================
// 附录：性能优化清单
// =============================================================================
//
// ✅ 移动语义
//   □ 大对象传参使用 const& 或 move
//   □ 移动构造/赋值标记 noexcept
//   □ 不要 return std::move(local) — 阻止 NRVO
//
// ✅ 内存
//   □ 按大小降序排列结构体成员 (减少 padding)
//   □ 多线程变量 alignas(64) 避免 false sharing
//   □ SoA 优于 AoS（数据导向设计）
//   □ 使用 reserve() 预分配容器
//
// ✅ 分支
//   □ 热路径减少分支
//   □ 考虑无分支替代方案
//   □ 用 [[likely]]/[[unlikely]] 给编译器提示
//
// ✅ 容器
//   □ 默认用 vector
//   □ 小数据集用 sorted vector 替代 map/set
//   □ emplace_back 替代 push_back
//   □ unordered_map::reserve() 减少 rehash
//
// ✅ 编译器
//   □ 至少 -O2 (生产环境)
//   □ -flto 链接时优化
//   □ -march=native 使用本机 CPU 指令集
//   □ 考虑 PGO（Profile-Guided Optimization）
//
// ✅ 字符串
//   □ 短字符串利用 SSO
//   □ 只读场景用 string_view
//   □ 循环中复用 string 变量
//   □ reserve + append 替代 +
//
// ✅ I/O
//   □ sync_with_stdio(false)
//   □ '\n' 替代 std::endl
//   □ 批量输出而非逐行
// =============================================================================


// =============================================================================
// main: 运行演示
// =============================================================================
int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "===== 现代 C++ 性能优化教程 演示 =====\n\n";

    // 1. 移动语义
    std::cout << "[1] 移动语义:\n";
    ch1::demo_noexcept_importance();
    ch1::demo_move_semantics();
    std::cout << "\n";

    // 2. 内存布局
    std::cout << "[2] 内存布局与缓存:\n";
    ch2::demo_layout();
    ch2::demo_aos_vs_soa();
    std::cout << "\n";

    // 3. 避免拷贝
    std::cout << "[3] 避免拷贝:\n";
    ch3::emplace_demo();
    ch5::erase_remove_demo();
    std::cout << "\n";

    // 4. 字符串
    std::cout << "[4] 字符串优化:\n";
    ch4::demo_sso();
    ch4::demo_string_perf();
    std::cout << "\n";

    // 5. 容器
    std::cout << "[5] 容器优化:\n";
    ch5::vector_optimizations();
    ch5::map_optimization();
    ch5::sorted_vector_vs_set();
    std::cout << "\n";

    // 6. 智能指针
    std::cout << "[6] 智能指针:\n";
    ch6::shared_ptr_demo();
    ch6::smart_ptr_benchmark();
    std::cout << "\n";

    // 7. 内联
    std::cout << "[7] 内联与回调:\n";
    ch7::demo_inline();
    ch7::demo_callback_overhead();
    std::cout << "\n";

    // 8. 分支
    std::cout << "[8] 分支预测:\n";
    ch8::demo_branch_prediction();
    ch8::demo_branchless();
    std::cout << "\n";

    // 9. 自动向量化
    std::cout << "[9] SIMD 向量化:\n";
    ch9::demo_vectorization();
    std::cout << "\n";

    // 10. 内存分配
    std::cout << "[10] 内存池:\n";
    ch10::demo_memory_pool();
    std::cout << "\n";

    // 11. 多线程
    std::cout << "[11] 多线程优化:\n";
    ch11::demo_false_sharing();
    ch11::demo_lock_free();
    std::cout << "\n";

    // 13. 虚函数 vs 替代方案
    std::cout << "[13] 虚函数 vs 替代方案:\n";
    ch13::demo_virtual_vs_alternatives();
    std::cout << "\n";

    // 14. I/O
    std::cout << "[14] I/O 优化:\n";
    ch14::demo_io_optimization();
    std::cout << "\n";

    // 15. 性能测量
    std::cout << "[15] 吞吐量测量:\n";
    ch15::throughput_demo();
    std::cout << "\n";

    // 16. 环形缓冲区
    std::cout << "[16] SPSC 环形缓冲区:\n";
    ch16::demo_ring_buffer();

    std::cout << "\n===== 演示完成 =====\n";
    return 0;
}
