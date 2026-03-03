/*
 * 专题十六：并查集 Union-Find — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_16_union_find.cpp -o union_find
 * 运行：./union_find
 *
 * Demo 列表：
 *   Demo1  — 标准并查集模板 + 原理演示
 *   Demo2  — LC 547  省份数量（连通分量计数）
 *   Demo3  — LC 200  岛屿数量（并查集解法）
 *   Demo4  — LC 684  冗余连接（环检测）
 *   Demo5  — LC 721  账户合并（等价类合并）
 *   Demo6  — LC 990  等式方程的可满足性
 *   Demo7  — LC 399  除法求值（带权并查集）
 *   Demo8  — LC 130  被围绕的区域（虚拟节点）
 *   Demo9  — LC 886  可能的二分法（种类并查集）
 *   Demo10 — LC 1584 连接所有点的最小费用（Kruskal）
 *   Demo11 — LC 1202 交换字符串中的元素
 */

#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <tuple>
#include <cmath>
using namespace std;

/* ============================================================
 * 通用 UnionFind 模板（按秩合并 + 路径压缩）
 * ============================================================ */
class UnionFind {
    vector<int> parent, rank_;
    int components;

public:
    UnionFind(int n) : parent(n), rank_(n, 0), components(n) {
        iota(parent.begin(), parent.end(), 0);
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
        components--;
        return true;
    }

    bool connected(int x, int y) {
        return find(x) == find(y);
    }

    int count() const { return components; }
};

/* ============================================================
 * Demo1: 标准并查集模板 + 原理演示
 * ============================================================ */
namespace Demo1 {

void run() {
    cout << "===== Demo1: 并查集原理演示 =====\n";
    UnionFind uf(6);  // 元素 0~5
    cout << "初始: 6 个独立集合, components = " << uf.count() << "\n";

    cout << "\nunite(0, 1): " << (uf.unite(0, 1) ? "成功" : "已连通") << "\n";
    cout << "unite(2, 3): " << (uf.unite(2, 3) ? "成功" : "已连通") << "\n";
    cout << "unite(4, 5): " << (uf.unite(4, 5) ? "成功" : "已连通") << "\n";
    cout << "components = " << uf.count() << "  (期望 3)\n";

    cout << "\nconnected(0, 1): " << uf.connected(0, 1) << "  (期望 1)\n";
    cout << "connected(0, 2): " << uf.connected(0, 2) << "  (期望 0)\n";

    uf.unite(0, 2);
    cout << "\nunite(0, 2): 合并 {0,1} 和 {2,3}\n";
    cout << "connected(1, 3): " << uf.connected(1, 3) << "  (期望 1)\n";
    cout << "components = " << uf.count() << "  (期望 2)\n";

    cout << "\nunite(1, 3): " << (uf.unite(1, 3) ? "成功" : "已连通(返回false)") << "\n";
    cout << "components = " << uf.count() << "  (期望 2, 不变)\n\n";
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 547 — 省份数量（连通分量计数）
 * ============================================================ */
namespace Demo2 {

int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (isConnected[i][j])
                uf.unite(i, j);
    return uf.count();
}

void run() {
    cout << "===== Demo2: LC 547 省份数量 =====\n";
    {
        vector<vector<int>> conn = {{1,1,0},{1,1,0},{0,0,1}};
        cout << "[[1,1,0],[1,1,0],[0,0,1]] → " << findCircleNum(conn)
             << " 个省份  (期望 2)\n";
    }
    {
        vector<vector<int>> conn = {{1,0,0},{0,1,0},{0,0,1}};
        cout << "[[1,0,0],[0,1,0],[0,0,1]] → " << findCircleNum(conn)
             << " 个省份  (期望 3)\n";
    }
    {
        vector<vector<int>> conn = {{1,1,1},{1,1,1},{1,1,1}};
        cout << "[[1,1,1],[1,1,1],[1,1,1]] → " << findCircleNum(conn)
             << " 个省份  (期望 1)\n";
    }
    cout << endl;
}
} // namespace Demo2

/* ============================================================
 * Demo3: LC 200 — 岛屿数量（并查集解法）
 * ============================================================ */
namespace Demo3 {

int numIslands(vector<vector<char>>& grid) {
    int m = grid.size(), n = grid[0].size();
    UnionFind uf(m * n);
    int waterCount = 0;

    int dx[] = {1, 0}, dy[] = {0, 1};  // 只看右和下

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

void run() {
    cout << "===== Demo3: LC 200 岛屿数量 (并查集) =====\n";
    {
        vector<vector<char>> grid = {
            {'1','1','1','1','0'},
            {'1','1','0','1','0'},
            {'1','1','0','0','0'},
            {'0','0','0','0','0'}
        };
        cout << "grid1 → " << numIslands(grid) << " 个岛屿  (期望 1)\n";
    }
    {
        vector<vector<char>> grid = {
            {'1','1','0','0','0'},
            {'1','1','0','0','0'},
            {'0','0','1','0','0'},
            {'0','0','0','1','1'}
        };
        cout << "grid2 → " << numIslands(grid) << " 个岛屿  (期望 3)\n";
    }
    cout << endl;
}
} // namespace Demo3

/* ============================================================
 * Demo4: LC 684 — 冗余连接（无向图环检测）
 * 找到那条使图不再是树的多余边
 * ============================================================ */
namespace Demo4 {

vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    int n = edges.size();
    UnionFind uf(n + 1);  // 节点从 1 开始
    for (auto& e : edges) {
        if (!uf.unite(e[0], e[1]))
            return e;  // 合并失败 → 已连通 → 这条边多余
    }
    return {};
}

void run() {
    cout << "===== Demo4: LC 684 冗余连接 (环检测) =====\n";
    {
        vector<vector<int>> edges = {{1,2},{1,3},{2,3}};
        auto res = findRedundantConnection(edges);
        cout << "edges: [[1,2],[1,3],[2,3]]\n";
        cout << "冗余边: [" << res[0] << "," << res[1] << "]  (期望 [2,3])\n";
    }
    {
        vector<vector<int>> edges = {{1,2},{2,3},{3,4},{1,4},{1,5}};
        auto res = findRedundantConnection(edges);
        cout << "edges: [[1,2],[2,3],[3,4],[1,4],[1,5]]\n";
        cout << "冗余边: [" << res[0] << "," << res[1] << "]  (期望 [1,4])\n";
    }
    cout << endl;
}
} // namespace Demo4

/* ============================================================
 * Demo5: LC 721 — 账户合并（等价类合并）
 * ============================================================ */
namespace Demo5 {

vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
    int n = accounts.size();
    UnionFind uf(n);
    unordered_map<string, int> emailToIdx;

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

    // 按根分组
    unordered_map<int, set<string>> groups;
    for (int i = 0; i < n; i++) {
        int root = uf.find(i);
        for (int j = 1; j < (int)accounts[i].size(); j++)
            groups[root].insert(accounts[i][j]);
    }

    // 组装结果
    vector<vector<string>> result;
    for (auto& [root, emails] : groups) {
        vector<string> account = {accounts[root][0]};
        account.insert(account.end(), emails.begin(), emails.end());
        result.push_back(account);
    }
    sort(result.begin(), result.end());
    return result;
}

void run() {
    cout << "===== Demo5: LC 721 账户合并 =====\n";
    vector<vector<string>> accounts = {
        {"John", "johnsmith@mail.com", "john_newyork@mail.com"},
        {"John", "johnsmith@mail.com", "john00@mail.com"},
        {"Mary", "mary@mail.com"},
        {"John", "johnnybravo@mail.com"}
    };

    auto result = accountsMerge(accounts);
    for (auto& acc : result) {
        cout << "  [" << acc[0];
        for (int i = 1; i < (int)acc.size(); i++)
            cout << ", " << acc[i];
        cout << "]\n";
    }
    cout << "期望: John(3封合并), John(1封独立), Mary(1封)\n\n";
}
} // namespace Demo5

/* ============================================================
 * Demo6: LC 990 — 等式方程的可满足性
 * 先处理 == 合并，再检查 != 是否矛盾
 * ============================================================ */
namespace Demo6 {

bool equationsPossible(vector<string>& equations) {
    UnionFind uf(26);

    // 第一遍: 处理等式
    for (auto& eq : equations) {
        if (eq[1] == '=')
            uf.unite(eq[0] - 'a', eq[3] - 'a');
    }

    // 第二遍: 检查不等式
    for (auto& eq : equations) {
        if (eq[1] == '!')
            if (uf.connected(eq[0] - 'a', eq[3] - 'a'))
                return false;
    }
    return true;
}

void run() {
    cout << "===== Demo6: LC 990 等式方程的可满足性 =====\n";
    {
        vector<string> eq = {"a==b", "b!=a"};
        cout << "[\"a==b\",\"b!=a\"] → " << (equationsPossible(eq) ? "true" : "false")
             << "  (期望 false)\n";
    }
    {
        vector<string> eq = {"b==a", "a==b"};
        cout << "[\"b==a\",\"a==b\"] → " << (equationsPossible(eq) ? "true" : "false")
             << "  (期望 true)\n";
    }
    {
        vector<string> eq = {"a==b", "b==c", "a==c"};
        cout << "[\"a==b\",\"b==c\",\"a==c\"] → " << (equationsPossible(eq) ? "true" : "false")
             << "  (期望 true)\n";
    }
    {
        vector<string> eq = {"a==b", "b!=c", "c==a"};
        cout << "[\"a==b\",\"b!=c\",\"c==a\"] → " << (equationsPossible(eq) ? "true" : "false")
             << "  (期望 false)\n";
    }
    cout << endl;
}
} // namespace Demo6

/* ============================================================
 * Demo7: LC 399 — 除法求值（带权并查集）
 * weight[x] 表示 x / parent[x] 的值
 * ============================================================ */
namespace Demo7 {

class WeightedUnionFind {
    unordered_map<string, string> parent;
    unordered_map<string, double> weight;  // weight[x] = x / parent[x]

public:
    void add(const string& x) {
        if (parent.count(x)) return;
        parent[x] = x;
        weight[x] = 1.0;
    }

    // 返回 {根, x到根的权重乘积}
    pair<string, double> find(const string& x) {
        if (parent[x] == x) return {x, 1.0};
        auto [root, w] = find(parent[x]);
        parent[x] = root;        // 路径压缩
        weight[x] *= w;          // 权重更新!
        return {root, weight[x]};
    }

    // 已知 x / y = val
    void unite(const string& x, const string& y, double val) {
        add(x); add(y);
        auto [rx, wx] = find(x);  // wx = x / rx
        auto [ry, wy] = find(y);  // wy = y / ry
        if (rx == ry) return;

        parent[rx] = ry;
        weight[rx] = val * wy / wx;
        // 推导: rx = x/wx, ry = y/wy
        //       x/y = val → x = val * y
        //       rx/ry = (x/wx) / (y/wy) 实际是反过来
        //       weight[rx] = rx→ry 的权重 = val * wy / wx
    }

    double query(const string& x, const string& y) {
        if (!parent.count(x) || !parent.count(y)) return -1.0;
        auto [rx, wx] = find(x);
        auto [ry, wy] = find(y);
        if (rx != ry) return -1.0;
        return wx / wy;  // x/y = (x/root) / (y/root) = wx/wy
    }
};

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
    for (auto& q : queries)
        result.push_back(wuf.query(q[0], q[1]));
    return result;
}

void run() {
    cout << "===== Demo7: LC 399 除法求值 (带权并查集) =====\n";
    vector<vector<string>> equations = {{"a","b"},{"b","c"}};
    vector<double> values = {2.0, 3.0};
    vector<vector<string>> queries = {
        {"a","c"},{"b","a"},{"a","e"},{"a","a"},{"x","x"}
    };

    auto result = calcEquation(equations, values, queries);

    cout << "a/b=2.0, b/c=3.0\n";
    vector<string> qstr = {"a/c","b/a","a/e","a/a","x/x"};
    for (int i = 0; i < (int)result.size(); i++) {
        cout << "  " << qstr[i] << " = " << result[i] << "\n";
    }
    cout << "期望: 6.0, 0.5, -1.0, 1.0, -1.0\n\n";
}
} // namespace Demo7

/* ============================================================
 * Demo8: LC 130 — 被围绕的区域（虚拟节点）
 * 边界 'O' 连到 dummy 节点，最后非 dummy 组的 'O' → 'X'
 * ============================================================ */
namespace Demo8 {

void solve(vector<vector<char>>& board) {
    int m = board.size(), n = board[0].size();
    UnionFind uf(m * n + 1);
    int dummy = m * n;

    int dx[] = {1, 0, -1, 0}, dy[] = {0, 1, 0, -1};

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] != 'O') continue;

            if (i == 0 || i == m-1 || j == 0 || j == n-1)
                uf.unite(i * n + j, dummy);

            for (int d = 0; d < 4; d++) {
                int ni = i + dx[d], nj = j + dy[d];
                if (ni >= 0 && ni < m && nj >= 0 && nj < n
                    && board[ni][nj] == 'O')
                    uf.unite(i * n + j, ni * n + nj);
            }
        }
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (board[i][j] == 'O' && !uf.connected(i*n+j, dummy))
                board[i][j] = 'X';
}

void run() {
    cout << "===== Demo8: LC 130 被围绕的区域 (虚拟节点) =====\n";
    vector<vector<char>> board = {
        {'X','X','X','X'},
        {'X','O','O','X'},
        {'X','X','O','X'},
        {'X','O','X','X'}
    };

    cout << "原始:\n";
    for (auto& row : board) {
        cout << "  ";
        for (char c : row) cout << c << ' ';
        cout << '\n';
    }

    solve(board);

    cout << "处理后:\n";
    for (auto& row : board) {
        cout << "  ";
        for (char c : row) cout << c << ' ';
        cout << '\n';
    }
    cout << "期望: 中间O变X, 第4行O保留(与边界不连通? 此例全变X)\n\n";
}
} // namespace Demo8

/* ============================================================
 * Demo9: LC 886 — 可能的二分法（种类并查集）
 * 元素 x 拆成 x(友域) 和 x+n(敌域)
 * ============================================================ */
namespace Demo9 {

bool possibleBipartition(int n, vector<vector<int>>& dislikes) {
    UnionFind uf(2 * n + 1);

    for (auto& d : dislikes) {
        int a = d[0], b = d[1];
        // a和b互为敌人 → a的友域连b的敌域, a的敌域连b的友域
        uf.unite(a, b + n);
        uf.unite(a + n, b);
    }

    // 检查矛盾：某人和自己的敌人在同一集合
    for (int i = 1; i <= n; i++) {
        if (uf.connected(i, i + n))
            return false;
    }
    return true;
}

void run() {
    cout << "===== Demo9: LC 886 可能的二分法 (种类并查集) =====\n";
    {
        int n = 4;
        vector<vector<int>> dislikes = {{1,2},{1,3},{2,4}};
        cout << "n=4, dislikes=[[1,2],[1,3],[2,4]] → "
             << (possibleBipartition(n, dislikes) ? "true" : "false")
             << "  (期望 true)\n";
    }
    {
        int n = 3;
        vector<vector<int>> dislikes = {{1,2},{1,3},{2,3}};
        cout << "n=3, dislikes=[[1,2],[1,3],[2,3]] → "
             << (possibleBipartition(n, dislikes) ? "true" : "false")
             << "  (期望 false)\n";
    }
    {
        int n = 5;
        vector<vector<int>> dislikes = {{1,2},{2,3},{3,4},{4,5},{1,5}};
        cout << "n=5, dislikes=[[1,2],[2,3],[3,4],[4,5],[1,5]] → "
             << (possibleBipartition(n, dislikes) ? "true" : "false")
             << "  (期望 false, 奇数环)\n";
    }
    cout << endl;
}
} // namespace Demo9

/* ============================================================
 * Demo10: LC 1584 — 连接所有点的最小费用（Kruskal MST）
 * ============================================================ */
namespace Demo10 {

int minCostConnectPoints(vector<vector<int>>& points) {
    int n = points.size();
    vector<tuple<int,int,int>> edges;  // {cost, i, j}

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            int cost = abs(points[i][0] - points[j][0])
                     + abs(points[i][1] - points[j][1]);
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

void run() {
    cout << "===== Demo10: LC 1584 最小费用连接所有点 (Kruskal) =====\n";
    {
        vector<vector<int>> points = {{0,0},{2,2},{3,10},{5,2},{7,0}};
        cout << "points: [[0,0],[2,2],[3,10],[5,2],[7,0]]\n";
        cout << "最小费用: " << minCostConnectPoints(points) << "  (期望 20)\n";
    }
    {
        vector<vector<int>> points = {{3,12},{-2,5},{-4,1}};
        cout << "points: [[3,12],[-2,5],[-4,1]]\n";
        cout << "最小费用: " << minCostConnectPoints(points) << "  (期望 18)\n";
    }
    cout << endl;
}
} // namespace Demo10

/* ============================================================
 * Demo11: LC 1202 — 交换字符串中的元素
 * 同一连通分量内的字符可以任意排列 → 各组排序取最小
 * ============================================================ */
namespace Demo11 {

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

void run() {
    cout << "===== Demo11: LC 1202 交换字符串中的元素 =====\n";
    {
        string s = "dcab";
        vector<vector<int>> pairs = {{0,3},{1,2}};
        cout << "s=\"dcab\", pairs=[[0,3],[1,2]]\n";
        cout << "结果: \"" << smallestStringWithSwaps(s, pairs)
             << "\"  (期望 \"bacd\")\n";
    }
    {
        string s = "dcab";
        vector<vector<int>> pairs = {{0,3},{1,2},{0,2}};
        cout << "s=\"dcab\", pairs=[[0,3],[1,2],[0,2]]\n";
        cout << "结果: \"" << smallestStringWithSwaps(s, pairs)
             << "\"  (期望 \"abcd\")\n";
    }
    {
        string s = "cba";
        vector<vector<int>> pairs = {{0,1},{1,2}};
        cout << "s=\"cba\", pairs=[[0,1],[1,2]]\n";
        cout << "结果: \"" << smallestStringWithSwaps(s, pairs)
             << "\"  (期望 \"abc\")\n";
    }
    cout << endl;
}
} // namespace Demo11

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════╗\n";
    cout << "║   专题十六：并查集 Union-Find — 可运行示例代码    ║\n";
    cout << "║   作者：大胖超 😜                               ║\n";
    cout << "╚══════════════════════════════════════════════════╝\n\n";

    Demo1::run();
    Demo2::run();
    Demo3::run();
    Demo4::run();
    Demo5::run();
    Demo6::run();
    Demo7::run();
    Demo8::run();
    Demo9::run();
    Demo10::run();
    Demo11::run();

    cout << "===== 全部 Demo 运行完毕！=====\n";
    return 0;
}
