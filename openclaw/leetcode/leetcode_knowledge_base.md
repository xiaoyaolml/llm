# 📖 LeetCode 知识点详细教学

*作者：大胖超 😜 | 配合 leetcode_guide.md 食用更佳*

---

## 目录

1. [数组与字符串](#1-数组与字符串)
2. [链表](#2-链表)
3. [栈与队列](#3-栈与队列)
4. [哈希表](#4-哈希表)
5. [双指针](#5-双指针)
6. [滑动窗口](#6-滑动窗口)
7. [二分查找](#7-二分查找)
8. [二叉树](#8-二叉树)
9. [回溯算法](#9-回溯算法)
10. [动态规划](#10-动态规划)
11. [贪心算法](#11-贪心算法)
12. [图论](#12-图论)
13. [排序算法](#13-排序算法)
14. [堆/优先队列](#14-堆优先队列)
15. [字典树 Trie](#15-字典树-trie)
16. [并查集](#16-并查集)
17. [单调栈/单调队列](#17-单调栈单调队列)
18. [位运算](#18-位运算)
19. [前缀和与差分](#19-前缀和与差分)
20. [线段树与树状数组](#20-线段树与树状数组)

---

# 1. 数组与字符串

## 1.1 核心知识点

数组是**连续内存**存储的线性结构，支持 O(1) 随机访问，但插入/删除需要移动元素 O(n)。

### 关键操作复杂度
| 操作 | 时间复杂度 |
|------|-----------|
| 按下标访问 | O(1) |
| 尾部插入/删除 | O(1) |
| 中间插入/删除 | O(n) |
| 查找（无序） | O(n) |
| 查找（有序） | O(log n) |

## 1.2 常用技巧

### 技巧 1：原地操作（双指针覆盖）
用于"删除"元素而不使用额外空间。

```cpp
// LC 26: 删除有序数组中的重复项
// 核心思想：慢指针指向结果位置，快指针遍历数组
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;  // slow 指向最后一个不重复元素
    for (int fast = 1; fast < nums.size(); fast++) {
        if (nums[fast] != nums[slow]) {
            slow++;
            nums[slow] = nums[fast];
        }
    }
    return slow + 1;
}
```

**原理图解**：
```
原数组:  [1, 1, 2, 2, 3]
         s
            f
         
Step 1: nums[f]=1 == nums[s]=1, 跳过
Step 2: nums[f]=2 != nums[s]=1, slow++, 复制
        [1, 2, 2, 2, 3]
            s
               f
Step 3: nums[f]=2 == nums[s]=2, 跳过
Step 4: nums[f]=3 != nums[s]=2, slow++, 复制
        [1, 2, 3, 2, 3]
               s
                     f
结果: 前 3 个元素 [1, 2, 3]
```

### 技巧 2：前缀和（Prefix Sum）
用于快速计算子数组和。

```cpp
// 构建前缀和数组
// prefix[i] = nums[0] + nums[1] + ... + nums[i-1]
// 区间 [l, r] 的和 = prefix[r+1] - prefix[l]
vector<int> buildPrefixSum(vector<int>& nums) {
    int n = nums.size();
    vector<int> prefix(n + 1, 0);
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + nums[i];
    }
    return prefix;
}

// LC 303: 区域和检索 - 数组不可变
class NumArray {
    vector<int> prefix;
public:
    NumArray(vector<int>& nums) {
        prefix.resize(nums.size() + 1, 0);
        for (int i = 0; i < nums.size(); i++)
            prefix[i + 1] = prefix[i] + nums[i];
    }
    int sumRange(int left, int right) {
        return prefix[right + 1] - prefix[left];
    }
};
```

### 技巧 3：矩阵操作
```cpp
// LC 48: 旋转图像（顺时针90度）
// 技巧：先转置，再水平翻转
void rotate(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // 转置：行列互换
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            swap(matrix[i][j], matrix[j][i]);
    // 水平翻转：每行左右对调
    for (int i = 0; i < n; i++)
        reverse(matrix[i].begin(), matrix[i].end());
}
```

## 1.3 字符串专题

```cpp
// LC 5: 最长回文子串（中心扩展法）
// 核心：从每个位置向两边扩展，检查回文
string longestPalindrome(string s) {
    int start = 0, maxLen = 1;
    
    auto expand = [&](int left, int right) {
        while (left >= 0 && right < s.size() && s[left] == s[right]) {
            if (right - left + 1 > maxLen) {
                start = left;
                maxLen = right - left + 1;
            }
            left--;
            right++;
        }
    };
    
    for (int i = 0; i < s.size(); i++) {
        expand(i, i);     // 奇数长度回文
        expand(i, i + 1); // 偶数长度回文
    }
    return s.substr(start, maxLen);
}
```

## 1.4 经典必做题

| 题号 | 题目 | 难度 | 核心技巧 |
|------|------|------|---------|
| 1 | 两数之和 | Easy | 哈希表 |
| 15 | 三数之和 | Medium | 排序+双指针 |
| 26 | 删除重复项 | Easy | 快慢指针 |
| 48 | 旋转图像 | Medium | 转置+翻转 |
| 53 | 最大子数组和 | Medium | Kadane/DP |
| 56 | 合并区间 | Medium | 排序+贪心 |
| 238 | 除自身外乘积 | Medium | 前缀积 |

---

# 2. 链表

## 2.1 核心知识点

链表是**非连续内存**的线性结构，通过指针连接节点。

```cpp
// 单链表节点定义
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
```

### 关键操作
| 操作 | 时间复杂度 |
|------|-----------|
| 头部插入/删除 | O(1) |
| 尾部插入 | O(n)（无尾指针） |
| 中间插入/删除 | O(1)（已知前驱） |
| 按值查找 | O(n) |

## 2.2 核心技巧

### 技巧 1：虚拟头节点（Dummy Node）

**为什么需要？** 当操作可能影响头节点时，虚拟头节点统一操作逻辑，避免特判。

```cpp
// LC 203: 移除链表元素
ListNode* removeElements(ListNode* head, int val) {
    ListNode dummy(0);       // 虚拟头节点
    dummy.next = head;
    ListNode* prev = &dummy;
    
    while (prev->next) {
        if (prev->next->val == val) {
            ListNode* toDelete = prev->next;
            prev->next = prev->next->next;
            delete toDelete;
        } else {
            prev = prev->next;
        }
    }
    return dummy.next;
}
```

### 技巧 2：快慢指针

```cpp
// LC 141: 环形链表
// 原理：快指针每次走 2 步，慢指针走 1 步
// 如果有环，它们一定会相遇（就像操场跑步，快的总会追上慢的）
bool hasCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;        // 走 1 步
        fast = fast->next->next;  // 走 2 步
        if (slow == fast) return true;  // 相遇 = 有环
    }
    return false;
}

// LC 142: 环形链表 II（找入环点）
// 数学推导：相遇后，从 head 和相遇点同时走，再次相遇就是入环点
ListNode* detectCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            ListNode* p = head;
            while (p != slow) {
                p = p->next;
                slow = slow->next;
            }
            return p;
        }
    }
    return nullptr;
}

// LC 876: 链表的中间结点
ListNode* middleNode(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;  // fast 到终点时，slow 正好在中间
}
```

### 技巧 3：反转链表（面试超高频！）

```cpp
// LC 206: 反转链表
// 迭代法（推荐掌握）
ListNode* reverseList(ListNode* head) {
    ListNode *prev = nullptr, *curr = head;
    while (curr) {
        ListNode* next = curr->next;  // 保存下一个
        curr->next = prev;            // 反转指向
        prev = curr;                  // prev 前进
        curr = next;                  // curr 前进
    }
    return prev;
}

// 图解过程:
// 原始:  1 -> 2 -> 3 -> nullptr
// Step1: nullptr <- 1    2 -> 3 -> nullptr
//               prev  curr
// Step2: nullptr <- 1 <- 2    3 -> nullptr
//                      prev  curr
// Step3: nullptr <- 1 <- 2 <- 3
//                           prev  curr(nullptr)
// 返回 prev = 3

// 递归法（理解递归思维）
ListNode* reverseListRecursive(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode* newHead = reverseListRecursive(head->next);
    head->next->next = head;  // 后继指向自己
    head->next = nullptr;     // 断开原来的指向
    return newHead;
}
```

### 技巧 4：合并链表

```cpp
// LC 21: 合并两个有序链表
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    
    while (l1 && l2) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;  // 接上剩余部分
    return dummy.next;
}
```

## 2.3 经典必做题

| 题号 | 题目 | 难度 | 核心技巧 |
|------|------|------|---------|
| 21 | 合并两个有序链表 | Easy | 虚拟头+迭代 |
| 141 | 环形链表 | Easy | 快慢指针 |
| 206 | 反转链表 | Easy | 迭代/递归 |
| 19 | 删除倒数第N个 | Medium | 快慢指针 |
| 148 | 排序链表 | Medium | 归并排序 |
| 23 | 合并K个升序链表 | Hard | 最小堆/分治 |

---

# 3. 栈与队列

## 3.1 核心知识点

- **栈 (Stack)**：后进先出 (LIFO)，就像叠盘子
- **队列 (Queue)**：先进先出 (FIFO)，就像排队买饭

```
栈:   push → [3, 2, 1] → pop 出 3
队列: push → [1, 2, 3] → pop 出 1
```

## 3.2 栈的经典应用

### 括号匹配

```cpp
// LC 20: 有效的括号
// 核心思想：遇到左括号入栈，遇到右括号检查栈顶是否匹配
bool isValid(string s) {
    stack<char> stk;
    unordered_map<char, char> pairs = {
        {')', '('}, {']', '['}, {'}', '{'}
    };
    
    for (char c : s) {
        if (pairs.count(c)) {  // 右括号
            if (stk.empty() || stk.top() != pairs[c])
                return false;
            stk.pop();
        } else {  // 左括号
            stk.push(c);
        }
    }
    return stk.empty();
}
```

### 最小栈

```cpp
// LC 155: 最小栈
// 技巧：辅助栈同步维护当前最小值
class MinStack {
    stack<int> dataStack;
    stack<int> minStack;
public:
    void push(int val) {
        dataStack.push(val);
        if (minStack.empty() || val <= minStack.top())
            minStack.push(val);
    }
    void pop() {
        if (dataStack.top() == minStack.top())
            minStack.pop();
        dataStack.pop();
    }
    int top() { return dataStack.top(); }
    int getMin() { return minStack.top(); }
};
```

### 计算器系列

```cpp
// LC 224: 基本计算器（含括号和加减号）
// 思路：用栈保存括号外的符号状态
int calculate(string s) {
    stack<int> signs;
    signs.push(1);      // 最外层符号为正
    int result = 0, num = 0, sign = 1;
    
    for (char c : s) {
        if (isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c == '+' || c == '-') {
            result += sign * num;
            num = 0;
            sign = signs.top() * (c == '+' ? 1 : -1);
        } else if (c == '(') {
            signs.push(sign);
        } else if (c == ')') {
            signs.pop();
        }
    }
    result += sign * num;
    return result;
}
```

## 3.3 队列的经典应用

```cpp
// LC 232: 用栈实现队列
// 思路：两个栈倒腾，就像把一摞盘子倒到另一摞，顺序就反了
class MyQueue {
    stack<int> inStack, outStack;
    
    void transfer() {
        while (!inStack.empty()) {
            outStack.push(inStack.top());
            inStack.pop();
        }
    }
public:
    void push(int x) { inStack.push(x); }
    
    int pop() {
        if (outStack.empty()) transfer();
        int val = outStack.top();
        outStack.pop();
        return val;
    }
    
    int peek() {
        if (outStack.empty()) transfer();
        return outStack.top();
    }
    
    bool empty() { return inStack.empty() && outStack.empty(); }
};
```

---

# 4. 哈希表

## 4.1 核心知识点

哈希表（Hash Map）通过**哈希函数**将 key 映射到数组下标，实现 O(1) 的增删查。

### 核心思想：用空间换时间

```
                    哈希函数
key "apple"  -----→  h("apple") = 3 -----→ table[3] = value
```

### 冲突解决
- **链地址法**：同一位置用链表串起来（Java HashMap）
- **开放寻址法**：冲突时找下一个空位（Python dict）

## 4.2 经典题解

```cpp
// LC 1: 两数之和（面试必考第一题！）
// 思路：一边遍历一边查哈希表，看 target - nums[i] 是否已存在
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> map;  // 值 → 下标
    for (int i = 0; i < nums.size(); i++) {
        int complement = target - nums[i];
        if (map.count(complement))
            return {map[complement], i};
        map[nums[i]] = i;
    }
    return {};
}

// LC 49: 字母异位词分组
// 技巧：排序后的字符串作为 key
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> groups;
    for (string& s : strs) {
        string key = s;
        sort(key.begin(), key.end());  // "eat" → "aet"
        groups[key].push_back(s);
    }
    vector<vector<string>> result;
    for (auto& [key, group] : groups)
        result.push_back(group);
    return result;
}

// LC 128: 最长连续序列
// 思路：用 set 去重，只从序列起点开始计数
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> numSet(nums.begin(), nums.end());
    int maxLen = 0;
    
    for (int num : numSet) {
        // 只从起点开始（前一个不存在）
        if (!numSet.count(num - 1)) {
            int current = num;
            int len = 1;
            while (numSet.count(current + 1)) {
                current++;
                len++;
            }
            maxLen = max(maxLen, len);
        }
    }
    return maxLen;
}
```

## 4.3 哈希表使用场景总结

| 场景 | 例子 |
|------|------|
| 快速查找 | 两数之和、查重 |
| 计数 | 字符频率、投票 |
| 分组 | 异位词分组 |
| 去重 | 最长连续序列 |
| 缓存 | LRU、记忆化搜索 |

---

# 5. 双指针

## 5.1 核心思想

用两个指针协同遍历，减少不必要的计算，将暴力的 O(n²) 优化到 O(n)。

### 三种模式
1. **对撞指针**：从两端往中间走（有序数组）
2. **快慢指针**：不同速度前行（链表、数组）
3. **同向指针**：同向不同步前进（滑动窗口变体）

## 5.2 经典题解

```cpp
// LC 15: 三数之和
// 思路：排序 + 固定一个数 + 对撞双指针找另外两个
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    
    for (int i = 0; i < (int)nums.size() - 2; i++) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;  // 去重
        
        int left = i + 1, right = nums.size() - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                while (left < right && nums[left] == nums[left + 1]) left++;   // 去重
                while (left < right && nums[right] == nums[right - 1]) right--; // 去重
                left++;
                right--;
            } else if (sum < 0) {
                left++;   // 和太小，左指针右移
            } else {
                right--;  // 和太大，右指针左移
            }
        }
    }
    return result;
}

// LC 11: 盛最多水的容器
// 核心洞察：移动较短的那根线，才可能找到更大的容器
int maxArea(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int maxWater = 0;
    
    while (left < right) {
        int water = min(height[left], height[right]) * (right - left);
        maxWater = max(maxWater, water);
        
        if (height[left] < height[right])
            left++;
        else
            right--;
    }
    return maxWater;
}

// LC 283: 移动零
// 快慢指针：慢指针标记非零位置
void moveZeroes(vector<int>& nums) {
    int slow = 0;
    for (int fast = 0; fast < nums.size(); fast++) {
        if (nums[fast] != 0) {
            swap(nums[slow], nums[fast]);
            slow++;
        }
    }
}
```

---

# 6. 滑动窗口

## 6.1 核心思想

维护一个可变大小的"窗口"在数组/字符串上滑动，用于解决**子数组/子串**问题。

### 模板（超重要！！）

```
初始化: left = right = 0, 窗口相关变量
while (right < n):
    1. 右指针扩展（窗口右移）
    2. 更新窗口状态
    3. while (窗口需要收缩):
         左指针收缩
         更新答案
    4. right++
```

## 6.2 经典题解

```cpp
// LC 3: 无重复字符的最长子串
// 滑动窗口 + 哈希表记录字符位置
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> window;  // 字符 → 出现次数
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < s.size(); right++) {
        window[s[right]]++;
        
        // 有重复字符，收缩窗口
        while (window[s[right]] > 1) {
            window[s[left]]--;
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

// LC 76: 最小覆盖子串（Hard 但面试高频）
// 思路：滑动窗口找包含所有目标字符的最短子串
string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    
    int left = 0, valid = 0;
    int start = 0, minLen = INT_MAX;
    
    for (int right = 0; right < s.size(); right++) {
        char c = s[right];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;
        }
        
        // 窗口已覆盖所有字符，尝试收缩
        while (valid == need.size()) {
            if (right - left + 1 < minLen) {
                start = left;
                minLen = right - left + 1;
            }
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

// LC 438: 找到字符串中所有字母异位词
vector<int> findAnagrams(string s, string p) {
    vector<int> result;
    if (s.size() < p.size()) return result;
    
    vector<int> need(26, 0), window(26, 0);
    for (char c : p) need[c - 'a']++;
    
    for (int right = 0; right < s.size(); right++) {
        window[s[right] - 'a']++;
        if (right >= (int)p.size())
            window[s[right - p.size()] - 'a']--;
        if (window == need)
            result.push_back(right - p.size() + 1);
    }
    return result;
}
```

## 6.3 滑动窗口适用场景

- 最长/最短子串/子数组
- 含特定条件的子串
- 固定窗口大小的统计
- 关键词：**连续子数组、子串、窗口**

---

# 7. 二分查找

## 7.1 核心知识点

二分查找的本质：在一个具有**单调性**（或可二分性）的序列上，每次排除一半的搜索空间。

### ⚠️ 二分最大的坑：边界条件

## 7.2 三种模板（任选一种熟练掌握）

### 模板 1：标准二分（找精确值）
```cpp
// 在有序数组中查找 target
int binarySearch(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;  // 闭区间 [left, right]
    while (left <= right) {
        int mid = left + (right - left) / 2;  // 防溢出！
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
```

### 模板 2：左边界二分（找第一个 >= target 的位置）
```cpp
// lower_bound: 找到第一个 >= target 的位置
int lowerBound(vector<int>& nums, int target) {
    int left = 0, right = nums.size();  // 左闭右开 [left, right)
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) left = mid + 1;
        else right = mid;  // 注意：不是 mid - 1
    }
    return left;
}
```

### 模板 3：右边界二分（找最后一个 <= target 的位置）
```cpp
// upper_bound - 1: 找到最后一个 <= target 的位置
int upperBound(vector<int>& nums, int target) {
    int left = 0, right = nums.size();
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] <= target) left = mid + 1;
        else right = mid;
    }
    return left - 1;
}
```

### 模板选择速查
| 场景 | 用哪个 |
|------|--------|
| 找精确值 | 模板 1 |
| 找第一个满足条件的 | 模板 2 |
| 找最后一个满足条件的 | 模板 3 |
| 旋转数组/山脉数组 | 模板 1 变体 |

## 7.3 经典题解

```cpp
// LC 33: 搜索旋转排序数组
// 关键洞察：旋转后数组分成两段有序部分，判断 mid 在哪段
int search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        
        if (nums[left] <= nums[mid]) {  // 左半段有序
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;
            else
                left = mid + 1;
        } else {  // 右半段有序
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }
    return -1;
}

// LC 69: x 的平方根
// 二分搜索答案空间
int mySqrt(int x) {
    long left = 0, right = x;
    while (left <= right) {
        long mid = left + (right - left) / 2;
        if (mid * mid <= x && (mid + 1) * (mid + 1) > x)
            return mid;
        else if (mid * mid > x)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return 0;
}

// LC 875: 爱吃香蕉的珂珂（二分答案）
// 思路：二分速度 k，检查能否在 h 小时内吃完
int minEatingSpeed(vector<int>& piles, int h) {
    int left = 1, right = *max_element(piles.begin(), piles.end());
    while (left < right) {
        int mid = left + (right - left) / 2;
        int hours = 0;
        for (int p : piles)
            hours += (p + mid - 1) / mid;  // 向上取整
        if (hours <= h)
            right = mid;
        else
            left = mid + 1;
    }
    return left;
}
```

---

# 8. 二叉树

## 8.1 核心知识点

```cpp
struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
```

### 遍历方式
| 遍历 | 顺序 | 助记 |
|------|------|------|
| 前序 | 根→左→右 | 先看自己 |
| 中序 | 左→根→右 | 中间看自己（BST 有序）|
| 后序 | 左→右→根 | 最后看自己 |
| 层序 | 逐层从左到右 | BFS |

## 8.2 递归三要素（核心思维！）

1. **确定递归函数的参数和返回值**
2. **确定终止条件**（base case）
3. **确定单层递归的逻辑**

```cpp
// LC 104: 二叉树的最大深度
// 典型的"自底向上"递归
int maxDepth(TreeNode* root) {
    if (!root) return 0;  // 终止条件
    int leftDepth = maxDepth(root->left);   // 左子树深度
    int rightDepth = maxDepth(root->right); // 右子树深度
    return max(leftDepth, rightDepth) + 1;  // 当前层逻辑
}

// LC 226: 翻转二叉树
// "自顶向下"递归
TreeNode* invertTree(TreeNode* root) {
    if (!root) return nullptr;
    swap(root->left, root->right);  // 先交换
    invertTree(root->left);         // 再递归处理子树
    invertTree(root->right);
    return root;
}

// LC 101: 对称二叉树
bool isSymmetric(TreeNode* root) {
    return isMirror(root, root);
}
bool isMirror(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val 
        && isMirror(p->left, q->right) 
        && isMirror(p->right, q->left);
}
```

## 8.3 遍历模板

```cpp
// 前序遍历（迭代）
vector<int> preorder(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> stk;
    if (root) stk.push(root);
    
    while (!stk.empty()) {
        TreeNode* node = stk.top(); stk.pop();
        result.push_back(node->val);
        if (node->right) stk.push(node->right); // 先右后左
        if (node->left) stk.push(node->left);   // 栈是后进先出
    }
    return result;
}

// 层序遍历（BFS）
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (!root) return result;
    
    queue<TreeNode*> q;
    q.push(root);
    
    while (!q.empty()) {
        int size = q.size();  // 当前层的节点数
        vector<int> level;
        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front(); q.pop();
            level.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
        result.push_back(level);
    }
    return result;
}
```

## 8.4 BST（二叉搜索树）

**核心性质**：左子树所有值 < 根 < 右子树所有值，中序遍历有序！

```cpp
// LC 98: 验证二叉搜索树
bool isValidBST(TreeNode* root) {
    return validate(root, LONG_MIN, LONG_MAX);
}
bool validate(TreeNode* node, long minVal, long maxVal) {
    if (!node) return true;
    if (node->val <= minVal || node->val >= maxVal) return false;
    return validate(node->left, minVal, node->val)
        && validate(node->right, node->val, maxVal);
}

// LC 230: BST 中第 K 小的元素
// 中序遍历就是有序序列，走到第 k 个就行
int kthSmallest(TreeNode* root, int k) {
    stack<TreeNode*> stk;
    TreeNode* node = root;
    
    while (node || !stk.empty()) {
        while (node) {
            stk.push(node);
            node = node->left;
        }
        node = stk.top(); stk.pop();
        if (--k == 0) return node->val;
        node = node->right;
    }
    return -1;
}
```

## 8.5 经典必做题

| 题号 | 题目 | 核心考点 |
|------|------|---------|
| 94 | 中序遍历 | 遍历基础 |
| 102 | 层序遍历 | BFS |
| 104 | 最大深度 | 递归 |
| 226 | 翻转二叉树 | 递归 |
| 236 | 最近公共祖先 | 递归 |
| 105 | 前序+中序构造树 | 分治 |
| 124 | 最大路径和 | 后序遍历 |

---

# 9. 回溯算法

## 9.1 核心思想

回溯 = **穷举 + 剪枝**。通过递归深入，不满足条件就"回溯"（撤销选择）。

### 万能模板（背下来！）

```
void backtrack(路径, 选择列表) {
    if (满足结束条件) {
        收集结果;
        return;
    }
    
    for (选择 : 选择列表) {
        做选择;           // 进入下一层
        backtrack(路径, 选择列表);
        撤销选择;          // 回到当前层
    }
}
```

### 回溯本质是一棵决策树

```
            []
          / | \
        [1] [2] [3]    ← 第一层选择
       / \    |
    [1,2] [1,3] [2,3]  ← 第二层选择
      |
   [1,2,3]             ← 叶子节点 = 一个结果
```

## 9.2 经典题解

### 排列（顺序有关）

```cpp
// LC 46: 全排列
// [1,2,3] 的排列：[1,2,3], [1,3,2], [2,1,3], [2,3,1], [3,1,2], [3,2,1]
vector<vector<int>> permute(vector<int>& nums) {
    vector<vector<int>> result;
    vector<int> path;
    vector<bool> used(nums.size(), false);
    
    function<void()> backtrack = [&]() {
        if (path.size() == nums.size()) {
            result.push_back(path);
            return;
        }
        for (int i = 0; i < nums.size(); i++) {
            if (used[i]) continue;  // 跳过已用的
            
            used[i] = true;
            path.push_back(nums[i]);
            backtrack();
            path.pop_back();     // 撤销
            used[i] = false;     // 撤销
        }
    };
    
    backtrack();
    return result;
}
```

### 组合（顺序无关）

```cpp
// LC 77: 组合
// C(4,2) = [1,2], [1,3], [1,4], [2,3], [2,4], [3,4]
vector<vector<int>> combine(int n, int k) {
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int)> backtrack = [&](int start) {
        if (path.size() == k) {
            result.push_back(path);
            return;
        }
        // 剪枝：剩余元素不够凑齐 k 个，直接跳过
        for (int i = start; i <= n - (k - path.size()) + 1; i++) {
            path.push_back(i);
            backtrack(i + 1);  // 从 i+1 开始，避免重复
            path.pop_back();
        }
    };
    
    backtrack(1);
    return result;
}
```

### 子集

```cpp
// LC 78: 子集
// [1,2,3] 的子集：[], [1], [2], [1,2], [3], [1,3], [2,3], [1,2,3]
vector<vector<int>> subsets(vector<int>& nums) {
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int)> backtrack = [&](int start) {
        result.push_back(path);  // 每个节点都是一个子集
        for (int i = start; i < nums.size(); i++) {
            path.push_back(nums[i]);
            backtrack(i + 1);
            path.pop_back();
        }
    };
    
    backtrack(0);
    return result;
}
```

### 去重技巧

```cpp
// LC 40: 组合总和 II（含重复元素的组合，每个元素只能用一次）
vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
    sort(candidates.begin(), candidates.end());  // 排序是去重的前提！
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int, int)> backtrack = [&](int start, int remain) {
        if (remain == 0) { result.push_back(path); return; }
        for (int i = start; i < candidates.size(); i++) {
            if (candidates[i] > remain) break;  // 剪枝
            // 去重：同一层中，相同的值只选第一个
            if (i > start && candidates[i] == candidates[i - 1]) continue;
            
            path.push_back(candidates[i]);
            backtrack(i + 1, remain - candidates[i]);
            path.pop_back();
        }
    };
    
    backtrack(0, target);
    return result;
}
```

## 9.3 排列/组合/子集速查

| 类型 | 能否重复使用 | start 参数 | 去重 |
|------|------------|-----------|------|
| 排列 | 否 | 无，用 used[] | — |
| 组合 | 否 | 有，从 start 开始 | 排序+跳过 |
| 子集 | 否 | 有，从 start 开始 | 排序+跳过 |
| 可重复组合 | 是 | 有，从 i 开始 | — |

---

# 10. 动态规划

## 10.1 核心思想

将一个大问题拆成**重叠子问题**，用**状态转移方程**从小问题推出大问题的解。

### DP 五步法（每道题都这么想！）

```
1. 定义状态：dp[i] 表示什么？
2. 状态转移：dp[i] 怎么从 dp[i-1] 等推出来？
3. 初始条件：dp[0] = ? dp[1] = ?
4. 遍历顺序：从小到大？从大到小？
5. 举例验证：用一个小例子手动推演
```

## 10.2 经典题型分类

### 类型 1：线性 DP（一维）

```cpp
// LC 70: 爬楼梯
// dp[i] = 到第 i 阶的方法数
// dp[i] = dp[i-1] + dp[i-2]（要么从 i-1 跨 1 步，要么从 i-2 跨 2 步）
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

// LC 198: 打家劫舍
// dp[i] = 偷到第 i 家能获得的最大金额
// dp[i] = max(dp[i-1], dp[i-2] + nums[i])
//         不偷第i家   偷第i家
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

// LC 53: 最大子数组和（Kadane 算法）
// dp[i] = 以 nums[i] 结尾的最大子数组和
// dp[i] = max(nums[i], dp[i-1] + nums[i])
int maxSubArray(vector<int>& nums) {
    int maxSum = nums[0], currentSum = nums[0];
    for (int i = 1; i < nums.size(); i++) {
        currentSum = max(nums[i], currentSum + nums[i]);
        maxSum = max(maxSum, currentSum);
    }
    return maxSum;
}
```

### 类型 2：线性 DP（二维/LCS/LIS）

```cpp
// LC 300: 最长递增子序列 (LIS)
// dp[i] = 以 nums[i] 结尾的 LIS 长度
int lengthOfLIS(vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 1);  // 每个元素自身是长度 1 的子序列
    int maxLen = 1;
    
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i])
                dp[i] = max(dp[i], dp[j] + 1);
        }
        maxLen = max(maxLen, dp[i]);
    }
    return maxLen;
}
// O(n²)，可优化到 O(n log n) 用二分查找

// LC 1143: 最长公共子序列 (LCS)
// dp[i][j] = text1 前i个字符和 text2 前j个字符的 LCS 长度
int longestCommonSubsequence(string text1, string text2) {
    int m = text1.size(), n = text2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (text1[i-1] == text2[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    }
    return dp[m][n];
}
```

### 类型 3：背包问题

```cpp
// === 0-1 背包：每个物品只能选一次 ===
// dp[j] = 容量为 j 时的最大价值
// 外层遍历物品，内层从大到小遍历容量
int knapsack01(vector<int>& weight, vector<int>& value, int capacity) {
    vector<int> dp(capacity + 1, 0);
    for (int i = 0; i < weight.size(); i++) {
        for (int j = capacity; j >= weight[i]; j--) {  // 从大到小！
            dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);
        }
    }
    return dp[capacity];
}

// LC 322: 零钱兑换（完全背包：每个硬币可以无限使用）
// dp[j] = 凑齐金额 j 所需的最少硬币数
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;
    
    for (int coin : coins) {
        for (int j = coin; j <= amount; j++) {  // 从小到大！
            if (dp[j - coin] != INT_MAX)
                dp[j] = min(dp[j], dp[j - coin] + 1);
        }
    }
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// LC 416: 分割等和子集（转化为 0-1 背包）
bool canPartition(vector<int>& nums) {
    int sum = accumulate(nums.begin(), nums.end(), 0);
    if (sum % 2 != 0) return false;
    int target = sum / 2;
    
    vector<bool> dp(target + 1, false);
    dp[0] = true;
    for (int num : nums) {
        for (int j = target; j >= num; j--) {
            dp[j] = dp[j] || dp[j - num];
        }
    }
    return dp[target];
}
```

### 类型 4：区间 DP

```cpp
// LC 516: 最长回文子序列
// dp[i][j] = s[i..j] 中最长回文子序列长度
int longestPalindromeSubseq(string s) {
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n, 0));
    
    for (int i = n - 1; i >= 0; i--) {
        dp[i][i] = 1;
        for (int j = i + 1; j < n; j++) {
            if (s[i] == s[j])
                dp[i][j] = dp[i+1][j-1] + 2;
            else
                dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
        }
    }
    return dp[0][n-1];
}
```

### 类型 5：状态机 DP

```cpp
// LC 121: 买卖股票的最佳时机
// 思路：记录到目前为止的最低价，计算每天卖出的利润
int maxProfit(vector<int>& prices) {
    int minPrice = INT_MAX, maxProfit = 0;
    for (int price : prices) {
        minPrice = min(minPrice, price);
        maxProfit = max(maxProfit, price - minPrice);
    }
    return maxProfit;
}

// LC 309: 最佳买卖股票时机含冷冻期
// 状态：持有、未持有(刚卖)、未持有(冷冻期后)
int maxProfit309(vector<int>& prices) {
    int n = prices.size();
    if (n < 2) return 0;
    
    int hold = -prices[0]; // 持有股票
    int sold = 0;          // 刚卖出（下一天冷冻）
    int rest = 0;          // 不持有且不在冷冻期
    
    for (int i = 1; i < n; i++) {
        int prevHold = hold, prevSold = sold, prevRest = rest;
        hold = max(prevHold, prevRest - prices[i]);
        sold = prevHold + prices[i];
        rest = max(prevRest, prevSold);
    }
    return max(sold, rest);
}
```

## 10.3 DP 经典题攻略速查

| 题号 | 题目 | DP 类型 | 状态定义 |
|------|------|--------|---------|
| 70 | 爬楼梯 | 线性 | dp[i]=到第i阶的方法 |
| 198 | 打家劫舍 | 线性 | dp[i]=偷到第i家最大值 |
| 322 | 零钱兑换 | 完全背包 | dp[j]=凑j元最少硬币 |
| 300 | LIS | 线性 | dp[i]=以i结尾的LIS |
| 1143 | LCS | 二维 | dp[i][j]=前i,j的LCS |
| 62 | 不同路径 | 网格 | dp[i][j]=到(i,j)的路径数 |
| 121 | 买卖股票 | 状态机 | 持有/不持有 |

---

# 11. 贪心算法

## 11.1 核心思想

每一步都做**局部最优**选择，期望最终得到**全局最优**。

### 贪心 vs DP
- 贪心：当前最优就是全局最优（需要**贪心证明**）
- DP：需要考虑所有子问题

### ⚠️ 贪心的难点：证明局部最优能推出全局最优

## 11.2 经典题解

```cpp
// LC 55: 跳跃游戏
// 贪心：维护能到达的最远位置
bool canJump(vector<int>& nums) {
    int maxReach = 0;
    for (int i = 0; i < nums.size(); i++) {
        if (i > maxReach) return false;  // 到不了这里
        maxReach = max(maxReach, i + nums[i]);
    }
    return true;
}

// LC 45: 跳跃游戏 II（最少跳跃次数）
int jump(vector<int>& nums) {
    int jumps = 0, curEnd = 0, farthest = 0;
    for (int i = 0; i < (int)nums.size() - 1; i++) {
        farthest = max(farthest, i + nums[i]);
        if (i == curEnd) {
            jumps++;
            curEnd = farthest;
        }
    }
    return jumps;
}

// LC 56: 合并区间
vector<vector<int>> merge(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end());
    vector<vector<int>> result;
    
    for (auto& interval : intervals) {
        if (result.empty() || result.back()[1] < interval[0])
            result.push_back(interval);
        else
            result.back()[1] = max(result.back()[1], interval[1]);
    }
    return result;
}

// LC 435: 无重叠区间
// 贪心策略：按结束时间排序，优先选结束早的
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end(),
         [](auto& a, auto& b) { return a[1] < b[1]; });
    
    int count = 0, prevEnd = INT_MIN;
    for (auto& interval : intervals) {
        if (interval[0] >= prevEnd) {
            prevEnd = interval[1];  // 不重叠，保留
        } else {
            count++;  // 重叠，删除
        }
    }
    return count;
}
```

---

# 12. 图论

## 12.1 核心知识点

### 图的表示
```cpp
// 邻接矩阵（适合稠密图）
vector<vector<int>> adjMatrix(n, vector<int>(n, 0));
adjMatrix[u][v] = weight;

// 邻接表（适合稀疏图，更常用）
vector<vector<int>> adjList(n);
adjList[u].push_back(v);

// 带权邻接表
vector<vector<pair<int,int>>> adjList(n);  // {邻居, 权重}
adjList[u].push_back({v, weight});
```

### BFS vs DFS
| 特点 | BFS | DFS |
|------|-----|-----|
| 数据结构 | 队列 | 栈/递归 |
| 空间 | O(宽度) | O(深度) |
| 适用场景 | 最短路径、层次遍历 | 连通性、路径问题 |

## 12.2 经典题解

### BFS：最短路径/层次遍历

```cpp
// LC 200: 岛屿数量
// BFS 遍历每个岛屿，标记已访问
int numIslands(vector<vector<char>>& grid) {
    int m = grid.size(), n = grid[0].size();
    int count = 0;
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') {
                count++;
                queue<pair<int,int>> q;
                q.push({i, j});
                grid[i][j] = '0';  // 标记已访问
                
                while (!q.empty()) {
                    auto [x, y] = q.front(); q.pop();
                    for (int d = 0; d < 4; d++) {
                        int nx = x + dx[d], ny = y + dy[d];
                        if (nx >= 0 && nx < m && ny >= 0 && ny < n 
                            && grid[nx][ny] == '1') {
                            grid[nx][ny] = '0';
                            q.push({nx, ny});
                        }
                    }
                }
            }
        }
    }
    return count;
}

// LC 994: 腐烂的橘子（多源 BFS）
int orangesRotting(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    queue<pair<int,int>> q;
    int fresh = 0;
    
    // 所有腐烂的橘子入队
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == 2) q.push({i, j});
            else if (grid[i][j] == 1) fresh++;
        }
    
    if (fresh == 0) return 0;
    
    int minutes = 0;
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};
    
    while (!q.empty()) {
        int size = q.size();
        bool rotted = false;
        for (int k = 0; k < size; k++) {
            auto [x, y] = q.front(); q.pop();
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (nx >= 0 && nx < m && ny >= 0 && ny < n 
                    && grid[nx][ny] == 1) {
                    grid[nx][ny] = 2;
                    q.push({nx, ny});
                    fresh--;
                    rotted = true;
                }
            }
        }
        if (rotted) minutes++;
    }
    return fresh == 0 ? minutes : -1;
}
```

### DFS：连通性/路径

```cpp
// LC 547: 省份数量（并查集也可以）
int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    vector<bool> visited(n, false);
    int count = 0;
    
    function<void(int)> dfs = [&](int city) {
        visited[city] = true;
        for (int j = 0; j < n; j++) {
            if (isConnected[city][j] == 1 && !visited[j])
                dfs(j);
        }
    };
    
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            dfs(i);
            count++;
        }
    }
    return count;
}
```

### 拓扑排序

```cpp
// LC 207: 课程表（拓扑排序：判断有向图是否有环）
// Kahn 算法：BFS 实现
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> graph(numCourses);
    vector<int> inDegree(numCourses, 0);
    
    for (auto& p : prerequisites) {
        graph[p[1]].push_back(p[0]);
        inDegree[p[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (inDegree[i] == 0) q.push(i);
    
    int count = 0;
    while (!q.empty()) {
        int course = q.front(); q.pop();
        count++;
        for (int next : graph[course]) {
            if (--inDegree[next] == 0)
                q.push(next);
        }
    }
    return count == numCourses;
}
```

---

# 13. 排序算法

## 13.1 排序算法复杂度一览

| 算法 | 平均 | 最坏 | 空间 | 稳定 |
|------|------|------|------|------|
| 快速排序 | O(nlogn) | O(n²) | O(logn) | 否 |
| 归并排序 | O(nlogn) | O(nlogn) | O(n) | 是 |
| 堆排序 | O(nlogn) | O(nlogn) | O(1) | 否 |
| 计数排序 | O(n+k) | O(n+k) | O(k) | 是 |

## 13.2 手写排序（面试可能要求）

### 快速排序
```cpp
// 核心：选一个 pivot，小的放左边，大的放右边，递归
void quickSort(vector<int>& nums, int left, int right) {
    if (left >= right) return;
    
    int pivot = nums[left + (right - left) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (nums[i] < pivot) i++;
        while (nums[j] > pivot) j--;
        if (i <= j) {
            swap(nums[i], nums[j]);
            i++;
            j--;
        }
    }
    quickSort(nums, left, j);
    quickSort(nums, i, right);
}
```

### 归并排序
```cpp
// 核心：分成两半分别排序，再合并
void mergeSort(vector<int>& nums, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(nums, left, mid);
    mergeSort(nums, mid + 1, right);
    
    // 合并两个有序部分
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (nums[i] <= nums[j]) temp[k++] = nums[i++];
        else temp[k++] = nums[j++];
    }
    while (i <= mid) temp[k++] = nums[i++];
    while (j <= right) temp[k++] = nums[j++];
    copy(temp.begin(), temp.end(), nums.begin() + left);
}
```

### 堆排序
```cpp
void heapify(vector<int>& nums, int n, int i) {
    int largest = i, left = 2 * i + 1, right = 2 * i + 2;
    if (left < n && nums[left] > nums[largest]) largest = left;
    if (right < n && nums[right] > nums[largest]) largest = right;
    if (largest != i) {
        swap(nums[i], nums[largest]);
        heapify(nums, n, largest);
    }
}

void heapSort(vector<int>& nums) {
    int n = nums.size();
    // 建堆
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(nums, n, i);
    // 排序
    for (int i = n - 1; i > 0; i--) {
        swap(nums[0], nums[i]);
        heapify(nums, i, 0);
    }
}
```

---

# 14. 堆/优先队列

## 14.1 核心知识点

堆是一个**完全二叉树**，满足：
- **最大堆**：父 >= 子（`priority_queue<int>` 默认）
- **最小堆**：父 <= 子（`priority_queue<int, vector<int>, greater<int>>`）

### 常用于
- Top K 问题
- 合并 K 个有序序列
- 中位数维护

## 14.2 经典题解

```cpp
// LC 215: 数组中的第K个最大元素
// 方法 1：最小堆，维护 k 个最大元素
int findKthLargest(vector<int>& nums, int k) {
    priority_queue<int, vector<int>, greater<int>> minHeap;
    for (int num : nums) {
        minHeap.push(num);
        if (minHeap.size() > k) minHeap.pop();
    }
    return minHeap.top();
}

// LC 347: 前 K 个高频元素
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int num : nums) freq[num]++;
    
    // 最小堆，按频率排序
    auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
        return a.second > b.second;
    };
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);
    
    for (auto& [num, count] : freq) {
        pq.push({num, count});
        if (pq.size() > k) pq.pop();
    }
    
    vector<int> result;
    while (!pq.empty()) {
        result.push_back(pq.top().first);
        pq.pop();
    }
    return result;
}

// LC 23: 合并K个升序链表
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);
    
    for (auto list : lists)
        if (list) pq.push(list);
    
    ListNode dummy(0);
    ListNode* tail = &dummy;
    
    while (!pq.empty()) {
        ListNode* node = pq.top(); pq.pop();
        tail->next = node;
        tail = tail->next;
        if (node->next) pq.push(node->next);
    }
    return dummy.next;
}

// LC 295: 数据流的中位数
// 技巧：大顶堆存较小一半，小顶堆存较大一半
class MedianFinder {
    priority_queue<int> maxHeap;  // 较小一半
    priority_queue<int, vector<int>, greater<int>> minHeap;  // 较大一半
public:
    void addNum(int num) {
        maxHeap.push(num);
        minHeap.push(maxHeap.top());
        maxHeap.pop();
        
        if (minHeap.size() > maxHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }
    
    double findMedian() {
        if (maxHeap.size() > minHeap.size())
            return maxHeap.top();
        return (maxHeap.top() + minHeap.top()) / 2.0;
    }
};
```

---

# 15. 字典树 Trie

## 15.1 核心知识点

Trie（前缀树）用于高效存储和查询字符串集合，特别适合**前缀匹配**。

```
插入 "apple", "app", "bat":
         root
        /    \
       a      b
       |      |
       p      a
       |      |
       p*     t*
       |
       l
       |
       e*
(* 表示一个完整单词的结尾)
```

## 15.2 实现

```cpp
// LC 208: 实现 Trie
class Trie {
    struct TrieNode {
        TrieNode* children[26] = {};
        bool isEnd = false;
    };
    TrieNode* root;
    
public:
    Trie() : root(new TrieNode()) {}
    
    void insert(string word) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx])
                node->children[idx] = new TrieNode();
            node = node->children[idx];
        }
        node->isEnd = true;
    }
    
    bool search(string word) {
        TrieNode* node = find(word);
        return node && node->isEnd;
    }
    
    bool startsWith(string prefix) {
        return find(prefix) != nullptr;
    }
    
private:
    TrieNode* find(string& s) {
        TrieNode* node = root;
        for (char c : s) {
            int idx = c - 'a';
            if (!node->children[idx]) return nullptr;
            node = node->children[idx];
        }
        return node;
    }
};
```

---

# 16. 并查集

## 16.1 核心知识点

并查集（Union-Find）用于处理**动态连通性**问题：
- `find(x)`：找到 x 的根节点（代表元素）
- `union(x, y)`：合并 x 和 y 所在的集合

### 两个优化
1. **路径压缩**：find 时直接指向根
2. **按秩合并**：矮树接到高树上

## 16.2 模板

```cpp
class UnionFind {
    vector<int> parent, rank_;
public:
    UnionFind(int n) : parent(n), rank_(n, 0) {
        iota(parent.begin(), parent.end(), 0);  // parent[i] = i
    }
    
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);  // 路径压缩
        return parent[x];
    }
    
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;  // 已在同一集合
        
        // 按秩合并
        if (rank_[px] < rank_[py]) swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        return true;
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

// LC 547: 省份数量（并查集解法）
int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    int provinces = n;
    
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (isConnected[i][j] == 1)
                if (uf.unite(i, j))
                    provinces--;
    
    return provinces;
}
```

---

# 17. 单调栈/单调队列

## 17.1 单调栈

核心：维护一个**元素单调递增/递减的栈**，用于找每个元素"左/右边第一个比它大/小的元素"。

```cpp
// LC 739: 每日温度
// 找到每天之后第一个更热的天数
vector<int> dailyTemperatures(vector<int>& temperatures) {
    int n = temperatures.size();
    vector<int> result(n, 0);
    stack<int> stk;  // 存下标，栈底到栈顶温度递减
    
    for (int i = 0; i < n; i++) {
        while (!stk.empty() && temperatures[i] > temperatures[stk.top()]) {
            int prevDay = stk.top(); stk.pop();
            result[prevDay] = i - prevDay;
        }
        stk.push(i);
    }
    return result;
}

// LC 84: 柱状图中最大的矩形
int largestRectangleArea(vector<int>& heights) {
    stack<int> stk;
    int maxArea = 0;
    heights.push_back(0);  // 哨兵
    
    for (int i = 0; i < heights.size(); i++) {
        while (!stk.empty() && heights[i] < heights[stk.top()]) {
            int h = heights[stk.top()]; stk.pop();
            int w = stk.empty() ? i : i - stk.top() - 1;
            maxArea = max(maxArea, h * w);
        }
        stk.push(i);
    }
    return maxArea;
}
```

## 17.2 单调队列

```cpp
// LC 239: 滑动窗口最大值
// 维护一个递减的双端队列
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;  // 存下标，对应值递减
    vector<int> result;
    
    for (int i = 0; i < nums.size(); i++) {
        // 移除过期元素
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();
        // 维护递减：比当前小的都不要了
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();
        dq.push_back(i);
        
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}
```

---

# 18. 位运算

## 18.1 核心操作

| 操作 | 符号 | 作用 | 例子 |
|------|------|------|------|
| 与 | & | 两位都为1才为1 | 清除位 |
| 或 | \| | 任一位为1就为1 | 设置位 |
| 异或 | ^ | 不同为1，相同为0 | 交换/去重 |
| 取反 | ~ | 0变1，1变0 | |
| 左移 | << | 乘以 2^n | |
| 右移 | >> | 除以 2^n | |

### 常用技巧
```cpp
n & (n - 1)  // 消除最低位的 1
n & (-n)     // 获取最低位的 1
n >> k & 1   // 获取第 k 位
n | (1 << k) // 将第 k 位设为 1
n & ~(1<<k)  // 将第 k 位设为 0
```

## 18.2 经典题解

```cpp
// LC 136: 只出现一次的数字
// 异或的性质：a ^ a = 0, a ^ 0 = a
int singleNumber(vector<int>& nums) {
    int result = 0;
    for (int num : nums)
        result ^= num;
    return result;
}

// LC 191: 位 1 的个数
int hammingWeight(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);  // 消除最低位的 1
        count++;
    }
    return count;
}

// LC 338: 比特位计数
vector<int> countBits(int n) {
    vector<int> dp(n + 1, 0);
    for (int i = 1; i <= n; i++)
        dp[i] = dp[i & (i - 1)] + 1;
    return dp;
}
```

---

# 19. 前缀和与差分

## 19.1 前缀和（详解）

```
原数组:   [1, 3, 5, 7, 9]
前缀和: [0, 1, 4, 9, 16, 25]
                ↓
区间 [1,3] 的和 = prefix[4] - prefix[1] = 16 - 1 = 15
即 3 + 5 + 7 = 15 ✓
```

### 二维前缀和
```cpp
// LC 304: 二维区域和检索
class NumMatrix {
    vector<vector<int>> prefix;
public:
    NumMatrix(vector<vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        prefix.assign(m + 1, vector<int>(n + 1, 0));
        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++)
                prefix[i][j] = matrix[i-1][j-1] + prefix[i-1][j] 
                             + prefix[i][j-1] - prefix[i-1][j-1];
    }
    
    int sumRegion(int r1, int c1, int r2, int c2) {
        return prefix[r2+1][c2+1] - prefix[r1][c2+1] 
             - prefix[r2+1][c1] + prefix[r1][c1];
    }
};
```

## 19.2 差分数组

对区间 [l, r] 统一加 val → 差分数组 d[l] += val, d[r+1] -= val，前缀和还原。

```cpp
// LC 1094: 拼车
bool carPooling(vector<vector<int>>& trips, int capacity) {
    vector<int> diff(1001, 0);
    for (auto& trip : trips) {
        diff[trip[1]] += trip[0];       // 上车
        diff[trip[2]] -= trip[0];       // 下车
    }
    
    int passengers = 0;
    for (int i = 0; i < 1001; i++) {
        passengers += diff[i];
        if (passengers > capacity) return false;
    }
    return true;
}
```

---

# 20. 线段树与树状数组

## 20.1 树状数组（Binary Indexed Tree / Fenwick Tree）

适用于**单点修改 + 区间查询**，代码比线段树简洁。

```cpp
class BIT {
    vector<int> tree;
    int n;
    
    int lowbit(int x) { return x & (-x); }
public:
    BIT(int n) : n(n), tree(n + 1, 0) {}
    
    void update(int i, int delta) {
        for (; i <= n; i += lowbit(i))
            tree[i] += delta;
    }
    
    int query(int i) {
        int sum = 0;
        for (; i > 0; i -= lowbit(i))
            sum += tree[i];
        return sum;
    }
    
    int rangeQuery(int l, int r) {
        return query(r) - query(l - 1);
    }
};
```

## 20.2 线段树（Segment Tree）

适用于**区间修改 + 区间查询**，功能更强大。

```cpp
class SegTree {
    vector<int> tree, lazy;
    int n;
    
    void build(vector<int>& nums, int node, int start, int end) {
        if (start == end) { tree[node] = nums[start]; return; }
        int mid = (start + end) / 2;
        build(nums, 2*node, start, mid);
        build(nums, 2*node+1, mid+1, end);
        tree[node] = tree[2*node] + tree[2*node+1];
    }
    
    void pushDown(int node, int start, int end) {
        if (lazy[node]) {
            int mid = (start + end) / 2;
            tree[2*node] += lazy[node] * (mid - start + 1);
            tree[2*node+1] += lazy[node] * (end - mid);
            lazy[2*node] += lazy[node];
            lazy[2*node+1] += lazy[node];
            lazy[node] = 0;
        }
    }
    
    void update(int node, int start, int end, int l, int r, int val) {
        if (l <= start && end <= r) {
            tree[node] += val * (end - start + 1);
            lazy[node] += val;
            return;
        }
        pushDown(node, start, end);
        int mid = (start + end) / 2;
        if (l <= mid) update(2*node, start, mid, l, r, val);
        if (r > mid) update(2*node+1, mid+1, end, l, r, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }
    
    int query(int node, int start, int end, int l, int r) {
        if (l <= start && end <= r) return tree[node];
        pushDown(node, start, end);
        int mid = (start + end) / 2, sum = 0;
        if (l <= mid) sum += query(2*node, start, mid, l, r);
        if (r > mid) sum += query(2*node+1, mid+1, end, l, r);
        return sum;
    }
    
public:
    SegTree(vector<int>& nums) : n(nums.size()), tree(4*n), lazy(4*n, 0) {
        build(nums, 1, 0, n-1);
    }
    void update(int l, int r, int val) { update(1, 0, n-1, l, r, val); }
    int query(int l, int r) { return query(1, 0, n-1, l, r); }
};
```

---

# 📊 附录：复杂度速查表

## 数据结构操作复杂度

| 数据结构 | 查找 | 插入 | 删除 | 空间 |
|---------|------|------|------|------|
| 数组 | O(n)/O(1) | O(n) | O(n) | O(n) |
| 链表 | O(n) | O(1) | O(1) | O(n) |
| 哈希表 | O(1) | O(1) | O(1) | O(n) |
| BST | O(logn) | O(logn) | O(logn) | O(n) |
| 堆 | O(n)/O(1) | O(logn) | O(logn) | O(n) |
| Trie | O(m) | O(m) | O(m) | O(Σ·m) |

## 算法复杂度

| 算法 | 时间 | 空间 | 适用 |
|------|------|------|------|
| 二分查找 | O(logn) | O(1) | 有序/单调 |
| 双指针 | O(n) | O(1) | 有序数组 |
| 滑动窗口 | O(n) | O(k) | 子串问题 |
| BFS | O(V+E) | O(V) | 最短路径 |
| DFS | O(V+E) | O(V) | 连通性 |
| 回溯 | O(k·n^k) | O(n) | 组合搜索 |
| DP | 看状态数 | 看状态数 | 最优子结构 |
| 并查集 | O(α(n))≈O(1) | O(n) | 连通性 |

---

# 🗺️ 题型→算法 快速决策图

```
看到题目第一反应：
│
├── 求最值/最优解
│   ├── 有最优子结构 → 动态规划
│   └── 局部最优=全局最优 → 贪心
│
├── 求所有方案/路径
│   └── 回溯
│
├── 有序数据查找
│   └── 二分查找
│
├── 连续子数组/子串
│   └── 滑动窗口 / 前缀和
│
├── 最短路径/层次
│   └── BFS
│
├── 连通性
│   ├── 静态 → DFS/BFS
│   └── 动态合并 → 并查集
│
├── Top K / 第 K 大
│   └── 堆
│
├── 前缀匹配
│   └── Trie
│
├── 下一个更大/更小元素
│   └── 单调栈
│
└── 括号/嵌套/配对
    └── 栈
```

---

---

# 📚 延伸阅读

| 文档 | 内容 |
|------|------|
| `knowlege_appendix_detailed.md` | 附录深度展开：复杂度完全指南、决策图详解、面试速查卡片 |
| `knowlege_gap_analysis.md` | 全 20 专题查漏补缺：跨专题衔接、易混淆辨析、竞赛模板补充 |
| `knowlege_study_roadmap.md` | 零基础到精通完全路线图：四阶段规划、100 题精选清单、刷题方法论 |

---

*最后更新：2026-03-03*
*Stay hungry, stay foolish! 🚀*
