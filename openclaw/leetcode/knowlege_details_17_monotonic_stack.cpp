/*
 * 专题十七（上）：单调栈 Monotonic Stack — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_17_monotonic_stack.cpp -o mono_stack
 * 运行：./mono_stack
 *
 * Demo 列表：
 *   Demo1  — 四种单调栈模板演示 (NGE/NSE/PGE/PSE)
 *   Demo2  — LC 739  每日温度
 *   Demo3  — LC 496  下一个更大元素 I
 *   Demo4  — LC 503  下一个更大元素 II (循环数组)
 *   Demo5  — LC 84   柱状图中最大的矩形
 *   Demo6  — LC 42   接雨水 (单调栈解法)
 *   Demo7  — LC 85   最大矩形 (逐行→LC84)
 *   Demo8  — LC 402  移掉K位数字
 *   Demo9  — LC 316  去除重复字母
 *   Demo10 — LC 907  子数组的最小值之和 (贡献法)
 *   Demo11 — LC 901  股票价格跨度
 */

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include <algorithm>
using namespace std;

/* ============================================================
 * Demo1: 四种单调栈模板全面演示
 * ============================================================ */
namespace Demo1 {

// 右边第一个更大元素 (Next Greater Element)
vector<int> nextGreater(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;  // 单调递减栈
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && nums[i] > nums[stk.top()]) {
            result[stk.top()] = nums[i];
            stk.pop();
        }
        stk.push(i);
    }
    return result;
}

// 右边第一个更小元素 (Next Smaller Element)
vector<int> nextSmaller(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;  // 单调递增栈
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && nums[i] < nums[stk.top()]) {
            result[stk.top()] = nums[i];
            stk.pop();
        }
        stk.push(i);
    }
    return result;
}

// 左边第一个更大元素 (Previous Greater Element)
vector<int> prevGreater(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;  // 单调递减栈
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && nums[stk.top()] <= nums[i])
            stk.pop();
        if (!stk.empty())
            result[i] = nums[stk.top()];
        stk.push(i);
    }
    return result;
}

// 左边第一个更小元素 (Previous Smaller Element)
vector<int> prevSmaller(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;  // 单调递增栈
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && nums[stk.top()] >= nums[i])
            stk.pop();
        if (!stk.empty())
            result[i] = nums[stk.top()];
        stk.push(i);
    }
    return result;
}

void printArr(const string& label, vector<int>& v) {
    cout << label << ": [";
    for (int i = 0; i < (int)v.size(); i++) {
        if (i) cout << ", ";
        cout << v[i];
    }
    cout << "]\n";
}

void run() {
    cout << "===== Demo1: 四种单调栈模板 =====\n";
    vector<int> nums = {2, 7, 4, 3, 5};
    printArr("nums", nums);

    auto ng = nextGreater(nums);
    auto ns = nextSmaller(nums);
    auto pg = prevGreater(nums);
    auto ps = prevSmaller(nums);

    printArr("右边第一个更大 (NGE)", ng);
    printArr("右边第一个更小 (NSE)", ns);
    printArr("左边第一个更大 (PGE)", pg);
    printArr("左边第一个更小 (PSE)", ps);

    cout << "期望 NGE: [7, -1, 5, 5, -1]\n";
    cout << "期望 NSE: [-1, 4, 3, -1, -1]\n";
    cout << "期望 PGE: [-1, -1, 7, 4, 7]\n";
    cout << "期望 PSE: [-1, 2, -1, -1, 3]\n\n";
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 739 — 每日温度
 * ============================================================ */
namespace Demo2 {

vector<int> dailyTemperatures(vector<int>& temperatures) {
    int n = temperatures.size();
    vector<int> result(n, 0);
    stack<int> stk;

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && temperatures[i] > temperatures[stk.top()]) {
            int prev = stk.top(); stk.pop();
            result[prev] = i - prev;
        }
        stk.push(i);
    }
    return result;
}

void run() {
    cout << "===== Demo2: LC 739 每日温度 =====\n";
    vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    auto res = dailyTemperatures(temps);
    cout << "temperatures: [73,74,75,71,69,72,76,73]\n";
    cout << "结果: [";
    for (int i = 0; i < (int)res.size(); i++) {
        if (i) cout << ",";
        cout << res[i];
    }
    cout << "]  (期望 [1,1,4,2,1,1,0,0])\n\n";
}
} // namespace Demo2

/* ============================================================
 * Demo3: LC 496 — 下一个更大元素 I
 * ============================================================ */
namespace Demo3 {

vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    unordered_map<int, int> nge;
    stack<int> stk;  // 这里存值

    for (int x : nums2) {
        while (!stk.empty() && x > stk.top()) {
            nge[stk.top()] = x;
            stk.pop();
        }
        stk.push(x);
    }

    vector<int> result;
    for (int x : nums1)
        result.push_back(nge.count(x) ? nge[x] : -1);
    return result;
}

void run() {
    cout << "===== Demo3: LC 496 下一个更大元素 I =====\n";
    vector<int> nums1 = {4, 1, 2}, nums2 = {1, 3, 4, 2};
    auto res = nextGreaterElement(nums1, nums2);
    cout << "nums1=[4,1,2], nums2=[1,3,4,2]\n";
    cout << "结果: [";
    for (int i = 0; i < (int)res.size(); i++) {
        if (i) cout << ",";
        cout << res[i];
    }
    cout << "]  (期望 [-1,3,-1])\n\n";
}
} // namespace Demo3

/* ============================================================
 * Demo4: LC 503 — 下一个更大元素 II (循环数组)
 * ============================================================ */
namespace Demo4 {

vector<int> nextGreaterElements(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;

    for (int i = 0; i < 2 * n; i++) {
        int idx = i % n;
        while (!stk.empty() && nums[idx] > nums[stk.top()]) {
            result[stk.top()] = nums[idx];
            stk.pop();
        }
        if (i < n) stk.push(i);  // 只第一遍入栈
    }
    return result;
}

void run() {
    cout << "===== Demo4: LC 503 下一个更大元素 II (循环) =====\n";
    vector<int> nums = {1, 2, 1};
    auto res = nextGreaterElements(nums);
    cout << "nums=[1,2,1]\n";
    cout << "结果: [";
    for (int i = 0; i < (int)res.size(); i++) {
        if (i) cout << ",";
        cout << res[i];
    }
    cout << "]  (期望 [2,-1,2])\n";

    vector<int> nums2 = {1, 2, 3, 4, 3};
    auto res2 = nextGreaterElements(nums2);
    cout << "nums=[1,2,3,4,3]\n结果: [";
    for (int i = 0; i < (int)res2.size(); i++) {
        if (i) cout << ",";
        cout << res2[i];
    }
    cout << "]  (期望 [2,3,4,-1,4])\n\n";
}
} // namespace Demo4

/* ============================================================
 * Demo5: LC 84 — 柱状图中最大的矩形
 * ============================================================ */
namespace Demo5 {

int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    stack<int> stk;
    int maxArea = 0;
    heights.push_back(0);  // 哨兵

    for (int i = 0; i <= n; i++) {
        while (!stk.empty() && heights[i] < heights[stk.top()]) {
            int h = heights[stk.top()]; stk.pop();
            int w = stk.empty() ? i : i - stk.top() - 1;
            maxArea = max(maxArea, h * w);
        }
        stk.push(i);
    }
    heights.pop_back();  // 恢复
    return maxArea;
}

void run() {
    cout << "===== Demo5: LC 84 柱状图中最大的矩形 =====\n";
    {
        vector<int> h = {2, 1, 5, 6, 2, 3};
        cout << "heights=[2,1,5,6,2,3] → " << largestRectangleArea(h)
             << "  (期望 10)\n";
    }
    {
        vector<int> h = {2, 4};
        cout << "heights=[2,4] → " << largestRectangleArea(h)
             << "  (期望 4)\n";
    }
    {
        vector<int> h = {1, 1, 1, 1};
        cout << "heights=[1,1,1,1] → " << largestRectangleArea(h)
             << "  (期望 4)\n";
    }
    cout << endl;
}
} // namespace Demo5

/* ============================================================
 * Demo6: LC 42 — 接雨水 (单调栈解法)
 * ============================================================ */
namespace Demo6 {

int trap(vector<int>& height) {
    stack<int> stk;
    int water = 0;

    for (int i = 0; i < (int)height.size(); i++) {
        while (!stk.empty() && height[i] > height[stk.top()]) {
            int bottom = stk.top(); stk.pop();
            if (stk.empty()) break;  // 左边没有墙, 无法积水
            int left = stk.top();
            int w = i - left - 1;
            int h = min(height[left], height[i]) - height[bottom];
            water += w * h;
        }
        stk.push(i);
    }
    return water;
}

void run() {
    cout << "===== Demo6: LC 42 接雨水 (单调栈) =====\n";
    {
        vector<int> h = {0,1,0,2,1,0,1,3,2,1,2,1};
        cout << "height=[0,1,0,2,1,0,1,3,2,1,2,1] → " << trap(h)
             << "  (期望 6)\n";
    }
    {
        vector<int> h = {4,2,0,3,2,5};
        cout << "height=[4,2,0,3,2,5] → " << trap(h)
             << "  (期望 9)\n";
    }
    cout << endl;
}
} // namespace Demo6

/* ============================================================
 * Demo7: LC 85 — 最大矩形 (逐行转柱状图 → LC 84)
 * ============================================================ */
namespace Demo7 {

int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    stack<int> stk;
    int maxArea = 0;
    heights.push_back(0);
    for (int i = 0; i <= n; i++) {
        while (!stk.empty() && heights[i] < heights[stk.top()]) {
            int h = heights[stk.top()]; stk.pop();
            int w = stk.empty() ? i : i - stk.top() - 1;
            maxArea = max(maxArea, h * w);
        }
        stk.push(i);
    }
    heights.pop_back();
    return maxArea;
}

int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty()) return 0;
    int m = matrix.size(), n = matrix[0].size();
    vector<int> heights(n, 0);
    int maxArea = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            heights[j] = (matrix[i][j] == '1') ? heights[j] + 1 : 0;
        maxArea = max(maxArea, largestRectangleArea(heights));
    }
    return maxArea;
}

void run() {
    cout << "===== Demo7: LC 85 最大矩形 =====\n";
    vector<vector<char>> matrix = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    cout << "matrix:\n";
    for (auto& row : matrix) {
        cout << "  ";
        for (char c : row) cout << c << ' ';
        cout << '\n';
    }
    cout << "最大矩形面积: " << maximalRectangle(matrix)
         << "  (期望 6)\n\n";
}
} // namespace Demo7

/* ============================================================
 * Demo8: LC 402 — 移掉K位数字
 * ============================================================ */
namespace Demo8 {

string removeKdigits(string num, int k) {
    string stk;

    for (char c : num) {
        while (k > 0 && !stk.empty() && stk.back() > c) {
            stk.pop_back();
            k--;
        }
        stk.push_back(c);
    }

    // 剩余要删的从末尾删
    while (k > 0) { stk.pop_back(); k--; }

    // 去除前导零
    int start = 0;
    while (start < (int)stk.size() && stk[start] == '0') start++;
    string result = stk.substr(start);
    return result.empty() ? "0" : result;
}

void run() {
    cout << "===== Demo8: LC 402 移掉K位数字 =====\n";
    cout << "\"1432219\", k=3 → \"" << removeKdigits("1432219", 3)
         << "\"  (期望 \"1219\")\n";
    cout << "\"10200\", k=1 → \"" << removeKdigits("10200", 1)
         << "\"  (期望 \"200\")\n";
    cout << "\"10\", k=2 → \"" << removeKdigits("10", 2)
         << "\"  (期望 \"0\")\n";
    cout << "\"112\", k=1 → \"" << removeKdigits("112", 1)
         << "\"  (期望 \"11\")\n\n";
}
} // namespace Demo8

/* ============================================================
 * Demo9: LC 316 — 去除重复字母 (最小字典序)
 * ============================================================ */
namespace Demo9 {

string removeDuplicateLetters(string s) {
    int lastIdx[26] = {};
    bool inStack[26] = {};

    for (int i = 0; i < (int)s.size(); i++)
        lastIdx[s[i] - 'a'] = i;

    string stk;
    for (int i = 0; i < (int)s.size(); i++) {
        int c = s[i] - 'a';
        if (inStack[c]) continue;

        while (!stk.empty() && stk.back() > s[i]
               && lastIdx[stk.back() - 'a'] > i) {
            inStack[stk.back() - 'a'] = false;
            stk.pop_back();
        }

        stk.push_back(s[i]);
        inStack[c] = true;
    }
    return stk;
}

void run() {
    cout << "===== Demo9: LC 316 去除重复字母 =====\n";
    cout << "\"bcabc\" → \"" << removeDuplicateLetters("bcabc")
         << "\"  (期望 \"abc\")\n";
    cout << "\"cbacdcbc\" → \"" << removeDuplicateLetters("cbacdcbc")
         << "\"  (期望 \"acdb\")\n";
    cout << "\"abacb\" → \"" << removeDuplicateLetters("abacb")
         << "\"  (期望 \"abc\")\n\n";
}
} // namespace Demo9

/* ============================================================
 * Demo10: LC 907 — 子数组的最小值之和 (贡献法)
 * ============================================================ */
namespace Demo10 {

int sumSubarrayMins(vector<int>& arr) {
    int n = arr.size();
    const int MOD = 1e9 + 7;
    vector<int> left(n), right(n);
    stack<int> stk;

    // 左边界: 左边第一个严格小于的位置 (>=弹出)
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && arr[stk.top()] >= arr[i])
            stk.pop();
        left[i] = stk.empty() ? -1 : stk.top();
        stk.push(i);
    }

    while (!stk.empty()) stk.pop();

    // 右边界: 右边第一个严格小于的位置 (>弹出, 处理重复)
    for (int i = n - 1; i >= 0; i--) {
        while (!stk.empty() && arr[stk.top()] > arr[i])
            stk.pop();
        right[i] = stk.empty() ? n : stk.top();
        stk.push(i);
    }

    long long sum = 0;
    for (int i = 0; i < n; i++) {
        long long cnt = (long long)(i - left[i]) * (right[i] - i);
        sum = (sum + (long long)arr[i] * cnt) % MOD;
    }
    return (int)sum;
}

void run() {
    cout << "===== Demo10: LC 907 子数组最小值之和 (贡献法) =====\n";
    {
        vector<int> arr = {3, 1, 2, 4};
        cout << "arr=[3,1,2,4] → " << sumSubarrayMins(arr)
             << "  (期望 17)\n";
    }
    {
        vector<int> arr = {11, 81, 94, 43, 3};
        cout << "arr=[11,81,94,43,3] → " << sumSubarrayMins(arr)
             << "  (期望 444)\n";
    }
    cout << endl;
}
} // namespace Demo10

/* ============================================================
 * Demo11: LC 901 — 股票价格跨度
 * 每天的跨度 = 包含今天的连续天数中股价 ≤ 今天
 * ============================================================ */
namespace Demo11 {

class StockSpanner {
    stack<pair<int, int>> stk;  // {价格, 跨度}
public:
    int next(int price) {
        int span = 1;
        while (!stk.empty() && stk.top().first <= price) {
            span += stk.top().second;
            stk.pop();
        }
        stk.push({price, span});
        return span;
    }
};

void run() {
    cout << "===== Demo11: LC 901 股票价格跨度 =====\n";
    StockSpanner sp;
    vector<int> prices = {100, 80, 60, 70, 60, 75, 85};
    vector<int> expected = {1, 1, 1, 2, 1, 4, 6};

    cout << "prices: [";
    for (int i = 0; i < (int)prices.size(); i++) {
        if (i) cout << ", ";
        cout << prices[i];
    }
    cout << "]\n";

    cout << "跨度:  [";
    for (int i = 0; i < (int)prices.size(); i++) {
        if (i) cout << ", ";
        int span = sp.next(prices[i]);
        cout << span;
    }
    cout << "]\n";
    cout << "期望:  [1, 1, 1, 2, 1, 4, 6]\n\n";
}
} // namespace Demo11

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════════╗\n";
    cout << "║  专题十七（上）：单调栈 Monotonic Stack — 示例代码    ║\n";
    cout << "║  作者：大胖超 😜                                    ║\n";
    cout << "╚══════════════════════════════════════════════════════╝\n\n";

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
