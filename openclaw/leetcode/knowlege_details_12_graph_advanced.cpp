/**
 * 专题十二（下）：图论进阶 — 拓扑排序、最短路径与高级算法 示例代码
 * 作者：大胖超 😜
 *
 * 编译: g++ -std=c++17 -o knowlege_details_12_graph_advanced knowlege_details_12_graph_advanced.cpp
 * 运行: ./knowlege_details_12_graph_advanced
 *
 * 覆盖题目：
 *   Demo1: 拓扑排序 BFS Kahn（LC 207 课程表 + LC 210 课程表II）
 *   Demo2: 拓扑排序 DFS（后序逆序法）
 *   Demo3: Dijkstra 最短路径（LC 743 网络延迟）
 *   Demo4: Bellman-Ford（LC 787 K站中转最便宜航班）
 *   Demo5: Floyd-Warshall 全源最短路（LC 1334 阈值距离邻居）
 *   Demo6: 最小生成树 Kruskal（LC 1584 最小费用连接点）
 *   Demo7: 图搜索进阶（LC 797 所有路径 + LC 841 钥匙和房间）
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>
#include <numeric>
#include <climits>
using namespace std;

// ==================== 并查集模板 ====================
class UnionFind {
    vector<int> parent, rank_;
public:
    int components;
    UnionFind(int n) : parent(n), rank_(n, 0), components(n) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        if (rank_[px] < rank_[py]) swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        components--;
        return true;
    }
};

// ==================== Demo1: 拓扑排序 BFS ====================
void demo1_topoSortBFS() {
    cout << "===== Demo1: 拓扑排序 BFS (Kahn) =====\n\n";

    // --- LC 207: 课程表 ---
    {
        cout << "--- LC 207: 课程表 ---\n";
        auto canFinish = [](int numCourses, vector<vector<int>>& prerequisites) -> bool {
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
        };

        vector<vector<int>> pre1 = {{1,0}};
        vector<vector<int>> pre2 = {{1,0},{0,1}};
        cout << "2门课, 先修[[1,0]] → " << (canFinish(2, pre1) ? "可完成" : "不可完成") << "\n";
        cout << "2门课, 先修[[1,0],[0,1]] → " << (canFinish(2, pre2) ? "可完成" : "不可完成(有环)") << "\n\n";
    }

    // --- LC 210: 课程表 II ---
    {
        cout << "--- LC 210: 课程表 II (输出拓扑序) ---\n";
        auto findOrder = [](int numCourses, vector<vector<int>>& prerequisites) -> vector<int> {
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
        };

        // 4门课: 0→1, 0→2, 1→3, 2→3
        vector<vector<int>> pre = {{1,0},{2,0},{3,1},{3,2}};
        cout << "4门课, 先修[[1,0],[2,0],[3,1],[3,2]]\n";
        cout << "  依赖图: 0 → 1 → 3\n";
        cout << "          0 → 2 → 3\n";

        auto order = findOrder(4, pre);
        cout << "  拓扑序: ";
        for (int c : order) cout << c << " ";
        cout << "\n";

        // 追踪过程
        {
            vector<vector<int>> adj(4);
            vector<int> inDeg(4, 0);
            for (auto& p : pre) { adj[p[1]].push_back(p[0]); inDeg[p[0]]++; }
            cout << "  入度: ";
            for (int i = 0; i < 4; i++) cout << i << "→" << inDeg[i] << " ";
            cout << "\n";

            queue<int> q;
            for (int i = 0; i < 4; i++) if (inDeg[i] == 0) q.push(i);
            cout << "  初始队列(入度0): ";
            queue<int> temp = q;
            while (!temp.empty()) { cout << temp.front() << " "; temp.pop(); }
            cout << "\n";

            int step = 0;
            while (!q.empty()) {
                int c = q.front(); q.pop();
                step++;
                cout << "  步骤" << step << ": 取出课程" << c;
                vector<int> newReady;
                for (int nb : adj[c])
                    if (--inDeg[nb] == 0) newReady.push_back(nb);
                if (!newReady.empty()) {
                    cout << ", 新增入度0: ";
                    for (int x : newReady) { cout << x << " "; q.push(x); }
                }
                cout << "\n";
            }
        }
        cout << "\n";
    }
}

// ==================== Demo2: 拓扑排序 DFS ====================
void demo2_topoSortDFS() {
    cout << "===== Demo2: 拓扑排序 DFS (后序逆序) =====\n\n";

    // 图: 0→1, 0→2, 1→3, 2→3
    int n = 4;
    vector<vector<int>> adj = {{1, 2}, {3}, {3}, {}};
    vector<int> color(n, 0);
    vector<int> order;
    bool hasCycle = false;

    function<void(int)> dfs = [&](int u) {
        color[u] = 1;
        cout << "  进入 " << u << " (灰)\n";
        for (int v : adj[u]) {
            if (color[v] == 1) { hasCycle = true; return; }
            if (color[v] == 0) dfs(v);
            if (hasCycle) return;
        }
        color[u] = 2;
        order.push_back(u);
        cout << "  完成 " << u << " (黑) → 后序记录\n";
    };

    cout << "图: 0→{1,2}, 1→{3}, 2→{3}, 3→{}\n";
    cout << "DFS 过程:\n";
    for (int i = 0; i < n; i++)
        if (color[i] == 0) dfs(i);

    reverse(order.begin(), order.end());
    cout << "后序: ";
    for (int i = order.size() - 1; i >= 0; i--) cout << order[i] << " ";
    cout << "\n逆后序(拓扑序): ";
    for (int x : order) cout << x << " ";
    cout << "\n\n";
}

// ==================== Demo3: Dijkstra ====================
void demo3_dijkstra() {
    cout << "===== Demo3: Dijkstra 最短路径 =====\n\n";

    // --- 经典 Dijkstra 演示 ---
    {
        cout << "--- 经典 Dijkstra ---\n";
        // 图:
        //   0 --2-- 1 --3-- 3
        //   |       |       |
        //   4       1       1
        //   |       |       |
        //   2 --5-- 4 ------+
        int n = 5;
        vector<vector<pair<int,int>>> adj(n);
        auto addEdge = [&](int u, int v, int w) {
            adj[u].push_back({v, w});
            adj[v].push_back({u, w});
        };
        addEdge(0, 1, 2);
        addEdge(0, 2, 4);
        addEdge(1, 3, 3);
        addEdge(1, 4, 1);
        addEdge(2, 4, 5);
        addEdge(3, 4, 1);

        cout << "图的边: 0-1(2), 0-2(4), 1-3(3), 1-4(1), 2-4(5), 3-4(1)\n";

        int src = 0;
        vector<int> dist(n, INT_MAX);
        dist[src] = 0;
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
        pq.push({0, src});

        cout << "从节点 " << src << " 出发:\n";
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            cout << "  处理节点 " << u << " (dist=" << d << ")\n";
            for (auto [v, w] : adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                    cout << "    松弛 " << u << "→" << v
                         << ": dist[" << v << "]=" << dist[v] << "\n";
                }
            }
        }
        cout << "最短距离: ";
        for (int i = 0; i < n; i++) cout << src << "→" << i << "=" << dist[i] << " ";
        cout << "\n\n";
    }

    // --- LC 743: 网络延迟时间 ---
    {
        cout << "--- LC 743: 网络延迟时间 ---\n";
        auto networkDelayTime = [](vector<vector<int>>& times, int n, int k) -> int {
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
        };

        vector<vector<int>> times = {{2,1,1},{2,3,1},{3,4,1}};
        int n = 4, k = 2;
        cout << "times=[[2,1,1],[2,3,1],[3,4,1]], n=4, k=2\n";
        cout << "  2→1(1), 2→3(1), 3→4(1)\n";
        cout << "  网络延迟: " << networkDelayTime(times, n, k) << "\n\n";
    }
}

// ==================== Demo4: Bellman-Ford ====================
void demo4_bellmanFord() {
    cout << "===== Demo4: Bellman-Ford =====\n\n";

    // --- 经典 Bellman-Ford ---
    {
        cout << "--- 经典 Bellman-Ford ---\n";
        int n = 4;
        // edges: {from, to, weight}
        vector<vector<int>> edges = {{0,1,1},{0,2,4},{1,2,2},{1,3,6},{2,3,3}};

        cout << "4个节点, 边: 0→1(1), 0→2(4), 1→2(2), 1→3(6), 2→3(3)\n";

        vector<int> dist(n, INT_MAX);
        dist[0] = 0;

        for (int round = 0; round < n - 1; round++) {
            cout << "  第" << round + 1 << "轮松弛: ";
            bool updated = false;
            for (auto& e : edges) {
                int u = e[0], v = e[1], w = e[2];
                if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    cout << u << "→" << v << "(dist=" << dist[v] << ") ";
                    updated = true;
                }
            }
            if (!updated) cout << "无更新,提前终止";
            cout << "\n";
            if (!updated) break;
        }
        cout << "最短距离: ";
        for (int i = 0; i < n; i++) cout << "0→" << i << "=" << dist[i] << " ";
        cout << "\n\n";
    }

    // --- LC 787: K 站中转内最便宜的航班 ---
    {
        cout << "--- LC 787: K站中转最便宜航班 ---\n";
        auto findCheapestPrice = [](int n, vector<vector<int>>& flights,
                                     int src, int dst, int k) -> int {
            vector<int> dist(n, INT_MAX);
            dist[src] = 0;
            for (int i = 0; i <= k; i++) {
                vector<int> temp(dist);
                for (auto& f : flights) {
                    int u = f[0], v = f[1], w = f[2];
                    if (dist[u] != INT_MAX && dist[u] + w < temp[v])
                        temp[v] = dist[u] + w;
                }
                dist = temp;
            }
            return dist[dst] == INT_MAX ? -1 : dist[dst];
        };

        int n = 4;
        vector<vector<int>> flights = {{0,1,100},{1,2,100},{2,0,100},{1,3,600},{2,3,200}};
        cout << "4城市, 航线:\n";
        for (auto& f : flights)
            cout << "  " << f[0] << "→" << f[1] << " 费用" << f[2] << "\n";

        cout << "src=0, dst=3, k=1(最多1次中转):\n";
        cout << "  最便宜: " << findCheapestPrice(n, flights, 0, 3, 1) << "\n";
        cout << "  路径: 0→1→3 = 100+600 = 700\n";

        cout << "src=0, dst=3, k=2(最多2次中转):\n";
        cout << "  最便宜: " << findCheapestPrice(n, flights, 0, 3, 2) << "\n";
        cout << "  路径: 0→1→2→3 = 100+100+200 = 400\n\n";
    }
}

// ==================== Demo5: Floyd-Warshall ====================
void demo5_floyd() {
    cout << "===== Demo5: Floyd-Warshall 全源最短路 =====\n\n";

    int n = 4;
    const int INF = 1e8;
    vector<vector<int>> dist = {
        {0, 3, INF, 7},
        {8, 0, 2, INF},
        {5, INF, 0, 1},
        {2, INF, INF, 0}
    };

    cout << "初始距离矩阵:\n";
    auto printMatrix = [&](vector<vector<int>>& d) {
        for (int i = 0; i < n; i++) {
            cout << "  ";
            for (int j = 0; j < n; j++) {
                if (d[i][j] >= INF) cout << "INF\t";
                else cout << d[i][j] << "\t";
            }
            cout << "\n";
        }
    };
    printMatrix(dist);

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k] < INF && dist[k][j] < INF)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
        cout << "\n中间点 k=" << k << " 后:\n";
        printMatrix(dist);
    }

    cout << "\n部分最短路径:\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i != j)
                cout << "  " << i << "→" << j << " = " << dist[i][j] << "\n";
    cout << "\n";
}

// ==================== Demo6: 最小生成树 Kruskal ====================
void demo6_kruskal() {
    cout << "===== Demo6: 最小生成树 Kruskal =====\n\n";

    // --- 经典 Kruskal ---
    {
        cout << "--- 经典 Kruskal ---\n";
        int n = 5;
        vector<vector<int>> edges = {
            {0,1,2}, {0,3,6}, {1,2,3}, {1,3,8}, {1,4,5},
            {2,4,7}, {3,4,9}
        };

        sort(edges.begin(), edges.end(),
             [](auto& a, auto& b) { return a[2] < b[2]; });

        cout << "排序后的边:\n";
        for (auto& e : edges)
            cout << "  " << e[0] << "-" << e[1] << " 权重" << e[2] << "\n";

        UnionFind uf(n);
        int total = 0, edgeCnt = 0;
        cout << "\n选边过程:\n";
        for (auto& e : edges) {
            if (uf.unite(e[0], e[1])) {
                total += e[2];
                edgeCnt++;
                cout << "  ✓ 选边 " << e[0] << "-" << e[1]
                     << " 权重" << e[2] << " (累计" << total << ")\n";
                if (edgeCnt == n - 1) break;
            } else {
                cout << "  ✗ 跳过 " << e[0] << "-" << e[1]
                     << " (会成环)\n";
            }
        }
        cout << "MST 总权重: " << total << "\n\n";
    }

    // --- LC 1584: 连接所有点的最小费用 ---
    {
        cout << "--- LC 1584: 连接所有点的最小费用 ---\n";
        vector<vector<int>> points = {{0,0},{2,2},{3,10},{5,2},{7,0}};
        int n = points.size();

        vector<vector<int>> edges;
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) {
                int dist = abs(points[i][0] - points[j][0])
                         + abs(points[i][1] - points[j][1]);
                edges.push_back({i, j, dist});
            }

        sort(edges.begin(), edges.end(),
             [](auto& a, auto& b) { return a[2] < b[2]; });

        UnionFind uf(n);
        int total = 0, cnt = 0;
        for (auto& e : edges) {
            if (uf.unite(e[0], e[1])) {
                total += e[2];
                if (++cnt == n - 1) break;
            }
        }

        cout << "点: ";
        for (auto& p : points) cout << "(" << p[0] << "," << p[1] << ") ";
        cout << "\n最小费用: " << total << "\n\n";
    }
}

// ==================== Demo7: 图搜索进阶 ====================
void demo7_graphSearch() {
    cout << "===== Demo7: 图搜索进阶 =====\n\n";

    // --- LC 797: 所有可能的路径 ---
    {
        cout << "--- LC 797: 所有可能的路径 (DAG 0→n-1) ---\n";
        auto allPathsSourceTarget = [](vector<vector<int>>& graph) -> vector<vector<int>> {
            vector<vector<int>> result;
            vector<int> path = {0};
            int target = graph.size() - 1;
            function<void(int)> dfs = [&](int node) {
                if (node == target) { result.push_back(path); return; }
                for (int nb : graph[node]) {
                    path.push_back(nb);
                    dfs(nb);
                    path.pop_back();
                }
            };
            dfs(0);
            return result;
        };

        vector<vector<int>> graph = {{1,2},{3},{3},{}};
        cout << "graph = [[1,2],[3],[3],[]]\n";
        cout << "  0→{1,2}, 1→{3}, 2→{3}, 3→{}\n";
        auto paths = allPathsSourceTarget(graph);
        cout << "所有路径 0→3:\n";
        for (auto& p : paths) {
            cout << "  ";
            for (int i = 0; i < (int)p.size(); i++)
                cout << p[i] << (i < (int)p.size()-1 ? "→" : "");
            cout << "\n";
        }
        cout << "\n";
    }

    // --- LC 841: 钥匙和房间 ---
    {
        cout << "--- LC 841: 钥匙和房间 ---\n";
        auto canVisitAllRooms = [](vector<vector<int>>& rooms) -> bool {
            int n = rooms.size();
            vector<bool> visited(n, false);
            queue<int> q;
            q.push(0);
            visited[0] = true;
            int count = 1;
            while (!q.empty()) {
                int room = q.front(); q.pop();
                for (int key : rooms[room])
                    if (!visited[key]) {
                        visited[key] = true;
                        count++;
                        q.push(key);
                    }
            }
            return count == n;
        };

        vector<vector<int>> rooms1 = {{1},{2},{3},{}};
        vector<vector<int>> rooms2 = {{1,3},{3,0,1},{2},{0}};
        cout << "房间1 [[1],[2],[3],[]] → "
             << (canVisitAllRooms(rooms1) ? "能全访问 ✓" : "不能") << "\n";
        cout << "  0→拿钥匙1 → 1→拿钥匙2 → 2→拿钥匙3 → 3\n";

        cout << "房间2 [[1,3],[3,0,1],[2],[0]] → "
             << (canVisitAllRooms(rooms2) ? "能全访问 ✓" : "不能 ✗") << "\n";
        cout << "  0→拿1,3 → 1→已有 → 3→拿0(已有) → 房间2无法到达\n\n";
    }
}

// ==================== main ====================
int main() {
    cout << "╔════════════════════════════════════════════════════════╗\n";
    cout << "║  专题十二（下）：拓扑排序 + 最短路径 + MST 演示        ║\n";
    cout << "║  作者：大胖超 😜                                       ║\n";
    cout << "╚════════════════════════════════════════════════════════╝\n\n";

    demo1_topoSortBFS();
    demo2_topoSortDFS();
    demo3_dijkstra();
    demo4_bellmanFord();
    demo5_floyd();
    demo6_kruskal();
    demo7_graphSearch();

    cout << "========================================\n";
    cout << "全部 Demo 运行完毕！\n";
    cout << "覆盖题目: LC 207, 210, 743, 787, 797, 841, 1334, 1584\n";
    cout << "算法: 拓扑排序(BFS/DFS), Dijkstra, Bellman-Ford,\n";
    cout << "      Floyd-Warshall, Kruskal MST\n";
    cout << "共 8 道 LeetCode 题 + 5 个经典算法模板\n";
    return 0;
}
