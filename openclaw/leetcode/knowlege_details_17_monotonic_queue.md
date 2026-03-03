# 专题十七（下）：单调队列 Monotonic Deque — 滑动窗口极值与优化DP

> 作者：大胖超 😜
> 
> 单调队列是单调栈在**滑动窗口**场景下的扩展。它用双端队列（deque）维护窗口内的极值，使得每次滑动都能 O(1) 获取最大/最小值。LC 239/862/1438 等高频题的最优解都依赖单调队列。

---

## 目录

1. [单调队列基本概念](#1-单调队列基本概念)
2. [单调队列核心模板](#2-单调队列核心模板)
3. [单调队列 vs 单调栈](#3-单调队列-vs-单调栈)
4. [单调队列优化 DP](#4-单调队列优化-dp)
5. [LeetCode 经典题解](#5-leetcode-经典题解)
6. [进阶应用](#6-进阶应用)
7. [题目总结与易错点](#7-题目总结与易错点)

---

## 1. 单调队列基本概念

### 1.1 什么是单调队列

单调队列是一个双端队列（deque），队列中的元素保持单调递增或递减。与单调栈的区别在于，单调队列**可以从队首弹出过期元素**（超出窗口范围）。

```
单调递减队列（维护最大值）:

窗口 [73, 74, 75], k=3:
  dq = [75]  ← 只保留最大的

窗口移动 → [74, 75, 71]:
  75入时已清掉73,74
  71入: dq = [75, 71]  ← 75最大, 71可能未来有用

窗口移动 → [75, 71, 69]:
  69入: dq = [75, 71, 69]

窗口移动 → [71, 69, 72]:
  75过期弹出, 72>69弹69, 72>71弹71
  dq = [72]

队首始终是当前窗口的最大值!
```

### 1.2 核心思想

```
单调队列三个操作:
  1. 队尾入队: 新元素入队前, 弹出所有不如它的
  2. 队首出队: 检查队首是否过期(超出窗口)
  3. 取极值:   队首就是当前窗口的最大/最小值

为什么弹出"不如它的"?
  如果队尾元素 ≤ 当前元素, 且队尾更早过期,
  那它永远不可能成为任何窗口的最大值 → 没用了!
```

### 1.3 复杂度

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n) — 每个元素最多入队出队各一次 |
| 空间 | O(k) — 队列最多 k 个元素 |

### 1.4 存下标还是存值

```
必须存下标! 原因:
  1. 需要判断队首是否过期: dq.front() <= i - k
  2. 通过下标访问值: nums[dq.front()]
  3. 与单调栈保持一致, 减少记忆负担
```

---

## 2. 单调队列核心模板

### 2.1 滑动窗口最大值（LC 239，最经典）

```cpp
// 单调递减队列: 队首是最大值
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;  // 存下标, 对应值从大到小
    vector<int> result;
    
    for (int i = 0; i < (int)nums.size(); i++) {
        // 1. 队首过期检查
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();
        
        // 2. 队尾维护单调性: 弹出所有 ≤ 当前的
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();
        
        // 3. 入队
        dq.push_back(i);
        
        // 4. 窗口形成后记录答案
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}
```

**图解过程**（k=3）：

```
nums = [1, 3, -1, -3, 5, 3, 6, 7]

i=0: dq=[0]              值:[1]
i=1: 3>1弹0, dq=[1]      值:[3]
i=2: -1<3, dq=[1,2]      值:[3,-1]       窗口[1,3,-1] → max=3
i=3: -3<-1, dq=[1,2,3]   值:[3,-1,-3]    窗口[3,-1,-3] → max=3
i=4: 1过期弹front=1
     5>-3弹3, 5>-1弹2
     dq=[4]              值:[5]           窗口[-1,-3,5] → max=5
i=5: 3<5, dq=[4,5]       值:[5,3]        窗口[-3,5,3] → max=5
i=6: 6>3弹5, 6>5弹4
     dq=[6]              值:[6]           窗口[5,3,6] → max=6
i=7: 7>6弹6
     dq=[7]              值:[7]           窗口[3,6,7] → max=7

result = [3, 3, 5, 5, 6, 7]  ✓
```

### 2.2 滑动窗口最小值

```cpp
// 单调递增队列: 队首是最小值
vector<int> minSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;
    vector<int> result;
    
    for (int i = 0; i < (int)nums.size(); i++) {
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();
        
        // 弹出所有 ≥ 当前的
        while (!dq.empty() && nums[dq.back()] >= nums[i])
            dq.pop_back();
        
        dq.push_back(i);
        
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}
```

### 2.3 模板总结

```
维护最大值 → 单调递减队列 (弹出 ≤ 当前的)
维护最小值 → 单调递增队列 (弹出 ≥ 当前的)

操作顺序:
  ① 弹过期  → dq.front() <= i - k
  ② 弹弱者  → nums[dq.back()] ≤/≥ nums[i]
  ③ 入队    → dq.push_back(i)
  ④ 记录    → 当 i >= k-1 时取 nums[dq.front()]
```

---

## 3. 单调队列 vs 单调栈

### 3.1 对比表

| 特性 | 单调栈 | 单调队列 |
|------|--------|----------|
| 底层结构 | stack (后进先出) | deque (双端操作) |
| 弹出方向 | 只从栈顶弹 | 栈顶弹(维护单调) + 队首弹(过期) |
| 核心问题 | 最近的更大/更小 | 窗口内的最大/最小 |
| 窗口约束 | 无窗口约束 | 有窗口大小限制 |
| 典型题 | LC 739/84/42 | LC 239/862/1438 |

### 3.2 什么时候用哪个

```
看到关键词:
  "下一个更大/更小" → 单调栈
  "柱状图/直方图"   → 单调栈
  "接雨水"          → 单调栈 (或双指针)

  "滑动窗口最大/最小" → 单调队列
  "固定窗口极值"      → 单调队列
  "DP转移窗口优化"    → 单调队列
```

---

## 4. 单调队列优化 DP

### 4.1 基本思想

```
普通 DP:
  dp[i] = max{ dp[j] + cost(j, i) }  for j in [i-k, i-1]

如果 cost 可以分离成与 i 和 j 各自相关的部分:
  dp[i] = max{ dp[j] + f(j) } + g(i)

那么 max{ dp[j] + f(j) } 可以用单调队列在 O(1) 维护!

总复杂度从 O(nk) 优化到 O(n)
```

### 4.2 跳跃游戏 VI（LC 1696）

```
nums = [1, -1, -2, 4, -7, 3], k = 2
每次最多跳 k 步, 收集沿途的值, 求到达末尾的最大值。

dp[i] = nums[i] + max(dp[i-k], ..., dp[i-1])
      = nums[i] + 窗口 k 内的 dp 最大值
→ 单调队列维护!
```

```cpp
int maxResult(vector<int>& nums, int k) {
    int n = nums.size();
    vector<int> dp(n);
    deque<int> dq;  // 维护 dp 值的递减队列
    
    dp[0] = nums[0];
    dq.push_back(0);
    
    for (int i = 1; i < n; i++) {
        // 弹过期
        while (!dq.empty() && dq.front() < i - k)
            dq.pop_front();
        
        dp[i] = nums[i] + dp[dq.front()];  // 窗口内dp最大值
        
        // 维护递减
        while (!dq.empty() && dp[dq.back()] <= dp[i])
            dq.pop_back();
        dq.push_back(i);
    }
    return dp[n-1];
}
```

### 4.3 满足不等式的最大值（LC 1499）

```
给 points[i] = (xi, yi), 找 i<j 使得:
  yi + yj + |xi - xj| 最大

因为 i<j 且 xi 递增(题目约束), 所以:
  |xi - xj| = xj - xi

目标 = (yj + xj) + max{yi - xi}  对 j 前面的 i
→ 用单调队列维护 yi - xi 的最大值, 窗口约束 xj - xi ≤ k
```

---

## 5. LeetCode 经典题解

### 5.1 LC 239: 滑动窗口最大值

```
nums = [1,3,-1,-3,5,3,6,7], k = 3

滑动窗口:
  [1,3,-1] → 3
  [3,-1,-3] → 3
  [-1,-3,5] → 5
  [-3,5,3] → 5
  [5,3,6] → 6
  [3,6,7] → 7

输出: [3,3,5,5,6,7]
```

见上方模板 2.1。

### 5.2 LC 862: 和至少为K的最短子数组

```
给定整数数组（可含负数）, 求和 ≥ k 的最短子数组长度。

思路:
  前缀和 pre[i], 求 pre[j] - pre[i] >= k (j > i)
  即 pre[i] <= pre[j] - k

  维护 pre 值的单调递增队列:
  对每个 j, 检查队首 pre[dq.front()] <= pre[j] - k
  如果满足, 更新答案并弹队首（更短的已记录）

  为什么可以弹? 后面的 j' > j, 用 dq.front() 只会更长
```

```cpp
int shortestSubarray(vector<int>& nums, int k) {
    int n = nums.size();
    vector<long long> pre(n + 1, 0);
    for (int i = 0; i < n; i++)
        pre[i+1] = pre[i] + nums[i];
    
    deque<int> dq;
    int minLen = n + 1;
    
    for (int j = 0; j <= n; j++) {
        // 队首满足条件 → 更新答案并弹出
        while (!dq.empty() && pre[j] - pre[dq.front()] >= k) {
            minLen = min(minLen, j - dq.front());
            dq.pop_front();
        }
        // 维护递增: 弹出 ≥ pre[j] 的队尾
        while (!dq.empty() && pre[dq.back()] >= pre[j])
            dq.pop_back();
        dq.push_back(j);
    }
    return minLen <= n ? minLen : -1;
}
```

> **注意**：本题数组可含负数，所以不能用普通滑动窗口（窗口不满足单调性），必须用前缀和 + 单调队列。

### 5.3 LC 1438: 绝对差不超过限制的最长连续子数组

```
找最长子数组使得 max - min ≤ limit。

思路: 双指针(滑动窗口) + 两个单调队列
  一个递减队列维护最大值
  一个递增队列维护最小值
  当 max - min > limit 时收缩左边界
```

```cpp
int longestSubarray(vector<int>& nums, int limit) {
    deque<int> maxDq, minDq;  // 递减→最大, 递增→最小
    int left = 0, result = 0;
    
    for (int right = 0; right < (int)nums.size(); right++) {
        // 维护最大值递减队列
        while (!maxDq.empty() && nums[maxDq.back()] <= nums[right])
            maxDq.pop_back();
        maxDq.push_back(right);
        
        // 维护最小值递增队列
        while (!minDq.empty() && nums[minDq.back()] >= nums[right])
            minDq.pop_back();
        minDq.push_back(right);
        
        // 收缩左边界
        while (nums[maxDq.front()] - nums[minDq.front()] > limit) {
            left++;
            if (maxDq.front() < left) maxDq.pop_front();
            if (minDq.front() < left) minDq.pop_front();
        }
        
        result = max(result, right - left + 1);
    }
    return result;
}
```

### 5.4 LC 1696: 跳跃游戏 VI

见上方 4.2 节。

### 5.5 LC 346/480: 数据流中的移动平均值 / 滑动窗口中位数

```
LC 346 (简单): 固定窗口中的平均值 → 队列即可
LC 480 (困难): 滑动窗口中位数 → multiset 或两个堆

单调队列在此类"窗口统计"题中起辅助作用。
```

---

## 6. 进阶应用

### 6.1 二维滑动窗口极值

```
在 m×n 矩阵中, 求所有 p×q 子矩阵的最大值。

思路: 先对每行用单调队列求宽度为 q 的窗口最大值,
     再对结果矩阵每列用单调队列求高度为 p 的窗口最大值。

两次一维扫描 → O(m×n)
```

### 6.2 最小/最大栈/队列

```
最小栈 (LC 155): 辅助栈存当前最小值
最大队列: 用单调递减队列实现 O(1) getMax

应用: 实时系统中维护区间极值
```

### 6.3 单调队列 + 多重背包

```
多重背包问题中, 物品数量有限, DP 状态转移涉及窗口:
  dp[j] = max{ dp[j - t*w] + t*v }  for t in [0, cnt]

按余数分组后, 每组内是一个滑动窗口最大值问题
→ 单调队列优化, O(n*V) → O(n*V)
```

---

## 7. 题目总结与易错点

### 7.1 题目分类

#### 固定窗口最大/最小
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 239 | 滑动窗口最大值 | 困难 | 经典单调队列 |
| 1438 | 绝对差不超限制的最长子数组 | 中等 | 双单调队列+双指针 |
| 346 | 数据流移动平均值 | 简单 | 基础队列 |

#### 变长窗口/前缀和
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 862 | 和至少为K的最短子数组 | 困难 | 前缀和+单调递增队列 |
| 1499 | 满足不等式最大值 | 困难 | 单调队列+数学变换 |

#### 单调队列优化 DP
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 1696 | 跳跃游戏 VI | 中等 | 窗口内dp最大值 |
| 1425 | 带限制的子序列和 | 困难 | 同1696思路 |
| 1687 | 从仓库到码头运砖块 | 困难 | 复杂DP+单调队列 |

### 7.2 高频易错点

```
❌ 1. 弹过期和弹弱者的顺序搞反
   应该先弹过期(pop_front), 再弹弱者(pop_back), 再入队
   ✅ 顺序: 弹过期 → 弹弱者 → 入队 → 记录答案

❌ 2. 过期条件写错
   dq.front() <= i - k  (≤ 不是 <)
   如果窗口 [i-k+1, i], 则 front 的下标 ≤ i-k 时过期
   ✅ 画图确认边界

❌ 3. 用 < 还是 <= 弹队尾
   维护最大值: nums[dq.back()] <= nums[i] 弹出 (等于也弹)
   维护最小值: nums[dq.back()] >= nums[i] 弹出 (等于也弹)
   如果不弹等于的, 可能导致过期时取到错误值
   ✅ 一般等于也弹, 保留最新的

❌ 4. 窗口还没形成就记录答案
   当 i < k-1 时窗口还没满, 不应记录结果
   ✅ if (i >= k - 1) result.push_back(...)

❌ 5. LC 862 忘记前缀和要长度 n+1
   pre[0] = 0, pre[i] = sum(nums[0..i-1])
   长度应该是 n+1, 遍历到 n
   ✅ vector<long long> pre(n+1, 0)

❌ 6. LC 1438 收缩时忘记检查队列是否需要弹
   left++ 后要检查 maxDq.front() 和 minDq.front()
   被 left 跳过的元素需要从队首弹出
   ✅ if (dq.front() < left) dq.pop_front()

❌ 7. 单调队列和优先队列混淆
   priority_queue: 可以取最大/最小, 但不能高效删除指定元素
   单调队列: 只维护窗口内极值, O(1)过期弹出
   ✅ 窗口极值问题首选单调队列, 不用堆
```

### 7.3 面试高频问答

> **Q: 单调队列和优先队列（堆）都能解 LC 239，有何区别？**  
> A: 单调队列 O(n)，堆 O(n log k)。单调队列还能利用元素过期**顺序性**，O(1) 弹出队首。堆的删除需要懒删除标记。

> **Q: LC 862 为什么能弹队首的"满足条件"的元素？**  
> A: 因为后面的 j 更大，用同一个 i 只会得到更长的子数组。已经记录了当前最短答案，所以该 i 不再有用。

> **Q: 单调队列能不能求窗口内的中位数？**  
> A: 不能直接求。中位数需要维护有序性，通常用两个堆（大顶堆+小顶堆）或 multiset。单调队列只适合极值问题。

> **Q: 什么时候DP需要单调队列优化？**  
> A: 当 DP 转移 dp[i] = max/min{ dp[j] + f(j) } 中 j 的范围是一个**滑动窗口**（j ∈ [i-k, i-1]），且 f(j) 可以与 i 无关时，单调队列可以将每步转移从 O(k) 优化到 O(1)。

---

> 📌 **配套代码**：[knowlege_details_17_monotonic_queue.cpp](knowlege_details_17_monotonic_queue.cpp) 包含上述所有算法的可运行演示。
