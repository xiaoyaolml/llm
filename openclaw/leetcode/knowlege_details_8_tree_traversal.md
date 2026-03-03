# 专题八（上）：二叉树基础与遍历

> 作者：大胖超 😜
> 配套代码：`knowlege_details_8_tree_traversal.cpp`
> 进阶内容：`knowlege_details_8_tree_bst_advanced.md`

---

## 目录

1. [二叉树基础概念](#1-二叉树基础概念)
2. [树节点定义与建树](#2-树节点定义与建树)
3. [递归思维三要素](#3-递归思维三要素)
4. [前序遍历（Preorder）](#4-前序遍历preorder)
5. [中序遍历（Inorder）](#5-中序遍历inorder)
6. [后序遍历（Postorder）](#6-后序遍历postorder)
7. [层序遍历（Level Order / BFS）](#7-层序遍历level-order--bfs)
8. [递归经典题精讲](#8-递归经典题精讲)
9. [路径问题](#9-路径问题)
10. [构造二叉树](#10-构造二叉树)
11. [必刷题目清单（遍历篇）](#11-必刷题目清单遍历篇)
12. [常见陷阱与调试技巧](#12-常见陷阱与调试技巧)

---

## 1. 二叉树基础概念

### 1.1 什么是二叉树？

每个节点最多有**两个子节点**（左孩子、右孩子）的树结构。

```
        1           ← 根节点 (root)
       / \
      2   3         ← 1 的左右子节点
     / \   \
    4   5   6       ← 叶子节点 (leaf): 没有子节点的节点
```

### 1.2 核心术语

| 术语 | 定义 |
|------|------|
| **根节点 (root)** | 最顶层节点，没有父节点 |
| **叶子节点 (leaf)** | 没有子节点的节点 |
| **深度 (depth)** | 从根到该节点的边数（根的深度=0 或 1，看题目定义） |
| **高度 (height)** | 从该节点到最远叶子的边数 |
| **层 (level)** | 根为第 1 层（或第 0 层） |
| **度 (degree)** | 子节点个数（二叉树的度 ≤ 2） |

### 1.3 特殊二叉树

| 类型 | 定义 | 特点 |
|------|------|------|
| **满二叉树** | 每层节点都满 | n 层有 $2^n - 1$ 个节点 |
| **完全二叉树** | 除最后一层外都满，最后一层从左到右连续 | 可用数组存储，堆就是完全二叉树 |
| **二叉搜索树 (BST)** | 左 < 根 < 右 | 中序遍历有序 |
| **平衡二叉树 (AVL)** | 任意节点左右子树高度差 ≤ 1 | 保证 O(log n) 操作 |

### 1.4 二叉树的存储

```cpp
// 链式存储（LeetCode 标准）
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// 数组存储（完全二叉树/堆）
// 下标 i 的节点：
//   左孩子: 2*i + 1
//   右孩子: 2*i + 2
//   父节点: (i - 1) / 2
```

---

## 2. 树节点定义与建树

### 2.1 LeetCode 标准定义

```cpp
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) 
        : val(x), left(left), right(right) {}
};
```

### 2.2 从数组构建二叉树（调试用）

LeetCode 用层序数组表示树，`null` 表示空节点：

```
[1, 2, 3, null, 5, null, 7]

        1
       / \
      2   3
       \   \
        5   7
```

```cpp
// 从层序数组构建二叉树（-1 表示 null）
TreeNode* buildTree(vector<int>& arr) {
    if (arr.empty() || arr[0] == -1) return nullptr;
    
    TreeNode* root = new TreeNode(arr[0]);
    queue<TreeNode*> q;
    q.push(root);
    int i = 1;
    
    while (!q.empty() && i < arr.size()) {
        TreeNode* node = q.front(); q.pop();
        
        if (i < arr.size() && arr[i] != -1) {
            node->left = new TreeNode(arr[i]);
            q.push(node->left);
        }
        i++;
        
        if (i < arr.size() && arr[i] != -1) {
            node->right = new TreeNode(arr[i]);
            q.push(node->right);
        }
        i++;
    }
    return root;
}
```

### 2.3 打印二叉树（调试用）

```cpp
// 层序打印（方便调试）
void printTree(TreeNode* root) {
    if (!root) { cout << "[]" << endl; return; }
    queue<TreeNode*> q;
    q.push(root);
    cout << "[";
    bool first = true;
    while (!q.empty()) {
        TreeNode* node = q.front(); q.pop();
        if (!first) cout << ", ";
        first = false;
        if (node) {
            cout << node->val;
            q.push(node->left);
            q.push(node->right);
        } else {
            cout << "null";
        }
    }
    cout << "]" << endl;
}
```

---

## 3. 递归思维三要素

二叉树的大部分题目都用递归解决。掌握递归三要素是核心！

### 三要素

```
1. 确定递归函数的参数和返回值
   → 函数签名是什么？是否需要额外参数（如深度、路径）？
   → 返回值是什么？int、bool、TreeNode*？

2. 确定终止条件（base case）
   → 通常是 if (!root) return ...
   → 或 if (!root->left && !root->right) ... （叶子节点）

3. 确定单层递归的逻辑
   → 当前节点做什么？
   → 如何利用左右子树的递归结果？
```

### 两种递归方向

| 方向 | 别名 | 思路 | 典型题 |
|------|------|------|--------|
| **自顶向下** | 前序式 | 先处理当前节点，再递归子树 | LC 226 翻转、LC 112 路径和 |
| **自底向上** | 后序式 | 先递归子树，再用子树结果处理当前节点 | LC 104 深度、LC 110 平衡、LC 124 路径和 |

```cpp
// 自顶向下示例：传递信息给子节点
void topDown(TreeNode* root, int depth) {
    if (!root) return;
    // 先处理当前节点
    cout << "节点 " << root->val << " 深度 " << depth << endl;
    // 再递归
    topDown(root->left, depth + 1);
    topDown(root->right, depth + 1);
}

// 自底向上示例：从子节点收集信息
int bottomUp(TreeNode* root) {
    if (!root) return 0;
    // 先递归
    int leftH = bottomUp(root->left);
    int rightH = bottomUp(root->right);
    // 再用子树结果
    return max(leftH, rightH) + 1;
}
```

---

## 4. 前序遍历（Preorder）

**顺序**：根 → 左 → 右

### 4.1 递归写法

```cpp
void preorderRecur(TreeNode* root, vector<int>& result) {
    if (!root) return;
    result.push_back(root->val);       // 根
    preorderRecur(root->left, result);  // 左
    preorderRecur(root->right, result); // 右
}
```

### 4.2 迭代写法（用栈）

```cpp
vector<int> preorderTraversal(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> stk;
    if (root) stk.push(root);
    
    while (!stk.empty()) {
        TreeNode* node = stk.top(); stk.pop();
        result.push_back(node->val);         // 访问根
        if (node->right) stk.push(node->right);  // 先压右
        if (node->left) stk.push(node->left);    // 再压左（后出=先访问）
    }
    return result;
}
```

**为什么先压右再压左**？因为栈是 LIFO，先压右，左会先弹出被访问。

### 4.3 Morris 遍历（O(1) 空间，进阶）

```cpp
vector<int> preorderMorris(TreeNode* root) {
    vector<int> result;
    TreeNode* cur = root;
    
    while (cur) {
        if (!cur->left) {
            result.push_back(cur->val);
            cur = cur->right;
        } else {
            TreeNode* pred = cur->left;
            while (pred->right && pred->right != cur)
                pred = pred->right;
            
            if (!pred->right) {
                result.push_back(cur->val);  // 前序：第一次到达就访问
                pred->right = cur;           // 建线索
                cur = cur->left;
            } else {
                pred->right = nullptr;       // 拆线索
                cur = cur->right;
            }
        }
    }
    return result;
}
```

---

## 5. 中序遍历（Inorder）

**顺序**：左 → 根 → 右

> **重要**：BST 的中序遍历结果是**升序排列**的！

### 5.1 递归写法

```cpp
void inorderRecur(TreeNode* root, vector<int>& result) {
    if (!root) return;
    inorderRecur(root->left, result);
    result.push_back(root->val);        // 在中间访问
    inorderRecur(root->right, result);
}
```

### 5.2 迭代写法（用栈）⭐⭐⭐

这是面试最常考的遍历迭代写法：

```cpp
vector<int> inorderTraversal(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> stk;
    TreeNode* cur = root;
    
    while (cur || !stk.empty()) {
        // 一路向左，全部压栈
        while (cur) {
            stk.push(cur);
            cur = cur->left;
        }
        // 弹出栈顶（最左节点）
        cur = stk.top(); stk.pop();
        result.push_back(cur->val);  // 访问
        // 转向右子树
        cur = cur->right;
    }
    return result;
}
```

**理解**：先一路向左到底（入栈），然后弹出访问，再转右子树。

---

## 6. 后序遍历（Postorder）

**顺序**：左 → 右 → 根

### 6.1 递归写法

```cpp
void postorderRecur(TreeNode* root, vector<int>& result) {
    if (!root) return;
    postorderRecur(root->left, result);
    postorderRecur(root->right, result);
    result.push_back(root->val);         // 最后访问根
}
```

### 6.2 迭代写法（巧妙技巧）

后序 = 左→右→根 = **（根→右→左）的反序**！

```cpp
vector<int> postorderTraversal(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> stk;
    if (root) stk.push(root);
    
    while (!stk.empty()) {
        TreeNode* node = stk.top(); stk.pop();
        result.push_back(node->val);
        if (node->left) stk.push(node->left);    // 先压左
        if (node->right) stk.push(node->right);  // 再压右（先出=先访问）
    }
    reverse(result.begin(), result.end());  // 反转得到后序
    return result;
}
```

### 6.3 三种遍历的迭代对比

| 遍历 | 栈操作特点 | 关键 |
|------|-----------|------|
| 前序 | push 根 → pop 访问 → 先右后左入栈 | 直接 |
| 中序 | 一路向左入栈 → pop 访问 → 转右 | 模拟递归 |
| 后序 | 类似前序(根右左) → 最后 reverse | 巧妙转换 |

---

## 7. 层序遍历（Level Order / BFS）

**顺序**：逐层从左到右

### 7.1 基础模板

```cpp
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (!root) return result;
    
    queue<TreeNode*> q;
    q.push(root);
    
    while (!q.empty()) {
        int size = q.size();      // ⚠️ 必须提前存！
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

### 7.2 层序遍历变体

| 变体 | 修改点 | 对应题目 |
|------|--------|---------|
| 自底向上 | 最后 reverse result | LC 107 |
| 锯齿形 | 偶数层 reverse 当前层 | LC 103 |
| 右视图 | 每层取最后一个 | LC 199 |
| 层最大值 | 每层取 max | LC 515 |
| 层平均值 | 每层求 avg | LC 637 |
| 最大宽度 | 给节点编号，每层宽=右编号-左编号+1 | LC 662 |

### 7.3 LC 199: 二叉树的右视图

```cpp
vector<int> rightSideView(TreeNode* root) {
    vector<int> result;
    if (!root) return result;
    
    queue<TreeNode*> q;
    q.push(root);
    
    while (!q.empty()) {
        int size = q.size();
        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front(); q.pop();
            if (i == size - 1) result.push_back(node->val);  // 每层最后一个
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    return result;
}
```

### 7.4 LC 103: 二叉树的锯齿形层序遍历

```cpp
vector<vector<int>> zigzagLevelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (!root) return result;
    
    queue<TreeNode*> q;
    q.push(root);
    bool leftToRight = true;
    
    while (!q.empty()) {
        int size = q.size();
        vector<int> level(size);
        
        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front(); q.pop();
            int idx = leftToRight ? i : (size - 1 - i);
            level[idx] = node->val;
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
        result.push_back(level);
        leftToRight = !leftToRight;
    }
    return result;
}
```

---

## 8. 递归经典题精讲

### 8.1 LC 104: 二叉树的最大深度

```cpp
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return max(maxDepth(root->left), maxDepth(root->right)) + 1;
}
```

### 8.2 LC 111: 二叉树的最小深度

⚠️ 注意：最小深度是到**叶子节点**的最短路径，不是到空节点！

```cpp
int minDepth(TreeNode* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return 1;  // 叶子
    if (!root->left) return minDepth(root->right) + 1;   // 只有右子树
    if (!root->right) return minDepth(root->left) + 1;   // 只有左子树
    return min(minDepth(root->left), minDepth(root->right)) + 1;
}
```

### 8.3 LC 226: 翻转二叉树

```cpp
TreeNode* invertTree(TreeNode* root) {
    if (!root) return nullptr;
    swap(root->left, root->right);
    invertTree(root->left);
    invertTree(root->right);
    return root;
}
```

### 8.4 LC 101: 对称二叉树

```cpp
bool isSymmetric(TreeNode* root) {
    if (!root) return true;
    return isMirror(root->left, root->right);
}

bool isMirror(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val
        && isMirror(p->left, q->right)
        && isMirror(p->right, q->left);
}
```

### 8.5 LC 100: 相同的树

```cpp
bool isSameTree(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val
        && isSameTree(p->left, q->left)
        && isSameTree(p->right, q->right);
}
```

### 8.6 LC 110: 平衡二叉树

```cpp
bool isBalanced(TreeNode* root) {
    return height(root) != -1;
}

int height(TreeNode* root) {
    if (!root) return 0;
    int leftH = height(root->left);
    if (leftH == -1) return -1;
    int rightH = height(root->right);
    if (rightH == -1) return -1;
    if (abs(leftH - rightH) > 1) return -1;  // 不平衡
    return max(leftH, rightH) + 1;
}
```

### 8.7 LC 572: 另一棵树的子树

```cpp
bool isSubtree(TreeNode* root, TreeNode* subRoot) {
    if (!root) return false;
    if (isSameTree(root, subRoot)) return true;
    return isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
}
```

### 8.8 LC 617: 合并二叉树

```cpp
TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2) {
    if (!t1) return t2;
    if (!t2) return t1;
    t1->val += t2->val;
    t1->left = mergeTrees(t1->left, t2->left);
    t1->right = mergeTrees(t1->right, t2->right);
    return t1;
}
```

---

## 9. 路径问题

### 9.1 LC 112: 路径总和

判断是否存在根到叶子的路径，使节点值之和等于 target。

```cpp
bool hasPathSum(TreeNode* root, int targetSum) {
    if (!root) return false;
    if (!root->left && !root->right)
        return root->val == targetSum;
    return hasPathSum(root->left, targetSum - root->val)
        || hasPathSum(root->right, targetSum - root->val);
}
```

### 9.2 LC 113: 路径总和 II（收集所有路径）

```cpp
vector<vector<int>> pathSum(TreeNode* root, int targetSum) {
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(TreeNode*, int)> dfs = [&](TreeNode* node, int remain) {
        if (!node) return;
        path.push_back(node->val);
        
        if (!node->left && !node->right && remain == node->val) {
            result.push_back(path);
        }
        
        dfs(node->left, remain - node->val);
        dfs(node->right, remain - node->val);
        path.pop_back();  // 回溯
    };
    
    dfs(root, targetSum);
    return result;
}
```

### 9.3 LC 257: 二叉树的所有路径

```cpp
vector<string> binaryTreePaths(TreeNode* root) {
    vector<string> result;
    
    function<void(TreeNode*, string)> dfs = [&](TreeNode* node, string path) {
        if (!node) return;
        path += to_string(node->val);
        
        if (!node->left && !node->right) {
            result.push_back(path);
        } else {
            path += "->";
            dfs(node->left, path);
            dfs(node->right, path);
        }
    };
    
    dfs(root, "");
    return result;
}
```

### 9.4 LC 124: 二叉树中的最大路径和 ⭐⭐⭐⭐⭐

**Hard 题，面试高频**！

路径可以不经过根，可以从任意节点到任意节点。

```cpp
int maxPathSum(TreeNode* root) {
    int maxSum = INT_MIN;
    
    function<int(TreeNode*)> dfs = [&](TreeNode* node) -> int {
        if (!node) return 0;
        
        int leftGain = max(0, dfs(node->left));    // 负贡献不要
        int rightGain = max(0, dfs(node->right));
        
        // 当前节点作为拐点的路径和
        maxSum = max(maxSum, node->val + leftGain + rightGain);
        
        // 返回给父节点：只能选一边
        return node->val + max(leftGain, rightGain);
    };
    
    dfs(root);
    return maxSum;
}
```

**关键**：
- 返回给父节点时只能选一边（路径不能分叉）
- 更新全局答案时可以选两边（当前节点是路径最高点）
- 负贡献直接取 0（不如不要）

---

## 10. 构造二叉树

### 10.1 LC 105: 从前序与中序遍历构造二叉树

```
前序: [3, 9, 20, 15, 7]    → 根 = 3
中序: [9, 3, 15, 20, 7]    → 根(3)左边={9} 右边={15,20,7}
                                 ↓
                           3
                          / \
                         9  20
                           /  \
                          15   7
```

```cpp
TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
    unordered_map<int, int> inMap;
    for (int i = 0; i < inorder.size(); i++)
        inMap[inorder[i]] = i;
    
    int preIdx = 0;
    
    function<TreeNode*(int, int)> build = [&](int inLeft, int inRight) -> TreeNode* {
        if (inLeft > inRight) return nullptr;
        
        int rootVal = preorder[preIdx++];
        TreeNode* root = new TreeNode(rootVal);
        
        int inIdx = inMap[rootVal];  // 根在中序中的位置
        root->left = build(inLeft, inIdx - 1);    // 先建左子树
        root->right = build(inIdx + 1, inRight);  // 再建右子树
        
        return root;
    };
    
    return build(0, inorder.size() - 1);
}
```

### 10.2 LC 106: 从中序与后序遍历构造二叉树

```cpp
TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
    unordered_map<int, int> inMap;
    for (int i = 0; i < inorder.size(); i++)
        inMap[inorder[i]] = i;
    
    int postIdx = postorder.size() - 1;
    
    function<TreeNode*(int, int)> build = [&](int inLeft, int inRight) -> TreeNode* {
        if (inLeft > inRight) return nullptr;
        
        int rootVal = postorder[postIdx--];
        TreeNode* root = new TreeNode(rootVal);
        
        int inIdx = inMap[rootVal];
        root->right = build(inIdx + 1, inRight);  // 先建右子树！
        root->left = build(inLeft, inIdx - 1);    // 再建左子树
        
        return root;
    };
    
    return build(0, inorder.size() - 1);
}
```

> **注意**：后序遍历从后往前取根，且要**先建右子树再建左子树**（与 LC 105 相反）。

### 10.3 LC 654: 最大二叉树

```cpp
TreeNode* constructMaximumBinaryTree(vector<int>& nums) {
    function<TreeNode*(int, int)> build = [&](int lo, int hi) -> TreeNode* {
        if (lo > hi) return nullptr;
        
        int maxIdx = lo;
        for (int i = lo + 1; i <= hi; i++)
            if (nums[i] > nums[maxIdx]) maxIdx = i;
        
        TreeNode* root = new TreeNode(nums[maxIdx]);
        root->left = build(lo, maxIdx - 1);
        root->right = build(maxIdx + 1, hi);
        return root;
    };
    
    return build(0, nums.size() - 1);
}
```

---

## 11. 必刷题目清单（遍历篇）

### 基础必做

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 94 | 中序遍历 | Easy | 遍历 | 迭代栈写法 |
| 144 | 前序遍历 | Easy | 遍历 | 栈：先右后左 |
| 145 | 后序遍历 | Easy | 遍历 | 前序变体+反转 |
| 102 | 层序遍历 | Medium | BFS | 用 size 分层 |
| 104 | 最大深度 | Easy | 递归 | 自底向上 |
| 226 | 翻转二叉树 | Easy | 递归 | 自顶向下 |

### 进阶必做

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 100 | 相同的树 | Easy | 递归 | 双树同步 |
| 101 | 对称二叉树 | Easy | 递归 | 镜像比较 |
| 110 | 平衡二叉树 | Easy | 递归 | -1 剪枝 |
| 111 | 最小深度 | Easy | 递归 | 叶子条件 |
| 112 | 路径总和 | Easy | 路径 | 叶子判断 |
| 199 | 右视图 | Medium | BFS | 每层最后一个 |
| 103 | 锯齿形遍历 | Medium | BFS | 交替方向 |

### 高级冲刺

| 题号 | 题目 | 难度 | 类型 | 关键点 |
|------|------|------|------|--------|
| 105 | 前序+中序构造 | Medium | 构造 | 哈希加速 |
| 106 | 中序+后序构造 | Medium | 构造 | 先右后左 |
| 113 | 路径总和 II | Medium | 路径+回溯 | 收集所有路径 |
| 124 | 最大路径和 | Hard | 路径 | 全局变量 |
| 257 | 所有路径 | Easy | 路径 | 字符串拼接 |
| 572 | 另一棵树的子树 | Easy | 递归 | isSameTree |
| 654 | 最大二叉树 | Medium | 构造 | 分治 |

### 推荐刷题顺序

```
94/144/145(三序遍历) → 102(层序) → 104(深度) → 226(翻转)
→ 100/101(相同/对称) → 110(平衡) → 111(最小深度)
→ 112/113/257(路径系列) → 199/103(BFS变体)
→ 105/106(构造) → 572/617(子树/合并) → 124(Hard 路径和)
```

---

## 12. 常见陷阱与调试技巧

### 12.1 常见错误

| 错误 | 说明 | 修正 |
|------|------|------|
| 忘记判空 | `if (!root)` 缺失 → 空指针崩溃 | 每个递归函数开头判空 |
| 最小深度写错 | 把空子树当叶子 | 单侧空要走另一侧 |
| 路径和忘记叶子条件 | 中间节点也当答案 | `!left && !right` |
| 构造树左右反了 | 前序先左再右，后序先右再左 | 画图确认 |
| 回溯忘记 pop_back | 路径收集不正确 | 每次 push 后必 pop |

### 12.2 调试方法

1. **画树图**：手画树结构，标上节点值
2. **打印中间状态**：在递归中打印当前节点和参数
3. **最小用例**：先用 1-3 个节点的树测试
4. **层序打印**：调用 printTree 验证构造结果

### 12.3 递归 vs 迭代选择

| 情况 | 建议 |
|------|------|
| 面试默认 | 先写递归，面试官满意后再讨论迭代 |
| 栈溢出风险 | 树非常深（链状）时用迭代 |
| 需要记录层信息 | BFS 层序遍历更自然 |
| 后序遍历 | 迭代较巧妙，递归更直觉 |

---

> **进阶内容**请看 `knowlege_details_8_tree_bst_advanced.md`：BST 操作、最近公共祖先、序列化、高级树算法。
