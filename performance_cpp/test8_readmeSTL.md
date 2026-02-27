# 现代 C++ STL 完全教程

> **配套代码**: `test8.cpp` (1974 行, 20 章, C++17)
>
> **编译**:
> ```bash
> # Linux / macOS
> g++ -std=c++17 -O2 -o test8 test8.cpp
>
> # Windows (MSVC)
> cl /std:c++17 /O2 /EHsc test8.cpp
> ```
>
> **运行**: 自动执行全部 20 章演示，无交互

---

## 目录

| 篇 | 章 | 主题 | 核心内容 | 难度 |
|---|---|---|---|---|
| **一、容器** | 1 | 序列容器 | `vector`/`array`/`deque`/`list`/`forward_list`、容器选择指南 | ⭐⭐ |
| | 2 | 关联容器 | `set`/`map`/`multiset`/`multimap`、红黑树 | ⭐⭐ |
| | 3 | 无序容器 | `unordered_set`/`unordered_map`、自定义哈希、性能对比 | ⭐⭐⭐ |
| | 4 | 容器适配器 | `stack`/`queue`/`priority_queue`、Dijkstra | ⭐⭐ |
| | 5 | 特殊容器 | `string`/`string_view`/`bitset` | ⭐⭐ |
| | 6 | C++17 更新 | `try_emplace`/`insert_or_assign`/节点操作/`merge` | ⭐⭐⭐ |
| **二、迭代器** | 7 | 迭代器分类 | `iterator_traits`/`advance`/`distance`/`next`/`prev` | ⭐⭐ |
| | 8 | 迭代器适配器 | 反向/插入/流/移动迭代器 | ⭐⭐⭐ |
| **三、算法** | 9 | 非修改算法 | `find`/`count`/`all_of`/`search`/`equal`/`mismatch` | ⭐⭐ |
| | 10 | 修改算法 | `copy`/`transform`/`replace`/`remove`/`fill`/`shuffle` | ⭐⭐ |
| | 11 | 排序与搜索 | `sort`/`stable_sort`/`partial_sort`/`nth_element`/二分 | ⭐⭐⭐ |
| | 12 | 集合算法 | `set_union`/`set_intersection`/`set_difference` | ⭐⭐ |
| | 13 | 数值算法 | `accumulate`/`inner_product`/`partial_sum`/`iota`/`gcd`/`clamp` | ⭐⭐ |
| | 14 | 最值与排列 | `minmax`/`next_permutation`/堆操作 | ⭐⭐⭐ |
| **四、工具** | 15 | 函数对象 | 标准函子/`std::function`/Lambda (泛型、mutable、移动、IIFE) | ⭐⭐⭐ |
| | 16 | optional/variant/any | 三大 C++17 词汇类型 | ⭐⭐⭐ |
| | 17 | tuple 与绑定 | `tuple`、结构化绑定、`tie` | ⭐⭐ |
| | 18 | 智能指针 | `unique_ptr`/`shared_ptr`/`weak_ptr`、循环引用 | ⭐⭐⭐ |
| **五、实战** | 19 | STL 组合拳 | 词频统计、TopK、分组统计、矩阵转置 | ⭐⭐⭐ |
| | 20 | 性能陷阱 | `reserve`、`emplace_back`、容器选择基准测试、最佳实践 | ⭐⭐⭐⭐ |

---

## 文件关系

| 文件 | 主题 | 行数 | 章节 | 与本文件关系 |
|---|---|---|---|---|
| `test5.cpp` / `test5_readme` | 零开销抽象 | - | - | **参考** — 模板/CRTP/variant |
| `test6.cpp` / `test6_readme` | 多线程 | ~2400 | 18 章 | **参考** — 并发容器 |
| `test7.cpp` / `test7_readme` | 网络编程 | 2222 | 19 章 | **参考** — Buffer/variant 用法 |
| **`test8.cpp`** / **本文件** | **STL** | **1974** | **20 章** | **当前** |
| `test12.cpp` / `test12_readme` | 编译期优化 | - | - | **进阶** — constexpr STL |

---

## 工具函数

test8.cpp 开头定义了两个贯穿全文的工具：

```cpp
// RAII 计时器 — 自动测量代码块耗时 (微秒)
class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    Timer(const char* l) : start_(Clock::now()), label_(l) {}
    ~Timer() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
            Clock::now() - start_).count();
        std::cout << "  [" << label_ << "] " << us << " us\n";
    }
};

// 防止编译器优化掉基准测试
template <typename T>
void do_not_optimize(T&& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}
```

---

# 一、容器篇

---

## 第 1 章：序列容器

### 1.1 std::vector — 动态数组（默认选择）

**特征**: 连续内存 → 缓存友好、随机访问 O(1)、尾部增删 O(1) 摊还、中间增删 O(n)

```cpp
// 创建
std::vector<int> v1;                       // 空
std::vector<int> v2(5, 42);               // 5 个 42
std::vector<int> v3 = {1, 2, 3, 4, 5};   // 初始化列表
std::vector<int> v4(v3.begin(), v3.end()); // 范围构造

// 容量管理
v.reserve(100);     // 预分配（不改变 size）
v.shrink_to_fit();  // 释放多余内存

// 访问
v.front();     // 第一个
v.back();      // 最后一个
v[5];          // 不检查边界 → 可能 UB
v.at(5);       // 检查边界 → 可能抛 out_of_range
v.data();      // 裸指针（C 兼容）

// 修改
v.push_back(10);              // 尾部添加（可能拷贝）
v.emplace_back(11);           // 原地构造（避免拷贝）
v.pop_back();                 // 删尾
v.insert(v.begin() + 3, 99); // 在位置 3 插入
v.erase(v.begin() + 3);      // 删除位置 3
```

**vector 扩容策略：**

```
push_back 超过 capacity 时:
  1. 分配新内存 (通常 2× 当前 capacity)
  2. 把所有元素 移动/拷贝 到新内存
  3. 释放旧内存
  4. 所有迭代器/指针/引用 全部失效！

因此: reserve() 预分配是关键优化手段
```

### 1.2 std::array — 编译期固定大小

```cpp
std::array<int, 5> arr = {1, 2, 3, 4, 5};
// 与 vector 相似的接口，但大小不可变
// 栈上分配，零开销（与 C 数组完全相同的内存布局）

constexpr std::array<int, 3> ca = {10, 20, 30};
static_assert(ca.size() == 3);  // 编译期
static_assert(ca[1] == 20);
```

### 1.3 std::deque — 双端队列

```cpp
// 两端 O(1) 增删、随机访问 O(1)、内存分段连续
std::deque<int> dq = {3, 4, 5};
dq.push_front(2);  // ✅ vector 不支持!
dq.push_back(6);
dq.pop_front();
dq.pop_back();
```

### 1.4 std::list — 双向链表

```cpp
std::list<int> lst = {5, 2, 8, 1, 9, 3};
lst.sort();    // list 有自己的 sort (不能用 std::sort)
lst.unique();  // 删除相邻重复
lst.reverse();
lst.merge(other_sorted_list);  // O(n) 合并两个有序链表

// splice: O(1) 转移节点（无拷贝/无分配）
lst.splice(lst.end(), lst2);  // lst2 全部移到 lst 末尾
```

### 1.5 std::forward_list — 单向链表

```cpp
// 最小内存开销的链表（只有 next 指针）
std::forward_list<int> fl = {3, 1, 4, 1, 5};
fl.push_front(0);
fl.sort(); fl.unique();
// 注意: 没有 size()、没有 push_back()、没有 back()
// 只有 before_begin(), begin(), end()
```

### 1.6 容器选择指南

| 需求 | 推荐容器 | 原因 |
|---|---|---|
| **默认选择** | `vector` | 缓存友好、最快 |
| 固定大小 | `array` | 栈上、零开销 |
| 两端增删 | `deque` | 头尾都是 O(1) |
| 中间频繁增删 | `list` | O(1) splice/insert |
| 最小开销链表 | `forward_list` | 只有 next 指针 |

> **NOTE (补充)**:
> “默认选择 vector”在通用业务代码中通常成立，但不是绝对规则。
> 若核心操作是头部插入/删除、稳定迭代器要求或严格内存布局约束，`deque/list/array` 可能更合适。

### 1.7 深入扩展

**vector vs deque 内部结构对比：**

```
vector (连续内存):
  ┌───┬───┬───┬───┬───┬───┬───┬───┐
  │ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │
  └───┴───┴───┴───┴───┴───┴───┴───┘
  ↑                               ↑
  data()                       data()+size()

deque (分段连续):
  map → [ ptr0 | ptr1 | ptr2 | ptr3 ]
           ↓      ↓       ↓      ↓
         ┌───┐  ┌───┐  ┌───┐  ┌───┐
         │blk│  │blk│  │blk│  │blk│  每块固定大小
         └───┘  └───┘  └───┘  └───┘
```

**list::splice 的零开销特性：**
- `splice` 只修改指针，不移动/拷贝任何元素
- 是 list 相对于 vector 的**唯一真正优势**
- 用例: LRU Cache (O(1) 提升最近访问的元素到头部)

**C++20 `std::erase` / `std::erase_if` 简化：**
```cpp
// C++17: erase-remove idiom
v.erase(std::remove(v.begin(), v.end(), 2), v.end());
// C++20: 一行
std::erase(v, 2);
std::erase_if(v, [](int x) { return x > 5; });
```

---

## 第 2 章：关联容器 — 红黑树

### 2.1 基于红黑树，元素自动排序

| 容器 | 特点 | 查找/插入/删除 |
|---|---|---|
| `set` | 唯一键，有序 | O(log n) |
| `map` | 唯一键值对，按键有序 | O(log n) |
| `multiset` | 允许重复键 | O(log n) |
| `multimap` | 允许重复键值对 | O(log n) |

### 2.2 std::set

```cpp
std::set<int> s = {5, 3, 8, 1, 3, 5}; // → {1, 3, 5, 8} 自动去重+排序

auto [it, success] = s.insert(4);  // 结构化绑定返回 (迭代器, 是否成功)
s.find(3);       // 返回迭代器 (O(log n))
s.count(3);      // 0 或 1
s.lower_bound(3); // >= 3 第一个
s.upper_bound(6); // > 6 第一个
s.erase(3);       // 删除
```

### 2.3 std::map

```cpp
std::map<std::string, int> ages = {
    {"Alice", 30}, {"Bob", 25}, {"Charlie", 35}
};

ages["David"] = 28;   // 不存在 → 插入
ages["Alice"] = 31;   // 存在 → 覆盖

// ⚠️ operator[] 的陷阱
ages["Eve"];  // key 不存在 → 自动插入 Eve:0!

// 安全查找
if (auto it = ages.find("Bob"); it != ages.end())
    std::cout << it->second;

// 遍历（按 key 字典序）
for (const auto& [name, age] : ages) { ... }
```

### 2.4 multiset / multimap

```cpp
std::multiset<int> ms = {3, 1, 4, 1, 5};
ms.count(1);  // → 2

ms.erase(1);        // 删除 ALL 等于 1 的
ms.erase(ms.find(1)); // 只删除一个

// multimap — 一个 key 多个 value
std::multimap<std::string, int> scores;
scores.emplace("Alice", 90);
scores.emplace("Alice", 95);
auto [lo, hi] = scores.equal_range("Alice"); // 获取所有 Alice 的成绩
```

### 2.5 深入扩展

**红黑树 vs 跳表 vs B-tree：**

| 结构 | 查找 | 插入/删除 | 缓存友好 | 用途 |
|---|---|---|---|---|
| 红黑树 (STL) | O(log n) | O(log n) | 差 | std::set/map |
| 跳表 | O(log n) 期望 | O(log n) 期望 | 中 | Redis ZSET |
| B-tree | O(log n) | O(log n) | 好 | 数据库索引 |

**自定义比较器：**

```cpp
// set 按自定义顺序排列
auto cmp = [](const std::string& a, const std::string& b) {
    return a.size() < b.size(); // 按长度排序
};
std::set<std::string, decltype(cmp)> s(cmp);
```

---

## 第 3 章：无序容器 — 哈希表

### 3.1 平均 O(1) 的查找/插入/删除

```cpp
std::unordered_set<std::string> us = {"cat", "dog", "bird"};
us.insert("rabbit");
us.bucket_count();     // 桶数量
us.load_factor();      // 负载因子 (元素数/桶数)

std::unordered_map<std::string, int> um = {{"width", 1920}, {"height", 1080}};
um["fps"] = 60;
```

### 3.2 自定义哈希

```cpp
struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

struct PointHash {
    size_t operator()(const Point& p) const {
        size_t h1 = std::hash<int>{}(p.x);
        size_t h2 = std::hash<int>{}(p.y);
        return h1 ^ (h2 << 1);  // 简单异或组合
    }
};

std::unordered_set<Point, PointHash> points;
```

### 3.3 有序 vs 无序性能对比

代码中实测 100 万元素的插入和 50 万次查找:

```cpp
{
    Timer t("set 插入 100万");
    std::set<int> s;
    for (int i = 0; i < 1000000; ++i) s.insert(i);
}
{
    Timer t("unordered_set 插入 100万");
    std::unordered_set<int> us;
    us.reserve(1000000);  // 关键: 预分配
    for (int i = 0; i < 1000000; ++i) us.insert(i);
}
```

**典型结果：**

| 操作 | `set` (红黑树) | `unordered_set` (哈希) | 加速比 |
|---|---|---|---|
| 插入 100 万 | ~500ms | ~80ms | **6×** |
| 查找 50 万 | ~300ms | ~30ms | **10×** |

> **NOTE (修正补充)**:
> 该表是示例数据，不是跨平台固定结论。
> `unordered_*` 在极端冲突或频繁 rehash 场景下可能显著退化，工程上应结合键分布与负载因子做实测。

### 3.4 深入扩展

**哈希冲突与链地址法：**

```
bucket[0] → [key_a] → [key_b] → null
bucket[1] → [key_c] → null
bucket[2] → null
bucket[3] → [key_d] → [key_e] → [key_f] → null  ← 冲突链
```

**好的哈希组合 (boost::hash_combine 模式)：**

```cpp
template <typename T>
void hash_combine(size_t& seed, const T& val) {
    seed ^= std::hash<T>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
// 0x9e3779b9 = 黄金比例的整数近似, 提供更好的比特分散
```

**何时选有序 vs 无序：**

| 选有序 (`map`/`set`) | 选无序 (`unordered_*`) |
|---|---|
| 需要按序遍历 | 只做查找/插入 |
| 范围查询 (`lower_bound`) | 最大吞吐量 |
| 键无好哈希函数 | 键有现成哈希 |
| 需要稳定的最坏情况 | 接受偶尔 rehash |

---

## 第 4 章：容器适配器

### 4.1 适配器 ≠ 新容器

适配器只是对底层容器的**接口包装**，限制了操作集:

| 适配器 | 底层默认 | 语义 | 操作 |
|---|---|---|---|
| `stack` | `deque` | LIFO (后进先出) | `push`/`pop`/`top` |
| `queue` | `deque` | FIFO (先进先出) | `push`/`pop`/`front`/`back` |
| `priority_queue` | `vector` | 最大堆 | `push`/`pop`/`top` |

### 4.2 priority_queue — 堆

```cpp
// 默认最大堆
std::priority_queue<int> pq;
pq.push(3); pq.push(1); pq.push(5);
pq.top(); // 5 (最大值)

// 最小堆
std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;

// Dijkstra 用法
using Edge = std::pair<int, int>; // {权重, 节点}
std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> pq;
pq.push({5, 0}); pq.push({2, 1}); pq.push({8, 2});
// 弹出顺序: (2,n1), (5,n0), (8,n2) — 最短路径优先
```

### 4.3 深入扩展

**stack/queue 可以指定底层容器：**

```cpp
std::stack<int, std::vector<int>> vec_stack;     // 用 vector 做底层
std::queue<int, std::list<int>> list_queue;       // 用 list 做底层
```

**手动堆操作 (见第 14 章):**

```cpp
std::make_heap(v.begin(), v.end());  // 建堆
std::push_heap(...);                  // 插入
std::pop_heap(...);                   // 弹出
std::sort_heap(...);                  // 堆排序
// priority_queue 内部就是用这些算法实现的
```

---

## 第 5 章：特殊容器

### 5.1 std::string

```cpp
std::string s = "Hello, World!";
s.substr(7, 5);          // "World" (新分配!)
s.find("World");         // 返回位置
s.replace(pos, 5, "C++");
s += " 17/20";

// 数值转换
int n = std::stoi("42");
double d = std::stod("3.14");
std::string ns = std::to_string(n);
```

### 5.2 std::string_view (C++17)

```cpp
std::string original = "Hello, Modern C++!";
std::string_view sv = original;

auto word = sv.substr(7, 6); // "Modern" — 零拷贝! 不分配内存!
sv.remove_prefix(1);         // 去掉前缀 (只是移动指针)

sizeof(std::string)      // 32 (SSO + heap pointer + size + capacity)
sizeof(std::string_view) // 16 (指针 + 长度, 就这么多)
```

### 5.3 std::bitset

```cpp
std::bitset<8> b1("10110011");
std::bitset<8> b2(0xAB);

b1 & b2;      // AND
b1 | b2;      // OR
~b1;           // NOT
b1 << 2;      // 左移
b1.count();   // popcount
b1.flip(0);   // 翻转第 0 位
b1.to_ulong(); b1.to_string();

std::bitset<256> big;  // 大 bitset 也没问题
big.set(0); big.set(100); big.set(255);
```

### 5.4 深入扩展

**string SSO (Small String Optimization):**

```
sizeof(std::string) = 32 字节 (典型实现)

短字符串 (≤ ~15字符):
  ┌──────────────────────────────┐
  │ 直接存在栈上 (无堆分配!) SSO │
  └──────────────────────────────┘

长字符串:
  ┌──────┐
  │ ptr ──→ [堆上的字符数据...]
  │ size │
  │ cap  │
  └──────┘
```

**string_view 的陷阱:**
- **不拥有**底层数据 → 底层 string 析构后 string_view 悬空!
- 不以 null 结尾 → 不能直接传给 C 函数!

---

## 第 6 章：C++17 容器更新

### 6.1 try_emplace — 避免不必要的构造

```cpp
std::map<std::string, std::string> m;
m.try_emplace("key1", "value1");     // ✅ 插入
m.try_emplace("key1", "IGNORED");    // key 已存在 → value 不会被构造!
// vs emplace("key1", "IGNORED"): 会先构造临时 string 再发现 key 重复
```

### 6.2 insert_or_assign — 更好的 operator[]

```cpp
auto [it, inserted] = m.insert_or_assign("key2", "first");
// inserted = true (新插入)
auto [it2, inserted2] = m.insert_or_assign("key2", "second");
// inserted2 = false (覆盖)
// 比 m["key2"] = "second" 多了一个 inserted 信息
```

### 6.3 extract + insert — 零拷贝节点操作

```cpp
std::map<int, std::string> src = {{1, "one"}, {2, "two"}, {3, "three"}};
std::map<int, std::string> dst;

auto node = src.extract(2);    // 从 src 取出节点 (不拷贝!)
node.key() = 20;               // 可以修改 key!! (通常不可能)
dst.insert(std::move(node));   // 插入到 dst (不拷贝!)
// src = {1:"one", 3:"three"}, dst = {20:"two"}
```

### 6.4 merge — 容器合并

```cpp
std::map<int, std::string> a = {{1, "a"}, {2, "b"}};
std::map<int, std::string> b = {{2, "B"}, {3, "C"}};
a.merge(b);
// a = {1:"a", 2:"b", 3:"C"}
// b = {2:"B"}  ← key=2 在 a 中已存在, 留在 b
```

### 6.5 深入扩展

**extract 的独特能力 — 修改 set 的 key：**

```cpp
std::set<std::string> s = {"apple", "banana", "cherry"};
auto node = s.extract("banana");
node.value() = "blueberry";  // 修改! (set 元素通常不可变)
s.insert(std::move(node));
// s = {"apple", "blueberry", "cherry"}
```

**C++17 容器操作总结：**

| 操作 | 用途 | 避免 |
|---|---|---|
| `try_emplace` | key 不存在才构造 value | 不必要的构造 |
| `insert_or_assign` | 覆盖或插入 | `operator[]` 默认构造 |
| `extract` + `insert` | 零拷贝节点转移 | 拷贝 + 删除 + 插入 |
| `merge` | 批量节点合并 | 循环 insert |

---

# 二、迭代器篇

---

## 第 7 章：迭代器分类与特征

### 7.1 迭代器层次

```
强
 ↑  ContiguousIterator   — 连续内存 (vector, array, string) [C++20]
 │  RandomAccessIterator  — 随机访问 (vector, deque, array)
 │  BidirectionalIterator — 双向     (list, set, map)
 │  ForwardIterator       — 多遍前向 (forward_list)
 │  InputIterator         — 单遍读取 (istream_iterator)
 ↓  OutputIterator        — 单遍写入 (ostream_iterator)
弱
```

### 7.2 iterator_traits

```cpp
// 通过 traits 判断迭代器类型
using VecIter = std::vector<int>::iterator;
constexpr bool is_random = std::is_same_v<
    std::iterator_traits<VecIter>::iterator_category,
    std::random_access_iterator_tag>;  // true

// 导航函数 (适用所有迭代器类型)
std::advance(it, 3);     // 前进 3 步 (随机 O(1), 双向 O(n))
std::distance(a, b);     // 距离
std::next(it, 2);        // 返回 it+2 的拷贝
std::prev(it, 1);        // 返回 it-1 的拷贝
```

### 7.3 深入扩展

**为什么 `std::sort` 要求 RandomAccess 而 `list::sort` 不是 `std::sort`：**
- `std::sort` 基于 IntroSort (快排+堆排+插入排序混合)
- 需要**随机访问**来做 partition 和 median-of-three
- `list::sort` 用的是**归并排序** (只需要前向/双向, 且 splice O(1))

**C++20 Concepts 替代 iterator_traits：**

```cpp
// C++20
template <std::random_access_iterator Iter>
void my_sort(Iter first, Iter last);  // 编译时约束
```

---

## 第 8 章：迭代器适配器

### 8.1 四种适配器

```cpp
// 1. 反向迭代器
for (auto rit = v.rbegin(); rit != v.rend(); ++rit) { ... }

// 2. 插入迭代器
std::copy(src.begin(), src.end(), std::back_inserter(dst));  // push_back
std::copy(src.begin(), src.end(), std::front_inserter(dq));  // push_front
std::copy(src.begin(), src.end(), std::inserter(mid, mid.begin() + 1));

// 3. 流迭代器
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));

std::istringstream iss("10 20 30");
std::vector<int> from_stream(std::istream_iterator<int>(iss),
                              std::istream_iterator<int>());

// 4. 移动迭代器
moved.insert(moved.end(),
    std::make_move_iterator(strs.begin()),
    std::make_move_iterator(strs.end()));
// strs 中的元素被移走 → 变成空 string
```

### 8.2 深入扩展

**移动迭代器的实际用途:**

```cpp
// 场景: 从一个 vector 转移所有字符串到另一个 (避免深拷贝)
std::vector<std::string> big_strings = load_data();  // 100万个大字符串
std::vector<std::string> result;
result.insert(result.end(),
    std::make_move_iterator(big_strings.begin()),
    std::make_move_iterator(big_strings.end()));
// 每个 string 只做指针移动, 不做堆分配!
```

**C++20 `std::views` 替代传统迭代器范围:**

```cpp
// C++20 Range-based
auto even = v | std::views::filter([](int x) { return x % 2 == 0; })
              | std::views::transform([](int x) { return x * x; });
// 惰性求值、可组合、更直观
```

---

# 三、算法篇

---

## 第 9 章：非修改算法

### 9.1 查找系列

```cpp
std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

std::find(v.begin(), v.end(), 9);                    // 找第一个 9
std::find_if(v.begin(), v.end(), [](int x) { return x > 7; });

std::count(v.begin(), v.end(), 5);                   // 计数
std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });

// 子序列查找
std::vector<int> pattern = {5, 9, 2};
std::search(v.begin(), v.end(), pattern.begin(), pattern.end());
```

### 9.2 谓词系列

```cpp
std::all_of(v.begin(), v.end(), pred);   // 全部满足?
std::any_of(v.begin(), v.end(), pred);   // 至少一个满足?
std::none_of(v.begin(), v.end(), pred);  // 全部不满足?
```

### 9.3 比较系列

```cpp
std::equal(a.begin(), a.end(), b.begin());                // 相等?
auto [mi1, mi2] = std::mismatch(a.begin(), a.end(), b.begin(), b.end());
std::adjacent_find(v.begin(), v.end());  // 第一对相邻相等元素
```

### 9.4 深入扩展

**`find` 的复杂度问题：**
- `std::find` 在 vector/list 上: O(n) 线性扫描
- 如果需要频繁查找: 用 `set`/`unordered_set` 或**排序后二分**
- `set.find()` O(log n), `unordered_set.find()` O(1)

---

## 第 10 章：修改算法

### 10.1 核心修改算法

```cpp
// copy_if — 过滤拷贝
std::copy_if(src.begin(), src.end(), std::back_inserter(dst), pred);

// transform — 映射变换
std::transform(src.begin(), src.end(), std::back_inserter(out),
               [](int x) { return x * x; });

// replace / replace_if
std::replace(v.begin(), v.end(), 5, 50);
std::replace_if(v.begin(), v.end(), [](int x) { return x > 10; }, 0);
```

### 10.2 Erase-Remove Idiom

```cpp
// remove 不真正删除! 只是把要保留的移到前面
auto new_end = std::remove(v.begin(), v.end(), 2);
v.erase(new_end, v.end()); // 真正删除尾部

// 原理:
// 原:   {1, 2, 3, 2, 4, 2, 5}
// remove 后: {1, 3, 4, 5, ?, ?, ?}
//                        ↑ new_end
// erase 后:  {1, 3, 4, 5}
```

### 10.3 其他修改算法

```cpp
std::fill(v.begin(), v.end(), 7);             // 填充
std::generate(v.begin(), v.end(), generator);  // 生成
std::unique(v.begin(), v.end());              // 去相邻重复
std::reverse(v.begin(), v.end());
std::rotate(v.begin(), v.begin() + 2, v.end()); // 旋转
std::shuffle(v.begin(), v.end(), std::mt19937{42});
```

### 10.4 深入扩展

**去除所有重复 (不只是相邻)：**

```cpp
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
// 必须先排序! unique 只去相邻重复
```

**`std::rotate` 的妙用:**
- 左旋数组: `rotate(begin, begin+k, end)`
- 实现 "删除中间多个元素并保持顺序"
- STL 的 `std::rotate` 是 O(n) 三次反转算法

---

## 第 11 章：排序与搜索

### 11.1 排序算法族

| 算法 | 复杂度 | 稳定? | 用途 |
|---|---|---|---|
| `sort` | O(n log n) | ❌ | 通用排序 (IntroSort) |
| `stable_sort` | O(n log n) | ✅ | 保持相等元素顺序 |
| `partial_sort` | O(n log k) | ❌ | 只排前 k 个 |
| `nth_element` | O(n) 平均 | ❌ | 找第 n 小 (快速选择) |

```cpp
// 自定义排序
std::sort(students.begin(), students.end(),
    [](const Student& a, const Student& b) {
        if (a.score != b.score) return a.score > b.score; // 分数降序
        return a.name < b.name;                            // 同分名字升序
    });

// 只要前 3 名
std::partial_sort(v.begin(), v.begin() + 3, v.end());

// 中位数 (第 n/2 小)
std::nth_element(v.begin(), v.begin() + n/2, v.end());
```

### 11.2 二分查找 (要求已排序!)

```cpp
std::binary_search(v.begin(), v.end(), target);  // bool
std::lower_bound(v.begin(), v.end(), target);     // >= target
std::upper_bound(v.begin(), v.end(), target);     // > target
auto [lo, hi] = std::equal_range(v.begin(), v.end(), target); // [>=, >]
```

### 11.3 深入扩展

**sort 的内部实现 — IntroSort：**

```
sort():
  if (size < 16) → 插入排序 (小数据更快)
  if (递归深度 > 2×log₂(n)) → 堆排序 (保证 O(n log n) 最坏)
  else → 快速排序 (平均最快)
```

**稳定排序何时需要：**

```cpp
// 场景: 先按部门排序, 再按薪资排序
// 如果用 sort → 部门内部顺序被打乱
// 如果用 stable_sort → 部门内部的薪资排序结果保留
```

---

## 第 12 章：集合算法

### 12.1 四大集合运算 (输入必须有序!)

```cpp
std::vector<int> a = {1, 2, 3, 4, 5};
std::vector<int> b = {3, 4, 5, 6, 7};

std::set_union(a, b, out);               // A ∪ B = {1,2,3,4,5,6,7}
std::set_intersection(a, b, out);        // A ∩ B = {3,4,5}
std::set_difference(a, b, out);          // A - B = {1,2}
std::set_symmetric_difference(a, b, out); // A △ B = {1,2,6,7}

std::includes(a, subset);  // subset ⊆ A ?
```

### 12.2 深入扩展

**如何对 `unordered_set` 做集合运算:**
- 方法 1: 拷贝到 vector → 排序 → 用 STL 集合算法
- 方法 2: 循环遍历 + `find()`/`count()`

---

## 第 13 章：数值算法

### 13.1 核心数值算法

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

// 折叠/归约
std::accumulate(v.begin(), v.end(), 0);            // 求和 = 15
std::accumulate(v.begin(), v.end(), 1,
    std::multiplies<int>{});                        // 阶乘 = 120

// 点积
std::inner_product(a.begin(), a.end(), b.begin(), 0); // 1*4+2*5+3*6=32

// 前缀和
std::partial_sum(v.begin(), v.end(), out_iter);    // {1,3,6,10,15}

// 差分
std::adjacent_difference(v.begin(), v.end(), out);  // {1,1,1,1,1}

// 递增填充
std::iota(v.begin(), v.end(), 1);                  // {1,2,3,...,n}

// C++17
std::gcd(12, 18);   // 最大公约数 = 6
std::lcm(12, 18);   // 最小公倍数 = 36
std::clamp(15, 0, 10); // = 10 (截断到 [0,10])
```

### 13.2 深入扩展

**accumulate 的灵活性 — 做字符串连接:**

```cpp
std::vector<std::string> words = {"Hello", " ", "World", "!"};
auto sentence = std::accumulate(words.begin(), words.end(), std::string{});
// → "Hello World!"
```

**C++17 `std::reduce` — 并行版 accumulate：**

```cpp
#include <execution>
auto sum = std::reduce(std::execution::par, v.begin(), v.end());
// 并行求和 (要求操作满足交换律和结合律)
```

---

## 第 14 章：最值与排列

### 14.1 最值

```cpp
std::min(3, 5);  std::max(3, 5);
auto [lo, hi] = std::minmax({3, 1, 4, 1, 5, 9});

auto min_it = std::min_element(v.begin(), v.end());
auto max_it = std::max_element(v.begin(), v.end());
auto [mm_min, mm_max] = std::minmax_element(v.begin(), v.end());
```

### 14.2 排列

```cpp
// 全排列 (字典序)
std::vector<int> perm = {1, 2, 3};
do {
    // 输出: 1 2 3, 1 3 2, 2 1 3, 2 3 1, 3 1 2, 3 2 1
} while (std::next_permutation(perm.begin(), perm.end()));

std::is_permutation(a.begin(), a.end(), b.begin()); // 判断排列关系
```

### 14.3 堆操作

```cpp
std::make_heap(v.begin(), v.end());    // 建堆 O(n)
// v.front() 是最大值

std::pop_heap(v.begin(), v.end());     // 最大值移到末尾
v.pop_back();                          // 再删除

v.push_back(10);
std::push_heap(v.begin(), v.end());    // 重新调整堆

std::sort_heap(v.begin(), v.end());    // 堆排序 → 升序
```

### 14.4 深入扩展

**`next_permutation` 的算法:**
1. 从右往左找第一个 `a[i] < a[i+1]` 的位置
2. 从右往左找第一个 `a[j] > a[i]` 的位置
3. 交换 `a[i]` 和 `a[j]`
4. 反转 `a[i+1:]`
- 复杂度: O(n)，可用于生成所有 n! 个排列

---

# 四、函数对象与工具篇

---

## 第 15 章：函数对象

### 15.1 标准函数对象

```cpp
std::sort(v.begin(), v.end(), std::greater<int>{}); // 降序

// 算术: plus, minus, multiplies, divides, modulus, negate
// 比较: less, greater, equal_to, less_equal, greater_equal
// 逻辑: logical_and, logical_or, logical_not

// C++14 透明比较器
std::less<>{}(3, 5.0);  // 可以比较不同类型
```

### 15.2 std::function — 可调用对象包装

```cpp
std::function<int(int, int)> op;
op = [](int a, int b) { return a + b; };     // Lambda
op = std::multiplies<int>{};                   // 函数对象

// 回调表
std::map<std::string, std::function<double(double, double)>> ops = {
    {"+", [](double a, double b) { return a + b; }},
    {"-", [](double a, double b) { return a - b; }},
    {"*", [](double a, double b) { return a * b; }},
    {"/", [](double a, double b) { return b != 0 ? a / b : 0; }},
};
```

### 15.3 Lambda 深入

```cpp
int x = 10;

// 值捕获 vs 引用捕获
[x]()  { return x; }     // 值 (拷贝)
[&x]() { return ++x; }   // 引用

// mutable — 修改值捕获的副本
auto counter = [n = 0]() mutable { return ++n; };
counter(); // 1, 2, 3...

// 泛型 Lambda (C++14)
auto add = [](auto a, auto b) { return a + b; };

// 移动捕获 (C++14)
auto ptr = std::make_unique<int>(42);
auto fn = [p = std::move(ptr)]() { return *p; };

// constexpr Lambda (C++17)
constexpr auto square = [](int x) { return x * x; };
static_assert(square(5) == 25);

// 递归 Lambda (需要 std::function)
std::function<int(int)> fib = [&fib](int n) -> int {
    return n <= 1 ? n : fib(n-1) + fib(n-2);
};

// IIFE (立即调用的函数表达式)
const auto config = [&]() {
    // 复杂初始化逻辑
    return std::string("initialized");
}();
```

### 15.4 深入扩展

**std::function 的开销:**
- 内部用**类型擦除** → 可能有堆分配
- 调用时有**虚函数或函数指针间接调用** → 无法内联
- **替代**: 模板参数 `template<typename F> void apply(F&& f)` → 零开销

**Lambda vs std::function 性能:**

| | Lambda (直接) | `std::function` |
|---|---|---|
| 大小 | 0 (无态无捕获) | 32~64 字节 |
| 调用 | 可内联 | 无法内联 |
| 分配 | 无 | 可能堆分配 |
| 用途 | 模板传递 | 存储回调 |

---

## 第 16 章：optional / variant / any (C++17)

### 16.1 std::optional — "可能没有值"

```cpp
auto divide = [](double a, double b) -> std::optional<double> {
    if (b == 0) return std::nullopt;
    return a / b;
};

auto r = divide(10, 0);
r.has_value();     // false
r.value_or(0.0);   // 0.0
if (r) { *r; }     // 安全访问
```

### 16.2 std::variant — 类型安全的 union

```cpp
std::variant<int, double, std::string> v;
v = 42;          std::get<int>(v);       // 42
v = 3.14;        std::get<double>(v);    // 3.14
v = "hello"s;    std::get<std::string>(v);

v.index();  // 当前持有类型的索引
std::get_if<int>(&v);  // 安全访问 (返回指针或 nullptr)

// visit — 模式匹配
std::visit([](const auto& val) {
    using T = std::decay_t<decltype(val)>;
    if constexpr (std::is_same_v<T, int>) { ... }
    else if constexpr (std::is_same_v<T, double>) { ... }
    else { ... }
}, v);
```

### 16.3 std::any — 任何类型

```cpp
std::any a = 42;
std::any_cast<int>(a);      // 42
a = std::string("hello");
std::any_cast<int>(a);      // 抛出 bad_any_cast!
```

### 16.4 三者对比

| | `optional<T>` | `variant<Ts...>` | `any` |
|---|---|---|---|
| 含义 | T 或空 | Ts 之一 | 任何类型 |
| 存储 | 栈上 | 栈上 | 可能堆 |
| 类型安全 | ✅ | ✅ 编译期 | ❌ 运行时 |
| 大小 | sizeof(T)+1 | max(sizeof(Ts...))+8 | ~32 |
| **优先级** | 第一选 | 第二选 | 最后选 |

### 16.5 深入扩展

**Overloaded idiom — 优雅的 visit:**

```cpp
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

std::visit(Overloaded{
    [](int v)         { std::cout << "int: " << v; },
    [](double v)      { std::cout << "double: " << v; },
    [](const string& v) { std::cout << "string: " << v; }
}, my_variant);
```

**variant 替代虚函数 (小类型集):**
- 虚函数: 堆分配 + vtable 间接调用, 但可扩展
- variant + visit: 栈上 + 编译期分派, 但类型集固定
- 游戏/高频交易: 优先 variant (缓存友好,无堆分配)

---

## 第 17 章：tuple 与结构化绑定

### 17.1 基础 tuple

```cpp
auto t = std::make_tuple(1, 3.14, std::string("hello"));

std::get<0>(t);        // 1
std::get<double>(t);   // 3.14 (按类型, 要求唯一)

// C++17 结构化绑定
auto [i, d, s] = t;

// tie + ignore
std::tie(a, std::ignore, c) = t;

// tuple_cat 拼接
auto big = std::tuple_cat(t1, t2);
```

### 17.2 实战: 返回多值

```cpp
auto get_stats(const std::vector<int>& v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    auto [min_it, max_it] = std::minmax_element(v.begin(), v.end());
    return std::tuple{sum / v.size(), *min_it, *max_it};
};

auto [mean, min_val, max_val] = get_stats(data);
```

### 17.3 深入扩展

**tuple 的比较 — 自动字典序：**

```cpp
std::tuple<int, std::string> a{1, "abc"};
std::tuple<int, std::string> b{1, "def"};
a < b;  // true (先比 int 相等, 再比 string)

// 妙用: 多字段排序
std::sort(employees.begin(), employees.end(),
    [](auto& a, auto& b) {
        return std::tie(a.dept, a.salary) < std::tie(b.dept, b.salary);
    });
```

**struct vs tuple 指南：**
- **有语义的字段** (name, age) → `struct` (更可读)
- **临时多返回值** → `tuple` + 结构化绑定
- **泛型编程** (编译期遍历字段) → `tuple` + `std::apply`

---

## 第 18 章：智能指针

### 18.1 unique_ptr — 独占所有权

```cpp
auto p = std::make_unique<Widget>("Unique");
// 不可拷贝, 只能移动
auto p2 = std::move(p);
// p == nullptr

// 自定义删除器
auto deleter = [](FILE* f) { if (f) fclose(f); };
std::unique_ptr<FILE, decltype(deleter)> fp(fopen("a.txt", "r"), deleter);

// 数组
auto arr = std::make_unique<int[]>(10);
arr[0] = 42;
```

### 18.2 shared_ptr — 共享所有权

```cpp
auto sp1 = std::make_shared<Widget>("Shared");
sp1.use_count();  // 1

{
    auto sp2 = sp1;       // 引用计数 → 2
    sp1.use_count();      // 2
}                         // sp2 析构 → 引用计数 → 1

// sp1 析构 → 引用计数 → 0 → Widget 被删除
```

### 18.3 weak_ptr — 不增加引用计数

```cpp
std::weak_ptr<Widget> wp;
{
    auto sp = std::make_shared<Widget>("W");
    wp = sp;
    wp.expired();     // false
    wp.lock()->name;  // "W"
}
wp.expired();         // true (对象已销毁)
wp.lock();            // nullptr
```

### 18.4 循环引用

```cpp
struct Node {
    // std::shared_ptr<Node> next; // ❌ A→B→A 永远不会被释放!
    std::weak_ptr<Node> next;      // ✅ 用 weak_ptr 打破循环
};

auto a = std::make_shared<Node>("A");
auto b = std::make_shared<Node>("B");
a->next = b;
b->next = a; // weak_ptr 不增加引用计数 → 正常析构
```

### 18.5 深入扩展

**make_shared vs new + shared_ptr:**

```cpp
// ❌ 两次分配 (对象 + 控制块)
std::shared_ptr<Widget> p(new Widget("W"));

// ✅ 一次分配 (对象和控制块合并)
auto p = std::make_shared<Widget>("W");
// 更快 + 异常安全
```

**智能指针选择指南：**

| 所有权模型 | 选择 | 示例场景 |
|---|---|---|
| 独占 (唯一拥有者) | `unique_ptr` | 工厂返回值、容器持有 |
| 共享 (多个拥有者) | `shared_ptr` | 缓存、观察者列表 |
| 非拥有引用 | `weak_ptr` | 缓存检查、打破循环 |
| 栈/成员对象 | 无需智能指针! | 局部变量 |

**enable_shared_from_this (可扩展方向):**

```cpp
class Widget : public std::enable_shared_from_this<Widget> {
    void register_self() {
        registry.add(shared_from_this()); // 安全地获取自身的 shared_ptr
    }
};
```

---

# 五、实战篇

---

## 第 19 章：STL 组合拳

### 19.1 词频统计

```cpp
std::istringstream iss(text);
std::map<std::string, int> freq;
std::string word;
while (iss >> word) freq[word]++;

// 按频率降序排列
std::vector<std::pair<std::string, int>> sorted(freq.begin(), freq.end());
std::sort(sorted.begin(), sorted.end(),
    [](auto& a, auto& b) { return a.second > b.second; });
```

STL 组合: `istringstream` + `map` + `vector` + `sort` + Lambda

### 19.2 TopK 问题 — 三种方法

| 方法 | 复杂度 | 思路 |
|---|---|---|
| `partial_sort` | O(n log k) | 只排前 k 个 |
| 最小堆 (size=k) | O(n log k) | 维护 k 大小的堆 |
| `nth_element` | O(n) 平均 | 快速选择划分 |

```cpp
// 方法 1: partial_sort
std::partial_sort(v.begin(), v.begin() + k, v.end(), std::greater<int>());

// 方法 2: 最小堆
std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
for (int x : data) {
    pq.push(x);
    if (pq.size() > k) pq.pop();  // 保持大小为 k
}

// 方法 3: nth_element + sort
std::nth_element(v.begin(), v.begin() + k, v.end(), std::greater<int>());
std::sort(v.begin(), v.begin() + k, std::greater<int>());
```

### 19.3 分组统计

```cpp
// 按部门分组
std::map<std::string, std::vector<Employee*>> groups;
for (auto& e : employees)
    groups[e.dept].push_back(&e);

// 各部门平均薪资
for (auto& [dept, members] : groups) {
    double avg = std::accumulate(members.begin(), members.end(), 0.0,
        [](double sum, const Employee* e) { return sum + e->salary; })
        / members.size();
}
```

STL 组合: `map` + `vector` + `accumulate` + 结构化绑定

### 19.4 矩阵转置

```cpp
using Matrix = std::vector<std::vector<int>>;
Matrix mat = {{1, 2, 3}, {4, 5, 6}};
Matrix transposed(cols, std::vector<int>(rows));
for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
        transposed[j][i] = mat[i][j];
```

### 19.5 深入扩展

**更多实战场景：**

| 场景 | STL 解法 |
|---|---|
| 去重 | `sort` → `unique` → `erase` |
| 两数之和 | `unordered_map` O(n) |
| 合并区间 | `sort` → 贪心扫描 |
| LRU Cache | `list` + `unordered_map` |
| 优先任务调度 | `priority_queue` |
| 中位数流 | 两个 `priority_queue` (大顶+小顶) |

---

## 第 20 章：性能陷阱与最佳实践

### 20.1 reserve 的巨大影响

```cpp
// 100 万 push_back
{
    Timer t("无 reserve");
    std::vector<int> v;
    for (int i = 0; i < 1000000; ++i) v.push_back(i);
}
{
    Timer t("有 reserve");
    std::vector<int> v;
    v.reserve(1000000);
    for (int i = 0; i < 1000000; ++i) v.push_back(i);
}
// 有 reserve 通常快 2~3×
```

### 20.2 emplace_back vs push_back

```cpp
// push_back: 构造临时对象 → 移动到 vector
v.push_back(std::to_string(i));

// emplace_back: 原地构造，减少一次移动
v.emplace_back(std::to_string(i));
// 注意: 对于 string 场景差距较小, 对于复杂对象差距更大
```

### 20.3 容器选择的性能影响

```cpp
// 100000 次头部插入
vector 中间插入:  ~巨慢 (每次 O(n) memmove)
deque  头部插入:  ~很快 (O(1))
list   头部插入:  ~快   (O(1), 但有指针开销)
```

### 20.4 最佳实践总结

| 建议 | 原因 |
|---|---|
| **默认用 `vector`** | 缓存友好、随机访问 |
| **`reserve` 预分配** | 避免重新分配 + 拷贝 |
| **`emplace_back` 替代 `push_back`** | 减少临时对象 |
| **`const auto&` 遍历** | 避免拷贝 |
| **`unordered_map` 替代 `map`（查找密集场景）** | 平均 O(1) vs O(log n) |
| **`sort` + `unique` 替代 `set` 去重** | 更快 |
| **erase-remove 删除元素** | O(n) 而非 O(n²) |
| **`string_view` 替代 `const string&`** | 避免分配 |
| **`optional` 替代指针/哨兵值** | 类型安全 |
| **`variant` 替代虚函数 (小类型集)** | 无堆分配 |
| **`std::move` 大对象** | 避免深拷贝 |
| **`shrink_to_fit` 释放多余内存** | 减少浪费 |

> **NOTE (修正补充)**:
> `shrink_to_fit` 是非强制请求；标准不保证一定缩容。
> 对延迟敏感路径也要谨慎调用，缩容可能引发额外分配/拷贝成本。

### 20.5 深入扩展

**vector 缓存局部性为什么重要：**

```
vector (连续内存):
  CPU Cache Line 可以预取下一块 → 遍历极快

list (离散节点):
  每个节点随机在堆上 → 每次访问可能 cache miss
  即使是 O(1) 操作, 常数也比 vector 大很多

实测: 遍历 100万 int
  vector: ~2ms
  list:   ~20ms  (10× 慢!)
```

**C++17 Parallel STL (可扩展方向):**

```cpp
#include <execution>

std::sort(std::execution::par, v.begin(), v.end());      // 并行排序
std::for_each(std::execution::par_unseq, v.begin(), v.end(), f);
std::reduce(std::execution::par, v.begin(), v.end());     // 并行归约
// 自动利用多核 CPU
```

---

## 附录 A：容器复杂度速查

| 操作 | `vector` | `deque` | `list` | `set`/`map` | `unordered_*` |
|---|---|---|---|---|---|
| 随机访问 | O(1) | O(1) | O(n) | O(log n) | O(n) |
| 头部插入 | O(n) | **O(1)** | **O(1)** | — | — |
| 尾部插入 | **O(1)** | **O(1)** | **O(1)** | — | — |
| 中间插入 | O(n) | O(n) | **O(1)*** | O(log n) | O(1) 均摊 |
| 查找 | O(n) | O(n) | O(n) | O(log n) | **O(1)** 均摊 |
| 删除 | O(n) | O(n) | **O(1)*** | O(log n) | **O(1)** 均摊 |

\* 给定迭代器

## 附录 B：算法复杂度速查

| 算法 | 复杂度 | 前提 |
|---|---|---|
| `find` / `count` | O(n) | — |
| `binary_search` / `lower_bound` | O(log n) | 已排序 |
| `sort` | O(n log n) | RandomAccess |
| `stable_sort` | O(n log n) | RandomAccess |
| `partial_sort` (前 k) | O(n log k) | RandomAccess |
| `nth_element` | O(n) 平均 | RandomAccess |
| `set_union` / `set_intersection` | O(n+m) | 已排序 |
| `accumulate` / `for_each` | O(n) | — |
| `next_permutation` | O(n) | — |
| `make_heap` | O(n) | RandomAccess |

## 附录 C：头文件速查

| 头文件 | 提供 |
|---|---|
| `<vector>` `<array>` `<deque>` `<list>` `<forward_list>` | 序列容器 |
| `<set>` `<map>` | 有序关联容器 |
| `<unordered_set>` `<unordered_map>` | 无序容器 |
| `<stack>` `<queue>` | 适配器 |
| `<string>` `<string_view>` | 字符串 |
| `<algorithm>` | 大部分算法 |
| `<numeric>` | 数值算法 |
| `<functional>` | 函数对象 / `std::function` |
| `<iterator>` | 迭代器适配器 |
| `<tuple>` | tuple |
| `<optional>` `<variant>` `<any>` | C++17 词汇类型 |
| `<memory>` | 智能指针 |

---

## 运行输出示例 (节选)

```
===== 现代 C++ STL 完全教程 =====

── 一、容器篇 ──

[1] 序列容器:
 --- vector ---
  初始: size=0 capacity=0
  reserve(100): size=0 capacity=100
  push 10: size=10 capacity=100
  shrink_to_fit: capacity=10
  front=0 back=9 v[5]=5 v.at(5)=5
  vector: { 0 1 2 3 4 5 6 7 8 9 10 }

 --- array ---
  size=5 front=1 back=5
  sorted desc: { 5 4 3 2 1 }

[3] 无序容器:
  [set 插入 100万] 487231 us
  [unordered_set 插入 100万] 82456 us
  [map 查找 50万] 312508 us
  [unordered_map 查找 50万] 28341 us

[11] 排序与搜索:
  学生排名:
    Bob: 95
    Diana: 95
    Alice: 90
    Charlie: 85
  top 3: 1 2 3
  中位数(第5小): 5

[19] STL 组合拳:
  词频统计:
       the: 4
       fox: 3
     quick: 2
       dog: 2

[20] 性能陷阱:
  [无 reserve] 12345 us
  [有 reserve] 4567 us

===== 演示完成 =====
```

---

## 扩展方向建议

| 方向 | 说明 | 优先级 |
|---|---|---|
| **C++20 Ranges** | `views::filter`/`transform`/`take`, 惰性求值 | ⭐⭐⭐⭐⭐ |
| **Parallel STL** | `std::execution::par` 并行算法 | ⭐⭐⭐⭐⭐ |
| **自定义分配器** | `std::allocator`, 内存池, Arena | ⭐⭐⭐⭐ |
| **`flat_map`/`flat_set`** | C++23 连续内存关联容器 | ⭐⭐⭐⭐ |
| **自定义迭代器** | 实现完整的自定义容器迭代器 | ⭐⭐⭐⭐ |
| **PMR (多态内存资源)** | `std::pmr::vector`, 栈分配器 | ⭐⭐⭐⭐ |
| **`std::span`** | C++20 非拥有连续范围视图 | ⭐⭐⭐ |
| **`std::mdspan`** | C++23 多维数组视图 | ⭐⭐⭐ |
| **Coroutine Generator** | `co_yield` 惰性序列 | ⭐⭐⭐ |
| **constexpr STL** | 编译期容器/算法 | ⭐⭐⭐ |
| **`std::format`** | C++20 格式化输出替代 `iostream` | ⭐⭐⭐ |
| **Concurrent 容器** | `concurrent_queue`, lock-free map | ⭐⭐⭐ |

---

> **核心原则**: STL 的设计哲学是**容器与算法分离**——通过迭代器作为桥梁，N 个容器 × M 个算法 = N×M 种组合，而不是 N×M 次实现。选择正确的容器和算法组合是性能优化的第一步：通常可先从 `vector` + `sort` + `unordered_map` 这组高频基线开始，再按访问模式与约束迭代。记住：缓存局部性往往比渐进复杂度更重要——一个缓存友好的 O(n) 操作可能比缓存不友好的 O(log n) 操作更快。
