# 专题十二（上）：图论基础 — BFS、DFS与网格搜索

> 作者：大胖超 😜
> 配合 `knowlege_details_12_graph_bfs_dfs.cpp` 食用更佳

---

## 目录

1. [图的基本概念](#1-图的基本概念)
2. [图的存储方式](#2-图的存储方式)
3. [BFS 广度优先搜索](#3-bfs-广度优先搜索)
4. [DFS 深度优先搜索](#4-dfs-深度优先搜索)
5. [BFS vs DFS 选择策略](#5-bfs-vs-dfs-选择策略)
6. [网格搜索（岛屿系列）](#6-网格搜索岛屿系列)
7. [多源 BFS](#7-多源-bfs)
8. [图的连通性](#8-图的连通性)
9. [并查集（Union-Find）](#9-并查集union-find)
10. [综合刷题清单](#10-综合刷题清单)
11. [易错点与总结](#11-易错点与总结)

---

## 1. 图的基本概念

### 图的分类

```
按方向:
  有向图 (Directed Graph)   → 边有方向: u → v
  无向图 (Undirected Graph) → 边无方向: u — v（等价于 u↔v）

按权重:
  无权图 → 边权都为 1
  带权图 → 边有权重 w

按密度:
  稠密图 → 边数接近 n² → 用邻接矩阵
  稀疏图 → 边数远小于 n² → 用邻接表

特殊图:
  DAG (有向无环图)  → 拓扑排序的前提
  树   → 无环连通图，n 个节点 n-1 条边
  二部图 → 节点可分两组，边只在组间
```

### 图的常见术语

| 术语 | 含义 |
|------|------|
| 顶点 (Vertex/Node) | 图中的节点 |
| 边 (Edge) | 连接两个节点的线 |
| 度 (Degree) | 节点连接的边数 |
| 入度 (In-degree) | 有向图中指向该节点的边数 |
| 出度 (Out-degree) | 有向图中从该节点出发的边数 |
| 路径 (Path) | 从一个节点到另一个节点经过的边序列 |
| 环 (Cycle) | 起点和终点相同的路径 |
| 连通分量 | 无向图中互相可达的最大节点集合 |
| 强连通分量 | 有向图中互相可达的最大节点集合 |

---

## 2. 图的存储方式

### 邻接矩阵

```cpp
// 适合稠密图，空间 O(n²)，查询 O(1)
int n = 5;
vector<vector<int>> adj(n, vector<int>(n, 0));
adj[0][1] = 1;  // 0→1 有边
adj[1][0] = 1;  // 无向图要双向
```

```
优点: 查询两点是否相连 O(1)
缺点: 空间 O(n²)，遍历邻居 O(n)
适用: n ≤ 1000 的稠密图
```

### 邻接表

```cpp
// 适合稀疏图，空间 O(n+m)
vector<vector<int>> adj(n);        // 无权
adj[0].push_back(1);               // 0→1
adj[1].push_back(0);               // 无向需双向

vector<vector<pair<int,int>>> adj(n);  // 带权 {邻居, 权重}
adj[0].push_back({1, 5});
```

```
优点: 空间 O(n+m)，遍历邻居高效
缺点: 查询两点是否相连 O(degree)
适用: LeetCode 绝大多数图题
```

### LeetCode 常见的图输入格式

```cpp
// 格式1: 边列表 edges = [[0,1],[1,2],[2,0]]
vector<vector<int>> buildGraph(int n, vector<vector<int>>& edges) {
    vector<vector<int>> adj(n);
    for (auto& e : edges) {
        adj[e[0]].push_back(e[1]);
        adj[e[1]].push_back(e[0]);  // 无向图
    }
    return adj;
}

// 格式2: 邻接矩阵 isConnected[i][j]（如 LC 547）
// 格式3: 网格 grid[i][j]（如 LC 200）
// 格式4: 先修关系 prerequisites = [[1,0],[2,1]]（如 LC 207）
```

---

## 3. BFS 广度优先搜索

### 核心模板

```cpp
// 图的 BFS
void bfs(vector<vector<int>>& adj, int start) {
    int n = adj.size();
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(start);
    visited[start] = true;
    
    while (!q.empty()) {
        int node = q.front(); q.pop();
        // 处理 node
        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
}
```

### BFS 带层次

```cpp
void bfsWithLevel(vector<vector<int>>& adj, int start) {
    vector<bool> visited(adj.size(), false);
    queue<int> q;
    q.push(start);
    visited[start] = true;
    int level = 0;
    
    while (!q.empty()) {
        int size = q.size();          // 当前层的节点数
        for (int i = 0; i < size; i++) {
            int node = q.front(); q.pop();
            // 处理 node，在第 level 层
            for (int nb : adj[node]) {
                if (!visited[nb]) {
                    visited[nb] = true;
                    q.push(nb);
                }
            }
        }
        level++;
    }
}
```

### BFS 求最短路径（无权图）

```
BFS 天然保证"先到的节点距离最短"
→ 无权图的最短路径 = BFS 层数
```

```cpp
int shortestPath(vector<vector<int>>& adj, int src, int dst) {
    vector<int> dist(adj.size(), -1);
    queue<int> q;
    q.push(src);
    dist[src] = 0;
    
    while (!q.empty()) {
        int node = q.front(); q.pop();
        if (node == dst) return dist[dst];
        for (int nb : adj[node]) {
            if (dist[nb] == -1) {
                dist[nb] = dist[node] + 1;
                q.push(nb);
            }
        }
    }
    return -1;  // 不可达
}
```

---

## 4. DFS 深度优先搜索

### 递归模板

```cpp
void dfs(vector<vector<int>>& adj, int node, vector<bool>& visited) {
    visited[node] = true;
    // 处理 node
    for (int nb : adj[node]) {
        if (!visited[nb])
            dfs(adj, nb, visited);
    }
}
```

### 栈模板（避免递归太深导致栈溢出）

```cpp
void dfsIterative(vector<vector<int>>& adj, int start) {
    vector<bool> visited(adj.size(), false);
    stack<int> stk;
    stk.push(start);
    
    while (!stk.empty()) {
        int node = stk.top(); stk.pop();
        if (visited[node]) continue;
        visited[node] = true;
        // 处理 node
        for (int nb : adj[node])
            if (!visited[nb])
                stk.push(nb);
    }
}
```

### DFS 检测环

```cpp
// 有向图检测环（三色标记法）
bool hasCycle(vector<vector<int>>& adj) {
    int n = adj.size();
    // 0=未访问, 1=正在访问(在当前DFS路径上), 2=已完成
    vector<int> color(n, 0);
    
    function<bool(int)> dfs = [&](int u) -> bool {
        color[u] = 1;  // 正在访问
        for (int v : adj[u]) {
            if (color[v] == 1) return true;   // 回边 → 有环
            if (color[v] == 0 && dfs(v)) return true;
        }
        color[u] = 2;  // 完成
        return false;
    };
    
    for (int i = 0; i < n; i++)
        if (color[i] == 0 && dfs(i)) return true;
    return false;
}
```

```
三色标记:
  白色(0) → 未访问
  灰色(1) → 正在当前 DFS 路径上
  黑色(2) → 已完成所有后代的访问

  DFS 过程中遇到灰色节点 → 形成环！
```

---

## 5. BFS vs DFS 选择策略

| 场景 | 推荐 | 理由 |
|------|------|------|
| 最短路径（无权） | BFS | BFS 天然保证最短 |
| 最短路径（带权） | Dijkstra/BFS变体 | 需要优先队列 |
| 层次遍历 | BFS | 逐层扫描 |
| 连通分量个数 | BFS 或 DFS 都行 | 都可以标记访问 |
| 判断环 | DFS（三色法） | 容易检测回边 |
| 拓扑排序 | BFS(Kahn) 或 DFS | 两种都经典 |
| 路径存在性 | DFS | 递归直觉好写 |
| 所有路径枚举 | DFS + 回溯 | 需要枚举 |
| 网格搜索 | BFS 或 DFS | 视是否求最短决定 |
| 二部图判定 | BFS（染色法） | 逐层交替染色 |

---

## 6. 网格搜索（岛屿系列）

网格搜索是图搜索的特殊形式：每个格子有上下左右 4 个邻居。

### 网格 BFS/DFS 通用模板

```cpp
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};

// 检查坐标是否合法
bool inBounds(int x, int y, int m, int n) {
    return x >= 0 && x < m && y >= 0 && y < n;
}
```

### LC 200: 岛屿数量

```cpp
int numIslands(vector<vector<char>>& grid) {
    int m = grid.size(), n = grid[0].size(), count = 0;
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};
    
    function<void(int, int)> dfs = [&](int x, int y) {
        if (x < 0 || x >= m || y < 0 || y >= n || grid[x][y] != '1') return;
        grid[x][y] = '0';  // 标记已访问
        for (int d = 0; d < 4; d++)
            dfs(x + dx[d], y + dy[d]);
    };
    
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (grid[i][j] == '1') { count++; dfs(i, j); }
    return count;
}
```

### LC 695: 岛屿的最大面积

```cpp
int maxAreaOfIsland(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size(), maxArea = 0;
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};
    
    function<int(int, int)> dfs = [&](int x, int y) -> int {
        if (x < 0 || x >= m || y < 0 || y >= n || grid[x][y] != 1) return 0;
        grid[x][y] = 0;
        int area = 1;
        for (int d = 0; d < 4; d++)
            area += dfs(x + dx[d], y + dy[d]);
        return area;
    };
    
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (grid[i][j] == 1)
                maxArea = max(maxArea, dfs(i, j));
    return maxArea;
}
```

### LC 130: 被围绕的区域

**策略**：从边界的 'O' 出发 DFS 标记，剩下的 'O' 就是被包围的

```cpp
void solve(vector<vector<char>>& board) {
    int m = board.size(), n = board[0].size();
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};
    
    function<void(int, int)> dfs = [&](int x, int y) {
        if (x < 0 || x >= m || y < 0 || y >= n || board[x][y] != 'O') return;
        board[x][y] = '#';  // 临时标记为安全
        for (int d = 0; d < 4; d++)
            dfs(x + dx[d], y + dy[d]);
    };
    
    // 从四条边界出发
    for (int i = 0; i < m; i++) { dfs(i, 0); dfs(i, n - 1); }
    for (int j = 0; j < n; j++) { dfs(0, j); dfs(m - 1, j); }
    
    // '#'→'O'(安全), 'O'→'X'(被围)
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'O') board[i][j] = 'X';
            if (board[i][j] == '#') board[i][j] = 'O';
        }
}
```

### LC 417: 太平洋大西洋水流问题

**策略**：反向思维——从海洋边界倒着 DFS，找能流到两个海洋的交集

```cpp
vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
    int m = heights.size(), n = heights[0].size();
    vector<vector<bool>> pacific(m, vector<bool>(n, false));
    vector<vector<bool>> atlantic(m, vector<bool>(n, false));
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};
    
    function<void(int, int, vector<vector<bool>>&)> dfs =
        [&](int x, int y, vector<vector<bool>>& ocean) {
        ocean[x][y] = true;
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < m && ny >= 0 && ny < n
                && !ocean[nx][ny] && heights[nx][ny] >= heights[x][y])
                dfs(nx, ny, ocean);
        }
    };
    
    for (int i = 0; i < m; i++) { dfs(i, 0, pacific); dfs(i, n-1, atlantic); }
    for (int j = 0; j < n; j++) { dfs(0, j, pacific); dfs(m-1, j, atlantic); }
    
    vector<vector<int>> result;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (pacific[i][j] && atlantic[i][j])
                result.push_back({i, j});
    return result;
}
```

---

## 7. 多源 BFS

### 原理

普通 BFS 从一个源点出发；多源 BFS 从**多个源点同时出发**，像波纹一样向外扩散。

```
初始化：所有源点同时入队
扩散：每一轮所有队列中的节点同时向外扩展一层
```

### LC 994: 腐烂的橘子

```cpp
int orangesRotting(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    queue<pair<int,int>> q;
    int fresh = 0;
    
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == 2) q.push({i, j});
            else if (grid[i][j] == 1) fresh++;
        }
    if (fresh == 0) return 0;
    
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};
    int minutes = 0;
    while (!q.empty()) {
        int size = q.size();
        bool rotted = false;
        for (int k = 0; k < size; k++) {
            auto [x, y] = q.front(); q.pop();
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (nx >= 0 && nx < m && ny >= 0 && ny < n && grid[nx][ny] == 1) {
                    grid[nx][ny] = 2;
                    q.push({nx, ny});
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

### LC 542: 01 矩阵（每个 1 到最近 0 的距离）

```cpp
vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    int m = mat.size(), n = mat[0].size();
    vector<vector<int>> dist(m, vector<int>(n, INT_MAX));
    queue<pair<int,int>> q;
    
    // 所有 0 作为源点
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (mat[i][j] == 0) { dist[i][j] = 0; q.push({i, j}); }
    
    int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < m && ny >= 0 && ny < n
                && dist[nx][ny] > dist[x][y] + 1) {
                dist[nx][ny] = dist[x][y] + 1;
                q.push({nx, ny});
            }
        }
    }
    return dist;
}
```

---

## 8. 图的连通性

### LC 547: 省份数量

```cpp
int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size(), count = 0;
    vector<bool> visited(n, false);
    
    function<void(int)> dfs = [&](int i) {
        visited[i] = true;
        for (int j = 0; j < n; j++)
            if (isConnected[i][j] == 1 && !visited[j])
                dfs(j);
    };
    
    for (int i = 0; i < n; i++)
        if (!visited[i]) { dfs(i); count++; }
    return count;
}
```

### LC 785: 判断二部图

**策略**：BFS 染色，相邻节点颜色不同

```cpp
bool isBipartite(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> color(n, -1);  // -1=未染色, 0=颜色A, 1=颜色B
    
    for (int i = 0; i < n; i++) {
        if (color[i] != -1) continue;
        queue<int> q;
        q.push(i);
        color[i] = 0;
        while (!q.empty()) {
            int node = q.front(); q.pop();
            for (int nb : graph[node]) {
                if (color[nb] == -1) {
                    color[nb] = 1 - color[node];
                    q.push(nb);
                } else if (color[nb] == color[node]) {
                    return false;  // 相邻同色 → 非二部图
                }
            }
        }
    }
    return true;
}
```

---

## 9. 并查集（Union-Find）

### 核心思想

并查集用于处理**动态连通性**问题——合并集合、查询是否在同一集合。

### 模板

```cpp
class UnionFind {
    vector<int> parent, rank_;
public:
    UnionFind(int n) : parent(n), rank_(n, 0) {
        iota(parent.begin(), parent.end(), 0);  // parent[i] = i
    }
    
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);  // 路径压缩
        return parent[x];
    }
    
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;  // 已在同一集合
        // 按秩合并
        if (rank_[px] < rank_[py]) swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        return true;
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};
```

### 关键优化

```
路径压缩 (Path Compression):
  find 时让节点直接指向根 → 树变扁
  → 后续 find 接近 O(1)

按秩合并 (Union by Rank):
  小树挂到大树上 → 树不会太深

两者结合: 均摊 O(α(n)) ≈ O(1)
```

### LC 547: 省份数量（并查集版）

```cpp
int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    int components = n;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (isConnected[i][j] && uf.unite(i, j))
                components--;
    return components;
}
```

### LC 684: 冗余连接（找多余的边让树变成图）

```cpp
vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    int n = edges.size();
    UnionFind uf(n + 1);
    for (auto& e : edges)
        if (!uf.unite(e[0], e[1]))
            return e;  // 合并失败 → 两端已连通 → 这条边多余
    return {};
}
```

### LC 399: 除法求值（带权并查集）

```
a/b=2.0, b/c=3.0
→ 构建关系图: a→b 权2.0, b→c 权3.0
→ a/c = a/b * b/c = 6.0
```

### 并查集适用场景

| 场景 | 说明 |
|------|------|
| 连通分量计数 | 边动态加入时维护连通性 |
| 冗余连接检测 | 加边时两端已连通→多余 |
| 最小生成树 (Kruskal) | 排序边，并查集判断是否成环 |
| 账户合并 | 同一邮箱的账户合并 |

---

## 10. 综合刷题清单

### 网格 BFS/DFS

| 序号 | 题目 | 难度 | 方法 |
|------|------|------|------|
| 1 | LC 200: 岛屿数量 | Medium | DFS/BFS |
| 2 | LC 695: 岛屿最大面积 | Medium | DFS |
| 3 | LC 130: 被围绕的区域 | Medium | 边界 DFS |
| 4 | LC 417: 太平洋大西洋 | Medium | 反向 DFS |
| 5 | LC 994: 腐烂的橘子 | Medium | 多源 BFS |
| 6 | LC 542: 01 矩阵 | Medium | 多源 BFS |
| 7 | LC 733: 图像渲染 | Easy | FloodFill DFS |

### 图遍历 & 连通性

| 序号 | 题目 | 难度 | 方法 |
|------|------|------|------|
| 8 | LC 547: 省份数量 | Medium | DFS/并查集 |
| 9 | LC 785: 判断二部图 | Medium | BFS 染色 |
| 10 | LC 684: 冗余连接 | Medium | 并查集 |
| 11 | LC 399: 除法求值 | Medium | 带权并查集/BFS |
| 12 | LC 841: 钥匙和房间 | Medium | DFS/BFS |

### 推荐刷题顺序

```
网格入门:
  岛屿数量(200) → 最大面积(695) → 图像渲染(733)

网格进阶:
  被包围区域(130) → 太平洋大西洋(417)

多源BFS:
  腐烂橘子(994) → 01矩阵(542)

图的连通性:
  省份数量(547) → 二部图(785) → 钥匙和房间(841)

并查集:
  省份数量(547并查集版) → 冗余连接(684)
```

---

## 11. 易错点与总结

### 常见错误

| 错误 | 正确做法 |
|------|---------|
| BFS 入队前不标记访问 | 入队时就标记，否则重复入队 |
| DFS 网格越界没检查 | 每次递归前检查边界 |
| 网格搜索忘记标记已访问 | 修改原数组或用 visited |
| 并查集 find 没路径压缩 | 一定要用路径压缩 |
| 有向图用无向图的方法判环 | 有向图用三色标记 |

### BFS 入队标记 vs 出队标记

```
❌ 出队时标记:
  节点可能被多次入队 → 浪费时间 + 可能出错

✅ 入队时标记:
  每个节点只入队一次 → O(V+E)
```

### 图搜索思维清单

```
✅ 是网格问题还是图问题？
✅ 有向还是无向？
✅ 求最短路径？ → BFS（无权）/ Dijkstra（带权）
✅ 求连通分量？ → DFS/BFS/并查集
✅ 有没有环？ → DFS 三色标记
✅ 需要拓扑排序？ → 有向无环图 DAG
✅ 多个起点？ → 多源 BFS
```
