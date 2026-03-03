/*
 * 专题十九（上）：前缀和 Prefix Sum — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_19_prefix_sum.cpp -o prefix_sum
 * 运行：./prefix_sum
 *
 * Demo 列表：
 *   Demo1  — 一维前缀和模板 + LC 303 区域和检索
 *   Demo2  — LC 560  和为K的子数组（前缀和+哈希表）
 *   Demo3  — LC 523  连续的子数组和（mod k）
 *   Demo4  — LC 974  和可被K整除的子数组
 *   Demo5  — LC 525  连续数组（0→-1 映射）
 *   Demo6  — LC 304  二维区域和检索
 *   Demo7  — LC 1074 元素和为目标值的子矩阵数量
 *   Demo8  — LC 238  除自身以外数组的乘积（前缀积）
 *   Demo9  — LC 1310 子数组异或查询（前缀异或）
 *   Demo10 — LC 528  按权重随机选择（前缀和+二分）
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <random>
using namespace std;

/* ============================================================
 * Demo1: 一维前缀和模板 + LC 303 区域和检索
 * ============================================================ */
namespace Demo1 {

class NumArray {
    vector<int> pre;
public:
    NumArray(vector<int>& nums) : pre(nums.size() + 1, 0) {
        for (int i = 0; i < (int)nums.size(); i++)
            pre[i + 1] = pre[i] + nums[i];
    }
    int sumRange(int l, int r) {
        return pre[r + 1] - pre[l];
    }
};

void run() {
    cout << "===== Demo1: LC 303 一维前缀和 =====\n";
    vector<int> nums = {-2, 0, 3, -5, 2, -1};
    NumArray na(nums);
    cout << "nums = [-2, 0, 3, -5, 2, -1]\n";
    cout << "sumRange(0, 2) = " << na.sumRange(0, 2) << "  (期望 1)\n";
    cout << "sumRange(2, 5) = " << na.sumRange(2, 5) << "  (期望 -1)\n";
    cout << "sumRange(0, 5) = " << na.sumRange(0, 5) << "  (期望 -3)\n\n";
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 560 — 和为K的子数组
 * 前缀和 + 哈希表: pre[j] - pre[i] = k → 查 mp[pre[j] - k]
 * ============================================================ */
namespace Demo2 {

int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> mp;
    mp[0] = 1;  // 前缀和为0出现1次（空前缀）
    int sum = 0, count = 0;
    for (int num : nums) {
        sum += num;
        if (mp.count(sum - k))
            count += mp[sum - k];
        mp[sum]++;
    }
    return count;
}

void run() {
    cout << "===== Demo2: LC 560 和为K的子数组 =====\n";
    {
        vector<int> nums = {1, 1, 1};
        int k = 2;
        cout << "[1,1,1] k=2 → " << subarraySum(nums, k) << "  (期望 2)\n";
    }
    {
        vector<int> nums = {1, 2, 3};
        int k = 3;
        cout << "[1,2,3] k=3 → " << subarraySum(nums, k) << "  (期望 2, [1,2]和[3])\n";
    }
    {
        vector<int> nums = {1, -1, 0};
        int k = 0;
        cout << "[1,-1,0] k=0 → " << subarraySum(nums, k) << "  (期望 3)\n";
    }
    cout << endl;
}
} // namespace Demo2

/* ============================================================
 * Demo3: LC 523 — 连续的子数组和
 * 子数组和为 k 的倍数(长度≥2) → pre%k 相同且距离≥2
 * ============================================================ */
namespace Demo3 {

bool checkSubarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> mp;
    mp[0] = -1;   // 余数0出现在"位置-1"
    int sum = 0;
    for (int i = 0; i < (int)nums.size(); i++) {
        sum += nums[i];
        int mod = ((sum % k) + k) % k;  // 处理负数
        if (mp.count(mod)) {
            if (i - mp[mod] >= 2) return true;
        } else {
            mp[mod] = i;
        }
    }
    return false;
}

void run() {
    cout << "===== Demo3: LC 523 连续的子数组和 =====\n";
    {
        vector<int> nums = {23, 2, 4, 6, 7};
        int k = 6;
        cout << "[23,2,4,6,7] k=6 → " << (checkSubarraySum(nums, k) ? "true" : "false")
             << "  (期望 true, [2,4]=6)\n";
    }
    {
        vector<int> nums = {23, 2, 6, 4, 7};
        int k = 13;
        cout << "[23,2,6,4,7] k=13 → " << (checkSubarraySum(nums, k) ? "true" : "false")
             << "  (期望 false)\n";
    }
    cout << endl;
}
} // namespace Demo3

/* ============================================================
 * Demo4: LC 974 — 和可被K整除的子数组
 * 统计 pre%k 值相同的对数
 * ============================================================ */
namespace Demo4 {

int subarraysDivByK(vector<int>& nums, int k) {
    unordered_map<int, int> mp;
    mp[0] = 1;
    int sum = 0, count = 0;
    for (int num : nums) {
        sum += num;
        int mod = ((sum % k) + k) % k;
        count += mp[mod];
        mp[mod]++;
    }
    return count;
}

void run() {
    cout << "===== Demo4: LC 974 和可被K整除的子数组 =====\n";
    {
        vector<int> nums = {4, 5, 0, -2, -3, 1};
        int k = 5;
        cout << "[4,5,0,-2,-3,1] k=5 → " << subarraysDivByK(nums, k)
             << "  (期望 7)\n";
    }
    {
        vector<int> nums = {5};
        int k = 9;
        cout << "[5] k=9 → " << subarraysDivByK(nums, k)
             << "  (期望 0)\n";
    }
    cout << endl;
}
} // namespace Demo4

/* ============================================================
 * Demo5: LC 525 — 连续数组
 * 把 0 视为 -1，找和为 0 的最长子数组
 * ============================================================ */
namespace Demo5 {

int findMaxLength(vector<int>& nums) {
    unordered_map<int, int> mp;
    mp[0] = -1;
    int sum = 0, maxLen = 0;
    for (int i = 0; i < (int)nums.size(); i++) {
        sum += (nums[i] == 0 ? -1 : 1);
        if (mp.count(sum))
            maxLen = max(maxLen, i - mp[sum]);
        else
            mp[sum] = i;
    }
    return maxLen;
}

void run() {
    cout << "===== Demo5: LC 525 连续数组 =====\n";
    {
        vector<int> nums = {0, 1};
        cout << "[0,1] → " << findMaxLength(nums) << "  (期望 2)\n";
    }
    {
        vector<int> nums = {0, 1, 0};
        cout << "[0,1,0] → " << findMaxLength(nums) << "  (期望 2)\n";
    }
    {
        vector<int> nums = {0, 0, 1, 0, 0, 0, 1, 1};
        cout << "[0,0,1,0,0,0,1,1] → " << findMaxLength(nums) << "  (期望 6)\n";
    }
    cout << endl;
}
} // namespace Demo5

/* ============================================================
 * Demo6: LC 304 — 二维区域和检索
 * 二维前缀和 + 容斥原理
 * ============================================================ */
namespace Demo6 {

class NumMatrix {
    vector<vector<int>> pre;
public:
    NumMatrix(vector<vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        pre.assign(m + 1, vector<int>(n + 1, 0));
        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++)
                pre[i][j] = matrix[i-1][j-1] + pre[i-1][j]
                          + pre[i][j-1] - pre[i-1][j-1];
    }
    int sumRegion(int r1, int c1, int r2, int c2) {
        return pre[r2+1][c2+1] - pre[r1][c2+1]
             - pre[r2+1][c1] + pre[r1][c1];
    }
};

void run() {
    cout << "===== Demo6: LC 304 二维区域和检索 =====\n";
    vector<vector<int>> matrix = {
        {3, 0, 1, 4, 2},
        {5, 6, 3, 2, 1},
        {1, 2, 0, 1, 5},
        {4, 1, 0, 1, 7},
        {1, 0, 3, 0, 5}
    };
    NumMatrix nm(matrix);
    cout << "sumRegion(2,1,4,3) = " << nm.sumRegion(2, 1, 4, 3)
         << "  (期望 8)\n";
    cout << "sumRegion(1,1,2,2) = " << nm.sumRegion(1, 1, 2, 2)
         << "  (期望 11)\n";
    cout << "sumRegion(1,2,2,4) = " << nm.sumRegion(1, 2, 2, 4)
         << "  (期望 12)\n\n";
}
} // namespace Demo6

/* ============================================================
 * Demo7: LC 1074 — 元素和为目标值的子矩阵数量
 * 枚举上下行 r1,r2 → 压缩列 → 转为一维 LC 560
 * ============================================================ */
namespace Demo7 {

int numSubmatrixSumTarget(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int count = 0;

    for (int r1 = 0; r1 < m; r1++) {
        vector<int> colSum(n, 0);
        for (int r2 = r1; r2 < m; r2++) {
            // 累加当前行到列和
            for (int j = 0; j < n; j++)
                colSum[j] += matrix[r2][j];

            // 一维 LC 560: colSum 中和为 target 的子数组
            unordered_map<int, int> mp;
            mp[0] = 1;
            int sum = 0;
            for (int j = 0; j < n; j++) {
                sum += colSum[j];
                if (mp.count(sum - target))
                    count += mp[sum - target];
                mp[sum]++;
            }
        }
    }
    return count;
}

void run() {
    cout << "===== Demo7: LC 1074 元素和为目标值的子矩阵 =====\n";
    {
        vector<vector<int>> matrix = {{0,1,0},{1,1,1},{0,1,0}};
        int target = 0;
        cout << "matrix=[[0,1,0],[1,1,1],[0,1,0]], target=0 → "
             << numSubmatrixSumTarget(matrix, target) << "  (期望 4)\n";
    }
    {
        vector<vector<int>> matrix = {{1,-1},{-1,1}};
        int target = 0;
        cout << "matrix=[[1,-1],[-1,1]], target=0 → "
             << numSubmatrixSumTarget(matrix, target) << "  (期望 5)\n";
    }
    cout << endl;
}
} // namespace Demo7

/* ============================================================
 * Demo8: LC 238 — 除自身以外数组的乘积
 * 前缀积 × 后缀积，不用除法
 * ============================================================ */
namespace Demo8 {

vector<int> productExceptSelf(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, 1);

    // 前缀积: result[i] = nums[0] * ... * nums[i-1]
    int left = 1;
    for (int i = 0; i < n; i++) {
        result[i] = left;
        left *= nums[i];
    }

    // 后缀积: result[i] *= nums[i+1] * ... * nums[n-1]
    int right = 1;
    for (int i = n - 1; i >= 0; i--) {
        result[i] *= right;
        right *= nums[i];
    }
    return result;
}

void run() {
    cout << "===== Demo8: LC 238 除自身以外数组的乘积 =====\n";
    {
        vector<int> nums = {1, 2, 3, 4};
        auto res = productExceptSelf(nums);
        cout << "[1,2,3,4] → [";
        for (int i = 0; i < (int)res.size(); i++) {
            if (i) cout << ",";
            cout << res[i];
        }
        cout << "]  (期望 [24,12,8,6])\n";
    }
    {
        vector<int> nums = {-1, 1, 0, -3, 3};
        auto res = productExceptSelf(nums);
        cout << "[-1,1,0,-3,3] → [";
        for (int i = 0; i < (int)res.size(); i++) {
            if (i) cout << ",";
            cout << res[i];
        }
        cout << "]  (期望 [0,0,9,0,0])\n";
    }
    cout << endl;
}
} // namespace Demo8

/* ============================================================
 * Demo9: LC 1310 — 子数组异或查询
 * 前缀异或: pre_xor[r+1] ^ pre_xor[l]
 * ============================================================ */
namespace Demo9 {

vector<int> xorQueries(vector<int>& arr, vector<vector<int>>& queries) {
    int n = arr.size();
    vector<int> preXor(n + 1, 0);
    for (int i = 0; i < n; i++)
        preXor[i + 1] = preXor[i] ^ arr[i];

    vector<int> result;
    for (auto& q : queries)
        result.push_back(preXor[q[1] + 1] ^ preXor[q[0]]);
    return result;
}

void run() {
    cout << "===== Demo9: LC 1310 子数组异或查询 =====\n";
    vector<int> arr = {1, 3, 4, 8};
    vector<vector<int>> queries = {{0,1},{1,2},{0,3},{3,3}};
    auto res = xorQueries(arr, queries);
    cout << "arr=[1,3,4,8]\n";
    cout << "queries: [0,1]→" << res[0] << " [1,2]→" << res[1]
         << " [0,3]→" << res[2] << " [3,3]→" << res[3] << "\n";
    cout << "(期望 [2,7,14,8])\n\n";
}
} // namespace Demo9

/* ============================================================
 * Demo10: LC 528 — 按权重随机选择
 * 前缀和 + 二分查找
 * ============================================================ */
namespace Demo10 {

class Solution {
    vector<int> pre;
    mt19937 rng;
public:
    Solution(vector<int>& w) : rng(42) {
        pre.resize(w.size());
        pre[0] = w[0];
        for (int i = 1; i < (int)w.size(); i++)
            pre[i] = pre[i - 1] + w[i];
    }

    int pickIndex() {
        // 随机 [1, total]
        int target = rng() % pre.back() + 1;
        // 二分：找第一个 pre[i] >= target
        return (int)(lower_bound(pre.begin(), pre.end(), target) - pre.begin());
    }
};

void run() {
    cout << "===== Demo10: LC 528 按权重随机选择 =====\n";
    vector<int> w = {1, 3, 2};
    Solution sol(w);

    // 统计 10000 次的分布
    vector<int> cnt(3, 0);
    for (int i = 0; i < 10000; i++)
        cnt[sol.pickIndex()]++;

    cout << "权重 [1,3,2], 期望比例 1:3:2\n";
    cout << "10000次采样: idx0=" << cnt[0] << " idx1=" << cnt[1]
         << " idx2=" << cnt[2] << "\n";
    double total = 10000.0;
    printf("实际比例: %.2f : %.2f : %.2f\n\n",
           cnt[0]/total*6, cnt[1]/total*6, cnt[2]/total*6);
}
} // namespace Demo10

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║  专题十九（上）：前缀和 Prefix Sum — 示例代码            ║\n";
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
    Demo10::run();

    cout << "===== 全部 Demo 运行完毕！=====\n";
    return 0;
}
