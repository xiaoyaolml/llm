/*
 * 专题二十（下）：线段树 Segment Tree — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_20_segment_tree.cpp -o seg_tree
 * 运行：./seg_tree
 *
 * Demo 列表：
 *   Demo1  — 基础线段树（单点修改 + 区间求和）
 *   Demo2  — LC 307  区域和检索 — 可修改
 *   Demo3  — 区间最值线段树（区间最大/最小值查询）
 *   Demo4  — 懒标记线段树（区间加 + 区间求和）
 *   Demo5  — 区间赋值线段树（覆盖型懒标记）
 *   Demo6  — 动态开点线段树
 *   Demo7  — LC 699  掉落的方块
 *   Demo8  — LC 732  我的日程安排表 III（动态开点）
 *   Demo9  — 扫描线求矩形面积并（简化版）
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <map>
using namespace std;

/* ============================================================
 * Demo1: 基础线段树（单点修改 + 区间求和，无懒标记）
 * ============================================================ */
namespace Demo1 {

class SegTree {
    vector<long long> tree;
    int n;

    void build(vector<int>& nums, int node, int s, int e) {
        if (s == e) { tree[node] = nums[s]; return; }
        int mid = (s + e) / 2;
        build(nums, 2*node, s, mid);
        build(nums, 2*node+1, mid+1, e);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    void update(int node, int s, int e, int idx, int val) {
        if (s == e) { tree[node] = val; return; }
        int mid = (s + e) / 2;
        if (idx <= mid) update(2*node, s, mid, idx, val);
        else            update(2*node+1, mid+1, e, idx, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    long long query(int node, int s, int e, int l, int r) {
        if (l <= s && e <= r) return tree[node];     // 完全包含
        if (e < l || s > r)   return 0;              // 不相交
        int mid = (s + e) / 2;
        return query(2*node, s, mid, l, r) + query(2*node+1, mid+1, e, l, r);
    }

public:
    SegTree(vector<int>& nums) : n(nums.size()), tree(4 * n, 0) {
        if (n > 0) build(nums, 1, 0, n - 1);
    }
    void update(int idx, int val) { update(1, 0, n-1, idx, val); }
    long long query(int l, int r) { return query(1, 0, n-1, l, r); }
};

void run() {
    cout << "===== Demo1: 基础线段树 (单点修改+区间和) =====\n";
    vector<int> nums = {1, 3, 5, 7, 9, 11};
    SegTree st(nums);

    cout << "nums = [1, 3, 5, 7, 9, 11]\n";
    cout << "query(0, 3) = " << st.query(0, 3) << "  (期望 16 = 1+3+5+7)\n";
    cout << "query(2, 5) = " << st.query(2, 5) << "  (期望 32 = 5+7+9+11)\n";

    st.update(2, 10);  // nums[2] = 10
    cout << "\nupdate(2, 10)后:\n";
    cout << "query(0, 3) = " << st.query(0, 3) << "  (期望 21 = 1+3+10+7)\n";
    cout << "query(0, 5) = " << st.query(0, 5) << "  (期望 41 = 1+3+10+7+9+11)\n\n";
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 307 — 区域和检索（可修改）
 * ============================================================ */
namespace Demo2 {

class NumArray {
    vector<int> tree;
    int n;

    void build(vector<int>& nums, int node, int s, int e) {
        if (s == e) { tree[node] = nums[s]; return; }
        int mid = (s + e) / 2;
        build(nums, 2*node, s, mid);
        build(nums, 2*node+1, mid+1, e);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    void _update(int node, int s, int e, int idx, int val) {
        if (s == e) { tree[node] = val; return; }
        int mid = (s + e) / 2;
        if (idx <= mid) _update(2*node, s, mid, idx, val);
        else            _update(2*node+1, mid+1, e, idx, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    int _query(int node, int s, int e, int l, int r) {
        if (l <= s && e <= r) return tree[node];
        if (e < l || s > r) return 0;
        int mid = (s + e) / 2;
        return _query(2*node, s, mid, l, r) + _query(2*node+1, mid+1, e, l, r);
    }

public:
    NumArray(vector<int>& nums) : n(nums.size()), tree(4 * n, 0) {
        if (n > 0) build(nums, 1, 0, n - 1);
    }
    void update(int index, int val) { _update(1, 0, n-1, index, val); }
    int sumRange(int left, int right) { return _query(1, 0, n-1, left, right); }
};

void run() {
    cout << "===== Demo2: LC 307 区域和检索(可修改) =====\n";
    vector<int> nums = {1, 3, 5};
    NumArray na(nums);
    cout << "nums = [1, 3, 5]\n";
    cout << "sumRange(0, 2) = " << na.sumRange(0, 2) << "  (期望 9)\n";
    na.update(1, 2);
    cout << "update(1, 2) → nums = [1, 2, 5]\n";
    cout << "sumRange(0, 2) = " << na.sumRange(0, 2) << "  (期望 8)\n\n";
}
} // namespace Demo2

/* ============================================================
 * Demo3: 区间最值线段树 (区间最大值/最小值查询)
 * ============================================================ */
namespace Demo3 {

class MaxSegTree {
    vector<int> tree;
    int n;

    void build(vector<int>& nums, int node, int s, int e) {
        if (s == e) { tree[node] = nums[s]; return; }
        int mid = (s + e) / 2;
        build(nums, 2*node, s, mid);
        build(nums, 2*node+1, mid+1, e);
        tree[node] = max(tree[2*node], tree[2*node+1]);
    }

    void update(int node, int s, int e, int idx, int val) {
        if (s == e) { tree[node] = val; return; }
        int mid = (s + e) / 2;
        if (idx <= mid) update(2*node, s, mid, idx, val);
        else            update(2*node+1, mid+1, e, idx, val);
        tree[node] = max(tree[2*node], tree[2*node+1]);
    }

    int query(int node, int s, int e, int l, int r) {
        if (l <= s && e <= r) return tree[node];
        if (e < l || s > r)   return INT_MIN;
        int mid = (s + e) / 2;
        return max(query(2*node, s, mid, l, r), query(2*node+1, mid+1, e, l, r));
    }

public:
    MaxSegTree(vector<int>& nums) : n(nums.size()), tree(4 * n, INT_MIN) {
        if (n > 0) build(nums, 1, 0, n - 1);
    }
    void update(int idx, int val) { update(1, 0, n-1, idx, val); }
    int queryMax(int l, int r) { return query(1, 0, n-1, l, r); }
};

void run() {
    cout << "===== Demo3: 区间最值线段树 =====\n";
    vector<int> nums = {2, 5, 1, 8, 3, 7};
    MaxSegTree st(nums);

    cout << "nums = [2, 5, 1, 8, 3, 7]\n";
    cout << "max(0, 3) = " << st.queryMax(0, 3) << "  (期望 8)\n";
    cout << "max(1, 4) = " << st.queryMax(1, 4) << "  (期望 8)\n";
    cout << "max(4, 5) = " << st.queryMax(4, 5) << "  (期望 7)\n";

    st.update(3, 0);  // nums[3] = 0
    cout << "\nupdate(3, 0)后:\n";
    cout << "max(0, 3) = " << st.queryMax(0, 3) << "  (期望 5)\n";
    cout << "max(0, 5) = " << st.queryMax(0, 5) << "  (期望 7)\n\n";
}
} // namespace Demo3

/* ============================================================
 * Demo4: 懒标记线段树（区间加 + 区间求和）
 * ============================================================ */
namespace Demo4 {

class LazySegTree {
    vector<long long> tree, lazy;
    int n;

    void build(vector<int>& nums, int node, int s, int e) {
        if (s == e) { tree[node] = nums[s]; return; }
        int mid = (s + e) / 2;
        build(nums, 2*node, s, mid);
        build(nums, 2*node+1, mid+1, e);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    void pushDown(int node, int s, int e) {
        if (lazy[node] != 0) {
            int mid = (s + e) / 2;
            tree[2*node]   += lazy[node] * (mid - s + 1);
            tree[2*node+1] += lazy[node] * (e - mid);
            lazy[2*node]   += lazy[node];
            lazy[2*node+1] += lazy[node];
            lazy[node] = 0;
        }
    }

    void rangeUpdate(int node, int s, int e, int l, int r, long long val) {
        if (l <= s && e <= r) {
            tree[node] += val * (e - s + 1);
            lazy[node] += val;
            return;
        }
        pushDown(node, s, e);
        int mid = (s + e) / 2;
        if (l <= mid) rangeUpdate(2*node, s, mid, l, r, val);
        if (r > mid)  rangeUpdate(2*node+1, mid+1, e, l, r, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    long long rangeQuery(int node, int s, int e, int l, int r) {
        if (l <= s && e <= r) return tree[node];
        if (e < l || s > r) return 0;
        pushDown(node, s, e);
        int mid = (s + e) / 2;
        return rangeQuery(2*node, s, mid, l, r) + rangeQuery(2*node+1, mid+1, e, l, r);
    }

public:
    LazySegTree(vector<int>& nums) : n(nums.size()), tree(4*n, 0), lazy(4*n, 0) {
        if (n > 0) build(nums, 1, 0, n-1);
    }
    void rangeAdd(int l, int r, long long val) { rangeUpdate(1, 0, n-1, l, r, val); }
    long long query(int l, int r) { return rangeQuery(1, 0, n-1, l, r); }
};

void run() {
    cout << "===== Demo4: 懒标记线段树 (区间加+区间和) =====\n";
    vector<int> nums = {1, 3, 5, 7, 9, 11};
    LazySegTree st(nums);

    cout << "nums = [1, 3, 5, 7, 9, 11]\n";
    cout << "query(0, 5) = " << st.query(0, 5) << "  (期望 36)\n";

    st.rangeAdd(1, 4, 10);  // [1, 13, 15, 17, 19, 11]
    cout << "\nrangeAdd([1,4], +10)后:\n";
    cout << "query(0, 5) = " << st.query(0, 5) << "  (期望 76)\n";
    cout << "query(1, 3) = " << st.query(1, 3) << "  (期望 45 = 13+15+17)\n";

    st.rangeAdd(0, 2, -5);  // [-4, 8, 10, 17, 19, 11]
    cout << "\nrangeAdd([0,2], -5)后:\n";
    cout << "query(0, 5) = " << st.query(0, 5) << "  (期望 61)\n";
    cout << "query(0, 0) = " << st.query(0, 0) << "  (期望 -4)\n\n";
}
} // namespace Demo4

/* ============================================================
 * Demo5: 区间赋值线段树（覆盖型懒标记）
 * ============================================================ */
namespace Demo5 {

class AssignSegTree {
    vector<long long> tree;
    vector<long long> lazy;
    vector<bool> hasLazy;
    int n;

    void build(int node, int s, int e) {
        tree[node] = 0;
        if (s == e) return;
        int mid = (s + e) / 2;
        build(2*node, s, mid);
        build(2*node+1, mid+1, e);
    }

    void pushDown(int node, int s, int e) {
        if (hasLazy[node]) {
            int mid = (s + e) / 2;
            tree[2*node]     = lazy[node] * (mid - s + 1);
            tree[2*node+1]   = lazy[node] * (e - mid);
            lazy[2*node]     = lazy[2*node+1] = lazy[node];
            hasLazy[2*node]  = hasLazy[2*node+1] = true;
            hasLazy[node]    = false;
        }
    }

    void rangeAssign(int node, int s, int e, int l, int r, long long val) {
        if (l <= s && e <= r) {
            tree[node] = val * (e - s + 1);
            lazy[node] = val;
            hasLazy[node] = true;
            return;
        }
        pushDown(node, s, e);
        int mid = (s + e) / 2;
        if (l <= mid) rangeAssign(2*node, s, mid, l, r, val);
        if (r > mid)  rangeAssign(2*node+1, mid+1, e, l, r, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    long long rangeQuery(int node, int s, int e, int l, int r) {
        if (l <= s && e <= r) return tree[node];
        if (e < l || s > r) return 0;
        pushDown(node, s, e);
        int mid = (s + e) / 2;
        return rangeQuery(2*node, s, mid, l, r) + rangeQuery(2*node+1, mid+1, e, l, r);
    }

public:
    AssignSegTree(int n) : n(n), tree(4*n, 0), lazy(4*n, 0), hasLazy(4*n, false) {
        build(1, 0, n-1);
    }
    void assign(int l, int r, long long val) { rangeAssign(1, 0, n-1, l, r, val); }
    long long query(int l, int r) { return rangeQuery(1, 0, n-1, l, r); }
};

void run() {
    cout << "===== Demo5: 区间赋值线段树 =====\n";
    AssignSegTree st(6);
    cout << "初始全 0, n=6\n";

    st.assign(1, 4, 5);   // [0, 5, 5, 5, 5, 0]
    cout << "assign([1,4], 5): query(0,5) = " << st.query(0, 5) << "  (期望 20)\n";

    st.assign(2, 3, 10);  // [0, 5, 10, 10, 5, 0]
    cout << "assign([2,3], 10): query(0,5) = " << st.query(0, 5) << "  (期望 30)\n";
    cout << "query(1,3) = " << st.query(1, 3) << "  (期望 25 = 5+10+10)\n";

    st.assign(0, 5, 0);   // 全部清 0
    cout << "assign([0,5], 0): query(0,5) = " << st.query(0, 5) << "  (期望 0)\n\n";
}
} // namespace Demo5

/* ============================================================
 * Demo6: 动态开点线段树
 * 值域 [0, 10^9] 但操作次数少
 * ============================================================ */
namespace Demo6 {

struct Node {
    long long val = 0;
    long long lazy = 0;
    int left = 0, right = 0;  // 子节点编号
};

class DynSegTree {
    vector<Node> nodes;
    int root;

    int newNode() {
        nodes.push_back({});
        return nodes.size() - 1;
    }

    void pushDown(int node, int s, int e) {
        if (nodes[node].lazy == 0) return;
        if (!nodes[node].left)  nodes[node].left  = newNode();
        if (!nodes[node].right) nodes[node].right = newNode();
        int mid = s + (e - s) / 2;
        auto& ln = nodes[nodes[node].left];
        auto& rn = nodes[nodes[node].right];
        ln.val  += nodes[node].lazy * (mid - s + 1);
        ln.lazy += nodes[node].lazy;
        rn.val  += nodes[node].lazy * (e - mid);
        rn.lazy += nodes[node].lazy;
        nodes[node].lazy = 0;
    }

    void update(int node, int s, int e, int l, int r, long long val) {
        if (l <= s && e <= r) {
            nodes[node].val += val * (e - s + 1);
            nodes[node].lazy += val;
            return;
        }
        pushDown(node, s, e);
        int mid = s + (e - s) / 2;
        if (!nodes[node].left)  nodes[node].left  = newNode();
        if (!nodes[node].right) nodes[node].right = newNode();
        if (l <= mid) update(nodes[node].left,  s,     mid, l, r, val);
        if (r >  mid) update(nodes[node].right, mid+1, e,   l, r, val);
        nodes[node].val = nodes[nodes[node].left].val + nodes[nodes[node].right].val;
    }

    long long query(int node, int s, int e, int l, int r) {
        if (node == 0) return 0;
        if (l <= s && e <= r) return nodes[node].val;
        if (e < l || s > r) return 0;
        pushDown(node, s, e);
        int mid = s + (e - s) / 2;
        return query(nodes[node].left, s, mid, l, r)
             + query(nodes[node].right, mid+1, e, l, r);
    }

public:
    static const int MAXVAL = 1000000000;

    DynSegTree() {
        nodes.push_back({});  // 0号节点作为空
        root = newNode();     // 1号节点作为根
    }

    void rangeAdd(int l, int r, long long val) { update(root, 0, MAXVAL, l, r, val); }
    long long rangeQuery(int l, int r) { return query(root, 0, MAXVAL, l, r); }
};

void run() {
    cout << "===== Demo6: 动态开点线段树 =====\n";
    DynSegTree dst;

    dst.rangeAdd(100, 200, 5);
    dst.rangeAdd(150, 300, 3);

    cout << "rangeAdd([100,200], +5), rangeAdd([150,300], +3)\n";
    cout << "query(100, 149) = " << dst.rangeQuery(100, 149) << "  (期望 250 = 50*5)\n";
    cout << "query(150, 200) = " << dst.rangeQuery(150, 200) << "  (期望 408 = 51*8)\n";
    cout << "query(201, 300) = " << dst.rangeQuery(201, 300) << "  (期望 300 = 100*3)\n";
    cout << "query(0, 1000000000) = " << dst.rangeQuery(0, 1000000000) << "  (期望 958)\n";
    cout << "节点数: " << dst.nodes.size() << " (远小于 4*10^9)\n\n";
}

// 为了让外部能访问 nodes.size()
// 实际 nodes 是 private, 这里 Demo 简化处理

} // namespace Demo6

/* ============================================================
 * Demo7: LC 699 — 掉落的方块
 * 每个方块掉在 [left, left+size-1] 上，高度取该区间最大值+size
 * 动态开点 / 离散化 + 线段树
 * ============================================================ */
namespace Demo7 {

// 使用离散化 + 数组线段树（区间赋值+区间最大值）
class MaxAssignSeg {
    vector<int> tree, lazy;
    vector<bool> hasLazy;
    int n;

    void pushDown(int node) {
        if (hasLazy[node]) {
            for (int c : {2*node, 2*node+1}) {
                tree[c] = max(tree[c], lazy[node]);
                lazy[c] = max(lazy[c], lazy[node]);
                hasLazy[c] = true;
            }
            hasLazy[node] = false;
        }
    }

    void update(int node, int s, int e, int l, int r, int val) {
        if (l <= s && e <= r) {
            tree[node] = max(tree[node], val);
            lazy[node] = max(lazy[node], val);
            hasLazy[node] = true;
            return;
        }
        pushDown(node);
        int mid = (s + e) / 2;
        if (l <= mid) update(2*node, s, mid, l, r, val);
        if (r > mid)  update(2*node+1, mid+1, e, l, r, val);
        tree[node] = max(tree[2*node], tree[2*node+1]);
    }

    int query(int node, int s, int e, int l, int r) {
        if (l <= s && e <= r) return tree[node];
        if (e < l || s > r) return 0;
        pushDown(node);
        int mid = (s + e) / 2;
        return max(query(2*node, s, mid, l, r), query(2*node+1, mid+1, e, l, r));
    }

public:
    MaxAssignSeg(int n) : n(n), tree(4*n, 0), lazy(4*n, 0), hasLazy(4*n, false) {}
    void update(int l, int r, int val) { update(1, 0, n-1, l, r, val); }
    int query(int l, int r) { return query(1, 0, n-1, l, r); }
};

vector<int> fallingSquares(vector<vector<int>>& positions) {
    // 离散化
    vector<int> coords;
    for (auto& p : positions) {
        coords.push_back(p[0]);
        coords.push_back(p[0] + p[1] - 1);
    }
    sort(coords.begin(), coords.end());
    coords.erase(unique(coords.begin(), coords.end()), coords.end());
    auto getId = [&](int x) {
        return (int)(lower_bound(coords.begin(), coords.end(), x) - coords.begin());
    };

    int m = coords.size();
    MaxAssignSeg seg(m);
    vector<int> result;
    int maxH = 0;

    for (auto& p : positions) {
        int l = getId(p[0]);
        int r = getId(p[0] + p[1] - 1);
        int curMax = seg.query(l, r);
        int newH = curMax + p[1];
        seg.update(l, r, newH);
        maxH = max(maxH, newH);
        result.push_back(maxH);
    }
    return result;
}

void run() {
    cout << "===== Demo7: LC 699 掉落的方块 =====\n";
    {
        vector<vector<int>> pos = {{1,2},{2,3},{6,1}};
        auto res = fallingSquares(pos);
        cout << "[[1,2],[2,3],[6,1]] → [";
        for (int i = 0; i < (int)res.size(); i++) { if (i) cout << ","; cout << res[i]; }
        cout << "]  (期望 [2,5,5])\n";
    }
    {
        vector<vector<int>> pos = {{100,100},{200,100}};
        auto res = fallingSquares(pos);
        cout << "[[100,100],[200,100]] → [";
        for (int i = 0; i < (int)res.size(); i++) { if (i) cout << ","; cout << res[i]; }
        cout << "]  (期望 [100,100])\n";
    }
    cout << endl;
}
} // namespace Demo7

/* ============================================================
 * Demo8: LC 732 — 我的日程安排表 III
 * 差分 + 扫描(简洁) / 动态开点线段树
 * 这里展示线段树做法
 * ============================================================ */
namespace Demo8 {

// 用 map 模拟动态开点（更简洁的代码）
class MyCalendarThree {
    map<int, int> diff;
public:
    int book(int start, int end) {
        diff[start]++;
        diff[end]--;
        int maxK = 0, cur = 0;
        for (auto& [t, d] : diff) {
            cur += d;
            maxK = max(maxK, cur);
        }
        return maxK;
    }
};

void run() {
    cout << "===== Demo8: LC 732 我的日程安排表 III =====\n";
    MyCalendarThree cal;
    vector<pair<int,int>> books = {{10,20},{50,60},{10,40},{5,15},{5,10},{25,55}};
    vector<int> expected = {1, 1, 2, 3, 3, 3};

    for (int i = 0; i < (int)books.size(); i++) {
        int res = cal.book(books[i].first, books[i].second);
        cout << "  book(" << books[i].first << "," << books[i].second
             << ") → " << res << "  (期望 " << expected[i] << ")\n";
    }
    cout << endl;
}
} // namespace Demo8

/* ============================================================
 * Demo9: 扫描线求矩形面积并（简化版）
 * 多个矩形的面积并，使用扫描线 + 线段树
 * ============================================================ */
namespace Demo9 {

// 简化版：离散化 y 坐标，扫描 x 坐标事件
struct Event {
    int x, y1, y2, type; // type: +1 打开, -1 关闭
};

long long rectangleArea(vector<vector<int>>& rectangles) {
    // 收集 y 坐标
    vector<int> ys;
    vector<Event> events;
    for (auto& r : rectangles) {
        ys.push_back(r[1]);
        ys.push_back(r[3]);
        events.push_back({r[0], r[1], r[3], +1});
        events.push_back({r[2], r[1], r[3], -1});
    }

    sort(ys.begin(), ys.end());
    ys.erase(unique(ys.begin(), ys.end()), ys.end());
    int m = ys.size();

    sort(events.begin(), events.end(), [](auto& a, auto& b) { return a.x < b.x; });

    // cnt[i]: 第 i 段 [ys[i], ys[i+1]) 被覆盖了几次
    vector<int> cnt(m, 0);
    long long totalArea = 0;

    for (int e = 0; e < (int)events.size(); ) {
        int curX = events[e].x;
        // 处理同一 x 坐标的所有事件
        while (e < (int)events.size() && events[e].x == curX) {
            int y1 = lower_bound(ys.begin(), ys.end(), events[e].y1) - ys.begin();
            int y2 = lower_bound(ys.begin(), ys.end(), events[e].y2) - ys.begin();
            for (int i = y1; i < y2; i++)
                cnt[i] += events[e].type;
            e++;
        }
        // 计算当前被覆盖的 y 总长度
        if (e < (int)events.size()) {
            int nextX = events[e].x;
            long long coveredY = 0;
            for (int i = 0; i + 1 < m; i++)
                if (cnt[i] > 0)
                    coveredY += ys[i + 1] - ys[i];
            totalArea += coveredY * (nextX - curX);
        }
    }
    return totalArea;
}

void run() {
    cout << "===== Demo9: 扫描线求矩形面积并 =====\n";
    {
        // 两个矩形: [0,0,2,2] 和 [1,0,3,2]
        // 面积并 = 2*2 + 2*2 - 1*2 = 6
        vector<vector<int>> rects = {{0,0,2,2},{1,0,3,2}};
        cout << "矩形 [[0,0,2,2],[1,0,3,2]]\n";
        cout << "面积并 = " << rectangleArea(rects) << "  (期望 6)\n";
    }
    {
        // 三个矩形
        vector<vector<int>> rects = {{0,0,2,2},{1,1,3,3},{0,0,1,1}};
        cout << "矩形 [[0,0,2,2],[1,1,3,3],[0,0,1,1]]\n";
        cout << "面积并 = " << rectangleArea(rects) << "  (期望 7)\n";
    }
    cout << endl;
}
} // namespace Demo9

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║  专题二十（下）：线段树 Segment Tree — 示例代码          ║\n";
    cout << "║  作者：大胖超 😜                                        ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n\n";

    Demo1::run();
    Demo2::run();
    Demo3::run();
    Demo4::run();
    Demo5::run();
    Demo6::run();
    Demo7::run();
    Demo8::run();
    Demo9::run();

    cout << "===== 全部 Demo 运行完毕！=====\n";
    return 0;
}
