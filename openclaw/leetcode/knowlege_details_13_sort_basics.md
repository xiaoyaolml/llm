# 专题十三（上）：排序算法基础 — 比较排序全解

> 作者：大胖超 😜
> 
> 排序是面试和竞赛中最基础也是最核心的话题。理解每种排序的原理、手写实现、时空复杂度和稳定性是解题的基石。

---

## 目录

1. [排序算法总览与分类](#1-排序算法总览与分类)
2. [冒泡排序 Bubble Sort](#2-冒泡排序-bubble-sort)
3. [选择排序 Selection Sort](#3-选择排序-selection-sort)
4. [插入排序 Insertion Sort](#4-插入排序-insertion-sort)
5. [希尔排序 Shell Sort](#5-希尔排序-shell-sort)
6. [快速排序 Quick Sort](#6-快速排序-quick-sort)
7. [归并排序 Merge Sort](#7-归并排序-merge-sort)
8. [堆排序 Heap Sort](#8-堆排序-heap-sort)
9. [排序稳定性详解](#9-排序稳定性详解)
10. [排序算法选择策略](#10-排序算法选择策略)

---

## 1. 排序算法总览与分类

### 1.1 复杂度速查表

| 算法 | 平均时间 | 最坏时间 | 最好时间 | 空间 | 稳定 | 原地 |
|------|---------|---------|---------|------|------|------|
| 冒泡排序 | O(n²) | O(n²) | O(n) | O(1) | ✅ | ✅ |
| 选择排序 | O(n²) | O(n²) | O(n²) | O(1) | ❌ | ✅ |
| 插入排序 | O(n²) | O(n²) | O(n) | O(1) | ✅ | ✅ |
| 希尔排序 | O(n^1.3) | O(n²) | O(n) | O(1) | ❌ | ✅ |
| 快速排序 | O(nlogn) | O(n²) | O(nlogn) | O(logn) | ❌ | ✅ |
| 归并排序 | O(nlogn) | O(nlogn) | O(nlogn) | O(n) | ✅ | ❌ |
| 堆排序   | O(nlogn) | O(nlogn) | O(nlogn) | O(1) | ❌ | ✅ |

### 1.2 分类方式

```
排序算法
├── 比较排序（本文）
│   ├── 交换排序：冒泡、快排
│   ├── 选择排序：简单选择、堆排序
│   ├── 插入排序：直接插入、希尔排序
│   └── 归并排序
└── 非比较排序（进阶篇）
    ├── 计数排序
    ├── 桶排序
    └── 基数排序
```

### 1.3 理解时间复杂度下界

> **比较排序的时间复杂度下界是 O(nlogn)**
> 
> 证明：n 个元素有 n! 种排列，决策树至少需要 log₂(n!) ≈ nlogn 个比较才能区分。
> 因此快排、归并、堆排序已达理论最优。

---

## 2. 冒泡排序 Bubble Sort

### 2.1 原理

反复交换相邻逆序元素，每轮**最大元素"冒泡"到末尾**。

### 2.2 图解过程

```
原始: [5, 3, 8, 1, 2]

第1轮: 将最大值冒到最后
  5,3 → swap → [3,5,8,1,2]
  5,8 → ok    → [3,5,8,1,2]
  8,1 → swap → [3,5,1,8,2]
  8,2 → swap → [3,5,1,2,|8|] ← 8 就位

第2轮:
  3,5 → ok    → [3,5,1,2,|8|]
  5,1 → swap → [3,1,5,2,|8|]
  5,2 → swap → [3,1,2,|5,8|] ← 5 就位

第3轮:
  3,1 → swap → [1,3,2,|5,8|]
  3,2 → swap → [1,2,|3,5,8|] ← 3 就位

第4轮:
  1,2 → ok → [1,2,3,5,8] ✓ 无交换,提前终止
```

### 2.3 代码

```cpp
void bubbleSort(vector<int>& nums) {
    int n = nums.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;  // 优化：无交换则提前终止
        for (int j = 0; j < n - 1 - i; j++) {
            if (nums[j] > nums[j + 1]) {
                swap(nums[j], nums[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}
```

### 2.4 要点

- **优化**：加 `swapped` 标志，如果某轮没有交换说明已有序，O(n) 最优
- **稳定性**：相等元素不交换，保持原始相对顺序 → 稳定
- **实际用途**：教学用，实际很少使用

---

## 3. 选择排序 Selection Sort

### 3.1 原理

每轮从未排序部分**选出最小值**，放到已排序末尾。

### 3.2 图解过程

```
原始: [3, 5, 1, 8, 2]

第1轮: 找最小值 1(下标2)，与位置0交换
  [|1|, 5, 3, 8, 2]

第2轮: 在 [5,3,8,2] 找最小值 2(下标4)，与位置1交换
  [1, |2|, 3, 8, 5]

第3轮: 在 [3,8,5] 找最小值 3(下标2)，不动
  [1, 2, |3|, 8, 5]

第4轮: 在 [8,5] 找最小值 5(下标4)，与位置3交换
  [1, 2, 3, |5|, 8]
```

### 3.3 代码

```cpp
void selectionSort(vector<int>& nums) {
    int n = nums.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++)
            if (nums[j] < nums[minIdx])
                minIdx = j;
        if (minIdx != i)
            swap(nums[i], nums[minIdx]);
    }
}
```

### 3.4 要点

- **不稳定**：远距离交换可能打乱相等元素顺序
  - 例: `[3a, 3b, 1]` → 选最小1，swap(3a, 1) → `[1, 3b, 3a]`，3a 3b 顺序变了
- **比较次数固定**：无论输入都是 n(n-1)/2 次比较
- **交换次数少**：最多 n-1 次交换（优于冒泡）

---

## 4. 插入排序 Insertion Sort

### 4.1 原理

类似**摸牌**：将每个新元素插入到前方已排序部分的正确位置。

### 4.2 图解过程

```
原始: [5, 3, 8, 1, 2]

i=1: 将3插入 [5] → 3<5,后移 → [3, 5, 8, 1, 2]
i=2: 将8插入 [3,5] → 8>5,不动 → [3, 5, 8, 1, 2]
i=3: 将1插入 [3,5,8] → 1<8,1<5,1<3 → [1, 3, 5, 8, 2]
i=4: 将2插入 [1,3,5,8] → 2<8,2<5,2<3,2>1 → [1, 2, 3, 5, 8]
```

### 4.3 代码

```cpp
void insertionSort(vector<int>& nums) {
    int n = nums.size();
    for (int i = 1; i < n; i++) {
        int key = nums[i];
        int j = i - 1;
        // 将比 key 大的元素后移
        while (j >= 0 && nums[j] > key) {
            nums[j + 1] = nums[j];
            j--;
        }
        nums[j + 1] = key;
    }
}
```

### 4.4 要点

- **对近乎有序的数组非常高效**：最好 O(n)
- **稳定**：相等元素不移动
- **在线算法**：可以边接收数据边排序
- **小规模最优**：n < 50 时通常比快排更快（STL内部在小区间会切换到插入排序）

---

## 5. 希尔排序 Shell Sort

### 5.1 原理

**改进的插入排序**：先对间隔 gap 的子序列做插入排序，逐步缩小 gap 直到 1。

### 5.2 图解过程

```
原始: [8, 5, 9, 1, 7, 3, 6, 2]   n=8

gap=4: 分组 (8,7) (5,3) (9,6) (1,2)
  插入排序各组 → [7, 3, 6, 1, 8, 5, 9, 2]

gap=2: 分组 (7,6,8,9) (3,1,5,2)
  插入排序各组 → [6, 1, 7, 2, 8, 3, 9, 5]

gap=1: 整体插入排序
  → [1, 2, 3, 5, 6, 7, 8, 9]
```

### 5.3 代码

```cpp
void shellSort(vector<int>& nums) {
    int n = nums.size();
    // 增量序列：n/2, n/4, ..., 1
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // 对每个间隔做插入排序
        for (int i = gap; i < n; i++) {
            int key = nums[i];
            int j = i - gap;
            while (j >= 0 && nums[j] > key) {
                nums[j + gap] = nums[j];
                j -= gap;
            }
            nums[j + gap] = key;
        }
    }
}
```

### 5.4 要点

- **不稳定**：跨间隔交换可能打乱相等元素顺序
- **增量序列选择影响性能**：Knuth序列 (3k-1)/2 可以达到 O(n^1.5)
- **实用性**：嵌入式系统中常用（不需要额外空间，实现简单）

---

## 6. 快速排序 Quick Sort

### 6.1 原理

**分治法**的经典应用：
1. 选一个**基准值 pivot**
2. **分区(Partition)**：将小于 pivot 的放左边，大于的放右边
3. 递归处理左右子数组

### 6.2 Partition 方案对比

| 方案 | 特点 | 适用 |
|------|------|------|
| Lomuto | 单指针从左扫，简单直观 | 教学 |
| Hoare | 双指针对向扫描，交换少 | 实际使用 |
| 三向切分 | 把等于pivot的分一组 | 大量重复元素 |

### 6.3 Lomuto Partition

```
pivot = nums[right]

 i              j→
[3, 6, 8, 1, 5, |2|]  pivot=2

j=0: 3>2 不动
j=1: 6>2 不动
j=2: 8>2 不动
j=3: 1<2 → swap(nums[0],nums[3]) → [1, 6, 8, 3, 5, 2], i=1
j=4: 5>2 不动
最后 swap(nums[1], nums[5]) → [1, 2, 8, 3, 5, 6]
                                    ^pivot
```

```cpp
// Lomuto 分区（教学版）
int lomutoPartition(vector<int>& nums, int lo, int hi) {
    int pivot = nums[hi];
    int i = lo;
    for (int j = lo; j < hi; j++) {
        if (nums[j] < pivot) {
            swap(nums[i], nums[j]);
            i++;
        }
    }
    swap(nums[i], nums[hi]);
    return i;
}
```

### 6.4 Hoare Partition（推荐）

```
[3, 6, 8, 1, 5, 2]  pivot = nums[mid] = 8

←j                i→
 从右找<pivot   从左找>pivot
 交换，继续，直到 i > j
```

```cpp
// Hoare 分区（实用版）
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

### 6.5 三向切分（Dutch National Flag）

处理大量重复元素时效率极高：将数组分为 `< pivot | == pivot | > pivot` 三段。

```cpp
void quickSort3Way(vector<int>& nums, int lo, int hi) {
    if (lo >= hi) return;
    int pivot = nums[lo];
    int lt = lo, i = lo + 1, gt = hi;
    //  [lo..lt-1] < pivot
    //  [lt..i-1]  == pivot
    //  [gt+1..hi] > pivot
    while (i <= gt) {
        if (nums[i] < pivot)
            swap(nums[lt++], nums[i++]);
        else if (nums[i] > pivot)
            swap(nums[i], nums[gt--]);
        else
            i++;
    }
    quickSort3Way(nums, lo, lt - 1);
    quickSort3Way(nums, gt + 1, hi);
}
```

### 6.6 快速排序优化策略

| 优化 | 说明 |
|------|------|
| 随机化 pivot | 避免最坏情况 O(n²)，`swap(nums[lo], nums[lo + rand()%(hi-lo+1)])` |
| 三数取中 | 取 lo、mid、hi 的中位数作 pivot |
| 小区间切换插入排序 | 当 hi - lo < 16 时用插入排序（STL 实际策略） |
| 尾递归优化 | 先递归短的一半，避免栈溢出 |

### 6.7 快速选择算法（Quick Select）

**O(n) 平均时间找到第 k 大/小元素**，只需递归一侧。

```cpp
// LC 215: 第 K 个最大元素
int quickSelect(vector<int>& nums, int lo, int hi, int k) {
    int pivot = nums[lo + (hi - lo) / 2];
    int i = lo, j = hi;
    while (i <= j) {
        while (nums[i] > pivot) i++;  // 注意：降序
        while (nums[j] < pivot) j--;
        if (i <= j) swap(nums[i++], nums[j--]);
    }
    if (lo + k - 1 <= j) return quickSelect(nums, lo, j, k);
    if (lo + k - 1 >= i) return quickSelect(nums, i, hi, k - (i - lo));
    return nums[j + 1];
}
```

### 6.8 经典面试题

- **LC 912 排序数组**：手写快排/归并的综合练习
- **LC 215 第K大**：快速选择 O(n)
- **LC 75 颜色分类**：三向切分变体

---

## 7. 归并排序 Merge Sort

### 7.1 原理

**分治法**：
1. 将数组从中间**分成两半**
2. 递归排序两半
3. **合并**两个有序子数组

### 7.2 图解过程

```
         [5, 3, 8, 1, 2, 7]
        /                    \
    [5, 3, 8]            [1, 2, 7]
    /      \              /      \
  [5]    [3, 8]       [1]    [2, 7]
         /    \               /    \
       [3]   [8]           [2]    [7]

合并过程（自下而上）：
  [3, 8] → 合并 → [3, 8]
  [2, 7] → 合并 → [2, 7]
  [5] + [3,8] → [3, 5, 8]
  [1] + [2,7] → [1, 2, 7]
  [3,5,8] + [1,2,7] → [1, 2, 3, 5, 7, 8]
```

### 7.3 合并过程详解

```
合并 [3, 5, 8] 和 [1, 2, 7]:

  i→ [3, 5, 8]
  j→ [1, 2, 7]
  result: []

  1<3 → 取1, j++      result: [1]
  2<3 → 取2, j++      result: [1, 2]
  3<7 → 取3, i++      result: [1, 2, 3]
  5<7 → 取5, i++      result: [1, 2, 3, 5]
  7<8 → 取7, j++      result: [1, 2, 3, 5, 7]
  剩8 → 取8           result: [1, 2, 3, 5, 7, 8]
```

### 7.4 代码

```cpp
void mergeSort(vector<int>& nums, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(nums, left, mid);
    mergeSort(nums, mid + 1, right);
    
    // 优化：如果已有序则跳过合并
    if (nums[mid] <= nums[mid + 1]) return;
    
    // 合并两个有序子数组
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (nums[i] <= nums[j])   // <= 保证稳定性
            temp[k++] = nums[i++];
        else
            temp[k++] = nums[j++];
    }
    while (i <= mid) temp[k++] = nums[i++];
    while (j <= right) temp[k++] = nums[j++];
    copy(temp.begin(), temp.end(), nums.begin() + left);
}
```

### 7.5 自底向上归并（迭代版）

不用递归，从长度 1 开始逐步合并：

```cpp
void mergeSortBottomUp(vector<int>& nums) {
    int n = nums.size();
    vector<int> temp(n);
    for (int width = 1; width < n; width *= 2) {
        for (int left = 0; left < n; left += 2 * width) {
            int mid = min(left + width - 1, n - 1);
            int right = min(left + 2 * width - 1, n - 1);
            // merge [left..mid] and [mid+1..right]
            int i = left, j = mid + 1, k = left;
            while (i <= mid && j <= right)
                temp[k++] = (nums[i] <= nums[j]) ? nums[i++] : nums[j++];
            while (i <= mid) temp[k++] = nums[i++];
            while (j <= right) temp[k++] = nums[j++];
            copy(temp.begin() + left, temp.begin() + right + 1, nums.begin() + left);
        }
    }
}
```

### 7.6 归并排序的重要应用：逆序对

利用归并过程统计**逆序对数量**：当右侧元素先于左侧被取出时，产生逆序对。

```cpp
// 剑指 Offer 51 / LeetCode 归并求逆序对
long long mergeSortCount(vector<int>& nums, int left, int right) {
    if (left >= right) return 0;
    int mid = left + (right - left) / 2;
    long long count = 0;
    count += mergeSortCount(nums, left, mid);
    count += mergeSortCount(nums, mid + 1, right);
    
    vector<int> temp;
    int i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        if (nums[i] <= nums[j]) {
            temp.push_back(nums[i++]);
        } else {
            // nums[j] < nums[i]
            // nums[i..mid] 都 > nums[j] → 产生 (mid-i+1) 个逆序对
            count += mid - i + 1;
            temp.push_back(nums[j++]);
        }
    }
    while (i <= mid) temp.push_back(nums[i++]);
    while (j <= right) temp.push_back(nums[j++]);
    copy(temp.begin(), temp.end(), nums.begin() + left);
    return count;
}
```

### 7.7 要点

- **稳定**：合并时 `<=` 保证相等元素相对位置不变
- **空间 O(n)**：需要临时数组
- **适用链表排序**：链表归并空间 O(1)，且不受随机访问限制
- **天然适合外排序**：大文件分块排序再合并

---

## 8. 堆排序 Heap Sort

### 8.1 原理

1. **建大顶堆**：使数组满足堆性质（父 ≥ 子）
2. **排序**：反复取堆顶（最大值）放到末尾，缩小堆范围

### 8.2 堆的基本概念

```
完全二叉树存储在数组中：
  父节点 i → 左子 2i+1, 右子 2i+2
  子节点 i → 父 (i-1)/2

大顶堆示例:
        9
       / \
      7   8
     / \ /
    3  5 2

数组: [9, 7, 8, 3, 5, 2]
```

### 8.3 Heapify（下沉调整）

```cpp
void heapify(vector<int>& nums, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && nums[left] > nums[largest])
        largest = left;
    if (right < n && nums[right] > nums[largest])
        largest = right;
    
    if (largest != i) {
        swap(nums[i], nums[largest]);
        heapify(nums, n, largest);  // 继续下沉
    }
}
```

### 8.4 完整堆排序

```cpp
void heapSort(vector<int>& nums) {
    int n = nums.size();
    
    // 1. 建堆：从最后一个非叶节点开始下沉
    //    最后一个非叶节点 = n/2 - 1
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(nums, n, i);
    
    // 2. 排序：把堆顶（最大值）交换到末尾
    for (int i = n - 1; i > 0; i--) {
        swap(nums[0], nums[i]);  // 堆顶→末尾
        heapify(nums, i, 0);     // 对[0..i-1]重新堆化
    }
}
```

### 8.5 图解堆排序过程

```
原始: [3, 5, 1, 8, 2]

建堆: 从 i=1 (最后非叶) 开始
  i=1: [3, |5|, 1, 8, 2] → 5 vs 8,2 → swap(5,8) → [3, 8, 1, 5, 2]
  i=0: [|3|, 8, 1, 5, 2] → 3 vs 8,1 → swap(3,8) → [8, 3, 1, 5, 2]
       继续下沉 3: 3 vs 5,2 → swap(3,5) → [8, 5, 1, 3, 2]
  
大顶堆: [8, 5, 1, 3, 2]

排序:
  swap(8,2) → [2, 5, 1, 3, |8|] → heapify → [5, 3, 1, 2, |8|]
  swap(5,2) → [2, 3, 1, |5, 8|] → heapify → [3, 2, 1, |5, 8|]
  swap(3,1) → [1, 2, |3, 5, 8|] → heapify → [2, 1, |3, 5, 8|]
  swap(2,1) → [1, |2, 3, 5, 8|]
  结果: [1, 2, 3, 5, 8]
```

### 8.6 要点

- **不稳定**：交换堆顶到末尾可能改变相等元素顺序
- **原地排序**：O(1) 额外空间
- **建堆时间**：O(n)（不是 O(nlogn)，自底向上分析）
- **优势**：最坏也保证 O(nlogn)，不像快排可能退化
- **劣势**：常数因子大，缓存不友好（跳跃访问）

---

## 9. 排序稳定性详解

### 9.1 什么是稳定性

**如果两个元素的排序键相等，排序后它们的相对顺序保持不变**，则称这个排序算法是稳定的。

### 9.2 为什么稳定性重要

```
学生数据按姓名排好后，再按成绩排序:
  
不稳定排序：
  原始: Alice-90, Bob-90, Charlie-85
  按成绩: Charlie-85, Bob-90, Alice-90  ← 同分数的顺序乱了

稳定排序：
  原始: Alice-90, Bob-90, Charlie-85
  按成绩: Charlie-85, Alice-90, Bob-90  ← 同分数保持原字母序
```

### 9.3 各算法稳定性原因

| 算法 | 稳定？ | 原因 |
|------|--------|------|
| 冒泡 | ✅ | 只在 `>` 时交换，相等不动 |
| 插入 | ✅ | 插入时遇到 `<=` 停止移动 |
| 归并 | ✅ | 合并时 `<=` 优先取左侧 |
| 选择 | ❌ | 远距离swap打乱相等元素 |
| 快排 | ❌ | partition交换跨越相等元素 |
| 堆排 | ❌ | 堆顶交换到末尾打乱顺序 |
| 希尔 | ❌ | 间隔交换打乱相等元素 |

### 9.4 面试技巧

> **口诀**：冒泡插入归并稳，快选堆希尔不稳。
> 
> 面试经常问："你知道哪些稳定的排序算法？" 答：冒泡、插入、归并。

---

## 10. 排序算法选择策略

### 10.1 选择决策树

```
需要排序？
├── 数据量 < 50
│   └── 插入排序 ✅ (常数因子最小)
├── 数据量适中 (< 10万)
│   ├── 需要稳定性？ → 归并排序
│   └── 不需要稳定性？ → 快速排序（随机化pivot）
├── 数据量很大
│   ├── 内存充足？ → 归并排序 / 快排
│   └── 内存受限？ → 堆排序 (原地O(1))
├── 数据基本有序？
│   └── 插入排序 / TimSort ✅
├── 数据范围小 (值域 ≤ 10^5)？
│   └── 计数排序 / 基数排序（见进阶篇）
└── 链表数据？
    └── 归并排序 ✅ (链表友好，空间O(1))
```

### 10.2 STL sort 的实际策略 — IntroSort

C++ `std::sort` 使用**内省排序 (IntroSort)**：

```
std::sort 的工作流程:
1. 先用快速排序 (Hoare partition + 三数取中)
2. 当递归深度超过 2*log₂(n) → 切换堆排序 (防止O(n²))
3. 当子数组 ≤ 16 个元素 → 切换插入排序 (小规模最快)
```

| 函数 | 底层算法 | 稳定 | 用途 |
|------|---------|------|------|
| `std::sort` | IntroSort | ❌ | 通用排序 |
| `std::stable_sort` | TimSort/归并 | ✅ | 需要稳定性 |
| `std::partial_sort` | 堆排序 | ❌ | 只排前K个 |
| `std::nth_element` | IntroSelect | ❌ | 找第K大 |

### 10.3 面试中写排序的建议

1. **快排必须会写**：面试频率最高，注意 pivot 选取和边界
2. **归并必须会写**：逆序对、链表排序必考
3. **堆排了解原理**：知道 heapify 和建堆过程
4. **三向切分**记住荷兰国旗思路，LC 75 直接考

---

## 附：LeetCode 排序题目清单

### 基础排序实现
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 912 | 排序数组 | 中等 | 手写快排/归并 |
| 75 | 颜色分类 | 中等 | 三向切分 |
| 215 | 第K大元素 | 中等 | 快速选择 |

### 归并排序应用
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 148 | 排序链表 | 中等 | 链表归并排序 |
| 315 | 右侧小于当前的个数 | 困难 | 归并+索引 |
| 剑指51 | 数组中的逆序对 | 困难 | 归并计数 |
| 23 | 合并K个升序链表 | 困难 | 归并思想/堆 |

### 自定义排序
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 56 | 合并区间 | 中等 | 按左端点排 |
| 179 | 最大数 | 中等 | 自定义比较 |
| 1122 | 数组的相对排序 | 简单 | 自定义序 |

### 非比较排序
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 164 | 最大间距 | 中等 | 桶排序 |
| 274 | H-Index | 中等 | 计数排序思想 |
| 347 | 前K个高频元素 | 中等 | 桶排序/堆 |

---

> 📌 **下篇预告**：[专题十三（下）：排序进阶应用](knowlege_details_13_sort_advanced.md) 将深入非比较排序（计数、桶、基数排序）和 LeetCode 排序应用题详解。
