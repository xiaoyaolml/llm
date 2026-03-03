/**
 * ============================================================
 *   专题八（下）：BST（二叉搜索树）与进阶树题 示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_8_tree_bst_advanced knowlege_details_8_tree_bst_advanced.cpp
 *   配合 knowlege_details_8_tree_bst_advanced.md 食用更佳
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
#include <sstream>
using namespace std;

// ============================================================
//  TreeNode 定义 & 工具（同上篇）
// ============================================================
struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

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

void printInorder(TreeNode* root, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    vector<int> result;
    function<void(TreeNode*)> inorder = [&](TreeNode* node) {
        if (!node) return;
        inorder(node->left);
        result.push_back(node->val);
        inorder(node->right);
    };
    inorder(root);
    cout << "[";
    for (int i = 0; i < (int)result.size(); i++) {
        if (i) cout << ", ";
        cout << result[i];
    }
    cout << "]" << endl;
}

void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// ============================================================
//  Demo 1: BST 搜索与验证
//  LC 700: 二叉搜索树中的搜索
//  LC 98:  验证二叉搜索树
// ============================================================

TreeNode* searchBST(TreeNode* root, int val) {
    while (root && root->val != val) {
        root = val < root->val ? root->left : root->right;
    }
    return root;
}

bool isValidBST(TreeNode* root, long lo = LONG_MIN, long hi = LONG_MAX) {
    if (!root) return true;
    if (root->val <= lo || root->val >= hi) return false;
    return isValidBST(root->left, lo, root->val)
        && isValidBST(root->right, root->val, hi);
}

void demo_search_validate() {
    cout << "\n===== Demo 1: BST 搜索与验证 =====" << endl;

    //        8
    //       / \
    //      3   10
    //     / \    \
    //    1   6   14
    //       / \  /
    //      4   7 13
    TreeNode* bst = buildTree({8, 3, 10, 1, 6, -1, 14, -1, -1, 4, 7, 13});
    printTree(bst, "BST");
    printInorder(bst, "中序（应有序）");

    // LC 700
    cout << "\n--- LC 700: 搜索 BST ---" << endl;
    TreeNode* found = searchBST(bst, 6);
    if (found) printTree(found, "搜索 6 → 找到子树");
    found = searchBST(bst, 5);
    cout << "搜索 5 → " << (found ? "找到" : "未找到") << endl;

    // LC 98
    cout << "\n--- LC 98: 验证 BST ---" << endl;
    cout << "合法 BST: " << (isValidBST(bst) ? "true" : "false")
         << " (期望=true)" << endl;

    // 非法 BST: 左子树中有节点 > 根
    //     5
    //    / \
    //   1   4
    //      / \
    //     3   6
    TreeNode* bad = buildTree({5, 1, 4, -1, -1, 3, 6});
    printTree(bad, "非法树");
    cout << "合法 BST: " << (isValidBST(bad) ? "true" : "false")
         << " (期望=false, 3<5 但在右子树)" << endl;

    deleteTree(bst);
    deleteTree(bad);
}

// ============================================================
//  Demo 2: BST 插入与删除
//  LC 701: 二叉搜索树中的插入操作
//  LC 450: 删除二叉搜索树中的节点
// ============================================================

TreeNode* insertIntoBST(TreeNode* root, int val) {
    if (!root) return new TreeNode(val);
    if (val < root->val)
        root->left = insertIntoBST(root->left, val);
    else
        root->right = insertIntoBST(root->right, val);
    return root;
}

TreeNode* deleteNode(TreeNode* root, int key) {
    if (!root) return nullptr;
    if (key < root->val) {
        root->left = deleteNode(root->left, key);
    } else if (key > root->val) {
        root->right = deleteNode(root->right, key);
    } else {
        // 找到目标
        if (!root->left) {
            TreeNode* ret = root->right;
            delete root;
            return ret;
        }
        if (!root->right) {
            TreeNode* ret = root->left;
            delete root;
            return ret;
        }
        // 两个孩子 → 找右子树最小值（后继）
        TreeNode* successor = root->right;
        while (successor->left) successor = successor->left;
        root->val = successor->val;
        root->right = deleteNode(root->right, successor->val);
    }
    return root;
}

void demo_insert_delete() {
    cout << "\n===== Demo 2: BST 插入与删除 =====" << endl;

    //     4
    //    / \
    //   2   7
    //  / \
    // 1   3
    TreeNode* bst = buildTree({4, 2, 7, 1, 3});
    printTree(bst, "原始 BST");

    // 插入 5
    cout << "\n--- LC 701: 插入 5 ---" << endl;
    bst = insertIntoBST(bst, 5);
    printTree(bst, "插入后");
    printInorder(bst, "中序验证");

    // 删除叶节点 1
    cout << "\n--- LC 450: 删除 1（叶节点）---" << endl;
    bst = deleteNode(bst, 1);
    printTree(bst, "删除后");

    // 删除有一个孩子的节点 7
    cout << "\n--- 删除 7（有一个孩子 5）---" << endl;
    bst = deleteNode(bst, 7);
    printTree(bst, "删除后");

    // 重建一棵树，测试删除有两个孩子
    TreeNode* bst2 = buildTree({5, 3, 8, 2, 4, 6, 9});
    printTree(bst2, "\n新 BST");
    cout << "\n--- 删除 5（两个孩子，后继=6）---" << endl;
    bst2 = deleteNode(bst2, 5);
    printTree(bst2, "删除后");
    printInorder(bst2, "中序验证");

    deleteTree(bst);
    deleteTree(bst2);
}

// ============================================================
//  Demo 3: BST 中序有序性
//  LC 230: 第K小的元素
//  LC 538: BST 转累加树
// ============================================================

int kthSmallest(TreeNode* root, int k) {
    stack<TreeNode*> stk;
    TreeNode* cur = root;
    while (cur || !stk.empty()) {
        while (cur) { stk.push(cur); cur = cur->left; }
        cur = stk.top(); stk.pop();
        if (--k == 0) return cur->val;
        cur = cur->right;
    }
    return -1;
}

TreeNode* convertBST(TreeNode* root) {
    int sum = 0;
    function<void(TreeNode*)> dfs = [&](TreeNode* node) {
        if (!node) return;
        dfs(node->right);      // 先右
        sum += node->val;
        node->val = sum;
        dfs(node->left);       // 后左
    };
    dfs(root);
    return root;
}

void demo_inorder_app() {
    cout << "\n===== Demo 3: BST 中序有序性应用 =====" << endl;

    //        5
    //       / \
    //      3   6
    //     / \   \
    //    2   4   8
    //   /       /
    //  1       7
    TreeNode* bst = buildTree({5, 3, 6, 2, 4, -1, 8, 1, -1, -1, -1, 7});
    printTree(bst, "BST");
    printInorder(bst, "中序");

    // LC 230
    cout << "\n--- LC 230: 第K小的元素 ---" << endl;
    for (int k = 1; k <= 4; k++)
        cout << "  k=" << k << " → " << kthSmallest(bst, k) << endl;
    // k=1→1, k=2→2, k=3→3, k=4→4

    // LC 538
    cout << "\n--- LC 538: BST 转累加树 ---" << endl;
    //     4
    //    / \
    //   1   6
    //  / \ / \
    // 0  2 5  7
    //    \    \
    //     3    8
    TreeNode* t2 = buildTree({4, 1, 6, 0, 2, 5, 7, -1, -1, -1, 3, -1, -1, -1, 8});
    printTree(t2, "原始");
    printInorder(t2, "中序");
    convertBST(t2);
    printTree(t2, "累加后");
    printInorder(t2, "中序（应递减）");

    deleteTree(bst);
    deleteTree(t2);
}

// ============================================================
//  Demo 4: 最近公共祖先 LCA
//  LC 235: BST 的 LCA
//  LC 236: 二叉树的 LCA
// ============================================================

// LC 235: BST 版本
TreeNode* lcaBST(TreeNode* root, TreeNode* p, TreeNode* q) {
    while (root) {
        if (p->val < root->val && q->val < root->val)
            root = root->left;
        else if (p->val > root->val && q->val > root->val)
            root = root->right;
        else
            return root;
    }
    return nullptr;
}

// LC 236: 通用版本
TreeNode* lcaGeneral(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;
    TreeNode* left = lcaGeneral(root->left, p, q);
    TreeNode* right = lcaGeneral(root->right, p, q);
    if (left && right) return root;
    return left ? left : right;
}

// 辅助：在树中找值为 val 的节点（用于构造 p、q）
TreeNode* findNode(TreeNode* root, int val) {
    if (!root) return nullptr;
    if (root->val == val) return root;
    TreeNode* left = findNode(root->left, val);
    return left ? left : findNode(root->right, val);
}

void demo_lca() {
    cout << "\n===== Demo 4: 最近公共祖先 LCA =====" << endl;

    // BST
    //        6
    //       / \
    //      2   8
    //     / \ / \
    //    0  4 7  9
    //      / \
    //     3   5
    TreeNode* bst = buildTree({6, 2, 8, 0, 4, 7, 9, -1, -1, 3, 5});
    printTree(bst, "BST");

    // LC 235
    cout << "\n--- LC 235: BST LCA ---" << endl;
    TreeNode* p = findNode(bst, 2), *q = findNode(bst, 8);
    cout << "LCA(2,8) = " << lcaBST(bst, p, q)->val << " (期望=6)" << endl;

    p = findNode(bst, 2); q = findNode(bst, 4);
    cout << "LCA(2,4) = " << lcaBST(bst, p, q)->val << " (期望=2)" << endl;

    p = findNode(bst, 3); q = findNode(bst, 5);
    cout << "LCA(3,5) = " << lcaBST(bst, p, q)->val << " (期望=4)" << endl;

    // LC 236: 通用二叉树
    cout << "\n--- LC 236: 通用 LCA ---" << endl;
    //        3
    //       / \
    //      5   1
    //     / \ / \
    //    6  2 0  8
    //      / \
    //     7   4
    TreeNode* tree = buildTree({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});
    printTree(tree, "树");

    p = findNode(tree, 5); q = findNode(tree, 1);
    cout << "LCA(5,1) = " << lcaGeneral(tree, p, q)->val << " (期望=3)" << endl;

    p = findNode(tree, 5); q = findNode(tree, 4);
    cout << "LCA(5,4) = " << lcaGeneral(tree, p, q)->val << " (期望=5)" << endl;

    p = findNode(tree, 7); q = findNode(tree, 4);
    cout << "LCA(7,4) = " << lcaGeneral(tree, p, q)->val << " (期望=2)" << endl;

    deleteTree(bst);
    deleteTree(tree);
}

// ============================================================
//  Demo 5: 序列化与反序列化
//  LC 297
// ============================================================

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
        if (!token.empty()) tokens.push(token);
        return buildFromTokens(tokens);
    }

private:
    TreeNode* buildFromTokens(queue<string>& tokens) {
        if (tokens.empty()) return nullptr;
        string val = tokens.front(); tokens.pop();
        if (val == "#") return nullptr;
        TreeNode* node = new TreeNode(stoi(val));
        node->left = buildFromTokens(tokens);
        node->right = buildFromTokens(tokens);
        return node;
    }
};

void demo_serialize() {
    cout << "\n===== Demo 5: 序列化与反序列化 =====" << endl;

    //      1
    //     / \
    //    2   3
    //       / \
    //      4   5
    TreeNode* root = buildTree({1, 2, 3, -1, -1, 4, 5});
    printTree(root, "原始");

    Codec codec;
    string encoded = codec.serialize(root);
    cout << "序列化: " << encoded << endl;

    TreeNode* decoded = codec.deserialize(encoded);
    printTree(decoded, "反序列化");

    // 验证一致性
    string reEncoded = codec.serialize(decoded);
    cout << "一致性: " << (encoded == reEncoded ? "通过✓" : "失败✗") << endl;

    // 边界：空树
    TreeNode* empty = nullptr;
    string emptyStr = codec.serialize(empty);
    cout << "\n空树序列化: " << emptyStr << endl;
    TreeNode* emptyDecoded = codec.deserialize(emptyStr);
    printTree(emptyDecoded, "空树反序列化");

    // 含负数
    TreeNode* neg = buildTree({-1, -2, 3});
    string negStr = codec.serialize(neg);
    cout << "\n含负数: " << negStr << endl;
    TreeNode* negDecoded = codec.deserialize(negStr);
    printTree(negDecoded, "反序列化");

    deleteTree(root); deleteTree(decoded);
    deleteTree(neg); deleteTree(negDecoded);
}

// ============================================================
//  Demo 6: 展平 & 直径
//  LC 114: 二叉树展开为链表
//  LC 543: 二叉树的直径
// ============================================================

void flatten(TreeNode* root) {
    TreeNode* cur = root;
    while (cur) {
        if (cur->left) {
            TreeNode* pre = cur->left;
            while (pre->right) pre = pre->right;
            pre->right = cur->right;
            cur->right = cur->left;
            cur->left = nullptr;
        }
        cur = cur->right;
    }
}

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

void demo_flatten_diameter() {
    cout << "\n===== Demo 6: 展平 & 直径 =====" << endl;

    // LC 114
    cout << "\n--- LC 114: 展开为链表 ---" << endl;
    //      1
    //     / \
    //    2   5
    //   / \   \
    //  3   4   6
    TreeNode* t1 = buildTree({1, 2, 5, 3, 4, -1, 6});
    printTree(t1, "原始");
    flatten(t1);
    // 打印链表
    cout << "展平后: ";
    TreeNode* cur = t1;
    while (cur) {
        cout << cur->val;
        if (cur->right) cout << " → ";
        cur = cur->right;
    }
    cout << endl;
    // 1 → 2 → 3 → 4 → 5 → 6

    // LC 543
    cout << "\n--- LC 543: 二叉树的直径 ---" << endl;
    //       1
    //      / \
    //     2   3
    //    / \
    //   4   5
    TreeNode* t2 = buildTree({1, 2, 3, 4, 5});
    printTree(t2, "树");
    cout << "直径: " << diameterOfBinaryTree(t2) << " (期望=3, 路径 4→2→1→3)" << endl;

    // 直径不经过根
    //         1
    //        /
    //       2
    //      / \
    //     3   4
    //    /     \
    //   5       6
    TreeNode* t3 = buildTree({1, 2, -1, 3, 4, 5, -1, -1, 6});
    printTree(t3, "树");
    cout << "直径: " << diameterOfBinaryTree(t3) << " (直径不经过根节点)" << endl;

    deleteTree(t1); deleteTree(t2); deleteTree(t3);
}

// ============================================================
//  Demo 7: 有序数组转 BST & 打家劫舍 III
//  LC 108: 有序数组转 BST
//  LC 337: 打家劫舍 III
// ============================================================

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

pair<int,int> robHelper(TreeNode* root) {
    // 返回 {不偷当前, 偷当前}
    if (!root) return {0, 0};
    auto [ln, ly] = robHelper(root->left);
    auto [rn, ry] = robHelper(root->right);
    int notRob = max(ln, ly) + max(rn, ry);
    int doRob = root->val + ln + rn;
    return {notRob, doRob};
}

int rob(TreeNode* root) {
    auto [n, y] = robHelper(root);
    return max(n, y);
}

void demo_array_to_bst_rob() {
    cout << "\n===== Demo 7: 有序数组转BST & 打家劫舍III =====" << endl;

    // LC 108
    cout << "\n--- LC 108: 有序数组转 BST ---" << endl;
    vector<int> nums1 = {-10, -3, 0, 5, 9};
    printVec(nums1, "有序数组");
    TreeNode* bst1 = sortedArrayToBST(nums1);
    printTree(bst1, "构造的 BST");
    printInorder(bst1, "中序验证");

    vector<int> nums2 = {1, 2, 3, 4, 5, 6, 7};
    printVec(nums2, "\n有序数组");
    TreeNode* bst2 = sortedArrayToBST(nums2);
    printTree(bst2, "构造的 BST");

    // LC 337
    cout << "\n--- LC 337: 打家劫舍 III ---" << endl;
    //      3
    //     / \
    //    2   3
    //     \   \
    //      3   1
    TreeNode* house1 = buildTree({3, 2, 3, -1, 3, -1, 1});
    printTree(house1, "房子1");
    cout << "最大金额: " << rob(house1) << " (期望=7, 偷 3+3+1)" << endl;

    //       3
    //      / \
    //     4   5
    //    / \   \
    //   1   3   1
    TreeNode* house2 = buildTree({3, 4, 5, 1, 3, -1, 1});
    printTree(house2, "房子2");
    cout << "最大金额: " << rob(house2) << " (期望=9, 偷 4+5)" << endl;

    // 单节点
    TreeNode* house3 = new TreeNode(100);
    cout << "单节点100: " << rob(house3) << " (期望=100)" << endl;

    deleteTree(bst1); deleteTree(bst2);
    deleteTree(house1); deleteTree(house2); deleteTree(house3);
}

// ============================================================
//  Demo 8: 综合小测 — 多种操作串联
// ============================================================
void demo_comprehensive() {
    cout << "\n===== Demo 8: 综合测试 =====" << endl;

    cout << "从空树开始，依次插入构建 BST:" << endl;
    TreeNode* bst = nullptr;
    vector<int> insertOrder = {5, 3, 7, 1, 4, 6, 8, 2};

    for (int val : insertOrder) {
        bst = insertIntoBST(bst, val);
        cout << "  插入 " << val << " → ";
        printInorder(bst, "中序");
    }

    // 验证
    cout << "\n合法 BST: " << (isValidBST(bst) ? "true" : "false") << endl;
    printTree(bst, "层序");

    // 查找第 k 小
    for (int k : {1, 3, 5, 8}) {
        cout << "第 " << k << " 小: " << kthSmallest(bst, k) << endl;
    }

    // LCA
    TreeNode* p = findNode(bst, 1), *q = findNode(bst, 4);
    cout << "\nLCA(1,4) = " << lcaGeneral(bst, p, q)->val << " (期望=3)" << endl;
    p = findNode(bst, 2); q = findNode(bst, 8);
    cout << "LCA(2,8) = " << lcaGeneral(bst, p, q)->val << " (期望=5)" << endl;

    // 删除
    cout << "\n删除根节点 5:" << endl;
    bst = deleteNode(bst, 5);
    printTree(bst, "删除后");
    printInorder(bst, "中序");
    cout << "仍合法: " << (isValidBST(bst) ? "true" : "false") << endl;

    // 序列化
    Codec codec;
    string encoded = codec.serialize(bst);
    cout << "\n序列化: " << encoded << endl;
    TreeNode* restored = codec.deserialize(encoded);
    printTree(restored, "恢复");

    // 直径
    cout << "直径: " << diameterOfBinaryTree(bst) << endl;

    deleteTree(bst); deleteTree(restored);
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题八（下）：BST与进阶树题 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_search_validate();     // Demo 1: BST 搜索与验证
    demo_insert_delete();       // Demo 2: BST 插入与删除
    demo_inorder_app();         // Demo 3: 中序有序性应用
    demo_lca();                 // Demo 4: LCA
    demo_serialize();           // Demo 5: 序列化
    demo_flatten_diameter();    // Demo 6: 展平 & 直径
    demo_array_to_bst_rob();   // Demo 7: 转BST & 打家劫舍
    demo_comprehensive();       // Demo 8: 综合测试

    cout << "\n============================================" << endl;
    cout << "  BST与进阶树题 Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 98, 108, 114, 230, 235," << endl;
    cout << "  236, 297, 337, 450, 538, 543, 700, 701" << endl;
    cout << "============================================" << endl;

    return 0;
}
