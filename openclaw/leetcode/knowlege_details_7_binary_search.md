# 专题七：二分查找（Binary Search）详解

> 作者：大胖超 😜
> 配套代码：`knowlege_details_7_binary_search.cpp`

---

## 目录

1. [核心思想](#1-核心思想)
2. [三种基础模板](#2-三种基础模板)
3. [模板对比与选择策略](#3-模板对比与选择策略)
4. [STL 二分函数](#4-stl-二分函数)
5. [有序数组查找](#5-有序数组查找)
6. [旋转排序数组](#6-旋转排序数组)
7. [山脉数组与峰值](#7-山脉数组与峰值)
8. [二分答案](#8-二分答案)
9. [矩阵二分](#9-矩阵二分)
10. [必刷题目清单](#10-必刷题目清单)
11. [常见陷阱与调试技巧](#11-常见陷阱与调试技巧)
12. [总结与面试要点](#12-总结与面试要点)

---

## 1. 核心思想

### 1.1 什么是二分查找？

二分查找的本质：在一个具有**单调性**（或更一般的**可二分性**）的搜索空间上，**每次排除一半**，将 O(n) 降为 O(log n)。

```
搜索空间:  [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
                        ↑ mid
           target > mid → 排除左半
           搜索 [6, 7, 8, 9, 10]
                     ↑ mid
           target < mid → 排除右半
           搜索 [6, 7]
                 ↑ mid = 答案!
```

### 1.2 能用二分的条件

不仅限于"有序数组"！只要满足：

> **存在一个判定函数 f(x)，使得搜索空间可以被分成两半：一半满足、一半不满足**

```
索引:   0  1  2  3  4  5  6  7  8
f(x):   F  F  F  T  T  T  T  T  T
                  ↑ 找第一个 T（左边界）
```

### 1.3 时间复杂度

| 方法 | 时间 | 空间 |
|------|------|------|
| 线性搜索 | O(n) | O(1) |
| **二分查找** | **O(log n)** | **O(1)** |

每次砍半：$n \to n/2 \to n/4 \to \cdots \to 1$，共 $\log_2 n$ 步。

### 1.4 二分最大的坑

> **边界条件**！！left/right 的初始值、循环条件、mid 的更新——差一个 1 就死循环或漏答案。

解决方法：**选一套模板，练到肌肉记忆，不要混用**。

---

## 2. 三种基础模板

### 2.1 模板一：标准二分（找精确值）

```cpp
// 闭区间 [left, right]
int binarySearch(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {                      // 注意 <=
        int mid = left + (right - left) / 2;     // 防溢出
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;  // 找不到
}
```

**要点**：
- 区间定义：`[left, right]` 闭区间
- 循环条件：`left <= right`（区间非空就继续）
- 更新：`left = mid + 1` 或 `right = mid - 1`（排除 mid）
- 终止时：`left == right + 1`，区间为空

### 2.2 模板二：左边界二分（第一个 ≥ target）

```cpp
// 左闭右开 [left, right)
// 等价于 C++ 的 lower_bound
int lowerBound(vector<int>& nums, int target) {
    int left = 0, right = nums.size();   // 注意 right = n
    while (left < right) {               // 注意 <
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) left = mid + 1;
        else right = mid;                // 注意不是 mid-1
    }
    return left;  // left == right，即第一个 >= target 的位置
}
```

**要点**：
- 区间定义：`[left, right)` 左闭右开
- 循环条件：`left < right`
- `nums[mid] < target`：mid 不可能是答案 → `left = mid + 1`
- `nums[mid] >= target`：mid 可能是答案 → `right = mid`（不排除 mid）
- 终止时：`left == right`

**变体**——找第一个 > target（upper_bound）：

```cpp
int upperBound(vector<int>& nums, int target) {
    int left = 0, right = nums.size();
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] <= target) left = mid + 1;  // <= 而非 <
        else right = mid;
    }
    return left;  // 第一个 > target 的位置
}
```

### 2.3 模板三：万能红蓝染色法

这是一种更通用的思维方式：

```
将搜索空间分成两段：
  蓝色(blue)：满足条件的一侧
  红色(red)：不满足条件的一侧

初始时 blue = -1, red = n (都在边界外)
每次 mid 要么涂蓝要么涂红，搜索空间持续缩小
最终 blue 和 red 相邻
```

```cpp
int binarySearchColor(vector<int>& nums, int target) {
    int blue = -1, red = nums.size();
    while (blue + 1 < red) {
        int mid = blue + (red - blue) / 2;
        if (nums[mid] < target)
            blue = mid;     // mid 涂蓝（太小）
        else
            red = mid;      // mid 涂红（够大）
    }
    // red = 第一个 >= target 的位置（等价 lower_bound）
    // blue = 最后一个 < target 的位置
    return red;
}
```

**优势**：不需要纠结 `<=` 还是 `<`、`mid-1` 还是 `mid`，永不死循环。

---

## 3. 模板对比与选择策略

### 3.1 三模板对比

| 特性 | 模板一 | 模板二 | 模板三（红蓝） |
|------|--------|--------|----------------|
| 区间定义 | `[l, r]` 闭 | `[l, r)` 半开 | `(blue, red)` 开 |
| 循环条件 | `l <= r` | `l < r` | `blue+1 < red` |
| 左边更新 | `l = mid+1` | `l = mid+1` | `blue = mid` |
| 右边更新 | `r = mid-1` | `r = mid` | `red = mid` |
| 用途 | 找精确值 | 找边界 | 通用 |
| 死循环风险 | 低 | 中（忘+1） | 无 |

### 3.2 选择建议

```
看到二分题 →
  ├─ 找精确值？→ 模板一
  ├─ 找第一个满足条件的？→ 模板二（lower_bound）
  ├─ 找最后一个满足条件的？→ 模板二变体（upper_bound - 1）
  └─ 不确定？→ 模板三（万能）
```

### 3.3 从 lower_bound 推导一切

| 需求 | 实现 |
|------|------|
| 第一个 == target | `lb = lower_bound(target); lb < n && nums[lb] == target ? lb : -1` |
| 最后一个 == target | `ub = upper_bound(target) - 1; ub >= 0 && nums[ub] == target ? ub : -1` |
| 第一个 > target | `upper_bound(target)` |
| 最后一个 < target | `lower_bound(target) - 1` |
| target 出现次数 | `upper_bound(target) - lower_bound(target)` |

---

## 4. STL 二分函数

C++ 标准库提供了现成的二分查找函数：

### 4.1 常用函数

```cpp
#include <algorithm>

vector<int> nums = {1, 2, 4, 4, 4, 7, 9};

// lower_bound: 第一个 >= target 的迭代器
auto it1 = lower_bound(nums.begin(), nums.end(), 4);
// → 指向下标 2（第一个 4）

// upper_bound: 第一个 > target 的迭代器
auto it2 = upper_bound(nums.begin(), nums.end(), 4);
// → 指向下标 5（第一个 7）

// 目标出现次数
int count = upper_bound(..., 4) - lower_bound(..., 4);  // 3

// binary_search: 是否存在
bool found = binary_search(nums.begin(), nums.end(), 4);  // true

// equal_range: 同时返回 lower_bound 和 upper_bound
auto [lo, hi] = equal_range(nums.begin(), nums.end(), 4);
// lo → 下标 2, hi → 下标 5
```

### 4.2 自定义比较器

```cpp
// 降序数组中使用
vector<int> desc = {9, 7, 4, 4, 4, 2, 1};
auto it = lower_bound(desc.begin(), desc.end(), 4, greater<int>());

// 对结构体二分
struct Event { int time; string name; };
vector<Event> events = {{1,"a"}, {3,"b"}, {5,"c"}};
auto it = lower_bound(events.begin(), events.end(), 3,
    [](const Event& e, int t) { return e.time < t; });
```

### 4.3 何时手写 vs 用 STL？

| 场景 | 建议 |
|------|------|
| 标准有序数组 | STL `lower_bound` / `upper_bound` |
| 旋转数组、山脉数组 | **手写**（有特殊判断逻辑） |
| 二分答案 | **手写**（判定函数自定义） |
| 面试 | 面试官通常期望你**手写** |

---

## 5. 有序数组查找

### 5.1 LC 704: 二分查找（入门）

```cpp
int search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
```

### 5.2 LC 34: 在排序数组中查找元素的第一个和最后一个位置

```cpp
vector<int> searchRange(vector<int>& nums, int target) {
    int first = lowerBound(nums, target);
    // 检查是否真的找到了 target
    if (first == nums.size() || nums[first] != target)
        return {-1, -1};
    int last = upperBound(nums, target) - 1;
    return {first, last};
}
// 输入: [5,7,7,8,8,10], target=8 → [3, 4]
// 输入: [5,7,7,8,8,10], target=6 → [-1, -1]
```

### 5.3 LC 35: 搜索插入位置

```cpp
int searchInsert(vector<int>& nums, int target) {
    // 就是 lower_bound！
    return lower_bound(nums.begin(), nums.end(), target) - nums.begin();
}
// 输入: [1,3,5,6], target=5 → 2
// 输入: [1,3,5,6], target=2 → 1
// 输入: [1,3,5,6], target=7 → 4
```

---

## 6. 旋转排序数组

### 6.1 思维关键

旋转后的数组被分成**两段有序**部分：

```
原数组: [1, 2, 3, 4, 5, 6, 7]
旋转后: [4, 5, 6, 7, 1, 2, 3]
         ←有序→  ↑  ←有序→
               旋转点
```

关键判断：**mid 在左段还是右段** → `nums[left] <= nums[mid]` 则 mid 在左段。

### 6.2 LC 33: 搜索旋转排序数组

```cpp
int search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        
        if (nums[left] <= nums[mid]) {
            // 左半段 [left, mid] 有序
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;  // target 在左半段
            else
                left = mid + 1;   // target 在右半段
        } else {
            // 右半段 [mid, right] 有序
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;   // target 在右半段
            else
                right = mid - 1;  // target 在左半段
        }
    }
    return -1;
}
```

### 6.3 LC 81: 搜索旋转排序数组 II（有重复）

与 LC 33 唯一区别：有重复元素时 `nums[left] == nums[mid]` 无法判断在哪段。

```cpp
bool search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return true;
        
        // 关键：去重！
        if (nums[left] == nums[mid]) {
            left++;
            continue;
        }
        
        if (nums[left] <= nums[mid]) {
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;
            else
                left = mid + 1;
        } else {
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }
    return false;
}
```

> 最坏 O(n)（全部相同元素），但平均仍然 O(log n)。

### 6.4 LC 153: 寻找旋转排序数组中的最小值

```cpp
int findMin(vector<int>& nums) {
    int left = 0, right = nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] > nums[right])
            left = mid + 1;    // 最小值在右半段
        else
            right = mid;       // 最小值在左半段（含 mid）
    }
    return nums[left];
}
// [4,5,6,7,0,1,2] → 0
// [3,4,5,1,2] → 1
```

---

## 7. 山脉数组与峰值

### 7.1 LC 852: 山脉数组的峰顶索引

```cpp
int peakIndexInMountainArray(vector<int>& arr) {
    int left = 0, right = arr.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] < arr[mid + 1])
            left = mid + 1;   // 上坡 → 峰在右边
        else
            right = mid;      // 下坡或峰顶 → 收缩右边界
    }
    return left;
}
```

### 7.2 LC 162: 寻找峰值

与山脉数组类似，但可能有多个峰。

```cpp
int findPeakElement(vector<int>& nums) {
    int left = 0, right = nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] < nums[mid + 1])
            left = mid + 1;   // 右邻居更大 → 右边一定有峰
        else
            right = mid;      // 左邻居更大或已是峰 → 左边(含)有峰
    }
    return left;
}
```

**为什么这样是正确的**？
- 若 `nums[mid] < nums[mid+1]`：mid 右边一定有更高的值（或边界 -∞），往右走必有峰
- 否则：mid 本身可能是峰，或左边有峰

---

## 8. 二分答案

### 8.1 什么是"二分答案"？

当题目问"最大的最小值"或"最小的最大值"时，直接二分**答案空间**。

```
不是在数组上二分，而是在"答案的可能范围"上二分！

答案范围: [lo, hi]
对每个 mid（候选答案），用判定函数 check(mid) 验证可行性
→ 根据结果缩小范围
```

### 8.2 LC 875: 爱吃香蕉的珂珂

**题意**：珂珂以速度 k 吃香蕉，一次最多吃一堆，求在 h 小时内吃完所有香蕉的最小速度 k。

```cpp
int minEatingSpeed(vector<int>& piles, int h) {
    int left = 1;
    int right = *max_element(piles.begin(), piles.end());
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        
        // 判定函数：速度为 mid 时能否在 h 小时内吃完？
        int hours = 0;
        for (int p : piles)
            hours += (p + mid - 1) / mid;  // 向上取整 ceil(p/mid)
        
        if (hours <= h)
            right = mid;     // 能吃完 → 尝试更慢的速度
        else
            left = mid + 1;  // 吃不完 → 必须更快
    }
    return left;
}
// piles = [3,6,7,11], h = 8 → 4
// piles = [30,11,23,4,20], h = 5 → 30
```

### 8.3 LC 69: x 的平方根

```cpp
int mySqrt(int x) {
    if (x <= 1) return x;
    int left = 1, right = x / 2;  // 优化右边界
    
    while (left <= right) {
        long mid = left + (right - left) / 2;
        if (mid * mid <= x && (mid + 1) * (mid + 1) > x)
            return mid;
        else if (mid * mid > x)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return left;
}
```

更简洁的写法（用 upper_bound 思路）：

```cpp
int mySqrt(int x) {
    long left = 0, right = x;
    while (left < right) {
        long mid = left + (right - left + 1) / 2;  // 上取整防死循环
        if (mid <= x / mid)     // 用除法防溢出
            left = mid;
        else
            right = mid - 1;
    }
    return left;
}
```

### 8.4 LC 1011: 在 D 天内送达包裹的能力

```cpp
int shipWithinDays(vector<int>& weights, int days) {
    int left = *max_element(weights.begin(), weights.end());  // 至少能装最重的一个
    int right = accumulate(weights.begin(), weights.end(), 0); // 一天全装完
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        
        // check: 载重为 mid 时需要几天？
        int need = 1, curLoad = 0;
        for (int w : weights) {
            if (curLoad + w > mid) {
                need++;
                curLoad = 0;
            }
            curLoad += w;
        }
        
        if (need <= days)
            right = mid;       // 够了 → 尝试更小载重
        else
            left = mid + 1;    // 不够 → 增加载重
    }
    return left;
}
// weights = [1,2,3,4,5,6,7,8,9,10], days = 5 → 15
```

### 8.5 二分答案模板总结

```cpp
// "最小化最大值" 或 "最大化最小值" 的通用框架
int binarySearchAnswer(问题参数) {
    int left = 最小可能答案;
    int right = 最大可能答案;
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        
        if (check(mid))       // 判定函数：mid 是否可行？
            right = mid;      // 可行 → 尝试更小（最小化问题）
        else
            left = mid + 1;   // 不可行 → 增大
    }
    return left;
}
```

---

## 9. 矩阵二分

### 9.1 LC 74: 搜索二维矩阵

m×n 矩阵每行有序、每行首元素 > 上行末元素 → 整体看成一维有序数组。

```cpp
bool searchMatrix(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int left = 0, right = m * n - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int val = matrix[mid / n][mid % n];  // 一维下标 → 二维坐标
        
        if (val == target) return true;
        else if (val < target) left = mid + 1;
        else right = mid - 1;
    }
    return false;
}
```

### 9.2 LC 240: 搜索二维矩阵 II

每行每列有序，但不保证上下行的关系 → 用**右上角/左下角**出发的 O(m+n) 搜索更优。

```cpp
bool searchMatrix(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int row = 0, col = n - 1;  // 从右上角出发
    
    while (row < m && col >= 0) {
        if (matrix[row][col] == target) return true;
        else if (matrix[row][col] > target) col--;   // 太大 → 左移
        else row++;                                    // 太小 → 下移
    }
    return false;
}
```

> 严格来说这不是二分，而是"梯形搞法"，但常与二分一起考。

---

## 10. 必刷题目清单

### 基础（必做）

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 704 | 二分查找 | Easy | 标准 | 入门模板 |
| 35 | 搜索插入位置 | Easy | lower_bound | 第一个 ≥ target |
| 69 | x 的平方根 | Easy | 二分答案 | 防溢出 |
| 34 | 查找首末位置 | Medium | 边界 | lower_bound + upper_bound |
| 74 | 搜索二维矩阵 | Medium | 标准 | 一维映射 |

### 进阶（面试重点）

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 33 | 搜索旋转排序数组 | Medium | 旋转 | 判断 mid 在哪段 |
| 153 | 旋转数组最小值 | Medium | 旋转 | 与 right 比较 |
| 162 | 寻找峰值 | Medium | 峰值 | 邻居比较 |
| 240 | 搜索二维矩阵 II | Medium | 矩阵 | 右上角出发 |
| 875 | 珂珂吃香蕉 | Medium | 二分答案 | check 函数 |

### 高级（冲刺）

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 4 | 两个有序数组中位数 | Hard | 二分 | 经典 Hard |
| 81 | 旋转数组搜索 II | Medium | 旋转+重复 | 去重处理 |
| 287 | 寻找重复数 | Medium | 二分答案 | 抽屉原理 |
| 410 | 分割数组最大值 | Hard | 二分答案 | 最小化最大值 |
| 852 | 山脉数组峰顶 | Medium | 峰值 | 上下坡判断 |
| 1011 | D天送包裹 | Medium | 二分答案 | 最小化载重 |

### 推荐刷题顺序

```
704(入门) → 35(lower_bound) → 34(边界组合) → 69(二分答案入门)
→ 74(矩阵) → 33(旋转数组) → 153(旋转最小值) → 162/852(峰值)
→ 875/1011(二分答案经典) → 240(矩阵II) → 81(旋转+重复)
→ 287/410(Hard 冲刺) → 4(终极 Hard)
```

---

## 11. 常见陷阱与调试技巧

### 11.1 死循环排查

| 症状 | 原因 | 修正 |
|------|------|------|
| `left = mid` 导致死循环 | 当 left == mid 时 left 不变 | 用 `mid = left + (right - left + 1) / 2` 上取整 |
| `left <= right` 用了 `right = mid` | right 不减小 | 改用 `right = mid - 1` 或换 `left < right` 模板 |
| 混用模板 | 闭区间的条件用了开区间的更新 | 只用一套模板！ |

### 11.2 溢出问题

```cpp
// ❌ 可能溢出
int mid = (left + right) / 2;

// ✅ 防溢出标准写法
int mid = left + (right - left) / 2;

// ❌ mid*mid 可能溢出 int
if (mid * mid <= x)

// ✅ 用 long 或除法
long mid = ...;
if (mid <= x / mid)  // 注意 mid=0 要特判
```

### 11.3 off-by-one 检查法

```
用最小的例子验证：
  - 数组只有 1 个元素
  - target 在最左/最右
  - target 不在数组中
  - 旋转数组的极端情况（未旋转/全部相同）
```

### 11.4 调试输出模板

```cpp
while (left < right) {
    int mid = left + (right - left) / 2;
    cout << "left=" << left << " right=" << right 
         << " mid=" << mid << " nums[mid]=" << nums[mid] << endl;
    // ...
}
```

---

## 12. 总结与面试要点

### 核心公式

```
二分查找 = 单调搜索空间 + 每次排除一半 + O(log n)
```

### 面试表达模板

> "这道题的搜索空间具有单调性/可二分性——
> 左半段不满足条件，右半段满足条件（或反之）。
> 我用二分查找，每次判断 mid 在哪一侧，
> 将搜索空间缩小一半。时间 O(log n)，空间 O(1)。"

### 三大类题型速记

| 类型 | 特征 | 典型题 |
|------|------|--------|
| **有序查找** | 数组有序，找目标/边界 | LC 704, 34, 35 |
| **条件二分** | 旋转/峰值，判断走向 | LC 33, 153, 162 |
| **二分答案** | 最大最小值 → 二分候选答案 + check | LC 69, 875, 1011 |

### `mid` 计算速记

| 写法 | 偏向 | 用途 |
|------|------|------|
| `left + (right - left) / 2` | 偏左（下取整） | 默认 |
| `left + (right - left + 1) / 2` | 偏右（上取整） | 当 `left = mid` 时防死循环 |

### 边界条件速记卡

| 闭区间 `[l, r]` | 半开区间 `[l, r)` |
|------------------|---------------------|
| `right = n - 1` | `right = n` |
| `while (l <= r)` | `while (l < r)` |
| `r = mid - 1` | `r = mid` |
| 终止时 `l = r + 1` | 终止时 `l = r` |

---

> **下一步**：配合 `knowlege_details_7_binary_search.cpp` 运行所有 Demo，用不同 target 测试边界情况！
