# 专题八（下）：BST（二叉搜索树）与进阶树题

> 作者：大胖超 😜
> 配合 `knowlege_details_8_tree_bst_advanced.cpp` 食用更佳

---

## 目录

1. [BST 核心性质](#1-bst-核心性质)
2. [BST 搜索与验证](#2-bst-搜索与验证)
3. [BST 插入与删除](#3-bst-插入与删除)
4. [BST 中序有序性应用](#4-bst-中序有序性应用)
5. [最近公共祖先 LCA](#5-最近公共祖先-lca)
6. [二叉树序列化与反序列化](#6-二叉树序列化与反序列化)
7. [二叉树展平 & 链表转换](#7-二叉树展平--链表转换)
8. [树的直径与深度进阶](#8-树的直径与深度进阶)
9. [有序数组/链表转 BST](#9-有序数组链表转-bst)
10. [树形 DP 入门](#10-树形-dp-入门)
11. [综合刷题清单](#11-综合刷题清单)
12. [易错点与调试技巧](#12-易错点与调试技巧)

---

## 1. BST 核心性质

### 什么是 BST

**二叉搜索树（Binary Search Tree）** 是一棵二叉树，满足：

- 左子树所有节点值 **< 根节点值**
- 右子树所有节点值 **> 根节点值**
- 左右子树也分别是 BST

```
       8
      / \
     3   10
    / \    \
   1   6   14
      / \  /
     4   7 13
```

### 关键推论

| 性质 | 说明 |
|------|------|
| 中序遍历有序 | BST 的中序遍历结果是**严格递增**序列 |
| 搜索 O(h) | 每次比较排除一半子树，平衡时 O(log n) |
| 最小值 | 一路向左走到底 |
| 最大值 | 一路向右走到底 |
| 后继（inorder successor） | 若有右子树→右子树最左节点；否则→最近的"左祖先" |

### ⚠️ 常见误区

```
      10
     /  \
    5    15
   / \
  3   12   ← 12 > 10，不满足 BST！
```

> 判断 BST 不能只看"左孩子 < 根 < 右孩子"，必须确保**整棵左子树 < 根 < 整棵右子树**

---

## 2. BST 搜索与验证

### LC 700: 二叉搜索树中的搜索

```cpp
TreeNode* searchBST(TreeNode* root, int val) {
    if (!root || root->val == val) return root;
    return val < root->val
        ? searchBST(root->left, val)
        : searchBST(root->right, val);
}
```

- 时间 O(h)，空间 O(h) 递归 / O(1) 迭代
- 迭代版本：

```cpp
TreeNode* searchBST(TreeNode* root, int val) {
    while (root && root->val != val) {
        root = val < root->val ? root->left : root->right;
    }
    return root;
}
```

### LC 98: 验证二叉搜索树

**方法一：递归 + 上下界**

```cpp
bool isValidBST(TreeNode* root, long lo = LONG_MIN, long hi = LONG_MAX) {
    if (!root) return true;
    if (root->val <= lo || root->val >= hi) return false;
    return isValidBST(root->left, lo, root->val)
        && isValidBST(root->right, root->val, hi);
}
```

**方法二：中序遍历判断递增**

```cpp
bool isValidBST(TreeNode* root) {
    long prev = LONG_MIN;
    stack<TreeNode*> stk;
    TreeNode* cur = root;
    while (cur || !stk.empty()) {
        while (cur) { stk.push(cur); cur = cur->left; }
        cur = stk.top(); stk.pop();
        if (cur->val <= prev) return false;
        prev = cur->val;
        cur = cur->right;
    }
    return true;
}
```

> **为什么用 `long`？** 因为节点值可以是 `INT_MIN`，初始边界必须更小

---

## 3. BST 插入与删除

### LC 701: 二叉搜索树中的插入操作

```cpp
TreeNode* insertIntoBST(TreeNode* root, int val) {
    if (!root) return new TreeNode(val);
    if (val < root->val)
        root->left = insertIntoBST(root->left, val);
    else
        root->right = insertIntoBST(root->right, val);
    return root;
}
```

> 利用返回值更新父节点指向，递归非常简洁

### LC 450: 删除二叉搜索树中的节点 ⛳

删除是 BST 最复杂的操作，分三种情况：

```
情况1: 叶节点 → 直接删
情况2: 只有一个孩子 → 用孩子替代
情况3: 有两个孩子 → 用右子树最小值（后继）替代
```

```cpp
TreeNode* deleteNode(TreeNode* root, int key) {
    if (!root) return nullptr;
    if (key < root->val) {
        root->left = deleteNode(root->left, key);
    } else if (key > root->val) {
        root->right = deleteNode(root->right, key);
    } else {
        // 找到目标节点
        if (!root->left) return root->right;   // 情况 1&2
        if (!root->right) return root->left;   // 情况 2
        // 情况 3: 找右子树最小值
        TreeNode* successor = root->right;
        while (successor->left) successor = successor->left;
        root->val = successor->val;  // 用后继值覆盖
        root->right = deleteNode(root->right, successor->val);
    }
    return root;
}
```

**删除过程图示：**

```
删除 5:
       5             6
      / \    →      / \
     3   8         3   8
    / \  /        / \
   2  4 6        2   4
        \
         7
→ 6 是右子树最小值，替代 5，然后从右子树删除 6
```

---

## 4. BST 中序有序性应用

### LC 230: 二叉搜索树中第 K 小的元素

```cpp
int kthSmallest(TreeNode* root, int k) {
    stack<TreeNode*> stk;
    TreeNode* cur = root;
    while (cur || !stk.empty()) {
        while (cur) { stk.push(cur); cur = cur->left; }
        cur = stk.top(); stk.pop();
        if (--k == 0) return cur->val;
        cur = cur->right;
    }
    return -1;  // 不会走到这里
}
```

### LC 538 / 1038: 把 BST 转换为累加树

**思路：** 反向中序（右→根→左），维护累加和

```cpp
TreeNode* convertBST(TreeNode* root) {
    int sum = 0;
    function<void(TreeNode*)> dfs = [&](TreeNode* node) {
        if (!node) return;
        dfs(node->right);
        sum += node->val;
        node->val = sum;
        dfs(node->left);
    };
    dfs(root);
    return root;
}
```

### LC 501: BST 中的众数

利用中序有序性，O(1) 空间找众数：

```cpp
vector<int> findMode(TreeNode* root) {
    vector<int> modes;
    int prev = INT_MIN, count = 0, maxCount = 0;
    function<void(TreeNode*)> inorder = [&](TreeNode* node) {
        if (!node) return;
        inorder(node->left);
        count = (node->val == prev) ? count + 1 : 1;
        if (count > maxCount) { maxCount = count; modes = {node->val}; }
        else if (count == maxCount) modes.push_back(node->val);
        prev = node->val;
        inorder(node->right);
    };
    inorder(root);
    return modes;
}
```

---

## 5. 最近公共祖先 LCA

### LC 235: BST 的最近公共祖先

利用 BST 性质：

```cpp
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    while (root) {
        if (p->val < root->val && q->val < root->val)
            root = root->left;
        else if (p->val > root->val && q->val > root->val)
            root = root->right;
        else
            return root;  // 分叉点就是 LCA
    }
    return nullptr;
}
```

> 当 p、q 分别在两侧（或一个就是当前节点），当前节点就是 LCA

### LC 236: 二叉树的最近公共祖先 ⛳

**通用做法（不依赖 BST）：**

```cpp
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;
    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);
    if (left && right) return root;  // p、q 分布两侧
    return left ? left : right;
}
```

**核心思想：**
- 后序遍历（从下往上汇报）
- 如果左右都找到了 → 当前节点就是 LCA
- 如果只有一侧找到 → 那一侧的结果就是 LCA

**图示：**

```
        3
       / \
      5   1
     / \ / \
    6  2 0  8
      / \
     7   4

LCA(5, 1) = 3    （分布两侧）
LCA(5, 4) = 5    （5 是 4 的祖先）
LCA(7, 4) = 2    （7 和 4 都在 2 的子树中）
```

---

## 6. 二叉树序列化与反序列化

### LC 297: 二叉树的序列化与反序列化 ⛳

**前序序列化：**

```cpp
class Codec {
public:
    string serialize(TreeNode* root) {
        if (!root) return "#";
        return to_string(root->val) + ","
             + serialize(root->left) + ","
             + serialize(root->right);
    }

    TreeNode* deserialize(string data) {
        queue<string> tokens;
        string token;
        for (char c : data) {
            if (c == ',') { tokens.push(token); token.clear(); }
            else token += c;
        }
        tokens.push(token);
        return buildTree(tokens);
    }

private:
    TreeNode* buildTree(queue<string>& tokens) {
        string val = tokens.front(); tokens.pop();
        if (val == "#") return nullptr;
        TreeNode* node = new TreeNode(stoi(val));
        node->left = buildTree(tokens);
        node->right = buildTree(tokens);
        return node;
    }
};
```

**层序序列化（LeetCode 风格）：**

```cpp
// 序列化
string serialize(TreeNode* root) {
    if (!root) return "[]";
    string result = "[";
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        TreeNode* node = q.front(); q.pop();
        if (node) {
            result += to_string(node->val) + ",";
            q.push(node->left);
            q.push(node->right);
        } else {
            result += "null,";
        }
    }
    // 去掉末尾的 null,
    result.pop_back();
    result += "]";
    return result;
}
```

---

## 7. 二叉树展平 & 链表转换

### LC 114: 二叉树展开为链表

```
    1             1
   / \             \
  2   5      →      2
 / \   \             \
3   4   6             3
                       \
                        4
                         \
                          5
                           \
                            6
```

**方法一：前序遍历 + 展开**

```cpp
void flatten(TreeNode* root) {
    vector<TreeNode*> nodes;
    function<void(TreeNode*)> preorder = [&](TreeNode* node) {
        if (!node) return;
        nodes.push_back(node);
        preorder(node->left);
        preorder(node->right);
    };
    preorder(root);
    for (int i = 1; i < (int)nodes.size(); i++) {
        nodes[i-1]->left = nullptr;
        nodes[i-1]->right = nodes[i];
    }
}
```

**方法二：O(1) 空间（Morris 思想）**

```cpp
void flatten(TreeNode* root) {
    TreeNode* cur = root;
    while (cur) {
        if (cur->left) {
            // 找左子树最右节点
            TreeNode* pre = cur->left;
            while (pre->right) pre = pre->right;
            pre->right = cur->right;
            cur->right = cur->left;
            cur->left = nullptr;
        }
        cur = cur->right;
    }
}
```

---

## 8. 树的直径与深度进阶

### LC 543: 二叉树的直径

**直径 = 经过某节点的"左深度 + 右深度"的最大值**

```cpp
int diameterOfBinaryTree(TreeNode* root) {
    int diameter = 0;
    function<int(TreeNode*)> depth = [&](TreeNode* node) -> int {
        if (!node) return 0;
        int left = depth(node->left);
        int right = depth(node->right);
        diameter = max(diameter, left + right);
        return max(left, right) + 1;
    };
    depth(root);
    return diameter;
}
```

> 注意：直径不一定经过根节点！

### LC 543 vs LC 124 对比

| 题目 | 维护的全局变量 | 返回值（给父节点用） |
|------|---------------|---------------------|
| LC 543 直径 | `maxDiameter = max(maxDiameter, L+R)` | `max(L, R) + 1` |
| LC 124 路径和 | `maxSum = max(maxSum, node->val+L+R)` | `node->val + max(L, R)` |

> 思路完全一致：**在每个节点算"拐点"值，更新全局最优；返回单边给父节点**

---

## 9. 有序数组/链表转 BST

### LC 108: 将有序数组转换为 BST

```cpp
TreeNode* sortedArrayToBST(vector<int>& nums) {
    function<TreeNode*(int, int)> build = [&](int left, int right) -> TreeNode* {
        if (left > right) return nullptr;
        int mid = left + (right - left) / 2;
        TreeNode* root = new TreeNode(nums[mid]);
        root->left = build(left, mid - 1);
        root->right = build(mid + 1, right);
        return root;
    };
    return build(0, nums.size() - 1);
}
```

> 每次取中间元素作为根 → 保证高度平衡

### LC 109: 有序链表转换为 BST

```cpp
// 利用中序遍历模拟
ListNode* cur;
TreeNode* sortedListToBST(ListNode* head) {
    int n = 0;
    for (ListNode* p = head; p; p = p->next) n++;
    cur = head;
    return build(0, n - 1);
}
TreeNode* build(int left, int right) {
    if (left > right) return nullptr;
    int mid = left + (right - left) / 2;
    TreeNode* leftChild = build(left, mid - 1);
    TreeNode* root = new TreeNode(cur->val);
    cur = cur->next;
    root->left = leftChild;
    root->right = build(mid + 1, right);
    return root;
}
```

---

## 10. 树形 DP 入门

树形 DP 是在树结构上做动态规划，通常模式：

```
int dfs(TreeNode* node) {
    if (!node) return 初始值;
    int left = dfs(node->left);
    int right = dfs(node->right);
    // 用 left、right 更新全局答案
    // 返回"给父节点用"的值
}
```

### LC 337: 打家劫舍 III

```
      3
     / \
    2   3
     \   \
      3   1
```

> 如果偷了爷爷（3），就不能偷两个儿子节点

```cpp
pair<int,int> rob(TreeNode* root) {
    // 返回 {不偷当前, 偷当前}
    if (!root) return {0, 0};
    auto [ln, ly] = rob(root->left);
    auto [rn, ry] = rob(root->right);
    int notRob = max(ln, ly) + max(rn, ry);
    int doRob = root->val + ln + rn;
    return {notRob, doRob};
}
// 调用：auto [n, y] = rob(root); return max(n, y);
```

### 树形 DP 通用套路

| 步骤 | 说明 |
|------|------|
| 1. 定义返回值 | dfs 返回什么信息给父节点？（通常用 pair/tuple/struct） |
| 2. 终止条件 | 空节点返回什么？ |
| 3. 合并子问题 | 用左右子树结果 + 当前节点信息，计算当前节点的答案 |
| 4. 全局答案 | 在 dfs 过程中更新全局变量（可选） |

---

## 11. 综合刷题清单

### BST 基础

| 序号 | 题目 | 难度 | 核心考点 |
|------|------|------|----------|
| 1 | LC 700: 搜索二叉搜索树 | Easy | BST 搜索 |
| 2 | LC 98: 验证 BST | Medium | 上下界 / 中序递增 |
| 3 | LC 701: BST 中的插入 | Medium | 递归插入 |
| 4 | LC 450: 删除 BST 中的节点 | Medium | 后继替代 |
| 5 | LC 230: 第K小元素 | Medium | 中序第K个 |
| 6 | LC 538/1038: BST 转累加树 | Medium | 反向中序 |
| 7 | LC 501: BST 中的众数 | Easy | 中序有序性 |
| 8 | LC 108: 有序数组转 BST | Easy | 中点建树 |
| 9 | LC 109: 有序链表转 BST | Medium | 中序模拟 |
| 10 | LC 235: BST 的 LCA | Medium | BST 分叉 |

### 进阶树题

| 序号 | 题目 | 难度 | 核心考点 |
|------|------|------|----------|
| 11 | LC 236: 二叉树的 LCA | Medium | 后序汇报 |
| 12 | LC 297: 序列化与反序列化 | Hard | 前序/层序编码 |
| 13 | LC 114: 展开为链表 | Medium | 前序 + Morris |
| 14 | LC 543: 二叉树直径 | Easy | 左深+右深 |
| 15 | LC 337: 打家劫舍 III | Medium | 树形 DP |
| 16 | LC 96: 不同的 BST | Medium | 卡特兰数 DP |
| 17 | LC 95: 不同的 BST II | Medium | 递归构造 |
| 18 | LC 99: 恢复 BST | Medium | 中序找逆序对 |
| 19 | LC 173: BST 迭代器 | Medium | 受控中序遍历 |
| 20 | LC 449: 序列化 BST | Medium | BST 前序唯一 |

### 推荐刷题顺序

```
BST搜索(700) → 验证(98) → 插入(701) → 删除(450) →
第K小(230) → 累加树(538) → LCA-BST(235) → LCA-通用(236) →
有序数组转BST(108) → 有序链表转BST(109) →
展平(114) → 直径(543) → 序列化(297) →
打家劫舍III(337) → 不同BST数(96) → 恢复BST(99)
```

---

## 12. 易错点与调试技巧

### 常见错误

| 错误 | 正确做法 |
|------|----------|
| BST 验证只看父子关系 | 必须传递上下界 |
| `deleteNode` 忘记返回新根 | 每一步 `root->left = deleteNode(...)` |
| LCA 忘记 `root == p` 的情况 | 开头 `if (!root \|\| root == p \|\| root == q) return root` |
| 树形 DP 只考虑偷 / 不偷，漏了组合 | 返回 pair，让父节点选择最优 |
| `long` vs `int` 边界溢出 | BST 验证用 `LONG_MIN / LONG_MAX` |
| 序列化分隔符问题 | 负数含 `-`，不能用 `-` 做分隔符 |

### 调试技巧

1. **打印树结构**：用层序 `printTree()` 辅助可视化
2. **中序打印**：检查 BST 是否保持有序
3. **画递归树**：追踪 dfs 调用过程
4. **小用例验证**：空树、单节点、链状树、满二叉树

### BST vs 普通二叉树 对比

| 特性 | BST | 普通二叉树 |
|------|-----|-----------|
| 搜索 | O(log n) 平衡时 | O(n) 只能遍历 |
| LCA | 利用值大小判断方向 | 后序遍历两边找 |
| 中序 | 升序序列 | 无特殊含义 |
| 插入 | 比较后放到叶子 | 视具体场景 |
| 删除 | 三种情况处理 | 视具体场景 |
| 构造 | 有序数组 → 中点递归 | 前序+中序 等 |
