# 专题十四：堆与优先队列 — 从原理到LeetCode实战

> 作者：大胖超 😜
> 
> 堆（Heap）是解决 Top-K、动态中位数、任务调度、多路归并等问题的核心数据结构。掌握堆的底层原理和 C++ `priority_queue` 的灵活使用，是面试和竞赛中拿分的利器。

---

## 目录

1. [堆的基本概念](#1-堆的基本概念)
2. [堆的核心操作](#2-堆的核心操作)
3. [C++ priority_queue 详解](#3-c-priority_queue-详解)
4. [Top-K 问题](#4-top-k-问题)
5. [动态中位数](#5-动态中位数)
6. [多路归并](#6-多路归并)
7. [任务调度与贪心](#7-任务调度与贪心)
8. [手写堆（面试考点）](#8-手写堆面试考点)
9. [高级技巧：懒删除与可删堆](#9-高级技巧懒删除与可删堆)
10. [LeetCode 题目总结与易错点](#10-leetcode-题目总结与易错点)

---

## 1. 堆的基本概念

### 1.1 什么是堆

堆是一棵**完全二叉树**，存储在数组中，满足**堆序性**：

```
最大堆 (Max-Heap):              最小堆 (Min-Heap):
  每个节点 >= 其子节点               每个节点 <= 其子节点

       9                              1
      / \                            / \
     7   8                          3   2
    / \ /                          / \ /
   3  5 2                         7  5 4

数组: [9,7,8,3,5,2]              数组: [1,3,2,7,5,4]
```

### 1.2 数组存储与下标关系

```
对于下标 i (0-based):
  父节点:     (i - 1) / 2
  左子节点:   2 * i + 1
  右子节点:   2 * i + 2

最后一个非叶节点: n / 2 - 1
```

### 1.3 堆 vs 二叉搜索树 (BST)

| 特性 | 堆 | BST |
|------|-----|-----|
| 结构 | 完全二叉树 | 任意二叉树 |
| 有序性 | 仅父子有序 | 左<中<右 |
| 查找最值 | O(1) | O(logn) 或 O(n) |
| 查找任意值 | O(n) | O(logn) |
| 插入 | O(logn) | O(logn) |
| 删除最值 | O(logn) | O(logn) |
| 适用场景 | 最值/Top-K | 有序遍历/区间查找 |

### 1.4 堆的时间复杂度

| 操作 | 时间复杂度 | 说明 |
|------|-----------|------|
| 获取最值 | O(1) | 堆顶 |
| 插入 | O(logn) | 上浮 |
| 删除最值 | O(logn) | 下沉 |
| 建堆 | O(n) | 自底向上 |
| 堆排序 | O(nlogn) | n次删除 |

> **为什么建堆是 O(n)？**
> 自底向上建堆，第 h 层有约 n/2^(h+1) 个节点，每个下沉 h 步。
> 总工作量 = Σ(h × n/2^(h+1)) = n × Σ(h/2^(h+1)) = O(n)

---

## 2. 堆的核心操作

### 2.1 上浮 (Sift Up / Swim)

**插入**新元素时使用：放到末尾，然后不断与父节点比较交换。

```
插入 10 到最大堆 [9, 7, 8, 3, 5, 2]:

放末尾: [9, 7, 8, 3, 5, 2, 10]
                                  ↑
10 > 父(8) → swap:
         [9, 7, 10, 3, 5, 2, 8]
               ↑
10 > 父(9) → swap:
         [10, 7, 9, 3, 5, 2, 8]
          ↑ 到堆顶，停止
```

```cpp
void siftUp(vector<int>& heap, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[i] > heap[parent]) {  // 最大堆
            swap(heap[i], heap[parent]);
            i = parent;
        } else {
            break;
        }
    }
}
```

### 2.2 下沉 (Sift Down / Sink)

**删除**堆顶时使用：将末尾元素放到堆顶，然后向下调整。

```
删除堆顶 10 从 [10, 7, 9, 3, 5, 2, 8]:

末尾 8 替换堆顶: [8, 7, 9, 3, 5, 2]
                   ↓
8 < 子max(9) → swap:
         [9, 7, 8, 3, 5, 2]
               ↓
8 > 子max(2) → 停止
```

```cpp
void siftDown(vector<int>& heap, int n, int i) {
    while (true) {
        int largest = i;
        int left = 2 * i + 1, right = 2 * i + 2;
        if (left < n && heap[left] > heap[largest]) largest = left;
        if (right < n && heap[right] > heap[largest]) largest = right;
        if (largest == i) break;
        swap(heap[i], heap[largest]);
        i = largest;
    }
}
```

### 2.3 建堆 (Build Heap)

从最后一个非叶节点开始，逐个下沉。

```cpp
void buildHeap(vector<int>& heap) {
    int n = heap.size();
    // 从最后一个非叶节点开始
    for (int i = n / 2 - 1; i >= 0; i--)
        siftDown(heap, n, i);
}
```

---

## 3. C++ priority_queue 详解

### 3.1 基本用法

```cpp
#include <queue>
using namespace std;

// 最大堆（默认）
priority_queue<int> maxPQ;

// 最小堆
priority_queue<int, vector<int>, greater<int>> minPQ;

// 常用操作
maxPQ.push(5);         // 插入 O(logn)
maxPQ.top();           // 堆顶 O(1)
maxPQ.pop();           // 删除堆顶 O(logn)
maxPQ.empty();         // 是否为空
maxPQ.size();          // 元素数量
```

### 3.2 自定义比较器（重要！）

```cpp
// ⚠️ priority_queue 比较器 vs sort 比较器 方向相反！
// priority_queue: 返回 true 的元素沉底（优先级低）
// sort: 返回 true 的元素在前

// 方式1: lambda（C++20 可直接用，C++17 需传入构造函数）
auto cmp = [](int a, int b) { return a > b; };  // 这是最小堆
priority_queue<int, vector<int>, decltype(cmp)> pq(cmp);

// 方式2: struct
struct Compare {
    bool operator()(const pair<int,int>& a, const pair<int,int>& b) {
        return a.second > b.second;  // 按 second 升序 → 最小堆
    }
};
priority_queue<pair<int,int>, vector<pair<int,int>>, Compare> pq;

// 方式3: greater / less
priority_queue<int, vector<int>, greater<int>> minHeap;  // 最小堆
priority_queue<int, vector<int>, less<int>> maxHeap;      // 最大堆(默认)
```

### 3.3 比较器方向速记

```
⚠️ 最容易搞混的点！

priority_queue 比较器的含义：
  cmp(a, b) = true → a 的优先级比 b 低 → a 沉底

所以：
  greater<int> → "a>b时a沉底" → 小的在顶 → 最小堆
  less<int>    → "a<b时a沉底" → 大的在顶 → 最大堆

  自定义: return a.val > b.val → 最小堆 (val小的在顶)
  自定义: return a.val < b.val → 最大堆 (val大的在顶)
```

### 3.4 pair/tuple 在优先队列中的行为

```cpp
// pair 本质是按first排，first相同按second排
// 最小堆: 先按first升序，再按second升序
priority_queue<pair<int,int>,
               vector<pair<int,int>>,
               greater<pair<int,int>>> pq;

pq.push({3, "B"});
pq.push({1, "A"});
pq.push({3, "A"});
// pop 顺序: {1,"A"}, {3,"A"}, {3,"B"}
```

### 3.5 priority_queue 的局限

| 问题 | 说明 | 解决方案 |
|------|------|---------|
| 不支持删除指定元素 | 只能 pop 堆顶 | 懒删除（见第9节） |
| 不支持修改优先级 | 无 decrease-key | push新值 + 旧值标记 |
| 不支持遍历 | 只能逐个 pop | 复制一份再 pop |
| 不支持合并 | 两个堆不能合并 | 逐个插入 O(nlogn) |

---

## 4. Top-K 问题

### 4.1 通用策略

```
找第 K 大:
├── 全排序 → O(nlogn) — 最简单
├── 最小堆大小K → O(nlogk) — 堆中维持K大元素，堆顶就是第K大
├── 快速选择 → O(n) 平均 — 最快但最坏O(n²)
└── nth_element → O(n) — STL内置

找前 K 大(不要求有序):
├── 最小堆大小K → O(nlogk) — 堆中K个就是答案
├── 快速选择 → O(n) — 分区后左侧K个
└── 桶排序 → O(n) — 值域不大时
```

### 4.2 核心思路：用相反的堆

```
关键直觉：
  找前 K 大 → 用最小堆！（大小K，堆顶是第K大）
  找前 K 小 → 用最大堆！（大小K，堆顶是第K小）

为什么？
  维护K个最大元素，新元素比堆顶大才能进来
  → 堆顶始终是这K个中最小的 = 第K大元素
```

### 4.3 LC 215: 第K个最大元素

```
输入: nums = [3,2,1,5,6,4], k = 2
输出: 5

方法: 最小堆大小2
  push 3 → [3]
  push 2 → [2,3]
  push 1 → 1<堆顶2, pop 1 不进 → 实际: push 1→[1,2,3], pop→[2,3]
  push 5 → [2,3,5], pop→[3,5]
  push 6 → [3,5,6], pop→[5,6]
  push 4 → [4,5,6], pop→[5,6]

  堆顶 = 5 = 第2大
```

```cpp
int findKthLargest(vector<int>& nums, int k) {
    priority_queue<int, vector<int>, greater<int>> minHeap;
    for (int num : nums) {
        minHeap.push(num);
        if ((int)minHeap.size() > k) minHeap.pop();
    }
    return minHeap.top();
}
```

### 4.4 LC 347: 前K个高频元素

```cpp
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int x : nums) freq[x]++;
    
    // 最小堆按频率，维护前K高频
    auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
        return a.second > b.second;
    };
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);
    
    for (auto& [val, cnt] : freq) {
        pq.push({val, cnt});
        if ((int)pq.size() > k) pq.pop();
    }
    
    vector<int> result;
    while (!pq.empty()) {
        result.push_back(pq.top().first);
        pq.pop();
    }
    return result;
}
```

### 4.5 LC 973: 最接近原点的K个点

```cpp
vector<vector<int>> kClosest(vector<vector<int>>& points, int k) {
    // 最大堆按距离，维护K个最近的
    auto cmp = [](vector<int>& a, vector<int>& b) {
        return a[0]*a[0]+a[1]*a[1] < b[0]*b[0]+b[1]*b[1];
    };
    priority_queue<vector<int>, vector<vector<int>>, decltype(cmp)> pq(cmp);
    
    for (auto& p : points) {
        pq.push(p);
        if ((int)pq.size() > k) pq.pop();
    }
    
    vector<vector<int>> result;
    while (!pq.empty()) {
        result.push_back(pq.top());
        pq.pop();
    }
    return result;
}
```

---

## 5. 动态中位数

### 5.1 问题描述

数据流不断到来，随时查询当前所有数的中位数。

### 5.2 双堆法（经典！）

```
核心思想：用两个堆把数据分成两半

  maxHeap (大顶堆)     minHeap (小顶堆)
  存较小的一半           存较大的一半
     3                    5
    / \                  / \
   1   2                7   6

  中位数 = maxHeap.top() 或 (maxHeap.top() + minHeap.top()) / 2

约束：
  1. maxHeap.size() == minHeap.size() 或 maxHeap.size() == minHeap.size() + 1
  2. maxHeap.top() <= minHeap.top()
```

### 5.3 插入规则

```
addNum(num):
  1. 先无脑放入 maxHeap
  2. 把 maxHeap 堆顶吐给 minHeap（保证左半≤右半）
  3. 如果 minHeap 比 maxHeap 大 → 把 minHeap 堆顶还给 maxHeap

这样三步保证：
  ✓ maxHeap.top() <= minHeap.top()
  ✓ maxHeap.size() >= minHeap.size()
  ✓ 大小差最多1
```

### 5.4 LC 295: 数据流的中位数

```cpp
class MedianFinder {
    priority_queue<int> maxHeap;  // 左半，较小
    priority_queue<int, vector<int>, greater<int>> minHeap;  // 右半，较大
public:
    void addNum(int num) {
        maxHeap.push(num);
        minHeap.push(maxHeap.top());
        maxHeap.pop();
        if (minHeap.size() > maxHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }
    
    double findMedian() {
        if (maxHeap.size() > minHeap.size())
            return maxHeap.top();
        return (maxHeap.top() + minHeap.top()) / 2.0;
    }
};
```

### 5.5 图解过程

```
依次加入 [5, 2, 8, 3, 7]:

加入5: max={5}, min={} → 中位数=5
加入2: max→{5,2}, pop 5→min → max={2}, min={5}
       大小相等 → 中位数=(2+5)/2=3.5
加入8: max→{8,2}, pop 8→min → max={2}, min={5,8}
       min大了 → pop 5→max → max={5,2}, min={8}
       中位数=5
加入3: max→{5,3,2}, pop 5→min → max={3,2}, min={5,8}
       大小相等 → 中位数=(3+5)/2=4
加入7: max→{7,3,2}, pop 7→min → max={3,2}, min={5,7,8}
       min大了 → pop 5→max → max={5,3,2}, min={7,8}
       中位数=5
```

---

## 6. 多路归并

### 6.1 核心思想

将 K 个有序序列合并成一个有序序列，用**最小堆**维护每个序列当前最小值。

```
K个有序链表:
  L1: 1 → 4 → 7
  L2: 2 → 5 → 8
  L3: 3 → 6 → 9

最小堆初始: {1, 2, 3}

pop 1(L1), push 4 → 堆: {2, 3, 4} → 输出: 1
pop 2(L2), push 5 → 堆: {3, 4, 5} → 输出: 1,2
pop 3(L3), push 6 → 堆: {4, 5, 6} → 输出: 1,2,3
...
```

### 6.2 LC 23: 合并K个升序链表

```cpp
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);
    
    for (auto h : lists)
        if (h) pq.push(h);
    
    ListNode dummy(0);
    ListNode* tail = &dummy;
    while (!pq.empty()) {
        auto node = pq.top(); pq.pop();
        tail->next = node;
        tail = node;
        if (node->next) pq.push(node->next);
    }
    return dummy.next;
}
// 时间 O(nlogk), 空间 O(k)
// n 是所有节点总数, k 是链表个数
```

### 6.3 LC 378: 有序矩阵中第K小的元素

```
矩阵每行每列都递增:
  [1,  5,  9 ]
  [10, 11, 13]
  [12, 13, 15]

思路: 把每行第一个元素入堆，每次pop最小值后push同行下一个
```

```cpp
int kthSmallest(vector<vector<int>>& matrix, int k) {
    int n = matrix.size();
    // {值, 行, 列}
    auto cmp = [](tuple<int,int,int>& a, tuple<int,int,int>& b) {
        return get<0>(a) > get<0>(b);
    };
    priority_queue<tuple<int,int,int>,
                   vector<tuple<int,int,int>>,
                   decltype(cmp)> pq(cmp);
    
    // 每行第一个元素入堆
    for (int i = 0; i < n; i++)
        pq.push({matrix[i][0], i, 0});
    
    int result = 0;
    for (int i = 0; i < k; i++) {
        auto [val, row, col] = pq.top(); pq.pop();
        result = val;
        if (col + 1 < n)
            pq.push({matrix[row][col + 1], row, col + 1});
    }
    return result;
}
```

### 6.4 LC 373: 查找和最小的K对数字

```
给定两个升序数组，找到和最小的 k 对 (u, v)。

nums1 = [1,7,11], nums2 = [2,4,6], k = 3
所有对: (1,2)=3, (1,4)=5, (1,6)=7, (7,2)=9, ...
前3小: [(1,2), (1,4), (1,6)]

思路: 初始堆中放 {nums1[i]+nums2[0], i, 0}
     pop 时 push {nums1[i]+nums2[j+1], i, j+1}
```

```cpp
vector<vector<int>> kSmallestPairs(vector<int>& nums1, vector<int>& nums2, int k) {
    auto cmp = [](tuple<int,int,int>& a, tuple<int,int,int>& b) {
        return get<0>(a) > get<0>(b);
    };
    priority_queue<tuple<int,int,int>,
                   vector<tuple<int,int,int>>,
                   decltype(cmp)> pq(cmp);
    
    int m = nums1.size(), n = nums2.size();
    for (int i = 0; i < min(m, k); i++)
        pq.push({nums1[i] + nums2[0], i, 0});
    
    vector<vector<int>> result;
    while (!pq.empty() && (int)result.size() < k) {
        auto [sum, i, j] = pq.top(); pq.pop();
        result.push_back({nums1[i], nums2[j]});
        if (j + 1 < n)
            pq.push({nums1[i] + nums2[j + 1], i, j + 1});
    }
    return result;
}
```

---

## 7. 任务调度与贪心

### 7.1 LC 621: 任务调度器

```
任务 = ['A','A','A','B','B','B'], 冷却 n=2
A执行后需等2个间隔再执行下一个A

最优调度: A B _ A B _ A B → 长度8

思路: 贪心 — 频率高的先安排
     最大堆存剩余次数，每轮取最多 n+1 个任务执行
```

```cpp
int leastInterval(vector<char>& tasks, int n) {
    vector<int> freq(26, 0);
    for (char c : tasks) freq[c - 'A']++;
    
    priority_queue<int> pq;  // 最大堆存频率
    for (int f : freq) if (f > 0) pq.push(f);
    
    int time = 0;
    while (!pq.empty()) {
        vector<int> temp;
        int cycle = n + 1;  // 每轮最多处理 n+1 个任务
        for (int i = 0; i < cycle; i++) {
            if (!pq.empty()) {
                int f = pq.top(); pq.pop();
                if (f > 1) temp.push_back(f - 1);
            }
            time++;
            if (pq.empty() && temp.empty()) break;
        }
        for (int f : temp) pq.push(f);
    }
    return time;
}
```

### 7.2 LC 767: 重组字符串

```
使相邻字符不相同。贪心：每次从堆中取出频率最高的未使用字符。

"aab" → "aba" ✓
"aaab" → 不可能 ✗
```

```cpp
string reorganizeString(string s) {
    vector<int> freq(26, 0);
    for (char c : s) freq[c - 'a']++;
    
    // 最大堆 {freq, char}
    priority_queue<pair<int,char>> pq;
    for (int i = 0; i < 26; i++)
        if (freq[i] > 0) pq.push({freq[i], 'a' + i});
    
    string result;
    while (pq.size() > 1) {
        auto [f1, c1] = pq.top(); pq.pop();
        auto [f2, c2] = pq.top(); pq.pop();
        result += c1;
        result += c2;
        if (f1 > 1) pq.push({f1 - 1, c1});
        if (f2 > 1) pq.push({f2 - 1, c2});
    }
    if (!pq.empty()) {
        auto [f, c] = pq.top();
        if (f > 1) return "";  // 不可能
        result += c;
    }
    return result;
}
```

---

## 8. 手写堆（面试考点）

### 8.1 完整手写最小堆

面试可能要求不使用 STL 手写堆：

```cpp
class MinHeap {
    vector<int> data;
    
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (data[i] < data[parent]) {
                swap(data[i], data[parent]);
                i = parent;
            } else break;
        }
    }
    
    void siftDown(int i) {
        int n = data.size();
        while (2 * i + 1 < n) {
            int child = 2 * i + 1;
            // 选较小的子节点
            if (child + 1 < n && data[child + 1] < data[child])
                child++;
            if (data[i] <= data[child]) break;
            swap(data[i], data[child]);
            i = child;
        }
    }
    
public:
    void push(int val) {
        data.push_back(val);
        siftUp(data.size() - 1);
    }
    
    int top() { return data[0]; }
    
    void pop() {
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) siftDown(0);
    }
    
    bool empty() { return data.empty(); }
    int size() { return data.size(); }
};
```

### 8.2 面试常见手写变体

| 变体 | 说明 |
|------|------|
| 支持任意删除 | 标记删除 + 延迟清理 |
| 支持修改值 | 找到下标 + siftUp/siftDown |
| K路归并 | 手写堆存 {值, 来源编号} |
| 中位数维护 | 两个堆: maxHeap + minHeap |

---

## 9. 高级技巧：懒删除与可删堆

### 9.1 问题

`priority_queue` 不支持删除指定元素。在滑动窗口等场景中，需要"删除"不在窗口内的元素。

### 9.2 懒删除法

用一个 map/set 记录需要删除的元素，在 `top()` 时检查：

```cpp
class LazyHeap {
    priority_queue<int, vector<int>, greater<int>> pq;
    unordered_map<int, int> toDelete;  // 值 → 待删次数
    
public:
    void push(int val) { pq.push(val); }
    
    void remove(int val) { toDelete[val]++; }
    
    int top() {
        // 清除堆顶的待删元素
        while (!pq.empty() && toDelete.count(pq.top()) && toDelete[pq.top()] > 0) {
            toDelete[pq.top()]--;
            if (toDelete[pq.top()] == 0) toDelete.erase(pq.top());
            pq.pop();
        }
        return pq.top();
    }
    
    void pop() {
        top();  // 先清理
        pq.pop();
    }
};
```

### 9.3 LC 480: 滑动窗口中位数

```
nums = [1,3,-1,-3,5,3,6,7], k = 3

窗口 [1,3,-1] → 中位数 1
窗口 [3,-1,-3] → 中位数 -1
...

思路: 双堆 + 懒删除
  maxHeap 存较小一半, minHeap 存较大一半
  窗口滑动时，离开的元素做"懒删除"标记
```

---

## 10. LeetCode 题目总结与易错点

### 10.1 题目分类

#### Top-K 问题
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 215 | 第K个最大元素 | 中等 | 最小堆/快选 |
| 347 | 前K个高频元素 | 中等 | 频率+最小堆 |
| 692 | 前K个高频单词 | 中等 | 自定义比较器 |
| 973 | 最接近原点的K个点 | 中等 | 距离+最大堆 |
| 703 | 数据流中第K大 | 简单 | 最小堆维护 |

#### 多路归并
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 23 | 合并K个升序链表 | 困难 | K路归并经典 |
| 378 | 有序矩阵第K小 | 中等 | 矩阵多路归并 |
| 373 | 和最小的K对数字 | 中等 | 双数组多路 |
| 264 | 丑数II | 中等 | 三路归并 |

#### 中位数/双堆
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 295 | 数据流中位数 | 困难 | 双堆经典 |
| 480 | 滑动窗口中位数 | 困难 | 双堆+懒删 |
| 502 | IPO | 困难 | 双堆贪心 |

#### 调度/贪心
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 621 | 任务调度器 | 中等 | 堆+贪心 |
| 767 | 重组字符串 | 中等 | 频率+交替 |
| 1054 | 距离相等的条形码 | 中等 | 频率+交替 |
| 1353 | 最多可以参加的会议数 | 中等 | 贪心+最小堆 |

#### 其他
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 239 | 滑动窗口最大值 | 困难 | 单调队列更优/堆懒删 |
| 355 | 设计推特 | 中等 | 多路归并+设计 |
| 1675 | 最小化数组偏差 | 困难 | 堆+贪心+变换 |

### 10.2 高频易错点

```
❌ 1. 比较器方向搞反
   想要最小堆却写了 return a < b → 实际是最大堆
   ✅ 记住: greater → 最小堆, 自定义 a>b → 最小堆

❌ 2. 忘记堆大小限制
   Top-K 问题忘记 if(pq.size()>k) pq.pop()
   → 堆持续增长至O(n)，失去O(nlogk)优势
   ✅ 每次push后检查大小

❌ 3. LC 23 链表为 nullptr
   空链表不要push进堆
   ✅ for (auto h : lists) if (h) pq.push(h)

❌ 4. 中位数双堆不平衡
   addNum 后两堆大小差 > 1
   ✅ 严格按三步走: 入max → max顶给min → 平衡

❌ 5. priority_queue 和 sort 比较器方向混淆
   sort: cmp(a,b)=true → a在前
   pq:   cmp(a,b)=true → a沉底(优先级低)
   ✅ 两者方向相反！

❌ 6. 浮点数中位数丢精度
   (maxHeap.top() + minHeap.top()) / 2 → 整数除法
   ✅ 用 2.0 或 static_cast<double>
```

### 10.3 面试高频问答

> **Q: 堆和优先队列的区别？**  
> A: 堆是数据结构，优先队列是抽象概念。优先队列可以用堆实现，也可以用其他方式。C++ `priority_queue` 底层就是堆。

> **Q: 什么时候用堆而不是排序？**  
> A: 当数据是**动态**到来的（流式），或者只需要**部分排序**（Top-K）时，堆更优。排序是一次性处理所有数据。

> **Q: 建堆为什么是 O(n) 而不是 O(nlogn)？**  
> A: 自底向上建堆，越底层节点越多但下沉距离越短。精确分析：Σ(n/2^(h+1) × h) ≈ n。

> **Q: 堆排序为什么不常用？**  
> A: 缓存不友好（数组跳跃访问），常数因子比快排大。实际中 `std::sort` (IntroSort) 更快。

---

> 📌 **配套代码**：[knowlege_details_14_heap.cpp](knowlege_details_14_heap.cpp) 包含上述所有算法的可运行演示。
