# 专题二十（上）：树状数组 Binary Indexed Tree (Fenwick Tree) — 详解文档

> 作者：大胖超 😜
>
> 树状数组（BIT / Fenwick Tree）是一种利用 **lowbit** 巧妙索引的数据结构，用极简的代码实现 O(log n) 的**单点修改**和**前缀查询**。它比线段树简洁得多，是面试和竞赛中的常客。

---

## 第一章 核心概念

### 1.1 问题背景

给定数组 `nums[1..n]`，需要支持两种操作：
1. **单点修改**：`nums[i] += delta`
2. **区间查询**：`sum(nums[l..r])`

| 方案 | 修改 | 查询 |
|------|------|------|
| 暴力数组 | O(1) | O(n) |
| 前缀和 | O(n) 重建 | O(1) |
| **树状数组** | **O(log n)** | **O(log n)** |
| 线段树 | O(log n) | O(log n) |

树状数组是"前缀和"和"暴力更新"之间的完美折中。

### 1.2 lowbit — 树状数组的核心

```cpp
int lowbit(int x) { return x & (-x); }
```

`lowbit(x)` 返回 x 的二进制中**最低位的 1** 对应的值。

| x | 二进制 | lowbit(x) | 含义 |
|---|--------|-----------|------|
| 1 | 0001 | 1 | 管理 1 个元素 |
| 2 | 0010 | 2 | 管理 2 个元素 |
| 3 | 0011 | 1 | 管理 1 个元素 |
| 4 | 0100 | 4 | 管理 4 个元素 |
| 5 | 0101 | 1 | 管理 1 个元素 |
| 6 | 0110 | 2 | 管理 2 个元素 |
| 7 | 0111 | 1 | 管理 1 个元素 |
| 8 | 1000 | 8 | 管理 8 个元素 |

**规律**：`tree[i]` 存储的是 `nums[i - lowbit(i) + 1 .. i]` 这 `lowbit(i)` 个元素的**聚合值**（通常是和）。

### 1.3 树状数组结构图

```
下标:  1    2    3    4    5    6    7    8
nums: [a₁] [a₂] [a₃] [a₄] [a₅] [a₆] [a₇] [a₈]

tree[1] = a₁                          (lowbit=1, 管1个)
tree[2] = a₁+a₂                       (lowbit=2, 管2个)
tree[3] = a₃                          (lowbit=1, 管1个)
tree[4] = a₁+a₂+a₃+a₄                (lowbit=4, 管4个)
tree[5] = a₅                          (lowbit=1, 管1个)
tree[6] = a₅+a₆                       (lowbit=2, 管2个)
tree[7] = a₇                          (lowbit=1, 管1个)
tree[8] = a₁+a₂+a₃+a₄+a₅+a₆+a₇+a₈   (lowbit=8, 管8个)

树形结构:
          tree[8]
        /        \
    tree[4]      tree[6]    tree[7]
    /    \         |          |
tree[1] tree[2]  tree[5]   tree[3]
```

### 1.4 两大核心操作

#### 前缀查询：query(i) — 求 sum(1..i)

从 i 出发，不断减去 lowbit：`i → i - lowbit(i) → ... → 0`

```
query(7):
  7(0111) → sum += tree[7]
  6(0110) → sum += tree[6]
  4(0100) → sum += tree[4]
  0 → 停止

sum = tree[7] + tree[6] + tree[4]
    = a₇ + (a₅+a₆) + (a₁+a₂+a₃+a₄)
    = a₁+a₂+...+a₇ ✓
```

#### 单点更新：update(i, delta) — 给 nums[i] 加 delta

从 i 出发，不断加上 lowbit：`i → i + lowbit(i) → ... → > n`

```
update(3, +5):
  3(0011) → tree[3] += 5
  4(0100) → tree[4] += 5
  8(1000) → tree[8] += 5
  > n → 停止

所有包含 a₃ 的节点都更新了 ✓
```

### 1.5 复杂度

| 操作 | 时间 | 空间 |
|------|------|------|
| 构建 | O(n) 或 O(n log n) | O(n) |
| 单点更新 | O(log n) | — |
| 前缀查询 | O(log n) | — |
| 区间查询 | O(log n) | — |

---

## 第二章 基本模板

### 2.1 标准模板（单点修改 + 区间查询）

```cpp
class BIT {
    vector<int> tree;
    int n;
public:
    BIT(int n) : n(n), tree(n + 1, 0) {}

    // 从数组构建 - O(n)
    BIT(vector<int>& nums) : n(nums.size()), tree(n + 1, 0) {
        for (int i = 1; i <= n; i++) {
            tree[i] += nums[i - 1];
            int parent = i + (i & (-i));
            if (parent <= n) tree[parent] += tree[i];
        }
    }

    void update(int i, int delta) {
        for (; i <= n; i += i & (-i))
            tree[i] += delta;
    }

    int query(int i) {
        int sum = 0;
        for (; i > 0; i -= i & (-i))
            sum += tree[i];
        return sum;
    }

    int rangeQuery(int l, int r) {
        return query(r) - query(l - 1);
    }
};
```

> ⚠️ **注意**：树状数组是 **1-indexed**！如果题目是 0-indexed，所有下标 +1。

### 2.2 O(n) 构建详解

逐个逐个 `update` 是 $O(n \log n)$。更优的 $O(n)$ 构建：

```cpp
for (int i = 1; i <= n; i++) {
    tree[i] += nums[i - 1];
    int parent = i + (i & (-i));   // 父节点
    if (parent <= n)
        tree[parent] += tree[i];
}
```

每个节点只向父节点传递一次。

---

## 第三章 树状数组的变种

### 3.1 区间修改 + 单点查询

思路：在**差分数组**上建树状数组。

```
对区间 [l, r] +val:
  update(l, +val)
  update(r+1, -val)

查询 nums[i] 的值:
  query(i)   // 差分数组的前缀和 = 原始值
```

### 3.2 区间修改 + 区间查询

需要维护两个树状数组 `B1` 和 `B2`：

$$\text{sum}(1, x) = (x+1) \cdot B1[x] - B2[x]$$

对区间 [l, r] +val：
```
B1.update(l, val);    B1.update(r+1, -val);
B2.update(l, val*l);  B2.update(r+1, -val*(r+1));
```

查询 sum(1, x)：
```
(x+1) * B1.query(x) - B2.query(x)
```

### 3.3 二维树状数组

```cpp
class BIT2D {
    vector<vector<int>> tree;
    int m, n;
public:
    BIT2D(int m, int n) : m(m), n(n), tree(m+1, vector<int>(n+1, 0)) {}

    void update(int x, int y, int delta) {
        for (int i = x; i <= m; i += i & (-i))
            for (int j = y; j <= n; j += j & (-j))
                tree[i][j] += delta;
    }

    int query(int x, int y) {
        int sum = 0;
        for (int i = x; i > 0; i -= i & (-i))
            for (int j = y; j > 0; j -= j & (-j))
                sum += tree[i][j];
        return sum;
    }

    int rangeQuery(int x1, int y1, int x2, int y2) {
        return query(x2, y2) - query(x1-1, y2)
             - query(x2, y1-1) + query(x1-1, y1-1);
    }
};
```

### 3.4 离散化 + 树状数组

当值域很大（如 $10^9$）但元素数量少（如 $10^5$），先离散化再建 BIT。

```cpp
// 离散化
vector<int> sorted_vals = vals;
sort(sorted_vals.begin(), sorted_vals.end());
sorted_vals.erase(unique(sorted_vals.begin(), sorted_vals.end()), sorted_vals.end());

auto getId = [&](int x) {
    return lower_bound(sorted_vals.begin(), sorted_vals.end(), x) - sorted_vals.begin() + 1;
};
```

---

## 第四章 经典应用模式

### 4.1 模式一：逆序对计数

**问题**：数组中 `i < j` 但 `nums[i] > nums[j]` 的对数。

**方法**：从右到左遍历，每次查询当前值前面有多少个数，然后插入当前值。

```cpp
long long countInversions(vector<int>& nums) {
    // 离散化
    vector<int> sorted = nums;
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());
    int m = sorted.size();
    BIT bit(m);

    long long inv = 0;
    for (int i = nums.size() - 1; i >= 0; i--) {
        int rank = lower_bound(sorted.begin(), sorted.end(), nums[i]) - sorted.begin() + 1;
        inv += bit.query(rank - 1);  // 比当前小的已出现数
        bit.update(rank, 1);
    }
    return inv;
}
```

应用：**LC 315 — 计算右侧小于当前元素的个数**、**剑指 Offer 51 — 逆序对**。

### 4.2 模式二：动态前缀和查询

LC 307 — 区域和检索（可修改）：最经典的 BIT 应用。

### 4.3 模式三：求第 K 小

在 BIT 上二分：找最小的 `pos` 使得 `query(pos) >= k`。

```cpp
int kthSmallest(BIT& bit, int n, int k) {
    int pos = 0;
    for (int pw = 1 << (int)log2(n); pw > 0; pw >>= 1) {
        if (pos + pw <= n && bit.tree[pos + pw] < k) {
            pos += pw;
            k -= bit.tree[pos];
        }
    }
    return pos + 1;
}
```

时间 $O(\log n)$，比在 BIT 上二分查找更优（不需要额外 log）。

### 4.4 模式四：二维偏序

统计满足 $a_i < a_j$ 且 $b_i < b_j$ 的对数 → 按一维排序，另一维用 BIT。

---

## 第五章 LeetCode 题目清单

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 307 | 区域和检索 — 可修改 | BIT 单点修改+区间查询 | ⭐⭐ |
| 315 | 计算右侧小于当前元素的个数 | 逆序对 BIT + 离散化 | ⭐⭐⭐⭐ |
| 493 | 翻转对 | 归并排序 / BIT + 离散化 | ⭐⭐⭐⭐ |
| 327 | 区间和的个数 | BIT / 归并排序 | ⭐⭐⭐⭐ |
| 308 | 二维区域和检索 — 可修改 | 二维 BIT | ⭐⭐⭐ |
| 1649 | 通过指令创建有序数组 | BIT + 离散化 | ⭐⭐⭐ |
| 剑指51 | 数组中的逆序对 | BIT / 归并排序 | ⭐⭐⭐ |

---

## 第六章 树状数组 vs 线段树

| 对比 | 树状数组 | 线段树 |
|------|----------|--------|
| 代码量 | ~20 行 | ~60 行 |
| 常数系数 | 小（快 2-3 倍） | 大 |
| 单点修改+区间查询 | ✅ 最佳选择 | ✅ 可以 |
| 区间修改+区间查询 | ⚠️ 需要 2 个 BIT | ✅ 懒标记 |
| 区间最值查询 | ❌ 不支持 | ✅ 支持 |
| 动态开点 | ❌ 不支持 | ✅ 支持 |
| 可持久化 | ❌ 不支持 | ✅ 主席树 |
| 合并 | ❌ 不支持 | ✅ 线段树合并 |

> **结论**：能用树状数组就用树状数组（简洁+快）。需要更强功能（区间最值、懒标记、可持久化）就上线段树。

---

## 第七章 总结与易错点

### 7.1 知识脑图

```
树状数组 (BIT / Fenwick Tree)
├── 核心
│   ├── lowbit(x) = x & (-x)
│   ├── 1-indexed!
│   ├── update: i += lowbit(i)
│   └── query:  i -= lowbit(i)
├── 变种
│   ├── 单点修改 + 区间查询 (标准)
│   ├── 区间修改 + 单点查询 (差分BIT)
│   ├── 区间修改 + 区间查询 (双BIT)
│   └── 二维 BIT
├── 应用模式
│   ├── 逆序对计数 (LC 315)
│   ├── 动态前缀和 (LC 307)
│   ├── 第K小查询
│   └── 二维偏序
└── 技巧
    ├── O(n) 建树
    ├── 离散化处理大值域
    └── BIT 上二分
```

### 7.2 七大易错点

| # | 易错点 | 正确做法 |
|---|--------|----------|
| 1 | 0-indexed | BIT 是 **1-indexed**，下标从 1 开始 |
| 2 | lowbit 写错 | `x & (-x)` 不是 `x & (x-1)` |
| 3 | update 循环方向 | update 是 `i += lowbit(i)` 向上，query 是 `i -= lowbit(i)` 向下 |
| 4 | 忘记离散化 | 值域大（如 $10^9$）必须先离散化 |
| 5 | rangeQuery 边界 | `query(r) - query(l-1)` 而非 `query(r) - query(l)` |
| 6 | 修改值 vs 差量 | BIT update 传的是**增量 delta**，不是新值！ |
| 7 | int 溢出 | 前缀和可能很大，tree 用 `long long` |

### 7.3 面试 FAQ

**Q：什么时候选树状数组而不是线段树？**  
A：如果只需"单点修改+区间求和"，树状数组代码量最少、速度最快。如果需要区间最值、懒标记等高级功能，必须用线段树。

**Q：树状数组能求区间最值吗？**  
A：标准 BIT 不行（lowbit 分块不满足 max 的可减性）。有魔改版本但不常用，建议直接用线段树。

**Q：为什么 lowbit 的定义是 x & (-x)？**  
A：补码中 `-x = ~x + 1`，取反后加 1 使得最低位的 1 及以下与原数相同，`&` 后就只保留那个 1。

---

> 📂 **配套代码文件**：`knowlege_details_20_bit.cpp`  
> 📂 **线段树专题**：`knowlege_details_20_segment_tree.md` + `.cpp`
