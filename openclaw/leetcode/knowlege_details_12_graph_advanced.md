# 专题十二（下）：图论进阶 — 拓扑排序、最短路径与高级算法

> 作者：大胖超 😜
> 配合 `knowlege_details_12_graph_advanced.cpp` 食用更佳

---

## 目录

1. [拓扑排序](#1-拓扑排序)
2. [最短路径算法总览](#2-最短路径算法总览)
3. [Dijkstra 算法](#3-dijkstra-算法)
4. [Bellman-Ford 算法](#4-bellman-ford-算法)
5. [Floyd-Warshall 全源最短路](#5-floyd-warshall-全源最短路)
6. [最小生成树](#6-最小生成树)
7. [图的高级搜索](#7-图的高级搜索)
8. [综合刷题清单](#8-综合刷题清单)
9. [易错点与总结](#9-易错点与总结)

---

## 1. 拓扑排序

### 核心概念

```
拓扑排序: 将 DAG（有向无环图）的节点排成线性序列，
         使得每条边 u→v 中 u 在 v 前面。

前提: 图必须是 DAG（有环则无拓扑排序）
应用: 课程安排、编译依赖、任务调度
```

### 方法一：BFS（Kahn 算法）

```
1. 计算所有节点的入度
2. 入度为 0 的节点入队
3. 每取出一个节点，将其邻居的入度 -1
4. 入度变为 0 的邻居入队
5. 如果最终取出的节点数 == 总节点数 → 无环，排序成功
```

```cpp
vector<int> topoSortBFS(int n, vector<vector<int>>& adj) {
    vector<int> inDegree(n, 0);
    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            inDegree[v]++;
    
    queue<int> q;
    for (int i = 0; i < n; i++)
        if (inDegree[i] == 0) q.push(i);
    
    vector<int> order;
    while (!q.empty()) {
        int node = q.front(); q.pop();
        order.push_back(node);
        for (int nb : adj[node])
            if (--inDegree[nb] == 0)
                q.push(nb);
    }
    
    // order.size() == n 说明无环
    return order.size() == n ? order : vector<int>{};
}
```

### 方法二：DFS（后序逆序）

```
DFS 后序遍历：先递归完所有后继再记录自己
然后逆序就是拓扑排序
```

```cpp
vector<int> topoSortDFS(int n, vector<vector<int>>& adj) {
    vector<int> color(n, 0);  // 三色标记
    vector<int> order;
    bool hasCycle = false;
    
    function<void(int)> dfs = [&](int u) {
        color[u] = 1;
        for (int v : adj[u]) {
            if (color[v] == 1) { hasCycle = true; return; }
            if (color[v] == 0) dfs(v);
            if (hasCycle) return;
        }
        color[u] = 2;
        order.push_back(u);  // 后序
    };
    
    for (int i = 0; i < n; i++)
        if (color[i] == 0) dfs(i);
    
    if (hasCycle) return {};
    reverse(order.begin(), order.end());  // 逆后序 = 拓扑序
    return order;
}
```

### LC 207: 课程表（判断是否有环）

```cpp
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> inDegree(numCourses, 0);
    for (auto& p : prerequisites) {
        adj[p[1]].push_back(p[0]);
        inDegree[p[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (inDegree[i] == 0) q.push(i);
    
    int count = 0;
    while (!q.empty()) {
        int c = q.front(); q.pop();
        count++;
        for (int nb : adj[c])
            if (--inDegree[nb] == 0) q.push(nb);
    }
    return count == numCourses;
}
```

### LC 210: 课程表 II（输出拓扑排序）

```cpp
vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> inDegree(numCourses, 0);
    for (auto& p : prerequisites) {
        adj[p[1]].push_back(p[0]);
        inDegree[p[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (inDegree[i] == 0) q.push(i);
    
    vector<int> order;
    while (!q.empty()) {
        int c = q.front(); q.pop();
        order.push_back(c);
        for (int nb : adj[c])
            if (--inDegree[nb] == 0) q.push(nb);
    }
    return order.size() == numCourses ? order : vector<int>{};
}
```

### 拓扑排序经典应用

| 问题 | 本质 |
|------|------|
| 课程安排 | 有向依赖关系的线性化 |
| 编译顺序 | 模块间的依赖 |
| 项目管理 | 任务的先后关系 |
| 字典序 | 外星语言排序（LC 269） |

---

## 2. 最短路径算法总览

| 算法 | 适用场景 | 时间复杂度 | 负权边 |
|------|---------|-----------|--------|
| BFS | 无权图 | O(V+E) | — |
| Dijkstra | 非负权图 | O((V+E)logV) | ❌ |
| Bellman-Ford | 有负权边 | O(VE) | ✅ |
| Floyd-Warshall | 全源最短路 | O(V³) | ✅ |
| SPFA | 有负权边（优化BF） | 平均O(E) 最坏O(VE) | ✅ |

### 选择策略

```
无权图 → BFS   (最简单)
单源、非负权 → Dijkstra  (最常用)
单源、有负权 → Bellman-Ford
全源最短路 → Floyd
LeetCode 限定 K 步 → Bellman-Ford
```

---

## 3. Dijkstra 算法

### 核心思想

```
贪心: 每次选择"距离源点最近的未确定节点"，
     用它来松弛（relax）它的邻居。

松弛操作:
  if dist[u] + w(u,v) < dist[v]:
      dist[v] = dist[u] + w(u,v)
```

### 标准模板（最小堆优化）

```cpp
vector<int> dijkstra(vector<vector<pair<int,int>>>& adj, int src) {
    int n = adj.size();
    vector<int> dist(n, INT_MAX);
    dist[src] = 0;
    
    // {距离, 节点}
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    pq.push({0, src});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;  // 已有更短路径，跳过
        
        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}
```

### 为什么不能处理负权边？

```
Dijkstra 假设: 一旦节点出队，其最短距离已确定。
负权边会打破这个假设:

  A --1--> B --(-3)--> C
  A --2--> C

  Dijkstra: dist[C] = 2 (直接)
  但实际: dist[C] = 1 + (-3) = -2 (经B)

  当 C 出队时 dist=2 已确定，
  但后来发现经 B 更短 → 错误!
```

### LC 743: 网络延迟时间

```cpp
int networkDelayTime(vector<vector<int>>& times, int n, int k) {
    vector<vector<pair<int,int>>> adj(n + 1);
    for (auto& t : times)
        adj[t[0]].push_back({t[1], t[2]});
    
    vector<int> dist(n + 1, INT_MAX);
    dist[k] = 0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    pq.push({0, k});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    
    int maxDist = *max_element(dist.begin() + 1, dist.end());
    return maxDist == INT_MAX ? -1 : maxDist;
}
```

### LC 1514: 概率最大的路径

**变体**：边权是概率（0~1），求最大概率（最大积路径）→ 改 max 堆

```cpp
double maxProbability(int n, vector<vector<int>>& edges,
                      vector<double>& succProb, int start, int end) {
    vector<vector<pair<int,double>>> adj(n);
    for (int i = 0; i < (int)edges.size(); i++) {
        adj[edges[i][0]].push_back({edges[i][1], succProb[i]});
        adj[edges[i][1]].push_back({edges[i][0], succProb[i]});
    }
    
    vector<double> prob(n, 0.0);
    prob[start] = 1.0;
    priority_queue<pair<double,int>> pq;  // max heap
    pq.push({1.0, start});
    
    while (!pq.empty()) {
        auto [p, u] = pq.top(); pq.pop();
        if (p < prob[u]) continue;
        for (auto [v, w] : adj[u]) {
            if (prob[u] * w > prob[v]) {
                prob[v] = prob[u] * w;
                pq.push({prob[v], v});
            }
        }
    }
    return prob[end];
}
```

---

## 4. Bellman-Ford 算法

### 核心思想

```
对所有边进行 V-1 轮松弛。
每轮至少确定一个节点的最短路径。
V-1 轮后如果还能松弛 → 存在负环。
```

### 标准模板

```cpp
vector<int> bellmanFord(int n, vector<vector<int>>& edges, int src) {
    // edges[i] = {from, to, weight}
    vector<int> dist(n, INT_MAX);
    dist[src] = 0;
    
    for (int i = 0; i < n - 1; i++) {    // V-1 轮
        for (auto& e : edges) {           // 遍历所有边
            int u = e[0], v = e[1], w = e[2];
            if (dist[u] != INT_MAX && dist[u] + w < dist[v])
                dist[v] = dist[u] + w;
        }
    }
    
    // 检测负环
    for (auto& e : edges) {
        int u = e[0], v = e[1], w = e[2];
        if (dist[u] != INT_MAX && dist[u] + w < dist[v])
            return {};  // 有负环
    }
    return dist;
}
```

### LC 787: K 站中转内最便宜的航班

**关键**：最多经过 K 个中转站 = 最多走 K+1 条边 → BF 只松弛 K+1 轮

```cpp
int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
    vector<int> dist(n, INT_MAX);
    dist[src] = 0;
    
    for (int i = 0; i <= k; i++) {         // 最多 k+1 轮
        vector<int> temp(dist);            // 重要:用上一轮的值
        for (auto& f : flights) {
            int u = f[0], v = f[1], w = f[2];
            if (dist[u] != INT_MAX && dist[u] + w < temp[v])
                temp[v] = dist[u] + w;
        }
        dist = temp;
    }
    return dist[dst] == INT_MAX ? -1 : dist[dst];
}
```

> **注意**：这里每轮用 `temp` 是为了防止同一轮中的连锁更新（限制步数时必须这样）

---

## 5. Floyd-Warshall 全源最短路

### 核心思想

```
dp[i][j] = i 到 j 的最短路径
考虑中间节点 k:
  dp[i][j] = min(dp[i][j], dp[i][k] + dp[k][j])

三层循环:外层枚举中间点 k, 内层枚举起终点 i,j
```

### 标准模板

```cpp
vector<vector<int>> floyd(vector<vector<int>>& dist) {
    int n = dist.size();
    // dist[i][j] 初始为边权，无直接边则为 INF，dist[i][i] = 0
    
    for (int k = 0; k < n; k++)             // 中间点
        for (int i = 0; i < n; i++)          // 起点
            for (int j = 0; j < n; j++)      // 终点
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
    
    return dist;
}
```

### LC 1334: 阈值距离内邻居最少的城市

```cpp
int findTheCity(int n, vector<vector<int>>& edges, int distanceThreshold) {
    vector<vector<int>> dist(n, vector<int>(n, INT_MAX / 2));
    for (int i = 0; i < n; i++) dist[i][i] = 0;
    for (auto& e : edges) {
        dist[e[0]][e[1]] = e[2];
        dist[e[1]][e[0]] = e[2];
    }
    
    // Floyd
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
    
    int minCount = n, result = -1;
    for (int i = 0; i < n; i++) {
        int count = 0;
        for (int j = 0; j < n; j++)
            if (i != j && dist[i][j] <= distanceThreshold) count++;
        if (count <= minCount) {
            minCount = count;
            result = i;
        }
    }
    return result;
}
```

---

## 6. 最小生成树

### 概念

```
给定一个带权无向连通图，找一棵包含所有节点、
边权总和最小的生成树。

两种经典算法:
  Kruskal: 排序 + 并查集（适合稀疏图）
  Prim: 贪心 + 优先队列（适合稠密图）
```

### Kruskal 算法

```
1. 将所有边按权重升序排序
2. 从小到大依次选边
3. 如果这条边连接的两个节点不在同一集合 → 选中
4. 用并查集判断是否在同一集合
5. 选了 n-1 条边后停止
```

```cpp
int kruskal(int n, vector<vector<int>>& edges) {
    // edges[i] = {u, v, weight}
    sort(edges.begin(), edges.end(),
         [](auto& a, auto& b) { return a[2] < b[2]; });
    
    UnionFind uf(n);  // 需要并查集
    int totalWeight = 0, edgeCount = 0;
    
    for (auto& e : edges) {
        if (uf.unite(e[0], e[1])) {
            totalWeight += e[2];
            edgeCount++;
            if (edgeCount == n - 1) break;
        }
    }
    
    return edgeCount == n - 1 ? totalWeight : -1;  // -1=不连通
}
```

### LC 1584: 连接所有点的最小费用

```cpp
int minCostConnectPoints(vector<vector<int>>& points) {
    int n = points.size();
    vector<vector<int>> edges;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            int dist = abs(points[i][0] - points[j][0])
                     + abs(points[i][1] - points[j][1]);
            edges.push_back({i, j, dist});
        }
    return kruskal(n, edges);  // 使用上面的 Kruskal
}
```

---

## 7. 图的高级搜索

### LC 797: 所有可能的路径（DAG 中 0 到 n-1 的所有路径）

```cpp
vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
    vector<vector<int>> result;
    vector<int> path = {0};
    int target = graph.size() - 1;
    
    function<void(int)> dfs = [&](int node) {
        if (node == target) {
            result.push_back(path);
            return;
        }
        for (int nb : graph[node]) {
            path.push_back(nb);
            dfs(nb);
            path.pop_back();
        }
    };
    
    dfs(0);
    return result;
}
```

### LC 841: 钥匙和房间

```cpp
bool canVisitAllRooms(vector<vector<int>>& rooms) {
    int n = rooms.size();
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(0);
    visited[0] = true;
    int count = 1;
    
    while (!q.empty()) {
        int room = q.front(); q.pop();
        for (int key : rooms[room]) {
            if (!visited[key]) {
                visited[key] = true;
                count++;
                q.push(key);
            }
        }
    }
    return count == n;
}
```

---

## 8. 综合刷题清单

### 拓扑排序

| 序号 | 题目 | 难度 | 要点 |
|------|------|------|------|
| 1 | LC 207: 课程表 | Medium | 判有向图是否有环 |
| 2 | LC 210: 课程表 II | Medium | 输出拓扑序 |
| 3 | LC 802: 找安全节点 | Medium | 反向图 + 拓扑 |

### 最短路径

| 序号 | 题目 | 难度 | 算法 |
|------|------|------|------|
| 4 | LC 743: 网络延迟 | Medium | Dijkstra |
| 5 | LC 787: K站中转最便宜 | Medium | Bellman-Ford |
| 6 | LC 1514: 最大概率路径 | Medium | Dijkstra 变体 |
| 7 | LC 1334: 阈值距离邻居 | Medium | Floyd |

### 图搜索

| 序号 | 题目 | 难度 | 方法 |
|------|------|------|------|
| 8 | LC 797: 所有可能路径 | Medium | DFS 回溯 |
| 9 | LC 841: 钥匙和房间 | Medium | BFS/DFS |
| 10 | LC 1584: 最小费用连接 | Medium | MST Kruskal |

### 推荐刷题顺序

```
拓扑排序:
  课程表(207) → 课程表II(210) → 安全节点(802)

最短路径:
  网络延迟(743) → K站最便宜(787) → 最大概率(1514)

全源最短路:
  阈值距离(1334)

图搜索与MST:
  所有路径(797) → 钥匙房间(841) → 最小费用连接(1584)
```

---

## 9. 易错点与总结

### 最短路径算法对比

| 特性 | BFS | Dijkstra | Bellman-Ford | Floyd |
|------|-----|----------|-------------|-------|
| 权重要求 | 无权 | 非负 | 任意 | 任意 |
| 源点 | 单源 | 单源 | 单源 | 全源 |
| 检测负环 | — | — | ✅ | ✅ |
| 步数限制 | ✅ | ❌ | ✅(K轮) | ❌ |

### 常见错误

| 错误 | 正确做法 |
|------|---------|
| Dijkstra 用于负权图 | 改用 Bellman-Ford |
| BF 限制步数时连锁更新 | 每轮用 temp 数组 |
| Floyd 的 k 放内层 | k 必须在最外层 |
| 拓扑排序用于有环图 | 先判环 |
| Kruskal 忘记并查集路径压缩 | 路径压缩+按秩合并 |

### 图论思维清单

```
✅ 有向还是无向？
✅ 有权还是无权？有没有负权？
✅ 求单源还是全源最短路径？
✅ 有步数/中转站限制？ → Bellman-Ford
✅ 需要判断环？ → 拓扑排序 / DFS三色
✅ 需要连通性？ → BFS/DFS/并查集
✅ 需要最小生成树？ → Kruskal/Prim
```
