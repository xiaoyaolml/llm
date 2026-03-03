# 专题十九（下）：差分数组 Difference Array — 详解文档

> 作者：大胖超 😜
>
> 差分是前缀和的**逆运算**。前缀和擅长"快速查询区间和"，差分擅长"快速对区间整体加减"。两者互为一体两面，合在一起覆盖了绝大多数区间操作题型。

---

## 第一章 核心概念

### 1.1 什么是差分数组？

给定数组 `nums[0..n-1]`，差分数组 `diff[0..n-1]` 定义为：

$$diff[0] = nums[0], \quad diff[i] = nums[i] - nums[i-1] \quad (i \geq 1)$$

**性质**：对差分数组做前缀和，就能还原出原数组。

$$nums[i] = \sum_{j=0}^{i} diff[j]$$

### 1.2 差分的核心操作

要对原数组 `nums` 的 **区间 [l, r] 同时加上 val**，只需要：

```
diff[l]   += val
diff[r+1] -= val   (如果 r+1 < n)
```

然后对 `diff` 做一遍前缀和就得到修改后的 `nums`。

### 1.3 图示理解

```
原数组: [0, 0, 0, 0, 0, 0]  (n=6)

操作1: 区间 [1,3] +2
diff:  [0, +2, 0, 0, -2, 0]
还原:  [0,  2, 2, 2,  0, 0]  ✓

操作2: 区间 [2,5] +3
diff:  [0, +2, +3, 0, -2, 0]   (注意 r+1=6 越界，不需要减)
还原:  [0,  2,  5, 5,  3, 3]  ✓
```

### 1.4 为什么差分有效？

在 `diff[l]` 加 val 后做前缀和 → 从位置 l 开始，所有后续元素都被加了 val。  
在 `diff[r+1]` 减 val → 从位置 r+1 开始，抵消了之前的加法。  
两者配合，**恰好只影响 [l, r]**。

### 1.5 复杂度对比

| 操作 | 暴力 | 差分 |
|------|------|------|
| 对区间 [l,r] +val | O(r-l+1) | **O(1)** |
| 执行 m 次区间加 | O(m·n) | **O(m+n)** |
| 查询单点值 | O(1) | O(n) 还原 |

> **适用场景**：多次区间修改 + 最后一次性查询。

---

## 第二章 一维差分模板与经典题

### 2.1 标准模板

```cpp
// 构建差分数组（初始nums全0时简化为直接在diff上操作）
void rangeAdd(vector<int>& diff, int l, int r, int val) {
    diff[l] += val;
    if (r + 1 < (int)diff.size())
        diff[r + 1] -= val;
}

// 还原原数组
vector<int> restore(vector<int>& diff) {
    vector<int> nums(diff.size());
    nums[0] = diff[0];
    for (int i = 1; i < (int)diff.size(); i++)
        nums[i] = nums[i - 1] + diff[i];
    return nums;
}
```

### 2.2 LC 1094 — 拼车

**题意**：trips[i] = [numPassengers, from, to]，capacity 为容量。问是否能完成所有行程。

**关键**：乘客在 `from` 上车，在 `to` **下车**（不是 `to` 仍在车上），所以区间是 `[from, to-1]` 或者说 `diff[from] += num, diff[to] -= num`。

```cpp
bool carPooling(vector<vector<int>>& trips, int capacity) {
    vector<int> diff(1001, 0);
    for (auto& t : trips) {
        diff[t[1]] += t[0];     // 上车
        diff[t[2]] -= t[0];     // 下车 (到站就下)
    }
    int passengers = 0;
    for (int i = 0; i < 1001; i++) {
        passengers += diff[i];
        if (passengers > capacity) return false;
    }
    return true;
}
```

### 2.3 LC 1109 — 航班预订统计

**题意**：`bookings[i] = [first, last, seats]`，第 first 到 last 号航班各预订 seats 个座位。返回每架航班的预订数。

```cpp
vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n) {
    vector<int> diff(n + 1, 0);  // 多一位方便处理
    for (auto& b : bookings) {
        diff[b[0] - 1] += b[2];   // 1-indexed → 0-indexed
        if (b[1] < n)
            diff[b[1]] -= b[2];
    }
    // 前缀和
    vector<int> result(n);
    result[0] = diff[0];
    for (int i = 1; i < n; i++)
        result[i] = result[i - 1] + diff[i];
    return result;
}
```

### 2.4 LC 370 — 区间加法（Premium）

最纯粹的差分模板题。给 n 个 0，执行若干 `[start, end, val]` 操作，返回最终数组。

### 2.5 LC 253 — 会议室 II（差分思维）

**题意**：给出若干会议的起止时间，求至少需要多少间会议室。

**等价于**：差分计算每个时间点同时进行的会议数，取最大值。

```cpp
int minMeetingRooms(vector<vector<int>>& intervals) {
    map<int, int> diff;
    for (auto& iv : intervals) {
        diff[iv[0]]++;     // 会议开始
        diff[iv[1]]--;     // 会议结束
    }
    int maxRooms = 0, cur = 0;
    for (auto& [time, delta] : diff) {
        cur += delta;
        maxRooms = max(maxRooms, cur);
    }
    return maxRooms;
}
```

> 用 `map` 而非数组：时间点可能很大但数量不多，用离散化思路。

---

## 第三章 二维差分

### 3.1 核心思想

要对二维矩阵的**子矩阵 (r1,c1)~(r2,c2) 同时加 val**，类比一维差分，用容斥：

```cpp
diff[r1][c1]     += val;
diff[r1][c2+1]   -= val;
diff[r2+1][c1]   -= val;
diff[r2+1][c2+1] += val;
```

还原时做二维前缀和。

### 3.2 图示

```
加 val 到 (r1,c1)~(r2,c2):

            c1        c2  c2+1
       ┌────┬─────────┬────┐
  r1   │+val│         │-val│
       ├────┼─────────┼────┤
       │    │ 目标区域 │    │
       │    │  +val    │    │
  r2   ├────┼─────────┼────┤
  r2+1 │-val│         │+val│
       └────┴─────────┴────┘

做二维前缀和后，只有目标区域被加了 val。
```

### 3.3 LC 2536 — 子矩阵元素加 1（二维差分直接应用）

```cpp
vector<vector<int>> rangeAddQueries(int n, vector<vector<int>>& queries) {
    vector<vector<int>> diff(n + 1, vector<int>(n + 1, 0));
    for (auto& q : queries) {
        int r1 = q[0], c1 = q[1], r2 = q[2], c2 = q[3];
        diff[r1][c1]++;
        if (c2 + 1 <= n) diff[r1][c2 + 1]--;
        if (r2 + 1 <= n) diff[r2 + 1][c1]--;
        if (r2 + 1 <= n && c2 + 1 <= n) diff[r2 + 1][c2 + 1]++;
    }
    // 二维前缀和还原
    vector<vector<int>> result(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            result[i][j] = diff[i][j];
            if (i > 0) result[i][j] += result[i-1][j];
            if (j > 0) result[i][j] += result[i][j-1];
            if (i > 0 && j > 0) result[i][j] -= result[i-1][j-1];
        }
    return result;
}
```

---

## 第四章 差分的进阶应用

### 4.1 等差数列差分

有时候要对区间 [l, r] 加一个**等差数列** `a, a+d, a+2d, ...` 而非常数。

**技巧**：用**二阶差分**（差分的差分）：

- 一阶差分处理常数增量。
- 二阶差分处理线性增量。

```
操作: nums[l..r] 分别加 [a, a+d, a+2d, ..., a+(r-l)*d]

diff1[l]   += a
diff1[l+1] += d - a    (实际: 第二阶差分 diff2[l] += d)
diff1[r+1] -= a + (r-l)*d

用两次前缀和还原。
```

### 4.2 差分 + 离散化

当值域很大（如时间戳到 10^9）但操作次数少（如 10^4），先**离散化**再差分。

```cpp
// 收集所有端点 → 排序去重 → 映射到紧凑下标 → 差分
```

### 4.3 差分在树上的应用

**树上差分**：在树的路径 u→v 上加 val。

- 点差分：`diff[u] += val, diff[v] += val, diff[lca] -= val, diff[parent[lca]] -= val`
- 边差分：类似，作用在边权上

结合 LCA（最近公共祖先）使用。

### 4.4 前缀和 → 差分 → 前缀和 的循环

```
原数组 nums
   ↓ 差分
差分数组 diff (diff[i] = nums[i] - nums[i-1])
   ↓ 前缀和
还原 nums

所以：差分 是 前缀和的逆运算
      前缀和 是 差分的逆运算
```

---

## 第五章 LeetCode 题目清单

### 5.1 基础差分

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 370 | 区间加法（Premium） | 纯差分模板 | ⭐⭐ |
| 1094 | 拼车 | 差分+容量判断 | ⭐⭐ |
| 1109 | 航班预订统计 | 差分+前缀和还原 | ⭐⭐ |
| 1854 | 人口最多的年份 | 差分+扫描线 | ⭐ |

### 5.2 差分思维

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 253 | 会议室 II | 差分+扫描最大同时数 | ⭐⭐ |
| 732 | 我的日程安排表 III | 差分+map | ⭐⭐⭐ |
| 1674 | 使数组互补的最少操作 | 差分思维+边界枚举 | ⭐⭐⭐ |

### 5.3 二维差分

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 2536 | 子矩阵元素加1 | 二维差分模板 | ⭐⭐ |
| 850 | 矩形面积 II | 离散化+扫描线/差分 | ⭐⭐⭐⭐ |

### 5.4 综合

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 995 | K连续位的最小翻转次数 | 差分/贪心 | ⭐⭐⭐ |
| 1589 | 所有排列中的最大和 | 差分计数+贪心 | ⭐⭐⭐ |

---

## 第六章 前缀和与差分的关系总结

```
            差分 (逆运算)
  nums ────────────────→ diff
    ↑                      │
    │    前缀和 (正运算)     │
    └──────────────────────┘
```

| 对比 | 前缀和 | 差分 |
|------|--------|------|
| 擅长 | 快速查询区间和 | 快速修改区间值 |
| 预处理 | O(n) 建前缀和 | O(n) 建差分数组 |
| 查询 | O(1) 区间和 | O(n) 需还原 |
| 修改 | O(n) 需重建 | O(1) 区间+val |
| 适用 | 多次查，不修改 | 多次改，最后查 |
| 二维版 | 容斥 4 项 | 容斥 4 项 |
| 互为逆 | 前缀和(差分) = 原数组 | 差分(前缀和) = 原数组 |

---

## 第七章 总结与易错点

### 7.1 知识脑图

```
差分
├── 一维差分
│   ├── diff[l] += val, diff[r+1] -= val
│   ├── 前缀和还原
│   ├── LC 370/1094/1109/1854
│   └── 扫描线思维 (LC 253/732)
├── 二维差分
│   ├── 容斥4项: +1/-1/-1/+1
│   └── LC 2536
├── 进阶
│   ├── 等差数列差分 (二阶差分)
│   ├── 离散化 + 差分
│   └── 树上差分 (LCA)
└── 与前缀和的关系
    └── 互为逆运算
```

### 7.2 七大易错点

| # | 易错点 | 正确做法 |
|---|--------|----------|
| 1 | 忘记 `diff[r+1] -= val` | 必须减，否则 r 以后全被加了 |
| 2 | `r+1` 越界 | 检查 `if (r+1 < n)` |
| 3 | 差分忘做前缀和还原 | 修改完要做一遍前缀和 |
| 4 | 1-indexed 和 0-indexed 混淆 | 统一下标风格 |
| 5 | 二维差分容斥写反 | 画图：左上+、右上-、左下-、右下+ |
| 6 | 差分适用场景搞错 | 差分是"批量修改+最后查"，不是"频繁查" |
| 7 | int 溢出 | 差分值可能很大，用 long long |

### 7.3 面试 FAQ

**Q: 什么时候用差分而不是线段树？**  
A: 如果是**离线**操作（先做完所有修改再查询），差分 O(n+m) 比线段树 O(m·logn) 更简单高效。如果需要**修改和查询交替进行**，必须用线段树或树状数组。

**Q: 差分适合哪些题型？**  
A: 看到"对区间 [l,r] 整体加/减"、"统计某时间点的最大重叠数"、"公交车/拼车/航班预订"这类关键词就该想到差分。

**Q: 二维差分怎么记公式？**  
A: 画图！在左上角 +val，然后在右方和下方各 -val（防扩散），右下角 +val（防过度抵消）。这就是容斥原理。

---

> 📂 **配套代码文件**：`knowlege_details_19_difference_array.cpp`  
> 📂 **前缀和专题**：`knowlege_details_19_prefix_sum.md` + `.cpp`
