# 专题十三（下）：排序进阶应用 — 非比较排序与LeetCode实战

> 作者：大胖超 😜
> 
> 本篇覆盖非比较排序（计数、桶、基数排序）、自定义排序比较器、STL部分排序技术，以及大量排序相关的LeetCode高频题详解。

---

## 目录

1. [计数排序 Counting Sort](#1-计数排序-counting-sort)
2. [桶排序 Bucket Sort](#2-桶排序-bucket-sort)
3. [基数排序 Radix Sort](#3-基数排序-radix-sort)
4. [自定义排序与比较器](#4-自定义排序与比较器)
5. [STL 部分排序技术](#5-stl-部分排序技术)
6. [LeetCode 实战题解](#6-leetcode-实战题解)
7. [排序技巧总结与易错点](#7-排序技巧总结与易错点)

---

## 1. 计数排序 Counting Sort

### 1.1 原理

不基于比较，而是**统计每个值的出现次数**，然后还原有序序列。

### 1.2 适用条件

- 值域范围 k 不大（如 0~1000）
- 时间 O(n+k)，空间 O(k)
- **稳定排序**（正确实现时）

### 1.3 图解过程

```
输入: [4, 2, 2, 8, 3, 3, 1]
值域: 0~8

Step 1: 计数
  count[1]=1, count[2]=2, count[3]=2, count[4]=1, count[8]=1

Step 2: 前缀和（确定每个值的终止位置）
  count: [0, 1, 3, 5, 6, 6, 6, 6, 7]
  含义: 值≤1有1个, 值≤2有3个, 值≤3有5个, ...

Step 3: 反向遍历原数组（保证稳定）
  output[count[1]-1] = output[0] = 1, count[1]--
  output[count[3]-1] = output[4] = 3, count[3]--
  ...

输出: [1, 2, 2, 3, 3, 4, 8]
```

### 1.4 代码

```cpp
void countingSort(vector<int>& nums) {
    if (nums.empty()) return;
    int maxVal = *max_element(nums.begin(), nums.end());
    int minVal = *min_element(nums.begin(), nums.end());
    int range = maxVal - minVal + 1;
    
    vector<int> count(range, 0);
    vector<int> output(nums.size());
    
    // 计数
    for (int x : nums) count[x - minVal]++;
    
    // 前缀和
    for (int i = 1; i < range; i++)
        count[i] += count[i - 1];
    
    // 反向填充（保证稳定性）
    for (int i = nums.size() - 1; i >= 0; i--) {
        output[count[nums[i] - minVal] - 1] = nums[i];
        count[nums[i] - minVal]--;
    }
    
    nums = output;
}
```

### 1.5 简化版（不需要稳定时）

```cpp
void countingSortSimple(vector<int>& nums) {
    int maxVal = *max_element(nums.begin(), nums.end());
    vector<int> count(maxVal + 1, 0);
    for (int x : nums) count[x]++;
    int idx = 0;
    for (int i = 0; i <= maxVal; i++)
        while (count[i]-- > 0)
            nums[idx++] = i;
}
```

### 1.6 应用场景

| 场景 | 说明 |
|------|------|
| LC 75 颜色分类 | 值域 {0,1,2}，计数排序一行搞定 |
| LC 274 H-Index | 引用次数计数 |
| LC 347 前K高频 | 频率→桶/计数 |
| 年龄排序 | 0~150 的小值域 |

---

## 2. 桶排序 Bucket Sort

### 2.1 原理

将数据分配到若干**桶**中，每个桶内单独排序，最后按桶序合并。

### 2.2 图解过程

```
输入: [0.42, 0.32, 0.33, 0.52, 0.37, 0.47, 0.51]
5个桶，桶编号 = floor(val * 5)

桶分配:
  桶1 [0.2~0.4): 0.32, 0.33, 0.37
  桶2 [0.4~0.6): 0.42, 0.47
  桶2 [0.4~0.6): 0.52, 0.51

桶内排序:
  桶1: [0.32, 0.33, 0.37]
  桶2: [0.42, 0.47, 0.51, 0.52]

合并: [0.32, 0.33, 0.37, 0.42, 0.47, 0.51, 0.52]
```

### 2.3 代码

```cpp
void bucketSort(vector<float>& nums) {
    int n = nums.size();
    if (n <= 1) return;
    
    // 创建 n 个桶
    vector<vector<float>> buckets(n);
    
    // 分配到桶
    for (float x : nums) {
        int idx = (int)(x * n);  // 假设值在 [0, 1)
        if (idx >= n) idx = n - 1;
        buckets[idx].push_back(x);
    }
    
    // 桶内排序
    for (auto& b : buckets)
        sort(b.begin(), b.end());
    
    // 合并
    int idx = 0;
    for (auto& b : buckets)
        for (float x : b)
            nums[idx++] = x;
}
```

### 2.4 整数版桶排序

```cpp
void bucketSortInt(vector<int>& nums) {
    if (nums.empty()) return;
    int minVal = *min_element(nums.begin(), nums.end());
    int maxVal = *max_element(nums.begin(), nums.end());
    int bucketSize = max(1, (maxVal - minVal) / (int)nums.size() + 1);
    int bucketCount = (maxVal - minVal) / bucketSize + 1;
    
    vector<vector<int>> buckets(bucketCount);
    for (int x : nums)
        buckets[(x - minVal) / bucketSize].push_back(x);
    
    int idx = 0;
    for (auto& b : buckets) {
        sort(b.begin(), b.end());
        for (int x : b)
            nums[idx++] = x;
    }
}
```

### 2.5 经典应用：LC 164 最大间距

> 给定未排序数组，找排序后相邻元素最大差值，要求线性时间。

思路：桶排序 + 鸽巢原理 → 最大间距一定出现在**桶间**而非桶内。

```cpp
int maximumGap(vector<int>& nums) {
    int n = nums.size();
    if (n < 2) return 0;
    int minVal = *min_element(nums.begin(), nums.end());
    int maxVal = *max_element(nums.begin(), nums.end());
    if (minVal == maxVal) return 0;
    
    // 桶大小 = ceil((max-min) / (n-1))
    int bucketSize = max(1, (maxVal - minVal) / (n - 1));
    int bucketCount = (maxVal - minVal) / bucketSize + 1;
    
    // 每个桶只记录最大最小值
    vector<int> bucketMin(bucketCount, INT_MAX);
    vector<int> bucketMax(bucketCount, INT_MIN);
    vector<bool> used(bucketCount, false);
    
    for (int x : nums) {
        int idx = (x - minVal) / bucketSize;
        used[idx] = true;
        bucketMin[idx] = min(bucketMin[idx], x);
        bucketMax[idx] = max(bucketMax[idx], x);
    }
    
    int maxGap = 0, prevMax = minVal;
    for (int i = 0; i < bucketCount; i++) {
        if (!used[i]) continue;
        maxGap = max(maxGap, bucketMin[i] - prevMax);
        prevMax = bucketMax[i];
    }
    return maxGap;
}
```

---

## 3. 基数排序 Radix Sort

### 3.1 原理

按**位**排序，从最低位到最高位（LSD），每位使用稳定的计数排序。

### 3.2 图解过程

```
输入: [170, 45, 75, 90, 802, 24, 2, 66]

按个位排序: [170, 90, 802, 2, 24, 45, 75, 66]
按十位排序: [802, 2, 24, 45, 66, 170, 75, 90]
按百位排序: [2, 24, 45, 66, 75, 90, 170, 802]

每位排序都是稳定的，所以最终结果正确。
```

### 3.3 代码

```cpp
void radixSort(vector<int>& nums) {
    int maxVal = *max_element(nums.begin(), nums.end());
    
    // 从个位开始，每位做一次计数排序
    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        vector<int> output(nums.size());
        int count[10] = {0};
        
        // 计数
        for (int x : nums)
            count[(x / exp) % 10]++;
        
        // 前缀和
        for (int i = 1; i < 10; i++)
            count[i] += count[i - 1];
        
        // 反向填充（稳定）
        for (int i = nums.size() - 1; i >= 0; i--) {
            int digit = (nums[i] / exp) % 10;
            output[count[digit] - 1] = nums[i];
            count[digit]--;
        }
        
        nums = output;
    }
}
```

### 3.4 三种非比较排序对比

| 算法 | 时间 | 空间 | 稳定 | 适用场景 |
|------|------|------|------|---------|
| 计数排序 | O(n+k) | O(k) | ✅ | 值域小 (k≤10^5) |
| 桶排序 | O(n+k) | O(n+k) | ✅ | 均匀分布数据 |
| 基数排序 | O(d×(n+k)) | O(n+k) | ✅ | 整数位数d不大 |

---

## 4. 自定义排序与比较器

### 4.1 C++ 自定义排序方式

```cpp
// 方式1: lambda
sort(v.begin(), v.end(), [](int a, int b) { return a > b; });

// 方式2: 函数对象
struct Cmp {
    bool operator()(int a, int b) const { return a > b; }
};
sort(v.begin(), v.end(), Cmp());

// 方式3: 涉及成员比较
sort(students.begin(), students.end(),
    [](const Student& a, const Student& b) {
        if (a.score != b.score) return a.score > b.score;  // 分数降序
        return a.name < b.name;  // 同分按名字升序
    });
```

### 4.2 比较器注意事项

```
⚠️ 严格弱序要求 (Strict Weak Ordering):
  1. 非自反: cmp(a, a) 必须返回 false
  2. 非对称: 若 cmp(a,b)=true, 则 cmp(b,a)=false
  3. 传递性: 若 cmp(a,b)=true 且 cmp(b,c)=true, 则 cmp(a,c)=true

❌ 错误: [](int a, int b) { return a >= b; }  // 违反非自反
✅ 正确: [](int a, int b) { return a > b; }
```

### 4.3 LC 179 最大数

将数组排成最大的数，关键在于自定义比较规则：

```cpp
string largestNumber(vector<int>& nums) {
    vector<string> strs;
    for (int x : nums) strs.push_back(to_string(x));
    
    sort(strs.begin(), strs.end(),
        [](const string& a, const string& b) {
            return a + b > b + a;  // 核心: "9"+"30" > "30"+"9"
        });
    
    if (strs[0] == "0") return "0";  // 全是0
    
    string result;
    for (auto& s : strs) result += s;
    return result;
}
```

> **比较规则**：两个字符串 a 和 b，如果 a+b > b+a 则 a 排在前面。
> 例：a="9", b="30" → "930" > "309" → 9 排在 30 前面。

---

## 5. STL 部分排序技术

### 5.1 `partial_sort` — 找前K小

```cpp
vector<int> v = {5, 3, 8, 1, 2, 7, 4};

// 将最小的3个元素排序到前面
partial_sort(v.begin(), v.begin() + 3, v.end());
// v = [1, 2, 3, ?, ?, ?, ?]  前3个有序，其余未定义
```

### 5.2 `nth_element` — 找第K大/分割

```cpp
vector<int> v = {5, 3, 8, 1, 2, 7, 4};

// 将第3小的元素放到正确位置（0-based下标2）
nth_element(v.begin(), v.begin() + 2, v.end());
// v[2] == 3, v[0..1] <= 3, v[3..] >= 3
```

### 5.3 `stable_sort` vs `sort`

```cpp
// sort: 快、不稳定、O(nlogn)
sort(v.begin(), v.end());

// stable_sort: 稳定、O(nlogn) 时间、O(n) 空间
stable_sort(v.begin(), v.end());

// 需要保持相等元素相对顺序时，必须用 stable_sort
```

### 5.4 `is_sorted` / `is_sorted_until`

```cpp
vector<int> v = {1, 3, 5, 4, 7};
cout << is_sorted(v.begin(), v.end());  // false
auto it = is_sorted_until(v.begin(), v.end());
// *it == 4, 前3个元素有序
```

---

## 6. LeetCode 实战题解

### 6.1 LC 56 合并区间

```
输入: [[1,3],[2,6],[8,10],[15,18]]
输出: [[1,6],[8,10],[15,18]]

思路: 按左端点排序 → 依次合并重叠区间
```

```cpp
vector<vector<int>> merge(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end());  // 按左端点排
    vector<vector<int>> result;
    for (auto& intv : intervals) {
        if (!result.empty() && intv[0] <= result.back()[1]) {
            // 有重叠，合并
            result.back()[1] = max(result.back()[1], intv[1]);
        } else {
            result.push_back(intv);
        }
    }
    return result;
}
```

### 6.2 LC 75 颜色分类（荷兰国旗）

```
将 [2,0,2,1,1,0] 原地排为 [0,0,1,1,2,2]
不用排序库函数，一趟扫描完成。

三指针法（Dutch National Flag）:
  lo: 0区域右边界
  hi: 2区域左边界
  i: 当前扫描位置
```

```cpp
void sortColors(vector<int>& nums) {
    int lo = 0, hi = nums.size() - 1, i = 0;
    while (i <= hi) {
        if (nums[i] == 0) {
            swap(nums[i++], nums[lo++]);
        } else if (nums[i] == 2) {
            swap(nums[i], nums[hi--]);
            // 注意：i 不动，因为换来的值还没检查
        } else {
            i++;
        }
    }
}
```

> ⚠️ **经典陷阱**：`nums[i]==2` 时只移动 hi，不移动 i，因为从 hi 换过来的值可能是 0。

### 6.3 LC 148 排序链表

```
要求: O(nlogn) 时间, O(1) 空间 → 归并排序 (自底向上)

自顶向下用递归，空间 O(logn)（递归栈）
自底向上用迭代，空间 O(1)
```

```cpp
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// 自顶向下归并排序链表
ListNode* sortList(ListNode* head) {
    if (!head || !head->next) return head;
    
    // 快慢指针找中点
    ListNode* slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* mid = slow->next;
    slow->next = nullptr;  // 断开
    
    // 递归排左右
    ListNode* left = sortList(head);
    ListNode* right = sortList(mid);
    
    // 合并
    ListNode dummy(0);
    ListNode* tail = &dummy;
    while (left && right) {
        if (left->val <= right->val) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }
    tail->next = left ? left : right;
    return dummy.next;
}
```

### 6.4 LC 912 排序数组

标准的排序实现练习题，可以用来练习各种排序：

```cpp
// 推荐答案：随机化三向切分快排
vector<int> sortArray(vector<int>& nums) {
    mt19937 rng(42);
    function<void(int, int)> quickSort = [&](int lo, int hi) {
        if (lo >= hi) return;
        int ri = lo + rng() % (hi - lo + 1);
        swap(nums[lo], nums[ri]);
        int pivot = nums[lo];
        int lt = lo, i = lo + 1, gt = hi;
        while (i <= gt) {
            if (nums[i] < pivot) swap(nums[lt++], nums[i++]);
            else if (nums[i] > pivot) swap(nums[i], nums[gt--]);
            else i++;
        }
        quickSort(lo, lt - 1);
        quickSort(gt + 1, hi);
    };
    quickSort(0, nums.size() - 1);
    return nums;
}
```

### 6.5 LC 315 计算右侧小于当前元素的个数

```
输入: [5, 2, 6, 1]
输出: [2, 1, 1, 0]

解释: 5的右侧小于5的有 2,1 共2个
      2的右侧小于2的有 1 共1个
      ...
```

思路: **归并排序 + 索引追踪**（类似逆序对，但要记录每个元素的逆序对数量）

```cpp
vector<int> countSmaller(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, 0);
    vector<int> index(n);  // 追踪原始下标
    iota(index.begin(), index.end(), 0);
    
    function<void(int, int)> mergeSort = [&](int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(left, mid);
        mergeSort(mid + 1, right);
        
        vector<int> tempIdx;
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            if (nums[index[i]] <= nums[index[j]]) {
                // index[i] 对应的元素比 [mid+1..j-1] 都大
                result[index[i]] += (j - mid - 1);
                tempIdx.push_back(index[i++]);
            } else {
                tempIdx.push_back(index[j++]);
            }
        }
        while (i <= mid) {
            result[index[i]] += (j - mid - 1);
            tempIdx.push_back(index[i++]);
        }
        while (j <= right) {
            tempIdx.push_back(index[j++]);
        }
        copy(tempIdx.begin(), tempIdx.end(), index.begin() + left);
    };
    
    mergeSort(0, n - 1);
    return result;
}
```

### 6.6 LC 347 前K个高频元素

```
输入: nums = [1,1,1,2,2,3], k = 2
输出: [1,2]
```

思路：**桶排序** — 频率作为桶下标，O(n) 时间。

```cpp
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int x : nums) freq[x]++;
    
    // 桶: buckets[i] 存储出现 i 次的元素
    int n = nums.size();
    vector<vector<int>> buckets(n + 1);
    for (auto& [val, cnt] : freq)
        buckets[cnt].push_back(val);
    
    // 从高频到低频收集
    vector<int> result;
    for (int i = n; i >= 0 && (int)result.size() < k; i--)
        for (int x : buckets[i]) {
            result.push_back(x);
            if ((int)result.size() == k) break;
        }
    return result;
}
```

---

## 7. 排序技巧总结与易错点

### 7.1 高频面试技巧

| 技巧 | 说明 |
|------|------|
| 排序+双指针 | 排序后用双指针找两数之和、三数之和 |
| 排序+二分 | 排序后用lower_bound/upper_bound查找 |
| 排序+贪心 | 按某个维度排序后贪心选择（区间调度、分配问题） |
| 排序稳定性利用 | 多关键字排序时先排次关键字，再stable_sort主关键字 |
| 逆序对=归并 | 统计逆序对数量 → 归并排序计数 |

### 7.2 易错点清单

```
❌ 1. 快排 pivot 取固定元素
   → 有序数组退化 O(n²)
   ✅ 用随机化或三数取中

❌ 2. 比较器违反严格弱序
   → cmp(a,a) 返回 true 导致 UB / 死循环
   ✅ 比较器用 < 或 >，不要用 <= 或 >=

❌ 3. 归并合并时 <= 写成 <
   → 破坏稳定性
   ✅ 相等时优先取左侧元素

❌ 4. 荷兰国旗问题中 nums[i]==2 时 i++
   → 从后面换来的可能是0，漏处理
   ✅ 只移动 hi，i 不动

❌ 5. 桶排序桶大小为0
   → 当 maxVal == minVal 时除以0
   ✅ bucketSize = max(1, ...)

❌ 6. 堆排序建堆从 0 开始
   → 应该从最后一个非叶节点 n/2-1 开始
   ✅ for (int i = n/2-1; i >= 0; i--)

❌ 7. LC 179 结果全是 "0"
   → [0, 0, 0] 应该返回 "0" 而不是 "000"
   ✅ 检查排序后第一个元素是否为 "0"
```

### 7.3 复杂度速记口诀

```
比较排下界 nlogn，
快归堆排都达到:
  快排均logn空间，最坏n方要随机
  归并稳定开n空，链表排序它最行
  堆排原地O(1)空，最坏也是nlogn

非比较排破下界:
  计数O(n+k)值域小
  桶排均匀O(n)均
  基数d(n+k)按位分
```

### 7.4 排序算法 vs 应用场景映射

```
题型识别:
├── "排序后找东西" → sort + 二分/双指针
├── "第K大/小" → 快速选择O(n) / 堆O(nlogk)
├── "合并区间/重叠" → 排序 + 贪心
├── "最大数/自定义序" → 自定义比较器
├── "逆序对" → 归并排序计数
├── "链表排序" → 归并排序（链表友好）
├── "颜色分类" → 三向切分/计数
├── "最大间距" → 桶排序 + 鸽巢原理
└── "前K个高频" → 桶排序 / 堆
```

---

## 附：完整排序题目清单

### 基础排序
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 912 | 排序数组 | 中等 | 手写快排/归并 |
| 75 | 颜色分类 | 中等 | 三向切分/计数 |
| 215 | 第K大元素 | 中等 | 快速选择/堆 |
| 148 | 排序链表 | 中等 | 链表归并排序 |

### 自定义排序
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 56 | 合并区间 | 中等 | 按左端点排 |
| 179 | 最大数 | 中等 | 字符串拼接比较 |
| 1122 | 数组的相对排序 | 简单 | 自定义序 |
| 1356 | 根据数字二进制下1的数目排序 | 简单 | 多关键字排序 |

### 归并排序变体
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 315 | 右侧小于当前的个数 | 困难 | 归并+索引追踪 |
| 剑指51 | 数组中的逆序对 | 困难 | 归并计数 |
| 23 | 合并K个升序链表 | 困难 | 归并思想/堆 |
| 327 | 区间和的个数 | 困难 | 前缀和+归并 |

### 非比较排序
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 164 | 最大间距 | 中等 | 桶排序+鸽巢 |
| 274 | H-Index | 中等 | 计数排序思想 |
| 347 | 前K个高频元素 | 中等 | 桶排序/堆 |
| 451 | 根据字符出现频率排序 | 中等 | 桶排序 |

### 综合排序应用
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 252 | 会议室 | 简单 | 排序+判重叠 |
| 253 | 会议室II | 中等 | 排序+扫描线 |
| 435 | 无重叠区间 | 中等 | 排序+贪心 |
| 973 | 最接近原点的K个点 | 中等 | 快速选择 |

---

> 📌 **上篇回顾**：[专题十三（上）：排序算法基础](knowlege_details_13_sort_basics.md) 包含七大比较排序的手写实现和原理详解。
