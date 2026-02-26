# 现代 C++ 零开销抽象完全指导教程

> **"What you don't use, you don't pay for.**
> **And further: What you do use, you couldn't hand code any better."**
> — Bjarne Stroustrup
>
> **对应源码**: `test5.cpp`（2041 行，19 章，每章附零开销证明）
> **标准**: C++17/20

---

## 编译方式

```bash
# GCC / Clang
g++ -std=c++17 -O2 -Wall -Wextra -lpthread -o test5 test5.cpp && ./test5

# 查看汇编（验证零开销）
g++ -std=c++17 -O2 -S -o test5.s test5.cpp

# MSVC
cl /std:c++17 /O2 /W4 /EHsc test5.cpp
```

---

## 教程目录

| 章节 | 主题 | 核心技术 | 零开销证明方式 |
|------|------|----------|----------------|
| 1 | **零开销抽象定义与原则** | Stroustrup 原则解析 | — |
| 2 | **RAII** | unique_ptr vs 裸指针、ScopeGuard | sizeof + 基准 |
| 3 | **迭代器抽象** | range-for/index/iterator/accumulate 四等效 | 基准完全相同 |
| 4 | **模板泛型** | 模板 vs 虚函数 vs std::function 对比 | 基准测试 |
| 5 | **constexpr** | 编译期查找表、编译期字符串哈希 | 运行期零计算 |
| 6 | **Strong Typedef** | `StrongType<Tag,T>` 类型安全 | `sizeof == sizeof(T)` |
| 7 | **std::optional** | 替代空指针/哨兵值 | 零堆分配 + 基准 |
| 8 | **std::variant** | 替代虚函数的栈上多态 | variant vs virtual 基准 |
| 9 | **零拷贝视图** | string_view、编译期 CSV 解析 | 基准对比 |
| 10 | **CRTP** | 编译期多态，无 vtable | sizeof 无增长 |
| 11 | **Policy-Based Design** | 策略通过模板注入 | 空策略被编译器消除 |
| 12 | **Expression Templates** | 消除临时对象 | 基准 vs 朴素方式 |
| 13 | **Proxy/Handle** | 延迟求值、读写分离 | — |
| 14 | **编译期状态检查** | Phantom Types、类型安全 Builder | sizeof == 1 |
| 15 | **Expected** | Rust 风格 `Result<T,E>`，零异常替代 | 无堆分配 |
| 16 | **实战：零开销矩阵库** | 编译期矩阵乘法 + static_assert | constexpr 验证 |
| 17 | **实战：零开销单位系统** | 量纲分析，编译期单位检查 | `sizeof == sizeof(double)` |
| 18 | **实战：零开销状态机** | variant + visit FSM | 零堆分配 |
| 19 | **汇编验证** | godbolt.org 验证方法 | 汇编对比 |
| 附录 | **零开销抽象速查表** | 零开销 vs 非零开销分类 | — |

### 文件体系定位

| 文件 | 标准 | 定位 |
|------|------|------|
| test1.cpp | C++17/20 | 编译期优化基础 |
| test2.cpp | C++20 | 编译期优化进阶 |
| test3.cpp | 严格 C++17 | C++17 编译期专项 |
| test4.cpp | C++17/20 | 运行期性能优化 |
| **test5.cpp** | **C++17/20** | **零开销抽象——C++ 设计哲学的核心** |

### 零开销抽象的核心地位

```
┌──────────────────────────────────────────────────────────────────┐
│              C++ 性能哲学                                        │
├──────────────────────┬───────────────────────────────────────────┤
│  编译期优化 (test1-3)│  运行期优化 (test4)                       │
├──────────────────────┴───────────────────────────────────────────┤
│           ▲ 底层支撑：零开销抽象 (test5) ▲                       │
│  "抽象不是免费的，但 C++ 的抽象可以是零开销的"                    │
│                                                                  │
│  编译期: constexpr, 模板特化, CRTP, Expression Templates        │
│  运行期: RAII, unique_ptr, optional, variant, span              │
│  设计:   StrongType, Policy-Based, Phantom Types, Units         │
└──────────────────────────────────────────────────────────────────┘
```

---

## 第1章：零开销抽象的定义与原则

### 1.1 两条核心原则

1. **你不使用的特性不会产生任何运行时开销**
2. **你使用的特性，手写底层代码也不会更快**

### 1.2 与手写 C 代码的对比

| C++ 抽象 | 等效手写代码 | 结果 |
|----------|-------------|------|
| `std::sort` | 手写快排 | C++ 版同样快甚至更快 |
| `std::unique_ptr` | 裸指针 + delete | **完全相同的汇编** |
| range-for | index loop | **完全相同的汇编** |
| `std::array` | C 数组 | **完全相同的汇编** |
| `StrongType<Tag, double>` | 裸 double | **完全相同的汇编** |

### 1.3 并非所有 C++ 特性都是零开销的

| 零开销 ✅ | 非零开销 ❌ | 开销来源 |
|-----------|------------|----------|
| 模板 | 虚函数 | vtable 间接跳转 + 无法内联 |
| constexpr | std::function | 类型擦除 + 可能堆分配 |
| RAII / unique_ptr | 异常 | 栈展开表（空间） |
| 迭代器 | RTTI | typeinfo 结构体（空间） |
| CRTP | shared_ptr | 引用计数原子操作 |
| optional / variant | | |
| span / string_view | | |

### 深入扩展：与其他语言的对比

| 语言 | 泛型实现 | 抽象代价 |
|------|----------|----------|
| **C++** | 模板单态化 (monomorphization) | 零运行期开销，代码膨胀 |
| Java | 类型擦除 (type erasure) | 运行期装箱/拆箱 |
| Go | 接口 (interface) | 运行期间接调用 |
| Rust | 单态化 + trait object | 单态化零开销；dyn trait 有 vtable |
| Python | 鸭子类型 | 全运行期动态查找 |

> C++ 和 Rust 是**仅有的两门**在泛型上实现真正零开销的系统级语言。

---

## 第2章：RAII — 资源管理的零开销

### 2.1 C 风格 vs RAII

```cpp
// ❌ C 风格：手动管理，容易泄漏
void c_style() {
    int* data = (int*)malloc(100 * sizeof(int));
    if (!data) return;
    // 如果这里抛异常或提前 return → 泄漏！
    free(data);
}

// ✅ RAII：编译器保证析构，异常安全
void raii_style() {
    auto data = std::make_unique<int[]>(100);
    // 无论如何退出，析构函数都会调用
    // 生成的析构代码与 free(data) 完全相同
}
```

### 2.2 自定义 RAII 封装

```cpp
class FileHandle {
    FILE* fp_ = nullptr;
public:
    explicit FileHandle(const char* filename, const char* mode)
        : fp_(fopen(filename, mode)) {}
    ~FileHandle() { if (fp_) fclose(fp_); }

    // 禁止拷贝，允许移动
    FileHandle(const FileHandle&) = delete;
    FileHandle(FileHandle&& other) noexcept : fp_(other.fp_) {
        other.fp_ = nullptr;
    }

    FILE* get() const { return fp_; }
    explicit operator bool() const { return fp_ != nullptr; }
};
// 编译后与裸 FILE* 使用完全相同的机器码
```

### 2.3 ScopeGuard — 通用 RAII

```cpp
template <typename Func>
class ScopeGuard {
    Func func_;
    bool active_ = true;
public:
    explicit ScopeGuard(Func f) : func_(std::move(f)) {}
    ~ScopeGuard() { if (active_) func_(); }
    void dismiss() { active_ = false; }
};

// 使用：
auto guard = make_scope_guard([&]() { delete[] raw; });
// 无论如何退出当前作用域，guard 的析构都会执行 lambda
```

### 2.4 零开销证明

```cpp
static_assert(sizeof(std::unique_ptr<int>) == sizeof(int*),
              "unique_ptr is same size as raw pointer");
```

基准测试（1000 万次 new/delete）：
```
  [裸指针 new/delete] ~20000 μs
  [unique_ptr]         ~20000 μs   ← 完全相同！
  sizeof(int*)            = 8
  sizeof(unique_ptr<int>) = 8      ← 大小也相同！
```

---

## 第3章：迭代器抽象 — 与指针等效的性能

### 3.1 四种循环方式的汇编等价

```cpp
std::vector<int> data(N);

// (a) 索引循环
for (size_t i = 0; i < data.size(); ++i) sum += data[i];

// (b) 迭代器
for (auto it = data.begin(); it != data.end(); ++it) sum += *it;

// (c) range-for
for (int x : data) sum += x;

// (d) std::accumulate
long sum = std::accumulate(data.begin(), data.end(), 0L);
```

> **四者在 `-O2` 下生成完全相同的机器码。** 这就是零开销迭代器抽象。

### 3.2 自定义 ArrayView：零开销容器视图

```cpp
template <typename T>
class ArrayView {
    const T* data_;
    size_t size_;
public:
    constexpr ArrayView(const T* data, size_t size) : data_(data), size_(size) {}
    constexpr ArrayView(const std::vector<T>& vec) : data_(vec.data()), size_(vec.size()) {}

    constexpr const T* begin() const { return data_; }
    constexpr const T* end()   const { return data_ + size_; }
    constexpr const T& operator[](size_t i) const { return data_[i]; }
    constexpr ArrayView subview(size_t offset, size_t count) const {
        return ArrayView(data_ + offset, count);
    }
};

// 大小 = 1 指针 + 1 size_t
static_assert(sizeof(ArrayView<int>) == sizeof(int*) + sizeof(size_t));
```

---

## 第4章：模板 — 零开销泛型

C++ 模板在**编译期完全具体化**（monomorphization）。`generic_max<int>` 编译后与手写的 `int_max` 汇编完全相同。

### 4.1 模板 vs 虚函数 vs std::function

```cpp
// (a) 模板：编译器内联比较器 → 零开销
template <typename Compare>
void sort_template(int* data, int n, Compare cmp);

// (b) 虚函数：间接调用 → 无法内联
void sort_virtual(int* data, int n, const ICompare& cmp);

// (c) std::function：类型擦除 + 可能堆分配 → 最慢
void sort_function(int* data, int n, std::function<bool(int,int)> cmp);
```

基准测试（10000 元素排序 × 100 次）：
```
  [模板 sort (零开销)]    ~15000 μs   ← 基准
  [虚函数 sort]           ~25000 μs   ← 慢 ~1.7x
  [std::function sort]    ~40000 μs   ← 慢 ~2.7x
```

### 深入扩展：模板零开销的代价

| 优势 | 代价 |
|------|------|
| 零运行期开销 | 编译时间增加 |
| 编译器可完全内联 | 代码膨胀（每个实例一份） |
| 类型安全 | 错误信息难读（C++20 Concepts 改善） |
| 常量传播优化 | 头文件暴露实现细节 |

---

## 第5章：constexpr — 编译期零运行时开销

```cpp
// 编译期查找表 → 运行期只需数组下标访问
constexpr auto make_square_table() {
    std::array<int, 256> table{};
    for (int i = 0; i < 256; ++i) table[i] = i * i;
    return table;
}
inline constexpr auto SQUARE_TABLE = make_square_table();

// 编译期哈希 → switch/case 标签在编译期计算
constexpr uint32_t ct_hash(std::string_view sv) {
    uint32_t h = 2166136261u;
    for (char c : sv) { h ^= c; h *= 16777619u; }
    return h;
}

void dispatch(std::string_view cmd) {
    switch (ct_hash(cmd)) {
        case ct_hash("open"):  /* ... */ break;
        case ct_hash("close"): /* ... */ break;
        case ct_hash("read"):  /* ... */ break;
    }
}
// 与手写整数 case 的 switch 完全相同
```

> 详细 constexpr 教程见 test1-3，本章重点是证明其零运行期开销。

---

## 第6章：Strong Typedef — 类型安全零开销

### 6.1 问题：类型混淆

```cpp
// ❌ 所有参数都是 double，容易传错顺序
double compute_force(double mass, double acceleration, double velocity);
```

### 6.2 解决：StrongType

```cpp
template <typename Tag, typename T = double>
class StrongType {
    T value_;
public:
    constexpr explicit StrongType(T val) : value_(val) {}
    constexpr T get() const { return value_; }
    constexpr StrongType operator+(StrongType rhs) const { return StrongType(value_ + rhs.value_); }
    constexpr StrongType operator-(StrongType rhs) const { return StrongType(value_ - rhs.value_); }
    constexpr StrongType operator*(T scalar) const { return StrongType(value_ * scalar); }
};

using Meters    = StrongType<struct MetersTag>;
using Seconds   = StrongType<struct SecondsTag>;
using Kilograms = StrongType<struct KilogramsTag>;

// 类型安全的函数
constexpr MetersPerSec compute_velocity(Meters distance, Seconds time) {
    return MetersPerSec(distance.get() / time.get());
}

// ❌ 编译错误 — 类型不匹配！
// Meters wrong = Meters(1.0) + Seconds(2.0);
// auto bad = compute_velocity(time_val, dist);
```

### 6.3 零开销证明

```cpp
static_assert(sizeof(Meters) == sizeof(double), "zero overhead");
// 编译器生成的代码与直接操作 double 完全相同
```

### 深入扩展：类型安全的层次

| 方案 | 安全层级 | 运行期开销 | 灵活性 |
|------|----------|-----------|--------|
| 裸 double | 无 | 零 | 最高 |
| `using Meters = double` | typedef 别名 — **无安全性** | 零 | 最高 |
| `StrongType<Tag, double>` | 编译期类型安全 | **零** | 中等 |
| `Quantity<Dimension>` (第17章) | 量纲分析 | **零** | 低 |

---

## 第7章：std::optional — 替代指针/哨兵值的零堆分配

### 7.1 内部布局

```cpp
// optional<T> = T + bool，无堆分配
static_assert(sizeof(std::optional<int>) == sizeof(int) + sizeof(int),
              "optional<int> = 8 bytes");
```

### 7.2 替代空指针

```cpp
// ❌ 旧方式：返回指针或哨兵值
int* find_bad(std::vector<int>& data, int target);   // 返回 nullptr 表示没找到
int  find_index_bad(const int* arr, int n, int val);  // 返回 -1 表示没找到

// ✅ 零开销方式
std::optional<int> find_good(const std::vector<int>& data, int target);
constexpr std::optional<int> find_index_good(const int* arr, int n, int target);
```

### 7.3 链式访问

```cpp
struct User {
    std::string name;
    std::optional<std::string> email;
    std::optional<int> age;
};

std::optional<size_t> get_email_length(const User& user) {
    if (user.email.has_value()) return user.email->size();
    return std::nullopt;
}
```

### 7.4 性能基准

```
  [指针方式查找]     ~xxx μs
  [optional 方式查找] ~xxx μs   ← 完全相同
```

### 深入扩展：optional 使用规范

| 场景 | 推荐方案 |
|------|----------|
| 函数可能无返回值 | `std::optional<T>` |
| 延迟初始化 | `std::optional<T>` |
| 指针参数可能为空 | `T*`（language-level nullable） |
| 引用参数可能为空 | `std::optional<std::reference_wrapper<T>>` |
| 值对象默认值有意义 | 直接用默认值，不需要 optional |

---

## 第8章：std::variant — 替代虚函数的零堆分配

### 8.1 variant 的零开销特性

```
variant 特性：
  ① 大小 = max(各类型大小) + 类型索引 — 无堆分配
  ② std::visit 编译期生成跳转表 — 与 switch 等效
  ③ 数据在栈上 / vector 中连续存储 — 缓存友好
```

```cpp
struct Circle   { double r; };
struct Rect     { double w, h; };
struct Triangle { double a, b, c; };
using Shape = std::variant<Circle, Rect, Triangle>;
// variant 在栈上，与 union + enum 等效，但类型安全
```

### 8.2 Overloaded 模式

```cpp
template <typename... Fs>
struct Overloaded : Fs... { using Fs::operator()...; };
template <typename... Fs>
Overloaded(Fs...) -> Overloaded<Fs...>;

double area(const Shape& s) {
    return std::visit(Overloaded{
        [](const Circle& c)   { return 3.14159 * c.r * c.r; },
        [](const Rect& r)     { return r.w * r.h; },
        [](const Triangle& t) { /* 海伦公式 */ }
    }, s);
}
```

### 8.3 variant vs 虚函数性能对比

```
  [variant visit (连续内存)]     ~25000 μs   ← 缓存友好
  [虚函数 (堆分配, 指针追踪)]    ~55000 μs   ← 慢 2x+
```

> **variant 快 2 倍的原因**：数据连续存储在 vector 中（缓存行命中率高），而虚函数方案的每个对象通过 `unique_ptr` 堆分配（指针追踪，缓存不友好）。

### 深入扩展：variant vs 虚函数选型

| | variant | 虚函数 |
|-----|---------|--------|
| 类型集合 | **封闭**（编译期确定） | 开放（运行期可扩展） |
| 内存 | 栈/连续内存 | 堆分配 |
| 分派 | 编译期跳转表 | vtable 间接调用 |
| 新增类型 | 修改 variant 定义 | 新增派生类即可 |
| 新增操作 | 新增 visitor 即可 | 修改所有派生类 |
| 最佳场景 | 类型少，操作多 | 类型多，操作少 |

---

## 第9章：零拷贝视图 — string_view

### 9.1 string_view 零开销

```cpp
// string_view = 指针 + 长度，与 C 风格完全等效
static_assert(sizeof(std::string_view) == sizeof(const char*) + sizeof(size_t));
```

### 9.2 编译期 CSV 解析 — 零堆分配

```cpp
struct CSVRow {
    std::string_view fields[32];
    size_t count = 0;
};

constexpr CSVRow parse_csv(std::string_view line) {
    CSVRow row;
    while (!line.empty() && row.count < 32) {
        auto comma = line.find(',');
        if (comma == std::string_view::npos) {
            row.fields[row.count++] = line; break;
        }
        row.fields[row.count++] = line.substr(0, comma);
        line.remove_prefix(comma + 1);
    }
    return row;
}

// 编译期解析！
constexpr auto csv = parse_csv("Alice,30,Engineer,NYC");
static_assert(csv.count == 4);
static_assert(csv.fields[0] == "Alice");
```

### 9.3 性能基准

```
  [string 子串 (堆分配)]     ~15000 μs
  [string_view 子串 (零拷贝)] ~800 μs    ← 快 ~20x
```

---

## 第10章：CRTP — 编译期多态零虚表

```cpp
template <typename Derived>
class Serializable {
public:
    std::string to_json() const {
        return "{" + static_cast<const Derived*>(this)->fields_json() + "}";
    }
};

template <typename Derived>
class Comparable {
public:
    bool operator<(const Derived& rhs) const {
        return static_cast<const Derived*>(this)->compare_value() < rhs.compare_value();
    }
};

class Product : public Serializable<Product>,
                public Comparable<Product> {
    std::string name_;
    double price_;
public:
    Product(std::string name, double price) : name_(std::move(name)), price_(price) {}
    std::string fields_json() const { return "\"name\":\"" + name_ + "\""; }
    double compare_value() const { return price_; }
};
```

```
  sizeof(Product) = sizeof(string) + sizeof(double)
  // CRTP 基类不增加任何大小！零 vtable！
```

### 深入扩展：CRTP 使用模式

| 模式 | 用途 | 示例 |
|------|------|------|
| 静态多态 | 替代虚函数 | `Shape_CRTP<Circle>` |
| Mixin 链 | 组合能力 | `Serializable + Comparable + Cloneable` |
| 静态接口检查 | 编译期约束 | 检查 Derived 是否有 `fields_json()` |
| 递归继承 | Policy 注入 | test2.cpp 的 Policy-Based Design |

---

## 第11章：Policy-Based Design — 可组合的零开销策略

策略通过模板参数注入，编译期确定，**空策略被编译器完全消除**。

### 11.1 三种策略维度

```cpp
// (1) 线程安全策略
struct SingleThreaded { /* 空操作 — 编译器消除 */ };
struct MultiThreaded  { /* mutex lock/unlock */ };

// (2) 边界检查策略
struct NoBoundsCheck  { static void check(size_t, size_t) {} /* 空 */ };
struct BoundsChecked  { static void check(size_t i, size_t n) { if(i>=n) throw...; } };

// (3) 存储策略
template<typename T, size_t N> struct StackStorage  { T data[N]; };
template<typename T>           struct HeapStorage   { std::vector<T> data; };
```

### 11.2 策略组合

```cpp
template <typename T,
          typename ThreadPolicy = SingleThreaded,
          typename BoundsPolicy = NoBoundsCheck>
class SmartArray {
    std::vector<T> data_;
public:
    void push_back(const T& val) {
        auto lock = ThreadPolicy::lock();  // 单线程时被优化掉
        data_.push_back(val);
    }
    T& operator[](size_t i) {
        BoundsPolicy::check(i, data_.size()); // 无检查时被优化掉
        return data_[i];
    }
};

// 不同配置
using FastArray = SmartArray<int, SingleThreaded, NoBoundsCheck>;  // Release
using SafeArray = SmartArray<int, SingleThreaded, BoundsChecked>;  // Debug
using ConcurrentArray = SmartArray<int, MultiThreaded, NoBoundsCheck>;
```

> **关键**：`SingleThreaded::lock()` 和 `NoBoundsCheck::check()` 是空操作——编译器在 `-O2` 下**完全删除**这些调用，FastArray 的 `operator[]` 与裸数组访问生成相同机器码。

---

## 第12章：Expression Templates — 消除临时对象

### 12.1 问题：朴素运算符重载

```
Vec a(N), b(N), c(N);

Vec result = a + b + c;
// 朴素实现：
//   temp1 = a + b;   ← 分配 N 个 double 的临时 vector
//   temp2 = temp1 + c; ← 再分配 N 个 double
//   result = temp2;
// 两次多余的内存分配 + 三次循环！
```

### 12.2 表达式模板解决方案

```cpp
template <typename E>
struct VecExpr {
    double operator[](size_t i) const {
        return static_cast<const E&>(*this)[i];
    }
    size_t size() const { return static_cast<const E&>(*this).size(); }
};

template <typename L, typename R>
struct VecAdd : public VecExpr<VecAdd<L, R>> {
    const L& lhs; const R& rhs;
    VecAdd(const L& l, const R& r) : lhs(l), rhs(r) {}
    double operator[](size_t i) const { return lhs[i] + rhs[i]; }
    size_t size() const { return lhs.size(); }
};

class Vec : public VecExpr<Vec> {
    std::vector<double> data_;
public:
    // 从表达式构造 — 延迟求值在此发生
    template <typename E>
    Vec(const VecExpr<E>& expr) : data_(expr.size()) {
        for (size_t i = 0; i < data_.size(); ++i)
            data_[i] = expr[i]; // 一次循环求值整个表达式树！
    }
};
```

```
a + b + c 构建类型: VecAdd<VecAdd<Vec, Vec>, Vec>
赋值时一次循环: result[i] = a[i] + b[i] + c[i]
零临时对象！
```

### 12.3 基准测试

```
  [朴素方式（临时对象）]       ~8000 μs
  [表达式模板（零临时对象）]    ~3000 μs   ← 快 ~2.5x
  [复杂表达式 (a+b)*c*2.0]    ~3000 μs   ← 同样快
```

### 深入扩展：表达式模板的应用

| 库 | 语言 | 表达式模板应用 |
|----|------|---------------|
| Eigen | C++ | 线性代数，自动 SIMD |
| Blaze | C++ | 高性能矩阵运算 |
| Armadillo | C++ | MATLAB 风格线性代数 |
| xtensor | C++ | NumPy 风格多维数组 |
| Boost.uBLAS | C++ | 基础线性代数 |

> **核心思想**：运算符返回"表达式节点"而非结果值，整个表达式在赋值时通过模板内联一次性求值。编译器看到的是单个循环。

---

## 第13章：Proxy/Handle 模式 — 延迟求值

```cpp
class SmartString {
    std::string data_;

    class CharProxy {
        SmartString& str_;
        size_t index_;
    public:
        // 读取（隐式转换）
        operator char() const {
            std::cout << "[read] index " << index_ << "\n";
            return str_.data_[index_];
        }
        // 写入（赋值）
        CharProxy& operator=(char c) {
            std::cout << "[write] index " << index_ << " = '" << c << "'\n";
            str_.data_[index_] = c;
            return *this;
        }
    };
public:
    CharProxy operator[](size_t i) { return CharProxy(*this, i); }
};
```

> **典型应用**：`std::vector<bool>` 的 proxy reference、COW (Copy-On-Write) 字符串、数据库 ORM 的延迟加载。

---

## 第14章：编译期状态检查 — 类型系统做守卫

利用类型系统在**编译期**防止非法状态转换，运行期**零检查开销**。

### 14.1 Phantom Types：编译期状态标记

```cpp
struct Locked {};
struct Unlocked {};

template <>
class Door<Unlocked> {
public:
    Door<Locked> lock() const;
    void enter() const;        // ✅ 只有 Unlocked 才能 enter
};

template <>
class Door<Locked> {
public:
    Door<Unlocked> unlock() const;
    // 没有 enter()！           // ❌ Locked 的门无法进入
};

// locked_door.enter();  // ❌ 编译错误！
auto unlocked = locked_door.unlock();
unlocked.enter();         // ✅ OK

static_assert(sizeof(Door<Locked>) == 1, "空类——零运行期开销");
```

### 14.2 类型安全 Builder

```cpp
auto user = UserBuilder<BuilderEmpty>{}
    .set_name("Alice")    // → BuilderHasName
    .set_age(30)          // → BuilderComplete
    .build();             // ✅ 只有 Complete 状态才有 build()

// 以下编译错误：
// UserBuilder<BuilderEmpty>{}.set_name("Bob").build();  // ❌ 缺 age
// UserBuilder<BuilderEmpty>{}.set_age(25);               // ❌ 缺 name
```

### 深入扩展：Phantom Types 的应用

| 应用 | 状态 | 编译期约束 |
|------|------|-----------|
| 文件句柄 | Open / Closed | 只有 Open 能读写 |
| 网络连接 | Connected / Disconnected | 只有 Connected 能发送 |
| 数据库事务 | Active / Committed | 只有 Active 能操作 |
| 权限系统 | Authenticated / Anonymous | 只有 Authenticated 能访问 |
| HTML 转义 | Raw / Escaped | 只有 Escaped 才能输出 |

---

## 第15章：零开销异常替代方案

### 15.1 C++ 异常的开销分析

| | 正常路径 | 抛出路径 | 二进制 |
|------|---------|---------|--------|
| 表驱动异常 | 几乎为零 | 非常昂贵（栈展开、RTTI） | 展开表占空间 |
| `-fno-exceptions` | 零 | 不可用 | 最小 |

> 嵌入式 / 游戏引擎 / 高频交易系统常常禁用异常。

### 15.2 Expected\<T, E\> — Rust 风格 Result

```cpp
template <typename T, typename E>
class Expected {
    std::variant<T, E> data_;
public:
    Expected(T value) : data_(std::move(value)) {}
    struct Error { E error; };
    Expected(Error err) : data_(std::move(err.error)) {}

    bool has_value() const { return data_.index() == 0; }
    T& value()             { return std::get<0>(data_); }
    E& error()             { return std::get<1>(data_); }

    // 函数式链式调用
    template <typename Func>
    auto map(Func f) const -> Expected<decltype(f(value())), E>;

    template <typename Func>
    auto and_then(Func f) const -> decltype(f(value()));
};
```

### 15.3 使用示例

```cpp
enum class ParseError { Empty, InvalidChar, Overflow };

Expected<int, ParseError> safe_parse(std::string_view str) {
    if (str.empty()) return Expected<int, ParseError>::Error{ParseError::Empty};
    // ...
    return result;
}

auto result = safe_parse("42");
if (result) std::cout << result.value();

// 链式调用
auto doubled = result.map([](int x) { return x * 2; });
```

> **C++23** 正式引入 `std::expected<T, E>`，与本章实现接口基本一致。

---

## 第16章：实战 — 零开销矩阵库

固定大小矩阵，全部在**栈上**，**编译期**确定维度。

```cpp
template <typename T, size_t Rows, size_t Cols>
class Matrix {
    std::array<T, Rows * Cols> data_{};
public:
    constexpr T& operator()(size_t r, size_t c) { return data_[r * Cols + c]; }

    // 矩阵乘法：编译期检查维度兼容性
    template <size_t OtherCols>
    constexpr Matrix<T, Rows, OtherCols> operator*(
        const Matrix<T, Cols, OtherCols>& rhs) const;

    constexpr Matrix<T, Cols, Rows> transpose() const;
    static constexpr Matrix identity();
};
```

### 编译期矩阵运算

```cpp
constexpr Matrix<int, 2, 3> A = { 1,2,3, 4,5,6 };
constexpr Matrix<int, 3, 2> B = { 7,8, 9,10, 11,12 };
constexpr auto C = A * B;  // 2×3 × 3×2 = 2×2，编译期完成

static_assert(C(0,0) == 58,  "1*7+2*9+3*11");
static_assert(C(0,1) == 64,  "1*8+2*10+3*12");
static_assert(C(1,0) == 139, "4*7+5*9+6*11");
static_assert(C(1,1) == 154, "4*8+5*10+6*12");

// 维度不匹配 → 编译错误
// auto bad = A * A;  // ❌ 2×3 × 2×3 不兼容
```

### 零开销分析

| 对比项 | 裸 C 数组矩阵 | Matrix\<T,R,C\> |
|--------|---------------|------------------|
| 内存布局 | `T data[R*C]` | `array<T,R*C>` → 完全相同 |
| 下标访问 | `data[r*C+c]` | `data_[r*C+c]` → 相同 |
| 维度检查 | 无 | **编译期检查，运行期零开销** |
| 乘法维度兼容 | 手动保证 | **编译期自动检查** |

---

## 第17章：实战 — 零开销单位系统 (Units Library)

量纲分析在**编译期**完成，运行期只有**裸浮点运算**。

### 17.1 量纲表示

```cpp
template <int L, int T, int M>
struct Dimension {};  // [Length^L, Time^T, Mass^M]

template <typename D1, typename D2>
using DimMultiply = Dimension<D1::length+D2::length, D1::time+D2::time, D1::mass+D2::mass>;

template <typename D1, typename D2>
using DimDivide = Dimension<D1::length-D2::length, D1::time-D2::time, D1::mass-D2::mass>;
```

### 17.2 物理量

```cpp
template <typename Dim>
class Quantity {
    double value_;
public:
    constexpr explicit Quantity(double v) : value_(v) {}
    constexpr double get() const { return value_; }
    constexpr Quantity operator+(Quantity rhs) const { return Quantity(value_ + rhs.value_); }
    constexpr Quantity operator-(Quantity rhs) const { return Quantity(value_ - rhs.value_); }
};

// 不同量纲的乘除 — 自动推导新量纲
template <typename D1, typename D2>
constexpr Quantity<DimMultiply<D1,D2>> operator*(Quantity<D1> a, Quantity<D2> b);

template <typename D1, typename D2>
constexpr Quantity<DimDivide<D1,D2>> operator/(Quantity<D1> a, Quantity<D2> b);
```

### 17.3 预定义物理量

```cpp
using Meter  = Quantity<Dimension<1,0,0>>;   // m
using Second = Quantity<Dimension<0,1,0>>;   // s
using Kg     = Quantity<Dimension<0,0,1>>;   // kg
using MPerS  = Quantity<Dimension<1,-1,0>>;  // m/s
using Newton = Quantity<Dimension<1,-2,1>>;  // kg·m/s² = N
```

### 17.4 编译期物理计算

```cpp
constexpr Meter   distance(100.0);
constexpr Second  time_val(9.58);
constexpr auto    velocity = distance / time_val;  // → MPerS
constexpr auto    acceleration = velocity / time_val; // → m/s²
constexpr Kg      mass(70.0);
constexpr auto    force = mass * acceleration;     // → Newton!

// ❌ 编译错误 — 量纲不对！
// auto bad = distance + time_val;   // 米 + 秒 无意义
// auto bad2 = distance + force;     // 米 + 牛顿 无意义
```

### 17.5 零开销证明

```cpp
static_assert(sizeof(Meter)  == sizeof(double), "无额外开销");
static_assert(sizeof(Newton) == sizeof(double), "无额外开销");
// sizeof(Newton) = 8 bytes — 与裸 double 完全相同！
```

### 深入扩展：量纲分析的进阶

| 扩展方向 | 说明 |
|----------|------|
| 更多基本量纲 | 电流 (A)、温度 (K)、物质量 (mol)、光强度 (cd) |
| 单位前缀 | kilo-, milli-, micro- 等（编译期缩放因子） |
| 单位转换 | meter → kilometer，编译期保证转换正确 |
| 自定义字面量 | `100.0_m`、`9.58_s` — C++11 UDL |
| 生产级库 | [mp-units](https://github.com/mpusz/mp-units) — C++23 标准提案 |

---

## 第18章：实战 — 零开销有限状态机

利用 `variant` + `visit` 实现零虚函数、零堆分配的 FSM。

### 18.1 定义

```cpp
// 状态
struct Off      { constexpr static std::string_view name = "Off"; };
struct Starting { constexpr static std::string_view name = "Starting"; int progress = 0; };
struct Running  { constexpr static std::string_view name = "Running"; double speed = 0; };
struct Stopping { constexpr static std::string_view name = "Stopping"; };
struct Error_   { constexpr static std::string_view name = "Error"; std::string_view msg; };

using State = std::variant<Off, Starting, Running, Stopping, Error_>;

// 事件
struct PowerOn {};
struct Initialized {};
struct SetSpeed { double speed; };
struct PowerOff {};
struct Fault { std::string_view message; };
```

### 18.2 转换规则

```cpp
// 默认：无效转换保持当前状态
template <typename S, typename E>
State on_event(const S& state, const E&) { return state; }

// 有效转换
State on_event(const Off&, const PowerOn&)          { return Starting{0}; }
State on_event(const Starting&, const Initialized&) { return Running{0.0}; }
State on_event(const Running&, const SetSpeed& e)   { return Running{e.speed}; }
State on_event(const Running&, const PowerOff&)     { return Stopping{}; }
State on_event(const Stopping&, const Initialized&) { return Off{}; }
State on_event(const auto&, const Fault& f)         { return Error_{f.message}; }
State on_event(const Error_&, const PowerOff&)      { return Off{}; }
```

### 18.3 FSM 引擎

```cpp
class Engine {
    State state_ = Off{};
public:
    template <typename Event>
    void process(const Event& event) {
        state_ = std::visit([&](const auto& s) -> State {
            return on_event(s, event);
        }, state_);
    }
};
```

状态转换图：

```
  Off ──PowerOn──▶ Starting ──Init──▶ Running
   ▲                                    │  │
   │                                    │ SetSpeed → Running(new speed)
   │                   PowerOff ◀───────┘
   │                      │
   └──────────────────────┘
                    ↕
         任何状态 ──Fault──▶ Error_ ──PowerOff──▶ Off
```

```
  sizeof(State) = 40 (栈上，无堆分配)
```

---

## 第19章：汇编验证 — 证明零开销

### 19.1 验证方法

```bash
# GCC 生成汇编
g++ -std=c++17 -O2 -S -o test5.s test5.cpp
grep -A 20 "function_name" test5.s

# Clang 生成 LLVM IR
clang++ -std=c++17 -O2 -S -emit-llvm -o test5.ll test5.cpp

# 推荐：Compiler Explorer
# https://godbolt.org/
# 粘贴代码，选择 -O2，对比两个版本的汇编
```

### 19.2 验证清单

| 抽象 | 应该等效的手写代码 | 验证方法 |
|------|-------------------|----------|
| `unique_ptr` | 裸指针 + delete | 汇编完全相同 |
| range-for | 指针循环 | 汇编完全相同 |
| `StrongType<Tag, T>` | 裸 `T` | 汇编完全相同 |
| `optional<T>` | `T` + `bool` | 相同大小 + 汇编 |
| constexpr 查找表 | 硬编码 `const` 数组 | 完全相同 |
| CRTP 调用 | 非虚成员函数 | 被内联 |
| 空策略类 | 什么都不做 | 被编译器消除 |
| 表达式模板 | 手写单循环 | 汇编相同 |
| `Quantity<Dim>` | 裸 `double` | 汇编完全相同 |
| variant visit | switch + union | 跳转表等效 |

---

## 附录：零开销抽象速查表

### ✅ 零开销特性

| 特性 | 替代方案 | 何时用 |
|------|---------|--------|
| `unique_ptr` | 裸指针 + delete | **总是用** |
| `array` | C 数组 | 固定大小 |
| `string_view` | `const char*` + size | 只读字符串 |
| `optional` | 指针 / 哨兵值 | 可能为空的值 |
| `variant` | union + enum | 有限类型集合 |
| `span` (C++20) | 指针 + 大小 | 数组视图 |
| constexpr | 宏 / 硬编码 | 编译期计算 |
| CRTP | 虚函数 | 编译期多态 |
| 模板策略 | 虚函数策略 | 可配置行为 |
| `StrongType` | 裸类型 | 防止类型混淆 |
| 表达式模板 | 手写循环 | 数学库 |
| `Quantity<Dim>` | 裸数值 | 物理量计算 |

### ❌ 非零开销特性

| 特性 | 开销来源 | 何时使用 |
|------|---------|----------|
| `virtual` | vtable + 间接跳转 | 需要运行时多态 |
| `std::function` | 类型擦除 + 可能堆分配 | 需要存储不同类型的可调用对象 |
| 异常 | 展开表 + 栈展开 | 错误不频繁时 |
| RTTI | typeinfo 结构体 | `dynamic_cast` |
| `shared_ptr` | 引用计数原子操作 | 共享所有权 |

### 零开销抽象设计原则

```
1. 编译期能做的事不要留到运行期
2. 类型系统能检查的不要用运行时检查
3. 模板实例化优于虚函数分派
4. 值语义优于指针间接
5. 栈分配优于堆分配
6. 连续内存优于分散内存
7. 编译期约束优于运行时断言
```

---

## 运行输出示例

```
===== 现代 C++ 零开销抽象教程 演示 =====

[2] RAII 零开销:
  ScopeGuard: 资源已释放
  [裸指针 new/delete] 21543 us
  [unique_ptr]         21876 us
  sizeof(int*)            = 8
  sizeof(unique_ptr<int>) = 8

[3] 迭代器零开销:
  [索引循环]       7234 us
  [迭代器]         7198 us
  [range-for]      7201 us
  [std::accumulate] 7189 us
  ArrayView: 1 2 3 4 5 6 7 8 9 10
  subview(3,4): 4 5 6 7

[4] 模板零开销泛型:
  [模板 sort (零开销)] 15432 us
  [虚函数 sort]        25678 us
  [std::function sort]  41234 us

[5] constexpr:
  open
  close
  read

[6] Strong Typedef 类型安全:
  100m / 9.58s = 10.4384 m/s
  10m + 20m = 30m

[7] std::optional:
  [指针方式查找]     3456 us
  [optional 方式查找] 3412 us

[8] std::variant vs 虚函数:
  sizeof(Circle)  = 8
  sizeof(Rect)    = 16
  sizeof(Triangle)= 24
  sizeof(variant) = 32
  [variant visit (连续内存)]      25432 us
  [虚函数 (堆分配, 指针追踪)]     55678 us

[9] 零拷贝视图:
  CSV 解析 (5 个字段): hello | world | foo | bar | baz
  [string 子串 (堆分配)]     15432 us
  [string_view 子串 (零拷贝)]  823 us

[10] CRTP 编译期多态:
  {"name":"Widget","price":9.990000}
  {"name":"Gadget","price":19.990000}
  Widget < Gadget? true
  sizeof(Product) = 40
  sizeof(string) + sizeof(double) = 40

[11] 策略模式:
  FastArray[2] = 3
  SafeArray[2] = 3

[12] 表达式模板:
  [朴素方式（临时对象）]       8123 us
  [表达式模板（零临时对象）]    3234 us
  [复杂表达式模板]             3198 us

[13] Proxy 延迟求值:
  读操作:
    [read] index 1
    got: 'e'
  写操作:
    [write] index 0 = 'h'
    result: hello

[14] Phantom Types:
  门已解锁
  进门
  门已锁定
  User: Alice, age 30

[15] Expected (异常替代):
  parse('42')  = 42
  parse('abc') = error: invalid character
  42 * 2 = 84
  sizeof(Expected<int, ParseError>) = 8

[16] 零开销矩阵库:
  A (2x3):
    [ 1 2 3 ]
    [ 4 5 6 ]
  B (3x2):
    [ 7 8 ]
    [ 9 10 ]
    [ 11 12 ]
  A * B (2x2):
    [ 58 64 ]
    [ 139 154 ]

[17] 零开销单位系统:
  距离: 100 m
  时间: 9.58 s
  速度: 10.4384 m/s
  质量: 70 kg
  力: 76.3067 N
  sizeof(Newton) = 8 bytes (与 double 相同: 8)

[18] 零开销状态机:
  State: Off
  State: Starting
  State: Running (speed=0)
  State: Running (speed=100)
  State: Error (overheating)
  State: Off
  sizeof(State) = 40 (栈上, 无堆分配)

===== 演示完成 =====
```

---

## 核心理念

> **零开销抽象不是"低开销抽象"。** 是**真正的零**——编译后的汇编与手写 C 代码一模一样。
>
> 这是 C++ 区别于所有其他高级语言的**根本特征**。Java、Python、Go 的抽象总有运行期代价。C++ 的抽象可以做到编译后完全消失，只留下与手写代码等价的机器指令。
>
> 验证方法：在 [godbolt.org](https://godbolt.org/) 对比两个版本的汇编输出。
