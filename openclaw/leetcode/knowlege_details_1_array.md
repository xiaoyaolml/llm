# 📖 专题一：数组（Array）— 从零基础到精通

*作者：大胖超 😜 | C++ 数组完全教学*

---

## 目录

1. [什么是数组？](#1-什么是数组)
2. [C++ 原生数组](#2-c-原生数组)
3. [STL vector 动态数组](#3-stl-vector-动态数组)
4. [STL array 静态数组](#4-stl-array-静态数组)
5. [数组遍历的所有方式](#5-数组遍历的所有方式)
6. [数组常用操作](#6-数组常用操作)
7. [双指针技巧](#7-双指针技巧)
8. [前缀和技巧](#8-前缀和技巧)
9. [差分数组技巧](#9-差分数组技巧)
10. [滑动窗口技巧](#10-滑动窗口技巧)
11. [二分查找进阶](#11-二分查找进阶)
12. [矩阵（二维数组）](#12-矩阵二维数组)
13. [数组排序专题](#13-数组排序专题)
14. [数组去重与统计](#14-数组去重与统计)
15. [子数组问题汇总](#15-子数组问题汇总)
16. [LeetCode 经典题精讲](#16-leetcode-经典题精讲)
17. [常见陷阱与最佳实践](#17-常见陷阱与最佳实践)

---

# 1. 什么是数组？

## 1.1 直观理解

数组就像一排**编了号的储物柜**：
- 每个柜子大小相同（相同类型）
- 柜子编号从 0 开始（下标/索引）
- 柜子是连续排列的（连续内存）

```
下标:    0     1     2     3     4
      ┌─────┬─────┬─────┬─────┬─────┐
值:   │  10 │  20 │  30 │  40 │  50 │
      └─────┴─────┴─────┴─────┴─────┘
地址:  0x100 0x104 0x108 0x10C 0x110
       ↑ 每个 int 占 4 字节，地址连续
```

## 1.2 为什么数组很重要？

1. **内存连续** → CPU 缓存友好 → 速度快
2. **O(1) 随机访问** → 知道下标就能直接跳到对应位置
3. **所有高级数据结构的基础**（堆、哈希表、栈、队列底层都可以用数组）

## 1.3 数组 vs 链表

| 特性 | 数组 | 链表 |
|------|------|------|
| 内存 | 连续 | 不连续 |
| 随机访问 | O(1) ✅ | O(n) ❌ |
| 头部插入 | O(n) ❌ | O(1) ✅ |
| 尾部插入 | O(1) 摊销 | O(1) |
| 中间插入 | O(n) | O(1)（已知位置） |
| 缓存友好 | ✅ 非常好 | ❌ 较差 |

---

# 2. C++ 原生数组

## 2.1 声明与初始化

```cpp
// ========== 一维数组 ==========

// 声明（未初始化，值是垃圾！）
int arr[5];                    // ⚠️ 局部变量未初始化，值随机

// 声明 + 完整初始化
int arr1[5] = {1, 2, 3, 4, 5};

// 部分初始化（其余为 0）
int arr2[5] = {1, 2};          // → {1, 2, 0, 0, 0}

// 全部初始化为 0
int arr3[5] = {};              // → {0, 0, 0, 0, 0}
int arr4[5] = {0};             // → {0, 0, 0, 0, 0}

// 自动推导大小
int arr5[] = {1, 2, 3};        // 编译器推导大小为 3

// C++11 统一初始化
int arr6[]{10, 20, 30};        // 不需要 = 号


// ========== 二维数组 ==========

int matrix[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};

// 二维数组在内存中是按行连续存储的
// matrix[0][0], matrix[0][1], ..., matrix[0][3], matrix[1][0], ...
```

## 2.2 数组大小

```cpp
int arr[5] = {1, 2, 3, 4, 5};

// 方法 1：sizeof（仅在数组没退化为指针时有效）
int size1 = sizeof(arr) / sizeof(arr[0]);  // 20/4 = 5

// 方法 2：C++17 std::size
#include <iterator>
int size2 = std::size(arr);  // 5

// ⚠️ 数组传入函数后退化为指针，sizeof 失效！
void foo(int arr[]) {
    // sizeof(arr) 在这里是指针大小（8字节），不是数组大小！
}
```

## 2.3 原生数组的局限性

```
❌ 不知道自己的大小（传参后退化为指针）
❌ 不能直接赋值（arr1 = arr2 编译报错）
❌ 不能作为函数返回值
❌ 没有越界检查（越界是未定义行为，可能崩溃或不崩溃）
❌ 栈上数组大小必须编译期确定
```

**结论：C++ 中优先使用 `std::vector` 或 `std::array`，而非原生数组。**

---

# 3. STL vector 动态数组

## 3.1 基本用法（零基础必看）

```cpp
#include <vector>
using namespace std;

// ========== 创建 ==========
vector<int> v1;                // 空数组
vector<int> v2(5);             // 5 个元素，全为 0
vector<int> v3(5, 10);         // 5 个 10 → {10, 10, 10, 10, 10}
vector<int> v4 = {1, 2, 3};    // 初始化列表
vector<int> v5{1, 2, 3};       // C++11 统一初始化
vector<int> v6(v4);            // 拷贝 v4
vector<int> v7(v4.begin(), v4.begin() + 2);  // 取 v4 前两个元素

// 二维 vector
vector<vector<int>> matrix(3, vector<int>(4, 0));  // 3x4 矩阵，全 0
```

## 3.2 增删改查

```cpp
vector<int> v = {10, 20, 30};

// ========== 增 ==========
v.push_back(40);            // 尾部添加 → {10, 20, 30, 40}
v.emplace_back(50);         // 尾部原地构造（更高效）→ {10, 20, 30, 40, 50}
v.insert(v.begin() + 1, 15); // 在下标 1 处插入 15 → {10, 15, 20, 30, 40, 50}

// ========== 删 ==========
v.pop_back();               // 删除尾部 → {10, 15, 20, 30, 40}
v.erase(v.begin() + 1);     // 删除下标 1 → {10, 20, 30, 40}
v.erase(v.begin(), v.begin() + 2);  // 删除 [0, 2) → {30, 40}
v.clear();                  // 清空

// ========== 改 ==========
v = {10, 20, 30};
v[0] = 100;                 // 直接修改 → {100, 20, 30}
v.at(1) = 200;              // 带越界检查 → {100, 200, 30}

// ========== 查 ==========
int first = v.front();      // 第一个元素
int last = v.back();        // 最后一个元素
int elem = v[2];            // 下标访问（不检查越界）
int safe = v.at(2);         // 下标访问（越界抛 out_of_range 异常）
bool empty = v.empty();     // 是否为空
int sz = v.size();          // 元素个数
```

## 3.3 vector 的内存机制（进阶重要！）

```
vector 的三个关键成员：
                                       capacity
                     ←───────────────────────────────→
                     ┌───┬───┬───┬───┬───┬───┬───┬───┐
v.data() ──────────→ │ 1 │ 2 │ 3 │ 4 │ 5 │   │   │   │
                     └───┴───┴───┴───┴───┴───┴───┴───┘
                     ←─────────────────→
                           size = 5        capacity = 8
```

### size vs capacity

```cpp
vector<int> v;
cout << v.size();      // 0 —— 已使用的元素个数
cout << v.capacity();  // 0 —— 已分配的内存空间

v.push_back(1);
// size=1, capacity 可能=1

v.push_back(2);
// size=2, capacity 可能=2

v.push_back(3);
// size=3, capacity 可能=4（扩容了！通常翻倍）

// 扩容过程：
// 1. 分配新的更大的内存（通常是当前 capacity 的 2 倍）
// 2. 把旧数据拷贝到新内存
// 3. 释放旧内存
// ⚠️ 扩容代价很高！如果提前知道大小，用 reserve 预分配

v.reserve(100);        // 预分配 100 个元素的空间（size 不变）
v.resize(50);          // 改变 size 为 50（多出的用0填充）
v.shrink_to_fit();     // 释放多余的内存，使 capacity = size
```

### 扩容策略详解

```
push_back 过程（假设增长因子 = 2）：

操作序列          size  capacity  是否扩容
───────────────────────────────────────────
初始               0      0        —
push_back(1)       1      1        是 (0→1)
push_back(2)       2      2        是 (1→2)
push_back(3)       3      4        是 (2→4)
push_back(4)       4      4        否
push_back(5)       5      8        是 (4→8)
push_back(6)       6      8        否
push_back(7)       7      8        否
push_back(8)       8      8        否
push_back(9)       9      16       是 (8→16)
```

**性能提示**：
- `push_back` 的**摊销时间复杂度**是 O(1)
- 如果知道大致大小，先 `reserve` 再 `push_back`，避免多次扩容

## 3.4 常用算法配合

```cpp
#include <algorithm>
#include <numeric>

vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

// 排序
sort(v.begin(), v.end());                    // 升序排序
sort(v.begin(), v.end(), greater<int>());    // 降序排序
// 自定义排序
sort(v.begin(), v.end(), [](int a, int b) { return a > b; });

// 查找
auto it = find(v.begin(), v.end(), 5);       // 线性查找
if (it != v.end()) cout << "找到了: " << *it;

// 二分查找（前提：已排序）
bool found = binary_search(v.begin(), v.end(), 5);
auto lb = lower_bound(v.begin(), v.end(), 5); // 第一个 >= 5 的位置
auto ub = upper_bound(v.begin(), v.end(), 5); // 第一个 > 5 的位置

// 最值
int maxVal = *max_element(v.begin(), v.end());
int minVal = *min_element(v.begin(), v.end());
auto [minIt, maxIt] = minmax_element(v.begin(), v.end()); // C++17

// 求和
int sum = accumulate(v.begin(), v.end(), 0);

// 去重（前提：已排序）
auto last = unique(v.begin(), v.end());
v.erase(last, v.end());

// 翻转
reverse(v.begin(), v.end());

// 填充
fill(v.begin(), v.end(), 0);     // 全部置 0
iota(v.begin(), v.end(), 1);     // 填充 1, 2, 3, ..., n

// 计数
int cnt = count(v.begin(), v.end(), 1);      // 值为 1 的个数
int cnt2 = count_if(v.begin(), v.end(), [](int x) { return x > 3; });

// 拷贝
vector<int> dest(v.size());
copy(v.begin(), v.end(), dest.begin());

// 变换
vector<int> result(v.size());
transform(v.begin(), v.end(), result.begin(), [](int x) { return x * 2; });

// 判断
bool allPositive = all_of(v.begin(), v.end(), [](int x) { return x > 0; });
bool anyNeg = any_of(v.begin(), v.end(), [](int x) { return x < 0; });
bool noneZero = none_of(v.begin(), v.end(), [](int x) { return x == 0; });
```

---

# 4. STL array 静态数组

## 4.1 什么时候用 std::array？

当数组大小**编译期确定**且不需要动态增长时，`std::array` 比 vector 更高效（无堆分配）。

```cpp
#include <array>

array<int, 5> arr = {1, 2, 3, 4, 5};

// 基本操作和 vector 类似
arr[0] = 10;
arr.at(0) = 10;        // 带越界检查
arr.front();            // 第一个
arr.back();             // 最后一个
arr.size();             // 5（编译期常量）
arr.empty();            // false
arr.fill(0);            // 全填 0

// 与原生数组的区别
// ✅ 知道自己的大小
// ✅ 可以用 = 赋值
// ✅ 可以作为函数参数不退化
// ✅ 支持迭代器和 STL 算法
```

---

# 5. 数组遍历的所有方式

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// ========== 方式 1：下标遍历（最基础） ==========
for (int i = 0; i < v.size(); i++) {
    cout << v[i] << " ";
}

// ⚠️ 注意 size() 返回的是 size_t（无符号整数）
// 如果 v 为空，v.size() - 1 会下溢变成超大正数！
// 安全写法：
for (int i = 0; i < (int)v.size(); i++) { ... }

// ========== 方式 2：范围 for 循环（C++11，推荐日常使用） ==========
for (int x : v) {           // 值拷贝，修改 x 不影响原数组
    cout << x << " ";
}

for (int& x : v) {          // 引用，可以修改原数组
    x *= 2;
}

for (const int& x : v) {    // 常量引用，不拷贝也不修改
    cout << x << " ";
}

// ========== 方式 3：迭代器 ==========
for (auto it = v.begin(); it != v.end(); ++it) {
    cout << *it << " ";
}

// 反向迭代
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    cout << *it << " ";
}

// ========== 方式 4：for_each 算法 ==========
#include <algorithm>
for_each(v.begin(), v.end(), [](int x) { cout << x << " "; });

// ========== 方式 5：带下标的范围 for（C++20 推荐方式未普及，可手动实现） ==========
for (int i = 0; auto& x : v) {  // C++20
    cout << "[" << i++ << "]=" << x << " ";
}
```

---

# 6. 数组常用操作

## 6.1 数组旋转

```cpp
// 方法 1：使用 STL rotate
// 将 [first, middle) 和 [middle, last) 交换
rotate(v.begin(), v.begin() + k, v.end());  // 左旋 k 位

// 方法 2：三次翻转法（面试经典）
// 右旋 k 位：先整体翻转，再翻转前 k 个，再翻转后 n-k 个
void rotateRight(vector<int>& nums, int k) {
    int n = nums.size();
    k %= n;  // 处理 k > n 的情况
    reverse(nums.begin(), nums.end());             // 整体翻转
    reverse(nums.begin(), nums.begin() + k);       // 前 k 个翻转
    reverse(nums.begin() + k, nums.end());         // 后 n-k 个翻转
}

// 图解（右旋 2 位）：
// 原始:      [1, 2, 3, 4, 5]
// 整体翻转:  [5, 4, 3, 2, 1]
// 前 2 翻转: [4, 5, 3, 2, 1]
// 后 3 翻转: [4, 5, 1, 2, 3]  ✓
```

## 6.2 数组合并

```cpp
// 合并两个有序数组（LC 88）
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    int i = m - 1, j = n - 1, k = m + n - 1;
    // 从后往前填，避免覆盖
    while (i >= 0 && j >= 0) {
        if (nums1[i] > nums2[j])
            nums1[k--] = nums1[i--];
        else
            nums1[k--] = nums2[j--];
    }
    while (j >= 0) nums1[k--] = nums2[j--];
}
```

## 6.3 原地删除元素

```cpp
// 原地删除所有等于 val 的元素（LC 27）
int removeElement(vector<int>& nums, int val) {
    int slow = 0;
    for (int fast = 0; fast < nums.size(); fast++) {
        if (nums[fast] != val) {
            nums[slow++] = nums[fast];
        }
    }
    return slow;  // 新长度
}
```

---

# 7. 双指针技巧

## 7.1 什么是双指针？

双指针是一种思想，不是具体的数据结构。用两个变量记录数组的位置，通过合理移动来降低时间复杂度。

### 三种类型

```
类型 1：对撞指针（两端向中间）
left →           ← right
[1, 2, 3, 4, 5, 6, 7]

类型 2：快慢指针（同向不同速）
slow →
    fast →→
[1, 1, 2, 2, 3, 4, 5]

类型 3：同向双指针（滑动窗口）
left →
       right →
[a, b, c, d, e, f, g]
```

## 7.2 对撞指针

```cpp
// 有序数组两数之和（LC 167）
vector<int> twoSum(vector<int>& numbers, int target) {
    int left = 0, right = numbers.size() - 1;
    while (left < right) {
        int sum = numbers[left] + numbers[right];
        if (sum == target)
            return {left + 1, right + 1};  // 题目 1-indexed
        else if (sum < target)
            left++;    // 和太小，增大左端
        else
            right--;   // 和太大，减小右端
    }
    return {};
}

// 判断回文（LC 125 简化版）
bool isPalindrome(string s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        if (s[left] != s[right]) return false;
        left++;
        right--;
    }
    return true;
}
```

## 7.3 快慢指针

```cpp
// 删除有序数组中的重复项（LC 26）
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;
    for (int fast = 1; fast < nums.size(); fast++) {
        if (nums[fast] != nums[slow]) {
            nums[++slow] = nums[fast];
        }
    }
    return slow + 1;
}

// 移动零（LC 283）
void moveZeroes(vector<int>& nums) {
    int slow = 0;
    for (int fast = 0; fast < nums.size(); fast++) {
        if (nums[fast] != 0) {
            swap(nums[slow++], nums[fast]);
        }
    }
}
```

## 7.4 三数之和（双指针综合运用）

```cpp
// LC 15: 三数之和
// 排序 + 固定一个数 + 对撞双指针
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    int n = nums.size();
    
    for (int i = 0; i < n - 2; i++) {
        // 剪枝：最小的数已经 > 0，不可能凑出 0
        if (nums[i] > 0) break;
        // 去重：跳过重复的 i
        if (i > 0 && nums[i] == nums[i - 1]) continue;
        
        int left = i + 1, right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                // 去重
                while (left < right && nums[left] == nums[left + 1]) left++;
                while (left < right && nums[right] == nums[right - 1]) right--;
                left++;
                right--;
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

---

# 8. 前缀和技巧

## 8.1 一维前缀和

### 原理

```
原数组:     nums = [2, 1, 5, 3, 4]
前缀和:   prefix = [0, 2, 3, 8, 11, 15]
                    ↑
               prefix[0] = 0（哨兵，简化计算）
               prefix[1] = nums[0] = 2
               prefix[2] = nums[0] + nums[1] = 3
               prefix[3] = nums[0] + nums[1] + nums[2] = 8
               ...

区间 [1, 3] 的和 = prefix[4] - prefix[1] = 11 - 2 = 9
验证: nums[1] + nums[2] + nums[3] = 1 + 5 + 3 = 9 ✓

公式: sum(i, j) = prefix[j+1] - prefix[i]
```

### 代码

```cpp
// 构建前缀和
vector<int> buildPrefixSum(vector<int>& nums) {
    int n = nums.size();
    vector<int> prefix(n + 1, 0);
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + nums[i];
    }
    return prefix;
}

// 查询区间 [l, r] 的和
int rangeSum(vector<int>& prefix, int l, int r) {
    return prefix[r + 1] - prefix[l];
}
```

## 8.2 前缀和 + 哈希表（高频考点）

```cpp
// LC 560: 和为 K 的子数组
// 核心思想：prefix[j] - prefix[i] == k
// 即对于当前前缀和 prefix[j]，查找之前有多少个 prefix[i] = prefix[j] - k
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> prefixCount;
    prefixCount[0] = 1;  // 前缀和为 0 出现 1 次（空前缀）
    int sum = 0, count = 0;
    
    for (int num : nums) {
        sum += num;
        if (prefixCount.count(sum - k))
            count += prefixCount[sum - k];
        prefixCount[sum]++;
    }
    return count;
}
```

## 8.3 二维前缀和

```
给定矩阵:
┌─────┬─────┬─────┐
│  1  │  2  │  3  │
├─────┼─────┼─────┤
│  4  │  5  │  6  │
├─────┼─────┼─────┤
│  7  │  8  │  9  │
└─────┴─────┴─────┘

prefix[i][j] = 从 (0,0) 到 (i-1,j-1) 的矩形区域和

构建公式:
prefix[i][j] = matrix[i-1][j-1]
             + prefix[i-1][j]
             + prefix[i][j-1]
             - prefix[i-1][j-1]

查询 (r1,c1) 到 (r2,c2) 的区域和:
sum = prefix[r2+1][c2+1]
    - prefix[r1][c2+1]
    - prefix[r2+1][c1]
    + prefix[r1][c1]
```

---

# 9. 差分数组技巧

## 9.1 原理

差分是前缀和的逆运算。对于区间 [l, r] 统一加 val，用差分数组只需两次操作：

```
原始:  [0, 0, 0, 0, 0, 0]
操作:  对 [1, 4] 全部 +3

差分:  [0, +3, 0, 0, 0, -3]
         ↑ d[l] += val      ↑ d[r+1] -= val

前缀和还原:
       [0, 3, 3, 3, 3, 0]
               ↑ 区间 [1,4] 全部 +3 ✓
```

## 9.2 代码模板

```cpp
class DiffArray {
    vector<int> diff;
public:
    DiffArray(int n) : diff(n + 1, 0) {}
    
    // 对 [l, r] 区间加 val
    void add(int l, int r, int val) {
        diff[l] += val;
        diff[r + 1] -= val;
    }
    
    // 还原结果数组
    vector<int> result() {
        int n = diff.size() - 1;
        vector<int> res(n);
        res[0] = diff[0];
        for (int i = 1; i < n; i++) {
            res[i] = res[i - 1] + diff[i];
        }
        return res;
    }
};

// LC 1094: 拼车
bool carPooling(vector<vector<int>>& trips, int capacity) {
    vector<int> diff(1001, 0);
    for (auto& t : trips) {
        diff[t[1]] += t[0];      // 上车
        diff[t[2]] -= t[0];      // 下车
    }
    int cur = 0;
    for (int d : diff) {
        cur += d;
        if (cur > capacity) return false;
    }
    return true;
}
```

---

# 10. 滑动窗口技巧

## 10.1 什么时候用？

看到以下关键词就考虑滑动窗口：
- "最长/最短子数组"
- "满足条件的连续子数组"
- "子串"
- "窗口"

## 10.2 万能模板

```cpp
// 滑动窗口万能模板
int slidingWindow(vector<int>& nums, int condition) {
    int left = 0, result = 0;
    // ... 窗口状态变量 ...
    
    for (int right = 0; right < nums.size(); right++) {
        // 1. 扩大窗口：加入 nums[right]
        // ... 更新窗口状态 ...
        
        // 2. 收缩窗口：直到窗口不满足条件
        while (/* 窗口需要收缩 */) {
            // ... 更新答案（如果求最短）...
            // 3. 缩小窗口：移除 nums[left]
            // ... 更新窗口状态 ...
            left++;
        }
        
        // 4. 更新答案（如果求最长）
        result = max(result, right - left + 1);
    }
    return result;
}
```

## 10.3 经典题解

```cpp
// LC 209: 长度最小的子数组
// 找满足和 >= target 的最短子数组
int minSubArrayLen(int target, vector<int>& nums) {
    int left = 0, sum = 0;
    int minLen = INT_MAX;
    
    for (int right = 0; right < nums.size(); right++) {
        sum += nums[right];  // 扩大窗口
        
        while (sum >= target) {
            minLen = min(minLen, right - left + 1);
            sum -= nums[left];  // 收缩窗口
            left++;
        }
    }
    return minLen == INT_MAX ? 0 : minLen;
}

// LC 904: 水果成篮（最多两种水果的最长子数组）
int totalFruit(vector<int>& fruits) {
    unordered_map<int, int> basket;  // 水果种类 → 数量
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < fruits.size(); right++) {
        basket[fruits[right]]++;
        
        while (basket.size() > 2) {
            basket[fruits[left]]--;
            if (basket[fruits[left]] == 0)
                basket.erase(fruits[left]);
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

---

# 11. 二分查找进阶

## 11.1 二分的本质

二分查找的本质不是"在有序数组中搜索"，而是：**在一个满足某种二分性质的空间中，缩小搜索范围**。

### 适用条件

只要满足：存在一个分界点，分界点左边满足某条件，右边不满足（或反过来），就可以二分。

## 11.2 二分答案

```cpp
// LC 875: 爱吃香蕉的珂珂
// 在答案空间 [1, max(piles)] 上二分
int minEatingSpeed(vector<int>& piles, int h) {
    int left = 1, right = *max_element(piles.begin(), piles.end());
    
    // 检查速度 k 能否在 h 小时内吃完
    auto canFinish = [&](int k) -> bool {
        long hours = 0;
        for (int p : piles)
            hours += (p + k - 1) / k;  // 向上取整
        return hours <= h;
    };
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (canFinish(mid))
            right = mid;    // mid 可以，但试试更小的
        else
            left = mid + 1; // mid 不行，必须更大
    }
    return left;
}
```

## 11.3 在旋转数组中搜索

```cpp
// LC 153: 寻找旋转排序数组中的最小值
int findMin(vector<int>& nums) {
    int left = 0, right = nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] > nums[right])
            left = mid + 1;  // 最小值在右半部分
        else
            right = mid;     // 最小值在左半部分（含 mid）
    }
    return nums[left];
}
```

---

# 12. 矩阵（二维数组）

## 12.1 矩阵遍历

```cpp
int m = matrix.size();    // 行数
int n = matrix[0].size(); // 列数

// 普通遍历
for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
        cout << matrix[i][j] << " ";

// 四方向遍历（上下左右邻居）
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};
for (int d = 0; d < 4; d++) {
    int nx = x + dx[d], ny = y + dy[d];
    if (nx >= 0 && nx < m && ny >= 0 && ny < n) {
        // 访问 matrix[nx][ny]
    }
}

// 八方向遍历（含对角线）
int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
```

## 12.2 矩阵操作

```cpp
// LC 48: 旋转图像（顺时针 90°）
// 技巧：转置 + 水平翻转
void rotate(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // 转置
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            swap(matrix[i][j], matrix[j][i]);
    // 水平翻转
    for (auto& row : matrix)
        reverse(row.begin(), row.end());
}

// LC 54: 螺旋矩阵
vector<int> spiralOrder(vector<vector<int>>& matrix) {
    vector<int> result;
    int top = 0, bottom = matrix.size() - 1;
    int left = 0, right = matrix[0].size() - 1;
    
    while (top <= bottom && left <= right) {
        for (int j = left; j <= right; j++) result.push_back(matrix[top][j]);
        top++;
        for (int i = top; i <= bottom; i++) result.push_back(matrix[i][right]);
        right--;
        if (top <= bottom) {
            for (int j = right; j >= left; j--) result.push_back(matrix[bottom][j]);
            bottom--;
        }
        if (left <= right) {
            for (int i = bottom; i >= top; i--) result.push_back(matrix[i][left]);
            left++;
        }
    }
    return result;
}

// LC 73: 矩阵置零
void setZeroes(vector<vector<int>>& matrix) {
    int m = matrix.size(), n = matrix[0].size();
    bool firstRowZero = false, firstColZero = false;
    
    // 检查第一行/列是否有零
    for (int j = 0; j < n; j++) if (matrix[0][j] == 0) firstRowZero = true;
    for (int i = 0; i < m; i++) if (matrix[i][0] == 0) firstColZero = true;
    
    // 用第一行/列标记
    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            if (matrix[i][j] == 0) {
                matrix[i][0] = 0;
                matrix[0][j] = 0;
            }
    
    // 根据标记置零
    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            if (matrix[i][0] == 0 || matrix[0][j] == 0)
                matrix[i][j] = 0;
    
    if (firstRowZero) for (int j = 0; j < n; j++) matrix[0][j] = 0;
    if (firstColZero) for (int i = 0; i < m; i++) matrix[i][0] = 0;
}
```

## 12.3 矩阵搜索

```cpp
// LC 240: 搜索二维矩阵 II
// 从右上角出发（每一步都能排除一行或一列）
bool searchMatrix(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int i = 0, j = n - 1;
    
    while (i < m && j >= 0) {
        if (matrix[i][j] == target) return true;
        else if (matrix[i][j] > target) j--;  // 排除当前列
        else i++;                               // 排除当前行
    }
    return false;
}
```

---

# 13. 数组排序专题

## 13.1 排序相关的题目技巧

```cpp
// LC 56: 合并区间
// 按起始点排序后，贪心合并
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

// LC 179: 最大数
// 自定义排序：比较 a+b 和 b+a
string largestNumber(vector<int>& nums) {
    vector<string> strs;
    for (int n : nums) strs.push_back(to_string(n));
    
    sort(strs.begin(), strs.end(), [](string& a, string& b) {
        return a + b > b + a;  // "30" vs "3": "303" < "330" → 3 在前
    });
    
    if (strs[0] == "0") return "0";
    string result;
    for (auto& s : strs) result += s;
    return result;
}
```

## 13.2 不需要完全排序的技巧

```cpp
// nth_element：找到第 k 大的元素，O(n) 平均
// 比完全排序 O(nlogn) 更快
nth_element(nums.begin(), nums.begin() + k, nums.end());
// nums.begin() + k 处现在是第 k+1 小的元素

// partial_sort：只排前 k 个
partial_sort(nums.begin(), nums.begin() + k, nums.end());
```

---

# 14. 数组去重与统计

```cpp
// 方法 1：排序 + unique（改变顺序）
sort(v.begin(), v.end());
v.erase(unique(v.begin(), v.end()), v.end());

// 方法 2：set 去重（不改变顺序，用 unordered_set 更快）
unordered_set<int> seen;
vector<int> result;
for (int x : v) {
    if (seen.insert(x).second)  // insert 返回是否插入成功
        result.push_back(x);
}

// 统计频率
unordered_map<int, int> freq;
for (int x : v) freq[x]++;
```

---

# 15. 子数组问题汇总

子数组问题是面试中非常高频的考点，核心方法有：

| 问题类型 | 方法 |
|---------|------|
| 最大/最小子数组和 | Kadane 算法 / 前缀和 |
| 和为 k 的子数组 | 前缀和 + 哈希表 |
| 满足条件的最长子数组 | 滑动窗口 |
| 满足条件的最短子数组 | 滑动窗口 |
| 子数组的最大积 | DP |
| 连续子数组计数 | 前缀和 + 哈希表 |

```cpp
// LC 53: 最大子数组和（Kadane 算法）
int maxSubArray(vector<int>& nums) {
    int maxSum = nums[0], curSum = nums[0];
    for (int i = 1; i < nums.size(); i++) {
        curSum = max(nums[i], curSum + nums[i]);
        maxSum = max(maxSum, curSum);
    }
    return maxSum;
}

// LC 152: 乘积最大子数组
// 因为负负得正，需要同时维护最大和最小
int maxProduct(vector<int>& nums) {
    int maxProd = nums[0], minProd = nums[0], result = nums[0];
    for (int i = 1; i < nums.size(); i++) {
        if (nums[i] < 0) swap(maxProd, minProd); // 负数翻转大小关系
        maxProd = max(nums[i], maxProd * nums[i]);
        minProd = min(nums[i], minProd * nums[i]);
        result = max(result, maxProd);
    }
    return result;
}
```

---

# 16. LeetCode 经典题精讲

## 必做经典题清单

### Easy（打基础）
| 题号 | 题目 | 核心考点 |
|------|------|---------|
| 1 | 两数之和 | 哈希表 |
| 26 | 删除重复项 | 快慢指针 |
| 27 | 移除元素 | 快慢指针 |
| 88 | 合并有序数组 | 从后往前合并 |
| 121 | 买卖股票 | 贪心/DP |
| 136 | 只出现一次的数 | 异或 |
| 169 | 多数元素 | 摩尔投票 |
| 217 | 存在重复元素 | 哈希集合 |
| 283 | 移动零 | 快慢指针 |
| 704 | 二分查找 | 标准二分 |

### Medium（提升能力）
| 题号 | 题目 | 核心考点 |
|------|------|---------|
| 11 | 盛最多水 | 对撞指针 |
| 15 | 三数之和 | 排序+双指针 |
| 33 | 搜索旋转数组 | 二分变体 |
| 48 | 旋转图像 | 矩阵操作 |
| 53 | 最大子数组和 | Kadane |
| 54 | 螺旋矩阵 | 模拟 |
| 56 | 合并区间 | 排序+贪心 |
| 238 | 除自身外乘积 | 前缀积 |
| 560 | 和为K的子数组 | 前缀和+哈希 |

### Hard（冲刺）
| 题号 | 题目 | 核心考点 |
|------|------|---------|
| 4 | 两个有序数组中位数 | 二分 |
| 42 | 接雨水 | 双指针/单调栈 |
| 239 | 滑动窗口最大值 | 单调队列 |

---

# 17. 常见陷阱与最佳实践

## 17.1 常见陷阱

```cpp
// ⚠️ 陷阱 1：整数溢出
int a = 100000, b = 100000;
int product = a * b;  // 溢出！用 long long
long long safe = (long long)a * b;

// ⚠️ 陷阱 2：size() 返回无符号数
vector<int> v;
if (v.size() - 1 >= 0) { }  // size()=0 时，0-1 下溢为超大正数！
// 正确写法：
if ((int)v.size() - 1 >= 0) { }
// 或：
if (v.size() >= 1) { }

// ⚠️ 陷阱 3：二分查找的 mid 溢出
int mid = (left + right) / 2;           // left+right 可能溢出
int mid = left + (right - left) / 2;     // ✅ 安全写法

// ⚠️ 陷阱 4：原地修改时的迭代器失效
vector<int> v = {1, 2, 3, 4, 5};
for (auto it = v.begin(); it != v.end(); ) {
    if (*it % 2 == 0)
        it = v.erase(it);  // erase 返回下一个有效迭代器
    else
        ++it;
}

// ⚠️ 陷阱 5：越界访问
v[v.size()];     // 越界！最大下标是 size()-1
v.at(v.size());  // 抛出 out_of_range 异常（更安全）
```

## 17.2 最佳实践

```cpp
// ✅ 优先用 vector 而非原生数组
// ✅ 提前 reserve 避免频繁扩容
// ✅ 用 emplace_back 代替 push_back（避免拷贝）
// ✅ 传递大 vector 用 const 引用
void process(const vector<int>& data);  // ✅ 不拷贝
void process(vector<int> data);          // ❌ 拷贝一份

// ✅ 返回 vector 不用担心拷贝（编译器会 RVO/移动语义）
vector<int> generate() {
    vector<int> result = {1, 2, 3};
    return result;  // 不会拷贝，直接移动或 RVO
}

// ✅ 初始化二维数组时注意维度顺序
vector<vector<int>> dp(m, vector<int>(n, 0));  // m 行 n 列

// ✅ for 循环中缓存 size()（虽然现代编译器通常会优化）
int n = v.size();
for (int i = 0; i < n; i++) { ... }
```

## 17.3 刷题时的数组代码模板

```cpp
#include <bits/stdc++.h>  // 竞赛/刷题万能头文件
using namespace std;

class Solution {
public:
    // 标准 LeetCode 函数签名
    vector<int> solve(vector<int>& nums) {
        int n = nums.size();
        vector<int> result;
        
        // 边界检查
        if (n == 0) return result;
        
        // ... 你的算法 ...
        
        return result;
    }
};
```

---

*最后更新：2026-03-02*
*数组是一切算法的基础，打牢基础再出发！💪*
