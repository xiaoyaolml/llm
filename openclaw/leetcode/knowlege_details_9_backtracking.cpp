/**
 * ============================================================
 *   专题九：回溯算法（Backtracking）示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_9_backtracking knowlege_details_9_backtracking.cpp
 *   配合 knowlege_details_9_backtracking.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
using namespace std;

// ============================================================
//  打印辅助
// ============================================================
void printVec(const vector<int>& v) {
    cout << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        if (i) cout << ",";
        cout << v[i];
    }
    cout << "]";
}

void printVecVec(const vector<vector<int>>& vv, const string& label) {
    cout << label << " (" << vv.size() << " 个):" << endl;
    for (auto& v : vv) { cout << "  "; printVec(v); cout << endl; }
}

void printStrVec(const vector<string>& vs, const string& label) {
    cout << label << " (" << vs.size() << " 个):" << endl;
    for (auto& s : vs) cout << "  " << s << endl;
}

void printStrVecVec(const vector<vector<string>>& vvs, const string& label) {
    cout << label << " (" << vvs.size() << " 个):" << endl;
    for (auto& vs : vvs) {
        cout << "  [";
        for (int i = 0; i < (int)vs.size(); i++) {
            if (i) cout << ", ";
            cout << "\"" << vs[i] << "\"";
        }
        cout << "]" << endl;
    }
}

// ============================================================
//  Demo 1: 子集问题
//  LC 78: 子集（无重复）
//  LC 90: 子集 II（有重复）
// ============================================================

vector<vector<int>> subsets(vector<int>& nums) {
    vector<vector<int>> result;
    vector<int> path;
    function<void(int)> backtrack = [&](int start) {
        result.push_back(path);  // 每个节点都收集
        for (int i = start; i < (int)nums.size(); i++) {
            path.push_back(nums[i]);
            backtrack(i + 1);
            path.pop_back();
        }
    };
    backtrack(0);
    return result;
}

vector<vector<int>> subsetsWithDup(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    vector<int> path;
    function<void(int)> backtrack = [&](int start) {
        result.push_back(path);
        for (int i = start; i < (int)nums.size(); i++) {
            if (i > start && nums[i] == nums[i - 1]) continue;  // 同层去重
            path.push_back(nums[i]);
            backtrack(i + 1);
            path.pop_back();
        }
    };
    backtrack(0);
    return result;
}

void demo_subsets() {
    cout << "\n===== Demo 1: 子集问题 =====" << endl;

    // LC 78
    cout << "\n--- LC 78: 子集 ---" << endl;
    vector<int> nums1 = {1, 2, 3};
    auto res1 = subsets(nums1);
    printVecVec(res1, "[1,2,3] 的所有子集");
    // [], [1], [1,2], [1,2,3], [1,3], [2], [2,3], [3]

    // LC 90
    cout << "\n--- LC 90: 子集 II（含重复元素）---" << endl;
    vector<int> nums2 = {1, 2, 2};
    auto res2 = subsetsWithDup(nums2);
    printVecVec(res2, "[1,2,2] 的子集（去重）");
    // [], [1], [1,2], [1,2,2], [2], [2,2]

    cout << "对比：不去重 → ";
    auto res2raw = subsets(nums2);
    cout << res2raw.size() << " 个，去重后 → " << res2.size() << " 个" << endl;
}

// ============================================================
//  Demo 2: 排列问题
//  LC 46: 全排列
//  LC 47: 全排列 II（含重复）
// ============================================================

vector<vector<int>> permute(vector<int>& nums) {
    vector<vector<int>> result;
    vector<int> path;
    vector<bool> used(nums.size(), false);
    function<void()> backtrack = [&]() {
        if (path.size() == nums.size()) {
            result.push_back(path);
            return;
        }
        for (int i = 0; i < (int)nums.size(); i++) {
            if (used[i]) continue;
            used[i] = true;
            path.push_back(nums[i]);
            backtrack();
            path.pop_back();
            used[i] = false;
        }
    };
    backtrack();
    return result;
}

vector<vector<int>> permuteUnique(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    vector<int> path;
    vector<bool> used(nums.size(), false);
    function<void()> backtrack = [&]() {
        if (path.size() == nums.size()) {
            result.push_back(path);
            return;
        }
        for (int i = 0; i < (int)nums.size(); i++) {
            if (used[i]) continue;
            // 去重：同层中前一个相同的已撤销 → 跳过
            if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) continue;
            used[i] = true;
            path.push_back(nums[i]);
            backtrack();
            path.pop_back();
            used[i] = false;
        }
    };
    backtrack();
    return result;
}

void demo_permute() {
    cout << "\n===== Demo 2: 排列问题 =====" << endl;

    // LC 46
    cout << "\n--- LC 46: 全排列 ---" << endl;
    vector<int> nums1 = {1, 2, 3};
    auto res1 = permute(nums1);
    printVecVec(res1, "[1,2,3] 的全排列");
    // 6 个

    // LC 47
    cout << "\n--- LC 47: 全排列 II（含重复元素）---" << endl;
    vector<int> nums2 = {1, 1, 2};
    auto res2 = permuteUnique(nums2);
    printVecVec(res2, "[1,1,2] 的全排列（去重）");
    // [1,1,2], [1,2,1], [2,1,1] → 3 个

    cout << "对比：不去重 → ";
    auto res2raw = permute(nums2);
    cout << res2raw.size() << " 个，去重后 → " << res2.size() << " 个" << endl;
}

// ============================================================
//  Demo 3: 组合问题
//  LC 77: 组合
//  LC 216: 组合总和 III
// ============================================================

vector<vector<int>> combine(int n, int k) {
    vector<vector<int>> result;
    vector<int> path;
    function<void(int)> backtrack = [&](int start) {
        if ((int)path.size() == k) {
            result.push_back(path);
            return;
        }
        // 剪枝：剩余不够凑齐 k 个
        for (int i = start; i <= n - (k - (int)path.size()) + 1; i++) {
            path.push_back(i);
            backtrack(i + 1);
            path.pop_back();
        }
    };
    backtrack(1);
    return result;
}

vector<vector<int>> combinationSum3(int k, int n) {
    vector<vector<int>> result;
    vector<int> path;
    function<void(int, int)> backtrack = [&](int start, int remain) {
        if ((int)path.size() == k) {
            if (remain == 0) result.push_back(path);
            return;
        }
        for (int i = start; i <= 9; i++) {
            if (i > remain) break;  // 剪枝
            path.push_back(i);
            backtrack(i + 1, remain - i);
            path.pop_back();
        }
    };
    backtrack(1, n);
    return result;
}

void demo_combine() {
    cout << "\n===== Demo 3: 组合问题 =====" << endl;

    // LC 77
    cout << "\n--- LC 77: C(4,2) ---" << endl;
    auto res1 = combine(4, 2);
    printVecVec(res1, "C(4,2)");
    // [1,2],[1,3],[1,4],[2,3],[2,4],[3,4]

    cout << "\n--- C(5,3) ---" << endl;
    auto res1b = combine(5, 3);
    printVecVec(res1b, "C(5,3)");

    // LC 216
    cout << "\n--- LC 216: 组合总和 III ---" << endl;
    auto res2 = combinationSum3(3, 7);
    printVecVec(res2, "k=3, n=7");
    // [1,2,4]

    auto res3 = combinationSum3(3, 9);
    printVecVec(res3, "k=3, n=9");
    // [1,2,6],[1,3,5],[2,3,4]
}

// ============================================================
//  Demo 4: 组合总和系列
//  LC 39: 组合总和（可重复选）
//  LC 40: 组合总和 II（不可重复，有重复元素）
// ============================================================

vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> result;
    vector<int> path;
    function<void(int, int)> backtrack = [&](int start, int remain) {
        if (remain == 0) { result.push_back(path); return; }
        for (int i = start; i < (int)candidates.size(); i++) {
            if (candidates[i] > remain) break;
            path.push_back(candidates[i]);
            backtrack(i, remain - candidates[i]);  // i 不是 i+1！可重复
            path.pop_back();
        }
    };
    backtrack(0, target);
    return result;
}

vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> result;
    vector<int> path;
    function<void(int, int)> backtrack = [&](int start, int remain) {
        if (remain == 0) { result.push_back(path); return; }
        for (int i = start; i < (int)candidates.size(); i++) {
            if (candidates[i] > remain) break;
            if (i > start && candidates[i] == candidates[i - 1]) continue;
            path.push_back(candidates[i]);
            backtrack(i + 1, remain - candidates[i]);
            path.pop_back();
        }
    };
    backtrack(0, target);
    return result;
}

void demo_combination_sum() {
    cout << "\n===== Demo 4: 组合总和系列 =====" << endl;

    // LC 39
    cout << "\n--- LC 39: 组合总和（可重复选）---" << endl;
    vector<int> cand1 = {2, 3, 6, 7};
    auto res1 = combinationSum(cand1, 7);
    printVecVec(res1, "candidates=[2,3,6,7], target=7");
    // [2,2,3], [7]

    vector<int> cand1b = {2, 3, 5};
    auto res1b = combinationSum(cand1b, 8);
    printVecVec(res1b, "candidates=[2,3,5], target=8");
    // [2,2,2,2], [2,3,3], [3,5]

    // LC 40
    cout << "\n--- LC 40: 组合总和 II（不可重复，有重复元素）---" << endl;
    vector<int> cand2 = {10, 1, 2, 7, 6, 1, 5};
    auto res2 = combinationSum2(cand2, 8);
    printVecVec(res2, "candidates=[10,1,2,7,6,1,5], target=8");
    // [1,1,6], [1,2,5], [1,7], [2,6]

    cout << "\n  对比 LC 39 vs LC 40:" << endl;
    cout << "  LC 39: 允许重复选（传 i），无重复元素" << endl;
    cout << "  LC 40: 不可重复选（传 i+1），但输入有重复 → 需去重" << endl;
}

// ============================================================
//  Demo 5: 分割问题
//  LC 131: 分割回文串
//  LC 93:  复原 IP 地址
// ============================================================

vector<vector<string>> partition(string s) {
    int n = s.size();
    // 预处理回文判断
    vector<vector<bool>> dp(n, vector<bool>(n, false));
    for (int i = n - 1; i >= 0; i--)
        for (int j = i; j < n; j++)
            dp[i][j] = (s[i] == s[j]) && (j - i <= 2 || dp[i + 1][j - 1]);

    vector<vector<string>> result;
    vector<string> path;
    function<void(int)> backtrack = [&](int start) {
        if (start == n) {
            result.push_back(path);
            return;
        }
        for (int end = start; end < n; end++) {
            if (!dp[start][end]) continue;
            path.push_back(s.substr(start, end - start + 1));
            backtrack(end + 1);
            path.pop_back();
        }
    };
    backtrack(0);
    return result;
}

vector<string> restoreIpAddresses(string s) {
    vector<string> result;
    vector<string> segments;
    function<void(int)> backtrack = [&](int start) {
        if ((int)segments.size() == 4) {
            if (start == (int)s.size()) {
                string ip = segments[0];
                for (int i = 1; i < 4; i++) ip += "." + segments[i];
                result.push_back(ip);
            }
            return;
        }
        for (int len = 1; len <= 3 && start + len <= (int)s.size(); len++) {
            string seg = s.substr(start, len);
            if (seg.size() > 1 && seg[0] == '0') break;  // 前导零
            if (stoi(seg) > 255) break;
            segments.push_back(seg);
            backtrack(start + len);
            segments.pop_back();
        }
    };
    backtrack(0);
    return result;
}

void demo_partition() {
    cout << "\n===== Demo 5: 分割问题 =====" << endl;

    // LC 131
    cout << "\n--- LC 131: 分割回文串 ---" << endl;
    auto res1 = partition("aab");
    printStrVecVec(res1, "\"aab\" 的回文分割");
    // ["a","a","b"], ["aa","b"]

    auto res1b = partition("abcba");
    printStrVecVec(res1b, "\"abcba\" 的回文分割");

    // LC 93
    cout << "\n--- LC 93: 复原 IP 地址 ---" << endl;
    auto res2 = restoreIpAddresses("25525511135");
    printStrVec(res2, "\"25525511135\"");
    // "255.255.11.135", "255.255.111.35"

    auto res3 = restoreIpAddresses("0000");
    printStrVec(res3, "\"0000\"");
    // "0.0.0.0"

    auto res4 = restoreIpAddresses("101023");
    printStrVec(res4, "\"101023\"");
}

// ============================================================
//  Demo 6: 电话号码 & 括号生成
//  LC 17: 电话号码的字母组合
//  LC 22: 括号生成
// ============================================================

vector<string> letterCombinations(string digits) {
    if (digits.empty()) return {};
    vector<string> mapping = {"", "", "abc", "def", "ghi", "jkl",
                               "mno", "pqrs", "tuv", "wxyz"};
    vector<string> result;
    string path;
    function<void(int)> backtrack = [&](int idx) {
        if (idx == (int)digits.size()) {
            result.push_back(path);
            return;
        }
        for (char c : mapping[digits[idx] - '0']) {
            path.push_back(c);
            backtrack(idx + 1);
            path.pop_back();
        }
    };
    backtrack(0);
    return result;
}

vector<string> generateParenthesis(int n) {
    vector<string> result;
    string path;
    function<void(int, int)> backtrack = [&](int open, int close) {
        if ((int)path.size() == 2 * n) {
            result.push_back(path);
            return;
        }
        if (open < n) {
            path.push_back('(');
            backtrack(open + 1, close);
            path.pop_back();
        }
        if (close < open) {
            path.push_back(')');
            backtrack(open, close + 1);
            path.pop_back();
        }
    };
    backtrack(0, 0);
    return result;
}

void demo_phone_parenthesis() {
    cout << "\n===== Demo 6: 电话号码 & 括号生成 =====" << endl;

    // LC 17
    cout << "\n--- LC 17: 电话号码的字母组合 ---" << endl;
    auto res1 = letterCombinations("23");
    printStrVec(res1, "digits=\"23\"");
    // ad ae af bd be bf cd ce cf → 9个

    auto res1b = letterCombinations("79");
    printStrVec(res1b, "digits=\"79\"");
    // 4×4 = 16个

    // LC 22
    cout << "\n--- LC 22: 括号生成 ---" << endl;
    for (int n = 1; n <= 4; n++) {
        auto res = generateParenthesis(n);
        printStrVec(res, "n=" + to_string(n));
    }
}

// ============================================================
//  Demo 7: N 皇后
//  LC 51
// ============================================================

vector<vector<string>> solveNQueens(int n) {
    vector<vector<string>> result;
    vector<string> board(n, string(n, '.'));
    vector<bool> cols(n, false);
    vector<bool> diag1(2 * n, false);  // 主对角线 row-col+n
    vector<bool> diag2(2 * n, false);  // 副对角线 row+col

    function<void(int)> backtrack = [&](int row) {
        if (row == n) {
            result.push_back(board);
            return;
        }
        for (int col = 0; col < n; col++) {
            if (cols[col] || diag1[row - col + n] || diag2[row + col])
                continue;
            board[row][col] = 'Q';
            cols[col] = diag1[row - col + n] = diag2[row + col] = true;
            backtrack(row + 1);
            board[row][col] = '.';
            cols[col] = diag1[row - col + n] = diag2[row + col] = false;
        }
    };
    backtrack(0);
    return result;
}

void demo_nqueens() {
    cout << "\n===== Demo 7: N 皇后 =====" << endl;

    for (int n = 4; n <= 8; n++) {
        auto solutions = solveNQueens(n);
        cout << n << " 皇后: " << solutions.size() << " 种解法" << endl;
    }

    // 展示 4 皇后的所有解
    cout << "\n4 皇后的两种解法：" << endl;
    auto sol4 = solveNQueens(4);
    for (int s = 0; s < (int)sol4.size(); s++) {
        cout << "  解法 " << s + 1 << ":" << endl;
        for (auto& row : sol4[s])
            cout << "    " << row << endl;
        cout << endl;
    }
}

// ============================================================
//  Demo 8: 单词搜索
//  LC 79
// ============================================================

bool exist(vector<vector<char>>& board, string word) {
    int m = board.size(), n = board[0].size();
    function<bool(int, int, int)> dfs = [&](int i, int j, int k) -> bool {
        if (k == (int)word.size()) return true;
        if (i < 0 || i >= m || j < 0 || j >= n) return false;
        if (board[i][j] != word[k]) return false;

        char tmp = board[i][j];
        board[i][j] = '#';  // 标记已访问
        bool found = dfs(i + 1, j, k + 1) || dfs(i - 1, j, k + 1)
                  || dfs(i, j + 1, k + 1) || dfs(i, j - 1, k + 1);
        board[i][j] = tmp;  // 回溯
        return found;
    };

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (dfs(i, j, 0)) return true;
    return false;
}

void demo_word_search() {
    cout << "\n===== Demo 8: 单词搜索 (LC 79) =====" << endl;

    vector<vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}
    };

    cout << "棋盘:" << endl;
    for (auto& row : board) {
        cout << "  ";
        for (char c : row) cout << c << " ";
        cout << endl;
    }

    vector<pair<string, bool>> tests = {
        {"ABCCED", true},
        {"SEE", true},
        {"ABCB", false},
        {"ABCESCFS", true},
        {"ABCESFDA", true}
    };

    for (auto& [word, expected] : tests) {
        bool result = exist(board, word);
        cout << "  \"" << word << "\": " << (result ? "true" : "false")
             << (result == expected ? " ✓" : " ✗") << endl;
    }
}

// ============================================================
//  Demo 9: 回溯模板对比总结
// ============================================================
void demo_summary() {
    cout << "\n===== Demo 9: 回溯模板对比总结 =====" << endl;

    cout << R"(
┌──────────┬───────────┬───────────┬───────────┐
│   类型   │   start   │  used[]   │  收集时机  │
├──────────┼───────────┼───────────┼───────────┤
│   排列   │    无     │    ✓     │   叶子    │
│   组合   │    ✓     │    无     │   叶子    │
│   子集   │    ✓     │    无     │  每个节点  │
│ 可重复组合│   ✓(传i) │    无     │   叶子    │
└──────────┴───────────┴───────────┴───────────┘

去重条件：
  组合/子集：i > start && nums[i] == nums[i-1]
  排  列  ：i > 0 && nums[i] == nums[i-1] && !used[i-1]

剪枝优化：
  值剪枝：candidates[i] > remain → break
  长度剪枝：i <= n - (k - path.size()) + 1
  合法性剪枝：isValid() → 棋盘类

)" << endl;

    // 实际数据统计
    cout << "各类问题结果数量统计:" << endl;
    vector<int> nums3 = {1, 2, 3};
    vector<int> nums4 = {1, 2, 3, 4};

    cout << "  [1,2,3]:" << endl;
    cout << "    全排列: " << permute(nums3).size() << " (3! = 6)" << endl;
    cout << "    子集:   " << subsets(nums3).size() << " (2^3 = 8)" << endl;
    cout << "    C(3,2): " << combine(3, 2).size() << " (C(3,2) = 3)" << endl;

    cout << "  [1,2,3,4]:" << endl;
    cout << "    全排列: " << permute(nums4).size() << " (4! = 24)" << endl;
    cout << "    子集:   " << subsets(nums4).size() << " (2^4 = 16)" << endl;
    cout << "    C(4,2): " << combine(4, 2).size() << " (C(4,2) = 6)" << endl;
    cout << "    C(4,3): " << combine(4, 3).size() << " (C(4,3) = 4)" << endl;
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题九：回溯算法 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_subsets();              // Demo 1: 子集
    demo_permute();             // Demo 2: 排列
    demo_combine();             // Demo 3: 组合
    demo_combination_sum();     // Demo 4: 组合总和
    demo_partition();           // Demo 5: 分割
    demo_phone_parenthesis();   // Demo 6: 电话号码 & 括号
    demo_nqueens();             // Demo 7: N 皇后
    demo_word_search();         // Demo 8: 单词搜索
    demo_summary();             // Demo 9: 模板对比

    cout << "\n============================================" << endl;
    cout << "  回溯算法 Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 17, 22, 39, 40, 46, 47," << endl;
    cout << "  51, 77, 78, 79, 90, 93, 131, 216" << endl;
    cout << "============================================" << endl;

    return 0;
}
