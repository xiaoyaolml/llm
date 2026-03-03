# 专题十九（上）：前缀和 Prefix Sum — 详解文档

> 作者：大胖超 😜
>
> 前缀和是最基础也最高频的算法技巧之一——**O(1) 查询任意区间和**。它的思想简单到可以一句话说完，但应用场景极其丰富：一维、二维、哈希表组合、与二分结合……掌握前缀和几乎是 LeetCode 刷题的必备功。

---

## 第一章 核心概念

### 1.1 什么是前缀和？

给定数组 `nums[0..n-1]`，构造前缀和数组 `pre[0..n]`：

$$pre[0] = 0, \quad pre[i] = \sum_{j=0}^{i-1} nums[j] = pre[i-1] + nums[i-1]$$

于是，**任意区间 [l, r] 的元素和**可以 O(1) 计算：

$$\text{sum}(l, r) = pre[r+1] - pre[l]$$

### 1.2 直觉图示

```
下标:       0    1    2    3    4
nums:     [ 1,   3,   5,   7,   9 ]

pre:  [0,   1,   4,   9,  16,  25 ]
       ↑                        ↑
     pre[0]                   pre[5]

区间 [1, 3] 的和 = pre[4] - pre[1] = 16 - 1 = 15
即 nums[1]+nums[2]+nums[3] = 3 + 5 + 7 = 15 ✓
```

### 1.3 为什么 pre 长度是 n+1？

多一个 `pre[0] = 0`，让公式统一：
- `sum(0, r) = pre[r+1] - pre[0] = pre[r+1]`，不需要特判 `l = 0`

### 1.4 模板代码

```cpp
// 构建前缀和
vector<long long> buildPrefix(vector<int>& nums) {
    int n = nums.size();
    vector<long long> pre(n + 1, 0);
    for (int i = 0; i < n; i++)
        pre[i + 1] = pre[i] + nums[i];
    return pre;
}

// 查询区间 [l, r] 的和
long long rangeSum(vector<long long>& pre, int l, int r) {
    return pre[r + 1] - pre[l];
}
```

> **注意**：使用 `long long` 防止前缀和溢出！

### 1.5 复杂度

| 操作 | 时间 | 空间 |
|------|------|------|
| 构建 | O(n) | O(n) |
| 查询 | O(1) | — |

---

## 第二章 一维前缀和经典应用

### 2.1 LC 303 — 区域和检索（不可变）

最基础的前缀和模板题：

```cpp
class NumArray {
    vector<int> pre;
public:
    NumArray(vector<int>& nums) : pre(nums.size() + 1, 0) {
        for (int i = 0; i < nums.size(); i++)
            pre[i + 1] = pre[i] + nums[i];
    }
    int sumRange(int l, int r) {
        return pre[r + 1] - pre[l];
    }
};
```

### 2.2 LC 560 — 和为 K 的子数组

**问题**：统计数组中和为 k 的连续子数组个数。

**核心思想**：`sum(l, r) = pre[r+1] - pre[l] = k` → `pre[l] = pre[r+1] - k`

用 **哈希表** 记录每个前缀和出现次数，遍历到 `pre[r+1]` 时，查找之前有多少个 `pre[r+1] - k`。

```cpp
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> mp;
    mp[0] = 1;  // 前缀和为0出现1次
    int sum = 0, count = 0;
    for (int num : nums) {
        sum += num;
        if (mp.count(sum - k))
            count += mp[sum - k];
        mp[sum]++;
    }
    return count;
}
```

> ⚠️ **不能用滑动窗口**：因为 nums 可能有负数，窗口没有单调性。

### 2.3 LC 523 — 连续的子数组和（mod k）

子数组和是 k 的倍数 → `(pre[r+1] - pre[l]) % k == 0` → `pre[r+1] % k == pre[l] % k`

```cpp
bool checkSubarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> mp;
    mp[0] = -1; // 余数0出现在"索引-1"处
    int sum = 0;
    for (int i = 0; i < nums.size(); i++) {
        sum += nums[i];
        int mod = sum % k;
        if (mp.count(mod)) {
            if (i - mp[mod] >= 2) return true;
        } else {
            mp[mod] = i;
        }
    }
    return false;
}
```

### 2.4 LC 974 — 和可被 K 整除的子数组

与 LC 523 类似，统计余数相同的前缀和对数。

```cpp
int subarraysDivByK(vector<int>& nums, int k) {
    unordered_map<int, int> mp;
    mp[0] = 1;
    int sum = 0, count = 0;
    for (int num : nums) {
        sum += num;
        int mod = ((sum % k) + k) % k;  // 处理负数取模
        count += mp[mod];
        mp[mod]++;
    }
    return count;
}
```

> ⚠️ C++ 中负数 `%` 结果可能为负！`((x % k) + k) % k` 保证非负。

### 2.5 LC 525 — 连续数组

将 0 视为 -1，问题转化为：**和为 0 的最长子数组** → 哈希表记录前缀和第一次出现的位置。

```cpp
int findMaxLength(vector<int>& nums) {
    unordered_map<int, int> mp;
    mp[0] = -1;
    int sum = 0, maxLen = 0;
    for (int i = 0; i < nums.size(); i++) {
        sum += (nums[i] == 0 ? -1 : 1);
        if (mp.count(sum))
            maxLen = max(maxLen, i - mp[sum]);
        else
            mp[sum] = i;
    }
    return maxLen;
}
```

### 2.6 LC 1248 — 统计「优美子数组」

奇数个数恰好为 k → 将奇数视为 1、偶数视为 0 → 前缀和 + 哈希表（同 LC 560 框架）。

---

## 第三章 二维前缀和

### 3.1 核心公式

给 `m × n` 矩阵 `matrix`，构建 `(m+1) × (n+1)` 前缀和矩阵 `pre`：

**构建**（容斥原理）：
$$pre[i][j] = matrix[i-1][j-1] + pre[i-1][j] + pre[i][j-1] - pre[i-1][j-1]$$

**查询** 子矩阵 `(r1,c1)` 到 `(r2,c2)` 的和：
$$\text{sum} = pre[r2+1][c2+1] - pre[r1][c2+1] - pre[r2+1][c1] + pre[r1][c1]$$

### 3.2 图示理解

```
求 (r1,c1) 到 (r2,c2) 的和:

  ┌──────────┬──────────┐
  │    A     │    B     │
  │          │          │
  ├──────────┼──────────┤ ← r1
  │    C     │  目标D   │
  │          │          │
  └──────────┴──────────┘ ← r2
             ↑          ↑
            c1         c2

sum(D) = pre(全部) - pre(A+B) - pre(A+C) + pre(A)
       = pre[r2+1][c2+1] - pre[r1][c2+1] - pre[r2+1][c1] + pre[r1][c1]
```

### 3.3 LC 304 — 二维区域和检索

```cpp
class NumMatrix {
    vector<vector<int>> pre;
public:
    NumMatrix(vector<vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        pre.assign(m + 1, vector<int>(n + 1, 0));
        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++)
                pre[i][j] = matrix[i-1][j-1] + pre[i-1][j]
                          + pre[i][j-1] - pre[i-1][j-1];
    }
    int sumRegion(int r1, int c1, int r2, int c2) {
        return pre[r2+1][c2+1] - pre[r1][c2+1]
             - pre[r2+1][c1] + pre[r1][c1];
    }
};
```

### 3.4 LC 1074 — 元素和为目标值的子矩阵数量

**思路**：固定上下行 `r1, r2`，将列方向压缩成一维 → 转化为一维 LC 560。

```
枚举 r1 (0..m-1):
  枚举 r2 (r1..m-1):
    对每列 j 计算 colSum[j] = Σ matrix[r1..r2][j]
    → 一维问题：colSum[] 中和为 target 的子数组个数
```

时间 $O(m^2 \cdot n)$，是不错的优化。

### 3.5 LC 221 — 最大正方形

虽然主流解法是 DP，但二维前缀和也能辅助：`O(1)` 判定一个正方形内全是 1。

---

## 第四章 前缀和变种

### 4.1 前缀积

```cpp
pre_prod[0] = 1;
pre_prod[i] = pre_prod[i-1] * nums[i-1];
// 区间 [l,r] 的积 = pre_prod[r+1] / pre_prod[l]
```

应用：**LC 238 — 除自身以外数组的乘积**
- 左侧前缀积 + 右侧后缀积

### 4.2 前缀异或

```cpp
pre_xor[0] = 0;
pre_xor[i] = pre_xor[i-1] ^ nums[i-1];
// 区间 [l,r] 的异或 = pre_xor[r+1] ^ pre_xor[l]
```

应用：**LC 1310 — 子数组异或查询**

### 4.3 前缀最大/最小值

```cpp
pre_max[i] = max(pre_max[i-1], nums[i]);
```

应用：与后缀最小值组合 → **LC 42 接雨水**变体。

### 4.4 前缀和 + 二分查找

当前缀和单调递增时（即原数组全非负），可以用**二分**查找满足条件的区间。

应用：**LC 209 — 长度最小的子数组** → 前缀和 + 二分 $O(n \log n)$（也可滑动窗口 $O(n)$）。

### 4.5 前缀和 + 哈希表 总结

| 看到什么 | 用什么组合 | 经典 LC |
|----------|-----------|---------|
| 子数组和 = k | pre + hashmap | 560 |
| 子数组和 % k = 0 | pre mod + hashmap | 523, 974 |
| 0/1 最长等量子数组 | 映射(-1/1) + pre + hashmap | 525 |
| 子数组异或 = k | pre_xor + hashmap | 1310 变体 |

---

## 第五章 三维前缀和（了解）

用于 3D 网格问题，容斥公式有 8 项。实际 LeetCode 很少考。

$$pre[i][j][k] = A[i-1][j-1][k-1] + pre[i-1][j][k] + pre[i][j-1][k] + pre[i][j][k-1] - pre[i-1][j-1][k] - pre[i-1][j][k-1] - pre[i][j-1][k-1] + pre[i-1][j-1][k-1]$$

---

## 第六章 LeetCode 题目清单

### 6.1 基础题

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 303 | 区域和检索 — 不可变 | 一维前缀和模板 | ⭐ |
| 304 | 二维区域和检索 | 二维前缀和模板 | ⭐⭐ |
| 238 | 除自身以外数组乘积 | 前缀积 + 后缀积 | ⭐⭐ |
| 1310 | 子数组异或查询 | 前缀异或 | ⭐⭐ |
| 1480 | 一维数组的动态和 | 最基础前缀和 | ⭐ |

### 6.2 前缀和 + 哈希表

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 560 | 和为K的子数组 | pre + hashmap 计数 | ⭐⭐ |
| 523 | 连续子数组和 | pre % k + hashmap 位置 | ⭐⭐ |
| 974 | 和可被K整除的子数组 | pre % k + 组合计数 | ⭐⭐ |
| 525 | 连续数组 | 0→-1 映射 + 最长 | ⭐⭐ |
| 930 | 和相同的二元子数组 | 同 560 框架 | ⭐⭐ |
| 1248 | 统计优美子数组 | 奇→1 偶→0 + 同 560 | ⭐⭐ |

### 6.3 二维前缀和

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 304 | 二维区域和检索 | 二维前缀和 | ⭐⭐ |
| 1074 | 元素和为目标的子矩阵 | 压缩行 + 560 | ⭐⭐⭐⭐ |
| 363 | 矩形区域不超过K的最大和 | 压缩行 + set二分 | ⭐⭐⭐⭐ |

### 6.4 综合应用

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 209 | 长度最小的子数组 | 前缀和+二分 / 滑动窗口 | ⭐⭐ |
| 862 | 和至少为K的最短子数组 | 前缀和+单调队列 | ⭐⭐⭐⭐ |
| 1171 | 从链表中删去和为零的节点 | 前缀和+哈希 | ⭐⭐ |
| 528 | 按权重随机选择 | 前缀和+二分 | ⭐⭐ |

---

## 第七章 总结与易错点

### 7.1 知识脑图

```
前缀和
├── 一维前缀和
│   ├── 基础: pre[i+1] = pre[i] + nums[i]
│   ├── + 哈希表 → 和为K / 整除K (LC 560/523/974/525)
│   ├── + 二分 → 非负数组最短子数组 (LC 209)
│   └── + 单调队列 → 有负数最短子数组 (LC 862)
├── 二维前缀和
│   ├── 容斥原理构建与查询 (LC 304)
│   └── 压缩行降维 → 一维问题 (LC 1074)
├── 变种
│   ├── 前缀积 (LC 238)
│   ├── 前缀异或 (LC 1310)
│   └── 前缀 max/min
└── 与差分的关系
    └── 差分是前缀和的逆运算
```

### 7.2 七大易错点

| # | 易错点 | 正确做法 |
|---|--------|----------|
| 1 | pre 长度 n 而非 n+1 | `vector<long long> pre(n+1, 0)` |
| 2 | 下标错位 | `sum(l,r) = pre[r+1] - pre[l]`，不是 `pre[r] - pre[l-1]` |
| 3 | int 溢出 | 前缀和用 `long long` |
| 4 | 负数取模为负 | `((x % k) + k) % k` |
| 5 | 哈希表未初始化 mp[0] | `mp[0] = 1`（计数）或 `mp[0] = -1`（位置） |
| 6 | 二维容斥漏减重复 | 画图理解 A、B、C、D 四块 |
| 7 | 把前缀和当滑动窗口 | 有负数时不能用滑动窗口！ |

### 7.3 面试 FAQ

**Q: 前缀和和滑动窗口什么时候用？**  
A: 数组全非负 → 滑动窗口更优（O(n), O(1) 空间）。有负数 → 必须用前缀和+哈希表。

**Q: 二维前缀和怎么背公式？**  
A: 不要死背，画图：求 D 区域 = 整个大矩形 - 上方 - 左方 + 左上角（重复减的加回来）。

**Q: LC 560 为什么不能用滑动窗口？**  
A: 因为 nums 有负数，当窗口和 > k 时缩小窗口，和可能先减后增，没有单调性。

---

> 📂 **配套代码文件**：`knowlege_details_19_prefix_sum.cpp`  
> 📂 **差分专题**：`knowlege_details_19_difference_array.md` + `.cpp`
