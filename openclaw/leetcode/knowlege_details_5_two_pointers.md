# 📖 专题五：双指针（Two Pointers）— 从零基础到精通

*作者：大胖超 😜 | 配合 leetcode_knowledge_base.md 食用更佳*

---

## 目录

1. [双指针的核心思想](#1-双指针的核心思想)
2. [对撞指针（左右指针）](#2-对撞指针左右指针)
3. [快慢指针](#3-快慢指针)
4. [同向双指针](#4-同向双指针)
5. [两数之和系列](#5-两数之和系列)
6. [三数之和 / N 数之和](#6-三数之和--n-数之和)
7. [盛水问题](#7-盛水问题)
8. [数组原地操作](#8-数组原地操作)
9. [链表双指针](#9-链表双指针)
10. [字符串双指针](#10-字符串双指针)
11. [经典 LeetCode 双指针题精讲](#11-经典-leetcode-双指针题精讲)
12. [常见陷阱与最佳实践](#12-常见陷阱与最佳实践)

---

# 1. 双指针的核心思想

## 1.1 什么是双指针？

**双指针 (Two Pointers)** = 用两个指针（下标/迭代器）协同遍历数据，利用单调性或规律跳过不必要的计算。

> 一句话：**暴力枚举 O(n²) → 双指针 O(n)，核心是利用单调性剪枝。**

```
暴力法：双重循环枚举所有 (i, j) 对 → O(n²)
双指针：利用某种单调性，一次遍历搞定 → O(n)

            对撞指针 (left / right)
              ←  →  ←  →
            [1, 2, 3, 4, 5]
             L→         ←R

            快慢指针 (slow / fast)
            slow → → fast → → → →
            [1, 2, 3, 4, 5, 6, 7]
```

## 1.2 三大模式

| 模式 | 初始位置 | 移动方向 | 适用场景 |
|------|----------|----------|----------|
| **对撞指针** | 两端 | 相向而行 | 有序数组、回文、盛水 |
| **快慢指针** | 同起点 | 同向不同速 | 链表环、去重、移零 |
| **同向指针** | 同起点 | 同向不同步 | 滑动窗口、归并 |

## 1.3 使用前提

```
能用双指针的关键条件：
├─ 数组已排序（对撞指针利用单调性）
├─ 数据有某种单调/递进关系
├─ 问题要求原地操作（快慢指针）
├─ 链表结构（快慢指针找环/中点）
└─ 子数组/子串问题（同向指针 → 滑动窗口）
```

---

# 2. 对撞指针（左右指针）

> 🎯 **最高频的双指针模式。**

## 2.1 模板

```cpp
// 对撞指针通用模板
int left = 0, right = n - 1;
while (left < right) {
    if (满足条件) {
        记录结果;
        left++;   // 或 right--;
    } else if (需要向右移动) {
        left++;
    } else {
        right--;
    }
}
```

## 2.2 核心原理：为什么有效？

```
以有序数组的两数之和为例：
sorted = [1, 2, 4, 6, 8, 10], target = 10

left=0, right=5: 1+10=11 > 10 → right-- (缩小和)
left=0, right=4: 1+8=9  < 10 → left++  (增大和)
left=1, right=4: 2+8=10 = 10 → 找到!

关键洞察：
- 数组已排序 → sum 具有单调性
- left++  → sum 变大
- right-- → sum 变小
- 每一步都可以排除一行/一列的可能性
- O(n²) 的枚举空间只需 O(n) 遍历
```

## 2.3 经典应用：有序数组两数之和

```cpp
// LC 167: 两数之和 II（输入有序数组）
vector<int> twoSum(vector<int>& numbers, int target) {
    int left = 0, right = numbers.size() - 1;
    while (left < right) {
        int sum = numbers[left] + numbers[right];
        if (sum == target) {
            return {left + 1, right + 1};  // 题目要求 1-indexed
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }
    return {};
}
```

## 2.4 回文判断

```cpp
// LC 125: 验证回文串
// 忽略非字母数字字符，不区分大小写
bool isPalindrome(string s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        // 跳过非字母数字
        while (left < right && !isalnum(s[left])) left++;
        while (left < right && !isalnum(s[right])) right--;

        if (tolower(s[left]) != tolower(s[right]))
            return false;
        left++;
        right--;
    }
    return true;
}

// LC 680: 验证回文串 II（最多删除一个字符）
bool validPalindrome(string s) {
    auto check = [&](int l, int r) {
        while (l < r) {
            if (s[l] != s[r]) return false;
            l++; r--;
        }
        return true;
    };

    int left = 0, right = s.size() - 1;
    while (left < right) {
        if (s[left] != s[right]) {
            // 尝试删左边或删右边
            return check(left + 1, right) || check(left, right - 1);
        }
        left++; right--;
    }
    return true;
}
```

## 2.5 反转系列

```cpp
// LC 344: 反转字符串
void reverseString(vector<char>& s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        swap(s[left], s[right]);
        left++;
        right--;
    }
}

// LC 541: 反转字符串 II
// 每 2k 个字符中反转前 k 个
string reverseStr(string s, int k) {
    for (int i = 0; i < (int)s.size(); i += 2 * k) {
        int left = i, right = min(i + k - 1, (int)s.size() - 1);
        while (left < right) {
            swap(s[left++], s[right--]);
        }
    }
    return s;
}
```

---

# 3. 快慢指针

## 3.1 模板

```cpp
// 快慢指针通用模板
int slow = 0;
for (int fast = 0; fast < n; fast++) {
    if (满足条件(nums[fast])) {
        // slow 位置写入有效值
        nums[slow] = nums[fast];
        slow++;
    }
}
// slow 就是处理后的有效长度
```

## 3.2 数组去重

```cpp
// LC 26: 删除有序数组中的重复项
// [1,1,2] → [1,2,_]  返回 2
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;
    for (int fast = 1; fast < (int)nums.size(); fast++) {
        if (nums[fast] != nums[slow]) {
            slow++;
            nums[slow] = nums[fast];
        }
    }
    return slow + 1;
}

// LC 80: 删除有序数组中的重复项 II（最多保留两个）
int removeDuplicatesII(vector<int>& nums) {
    if (nums.size() <= 2) return nums.size();
    int slow = 2;
    for (int fast = 2; fast < (int)nums.size(); fast++) {
        if (nums[fast] != nums[slow - 2]) {
            nums[slow] = nums[fast];
            slow++;
        }
    }
    return slow;
}
```

## 3.3 移动元素

```cpp
// LC 283: 移动零
// 把所有 0 移到数组末尾，保持非零元素相对顺序
void moveZeroes(vector<int>& nums) {
    int slow = 0;
    for (int fast = 0; fast < (int)nums.size(); fast++) {
        if (nums[fast] != 0) {
            swap(nums[slow], nums[fast]);
            slow++;
        }
    }
}

// LC 27: 移除元素
// 移除所有值等于 val 的元素
int removeElement(vector<int>& nums, int val) {
    int slow = 0;
    for (int fast = 0; fast < (int)nums.size(); fast++) {
        if (nums[fast] != val) {
            nums[slow] = nums[fast];
            slow++;
        }
    }
    return slow;
}
```

## 3.4 链表的快慢指针

```cpp
// LC 141: 环形链表（判断是否有环）
bool hasCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;  // 相遇 = 有环
    }
    return false;
}

// LC 142: 环形链表 II（找入环节点）
ListNode* detectCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            // 相遇后，一个从 head 出发，一个从相遇点出发
            // 再次相遇就是入环点
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
        slow = slow->next;        // 走 1 步
        fast = fast->next->next;  // 走 2 步
    }
    return slow;  // 快指针到末尾时，慢指针在中间
}

// LC 19: 删除链表倒数第 N 个节点
// 快指针先走 n 步，然后快慢同步走
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode *slow = &dummy, *fast = &dummy;

    // fast 先走 n+1 步
    for (int i = 0; i <= n; i++)
        fast = fast->next;

    // 同步走
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    // slow->next 就是要删的节点
    ListNode* toDelete = slow->next;
    slow->next = toDelete->next;
    delete toDelete;

    return dummy.next;
}
```

### 快慢指针数学原理（环检测）

```
为什么快慢指针能检测环？

假设环前长度 a，环长 c，慢在入环后走了 b 步相遇：
  slow 走了: a + b
  fast 走了: a + b + k*c  (多走了 k 圈)

fast = 2 * slow → a + b + k*c = 2(a+b)
→ k*c = a + b
→ a = k*c - b

从相遇点再走 a 步 = 走 k*c - b 步 = 绕回到入环点！
所以从 head 和相遇点同时走，再次相遇就是入环点。
```

---

# 4. 同向双指针

> 同向双指针是滑动窗口的基础，但比滑动窗口更通用。

## 4.1 模板

```cpp
int left = 0;
for (int right = 0; right < n; right++) {
    // 扩展右边界
    更新窗口状态;

    // 收缩左边界
    while (需要收缩) {
        更新答案;
        left++;
    }
}
```

## 4.2 合并两个有序数组

```cpp
// LC 88: 合并两个有序数组
// 从后往前填充，避免覆盖
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    int p1 = m - 1, p2 = n - 1, p = m + n - 1;
    while (p1 >= 0 && p2 >= 0) {
        if (nums1[p1] > nums2[p2])
            nums1[p--] = nums1[p1--];
        else
            nums1[p--] = nums2[p2--];
    }
    while (p2 >= 0) {
        nums1[p--] = nums2[p2--];
    }
}
```

## 4.3 排序颜色

```cpp
// LC 75: 颜色分类（荷兰国旗问题）
// 三指针：lo, mid, hi
void sortColors(vector<int>& nums) {
    int lo = 0, mid = 0, hi = nums.size() - 1;
    while (mid <= hi) {
        if (nums[mid] == 0) {
            swap(nums[lo], nums[mid]);
            lo++; mid++;
        } else if (nums[mid] == 1) {
            mid++;
        } else {  // nums[mid] == 2
            swap(nums[mid], nums[hi]);
            hi--;
            // 注意：mid 不动！交换过来的可能是 0
        }
    }
}
```

## 4.4 接雨水（双指针解法）

```cpp
// LC 42: 接雨水
// 双指针解法：O(1) 空间
int trap(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int leftMax = 0, rightMax = 0;
    int water = 0;

    while (left < right) {
        if (height[left] < height[right]) {
            leftMax = max(leftMax, height[left]);
            water += leftMax - height[left];
            left++;
        } else {
            rightMax = max(rightMax, height[right]);
            water += rightMax - height[right];
            right--;
        }
    }
    return water;
}
// 原理：
// 某个位置能接的水 = min(左边最高, 右边最高) - 当前高度
// 如果 height[left] < height[right]:
//   左边最高 ≤ 右边最高（因为 right 那边更高）
//   所以水量由 leftMax 决定
```

---

# 5. 两数之和系列

## 5.1 三种情况

| 变体 | 方法 | 时间 |
|------|------|------|
| 无序数组 | 哈希表 | O(n) |
| 有序数组 | 对撞指针 | O(n) |
| 数据流 | 排序+指针 / 哈希 | O(n) |

```cpp
// 无序 → 哈希表 (LC 1)
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> mp;
    for (int i = 0; i < (int)nums.size(); i++) {
        if (mp.count(target - nums[i]))
            return {mp[target - nums[i]], i};
        mp[nums[i]] = i;
    }
    return {};
}

// 有序 → 对撞指针 (LC 167)
vector<int> twoSumSorted(vector<int>& nums, int target) {
    int l = 0, r = nums.size() - 1;
    while (l < r) {
        int sum = nums[l] + nums[r];
        if (sum == target) return {l + 1, r + 1};
        else if (sum < target) l++;
        else r--;
    }
    return {};
}
```

---

# 6. 三数之和 / N 数之和

## 6.1 LC 15: 三数之和

```cpp
// 排序 + 固定一个 + 双指针找另外两个
// 时间 O(n²), 空间 O(1)
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;

    for (int i = 0; i < (int)nums.size() - 2; i++) {
        if (nums[i] > 0) break;   // 最小值 > 0，不可能凑 0
        if (i > 0 && nums[i] == nums[i-1]) continue;  // 跳过重复

        int left = i + 1, right = nums.size() - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                // 跳过重复
                while (left < right && nums[left] == nums[left+1]) left++;
                while (left < right && nums[right] == nums[right-1]) right--;
                left++; right--;
            } else if (sum < 0) {
                left++;
            } else {
                right--;
            }
        }
    }
    return result;
}
```

## 6.2 LC 18: 四数之和

```cpp
// 模式: 固定两个 + 双指针找另外两个
// 时间 O(n³)
vector<vector<int>> fourSum(vector<int>& nums, int target) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    int n = nums.size();

    for (int i = 0; i < n - 3; i++) {
        if (i > 0 && nums[i] == nums[i-1]) continue;

        for (int j = i + 1; j < n - 2; j++) {
            if (j > i + 1 && nums[j] == nums[j-1]) continue;

            int left = j + 1, right = n - 1;
            while (left < right) {
                long sum = (long)nums[i] + nums[j] + nums[left] + nums[right];
                if (sum == target) {
                    result.push_back({nums[i], nums[j], nums[left], nums[right]});
                    while (left < right && nums[left] == nums[left+1]) left++;
                    while (left < right && nums[right] == nums[right-1]) right--;
                    left++; right--;
                } else if (sum < target) {
                    left++;
                } else {
                    right--;
                }
            }
        }
    }
    return result;
}
```

## 6.3 N 数之和通用模板

```cpp
// 递归 + 双指针：N 数之和 → 2 数之和
vector<vector<int>> nSum(vector<int>& nums, int n, int start, long target) {
    vector<vector<int>> result;
    int sz = nums.size();
    if (n < 2 || sz < n) return result;

    if (n == 2) {
        // 双指针
        int lo = start, hi = sz - 1;
        while (lo < hi) {
            long sum = (long)nums[lo] + nums[hi];
            if (sum == target) {
                result.push_back({nums[lo], nums[hi]});
                while (lo < hi && nums[lo] == nums[lo+1]) lo++;
                while (lo < hi && nums[hi] == nums[hi-1]) hi--;
                lo++; hi--;
            } else if (sum < target) lo++;
            else hi--;
        }
    } else {
        for (int i = start; i < sz - n + 1; i++) {
            if (i > start && nums[i] == nums[i-1]) continue;
            auto sub = nSum(nums, n-1, i+1, target - nums[i]);
            for (auto& v : sub) {
                v.insert(v.begin(), nums[i]);
                result.push_back(v);
            }
        }
    }
    return result;
}
```

---

# 7. 盛水问题

## 7.1 LC 11: 盛最多水的容器

```cpp
int maxArea(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int maxWater = 0;

    while (left < right) {
        int w = right - left;
        int h = min(height[left], height[right]);
        maxWater = max(maxWater, w * h);

        // 移动较矮的那一边（只有这样才可能变大）
        if (height[left] < height[right])
            left++;
        else
            right--;
    }
    return maxWater;
}

// 为什么移动较矮的？
// 面积 = min(h[l], h[r]) * (r - l)
// 宽度在缩小，只有高度增加才可能面积增大
// 移动较高的 → min 不变或变小 → 面积一定变小
// 移动较矮的 → min 可能变大 → 面积有可能变大
```

## 7.2 LC 42: 接雨水（多种解法对比）

```
解法        时间    空间    思路
前后缀     O(n)    O(n)    预计算 leftMax[], rightMax[]
单调栈     O(n)    O(n)    横着算水，用递减栈
双指针     O(n)    O(1)    左右逼近，最优
```

```cpp
// 双指针解法（最优）— 已在第 4 节给出
// 前后缀解法（更好理解）
int trap_prefix(vector<int>& height) {
    int n = height.size();
    vector<int> leftMax(n), rightMax(n);

    leftMax[0] = height[0];
    for (int i = 1; i < n; i++)
        leftMax[i] = max(leftMax[i-1], height[i]);

    rightMax[n-1] = height[n-1];
    for (int i = n-2; i >= 0; i--)
        rightMax[i] = max(rightMax[i+1], height[i]);

    int water = 0;
    for (int i = 0; i < n; i++)
        water += min(leftMax[i], rightMax[i]) - height[i];

    return water;
}
```

---

# 8. 数组原地操作

> 快慢指针的核心场景：不使用额外空间，原地修改数组。

## 8.1 题目清单

| 题号 | 题目 | 技巧 |
|------|------|------|
| 26 | 删除有序数组重复项 | 快慢指针 |
| 27 | 移除元素 | 快慢指针 |
| 80 | 删除重复项 II | 快慢指针 |
| 283 | 移动零 | 快慢指针+swap |
| 75 | 颜色分类 | 三指针 |
| 88 | 合并两个有序数组 | 双指针从后往前 |

## 8.2 通用思路

```
快指针 fast：遍历所有元素
慢指针 slow：标记有效位置

for (fast = 0; fast < n; fast++) {
    if (nums[fast] 满足保留条件) {
        nums[slow++] = nums[fast];
    }
}
return slow;  // 有效长度
```

---

# 9. 链表双指针

## 9.1 题目清单

| 题号 | 题目 | 技巧 |
|------|------|------|
| 141 | 环形链表 | 快慢指针 |
| 142 | 环形链表 II | 快慢+数学 |
| 876 | 链表的中间结点 | 快慢指针 |
| 19 | 删除倒数第 N 个 | 先行 n 步 |
| 160 | 相交链表 | 两指针交替走 |
| 21 | 合并两个有序链表 | 双指针归并 |
| 86 | 分隔链表 | 双链表拼接 |
| 234 | 回文链表 | 快慢+反转 |

## 9.2 相交链表

```cpp
// LC 160: 找两个链表的交点
// 思路：两个指针分别走 A→B 和 B→A，一定会同时到达交点
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    ListNode *pA = headA, *pB = headB;
    while (pA != pB) {
        pA = pA ? pA->next : headB;
        pB = pB ? pB->next : headA;
    }
    return pA;
}

// 数学证明:
// A 独有部分长 a, B 独有部分长 b, 公共部分长 c
// pA 走: a + c + b
// pB 走: b + c + a
// 长度相同！一定同时到达交点（或同时到 null）
```

## 9.3 回文链表

```cpp
// LC 234: 回文链表
// 快慢指针找中点 → 反转后半段 → 对比
bool isPalindromeList(ListNode* head) {
    // 1. 找中点
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 反转后半段
    ListNode* prev = nullptr;
    while (slow) {
        ListNode* next = slow->next;
        slow->next = prev;
        prev = slow;
        slow = next;
    }

    // 3. 对比前半段和反转后的后半段
    ListNode *p1 = head, *p2 = prev;
    while (p2) {
        if (p1->val != p2->val) return false;
        p1 = p1->next;
        p2 = p2->next;
    }
    return true;
}
```

---

# 10. 字符串双指针

## 10.1 LC 345: 反转字符串中的元音字母

```cpp
string reverseVowels(string s) {
    auto isVowel = [](char c) {
        c = tolower(c);
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
    };

    int left = 0, right = s.size() - 1;
    while (left < right) {
        while (left < right && !isVowel(s[left])) left++;
        while (left < right && !isVowel(s[right])) right--;
        swap(s[left], s[right]);
        left++; right--;
    }
    return s;
}
```

## 10.2 LC 392: 判断子序列

```cpp
// 判断 s 是否为 t 的子序列
bool isSubsequence(string s, string t) {
    int i = 0, j = 0;
    while (i < (int)s.size() && j < (int)t.size()) {
        if (s[i] == t[j]) i++;
        j++;
    }
    return i == (int)s.size();
}
```

## 10.3 LC 443: 压缩字符串

```cpp
int compress(vector<char>& chars) {
    int write = 0, read = 0;
    while (read < (int)chars.size()) {
        char ch = chars[read];
        int count = 0;
        while (read < (int)chars.size() && chars[read] == ch) {
            read++;
            count++;
        }
        chars[write++] = ch;
        if (count > 1) {
            string cnt = to_string(count);
            for (char c : cnt) chars[write++] = c;
        }
    }
    return write;
}
```

---

# 11. 经典 LeetCode 双指针题精讲

## 必做题清单

| 题号 | 题目 | 难度 | 指针类型 | 频率 |
|------|------|------|----------|------|
| 167 | 两数之和 II | Medium | 对撞 | ⭐⭐⭐⭐⭐ |
| 15 | 三数之和 | Medium | 对撞 | ⭐⭐⭐⭐⭐ |
| 18 | 四数之和 | Medium | 对撞 | ⭐⭐⭐⭐ |
| 11 | 盛最多水的容器 | Medium | 对撞 | ⭐⭐⭐⭐⭐ |
| 42 | 接雨水 | Hard | 对撞 | ⭐⭐⭐⭐⭐ |
| 125 | 验证回文串 | Easy | 对撞 | ⭐⭐⭐⭐ |
| 680 | 验证回文串 II | Easy | 对撞 | ⭐⭐⭐⭐ |
| 344 | 反转字符串 | Easy | 对撞 | ⭐⭐⭐ |
| 26 | 删除排序数组重复项 | Easy | 快慢 | ⭐⭐⭐⭐ |
| 27 | 移除元素 | Easy | 快慢 | ⭐⭐⭐ |
| 283 | 移动零 | Easy | 快慢 | ⭐⭐⭐⭐⭐ |
| 75 | 颜色分类 | Medium | 三指针 | ⭐⭐⭐⭐ |
| 88 | 合并有序数组 | Easy | 同向 | ⭐⭐⭐⭐ |
| 141 | 环形链表 | Easy | 快慢 | ⭐⭐⭐⭐⭐ |
| 142 | 环形链表 II | Medium | 快慢 | ⭐⭐⭐⭐⭐ |
| 876 | 链表的中间结点 | Easy | 快慢 | ⭐⭐⭐⭐ |
| 19 | 删除倒数第N个 | Medium | 先行指针 | ⭐⭐⭐⭐⭐ |
| 160 | 相交链表 | Easy | 双指针交替 | ⭐⭐⭐⭐⭐ |
| 234 | 回文链表 | Easy | 快慢+反转 | ⭐⭐⭐⭐ |
| 392 | 判断子序列 | Easy | 同向 | ⭐⭐⭐ |

## 解题思路速查

```
看到双指针问题 → 先分类：
├─ 有序数组找和/差？ → 对撞指针
├─ 回文判断？ → 对撞指针
├─ 原地去重/移除？ → 快慢指针
├─ 链表找环/中点？ → 快慢指针
├─ 链表倒数第 N 个？ → 先行指针
├─ 链表相交？ → 交替遍历
├─ 合并有序？ → 同向双指针
├─ 水问题（盛水/接雨水）？ → 对撞指针
├─ 颜色分类？ → 三指针（荷兰国旗）
└─ 子序列判断？ → 同向指针
```

---

# 12. 常见陷阱与最佳实践

## 12.1 经典陷阱

### 陷阱 1：忘记去重

```cpp
// ❌ 三数之和不去重 → 结果有重复三元组
for (int i = 0; i < n - 2; i++) {
    // 没有跳过重复的 nums[i]
}

// ✅ 跳过重复
for (int i = 0; i < n - 2; i++) {
    if (i > 0 && nums[i] == nums[i-1]) continue;
    // ...
    while (left < right && nums[left] == nums[left+1]) left++;
    while (left < right && nums[right] == nums[right-1]) right--;
}
```

### 陷阱 2：整数溢出

```cpp
// ❌ 两数之和可能溢出 int
int sum = nums[i] + nums[j] + nums[k];

// ✅ 用 long
long sum = (long)nums[i] + nums[j] + nums[k];
```

### 陷阱 3：快慢指针初始位置

```cpp
// ❌ fast 从 0 开始（和 slow 重合）
// 去重时 slow 自己和自己比
int slow = 0;
for (int fast = 0; fast < n; fast++) {
    if (nums[fast] != nums[slow])  // 第一次比较：self == self
        nums[++slow] = nums[fast];
}

// ✅ 无问题，因为相等时不操作
// 但要注意最终返回 slow + 1
```

### 陷阱 4：指针越界

```cpp
// ❌ while 循环出去重时没检查边界
while (nums[left] == nums[left+1]) left++;  // left+1 可能越界！

// ✅ 加边界检查
while (left < right && nums[left] == nums[left+1]) left++;
```

### 陷阱 5：接雨水双指针方向

```cpp
// ❌ 搞反了移动方向
if (height[left] < height[right])
    right--;  // 应该是 left++！

// ✅ 移动较小的那一边
if (height[left] < height[right]) {
    leftMax = max(leftMax, height[left]);
    water += leftMax - height[left];
    left++;    // 移左边
} else {
    rightMax = max(rightMax, height[right]);
    water += rightMax - height[right];
    right--;   // 移右边
}
```

## 12.2 最佳实践

```
✅ 对撞指针要求数组有序（或有某种单调性）
✅ 快慢指针 slow 标记「有效区域」的边界
✅ N 数之和：排序 + 固定外层 + 双指针内层
✅ 链表快慢指针：fast 走 2 步，slow 走 1 步
✅ 去重三步：外层 continue + 内层 while 跳重复 + left++ right--
✅ 尽量用 long 避免溢出
✅ 指针移动前检查边界
```

## 12.3 双指针技巧速查卡

```
┌──────────────────────────────────────────────────────┐
│              双指针技巧速查                            │
├──────────────────────────────────────────────────────┤
│  对撞指针     → l=0, r=n-1, 相向而行                  │
│  快慢指针     → slow 标记有效位，fast 遍历              │
│  先行指针     → fast 先走 k 步 → 同步走                 │
│  交替指针     → 两链表交替走 → 相交点                    │
│  三指针       → 荷兰国旗：lo=0, mid=0, hi=n-1          │
│  关键前提     → 排序/有序/单调                          │
│  去重口诀     → 排序 + 跳重复 + 内层跳重复               │
│  链表环检测   → 快走2慢走1 → 相遇后双出发找入环          │
│  接雨水       → 移动较矮的那一边                        │
│  盛水容器     → 移动较矮的那一边                        │
└──────────────────────────────────────────────────────┘
```

---

*📝 下一步：配合 `knowlege_details_5_two_pointers.cpp` 运行示例代码*

*💪 双指针是面试最高频的技巧之一，三数之和 + 接雨水 + 环检测必须手写！*
