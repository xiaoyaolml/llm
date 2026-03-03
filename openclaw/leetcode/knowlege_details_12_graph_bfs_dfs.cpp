/**
 * 专题十二（上）：图论基础 — BFS、DFS与网格搜索 示例代码
 * 作者：大胖超 😜
 *
 * 编译: g++ -std=c++17 -o knowlege_details_12_graph_bfs_dfs knowlege_details_12_graph_bfs_dfs.cpp
 * 运行: ./knowlege_details_12_graph_bfs_dfs
 *
 * 覆盖题目：
 *   Demo1: 图的存储与遍历（邻接表 BFS + DFS）
 *   Demo2: 网格搜索 — 岛屿系列（LC 200 岛屿数量 + LC 695 最大面积）
 *   Demo3: 网格进阶（LC 130 被围绕区域 + LC 417 太平洋大西洋）
 *   Demo4: 多源 BFS（LC 994 腐烂橘子 + LC 542 01矩阵）
 *   Demo5: 图连通性（LC 547 省份数量 + LC 785 二部图判定）
 *   Demo6: 并查集（LC 547 并查集版 + LC 684 冗余连接）
 *   Demo7: 环检测（有向图三色标记法）
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
    bool connected(int x, int y) { return find(x) == find(y); }
};

// ==================== Demo1: 图的存储与遍历 ====================
void demo1_graphTraversal() {
    cout << "===== Demo1: 图的存储与遍历 =====\n\n";

    // 构建无向图
    //   0 --- 1 --- 3
    //   |     |
    //   2 --- 4
    int n = 5;
    vector<vector<int>> adj(n);
    vector<pair<int,int>> edges = {{0,1},{0,2},{1,3},{1,4},{2,4}};
    for (auto& [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    cout << "邻接表:\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << i << " → ";
        for (int nb : adj[i]) cout << nb << " ";
        cout << "\n";
    }

    // BFS
    {
        cout << "\nBFS (从节点0出发): ";
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(0);
        visited[0] = true;
        while (!q.empty()) {
            int node = q.front(); q.pop();
            cout << node << " ";
            for (int nb : adj[node])
                if (!visited[nb]) { visited[nb] = true; q.push(nb); }
        }
        cout << "\n";
    }

    // BFS 带层次
    {
        cout << "BFS 分层:\n";
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(0);
        visited[0] = true;
        int level = 0;
        while (!q.empty()) {
            int size = q.size();
            cout << "  层" << level << ": ";
            for (int i = 0; i < size; i++) {
                int node = q.front(); q.pop();
                cout << node << " ";
                for (int nb : adj[node])
                    if (!visited[nb]) { visited[nb] = true; q.push(nb); }
            }
            cout << "\n";
            level++;
        }
    }

    // DFS 递归
    {
        cout << "DFS 递归 (从节点0出发): ";
        vector<bool> visited(n, false);
        function<void(int)> dfs = [&](int node) {
            visited[node] = true;
            cout << node << " ";
            for (int nb : adj[node])
                if (!visited[nb]) dfs(nb);
        };
        dfs(0);
        cout << "\n";
    }

    // DFS 迭代（栈）
    {
        cout << "DFS 迭代 (从节点0出发): ";
        vector<bool> visited(n, false);
        stack<int> stk;
        stk.push(0);
        while (!stk.empty()) {
            int node = stk.top(); stk.pop();
            if (visited[node]) continue;
            visited[node] = true;
            cout << node << " ";
            for (int i = adj[node].size() - 1; i >= 0; i--)
                if (!visited[adj[node][i]])
                    stk.push(adj[node][i]);
        }
        cout << "\n\n";
    }
}

// ==================== Demo2: 岛屿系列 ====================
void demo2_islands() {
    cout << "===== Demo2: 岛屿系列 =====\n\n";

    // --- LC 200: 岛屿数量 ---
    {
        cout << "--- LC 200: 岛屿数量 ---\n";
        auto numIslands = [](vector<vector<char>> grid) -> int {
            int m = grid.size(), n = grid[0].size(), count = 0;
            int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};
            function<void(int,int)> dfs = [&](int x, int y) {
                if (x < 0 || x >= m || y < 0 || y >= n || grid[x][y] != '1') return;
                grid[x][y] = '0';
                for (int d = 0; d < 4; d++) dfs(x+dx[d], y+dy[d]);
            };
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++)
                    if (grid[i][j] == '1') { count++; dfs(i, j); }
            return count;
        };

        vector<vector<char>> grid = {
            {'1','1','0','0','0'},
            {'1','1','0','0','0'},
            {'0','0','1','0','0'},
            {'0','0','0','1','1'}
        };
        cout << "网格:\n";
        for (auto& row : grid) {
            cout << "  ";
            for (char c : row) cout << c << " ";
            cout << "\n";
        }
        cout << "岛屿数量: " << numIslands(grid) << "\n\n";
    }

    // --- LC 695: 岛屿的最大面积 ---
    {
        cout << "--- LC 695: 岛屿的最大面积 ---\n";
        auto maxAreaOfIsland = [](vector<vector<int>> grid) -> int {
            int m = grid.size(), n = grid[0].size(), maxArea = 0;
            int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};
            function<int(int,int)> dfs = [&](int x, int y) -> int {
                if (x < 0 || x >= m || y < 0 || y >= n || grid[x][y] != 1) return 0;
                grid[x][y] = 0;
                int area = 1;
                for (int d = 0; d < 4; d++) area += dfs(x+dx[d], y+dy[d]);
                return area;
            };
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++)
                    if (grid[i][j] == 1) maxArea = max(maxArea, dfs(i, j));
            return maxArea;
        };

        vector<vector<int>> grid = {
            {0,0,1,0,0,0,0,1,0,0,0,0,0},
            {0,0,0,0,0,0,0,1,1,1,0,0,0},
            {0,1,1,0,1,0,0,0,0,0,0,0,0},
            {0,1,0,0,1,1,0,0,1,0,1,0,0},
            {0,1,0,0,1,1,0,0,1,1,1,0,0},
            {0,0,0,0,0,0,0,0,0,0,1,0,0},
            {0,0,0,0,0,0,0,1,1,1,0,0,0},
            {0,0,0,0,0,0,0,1,1,0,0,0,0}
        };
        cout << "最大岛屿面积: " << maxAreaOfIsland(grid) << "\n\n";
    }
}

// ==================== Demo3: 网格进阶 ====================
void demo3_gridAdvanced() {
    cout << "===== Demo3: 网格进阶 =====\n\n";

    // --- LC 130: 被围绕的区域 ---
    {
        cout << "--- LC 130: 被围绕的区域 ---\n";
        auto solve = [](vector<vector<char>> board) -> vector<vector<char>> {
            int m = board.size(), n = board[0].size();
            int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};
            function<void(int,int)> dfs = [&](int x, int y) {
                if (x < 0 || x >= m || y < 0 || y >= n || board[x][y] != 'O') return;
                board[x][y] = '#';
                for (int d = 0; d < 4; d++) dfs(x+dx[d], y+dy[d]);
            };
            for (int i = 0; i < m; i++) { dfs(i, 0); dfs(i, n-1); }
            for (int j = 0; j < n; j++) { dfs(0, j); dfs(m-1, j); }
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++) {
                    if (board[i][j] == 'O') board[i][j] = 'X';
                    if (board[i][j] == '#') board[i][j] = 'O';
                }
            return board;
        };

        vector<vector<char>> board = {
            {'X','X','X','X'},
            {'X','O','O','X'},
            {'X','X','O','X'},
            {'X','O','X','X'}
        };
        cout << "输入:\n";
        for (auto& row : board) {
            cout << "  ";
            for (char c : row) cout << c << " ";
            cout << "\n";
        }
        auto result = solve(board);
        cout << "输出:\n";
        for (auto& row : result) {
            cout << "  ";
            for (char c : row) cout << c << " ";
            cout << "\n";
        }
        cout << "策略: 从边界'O'出发DFS标记安全,剩余'O'被围\n\n";
    }

    // --- LC 417: 太平洋大西洋 简化示例 ---
    {
        cout << "--- LC 417: 太平洋大西洋水流 ---\n";
        auto pacificAtlantic = [](vector<vector<int>>& heights) -> vector<vector<int>> {
            int m = heights.size(), n = heights[0].size();
            vector<vector<bool>> pac(m, vector<bool>(n, false));
            vector<vector<bool>> atl(m, vector<bool>(n, false));
            int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};
            function<void(int,int,vector<vector<bool>>&)> dfs =
                [&](int x, int y, vector<vector<bool>>& ocean) {
                ocean[x][y] = true;
                for (int d = 0; d < 4; d++) {
                    int nx = x+dx[d], ny = y+dy[d];
                    if (nx >= 0 && nx < m && ny >= 0 && ny < n
                        && !ocean[nx][ny] && heights[nx][ny] >= heights[x][y])
                        dfs(nx, ny, ocean);
                }
            };
            for (int i = 0; i < m; i++) { dfs(i, 0, pac); dfs(i, n-1, atl); }
            for (int j = 0; j < n; j++) { dfs(0, j, pac); dfs(m-1, j, atl); }
            vector<vector<int>> result;
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++)
                    if (pac[i][j] && atl[i][j]) result.push_back({i, j});
            return result;
        };

        vector<vector<int>> heights = {
            {1,2,2,3,5},
            {3,2,3,4,4},
            {2,4,5,3,1},
            {6,7,1,4,5},
            {5,1,1,2,4}
        };
        auto res = pacificAtlantic(heights);
        cout << "能同时流到两个海洋的位置: ";
        for (auto& pos : res) cout << "(" << pos[0] << "," << pos[1] << ") ";
        cout << "\n\n";
    }
}

// ==================== Demo4: 多源 BFS ====================
void demo4_multiBFS() {
    cout << "===== Demo4: 多源 BFS =====\n\n";

    // --- LC 994: 腐烂的橘子 ---
    {
        cout << "--- LC 994: 腐烂的橘子 ---\n";
        auto orangesRotting = [](vector<vector<int>> grid) -> int {
            int m = grid.size(), n = grid[0].size();
            queue<pair<int,int>> q;
            int fresh = 0;
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++) {
                    if (grid[i][j] == 2) q.push({i, j});
                    else if (grid[i][j] == 1) fresh++;
                }
            if (fresh == 0) return 0;
            int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};
            int minutes = 0;
            while (!q.empty()) {
                int size = q.size();
                bool rotted = false;
                for (int k = 0; k < size; k++) {
                    auto [x, y] = q.front(); q.pop();
                    for (int d = 0; d < 4; d++) {
                        int nx = x+dx[d], ny = y+dy[d];
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
        };

        vector<vector<int>> grid = {{2,1,1},{1,1,0},{0,1,1}};
        cout << "网格:\n";
        for (auto& row : grid) {
            cout << "  ";
            for (int x : row) cout << x << " ";
            cout << "\n";
        }
        cout << "(2=烂, 1=鲜, 0=空)\n";
        cout << "最少分钟: " << orangesRotting(grid) << "\n\n";

        // 追踪过程
        cout << "扩散过程:\n";
        grid = {{2,1,1},{1,1,0},{0,1,1}};
        queue<pair<int,int>> q;
        int fresh = 0;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) {
                if (grid[i][j] == 2) q.push({i, j});
                else if (grid[i][j] == 1) fresh++;
            }
        int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};
        int minute = 0;
        while (!q.empty() && fresh > 0) {
            int size = q.size();
            minute++;
            cout << "  第" << minute << "分钟: ";
            for (int k = 0; k < size; k++) {
                auto [x, y] = q.front(); q.pop();
                for (int d = 0; d < 4; d++) {
                    int nx = x+dx[d], ny = y+dy[d];
                    if (nx >= 0 && nx < 3 && ny >= 0 && ny < 3 && grid[nx][ny] == 1) {
                        grid[nx][ny] = 2;
                        q.push({nx, ny});
                        fresh--;
                        cout << "(" << nx << "," << ny << ") ";
                    }
                }
            }
            cout << "腐烂\n";
        }
        cout << "\n";
    }

    // --- LC 542: 01 矩阵 ---
    {
        cout << "--- LC 542: 01 矩阵 (每个1到最近0的距离) ---\n";
        auto updateMatrix = [](vector<vector<int>> mat) -> vector<vector<int>> {
            int m = mat.size(), n = mat[0].size();
            vector<vector<int>> dist(m, vector<int>(n, INT_MAX));
            queue<pair<int,int>> q;
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++)
                    if (mat[i][j] == 0) { dist[i][j] = 0; q.push({i, j}); }
            int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};
            while (!q.empty()) {
                auto [x, y] = q.front(); q.pop();
                for (int d = 0; d < 4; d++) {
                    int nx = x+dx[d], ny = y+dy[d];
                    if (nx >= 0 && nx < m && ny >= 0 && ny < n
                        && dist[nx][ny] > dist[x][y] + 1) {
                        dist[nx][ny] = dist[x][y] + 1;
                        q.push({nx, ny});
                    }
                }
            }
            return dist;
        };

        vector<vector<int>> mat = {{0,0,0},{0,1,0},{1,1,1}};
        auto dist = updateMatrix(mat);
        cout << "输入:     输出:\n";
        for (int i = 0; i < 3; i++) {
            cout << "  ";
            for (int j = 0; j < 3; j++) cout << mat[i][j] << " ";
            cout << "    ";
            for (int j = 0; j < 3; j++) cout << dist[i][j] << " ";
            cout << "\n";
        }
        cout << "\n";
    }
}

// ==================== Demo5: 图连通性 ====================
void demo5_connectivity() {
    cout << "===== Demo5: 图连通性 =====\n\n";

    // --- LC 547: 省份数量 (DFS) ---
    {
        cout << "--- LC 547: 省份数量 (DFS) ---\n";
        auto findCircleNum = [](vector<vector<int>> isConnected) -> int {
            int n = isConnected.size(), count = 0;
            vector<bool> visited(n, false);
            function<void(int)> dfs = [&](int i) {
                visited[i] = true;
                for (int j = 0; j < n; j++)
                    if (isConnected[i][j] == 1 && !visited[j]) dfs(j);
            };
            for (int i = 0; i < n; i++)
                if (!visited[i]) { dfs(i); count++; }
            return count;
        };

        vector<vector<int>> conn = {{1,1,0},{1,1,0},{0,0,1}};
        cout << "连接矩阵:\n";
        for (auto& row : conn) {
            cout << "  ";
            for (int x : row) cout << x << " ";
            cout << "\n";
        }
        cout << "省份数量: " << findCircleNum(conn) << "\n\n";
    }

    // --- LC 785: 判断二部图 ---
    {
        cout << "--- LC 785: 判断二部图 (BFS染色) ---\n";
        auto isBipartite = [](vector<vector<int>>& graph) -> bool {
            int n = graph.size();
            vector<int> color(n, -1);
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
                            return false;
                        }
                    }
                }
            }
            return true;
        };

        // 二部图: 0-1, 0-3, 2-1, 2-3
        vector<vector<int>> g1 = {{1,3},{0,2},{1,3},{0,2}};
        // 非二部图: 0-1, 0-2, 1-2 (三角形)
        vector<vector<int>> g2 = {{1,2},{0,2},{0,1}};

        cout << "图1 [[1,3],[0,2],[1,3],[0,2]]: "
             << (isBipartite(g1) ? "是二部图 ✓" : "非二部图 ✗") << "\n";
        cout << "  染色: 0→A, 1→B, 2→A, 3→B (相邻不同色)\n";
        cout << "图2 [[1,2],[0,2],[0,1]]: "
             << (isBipartite(g2) ? "是二部图 ✓" : "非二部图 ✗") << "\n";
        cout << "  三角形无法二染色\n\n";
    }
}

// ==================== Demo6: 并查集 ====================
void demo6_unionFind() {
    cout << "===== Demo6: 并查集 =====\n\n";

    // --- 并查集基本操作演示 ---
    {
        cout << "--- 并查集基本操作 ---\n";
        UnionFind uf(6);
        cout << "初始: 6个独立节点 {0},{1},{2},{3},{4},{5}\n";

        vector<pair<int,int>> ops = {{0,1},{1,2},{3,4}};
        for (auto [u, v] : ops) {
            uf.unite(u, v);
            cout << "合并(" << u << "," << v << ") → 连通分量: " << uf.components << "\n";
        }

        cout << "0与2连通? " << (uf.connected(0, 2) ? "是" : "否") << "\n";
        cout << "0与3连通? " << (uf.connected(0, 3) ? "是" : "否") << "\n";

        uf.unite(2, 4);
        cout << "合并(2,4) → 连通分量: " << uf.components << "\n";
        cout << "0与3连通? " << (uf.connected(0, 3) ? "是" : "否") << "\n\n";
    }

    // --- LC 547: 省份数量（并查集版） ---
    {
        cout << "--- LC 547: 省份数量（并查集版） ---\n";
        vector<vector<int>> conn = {{1,0,0,1},{0,1,1,0},{0,1,1,1},{1,0,1,1}};
        int n = conn.size();
        UnionFind uf(n);
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
                if (conn[i][j]) uf.unite(i, j);
        cout << "连接矩阵:\n";
        for (auto& row : conn) {
            cout << "  ";
            for (int x : row) cout << x << " ";
            cout << "\n";
        }
        cout << "省份数量: " << uf.components << "\n\n";
    }

    // --- LC 684: 冗余连接 ---
    {
        cout << "--- LC 684: 冗余连接 ---\n";
        auto findRedundantConnection = [](vector<vector<int>>& edges) -> vector<int> {
            int n = edges.size();
            UnionFind uf(n + 1);
            for (auto& e : edges) {
                if (!uf.unite(e[0], e[1]))
                    return e;
            }
            return {};
        };

        vector<vector<int>> edges = {{1,2},{1,3},{2,3}};
        cout << "edges = [[1,2],[1,3],[2,3]]\n";
        cout << "添加边过程:\n";
        {
            UnionFind uf(4);
            for (auto& e : edges) {
                bool merged = uf.unite(e[0], e[1]);
                cout << "  [" << e[0] << "," << e[1] << "] → "
                     << (merged ? "成功合并" : "已连通 → 冗余边!") << "\n";
            }
        }
        auto redundant = findRedundantConnection(edges);
        cout << "冗余边: [" << redundant[0] << "," << redundant[1] << "]\n\n";
    }
}

// ==================== Demo7: 环检测 ====================
void demo7_cycleDetection() {
    cout << "===== Demo7: 有向图环检测 (三色标记) =====\n\n";

    auto hasCycleWithTrace = [](vector<vector<int>>& adj, int n) -> bool {
        vector<int> color(n, 0);  // 0白 1灰 2黑
        vector<string> colorName = {"白(未访问)", "灰(路径上)", "黑(完成)"};
        bool found = false;

        function<bool(int)> dfs = [&](int u) -> bool {
            color[u] = 1;
            cout << "    进入 " << u << " (染灰)\n";
            for (int v : adj[u]) {
                if (color[v] == 1) {
                    cout << "    " << u << "→" << v << " 遇到灰色节点 → 有环!\n";
                    return true;
                }
                if (color[v] == 0 && dfs(v)) return true;
            }
            color[u] = 2;
            cout << "    完成 " << u << " (染黑)\n";
            return false;
        };

        for (int i = 0; i < n; i++)
            if (color[i] == 0 && dfs(i)) return true;
        return false;
    };

    // 有环图: 0→1→2→0
    {
        cout << "--- 有环图: 0→1→2→0 ---\n";
        vector<vector<int>> adj = {{1}, {2}, {0}};
        cout << "  邻接表: 0→[1], 1→[2], 2→[0]\n";
        cout << "  DFS 过程:\n";
        bool cycle = hasCycleWithTrace(adj, 3);
        cout << "  结果: " << (cycle ? "有环 ✓" : "无环") << "\n\n";
    }

    // 无环图 (DAG): 0→1→2, 0→2
    {
        cout << "--- 无环图: 0→1→2, 0→2 ---\n";
        vector<vector<int>> adj = {{1, 2}, {2}, {}};
        cout << "  邻接表: 0→[1,2], 1→[2], 2→[]\n";
        cout << "  DFS 过程:\n";
        bool cycle = hasCycleWithTrace(adj, 3);
        cout << "  结果: " << (cycle ? "有环" : "无环(DAG) ✓") << "\n\n";
    }
}

// ==================== main ====================
int main() {
    cout << "╔════════════════════════════════════════════════════╗\n";
    cout << "║  专题十二（上）：图论基础 BFS/DFS/网格/并查集      ║\n";
    cout << "║  作者：大胖超 😜                                   ║\n";
    cout << "╚════════════════════════════════════════════════════╝\n\n";

    demo1_graphTraversal();
    demo2_islands();
    demo3_gridAdvanced();
    demo4_multiBFS();
    demo5_connectivity();
    demo6_unionFind();
    demo7_cycleDetection();

    cout << "========================================\n";
    cout << "全部 Demo 运行完毕！\n";
    cout << "覆盖题目: LC 130, 200, 417, 542, 547, 684, 695, 785, 994\n";
    cout << "额外: 图存储、BFS/DFS遍历、三色环检测、并查集模板\n";
    cout << "共 9 道 LeetCode 题 + 多个基础模板\n";
    return 0;
}
