# 📖 专题三（下）：队列（Queue）— 从零基础到精通

*作者：大胖超 😜 | 配合 leetcode_knowledge_base.md 食用更佳*

---

## 目录

1. [队列的基本概念](#1-队列的基本概念)
2. [C++ 中的队列实现](#2-c-中的队列实现)
3. [手写队列（加深理解）](#3-手写队列加深理解)
4. [双端队列 deque](#4-双端队列-deque)
5. [用队列实现栈 / 用栈实现队列](#5-用队列实现栈--用栈实现队列)
6. [BFS 与队列](#6-bfs-与队列)
7. [优先队列 / 堆（priority_queue）](#7-优先队列--堆priority_queue)
8. [单调队列（Monotonic Queue）](#8-单调队列monotonic-queue)
9. [循环队列](#9-循环队列)
10. [队列在系统设计中的应用](#10-队列在系统设计中的应用)
11. [经典 LeetCode 队列题精讲](#11-经典-leetcode-队列题精讲)
12. [常见陷阱与最佳实践](#12-常见陷阱与最佳实践)

---

# 1. 队列的基本概念

## 1.1 什么是队列？

**队列 (Queue)** = **先进先出 (FIFO: First In, First Out)**

生活类比：
- 🧑‍🤝‍🧑 排队买饭：先来的先买
- 🖨️ 打印队列：先提交的先打印
- 📬 消息队列：按顺序处理消息
- 🚗 收费站排队：先到先走

```
      入队 (enqueue/push)
         ↓
  队尾 → [4, 3, 2, 1] → 队头
                          ↓
                    出队 (dequeue/pop)  → 取出 1
```

## 1.2 核心操作与复杂度

| 操作 | 说明 | 时间复杂度 |
|------|------|-----------|
| `push(x)` / `enqueue` | 元素入队（队尾） | O(1) |
| `pop()` / `dequeue` | 队头元素出队 | O(1) |
| `front()` | 查看队头 | O(1) |
| `back()` | 查看队尾 | O(1) |
| `empty()` | 判断是否为空 | O(1) |
| `size()` | 获取元素个数 | O(1) |

## 1.3 队列的应用场景

```
队列的使用场景：
├─ BFS（广度优先搜索） → 图/树的层序遍历
├─ 滑动窗口最大值 → 单调队列
├─ 任务调度 → 操作系统进程调度
├─ 消息队列 → Kafka, RabbitMQ
├─ 缓冲区 → IO 缓冲、网络数据包
├─ 树的层序遍历 → LC 102, 107, 199
└─ 拓扑排序 → DAG 中的 BFS
```

---

# 2. C++ 中的队列实现

## 2.1 `std::queue`

```cpp
#include <queue>

queue<int> q;

q.push(10);        // 入队
q.push(20);
q.push(30);

cout << q.front();  // 10（队头）
cout << q.back();   // 30（队尾）
cout << q.size();   // 3

q.pop();            // 出队 10  ⚠️ 返回 void！
cout << q.front();  // 20
```

> ⚠️ 和 stack 一样，`pop()` 不返回值！先 `front()` 再 `pop()`。

## 2.2 底层容器

```cpp
// queue 默认底层是 deque
queue<int>                q1;  // 底层 deque（推荐）
queue<int, list<int>>     q2;  // 底层 list

// ⚠️ 不能用 vector 做 queue 的底层
// 因为 vector 不支持 pop_front()
```

## 2.3 各种队列对比

| 类型 | 头文件 | 特点 |
|------|--------|------|
| `queue` | `<queue>` | 普通 FIFO 队列 |
| `deque` | `<deque>` | 双端队列，两头都能进出 |
| `priority_queue` | `<queue>` | 优先队列/堆，最大（小）元素先出 |
| `list` | `<list>` | 双向链表，可当队列用 |

---

# 3. 手写队列（加深理解）

## 3.1 链表实现

```cpp
class LinkedQueue {
    struct Node {
        int val;
        Node* next;
        Node(int v) : val(v), next(nullptr) {}
    };
    Node* head = nullptr;   // 队头（出队端）
    Node* tail = nullptr;   // 队尾（入队端）
    int count = 0;

public:
    void push(int val) {
        Node* node = new Node(val);
        if (tail) tail->next = node;
        tail = node;
        if (!head) head = node;
        count++;
    }

    void pop() {
        if (!head) throw runtime_error("Queue is empty");
        Node* old = head;
        head = head->next;
        if (!head) tail = nullptr;
        delete old;
        count--;
    }

    int front() {
        if (!head) throw runtime_error("Queue is empty");
        return head->val;
    }

    bool empty() { return head == nullptr; }
    int size() { return count; }

    ~LinkedQueue() {
        while (head) { Node* t = head; head = head->next; delete t; }
    }
};
```

## 3.2 数组实现（环形队列思路）

```cpp
class ArrayQueue {
    vector<int> data;
    int headIdx = 0;   // 队头下标
    int count = 0;

public:
    ArrayQueue(int capacity) : data(capacity) {}

    void push(int val) {
        if (count == (int)data.size()) throw runtime_error("Queue full");
        int tailIdx = (headIdx + count) % data.size();
        data[tailIdx] = val;
        count++;
    }

    void pop() {
        if (count == 0) throw runtime_error("Queue empty");
        headIdx = (headIdx + 1) % data.size();
        count--;
    }

    int front() {
        if (count == 0) throw runtime_error("Queue empty");
        return data[headIdx];
    }

    bool empty() { return count == 0; }
    int size() { return count; }
};
```

---

# 4. 双端队列 deque

> deque (Double-Ended Queue)：两头都能进出的队列。

## 4.1 `std::deque`

```cpp
#include <deque>

deque<int> dq;

// 两端操作
dq.push_back(1);       // [1]
dq.push_back(2);       // [1, 2]
dq.push_front(0);      // [0, 1, 2]
dq.push_front(-1);     // [-1, 0, 1, 2]

cout << dq.front();    // -1
cout << dq.back();     // 2

dq.pop_front();         // [0, 1, 2]
dq.pop_back();          // [0, 1]

// 支持随机访问！
cout << dq[0];          // 0
cout << dq[1];          // 1
cout << dq.size();      // 2
```

## 4.2 deque vs vector

| 特性 | deque | vector |
|------|-------|--------|
| 头部插入/删除 | **O(1)** | O(n) |
| 尾部插入/删除 | O(1) | O(1) 均摊 |
| 随机访问 | O(1) | O(1) |
| 内存布局 | 分段连续 | 完全连续 |
| 缓存友好性 | 较差 | **好** |

> 需要头部操作 → deque，只需尾部操作 → vector。

---

# 5. 用队列实现栈 / 用栈实现队列

## 5.1 用栈实现队列

```cpp
// LC 232: 用栈实现队列
class MyQueue {
    stack<int> inStack, outStack;

    void transfer() {
        while (!inStack.empty()) {
            outStack.push(inStack.top());
            inStack.pop();
        }
    }

public:
    void push(int x) { inStack.push(x); }

    int pop() {
        if (outStack.empty()) transfer();
        int val = outStack.top(); outStack.pop();
        return val;
    }

    int peek() {
        if (outStack.empty()) transfer();
        return outStack.top();
    }

    bool empty() { return inStack.empty() && outStack.empty(); }
};

// 原理图解：
// push 1,2,3 → inStack: [3,2,1] (top=3)
// 当需要 pop/peek 时，把 in 全倒入 out:
// outStack: [1,2,3] (top=1) → pop 返回 1 ✅ FIFO!
```

## 5.2 用队列实现栈

```cpp
// LC 225: 用队列实现栈
class MyStack {
    queue<int> q;

public:
    void push(int x) {
        q.push(x);
        // 把前面的元素全部重新排到后面
        int sz = q.size();
        for (int i = 0; i < sz - 1; i++) {
            q.push(q.front());
            q.pop();
        }
    }

    int pop() {
        int val = q.front();
        q.pop();
        return val;
    }

    int top() { return q.front(); }
    bool empty() { return q.empty(); }
};

// 原理：push(x) 后把 x 前面的元素都转到后面
// push 1: [1]
// push 2: [2,1]  (push 2, 然后 1 转到后面 → [2,1])
// push 3: [3,2,1] (push 3, 然后 2,1 转到后面)
// pop() → 3 ✅ LIFO!
```

---

# 6. BFS 与队列

> 🎯 **BFS（广度优先搜索）是队列最重要的应用。**

## 6.1 BFS 模板

```cpp
// 通用 BFS 模板
void bfs(起点) {
    queue<State> q;
    set<State> visited;

    q.push(起点);
    visited.insert(起点);

    while (!q.empty()) {
        State curr = q.front(); q.pop();

        if (curr == 目标) return 结果;

        for (State next : 相邻状态) {
            if (!visited.count(next)) {
                visited.insert(next);
                q.push(next);
            }
        }
    }
}
```

## 6.2 二叉树层序遍历

```cpp
// LC 102: 二叉树的层序遍历
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (!root) return result;

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int levelSize = q.size();    // 当前层的节点数
        vector<int> level;

        for (int i = 0; i < levelSize; i++) {
            TreeNode* node = q.front(); q.pop();
            level.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
        result.push_back(level);
    }
    return result;
}
```

## 6.3 图的 BFS（最短路径）

```cpp
// LC 994: 腐烂的橘子
// 多源 BFS：所有烂橘子同时开始扩散
int orangesRotting(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    queue<pair<int,int>> q;
    int fresh = 0;

    // 初始化：所有烂橘子入队，统计新鲜橘子
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == 2) q.push({i, j});
            else if (grid[i][j] == 1) fresh++;
        }

    if (fresh == 0) return 0;

    int minutes = 0;
    int dirs[] = {-1, 0, 1, 0, -1};

    while (!q.empty()) {
        int sz = q.size();
        bool rotted = false;

        for (int k = 0; k < sz; k++) {
            auto [r, c] = q.front(); q.pop();
            for (int d = 0; d < 4; d++) {
                int nr = r + dirs[d], nc = c + dirs[d+1];
                if (nr >= 0 && nr < m && nc >= 0 && nc < n
                    && grid[nr][nc] == 1) {
                    grid[nr][nc] = 2;
                    q.push({nr, nc});
                    fresh--;
                    rotted = true;
                }
            }
        }
        if (rotted) minutes++;
    }
    return fresh == 0 ? minutes : -1;
}
```

## 6.4 BFS 求最短路径

```cpp
// LC 752: 打开转盘锁
int openLock(vector<string>& deadends, string target) {
    unordered_set<string> dead(deadends.begin(), deadends.end());
    if (dead.count("0000")) return -1;

    queue<string> q;
    unordered_set<string> visited;
    q.push("0000");
    visited.insert("0000");
    int steps = 0;

    while (!q.empty()) {
        int sz = q.size();
        for (int i = 0; i < sz; i++) {
            string curr = q.front(); q.pop();
            if (curr == target) return steps;

            // 每个位可以 +1 或 -1
            for (int j = 0; j < 4; j++) {
                for (int d : {-1, 1}) {
                    string next = curr;
                    next[j] = (next[j] - '0' + d + 10) % 10 + '0';

                    if (!visited.count(next) && !dead.count(next)) {
                        visited.insert(next);
                        q.push(next);
                    }
                }
            }
        }
        steps++;
    }
    return -1;
}
```

## 6.5 BFS 经典问题清单

| 题号 | 题目 | 类型 |
|------|------|------|
| 102 | 二叉树层序遍历 | 树的 BFS |
| 107 | 层序遍历 II | 树的 BFS |
| 199 | 二叉树的右视图 | 树的 BFS |
| 200 | 岛屿数量 | 图的 BFS/DFS |
| 994 | 腐烂的橘子 | 多源 BFS |
| 752 | 打开转盘锁 | 状态空间 BFS |
| 127 | 单词接龙 | 图的 BFS |
| 542 | 01 矩阵 | 多源 BFS |
| 1091 | 二进制矩阵最短路径 | 网格 BFS |

---

# 7. 优先队列 / 堆（priority_queue）

> 优先队列：不是按入队顺序，而是按**优先级**出队。

## 7.1 `std::priority_queue`

```cpp
#include <queue>

// 默认：大顶堆（最大的先出）
priority_queue<int> maxPQ;
maxPQ.push(3);
maxPQ.push(1);
maxPQ.push(5);
maxPQ.push(2);

cout << maxPQ.top();  // 5（最大）
maxPQ.pop();
cout << maxPQ.top();  // 3

// 小顶堆
priority_queue<int, vector<int>, greater<int>> minPQ;
minPQ.push(3);
minPQ.push(1);
minPQ.push(5);
cout << minPQ.top();  // 1（最小）
```

## 7.2 自定义比较

```cpp
// 方法 1: lambda
auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
    return a.second > b.second;  // 按 second 升序 → 小顶堆
};
priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);

// 方法 2: 结构体
struct Compare {
    bool operator()(const pair<int,int>& a, const pair<int,int>& b) {
        return a.second > b.second;
    }
};
priority_queue<pair<int,int>, vector<pair<int,int>>, Compare> pq2;
```

## 7.3 Top K 问题

```cpp
// LC 215: 数组中的第 K 个最大元素
int findKthLargest(vector<int>& nums, int k) {
    // 小顶堆，维护 k 个最大元素
    priority_queue<int, vector<int>, greater<int>> minPQ;

    for (int num : nums) {
        minPQ.push(num);
        if ((int)minPQ.size() > k)
            minPQ.pop();   // 弹掉最小的
    }
    return minPQ.top();     // 堆顶就是第 k 大
}

// LC 347: 前 K 个高频元素
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int num : nums) freq[num]++;

    // 小顶堆：按频率排序
    auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
        return a.second > b.second;
    };
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);

    for (auto& [num, cnt] : freq) {
        pq.push({num, cnt});
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

## 7.4 合并 K 个有序链表

```cpp
// LC 23: 合并 K 个升序链表
// 用最小堆维护每个链表的当前头节点
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);

    for (auto* list : lists)
        if (list) pq.push(list);

    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (!pq.empty()) {
        ListNode* min = pq.top(); pq.pop();
        tail->next = min;
        tail = tail->next;
        if (min->next) pq.push(min->next);
    }
    return dummy.next;
}
```

## 7.5 堆的复杂度

| 操作 | 时间复杂度 |
|------|-----------|
| 插入 (push) | O(log n) |
| 取最值 (top) | O(1) |
| 删除最值 (pop) | O(log n) |
| 建堆 | O(n) |

---

# 8. 单调队列（Monotonic Queue）

> 🔥 解决「滑动窗口最值」的终极武器。

## 8.1 核心思想

**单调队列** = 队列内元素始终保持单调递减（或递增）。

```
维护滑动窗口最大值 → 单调递减队列：
  队头永远是当前窗口的最大值。
  入队时，从队尾弹出所有比新元素小的元素。
```

## 8.2 滑动窗口最大值

```cpp
// LC 239: 滑动窗口最大值 ⭐⭐⭐⭐⭐
// 给定窗口大小 k，返回每个窗口的最大值
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;         // 存下标，维护单调递减
    vector<int> result;

    for (int i = 0; i < (int)nums.size(); i++) {
        // 1. 移除超出窗口的元素
        while (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        // 2. 维护单调递减：弹出所有比当前小的
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();

        dq.push_back(i);

        // 3. 窗口形成后，队头就是最大值
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}

// nums = [1,3,-1,-3,5,3,6,7], k = 3
// 窗口:          最大值
// [1  3 -1]       3
// [3 -1 -3]       3
// [-1 -3  5]      5
// [-3  5  3]      5
// [5  3  6]       6
// [3  6  7]       7
// result = [3, 3, 5, 5, 6, 7]
```

## 8.3 单调队列与单调栈的区别

| 特性 | 单调栈 | 单调队列 |
|------|--------|----------|
| 数据结构 | stack | deque |
| 出入端 | 只有栈顶 | 两端都可以 |
| 典型问题 | 下一个更大元素 | 滑动窗口最值 |
| 核心 | 每个元素的下一个更大/小 | 定长窗口内的最值 |
| 窗口是否固定 | 不固定 | 固定大小 |

---

# 9. 循环队列

```cpp
// LC 622: 设计循环队列
class MyCircularQueue {
    vector<int> data;
    int head = 0, count = 0;

public:
    MyCircularQueue(int k) : data(k) {}

    bool enQueue(int val) {
        if (isFull()) return false;
        int tail = (head + count) % data.size();
        data[tail] = val;
        count++;
        return true;
    }

    bool deQueue() {
        if (isEmpty()) return false;
        head = (head + 1) % data.size();
        count--;
        return true;
    }

    int Front() {
        return isEmpty() ? -1 : data[head];
    }

    int Rear() {
        if (isEmpty()) return -1;
        int tail = (head + count - 1) % data.size();
        return data[tail];
    }

    bool isEmpty() { return count == 0; }
    bool isFull() { return count == (int)data.size(); }
};

// 循环队列的核心：用取模 % 实现环形索引
// tail = (head + count) % capacity
```

---

# 10. 队列在系统设计中的应用

## 10.1 消息队列模型

```
生产者 → [消息队列] → 消费者

场景：
- 异步处理：用户注册 → 发送邮件（不阻塞主流程）
- 削峰填谷：秒杀请求 → 队列缓冲 → 慢慢处理
- 解耦：订单服务 → 队列 → 库存/物流/通知服务
```

## 10.2 任务调度

```cpp
// 简易任务调度器
// LC 621: 任务调度器
int leastInterval(vector<char>& tasks, int n) {
    vector<int> freq(26, 0);
    for (char t : tasks) freq[t - 'A']++;

    int maxFreq = *max_element(freq.begin(), freq.end());
    int maxCount = count(freq.begin(), freq.end(), maxFreq);

    // 公式: (maxFreq - 1) * (n + 1) + maxCount
    // 至少需要的时间 = 框架时间 vs 任务总数，取较大值
    return max((int)tasks.size(), (maxFreq - 1) * (n + 1) + maxCount);
}
```

---

# 11. 经典 LeetCode 队列题精讲

## 必做题清单

| 题号 | 题目 | 难度 | 核心技巧 | 频率 |
|------|------|------|----------|------|
| 232 | 用栈实现队列 | Easy | 两栈倒腾 | ⭐⭐⭐⭐⭐ |
| 225 | 用队列实现栈 | Easy | 单队列翻转 | ⭐⭐⭐⭐ |
| 622 | 设计循环队列 | Medium | 取模环形 | ⭐⭐⭐⭐ |
| 102 | 二叉树层序遍历 | Medium | BFS | ⭐⭐⭐⭐⭐ |
| 199 | 二叉树右视图 | Medium | BFS/DFS | ⭐⭐⭐⭐ |
| 200 | 岛屿数量 | Medium | BFS/DFS | ⭐⭐⭐⭐⭐ |
| 994 | 腐烂的橘子 | Medium | 多源 BFS | ⭐⭐⭐⭐⭐ |
| 752 | 打开转盘锁 | Medium | 状态 BFS | ⭐⭐⭐⭐ |
| 127 | 单词接龙 | Hard | 图 BFS | ⭐⭐⭐⭐ |
| 239 | 滑动窗口最大值 | Hard | 单调队列 | ⭐⭐⭐⭐⭐ |
| 215 | 第K个最大元素 | Medium | 堆 | ⭐⭐⭐⭐⭐ |
| 347 | 前K个高频元素 | Medium | 堆+哈希 | ⭐⭐⭐⭐⭐ |
| 23 | 合并K个升序链表 | Hard | 最小堆 | ⭐⭐⭐⭐⭐ |
| 621 | 任务调度器 | Medium | 贪心/队列 | ⭐⭐⭐⭐ |
| 295 | 数据流中位数 | Hard | 双堆 | ⭐⭐⭐⭐⭐ |

## 解题思路速查

```
看到队列/BFS 相关问题 → 先分类：
├─ 层序遍历/最短路径？ → BFS + queue
├─ 多源同时扩散？ → 多源 BFS（所有源先入队）
├─ 滑动窗口最值？ → 单调队列 (deque)
├─ 前 K 大/小？ → 优先队列 (priority_queue)
├─ 合并 K 路？ → 最小堆
├─ 数据流中位数？ → 双堆（大顶+小顶）
├─ FIFO 模拟？ → queue
└─ 循环缓冲区？ → 环形队列（取模）
```

---

# 12. 常见陷阱与最佳实践

## 12.1 经典陷阱

### 陷阱 1：空队列操作

```cpp
// ❌ 空队列调用 front()/pop()
queue<int> q;
q.front();  // 未定义行为！

// ✅ 先检查
if (!q.empty()) {
    int val = q.front();
    q.pop();
}
```

### 陷阱 2：BFS 忘记标记 visited

```cpp
// ❌ 不标记 → 死循环
q.push(start);
while (!q.empty()) {
    auto curr = q.front(); q.pop();
    for (auto next : neighbors) {
        q.push(next);  // 可能重复入队！
    }
}

// ✅ 入队时就标记
q.push(start);
visited.insert(start);
while (!q.empty()) {
    auto curr = q.front(); q.pop();
    for (auto next : neighbors) {
        if (!visited.count(next)) {
            visited.insert(next);    // 入队时标记
            q.push(next);
        }
    }
}
```

### 陷阱 3：BFS 层级计数错误

```cpp
// ❌ 没有按层处理
while (!q.empty()) {
    auto node = q.front(); q.pop();
    // 无法区分哪些节点是同一层
}

// ✅ 按层遍历
while (!q.empty()) {
    int levelSize = q.size();     // 先记录当前层大小
    for (int i = 0; i < levelSize; i++) {
        auto node = q.front(); q.pop();
        // 处理同一层的节点
    }
    level++;
}
```

### 陷阱 4：优先队列方向搞反

```cpp
// 大顶堆 vs 小顶堆 的比较函数方向：
// 大顶堆（默认）: a < b → less → 大的在前
// 小顶堆: a > b → greater → 小的在前

// ⚠️ 自定义 cmp 的语义和 sort 的 cmp 相反！
// sort: cmp 返回 true → a 排在 b 前面
// priority_queue: cmp 返回 true → a 的优先级低于 b
```

### 陷阱 5：单调队列忘记移除过期元素

```cpp
// ❌ 只维护单调性，忘记检查窗口范围
while (!dq.empty() && nums[dq.back()] <= nums[i])
    dq.pop_back();
dq.push_back(i);
// 忘记检查队头是否超出窗口！

// ✅ 先检查过期，再维护单调
while (!dq.empty() && dq.front() <= i - k)
    dq.pop_front();     // 移除过期元素
while (!dq.empty() && nums[dq.back()] <= nums[i])
    dq.pop_back();      // 维护单调递减
dq.push_back(i);
```

## 12.2 最佳实践

```
✅ 操作前检查 empty()
✅ BFS 入队时就标记 visited（不是出队时）
✅ BFS 按层遍历时先记录 levelSize
✅ 优先队列注意比较函数方向
✅ 单调队列存下标（方便判断过期）
✅ 用 deque 实现单调队列（两端操作）
```

## 12.3 队列技巧速查卡

```
┌──────────────────────────────────────────────────┐
│               队列技巧速查                        │
├──────────────────────────────────────────────────┤
│  普通队列       → FIFO，BFS 的基础                │
│  双端队列       → deque，两端进出                  │
│  优先队列       → 堆，Top K / 合并 K 路            │
│  单调队列       → deque，滑动窗口最值              │
│  循环队列       → 取模实现环形                     │
│  BFS 模板       → queue + visited + 按层遍历       │
│  多源 BFS       → 所有源点同时入队                 │
│  双堆           → 大顶堆 + 小顶堆 = 动态中位数     │
└──────────────────────────────────────────────────┘
```

---

*📝 下一步：完成队列示例代码 `knowlege_details_3_queue.cpp`*

*💪 BFS 是队列最核心的应用，单调队列是进阶必备！*
