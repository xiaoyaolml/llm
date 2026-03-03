# 🔍 全 20 专题查漏补缺 — 跨专题知识衔接与易混淆点辨析

*作者：大胖超 😜 | 看完 20 个专题后的查漏补缺总指南*

---

## 目录

1. [跨专题知识衔接地图](#1-跨专题知识衔接地图)
2. [易混淆算法辨析](#2-易混淆算法辨析)
3. [高频遗漏知识点补充](#3-高频遗漏知识点补充)
4. [20 专题知识自测清单](#4-20-专题知识自测清单)
5. [常见 Bug 模式与调试技巧](#5-常见-bug-模式与调试技巧)
6. [竞赛级补充模板](#6-竞赛级补充模板)

---

# 1. 跨专题知识衔接地图

很多题目需要**组合使用**多个专题的技巧。这里整理专题之间的关联。

## 1.1 专题关联矩阵

```
专题              常搭配使用的专题                        典型题
─────────────────────────────────────────────────────────────────
数组 (1)         + 双指针(5), 滑窗(6), 前缀和(19)        LC 238, 560
链表 (2)         + 双指针(5), 排序(13)                    LC 148, 23
栈 (3)           + 单调栈(17), 哈希(4)                    LC 84, 85
哈希 (4)         + 前缀和(19), 滑窗(6)                    LC 560, 3
双指针 (5)       + 排序(13), 二分(7)                      LC 15, 167
滑窗 (6)         + 哈希(4), 单调队列(17)                  LC 76, 239
二分 (7)         + 排序(13), DP(10), 贪心(11)             LC 300, 875
二叉树 (8)       + DFS/回溯(9), DP(10)                    LC 124, 337
回溯 (9)         + 剪枝(排序13), 位运算(18)               LC 47, 78
DP (10)          + 二分(7), 贪心(11), 位运算(18)          LC 300, 416
贪心 (11)        + 排序(13), DP(10)                       LC 435, 55
图论 (12)        + 并查集(16), 堆(14), 拓扑               LC 743, 207
排序 (13)        + 双指针(5), 贪心(11)                    LC 56, 179
堆 (14)          + 排序(13), 贪心(11)                     LC 23, 295
Trie (15)        + DFS/回溯(9), 位运算(18)                LC 212, 421
并查集 (16)      + 图论(12), 排序(13)                     LC 684, 1584
单调栈/队列 (17) + 滑窗(6), DP(10)                        LC 84, 239
位运算 (18)      + 回溯(9), DP(10)                        LC 78, 338
前缀和/差分 (19) + 哈希(4), 二分(7)                       LC 560, 1094
线段树/BIT (20)  + 离散化(排序13), 前缀和(19)             LC 315, 307
```

## 1.2 典型组合技（同一道题用多种技巧）

| 组合技 | 思路 | 典型题 |
|-------|------|--------|
| **前缀和 + 哈希表** | 前缀和把"区间和"转"两数之差" | LC 560, 974, 523 |
| **排序 + 双指针** | 排序消除无序性，双指针高效遍历 | LC 15, 16, 18 |
| **排序 + 贪心** | 排序后按某维度贪心选择 | LC 56, 435, 452 |
| **DFS + 回溯 + 剪枝** | 搜索所有方案，提前排除无效分支 | LC 39, 40, 47 |
| **BFS + 哈希** | 层次遍历 + 记录已访问状态 | LC 127, 752, 773 |
| **二分 + 贪心验证** | 二分答案，贪心验证可行性 | LC 875, 1011, 410 |
| **单调栈 + DP** | 单调栈确定边界，DP 计算结果 | LC 84, 85, 907 |
| **树状数组 + 离散化** | 离散化值域，BIT 动态统计 | LC 315, 493, 327 |
| **线段树 + 扫描线** | 扫描一维，线段树维护另一维 | LC 218, 850 |
| **DP + 位运算（状压）** | 位掩码表示集合状态 | LC 464, 698, 847 |

---

# 2. 易混淆算法辨析

## 2.1 DP vs 贪心 vs 回溯

| 维度 | DP | 贪心 | 回溯 |
|------|-----|------|------|
| **核心** | 最优子结构 + 重叠子问题 | 局部最优 → 全局最优 | 穷举所有可能 |
| **是否回退** | 不回退（记忆化） | 不回退（一路向前） | 会回退（撤销选择） |
| **保证最优** | ✅ 一定最优 | ❌ 需要证明 | ✅ 穷举保证 |
| **效率** | 中等 | 最快 | 最慢 |
| **什么时候用** | 能写出状态转移方程 | 能证明贪心正确性 | 求所有方案/无法 DP |

**辨别口诀**：
- 求**一个**最优解 → 先试 DP，能贪心更好
- 求**所有**方案 → 回溯
- 不确定 → DP 最安全

## 2.2 BFS vs DFS

| 维度 | BFS | DFS |
|------|-----|-----|
| 数据结构 | 队列 | 栈/递归 |
| 遍历顺序 | 一圈一圈向外扩 | 一条路走到黑 |
| 适合 | 最短路径、层次遍历 | 连通性、路径搜索、回溯 |
| 空间 | 可能很大（存一整层） | 递归深度（可能栈溢出） |
| 找最短路 | ✅ 天然保证 | ❌ 需要额外处理 |

**口诀**：最短路/最少步 → BFS；所有路径/连通性 → DFS

## 2.3 二分查找三种写法对比

| 写法 | while 条件 | 区间含义 | mid 取法 | 缩小区间 | 返回值 |
|------|-----------|---------|---------|---------|--------|
| 左闭右闭 | `l <= r` | `[l, r]` | `l + (r-l)/2` | `l = mid+1` 或 `r = mid-1` | `l` 或 `r` |
| 左闭右开 | `l < r` | `[l, r)` | `l + (r-l)/2` | `l = mid+1` 或 `r = mid` | `l` |
| 左开右开 | `l + 1 < r` | `(l, r)` | `l + (r-l)/2` | `l = mid` 或 `r = mid` | 需判断 `l+1` 和 `r-1` |

> **推荐**：左闭右闭最通用，但找左边界用左闭右开更不易出错。

## 2.4 栈 vs 单调栈 vs 单调队列

| 结构 | 特点 | 经典应用 |
|------|------|---------|
| 栈 | LIFO，任意出入 | 括号匹配、表达式求值、DFS |
| 单调栈 | 维护递增/递减顺序 | 下一个更大/更小元素 |
| 单调队列 | 维护递增/递减 + 两端操作 | 滑动窗口最值 |

## 2.5 前缀和 vs 差分 vs BIT vs 线段树

| 工具 | 修改 | 查询 | 时间 | 适用 |
|------|------|------|------|------|
| 前缀和 | 不支持 | 区间和 O(1) | 预处理 O(n) | 静态数组 |
| 差分 | 区间加 O(1) | 需还原 O(n) | 预处理后一次还原 | 离线批量区间加 |
| BIT | 单点修改 O(log n) | 区间和 O(log n) | — | 动态单点修改+前缀和 |
| 线段树 | 区间修改 O(log n) | 区间查询 O(log n) | — | 万能（最灵活） |

**选择口诀**：
```
不修改      → 前缀和（最简单）
批量离线加  → 差分（最省事）
动态单点改  → BIT（代码最短）
区间改+查  → 线段树（最强大）
```

## 2.6 并查集 vs DFS/BFS 判连通

| 场景 | 并查集 | DFS/BFS |
|------|--------|---------|
| 静态连通性 | ✅ 可以 | ✅ 可以 |
| 动态加边 | ✅ 天然支持 | ❌ 需要重新遍历 |
| 最短路径 | ❌ 不支持 | ✅ BFS |
| 有向图 | ⚠️ 需要特殊处理 | ✅ 自然支持 |
| 代码量 | 很少 | 中等 |

---

# 3. 高频遗漏知识点补充

通过检查全部 20 个专题，以下知识点在主文档中涉及较少，但 LeetCode 中经常出现：

## 3.1 快速幂 Fast Power

```cpp
// 计算 base^exp % mod
long long quickPow(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = result * base % mod;
        base = base * base % mod;
        exp >>= 1;
    }
    return result;
}
```

常见应用：
- LC 50 Pow(x, n)
- 矩阵快速幂加速线性递推（斐波那契 $O(\log n)$）
- 大数取模

## 3.2 GCD / LCM（最大公约数/最小公倍数）

```cpp
// C++17 直接用 __gcd 或 <numeric> 的 gcd
int g = __gcd(a, b);
int l = a / g * b;  // LCM，先除后乘防溢出

// 手写欧几里得
int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
```

## 3.3 质数筛（埃拉托斯特尼筛法）

```cpp
vector<bool> sieve(int n) {
    vector<bool> is_prime(n + 1, true);
    is_prime[0] = is_prime[1] = false;
    for (int i = 2; i * i <= n; i++)
        if (is_prime[i])
            for (int j = i * i; j <= n; j += i)
                is_prime[j] = false;
    return is_prime;
}
```

应用：LC 204 Count Primes

## 3.4 字符串哈希（Rabin-Karp）

```cpp
// 多项式滚动哈希
const long long BASE = 131, MOD = 1e9 + 7;
vector<long long> h(n + 1, 0), pw(n + 1, 1);
for (int i = 0; i < n; i++) {
    h[i + 1] = (h[i] * BASE + s[i]) % MOD;
    pw[i + 1] = pw[i] * BASE % MOD;
}
// 获取 s[l..r] 的哈希值
auto getHash = [&](int l, int r) -> long long {
    return (h[r + 1] - h[l] * pw[r - l + 1] % MOD + MOD) % MOD;
};
```

应用：LC 187, 28, 1044

## 3.5 KMP 字符串匹配

```cpp
vector<int> buildNext(string& p) {
    int m = p.size();
    vector<int> next(m, 0);
    for (int i = 1, j = 0; i < m; i++) {
        while (j > 0 && p[i] != p[j]) j = next[j - 1];
        if (p[i] == p[j]) j++;
        next[i] = j;
    }
    return next;
}

int kmpSearch(string& s, string& p) {
    auto next = buildNext(p);
    for (int i = 0, j = 0; i < (int)s.size(); i++) {
        while (j > 0 && s[i] != p[j]) j = next[j - 1];
        if (s[i] == p[j]) j++;
        if (j == (int)p.size()) return i - j + 1;  // 找到
    }
    return -1;
}
```

应用：LC 28, 459, 214

## 3.6 拓扑排序（再次强调 — 最高频图算法之一）

```cpp
// Kahn's BFS 拓扑排序
vector<int> topoSort(int n, vector<vector<int>>& adj) {
    vector<int> indegree(n, 0), result;
    for (int u = 0; u < n; u++)
        for (int v : adj[u]) indegree[v]++;

    queue<int> q;
    for (int i = 0; i < n; i++)
        if (indegree[i] == 0) q.push(i);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        result.push_back(u);
        for (int v : adj[u])
            if (--indegree[v] == 0) q.push(v);
    }
    // result.size() == n 则无环
    return result;
}
```

## 3.7 最短路径算法对比强化

| 算法 | 适用 | 时间 | 注意 |
|------|------|------|------|
| **BFS** | 无权图 | $O(V+E)$ | 最简单，优先用 |
| **Dijkstra** | 非负权图 | $O((V+E)\log V)$ | 优先队列实现 |
| **Bellman-Ford** | 可有负权 | $O(VE)$ | 可检测负环 |
| **SPFA** | 负权（优化BF）| 平均 $O(kE)$ | 最坏 $O(VE)$，可被卡 |
| **Floyd** | 全源最短路 | $O(V^3)$ | 适合 $V \leq 400$ |

## 3.8 最小生成树

```cpp
// Kruskal + 并查集
struct Edge { int u, v, w; };
long long kruskal(int n, vector<Edge>& edges) {
    sort(edges.begin(), edges.end(), [](auto& a, auto& b) { return a.w < b.w; });
    vector<int> parent(n);
    iota(parent.begin(), parent.end(), 0);
    function<int(int)> find = [&](int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
    };
    long long mst = 0;
    int cnt = 0;
    for (auto& [u, v, w] : edges) {
        int pu = find(u), pv = find(v);
        if (pu != pv) {
            parent[pu] = pv;
            mst += w;
            if (++cnt == n - 1) break;
        }
    }
    return mst;
}
```

---

# 4. 20 专题知识自测清单

**使用方法**：每个问题试着自己回答，答不出来就回看对应专题。全部能答出 ≈ 知识掌握扎实。

## 专题 1-5：基础数据结构与技巧

| # | 自测问题 | 涉及专题 |
|---|---------|---------|
| 1 | `vector` vs `array` vs `deque` 各自优势？ | 1-数组 |
| 2 | 链表反转的迭代和递归写法都能默写吗？ | 2-链表 |
| 3 | 如何用 O(1) 空间判断链表有环并找入环点？ | 2-链表, 5-双指针 |
| 4 | 栈实现队列、队列实现栈怎么做？ | 3-栈与队列 |
| 5 | `unordered_map` 和 `map` 什么时候选哪个？ | 4-哈希表 |
| 6 | 对撞指针、快慢指针、同向双指针各适合什么模式？ | 5-双指针 |

## 专题 6-10：核心算法范式

| # | 自测问题 | 涉及专题 |
|---|---------|---------|
| 7 | 滑动窗口万能模板能默写吗？ | 6-滑窗 |
| 8 | 二分查找找左边界 vs 右边界的区别？ | 7-二分 |
| 9 | "二分答案"是什么意思？怎么写 check 函数？ | 7-二分 |
| 10 | 二叉树前/中/后序遍历的递归和迭代都能写吗？ | 8-二叉树 |
| 11 | 回溯三要素：选择列表、路径、结束条件？ | 9-回溯 |
| 12 | 子集和排列中"含重复元素"怎么去重？ | 9-回溯 |
| 13 | DP 五步法能讲清楚吗？ | 10-DP |
| 14 | 0-1 背包和完全背包的区别？遍历顺序为什么不同？ | 10-DP |

## 专题 11-15：进阶技巧

| # | 自测问题 | 涉及专题 |
|---|---------|---------|
| 15 | 贪心正确性怎么证明？（交换论证/反证法） | 11-贪心 |
| 16 | Dijkstra 为什么不能处理负权边？ | 12-图论 |
| 17 | 拓扑排序两种写法（BFS Kahn / DFS）？ | 12-图论 |
| 18 | 快排 partition 能手写吗？随机 pivot 为什么重要？ | 13-排序 |
| 19 | 归并排序过程中怎么统计逆序对？ | 13-排序 |
| 20 | 大根堆/小根堆在 C++ 中怎么声明？ | 14-堆 |
| 21 | Trie 的 insert/search/startsWith 能默写吗？ | 15-字典树 |

## 专题 16-20：高级数据结构

| # | 自测问题 | 涉及专题 |
|---|---------|---------|
| 22 | 并查集的路径压缩和按秩合并怎么写？ | 16-并查集 |
| 23 | 单调栈处理"下一个更大元素"的原理？ | 17-单调栈 |
| 24 | 单调队列维护滑动窗口最大值的原理？ | 17-单调队列 |
| 25 | `n & (n-1)` 和 `n & (-n)` 分别有什么用？ | 18-位运算 |
| 26 | 异或的三大性质？找"只出现一次的数"怎么做？ | 18-位运算 |
| 27 | 一维/二维前缀和的公式？ | 19-前缀和 |
| 28 | 差分数组适合什么场景？怎么还原？ | 19-差分 |
| 29 | BIT 的 `lowbit` 原理？update/query 操作？ | 20-树状数组 |
| 30 | 线段树的 lazy 标记 pushDown 什么时候调用？ | 20-线段树 |

---

# 5. 常见 Bug 模式与调试技巧

## 5.1 Top 10 最常见 Bug

| # | Bug 模式 | 症状 | 解决方案 |
|---|---------|------|---------|
| 1 | **整数溢出** | 答案错误、负数 | 用 `long long`，中间结果也要注意 |
| 2 | **数组越界** | 运行时崩溃 / 随机结果 | 检查所有下标是否在 `[0, n-1]` |
| 3 | **二分死循环** | TLE、程序挂起 | 检查 `while` 条件和 `l/r` 更新逻辑 |
| 4 | **忘记排序** | 双指针/二分答案错误 | 贪心/双指针题通常需要预排序 |
| 5 | **DP 初始化错误** | 第一个值就错 | `dp[0]` 的含义要和状态定义一致 |
| 6 | **回溯忘撤销** | 方案重复或错误 | `path.pop_back()` 或 `visited[i] = false` |
| 7 | **图的重复访问** | TLE 或死循环 | 使用 `visited` 数组 |
| 8 | **边界条件遗漏** | 空数组、单元素出错 | 先处理 `n == 0`, `n == 1` 特殊情况 |
| 9 | **字符串 vs 字符** | 编译错误或逻辑错误 | `'a'` (char) vs `"a"` (string) |
| 10 | **引用 vs 拷贝** | 修改不生效或意外修改 | 函数参数 `&` 的使用 |

## 5.2 调试技巧

### 打印调试法
```cpp
// 在关键位置加 cerr 输出（不影响 stdout）
cerr << "i=" << i << " j=" << j << " val=" << dp[i][j] << endl;
```

### 小用例手算法
```
对于 DP 题：
1. 画一个 3×3 或 4×4 的 DP 表格
2. 手动填表
3. 检查是否和代码填的一致
```

### 边界用例清单
```
必须测试的用例：
□ 空数组 / 空字符串
□ 单个元素
□ 两个元素
□ 全部相同元素
□ 已排序 / 逆序
□ 最大值 / 最小值（溢出测试）
□ 负数
□ 重复元素
```

## 5.3 常见 TLE（超时）原因

| 原因 | 检查方法 | 解决 |
|------|---------|------|
| 复杂度太高 | 看数据范围反推 | 换更优算法 |
| 不必要的拷贝 | 函数参数传 vector 没加 `&` | 加 `const auto&` |
| 重复计算 | DP/递归没有记忆化 | 加 `memo` |
| `unordered_map` 被卡 | 哈希碰撞退化 O(n) | 自定义哈希函数 |
| `endl` 写多了 | I/O 刷新慢 | 用 `'\n'` |
| 递归深度太大 | 段错误 | 改迭代或加 `#pragma` |

---

# 6. 竞赛级补充模板

以下模板在 LeetCode Hard 和竞赛中经常出现，补充到知识体系中。

## 6.1 区间 DP 模板

```cpp
// dp[i][j] = 区间 [i, j] 的最优解
// 枚举长度 → 枚举起点 → 枚举分割点
for (int len = 2; len <= n; len++)           // 区间长度
    for (int i = 0; i + len - 1 < n; i++) {  // 起点
        int j = i + len - 1;                 // 终点
        dp[i][j] = INT_MAX;
        for (int k = i; k < j; k++)          // 分割点
            dp[i][j] = min(dp[i][j], dp[i][k] + dp[k+1][j] + cost(i, j));
    }
```

典型题：LC 312 戳气球, LC 516 最长回文子序列, LC 1039 多边形三角剖分

## 6.2 状态压缩 DP 模板

```cpp
// mask 用一个整数表示"哪些元素被选了"
// dp[mask] = 选了 mask 中这些元素时的最优解
int n = nums.size();
vector<int> dp(1 << n, INF);
dp[0] = 0;  // 初始什么都不选
for (int mask = 0; mask < (1 << n); mask++) {
    if (dp[mask] == INF) continue;
    for (int i = 0; i < n; i++) {
        if (mask & (1 << i)) continue;  // 已经选了 i
        int newMask = mask | (1 << i);
        dp[newMask] = min(dp[newMask], dp[mask] + cost(mask, i));
    }
}
// 答案在 dp[(1 << n) - 1]
```

典型题：LC 847 访问所有节点的最短路, LC 698 划分为 K 个相等子集

## 6.3 数位 DP 模板

```cpp
// 统计 [1, n] 中满足某条件的数的个数
// 记忆化搜索版
string num = to_string(n);
int len = num.size();
// dp[pos][state][isLimit][isNum]
map<tuple<int,int,bool,bool>, long long> memo;

function<long long(int, int, bool, bool)> dfs = 
    [&](int pos, int state, bool isLimit, bool isNum) -> long long {
    if (pos == len) return isNum ? 1 : 0;
    auto key = make_tuple(pos, state, isLimit, isNum);
    if (memo.count(key)) return memo[key];
    
    long long res = 0;
    if (!isNum) res = dfs(pos + 1, state, false, false);  // 跳过（前导零）
    
    int lo = isNum ? 0 : 1;
    int hi = isLimit ? (num[pos] - '0') : 9;
    for (int d = lo; d <= hi; d++) {
        // 根据条件更新 newState
        res += dfs(pos + 1, newState, isLimit && d == hi, true);
    }
    return memo[key] = res;
};
```

典型题：LC 233 数字1的个数, LC 2376 统计特殊整数

## 6.4 Meet in the Middle（折半枚举）

```cpp
// 将 n 个元素分成两半，分别枚举，合并结果
// 把 O(2^n) 降到 O(2^{n/2} * log)
int half = n / 2;
vector<long long> left_sums, right_sums;

// 枚举左半部分所有子集和
for (int mask = 0; mask < (1 << half); mask++) {
    long long s = 0;
    for (int i = 0; i < half; i++)
        if (mask & (1 << i)) s += nums[i];
    left_sums.push_back(s);
}

// 枚举右半部分所有子集和
for (int mask = 0; mask < (1 << (n - half)); mask++) {
    long long s = 0;
    for (int i = 0; i < n - half; i++)
        if (mask & (1 << i)) s += nums[half + i];
    right_sums.push_back(s);
}

sort(right_sums.begin(), right_sums.end());
// 对每个 left_sum，在 right_sums 中二分查找 target - left_sum
```

典型题：LC 1755, LC 2035

## 6.5 自定义哈希函数（防 hack）

```cpp
struct custom_hash {
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED_RANDOM = 
            chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
};
unordered_map<int, int, custom_hash> safe_map;
```

---

*查漏补缺完成！结合各专题详解和附录分析，知识体系已全面覆盖 📚*
