/*
 * 专题二十（上）：树状数组 BIT (Fenwick Tree) — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_20_bit.cpp -o bit_fenwick
 * 运行：./bit_fenwick
 *
 * Demo 列表：
 *   Demo1  — 标准 BIT 模板（单点修改+区间查询）
 *   Demo2  — LC 307  区域和检索 — 可修改
 *   Demo3  — LC 315  计算右侧小于当前元素的个数（逆序对 BIT）
 *   Demo4  — 区间修改 + 单点查询（差分 BIT）
 *   Demo5  — 区间修改 + 区间查询（双 BIT）
 *   Demo6  — 二维 BIT + LC 308 二维区域和检索
 *   Demo7  — LC 1649 通过指令创建有序数组
 *   Demo8  — BIT 上二分求第 K 小
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
using namespace std;

/* ============================================================
 * Demo1: 标准 BIT 模板 (单点修改 + 区间查询)
 * ============================================================ */
namespace Demo1 {

class BIT {
    vector<long long> tree;
    int n;
public:
    BIT(int n) : n(n), tree(n + 1, 0) {}

    // O(n) 建树
    BIT(vector<int>& nums) : n(nums.size()), tree(n + 1, 0) {
        for (int i = 1; i <= n; i++) {
            tree[i] += nums[i - 1];
            int p = i + (i & (-i));
            if (p <= n) tree[p] += tree[i];
        }
    }

    void update(int i, int delta) {
        for (; i <= n; i += i & (-i))
            tree[i] += delta;
    }

    long long query(int i) {
        long long sum = 0;
        for (; i > 0; i -= i & (-i))
            sum += tree[i];
        return sum;
    }

    long long rangeQuery(int l, int r) {
        return query(r) - query(l - 1);
    }
};

void run() {
    cout << "===== Demo1: 标准 BIT 模板 =====\n";
    vector<int> nums = {1, 3, 5, 7, 9, 11};
    BIT bit(nums);

    cout << "nums = [1, 3, 5, 7, 9, 11] (1-indexed)\n";
    cout << "query(1..4) = " << bit.rangeQuery(1, 4) << "  (期望 16 = 1+3+5+7)\n";
    cout << "query(2..5) = " << bit.rangeQuery(2, 5) << "  (期望 24 = 3+5+7+9)\n";

    // 修改: nums[3] += 10 (即第3个元素 +10)
    bit.update(3, 10);
    cout << "\nupdate(3, +10)后:\n";
    cout << "query(1..4) = " << bit.rangeQuery(1, 4) << "  (期望 26 = 1+3+15+7)\n";
    cout << "query(3..3) = " << bit.rangeQuery(3, 3) << "  (期望 15)\n\n";
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 307 — 区域和检索（可修改）
 * ============================================================ */
namespace Demo2 {

class NumArray {
    vector<int> nums;
    vector<int> tree;
    int n;

    void bitUpdate(int i, int delta) {
        for (; i <= n; i += i & (-i))
            tree[i] += delta;
    }

    int bitQuery(int i) {
        int sum = 0;
        for (; i > 0; i -= i & (-i))
            sum += tree[i];
        return sum;
    }

public:
    NumArray(vector<int>& nums) : nums(nums), n(nums.size()), tree(n + 1, 0) {
        for (int i = 0; i < n; i++)
            bitUpdate(i + 1, nums[i]);
    }

    void update(int index, int val) {
        int delta = val - nums[index];
        nums[index] = val;
        bitUpdate(index + 1, delta);
    }

    int sumRange(int left, int right) {
        return bitQuery(right + 1) - bitQuery(left);
    }
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
 * Demo3: LC 315 — 计算右侧小于当前元素的个数
 * 离散化 + BIT 逆序对
 * ============================================================ */
namespace Demo3 {

class BIT {
    vector<int> tree;
    int n;
public:
    BIT(int n) : n(n), tree(n + 1, 0) {}
    void update(int i, int d) { for (; i <= n; i += i & (-i)) tree[i] += d; }
    int query(int i) { int s = 0; for (; i > 0; i -= i & (-i)) s += tree[i]; return s; }
};

vector<int> countSmaller(vector<int>& nums) {
    // 离散化
    vector<int> sorted = nums;
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());
    int m = sorted.size();

    BIT bit(m);
    int n = nums.size();
    vector<int> result(n);

    // 从右到左遍历
    for (int i = n - 1; i >= 0; i--) {
        int rank = lower_bound(sorted.begin(), sorted.end(), nums[i]) - sorted.begin() + 1;
        result[i] = bit.query(rank - 1);  // 已出现的比 nums[i] 小的数
        bit.update(rank, 1);
    }
    return result;
}

void run() {
    cout << "===== Demo3: LC 315 右侧小于当前元素的个数 =====\n";
    {
        vector<int> nums = {5, 2, 6, 1};
        auto res = countSmaller(nums);
        cout << "[5,2,6,1] → [";
        for (int i = 0; i < (int)res.size(); i++) { if (i) cout << ","; cout << res[i]; }
        cout << "]  (期望 [2,1,1,0])\n";
    }
    {
        vector<int> nums = {-1};
        auto res = countSmaller(nums);
        cout << "[-1] → [" << res[0] << "]  (期望 [0])\n";
    }
    {
        vector<int> nums = {-1, -1};
        auto res = countSmaller(nums);
        cout << "[-1,-1] → [" << res[0] << "," << res[1] << "]  (期望 [0,0])\n";
    }
    cout << endl;
}
} // namespace Demo3

/* ============================================================
 * Demo4: 区间修改 + 单点查询（差分 BIT）
 * 在差分数组上建 BIT
 * ============================================================ */
namespace Demo4 {

class DiffBIT {
    vector<long long> tree;
    int n;
    void _update(int i, long long d) { for (; i <= n; i += i & (-i)) tree[i] += d; }
    long long _query(int i) { long long s = 0; for (; i > 0; i -= i & (-i)) s += tree[i]; return s; }
public:
    DiffBIT(int n) : n(n), tree(n + 1, 0) {}

    // [l, r] += val
    void rangeAdd(int l, int r, long long val) {
        _update(l, val);
        if (r + 1 <= n) _update(r + 1, -val);
    }

    // 查询 nums[i] 的值
    long long pointQuery(int i) {
        return _query(i);
    }
};

void run() {
    cout << "===== Demo4: 区间修改 + 单点查询 (差分BIT) =====\n";
    int n = 6;
    DiffBIT dbit(n);

    dbit.rangeAdd(2, 4, 5);  // [_, 5, 5, 5, _, _]
    dbit.rangeAdd(3, 6, 3);  // [_, 5, 8, 8, 3, 3]

    cout << "rangeAdd([2,4], +5), rangeAdd([3,6], +3):\n";
    cout << "结果: [";
    for (int i = 1; i <= n; i++) {
        if (i > 1) cout << ",";
        cout << dbit.pointQuery(i);
    }
    cout << "]  (期望 [0,5,8,8,3,3])\n\n";
}
} // namespace Demo4

/* ============================================================
 * Demo5: 区间修改 + 区间查询（双 BIT）
 * sum(1..x) = (x+1)*B1(x) - B2(x)
 * ============================================================ */
namespace Demo5 {

class RangeBIT {
    vector<long long> b1, b2;
    int n;
    void upd(vector<long long>& t, int i, long long d) {
        for (; i <= n; i += i & (-i)) t[i] += d;
    }
    long long qry(vector<long long>& t, int i) {
        long long s = 0;
        for (; i > 0; i -= i & (-i)) s += t[i];
        return s;
    }
public:
    RangeBIT(int n) : n(n), b1(n + 1, 0), b2(n + 1, 0) {}

    void rangeAdd(int l, int r, long long val) {
        upd(b1, l, val);          upd(b1, r + 1, -val);
        upd(b2, l, val * l);     upd(b2, r + 1, -val * (r + 1));
    }

    long long prefixSum(int x) {
        return (x + 1) * qry(b1, x) - qry(b2, x);
    }

    long long rangeSum(int l, int r) {
        return prefixSum(r) - prefixSum(l - 1);
    }
};

void run() {
    cout << "===== Demo5: 区间修改 + 区间查询 (双BIT) =====\n";
    int n = 6;
    RangeBIT rbit(n);

    rbit.rangeAdd(2, 4, 5);  // [0, 5, 5, 5, 0, 0]
    rbit.rangeAdd(3, 6, 3);  // [0, 5, 8, 8, 3, 3]

    cout << "rangeAdd([2,4],+5), rangeAdd([3,6],+3):\n";
    cout << "rangeSum(1,6) = " << rbit.rangeSum(1, 6) << "  (期望 27)\n";
    cout << "rangeSum(2,4) = " << rbit.rangeSum(2, 4) << "  (期望 21 = 5+8+8)\n";
    cout << "rangeSum(5,6) = " << rbit.rangeSum(5, 6) << "  (期望 6 = 3+3)\n\n";
}
} // namespace Demo5

/* ============================================================
 * Demo6: 二维 BIT + LC 308 二维区域和检索
 * ============================================================ */
namespace Demo6 {

class BIT2D {
    vector<vector<int>> tree;
    vector<vector<int>> mat;
    int m, n;
public:
    BIT2D(vector<vector<int>>& matrix) : mat(matrix), m(matrix.size()), n(matrix[0].size()),
        tree(m + 1, vector<int>(n + 1, 0)) {
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                add(i + 1, j + 1, matrix[i][j]);
    }

    void add(int x, int y, int delta) {
        for (int i = x; i <= m; i += i & (-i))
            for (int j = y; j <= n; j += j & (-j))
                tree[i][j] += delta;
    }

    int query(int x, int y) {
        int sum = 0;
        for (int i = x; i > 0; i -= i & (-i))
            for (int j = y; j > 0; j -= j & (-j))
                sum += tree[i][j];
        return sum;
    }

    void update(int row, int col, int val) {
        int delta = val - mat[row][col];
        mat[row][col] = val;
        add(row + 1, col + 1, delta);
    }

    int sumRegion(int r1, int c1, int r2, int c2) {
        return query(r2+1, c2+1) - query(r1, c2+1) - query(r2+1, c1) + query(r1, c1);
    }
};

void run() {
    cout << "===== Demo6: 二维 BIT =====\n";
    vector<vector<int>> matrix = {
        {3, 0, 1, 4, 2},
        {5, 6, 3, 2, 1},
        {1, 2, 0, 1, 5},
        {4, 1, 0, 1, 7},
        {1, 0, 3, 0, 5}
    };
    BIT2D bit2d(matrix);

    cout << "sumRegion(2,1,4,3) = " << bit2d.sumRegion(2, 1, 4, 3)
         << "  (期望 8)\n";
    bit2d.update(3, 2, 2);
    cout << "update(3,2,2)后:\n";
    cout << "sumRegion(2,1,4,3) = " << bit2d.sumRegion(2, 1, 4, 3)
         << "  (期望 10)\n\n";
}
} // namespace Demo6

/* ============================================================
 * Demo7: LC 1649 — 通过指令创建有序数组
 * 每次插入一个数，代价 = min(左边比它小的, 右边比它大的)
 * ============================================================ */
namespace Demo7 {

class BIT {
    vector<int> tree;
    int n;
public:
    BIT(int n) : n(n), tree(n + 1, 0) {}
    void update(int i) { for (; i <= n; i += i & (-i)) tree[i]++; }
    int query(int i) { int s = 0; for (; i > 0; i -= i & (-i)) s += tree[i]; return s; }
};

int createSortedArray(vector<int>& instructions) {
    const int MOD = 1e9 + 7;
    int maxVal = *max_element(instructions.begin(), instructions.end());
    BIT bit(maxVal);
    long long cost = 0;

    for (int i = 0; i < (int)instructions.size(); i++) {
        int val = instructions[i];
        int less = bit.query(val - 1);          // 已插入的 < val
        int greater = i - bit.query(val);       // 已插入的 > val
        cost = (cost + min(less, greater)) % MOD;
        bit.update(val);
    }
    return (int)cost;
}

void run() {
    cout << "===== Demo7: LC 1649 创建有序数组的代价 =====\n";
    {
        vector<int> inst = {1, 5, 6, 2};
        cout << "[1,5,6,2] → " << createSortedArray(inst) << "  (期望 1)\n";
    }
    {
        vector<int> inst = {1, 2, 3, 6, 5, 4};
        cout << "[1,2,3,6,5,4] → " << createSortedArray(inst) << "  (期望 3)\n";
    }
    {
        vector<int> inst = {1, 3, 3, 3, 2, 4, 2, 1, 2};
        cout << "[1,3,3,3,2,4,2,1,2] → " << createSortedArray(inst) << "  (期望 4)\n";
    }
    cout << endl;
}
} // namespace Demo7

/* ============================================================
 * Demo8: BIT 上二分求第 K 小
 * O(log n) 在 BIT 上做倍增二分
 * ============================================================ */
namespace Demo8 {

class BIT {
    int n;
public:
    vector<int> tree;
    BIT(int n) : n(n), tree(n + 1, 0) {}
    void update(int i, int d) { for (; i <= n; i += i & (-i)) tree[i] += d; }
    int query(int i) { int s = 0; for (; i > 0; i -= i & (-i)) s += tree[i]; return s; }

    // BIT 上倍增二分 O(log n) 求第 k 小
    int kthSmallest(int k) {
        int pos = 0;
        // 找到最大的 2 的幂 ≤ n
        int logn = 0;
        while ((1 << (logn + 1)) <= n) logn++;

        for (int pw = 1 << logn; pw > 0; pw >>= 1) {
            if (pos + pw <= n && tree[pos + pw] < k) {
                pos += pw;
                k -= tree[pos];
            }
        }
        return pos + 1;
    }
};

void run() {
    cout << "===== Demo8: BIT 上二分求第 K 小 =====\n";

    // 值域 1~10, 插入一些数
    BIT bit(10);
    vector<int> insert = {3, 1, 7, 5, 3, 9, 2};
    for (int x : insert)
        bit.update(x, 1);

    cout << "插入: [3,1,7,5,3,9,2] → 排序后 [1,2,3,3,5,7,9]\n";
    for (int k = 1; k <= 7; k++) {
        cout << "  第 " << k << " 小 = " << bit.kthSmallest(k);
        if (k <= 7) cout << "\n";
    }
    cout << "(期望: 1,2,3,3,5,7,9)\n\n";
}
} // namespace Demo8

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║  专题二十（上）：树状数组 BIT (Fenwick Tree) — 示例代码   ║\n";
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

    cout << "===== 全部 Demo 运行完毕！=====\n";
    return 0;
}
