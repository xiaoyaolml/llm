/**
 * 专题十（下）：背包问题、区间DP与进阶DP 示例代码
 * 作者：大胖超 😜
 *
 * 编译: g++ -std=c++17 -o knowlege_details_10_dp_knapsack knowlege_details_10_dp_knapsack.cpp
 * 运行: ./knowlege_details_10_dp_knapsack
 *
 * 覆盖题目：
 *   Demo1: 0-1 背包（经典 + LC 416 分割等和子集 + LC 1049 石头）
 *   Demo2: 完全背包（经典 + LC 322 零钱兑换 + LC 279 完全平方数）
 *   Demo3: 背包计数（LC 518 零钱II + LC 377 组合IV + LC 494 目标和）
 *   Demo4: 区间 DP（LC 516 最长回文子序列 + LC 5 最长回文子串 + LC 647）
 *   Demo5: 字符串 DP（LC 139 单词拆分 + LC 115 不同子序列）
 *   Demo6: 计数 DP（LC 96 不同 BST + LC 343 整数拆分）
 *   Demo7: 记忆化搜索（LC 516 递归版）
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <unordered_set>
#include <unordered_map>
#include <functional>
using namespace std;

// ==================== Demo1: 0-1 背包 ====================
void demo1_01knapsack() {
    cout << "===== Demo1: 0-1 背包 =====\n\n";

    // --- 经典 0-1 背包 ---
    {
        cout << "--- 经典 0-1 背包 ---\n";
        vector<int> weight = {1, 3, 4};
        vector<int> value  = {15, 20, 30};
        int capacity = 4;

        // 二维版本（直观理解）
        int n = weight.size();
        vector<vector<int>> dp2d(n + 1, vector<int>(capacity + 1, 0));
        for (int i = 1; i <= n; i++) {
            for (int j = 0; j <= capacity; j++) {
                dp2d[i][j] = dp2d[i-1][j];
                if (j >= weight[i-1])
                    dp2d[i][j] = max(dp2d[i][j], dp2d[i-1][j-weight[i-1]] + value[i-1]);
            }
        }
        cout << "二维 dp 表:\n";
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= capacity; j++)
                cout << dp2d[i][j] << "\t";
            cout << "\n";
        }

        // 一维滚动数组
        vector<int> dp(capacity + 1, 0);
        for (int i = 0; i < n; i++)
            for (int j = capacity; j >= weight[i]; j--)
                dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);

        cout << "一维 dp: ";
        for (int x : dp) cout << x << " ";
        cout << "\n最大价值: " << dp[capacity] << "\n\n";
    }

    // --- LC 416: 分割等和子集 ---
    {
        cout << "--- LC 416: 分割等和子集 ---\n";
        auto canPartition = [](vector<int>& nums) -> bool {
            int sum = 0;
            for (int x : nums) sum += x;
            if (sum % 2 != 0) return false;
            int target = sum / 2;
            vector<bool> dp(target + 1, false);
            dp[0] = true;
            for (int num : nums) {
                for (int j = target; j >= num; j--)
                    dp[j] = dp[j] || dp[j - num];
            }
            return dp[target];
        };

        vector<int> nums1 = {1, 5, 11, 5};
        vector<int> nums2 = {1, 2, 3, 5};
        cout << "[1,5,11,5] → " << (canPartition(nums1) ? "true" : "false") << "\n";
        cout << "[1,2,3,5]  → " << (canPartition(nums2) ? "true" : "false") << "\n";
    }

    // --- LC 1049: 最后一块石头的重量 II ---
    {
        cout << "\n--- LC 1049: 最后一块石头的重量 II ---\n";
        auto lastStoneWeightII = [](vector<int>& stones) -> int {
            int sum = 0;
            for (int s : stones) sum += s;
            int target = sum / 2;
            vector<bool> dp(target + 1, false);
            dp[0] = true;
            for (int s : stones)
                for (int j = target; j >= s; j--)
                    dp[j] = dp[j] || dp[j - s];
            for (int j = target; j >= 0; j--)
                if (dp[j]) return sum - 2 * j;
            return sum;
        };

        vector<int> stones = {2, 7, 4, 1, 8, 1};
        cout << "[2,7,4,1,8,1] → " << lastStoneWeightII(stones) << "\n\n";
    }
}

// ==================== Demo2: 完全背包 ====================
void demo2_completeKnapsack() {
    cout << "===== Demo2: 完全背包 =====\n\n";

    // --- 经典完全背包 ---
    {
        cout << "--- 经典完全背包 ---\n";
        vector<int> weight = {1, 3, 4};
        vector<int> value  = {15, 20, 30};
        int capacity = 4;
        int n = weight.size();

        vector<int> dp(capacity + 1, 0);
        for (int i = 0; i < n; i++)
            for (int j = weight[i]; j <= capacity; j++)  // 正序！
                dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);

        cout << "dp: ";
        for (int x : dp) cout << x << " ";
        cout << "\n最大价值: " << dp[capacity] << "\n\n";

        // 对比 0-1 背包（倒序） vs 完全背包（正序）
        cout << "遍历对比:\n";
        cout << "  0-1 背包: 容量从大到小 → 每个物品最多选 1 次\n";
        cout << "  完全背包: 容量从小到大 → 同一物品可重复选\n\n";
    }

    // --- LC 322: 零钱兑换 ---
    {
        cout << "--- LC 322: 零钱兑换 ---\n";
        auto coinChange = [](vector<int>& coins, int amount) -> int {
            vector<int> dp(amount + 1, INT_MAX);
            dp[0] = 0;
            for (int coin : coins)
                for (int j = coin; j <= amount; j++)
                    if (dp[j - coin] != INT_MAX)
                        dp[j] = min(dp[j], dp[j - coin] + 1);
            return dp[amount] == INT_MAX ? -1 : dp[amount];
        };

        vector<int> coins = {1, 2, 5};
        int amount = 11;
        cout << "coins=[1,2,5], amount=11 → " << coinChange(coins, amount) << "\n";

        // 打印 dp 过程
        cout << "dp 数组: ";
        vector<int> dp(amount + 1, INT_MAX);
        dp[0] = 0;
        for (int coin : coins)
            for (int j = coin; j <= amount; j++)
                if (dp[j - coin] != INT_MAX)
                    dp[j] = min(dp[j], dp[j - coin] + 1);
        for (int x : dp) cout << (x == INT_MAX ? -1 : x) << " ";
        cout << "\n\n";
    }

    // --- LC 279: 完全平方数 ---
    {
        cout << "--- LC 279: 完全平方数 ---\n";
        auto numSquares = [](int n) -> int {
            vector<int> dp(n + 1, INT_MAX);
            dp[0] = 0;
            for (int i = 1; i * i <= n; i++) {
                int sq = i * i;
                for (int j = sq; j <= n; j++)
                    dp[j] = min(dp[j], dp[j - sq] + 1);
            }
            return dp[n];
        };

        cout << "12 → " << numSquares(12) << " (4+4+4)\n";
        cout << "13 → " << numSquares(13) << " (4+9)\n\n";
    }
}

// ==================== Demo3: 背包计数 ====================
void demo3_knapsackCount() {
    cout << "===== Demo3: 背包计数 =====\n\n";

    // --- LC 518: 零钱兑换 II（组合数） ---
    {
        cout << "--- LC 518: 零钱兑换 II（组合数） ---\n";
        auto change = [](int amount, vector<int>& coins) -> int {
            vector<int> dp(amount + 1, 0);
            dp[0] = 1;
            for (int coin : coins)              // 外层物品 → 组合
                for (int j = coin; j <= amount; j++)
                    dp[j] += dp[j - coin];
            return dp[amount];
        };

        vector<int> coins = {1, 2, 5};
        cout << "coins=[1,2,5], amount=5 → " << change(5, coins) << " 种组合\n";

        // 打印 dp 数组
        cout << "dp: ";
        vector<int> dp(6, 0);
        dp[0] = 1;
        for (int coin : coins)
            for (int j = coin; j <= 5; j++)
                dp[j] += dp[j - coin];
        for (int x : dp) cout << x << " ";
        cout << "\n\n";
    }

    // --- LC 377: 组合总和 IV（排列数） ---
    {
        cout << "--- LC 377: 组合总和 IV（排列数） ---\n";
        auto combinationSum4 = [](vector<int>& nums, int target) -> int {
            vector<unsigned int> dp(target + 1, 0);
            dp[0] = 1;
            for (int j = 1; j <= target; j++)     // 外层容量 → 排列
                for (int num : nums)
                    if (j >= num) dp[j] += dp[j - num];
            return dp[target];
        };

        vector<int> nums = {1, 2, 3};
        cout << "nums=[1,2,3], target=4 → " << combinationSum4(nums, 4) << " 种排列\n";
        cout << "  (1111)(112)(121)(211)(13)(31)(22)(4不存在)...\n\n";
    }

    // --- LC 494: 目标和 ---
    {
        cout << "--- LC 494: 目标和 ---\n";
        auto findTargetSumWays = [](vector<int>& nums, int target) -> int {
            int sum = 0;
            for (int x : nums) sum += x;
            if ((sum + target) % 2 != 0 || sum + target < 0) return 0;
            int bagSize = (sum + target) / 2;
            vector<int> dp(bagSize + 1, 0);
            dp[0] = 1;
            for (int num : nums)
                for (int j = bagSize; j >= num; j--)  // 0-1 背包
                    dp[j] += dp[j - num];
            return dp[bagSize];
        };

        vector<int> nums = {1, 1, 1, 1, 1};
        cout << "[1,1,1,1,1], target=3 → " << findTargetSumWays(nums, 3) << " 种\n";
        cout << "  转化: P=(5+3)/2=4, 选数凑出 4 的方案数\n\n";
    }

    // --- 组合 vs 排列对比 ---
    {
        cout << "--- 组合 vs 排列 对比 ---\n";
        vector<int> coins = {1, 2, 5};
        int amount = 5;

        // 组合数（外层物品）
        vector<int> dp1(amount + 1, 0);
        dp1[0] = 1;
        for (int coin : coins)
            for (int j = coin; j <= amount; j++)
                dp1[j] += dp1[j - coin];

        // 排列数（外层容量）
        vector<unsigned int> dp2(amount + 1, 0);
        dp2[0] = 1;
        for (int j = 1; j <= amount; j++)
            for (int coin : coins)
                if (j >= coin) dp2[j] += dp2[j - coin];

        cout << "coins=[1,2,5], amount=5:\n";
        cout << "  组合数(先物品后容量): " << dp1[amount] << "\n";
        cout << "  排列数(先容量后物品): " << dp2[amount] << "\n\n";
    }
}

// ==================== Demo4: 区间 DP ====================
void demo4_intervalDP() {
    cout << "===== Demo4: 区间 DP =====\n\n";

    // --- LC 516: 最长回文子序列 ---
    {
        cout << "--- LC 516: 最长回文子序列 ---\n";
        auto longestPalindromeSubseq = [](string s) -> int {
            int n = s.size();
            vector<vector<int>> dp(n, vector<int>(n, 0));
            for (int i = n - 1; i >= 0; i--) {
                dp[i][i] = 1;
                for (int j = i + 1; j < n; j++) {
                    if (s[i] == s[j])
                        dp[i][j] = dp[i + 1][j - 1] + 2;
                    else
                        dp[i][j] = max(dp[i + 1][j], dp[i][j - 1]);
                }
            }
            return dp[0][n - 1];
        };

        string s = "bbbab";
        cout << "\"" << s << "\" → " << longestPalindromeSubseq(s) << "\n";

        // 打印 dp 表
        int n = s.size();
        vector<vector<int>> dp(n, vector<int>(n, 0));
        for (int i = n - 1; i >= 0; i--) {
            dp[i][i] = 1;
            for (int j = i + 1; j < n; j++) {
                if (s[i] == s[j]) dp[i][j] = dp[i + 1][j - 1] + 2;
                else dp[i][j] = max(dp[i + 1][j], dp[i][j - 1]);
            }
        }
        cout << "dp 表 (i\\j):\n    ";
        for (int j = 0; j < n; j++) cout << s[j] << " ";
        cout << "\n";
        for (int i = 0; i < n; i++) {
            cout << s[i] << " | ";
            for (int j = 0; j < n; j++) {
                if (j < i) cout << ". ";
                else cout << dp[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    // --- LC 5: 最长回文子串 ---
    {
        cout << "--- LC 5: 最长回文子串 ---\n";
        auto longestPalindrome = [](string s) -> string {
            int n = s.size(), start = 0, maxLen = 1;
            vector<vector<bool>> dp(n, vector<bool>(n, false));
            for (int i = n - 1; i >= 0; i--) {
                for (int j = i; j < n; j++) {
                    dp[i][j] = (s[i] == s[j]) && (j - i <= 2 || dp[i+1][j-1]);
                    if (dp[i][j] && j - i + 1 > maxLen) {
                        maxLen = j - i + 1;
                        start = i;
                    }
                }
            }
            return s.substr(start, maxLen);
        };

        cout << "\"babad\" → \"" << longestPalindrome("babad") << "\"\n";
        cout << "\"cbbd\"  → \"" << longestPalindrome("cbbd") << "\"\n";
        cout << "\"aacabdkacaa\" → \"" << longestPalindrome("aacabdkacaa") << "\"\n\n";
    }

    // --- LC 647: 回文子串个数 ---
    {
        cout << "--- LC 647: 回文子串个数 ---\n";
        auto countSubstrings = [](string s) -> int {
            int n = s.size(), count = 0;
            vector<vector<bool>> dp(n, vector<bool>(n, false));
            for (int i = n - 1; i >= 0; i--)
                for (int j = i; j < n; j++) {
                    dp[i][j] = (s[i] == s[j]) && (j - i <= 2 || dp[i+1][j-1]);
                    if (dp[i][j]) count++;
                }
            return count;
        };

        cout << "\"abc\" → " << countSubstrings("abc") << " (a,b,c)\n";
        cout << "\"aaa\" → " << countSubstrings("aaa") << " (a,a,a,aa,aa,aaa)\n\n";
    }
}

// ==================== Demo5: 字符串 DP 进阶 ====================
void demo5_stringDP() {
    cout << "===== Demo5: 字符串 DP 进阶 =====\n\n";

    // --- LC 139: 单词拆分 ---
    {
        cout << "--- LC 139: 单词拆分 ---\n";
        auto wordBreak = [](string s, vector<string>& wordDict) -> bool {
            unordered_set<string> dict(wordDict.begin(), wordDict.end());
            int n = s.size();
            vector<bool> dp(n + 1, false);
            dp[0] = true;
            for (int i = 1; i <= n; i++)
                for (int j = 0; j < i; j++)
                    if (dp[j] && dict.count(s.substr(j, i - j))) {
                        dp[i] = true;
                        break;
                    }
            return dp[n];
        };

        vector<string> dict1 = {"leet", "code"};
        vector<string> dict2 = {"apple", "pen"};
        cout << "\"leetcode\" [leet,code] → " << (wordBreak("leetcode", dict1) ? "true" : "false") << "\n";
        cout << "\"applepenapple\" [apple,pen] → " << (wordBreak("applepenapple", dict2) ? "true" : "false") << "\n\n";
    }

    // --- LC 115: 不同的子序列 ---
    {
        cout << "--- LC 115: 不同的子序列 ---\n";
        auto numDistinct = [](string s, string t) -> int {
            int m = s.size(), n = t.size();
            // 用 unsigned long long 防溢出
            vector<vector<unsigned long long>> dp(m + 1, vector<unsigned long long>(n + 1, 0));
            for (int i = 0; i <= m; i++) dp[i][0] = 1;
            for (int i = 1; i <= m; i++) {
                for (int j = 1; j <= n; j++) {
                    dp[i][j] = dp[i-1][j];  // 不用 s[i-1]
                    if (s[i-1] == t[j-1])
                        dp[i][j] += dp[i-1][j-1];  // 用 s[i-1] 匹配
                }
            }
            return (int)dp[m][n];
        };

        cout << "s=\"rabbbit\", t=\"rabbit\" → " << numDistinct("rabbbit", "rabbit") << "\n";
        cout << "s=\"babgbag\", t=\"bag\" → " << numDistinct("babgbag", "bag") << "\n";

        // 打印 dp 表
        string s = "rabbbit", t = "rabbit";
        int m = s.size(), n = t.size();
        vector<vector<unsigned long long>> dp(m + 1, vector<unsigned long long>(n + 1, 0));
        for (int i = 0; i <= m; i++) dp[i][0] = 1;
        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++) {
                dp[i][j] = dp[i-1][j];
                if (s[i-1] == t[j-1]) dp[i][j] += dp[i-1][j-1];
            }
        cout << "\ndp 表 (s=\"rabbbit\", t=\"rabbit\"):\n";
        cout << "    ε ";
        for (char c : t) cout << c << " ";
        cout << "\n";
        for (int i = 0; i <= m; i++) {
            cout << (i == 0 ? 'ε' : s[i-1]) << " | ";
            for (int j = 0; j <= n; j++)
                cout << dp[i][j] << " ";
            cout << "\n";
        }
        cout << "\n";
    }
}

// ==================== Demo6: 计数 DP ====================
void demo6_countingDP() {
    cout << "===== Demo6: 计数 DP =====\n\n";

    // --- LC 96: 不同的二叉搜索树 ---
    {
        cout << "--- LC 96: 不同的二叉搜索树（卡特兰数） ---\n";
        auto numTrees = [](int n) -> int {
            vector<int> dp(n + 1, 0);
            dp[0] = dp[1] = 1;
            for (int i = 2; i <= n; i++)
                for (int j = 1; j <= i; j++)
                    dp[i] += dp[j - 1] * dp[i - j];
            return dp[n];
        };

        cout << "n: ";
        for (int i = 1; i <= 8; i++)
            cout << i << "→" << numTrees(i) << "  ";
        cout << "\n";
        cout << "卡特兰数: 1, 2, 5, 14, 42, 132, 429, 1430...\n\n";
    }

    // --- LC 343: 整数拆分 ---
    {
        cout << "--- LC 343: 整数拆分 ---\n";
        auto integerBreak = [](int n) -> int {
            vector<int> dp(n + 1, 0);
            dp[2] = 1;
            for (int i = 3; i <= n; i++)
                for (int j = 1; j < i; j++)
                    dp[i] = max(dp[i], max(j * (i - j), j * dp[i - j]));
            return dp[n];
        };

        for (int n = 2; n <= 10; n++)
            cout << n << " → " << integerBreak(n) << "  ";
        cout << "\n";
        cout << "规律: 尽量拆成 3，余 1 时用 2*2 替代\n\n";
    }
}

// ==================== Demo7: 记忆化搜索 ====================
void demo7_memoization() {
    cout << "===== Demo7: 记忆化搜索 =====\n\n";

    // --- LC 516: 记忆化搜索版 ---
    {
        cout << "--- LC 516: 记忆化搜索版 ---\n";
        string s = "bbbab";
        int n = s.size();
        vector<vector<int>> memo(n, vector<int>(n, -1));
        int callCount = 0;

        function<int(int, int)> helper = [&](int i, int j) -> int {
            callCount++;
            if (i > j) return 0;
            if (i == j) return 1;
            if (memo[i][j] != -1) return memo[i][j];

            int res;
            if (s[i] == s[j])
                res = helper(i + 1, j - 1) + 2;
            else
                res = max(helper(i + 1, j), helper(i, j - 1));

            return memo[i][j] = res;
        };

        int ans = helper(0, n - 1);
        cout << "\"" << s << "\" → " << ans << "\n";
        cout << "递归调用次数: " << callCount << "\n\n";

        // 对比：不用缓存的次数
        int noMemoCount = 0;
        function<int(int, int)> bruteForce = [&](int i, int j) -> int {
            noMemoCount++;
            if (i > j) return 0;
            if (i == j) return 1;
            if (s[i] == s[j]) return bruteForce(i + 1, j - 1) + 2;
            return max(bruteForce(i + 1, j), bruteForce(i, j - 1));
        };
        bruteForce(0, n - 1);
        cout << "无缓存调用次数: " << noMemoCount << "\n";
        cout << "记忆化节省: " << (noMemoCount - callCount) << " 次调用\n\n";
    }

    // --- 记忆化 vs 表格法对比总结 ---
    {
        cout << "--- 记忆化 vs 表格法 对比 ---\n";
        cout << "┌──────────────┬──────────────────┬──────────────────┐\n";
        cout << "│              │ 记忆化搜索 (Top-Down)│ 表格法 (Bottom-Up) │\n";
        cout << "├──────────────┼──────────────────┼──────────────────┤\n";
        cout << "│ 实现方式      │ 递归 + 缓存       │ 循环填表          │\n";
        cout << "│ 遍历顺序      │ 自动（递归决定）    │ 手动决定         │\n";
        cout << "│ 空间优化      │ 困难              │ 容易（滚动数组）   │\n";
        cout << "│ 优势          │ 只计算有用状态      │ 无递归栈开销     │\n";
        cout << "│ 推荐场景      │ 状态空间稀疏        │ 状态空间密集     │\n";
        cout << "└──────────────┴──────────────────┴──────────────────┘\n\n";
    }
}

// ==================== main ====================
int main() {
    cout << "╔════════════════════════════════════════════════════╗\n";
    cout << "║  专题十（下）：背包 + 区间DP + 进阶DP 演示         ║\n";
    cout << "║  作者：大胖超 😜                                   ║\n";
    cout << "╚════════════════════════════════════════════════════╝\n\n";

    demo1_01knapsack();
    demo2_completeKnapsack();
    demo3_knapsackCount();
    demo4_intervalDP();
    demo5_stringDP();
    demo6_countingDP();
    demo7_memoization();

    cout << "============================\n";
    cout << "全部 Demo 运行完毕！\n";
    cout << "覆盖题目: LC 5, 96, 115, 139, 279, 322, 343, 377,\n";
    cout << "         416, 494, 516, 518, 647, 1049\n";
    cout << "共 14 道 LeetCode 题 + 经典背包模板\n";
    return 0;
}
