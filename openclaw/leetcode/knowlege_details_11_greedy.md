# 专题十一：贪心算法（Greedy Algorithm）

> 作者：大胖超 😜
> 配合 `knowlege_details_11_greedy.cpp` 食用更佳

---

## 目录

1. [贪心算法核心思想](#1-贪心算法核心思想)
2. [贪心 vs 动态规划 vs 回溯](#2-贪心-vs-动态规划-vs-回溯)
3. [贪心证明方法](#3-贪心证明方法)
4. [区间问题（核心题型）](#4-区间问题核心题型)
5. [跳跃问题](#5-跳跃问题)
6. [分配问题](#6-分配问题)
7. [序列贪心](#7-序列贪心)
8. [股票买卖贪心](#8-股票买卖贪心)
9. [字符串贪心](#9-字符串贪心)
10. [加油站与环形问题](#10-加油站与环形问题)
11. [数学贪心](#11-数学贪心)
12. [综合刷题清单与推荐顺序](#12-综合刷题清单与推荐顺序)
13. [易错点与总结](#13-易错点与总结)

---

## 1. 贪心算法核心思想

### 什么是贪心

**贪心 = 每一步都做"当前看起来最好"的选择，且不回头。**

局部最优 → 全局最优

```
在每个决策点，做出当前看来最优的选择，
然后把剩下的问题当作一个新的子问题继续求解。
整个过程中，不会回溯、不会重新审视已做的决策。
```

### 贪心算法的一般步骤

```
1. 建立问题的数学模型
2. 把问题分解成若干个子问题
3. 对每个子问题，做出贪心选择（局部最优）
4. 合并所有贪心选择，得到全局最优解
```

### 贪心适用条件

- **贪心选择性质**：局部最优能推出全局最优
- **最优子结构**：子问题的最优解包含在原问题的最优解中

> **核心难点不在于编码，而在于"想到贪心策略"和"证明它的正确性"**

---

## 2. 贪心 vs 动态规划 vs 回溯

| 对比项 | 贪心 | 动态规划 | 回溯 |
|--------|------|---------|------|
| 决策方式 | 每步选当前最优 | 穷举所有子问题 | 递归枚举所有可能 |
| 回溯吗？ | ❌ 不回头 | ❌ 但记录所有状态 | ✅ 回溯剪枝 |
| 时间复杂度 | 通常 O(n log n) 或 O(n) | O(n²) 或更高 | 指数级 |
| 正确性保证 | 需要证明 | 一定正确 | 一定正确（穷举） |
| 代码复杂度 | 通常最简单 | 中等 | 较复杂 |

### 如何判断用贪心还是 DP

```
问自己：
1. 能否通过排序 + 一次扫描解决？ → 贪心
2. 当前选择是否影响后续选择的约束？
   - 不影响（或影响可预测） → 贪心
   - 影响且结果不确定 → DP
3. 能不能举出反例推翻贪心策略？ → 能 → DP
```

**示例：**
- 零钱兑换（面值 1, 5, 10, 25）→ 贪心可行（先用大面值）
- 零钱兑换（面值 1, 3, 4, amount=6）→ 贪心失败（4+1+1=3枚 ≠ 最优 3+3=2枚）→ 用 DP

---

## 3. 贪心证明方法

### 方法一：交换论证（Exchange Argument）

```
思路：假设存在一个最优解 OPT，如果 OPT 和贪心选择不同，
     证明可以将 OPT 中的某些选择"交换"成贪心选择，
     且结果不变或更优。
```

**例子：活动选择问题（LC 435 无重叠区间）**

```
排序方式：按结束时间排序
证明：假设最优解选了活动 A（非最早结束），
     用最早结束的 B 替换 A：
     - B 结束更早，给后续活动留更多空间
     - 所以替换后解不会变差
     → 贪心选择不会错过最优
```

### 方法二：归纳法

```
1. 基础情况：n=1 时贪心显然正确
2. 归纳假设：对 n=k 时贪心正确
3. 归纳步骤：证明 n=k+1 时，做出贪心选择后，
            剩下 k 个子问题仍满足归纳假设
```

### 方法三：反证法

```
假设贪心解不是最优的，
则存在更优解 → 推导出矛盾 → 贪心解就是最优的
```

> **刷题时不需要严格证明，但需要能直觉判断 + 找不到反例**

---

## 4. 区间问题（核心题型）

区间贪心是 LeetCode 中贪心算法**最高频**的题型。

### 通用套路

```
1. 排序（按左端点 or 右端点）
2. 扫描区间，贪心决策
```

### LC 56: 合并区间

**策略**：按左端点排序，依次合并重叠区间

```cpp
vector<vector<int>> merge(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end());
    vector<vector<int>> res;
    for (auto& iv : intervals) {
        if (res.empty() || res.back()[1] < iv[0])
            res.push_back(iv);
        else
            res.back()[1] = max(res.back()[1], iv[1]);
    }
    return res;
}
```

**图解**：
```
输入:  [1,3] [2,6] [8,10] [15,18]
排序后: [1,3] [2,6] [8,10] [15,18]

  1---3
    2------6
              8--10
                        15--18
合并:
  1------6    8--10     15--18

输出: [1,6] [8,10] [15,18]
```

### LC 435: 无重叠区间（最少删几个）

**策略**：按右端点排序，贪心保留不重叠的区间（选"结束最早"的）

```cpp
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end(),
         [](auto& a, auto& b) { return a[1] < b[1]; });
    int count = 0, prevEnd = INT_MIN;
    for (auto& iv : intervals) {
        if (iv[0] >= prevEnd)
            prevEnd = iv[1];    // 不重叠，保留
        else
            count++;            // 重叠，删除
    }
    return count;
}
```

**为什么按右端点排？**
```
选结束最早的活动 → 给后续活动留更多空间 → 能安排更多活动
这就是经典的"活动选择问题"（Activity Selection Problem）
```

### LC 452: 用最少数量的箭引爆气球

**策略**：按右端点排序，一箭射到尽可能多的重叠气球

```cpp
int findMinArrowShots(vector<vector<int>>& points) {
    sort(points.begin(), points.end(),
         [](auto& a, auto& b) { return a[1] < b[1]; });
    int arrows = 1;
    int arrowPos = points[0][1];
    for (int i = 1; i < (int)points.size(); i++) {
        if (points[i][0] > arrowPos) {
            arrows++;
            arrowPos = points[i][1];
        }
    }
    return arrows;
}
```

### LC 763: 划分字母区间

**策略**：记录每个字母最后出现的位置，动态扩展当前片段

```cpp
vector<int> partitionLabels(string s) {
    vector<int> last(26, 0);
    for (int i = 0; i < (int)s.size(); i++)
        last[s[i] - 'a'] = i;           // 每个字符最后出现位置
    
    vector<int> result;
    int start = 0, end = 0;
    for (int i = 0; i < (int)s.size(); i++) {
        end = max(end, last[s[i] - 'a']); // 扩展当前片段
        if (i == end) {                    // 到达片段边界
            result.push_back(end - start + 1);
            start = end + 1;
        }
    }
    return result;
}
```

### 区间问题模板总结

| 题目 | 排序方式 | 贪心策略 |
|------|---------|---------|
| LC 56 合并区间 | 按左端点 | 合并重叠 |
| LC 435 无重叠区间 | 按右端点 | 保留结束早的 |
| LC 452 射气球 | 按右端点 | 在一箭范围内尽量多射 |
| LC 763 划分字母区间 | 不需排序 | 动态扩展段边界 |
| LC 57 插入区间 | 已排好序 | 找重叠范围合并 |

---

## 5. 跳跃问题

### LC 55: 跳跃游戏

**策略**：维护能到达的最远位置，一旦当前位置超过 maxReach 则失败

```cpp
bool canJump(vector<int>& nums) {
    int maxReach = 0;
    for (int i = 0; i < (int)nums.size(); i++) {
        if (i > maxReach) return false;
        maxReach = max(maxReach, i + nums[i]);
    }
    return true;
}
```

**图解**：
```
nums = [2,3,1,1,4]

i=0: maxReach = max(0, 0+2) = 2     能到位置2
i=1: maxReach = max(2, 1+3) = 4     能到位置4（终点!）
i=2: maxReach = max(4, 2+1) = 4
i=3: maxReach = max(4, 3+1) = 4
i=4: 到达终点 ✓

nums = [3,2,1,0,4]
i=0: maxReach = 3
i=1: maxReach = 3
i=2: maxReach = 3
i=3: maxReach = 3    (位置3的值是0，无法前进)
i=4: i=4 > maxReach=3 → false ✗
```

### LC 45: 跳跃游戏 II（最少跳跃次数）

**策略**：在当前跳跃范围内，找能跳最远的位置作为下一跳

```cpp
int jump(vector<int>& nums) {
    int jumps = 0, curEnd = 0, farthest = 0;
    for (int i = 0; i < (int)nums.size() - 1; i++) {
        farthest = max(farthest, i + nums[i]);
        if (i == curEnd) {      // 到当前跳跃的边界
            jumps++;
            curEnd = farthest;  // 更新为最远位置
        }
    }
    return jumps;
}
```

**图解**：
```
nums = [2,3,1,1,4]

第 0 步: curEnd=0, farthest=0
  i=0: farthest = max(0, 2) = 2
       i==curEnd → jumps=1, curEnd=2    ← 第一跳：可到[1,2]

第 1 跳范围内:
  i=1: farthest = max(2, 4) = 4
  i=2: farthest = max(4, 3) = 4
       i==curEnd → jumps=2, curEnd=4    ← 第二跳：可到[3,4]

到达终点，最少 2 跳
```

---

## 6. 分配问题

### LC 455: 分发饼干

**策略**：排序后，用最小能满足的饼干喂最小胃口的孩子

```cpp
int findContentChildren(vector<int>& g, vector<int>& s) {
    sort(g.begin(), g.end());
    sort(s.begin(), s.end());
    int child = 0, cookie = 0;
    while (child < (int)g.size() && cookie < (int)s.size()) {
        if (s[cookie] >= g[child])
            child++;         // 满足了这个孩子
        cookie++;            // 这块饼干用掉（或太小跳过）
    }
    return child;
}
```

### LC 135: 分发糖果

**策略**：两次遍历——先从左到右保证右边比左边大时多给糖；再从右到左保证左边比右边大时多给糖

```cpp
int candy(vector<int>& ratings) {
    int n = ratings.size();
    vector<int> candies(n, 1);
    
    // 左→右：右边评分高，比左邻多给 1
    for (int i = 1; i < n; i++)
        if (ratings[i] > ratings[i - 1])
            candies[i] = candies[i - 1] + 1;
    
    // 右→左：左边评分高，取已有值和右邻+1的最大值
    for (int i = n - 2; i >= 0; i--)
        if (ratings[i] > ratings[i + 1])
            candies[i] = max(candies[i], candies[i + 1] + 1);
    
    int total = 0;
    for (int c : candies) total += c;
    return total;
}
```

**图解**：
```
ratings = [1, 0, 2]

初始:       [1, 1, 1]
左→右:      [1, 1, 2]  (ratings[2]>ratings[1] → 加1)
右→左:      [2, 1, 2]  (ratings[0]>ratings[1] → 取max(1,1+1)=2)
总计: 5

ratings = [1, 2, 2]

初始:       [1, 1, 1]
左→右:      [1, 2, 1]  (ratings[1]>ratings[0], ratings[2]==ratings[1]不加)
右→左:      [1, 2, 1]  (无变化)
总计: 4
```

---

## 7. 序列贪心

### LC 376: 摆动序列

**策略**：统计波峰波谷的个数。遇到方向改变时计数+1

```cpp
int wiggleMaxLength(vector<int>& nums) {
    int n = nums.size();
    if (n < 2) return n;
    
    int up = 1, down = 1;
    for (int i = 1; i < n; i++) {
        if (nums[i] > nums[i - 1])
            up = down + 1;      // 上升 = 上一个下降 + 1
        else if (nums[i] < nums[i - 1])
            down = up + 1;      // 下降 = 上一个上升 + 1
    }
    return max(up, down);
}
```

### LC 738: 单调递增的数字

**策略**：从后向前扫描，一旦 s[i] > s[i+1]，将 s[i] 减 1，后面全变 9

```cpp
int monotoneIncreasingDigits(int n) {
    string s = to_string(n);
    int mark = s.size();  // 从 mark 起全变 9
    for (int i = s.size() - 1; i > 0; i--) {
        if (s[i] < s[i - 1]) {
            s[i - 1]--;
            mark = i;
        }
    }
    for (int i = mark; i < (int)s.size(); i++)
        s[i] = '9';
    return stoi(s);
}
```

**图解**：
```
n = 332

从后向前:
  i=2: s[2]='2' < s[1]='3' → s[1]='2', mark=2 → "322" → mark=2
  i=1: s[1]='2' < s[0]='3' → s[0]='2', mark=1 → "222" → mark=1

mark=1 起变9: "299"
输出: 299

验证: 299 ≤ 332 且每位单调递增 ✓
```

### LC 406: 根据身高重建队列

**策略**：按身高降序排列（相同身高按 k 升序），然后按 k 值插入位置

```cpp
vector<vector<int>> reconstructQueue(vector<vector<int>>& people) {
    // 身高降序，k 升序
    sort(people.begin(), people.end(),
         [](auto& a, auto& b) {
             return a[0] > b[0] || (a[0] == b[0] && a[1] < b[1]);
         });
    
    vector<vector<int>> result;
    for (auto& p : people)
        result.insert(result.begin() + p[1], p);
    return result;
}
```

**图解**：
```
输入: [[7,0],[4,4],[7,1],[5,0],[6,1],[5,2]]
排序: [[7,0],[7,1],[6,1],[5,0],[5,2],[4,4]]

按 k 值插入:
  [7,0] → [[7,0]]
  [7,1] → [[7,0],[7,1]]
  [6,1] → [[7,0],[6,1],[7,1]]
  [5,0] → [[5,0],[7,0],[6,1],[7,1]]
  [5,2] → [[5,0],[7,0],[5,2],[6,1],[7,1]]
  [4,4] → [[5,0],[7,0],[5,2],[6,1],[4,4],[7,1]]
```

> 为什么先插高的？因为矮的人插入不会影响高的人的 k 值

---

## 8. 股票买卖贪心

### LC 122: 买卖股票的最佳时机 II（不限次数）

**贪心**：只要明天比今天贵，就今天买明天卖（收集所有上涨差价）

```cpp
int maxProfit(vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < (int)prices.size(); i++) {
        if (prices[i] > prices[i - 1])
            profit += prices[i] - prices[i - 1];
    }
    return profit;
}
```

**图解**：
```
prices = [7, 1, 5, 3, 6, 4]

差值:     -6  +4  -2  +3  -2
收集正差:     +4      +3      = 7

等价于: 第2天买(1)第3天卖(5)，第4天买(3)第6天卖(6)
```

> 这个贪心等效于 DP 状态机解法，但代码简洁得多

---

## 9. 字符串贪心

### LC 316 / LC 1081: 去除重复字母（保持字典序最小）

**策略**：单调栈 + 贪心。栈中字符保持递增，如果当前字符更小且栈顶字符后面还有，就弹出栈顶

```cpp
string removeDuplicateLetters(string s) {
    vector<int> count(26, 0);
    vector<bool> inStack(26, false);
    for (char c : s) count[c - 'a']++;
    
    string stack;
    for (char c : s) {
        count[c - 'a']--;
        if (inStack[c - 'a']) continue;
        
        // 栈顶比当前大 且 后面还有栈顶字符 → 弹出
        while (!stack.empty() && stack.back() > c 
               && count[stack.back() - 'a'] > 0) {
            inStack[stack.back() - 'a'] = false;
            stack.pop_back();
        }
        stack.push_back(c);
        inStack[c - 'a'] = true;
    }
    return stack;
}
```

### LC 402: 移掉 K 位数字

**策略**：单调栈贪心，维护递增栈，遇到更小的数字就弹出前面较大的

```cpp
string removeKdigits(string num, int k) {
    string stack;
    for (char c : num) {
        while (k > 0 && !stack.empty() && stack.back() > c) {
            stack.pop_back();
            k--;
        }
        stack.push_back(c);
    }
    // 如果还有 k 没用完，从末尾删
    while (k > 0) { stack.pop_back(); k--; }
    
    // 去前导零
    int start = 0;
    while (start < (int)stack.size() && stack[start] == '0') start++;
    string result = stack.substr(start);
    return result.empty() ? "0" : result;
}
```

---

## 10. 加油站与环形问题

### LC 134: 加油站

**策略 1（暴力优化）**：如果总油量 ≥ 总消耗，一定有解。从油量累积最低点的**下一站**出发。

**策略 2（贪心扫描）**：

```cpp
int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
    int totalTank = 0, curTank = 0, start = 0;
    for (int i = 0; i < (int)gas.size(); i++) {
        int diff = gas[i] - cost[i];
        totalTank += diff;
        curTank += diff;
        if (curTank < 0) {     // 从 start 到 i 走不通
            start = i + 1;     // 从 i+1 重新开始
            curTank = 0;
        }
    }
    return totalTank >= 0 ? start : -1;
}
```

**核心直觉**：
```
如果从 A 出发到不了 B，那么 A 和 B 之间的任何站都到不了 B。
因为从 A 出发在到达中间站 C 时，油量 ≥ 0（能到 C），
即从 C 出发时油量比从 A 出发时更少，更到不了 B。
所以直接跳到 B+1 继续尝试。
```

---

## 11. 数学贪心

### LC 53: 最大子数组和（Kadane 算法也是贪心）

```cpp
int maxSubArray(vector<int>& nums) {
    int maxSum = nums[0], curSum = nums[0];
    for (int i = 1; i < (int)nums.size(); i++) {
        curSum = max(nums[i], curSum + nums[i]);  // 要不要接上前面
        maxSum = max(maxSum, curSum);
    }
    return maxSum;
}
```

> 贪心选择：如果前面的累加和是负数，就丢弃，从当前重新开始

### LC 179: 最大数

**策略**：自定义排序——比较 a+b 和 b+a 拼接后谁更大

```cpp
string largestNumber(vector<int>& nums) {
    vector<string> strs;
    for (int n : nums) strs.push_back(to_string(n));
    sort(strs.begin(), strs.end(),
         [](string& a, string& b) { return a + b > b + a; });
    if (strs[0] == "0") return "0";
    string result;
    for (auto& s : strs) result += s;
    return result;
}
```

### LC 621: 任务调度器

**策略**：最高频任务优先安排，冷却时间用其他任务或空闲填充

```cpp
int leastInterval(vector<char>& tasks, int n) {
    vector<int> freq(26, 0);
    for (char c : tasks) freq[c - 'A']++;
    int maxFreq = *max_element(freq.begin(), freq.end());
    int maxCount = count(freq.begin(), freq.end(), maxFreq);
    
    // (最高频-1) 组 × 每组(n+1) + 末尾的最高频任务数
    int result = (maxFreq - 1) * (n + 1) + maxCount;
    return max(result, (int)tasks.size());
}
```

**图解**：
```
tasks = [A,A,A,B,B,B], n=2

maxFreq=3 (A和B各出现3次), maxCount=2

安排:  A B _ | A B _ | A B
      每组 n+1=3 个槽
      (3-1)*3 + 2 = 8

但如果任务足够多填满空闲:
  result = max(8, task总数)
```

---

## 12. 综合刷题清单与推荐顺序

### 按题型分组

**区间问题（最核心）：**
| 序号 | 题目 | 难度 | 核心策略 |
|------|------|------|---------|
| 1 | LC 56: 合并区间 | Medium | 按左端排序合并 |
| 2 | LC 435: 无重叠区间 | Medium | 按右端排序，保留不重叠 |
| 3 | LC 452: 射气球 | Medium | 按右端排序，贪心射 |
| 4 | LC 763: 划分字母区间 | Medium | 记录最后位置，扩展边界 |
| 5 | LC 57: 插入区间 | Medium | 分三段处理 |

**跳跃 & 覆盖：**
| 序号 | 题目 | 难度 | 核心策略 |
|------|------|------|---------|
| 6 | LC 55: 跳跃游戏 | Medium | 维护最远可达 |
| 7 | LC 45: 跳跃游戏 II | Medium | 边界触发跳跃 |

**分配问题：**
| 序号 | 题目 | 难度 | 核心策略 |
|------|------|------|---------|
| 8 | LC 455: 分发饼干 | Easy | 排序双指针 |
| 9 | LC 135: 分发糖果 | Hard | 两次遍历 |
| 10 | LC 406: 身高重建 | Medium | 降序排 + 按k插入 |

**序列贪心：**
| 序号 | 题目 | 难度 | 核心策略 |
|------|------|------|---------|
| 11 | LC 376: 摆动序列 | Medium | 统计波峰波谷 |
| 12 | LC 738: 单调递增数字 | Medium | 从后向前标记 |
| 13 | LC 122: 股票II | Easy | 收集正差 |
| 14 | LC 53: 最大子数组和 | Medium | Kadane 贪心 |

**字符串 & 数学贪心：**
| 序号 | 题目 | 难度 | 核心策略 |
|------|------|------|---------|
| 15 | LC 316: 去重复字母 | Medium | 单调栈贪心 |
| 16 | LC 402: 移K位数字 | Medium | 单调栈 |
| 17 | LC 134: 加油站 | Medium | 跟踪累积亏损 |
| 18 | LC 179: 最大数 | Medium | 自定义排序 |
| 19 | LC 621: 任务调度器 | Medium | 高频优先 |

### 推荐刷题顺序

```
入门理解:
  分发饼干(455) → 最大子数组和(53) → 买股票II(122)

区间系列:
  合并区间(56) → 无重叠区间(435) → 射气球(452) → 划分字母区间(763)

跳跃系列:
  跳跃游戏(55) → 跳跃游戏II(45)

序列 & 分配:
  摆动序列(376) → 单调递增数字(738) → 分发糖果(135) → 身高重建(406)

字符串 & 进阶:
  去重复字母(316) → 移K位数字(402) → 加油站(134) → 最大数(179) → 任务调度(621)
```

---

## 13. 易错点与总结

### 常见易错

| 错误 | 正确做法 |
|------|---------|
| 区间问题忘记排序 | 排序是区间贪心的**前提** |
| 按左端点排 vs 右端点排搞混 | 合并→左端点; 不重叠→右端点 |
| 跳跃游戏遍历到最后一个元素 | LC 45 只遍历到 n-2 |
| 分发糖果只遍历一遍 | 必须两遍（左→右 + 右→左） |
| 以为所有题都能贪心 | 多试反例，不行就用 DP |

### 贪心算法思维清单

```
✅ 排序是不是能化简问题？
✅ 能不能一次扫描得到答案？
✅ "当前最优"是什么？怎么定义？
✅ 能不能找到反例推翻策略？
✅ 问题有没有贪心选择性质 + 最优子结构？
```

### 贪心常见模式速查

```
模式1: 排序后贪心         → 区间问题, 分配问题
模式2: 维护最值/边界      → 跳跃, 最大子数组和
模式3: 左右两遍扫描       → 分发糖果, 接雨水
模式4: 单调栈贪心         → 去重复字母, 移K位
模式5: 收集正差           → 股票买卖
模式6: 自定义排序规则      → 最大数, 身高重建
模式7: 累积判断起点        → 加油站
```
