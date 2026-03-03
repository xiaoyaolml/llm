/*
 * 专题十七（下）：单调队列 Monotonic Deque — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_17_monotonic_queue.cpp -o mono_queue
 * 运行：./mono_queue
 *
 * Demo 列表：
 *   Demo1  — LC 239  滑动窗口最大值（经典单调队列）
 *   Demo2  — 滑动窗口最小值 模板演示
 *   Demo3  — LC 862  和至少为K的最短子数组（前缀和+单调队列）
 *   Demo4  — LC 1438 绝对差不超过限制的最长连续子数组
 *   Demo5  — LC 1696 跳跃游戏 VI（单调队列优化DP）
 *   Demo6  — LC 1499 满足不等式的最大值
 *   Demo7  — 单调队列 vs 单调栈 对比演示
 */

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <climits>
using namespace std;

/* ============================================================
 * Demo1: LC 239 — 滑动窗口最大值（最经典的单调队列题）
 * ============================================================ */
namespace Demo1 {

vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;  // 存下标, 值从大到小（单调递减）
    vector<int> result;

    for (int i = 0; i < (int)nums.size(); i++) {
        // 1. 弹出过期的队首
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        // 2. 弹出不如当前的队尾（维护递减）
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();

        // 3. 入队
        dq.push_back(i);

        // 4. 窗口形成后，队首就是最大值
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}

void run() {
    cout << "===== Demo1: LC 239 滑动窗口最大值 =====\n";
    {
        vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
        int k = 3;
        auto res = maxSlidingWindow(nums, k);
        cout << "nums=[1,3,-1,-3,5,3,6,7], k=3\n结果: [";
        for (int i = 0; i < (int)res.size(); i++) {
            if (i) cout << ",";
            cout << res[i];
        }
        cout << "]  (期望 [3,3,5,5,6,7])\n";
    }
    {
        vector<int> nums = {1};
        int k = 1;
        auto res = maxSlidingWindow(nums, k);
        cout << "nums=[1], k=1 → [" << res[0] << "]  (期望 [1])\n";
    }
    {
        vector<int> nums = {1, -1};
        int k = 1;
        auto res = maxSlidingWindow(nums, k);
        cout << "nums=[1,-1], k=1 → [";
        for (int i = 0; i < (int)res.size(); i++) {
            if (i) cout << ",";
            cout << res[i];
        }
        cout << "]  (期望 [1,-1])\n";
    }
    cout << endl;
}
} // namespace Demo1

/* ============================================================
 * Demo2: 滑动窗口最小值 模板演示
 * ============================================================ */
namespace Demo2 {

vector<int> minSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;  // 单调递增队列，队首最小
    vector<int> result;

    for (int i = 0; i < (int)nums.size(); i++) {
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        while (!dq.empty() && nums[dq.back()] >= nums[i])
            dq.pop_back();

        dq.push_back(i);

        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}

void run() {
    cout << "===== Demo2: 滑动窗口最小值 =====\n";
    vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    auto res = minSlidingWindow(nums, k);
    cout << "nums=[1,3,-1,-3,5,3,6,7], k=3\n结果: [";
    for (int i = 0; i < (int)res.size(); i++) {
        if (i) cout << ",";
        cout << res[i];
    }
    cout << "]  (期望 [-1,-3,-3,-3,3,3])\n\n";
}
} // namespace Demo2

/* ============================================================
 * Demo3: LC 862 — 和至少为K的最短子数组
 * 前缀和 + 单调递增队列
 * ============================================================ */
namespace Demo3 {

int shortestSubarray(vector<int>& nums, int k) {
    int n = nums.size();
    vector<long long> pre(n + 1, 0);
    for (int i = 0; i < n; i++)
        pre[i + 1] = pre[i] + nums[i];

    deque<int> dq;
    int minLen = n + 1;

    for (int j = 0; j <= n; j++) {
        // 队首满足 pre[j] - pre[front] >= k → 记录答案并弹出
        while (!dq.empty() && pre[j] - pre[dq.front()] >= k) {
            minLen = min(minLen, j - dq.front());
            dq.pop_front();
        }
        // 维护递增: 弹出 >= pre[j] 的队尾
        while (!dq.empty() && pre[dq.back()] >= pre[j])
            dq.pop_back();
        dq.push_back(j);
    }
    return minLen <= n ? minLen : -1;
}

void run() {
    cout << "===== Demo3: LC 862 和至少为K的最短子数组 =====\n";
    {
        vector<int> nums = {1};
        int k = 1;
        cout << "nums=[1], k=1 → " << shortestSubarray(nums, k)
             << "  (期望 1)\n";
    }
    {
        vector<int> nums = {1, 2};
        int k = 4;
        cout << "nums=[1,2], k=4 → " << shortestSubarray(nums, k)
             << "  (期望 -1)\n";
    }
    {
        vector<int> nums = {2, -1, 2};
        int k = 3;
        cout << "nums=[2,-1,2], k=3 → " << shortestSubarray(nums, k)
             << "  (期望 3)\n";
    }
    {
        vector<int> nums = {84, -37, 32, 40, 95};
        int k = 167;
        cout << "nums=[84,-37,32,40,95], k=167 → " << shortestSubarray(nums, k)
             << "  (期望 3)\n";
    }
    cout << endl;
}
} // namespace Demo3

/* ============================================================
 * Demo4: LC 1438 — 绝对差不超过限制的最长连续子数组
 * 双指针 + 两个单调队列（最大 & 最小）
 * ============================================================ */
namespace Demo4 {

int longestSubarray(vector<int>& nums, int limit) {
    deque<int> maxDq, minDq;  // 递减→最大, 递增→最小
    int left = 0, result = 0;

    for (int right = 0; right < (int)nums.size(); right++) {
        // 维护最大值递减队列
        while (!maxDq.empty() && nums[maxDq.back()] <= nums[right])
            maxDq.pop_back();
        maxDq.push_back(right);

        // 维护最小值递增队列
        while (!minDq.empty() && nums[minDq.back()] >= nums[right])
            minDq.pop_back();
        minDq.push_back(right);

        // 收缩: 当 max - min > limit
        while (nums[maxDq.front()] - nums[minDq.front()] > limit) {
            left++;
            if (maxDq.front() < left) maxDq.pop_front();
            if (minDq.front() < left) minDq.pop_front();
        }

        result = max(result, right - left + 1);
    }
    return result;
}

void run() {
    cout << "===== Demo4: LC 1438 绝对差不超过限制的最长子数组 =====\n";
    {
        vector<int> nums = {8, 2, 4, 7};
        int limit = 4;
        cout << "nums=[8,2,4,7], limit=4 → " << longestSubarray(nums, limit)
             << "  (期望 2)\n";
    }
    {
        vector<int> nums = {10, 1, 2, 4, 7, 2};
        int limit = 5;
        cout << "nums=[10,1,2,4,7,2], limit=5 → " << longestSubarray(nums, limit)
             << "  (期望 4)\n";
    }
    {
        vector<int> nums = {4, 2, 2, 2, 4, 4, 2, 2};
        int limit = 0;
        cout << "nums=[4,2,2,2,4,4,2,2], limit=0 → " << longestSubarray(nums, limit)
             << "  (期望 3)\n";
    }
    cout << endl;
}
} // namespace Demo4

/* ============================================================
 * Demo5: LC 1696 — 跳跃游戏 VI（单调队列优化DP）
 * dp[i] = nums[i] + max(dp[i-k], ..., dp[i-1])
 * ============================================================ */
namespace Demo5 {

int maxResult(vector<int>& nums, int k) {
    int n = nums.size();
    vector<int> dp(n);
    deque<int> dq;  // 维护 dp 值的递减队列

    dp[0] = nums[0];
    dq.push_back(0);

    for (int i = 1; i < n; i++) {
        // 弹过期
        while (!dq.empty() && dq.front() < i - k)
            dq.pop_front();

        // dp[i] = nums[i] + 窗口内dp最大值
        dp[i] = nums[i] + dp[dq.front()];

        // 维护递减
        while (!dq.empty() && dp[dq.back()] <= dp[i])
            dq.pop_back();
        dq.push_back(i);
    }
    return dp[n - 1];
}

void run() {
    cout << "===== Demo5: LC 1696 跳跃游戏 VI (DP+单调队列) =====\n";
    {
        vector<int> nums = {1, -1, -2, 4, -7, 3};
        int k = 2;
        cout << "nums=[1,-1,-2,4,-7,3], k=2 → " << maxResult(nums, k)
             << "  (期望 7, 路径: 1→-1→4→3)\n";
    }
    {
        vector<int> nums = {10, -5, -2, 4, 0, 3};
        int k = 3;
        cout << "nums=[10,-5,-2,4,0,3], k=3 → " << maxResult(nums, k)
             << "  (期望 17, 路径: 10→4→3)\n";
    }
    {
        vector<int> nums = {1, -5, -20, 4, -1, 3, -6, -3};
        int k = 2;
        cout << "nums=[1,-5,-20,4,-1,3,-6,-3], k=2 → " << maxResult(nums, k)
             << "  (期望 0)\n";
    }
    cout << endl;
}
} // namespace Demo5

/* ============================================================
 * Demo6: LC 1499 — 满足不等式的最大值
 * yi + yj + |xi - xj|, 其中 xi 严格递增, xj - xi ≤ k
 * = (yj + xj) + max{yi - xi}  → 单调队列维护
 * ============================================================ */
namespace Demo6 {

int findMaxValueOfEquation(vector<vector<int>>& points, int k) {
    deque<pair<int, int>> dq;  // {yi - xi, xi}
    int maxVal = INT_MIN;

    for (auto& p : points) {
        int xj = p[0], yj = p[1];

        // 弹过期: xj - xi > k
        while (!dq.empty() && xj - dq.front().second > k)
            dq.pop_front();

        // 更新答案
        if (!dq.empty())
            maxVal = max(maxVal, yj + xj + dq.front().first);

        // 维护递减 (按 yi - xi 递减)
        int val = yj - xj;
        while (!dq.empty() && dq.back().first <= val)
            dq.pop_back();
        dq.push_back({val, xj});
    }
    return maxVal;
}

void run() {
    cout << "===== Demo6: LC 1499 满足不等式的最大值 =====\n";
    {
        vector<vector<int>> pts = {{1,3},{2,0},{5,10},{6,-10}};
        int k = 1;
        cout << "points=[[1,3],[2,0],[5,10],[6,-10]], k=1\n";
        cout << "结果: " << findMaxValueOfEquation(pts, k)
             << "  (期望 4, 选 i=0,j=1: 3+0+|1-2|=4)\n";
    }
    {
        vector<vector<int>> pts = {{0,0},{3,0},{9,2}};
        int k = 3;
        cout << "points=[[0,0],[3,0],[9,2]], k=3\n";
        cout << "结果: " << findMaxValueOfEquation(pts, k)
             << "  (期望 3, 选 i=0,j=1: 0+0+3=3)\n";
    }
    cout << endl;
}
} // namespace Demo6

/* ============================================================
 * Demo7: 单调队列 vs 单调栈 对比演示
 * 同一数组，展示两者的不同关注点
 * ============================================================ */
namespace Demo7 {

void run() {
    cout << "===== Demo7: 单调队列 vs 单调栈 对比 =====\n";
    vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;

    // --- 单调栈: 每个元素的右边第一个更大值 ---
    cout << "--- 单调栈: 右边第一个更大元素 ---\n";
    {
        int n = nums.size();
        vector<int> nge(n, -1);
        vector<int> stk;
        for (int i = 0; i < n; i++) {
            while (!stk.empty() && nums[i] > nums[stk.back()]) {
                nge[stk.back()] = nums[i];
                stk.pop_back();
            }
            stk.push_back(i);
        }
        cout << "nums: [";
        for (int i = 0; i < n; i++) { if (i) cout << ","; cout << nums[i]; }
        cout << "]\nNGE:  [";
        for (int i = 0; i < n; i++) { if (i) cout << ","; cout << nge[i]; }
        cout << "]\n";
    }

    // --- 单调队列: 窗口k内最大值 ---
    cout << "\n--- 单调队列: 窗口k=" << k << "内最大值 ---\n";
    {
        deque<int> dq;
        vector<int> res;
        for (int i = 0; i < (int)nums.size(); i++) {
            if (!dq.empty() && dq.front() <= i - k) dq.pop_front();
            while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
            dq.push_back(i);
            if (i >= k - 1) res.push_back(nums[dq.front()]);
        }
        cout << "窗口最大: [";
        for (int i = 0; i < (int)res.size(); i++) { if (i) cout << ","; cout << res[i]; }
        cout << "]\n";
    }

    cout << "\n区别:\n";
    cout << "  单调栈: 关注每个元素的\"最近更大/更小\" (无窗口限制)\n";
    cout << "  单调队列: 关注固定窗口内的极值 (有窗口过期)\n\n";
}
} // namespace Demo7

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║  专题十七（下）：单调队列 Monotonic Deque — 示例代码      ║\n";
    cout << "║  作者：大胖超 😜                                        ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n\n";

    Demo1::run();
    Demo2::run();
    Demo3::run();
    Demo4::run();
    Demo5::run();
    Demo6::run();
    Demo7::run();

    cout << "===== 全部 Demo 运行完毕！=====\n";
    return 0;
}
