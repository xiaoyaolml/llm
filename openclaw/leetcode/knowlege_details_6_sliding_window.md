# 专题六：滑动窗口（Sliding Window）详解

> 作者：大胖超 😜
> 配套代码：`knowlege_details_6_sliding_window.cpp`

---

## 目录

1. [核心思想](#1-核心思想)
2. [两类窗口模型](#2-两类窗口模型)
3. [万能模板](#3-万能模板)
4. [可变窗口 — 最长型](#4-可变窗口--最长型)
5. [可变窗口 — 最短型](#5-可变窗口--最短型)
6. [固定窗口](#6-固定窗口)
7. [辅助数据结构选型](#7-辅助数据结构选型)
8. [N-sum 到 滑窗：题目特征识别](#8-n-sum-到-滑窗题目特征识别)
9. [经典题型精讲](#9-经典题型精讲)
10. [必刷题目清单](#10-必刷题目清单)
11. [常见陷阱与调试技巧](#11-常见陷阱与调试技巧)
12. [总结与面试要点](#12-总结与面试要点)

---

## 1. 核心思想

### 1.1 什么是滑动窗口？

滑动窗口是**双指针的特化形式**，用两个同向移动的指针 `left` 和 `right` 维护一个**连续子数组/子串**。

```
数组:  [a, b, c, d, e, f, g, h]
           ↑           ↑
          left        right
          ←—— 窗口 ——→
```

### 1.2 为什么能用滑动窗口？

关键前提：**单调性**（也称"可行性递推"）——

- 当窗口满足条件时，缩小窗口**可能不满足**
- 当窗口不满足条件时，扩大窗口**可能满足**

这意味着 `right` 只往右走，`left` 也只往右走，**总移动量 O(n)**。

### 1.3 时间复杂度

| 方法 | 时间 | 空间 |
|------|------|------|
| 暴力枚举所有子串 | O(n²) 或 O(n³) | — |
| 滑动窗口 | **O(n)** | O(k)，k 为窗口辅助结构大小 |

### 1.4 与双指针的关系

| 技巧 | 指针方向 | 典型场景 |
|------|----------|----------|
| 对撞指针 | ← → 相向 | 有序数组两数之和 |
| 快慢指针 | → → 同向不同速 | 链表环检测 |
| **滑动窗口** | **→ → 同向同速** | **连续子数组/子串** |

---

## 2. 两类窗口模型

### 2.1 可变窗口（Variable-size Window）

窗口大小随条件动态变化。

| 子类 | 目标 | 收缩时机 | 典型题 |
|------|------|----------|--------|
| **最长型** | 求满足条件的最长子串 | 条件被破坏时收缩 | LC 3, 159, 340, 424, 904 |
| **最短型** | 求满足条件的最短子串 | 条件满足后收缩 | LC 76, 209, 862 |

**核心区别**：
- 最长型：在窗口合法时更新答案，不合法时收缩
- 最短型：在窗口合法时收缩并更新答案

### 2.2 固定窗口（Fixed-size Window）

窗口大小固定为 k。

| 特点 | 说明 |
|------|------|
| 右指针每移一步 | 左指针也移一步 |
| 无需 while 收缩 | if (right - left + 1 == k) 时处理 |
| 典型题 | LC 438, 567, 643, 1004, 1456 |

---

## 3. 万能模板

### 3.1 最长型模板（找满足条件的最长窗口）

```cpp
int slidingWindowMax(string/vector s) {
    unordered_map<char, int> window;
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < s.size(); right++) {
        // 1. 右端元素入窗
        window[s[right]]++;
        
        // 2. 窗口不合法时，收缩左端
        while (窗口不满足条件) {
            window[s[left]]--;
            if (window[s[left]] == 0) window.erase(s[left]);
            left++;
        }
        
        // 3. 此时窗口合法，更新答案
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

### 3.2 最短型模板（找满足条件的最短窗口）

```cpp
int slidingWindowMin(string s, ...) {
    unordered_map<char, int> need, window;
    // 初始化 need
    
    int left = 0, minLen = INT_MAX;
    int valid = 0;  // 已满足的条件数
    
    for (int right = 0; right < s.size(); right++) {
        // 1. 右端元素入窗，更新状态
        char c = s[right];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;
        }
        
        // 2. 窗口满足条件，收缩左端并更新答案
        while (valid == need.size()) {
            minLen = min(minLen, right - left + 1);
            
            char d = s[left];
            left++;
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;
                window[d]--;
            }
        }
    }
    return minLen == INT_MAX ? 0 : minLen;
}
```

### 3.3 固定窗口模板

```cpp
void fixedWindow(vector<int>& nums, int k) {
    // 可选：初始化窗口 [0, k-1]
    for (int right = 0; right < nums.size(); right++) {
        // 1. 右端元素入窗
        
        // 2. 窗口未满，继续扩展
        if (right < k - 1) continue;
        
        // 3. 窗口已满，记录答案
        // ans = ...
        
        // 4. 左端元素出窗
        // 移除 nums[right - k + 1]
    }
}
```

---

## 4. 可变窗口 — 最长型

### 4.1 LC 3: 无重复字符的最长子串 ⭐⭐⭐

**题意**：给定字符串 s，找不含重复字符的最长子串长度。

**思路**：窗口内维护无重复字符，出现重复时收缩。

```cpp
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> window;  // 字符出现次数
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < s.size(); right++) {
        window[s[right]]++;
        
        // 出现重复 → 收缩
        while (window[s[right]] > 1) {
            window[s[left]]--;
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
// 输入: "abcabcbb" → 3 ("abc")
// 输入: "bbbbb" → 1 ("b")
// 输入: "pwwkew" → 3 ("wke")
```

**优化版（用 last_pos 跳跃）**：

```cpp
int lengthOfLongestSubstring_v2(string s) {
    unordered_map<char, int> lastPos;  // 字符 → 最后出现位置
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < s.size(); right++) {
        if (lastPos.count(s[right]) && lastPos[s[right]] >= left) {
            left = lastPos[s[right]] + 1;  // 直接跳过
        }
        lastPos[s[right]] = right;
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

### 4.2 LC 904: 水果成篮

**题意**：给定整数数组 fruits，找只包含最多两种类型水果的最长连续子数组。

等价于：**最多包含 2 种不同数字的最长子数组**。

```cpp
int totalFruit(vector<int>& fruits) {
    unordered_map<int, int> window;  // 水果类型 → 个数
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < fruits.size(); right++) {
        window[fruits[right]]++;
        
        while (window.size() > 2) {            // 超过 2 种
            window[fruits[left]]--;
            if (window[fruits[left]] == 0)
                window.erase(fruits[left]);
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
// 输入: [1,2,1] → 3
// 输入: [0,1,2,2] → 3 ([1,2,2])
// 输入: [1,2,3,2,2] → 4 ([2,3,2,2])
```

### 4.3 LC 424: 替换后的最长重复字符

**题意**：给字符串 s 和整数 k，最多替换 k 个字符，使得结果中最长的重复字母子串最长。

**核心公式**：`窗口长度 - 窗口内最多的字符个数 ≤ k`

```cpp
int characterReplacement(string s, int k) {
    vector<int> count(26, 0);
    int left = 0, maxFreq = 0, maxLen = 0;
    
    for (int right = 0; right < s.size(); right++) {
        count[s[right] - 'A']++;
        maxFreq = max(maxFreq, count[s[right] - 'A']);
        
        // 需要替换的字符 > k，收缩
        while (right - left + 1 - maxFreq > k) {
            count[s[left] - 'A']--;
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
// 输入: s = "AABABBA", k = 1 → 4 ("AABA"→"AAAA")
```

> **面试追问**：maxFreq 不会在收缩时减小吗？——答：不影响正确性，因为 maxLen 只会被更大的 maxFreq 刷新。

### 4.4 LC 1004: 最大连续 1 的个数 III

**题意**：给二进制数组 nums 和整数 k，最多翻转 k 个 0，求最长连续 1 的长度。

等价于：**最多包含 k 个 0 的最长窗口**。

```cpp
int longestOnes(vector<int>& nums, int k) {
    int left = 0, zeros = 0, maxLen = 0;
    
    for (int right = 0; right < nums.size(); right++) {
        if (nums[right] == 0) zeros++;
        
        while (zeros > k) {
            if (nums[left] == 0) zeros--;
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
// 输入: [1,1,1,0,0,0,1,1,1,1,0], k=2 → 6
```

---

## 5. 可变窗口 — 最短型

### 5.1 LC 76: 最小覆盖子串 ⭐⭐⭐⭐⭐

**题意**：给字符串 s 和 t，找 s 中包含 t 所有字符的最短子串。

**这是滑动窗口的巅峰题，面试高频 Hard！**

```cpp
string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    
    int left = 0, valid = 0;
    int start = 0, minLen = INT_MAX;
    
    for (int right = 0; right < s.size(); right++) {
        // 入窗
        char c = s[right];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;
        }
        
        // 满足条件，收缩
        while (valid == need.size()) {
            // 更新答案
            if (right - left + 1 < minLen) {
                start = left;
                minLen = right - left + 1;
            }
            // 出窗
            char d = s[left];
            left++;
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;
                window[d]--;
            }
        }
    }
    return minLen == INT_MAX ? "" : s.substr(start, minLen);
}
// 输入: s = "ADOBECODEBANC", t = "ABC" → "BANC"
```

**关键细节**：
- `valid` 计数：每种字符满足需求时 +1
- 收缩时机：valid == need.size()（所有字符种类都满足）
- 出窗时先更新答案再移 left

### 5.2 LC 209: 长度最小的子数组

**题意**：给正整数数组和目标值 target，找和 ≥ target 的最短子数组长度。

```cpp
int minSubArrayLen(int target, vector<int>& nums) {
    int left = 0, sum = 0, minLen = INT_MAX;
    
    for (int right = 0; right < nums.size(); right++) {
        sum += nums[right];
        
        while (sum >= target) {
            minLen = min(minLen, right - left + 1);
            sum -= nums[left];
            left++;
        }
    }
    return minLen == INT_MAX ? 0 : minLen;
}
// 输入: target=7, nums=[2,3,1,2,4,3] → 2 ([4,3])
```

### 5.3 LC 862: 和至少为 K 的最短子数组（进阶）

> 注意：当数组包含**负数**时，窗口单调性被破坏，不能直接用滑窗！
> 需要用**前缀和 + 单调双端队列**，这已超出基础滑窗范畴。

---

## 6. 固定窗口

### 6.1 LC 438: 找到字符串中所有字母异位词 ⭐⭐⭐

**题意**：给字符串 s 和短串 p，找 s 中所有 p 的字母异位词的起始索引。

**思路**：固定窗口大小 = p.size()，比较字符频率数组。

```cpp
vector<int> findAnagrams(string s, string p) {
    vector<int> result;
    if (s.size() < p.size()) return result;
    
    vector<int> need(26, 0), window(26, 0);
    for (char c : p) need[c - 'a']++;
    
    for (int right = 0; right < s.size(); right++) {
        window[s[right] - 'a']++;
        
        // 窗口超过 p 的长度，左端出窗
        if (right >= (int)p.size()) {
            window[s[right - p.size()] - 'a']--;
        }
        
        // 窗口大小 == |p| 时比较
        if (window == need) {
            result.push_back(right - p.size() + 1);
        }
    }
    return result;
}
// 输入: s = "cbaebabacd", p = "abc" → [0, 6]
```

### 6.2 LC 567: 字符串的排列

**题意**：判断 s2 是否包含 s1 的排列。

与 LC 438 本质相同，只不过只需判断存在性。

```cpp
bool checkInclusion(string s1, string s2) {
    if (s1.size() > s2.size()) return false;
    
    vector<int> cnt1(26, 0), cnt2(26, 0);
    for (int i = 0; i < s1.size(); i++) {
        cnt1[s1[i] - 'a']++;
        cnt2[s2[i] - 'a']++;
    }
    if (cnt1 == cnt2) return true;
    
    for (int i = s1.size(); i < s2.size(); i++) {
        cnt2[s2[i] - 'a']++;
        cnt2[s2[i - s1.size()] - 'a']--;
        if (cnt1 == cnt2) return true;
    }
    return false;
}
// 输入: s1 = "ab", s2 = "eidbaooo" → true ("ba")
```

### 6.3 LC 643: 子数组最大平均数 I

```cpp
double findMaxAverage(vector<int>& nums, int k) {
    int sum = 0;
    for (int i = 0; i < k; i++) sum += nums[i];
    
    int maxSum = sum;
    for (int i = k; i < nums.size(); i++) {
        sum += nums[i] - nums[i - k];
        maxSum = max(maxSum, sum);
    }
    return (double)maxSum / k;
}
// 输入: nums = [1,12,-5,-6,50,3], k = 4 → 12.75
```

---

## 7. 辅助数据结构选型

根据窗口内需维护的信息，选择合适的辅助数据结构：

| 需求 | 数据结构 | 典型题 |
|------|----------|--------|
| 字符频率 | `int[26]` 或 `unordered_map` | LC 3, 76, 438, 567 |
| 去重判断 | `unordered_set` | LC 3 |
| 有序最值 | `multiset` / `map` | LC 220, 239 |
| 单调最值 | `deque`（单调队列） | LC 239 |
| 窗口和 | `int sum` | LC 209, 643 |
| 条件计数 | `int valid / zeros` | LC 76, 1004 |

### 7.1 字符数组 vs 哈希表

```cpp
// 当字符集确定（如小写字母），用数组更快
vector<int> count(26, 0);
count[c - 'a']++;

// 当字符集不确定（如 ASCII），用哈希表
unordered_map<char, int> window;
window[c]++;
```

### 7.2 单调队列求窗口最值

LC 239: 滑动窗口最大值（用 deque 维护单调递减队列）

```cpp
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;  // 存下标，保持对应值递减
    vector<int> result;
    
    for (int i = 0; i < nums.size(); i++) {
        // 移除超出窗口的元素
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();
        
        // 维护递减：弹出比当前小的
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();
        
        dq.push_back(i);
        
        // 窗口满了，记录答案
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}
// 输入: [1,3,-1,-3,5,3,6,7], k=3 → [3,3,5,5,6,7]
```

---

## 8. N-sum 到 滑窗：题目特征识别

### 如何判断用滑动窗口？

**关键词检测**：

| 关键词 | 大概率滑窗 |
|--------|-----------|
| "连续子数组" / "连续子串" | ✅ |
| "最长"/"最短" + "满足条件" | ✅ |
| "包含所有字符" | ✅ |
| "不含重复" | ✅ |
| "最多/至少 k 个" | ✅ |
| "固定长度 k 的子数组" | ✅ |
| "子序列" | ❌ 不连续，不用滑窗 |
| "排列组合" | ❌ |
| "有负数的子数组和" | ⚠️ 可能不适用 |

### 反例：什么时候不能用滑窗？

1. **数组有负数 + 求最短子数组和**（单调性破坏，用前缀和 + 单调栈）
2. **子序列问题**（不连续，用 DP 或贪心）
3. **需要精确组合计数**（用回溯或 DP）

---

## 9. 经典题型精讲

### 9.1 LC 30: 串联所有单词的子串

**题意**：给定字符串 s 和一个单词数组 words（长度相同），找所有串联子串的起始索引。

```
s = "barfoothefoobarman", words = ["foo","bar"]
→ [0, 9]
解释: "barfoo"(0) 和 "foobar"(9)
```

**思路**：因为每个单词长度相同，可以将问题转换为固定窗口在 "单词级别" 的滑窗。

```cpp
vector<int> findSubstring(string s, vector<string>& words) {
    vector<int> result;
    if (words.empty() || s.empty()) return result;
    
    int wordLen = words[0].size();
    int wordCnt = words.size();
    int totalLen = wordLen * wordCnt;
    
    unordered_map<string, int> need;
    for (auto& w : words) need[w]++;
    
    // 枚举每个起始偏移 [0, wordLen)
    for (int i = 0; i < wordLen; i++) {
        unordered_map<string, int> window;
        int valid = 0;
        
        for (int j = i; j + wordLen <= s.size(); j += wordLen) {
            string word = s.substr(j, wordLen);
            
            // 出窗
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
            
            if (valid == need.size())
                result.push_back(j - totalLen + wordLen);
        }
    }
    return result;
}
```

### 9.2 LC 395: 至少有 K 个重复字符的最长子串

**题意**：找 s 中每个字符都出现至少 k 次的最长子串。

**陷阱**：直接滑窗不行！因为"窗口内每个字符至少 k 次"不满足单调性。

**解法**：枚举窗口内的字符种类数 `uniqueTarget`（1~26），对每种做滑窗。

```cpp
int longestSubstring(string s, int k) {
    int maxLen = 0;
    
    // 枚举窗口内的不同字符种类数
    for (int uniqueTarget = 1; uniqueTarget <= 26; uniqueTarget++) {
        vector<int> count(26, 0);
        int left = 0, unique = 0, atLeastK = 0;
        
        for (int right = 0; right < s.size(); right++) {
            int idx = s[right] - 'a';
            if (count[idx] == 0) unique++;
            count[idx]++;
            if (count[idx] == k) atLeastK++;
            
            while (unique > uniqueTarget) {
                int lidx = s[left] - 'a';
                if (count[lidx] == k) atLeastK--;
                count[lidx]--;
                if (count[lidx] == 0) unique--;
                left++;
            }
            
            if (unique == uniqueTarget && atLeastK == uniqueTarget)
                maxLen = max(maxLen, right - left + 1);
        }
    }
    return maxLen;
}
```

> 时间 O(26n) = O(n)，是一个很巧妙的技巧。

---

## 10. 必刷题目清单

### 基础（必做）

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 3 | 无重复字符的最长子串 | Medium | 最长 | 哈希/set 去重 |
| 209 | 长度最小的子数组 | Medium | 最短 | sum ≥ target |
| 438 | 字母异位词 | Medium | 固定 | 频率数组比较 |
| 567 | 字符串的排列 | Medium | 固定 | 同 438 |
| 643 | 最大平均数子数组 | Easy | 固定 | 窗口和 |

### 进阶（面试常考）

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 76 | 最小覆盖子串 | Hard | 最短 | valid 计数 |
| 239 | 滑动窗口最大值 | Hard | 固定 | 单调队列 |
| 424 | 最长重复字符替换 | Medium | 最长 | maxFreq 技巧 |
| 904 | 水果成篮 | Medium | 最长 | ≤2 种类 |
| 1004 | 最大连续1的个数 III | Medium | 最长 | 翻转=忽略 |

### 高级（冲刺）

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 30 | 串联所有单词 | Hard | 固定 | 多偏移滑窗 |
| 159 | 至多两个不同字符 | Medium | 最长 | 类似 904 |
| 340 | 至多K个不同字符 | Medium | 最长 | 904 泛化版 |
| 395 | 至少K次重复字符 | Medium | 最长 | 枚举种类数 |
| 862 | 和至少为K | Hard | 最短 | 前缀和+单调队列 |
| 992 | K个不同整数子数组 | Hard | 恰好K | 恰好K = 至多K − 至多K-1 |

### 推荐刷题顺序

```
643(熟悉窗口) → 3(最长入门) → 209(最短入门) → 438/567(固定窗口)
→ 904/1004(最长变体) → 424(maxFreq 技巧) → 76(Hard 最短巅峰)
→ 239(单调队列) → 30/395/992(综合)
```

---

## 11. 常见陷阱与调试技巧

### 11.1 常见错误

| 错误 | 说明 | 修正 |
|------|------|------|
| while vs if | 最长型：while 收缩直到合法；固定型：if 收缩一次 | 区分窗口类型 |
| 更新时机错误 | 最长型在收缩后更新，最短型在收缩前更新 | 画状态图 |
| 忘记出窗清理 | 哈希计数减到 0 后没 erase | 加 `if (count == 0) erase` |
| int 溢出 | 窗口和超过 INT_MAX | 用 `long long` |
| 空窗判断 | 空串或 k > n 的边界 | 开头特判 |

### 11.2 最长 vs 最短 对比

```
最长型:                              最短型:
for right:                           for right:
  入窗                                 入窗
  while (不合法):                      while (合法):
    出窗                                 更新答案（取 min）
  更新答案（取 max）                      出窗
```

### 11.3 调试技巧

1. **打印窗口状态**：每次循环输出 `[left, right]`、窗口内容和计数
2. **手画滑窗过程**：在纸上模拟前几步
3. **检查边界**：空数组、k=0、全部相同元素

```cpp
// 调试模板
for (int right = 0; right < n; right++) {
    // ... 入窗 ...
    // ... 收缩 ...
    
    // 🔍 调试输出
    cout << "left=" << left << " right=" << right
         << " window=[" << left << "," << right << "]"
         << " len=" << right - left + 1 << endl;
}
```

---

## 12. 总结与面试要点

### 核心要素

```
滑动窗口 = 同向双指针 + 窗口状态维护 + 合适的收缩时机
```

### 面试表达模板

> "这道题求的是连续子串/子数组的最长/最短，具有单调性——
> 窗口扩大可能满足/不满足条件，因此可以用滑动窗口。
> 我用 right 指针扩展窗口，left 指针收缩窗口，
> 辅助 xxx 数据结构维护窗口状态，
> 时间 O(n)，空间 O(k)。"

### 快速判断流程

```
看到"连续子数组/子串"？
  ├─ 是 → 滑动窗口可能适用
  │    ├─ 有负数且求最短和？→ 前缀和+单调队列
  │    ├─ 求最长？→ 最长型模板
  │    ├─ 求最短？→ 最短型模板
  │    └─ 固定长度？→ 固定窗口模板
  └─ 否 → 考虑其他方法
```

### 复杂度速查

| 模型 | 时间 | 空间 |
|------|------|------|
| 可变窗口（最长/最短） | O(n) | O(k)，k = 字符集大小 |
| 固定窗口 | O(n) | O(k) |
| 单调队列辅助 | O(n) | O(k) |
| 枚举种类数（LC 395） | O(26n) ≈ O(n) | O(26) |

---

> **下一步**：配合 `knowlege_details_6_sliding_window.cpp` 运行所有 Demo，手动在纸上画出几个题的窗口滑动过程，加深理解！
