/*
 * 专题十九（下）：差分数组 Difference Array — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_19_difference_array.cpp -o diff_array
 * 运行：./diff_array
 *
 * Demo 列表：
 *   Demo1  — 一维差分模板演示
 *   Demo2  — LC 1094 拼车
 *   Demo3  — LC 1109 航班预订统计
 *   Demo4  — LC 253  会议室 II（差分思维+map）
 *   Demo5  — LC 732  我的日程安排表 III
 *   Demo6  — LC 2536 子矩阵元素加1（二维差分）
 *   Demo7  — LC 995  K连续位的最小翻转次数
 *   Demo8  — 前缀和 ↔ 差分 互逆关系演示
 */

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

/* ============================================================
 * Demo1: 一维差分模板演示
 * ============================================================ */
namespace Demo1 {

void rangeAdd(vector<int>& diff, int l, int r, int val) {
    diff[l] += val;
    if (r + 1 < (int)diff.size())
        diff[r + 1] -= val;
}

vector<int> restore(vector<int>& diff) {
    int n = diff.size();
    vector<int> nums(n);
    nums[0] = diff[0];
    for (int i = 1; i < n; i++)
        nums[i] = nums[i - 1] + diff[i];
    return nums;
}

void run() {
    cout << "===== Demo1: 一维差分模板 =====\n";
    int n = 6;
    vector<int> diff(n, 0);

    // 操作1: [1,3] +2
    rangeAdd(diff, 1, 3, 2);
    cout << "操作1: [1,3] +2\n";
    auto nums = restore(diff);
    cout << "  结果: [";
    for (int i = 0; i < n; i++) { if (i) cout << ","; cout << nums[i]; }
    cout << "]  (期望 [0,2,2,2,0,0])\n";

    // 操作2: [2,5] +3
    rangeAdd(diff, 2, 5, 3);
    cout << "操作2: [2,5] +3\n";
    nums = restore(diff);
    cout << "  结果: [";
    for (int i = 0; i < n; i++) { if (i) cout << ","; cout << nums[i]; }
    cout << "]  (期望 [0,2,5,5,3,3])\n";

    // 操作3: [0,4] -1
    rangeAdd(diff, 0, 4, -1);
    cout << "操作3: [0,4] -1\n";
    nums = restore(diff);
    cout << "  结果: [";
    for (int i = 0; i < n; i++) { if (i) cout << ","; cout << nums[i]; }
    cout << "]  (期望 [-1,1,4,4,2,3])\n\n";
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 1094 — 拼车
 * diff[from] += passengers, diff[to] -= passengers
 * ============================================================ */
namespace Demo2 {

bool carPooling(vector<vector<int>>& trips, int capacity) {
    vector<int> diff(1001, 0);
    for (auto& t : trips) {
        diff[t[1]] += t[0];   // 上车
        diff[t[2]] -= t[0];   // 到站下车
    }
    int passengers = 0;
    for (int i = 0; i < 1001; i++) {
        passengers += diff[i];
        if (passengers > capacity) return false;
    }
    return true;
}

void run() {
    cout << "===== Demo2: LC 1094 拼车 =====\n";
    {
        vector<vector<int>> trips = {{2,1,5},{3,3,7}};
        int cap = 4;
        cout << "trips=[[2,1,5],[3,3,7]], cap=4 → "
             << (carPooling(trips, cap) ? "true" : "false")
             << "  (期望 false, 3~5时有5人>4)\n";
    }
    {
        vector<vector<int>> trips = {{2,1,5},{3,3,7}};
        int cap = 5;
        cout << "trips=[[2,1,5],[3,3,7]], cap=5 → "
             << (carPooling(trips, cap) ? "true" : "false")
             << "  (期望 true)\n";
    }
    {
        vector<vector<int>> trips = {{2,1,5},{3,5,7}};
        int cap = 3;
        cout << "trips=[[2,1,5],[3,5,7]], cap=3 → "
             << (carPooling(trips, cap) ? "true" : "false")
             << "  (期望 true, 不重叠)\n";
    }
    cout << endl;
}
} // namespace Demo2

/* ============================================================
 * Demo3: LC 1109 — 航班预订统计
 * bookings[i] = [first, last, seats], 1-indexed
 * ============================================================ */
namespace Demo3 {

vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n) {
    vector<int> diff(n + 1, 0);
    for (auto& b : bookings) {
        diff[b[0] - 1] += b[2];   // 转 0-indexed
        if (b[1] < n)
            diff[b[1]] -= b[2];
    }
    vector<int> result(n);
    result[0] = diff[0];
    for (int i = 1; i < n; i++)
        result[i] = result[i - 1] + diff[i];
    return result;
}

void run() {
    cout << "===== Demo3: LC 1109 航班预订统计 =====\n";
    {
        vector<vector<int>> bookings = {{1,2,10},{2,3,20},{2,5,25}};
        int n = 5;
        auto res = corpFlightBookings(bookings, n);
        cout << "bookings=[[1,2,10],[2,3,20],[2,5,25]], n=5\n  结果: [";
        for (int i = 0; i < n; i++) { if (i) cout << ","; cout << res[i]; }
        cout << "]  (期望 [10,55,45,25,25])\n";
    }
    {
        vector<vector<int>> bookings = {{1,2,10},{2,2,15}};
        int n = 2;
        auto res = corpFlightBookings(bookings, n);
        cout << "bookings=[[1,2,10],[2,2,15]], n=2\n  结果: [";
        for (int i = 0; i < n; i++) { if (i) cout << ","; cout << res[i]; }
        cout << "]  (期望 [10,25])\n";
    }
    cout << endl;
}
} // namespace Demo3

/* ============================================================
 * Demo4: LC 253 — 会议室 II
 * 差分思维: 每个会议 [start,end) → diff[start]++, diff[end]--
 * 扫描最大同时进行的会议数
 * ============================================================ */
namespace Demo4 {

int minMeetingRooms(vector<vector<int>>& intervals) {
    map<int, int> diff;
    for (auto& iv : intervals) {
        diff[iv[0]]++;     // 开始 +1
        diff[iv[1]]--;     // 结束 -1
    }
    int maxRooms = 0, cur = 0;
    for (auto& [time, delta] : diff) {
        cur += delta;
        maxRooms = max(maxRooms, cur);
    }
    return maxRooms;
}

void run() {
    cout << "===== Demo4: LC 253 会议室 II =====\n";
    {
        vector<vector<int>> intervals = {{0,30},{5,10},{15,20}};
        cout << "[[0,30],[5,10],[15,20]] → " << minMeetingRooms(intervals)
             << " 间  (期望 2)\n";
    }
    {
        vector<vector<int>> intervals = {{7,10},{2,4}};
        cout << "[[7,10],[2,4]] → " << minMeetingRooms(intervals)
             << " 间  (期望 1)\n";
    }
    {
        vector<vector<int>> intervals = {{1,5},{2,6},{3,7},{4,8}};
        cout << "[[1,5],[2,6],[3,7],[4,8]] → " << minMeetingRooms(intervals)
             << " 间  (期望 4)\n";
    }
    cout << endl;
}
} // namespace Demo4

/* ============================================================
 * Demo5: LC 732 — 我的日程安排表 III
 * 差分 + map: 每次 book(start, end) 返回当前最大重叠
 * ============================================================ */
namespace Demo5 {

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
    cout << "===== Demo5: LC 732 我的日程安排表 III =====\n";
    MyCalendarThree cal;

    vector<pair<int,int>> bookings = {{10,20},{50,60},{10,40},{5,15},{5,10},{25,55}};
    vector<int> expected = {1, 1, 2, 3, 3, 3};

    for (int i = 0; i < (int)bookings.size(); i++) {
        int res = cal.book(bookings[i].first, bookings[i].second);
        cout << "  book(" << bookings[i].first << "," << bookings[i].second
             << ") → " << res << "  (期望 " << expected[i] << ")\n";
    }
    cout << endl;
}
} // namespace Demo5

/* ============================================================
 * Demo6: LC 2536 — 子矩阵元素加1（二维差分）
 * ============================================================ */
namespace Demo6 {

vector<vector<int>> rangeAddQueries(int n, vector<vector<int>>& queries) {
    // 二维差分数组 (多一行一列)
    vector<vector<int>> diff(n + 1, vector<int>(n + 1, 0));
    for (auto& q : queries) {
        int r1 = q[0], c1 = q[1], r2 = q[2], c2 = q[3];
        diff[r1][c1]++;
        if (c2 + 1 <= n) diff[r1][c2 + 1]--;
        if (r2 + 1 <= n) diff[r2 + 1][c1]--;
        if (r2 + 1 <= n && c2 + 1 <= n) diff[r2 + 1][c2 + 1]++;
    }
    // 二维前缀和还原
    vector<vector<int>> result(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            result[i][j] = diff[i][j];
            if (i > 0) result[i][j] += result[i-1][j];
            if (j > 0) result[i][j] += result[i][j-1];
            if (i > 0 && j > 0) result[i][j] -= result[i-1][j-1];
        }
    return result;
}

void run() {
    cout << "===== Demo6: LC 2536 子矩阵元素加1 (二维差分) =====\n";
    {
        int n = 3;
        vector<vector<int>> queries = {{1,1,2,2},{0,0,1,1}};
        auto res = rangeAddQueries(n, queries);
        cout << "n=3, queries=[[1,1,2,2],[0,0,1,1]]\n";
        for (int i = 0; i < n; i++) {
            cout << "  [";
            for (int j = 0; j < n; j++) { if (j) cout << ","; cout << res[i][j]; }
            cout << "]\n";
        }
        cout << "  (期望 [[1,1,0],[1,2,1],[0,1,1]])\n";
    }
    {
        int n = 2;
        vector<vector<int>> queries = {{0,0,1,1}};
        auto res = rangeAddQueries(n, queries);
        cout << "n=2, queries=[[0,0,1,1]]\n";
        for (int i = 0; i < n; i++) {
            cout << "  [";
            for (int j = 0; j < n; j++) { if (j) cout << ","; cout << res[i][j]; }
            cout << "]\n";
        }
        cout << "  (期望 [[1,1],[1,1]])\n";
    }
    cout << endl;
}
} // namespace Demo6

/* ============================================================
 * Demo7: LC 995 — K 连续位的最小翻转次数
 * 贪心 + 差分标记翻转区间
 * ============================================================ */
namespace Demo7 {

int minKBitFlips(vector<int>& nums, int k) {
    int n = nums.size();
    vector<int> diff(n + 1, 0);  // 差分记录翻转
    int flips = 0, flipCount = 0;

    for (int i = 0; i < n; i++) {
        flipCount += diff[i];  // 当前位置的累计翻转次数

        // 如果当前位经过翻转后仍为 0 → 需要翻转
        if ((nums[i] + flipCount) % 2 == 0) {
            if (i + k > n) return -1;  // 超出范围
            flips++;
            flipCount++;
            diff[i + k]--;  // 翻转影响在 i+k 处结束
        }
    }
    return flips;
}

void run() {
    cout << "===== Demo7: LC 995 K连续位的最小翻转次数 =====\n";
    {
        vector<int> nums = {0, 1, 0};
        int k = 1;
        cout << "[0,1,0] k=1 → " << minKBitFlips(nums, k) << "  (期望 2)\n";
    }
    {
        vector<int> nums = {1, 1, 0};
        int k = 2;
        cout << "[1,1,0] k=2 → " << minKBitFlips(nums, k) << "  (期望 -1)\n";
    }
    {
        vector<int> nums = {0, 0, 0, 1, 0, 1, 1, 0};
        int k = 3;
        cout << "[0,0,0,1,0,1,1,0] k=3 → " << minKBitFlips(nums, k) << "  (期望 3)\n";
    }
    cout << endl;
}
} // namespace Demo7

/* ============================================================
 * Demo8: 前缀和 ↔ 差分 互逆关系演示
 * ============================================================ */
namespace Demo8 {

void run() {
    cout << "===== Demo8: 前缀和 ↔ 差分 互逆关系 =====\n";
    vector<int> nums = {3, 1, 4, 1, 5, 9};
    int n = nums.size();

    // 1. 原数组 → 差分
    vector<int> diff(n);
    diff[0] = nums[0];
    for (int i = 1; i < n; i++)
        diff[i] = nums[i] - nums[i - 1];

    cout << "原数组:  [";
    for (int i = 0; i < n; i++) { if (i) cout << ","; cout << nums[i]; }
    cout << "]\n";

    cout << "差分数组: [";
    for (int i = 0; i < n; i++) { if (i) cout << ","; cout << diff[i]; }
    cout << "]\n";

    // 2. 差分 → 前缀和还原
    vector<int> restored(n);
    restored[0] = diff[0];
    for (int i = 1; i < n; i++)
        restored[i] = restored[i - 1] + diff[i];

    cout << "还原数组: [";
    for (int i = 0; i < n; i++) { if (i) cout << ","; cout << restored[i]; }
    cout << "]  (与原数组一致 ✓)\n";

    // 3. 原数组 → 前缀和
    vector<int> pre(n + 1, 0);
    for (int i = 0; i < n; i++)
        pre[i + 1] = pre[i] + nums[i];

    cout << "\n前缀和:   [";
    for (int i = 0; i <= n; i++) { if (i) cout << ","; cout << pre[i]; }
    cout << "]\n";

    // 4. 前缀和 → 差分还原
    vector<int> diff2(n);
    diff2[0] = pre[1] - pre[0];
    for (int i = 1; i < n; i++)
        diff2[i] = pre[i + 1] - pre[i];

    cout << "前缀和差分: [";
    for (int i = 0; i < n; i++) { if (i) cout << ","; cout << diff2[i]; }
    cout << "]  (与原数组一致 ✓)\n";

    cout << "\n结论: 差分和前缀和互为逆运算!\n";
    cout << "  差分(原数组) → 差分数组 → 前缀和(差分数组) = 原数组\n";
    cout << "  前缀和(原数组) → 前缀和数组 → 差分(前缀和数组) = 原数组\n\n";
}
} // namespace Demo8

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║  专题十九（下）：差分数组 Difference Array — 示例代码     ║\n";
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
