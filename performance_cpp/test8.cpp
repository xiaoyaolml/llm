// =============================================================================
// 现代 C++ STL 完全指导教程与代码示例
// =============================================================================
// STL (Standard Template Library) 是 C++ 的核心基础设施。
// 本教程从容器、迭代器、算法、函数对象到 C++17/20 新特性，
// 配合大量可运行示例，全面掌握 STL 的每个角落。
//
// 标准：C++17（部分 C++20 以注释说明）
// 编译：g++ -std=c++17 -O2 -o test8 test8.cpp
//       cl /std:c++17 /O2 /EHsc test8.cpp
//
// 目录：
//  一、容器篇
//   1.  序列容器 — vector / array / deque / list / forward_list
//   2.  关联容器 — set / map / multiset / multimap
//   3.  无序容器 — unordered_set / unordered_map
//   4.  容器适配器 — stack / queue / priority_queue
//   5.  特殊容器 — string / string_view / bitset / valarray
//   6.  C++17 容器更新 — 节点操作 / try_emplace / insert_or_assign
//
//  二、迭代器篇
//   7.  迭代器分类与特征
//   8.  迭代器适配器 — reverse / insert / stream / move
//
//  三、算法篇
//   9.  非修改算法 — find / count / search / mismatch / equal
//  10.  修改算法 — copy / transform / replace / remove / fill / generate
//  11.  排序与搜索 — sort / stable_sort / nth_element / binary_search
//  12.  集合算法 — set_union / set_intersection / set_difference
//  13.  数值算法 — accumulate / inner_product / partial_sum / iota
//  14.  最值与排列 — min_element / max_element / minmax / next_permutation
//
//  四、函数对象与工具篇
//  15.  函数对象 — std::function / std::bind / Lambda
//  16.  std::optional / std::variant / std::any (C++17)
//  17.  std::tuple 与结构化绑定
//  18.  智能指针 — unique_ptr / shared_ptr / weak_ptr
//
//  五、实战篇
//  19.  实战：STL 组合拳解决实际问题
//  20.  STL 性能陷阱与最佳实践
// =============================================================================

#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <list>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <string>
#include <string_view>
#include <bitset>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <tuple>
#include <utility>
#include <optional>
#include <variant>
#include <any>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <type_traits>
#include <initializer_list>
#include <random>

// 工具
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

template <typename T>
void do_not_optimize(T&& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}

template <typename Container>
void print(const char* label, const Container& c) {
    std::cout << "  " << label << ": { ";
    for (const auto& x : c) std::cout << x << " ";
    std::cout << "}\n";
}

template <typename K, typename V>
void print_map(const char* label, const std::map<K, V>& m) {
    std::cout << "  " << label << ": { ";
    for (const auto& [k, v] : m) std::cout << k << ":" << v << " ";
    std::cout << "}\n";
}


// =============================================================================
// ██ 一、容器篇
// =============================================================================

// =============================================================================
// 第1章：序列容器
// =============================================================================

namespace ch1 {

// --- 1.1 std::vector — 动态数组（最常用）---
//
// 特点：连续内存、随机访问 O(1)、尾部增删 O(1) 摊还、中间增删 O(n)
// 何时用：默认选择。90% 的场景用 vector。

void demo_vector() {
    // 创建方式
    std::vector<int> v1;                      // 空
    std::vector<int> v2(5, 42);               // 5个42
    std::vector<int> v3 = {1, 2, 3, 4, 5};   // 初始化列表
    std::vector<int> v4(v3.begin(), v3.end()); // 范围构造

    // 容量管理
    std::vector<int> v;
    std::cout << "  初始: size=" << v.size()
              << " capacity=" << v.capacity() << "\n";

    v.reserve(100); // 预分配（不改变 size）
    std::cout << "  reserve(100): size=" << v.size()
              << " capacity=" << v.capacity() << "\n";

    for (int i = 0; i < 10; ++i) v.push_back(i);
    std::cout << "  push 10: size=" << v.size()
              << " capacity=" << v.capacity() << "\n";

    v.shrink_to_fit(); // 释放多余内存
    std::cout << "  shrink_to_fit: capacity=" << v.capacity() << "\n";

    // 访问
    std::cout << "  front=" << v.front()
              << " back=" << v.back()
              << " v[5]=" << v[5]
              << " v.at(5)=" << v.at(5) << "\n";
    // v[100] → 未定义行为; v.at(100) → 抛出 out_of_range

    // 修改
    v.push_back(10);          // 尾部添加
    v.emplace_back(11);       // 原地构造（避免拷贝）
    v.pop_back();             // 删尾部
    v.insert(v.begin() + 3, 99); // 在位置3插入
    v.erase(v.begin() + 3);     // 删除位置3

    // 遍历
    print("vector", v);

    // data() 获取裸指针（C 兼容）
    int* raw = v.data();
    std::cout << "  data()[0] = " << raw[0] << "\n";
}

// --- 1.2 std::array — 固定大小数组 ---
//
// 特点：编译期大小、栈上分配、零开销（与 C 数组相同）
// 何时用：大小编译期已知时替代 C 数组。

void demo_array() {
    std::array<int, 5> arr = {1, 2, 3, 4, 5};

    // 与 vector 类似的接口
    std::cout << "  size=" << arr.size()
              << " front=" << arr.front()
              << " back=" << arr.back() << "\n";

    // 可以用 STL 算法
    std::sort(arr.begin(), arr.end(), std::greater<int>());
    print("sorted desc", arr);

    // 编译期大小
    constexpr std::array<int, 3> ca = {10, 20, 30};
    static_assert(ca.size() == 3);
    static_assert(ca[1] == 20);

    // 填充
    std::array<double, 4> da;
    da.fill(3.14);
    print("filled", da);
}

// --- 1.3 std::deque — 双端队列 ---
//
// 特点：两端 O(1) 增删、随机访问 O(1)、内存不连续（分段连续）
// 何时用：需要频繁在头尾增删时。

void demo_deque() {
    std::deque<int> dq = {3, 4, 5};

    dq.push_front(2);   // 头部添加
    dq.push_front(1);
    dq.push_back(6);    // 尾部添加

    print("deque", dq);

    dq.pop_front();     // 删头部
    dq.pop_back();      // 删尾部
    print("after pop", dq);
}

// --- 1.4 std::list — 双向链表 ---
//
// 特点：任意位置 O(1) 增删（给定迭代器）、不支持随机访问、每个元素额外指针开销
// 何时用：频繁在中间位置插入删除，且不需要随机访问。

void demo_list() {
    std::list<int> lst = {5, 2, 8, 1, 9, 3};

    // list 有自己的 sort（不能用 std::sort，因为没有随机访问迭代器）
    lst.sort();
    print("sorted", lst);

    // splice: O(1) 移动元素
    std::list<int> lst2 = {100, 200};
    lst.splice(lst.end(), lst2); // 将 lst2 全部移到 lst 末尾
    print("after splice", lst);
    std::cout << "  lst2 empty: " << std::boolalpha << lst2.empty() << "\n";

    // unique (删除相邻重复)
    std::list<int> dup = {1, 1, 2, 2, 2, 3, 3, 1};
    dup.unique();
    print("unique", dup);

    // merge (合并两个已排序 list)
    std::list<int> a = {1, 3, 5};
    std::list<int> b = {2, 4, 6};
    a.merge(b);
    print("merged", a);
}

// --- 1.5 std::forward_list — 单向链表 ---
//
// 特点：最小内存开销的链表（只有 next 指针）、只能前向迭代
// 何时用：内存极度敏感的场景。

void demo_forward_list() {
    std::forward_list<int> fl = {3, 1, 4, 1, 5};

    fl.push_front(0);
    fl.sort();
    fl.unique();
    print("forward_list", fl);

    // 注意：no size(), no push_back(), no back()
    // 只有 before_begin(), begin(), end()
    fl.insert_after(fl.before_begin(), -1);
    print("insert_after", fl);
}

// --- 1.6 容器选择指南 ---
//
// | 需求               | 推荐容器        |
// |-------------------|----------------|
// | 默认选择           | vector         |
// | 固定大小           | array          |
// | 两端增删           | deque          |
// | 中间频繁增删       | list           |
// | 最小开销链表       | forward_list   |
// | 随机访问+尾部增删  | vector         |

} // namespace ch1


// =============================================================================
// 第2章：关联容器
// =============================================================================

namespace ch2 {

// 基于红黑树实现，元素自动排序，查找/插入/删除 O(log n)

// --- 2.1 std::set ---

void demo_set() {
    std::set<int> s = {5, 3, 8, 1, 3, 5}; // 自动去重+排序
    print("set", s);

    // 插入
    auto [it, success] = s.insert(4);
    std::cout << "  insert 4: success=" << std::boolalpha << success << "\n";
    auto [it2, success2] = s.insert(5);
    std::cout << "  insert 5: success=" << success2 << " (已存在)\n";

    // 查找
    if (s.find(3) != s.end())
        std::cout << "  找到 3\n";
    std::cout << "  count(3)=" << s.count(3) << "\n";

    // C++20: s.contains(3)

    // 范围查找
    auto lo = s.lower_bound(3); // >= 3 的第一个
    auto hi = s.upper_bound(6); // > 6 的第一个
    std::cout << "  [3, 6] 范围内: ";
    for (auto it = lo; it != hi; ++it) std::cout << *it << " ";
    std::cout << "\n";

    // 删除
    s.erase(3);
    print("after erase 3", s);
}

// --- 2.2 std::map ---

void demo_map() {
    std::map<std::string, int> ages = {
        {"Alice", 30}, {"Bob", 25}, {"Charlie", 35}
    };

    // 访问 / 插入
    ages["David"] = 28;       // 不存在则插入
    ages["Alice"] = 31;       // 存在则覆盖
    std::cout << "  Alice: " << ages["Alice"] << "\n";

    // ⚠️ operator[] 会插入默认值！
    std::cout << "  Eve: " << ages["Eve"] << "\n"; // 插入 Eve:0
    ages.erase("Eve");

    // 安全查找
    if (auto it = ages.find("Bob"); it != ages.end()) {
        std::cout << "  找到 Bob: " << it->second << "\n";
    }

    // 遍历（按 key 有序）
    std::cout << "  按 key 排序:\n";
    for (const auto& [name, age] : ages) {
        std::cout << "    " << name << " → " << age << "\n";
    }

    // emplace: 避免临时对象
    ages.emplace("Frank", 40);

    // 大小
    std::cout << "  size=" << ages.size() << "\n";
}

// --- 2.3 std::multiset / std::multimap ---

void demo_multi() {
    // multiset: 允许重复
    std::multiset<int> ms = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    print("multiset", ms);
    std::cout << "  count(5)=" << ms.count(5) << "\n";

    // 删除所有等于 5 的
    ms.erase(5); // 删除 ALL
    print("after erase 5", ms);

    // 删除一个
    ms.insert(1);
    auto it = ms.find(1);
    if (it != ms.end()) ms.erase(it); // 删除一个 1
    print("erase one 1", ms);

    // multimap: 一个 key 多个 value
    std::multimap<std::string, int> scores;
    scores.emplace("Alice", 90);
    scores.emplace("Alice", 95);
    scores.emplace("Bob", 80);

    // 查找 Alice 的所有成绩
    auto [lo, hi] = scores.equal_range("Alice");
    std::cout << "  Alice 的成绩: ";
    for (auto it = lo; it != hi; ++it)
        std::cout << it->second << " ";
    std::cout << "\n";
}

} // namespace ch2


// =============================================================================
// 第3章：无序容器
// =============================================================================

namespace ch3 {

// 基于哈希表实现，平均 O(1) 查找/插入/删除

void demo_unordered() {
    // --- 3.1 unordered_set ---
    std::unordered_set<std::string> us = {"cat", "dog", "bird", "fish"};
    us.insert("rabbit");

    std::cout << "  unordered_set:";
    for (const auto& s : us) std::cout << " " << s;
    std::cout << "\n";

    // 桶信息
    std::cout << "  buckets=" << us.bucket_count()
              << " load_factor=" << us.load_factor() << "\n";

    // --- 3.2 unordered_map ---
    std::unordered_map<std::string, int> config = {
        {"width", 1920}, {"height", 1080}, {"fps", 60}
    };

    config["vsync"] = 1;

    for (const auto& [key, val] : config)
        std::cout << "  " << key << " = " << val << "\n";

    // --- 3.3 自定义哈希 ---
    struct Point {
        int x, y;
        bool operator==(const Point& o) const { return x == o.x && y == o.y; }
    };

    struct PointHash {
        size_t operator()(const Point& p) const {
            size_t h1 = std::hash<int>{}(p.x);
            size_t h2 = std::hash<int>{}(p.y);
            return h1 ^ (h2 << 1);
        }
    };

    std::unordered_set<Point, PointHash> points;
    points.insert({1, 2});
    points.insert({3, 4});
    points.insert({1, 2}); // 重复，不插入

    std::cout << "  点集合大小: " << points.size() << "\n";
}

// --- 3.4 有序 vs 无序 性能对比 ---

void demo_perf_comparison() {
    constexpr int N = 1000000;

    // set (红黑树, O(log n))
    {
        Timer t("set 插入 100万");
        std::set<int> s;
        for (int i = 0; i < N; ++i) s.insert(i);
    }

    // unordered_set (哈希, O(1) 平均)
    {
        Timer t("unordered_set 插入 100万");
        std::unordered_set<int> us;
        us.reserve(N);
        for (int i = 0; i < N; ++i) us.insert(i);
    }

    // map vs unordered_map
    {
        Timer t("map 查找 50万");
        std::map<int, int> m;
        for (int i = 0; i < N; ++i) m[i] = i;
        long s = 0;
        for (int i = 0; i < N / 2; ++i) s += m[i];
        do_not_optimize(s);
    }

    {
        Timer t("unordered_map 查找 50万");
        std::unordered_map<int, int> um;
        um.reserve(N);
        for (int i = 0; i < N; ++i) um[i] = i;
        long s = 0;
        for (int i = 0; i < N / 2; ++i) s += um[i];
        do_not_optimize(s);
    }
}

} // namespace ch3


// =============================================================================
// 第4章：容器适配器
// =============================================================================

namespace ch4 {

// 适配器不是新容器，只是对底层容器的接口包装。

void demo_stack() {
    // stack: LIFO (Last In First Out)
    // 底层默认 deque
    std::stack<int> stk;
    stk.push(1); stk.push(2); stk.push(3);

    std::cout << "  stack: ";
    while (!stk.empty()) {
        std::cout << stk.top() << " ";
        stk.pop();
    }
    std::cout << "(后进先出)\n";
}

void demo_queue() {
    // queue: FIFO (First In First Out)
    std::queue<std::string> q;
    q.push("first"); q.push("second"); q.push("third");

    std::cout << "  queue: ";
    while (!q.empty()) {
        std::cout << q.front() << " ";
        q.pop();
    }
    std::cout << "(先进先出)\n";
}

void demo_priority_queue() {
    // priority_queue: 最大堆（默认）
    std::priority_queue<int> pq;
    pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);

    std::cout << "  max-heap: ";
    while (!pq.empty()) {
        std::cout << pq.top() << " ";
        pq.pop();
    }
    std::cout << "\n";

    // 最小堆
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
    min_pq.push(3); min_pq.push(1); min_pq.push(4);

    std::cout << "  min-heap: ";
    while (!min_pq.empty()) {
        std::cout << min_pq.top() << " ";
        min_pq.pop();
    }
    std::cout << "\n";

    // 自定义比较：按边权排序（图算法常用）
    using Edge = std::pair<int, int>; // {权重, 节点}
    std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> graph_pq;
    graph_pq.push({5, 0}); graph_pq.push({2, 1}); graph_pq.push({8, 2});

    std::cout << "  Dijkstra 顺序: ";
    while (!graph_pq.empty()) {
        auto [w, node] = graph_pq.top();
        std::cout << "(" << w << ",n" << node << ") ";
        graph_pq.pop();
    }
    std::cout << "\n";
}

} // namespace ch4


// =============================================================================
// 第5章：特殊容器
// =============================================================================

namespace ch5 {

// --- 5.1 std::string ---

void demo_string() {
    std::string s = "Hello, World!";

    // 子串
    std::cout << "  substr(7,5)= " << s.substr(7, 5) << "\n"; // "World"

    // 查找
    auto pos = s.find("World");
    std::cout << "  find World at " << pos << "\n";

    // 替换
    s.replace(pos, 5, "C++");
    std::cout << "  replace: " << s << "\n";

    // 追加
    s += " 17/20";
    std::cout << "  append: " << s << "\n";

    // 数值转换
    int n = std::stoi("42");
    double d = std::stod("3.14");
    std::string ns = std::to_string(n) + " " + std::to_string(d);
    std::cout << "  stoi/stod: " << ns << "\n";

    // C++17: starts_with / ends_with (部分编译器支持)
    // s.starts_with("Hello") → true
}

// --- 5.2 std::string_view (C++17) ---

void demo_string_view() {
    std::string original = "Hello, Modern C++!";
    std::string_view sv = original;

    // 零拷贝子串
    auto word = sv.substr(7, 6); // "Modern" — 不分配内存！
    std::cout << "  string_view substr: " << word << "\n";

    // 剥离前后缀
    std::string_view path = "/usr/local/bin/app";
    path.remove_prefix(1); // 去掉 "/"
    std::cout << "  remove_prefix: " << path << "\n";

    // 与 string 对比大小
    std::cout << "  sizeof(string)      = " << sizeof(std::string) << "\n";
    std::cout << "  sizeof(string_view) = " << sizeof(std::string_view) << "\n";
}

// --- 5.3 std::bitset ---

void demo_bitset() {
    std::bitset<8> b1("10110011");
    std::bitset<8> b2(0xAB);

    std::cout << "  b1 = " << b1 << " (count=" << b1.count() << ")\n";
    std::cout << "  b2 = " << b2 << "\n";

    // 位运算
    std::cout << "  b1 & b2 = " << (b1 & b2) << "\n";
    std::cout << "  b1 | b2 = " << (b1 | b2) << "\n";
    std::cout << "  ~b1     = " << (~b1) << "\n";
    std::cout << "  b1 << 2 = " << (b1 << 2) << "\n";

    // 访问
    std::cout << "  b1[0]=" << b1[0] << " b1[7]=" << b1[7] << "\n";
    b1.flip(0);
    std::cout << "  flip(0): " << b1 << "\n";

    // 转换
    std::cout << "  to_ulong=" << b1.to_ulong()
              << " to_string=" << b1.to_string() << "\n";

    // 大 bitset
    std::bitset<256> big;
    big.set(0); big.set(100); big.set(255);
    std::cout << "  big bitset count=" << big.count() << "\n";
}

} // namespace ch5


// =============================================================================
// 第6章：C++17 容器更新
// =============================================================================

namespace ch6 {

void demo_cpp17_updates() {
    // --- 6.1 try_emplace ---
    // 只在 key 不存在时才构造 value（避免不必要的构造/移动）
    std::map<std::string, std::string> m;
    m.try_emplace("key1", "value1");
    m.try_emplace("key1", "IGNORED"); // key1 已存在，value 不会被构造
    std::cout << "  try_emplace: key1=" << m["key1"] << "\n";

    // --- 6.2 insert_or_assign ---
    // 不存在则插入，存在则覆盖（与 [] 类似但返回更多信息）
    auto [it, inserted] = m.insert_or_assign("key2", "first");
    std::cout << "  insert_or_assign: inserted=" << std::boolalpha << inserted << "\n";
    auto [it2, inserted2] = m.insert_or_assign("key2", "second");
    std::cout << "  insert_or_assign again: inserted=" << inserted2
              << " val=" << it2->second << "\n";

    // --- 6.3 extract + insert (节点操作) ---
    // 可以不拷贝/不移动地从一个容器转移到另一个
    std::map<int, std::string> src = {{1, "one"}, {2, "two"}, {3, "three"}};
    std::map<int, std::string> dst;

    // 提取节点
    auto node = src.extract(2);
    if (!node.empty()) {
        node.key() = 20; // 可以修改 key！
        dst.insert(std::move(node));
    }

    std::cout << "  src after extract: ";
    for (auto& [k, v] : src) std::cout << k << ":" << v << " ";
    std::cout << "\n  dst after insert: ";
    for (auto& [k, v] : dst) std::cout << k << ":" << v << " ";
    std::cout << "\n";

    // --- 6.4 merge ---
    std::map<int, std::string> a = {{1, "a"}, {2, "b"}};
    std::map<int, std::string> b = {{2, "B"}, {3, "C"}};
    a.merge(b);
    // a = {1:a, 2:b, 3:C}, b = {2:B} (key=2 在 a 中已存在，留在 b)
    std::cout << "  merged a: ";
    for (auto& [k, v] : a) std::cout << k << ":" << v << " ";
    std::cout << "\n  remaining b: ";
    for (auto& [k, v] : b) std::cout << k << ":" << v << " ";
    std::cout << "\n";

    // --- 6.5 结构化绑定遍历 ---
    std::unordered_map<std::string, int> scores = {
        {"Alice", 95}, {"Bob", 80}, {"Charlie", 90}
    };

    // C++17 结构化绑定
    for (const auto& [name, score] : scores) {
        std::cout << "  " << name << ": " << score << "\n";
    }
}

} // namespace ch6


// =============================================================================
// ██ 二、迭代器篇
// =============================================================================

// =============================================================================
// 第7章：迭代器分类与特征
// =============================================================================

namespace ch7 {

// 迭代器层次（从弱到强）:
//
//   InputIterator        — 单遍读取 (istream)
//   OutputIterator       — 单遍写入 (ostream)
//   ForwardIterator      — 多遍前向 (forward_list)
//   BidirectionalIterator — 双向     (list, set, map)
//   RandomAccessIterator  — 随机     (vector, deque, array)
//   ContiguousIterator   — 连续内存  (vector, array, string) [C++20]

void demo_iterator_traits() {
    // 通过 iterator_traits 获取迭代器信息
    using VecIter = std::vector<int>::iterator;
    using ListIter = std::list<int>::iterator;

    // 类型判断
    constexpr bool vec_random = std::is_same_v<
        std::iterator_traits<VecIter>::iterator_category,
        std::random_access_iterator_tag>;

    constexpr bool list_bidir = std::is_same_v<
        std::iterator_traits<ListIter>::iterator_category,
        std::bidirectional_iterator_tag>;

    std::cout << "  vector iter 是随机访问: " << std::boolalpha << vec_random << "\n";
    std::cout << "  list iter 是双向: " << list_bidir << "\n";

    // std::advance / std::distance / std::next / std::prev
    std::vector<int> v = {10, 20, 30, 40, 50};
    auto it = v.begin();
    std::advance(it, 3);          // 前进 3 步
    std::cout << "  advance 3: " << *it << "\n";
    std::cout << "  distance(begin, it)=" << std::distance(v.begin(), it) << "\n";
    std::cout << "  next(begin,2)=" << *std::next(v.begin(), 2) << "\n";
    std::cout << "  prev(end,1)=" << *std::prev(v.end(), 1) << "\n";
}

} // namespace ch7


// =============================================================================
// 第8章：迭代器适配器
// =============================================================================

namespace ch8 {

void demo_iterator_adapters() {
    // --- 8.1 反向迭代器 ---
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::cout << "  反向: ";
    for (auto rit = v.rbegin(); rit != v.rend(); ++rit)
        std::cout << *rit << " ";
    std::cout << "\n";

    // --- 8.2 插入迭代器 ---
    std::vector<int> src = {1, 2, 3};
    std::vector<int> dst;

    // back_inserter: 调用 push_back
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    print("back_insert", dst);

    // front_inserter (只能用于有 push_front 的容器)
    std::deque<int> dq;
    std::copy(src.begin(), src.end(), std::front_inserter(dq));
    print("front_insert", dq);

    // inserter: 在指定位置插入
    std::vector<int> mid = {10, 20, 30};
    std::copy(src.begin(), src.end(),
              std::inserter(mid, mid.begin() + 1));
    print("inserter", mid); // {10, 1, 2, 3, 20, 30}

    // --- 8.3 流迭代器 ---
    std::cout << "  ostream_iterator: ";
    std::copy(v.begin(), v.end(),
              std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";

    // istream_iterator 例子
    std::istringstream iss("10 20 30 40 50");
    std::vector<int> from_stream(
        std::istream_iterator<int>(iss),
        std::istream_iterator<int>());
    print("from stream", from_stream);

    // --- 8.4 移动迭代器 ---
    std::vector<std::string> strs = {"hello", "world", "foo"};
    std::vector<std::string> moved;
    moved.insert(moved.end(),
                 std::make_move_iterator(strs.begin()),
                 std::make_move_iterator(strs.end()));

    print("moved to", moved);
    // strs 中的元素现在是空的（已被移走）
}

} // namespace ch8


// =============================================================================
// ██ 三、算法篇
// =============================================================================

// =============================================================================
// 第9章：非修改算法
// =============================================================================

namespace ch9 {

void demo_non_modifying() {
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

    // --- find / find_if ---
    auto it = std::find(v.begin(), v.end(), 9);
    std::cout << "  find 9 at index " << std::distance(v.begin(), it) << "\n";

    auto it2 = std::find_if(v.begin(), v.end(), [](int x) { return x > 7; });
    std::cout << "  find_if >7: " << *it2 << "\n";

    // --- count / count_if ---
    std::cout << "  count 5: " << std::count(v.begin(), v.end(), 5) << "\n";
    auto even = std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    std::cout << "  count even: " << even << "\n";

    // --- all_of / any_of / none_of ---
    std::cout << "  all positive: "
              << std::boolalpha
              << std::all_of(v.begin(), v.end(), [](int x) { return x > 0; })
              << "\n";
    std::cout << "  any >8: "
              << std::any_of(v.begin(), v.end(), [](int x) { return x > 8; })
              << "\n";
    std::cout << "  none negative: "
              << std::none_of(v.begin(), v.end(), [](int x) { return x < 0; })
              << "\n";

    // --- for_each ---
    std::cout << "  for_each × 2: ";
    std::for_each(v.begin(), v.end(), [](int x) { std::cout << x * 2 << " "; });
    std::cout << "\n";

    // --- search (子序列查找) ---
    std::vector<int> pattern = {5, 9, 2};
    auto found = std::search(v.begin(), v.end(), pattern.begin(), pattern.end());
    if (found != v.end())
        std::cout << "  pattern found at index "
                  << std::distance(v.begin(), found) << "\n";

    // --- equal / mismatch ---
    std::vector<int> v2 = {3, 1, 4};
    std::cout << "  前3个相等: "
              << std::equal(v.begin(), v.begin() + 3, v2.begin()) << "\n";

    auto [mi1, mi2] = std::mismatch(v.begin(), v.end(), v2.begin(), v2.end());
    if (mi1 != v.end() && mi2 != v2.end())
        std::cout << "  first mismatch: " << *mi1 << " vs " << *mi2 << "\n";

    // --- adjacent_find ---
    std::vector<int> adj = {1, 2, 2, 3, 3, 3};
    auto adj_it = std::adjacent_find(adj.begin(), adj.end());
    if (adj_it != adj.end())
        std::cout << "  adjacent duplicate: " << *adj_it
                  << " at index " << std::distance(adj.begin(), adj_it) << "\n";
}

} // namespace ch9


// =============================================================================
// 第10章：修改算法
// =============================================================================

namespace ch10 {

void demo_modifying() {
    // --- copy / copy_if ---
    std::vector<int> src = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> dst;

    std::copy_if(src.begin(), src.end(), std::back_inserter(dst),
                 [](int x) { return x % 2 == 0; });
    print("copy_if even", dst);

    // --- transform ---
    std::vector<int> squared;
    std::transform(src.begin(), src.end(), std::back_inserter(squared),
                   [](int x) { return x * x; });
    print("squared", squared);

    // 二元 transform
    std::vector<int> a = {1, 2, 3}, b = {10, 20, 30};
    std::vector<int> sum;
    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(sum), std::plus<int>{});
    print("a+b", sum);

    // --- replace / replace_if ---
    auto v = src;
    std::replace(v.begin(), v.end(), 5, 50);
    print("replace 5→50", v);

    std::replace_if(v.begin(), v.end(), [](int x) { return x > 10; }, 0);
    print("replace >10→0", v);

    // --- remove / remove_if (erase-remove idiom) ---
    v = {1, 2, 3, 2, 4, 2, 5};
    // remove 不真正删除，只是把要保留的移到前面
    auto new_end = std::remove(v.begin(), v.end(), 2);
    v.erase(new_end, v.end()); // 真正删除
    print("remove 2", v);

    // C++20 简化: std::erase(v, 2); std::erase_if(v, pred);

    // --- fill / fill_n ---
    std::vector<int> filled(5);
    std::fill(filled.begin(), filled.end(), 7);
    print("fill 7", filled);

    std::fill_n(filled.begin(), 3, 0);
    print("fill_n 3×0", filled);

    // --- generate ---
    int counter = 0;
    std::vector<int> gen(5);
    std::generate(gen.begin(), gen.end(), [&counter]() { return counter++; });
    print("generate", gen);

    // --- unique (去除相邻重复) ---
    std::vector<int> dup = {1, 1, 2, 2, 3, 1, 1};
    dup.erase(std::unique(dup.begin(), dup.end()), dup.end());
    print("unique", dup); // {1, 2, 3, 1} (只去相邻重复)

    // 去除所有重复: 先排序再 unique
    dup = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    std::sort(dup.begin(), dup.end());
    dup.erase(std::unique(dup.begin(), dup.end()), dup.end());
    print("sort+unique", dup);

    // --- reverse / rotate ---
    v = {1, 2, 3, 4, 5};
    std::reverse(v.begin(), v.end());
    print("reverse", v);

    v = {1, 2, 3, 4, 5};
    std::rotate(v.begin(), v.begin() + 2, v.end());
    print("rotate by 2", v); // {3, 4, 5, 1, 2}

    // --- shuffle ---
    v = {1, 2, 3, 4, 5, 6, 7, 8};
    std::mt19937 rng(42); // 固定种子
    std::shuffle(v.begin(), v.end(), rng);
    print("shuffle", v);
}

} // namespace ch10


// =============================================================================
// 第11章：排序与搜索
// =============================================================================

namespace ch11 {

void demo_sorting() {
    // --- sort ---
    std::vector<int> v = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    std::sort(v.begin(), v.end());
    print("sort asc", v);

    std::sort(v.begin(), v.end(), std::greater<int>{});
    print("sort desc", v);

    // --- 自定义排序 ---
    struct Student {
        std::string name;
        int score;
    };
    std::vector<Student> students = {
        {"Alice", 90}, {"Bob", 95}, {"Charlie", 85}, {"Diana", 95}
    };

    // 按分数降序，同分按名字升序
    std::sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.name < b.name;
    });

    std::cout << "  学生排名:\n";
    for (auto& [name, score] : students)
        std::cout << "    " << name << ": " << score << "\n";

    // --- stable_sort (保持相等元素的相对顺序) ---
    std::vector<std::pair<int, char>> data = {
        {3, 'a'}, {1, 'b'}, {3, 'c'}, {1, 'd'}, {2, 'e'}
    };
    std::stable_sort(data.begin(), data.end(),
        [](auto& a, auto& b) { return a.first < b.first; });

    std::cout << "  stable_sort: ";
    for (auto& [n, c] : data) std::cout << n << c << " ";
    std::cout << "\n"; // 3a 和 3c 的相对顺序保持

    // --- partial_sort (只排前 k 个) ---
    v = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    std::partial_sort(v.begin(), v.begin() + 3, v.end());
    std::cout << "  top 3: " << v[0] << " " << v[1] << " " << v[2] << "\n";

    // --- nth_element (找第 n 小的元素) ---
    v = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    std::nth_element(v.begin(), v.begin() + 4, v.end());
    std::cout << "  中位数(第5小): " << v[4] << "\n";

    // --- is_sorted / is_sorted_until ---
    std::vector<int> test = {1, 2, 3, 5, 4};
    std::cout << "  is_sorted: " << std::boolalpha
              << std::is_sorted(test.begin(), test.end()) << "\n";
    auto until = std::is_sorted_until(test.begin(), test.end());
    std::cout << "  sorted until index "
              << std::distance(test.begin(), until) << "\n";
}

void demo_binary_search() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // --- binary_search (只返回 bool) ---
    std::cout << "  binary_search 7: "
              << std::boolalpha << std::binary_search(v.begin(), v.end(), 7) << "\n";
    std::cout << "  binary_search 11: "
              << std::binary_search(v.begin(), v.end(), 11) << "\n";

    // --- lower_bound / upper_bound ---
    auto lb = std::lower_bound(v.begin(), v.end(), 5); // >= 5
    auto ub = std::upper_bound(v.begin(), v.end(), 5); // > 5
    std::cout << "  lower_bound(5)=" << *lb << " at "
              << std::distance(v.begin(), lb) << "\n";
    std::cout << "  upper_bound(5)=" << *ub << " at "
              << std::distance(v.begin(), ub) << "\n";

    // --- equal_range ---
    std::vector<int> dup = {1, 2, 2, 2, 3, 4, 5};
    auto [lo, hi] = std::equal_range(dup.begin(), dup.end(), 2);
    std::cout << "  equal_range(2): count="
              << std::distance(lo, hi) << "\n";
}

} // namespace ch11


// =============================================================================
// 第12章：集合算法
// =============================================================================

namespace ch12 {

void demo_set_algorithms() {
    // 输入必须是有序的！
    std::vector<int> a = {1, 2, 3, 4, 5};
    std::vector<int> b = {3, 4, 5, 6, 7};

    std::vector<int> result;

    // 并集
    result.clear();
    std::set_union(a.begin(), a.end(), b.begin(), b.end(),
                   std::back_inserter(result));
    print("A ∪ B", result);

    // 交集
    result.clear();
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(),
                          std::back_inserter(result));
    print("A ∩ B", result);

    // 差集 (A - B)
    result.clear();
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(),
                        std::back_inserter(result));
    print("A - B", result);

    // 对称差集 (A △ B)
    result.clear();
    std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(),
                                   std::back_inserter(result));
    print("A △ B", result);

    // 子集判断
    std::vector<int> subset = {2, 3, 4};
    bool is_subset = std::includes(a.begin(), a.end(),
                                    subset.begin(), subset.end());
    std::cout << "  {2,3,4} ⊆ A: " << std::boolalpha << is_subset << "\n";
}

} // namespace ch12


// =============================================================================
// 第13章：数值算法
// =============================================================================

namespace ch13 {

void demo_numeric() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    // --- accumulate (折叠) ---
    int sum = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "  sum = " << sum << "\n";

    // 自定义操作: 阶乘
    int factorial = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>{});
    std::cout << "  5! = " << factorial << "\n";

    // 字符串连接
    std::vector<std::string> words = {"Hello", " ", "World", "!"};
    std::string sentence = std::accumulate(words.begin(), words.end(), std::string{});
    std::cout << "  concat: " << sentence << "\n";

    // --- inner_product (点积) ---
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = {4, 5, 6};
    int dot = std::inner_product(a.begin(), a.end(), b.begin(), 0);
    std::cout << "  dot product: " << dot << "\n"; // 1*4+2*5+3*6=32

    // --- partial_sum (前缀和) ---
    std::vector<int> prefix;
    std::partial_sum(v.begin(), v.end(), std::back_inserter(prefix));
    print("partial_sum", prefix); // {1, 3, 6, 10, 15}

    // --- adjacent_difference (差分) ---
    std::vector<int> prices = {100, 105, 102, 110, 108};
    std::vector<int> changes;
    std::adjacent_difference(prices.begin(), prices.end(),
                              std::back_inserter(changes));
    print("adj_diff", changes); // {100, 5, -3, 8, -2}

    // --- iota (递增填充) ---
    std::vector<int> seq(10);
    std::iota(seq.begin(), seq.end(), 1);
    print("iota 1..10", seq);

    // --- GCD / LCM (C++17) ---
    std::cout << "  gcd(12,18) = " << std::gcd(12, 18) << "\n";
    std::cout << "  lcm(12,18) = " << std::lcm(12, 18) << "\n";

    // --- clamp (C++17) ---
    std::cout << "  clamp(15, 0, 10) = " << std::clamp(15, 0, 10) << "\n";
    std::cout << "  clamp(-5, 0, 10) = " << std::clamp(-5, 0, 10) << "\n";
    std::cout << "  clamp(5, 0, 10)  = " << std::clamp(5, 0, 10) << "\n";
}

} // namespace ch13


// =============================================================================
// 第14章：最值与排列
// =============================================================================

namespace ch14 {

void demo_minmax() {
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

    // --- min / max / minmax ---
    std::cout << "  min(3,5) = " << std::min(3, 5) << "\n";
    std::cout << "  max(3,5) = " << std::max(3, 5) << "\n";

    auto [lo, hi] = std::minmax({3, 1, 4, 1, 5, 9});
    std::cout << "  minmax: min=" << lo << " max=" << hi << "\n";

    // --- min_element / max_element / minmax_element ---
    auto min_it = std::min_element(v.begin(), v.end());
    auto max_it = std::max_element(v.begin(), v.end());
    std::cout << "  min_element=" << *min_it
              << " max_element=" << *max_it << "\n";

    auto [mm_min, mm_max] = std::minmax_element(v.begin(), v.end());
    std::cout << "  minmax_element: " << *mm_min << ".." << *mm_max << "\n";
}

void demo_permutations() {
    // --- next_permutation / prev_permutation ---
    std::vector<int> perm = {1, 2, 3};

    std::cout << "  全排列:\n";
    do {
        std::cout << "    ";
        for (int x : perm) std::cout << x << " ";
        std::cout << "\n";
    } while (std::next_permutation(perm.begin(), perm.end()));

    // --- is_permutation ---
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = {3, 1, 2};
    std::cout << "  is_permutation: " << std::boolalpha
              << std::is_permutation(a.begin(), a.end(), b.begin()) << "\n";
}

void demo_heap() {
    // --- 堆操作 ---
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

    std::make_heap(v.begin(), v.end());
    print("make_heap", v);
    std::cout << "  heap top: " << v.front() << "\n"; // 最大值

    std::pop_heap(v.begin(), v.end()); // 最大值移到末尾
    int max = v.back(); v.pop_back();
    std::cout << "  pop_heap: " << max << "\n";

    v.push_back(10);
    std::push_heap(v.begin(), v.end());
    std::cout << "  push_heap 10, top=" << v.front() << "\n";

    // 堆排序
    std::sort_heap(v.begin(), v.end());
    print("sort_heap", v);
}

} // namespace ch14


// =============================================================================
// ██ 四、函数对象与工具篇
// =============================================================================

// =============================================================================
// 第15章：函数对象
// =============================================================================

namespace ch15 {

// --- 15.1 标准函数对象 ---
void demo_standard_functors() {
    std::vector<int> v = {5, 2, 8, 1, 9, 3};

    // 用标准函数对象做排序
    std::sort(v.begin(), v.end(), std::greater<int>{});
    print("greater sort", v);

    // 算术
    std::cout << "  plus: " << std::plus<int>{}(3, 4) << "\n";
    std::cout << "  multiplies: " << std::multiplies<int>{}(3, 4) << "\n";
    std::cout << "  modulus: " << std::modulus<int>{}(10, 3) << "\n";
    std::cout << "  negate: " << std::negate<int>{}(5) << "\n";

    // 逻辑
    std::cout << "  logical_and: " << std::logical_and<>{}(true, false) << "\n";

    // 透明比较器 (C++14)
    // std::less<> 可以比较不同类型
    std::cout << "  less<>: " << std::less<>{}(3, 5.0) << "\n";
}

// --- 15.2 std::function ---
void demo_std_function() {
    // 可以持有任何可调用对象
    std::function<int(int, int)> op;

    // 普通函数
    op = [](int a, int b) { return a + b; };
    std::cout << "  lambda: " << op(3, 4) << "\n";

    // 函数对象
    op = std::multiplies<int>{};
    std::cout << "  multiplies: " << op(3, 4) << "\n";

    // 回调表
    std::map<std::string, std::function<double(double, double)>> ops = {
        {"+", [](double a, double b) { return a + b; }},
        {"-", [](double a, double b) { return a - b; }},
        {"*", [](double a, double b) { return a * b; }},
        {"/", [](double a, double b) { return b != 0 ? a / b : 0; }},
    };

    for (auto& [name, fn] : ops) {
        std::cout << "  10 " << name << " 3 = " << fn(10, 3) << "\n";
    }
}

// --- 15.3 Lambda 深入 ---
void demo_lambda() {
    int x = 10;

    // 值捕获 vs 引用捕获
    auto by_val = [x]() { return x; };
    auto by_ref = [&x]() { return ++x; };

    by_ref();
    std::cout << "  x after ref capture: " << x << "\n"; // 11

    // mutable lambda（修改值捕获的副本）
    auto counter = [n = 0]() mutable { return ++n; };
    std::cout << "  counter: " << counter() << " "
              << counter() << " " << counter() << "\n"; // 1 2 3

    // 泛型 lambda (C++14)
    auto add = [](auto a, auto b) { return a + b; };
    std::cout << "  generic add: " << add(1, 2) << " "
              << add(1.5, 2.5) << " " << add(std::string("a"), std::string("b")) << "\n";

    // 初始化捕获 (C++14)
    auto ptr = std::make_unique<int>(42);
    auto fn = [p = std::move(ptr)]() { return *p; };
    std::cout << "  move capture: " << fn() << "\n";

    // constexpr lambda (C++17)
    constexpr auto square = [](int x) { return x * x; };
    static_assert(square(5) == 25);

    // 递归 lambda（需要 std::function 或 Y combinator）
    std::function<int(int)> fib = [&fib](int n) -> int {
        return n <= 1 ? n : fib(n - 1) + fib(n - 2);
    };
    std::cout << "  fib(10) = " << fib(10) << "\n";

    // IIFE (Immediately Invoked Function Expression)
    const auto config = [&]() {
        // 复杂初始化逻辑
        return std::string("initialized");
    }();
    std::cout << "  IIFE: " << config << "\n";
}

} // namespace ch15


// =============================================================================
// 第16章：std::optional / std::variant / std::any (C++17)
// =============================================================================

namespace ch16 {

// --- 16.1 std::optional ---
void demo_optional() {
    // 替代指针/哨兵值表示"可能没有值"
    auto divide = [](double a, double b) -> std::optional<double> {
        if (b == 0) return std::nullopt;
        return a / b;
    };

    auto r1 = divide(10, 3);
    auto r2 = divide(10, 0);

    std::cout << "  10/3: " << (r1 ? std::to_string(*r1) : "无") << "\n";
    std::cout << "  10/0: " << (r2 ? std::to_string(*r2) : "无") << "\n";

    // value_or
    std::cout << "  value_or(0): " << r2.value_or(0.0) << "\n";

    // optional 的 map/flatmap 模拟
    auto result = divide(10, 2);
    if (result) {
        auto doubled = *result * 2;
        std::cout << "  10/2 * 2 = " << doubled << "\n";
    }
}

// --- 16.2 std::variant ---
void demo_variant() {
    // 类型安全的 union
    std::variant<int, double, std::string> v;

    v = 42;
    std::cout << "  int: " << std::get<int>(v) << "\n";

    v = 3.14;
    std::cout << "  double: " << std::get<double>(v) << "\n";

    v = std::string("hello");
    std::cout << "  string: " << std::get<std::string>(v) << "\n";

    // index
    std::cout << "  index: " << v.index() << " (0=int, 1=double, 2=string)\n";

    // get_if (安全访问)
    if (auto* p = std::get_if<std::string>(&v)) {
        std::cout << "  get_if string: " << *p << "\n";
    }

    // visit (模式匹配)
    using Var = std::variant<int, double, std::string>;

    auto visitor = [](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>)
            std::cout << "  visit int: " << val << "\n";
        else if constexpr (std::is_same_v<T, double>)
            std::cout << "  visit double: " << val << "\n";
        else
            std::cout << "  visit string: " << val << "\n";
    };

    std::vector<Var> items = {42, 3.14, std::string("world")};
    for (const auto& item : items)
        std::visit(visitor, item);

    // Overloaded idiom (C++17 聚合 + 推导指引)
    //   template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
    //   template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;
    //   std::visit(Overloaded{ [](int v){...}, [](double v){...}, [](string v){...} }, item);
    //   （详细实现参见 test5.cpp）
}

// --- 16.3 std::any ---
void demo_any() {
    std::any a = 42;
    std::cout << "  any int: " << std::any_cast<int>(a) << "\n";

    a = std::string("hello");
    std::cout << "  any string: " << std::any_cast<std::string>(a) << "\n";

    a = 3.14;
    std::cout << "  any type: " << a.type().name() << "\n";

    // 安全转换
    try {
        std::any_cast<int>(a); // 类型不匹配
    } catch (const std::bad_any_cast& e) {
        std::cout << "  bad_any_cast: " << e.what() << "\n";
    }

    // any vs variant:
    //   any: 任何类型，可能堆分配，运行时类型检查
    //   variant: 固定类型集合，栈上，编译时类型安全
    //   优先用 variant！
}

} // namespace ch16


// =============================================================================
// 第17章：std::tuple 与结构化绑定
// =============================================================================

namespace ch17 {

void demo_tuple() {
    // --- 创建 ---
    auto t = std::make_tuple(1, 3.14, std::string("hello"));
    std::tuple<int, double, std::string> t2{2, 2.71, "world"};

    // --- 访问 ---
    std::cout << "  get<0>: " << std::get<0>(t) << "\n";
    std::cout << "  get<1>: " << std::get<1>(t) << "\n";
    std::cout << "  get<2>: " << std::get<2>(t) << "\n";

    // 按类型访问（要求类型唯一）
    std::cout << "  get<double>: " << std::get<double>(t) << "\n";

    // --- 结构化绑定 (C++17) ---
    auto [i, d, s] = t;
    std::cout << "  structured binding: " << i << " " << d << " " << s << "\n";

    // --- tuple_size / tuple_element ---
    constexpr auto size = std::tuple_size_v<decltype(t)>;
    std::cout << "  tuple_size: " << size << "\n";

    // --- tie ---
    int a; double b; std::string c;
    std::tie(a, b, c) = t;
    std::cout << "  tie: " << a << " " << b << " " << c << "\n";

    // 用 std::ignore 忽略不需要的值
    std::tie(a, std::ignore, c) = t;

    // --- tuple_cat ---
    auto t3 = std::tuple_cat(t, t2);
    std::cout << "  cat size: " << std::tuple_size_v<decltype(t3)> << "\n";

    // --- 返回多值 ---
    auto get_stats = [](const std::vector<int>& v) {
        double sum = std::accumulate(v.begin(), v.end(), 0.0);
        double mean = sum / v.size();
        auto [min_it, max_it] = std::minmax_element(v.begin(), v.end());
        return std::tuple{mean, *min_it, *max_it};
    };

    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6};
    auto [mean, min_val, max_val] = get_stats(data);
    std::cout << "  stats: mean=" << mean
              << " min=" << min_val
              << " max=" << max_val << "\n";

    // --- 比较 ---
    std::tuple<int, std::string> x{1, "abc"};
    std::tuple<int, std::string> y{1, "def"};
    std::cout << "  tuple compare: " << std::boolalpha << (x < y) << "\n"; // true
}

} // namespace ch17


// =============================================================================
// 第18章：智能指针
// =============================================================================

namespace ch18 {

struct Widget {
    std::string name;
    Widget(std::string n) : name(std::move(n)) {
        std::cout << "    Widget(" << name << ") 构造\n";
    }
    ~Widget() { std::cout << "    Widget(" << name << ") 析构\n"; }
};

void demo_unique_ptr() {
    // --- unique_ptr: 独占所有权 ---
    std::cout << "  -- unique_ptr --\n";

    auto p = std::make_unique<Widget>("Unique");
    std::cout << "    name: " << p->name << "\n";

    // 移动所有权
    auto p2 = std::move(p);
    std::cout << "    p is null: " << (p == nullptr) << "\n";
    std::cout << "    p2 name: " << p2->name << "\n";

    // 自定义删除器
    auto file_deleter = [](FILE* f) {
        if (f) { fclose(f); std::cout << "    file closed\n"; }
    };
    // std::unique_ptr<FILE, decltype(file_deleter)> fp(fopen("test.txt", "r"), file_deleter);

    // 数组
    auto arr = std::make_unique<int[]>(10);
    arr[0] = 42;
    std::cout << "    unique_ptr array[0]: " << arr[0] << "\n";
}

void demo_shared_ptr() {
    // --- shared_ptr: 共享所有权 ---
    std::cout << "  -- shared_ptr --\n";

    auto sp1 = std::make_shared<Widget>("Shared");
    std::cout << "    use_count: " << sp1.use_count() << "\n";

    {
        auto sp2 = sp1; // 引用计数 +1
        std::cout << "    use_count (2 refs): " << sp1.use_count() << "\n";
    } // sp2 析构，引用计数 -1

    std::cout << "    use_count (1 ref): " << sp1.use_count() << "\n";
}

void demo_weak_ptr() {
    // --- weak_ptr: 不增加引用计数 ---
    std::cout << "  -- weak_ptr --\n";

    std::weak_ptr<Widget> wp;

    {
        auto sp = std::make_shared<Widget>("Weak");
        wp = sp;
        std::cout << "    expired: " << std::boolalpha << wp.expired() << "\n";

        if (auto locked = wp.lock()) {
            std::cout << "    locked name: " << locked->name << "\n";
        }
    }

    // sp 已销毁
    std::cout << "    expired: " << wp.expired() << "\n";
    if (auto locked = wp.lock()) {
        std::cout << "    SHOULD NOT PRINT\n";
    } else {
        std::cout << "    lock failed (object destroyed)\n";
    }
}

// --- 循环引用演示 ---
struct Node {
    std::string name;
    // std::shared_ptr<Node> next; // ❌ 循环引用 → 内存泄漏
    std::weak_ptr<Node> next;      // ✅ 用 weak_ptr 打破循环

    Node(std::string n) : name(std::move(n)) {
        std::cout << "    Node(" << name << ") 构造\n";
    }
    ~Node() { std::cout << "    Node(" << name << ") 析构\n"; }
};

void demo_circle_reference() {
    std::cout << "  -- 打破循环引用 --\n";
    auto a = std::make_shared<Node>("A");
    auto b = std::make_shared<Node>("B");
    a->next = b;
    b->next = a; // 用 weak_ptr，不增加引用计数
    // A 和 B 都能正确析构
}

} // namespace ch18


// =============================================================================
// ██ 五、实战篇
// =============================================================================

// =============================================================================
// 第19章：实战 — STL 组合拳
// =============================================================================

namespace ch19 {

// --- 19.1 词频统计 ---
void word_frequency() {
    std::string text =
        "the quick brown fox jumps over the lazy dog "
        "the fox the dog the quick fox";

    std::istringstream iss(text);
    std::map<std::string, int> freq;

    std::string word;
    while (iss >> word) freq[word]++;

    // 按频率降序排列
    std::vector<std::pair<std::string, int>> sorted(freq.begin(), freq.end());
    std::sort(sorted.begin(), sorted.end(),
              [](auto& a, auto& b) { return a.second > b.second; });

    std::cout << "  词频统计:\n";
    for (auto& [w, c] : sorted)
        std::cout << "    " << std::setw(8) << w << ": " << c << "\n";
}

// --- 19.2 TopK 问题 ---
void topk() {
    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7};
    int k = 5;

    // 方法1: partial_sort
    auto v1 = data;
    std::partial_sort(v1.begin(), v1.begin() + k, v1.end(), std::greater<int>());
    std::cout << "  Top " << k << " (partial_sort): ";
    for (int i = 0; i < k; ++i) std::cout << v1[i] << " ";
    std::cout << "\n";

    // 方法2: priority_queue (最小堆, 大小为 k)
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
    for (int x : data) {
        pq.push(x);
        if ((int)pq.size() > k) pq.pop();
    }
    std::cout << "  Top " << k << " (min-heap): ";
    std::vector<int> top;
    while (!pq.empty()) { top.push_back(pq.top()); pq.pop(); }
    std::reverse(top.begin(), top.end());
    for (int x : top) std::cout << x << " ";
    std::cout << "\n";

    // 方法3: nth_element
    auto v3 = data;
    std::nth_element(v3.begin(), v3.begin() + k, v3.end(), std::greater<int>());
    std::sort(v3.begin(), v3.begin() + k, std::greater<int>());
    std::cout << "  Top " << k << " (nth_element): ";
    for (int i = 0; i < k; ++i) std::cout << v3[i] << " ";
    std::cout << "\n";
}

// --- 19.3 分组统计 ---
void group_by() {
    struct Employee {
        std::string name;
        std::string dept;
        int salary;
    };

    std::vector<Employee> employees = {
        {"Alice",   "Engineering", 120000},
        {"Bob",     "Engineering", 110000},
        {"Charlie", "Marketing",   90000},
        {"Diana",   "Marketing",   95000},
        {"Eve",     "Engineering", 130000},
        {"Frank",   "HR",          80000},
    };

    // 按部门分组
    std::map<std::string, std::vector<Employee*>> groups;
    for (auto& e : employees)
        groups[e.dept].push_back(&e);

    // 各部门平均薪资
    std::cout << "  部门平均薪资:\n";
    for (auto& [dept, members] : groups) {
        double avg = std::accumulate(members.begin(), members.end(), 0.0,
            [](double sum, const Employee* e) { return sum + e->salary; })
            / members.size();
        std::cout << "    " << std::setw(12) << dept
                  << ": $" << std::fixed << std::setprecision(0) << avg
                  << " (" << members.size() << " 人)\n";
    }
}

// --- 19.4 矩阵转置（STL 风格）---
void matrix_transpose() {
    using Matrix = std::vector<std::vector<int>>;

    Matrix mat = {{1, 2, 3}, {4, 5, 6}};
    int rows = mat.size(), cols = mat[0].size();

    Matrix transposed(cols, std::vector<int>(rows));
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            transposed[j][i] = mat[i][j];

    std::cout << "  原矩阵:\n";
    for (auto& row : mat) {
        std::cout << "    ";
        for (int x : row) std::cout << x << " ";
        std::cout << "\n";
    }
    std::cout << "  转置:\n";
    for (auto& row : transposed) {
        std::cout << "    ";
        for (int x : row) std::cout << x << " ";
        std::cout << "\n";
    }
}

} // namespace ch19


// =============================================================================
// 第20章：STL 性能陷阱与最佳实践
// =============================================================================

namespace ch20 {

void demo_performance_tips() {
    constexpr int N = 1000000;

    // --- 20.1 vector::reserve ---
    {
        Timer t("无 reserve");
        std::vector<int> v;
        for (int i = 0; i < N; ++i) v.push_back(i);
    }
    {
        Timer t("有 reserve");
        std::vector<int> v;
        v.reserve(N);
        for (int i = 0; i < N; ++i) v.push_back(i);
    }

    // --- 20.2 emplace_back vs push_back ---
    {
        Timer t("push_back string");
        std::vector<std::string> v;
        v.reserve(N);
        for (int i = 0; i < N; ++i) v.push_back(std::to_string(i));
    }
    {
        Timer t("emplace_back string");
        std::vector<std::string> v;
        v.reserve(N);
        for (int i = 0; i < N; ++i) v.emplace_back(std::to_string(i));
    }

    // --- 20.3 避免不必要的拷贝 ---
    std::cout << "\n  迭代拷贝陷阱:\n";
    std::vector<std::string> words = {"hello", "world", "foo", "bar"};

    // ❌ 拷贝每个 string
    // for (auto s : words) { ... }

    // ✅ 引用
    // for (const auto& s : words) { ... }

    // ❌ 函数参数拷贝
    // void process(std::vector<int> v) { ... }

    // ✅ const 引用
    // void process(const std::vector<int>& v) { ... }

    // --- 20.4 容器选择影响 ---
    std::cout << "  容器中间插入性能:\n";
    {
        Timer t("vector 中间插入");
        std::vector<int> v;
        for (int i = 0; i < 100000; ++i) v.insert(v.begin(), i);
    }
    {
        Timer t("deque 头部插入");
        std::deque<int> d;
        for (int i = 0; i < 100000; ++i) d.push_front(i);
    }
    {
        Timer t("list 头部插入");
        std::list<int> l;
        for (int i = 0; i < 100000; ++i) l.push_front(i);
    }
}

// --- 20.5 最佳实践总结 ---
//
// | 建议                              | 原因                    |
// |----------------------------------|------------------------|
// | 默认用 vector                    | 缓存友好、随机访问       |
// | reserve 预分配                   | 避免重新分配+拷贝        |
// | emplace_back 替代 push_back      | 减少临时对象             |
// | const auto& 遍历                 | 避免拷贝               |
// | unordered_map 替代 map           | O(1) vs O(log n)       |
// | sort+unique 替代 set 去重         | 更快                   |
// | erase-remove 删除元素            | O(n) 而非 O(n²)        |
// | string_view 替代 const string&   | 避免分配               |
// | optional 替代 指针/哨兵值         | 类型安全               |
// | variant 替代 虚函数(小类型集)     | 无堆分配               |
// | 移动 std::move 大对象             | 避免深拷贝             |
// | shrink_to_fit 释放多余内存        | 减少内存浪费           |

} // namespace ch20


// =============================================================================
// main
// =============================================================================
int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "===== 现代 C++ STL 完全教程 =====\n\n";

    // 一、容器篇
    std::cout << "── 一、容器篇 ──\n\n";

    std::cout << "[1] 序列容器:\n";
    std::cout << " --- vector ---\n";
    ch1::demo_vector();
    std::cout << " --- array ---\n";
    ch1::demo_array();
    std::cout << " --- deque ---\n";
    ch1::demo_deque();
    std::cout << " --- list ---\n";
    ch1::demo_list();
    std::cout << " --- forward_list ---\n";
    ch1::demo_forward_list();
    std::cout << "\n";

    std::cout << "[2] 关联容器:\n";
    ch2::demo_set();
    ch2::demo_map();
    ch2::demo_multi();
    std::cout << "\n";

    std::cout << "[3] 无序容器:\n";
    ch3::demo_unordered();
    ch3::demo_perf_comparison();
    std::cout << "\n";

    std::cout << "[4] 容器适配器:\n";
    ch4::demo_stack();
    ch4::demo_queue();
    ch4::demo_priority_queue();
    std::cout << "\n";

    std::cout << "[5] 特殊容器:\n";
    ch5::demo_string();
    ch5::demo_string_view();
    ch5::demo_bitset();
    std::cout << "\n";

    std::cout << "[6] C++17 容器更新:\n";
    ch6::demo_cpp17_updates();
    std::cout << "\n";

    // 二、迭代器篇
    std::cout << "── 二、迭代器篇 ──\n\n";

    std::cout << "[7] 迭代器分类:\n";
    ch7::demo_iterator_traits();
    std::cout << "\n";

    std::cout << "[8] 迭代器适配器:\n";
    ch8::demo_iterator_adapters();
    std::cout << "\n";

    // 三、算法篇
    std::cout << "── 三、算法篇 ──\n\n";

    std::cout << "[9] 非修改算法:\n";
    ch9::demo_non_modifying();
    std::cout << "\n";

    std::cout << "[10] 修改算法:\n";
    ch10::demo_modifying();
    std::cout << "\n";

    std::cout << "[11] 排序与搜索:\n";
    ch11::demo_sorting();
    ch11::demo_binary_search();
    std::cout << "\n";

    std::cout << "[12] 集合算法:\n";
    ch12::demo_set_algorithms();
    std::cout << "\n";

    std::cout << "[13] 数值算法:\n";
    ch13::demo_numeric();
    std::cout << "\n";

    std::cout << "[14] 最值与排列:\n";
    ch14::demo_minmax();
    ch14::demo_permutations();
    ch14::demo_heap();
    std::cout << "\n";

    // 四、函数对象与工具
    std::cout << "── 四、函数对象与工具 ──\n\n";

    std::cout << "[15] 函数对象:\n";
    ch15::demo_standard_functors();
    ch15::demo_std_function();
    ch15::demo_lambda();
    std::cout << "\n";

    std::cout << "[16] optional/variant/any:\n";
    ch16::demo_optional();
    ch16::demo_variant();
    ch16::demo_any();
    std::cout << "\n";

    std::cout << "[17] tuple:\n";
    ch17::demo_tuple();
    std::cout << "\n";

    std::cout << "[18] 智能指针:\n";
    ch18::demo_unique_ptr();
    ch18::demo_shared_ptr();
    ch18::demo_weak_ptr();
    ch18::demo_circle_reference();
    std::cout << "\n";

    // 五、实战
    std::cout << "── 五、实战篇 ──\n\n";

    std::cout << "[19] STL 组合拳:\n";
    ch19::word_frequency();
    ch19::topk();
    ch19::group_by();
    ch19::matrix_transpose();
    std::cout << "\n";

    std::cout << "[20] 性能陷阱:\n";
    ch20::demo_performance_tips();

    std::cout << "\n===== 演示完成 =====\n";
    return 0;
}
