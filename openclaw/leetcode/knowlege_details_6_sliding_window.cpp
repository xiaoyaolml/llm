/**
 * ============================================================
 *   专题六：滑动窗口（Sliding Window）示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_6_sliding_window knowlege_details_6_sliding_window.cpp
 *   配合 knowlege_details_6_sliding_window.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <deque>
#include <climits>
#include <algorithm>
using namespace std;

// ============================================================
//  工具函数
// ============================================================
void printVec(const vector<int>& v, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    cout << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        if (i) cout << ", ";
        cout << v[i];
    }
    cout << "]" << endl;
}

// ============================================================
//  Demo 1: 可变窗口（最长型）— LC 3 无重复字符的最长子串
//  两种写法对比：哈希计数法 & 最后位置跳跃法
// ============================================================

// 写法一：哈希计数法
int lengthOfLongestSubstring_v1(string s) {
    unordered_map<char, int> window;  // 字符 → 窗口内出现次数
    int left = 0, maxLen = 0;

    for (int right = 0; right < (int)s.size(); right++) {
        window[s[right]]++;

        // 出现重复 → 收缩左端直到不重复
        while (window[s[right]] > 1) {
            window[s[left]]--;
            left++;
        }

        // 此时窗口 [left, right] 无重复
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

// 写法二：最后位置跳跃法（更快，一步到位）
int lengthOfLongestSubstring_v2(string s) {
    unordered_map<char, int> lastPos;  // 字符 → 上次出现的下标
    int left = 0, maxLen = 0;

    for (int right = 0; right < (int)s.size(); right++) {
        // 如果字符在窗口内出现过，left 直接跳到其后面
        if (lastPos.count(s[right]) && lastPos[s[right]] >= left) {
            left = lastPos[s[right]] + 1;
        }
        lastPos[s[right]] = right;
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

void demo_longest_no_repeat() {
    cout << "\n===== Demo 1: LC 3 无重复字符的最长子串 =====" << endl;

    vector<pair<string, int>> tests = {
        {"abcabcbb", 3},   // "abc"
        {"bbbbb", 1},      // "b"
        {"pwwkew", 3},     // "wke"
        {"", 0},
        {"abcdef", 6},     // 全不重复
        {"dvdf", 3},       // "vdf"
    };

    for (auto& [s, expected] : tests) {
        int r1 = lengthOfLongestSubstring_v1(s);
        int r2 = lengthOfLongestSubstring_v2(s);
        cout << "\"" << s << "\" → v1=" << r1 << ", v2=" << r2
             << " (期望=" << expected << ")"
             << (r1 == expected && r2 == expected ? " ✓" : " ✗") << endl;
    }

    // 手动模拟 "abcabcbb"
    cout << "\n手动模拟 \"abcabcbb\":" << endl;
    cout << "  right=0: 'a', window={a:1}, [0,0] len=1" << endl;
    cout << "  right=1: 'b', window={a:1,b:1}, [0,1] len=2" << endl;
    cout << "  right=2: 'c', window={a:1,b:1,c:1}, [0,2] len=3 ★maxLen=3" << endl;
    cout << "  right=3: 'a' 重复! → left 收缩到 1, [1,3] len=3" << endl;
    cout << "  right=4: 'b' 重复! → left 收缩到 2, [2,4] len=3" << endl;
    cout << "  right=5: 'c' 重复! → left 收缩到 3, [3,5] len=3" << endl;
    cout << "  right=6: 'b' 重复! → left 收缩到 5, [5,6] len=2" << endl;
    cout << "  right=7: 'b' 重复! → left 收缩到 7, [7,7] len=1" << endl;
    cout << "  最终 maxLen = 3" << endl;
}

// ============================================================
//  Demo 2: 可变窗口（最长型）— LC 904 水果成篮 & LC 1004
//  LC 904: 最多 2 种水果的最长子数组
//  LC 1004: 最多翻转 k 个 0 的最长连续 1
// ============================================================

// LC 904: 水果成篮
int totalFruit(vector<int>& fruits) {
    unordered_map<int, int> window;  // 水果类型 → 个数
    int left = 0, maxLen = 0;

    for (int right = 0; right < (int)fruits.size(); right++) {
        window[fruits[right]]++;

        // 超过 2 种 → 收缩
        while ((int)window.size() > 2) {
            window[fruits[left]]--;
            if (window[fruits[left]] == 0)
                window.erase(fruits[left]);
            left++;
        }

        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

// LC 1004: 最大连续 1 的个数 III
int longestOnes(vector<int>& nums, int k) {
    int left = 0, zeros = 0, maxLen = 0;

    for (int right = 0; right < (int)nums.size(); right++) {
        if (nums[right] == 0) zeros++;

        // 0 的个数超过 k → 收缩
        while (zeros > k) {
            if (nums[left] == 0) zeros--;
            left++;
        }

        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

void demo_variable_max() {
    cout << "\n===== Demo 2: 可变窗口（最长型）=====" << endl;

    // LC 904
    cout << "\n--- LC 904: 水果成篮 ---" << endl;
    vector<int> f1 = {1, 2, 1};
    printVec(f1, "fruits");
    cout << "最多水果: " << totalFruit(f1) << " (期望=3)" << endl;

    vector<int> f2 = {0, 1, 2, 2};
    printVec(f2, "fruits");
    cout << "最多水果: " << totalFruit(f2) << " (期望=3)" << endl;

    vector<int> f3 = {1, 2, 3, 2, 2};
    printVec(f3, "fruits");
    cout << "最多水果: " << totalFruit(f3) << " (期望=4)" << endl;

    vector<int> f4 = {3, 3, 3, 1, 2, 1, 1, 2, 3, 3, 4};
    printVec(f4, "fruits");
    cout << "最多水果: " << totalFruit(f4) << " (期望=5)" << endl;

    // LC 1004
    cout << "\n--- LC 1004: 最大连续 1 的个数 III ---" << endl;
    vector<int> n1 = {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0};
    int k1 = 2;
    printVec(n1, "nums");
    cout << "k=" << k1 << ", 最长连续1: " << longestOnes(n1, k1)
         << " (期望=6)" << endl;

    vector<int> n2 = {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1};
    int k2 = 3;
    printVec(n2, "nums");
    cout << "k=" << k2 << ", 最长连续1: " << longestOnes(n2, k2)
         << " (期望=10)" << endl;
}

// ============================================================
//  Demo 3: 可变窗口（最长型）— LC 424 替换后的最长重复字符
// ============================================================

int characterReplacement(string s, int k) {
    vector<int> count(26, 0);
    int left = 0, maxFreq = 0, maxLen = 0;

    for (int right = 0; right < (int)s.size(); right++) {
        count[s[right] - 'A']++;
        // maxFreq = 窗口内出现次数最多的字符
        maxFreq = max(maxFreq, count[s[right] - 'A']);

        // 需要替换的字符数 = 窗口长度 - maxFreq > k → 收缩
        while (right - left + 1 - maxFreq > k) {
            count[s[left] - 'A']--;
            left++;
            // 注意: 这里不重新计算 maxFreq，不影响正确性！
            // 因为只有更大的 maxFreq 才能产生更长的答案
        }

        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

void demo_char_replacement() {
    cout << "\n===== Demo 3: LC 424 替换后的最长重复字符 =====" << endl;

    vector<tuple<string, int, int>> tests = {
        {"ABAB", 2, 4},       // 全替换
        {"AABABBA", 1, 4},    // "AABA"→"AAAA"
        {"AAAA", 0, 4},       // 已经是
        {"ABCDE", 2, 3},      // 任意连续3个替换2个
    };

    for (auto& [s, k, expected] : tests) {
        int res = characterReplacement(s, k);
        cout << "\"" << s << "\", k=" << k << " → " << res
             << " (期望=" << expected << ")"
             << (res == expected ? " ✓" : " ✗") << endl;
    }

    // 解释 maxFreq 不减少的原因
    cout << "\n💡 为什么 maxFreq 不需要在收缩时更新？" << endl;
    cout << "   因为只有 maxFreq 增大时窗口才可能变得更长。" << endl;
    cout << "   即使 maxFreq 过时（偏大），不会漏掉更优解。" << endl;
}

// ============================================================
//  Demo 4: 可变窗口（最短型）— LC 76 最小覆盖子串
// ============================================================

string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;

    int left = 0, valid = 0;
    int start = 0, minLen = INT_MAX;

    for (int right = 0; right < (int)s.size(); right++) {
        // ① 入窗
        char c = s[right];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;  // 这种字符刚好满足需求
        }

        // ② 窗口已覆盖所有字符 → 收缩并更新答案
        while (valid == (int)need.size()) {
            // 先更新答案
            if (right - left + 1 < minLen) {
                start = left;
                minLen = right - left + 1;
            }

            // 出窗
            char d = s[left];
            left++;
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;  // 不再满足
                window[d]--;
            }
        }
    }
    return minLen == INT_MAX ? "" : s.substr(start, minLen);
}

void demo_min_window() {
    cout << "\n===== Demo 4: LC 76 最小覆盖子串 =====" << endl;

    vector<tuple<string, string, string>> tests = {
        {"ADOBECODEBANC", "ABC", "BANC"},
        {"a", "a", "a"},
        {"a", "aa", ""},     // 不够
        {"aa", "aa", "aa"},
        {"cabwefgewcwaefgcf", "cae", "cwae"},
    };

    for (auto& [s, t, expected] : tests) {
        string res = minWindow(s, t);
        cout << "s=\"" << s << "\", t=\"" << t << "\" → \"" << res << "\""
             << " (期望=\"" << expected << "\")"
             << (res == expected ? " ✓" : " ✗") << endl;
    }

    // 手动模拟简化版
    cout << "\n手动模拟 s=\"ADOBECODEBANC\", t=\"ABC\":" << endl;
    cout << "  need={A:1, B:1, C:1}" << endl;
    cout << "  right→A: valid=1 (A满足)" << endl;
    cout << "  right→D,O: valid=1" << endl;
    cout << "  right→B: valid=2 (B满足)" << endl;
    cout << "  right→E: valid=2" << endl;
    cout << "  right→C: valid=3 ★全满足! → 收缩" << endl;
    cout << "    minLen=6 (ADOBEC), left→D: valid=2 (A不足)" << endl;
    cout << "  ... 继续右扩 ..." << endl;
    cout << "  right→A: valid=3 ★ → 收缩找更短" << endl;
    cout << "    → BANC (长度4) ★最终答案" << endl;
}

// ============================================================
//  Demo 5: 可变窗口（最短型）— LC 209 长度最小的子数组
// ============================================================

int minSubArrayLen(int target, vector<int>& nums) {
    int left = 0, minLen = INT_MAX;
    int sum = 0;

    for (int right = 0; right < (int)nums.size(); right++) {
        sum += nums[right];

        // sum >= target → 记录答案并收缩
        while (sum >= target) {
            minLen = min(minLen, right - left + 1);
            sum -= nums[left];
            left++;
        }
    }
    return minLen == INT_MAX ? 0 : minLen;
}

void demo_min_subarray() {
    cout << "\n===== Demo 5: LC 209 长度最小的子数组 =====" << endl;

    vector<int> nums1 = {2, 3, 1, 2, 4, 3};
    int t1 = 7;
    printVec(nums1, "nums");
    cout << "target=" << t1 << ", 最短长度: " << minSubArrayLen(t1, nums1)
         << " (期望=2, 即[4,3])" << endl;

    vector<int> nums2 = {1, 4, 4};
    int t2 = 4;
    printVec(nums2, "nums");
    cout << "target=" << t2 << ", 最短长度: " << minSubArrayLen(t2, nums2)
         << " (期望=1, 即[4])" << endl;

    vector<int> nums3 = {1, 1, 1, 1, 1, 1, 1, 1};
    int t3 = 11;
    printVec(nums3, "nums");
    cout << "target=" << t3 << ", 最短长度: " << minSubArrayLen(t3, nums3)
         << " (期望=0, 无法达到)" << endl;
}

// ============================================================
//  Demo 6: 固定窗口 — LC 438 & LC 567
//  LC 438: 找到字符串中所有字母异位词
//  LC 567: 字符串的排列
// ============================================================

// LC 438: 找到字符串中所有字母异位词
vector<int> findAnagrams(string s, string p) {
    vector<int> result;
    if (s.size() < p.size()) return result;

    vector<int> need(26, 0), window(26, 0);
    for (char c : p) need[c - 'a']++;

    for (int right = 0; right < (int)s.size(); right++) {
        // 入窗
        window[s[right] - 'a']++;

        // 窗口超过 p 的长度 → 左端出窗
        if (right >= (int)p.size()) {
            window[s[right - p.size()] - 'a']--;
        }

        // 窗口大小 == |p| 且频率匹配
        if (window == need) {
            result.push_back(right - (int)p.size() + 1);
        }
    }
    return result;
}

// LC 567: 字符串的排列
bool checkInclusion(string s1, string s2) {
    if (s1.size() > s2.size()) return false;

    vector<int> cnt1(26, 0), cnt2(26, 0);
    int n = s1.size();
    for (int i = 0; i < n; i++) {
        cnt1[s1[i] - 'a']++;
        cnt2[s2[i] - 'a']++;
    }
    if (cnt1 == cnt2) return true;

    for (int i = n; i < (int)s2.size(); i++) {
        cnt2[s2[i] - 'a']++;
        cnt2[s2[i - n] - 'a']--;
        if (cnt1 == cnt2) return true;
    }
    return false;
}

void demo_fixed_window_anagram() {
    cout << "\n===== Demo 6: 固定窗口 — 异位词 & 排列 =====" << endl;

    // LC 438
    cout << "\n--- LC 438: 字母异位词 ---" << endl;
    {
        string s = "cbaebabacd", p = "abc";
        cout << "s=\"" << s << "\", p=\"" << p << "\"" << endl;
        auto res = findAnagrams(s, p);
        printVec(res, "起始索引");  // [0, 6]
        cout << "  s[0..2]=\"cba\" ✓  s[6..8]=\"bac\" ✓" << endl;
    }
    {
        string s = "abab", p = "ab";
        cout << "s=\"" << s << "\", p=\"" << p << "\"" << endl;
        auto res = findAnagrams(s, p);
        printVec(res, "起始索引");  // [0, 1, 2]
    }

    // LC 567
    cout << "\n--- LC 567: 字符串的排列 ---" << endl;
    cout << "s1=\"ab\", s2=\"eidbaooo\" → "
         << (checkInclusion("ab", "eidbaooo") ? "true" : "false")
         << " (期望=true, \"ba\")" << endl;
    cout << "s1=\"ab\", s2=\"eidboaoo\" → "
         << (checkInclusion("ab", "eidboaoo") ? "true" : "false")
         << " (期望=false)" << endl;
}

// ============================================================
//  Demo 7: 固定窗口 — LC 643 子数组最大平均数 I
//                   & LC 239 滑动窗口最大值（单调队列）
// ============================================================

// LC 643: 子数组最大平均数 I
double findMaxAverage(vector<int>& nums, int k) {
    int sum = 0;
    for (int i = 0; i < k; i++) sum += nums[i];

    int maxSum = sum;
    for (int i = k; i < (int)nums.size(); i++) {
        sum += nums[i] - nums[i - k];   // 右入左出
        maxSum = max(maxSum, sum);
    }
    return (double)maxSum / k;
}

// LC 239: 滑动窗口最大值（单调队列）
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;  // 存下标，对应值保持递减
    vector<int> result;

    for (int i = 0; i < (int)nums.size(); i++) {
        // ① 移除超出窗口的队头
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        // ② 维护递减性：弹出队尾所有比当前小的
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();

        // ③ 入队
        dq.push_back(i);

        // ④ 窗口满了 → 队头就是最大值
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}

void demo_fixed_window_stats() {
    cout << "\n===== Demo 7: 固定窗口（统计 & 单调队列）=====" << endl;

    // LC 643
    cout << "\n--- LC 643: 子数组最大平均数 I ---" << endl;
    vector<int> nums1 = {1, 12, -5, -6, 50, 3};
    int k1 = 4;
    printVec(nums1, "nums");
    cout << "k=" << k1 << ", 最大平均数: " << findMaxAverage(nums1, k1)
         << " (期望=12.75)" << endl;
    cout << "  窗口 [12,-5,-6,50] → sum=51 → 51/4=12.75" << endl;

    // LC 239
    cout << "\n--- LC 239: 滑动窗口最大值 ---" << endl;
    vector<int> nums2 = {1, 3, -1, -3, 5, 3, 6, 7};
    int k2 = 3;
    printVec(nums2, "nums");
    cout << "k=" << k2 << endl;
    auto res = maxSlidingWindow(nums2, k2);
    printVec(res, "窗口最大值");  // [3, 3, 5, 5, 6, 7]

    cout << "\n单调队列过程:" << endl;
    cout << "  i=0: dq=[0(1)]" << endl;
    cout << "  i=1: 3>1 弹出0, dq=[1(3)]" << endl;
    cout << "  i=2: dq=[1(3),2(-1)], 窗口满 → max=3" << endl;
    cout << "  i=3: dq=[1(3),3(-3)], 窗口满 → max=3" << endl;
    cout << "  i=4: 5>-3弹,-3弹 5>3弹, dq=[4(5)], 窗口满 → max=5" << endl;
    cout << "  i=5: dq=[4(5),5(3)], 窗口满 → max=5" << endl;
    cout << "  i=6: 6>3弹 6>5弹, dq=[6(6)], 窗口满 → max=6" << endl;
    cout << "  i=7: 7>6弹, dq=[7(7)], 窗口满 → max=7" << endl;
}

// ============================================================
//  Demo 8: 综合进阶
//  LC 30: 串联所有单词的子串
//  LC 395: 至少有 K 个重复字符的最长子串
// ============================================================

// LC 30: 串联所有单词的子串
vector<int> findSubstring(string s, vector<string>& words) {
    vector<int> result;
    if (words.empty() || s.empty()) return result;

    int wordLen = words[0].size();
    int wordCnt = words.size();
    int totalLen = wordLen * wordCnt;
    if ((int)s.size() < totalLen) return result;

    unordered_map<string, int> need;
    for (auto& w : words) need[w]++;

    // 枚举每个起始偏移 [0, wordLen)
    for (int i = 0; i < wordLen; i++) {
        unordered_map<string, int> window;
        int valid = 0;

        for (int j = i; j + wordLen <= (int)s.size(); j += wordLen) {
            string word = s.substr(j, wordLen);

            // 出窗：窗口超过 totalLen
            if (j >= i + totalLen) {
                string out = s.substr(j - totalLen, wordLen);
                if (need.count(out)) {
                    if (window[out] == need[out]) valid--;
                    window[out]--;
                }
            }

            // 入窗
            if (need.count(word)) {
                window[word]++;
                if (window[word] == need[word]) valid++;
            }

            // 检查
            if (valid == (int)need.size())
                result.push_back(j - totalLen + wordLen);
        }
    }
    sort(result.begin(), result.end());
    return result;
}

// LC 395: 至少有 K 个重复字符的最长子串
int longestSubstring(string s, int k) {
    int maxLen = 0;

    // 技巧：枚举窗口内的不同字符种类数 1~26
    for (int uniqueTarget = 1; uniqueTarget <= 26; uniqueTarget++) {
        vector<int> count(26, 0);
        int left = 0;
        int unique = 0;     // 窗口内不同字符种类
        int atLeastK = 0;   // 达到 k 次的种类数

        for (int right = 0; right < (int)s.size(); right++) {
            int idx = s[right] - 'a';
            if (count[idx] == 0) unique++;
            count[idx]++;
            if (count[idx] == k) atLeastK++;

            // 种类数超标 → 收缩
            while (unique > uniqueTarget) {
                int lidx = s[left] - 'a';
                if (count[lidx] == k) atLeastK--;
                count[lidx]--;
                if (count[lidx] == 0) unique--;
                left++;
            }

            // 种类数恰好 = uniqueTarget 且都达到 k 次
            if (unique == uniqueTarget && atLeastK == uniqueTarget)
                maxLen = max(maxLen, right - left + 1);
        }
    }
    return maxLen;
}

void demo_advanced() {
    cout << "\n===== Demo 8: 综合进阶 =====" << endl;

    // LC 30
    cout << "\n--- LC 30: 串联所有单词的子串 ---" << endl;
    {
        string s = "barfoothefoobarman";
        vector<string> words = {"foo", "bar"};
        cout << "s=\"" << s << "\", words=[\"foo\",\"bar\"]" << endl;
        auto res = findSubstring(s, words);
        printVec(res, "起始索引");  // [0, 9]
        cout << "  s[0..5]=\"barfoo\" ✓  s[9..14]=\"foobar\" ✓" << endl;
    }
    {
        string s = "wordgoodgoodgoodbestword";
        vector<string> words = {"word", "good", "best", "word"};
        cout << "s=\"" << s << "\", words=[\"word\",\"good\",\"best\",\"word\"]" << endl;
        auto res = findSubstring(s, words);
        printVec(res, "起始索引");  // []
    }
    {
        string s = "wordgoodgoodgoodbestword";
        vector<string> words = {"word", "good", "best", "good"};
        cout << "s=\"" << s << "\"" << endl;
        auto res = findSubstring(s, words);
        printVec(res, "起始索引");  // [8]
    }

    // LC 395
    cout << "\n--- LC 395: 至少有 K 个重复字符的最长子串 ---" << endl;
    {
        string s = "aaabb";
        int k = 3;
        cout << "s=\"" << s << "\", k=" << k << " → " << longestSubstring(s, k)
             << " (期望=3, \"aaa\")" << endl;
    }
    {
        string s = "ababbc";
        int k = 2;
        cout << "s=\"" << s << "\", k=" << k << " → " << longestSubstring(s, k)
             << " (期望=5, \"ababb\")" << endl;
    }
    {
        string s = "weitong";
        int k = 2;
        cout << "s=\"" << s << "\", k=" << k << " → " << longestSubstring(s, k)
             << " (期望=0, 无)" << endl;
    }

    cout << "\n💡 LC 395 技巧解析:" << endl;
    cout << "  直接滑窗不行：\"每个字符≥k次\" 不满足单调性" << endl;
    cout << "  解法：枚举窗口内不同字符种类数 1~26" << endl;
    cout << "  对每种做滑窗 → 种类超标收缩 → O(26n)=O(n)" << endl;
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题六：滑动窗口 Sliding Window 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_longest_no_repeat();     // Demo 1: LC 3 两种写法
    demo_variable_max();          // Demo 2: LC 904, 1004
    demo_char_replacement();      // Demo 3: LC 424
    demo_min_window();            // Demo 4: LC 76
    demo_min_subarray();          // Demo 5: LC 209
    demo_fixed_window_anagram();  // Demo 6: LC 438, 567
    demo_fixed_window_stats();    // Demo 7: LC 643, 239
    demo_advanced();              // Demo 8: LC 30, 395

    cout << "\n============================================" << endl;
    cout << "  所有滑动窗口 Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 3, 30, 76, 209, 239, 395," << endl;
    cout << "  424, 438, 567, 643, 904, 1004" << endl;
    cout << "============================================" << endl;

    return 0;
}
