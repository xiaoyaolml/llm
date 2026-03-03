# 📖 专题四（上）：映射（Map）与哈希表 — 从零基础到精通

*作者：大胖超 😜 | 配合 leetcode_knowledge_base.md 食用更佳*

---

## 目录

1. [哈希表底层原理](#1-哈希表底层原理)
2. [C++ STL 中的 Map 家族](#2-c-stl-中的-map-家族)
3. [unordered_map 详解](#3-unordered_map-详解)
4. [map（有序映射）详解](#4-map有序映射详解)
5. [multimap（允许重复键）](#5-multimap允许重复键)
6. [自定义哈希函数](#6-自定义哈希函数)
7. [哈希表经典解题模式](#7-哈希表经典解题模式)
8. [两数之和系列](#8-两数之和系列)
9. [字母异位词与分组](#9-字母异位词与分组)
10. [前缀和 + 哈希表](#10-前缀和--哈希表)
11. [LRU / LFU 缓存](#11-lru--lfu-缓存)
12. [经典 LeetCode Map 题精讲](#12-经典-leetcode-map-题精讲)
13. [常见陷阱与最佳实践](#13-常见陷阱与最佳实践)

---

# 1. 哈希表底层原理

## 1.1 什么是哈希表？

**哈希表 (Hash Table)** 是一种通过**哈希函数**把「键 (Key)」映射到「数组下标」的数据结构，从而实现**近乎 O(1)** 的增删查改。

> 一句话：**用空间换时间，以数组直接定位代替线性查找。**

```
                    哈希函数 h(key)
key "apple"  ────────────────────→  h("apple") = 3  ──→ table[3] = 苹果的信息

工作流程：
1. 计算 hash = h(key)
2. index = hash % table_size
3. 在 table[index] 位置操作（增/删/查/改）
```

## 1.2 哈希函数

哈希函数的目标：**把任意类型的 key 均匀映射到一个整数**。

```cpp
// 整数哈希：直接用自身
h(42) = 42

// 字符串哈希（常见方法）
h("abc") = 'a'*31^2 + 'b'*31^1 + 'c'*31^0

// C++ 标准库自带 std::hash
size_t h1 = hash<int>{}(42);         // 整数哈希
size_t h2 = hash<string>{}("hello"); // 字符串哈希
```

### 好的哈希函数要求

| 要求 | 说明 |
|------|------|
| 确定性 | 同一个 key 永远得到同一个哈希值 |
| 均匀性 | 不同 key 尽量映射到不同位置 |
| 高效性 | 计算要快 |

## 1.3 冲突（Collision）与解决

两个不同的 key 映射到同一个下标 → **哈希冲突**。

### 方法一：链地址法 (Separate Chaining)

```
table[0] → null
table[1] → ["apple", 5] → ["banana", 3] → null
table[2] → ["cat", 7] → null
table[3] → null

同一个位置用链表串起来。
查找时：先定位到桶，再在链表中线性搜索。
```

- **C++ unordered_map** 和 **Java HashMap** 默认使用此方法
- 最坏情况退化为 O(n)（所有 key 冲突到同一桶）

### 方法二：开放寻址法 (Open Addressing)

```
冲突时，按某种策略查找下一个空位：
- 线性探测：h(k), h(k)+1, h(k)+2, ...
- 二次探测：h(k), h(k)+1², h(k)+2², ...
- 双重哈希：h(k), h(k)+h2(k), h(k)+2*h2(k), ...
```

- **Python dict** 使用此方法

## 1.4 负载因子与 rehash

```
负载因子 (Load Factor) = 元素数量 / 桶数量

C++ unordered_map 默认：
  max_load_factor = 1.0
  当 load_factor > max_load_factor 时自动 rehash
  rehash: 扩大桶数量（通常 2 倍），重新计算每个 key 的位置
```

## 1.5 时间复杂度

| 操作 | 平均 | 最坏 |
|------|------|------|
| 插入 | O(1) | O(n) |
| 查找 | O(1) | O(n) |
| 删除 | O(1) | O(n) |

> 最坏情况（大量冲突）几乎不会发生。面试和刷题中**一律视为 O(1)**。

---

# 2. C++ STL 中的 Map 家族

## 2.1 四种 Map 一览

| 容器 | 头文件 | 底层 | 有序 | 键唯一 | 查找 |
|------|--------|------|------|--------|------|
| `unordered_map` | `<unordered_map>` | 哈希表 | ❌ | ✅ | O(1) |
| `map` | `<map>` | 红黑树 | ✅ | ✅ | O(log n) |
| `unordered_multimap` | `<unordered_map>` | 哈希表 | ❌ | ❌ | O(1) |
| `multimap` | `<map>` | 红黑树 | ✅ | ❌ | O(log n) |

## 2.2 怎么选？

```
做 LeetCode 选哪个？
├─ 需要 O(1) 查找，不关心顺序 → unordered_map（最常用 ✅）
├─ 需要 key 有序（升序遍历） → map
├─ 需要 key 有序 + 上下界查找 → map（lower_bound / upper_bound）
├─ 允许重复 key → multimap / unordered_multimap
└─ 其他情况 → 优先 unordered_map
```

---

# 3. unordered_map 详解

## 3.1 创建与初始化

```cpp
#include <unordered_map>

// 方式 1: 默认构造
unordered_map<string, int> mp;

// 方式 2: 初始化列表
unordered_map<string, int> mp2 = {
    {"apple", 3},
    {"banana", 5},
    {"cherry", 2}
};

// 方式 3: 从另一个 map 拷贝
unordered_map<string, int> mp3(mp2);

// 方式 4: 从迭代器范围
unordered_map<string, int> mp4(mp2.begin(), mp2.end());
```

## 3.2 增删查改

```cpp
unordered_map<string, int> mp;

// ========== 插入 ==========
mp["apple"] = 3;                      // 下标运算符（最常用）
mp.insert({"banana", 5});             // insert
mp.insert(make_pair("cherry", 2));    // make_pair
mp.emplace("date", 7);               // emplace（原地构造，更高效）

// ========== 查找 ==========
// 方法 1: count() — 返回 0 或 1
if (mp.count("apple"))     // "apple" 存在
    cout << mp["apple"];

// 方法 2: find() — 返回迭代器
auto it = mp.find("apple");
if (it != mp.end())
    cout << it->second;     // 3

// 方法 3: contains() — C++20
// if (mp.contains("apple"))  // C++20

// ⚠️ 方法 4: 直接 mp[key] 也能查，但不存在的 key 会被自动插入！
int val = mp["grape"];      // "grape" 不存在 → 自动插入 {"grape", 0}

// ========== 修改 ==========
mp["apple"] = 10;           // 直接覆盖
mp.at("apple") = 20;        // at() 会检查 key 是否存在，不存在抛异常

// ========== 删除 ==========
mp.erase("apple");          // 按 key 删除
mp.erase(mp.find("banana")); // 按迭代器删除
mp.clear();                  // 清空
```

## 3.3 遍历

```cpp
unordered_map<string, int> mp = {{"a",1}, {"b",2}, {"c",3}};

// 方式 1: 范围 for + 结构化绑定（C++17，推荐）
for (auto& [key, val] : mp) {
    cout << key << " → " << val << endl;
}

// 方式 2: 范围 for + pair
for (auto& p : mp) {
    cout << p.first << " → " << p.second << endl;
}

// 方式 3: 迭代器
for (auto it = mp.begin(); it != mp.end(); ++it) {
    cout << it->first << " → " << it->second << endl;
}

// ⚠️ unordered_map 的遍历顺序是不确定的！
```

## 3.4 常用成员函数速查

| 函数 | 说明 | 时间 |
|------|------|------|
| `mp[key]` | 访问/插入（不存在则创建默认值） | O(1) |
| `mp.at(key)` | 访问（不存在抛 `out_of_range`） | O(1) |
| `mp.count(key)` | key 存在返回 1，否则 0 | O(1) |
| `mp.find(key)` | 返回迭代器（`end()` 表示不存在） | O(1) |
| `mp.insert({k,v})` | 插入（key 已存在则不覆盖） | O(1) |
| `mp.emplace(k,v)` | 原地构造插入 | O(1) |
| `mp.erase(key)` | 删除 | O(1) |
| `mp.size()` | 元素数量 | O(1) |
| `mp.empty()` | 是否为空 | O(1) |
| `mp.clear()` | 清空 | O(n) |
| `mp.bucket_count()` | 桶的数量 | O(1) |
| `mp.load_factor()` | 当前负载因子 | O(1) |
| `mp.reserve(n)` | 预分配桶（提高性能） | O(n) |

## 3.5 `[]` vs `insert` vs `emplace`

```cpp
// [] ：如果 key 不存在会创建，如果存在会覆盖
mp["key"] = 10;  // 不管存不存在，最后都是 10

// insert：如果 key 已存在，不覆盖
mp.insert({"key", 20});  // 如果 "key" 已存在，值不变

// emplace：和 insert 类似，但原地构造
mp.emplace("key", 30);   // 如果 "key" 已存在，值不变

// 需要「不存在才插入」→ insert / emplace
// 需要「无论如何都更新」→ []
```

---

# 4. map（有序映射）详解

## 4.1 底层：红黑树

`map` 底层是**红黑树（自平衡二叉搜索树）**，元素按 key 升序排列。

```
          "c"
         / \
       "a"  "e"
         \   / \
         "b" "d" "f"

遍历红黑树 = 中序遍历 = key 升序
```

## 4.2 基本用法

```cpp
#include <map>

map<string, int> m;
m["banana"] = 2;
m["apple"]  = 3;
m["cherry"] = 1;

// 遍历：自动按 key 字母序
for (auto& [k, v] : m) {
    cout << k << " → " << v << endl;
}
// apple → 3
// banana → 2
// cherry → 1  （自动排序！）
```

## 4.3 map 独有能力：有序操作

```cpp
map<int, string> m = {{1,"a"}, {3,"c"}, {5,"e"}, {7,"g"}, {9,"i"}};

// lower_bound(key): 返回 >= key 的第一个迭代器
auto it1 = m.lower_bound(4);   // 指向 {5, "e"}

// upper_bound(key): 返回 > key 的第一个迭代器
auto it2 = m.upper_bound(5);   // 指向 {7, "g"}

// 范围查询: [3, 7] 之间的元素
auto lo = m.lower_bound(3);
auto hi = m.upper_bound(7);
for (auto it = lo; it != hi; ++it) {
    cout << it->first << " ";  // 3 5 7
}

// begin() / end(): 最小/最大 key
cout << m.begin()->first;       // 1（最小 key）
cout << m.rbegin()->first;      // 9（最大 key）
```

## 4.4 map vs unordered_map

| 特性 | `unordered_map` | `map` |
|------|-----------------|-------|
| 底层 | 哈希表 | 红黑树 |
| 查找 | O(1) 平均 | O(log n) |
| 插入 | O(1) 平均 | O(log n) |
| 有序 | ❌ | ✅ key 升序 |
| upper/lower_bound | ❌ | ✅ |
| 内存 | 较大（桶+链表） | 较小（节点） |
| 刷题首选 | ✅ 大多数情况 | 需要有序时 |

> **经验法则**：
> - 只需要查找/计数 → `unordered_map`
> - 需要 key 排序 / 范围查询 / 有序遍历 → `map`

---

# 5. multimap（允许重复键）

```cpp
#include <map>

multimap<string, int> mm;
mm.insert({"apple", 1});
mm.insert({"apple", 2});   // 允许重复 key！
mm.insert({"apple", 3});
mm.insert({"banana", 4});

// count: key 出现的次数
cout << mm.count("apple");  // 3

// equal_range: 返回 [first, second) 区间
auto [lo, hi] = mm.equal_range("apple");
for (auto it = lo; it != hi; ++it) {
    cout << it->second << " ";  // 1 2 3
}

// ⚠️ multimap 没有 [] 运算符！
// mm["apple"] = 5;  // ❌ 编译错误
```

---

# 6. 自定义哈希函数

## 6.1 为什么需要？

`unordered_map` 的 key 需要有对应的 `hash` 函数。标准库为以下类型提供了内置 hash：
- `int`, `long`, `double` 等基本类型
- `string`, `string_view`
- 指针

**自定义类型（struct / pair / tuple）需要自己写 hash。**

## 6.2 pair 作为 key

```cpp
// pair<int,int> 没有内置 hash → 需要自定义
struct PairHash {
    size_t operator()(const pair<int,int>& p) const {
        size_t h1 = hash<int>{}(p.first);
        size_t h2 = hash<int>{}(p.second);
        return h1 ^ (h2 << 32);   // 简单异或+移位
    }
};

unordered_map<pair<int,int>, int, PairHash> mp;
mp[{1, 2}] = 100;
mp[{3, 4}] = 200;
```

## 6.3 自定义 struct 作为 key

```cpp
struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct PointHash {
    size_t operator()(const Point& p) const {
        return hash<int>{}(p.x) ^ (hash<int>{}(p.y) << 16);
    }
};

unordered_map<Point, string, PointHash> pointMap;
pointMap[{0, 0}] = "origin";
```

## 6.4 用 map 代替（不用写 hash）

```cpp
// 如果不想写自定义 hash，可以用 map
// map 只需要 operator< ，不需要 hash
struct Point {
    int x, y;
    bool operator<(const Point& other) const {
        return tie(x, y) < tie(other.x, other.y);
    }
};

map<Point, string> pointMap;  // 红黑树，不需要 hash
pointMap[{0, 0}] = "origin";
```

## 6.5 字符串编码技巧（避免自定义 hash）

```cpp
// 把 pair<int,int> 编码为字符串作为 key
string encode(int x, int y) {
    return to_string(x) + "," + to_string(y);
}

unordered_map<string, int> mp;
mp[encode(1, 2)] = 100;

// 这种方法代码简单，但性能略差（字符串构造开销）
// 刷题时完全可以接受 ✅
```

---

# 7. 哈希表经典解题模式

## 7.1 模式一：查找配对

```
问题：在数组中找两个元素满足某条件
技巧：遍历时查哈希表，看「互补元素」是否已存在

模板：
for (int i = 0; i < n; i++) {
    if (mp.count(complement))
        找到了！
    mp[nums[i]] = i;
}
```

## 7.2 模式二：计数频率

```
问题：统计频率 / 判断出现次数
技巧：用 map 计数

模板：
unordered_map<int, int> freq;
for (int x : nums) freq[x]++;

// 查找频率等于 k 的元素
for (auto& [num, cnt] : freq) {
    if (cnt == k) ...
}
```

## 7.3 模式三：分组归类

```
问题：把具有相同特征的元素分组
技巧：特征作为 key，元素列表作为 value

模板：
unordered_map<string, vector<int>> groups;
for (int i = 0; i < n; i++) {
    string key = computeKey(nums[i]);
    groups[key].push_back(i);
}
```

## 7.4 模式四：前缀和 + 哈希表

```
问题：连续子数组和等于 k
技巧：哈希表存前缀和出现的次数

模板：
unordered_map<int, int> prefixCount;
prefixCount[0] = 1;
int sum = 0;
for (int x : nums) {
    sum += x;
    if (prefixCount.count(sum - k))
        count += prefixCount[sum - k];
    prefixCount[sum]++;
}
```

## 7.5 模式五：映射索引

```
问题：需要记住「某个值最近/最早/出现的位置」
技巧：值 → 下标的映射

模板：
unordered_map<int, int> lastSeen;
for (int i = 0; i < n; i++) {
    if (lastSeen.count(nums[i])) {
        int dist = i - lastSeen[nums[i]];
        ...
    }
    lastSeen[nums[i]] = i;
}
```

---

# 8. 两数之和系列

## 8.1 LC 1: 两数之和

```cpp
// 面试必考第一题！
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> map;  // 值 → 下标
    for (int i = 0; i < (int)nums.size(); i++) {
        int complement = target - nums[i];
        if (map.count(complement))
            return {map[complement], i};
        map[nums[i]] = i;
    }
    return {};
}
// 时间 O(n), 空间 O(n)
// 核心：一边遍历一边查「互补数」
```

## 8.2 LC 454: 四数相加 II

```cpp
// 思路：A+B 的和存哈希表，遍历 C+D 查 -(C+D)
int fourSumCount(vector<int>& A, vector<int>& B,
                 vector<int>& C, vector<int>& D) {
    unordered_map<int, int> sumAB;
    for (int a : A)
        for (int b : B)
            sumAB[a + b]++;

    int count = 0;
    for (int c : C)
        for (int d : D)
            if (sumAB.count(-(c + d)))
                count += sumAB[-(c + d)];
    return count;
}
// 时间 O(n²), 空间 O(n²)
```

## 8.3 LC 560: 和为 K 的子数组

```cpp
// 前缀和 + 哈希表
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> prefixCount;
    prefixCount[0] = 1;    // 前缀和本身等于 k 的情况
    int sum = 0, count = 0;

    for (int num : nums) {
        sum += num;
        // 如果存在前缀和 = sum - k，说明中间那段的和 = k
        if (prefixCount.count(sum - k))
            count += prefixCount[sum - k];
        prefixCount[sum]++;
    }
    return count;
}
// 时间 O(n), 空间 O(n)
```

---

# 9. 字母异位词与分组

## 9.1 LC 49: 字母异位词分组

```cpp
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> groups;

    for (string& s : strs) {
        string key = s;
        sort(key.begin(), key.end());  // "eat" → "aet"
        groups[key].push_back(s);
    }

    vector<vector<string>> result;
    for (auto& [key, group] : groups)
        result.push_back(group);
    return result;
}

// 优化 key：用频率数组代替排序
// "eat" → "1#0#0#0#1#0#...#1#0#..."（每个字母出现次数）
string getKey(const string& s) {
    vector<int> cnt(26, 0);
    for (char c : s) cnt[c - 'a']++;
    string key;
    for (int i = 0; i < 26; i++) {
        key += to_string(cnt[i]) + "#";
    }
    return key;
}
```

## 9.2 LC 242: 有效的字母异位词

```cpp
bool isAnagram(string s, string t) {
    if (s.size() != t.size()) return false;
    // 方法 1: 排序比较（简单）
    // sort(s.begin(), s.end());
    // sort(t.begin(), t.end());
    // return s == t;

    // 方法 2: 频率计数（O(n)）
    vector<int> cnt(26, 0);
    for (char c : s) cnt[c - 'a']++;
    for (char c : t) {
        cnt[c - 'a']--;
        if (cnt[c - 'a'] < 0) return false;
    }
    return true;
}
```

---

# 10. 前缀和 + 哈希表

> 🔥 这是一个**极高频**的面试 + 竞赛组合技。

## 10.1 核心思想

```
前缀和 prefix[i] = nums[0] + nums[1] + ... + nums[i-1]

子数组 [l, r] 的和 = prefix[r+1] - prefix[l]

如果要找和 = k 的子数组：
  prefix[r+1] - prefix[l] = k
  ⟹ prefix[l] = prefix[r+1] - k

用哈希表存前缀和出现的次数，
遍历时查「当前前缀和 - k」是否出现过。
```

## 10.2 LC 525: 连续数组

```cpp
// 找最长的子数组使得 0 和 1 的个数相等
// 技巧：把 0 变成 -1，问题转化为「最长和为 0 的子数组」
int findMaxLength(vector<int>& nums) {
    unordered_map<int, int> firstSeen;
    firstSeen[0] = -1;    // 前缀和 0 出现在下标 -1
    int sum = 0, maxLen = 0;

    for (int i = 0; i < (int)nums.size(); i++) {
        sum += (nums[i] == 0 ? -1 : 1);
        if (firstSeen.count(sum)) {
            maxLen = max(maxLen, i - firstSeen[sum]);
        } else {
            firstSeen[sum] = i;  // 只记录第一次出现的位置
        }
    }
    return maxLen;
}
```

## 10.3 LC 974: 和可被 K 整除的子数组

```cpp
int subarraysDivByK(vector<int>& nums, int k) {
    unordered_map<int, int> modCount;
    modCount[0] = 1;
    int sum = 0, count = 0;

    for (int num : nums) {
        sum += num;
        int mod = ((sum % k) + k) % k;  // 处理负数取模
        count += modCount[mod];
        modCount[mod]++;
    }
    return count;
}
```

---

# 11. LRU / LFU 缓存

> 🔥 面试高频系统设计题。

## 11.1 LC 146: LRU 缓存

```cpp
// LRU = Least Recently Used（最近最少使用）
// 核心：哈希表 + 双向链表

class LRUCache {
    struct Node {
        int key, val;
        Node *prev, *next;
        Node(int k, int v) : key(k), val(v), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    unordered_map<int, Node*> map;   // key → 节点指针
    Node *head, *tail;               // 虚拟头尾节点

    void addToHead(Node* node) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }

    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(Node* node) {
        removeNode(node);
        addToHead(node);
    }

    Node* removeTail() {
        Node* node = tail->prev;
        removeNode(node);
        return node;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
    }

    int get(int key) {
        if (!map.count(key)) return -1;
        Node* node = map[key];
        moveToHead(node);       // 刚访问 → 移到头部
        return node->val;
    }

    void put(int key, int value) {
        if (map.count(key)) {
            Node* node = map[key];
            node->val = value;
            moveToHead(node);
        } else {
            Node* node = new Node(key, value);
            map[key] = node;
            addToHead(node);
            if ((int)map.size() > capacity) {
                Node* removed = removeTail();   // 淘汰最久没用的
                map.erase(removed->key);
                delete removed;
            }
        }
    }

    ~LRUCache() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }
};

// 图解:
// head ↔ [最近使用] ↔ [次近] ↔ ... ↔ [最久] ↔ tail
// get/put 时移到 head 后面
// 容量满时删 tail 前面的节点
```

---

# 12. 经典 LeetCode Map 题精讲

## 必做题清单

| 题号 | 题目 | 难度 | 核心技巧 | 频率 |
|------|------|------|----------|------|
| 1 | 两数之和 | Easy | 查找配对 | ⭐⭐⭐⭐⭐ |
| 49 | 字母异位词分组 | Medium | 分组归类 | ⭐⭐⭐⭐⭐ |
| 128 | 最长连续序列 | Medium | 集合查找 | ⭐⭐⭐⭐⭐ |
| 146 | LRU 缓存 | Medium | 哈希+链表 | ⭐⭐⭐⭐⭐ |
| 242 | 有效的字母异位词 | Easy | 频率计数 | ⭐⭐⭐⭐ |
| 454 | 四数相加 II | Medium | 两两配对 | ⭐⭐⭐⭐ |
| 560 | 和为K的子数组 | Medium | 前缀和+哈希 | ⭐⭐⭐⭐⭐ |
| 525 | 连续数组 | Medium | 前缀和+哈希 | ⭐⭐⭐⭐ |
| 974 | 和可被K整除的子数组 | Medium | 前缀和+取模 | ⭐⭐⭐⭐ |
| 205 | 同构字符串 | Easy | 双向映射 | ⭐⭐⭐ |
| 290 | 单词规律 | Easy | 双向映射 | ⭐⭐⭐ |
| 383 | 赎金信 | Easy | 频率计数 | ⭐⭐⭐ |
| 350 | 两个数组的交集 II | Easy | 频率计数 | ⭐⭐⭐ |
| 76 | 最小覆盖子串 | Hard | 滑动窗口+哈希 | ⭐⭐⭐⭐⭐ |
| 438 | 找所有字母异位词 | Medium | 滑动窗口+哈希 | ⭐⭐⭐⭐ |

---

# 13. 常见陷阱与最佳实践

## 13.1 经典陷阱

### 陷阱 1：`[]` 的副作用

```cpp
unordered_map<int, int> mp;

// ❌ 查找时用 []，不存在的 key 会被自动插入！
if (mp[42] == 0) {
    // 现在 mp 里多了一个 {42, 0}！
}

// ✅ 用 count() 或 find()
if (mp.count(42)) { ... }
if (mp.find(42) != mp.end()) { ... }
```

### 陷阱 2：遍历时修改

```cpp
// ❌ 遍历时删除/插入 → 迭代器失效
for (auto& [k, v] : mp) {
    if (v == 0) mp.erase(k);  // ❌ 未定义行为！
}

// ✅ 用 erase 的返回值
for (auto it = mp.begin(); it != mp.end(); ) {
    if (it->second == 0)
        it = mp.erase(it);   // erase 返回下一个有效迭代器
    else
        ++it;
}
```

### 陷阱 3：哈希冲突导致 TLE

```cpp
// 某些特殊输入（针对默认 hash 的 hack）会导致
// unordered_map 退化为 O(n) → TLE

// ✅ 解决方案 1: 换用 map（O(log n) 稳定）
// ✅ 解决方案 2: 自定义 hash（加随机种子）
struct SafeHash {
    size_t operator()(int x) const {
        static const size_t FIXED_RANDOM =
            chrono::steady_clock::now().time_since_epoch().count();
        return x ^ (FIXED_RANDOM + 0x9e3779b9 + (x << 6) + (x >> 2));
    }
};
unordered_map<int, int, SafeHash> mp;
```

### 陷阱 4：对 unordered_map 排序

```cpp
// ❌ unordered_map 没有顺序，不能直接排序

// ✅ 方案 1: 转成 vector<pair> 后排序
vector<pair<string,int>> v(mp.begin(), mp.end());
sort(v.begin(), v.end(), [](auto& a, auto& b) {
    return a.second > b.second;  // 按 value 降序
});

// ✅ 方案 2: 直接用 map（按 key 有序）
```

## 13.2 性能优化

```cpp
// 1. 预分配桶数量（减少 rehash）
unordered_map<int, int> mp;
mp.reserve(10000);  // 预分配 10000 个桶

// 2. 用数组代替哈希表（key 范围小时）
// 比如 key 是 [0, 1000] → 直接用 int arr[1001]
int freq[1001] = {};   // 比 unordered_map<int,int> 快 3-5 倍

// 3. 用 char freq[128] 代替 map<char, int>
// 字符的 ASCII 范围 0~127
int charFreq[128] = {};
for (char c : s) charFreq[c]++;
```

## 13.3 Map 技巧速查卡

```
┌──────────────────────────────────────────────────────┐
│              Map 技巧速查                             │
├──────────────────────────────────────────────────────┤
│  查找配对       → 一边遍历一边查互补数                  │
│  频率计数       → map[x]++ / 数组 freq[x]++            │
│  分组归类       → 特征编码作 key，元素列表作 value       │
│  前缀和+哈希    → 存前缀和出现次数，查 sum-k             │
│  双向映射       → 两个 map 互查                        │
│  滑动窗口+哈希  → map 记录窗口内频率                    │
│  LRU 缓存      → 哈希表 + 双向链表                     │
│  避免 [] 副作用 → 查找用 count() / find()              │
│  防 hash hack  → 加随机种子或换 map                    │
│  小范围 key    → 用数组代替 map（更快）                  │
└──────────────────────────────────────────────────────┘
```

---

*📝 下一步：配合 `knowlege_details_4_set.md` 学习集合（Set）*

*💪 哈希表是面试最高频的数据结构之一，务必吃透两数之和、前缀和+哈希、LRU！*
