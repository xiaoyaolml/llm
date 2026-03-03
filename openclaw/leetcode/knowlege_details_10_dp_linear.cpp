/**
 * ============================================================
 *   专题十（上）：DP基础、线性DP与状态机DP 示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_10_dp_linear knowlege_details_10_dp_linear.cpp
 *   配合 knowlege_details_10_dp_linear.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <functional>
using namespace std;

void printVec(const vector<int>& v, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    cout << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        if (i) cout << ", ";
        cout << v[i];
    }
    cout << "]" << endl;
}

void printDP(const vector<vector<int>>& dp, const string& label = "") {
    if (!label.empty()) cout << label << ":" << endl;
    for (auto& row : dp) {
        cout << "  ";
        for (int v : row) cout << v << "\t";
        cout << endl;
    }
}

// ============================================================
//  Demo 1: 斐波那契型 DP
//  LC 509: 斐波那契数
//  LC 70:  爬楼梯
//  LC 746: 使用最小花费爬楼梯
// ============================================================

int fib(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int c = a + b; a = b; b = c;
    }
    return b;
}

int climbStairs(int n) {
    if (n <= 2) return n;
    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; i++) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

int minCostClimbingStairs(vector<int>& cost) {
    int n = cost.size();
    int prev2 = 0, prev1 = 0;
    for (int i = 2; i <= n; i++) {
        int curr = min(prev1 + cost[i - 1], prev2 + cost[i - 2]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

void demo_fibonacci() {
    cout << "\n===== Demo 1: 斐波那契型 DP =====" << endl;

    // LC 509
    cout << "\n--- LC 509: 斐波那契数 ---" << endl;
    for (int n : {0, 1, 2, 3, 5, 10})
        cout << "  fib(" << n << ") = " << fib(n) << endl;

    // LC 70
    cout << "\n--- LC 70: 爬楼梯 ---" << endl;
    for (int n : {1, 2, 3, 5, 10})
        cout << "  n=" << n << " → " << climbStairs(n) << " 种方法" << endl;

    // LC 746
    cout << "\n--- LC 746: 最小花费爬楼梯 ---" << endl;
    vector<int> cost1 = {10, 15, 20};
    printVec(cost1, "  cost");
    cout << "  最小花费: " << minCostClimbingStairs(cost1) << " (期望=15)" << endl;
    vector<int> cost2 = {1, 100, 1, 1, 1, 100, 1, 1, 100, 1};
    printVec(cost2, "  cost");
    cout << "  最小花费: " << minCostClimbingStairs(cost2) << " (期望=6)" << endl;
}

// ============================================================
//  Demo 2: 打家劫舍系列
//  LC 198: 打家劫舍
//  LC 213: 打家劫舍 II
// ============================================================

int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 1) return nums[0];
    int prev2 = nums[0], prev1 = max(nums[0], nums[1]);
    for (int i = 2; i < n; i++) {
        int curr = max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

int rob2(vector<int>& nums) {
    int n = nums.size();
    if (n == 1) return nums[0];
    auto robRange = [&](int lo, int hi) -> int {
        int prev2 = 0, prev1 = 0;
        for (int i = lo; i <= hi; i++) {
            int curr = max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = curr;
        }
        return prev1;
    };
    return max(robRange(0, n - 2), robRange(1, n - 1));
}

void demo_rob() {
    cout << "\n===== Demo 2: 打家劫舍系列 =====" << endl;

    // LC 198
    cout << "\n--- LC 198: 打家劫舍 ---" << endl;
    vector<int> h1 = {1, 2, 3, 1};
    printVec(h1, "  房屋");
    cout << "  最大金额: " << rob(h1) << " (期望=4, 偷 1+3)" << endl;

    vector<int> h2 = {2, 7, 9, 3, 1};
    printVec(h2, "  房屋");
    cout << "  最大金额: " << rob(h2) << " (期望=12, 偷 2+9+1)" << endl;

    // LC 213
    cout << "\n--- LC 213: 打家劫舍 II（环形）---" << endl;
    vector<int> h3 = {2, 3, 2};
    printVec(h3, "  房屋");
    cout << "  最大金额: " << rob2(h3) << " (期望=3)" << endl;

    vector<int> h4 = {1, 2, 3, 1};
    printVec(h4, "  房屋");
    cout << "  最大金额: " << rob2(h4) << " (期望=4)" << endl;

    vector<int> h5 = {200, 3, 140, 20, 10};
    printVec(h5, "  房屋");
    cout << "  最大金额: " << rob2(h5) << " (期望=340, 偷 200+140)" << endl;
}

// ============================================================
//  Demo 3: 最大子数组和 & 乘积最大
//  LC 53:  最大子数组和
//  LC 152: 乘积最大子数组
// ============================================================

int maxSubArray(vector<int>& nums) {
    int maxSum = nums[0], curSum = nums[0];
    for (int i = 1; i < (int)nums.size(); i++) {
        curSum = max(nums[i], curSum + nums[i]);
        maxSum = max(maxSum, curSum);
    }
    return maxSum;
}

int maxProduct(vector<int>& nums) {
    int maxProd = nums[0], curMax = nums[0], curMin = nums[0];
    for (int i = 1; i < (int)nums.size(); i++) {
        if (nums[i] < 0) swap(curMax, curMin);
        curMax = max(nums[i], curMax * nums[i]);
        curMin = min(nums[i], curMin * nums[i]);
        maxProd = max(maxProd, curMax);
    }
    return maxProd;
}

void demo_subarray() {
    cout << "\n===== Demo 3: 最大子数组和 & 乘积 =====" << endl;

    // LC 53
    cout << "\n--- LC 53: 最大子数组和 ---" << endl;
    vector<int> a1 = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    printVec(a1, "  数组");
    cout << "  最大子数组和: " << maxSubArray(a1) << " (期望=6, [4,-1,2,1])" << endl;

    vector<int> a2 = {-1};
    cout << "  [-1] → " << maxSubArray(a2) << " (期望=-1)" << endl;

    vector<int> a3 = {5, 4, -1, 7, 8};
    printVec(a3, "  数组");
    cout << "  最大子数组和: " << maxSubArray(a3) << " (期望=23)" << endl;

    // LC 152
    cout << "\n--- LC 152: 乘积最大子数组 ---" << endl;
    vector<int> b1 = {2, 3, -2, 4};
    printVec(b1, "  数组");
    cout << "  最大乘积: " << maxProduct(b1) << " (期望=6, [2,3])" << endl;

    vector<int> b2 = {-2, 0, -1};
    printVec(b2, "  数组");
    cout << "  最大乘积: " << maxProduct(b2) << " (期望=0)" << endl;

    vector<int> b3 = {-2, 3, -4};
    printVec(b3, "  数组");
    cout << "  最大乘积: " << maxProduct(b3) << " (期望=24, 负负得正)" << endl;
}

// ============================================================
//  Demo 4: LIS
//  LC 300: 最长递增子序列
// ============================================================

int lengthOfLIS_dp(vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 1);
    int maxLen = 1;
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++)
            if (nums[j] < nums[i])
                dp[i] = max(dp[i], dp[j] + 1);
        maxLen = max(maxLen, dp[i]);
    }
    return maxLen;
}

int lengthOfLIS_fast(vector<int>& nums) {
    vector<int> tails;
    for (int num : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), num);
        if (it == tails.end()) tails.push_back(num);
        else *it = num;
    }
    return tails.size();
}

void demo_lis() {
    cout << "\n===== Demo 4: LIS =====" << endl;

    vector<int> nums1 = {10, 9, 2, 5, 3, 7, 101, 18};
    printVec(nums1, "  数组");
    cout << "  LIS O(n²): " << lengthOfLIS_dp(nums1) << " (期望=4)" << endl;
    cout << "  LIS O(nlogn): " << lengthOfLIS_fast(nums1) << endl;

    // 展示 tails 过程
    cout << "\n  O(n log n) 过程:" << endl;
    vector<int> tails;
    for (int num : nums1) {
        auto it = lower_bound(tails.begin(), tails.end(), num);
        if (it == tails.end()) tails.push_back(num);
        else *it = num;
        cout << "    处理 " << num << " → tails = ";
        printVec(tails);
    }

    vector<int> nums2 = {0, 1, 0, 3, 2, 3};
    printVec(nums2, "\n  数组");
    cout << "  LIS: " << lengthOfLIS_fast(nums2) << " (期望=4)" << endl;

    vector<int> nums3 = {7, 7, 7, 7};
    printVec(nums3, "  数组");
    cout << "  LIS: " << lengthOfLIS_fast(nums3) << " (期望=1)" << endl;
}

// ============================================================
//  Demo 5: LCS & 编辑距离
//  LC 1143: 最长公共子序列
//  LC 72:   编辑距离
// ============================================================

int longestCommonSubsequence(string text1, string text2) {
    int m = text1.size(), n = text2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (text1[i - 1] == text2[j - 1])
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        }
    return dp[m][n];
}

int minDistance(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (word1[i - 1] == word2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    return dp[m][n];
}

void demo_lcs_edit() {
    cout << "\n===== Demo 5: LCS & 编辑距离 =====" << endl;

    // LC 1143
    cout << "\n--- LC 1143: 最长公共子序列 ---" << endl;
    string t1 = "abcde", t2 = "ace";
    cout << "  \"" << t1 << "\" vs \"" << t2 << "\" → LCS="
         << longestCommonSubsequence(t1, t2) << " (期望=3, \"ace\")" << endl;

    t1 = "abc"; t2 = "abc";
    cout << "  \"" << t1 << "\" vs \"" << t2 << "\" → LCS="
         << longestCommonSubsequence(t1, t2) << " (期望=3)" << endl;

    t1 = "abc"; t2 = "def";
    cout << "  \"" << t1 << "\" vs \"" << t2 << "\" → LCS="
         << longestCommonSubsequence(t1, t2) << " (期望=0)" << endl;

    // LC 72
    cout << "\n--- LC 72: 编辑距离 ---" << endl;
    string w1 = "horse", w2 = "ros";
    cout << "  \"" << w1 << "\" → \"" << w2 << "\" = "
         << minDistance(w1, w2) << " 次操作 (期望=3)" << endl;

    w1 = "intention"; w2 = "execution";
    cout << "  \"" << w1 << "\" → \"" << w2 << "\" = "
         << minDistance(w1, w2) << " 次操作 (期望=5)" << endl;

    // 打印 DP 表格示例
    cout << "\n  \"horse\"→\"ros\" 的 DP 表格:" << endl;
    w1 = "horse"; w2 = "ros";
    int m = w1.size(), n = w2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (w1[i - 1] == w2[j - 1]) dp[i][j] = dp[i - 1][j - 1];
            else dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    cout << "       \"\"  r   o   s" << endl;
    for (int i = 0; i <= m; i++) {
        cout << "  " << (i == 0 ? "\"\"" : string(1, '"') + string(1, w1[i-1]) + "\"") << "  ";
        for (int j = 0; j <= n; j++) cout << dp[i][j] << "   ";
        cout << endl;
    }
}

// ============================================================
//  Demo 6: 股票买卖系列（状态机 DP）
//  LC 121, 122, 309, 714
// ============================================================

int maxProfit121(vector<int>& prices) {
    int minP = INT_MAX, maxP = 0;
    for (int p : prices) {
        minP = min(minP, p);
        maxP = max(maxP, p - minP);
    }
    return maxP;
}

int maxProfit122(vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < (int)prices.size(); i++)
        if (prices[i] > prices[i - 1])
            profit += prices[i] - prices[i - 1];
    return profit;
}

int maxProfit309(vector<int>& prices) {
    int n = prices.size();
    if (n < 2) return 0;
    int hold = -prices[0], sold = 0, rest = 0;
    for (int i = 1; i < n; i++) {
        int nH = max(hold, rest - prices[i]);
        int nS = hold + prices[i];
        int nR = max(rest, sold);
        hold = nH; sold = nS; rest = nR;
    }
    return max(sold, rest);
}

int maxProfit714(vector<int>& prices, int fee) {
    int hold = -prices[0], cash = 0;
    for (int i = 1; i < (int)prices.size(); i++) {
        hold = max(hold, cash - prices[i]);
        cash = max(cash, hold + prices[i] - fee);
    }
    return cash;
}

void demo_stock() {
    cout << "\n===== Demo 6: 股票买卖系列 =====" << endl;

    // LC 121
    cout << "\n--- LC 121: 一次交易 ---" << endl;
    vector<int> p1 = {7, 1, 5, 3, 6, 4};
    printVec(p1, "  价格");
    cout << "  最大利润: " << maxProfit121(p1) << " (期望=5, 1→6)" << endl;

    vector<int> p1b = {7, 6, 4, 3, 1};
    printVec(p1b, "  价格");
    cout << "  最大利润: " << maxProfit121(p1b) << " (期望=0, 持续下跌)" << endl;

    // LC 122
    cout << "\n--- LC 122: 不限次数 ---" << endl;
    vector<int> p2 = {7, 1, 5, 3, 6, 4};
    printVec(p2, "  价格");
    cout << "  最大利润: " << maxProfit122(p2) << " (期望=7, 1→5+3→6)" << endl;

    // LC 309
    cout << "\n--- LC 309: 含冷冻期 ---" << endl;
    vector<int> p3 = {1, 2, 3, 0, 2};
    printVec(p3, "  价格");
    cout << "  最大利润: " << maxProfit309(p3) << " (期望=3, 1→3 冷冻 0→2)" << endl;

    // 展示状态变化
    cout << "\n  状态变化过程:" << endl;
    {
        int hold = -1, sold = 0, rest = 0;
        cout << "    day0: hold=" << hold << " sold=" << sold << " rest=" << rest << endl;
        vector<int>& prices = p3;
        for (int i = 1; i < (int)prices.size(); i++) {
            int nH = max(hold, rest - prices[i]);
            int nS = hold + prices[i];
            int nR = max(rest, sold);
            hold = nH; sold = nS; rest = nR;
            cout << "    day" << i << " (price=" << prices[i]
                 << "): hold=" << hold << " sold=" << sold << " rest=" << rest << endl;
        }
    }

    // LC 714
    cout << "\n--- LC 714: 含手续费 ---" << endl;
    vector<int> p4 = {1, 3, 2, 8, 4, 9};
    printVec(p4, "  价格");
    cout << "  手续费=2, 最大利润: " << maxProfit714(p4, 2)
         << " (期望=8, 1→8-2 + 4→9-2)" << endl;
}

// ============================================================
//  Demo 7: 路径计数（网格 DP）
//  LC 62, 63, 64
// ============================================================

int uniquePaths(int m, int n) {
    vector<int> dp(n, 1);
    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            dp[j] += dp[j - 1];
    return dp[n - 1];
}

int uniquePathsWithObstacles(vector<vector<int>>& grid) {
    int n = grid[0].size();
    vector<int> dp(n, 0);
    dp[0] = 1;
    for (auto& row : grid) {
        for (int j = 0; j < n; j++) {
            if (row[j] == 1) dp[j] = 0;
            else if (j > 0) dp[j] += dp[j - 1];
        }
    }
    return dp[n - 1];
}

int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<vector<int>> dp(m, vector<int>(n));
    dp[0][0] = grid[0][0];
    for (int i = 1; i < m; i++) dp[i][0] = dp[i - 1][0] + grid[i][0];
    for (int j = 1; j < n; j++) dp[0][j] = dp[0][j - 1] + grid[0][j];
    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            dp[i][j] = grid[i][j] + min(dp[i - 1][j], dp[i][j - 1]);
    return dp[m - 1][n - 1];
}

void demo_grid() {
    cout << "\n===== Demo 7: 路径计数（网格 DP）=====" << endl;

    // LC 62
    cout << "\n--- LC 62: 不同路径 ---" << endl;
    for (auto [m, n] : vector<pair<int,int>>{{3,7},{3,2},{3,3}})
        cout << "  " << m << "x" << n << " → " << uniquePaths(m, n) << " 条路径" << endl;
    // 3x7→28, 3x2→3, 3x3→6

    // LC 63
    cout << "\n--- LC 63: 不同路径 II（有障碍）---" << endl;
    vector<vector<int>> grid1 = {{0,0,0},{0,1,0},{0,0,0}};
    cout << "  有障碍 → " << uniquePathsWithObstacles(grid1) << " (期望=2)" << endl;

    // LC 64
    cout << "\n--- LC 64: 最小路径和 ---" << endl;
    vector<vector<int>> grid2 = {{1,3,1},{1,5,1},{4,2,1}};
    cout << "  网格:" << endl;
    for (auto& row : grid2) {
        cout << "    ";
        for (int v : row) cout << v << " ";
        cout << endl;
    }
    cout << "  最小路径和: " << minPathSum(grid2) << " (期望=7, 1→3→1→1→1)" << endl;
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题十（上）：DP基础、线性DP与状态机DP" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_fibonacci();    // Demo 1: 斐波那契型
    demo_rob();          // Demo 2: 打家劫舍
    demo_subarray();     // Demo 3: 子数组
    demo_lis();          // Demo 4: LIS
    demo_lcs_edit();     // Demo 5: LCS & 编辑距离
    demo_stock();        // Demo 6: 股票
    demo_grid();         // Demo 7: 网格

    cout << "\n============================================" << endl;
    cout << "  线性DP & 状态机DP Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 53, 62, 63, 64, 70, 72," << endl;
    cout << "  121, 122, 152, 198, 213, 300, 309, 509," << endl;
    cout << "  714, 746, 1143" << endl;
    cout << "============================================" << endl;

    return 0;
}
