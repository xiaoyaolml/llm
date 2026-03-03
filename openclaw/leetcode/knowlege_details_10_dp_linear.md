# 专题十（上）：动态规划基础、线性DP与状态机DP

> 作者：大胖超 😜
> 配合 `knowlege_details_10_dp_linear.cpp` 食用更佳

---

## 目录

1. [动态规划本质](#1-动态规划本质)
2. [DP 五步法](#2-dp-五步法)
3. [DP vs 贪心 vs 回溯](#3-dp-vs-贪心-vs-回溯)
4. [斐波那契型 DP](#4-斐波那契型-dp)
5. [打家劫舍系列](#5-打家劫舍系列)
6. [最大子数组和（Kadane 算法）](#6-最大子数组和kadane-算法)
7. [最长递增子序列 LIS](#7-最长递增子序列-lis)
8. [最长公共子序列 LCS](#8-最长公共子序列-lcs)
9. [编辑距离](#9-编辑距离)
10. [股票买卖系列（状态机 DP）](#10-股票买卖系列状态机-dp)
11. [路径计数（网格 DP）](#11-路径计数网格-dp)
12. [空间优化技巧](#12-空间优化技巧)
13. [综合刷题清单](#13-综合刷题清单)
14. [易错点与调试技巧](#14-易错点与调试技巧)

---

## 1. 动态规划本质

### 一句话定义

> **动态规划 = 记忆化的穷举**  
> 将大问题拆成**重叠子问题**，避免重复计算，用**状态转移方程**从小问题推出大问题的解。

### DP 成立的两个条件

| 条件 | 含义 | 例子 |
|------|------|------|
| **最优子结构** | 大问题的最优解包含子问题的最优解 | 最短路径：A→C 的最短路 = A→B 最短路 + B→C 最短路 |
| **重叠子问题** | 不同路径会计算同一个子问题 | 斐波那契：f(5)=f(4)+f(3)，f(4)=f(3)+f(2)，f(3) 被算两次 |

### 递归 vs 记忆化 vs DP

```
普通递归（自顶向下）：
  fib(5)
  ├── fib(4)
  │   ├── fib(3) ← 计算 1 次
  │   └── fib(2)
  └── fib(3)     ← 重复计算！

记忆化搜索（自顶向下 + 缓存）：
  fib(5) → 查表，未计算 → fib(4) + fib(3)
  fib(4) → 查表，未计算 → fib(3) + fib(2)
  fib(3) → 查表，未计算 → fib(2) + fib(1) = 2
  fib(3) → 查表，已有！直接返回 2

动态规划（自底向上）：
  dp[1]=1, dp[2]=1, dp[3]=2, dp[4]=3, dp[5]=5
  顺序填表，无递归开销
```

| 方式 | 方向 | 优点 | 缺点 |
|------|------|------|------|
| 记忆化搜索 | 自顶向下 | 代码直观，只算需要的子问题 | 递归栈开销 |
| DP 表格 | 自底向上 | 无栈开销，可空间优化 | 需想清楚遍历顺序 |

---

## 2. DP 五步法

**每道 DP 题都按这 5 步思考：**

### 步骤详解

```
Step 1: 定义状态 — dp[i] 或 dp[i][j] 表示什么？
   ┗ 这是最关键的一步，决定了整个解法

Step 2: 状态转移方程 — dp[i] 怎么从之前的状态推出？
   ┗ 把"做选择"翻译成数学关系

Step 3: 初始条件 — dp[0], dp[1] 等基础情况
   ┗ 递推的起点

Step 4: 遍历顺序 — 从前往后？从后往前？先行后列？
   ┗ 保证计算 dp[i] 时，它依赖的状态已经算好

Step 5: 举例验证 — 用小规模输入手动推演
   ┗ 检查转移方程和边界是否正确
```

### 实战示例：LC 70 爬楼梯

```
Step 1: dp[i] = 到达第 i 阶楼梯的方法数
Step 2: dp[i] = dp[i-1] + dp[i-2]
        （从 i-1 上一步，或从 i-2 上两步）
Step 3: dp[1] = 1, dp[2] = 2
Step 4: 从小到大遍历（i = 3 → n）
Step 5: n=5 → dp: 1,2,3,5,8 ✓
```

---

## 3. DP vs 贪心 vs 回溯

| 特征 | 回溯 | DP | 贪心 |
|------|------|-----|------|
| 策略 | 穷举所有方案 | 记忆化穷举 | 每步局部最优 |
| 适用 | 求"所有方案" | 求"最优值/计数" | 有贪心性质的最优 |
| 时间 | 指数级 | 多项式级 | 通常 O(n) 或 O(n log n) |
| 关系 | — | 回溯+记忆化≈DP | DP 的特例 |

**选择指南：**
```
题目要求"求所有具体方案" → 回溯
题目要求"计数/最优值" → 先想 DP
DP 中如果只需要上一个状态 → 可能可以贪心
```

---

## 4. 斐波那契型 DP

### LC 70: 爬楼梯

```cpp
int climbStairs(int n) {
    if (n <= 2) return n;
    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; i++) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

### LC 746: 使用最小花费爬楼梯

```
dp[i] = 到达第 i 阶的最小花费
dp[i] = min(dp[i-1] + cost[i-1], dp[i-2] + cost[i-2])
```

```cpp
int minCostClimbingStairs(vector<int>& cost) {
    int n = cost.size();
    int prev2 = 0, prev1 = 0;
    for (int i = 2; i <= n; i++) {
        int curr = min(prev1 + cost[i-1], prev2 + cost[i-2]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

### LC 509: 斐波那契数

```cpp
int fib(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int c = a + b;
        a = b;
        b = c;
    }
    return b;
}
```

### 斐波那契型总结

```
共同特点：dp[i] 只依赖 dp[i-1] 和 dp[i-2]
空间优化：用两个变量代替数组 → O(1) 空间
```

---

## 5. 打家劫舍系列

### LC 198: 打家劫舍

```
dp[i] = 偷到第 i 家时的最大金额
dp[i] = max(dp[i-1],        ← 不偷第 i 家
            dp[i-2] + nums[i])  ← 偷第 i 家
```

```cpp
int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 1) return nums[0];
    int prev2 = nums[0], prev1 = max(nums[0], nums[1]);
    for (int i = 2; i < n; i++) {
        int curr = max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

### LC 213: 打家劫舍 II（环形）

**关键思路：** 第一家和最后一家不能同时偷 → 拆成两个子问题

```cpp
int rob2(vector<int>& nums) {
    int n = nums.size();
    if (n == 1) return nums[0];
    
    auto robRange = [&](int lo, int hi) -> int {
        int prev2 = 0, prev1 = 0;
        for (int i = lo; i <= hi; i++) {
            int curr = max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = curr;
        }
        return prev1;
    };
    
    return max(robRange(0, n - 2),   // 选第一家，不选最后
               robRange(1, n - 1));  // 不选第一家，可选最后
}
```

---

## 6. 最大子数组和（Kadane 算法）

### LC 53: 最大子数组和

```
dp[i] = 以 nums[i] 结尾的最大子数组和
dp[i] = max(nums[i], dp[i-1] + nums[i])
       （要么从 i 重新开始，要么接在前面的后面）
```

```cpp
int maxSubArray(vector<int>& nums) {
    int maxSum = nums[0], curSum = nums[0];
    for (int i = 1; i < (int)nums.size(); i++) {
        curSum = max(nums[i], curSum + nums[i]);
        maxSum = max(maxSum, curSum);
    }
    return maxSum;
}
```

### LC 152: 乘积最大子数组

**关键：** 负数 × 负数 = 正数，所以要同时维护最大和最小

```cpp
int maxProduct(vector<int>& nums) {
    int maxProd = nums[0], curMax = nums[0], curMin = nums[0];
    for (int i = 1; i < (int)nums.size(); i++) {
        if (nums[i] < 0) swap(curMax, curMin);  // 负数翻转大小
        curMax = max(nums[i], curMax * nums[i]);
        curMin = min(nums[i], curMin * nums[i]);
        maxProd = max(maxProd, curMax);
    }
    return maxProd;
}
```

**为什么要 swap？**
```
[-2, 3, -4]
  i=0: curMax=-2, curMin=-2
  i=1: curMax=3,  curMin=-6
  i=2: swap → curMax=-6, curMin=3
       curMax = max(-4, -6×-4=24) = 24  ← 负负得正！
       curMin = min(-4, 3×-4=-12) = -12
```

---

## 7. 最长递增子序列 LIS

### LC 300: 最长递增子序列

**方法一：O(n²) DP**

```
dp[i] = 以 nums[i] 结尾的 LIS 长度
对于 j < i，若 nums[j] < nums[i]，dp[i] = max(dp[i], dp[j]+1)
```

```cpp
int lengthOfLIS(vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 1);
    int maxLen = 1;
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i])
                dp[i] = max(dp[i], dp[j] + 1);
        }
        maxLen = max(maxLen, dp[i]);
    }
    return maxLen;
}
```

**方法二：O(n log n) 贪心+二分**

维护一个 `tails` 数组：`tails[i]` = 长度为 `i+1` 的 LIS 的最小末尾

```cpp
int lengthOfLIS_fast(vector<int>& nums) {
    vector<int> tails;
    for (int num : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), num);
        if (it == tails.end())
            tails.push_back(num);    // 扩展
        else
            *it = num;               // 替换（让末尾更小，贪心）
    }
    return tails.size();
}
```

**图解 O(n log n)：**
```
nums = [10, 9, 2, 5, 3, 7, 101, 18]

10  → tails = [10]
9   → 9 < 10, 替换 → tails = [9]
2   → 2 < 9,  替换 → tails = [2]
5   → 5 > 2,  扩展 → tails = [2, 5]
3   → 3 < 5,  替换 → tails = [2, 3]
7   → 7 > 3,  扩展 → tails = [2, 3, 7]
101 → 101>7,  扩展 → tails = [2, 3, 7, 101]
18  → 18<101, 替换 → tails = [2, 3, 7, 18]

LIS 长度 = tails.size() = 4
```

> ⚠️ `tails` 不是 LIS 本身，只是辅助数组，其长度等于 LIS 长度

---

## 8. 最长公共子序列 LCS

### LC 1143: 最长公共子序列

```
dp[i][j] = text1 前 i 个字符与 text2 前 j 个字符的 LCS 长度

if text1[i-1] == text2[j-1]:
    dp[i][j] = dp[i-1][j-1] + 1   ← 两个都选
else:
    dp[i][j] = max(dp[i-1][j], dp[i][j-1])  ← 选一个跳过
```

```cpp
int longestCommonSubsequence(string text1, string text2) {
    int m = text1.size(), n = text2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (text1[i-1] == text2[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    return dp[m][n];
}
```

**DP 表格示例：**

```
      ""  a  c  e
  ""   0  0  0  0
  a    0  1  1  1
  b    0  1  1  1
  c    0  1  2  2
  d    0  1  2  2
  e    0  1  2  3

text1="abcde", text2="ace" → LCS=3 ("ace")
```

### LC 583: 两个字符串的删除操作

利用 LCS：最少删除次数 = `m + n - 2 * LCS(m, n)`

### LC 718: 最长重复子数组

注意"子数组"是**连续**的，不同于"子序列"：

```cpp
int findLength(vector<int>& nums1, vector<int>& nums2) {
    int m = nums1.size(), n = nums2.size(), maxLen = 0;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (nums1[i-1] == nums2[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            // 不相等时 dp[i][j] = 0（不是取 max！）
            maxLen = max(maxLen, dp[i][j]);
        }
    return maxLen;
}
```

> **子序列 vs 子数组/子串：**  
> 子序列：不连续，不相等时 `dp[i][j] = max(dp[i-1][j], dp[i][j-1])`  
> 子数组：连续，不相等时 `dp[i][j] = 0`

---

## 9. 编辑距离

### LC 72: 编辑距离 ⛳

**三种操作：** 插入、删除、替换

```
dp[i][j] = word1 前 i 个字符 → word2 前 j 个字符的最少操作数

if word1[i-1] == word2[j-1]:
    dp[i][j] = dp[i-1][j-1]     ← 字符相同，不操作
else:
    dp[i][j] = 1 + min(
        dp[i-1][j],     ← 删除 word1[i-1]
        dp[i][j-1],     ← 插入 word2[j-1]
        dp[i-1][j-1]    ← 替换
    )
```

```cpp
int minDistance(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (word1[i-1] == word2[j-1])
                dp[i][j] = dp[i-1][j-1];
            else
                dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        }
    return dp[m][n];
}
```

**DP 表格示例：**

```
       ""  r  o  s
  ""    0  1  2  3
  h     1  1  2  3
  o     2  2  1  2
  r     3  2  2  2
  s     4  3  3  2
  e     5  4  4  3

"horse" → "ros" = 3 次操作
```

---

## 10. 股票买卖系列（状态机 DP）

### 状态机思想

用**状态**描述当前持有情况，用**状态转移**描述每天的决策：

```
           买入           卖出
  rest ──────→ hold ──────→ sold
    ↑                        │
    └────────────────────────┘
              (冷冻期后)
```

### LC 121: 买卖股票（一次交易）

```cpp
int maxProfit(vector<int>& prices) {
    int minPrice = INT_MAX, maxProfit = 0;
    for (int p : prices) {
        minPrice = min(minPrice, p);
        maxProfit = max(maxProfit, p - minPrice);
    }
    return maxProfit;
}
```

### LC 122: 买卖股票 II（不限次数）

```cpp
int maxProfit2(vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < (int)prices.size(); i++)
        if (prices[i] > prices[i-1])
            profit += prices[i] - prices[i-1];  // 每次上涨都赚
    return profit;
}
```

### LC 309: 含冷冻期

```
状态：
  hold: 持有股票
  sold: 刚卖出（明天冷冻）
  rest: 不持有且不在冷冻期

转移：
  hold[i] = max(hold[i-1], rest[i-1] - prices[i])   ← 继续持有 or 买入
  sold[i] = hold[i-1] + prices[i]                    ← 卖出
  rest[i] = max(rest[i-1], sold[i-1])                ← 继续休息 or 冷冻结束
```

```cpp
int maxProfit309(vector<int>& prices) {
    int n = prices.size();
    if (n < 2) return 0;
    int hold = -prices[0], sold = 0, rest = 0;
    for (int i = 1; i < n; i++) {
        int newHold = max(hold, rest - prices[i]);
        int newSold = hold + prices[i];
        int newRest = max(rest, sold);
        hold = newHold; sold = newSold; rest = newRest;
    }
    return max(sold, rest);
}
```

### LC 714: 含手续费

```cpp
int maxProfit714(vector<int>& prices, int fee) {
    int hold = -prices[0], cash = 0;
    for (int i = 1; i < (int)prices.size(); i++) {
        hold = max(hold, cash - prices[i]);
        cash = max(cash, hold + prices[i] - fee);
    }
    return cash;
}
```

### 股票系列总结

| 题目 | 交易次数 | 额外约束 | 状态 |
|------|---------|---------|------|
| LC 121 | 1 次 | — | minPrice + maxProfit |
| LC 122 | 不限 | — | 每次涨幅累加 |
| LC 309 | 不限 | 冷冻期 | hold/sold/rest |
| LC 714 | 不限 | 手续费 | hold/cash |
| LC 123 | 至多 2 次 | — | hold1/sold1/hold2/sold2 |
| LC 188 | 至多 k 次 | — | hold[k]/sold[k] |

---

## 11. 路径计数（网格 DP）

### LC 62: 不同路径

```
dp[i][j] = 到达 (i,j) 的路径数
dp[i][j] = dp[i-1][j] + dp[i][j-1]
```

```cpp
int uniquePaths(int m, int n) {
    vector<int> dp(n, 1);
    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            dp[j] += dp[j-1];
    return dp[n-1];
}
```

### LC 63: 不同路径 II（有障碍物）

```cpp
int uniquePathsWithObstacles(vector<vector<int>>& grid) {
    int n = grid[0].size();
    vector<int> dp(n, 0);
    dp[0] = 1;
    for (auto& row : grid) {
        for (int j = 0; j < n; j++) {
            if (row[j] == 1) dp[j] = 0;
            else if (j > 0) dp[j] += dp[j-1];
        }
    }
    return dp[n-1];
}
```

### LC 64: 最小路径和

```cpp
int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            if (i == 0 && j == 0) continue;
            else if (i == 0) grid[i][j] += grid[i][j-1];
            else if (j == 0) grid[i][j] += grid[i-1][j];
            else grid[i][j] += min(grid[i-1][j], grid[i][j-1]);
        }
    return grid[m-1][n-1];
}
```

---

## 12. 空间优化技巧

### 滚动数组

当 `dp[i]` 只依赖 `dp[i-1]` 时，可以用一维数组代替二维：

```cpp
// 二维 → 一维
// 原始：dp[i][j] = dp[i-1][j] + dp[i][j-1]
// 优化：dp[j] += dp[j-1]（dp[j] 保存的就是上一行的值）
```

### 两个变量

当 `dp[i]` 只依赖 `dp[i-1]` 和 `dp[i-2]` 时：

```cpp
// 原始：dp[i] = dp[i-1] + dp[i-2]
// 优化：
int prev2 = dp0, prev1 = dp1;
for (...) {
    int curr = prev1 + prev2;
    prev2 = prev1;
    prev1 = curr;
}
```

### 优化适用条件

| 情况 | 优化方式 |
|------|---------|
| dp[i] 依赖 dp[i-1], dp[i-2] | 两个变量 |
| dp[i][j] 依赖同行+上行 | 一维滚动 |
| dp[i][j] 依赖 dp[i-1][j-1] | 需要额外 prev 变量 |
| dp[i][j] 依赖更多行 | 不易优化 |

---

## 13. 综合刷题清单

### 入门必做

| 序号 | 题目 | 难度 | 类型 |
|------|------|------|------|
| 1 | LC 509: 斐波那契 | Easy | 基础 |
| 2 | LC 70: 爬楼梯 | Easy | 斐波那契型 |
| 3 | LC 746: 最小花费爬楼梯 | Easy | 斐波那契型 |
| 4 | LC 198: 打家劫舍 | Medium | 线性 DP |
| 5 | LC 213: 打家劫舍 II | Medium | 环形处理 |
| 6 | LC 53: 最大子数组和 | Medium | Kadane |
| 7 | LC 121: 买卖股票 | Easy | 状态机 |

### 进阶

| 序号 | 题目 | 难度 | 类型 |
|------|------|------|------|
| 8 | LC 300: LIS | Medium | 线性 DP |
| 9 | LC 1143: LCS | Medium | 二维 DP |
| 10 | LC 72: 编辑距离 | Medium | 二维 DP |
| 11 | LC 62: 不同路径 | Medium | 网格 DP |
| 12 | LC 64: 最小路径和 | Medium | 网格 DP |
| 13 | LC 152: 乘积最大子数组 | Medium | 线性 DP |
| 14 | LC 309: 股票含冷冻期 | Medium | 状态机 |
| 15 | LC 714: 股票含手续费 | Medium | 状态机 |
| 16 | LC 122: 股票 II | Medium | 贪心/状态机 |
| 17 | LC 718: 最长重复子数组 | Medium | 二维 DP |
| 18 | LC 583: 删除操作 | Medium | LCS 变体 |

### 推荐刷题顺序

```
斐波那契(509) → 爬楼梯(70) → 最小花费(746) →
打家劫舍(198) → 打家劫舍II(213) →
最大子数组和(53) → 乘积最大(152) →
LIS(300) → LCS(1143) → 编辑距离(72) →
不同路径(62/63) → 最小路径和(64) →
股票I(121) → 股票II(122) → 含冷冻期(309) → 含手续费(714)
```

---

## 14. 易错点与调试技巧

### 常见错误 Top 5

| 排名 | 错误 | 正确做法 |
|------|------|---------|
| 1 | 初始条件搞错 | 仔细想 dp[0]、dp[1] 代表什么 |
| 2 | 遍历顺序反了 | 0-1 背包从大到小，完全背包从小到大 |
| 3 | 状态定义不清 | 写注释明确"dp[i] 表示以 i 结尾/前 i 个..." |
| 4 | 忘记边界检查 | `i-1 >= 0`、`j-1 >= 0` |
| 5 | 空间优化时丢信息 | 优化前先写出正确的二维版本 |

### 调试技巧

```
1. 打印 DP 表：不确定时把整个 dp 数组/表格打出来
2. 手动推演：用 3-5 个元素的小例子，和代码结果对比
3. 先写暴力：先写回溯/暴力版本，确保正确，再优化为 DP
4. 画状态转移图：箭头标明"从哪来到哪去"
```
