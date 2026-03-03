# 专题十六：并查集 Union-Find — 从原理到LeetCode实战

> 作者：大胖超 😜
> 
> 并查集是处理**动态连通性**问题的高效数据结构，在社交网络、图的连通分量、最小生成树（Kruskal）、等价类划分等场景中不可替代。面试高频题 LC 547/200/684/721/399 都以并查集为核心。

---

## 目录

1. [并查集基本概念](#1-并查集基本概念)
2. [核心操作与优化](#2-核心操作与优化)
3. [完整模板与变体](#3-完整模板与变体)
4. [并查集经典应用模式](#4-并查集经典应用模式)
5. [带权并查集](#5-带权并查集)
6. [并查集与图算法的结合](#6-并查集与图算法的结合)
7. [LeetCode 经典题解](#7-leetcode-经典题解)
8. [并查集 vs 其他方法](#8-并查集-vs-其他方法)
9. [题目总结与易错点](#9-题目总结与易错点)

---

## 1. 并查集基本概念

### 1.1 什么是并查集

并查集（Disjoint Set Union，DSU）维护一组**不相交集合**，支持两个核心操作：

| 操作 | 说明 | 复杂度（优化后） |
|------|------|----------------|
| **Find(x)** | 找到 x 所在集合的代表元素（根） | 接近 O(1) |
| **Union(x, y)** | 将 x 和 y 所在的两个集合合并 | 接近 O(1) |

> 准确复杂度为 O(α(n))，其中 α 是反阿克曼函数，对所有实际输入 α(n) ≤ 4。

### 1.2 直觉理解

```
初始状态: 每个元素自成一个集合（自己是自己的父亲）
  {0} {1} {2} {3} {4}
  各自独立

union(0, 1):        union(2, 3):        union(0, 2):
  {0,1} {2} {3} {4}   {0,1} {2,3} {4}   {0,1,2,3} {4}

              0                 0
             /                 / \
            1                 1   2
                                  |
                                  3

find(3) → 沿 3→2→0 找到根 0
connected(1, 3) → find(1)==find(3)==0 → true
```

### 1.3 并查集能解决什么问题

```
核心能力: 判断两个元素是否属于同一个集合

典型场景:
├── 图的连通分量计数 (LC 547, 200, 323)
├── 检测无向图中的环 (LC 684, 685)
├── 最小生成树 Kruskal (LC 1135, 1584)
├── 账户合并 / 等价类划分 (LC 721, 990)
├── 带权关系推导 (LC 399, 839)
├── 动态连通性维护
└── 冗余连接判定
```

---

## 2. 核心操作与优化

### 2.1 朴素实现（无优化）

```cpp
class NaiveUnionFind {
    vector<int> parent;
public:
    NaiveUnionFind(int n) : parent(n) {
        iota(parent.begin(), parent.end(), 0);  // parent[i] = i
    }
    
    int find(int x) {
        while (parent[x] != x)  // 沿链找根
            x = parent[x];
        return x;
    }
    
    void unite(int x, int y) {
        parent[find(x)] = find(y);  // 简单挂接
    }
};
```

**问题**：在最坏情况下，树退化成链，find 变成 O(n)。

```
最坏情况: union(0,1), union(1,2), union(2,3), ...

  0 ← 1 ← 2 ← 3 ← 4 ← 5 ...
  
  find(5) 需要走 5 步 → O(n)
```

### 2.2 优化一：路径压缩

**核心思想**：find 时，将路径上所有节点直接指向根。

```
find(5) 之前:            find(5) 之后（路径压缩）:
    0                        0
    |                      / | \ \
    1                     1  2  3  5
    |                            |
    2                            4
    |
    3
    |
    4
    |
    5

  5→4→3→2→1→0             所有节点直接指向根 0
```

```cpp
// 递归写法（简洁，面试首选）
int find(int x) {
    if (parent[x] != x)
        parent[x] = find(parent[x]);  // 递归 + 直接挂到根
    return parent[x];
}

// 迭代写法（避免栈溢出）
int find(int x) {
    int root = x;
    while (parent[root] != root) root = parent[root]; // 先找根
    while (parent[x] != root) {  // 再把路径上的节点都指向根
        int next = parent[x];
        parent[x] = root;
        x = next;
    }
    return root;
}
```

### 2.3 优化二：按秩合并 / 按大小合并

**核心思想**：合并时，将较矮（或较小）的树接到较高（或较大）的树上，避免深度增长。

#### 按秩（rank）合并

```
rank 初始都为 0

union(1, 2):  rank 相同 → 选一个做根, rank+1
    1(rank=1)
    |
    2(rank=0)

union(3, 4):
    3(rank=1)
    |
    4(rank=0)

union(1, 3):  rank 相同(都是1) → 1做根, rank变2
    1(rank=2)
   / \
  2   3(rank=1)
      |
      4

union(5, 1):  rank 不同(0 < 2) → 挂到高的下面
    1(rank=2)
   /|\
  2 3 5
    |
    4
```

```cpp
void unite(int x, int y) {
    int px = find(x), py = find(y);
    if (px == py) return;
    
    if (rank_[px] < rank_[py]) swap(px, py);
    parent[py] = px;                          // 矮树挂到高树
    if (rank_[px] == rank_[py]) rank_[px]++;  // 等高时+1
}
```

#### 按大小（size）合并

```cpp
void unite(int x, int y) {
    int px = find(x), py = find(y);
    if (px == py) return;
    
    if (size_[px] < size_[py]) swap(px, py);
    parent[py] = px;
    size_[px] += size_[py];  // 更新大小
}
```

> **面试推荐**：路径压缩 + 按秩合并，两者结合后时间复杂度为 O(α(n))。

### 2.4 两种优化的对比

| 优化方式 | 效果 | 适用 |
|----------|------|------|
| 仅路径压缩 | O(log n) 均摊 | 大多数情况已足够 |
| 仅按秩合并 | O(log n) 最坏 | 不常单独使用 |
| 两者结合 | O(α(n)) ≈ O(1) | 最优，面试标准 |

---

## 3. 完整模板与变体

### 3.1 标准模板（面试万能版）

```cpp
class UnionFind {
    vector<int> parent, rank_;
    int components;  // 连通分量个数
    
public:
    UnionFind(int n) : parent(n), rank_(n, 0), components(n) {
        iota(parent.begin(), parent.end(), 0);
    }
    
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }
    
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        
        if (rank_[px] < rank_[py]) swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        components--;
        return true;  // 合并成功
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
    
    int count() const { return components; }
};
```

### 3.2 带 size 的模板

```cpp
class UnionFindSize {
    vector<int> parent, size_;
    int components;
    
public:
    UnionFindSize(int n) : parent(n), size_(n, 1), components(n) {
        iota(parent.begin(), parent.end(), 0);
    }
    
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }
    
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        if (size_[px] < size_[py]) swap(px, py);
        parent[py] = px;
        size_[px] += size_[py];
        components--;
        return true;
    }
    
    int getSize(int x) { return size_[find(x)]; }
    int count() const { return components; }
};
```

### 3.3 HashMap 版（节点不是 0~n-1 整数时）

```cpp
class UnionFindMap {
    unordered_map<string, string> parent;
    unordered_map<string, int> rank_;
    
public:
    void add(const string& x) {
        if (parent.count(x)) return;
        parent[x] = x;
        rank_[x] = 0;
    }
    
    string find(const string& x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }
    
    void unite(const string& x, const string& y) {
        add(x); add(y);
        string px = find(x), py = find(y);
        if (px == py) return;
        if (rank_[px] < rank_[py]) swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
    }
    
    bool connected(const string& x, const string& y) {
        return find(x) == find(y);
    }
};
```

---

## 4. 并查集经典应用模式

### 4.1 模式一：连通分量计数

```
给定 n 个节点和若干条边，求连通分量个数。
初始 components = n，每次成功 union 则 components--。

典型题: LC 547 省份数量, LC 323 无向图连通分量数
```

### 4.2 模式二：环检测

```
在无向图中，如果 union(x, y) 时 find(x) == find(y)，
说明 x, y 已经连通 → 这条边形成了环！

典型题: LC 684 冗余连接
```

```cpp
// LC 684: 找到使图成为树的那条多余边
vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    int n = edges.size();
    UnionFind uf(n + 1);  // 节点从 1 开始
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1]))  // 合并失败 = 已连通 = 环
            return e;
    }
    return {};
}
```

### 4.3 模式三：等价类/分组合并

```
将具有传递关系的元素合并到同一组。
例如: 如果 a等价b, b等价c → a,b,c 属于同一组。

典型题: LC 721 账户合并, LC 990 等式方程的可满足性
```

### 4.4 模式四：虚拟节点

```
当需要表达"与集合外某个概念连通"时，引入虚拟节点。
例如: 被围绕的区域（LC 130）→ 边界'O'连接到虚拟节点。

   虚拟节点 (dummy)
   / | \ \
  所有边界上的 'O'
```

### 4.5 模式五：逆向操作（离线处理）

```
并查集只支持 union 不支持 split。
如果题目要求删除操作 → 逆向处理：
  从最终状态开始，逐步"添加"元素（即 union）。

典型题: LC 803 打砖块
```

---

## 5. 带权并查集

### 5.1 什么是带权并查集

在 `parent[x] = root` 的关系上附加一个**权值**，表示 x 到其父节点的某种关系（比例、距离、方向等）。

```
标准并查集:  parent[x] = y  仅表示 x 和 y 在同一集合
带权并查集:  parent[x] = y, weight[x] = w  
            表示 x/y = w  或  x = y + w （根据语义）
```

### 5.2 LC 399: 除法求值（经典带权并查集）

```
已知: a/b = 2.0, b/c = 3.0
求:   a/c = ?, b/a = ?, a/e = ?

建模:
  weight[a→b] = 2.0  (a = 2.0 * b)
  weight[b→c] = 3.0  (b = 3.0 * c)

  a/c = a/b × b/c = 2.0 × 3.0 = 6.0
  b/a = 1/(a/b) = 0.5
  a/e = -1.0 (e 不存在)
```

```cpp
class WeightedUnionFind {
    unordered_map<string, string> parent;
    unordered_map<string, double> weight;  // weight[x] = x / parent[x]
    
public:
    void add(const string& x) {
        if (parent.count(x)) return;
        parent[x] = x;
        weight[x] = 1.0;
    }
    
    // 返回 x 到根的权重乘积
    pair<string, double> find(const string& x) {
        if (parent[x] == x) return {x, 1.0};
        auto [root, w] = find(parent[x]);
        parent[x] = root;
        weight[x] *= w;  // 路径压缩时更新权重
        return {root, weight[x]};
    }
    
    // 已知 x / y = val，合并 x 和 y
    void unite(const string& x, const string& y, double val) {
        add(x); add(y);
        auto [rx, wx] = find(x);  // wx = x / rx
        auto [ry, wy] = find(y);  // wy = y / ry
        if (rx == ry) return;
        
        parent[rx] = ry;
        // rx / ry = (x / wx) / (y / wy) × (x/y = val)
        // → rx/ry = val × wy / wx
        weight[rx] = val * wy / wx;
    }
    
    // 查询 x / y
    double query(const string& x, const string& y) {
        if (!parent.count(x) || !parent.count(y)) return -1.0;
        auto [rx, wx] = find(x);
        auto [ry, wy] = find(y);
        if (rx != ry) return -1.0;
        return wx / wy;  // x/y = (x/root) / (y/root)
    }
};
```

### 5.3 带权路径压缩图解

```
初始:  a --(2.0)--> b --(3.0)--> c (根)
       weight[a]=2.0, parent[a]=b
       weight[b]=3.0, parent[b]=c

find(a) 路径压缩后:
       a --(6.0)--> c (根)    weight[a] = 2.0 × 3.0 = 6.0
       b --(3.0)--> c (根)    weight[b] = 3.0

关系含义:
       a/c = 6.0,  b/c = 3.0
       a/b = (a/c) / (b/c) = 6.0/3.0 = 2.0 ✓
```

### 5.4 种类并查集（拓展并查集）

用于处理"敌友关系"等两种或多种类型关系。

```
关键思想: 将每个元素拆分成 2（或3）个虚拟节点:
  x_friend (x 的朋友域)
  x_enemy  (x 的敌人域)

"x和y是朋友": union(x_friend, y_friend), union(x_enemy, y_enemy)
"x和y是敌人": union(x_friend, y_enemy), union(x_enemy, y_friend)

具体实现: 元素 x 映射到 x, x+n

典型题: LC 886 可能的二分法
```

```cpp
// LC 886: 判断是否能把人分成两组（敌人不在同组）
bool possibleBipartition(int n, vector<vector<int>>& dislikes) {
    // 每人 x 拆成 x(友) 和 x+n(敌)
    UnionFind uf(2 * n + 1);
    
    for (auto& d : dislikes) {
        int a = d[0], b = d[1];
        // a和b是敌人 → a的友域连b的敌域, a的敌域连b的友域
        uf.unite(a, b + n);
        uf.unite(a + n, b);
    }
    
    // 检查是否有人和自己的敌人在同一组
    for (int i = 1; i <= n; i++) {
        if (uf.connected(i, i + n))
            return false;  // 矛盾
    }
    return true;
}
```

---

## 6. 并查集与图算法的结合

### 6.1 Kruskal 最小生成树

```
Kruskal 算法:
1. 将所有边按权重排序
2. 从小到大遍历，用并查集判断是否形成环
3. 不形成环 → 加入 MST
4. 直到选了 n-1 条边

时间: O(E log E) (排序主导)
```

```cpp
// LC 1584: 连接所有点的最小费用
int minCostConnectPoints(vector<vector<int>>& points) {
    int n = points.size();
    vector<tuple<int,int,int>> edges;  // {cost, i, j}
    
    // 生成所有边
    for (int i = 0; i < n; i++)
        for (int j = i+1; j < n; j++) {
            int cost = abs(points[i][0]-points[j][0]) 
                     + abs(points[i][1]-points[j][1]);
            edges.push_back({cost, i, j});
        }
    
    sort(edges.begin(), edges.end());
    
    UnionFind uf(n);
    int totalCost = 0, edgeCount = 0;
    for (auto& [cost, i, j] : edges) {
        if (uf.unite(i, j)) {
            totalCost += cost;
            if (++edgeCount == n - 1) break;
        }
    }
    return totalCost;
}
```

### 6.2 判断图是否为树

```
一个无向图是树 ⟺ 
  1. 恰好 n-1 条边
  2. 图连通（并查集最终只有 1 个分量）
  3. 没有环（union 全部成功）

实际操作: 用并查集加边, 如果 union 失败说明有环
```

### 6.3 检测有向图中的冗余边

```
LC 685: 冗余连接 II（有向图版本）
比无向图复杂，需要考虑:
  1. 某节点入度为 2（两个父亲）
  2. 存在环
  需要分情况讨论
```

---

## 7. LeetCode 经典题解

### 7.1 LC 547: 省份数量

```
给定 n 个城市的连接矩阵，求连通分量（省份）个数。

isConnected = [[1,1,0],[1,1,0],[0,0,1]]
城市 0-1 连通, 城市 2 独立
→ 2 个省份
```

```cpp
int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (isConnected[i][j])
                uf.unite(i, j);
    return uf.count();
}
```

### 7.2 LC 200: 岛屿数量（并查集解法）

```
通常用 DFS/BFS, 但并查集也能解:
将相邻的 '1' 合并, 最后统计分量数。
注意: '0' 不参与并查集。
```

```cpp
int numIslands(vector<vector<char>>& grid) {
    int m = grid.size(), n = grid[0].size();
    UnionFind uf(m * n);
    int waterCount = 0;
    
    int dx[] = {1, 0}, dy[] = {0, 1};  // 只看右和下（避免重复）
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '0') {
                waterCount++;
                continue;
            }
            for (int d = 0; d < 2; d++) {
                int ni = i + dx[d], nj = j + dy[d];
                if (ni < m && nj < n && grid[ni][nj] == '1')
                    uf.unite(i * n + j, ni * n + nj);
            }
        }
    }
    return uf.count() - waterCount;
}
```

### 7.3 LC 684: 冗余连接

```
树有 n 个节点和 n-1 条边。现在多了一条边,找出它。
→ 第一条使得 union 失败（已连通）的边就是答案。
```

```cpp
vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    int n = edges.size();
    UnionFind uf(n + 1);
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1]))
            return e;
    }
    return {};
}
```

### 7.4 LC 721: 账户合并

```
给定一组账户 [name, email1, email2, ...]
如果两个账户有相同的邮箱，则它们属于同一个人。
合并后输出每个人的所有邮箱（排序）。

思路: 用 email → 第一个出现该 email 的 index 做 union
```

```cpp
vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
    int n = accounts.size();
    UnionFind uf(n);
    unordered_map<string, int> emailToIdx;  // email → 首次出现的账户下标
    
    for (int i = 0; i < n; i++) {
        for (int j = 1; j < (int)accounts[i].size(); j++) {
            auto& email = accounts[i][j];
            if (emailToIdx.count(email)) {
                uf.unite(i, emailToIdx[email]);
            } else {
                emailToIdx[email] = i;
            }
        }
    }
    
    // 按根分组收集所有邮箱
    unordered_map<int, set<string>> groups;
    for (int i = 0; i < n; i++) {
        int root = uf.find(i);
        for (int j = 1; j < (int)accounts[i].size(); j++)
            groups[root].insert(accounts[i][j]);
    }
    
    // 组装结果
    vector<vector<string>> result;
    for (auto& [root, emails] : groups) {
        vector<string> account = {accounts[root][0]};  // name
        account.insert(account.end(), emails.begin(), emails.end());
        result.push_back(account);
    }
    return result;
}
```

### 7.5 LC 990: 等式方程的可满足性

```
给定方程列表: ["a==b", "b!=c", "c==a"]
判断是否存在矛盾。

思路:
1. 先处理所有 "==" → union
2. 再检查所有 "!=" → 如果 connected 则矛盾
```

```cpp
bool equationsPossible(vector<string>& equations) {
    UnionFind uf(26);  // 26个小写字母
    
    // 先处理等式
    for (auto& eq : equations) {
        if (eq[1] == '=')  // "x==y"
            uf.unite(eq[0] - 'a', eq[3] - 'a');
    }
    
    // 再检查不等式
    for (auto& eq : equations) {
        if (eq[1] == '!')  // "x!=y"
            if (uf.connected(eq[0] - 'a', eq[3] - 'a'))
                return false;
    }
    return true;
}
```

### 7.6 LC 399: 除法求值

```
已知 equations 和 values (a/b = 2.0)
查询 queries 中的除法结果。
→ 带权并查集典型题
```

```cpp
vector<double> calcEquation(
    vector<vector<string>>& equations,
    vector<double>& values,
    vector<vector<string>>& queries) 
{
    WeightedUnionFind wuf;
    for (int i = 0; i < (int)equations.size(); i++) {
        wuf.unite(equations[i][0], equations[i][1], values[i]);
    }
    
    vector<double> result;
    for (auto& q : queries) {
        result.push_back(wuf.query(q[0], q[1]));
    }
    return result;
}
```

### 7.7 LC 130: 被围绕的区域（虚拟节点）

```
将被 'X' 围绕的 'O' 全部变成 'X'。
边界上的 'O' 及其连通的 'O' 保留。

思路: 虚拟节点 dummy 与所有边界 'O' union,
     最后非 dummy 组的 'O' → 'X'
```

```cpp
void solve(vector<vector<char>>& board) {
    int m = board.size(), n = board[0].size();
    UnionFind uf(m * n + 1);
    int dummy = m * n;  // 虚拟节点
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] != 'O') continue;
            
            if (i == 0 || i == m-1 || j == 0 || j == n-1)
                uf.unite(i*n+j, dummy);  // 边界O连虚拟节点
            
            // 与相邻O合并
            int dx[] = {1, 0, -1, 0}, dy[] = {0, 1, 0, -1};
            for (int d = 0; d < 4; d++) {
                int ni = i+dx[d], nj = j+dy[d];
                if (ni >= 0 && ni < m && nj >= 0 && nj < n 
                    && board[ni][nj] == 'O')
                    uf.unite(i*n+j, ni*n+nj);
            }
        }
    }
    
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (board[i][j] == 'O' && !uf.connected(i*n+j, dummy))
                board[i][j] = 'X';
}
```

### 7.8 LC 1202: 交换字符串中的元素

```
给 s 和 pairs (可交换的下标对), 求字典序最小的字符串。
同一连通分量内的字符可以任意排列 → 排序取最小。
```

```cpp
string smallestStringWithSwaps(string s, vector<vector<int>>& pairs) {
    int n = s.size();
    UnionFind uf(n);
    for (auto& p : pairs) uf.unite(p[0], p[1]);
    
    // 按连通分量分组
    unordered_map<int, vector<int>> groups;
    for (int i = 0; i < n; i++)
        groups[uf.find(i)].push_back(i);
    
    // 每组内排序字符
    for (auto& [root, indices] : groups) {
        string chars;
        for (int i : indices) chars += s[i];
        sort(chars.begin(), chars.end());
        for (int k = 0; k < (int)indices.size(); k++)
            s[indices[k]] = chars[k];
    }
    return s;
}
```

---

## 8. 并查集 vs 其他方法

### 8.1 并查集 vs DFS/BFS

| 对比 | 并查集 | DFS/BFS |
|------|--------|---------|
| 连通分量计数 | ✅ 适合，动态加边 | ✅ 适合，静态图 |
| 环检测 | ✅ 简洁 | ✅ 也能做 |
| 动态连通性 | ✅ 专长 | ❌ 每次需重新遍历 |
| 最短路径 | ❌ 不支持 | ✅ BFS 适合 |
| 拓扑排序 | ❌ 不支持 | ✅ DFS/BFS |
| 带权关系 | ✅ 带权并查集 | ⚠️ 可以但更复杂 |

### 8.2 选择并查集的信号

```
看到以下关键词考虑并查集:
  "连通" / "连接" / "属于同一组" / "等价"
  "合并" / "分组"
  "冗余连接" / "多余的边"
  "最小生成树"
  "传递关系"（a=b, b=c → a=c）
  "动态加边"
```

---

## 9. 题目总结与易错点

### 9.1 题目分类

#### 连通分量
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 547 | 省份数量 | 中等 | 基础连通分量 |
| 200 | 岛屿数量 | 中等 | 网格+并查集 |
| 323 | 无向图连通分量 | 中等 | 基础 |
| 1202 | 交换字符串中的元素 | 中等 | 分组排序 |

#### 环检测/冗余边
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 684 | 冗余连接 | 中等 | 无向图环检测 |
| 685 | 冗余连接 II | 困难 | 有向图，入度+环 |
| 261 | 以图判树 | 中等 | 连通+无环 |

#### 等价类/合并
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 721 | 账户合并 | 中等 | email → id 映射 |
| 990 | 等式方程 | 中等 | 先合并再检查 |
| 130 | 被围绕的区域 | 中等 | 虚拟节点 |
| 886 | 可能的二分法 | 中等 | 种类并查集 |

#### 带权并查集
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 399 | 除法求值 | 中等 | 比值关系 |
| 839 | 相似字符串组 | 困难 | 判断相似+合并 |

#### 最小生成树 (Kruskal)
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 1135 | 最低成本联通所有城市 | 中等 | Kruskal |
| 1584 | 连接所有点最小费用 | 中等 | Kruskal |

### 9.2 高频易错点

```
❌ 1. 节点编号从 1 开始，数组却从 0 开始
   LC 684 节点从 1 到 n → UnionFind(n+1)
   ✅ 注意题目节点编号范围

❌ 2. 忘记路径压缩
   find(x) 写成 while 循环但没有更新 parent
   → 退化为 O(n)
   ✅ parent[x] = find(parent[x])

❌ 3. unite 没有先 find
   直接 parent[x] = y 而不是 parent[find(x)] = find(y)
   → 连接错误节点
   ✅ 必须用 find 获取根再操作

❌ 4. 二维网格映射到一维
   (i, j) → i * n + j (不是 i * m + j !)
   ✅ 行号 × 列数 + 列号

❌ 5. 连通分量计数忘记减去不参与的节点
   LC 200: 水域 '0' 不参与 → count 需减去 waterCount
   ✅ 仔细处理不参与 union 的元素

❌ 6. 带权并查集路径压缩时忘记更新权重
   find 时 weight[x] 应该乘上递归返回的权重
   ✅ weight[x] *= w

❌ 7. LC 990 先处理 != 导致错误
   必须先处理 == 完成所有合并，再检查 !=
   ✅ 两遍遍历：第一遍 ==，第二遍 !=

❌ 8. 种类并查集下标越界
   元素 x 拆成 x 和 x+n → 数组大小需要 2n 或 3n
   ✅ UnionFind(2*n+1) 或 UnionFind(3*n+1)
```

### 9.3 面试高频问答

> **Q: 并查集的时间复杂度怎么分析？**  
> A: 路径压缩 + 按秩合并后，单次操作均摊 O(α(n))，α 是反阿克曼函数，对所有实际输入 ≤ 4，近似 O(1)。m 次操作总共 O(m·α(n))。

> **Q: 并查集能"拆分"集合吗？**  
> A: 标准并查集不支持。如果需要删边，可以离线逆向处理（倒着加边）。可持久化并查集可以回退但实现复杂。

> **Q: 什么时候用 size 合并，什么时候用 rank 合并？**  
> A: 需要知道集合大小（如最大连通分量）→ 用 size。只关心合并效率 → 用 rank。两者结合路径压缩效果相同。

> **Q: 带权并查集 find 时权重怎么更新？**  
> A: 路径压缩时，`weight[x] *= weight[parent[x]]`（在递归 find 更新 parent 之后）。核心是维护 x 到根的关系链乘积。

---

> 📌 **配套代码**：[knowlege_details_16_union_find.cpp](knowlege_details_16_union_find.cpp) 包含上述所有算法的可运行演示。
