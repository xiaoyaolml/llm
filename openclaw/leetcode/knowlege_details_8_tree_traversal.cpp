/**
 * ============================================================
 *   专题八（上）：二叉树基础与遍历 示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_8_tree_traversal knowlege_details_8_tree_traversal.cpp
 *   配合 knowlege_details_8_tree_traversal.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <functional>
#include <climits>
#include <unordered_map>
using namespace std;

// ============================================================
//  TreeNode 定义 & 建树/打印工具
// ============================================================
struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// 从层序数组构建二叉树（-1 表示 null）
TreeNode* buildTree(const vector<int>& arr) {
    if (arr.empty() || arr[0] == -1) return nullptr;
    TreeNode* root = new TreeNode(arr[0]);
    queue<TreeNode*> q;
    q.push(root);
    int i = 1;
    while (!q.empty() && i < (int)arr.size()) {
        TreeNode* node = q.front(); q.pop();
        if (i < (int)arr.size() && arr[i] != -1) {
            node->left = new TreeNode(arr[i]);
            q.push(node->left);
        }
        i++;
        if (i < (int)arr.size() && arr[i] != -1) {
            node->right = new TreeNode(arr[i]);
            q.push(node->right);
        }
        i++;
    }
    return root;
}

// 层序打印
void printTree(TreeNode* root, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    if (!root) { cout << "[]" << endl; return; }
    queue<TreeNode*> q;
    q.push(root);
    vector<string> vals;
    while (!q.empty()) {
        TreeNode* node = q.front(); q.pop();
        if (node) {
            vals.push_back(to_string(node->val));
            q.push(node->left);
            q.push(node->right);
        } else {
            vals.push_back("null");
        }
    }
    // 去掉末尾的 null
    while (!vals.empty() && vals.back() == "null") vals.pop_back();
    cout << "[";
    for (int i = 0; i < (int)vals.size(); i++) {
        if (i) cout << ", ";
        cout << vals[i];
    }
    cout << "]" << endl;
}

void printVec(const vector<int>& v, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    cout << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        if (i) cout << ", ";
        cout << v[i];
    }
    cout << "]" << endl;
}

void printVecVec(const vector<vector<int>>& vv, const string& label = "") {
    if (!label.empty()) cout << label << ":" << endl;
    for (auto& v : vv) {
        cout << "  [";
        for (int i = 0; i < (int)v.size(); i++) {
            if (i) cout << ", ";
            cout << v[i];
        }
        cout << "]" << endl;
    }
}

// 释放树（防内存泄漏）
void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// ============================================================
//  Demo 1: 三种遍历 — 递归 + 迭代
//  LC 144: 前序  LC 94: 中序  LC 145: 后序
// ============================================================

// --- 前序遍历 ---
vector<int> preorderRecur(TreeNode* root) {
    vector<int> result;
    function<void(TreeNode*)> dfs = [&](TreeNode* node) {
        if (!node) return;
        result.push_back(node->val);
        dfs(node->left);
        dfs(node->right);
    };
    dfs(root);
    return result;
}

vector<int> preorderIter(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> stk;
    if (root) stk.push(root);
    while (!stk.empty()) {
        TreeNode* node = stk.top(); stk.pop();
        result.push_back(node->val);
        if (node->right) stk.push(node->right);  // 先右后左！
        if (node->left) stk.push(node->left);
    }
    return result;
}

// --- 中序遍历 ---
vector<int> inorderRecur(TreeNode* root) {
    vector<int> result;
    function<void(TreeNode*)> dfs = [&](TreeNode* node) {
        if (!node) return;
        dfs(node->left);
        result.push_back(node->val);
        dfs(node->right);
    };
    dfs(root);
    return result;
}

vector<int> inorderIter(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> stk;
    TreeNode* cur = root;
    while (cur || !stk.empty()) {
        while (cur) {            // 一路向左入栈
            stk.push(cur);
            cur = cur->left;
        }
        cur = stk.top(); stk.pop();
        result.push_back(cur->val);  // 访问
        cur = cur->right;            // 转右子树
    }
    return result;
}

// --- 后序遍历 ---
vector<int> postorderRecur(TreeNode* root) {
    vector<int> result;
    function<void(TreeNode*)> dfs = [&](TreeNode* node) {
        if (!node) return;
        dfs(node->left);
        dfs(node->right);
        result.push_back(node->val);
    };
    dfs(root);
    return result;
}

vector<int> postorderIter(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> stk;
    if (root) stk.push(root);
    while (!stk.empty()) {
        TreeNode* node = stk.top(); stk.pop();
        result.push_back(node->val);
        if (node->left) stk.push(node->left);    // 先左后右！
        if (node->right) stk.push(node->right);
    }
    reverse(result.begin(), result.end());  // 反转
    return result;
}

void demo_traversal() {
    cout << "\n===== Demo 1: 三种遍历（递归+迭代）=====" << endl;

    //       1
    //      / \
    //     2   3
    //    / \   \
    //   4   5   6
    TreeNode* root = buildTree({1, 2, 3, 4, 5, -1, 6});
    printTree(root, "树");

    cout << "\n前序（根→左→右）:" << endl;
    printVec(preorderRecur(root), "  递归");   // [1,2,4,5,3,6]
    printVec(preorderIter(root), "  迭代");

    cout << "\n中序（左→根→右）:" << endl;
    printVec(inorderRecur(root), "  递归");    // [4,2,5,1,3,6]
    printVec(inorderIter(root), "  迭代");

    cout << "\n后序（左→右→根）:" << endl;
    printVec(postorderRecur(root), "  递归");  // [4,5,2,6,3,1]
    printVec(postorderIter(root), "  迭代");

    deleteTree(root);
}

// ============================================================
//  Demo 2: 层序遍历及变体
//  LC 102: 层序遍历
//  LC 103: 锯齿形层序遍历
//  LC 199: 二叉树的右视图
// ============================================================

// LC 102: 层序遍历
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (!root) return result;
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int size = q.size();
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

// LC 103: 锯齿形层序遍历
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

// LC 199: 二叉树的右视图
vector<int> rightSideView(TreeNode* root) {
    vector<int> result;
    if (!root) return result;
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int size = q.size();
        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front(); q.pop();
            if (i == size - 1) result.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    return result;
}

void demo_level_order() {
    cout << "\n===== Demo 2: 层序遍历及变体 =====" << endl;

    //        3
    //       / \
    //      9  20
    //        /  \
    //       15   7
    TreeNode* root = buildTree({3, 9, 20, -1, -1, 15, 7});
    printTree(root, "树");

    // LC 102
    cout << "\n--- LC 102: 层序遍历 ---" << endl;
    printVecVec(levelOrder(root), "层序");
    // [[3], [9, 20], [15, 7]]

    // LC 103
    cout << "\n--- LC 103: 锯齿形层序 ---" << endl;
    printVecVec(zigzagLevelOrder(root), "锯齿");
    // [[3], [20, 9], [15, 7]]

    // LC 199
    cout << "\n--- LC 199: 右视图 ---" << endl;
    printVec(rightSideView(root), "右视图");
    // [3, 20, 7]

    //        1
    //       / \
    //      2   3
    //       \
    //        5
    TreeNode* root2 = buildTree({1, 2, 3, -1, 5});
    printTree(root2, "\n树2");
    printVec(rightSideView(root2), "右视图");
    // [1, 3, 5]

    deleteTree(root);
    deleteTree(root2);
}

// ============================================================
//  Demo 3: 递归经典题
//  LC 104: 最大深度   LC 111: 最小深度
//  LC 226: 翻转      LC 101: 对称
//  LC 110: 平衡判断
// ============================================================

int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return max(maxDepth(root->left), maxDepth(root->right)) + 1;
}

int minDepth(TreeNode* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return 1;
    if (!root->left) return minDepth(root->right) + 1;
    if (!root->right) return minDepth(root->left) + 1;
    return min(minDepth(root->left), minDepth(root->right)) + 1;
}

TreeNode* invertTree(TreeNode* root) {
    if (!root) return nullptr;
    swap(root->left, root->right);
    invertTree(root->left);
    invertTree(root->right);
    return root;
}

bool isMirror(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val
        && isMirror(p->left, q->right)
        && isMirror(p->right, q->left);
}
bool isSymmetric(TreeNode* root) {
    if (!root) return true;
    return isMirror(root->left, root->right);
}

int heightCheck(TreeNode* root) {
    if (!root) return 0;
    int l = heightCheck(root->left);
    if (l == -1) return -1;
    int r = heightCheck(root->right);
    if (r == -1) return -1;
    if (abs(l - r) > 1) return -1;
    return max(l, r) + 1;
}
bool isBalanced(TreeNode* root) {
    return heightCheck(root) != -1;
}

void demo_recursive_classic() {
    cout << "\n===== Demo 3: 递归经典题 =====" << endl;

    //       3
    //      / \
    //     9  20
    //       /  \
    //      15   7
    TreeNode* t1 = buildTree({3, 9, 20, -1, -1, 15, 7});
    printTree(t1, "树");
    cout << "最大深度: " << maxDepth(t1) << " (期望=3)" << endl;
    cout << "最小深度: " << minDepth(t1) << " (期望=2, 根→9)" << endl;
    cout << "是否平衡: " << (isBalanced(t1) ? "true" : "false") << " (期望=true)" << endl;

    // 对称树
    //       1
    //      / \
    //     2   2
    //    / \ / \
    //   3  4 4  3
    TreeNode* t2 = buildTree({1, 2, 2, 3, 4, 4, 3});
    printTree(t2, "\n对称树");
    cout << "是否对称: " << (isSymmetric(t2) ? "true" : "false") << " (期望=true)" << endl;

    // 非对称
    TreeNode* t3 = buildTree({1, 2, 2, -1, 3, -1, 3});
    printTree(t3, "非对称树");
    cout << "是否对称: " << (isSymmetric(t3) ? "true" : "false") << " (期望=false)" << endl;

    // 翻转
    cout << "\n--- LC 226: 翻转二叉树 ---" << endl;
    TreeNode* t4 = buildTree({4, 2, 7, 1, 3, 6, 9});
    printTree(t4, "翻转前");
    invertTree(t4);
    printTree(t4, "翻转后");  // [4, 7, 2, 9, 6, 3, 1]

    // 不平衡
    //   1
    //    \
    //     2
    //      \
    //       3
    TreeNode* t5 = buildTree({1, -1, 2, -1, -1, -1, 3});
    cout << "\n链状树: 是否平衡: " << (isBalanced(t5) ? "true" : "false")
         << " (期望=false)" << endl;

    deleteTree(t1); deleteTree(t2); deleteTree(t3);
    deleteTree(t4); deleteTree(t5);
}

// ============================================================
//  Demo 4: 路径问题
//  LC 112: 路径总和
//  LC 113: 路径总和 II
//  LC 257: 二叉树的所有路径
//  LC 124: 二叉树中的最大路径和
// ============================================================

// LC 112
bool hasPathSum(TreeNode* root, int targetSum) {
    if (!root) return false;
    if (!root->left && !root->right) return root->val == targetSum;
    return hasPathSum(root->left, targetSum - root->val)
        || hasPathSum(root->right, targetSum - root->val);
}

// LC 113
vector<vector<int>> pathSum(TreeNode* root, int targetSum) {
    vector<vector<int>> result;
    vector<int> path;
    function<void(TreeNode*, int)> dfs = [&](TreeNode* node, int remain) {
        if (!node) return;
        path.push_back(node->val);
        if (!node->left && !node->right && remain == node->val)
            result.push_back(path);
        dfs(node->left, remain - node->val);
        dfs(node->right, remain - node->val);
        path.pop_back();
    };
    dfs(root, targetSum);
    return result;
}

// LC 257
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

// LC 124
int maxPathSum(TreeNode* root) {
    int maxSum = INT_MIN;
    function<int(TreeNode*)> dfs = [&](TreeNode* node) -> int {
        if (!node) return 0;
        int leftGain = max(0, dfs(node->left));    // 负贡献 → 0
        int rightGain = max(0, dfs(node->right));
        maxSum = max(maxSum, node->val + leftGain + rightGain);  // 拐点
        return node->val + max(leftGain, rightGain);  // 只选一边
    };
    dfs(root);
    return maxSum;
}

void demo_path() {
    cout << "\n===== Demo 4: 路径问题 =====" << endl;

    //        5
    //       / \
    //      4   8
    //     /   / \
    //    11  13  4
    //   / \    / \
    //  7   2  5   1
    TreeNode* root = buildTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, 5, 1});
    printTree(root, "树");

    // LC 112
    cout << "\n--- LC 112: 路径总和 ---" << endl;
    cout << "target=22: " << (hasPathSum(root, 22) ? "true" : "false")
         << " (期望=true, 5→4→11→2)" << endl;
    cout << "target=26: " << (hasPathSum(root, 26) ? "true" : "false")
         << " (期望=true, 5→8→13)" << endl;
    cout << "target=100: " << (hasPathSum(root, 100) ? "true" : "false")
         << " (期望=false)" << endl;

    // LC 113
    cout << "\n--- LC 113: 路径总和 II ---" << endl;
    auto paths = pathSum(root, 22);
    printVecVec(paths, "target=22 的所有路径");
    // [5,4,11,2], [5,8,4,5]

    // LC 257
    cout << "\n--- LC 257: 所有路径 ---" << endl;
    //    1
    //   / \
    //  2   3
    //   \
    //    5
    TreeNode* t2 = buildTree({1, 2, 3, -1, 5});
    printTree(t2, "树");
    auto allPaths = binaryTreePaths(t2);
    for (auto& p : allPaths) cout << "  " << p << endl;
    // "1->2->5", "1->3"

    // LC 124
    cout << "\n--- LC 124: 最大路径和 ---" << endl;
    //    -10
    //    / \
    //   9  20
    //     /  \
    //    15   7
    TreeNode* t3 = buildTree({-10, 9, 20, -1, -1, 15, 7});
    printTree(t3, "树");
    cout << "最大路径和: " << maxPathSum(t3) << " (期望=42, 15→20→7)" << endl;

    TreeNode* t4 = buildTree({1, 2, 3});
    printTree(t4, "树");
    cout << "最大路径和: " << maxPathSum(t4) << " (期望=6, 2→1→3)" << endl;

    TreeNode* t5 = buildTree({-3});
    cout << "只有 -3: 最大路径和: " << maxPathSum(t5) << " (期望=-3)" << endl;

    deleteTree(root); deleteTree(t2); deleteTree(t3);
    deleteTree(t4); deleteTree(t5);
}

// ============================================================
//  Demo 5: 构造二叉树
//  LC 105: 前序 + 中序 → 构造
//  LC 106: 中序 + 后序 → 构造
// ============================================================

// LC 105: 前序+中序构造
TreeNode* buildFromPreIn(vector<int>& preorder, vector<int>& inorder) {
    unordered_map<int, int> inMap;
    for (int i = 0; i < (int)inorder.size(); i++)
        inMap[inorder[i]] = i;

    int preIdx = 0;
    function<TreeNode*(int, int)> build = [&](int inLeft, int inRight) -> TreeNode* {
        if (inLeft > inRight) return nullptr;
        int rootVal = preorder[preIdx++];
        TreeNode* root = new TreeNode(rootVal);
        int inIdx = inMap[rootVal];
        root->left = build(inLeft, inIdx - 1);
        root->right = build(inIdx + 1, inRight);
        return root;
    };
    return build(0, inorder.size() - 1);
}

// LC 106: 中序+后序构造
TreeNode* buildFromInPost(vector<int>& inorder, vector<int>& postorder) {
    unordered_map<int, int> inMap;
    for (int i = 0; i < (int)inorder.size(); i++)
        inMap[inorder[i]] = i;

    int postIdx = postorder.size() - 1;
    function<TreeNode*(int, int)> build = [&](int inLeft, int inRight) -> TreeNode* {
        if (inLeft > inRight) return nullptr;
        int rootVal = postorder[postIdx--];
        TreeNode* root = new TreeNode(rootVal);
        int inIdx = inMap[rootVal];
        root->right = build(inIdx + 1, inRight);  // 先右！
        root->left = build(inLeft, inIdx - 1);
        return root;
    };
    return build(0, inorder.size() - 1);
}

void demo_construct() {
    cout << "\n===== Demo 5: 构造二叉树 =====" << endl;

    // LC 105
    cout << "\n--- LC 105: 前序+中序 → 构造 ---" << endl;
    vector<int> pre = {3, 9, 20, 15, 7};
    vector<int> in1 = {9, 3, 15, 20, 7};
    printVec(pre, "前序");
    printVec(in1, "中序");
    TreeNode* t1 = buildFromPreIn(pre, in1);
    printTree(t1, "构造结果");
    // [3, 9, 20, null, null, 15, 7]

    // LC 106
    cout << "\n--- LC 106: 中序+后序 → 构造 ---" << endl;
    vector<int> in2 = {9, 3, 15, 20, 7};
    vector<int> post = {9, 15, 7, 20, 3};
    printVec(in2, "中序");
    printVec(post, "后序");
    TreeNode* t2 = buildFromInPost(in2, post);
    printTree(t2, "构造结果");

    cout << "\n构造原理:" << endl;
    cout << "  前序第一个 = 根（3）" << endl;
    cout << "  中序中找到根 → 左边{9}=左子树，右边{15,20,7}=右子树" << endl;
    cout << "  递归构造左右子树" << endl;
    cout << "  后序: 最后一个 = 根，且先建右子树再建左子树" << endl;

    deleteTree(t1); deleteTree(t2);
}

// ============================================================
//  Demo 6: 其他经典题
//  LC 572: 另一棵树的子树
//  LC 617: 合并二叉树
// ============================================================

bool isSameTree(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val
        && isSameTree(p->left, q->left)
        && isSameTree(p->right, q->right);
}

// LC 572
bool isSubtree(TreeNode* root, TreeNode* subRoot) {
    if (!root) return false;
    if (isSameTree(root, subRoot)) return true;
    return isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
}

// LC 617
TreeNode* mergeTrees(TreeNode* t1, TreeNode* t2) {
    if (!t1) return t2;
    if (!t2) return t1;
    TreeNode* merged = new TreeNode(t1->val + t2->val);
    merged->left = mergeTrees(t1->left, t2->left);
    merged->right = mergeTrees(t1->right, t2->right);
    return merged;
}

void demo_misc() {
    cout << "\n===== Demo 6: 子树判断 & 合并 =====" << endl;

    // LC 572
    cout << "\n--- LC 572: 另一棵树的子树 ---" << endl;
    //       3
    //      / \
    //     4   5
    //    / \
    //   1   2
    TreeNode* t1 = buildTree({3, 4, 5, 1, 2});
    //     4
    //    / \
    //   1   2
    TreeNode* sub1 = buildTree({4, 1, 2});
    printTree(t1, "主树");
    printTree(sub1, "子树");
    cout << "是否是子树: " << (isSubtree(t1, sub1) ? "true" : "false")
         << " (期望=true)" << endl;

    TreeNode* sub2 = buildTree({4, 1, -1});
    printTree(sub2, "子树2");
    cout << "是否是子树: " << (isSubtree(t1, sub2) ? "true" : "false")
         << " (期望=false)" << endl;

    // LC 617
    cout << "\n--- LC 617: 合并二叉树 ---" << endl;
    //     1           2
    //    / \         / \
    //   3   2       1   3
    //  /              \   \
    // 5                4   7
    TreeNode* m1 = buildTree({1, 3, 2, 5});
    TreeNode* m2 = buildTree({2, 1, 3, -1, 4, -1, 7});
    printTree(m1, "树1");
    printTree(m2, "树2");
    TreeNode* merged = mergeTrees(m1, m2);
    printTree(merged, "合并后");
    // [3, 4, 5, 5, 4, null, 7]

    deleteTree(t1); deleteTree(sub1); deleteTree(sub2);
    // m1, m2 的节点已在 mergeTrees 中部分复用，只删 merged
    deleteTree(merged);
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题八（上）：二叉树基础与遍历 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_traversal();          // Demo 1: 三种遍历
    demo_level_order();        // Demo 2: 层序及变体
    demo_recursive_classic();  // Demo 3: 递归经典
    demo_path();               // Demo 4: 路径问题
    demo_construct();          // Demo 5: 构造二叉树
    demo_misc();               // Demo 6: 子树 & 合并

    cout << "\n============================================" << endl;
    cout << "  二叉树基础与遍历 Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 94, 100, 101, 102, 103," << endl;
    cout << "  104, 105, 106, 110, 111, 112, 113, 124," << endl;
    cout << "  144, 145, 199, 226, 257, 572, 617" << endl;
    cout << "============================================" << endl;

    return 0;
}
