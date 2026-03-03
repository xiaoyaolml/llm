# 专题二十（下）：线段树 Segment Tree — 详解文档

> 作者：大胖超 😜
>
> 线段树是最强大的区间数据结构——支持**区间修改**和**区间查询**的各种操作（求和、最值、GCD 等），配合懒标记 (Lazy Propagation) 能在 O(log n) 完成所有操作。掌握线段树相当于解锁了区间问题的大杀器。

---

## 第一章 核心概念

### 1.1 什么是线段树？

线段树是一棵**完全二叉树**，每个节点代表数组的一个**区间**：

```
数组: [1, 3, 5, 7, 9, 11]  (n=6)

              [0,5]:36
            /          \
       [0,2]:9        [3,5]:27
       /    \          /    \
   [0,1]:4  [2,2]:5  [3,4]:16  [5,5]:11
   /    \             /    \
[0,0]:1 [1,1]:3  [3,3]:7  [4,4]:9
```

- **叶节点**：单个元素
- **内部节点**：左右子节点的聚合值（如求和、最大值、最小值）
- **根节点**：整个数组的聚合值

### 1.2 为什么需要线段树？

| 需求 | 前缀和 | 树状数组 | 线段树 |
|------|--------|----------|--------|
| 单点修改+区间和 | O(n) | O(logn) | O(logn) |
| 区间修改+区间和 | O(n) | O(logn)* | **O(logn)** |
| 区间修改+区间最值 | O(n) | ❌ | **O(logn)** |
| 混合操作 | ❌ | ❌ | **O(logn)** |

> *树状数组需要两个 BIT 的复杂变形。

### 1.3 存储方式

用**数组**存储（编号从 1 开始）：

- 节点 `i` 的左子节点：`2*i`
- 节点 `i` 的右子节点：`2*i + 1`
- 节点 `i` 的父节点：`i / 2`

数组大小需要 **4n**（最坏情况下完全二叉树的节点数）。

### 1.4 复杂度

| 操作 | 时间 | 空间 |
|------|------|------|
| 建树 | O(n) | O(4n) |
| 单点修改 | O(log n) | — |
| 区间修改（懒标记） | O(log n) | — |
| 区间查询 | O(log n) | — |

---

## 第二章 基础线段树（无懒标记）

### 2.1 建树 build

递归建树：叶节点赋值，内部节点合并子节点。

```cpp
void build(int node, int start, int end) {
    if (start == end) {
        tree[node] = nums[start];
        return;
    }
    int mid = (start + end) / 2;
    build(2 * node, start, mid);
    build(2 * node + 1, mid + 1, end);
    tree[node] = tree[2 * node] + tree[2 * node + 1]; // 合并
}
```

### 2.2 单点修改 update

从根到叶，只修改包含目标下标的路径上的节点。

```cpp
void update(int node, int start, int end, int idx, int val) {
    if (start == end) {
        tree[node] = val;
        return;
    }
    int mid = (start + end) / 2;
    if (idx <= mid)
        update(2 * node, start, mid, idx, val);
    else
        update(2 * node + 1, mid + 1, end, idx, val);
    tree[node] = tree[2 * node] + tree[2 * node + 1]; // 回溯合并
}
```

### 2.3 区间查询 query

```cpp
int query(int node, int start, int end, int l, int r) {
    if (l <= start && end <= r)   // 完全包含
        return tree[node];
    if (end < l || start > r)     // 完全不交
        return 0;                 // 求和的单位元
    int mid = (start + end) / 2;
    return query(2 * node, start, mid, l, r)
         + query(2 * node + 1, mid + 1, end, l, r);
}
```

**三种情况**：
1. 当前区间被 [l,r] **完全包含** → 直接返回
2. 当前区间与 [l,r] **不相交** → 返回单位元
3. **部分相交** → 递归左右子树

---

## 第三章 懒标记（Lazy Propagation）

### 3.1 为什么需要懒标记？

没有懒标记时，区间修改需要更新**每个叶节点**→ O(n)。
懒标记的核心思想：**先不修改子节点，等需要时再下推**。

### 3.2 核心机制

```
lazy[node] 表示: 节点 node 的子节点们还没有加上的值

update [2,5] +3:
  1. 如果当前区间被 [2,5] 完全包含 → 直接修改 tree[node]，记录 lazy[node] += 3
  2. 否则 → 先把 lazy 下推到子节点，再递归
```

### 3.3 pushDown 操作

```cpp
void pushDown(int node, int start, int end) {
    if (lazy[node] != 0) {
        int mid = (start + end) / 2;
        // 更新左子树
        tree[2*node] += lazy[node] * (long long)(mid - start + 1);
        lazy[2*node] += lazy[node];
        // 更新右子树
        tree[2*node+1] += lazy[node] * (long long)(end - mid);
        lazy[2*node+1] += lazy[node];
        // 清除当前节点的懒标记
        lazy[node] = 0;
    }
}
```

### 3.4 区间修改（带懒标记）

```cpp
void rangeUpdate(int node, int start, int end, int l, int r, int val) {
    if (l <= start && end <= r) { // 完全包含
        tree[node] += (long long)val * (end - start + 1);
        lazy[node] += val;
        return;
    }
    pushDown(node, start, end);    // 下推懒标记
    int mid = (start + end) / 2;
    if (l <= mid) rangeUpdate(2*node, start, mid, l, r, val);
    if (r > mid)  rangeUpdate(2*node+1, mid+1, end, l, r, val);
    tree[node] = tree[2*node] + tree[2*node+1]; // 回溯合并
}
```

### 3.5 区间查询（带懒标记）

```cpp
long long rangeQuery(int node, int start, int end, int l, int r) {
    if (l <= start && end <= r) return tree[node];
    if (end < l || start > r) return 0;
    pushDown(node, start, end);
    int mid = (start + end) / 2;
    return rangeQuery(2*node, start, mid, l, r)
         + rangeQuery(2*node+1, mid+1, end, l, r);
}
```

### 3.6 懒标记的本质

```
懒标记 = "承诺": 我承诺子节点们都要各加 lazy[node]，但我还没下推。

- 还没查到子节点时，承诺不需要兑现
- 一旦需要递归子节点，先兑现承诺 (pushDown) 再递归
```

---

## 第四章 线段树的变种

### 4.1 区间最值线段树

把 `+` 改成 `max` / `min`：

```cpp
// 建树
tree[node] = max(tree[2*node], tree[2*node+1]);

// 查询
return max(query(left_child), query(right_child));

// 注意: 单位元改为 INT_MIN (max) 或 INT_MAX (min)
```

### 4.2 区间赋值（覆盖型懒标记）

与"区间加"不同，"区间赋值"的懒标记语义是**替换**而非**累加**：

```cpp
void pushDown(int node, int start, int end) {
    if (hasLazy[node]) {
        int mid = (start + end) / 2;
        tree[2*node] = lazy[node] * (mid - start + 1);
        tree[2*node+1] = lazy[node] * (end - mid);
        lazy[2*node] = lazy[2*node+1] = lazy[node];
        hasLazy[2*node] = hasLazy[2*node+1] = true;
        hasLazy[node] = false;
    }
}
```

> ⚠️ 需要额外 `hasLazy` 标志区分"没有懒标记"和"懒标记值为0"的情况。

### 4.3 混合操作：区间加 + 区间乘

需要**两个**懒标记：`add` 和 `mul`，下推时先乘后加。

```
实际值 = 原值 * mul + add
pushDown 时：子节点的 mul *= 父的 mul，子节点的 add = add * 父的 mul + 父的 add
```

### 4.4 区间 GCD

```cpp
tree[node] = gcd(tree[2*node], tree[2*node+1]);
```

配合"单点修改"使用，不需要懒标记（GCD 不支持简单的区间修改）。

### 4.5 动态开点线段树

当值域很大（如 $10^9$）但操作次数少时，不预分配 4n 空间，而是**用到时再创建节点**。

```cpp
struct Node {
    long long val;
    int left, right;  // 子节点编号（而非 2*i, 2*i+1）
    long long lazy;
};
vector<Node> nodes;
int newNode() {
    nodes.push_back({0, 0, 0, 0});
    return nodes.size() - 1;
}
```

应用：**LC 715 — Range 模块**、**LC 729/731/732 日程安排** 等。

### 4.6 主席树（可持久化线段树）

保留每次修改的历史版本，可以查询任意历史状态。  
核心：每次修改只新建 O(log n) 个节点（路径复制）。

应用：**静态区间第 K 小**。

---

## 第五章 LeetCode 经典题目

### 5.1 基础线段树

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 307 | 区域和检索 — 可修改 | 单点修改+区间和 | ⭐⭐ |
| 303 | 区域和检索 — 不可变 | 前缀和即可 | ⭐ |

### 5.2 区间修改型（懒标记）

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 370 | 区间加法（Premium） | 区间加+懒标记 | ⭐⭐ |
| 699 | 掉落的方块 | 区间赋值+区间最值 | ⭐⭐⭐ |
| 732 | 日程安排III — 最大交叠 | 动态开点+区间加 | ⭐⭐⭐ |
| 715 | Range模块 | 动态开点+区间赋值 | ⭐⭐⭐⭐ |

### 5.3 合并型

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 2286 | 以组为单位订音乐会座位 | 线段树维护区间最大连续空 | ⭐⭐⭐⭐ |
| 218 | 天际线问题 | 扫描线+线段树/优先队列 | ⭐⭐⭐⭐ |

### 5.4 高级应用

| # | 题目 | 核心思路 | 难度 |
|---|------|----------|------|
| 493 | 翻转对 | 线段树/BIT/归并 | ⭐⭐⭐⭐ |
| 327 | 区间和的个数 | 线段树/BIT/归并 | ⭐⭐⭐⭐ |
| 850 | 矩形面积 II | 扫描线+线段树 | ⭐⭐⭐⭐⭐ |

---

## 第六章 扫描线 + 线段树

### 6.1 扫描线思想

将二维问题转化为一维：按 x 坐标扫描，用线段树维护 y 维度的信息。

```
矩形面积并:
1. 收集所有矩形的左边和右边 → 事件
2. 按 x 排序, 每到一个事件:
   - 左边: 线段树区间 [y1, y2] +1
   - 右边: 线段树区间 [y1, y2] -1
3. 不为 0 的长度 × 当前 x 步长 = 该段面积
```

### 6.2 LC 218 — 天际线问题

虽然通常用优先队列解，但线段树的扫描线解法更通用。

---

## 第七章 实战中的线段树选型

```
需要什么操作？
│
├── 只有单点修改 + 区间和
│   └── ✅ 树状数组（最简洁）
│
├── 区间修改 + 区间和
│   └── ✅ 线段树 + 懒标记（区间加）
│
├── 区间修改 + 区间最值
│   └── ✅ 线段树 + 懒标记
│
├── 值域很大 (10^9)
│   └── ✅ 动态开点线段树
│
├── 需要历史版本查询
│   └── ✅ 主席树（可持久化）
│
└── 多棵树合并
    └── ✅ 线段树合并
```

---

## 第八章 总结与易错点

### 8.1 知识脑图

```
线段树
├── 基础
│   ├── 数组存储 (4n空间)
│   ├── build / update / query
│   └── 三种情况: 完全包含/不交/部分相交
├── 懒标记 (Lazy Propagation)
│   ├── pushDown 下推
│   ├── 区间加懒标记
│   ├── 区间赋值懒标记 (需 hasLazy)
│   └── 混合 (加+乘: 先乘后加)
├── 变种
│   ├── 区间最值 (max/min)
│   ├── 区间 GCD
│   ├── 动态开点 (大值域)
│   └── 主席树 (可持久化)
├── 应用
│   ├── 区间修改+查询 (LC 307/370)
│   ├── 扫描线 (LC 218/850)
│   ├── 日程安排 (LC 715/732)
│   └── 区间合并 (LC 2286)
└── 对比
    ├── vs 树状数组: 功能更强, 代码更长
    └── vs 平衡BST: 线段树更适合区间操作
```

### 8.2 七大易错点

| # | 易错点 | 正确做法 |
|---|--------|----------|
| 1 | 数组空间不够 | 开 **4n** 大小（不是 2n） |
| 2 | 忘记回溯合并 | 递归返回时 `tree[node] = merge(left, right)` |
| 3 | pushDown 顺序错 | 先 pushDown，再递归子节点 |
| 4 | 懒标记值为 0 与无标记混淆 | 区间赋值需要 `hasLazy` 标志 |
| 5 | 区间边界搞错 | `[start, end]` 是闭区间，mid 归左子 |
| 6 | int 溢出 | `tree` 和 `lazy` 用 `long long` |
| 7 | 混合操作下推顺序 | 加+乘：先乘后加，pushDown 时注意顺序 |

### 8.3 面试 FAQ

**Q: 线段树面试常考吗？**  
A: 中高难度面试（如 Meta、Google）可能考到基础线段树。懒标记一般出现在竞赛中。面试中更常考树状数组（简洁）或用其他方法替代。

**Q: 动态开点和普通线段树的区别？**  
A: 普通线段树预分配 4n 空间，适合值域 ≤ $10^6$ 的情况。动态开点不预分配，按需创建节点，适合值域到 $10^9$ 但操作数 ≤ $10^5$ 的场景。

**Q: 什么时候必须用线段树？**  
A: 当需要"区间修改+区间最值查询"时，树状数组做不到（只能做区间和），必须线段树。或者需要维护复杂信息（如最大连续子段和）时。

**Q: 主席树和普通线段树的区别？**  
A: 主席树通过路径复制实现版本管理，每次修改只新建 O(log n) 个节点。可以查询任意历史版本的状态，典型应用是静态区间第 K 小。

---

> 📂 **配套代码文件**：`knowlege_details_20_segment_tree.cpp`  
> 📂 **树状数组专题**：`knowlege_details_20_bit.md` + `.cpp`
