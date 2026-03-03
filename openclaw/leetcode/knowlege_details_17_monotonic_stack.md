# 专题十七（上）：单调栈 Monotonic Stack — 原理与LeetCode实战

> 作者：大胖超 😜
> 
> 单调栈是解决"下一个更大/更小元素"类问题的利器。LC 739/84/42/496/503/85 等高频题都以单调栈为核心。掌握单调栈的思想后，很多看似复杂的数组问题都能优雅求解。

---

## 目录

1. [单调栈基本概念](#1-单调栈基本概念)
2. [单调栈核心模板](#2-单调栈核心模板)
3. [四种变体详解](#3-四种变体详解)
4. [单调栈经典应用](#4-单调栈经典应用)
5. [LeetCode 经典题解](#5-leetcode-经典题解)
6. [进阶技巧](#6-进阶技巧)
7. [题目总结与易错点](#7-题目总结与易错点)

---

## 1. 单调栈基本概念

### 1.1 什么是单调栈

单调栈是一个**栈内元素保持单调递增或单调递减**的栈。每次新元素入栈前，会弹出所有破坏单调性的栈顶元素。

```
普通栈: 元素随意进出，无约束
单调栈: 入栈前"清场"，保持栈内单调性

示例 — 单调递减栈（栈底→栈顶 递减）:
  初始: []

  push 5: [5]
  push 3: [5, 3]        ← 3 < 5, 直接入
  push 4: [5, 3] → 弹3 → [5, 4]  ← 4 > 3, 弹3后入4
  push 2: [5, 4, 2]     ← 2 < 4, 直接入
  push 6: 弹2→弹4→弹5 → [6]  ← 6比所有都大, 清场

  每个元素恰好入栈出栈一次 → 总时间 O(n)
```

### 1.2 核心能力

| 问题类型 | 栈类型 | 说明 |
|----------|--------|------|
| 下一个更大元素 (NGE) | 单调递减栈 | 被弹出时，当前元素就是"下一个更大" |
| 下一个更小元素 (NSE) | 单调递增栈 | 被弹出时，当前元素就是"下一个更小" |
| 上一个更大元素 (PGE) | 单调递减栈 | 入栈时，栈顶就是"上一个更大" |
| 上一个更小元素 (PSE) | 单调递增栈 | 入栈时，栈顶就是"上一个更小" |

> **关键洞察**：一个元素**被弹出**的时刻，就是找到答案的时刻。

### 1.3 复杂度

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n) — 每个元素最多入栈出栈各一次 |
| 空间 | O(n) — 栈最多存 n 个元素 |

### 1.4 为什么存下标而不是值？

```
存下标的好处:
  1. 可以通过下标访问原始值: nums[stk.top()]
  2. 可以计算距离: i - stk.top()
  3. 可以判断窗口范围: stk.front() > i - k
  4. 既保留了值信息，又保留了位置信息

只有极少数情况（如 LC 496 不需要距离）可以只存值。
面试中统一存下标最安全。
```

---

## 2. 单调栈核心模板

### 2.1 模板一：找右边第一个更大元素（最常见）

```cpp
// 单调递减栈：栈底到栈顶递减
// 对于每个元素，找到右边第一个比它大的元素
vector<int> nextGreaterElement(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);  // -1 表示没有更大的
    stack<int> stk;             // 存下标
    
    for (int i = 0; i < n; i++) {
        // 当前元素比栈顶大 → 栈顶的"下一个更大"就是当前元素
        while (!stk.empty() && nums[i] > nums[stk.top()]) {
            result[stk.top()] = nums[i];  // 或记录下标 i
            stk.pop();
        }
        stk.push(i);
    }
    return result;
}
```

**图解过程**：

```
nums = [2, 1, 4, 3, 5]

i=0: push 0    stk=[0]         (值:[2])
i=1: push 1    stk=[0,1]       (值:[2,1])     1<2 直接入
i=2: 4>1 弹1   result[1]=4     stk=[0]
     4>2 弹0   result[0]=4     stk=[]
     push 2    stk=[2]         (值:[4])
i=3: push 3    stk=[2,3]       (值:[4,3])     3<4 直接入
i=4: 5>3 弹3   result[3]=5     stk=[2]
     5>4 弹2   result[2]=5     stk=[]
     push 4    stk=[4]         (值:[5])

result = [4, 4, 5, 5, -1]
         下一个更大: 2→4, 1→4, 4→5, 3→5, 5→无
```

### 2.2 模板二：找右边第一个更小元素

```cpp
// 单调递增栈：栈底到栈顶递增
vector<int> nextSmallerElement(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;
    
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && nums[i] < nums[stk.top()]) {
            result[stk.top()] = nums[i];
            stk.pop();
        }
        stk.push(i);
    }
    return result;
}
```

### 2.3 模板三：找左边第一个更大/更小元素

```cpp
// 找左边第一个更大元素 → 入栈时栈顶就是答案
vector<int> prevGreaterElement(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;  // 单调递减栈
    
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && nums[stk.top()] <= nums[i])
            stk.pop();
        if (!stk.empty())
            result[i] = nums[stk.top()];  // 栈顶就是左边第一个更大
        stk.push(i);
    }
    return result;
}

// 找左边第一个更小元素
vector<int> prevSmallerElement(vector<int>& nums) {
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
```

---

## 3. 四种变体详解

### 3.1 总结对照表

```
            找更大        找更小
   右边    单调递减栈     单调递增栈      ← 弹出时得到答案
   左边    单调递减栈     单调递增栈      ← 入栈时栈顶是答案

判断栈类型助记:
  找"更大" → 栈递减 (大的留着, 小的被弹)
  找"更小" → 栈递增 (小的留着, 大的被弹)
```

### 3.2 严格 vs 非严格

```
严格大于 (>):
  while (!stk.empty() && nums[i] > nums[stk.top()])
  相等元素不弹出

大于等于 (>=):
  while (!stk.empty() && nums[i] >= nums[stk.top()])
  相等元素也弹出

选择取决于题意:
  "下一个更大" → 用 >
  "下一个更大或等于" → 用 >=
  LC 84/85 中因为要计算宽度，等号的选择影响结果正确性
```

### 3.3 从左到右 vs 从右到左

```
两种遍历方向都可以解决同一问题:

从左到右 (正向): 弹出时确定答案
  → 自然流畅, 面试主推

从右到左 (反向): 入栈时确定答案
  → 某些场景更直观, 如需要同时知道左右答案

// 从右到左找"右边第一个更大"
vector<int> nge_reverse(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;  // 单调递减栈
    
    for (int i = n-1; i >= 0; i--) {
        while (!stk.empty() && nums[stk.top()] <= nums[i])
            stk.pop();
        if (!stk.empty())
            result[i] = nums[stk.top()];
        stk.push(i);
    }
    return result;
}
```

---

## 4. 单调栈经典应用

### 4.1 柱状图最大矩形（LC 84）

```
heights = [2,1,5,6,2,3]

对每根柱子，找到它能扩展的最大宽度:
  左边界: 左边第一个比它矮的柱子
  右边界: 右边第一个比它矮的柱子
  面积 = height × (right - left - 1)

           ___
       ___|   |
      |   |   |   ___
  ___ |   |   |  |   |
 |   ||   |   |  |   |
 |___|_|___|___|__|___|

  h: 2  1  5  6  2  3

  对 h=6: 左边界=5(idx=2), 右边界=2(idx=4)
  宽度 = 4-2-1 = 1, 面积 = 6×1 = 6

  对 h=5: 左边界=1(idx=1), 右边界=2(idx=4)
  宽度 = 4-1-1 = 2, 面积 = 5×2 = 10 ← 最大

单调栈一次遍历同时获得左右边界!
```

```cpp
int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    stack<int> stk;
    int maxArea = 0;
    heights.push_back(0);  // 哨兵: 确保最后全部弹出
    
    for (int i = 0; i <= n; i++) {
        while (!stk.empty() && heights[i] < heights[stk.top()]) {
            int h = heights[stk.top()]; stk.pop();
            // 宽度 = 左边界(栈顶)到右边界(i)
            int w = stk.empty() ? i : i - stk.top() - 1;
            maxArea = max(maxArea, h * w);
        }
        stk.push(i);
    }
    heights.pop_back();  // 恢复
    return maxArea;
}
```

> **哨兵技巧**：末尾加一个高度为0的柱子，确保栈中所有元素都能被弹出处理。

### 4.2 接雨水（LC 42）

```
height = [0,1,0,2,1,0,1,3,2,1,2,1]

三种解法: 双指针 / DP预处理 / 单调栈

单调栈思路（按行计算）:
  维护递减栈, 遇到更高柱子时:
  "凹槽"被填平 → 雨水量 = 宽×高

       |
   |###||#|
   |#|||||#|  
  _|_||||||_|_
  0 1 0 2 1 0 1 3 2 1 2 1
```

```cpp
int trap(vector<int>& height) {
    stack<int> stk;
    int water = 0;
    
    for (int i = 0; i < (int)height.size(); i++) {
        while (!stk.empty() && height[i] > height[stk.top()]) {
            int bottom = stk.top(); stk.pop();
            if (stk.empty()) break;
            int left = stk.top();
            int w = i - left - 1;
            int h = min(height[left], height[i]) - height[bottom];
            water += w * h;
        }
        stk.push(i);
    }
    return water;
}
```

### 4.3 最大矩形（LC 85）

```
将二维01矩阵的每一行看作柱状图的底部，
每列的连续1的高度就是柱子高度，
然后对每行调用 LC 84 的单调栈算法。

matrix:
  1 0 1 0 0
  1 0 1 1 1
  1 1 1 1 1
  1 0 0 1 0

对应高度:
  1 0 1 0 0   ← 第0行直接取
  2 0 2 1 1   ← 第1行 = 上一行+1（如果当前=1）
  3 1 3 2 2   ← 第2行
  4 0 0 3 0   ← 第3行（如果当前=0则高度归0）

每行用 LC 84 求最大矩形, 取所有行的最大值。
```

```cpp
int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty()) return 0;
    int m = matrix.size(), n = matrix[0].size();
    vector<int> heights(n, 0);
    int maxArea = 0;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            heights[j] = (matrix[i][j] == '1') ? heights[j] + 1 : 0;
        }
        maxArea = max(maxArea, largestRectangleArea(heights));
    }
    return maxArea;
}
```

### 4.4 股票价格跨度（LC 901）

```
每天调用 next(price), 返回包含今天在内的
连续多少天股价 ≤ 今天的价格。

核心: 找左边第一个比当前价格大的那天 → 跨度 = 天数差

单调递减栈: 存 {价格, 下标}
```

---

## 5. LeetCode 经典题解

### 5.1 LC 739: 每日温度

```
给定每天的温度, 对于每天,
求之后第一个更高温度的天数差。

temperatures = [73,74,75,71,69,72,76,73]
输出: [1,1,4,2,1,1,0,0]

73→74(差1), 74→75(差1), 75→76(差4), ...
```

```cpp
vector<int> dailyTemperatures(vector<int>& temperatures) {
    int n = temperatures.size();
    vector<int> result(n, 0);
    stack<int> stk;  // 单调递减栈
    
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && temperatures[i] > temperatures[stk.top()]) {
            int prev = stk.top(); stk.pop();
            result[prev] = i - prev;
        }
        stk.push(i);
    }
    return result;
}
```

### 5.2 LC 496/503: 下一个更大元素 I / II

```
LC 496: 在 nums2 中找 nums1 元素的下一个更大元素
LC 503: 循环数组中下一个更大元素

循环数组技巧: 遍历 2n 次, 下标取模 i % n
```

```cpp
// LC 496
vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    unordered_map<int, int> nge;
    stack<int> stk;
    
    for (int x : nums2) {
        while (!stk.empty() && x > stk.top()) {
            nge[stk.top()] = x;
            stk.pop();
        }
        stk.push(x);  // 这里存值而非下标
    }
    
    vector<int> result;
    for (int x : nums1)
        result.push_back(nge.count(x) ? nge[x] : -1);
    return result;
}

// LC 503: 循环数组
vector<int> nextGreaterElements(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;
    
    // 遍历两遍模拟循环
    for (int i = 0; i < 2 * n; i++) {
        int idx = i % n;
        while (!stk.empty() && nums[idx] > nums[stk.top()]) {
            result[stk.top()] = nums[idx];
            stk.pop();
        }
        if (i < n) stk.push(i);  // 只在第一遍push
    }
    return result;
}
```

### 5.3 LC 402: 移掉 K 位数字

```
从数字字符串中删掉 k 位, 使结果最小。

"1432219", k=3 → "1219"

贪心 + 单调递增栈:
  遇到比栈顶小的数字 → 弹栈顶(相当于删除)
  最多弹 k 次
```

```cpp
string removeKdigits(string num, int k) {
    string stk;  // 用 string 当栈
    
    for (char c : num) {
        while (k > 0 && !stk.empty() && stk.back() > c) {
            stk.pop_back();
            k--;
        }
        stk.push_back(c);
    }
    
    // 如果还有剩余要删的, 从末尾删
    while (k > 0) { stk.pop_back(); k--; }
    
    // 去除前导零
    int start = 0;
    while (start < (int)stk.size() && stk[start] == '0') start++;
    string result = stk.substr(start);
    return result.empty() ? "0" : result;
}
```

### 5.4 LC 316/1081: 去除重复字母 / 最小字典序

```
去除字符串中的重复字母，使每个字母只出现一次，
且结果的字典序最小。

"bcabc" → "abc"
"cbacdcbc" → "acdb"

单调递增栈 + 贪心:
  如果栈顶字符 > 当前字符, 且栈顶字符后面还会出现, 则弹出
```

```cpp
string removeDuplicateLetters(string s) {
    int lastIdx[26] = {};
    bool inStack[26] = {};
    
    // 记录每个字符最后出现的位置
    for (int i = 0; i < (int)s.size(); i++)
        lastIdx[s[i] - 'a'] = i;
    
    string stk;
    for (int i = 0; i < (int)s.size(); i++) {
        int c = s[i] - 'a';
        if (inStack[c]) continue;  // 已在栈中, 跳过
        
        // 栈顶比当前大, 且以后还有机会 → 弹
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
```

### 5.5 LC 907: 子数组的最小值之和

```
数组每个子数组的最小值之和。

arr = [3, 1, 2, 4]
子数组: [3]=3, [1]=1, [2]=2, [4]=4,
       [3,1]=1, [1,2]=1, [2,4]=2,
       [3,1,2]=1, [1,2,4]=1,
       [3,1,2,4]=1
总和 = 3+1+2+4+1+1+2+1+1+1 = 17

贡献法: 对每个 arr[i], 计算它作为最小值的子数组个数。
  左边界: 左边第一个比它小的位置 left[i]
  右边界: 右边第一个比它小的(或等于)的位置 right[i]
  贡献 = arr[i] × (i - left[i]) × (right[i] - i)
```

```cpp
int sumSubarrayMins(vector<int>& arr) {
    int n = arr.size();
    const int MOD = 1e9 + 7;
    vector<int> left(n), right(n);
    stack<int> stk;
    
    // 左边界: 左边第一个严格小于的位置
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && arr[stk.top()] >= arr[i])
            stk.pop();
        left[i] = stk.empty() ? -1 : stk.top();
        stk.push(i);
    }
    
    while (!stk.empty()) stk.pop();
    
    // 右边界: 右边第一个严格小于的位置（注意: 用<不用<=, 处理重复）
    for (int i = n-1; i >= 0; i--) {
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
```

> **重复元素处理**：左边用 >=、右边用 >, 或反过来, 但两边必须一个严格一个非严格, 避免重复计数!

---

## 6. 进阶技巧

### 6.1 哨兵技巧

```
在数组头部和/或尾部添加特殊值（如0或INT_MIN）,
避免处理栈为空的边界情况。

LC 84: heights 末尾加 0 → 确保所有柱子都会被弹出
LC 42: 可以首尾加 0 → 统一左右边界处理

优点: 代码更简洁, 减少边界判断
缺点: 需要记得恢复或补偿
```

### 6.2 循环数组处理

```
遍历两遍, 下标取模:

for (int i = 0; i < 2 * n; i++) {
    int idx = i % n;
    ...
}

适用: LC 503 循环数组的下一个更大元素
```

### 6.3 贡献法

```
核心思想: 不逐个枚举子数组, 而是计算每个元素的"贡献"。

对于 arr[i]:
  可以作为最小值的子数组范围 = (left, right)
  子数组个数 = (i - left) × (right - i)
  贡献 = arr[i] × 子数组个数

适用: LC 907(最小值之和), LC 2104(子数组范围和)
```

### 6.4 单调栈 + 前缀和

```
当需要区间和 + 单调栈配合时, 预处理前缀和。

例如: 找到以每个位置为最小值的子数组中,
     元素和最大的子数组。

→ 用单调栈确定左右边界, 用前缀和快速计算区间和。
```

---

## 7. 题目总结与易错点

### 7.1 题目分类

#### 基础模板
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 739 | 每日温度 | 中等 | 下一个更大, 距离 |
| 496 | 下一个更大元素 I | 简单 | 哈希+单调栈 |
| 503 | 下一个更大元素 II | 中等 | 循环数组 |
| 901 | 股票价格跨度 | 中等 | 左边第一个更大 |

#### 矩形面积
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 84 | 柱状图最大矩形 | 困难 | 经典单调栈+哨兵 |
| 85 | 最大矩形 | 困难 | 逐行→LC84 |
| 42 | 接雨水 | 困难 | 凹槽填充/多解 |

#### 贪心+单调栈
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 402 | 移掉K位数字 | 中等 | 单调递增栈+贪心 |
| 316 | 去除重复字母 | 中等 | 单调栈+贪心+计数 |
| 321 | 拼接最大数 | 困难 | 单调栈+归并 |

#### 贡献法
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 907 | 子数组最小值之和 | 中等 | 贡献法+重复处理 |
| 2104 | 子数组范围和 | 中等 | 最大最小贡献差 |
| 1856 | 子数组最小乘积最大值 | 中等 | 贡献法+前缀和 |

### 7.2 高频易错点

```
❌ 1. 栈存值而非下标
   大多数题需要距离/位置信息 → 必须存下标
   ✅ stack<int> stk; // 存下标, 用 nums[stk.top()] 取值

❌ 2. 忘记处理栈中剩余元素
   遍历结束后栈中可能还有元素（没找到更大/更小的）
   ✅ 这些元素的答案是默认值(-1 或 0 或 n)
   ✅ 或者使用哨兵确保全部弹出

❌ 3. 严格/非严格搞混
   "下一个更大" 用 >, "下一个 ≥" 用 >=
   LC 907 中一边严格一边非严格否则重复计数
   ✅ 仔细审题, 重复元素时画图验证

❌ 4. LC 84 宽度计算错误
   弹出后宽度 = stk.empty() ? i : i - stk.top() - 1
   而不是 i - stk.top() 或 i - top - 1
   ✅ 栈空意味着左边没有更矮的 → 宽度就是 i

❌ 5. LC 42 接雨水单调栈漏了 stk.empty() 检查
   弹出底部后, 如果栈空说明左边没有墙, 不能积水
   ✅ if (stk.empty()) break;

❌ 6. 循环数组第二遍还在 push
   for (int i = 0; i < 2*n; i++)
   只在 i < n 时 push, 否则重复入栈
   ✅ if (i < n) stk.push(i);

❌ 7. LC 402 忘记处理前导零
   "10200", k=1 → "0200" → 应该输出 "200"
   ✅ 去除前导零, 空串返回 "0"
```

### 7.3 面试高频问答

> **Q: 单调栈为什么是 O(n)？看起来有 while 循环啊？**  
> A: 每个元素最多入栈一次、出栈一次，总操作 ≤ 2n，所以整体 O(n)。这叫"均摊分析"。

> **Q: 什么时候用递减栈，什么时候用递增栈？**  
> A: 找"更大" → 递减栈（小的被大的弹出）。找"更小" → 递增栈（大的被小的弹出）。

> **Q: 单调栈和双指针有什么区别？**  
> A: 双指针适合有序数组或配对问题。单调栈适合在无序数组中找"最近的更大/更小"。LC 42 接雨水两者都能解。

---

> 📌 **配套代码**：[knowlege_details_17_monotonic_stack.cpp](knowlege_details_17_monotonic_stack.cpp) 包含上述所有算法的可运行演示。
