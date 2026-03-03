/**
 * 专题十一：贪心算法（Greedy Algorithm）示例代码
 * 作者：大胖超 😜
 *
 * 编译: g++ -std=c++17 -o knowlege_details_11_greedy knowlege_details_11_greedy.cpp
 * 运行: ./knowlege_details_11_greedy
 *
 * 覆盖题目：
 *   Demo1: 区间贪心（LC 56 合并区间 + LC 435 无重叠区间 + LC 452 射气球 + LC 763 划分字母区间）
 *   Demo2: 跳跃问题（LC 55 跳跃游戏 + LC 45 跳跃游戏II）
 *   Demo3: 分配问题（LC 455 分发饼干 + LC 135 分发糖果 + LC 406 身高重建）
 *   Demo4: 序列贪心（LC 376 摆动序列 + LC 738 单调递增数字）
 *   Demo5: 股票与子数组（LC 122 买卖股票II + LC 53 最大子数组和）
 *   Demo6: 字符串贪心（LC 316 去重复字母 + LC 402 移K位数字）
 *   Demo7: 加油站与数学贪心（LC 134 加油站 + LC 179 最大数 + LC 621 任务调度器）
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <numeric>
using namespace std;

// ==================== Demo1: 区间贪心 ====================
void demo1_intervals() {
    cout << "===== Demo1: 区间贪心 =====\n\n";

    // --- LC 56: 合并区间 ---
    {
        cout << "--- LC 56: 合并区间 ---\n";
        auto merge = [](vector<vector<int>> intervals) -> vector<vector<int>> {
            sort(intervals.begin(), intervals.end());
            vector<vector<int>> res;
            for (auto& iv : intervals) {
                if (res.empty() || res.back()[1] < iv[0])
                    res.push_back(iv);
                else
                    res.back()[1] = max(res.back()[1], iv[1]);
            }
            return res;
        };

        vector<vector<int>> intervals = {{1,3},{2,6},{8,10},{15,18}};
        cout << "输入: [1,3],[2,6],[8,10],[15,18]\n";
        auto result = merge(intervals);
        cout << "输出: ";
        for (auto& iv : result) cout << "[" << iv[0] << "," << iv[1] << "] ";
        cout << "\n\n";

        // 可视化过程
        cout << "合并过程:\n";
        cout << "  1---3      → 加入结果\n";
        cout << "    2------6 → 与[1,3]重叠,合并为[1,6]\n";
        cout << "              8--10  → 不重叠,加入结果\n";
        cout << "                      15--18 → 不重叠,加入结果\n\n";
    }

    // --- LC 435: 无重叠区间 ---
    {
        cout << "--- LC 435: 无重叠区间（最少删几个） ---\n";
        auto eraseOverlapIntervals = [](vector<vector<int>> intervals) -> int {
            sort(intervals.begin(), intervals.end(),
                 [](auto& a, auto& b) { return a[1] < b[1]; });
            int count = 0, prevEnd = INT_MIN;
            for (auto& iv : intervals) {
                if (iv[0] >= prevEnd)
                    prevEnd = iv[1];
                else
                    count++;
            }
            return count;
        };

        vector<vector<int>> iv1 = {{1,2},{2,3},{3,4},{1,3}};
        cout << "[[1,2],[2,3],[3,4],[1,3]] → 删除 "
             << eraseOverlapIntervals(iv1) << " 个\n";

        vector<vector<int>> iv2 = {{1,2},{1,2},{1,2}};
        cout << "[[1,2],[1,2],[1,2]] → 删除 "
             << eraseOverlapIntervals(iv2) << " 个\n";

        cout << "策略: 按右端点排序,保留结束最早的\n\n";
    }

    // --- LC 452: 用最少的箭引爆气球 ---
    {
        cout << "--- LC 452: 用最少的箭引爆气球 ---\n";
        auto findMinArrowShots = [](vector<vector<int>> points) -> int {
            sort(points.begin(), points.end(),
                 [](auto& a, auto& b) { return a[1] < b[1]; });
            int arrows = 1, arrowPos = points[0][1];
            for (int i = 1; i < (int)points.size(); i++) {
                if (points[i][0] > arrowPos) {
                    arrows++;
                    arrowPos = points[i][1];
                }
            }
            return arrows;
        };

        vector<vector<int>> pts = {{10,16},{2,8},{1,6},{7,12}};
        cout << "[[10,16],[2,8],[1,6],[7,12]] → " << findMinArrowShots(pts) << " 箭\n";

        cout << "排序后: [1,6],[2,8],[7,12],[10,16]\n";
        cout << "  箭1射在6: 覆盖[1,6]和[2,8]\n";
        cout << "  箭2射在12: 覆盖[7,12]和[10,16]\n\n";
    }

    // --- LC 763: 划分字母区间 ---
    {
        cout << "--- LC 763: 划分字母区间 ---\n";
        auto partitionLabels = [](string s) -> vector<int> {
            vector<int> last(26, 0);
            for (int i = 0; i < (int)s.size(); i++)
                last[s[i] - 'a'] = i;
            vector<int> result;
            int start = 0, end = 0;
            for (int i = 0; i < (int)s.size(); i++) {
                end = max(end, last[s[i] - 'a']);
                if (i == end) {
                    result.push_back(end - start + 1);
                    start = end + 1;
                }
            }
            return result;
        };

        string s = "ababcbacadefegdehijhklij";
        cout << "\"" << s << "\"\n划分: ";
        auto parts = partitionLabels(s);
        int pos = 0;
        for (int len : parts) {
            cout << "[" << s.substr(pos, len) << "]=" << len << " ";
            pos += len;
        }
        cout << "\n\n";
    }
}

// ==================== Demo2: 跳跃问题 ====================
void demo2_jump() {
    cout << "===== Demo2: 跳跃问题 =====\n\n";

    // --- LC 55: 跳跃游戏 ---
    {
        cout << "--- LC 55: 跳跃游戏 ---\n";
        auto canJump = [](vector<int>& nums) -> bool {
            int maxReach = 0;
            for (int i = 0; i < (int)nums.size(); i++) {
                if (i > maxReach) return false;
                maxReach = max(maxReach, i + nums[i]);
            }
            return true;
        };

        auto traceJump = [](vector<int>& nums) {
            int maxReach = 0;
            cout << "  idx: ";
            for (int i = 0; i < (int)nums.size(); i++) cout << i << " ";
            cout << "\n  num: ";
            for (int x : nums) cout << x << " ";
            cout << "\n";
            for (int i = 0; i < (int)nums.size(); i++) {
                if (i > maxReach) {
                    cout << "  i=" << i << " > maxReach=" << maxReach << " → FALSE\n";
                    return;
                }
                maxReach = max(maxReach, i + nums[i]);
                cout << "  i=" << i << ": maxReach=" << maxReach << "\n";
            }
            cout << "  → TRUE\n";
        };

        vector<int> nums1 = {2,3,1,1,4};
        vector<int> nums2 = {3,2,1,0,4};
        traceJump(nums1);
        traceJump(nums2);
        cout << "\n";
    }

    // --- LC 45: 跳跃游戏 II ---
    {
        cout << "--- LC 45: 跳跃游戏 II（最少跳数） ---\n";
        auto jump = [](vector<int>& nums) -> int {
            int jumps = 0, curEnd = 0, farthest = 0;
            for (int i = 0; i < (int)nums.size() - 1; i++) {
                farthest = max(farthest, i + nums[i]);
                if (i == curEnd) {
                    jumps++;
                    curEnd = farthest;
                }
            }
            return jumps;
        };

        vector<int> nums = {2,3,1,1,4};
        cout << "[2,3,1,1,4] → " << jump(nums) << " 跳\n";

        // 详细过程
        cout << "过程追踪:\n";
        int jumps = 0, curEnd = 0, farthest = 0;
        for (int i = 0; i < (int)nums.size() - 1; i++) {
            farthest = max(farthest, i + nums[i]);
            if (i == curEnd) {
                jumps++;
                cout << "  第" << jumps << "跳: 在i=" << i
                     << "处触发, curEnd更新为" << farthest << "\n";
                curEnd = farthest;
            }
        }
        cout << "\n";
    }
}

// ==================== Demo3: 分配问题 ====================
void demo3_assign() {
    cout << "===== Demo3: 分配问题 =====\n\n";

    // --- LC 455: 分发饼干 ---
    {
        cout << "--- LC 455: 分发饼干 ---\n";
        auto findContentChildren = [](vector<int> g, vector<int> s) -> int {
            sort(g.begin(), g.end());
            sort(s.begin(), s.end());
            int child = 0, cookie = 0;
            while (child < (int)g.size() && cookie < (int)s.size()) {
                if (s[cookie] >= g[child]) child++;
                cookie++;
            }
            return child;
        };

        vector<int> g = {1, 2, 3}, s = {1, 1};
        cout << "胃口[1,2,3], 饼干[1,1] → 满足 " << findContentChildren(g, s) << " 个\n";

        g = {1, 2}; s = {1, 2, 3};
        cout << "胃口[1,2], 饼干[1,2,3] → 满足 " << findContentChildren(g, s) << " 个\n\n";
    }

    // --- LC 135: 分发糖果 ---
    {
        cout << "--- LC 135: 分发糖果 ---\n";
        auto candy = [](vector<int>& ratings) -> int {
            int n = ratings.size();
            vector<int> candies(n, 1);
            for (int i = 1; i < n; i++)
                if (ratings[i] > ratings[i - 1])
                    candies[i] = candies[i - 1] + 1;
            for (int i = n - 2; i >= 0; i--)
                if (ratings[i] > ratings[i + 1])
                    candies[i] = max(candies[i], candies[i + 1] + 1);
            int total = 0;
            for (int c : candies) total += c;
            return total;
        };

        vector<int> r1 = {1, 0, 2};
        cout << "ratings=[1,0,2]\n";
        {
            int n = r1.size();
            vector<int> c(n, 1);
            cout << "  初始:  ";
            for (int x : c) cout << x << " ";
            cout << "\n";
            for (int i = 1; i < n; i++)
                if (r1[i] > r1[i - 1]) c[i] = c[i - 1] + 1;
            cout << "  左→右: ";
            for (int x : c) cout << x << " ";
            cout << "\n";
            for (int i = n - 2; i >= 0; i--)
                if (r1[i] > r1[i + 1]) c[i] = max(c[i], c[i + 1] + 1);
            cout << "  右→左: ";
            for (int x : c) cout << x << " ";
            int total = 0;
            for (int x : c) total += x;
            cout << "\n  总计: " << total << "\n";
        }

        vector<int> r2 = {1, 2, 87, 87, 87, 2, 1};
        cout << "\nratings=[1,2,87,87,87,2,1] → " << candy(r2) << "\n\n";
    }

    // --- LC 406: 根据身高重建队列 ---
    {
        cout << "--- LC 406: 身高重建队列 ---\n";
        auto reconstructQueue = [](vector<vector<int>> people) -> vector<vector<int>> {
            sort(people.begin(), people.end(),
                 [](auto& a, auto& b) {
                     return a[0] > b[0] || (a[0] == b[0] && a[1] < b[1]);
                 });
            vector<vector<int>> result;
            for (auto& p : people)
                result.insert(result.begin() + p[1], p);
            return result;
        };

        vector<vector<int>> people = {{7,0},{4,4},{7,1},{5,0},{6,1},{5,2}};
        cout << "输入: [7,0],[4,4],[7,1],[5,0],[6,1],[5,2]\n";
        cout << "排序: ";
        auto sorted = people;
        sort(sorted.begin(), sorted.end(),
             [](auto& a, auto& b) { return a[0] > b[0] || (a[0] == b[0] && a[1] < b[1]); });
        for (auto& p : sorted) cout << "[" << p[0] << "," << p[1] << "] ";
        cout << "\n";

        auto result = reconstructQueue(people);
        cout << "结果: ";
        for (auto& p : result) cout << "[" << p[0] << "," << p[1] << "] ";
        cout << "\n\n";

        // 插入追踪
        cout << "插入过程:\n";
        vector<vector<int>> trace;
        for (auto& p : sorted) {
            trace.insert(trace.begin() + p[1], p);
            cout << "  插入[" << p[0] << "," << p[1] << "] → ";
            for (auto& t : trace) cout << "[" << t[0] << "," << t[1] << "] ";
            cout << "\n";
        }
        cout << "\n";
    }
}

// ==================== Demo4: 序列贪心 ====================
void demo4_sequence() {
    cout << "===== Demo4: 序列贪心 =====\n\n";

    // --- LC 376: 摆动序列 ---
    {
        cout << "--- LC 376: 摆动序列 ---\n";
        auto wiggleMaxLength = [](vector<int>& nums) -> int {
            int n = nums.size();
            if (n < 2) return n;
            int up = 1, down = 1;
            for (int i = 1; i < n; i++) {
                if (nums[i] > nums[i - 1]) up = down + 1;
                else if (nums[i] < nums[i - 1]) down = up + 1;
            }
            return max(up, down);
        };

        vector<int> nums = {1, 7, 4, 9, 2, 5};
        cout << "[1,7,4,9,2,5]\n";
        cout << "追踪:\n";
        int up = 1, down = 1;
        cout << "  i=0: up=1, down=1\n";
        for (int i = 1; i < (int)nums.size(); i++) {
            if (nums[i] > nums[i-1]) up = down + 1;
            else if (nums[i] < nums[i-1]) down = up + 1;
            cout << "  i=" << i << " (" << nums[i-1] << "→" << nums[i] << "): "
                 << "up=" << up << ", down=" << down;
            if (nums[i] > nums[i-1]) cout << " ↑";
            else if (nums[i] < nums[i-1]) cout << " ↓";
            cout << "\n";
        }
        cout << "最长摆动子序列长度: " << max(up, down) << "\n\n";
    }

    // --- LC 738: 单调递增的数字 ---
    {
        cout << "--- LC 738: 单调递增的数字 ---\n";
        auto monotoneIncreasingDigits = [](int n) -> int {
            string s = to_string(n);
            int mark = s.size();
            for (int i = s.size() - 1; i > 0; i--) {
                if (s[i] < s[i - 1]) {
                    s[i - 1]--;
                    mark = i;
                }
            }
            for (int i = mark; i < (int)s.size(); i++)
                s[i] = '9';
            return stoi(s);
        };

        vector<int> cases = {10, 332, 1234, 4321, 100};
        for (int n : cases) {
            cout << n << " → " << monotoneIncreasingDigits(n) << "\n";
        }
        cout << "\n";
    }
}

// ==================== Demo5: 股票与子数组贪心 ====================
void demo5_stockAndSubarray() {
    cout << "===== Demo5: 股票与子数组贪心 =====\n\n";

    // --- LC 122: 买卖股票 II ---
    {
        cout << "--- LC 122: 买卖股票 II（贪心：收集正差价） ---\n";
        auto maxProfit = [](vector<int>& prices) -> int {
            int profit = 0;
            for (int i = 1; i < (int)prices.size(); i++)
                if (prices[i] > prices[i - 1])
                    profit += prices[i] - prices[i - 1];
            return profit;
        };

        vector<int> prices = {7, 1, 5, 3, 6, 4};
        cout << "prices=[7,1,5,3,6,4]\n";
        cout << "差值:  ";
        int profit = 0;
        for (int i = 1; i < (int)prices.size(); i++) {
            int diff = prices[i] - prices[i - 1];
            cout << (diff >= 0 ? "+" : "") << diff << " ";
            if (diff > 0) profit += diff;
        }
        cout << "\n收集正差: " << profit << "\n\n";
    }

    // --- LC 53: 最大子数组和（Kadane 贪心） ---
    {
        cout << "--- LC 53: 最大子数组和 (Kadane) ---\n";
        vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
        cout << "nums=[-2,1,-3,4,-1,2,1,-5,4]\n";

        int maxSum = nums[0], curSum = nums[0];
        int start = 0, end = 0, tempStart = 0;
        cout << "追踪:\n";
        cout << "  i=0: curSum=" << curSum << ", maxSum=" << maxSum << "\n";
        for (int i = 1; i < (int)nums.size(); i++) {
            if (curSum + nums[i] < nums[i]) {
                curSum = nums[i];
                tempStart = i;
            } else {
                curSum += nums[i];
            }
            if (curSum > maxSum) {
                maxSum = curSum;
                start = tempStart;
                end = i;
            }
            cout << "  i=" << i << " num=" << nums[i]
                 << ": curSum=" << curSum << ", maxSum=" << maxSum << "\n";
        }
        cout << "最大子数组: [" << start << ".." << end << "] = ";
        for (int i = start; i <= end; i++) cout << nums[i] << " ";
        cout << "= " << maxSum << "\n\n";
    }
}

// ==================== Demo6: 字符串贪心 ====================
void demo6_stringGreedy() {
    cout << "===== Demo6: 字符串贪心 =====\n\n";

    // --- LC 316: 去除重复字母 ---
    {
        cout << "--- LC 316: 去除重复字母（保持字典序最小） ---\n";
        auto removeDuplicateLetters = [](string s) -> string {
            vector<int> count(26, 0);
            vector<bool> inStack(26, false);
            for (char c : s) count[c - 'a']++;

            string stack;
            for (char c : s) {
                count[c - 'a']--;
                if (inStack[c - 'a']) continue;
                while (!stack.empty() && stack.back() > c
                       && count[stack.back() - 'a'] > 0) {
                    inStack[stack.back() - 'a'] = false;
                    stack.pop_back();
                }
                stack.push_back(c);
                inStack[c - 'a'] = true;
            }
            return stack;
        };

        vector<string> cases = {"bcabc", "cbacdcbc", "abacb"};
        for (auto& s : cases)
            cout << "\"" << s << "\" → \"" << removeDuplicateLetters(s) << "\"\n";

        // 追踪 "cbacdcbc"
        cout << "\n追踪 \"cbacdcbc\":\n";
        string s = "cbacdcbc";
        vector<int> cnt(26, 0);
        for (char c : s) cnt[c - 'a']++;
        vector<bool> inS(26, false);
        string stk;
        for (int idx = 0; idx < (int)s.size(); idx++) {
            char c = s[idx];
            cnt[c - 'a']--;
            if (inS[c - 'a']) {
                cout << "  '" << c << "' 已在栈中, 跳过\n";
                continue;
            }
            while (!stk.empty() && stk.back() > c && cnt[stk.back()-'a'] > 0) {
                cout << "  弹出'" << stk.back() << "' (后面还有" << cnt[stk.back()-'a'] << "个)\n";
                inS[stk.back()-'a'] = false;
                stk.pop_back();
            }
            stk.push_back(c);
            inS[c - 'a'] = true;
            cout << "  压入'" << c << "' → 栈: \"" << stk << "\"\n";
        }
        cout << "结果: \"" << stk << "\"\n\n";
    }

    // --- LC 402: 移掉K位数字 ---
    {
        cout << "--- LC 402: 移掉K位数字 ---\n";
        auto removeKdigits = [](string num, int k) -> string {
            string stack;
            for (char c : num) {
                while (k > 0 && !stack.empty() && stack.back() > c) {
                    stack.pop_back();
                    k--;
                }
                stack.push_back(c);
            }
            while (k > 0) { stack.pop_back(); k--; }
            int start = 0;
            while (start < (int)stack.size() && stack[start] == '0') start++;
            string result = stack.substr(start);
            return result.empty() ? "0" : result;
        };

        cout << "\"1432219\", k=3 → \"" << removeKdigits("1432219", 3) << "\"\n";
        cout << "\"10200\",   k=1 → \"" << removeKdigits("10200", 1) << "\"\n";
        cout << "\"10\",      k=2 → \"" << removeKdigits("10", 2) << "\"\n\n";
    }
}

// ==================== Demo7: 加油站与数学贪心 ====================
void demo7_gasAndMath() {
    cout << "===== Demo7: 加油站与数学贪心 =====\n\n";

    // --- LC 134: 加油站 ---
    {
        cout << "--- LC 134: 加油站 ---\n";
        auto canCompleteCircuit = [](vector<int>& gas, vector<int>& cost) -> int {
            int totalTank = 0, curTank = 0, start = 0;
            for (int i = 0; i < (int)gas.size(); i++) {
                int diff = gas[i] - cost[i];
                totalTank += diff;
                curTank += diff;
                if (curTank < 0) {
                    start = i + 1;
                    curTank = 0;
                }
            }
            return totalTank >= 0 ? start : -1;
        };

        vector<int> gas  = {1, 2, 3, 4, 5};
        vector<int> cost = {3, 4, 5, 1, 2};
        cout << "gas  = [1,2,3,4,5]\n";
        cout << "cost = [3,4,5,1,2]\n";
        cout << "diff = ";
        for (int i = 0; i < (int)gas.size(); i++)
            cout << gas[i] - cost[i] << " ";
        cout << "\n";

        // 追踪过程
        int totalTank = 0, curTank = 0, start = 0;
        for (int i = 0; i < (int)gas.size(); i++) {
            int diff = gas[i] - cost[i];
            totalTank += diff;
            curTank += diff;
            cout << "  站" << i << ": diff=" << diff
                 << ", curTank=" << curTank
                 << ", totalTank=" << totalTank;
            if (curTank < 0) {
                start = i + 1;
                curTank = 0;
                cout << " → 重置start=" << start;
            }
            cout << "\n";
        }
        cout << "起始站: " << start << "\n\n";
    }

    // --- LC 179: 最大数 ---
    {
        cout << "--- LC 179: 最大数 ---\n";
        auto largestNumber = [](vector<int> nums) -> string {
            vector<string> strs;
            for (int n : nums) strs.push_back(to_string(n));
            sort(strs.begin(), strs.end(),
                 [](string& a, string& b) { return a + b > b + a; });
            if (strs[0] == "0") return "0";
            string result;
            for (auto& s : strs) result += s;
            return result;
        };

        vector<int> nums1 = {10, 2};
        vector<int> nums2 = {3, 30, 34, 5, 9};
        cout << "[10,2] → \"" << largestNumber(nums1) << "\"\n";
        cout << "[3,30,34,5,9] → \"" << largestNumber(nums2) << "\"\n";
        cout << "排序规则: 比较 a+b vs b+a\n";
        cout << "  \"3\"vs\"30\": \"330\">\"303\" → 3在前\n";
        cout << "  \"3\"vs\"34\": \"334\"<\"343\" → 34在前\n\n";
    }

    // --- LC 621: 任务调度器 ---
    {
        cout << "--- LC 621: 任务调度器 ---\n";
        auto leastInterval = [](vector<char>& tasks, int n) -> int {
            vector<int> freq(26, 0);
            for (char c : tasks) freq[c - 'A']++;
            int maxFreq = *max_element(freq.begin(), freq.end());
            int maxCount = count(freq.begin(), freq.end(), maxFreq);
            int result = (maxFreq - 1) * (n + 1) + maxCount;
            return max(result, (int)tasks.size());
        };

        vector<char> tasks = {'A','A','A','B','B','B'};
        int n = 2;
        cout << "tasks=[A,A,A,B,B,B], n=2\n";
        cout << "最高频率=3 (A和B), 最高频个数=2\n";
        cout << "公式: (3-1)*(2+1)+2 = " << (3-1)*(2+1)+2 << "\n";
        cout << "安排: A B _ | A B _ | A B\n";
        cout << "最少时间: " << leastInterval(tasks, n) << "\n\n";

        vector<char> tasks2 = {'A','A','A','A','A','A','B','C','D','E','F','G'};
        int n2 = 2;
        cout << "tasks=[A*6,B,C,D,E,F,G], n=2\n";
        cout << "安排: A B C | A D E | A F G | A _ _ | A _ _ | A\n";
        cout << "最少时间: " << leastInterval(tasks2, n2) << "\n\n";
    }
}

// ==================== main ====================
int main() {
    cout << "╔════════════════════════════════════════════════╗\n";
    cout << "║  专题十一：贪心算法 全题型演示                  ║\n";
    cout << "║  作者：大胖超 😜                               ║\n";
    cout << "╚════════════════════════════════════════════════╝\n\n";

    demo1_intervals();
    demo2_jump();
    demo3_assign();
    demo4_sequence();
    demo5_stockAndSubarray();
    demo6_stringGreedy();
    demo7_gasAndMath();

    cout << "========================================\n";
    cout << "全部 Demo 运行完毕！\n";
    cout << "覆盖题目: LC 45, 53, 55, 56, 122, 134, 135,\n";
    cout << "         179, 316, 376, 402, 406, 435, 452,\n";
    cout << "         455, 621, 738, 763\n";
    cout << "共 18 道 LeetCode 贪心题\n";
    return 0;
}
