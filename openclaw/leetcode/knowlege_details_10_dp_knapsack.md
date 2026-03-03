# 专题十（下）：背包问题、区间DP与进阶DP

> 作者：大胖超 😜
> 配合 `knowlege_details_10_dp_knapsack.cpp` 食用更佳

---

## 目录

1. [背包问题总览](#1-背包问题总览)
2. [0-1 背包](#2-0-1-背包)
3. [完全背包](#3-完全背包)
4. [背包变体：目标和与分割](#4-背包变体目标和与分割)
5. [背包变体：零钱兑换](#5-背包变体零钱兑换)
6. [区间 DP](#6-区间-dp)
7. [字符串 DP 进阶](#7-字符串-dp-进阶)
8. [DP 计数问题](#8-dp-计数问题)
9. [记忆化搜索](#9-记忆化搜索)
10. [综合刷题清单](#10-综合刷题清单)
11. [易错点与总结](#11-易错点与总结)

---

## 1. 背包问题总览

### 背包三兄弟

| 类型 | 物品使用 | 内层遍历 | 典型题 |
|------|---------|---------|--------|
| **0-1 背包** | 每种物品最多选 1 次 | 容量**从大到小** | LC 416 |
| **完全背包** | 每种物品可选无限次 | 容量**从小到大** | LC 322, 518 |
| **多重背包** | 每种物品选有限次 | 二进制优化 | 竞赛题 |

### 核心记忆口诀

```
0-1 背包内层倒着遍历 → 保证每个物品只用一次
完全背包内层正着遍历 → 允许同一物品重复使用
```

### 遍历顺序的本质

```
dp[j] = max(dp[j], dp[j - w[i]] + v[i])

倒序（0-1）：
  dp[j] 还没被当前物品 i 更新过
  dp[j-w[i]] 也没被更新 → 来自上一轮（不含物品i）

正序（完全）：
  dp[j-w[i]] 可能已被物品 i 更新 → 相当于物品 i 已选过
  → 允许重复选择
```

---

## 2. 0-1 背包

### 标准模板

```
物品：weight[i], value[i]
容量：capacity

dp[j] = 容量为 j 时的最大价值
dp[j] = max(dp[j], dp[j - weight[i]] + value[i])
```

```cpp
int knapsack01(vector<int>& weight, vector<int>& value, int capacity) {
    vector<int> dp(capacity + 1, 0);
    for (int i = 0; i < (int)weight.size(); i++) {
        for (int j = capacity; j >= weight[i]; j--) {  // 倒序！
            dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);
        }
    }
    return dp[capacity];
}
```

### 二维版本（便于理解）

```cpp
int knapsack01_2d(vector<int>& w, vector<int>& v, int cap) {
    int n = w.size();
    // dp[i][j] = 前 i 个物品、容量 j 时的最大价值
    vector<vector<int>> dp(n + 1, vector<int>(cap + 1, 0));
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= cap; j++) {
            dp[i][j] = dp[i-1][j];  // 不选第 i 个
            if (j >= w[i-1])
                dp[i][j] = max(dp[i][j], dp[i-1][j-w[i-1]] + v[i-1]);
        }
    }
    return dp[n][cap];
}
```

### LC 416: 分割等和子集（0-1 背包变体）

**转化：** 能否从数组中选出一些数，使得它们的和恰好 = sum/2？

```cpp
bool canPartition(vector<int>& nums) {
    int sum = 0;
    for (int x : nums) sum += x;
    if (sum % 2 != 0) return false;
    int target = sum / 2;
    
    vector<bool> dp(target + 1, false);
    dp[0] = true;
    for (int num : nums) {
        for (int j = target; j >= num; j--) {  // 0-1 背包倒序
            dp[j] = dp[j] || dp[j - num];
        }
    }
    return dp[target];
}
```

### LC 1049: 最后一块石头的重量 II

**转化：** 把石头分成两堆，求两堆差值最小 → 0-1 背包求能装的最大重量

```cpp
int lastStoneWeightII(vector<int>& stones) {
    int sum = 0;
    for (int s : stones) sum += s;
    int target = sum / 2;
    
    vector<bool> dp(target + 1, false);
    dp[0] = true;
    for (int s : stones)
        for (int j = target; j >= s; j--)
            dp[j] = dp[j] || dp[j - s];
    
    // 找到 <= target 的最大可装重量
    for (int j = target; j >= 0; j--)
        if (dp[j]) return sum - 2 * j;
    return sum;
}
```

---

## 3. 完全背包

### 标准模板

```cpp
int knapsackComplete(vector<int>& weight, vector<int>& value, int capacity) {
    vector<int> dp(capacity + 1, 0);
    for (int i = 0; i < (int)weight.size(); i++) {
        for (int j = weight[i]; j <= capacity; j++) {  // 正序！
            dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);
        }
    }
    return dp[capacity];
}
```

### LC 322: 零钱兑换（最少硬币数）

```
dp[j] = 凑齐金额 j 所需的最少硬币数
dp[j] = min(dp[j], dp[j - coin] + 1)
```

```cpp
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;
    for (int coin : coins) {
        for (int j = coin; j <= amount; j++) {
            if (dp[j - coin] != INT_MAX)
                dp[j] = min(dp[j], dp[j - coin] + 1);
        }
    }
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}
```

### LC 518: 零钱兑换 II（组合数）

```
dp[j] = 凑齐金额 j 的组合数
dp[j] += dp[j - coin]
```

```cpp
int change(int amount, vector<int>& coins) {
    vector<int> dp(amount + 1, 0);
    dp[0] = 1;
    for (int coin : coins) {              // 外层遍历物品 → 组合数
        for (int j = coin; j <= amount; j++) {
            dp[j] += dp[j - coin];
        }
    }
    return dp[amount];
}
```

### LC 279: 完全平方数

```cpp
int numSquares(int n) {
    vector<int> dp(n + 1, INT_MAX);
    dp[0] = 0;
    for (int i = 1; i * i <= n; i++) {
        int sq = i * i;
        for (int j = sq; j <= n; j++) {
            dp[j] = min(dp[j], dp[j - sq] + 1);
        }
    }
    return dp[n];
}
```

---

## 4. 背包变体：目标和与分割

### LC 494: 目标和

**转化：** 设正数和为 P，负数和为 N，则 P - N = target，P + N = sum → P = (sum + target) / 2

```cpp
int findTargetSumWays(vector<int>& nums, int target) {
    int sum = 0;
    for (int x : nums) sum += x;
    if ((sum + target) % 2 != 0 || sum + target < 0) return 0;
    int bagSize = (sum + target) / 2;
    
    vector<int> dp(bagSize + 1, 0);
    dp[0] = 1;
    for (int num : nums) {
        for (int j = bagSize; j >= num; j--) {  // 0-1 背包
            dp[j] += dp[j - num];
        }
    }
    return dp[bagSize];
}
```

> 这题是"装满背包有多少种方法"，不是求最大价值

---

## 5. 背包变体：零钱兑换

### 组合数 vs 排列数

```
组合（LC 518）：先遍历物品，再遍历背包 → {1,2} 和 {2,1} 算同一种
排列（LC 377）：先遍历背包，再遍历物品 → {1,2} 和 {2,1} 算两种
```

### LC 377: 组合总和 IV（排列数）

```cpp
int combinationSum4(vector<int>& nums, int target) {
    vector<unsigned int> dp(target + 1, 0);
    dp[0] = 1;
    for (int j = 1; j <= target; j++) {       // 外层遍历背包容量
        for (int num : nums) {                 // 内层遍历物品
            if (j >= num) dp[j] += dp[j - num];
        }
    }
    return dp[target];
}
```

### 背包遍历顺序总结

| 需求 | 外层 | 内层 | 典型题 |
|------|------|------|--------|
| 0-1 背包（最大值） | 物品 | 容量（倒序） | LC 416 |
| 完全背包（最大值） | 物品 | 容量（正序） | LC 322 |
| 组合数 | 物品 | 容量 | LC 518 |
| 排列数 | 容量 | 物品 | LC 377 |

---

## 6. 区间 DP

### 核心思想

在区间 `[i, j]` 上定义状态，通过枚举分割点 `k` 合并子区间。

### 通用模板

```cpp
// dp[i][j] = 区间 [i,j] 上的最优值
for (int len = 2; len <= n; len++) {        // 枚举区间长度
    for (int i = 0; i + len - 1 < n; i++) { // 枚举起点
        int j = i + len - 1;                // 终点
        for (int k = i; k < j; k++) {       // 枚举分割点
            dp[i][j] = merge(dp[i][k], dp[k+1][j]);
        }
    }
}
```

### LC 516: 最长回文子序列

```
dp[i][j] = s[i..j] 中最长回文子序列长度

if s[i] == s[j]:
    dp[i][j] = dp[i+1][j-1] + 2   ← 两端字符加入回文
else:
    dp[i][j] = max(dp[i+1][j], dp[i][j-1])  ← 去掉一端
```

```cpp
int longestPalindromeSubseq(string s) {
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int i = n - 1; i >= 0; i--) {
        dp[i][i] = 1;
        for (int j = i + 1; j < n; j++) {
            if (s[i] == s[j])
                dp[i][j] = dp[i + 1][j - 1] + 2;
            else
                dp[i][j] = max(dp[i + 1][j], dp[i][j - 1]);
        }
    }
    return dp[0][n - 1];
}
```

### LC 5: 最长回文子串

注意："子串"是连续的，"子序列"可以不连续

```cpp
string longestPalindrome(string s) {
    int n = s.size(), start = 0, maxLen = 1;
    vector<vector<bool>> dp(n, vector<bool>(n, false));
    
    for (int i = n - 1; i >= 0; i--) {
        for (int j = i; j < n; j++) {
            dp[i][j] = (s[i] == s[j]) && (j - i <= 2 || dp[i+1][j-1]);
            if (dp[i][j] && j - i + 1 > maxLen) {
                maxLen = j - i + 1;
                start = i;
            }
        }
    }
    return s.substr(start, maxLen);
}
```

### LC 647: 回文子串个数

```cpp
int countSubstrings(string s) {
    int n = s.size(), count = 0;
    vector<vector<bool>> dp(n, vector<bool>(n, false));
    for (int i = n - 1; i >= 0; i--)
        for (int j = i; j < n; j++) {
            dp[i][j] = (s[i] == s[j]) && (j - i <= 2 || dp[i+1][j-1]);
            if (dp[i][j]) count++;
        }
    return count;
}
```

---

## 7. 字符串 DP 进阶

### LC 139: 单词拆分

```
dp[i] = s[0..i-1] 能否被拆分成词典中的单词
dp[i] = any(dp[j] && s[j..i-1] in dict)  for j in [0, i-1]
```

```cpp
bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> dict(wordDict.begin(), wordDict.end());
    int n = s.size();
    vector<bool> dp(n + 1, false);
    dp[0] = true;
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            if (dp[j] && dict.count(s.substr(j, i - j))) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[n];
}
```

### LC 115: 不同的子序列

```
dp[i][j] = s 的前 i 个字符中，t 的前 j 个字符出现的次数

if s[i-1] == t[j-1]:
    dp[i][j] = dp[i-1][j-1] + dp[i-1][j]  ← 用 s[i-1] 匹配 + 不用
else:
    dp[i][j] = dp[i-1][j]  ← 跳过 s[i-1]
```

---

## 8. DP 计数问题

### LC 96: 不同的二叉搜索树（卡特兰数）

```
dp[n] = n 个节点组成的不同 BST 数量
dp[n] = Σ dp[i-1] * dp[n-i]  for i=1..n
   （i 作为根，左子树 i-1 个节点，右子树 n-i 个节点）
```

```cpp
int numTrees(int n) {
    vector<int> dp(n + 1, 0);
    dp[0] = dp[1] = 1;
    for (int i = 2; i <= n; i++)
        for (int j = 1; j <= i; j++)
            dp[i] += dp[j - 1] * dp[i - j];
    return dp[n];
}
```

### LC 343: 整数拆分

```
dp[i] = 拆分整数 i 得到的最大乘积
dp[i] = max(j*(i-j), j*dp[i-j])  for j=1..i-1
```

```cpp
int integerBreak(int n) {
    vector<int> dp(n + 1, 0);
    dp[2] = 1;
    for (int i = 3; i <= n; i++)
        for (int j = 1; j < i; j++)
            dp[i] = max(dp[i], max(j * (i - j), j * dp[i - j]));
    return dp[n];
}
```

---

## 9. 记忆化搜索

**当 DP 遍历顺序不好想时，可以用记忆化搜索代替：**

```cpp
// 以 LC 516 为例
unordered_map<int, int> memo;

int helper(string& s, int i, int j) {
    if (i > j) return 0;
    if (i == j) return 1;
    int key = i * 1000 + j;
    if (memo.count(key)) return memo[key];
    
    int res;
    if (s[i] == s[j])
        res = helper(s, i + 1, j - 1) + 2;
    else
        res = max(helper(s, i + 1, j), helper(s, i, j - 1));
    return memo[key] = res;
}
```

### 什么时候用记忆化搜索

| 场景 | 推荐方式 |
|------|---------|
| 遍历顺序不明确 | 记忆化搜索 |
| 需要剪枝跳过大量无用状态 | 记忆化搜索 |
| 遍历顺序清晰，不需要剪枝 | 表格法 DP |
| 需要空间优化 | 表格法 DP |

---

## 10. 综合刷题清单

### 背包问题

| 序号 | 题目 | 难度 | 类型 |
|------|------|------|------|
| 1 | LC 416: 分割等和子集 | Medium | 0-1 背包 |
| 2 | LC 1049: 最后一块石头 II | Medium | 0-1 背包 |
| 3 | LC 494: 目标和 | Medium | 0-1 计数 |
| 4 | LC 322: 零钱兑换 | Medium | 完全背包（最小值）|
| 5 | LC 518: 零钱兑换 II | Medium | 完全背包（组合数）|
| 6 | LC 377: 组合总和 IV | Medium | 完全背包（排列数）|
| 7 | LC 279: 完全平方数 | Medium | 完全背包 |
| 8 | LC 139: 单词拆分 | Medium | 完全背包变体 |

### 区间 DP & 字符串 DP

| 序号 | 题目 | 难度 | 类型 |
|------|------|------|------|
| 9 | LC 516: 最长回文子序列 | Medium | 区间 DP |
| 10 | LC 5: 最长回文子串 | Medium | 区间 DP |
| 11 | LC 647: 回文子串个数 | Medium | 区间 DP |
| 12 | LC 115: 不同的子序列 | Hard | 字符串 DP |

### 计数 DP

| 序号 | 题目 | 难度 | 类型 |
|------|------|------|------|
| 13 | LC 96: 不同的 BST | Medium | 卡特兰数 |
| 14 | LC 343: 整数拆分 | Medium | 乘积最大化 |

### 推荐刷题顺序

```
0-1背包理论 → 分割等和(416) → 最后石头II(1049) → 目标和(494) →
零钱兑换(322) → 零钱II(518) → 组合IV(377) → 完全平方数(279) →
单词拆分(139) → 最长回文子串(5) → 回文子串数(647) →
最长回文子序列(516) → 不同BST(96) → 整数拆分(343) → 不同子序列(115)
```

---

## 11. 易错点与总结

### 背包问题易混淆点

| 易混淆 | 正确理解 |
|--------|---------|
| 0-1 和完全背包的内层方向 | 0-1 倒序，完全正序 |
| 组合数和排列数的外层选择 | 组合：外层物品；排列：外层容量 |
| dp 初始化 `0` vs `INT_MAX` | 求最大值初始化 0；求最小值初始化 `INT_MAX` |
| dp[0] 初始化 | 求方案数 dp[0]=1；求最值 dp[0]=0 |

### DP 系统总结

| DP 类型 | 状态定义模式 | 典型题 |
|---------|------------|--------|
| 线性 DP | dp[i] = 前 i 个/以 i 结尾 | 53, 198, 300 |
| 二维 DP | dp[i][j] = 前 i,j 个 | 72, 1143 |
| 网格 DP | dp[i][j] = 到(i,j)的值 | 62, 64 |
| 背包 DP | dp[j] = 容量 j 时的值 | 322, 416 |
| 区间 DP | dp[i][j] = 区间[i,j]的值 | 5, 516 |
| 状态机 DP | dp[state] = 状态下的值 | 121, 309 |
| 计数 DP | dp[n] = n 的方案数 | 96, 343 |
| 树形 DP | dfs 返回 pair | 337 |

### 万能 Debug 流程

```
1. 打印 dp 数组/表格，和手算对比
2. 检查初始条件（dp[0] 是什么语义？）
3. 检查遍历顺序（要不要倒序？先物品还是先容量？）
4. 检查状态转移方程（漏了哪种情况？）
5. 检查边界（i-1 >= 0？j >= weight[i]？）
```
