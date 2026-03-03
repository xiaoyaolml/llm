# 📖 专题四（下）：集合（Set）— 从零基础到精通

*作者：大胖超 😜 | 配合 leetcode_knowledge_base.md 食用更佳*

---

## 目录

1. [集合的基本概念](#1-集合的基本概念)
2. [C++ STL 中的 Set 家族](#2-c-stl-中的-set-家族)
3. [unordered_set 详解](#3-unordered_set-详解)
4. [set（有序集合）详解](#4-set有序集合详解)
5. [multiset（允许重复元素）](#5-multiset允许重复元素)
6. [bitset（位集合）](#6-bitset位集合)
7. [集合经典解题模式](#7-集合经典解题模式)
8. [去重与判重](#8-去重与判重)
9. [集合运算（交集、并集、差集）](#9-集合运算交集并集差集)
10. [滑动窗口 + 集合](#10-滑动窗口--集合)
11. [经典 LeetCode Set 题精讲](#11-经典-leetcode-set-题精讲)
12. [常见陷阱与最佳实践](#12-常见陷阱与最佳实践)

---

# 1. 集合的基本概念

## 1.1 什么是集合？

**集合 (Set)** = **不重复元素的容器**，只关心元素「存不存在」。

与 Map 的关系：
```
Map  = key → value  (键值对)
Set  = key only     (只有键，没有值)

本质上 Set 就是「value 为空的 Map」。
```

核心能力：
- ✅ **O(1) 判断元素是否存在**（unordered_set）
- ✅ **自动去重**
- ✅ **集合运算**（交集、并集、差集）

## 1.2 使用场景

```
什么时候用 Set？
├─ 去重 → 从数组中去除重复元素
├─ 判重 → 判断元素是否出现过（visited 集合）
├─ 查找 → O(1) 判断某个值是否存在
├─ 集合运算 → 交集、并集、差集
├─ BFS 的 visited 集合
└─ 滑动窗口中判断重复
```

---

# 2. C++ STL 中的 Set 家族

## 2.1 四种 Set 一览

| 容器 | 头文件 | 底层 | 有序 | 允许重复 | 查找 |
|------|--------|------|------|----------|------|
| `unordered_set` | `<unordered_set>` | 哈希表 | ❌ | ❌ | O(1) |
| `set` | `<set>` | 红黑树 | ✅ | ❌ | O(log n) |
| `unordered_multiset` | `<unordered_set>` | 哈希表 | ❌ | ✅ | O(1) |
| `multiset` | `<set>` | 红黑树 | ✅ | ✅ | O(log n) |

## 2.2 怎么选？

```
刷题选哪个 Set？
├─ 只判断存不在 → unordered_set（最常用 ✅）
├─ 需要元素有序 → set
├─ 需要有序 + 上下界 → set（lower_bound / upper_bound）
├─ 需要有序 + 重复计数 → multiset
└─ 其他情况 → 优先 unordered_set
```

## 2.3 Set vs Map 对比

| 操作 | Set | Map |
|------|-----|-----|
| 存储 | 只存 key | 存 key + value |
| 判断存在 | `s.count(x)` | `m.count(key)` |
| 访问值 | — | `m[key]` |
| 去重 | 天然去重 | 天然 key 去重 |
| 适用场景 | 只关心有没有 | 需要映射关系 |

---

# 3. unordered_set 详解

## 3.1 创建与初始化

```cpp
#include <unordered_set>

// 方式 1: 默认构造
unordered_set<int> s;

// 方式 2: 初始化列表
unordered_set<int> s2 = {1, 2, 3, 4, 5};

// 方式 3: 从数组/vector 构造（自动去重）
vector<int> v = {1, 2, 2, 3, 3, 3};
unordered_set<int> s3(v.begin(), v.end());  // {1, 2, 3}

// 方式 4: 从另一个 set 拷贝
unordered_set<int> s4(s2);
```

## 3.2 增删查

```cpp
unordered_set<int> s;

// ========== 插入 ==========
s.insert(10);              // 插入单个元素
s.insert({20, 30, 40});    // 插入多个
s.emplace(50);             // 原地构造

// insert 返回 pair<iterator, bool>
auto [it, inserted] = s.insert(10);
cout << inserted;           // false（10 已存在，不重复插入）

// ========== 查找 ==========
// count(): 返回 0 或 1
if (s.count(10))
    cout << "10 存在";

// find(): 返回迭代器
auto it2 = s.find(10);
if (it2 != s.end())
    cout << *it2;  // 10

// contains(): C++20
// if (s.contains(10)) ...

// ========== 删除 ==========
s.erase(10);                // 按值删除
s.erase(s.find(20));        // 按迭代器删除
s.clear();                  // 清空
```

## 3.3 遍历

```cpp
unordered_set<string> s = {"apple", "banana", "cherry"};

// 范围 for（推荐）
for (const auto& elem : s) {
    cout << elem << " ";
}

// 迭代器
for (auto it = s.begin(); it != s.end(); ++it) {
    cout << *it << " ";
}

// ⚠️ unordered_set 顺序不确定！
// ⚠️ set 中的元素是 const 的，不能修改（修改会破坏哈希/排序）
```

## 3.4 常用 API 速查

| 函数 | 说明 | 时间 |
|------|------|------|
| `s.insert(x)` | 插入元素 | O(1) |
| `s.emplace(x)` | 原地构造插入 | O(1) |
| `s.count(x)` | 存在返回 1，否则 0 | O(1) |
| `s.find(x)` | 返回迭代器 | O(1) |
| `s.erase(x)` | 删除元素 | O(1) |
| `s.size()` | 元素数量 | O(1) |
| `s.empty()` | 是否为空 | O(1) |
| `s.clear()` | 清空 | O(n) |
| `s.reserve(n)` | 预分配桶 | O(n) |

---

# 4. set（有序集合）详解

## 4.1 底层：红黑树

```
          5
         / \
        3   7
       / \   \
      1   4   9

元素自动按升序排列，遍历 = 中序遍历 = 升序输出
```

## 4.2 基本用法

```cpp
#include <set>

set<int> s;
s.insert(5);
s.insert(2);
s.insert(8);
s.insert(1);
s.insert(9);

// 遍历：自动升序
for (int x : s) cout << x << " ";
// 输出: 1 2 5 8 9

// 反向遍历
for (auto it = s.rbegin(); it != s.rend(); ++it)
    cout << *it << " ";
// 输出: 9 8 5 2 1
```

## 4.3 set 独有能力：有序操作

```cpp
set<int> s = {1, 3, 5, 7, 9, 11, 13};

// lower_bound(x): >= x 的第一个元素
auto it1 = s.lower_bound(6);   // 指向 7

// upper_bound(x): > x 的第一个元素
auto it2 = s.upper_bound(7);   // 指向 9

// 范围查询: [5, 11] 之间的元素
cout << "范围 [5,11]: ";
for (auto it = s.lower_bound(5); it != s.upper_bound(11); ++it)
    cout << *it << " ";
// 输出: 5 7 9 11

// 最小/最大元素
cout << *s.begin();    // 1 (最小)
cout << *s.rbegin();   // 13 (最大)

// 第 k 小的元素 → 用 next(s.begin(), k-1)
// ⚠️ 这是 O(k) 的，set 不支持 O(1) 随机访问
auto it3 = next(s.begin(), 2);  // 第 3 小的元素
cout << *it3;  // 5

// prev(): 前一个元素
auto it4 = prev(s.end());   // 最后一个 = 13
auto it5 = prev(it4);       // 倒数第二个 = 11
```

## 4.4 set 的典型 LeetCode 场景

```
set 能做，unordered_set 做不了的：
├─ 有序遍历 → 直接遍历就是升序
├─ 求最小/最大 → begin() / rbegin()
├─ 前驱/后继 → lower_bound / upper_bound / prev / next
├─ 范围查询 → [lower_bound, upper_bound)
├─ 维护动态有序数据 → 随时插入删除
└─ 滑动窗口中的有序维护
```

## 4.5 自定义排序

```cpp
// 降序排列
set<int, greater<int>> s = {1, 5, 3, 7, 2};
for (int x : s) cout << x << " ";
// 输出: 7 5 3 2 1

// 自定义比较器
struct Cmp {
    bool operator()(const pair<int,int>& a, const pair<int,int>& b) const {
        return a.second < b.second;  // 按 second 升序
    }
};
set<pair<int,int>, Cmp> pairSet;
pairSet.insert({1, 30});
pairSet.insert({2, 10});
pairSet.insert({3, 20});
// 遍历: (2,10) (3,20) (1,30) — 按 second 排序
```

---

# 5. multiset（允许重复元素）

```cpp
#include <set>

multiset<int> ms;
ms.insert(3);
ms.insert(1);
ms.insert(3);   // 可以重复！
ms.insert(2);
ms.insert(3);

// 遍历: 1 2 3 3 3
for (int x : ms) cout << x << " ";

// count: 返回出现次数
cout << ms.count(3);  // 3

// erase 的坑！
// erase(val) 会删除所有等于 val 的元素！
ms.erase(3);  // 删掉所有的 3！
// 如果只想删一个:
ms.insert(3); ms.insert(3); ms.insert(3);
auto it = ms.find(3);
if (it != ms.end()) ms.erase(it);  // 只删一个 3

// equal_range: 获取所有等值元素的范围
ms.insert(3); ms.insert(3);
auto [lo, hi] = ms.equal_range(3);
cout << "3 的个数: " << distance(lo, hi) << endl;
```

### multiset 的典型应用

```cpp
// 维护动态数据的中位数
// 配合两个 multiset/priority_queue 实现

// 滑动窗口中位数 (LC 480):
// 维护一个 multiset，删除时用 find+erase 精确删一个
```

---

# 6. bitset（位集合）

> 当元素范围已知（如 0~N），用位图可以极大节省空间。

```cpp
#include <bitset>

// 固定大小（编译期确定）
bitset<10> bs;          // 10 位，初始全 0

bs.set(3);              // 第 3 位置 1
bs.set(7);              // 第 7 位置 1
bs.reset(3);            // 第 3 位置 0
bs.flip(5);             // 第 5 位取反

cout << bs;             // 0010100000
cout << bs.count();     // 2 (1 的个数)
cout << bs.test(7);     // true
cout << bs.any();       // true (有没有至少一个 1)
cout << bs.none();      // false

// 集合运算
bitset<10> a, b;
a.set(1); a.set(3); a.set(5);   // a = {1,3,5}
b.set(3); b.set(5); b.set(7);   // b = {3,5,7}

cout << (a & b);  // 交集: {3,5}
cout << (a | b);  // 并集: {1,3,5,7}
cout << (a ^ b);  // 对称差: {1,7}
cout << (~a);      // 补集
```

### bitset vs unordered_set

| 特性 | bitset | unordered_set |
|------|--------|---------------|
| 大小 | 固定（编译期） | 动态 |
| 元素类型 | 整数 0~N | 任意 |
| 内存 | 极小（N bits） | 较大 |
| 查找 | O(1) | O(1) |
| 集合运算 | O(N/64) 极快 | O(n) |
| 适用 | 元素范围小且已知 | 通用 |

---

# 7. 集合经典解题模式

## 7.1 模式一：去重

```
问题：数组/序列中有重复，需要去除
技巧：放入 set 自动去重

// 方法 1: unordered_set
unordered_set<int> s(nums.begin(), nums.end());
vector<int> unique(s.begin(), s.end());

// 方法 2: sort + unique（原地去重，不用额外空间）
sort(nums.begin(), nums.end());
nums.erase(unique(nums.begin(), nums.end()), nums.end());
```

## 7.2 模式二：判重（visited）

```
问题：遍历过程中避免重复访问
技巧：visited 集合记录已访问状态

unordered_set<State> visited;
while (...) {
    if (visited.count(state)) continue;
    visited.insert(state);
    // 处理 state
}
```

## 7.3 模式三：快速存在性判断

```
问题：判断某个值是否在一个大集合中
技巧：把集合元素放入 set，O(1) 查询

unordered_set<int> numSet(nums.begin(), nums.end());
if (numSet.count(target)) ...  // O(1)
```

## 7.4 模式四：有序集合维护动态数据

```
问题：需要动态插入/删除同时维护有序
技巧：set/multiset + lower_bound

set<int> s;
s.insert(x);
auto it = s.lower_bound(x);
if (it != s.begin()) {
    auto prev = std::prev(it);
    // prev 是 x 的前驱
}
if (it != s.end()) {
    auto next = std::next(it);
    // next 是 x 的后继
}
```

---

# 8. 去重与判重

## 8.1 LC 217: 存在重复元素

```cpp
bool containsDuplicate(vector<int>& nums) {
    unordered_set<int> seen;
    for (int num : nums) {
        if (seen.count(num)) return true;
        seen.insert(num);
    }
    return false;
}
// 时间 O(n), 空间 O(n)

// 更简洁的写法:
bool containsDuplicate2(vector<int>& nums) {
    return unordered_set<int>(nums.begin(), nums.end()).size() < nums.size();
}
```

## 8.2 LC 128: 最长连续序列

```cpp
// 用 set 去重后，只从序列起点开始计数
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> numSet(nums.begin(), nums.end());
    int maxLen = 0;

    for (int num : numSet) {
        // 只从序列起点开始（前一个不存在）
        if (!numSet.count(num - 1)) {
            int current = num;
            int len = 1;
            while (numSet.count(current + 1)) {
                current++;
                len++;
            }
            maxLen = max(maxLen, len);
        }
    }
    return maxLen;
}
// 时间 O(n)（每个元素最多被访问两次）
```

## 8.3 LC 202: 快乐数

```cpp
// 判断一个数是否为快乐数（各位平方和最终为 1）
// 用 set 检测是否进入循环
bool isHappy(int n) {
    unordered_set<int> seen;

    while (n != 1) {
        if (seen.count(n)) return false;  // 出现循环
        seen.insert(n);

        int sum = 0;
        while (n > 0) {
            int d = n % 10;
            sum += d * d;
            n /= 10;
        }
        n = sum;
    }
    return true;
}
```

---

# 9. 集合运算（交集、并集、差集）

## 9.1 两个数组的交集

```cpp
// LC 349: 两个数组的交集（去重）
vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
    unordered_set<int> s1(nums1.begin(), nums1.end());
    unordered_set<int> s2(nums2.begin(), nums2.end());

    vector<int> result;
    for (int x : s1) {
        if (s2.count(x))
            result.push_back(x);
    }
    return result;
}
```

## 9.2 使用 STL 算法的集合运算

```cpp
// set_intersection, set_union, set_difference
// ⚠️ 要求两个容器已排序！

set<int> s1 = {1, 2, 3, 4, 5};
set<int> s2 = {3, 4, 5, 6, 7};

// 交集
vector<int> inter;
set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                 back_inserter(inter));
// inter = {3, 4, 5}

// 并集
vector<int> uni;
set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
          back_inserter(uni));
// uni = {1, 2, 3, 4, 5, 6, 7}

// 差集 (s1 - s2)
vector<int> diff;
set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
               back_inserter(diff));
// diff = {1, 2}

// 对称差集 (s1 △ s2)
vector<int> symDiff;
set_symmetric_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
                          back_inserter(symDiff));
// symDiff = {1, 2, 6, 7}
```

---

# 10. 滑动窗口 + 集合

## 10.1 LC 3: 无重复字符的最长子串

```cpp
int lengthOfLongestSubstring(string s) {
    unordered_set<char> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < (int)s.size(); right++) {
        // 窗口中有重复 → 收缩左边界
        while (window.count(s[right])) {
            window.erase(s[left]);
            left++;
        }
        window.insert(s[right]);
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
// 时间 O(n), 空间 O(字符集大小)
```

## 10.2 LC 219: 存在重复元素 II

```cpp
// 判断是否有 nums[i] == nums[j] 且 |i-j| <= k
bool containsNearbyDuplicate(vector<int>& nums, int k) {
    unordered_set<int> window;

    for (int i = 0; i < (int)nums.size(); i++) {
        if (window.count(nums[i])) return true;
        window.insert(nums[i]);

        // 维护窗口大小为 k
        if ((int)window.size() > k) {
            window.erase(nums[i - k]);
        }
    }
    return false;
}
```

## 10.3 LC 220: 存在重复元素 III（set + 有序操作）

```cpp
// 判断是否有 |nums[i]-nums[j]| <= t 且 |i-j| <= k
// 需要有序集合做前驱/后继查找
bool containsNearbyAlmostDuplicate(vector<int>& nums, int indexDiff, int valueDiff) {
    set<long> window;

    for (int i = 0; i < (int)nums.size(); i++) {
        // 查找 >= nums[i] - valueDiff 的最小元素
        auto it = window.lower_bound((long)nums[i] - valueDiff);

        // 如果找到且 <= nums[i] + valueDiff
        if (it != window.end() && *it <= (long)nums[i] + valueDiff)
            return true;

        window.insert(nums[i]);

        // 维护窗口大小为 indexDiff
        if ((int)window.size() > indexDiff) {
            window.erase(nums[i - indexDiff]);
        }
    }
    return false;
}
// 关键: set.lower_bound() 实现 O(log k) 的范围查找
```

---

# 11. 经典 LeetCode Set 题精讲

## 必做题清单

| 题号 | 题目 | 难度 | 核心技巧 | 频率 |
|------|------|------|----------|------|
| 217 | 存在重复元素 | Easy | set 判重 | ⭐⭐⭐⭐ |
| 219 | 存在重复元素 II | Easy | 滑动窗口+set | ⭐⭐⭐⭐ |
| 220 | 存在重复元素 III | Hard | set + lower_bound | ⭐⭐⭐⭐ |
| 349 | 两个数组的交集 | Easy | set 交集 | ⭐⭐⭐ |
| 128 | 最长连续序列 | Medium | set 查找 | ⭐⭐⭐⭐⭐ |
| 202 | 快乐数 | Easy | set 检测循环 | ⭐⭐⭐ |
| 3 | 无重复最长子串 | Medium | 滑动窗口+set | ⭐⭐⭐⭐⭐ |
| 36 | 有效的数独 | Medium | set 判重 | ⭐⭐⭐⭐ |
| 771 | 宝石与石头 | Easy | set 查找 | ⭐⭐⭐ |
| 705 | 设计 HashSet | Easy | 手写哈希集合 | ⭐⭐⭐ |
| 706 | 设计 HashMap | Easy | 手写哈希映射 | ⭐⭐⭐ |
| 448 | 找消失的数字 | Easy | set/原地标记 | ⭐⭐⭐⭐ |
| 41 | 缺失的第一个正数 | Hard | 原地哈希 | ⭐⭐⭐⭐⭐ |

## 解题思路速查

```
看到 Set 相关问题 → 先分类：
├─ 判断重复/存在？ → unordered_set.count()
├─ 去重？ → 放入 set 或 sort+unique
├─ 检测循环？ → set 记录已见状态
├─ 连续序列？ → set 查找上下连续
├─ 滑动窗口判重？ → 固定大小 set
├─ 需要前驱/后继？ → set + lower_bound
├─ 交集/并集？ → 两个 set 交叉查找
└─ 元素范围小？ → 数组/bitset 代替
```

---

# 12. 常见陷阱与最佳实践

## 12.1 经典陷阱

### 陷阱 1：set 元素不可修改

```cpp
set<int> s = {1, 2, 3};

// ❌ 不能直接修改 set 中的元素
// *s.find(2) = 10;  // 编译错误！

// ✅ 先删后插
s.erase(2);
s.insert(10);
```

### 陷阱 2：multiset 的 erase 删过头

```cpp
multiset<int> ms = {3, 3, 3, 5, 5};

// ❌ erase(val) 删除所有等于 val 的元素！
ms.erase(3);  // 3 全没了！

// ✅ 只删一个
auto it = ms.find(3);
if (it != ms.end()) ms.erase(it);  // 只删一个 3
```

### 陷阱 3：unordered_set 自定义类型

```cpp
// ❌ struct 没有 hash → 编译错误
struct Point { int x, y; };
// unordered_set<Point> s;  // ❌

// ✅ 提供自定义 hash + operator==
struct Point {
    int x, y;
    bool operator==(const Point& o) const {
        return x == o.x && y == o.y;
    }
};
struct PointHash {
    size_t operator()(const Point& p) const {
        return hash<int>{}(p.x) ^ (hash<int>{}(p.y) << 16);
    }
};
unordered_set<Point, PointHash> s;

// ✅ 或者用 set + operator<
struct Point2 {
    int x, y;
    bool operator<(const Point2& o) const {
        return tie(x, y) < tie(o.x, o.y);
    }
};
set<Point2> s2;  // 不需要 hash
```

### 陷阱 4：性能问题

```cpp
// ❌ 在 unordered_set 中存大对象（拷贝开销）
unordered_set<vector<int>> s;  // 每次 insert 拷贝整个 vector

// ✅ 把大对象编码为 string 或整数作为 key
string encode(vector<int>& v) {
    string s;
    for (int x : v) s += to_string(x) + ",";
    return s;
}
unordered_set<string> encoded;
```

## 12.2 最佳实践

```
✅ 判存在用 count()，不要用 find() == end()（虽然效果一样，count 更简洁）
✅ 小范围整数用 bool 数组/bitset 代替 set（更快）
✅ 需要有序操作就用 set，否则用 unordered_set
✅ multiset 删除单个元素用 find() + erase(iterator)
✅ 预分配桶: unordered_set::reserve(n)
✅ BFS 的 visited 用 unordered_set<string> 或 unordered_set<int>
```

## 12.3 Set 技巧速查卡

```
┌──────────────────────────────────────────────────────┐
│              Set 技巧速查                             │
├──────────────────────────────────────────────────────┤
│  判断存在       → unordered_set.count(x)              │
│  去重           → set(v.begin(), v.end())             │
│  检测循环       → seen.count(state)                   │
│  连续序列       → 从起点开始计数（前驱不存在）          │
│  滑动窗口判重   → 固定大小 set + erase 过期元素        │
│  有序前驱后继   → set.lower_bound() / prev()          │
│  交集/并集      → 两个 set + count() 或 STL 算法       │
│  多重集删单个   → find(x) + erase(iterator)           │
│  小范围整数     → bool 数组 / bitset 代替              │
│  自定义类型     → 提供 hash+== 或 operator<            │
└──────────────────────────────────────────────────────┘
```

---

*📝 下一步：完成集合示例代码 `knowlege_details_4_set.cpp`*

*💪 Set 看似简单，但「集合 + 滑动窗口」、「有序集合 + lower_bound」是面试考点！*
