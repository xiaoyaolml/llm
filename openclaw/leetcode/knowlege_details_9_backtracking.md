# 专题九：回溯算法（Backtracking）完全指南

> 作者：大胖超 😜
> 配合 `knowlege_details_9_backtracking.cpp` 食用更佳

---

## 目录

1. [回溯算法本质](#1-回溯算法本质)
2. [万能模板与决策树](#2-万能模板与决策树)
3. [排列问题](#3-排列问题)
4. [组合问题](#4-组合问题)
5. [子集问题](#5-子集问题)
6. [去重技巧（含重复元素）](#6-去重技巧含重复元素)
7. [组合总和系列](#7-组合总和系列)
8. [分割问题](#8-分割问题)
9. [棋盘问题](#9-棋盘问题)
10. [排列/组合/子集速查对照表](#10-排列组合子集速查对照表)
11. [剪枝优化策略](#11-剪枝优化策略)
12. [综合刷题清单](#12-综合刷题清单)
13. [易错点与调试技巧](#13-易错点与调试技巧)

---

## 1. 回溯算法本质

### 一句话定义

> **回溯 = 暴力穷举 + 剪枝**

回溯算法本质上是 **DFS（深度优先搜索）** 在**决策树**上的应用。它尝试所有可能的选择，不满足条件时"回头"（撤销选择），换一条路继续探索。

### 回溯 vs 递归 vs DFS

| 概念 | 说明 |
|------|------|
| 递归 | 一种编程技巧，函数调用自身 |
| DFS | 一种搜索策略，深入到底再回头 |
| 回溯 | **DFS + 做选择 / 撤销选择** 的决策过程 |

> 回溯**一定**用到递归，但递归不一定是回溯。回溯的核心在于**"选择→探索→撤销"**这套三步曲。

### 回溯能解决的问题

| 问题类型 | 典型题目 |
|----------|---------|
| 排列 | 全排列 LC 46/47 |
| 组合 | 组合 LC 77、组合总和 LC 39/40/216 |
| 子集 | 子集 LC 78/90 |
| 分割 | 分割回文串 LC 131、复原 IP LC 93 |
| 棋盘 | N 皇后 LC 51、解数独 LC 37 |
| 路径 | 单词搜索 LC 79 |
| 其他 | 括号生成 LC 22、电话号码字母组合 LC 17 |

### 时间复杂度特点

回溯算法的时间复杂度通常是**指数级**的：

| 问题 | 时间复杂度 |
|------|-----------|
| 排列 n! | O(n! × n) |
| 组合 C(n,k) | O(C(n,k) × k) |
| 子集 2ⁿ | O(2ⁿ × n) |
| N 皇后 | O(n!) |

> 所以回溯的核心优化手段是**剪枝**——尽早排除不可能的分支

---

## 2. 万能模板与决策树

### 万能模板 🎯

```cpp
void backtrack(路径 path, 选择列表 choices) {
    if (满足结束条件) {
        result.push_back(path);  // 收集结果
        return;
    }
    
    for (选择 : 选择列表) {
        // 剪枝（可选）
        if (不合法) continue;
        
        做选择;                    // path.push_back(choice)
        backtrack(路径, 选择列表);  // 递归进入下一层
        撤销选择;                  // path.pop_back()
    }
}
```

### 三个核心要素

| 要素 | 说明 | 对应代码 |
|------|------|---------|
| **路径** | 已经做出的选择 | `path` 变量 |
| **选择列表** | 当前还能做的选择 | `for` 循环遍历的范围 |
| **结束条件** | 到达决策树底部 | `if` 判断收集结果 |

### 决策树可视化

以 `[1,2,3]` 的全排列为例：

```
                        []
                 /       |       \
              [1]       [2]      [3]        ← 第1层：选第1个数
             /   \     /   \    /   \
          [1,2] [1,3] [2,1] [2,3] [3,1] [3,2]  ← 第2层：选第2个数
            |     |     |     |     |     |
         [1,2,3] [1,3,2] [2,1,3] [2,3,1] [3,1,2] [3,2,1]  ← 第3层：叶子=结果
```

**在这棵树中：**
- 每个**节点** = 当前路径 `path`
- 每条**边** = 一次选择
- 每片**叶子** = 一个完整结果
- **回溯** = 从子节点返回父节点时，撤销最后一次选择

---

## 3. 排列问题

### LC 46: 全排列（无重复元素）

**思路：** 每层遍历所有数字，跳过已使用的

```cpp
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
```

**关键点：**
- 没有 `start` 参数，每次从 0 开始遍历
- 用 `used[]` 数组标记已选元素
- 排列关心**顺序**，`[1,2]` 和 `[2,1]` 是不同排列

### LC 47: 全排列 II（含重复元素）

**关键去重条件：**

```cpp
if (i > 0 && nums[i] == nums[i-1] && !used[i-1]) continue;
```

```cpp
vector<vector<int>> permuteUnique(vector<int>& nums) {
    sort(nums.begin(), nums.end());  // 排序是前提！
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
            // 去重：同层中相同值，只选第一个
            if (i > 0 && nums[i] == nums[i-1] && !used[i-1]) continue;
            
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
```

**去重原理图解 `[1,1,2]`：**

```
第一层选择：
  选 nums[0]=1 ✓   选 nums[1]=1 ✗（同层重复）   选 nums[2]=2 ✓
       |                                              |
  第二层...                                       第二层...

为什么 !used[i-1]？
  如果 used[i-1]=true → 说明 nums[i-1] 是上一层选的（纵向），允许
  如果 used[i-1]=false → 说明 nums[i-1] 在同层被撤销了（横向），跳过
```

---

## 4. 组合问题

### LC 77: 组合

**与排列的关键区别：** 有 `start` 参数，只看后面的元素，避免重复

```cpp
vector<vector<int>> combine(int n, int k) {
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int)> backtrack = [&](int start) {
        if ((int)path.size() == k) {
            result.push_back(path);
            return;
        }
        // 剪枝：剩余元素不够
        for (int i = start; i <= n - (k - (int)path.size()) + 1; i++) {
            path.push_back(i);
            backtrack(i + 1);
            path.pop_back();
        }
    };
    backtrack(1);
    return result;
}
```

**剪枝分析：**

```
n=4, k=2 时：
  不剪枝：i 从 start 到 4
  剪枝后：i 从 start 到 n-(k-path.size())+1
  
当 path.size()=0 时：i ≤ 4-(2-0)+1 = 3
  即 i 遍历 1,2,3（i=4 时只剩一个数，凑不够2个）

当 path.size()=1 时：i ≤ 4-(2-1)+1 = 4
  正常遍历
```

### 排列 vs 组合核心区别

| 特征 | 排列 | 组合 |
|------|------|------|
| 顺序 | 有序，[1,2] ≠ [2,1] | 无序，[1,2] = [2,1] |
| for 循环 | 从 0 开始 | 从 start 开始 |
| 状态标记 | used[] 数组 | start 参数 |
| 结果数 | n! / (n-k)! | C(n,k) |

---

## 5. 子集问题

### LC 78: 子集（无重复元素）

**关键差异：** 不需要结束条件判断，**每个节点都是一个子集**

```cpp
vector<vector<int>> subsets(vector<int>& nums) {
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int)> backtrack = [&](int start) {
        result.push_back(path);  // 所有节点都收集
        for (int i = start; i < (int)nums.size(); i++) {
            path.push_back(nums[i]);
            backtrack(i + 1);
            path.pop_back();
        }
    };
    backtrack(0);
    return result;
}
```

### LC 90: 子集 II（含重复元素）

```cpp
vector<vector<int>> subsetsWithDup(vector<int>& nums) {
    sort(nums.begin(), nums.end());  // 排序！
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int)> backtrack = [&](int start) {
        result.push_back(path);
        for (int i = start; i < (int)nums.size(); i++) {
            if (i > start && nums[i] == nums[i-1]) continue;  // 同层去重
            path.push_back(nums[i]);
            backtrack(i + 1);
            path.pop_back();
        }
    };
    backtrack(0);
    return result;
}
```

### 子集 vs 组合

| 对比 | 子集 | 组合 |
|------|------|------|
| 结果收集 | 每个节点都收集 | 只在叶子收集 |
| 结束条件 | 无（自然终止） | `path.size() == k` |
| 本质 | 组合的超集 | 固定长度的子集 |

**它们共享相同的遍历框架**，只是收集时机不同！

---

## 6. 去重技巧（含重复元素）

### 去重的通用策略

```
1. 排序（前提条件）
2. 同层跳过相同元素
3. 判断条件：i > start && nums[i] == nums[i-1]（组合/子集）
            i > 0 && nums[i] == nums[i-1] && !used[i-1]（排列）
```

### 树层去重 vs 树枝去重

```
输入 [1,1,2]，求子集：

树层去重（横向）✓
        []
      / | \
    [1] [1]✗ [2]     ← 同层第二个 1 跳过
    /\    |
 [1,1][1,2] [2]
   |
[1,1,2]

树枝去重（纵向）✗
  [1] → [1,1] 是允许的（纵向选两个不同的1）
```

**核心区分：**
- **树层去重**（组合/子集）：`i > start && nums[i] == nums[i-1]`  
  — 同一层（for 循环内）不选重复值
- **树枝去重**（排列）：`nums[i] == nums[i-1] && !used[i-1]`  
  — `used[i-1]=false` 说明同层刚撤销，跳过

### 去重条件对照

| 题型 | 去重代码 | 含义 |
|------|---------|------|
| 组合/子集 | `i > start && nums[i] == nums[i-1]` | 同层前面有相同的，跳过 |
| 排列 | `nums[i] == nums[i-1] && !used[i-1]` | 同层前一个相同的已撤销，跳过 |

---

## 7. 组合总和系列

### LC 39: 组合总和（元素可重复使用）

```cpp
vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int, int)> backtrack = [&](int start, int remain) {
        if (remain == 0) { result.push_back(path); return; }
        for (int i = start; i < (int)candidates.size(); i++) {
            if (candidates[i] > remain) break;  // 剪枝
            path.push_back(candidates[i]);
            backtrack(i, remain - candidates[i]);  // ← 注意是 i，不是 i+1！
            path.pop_back();
        }
    };
    backtrack(0, target);
    return result;
}
```

> 关键：`backtrack(i, ...)` 而不是 `backtrack(i+1, ...)`，允许重复选同一个元素

### LC 40: 组合总和 II（每个元素只能用一次，有重复元素）

```cpp
vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int, int)> backtrack = [&](int start, int remain) {
        if (remain == 0) { result.push_back(path); return; }
        for (int i = start; i < (int)candidates.size(); i++) {
            if (candidates[i] > remain) break;
            if (i > start && candidates[i] == candidates[i-1]) continue;  // 去重
            path.push_back(candidates[i]);
            backtrack(i + 1, remain - candidates[i]);
            path.pop_back();
        }
    };
    backtrack(0, target);
    return result;
}
```

### LC 216: 组合总和 III（1-9 中选 k 个数和为 n）

```cpp
vector<vector<int>> combinationSum3(int k, int n) {
    vector<vector<int>> result;
    vector<int> path;
    
    function<void(int, int)> backtrack = [&](int start, int remain) {
        if ((int)path.size() == k) {
            if (remain == 0) result.push_back(path);
            return;
        }
        for (int i = start; i <= 9; i++) {
            if (i > remain) break;
            path.push_back(i);
            backtrack(i + 1, remain - i);
            path.pop_back();
        }
    };
    backtrack(1, n);
    return result;
}
```

### 组合总和系列对照

| 题目 | 元素范围 | 可重复 | 去重 | 递归参数 |
|------|---------|--------|------|---------|
| LC 39 | candidates | 是 | 无需 | `backtrack(i, ...)` |
| LC 40 | candidates（有重复） | 否 | 排序+同层跳过 | `backtrack(i+1, ...)` |
| LC 216 | 1~9 | 否 | 天然无重复 | `backtrack(i+1, ...)` |

---

## 8. 分割问题

### LC 131: 分割回文串

**思路：** 把分割看成在每个位置做"切割"选择

```
字符串 "aab" 的切割位置：
  a | a | b  → ["a","a","b"] ✓
  a | ab     → ["a","ab"]   ✗ (ab不是回文)
  aa | b     → ["aa","b"]   ✓
  aab        → ["aab"]      ✗ (aab不是回文)
```

```cpp
vector<vector<string>> partition(string s) {
    vector<vector<string>> result;
    vector<string> path;
    int n = s.size();
    
    // 预处理回文判断 dp[i][j] = s[i..j] 是否回文
    vector<vector<bool>> dp(n, vector<bool>(n, false));
    for (int i = n - 1; i >= 0; i--)
        for (int j = i; j < n; j++)
            dp[i][j] = (s[i] == s[j]) && (j - i <= 2 || dp[i+1][j-1]);
    
    function<void(int)> backtrack = [&](int start) {
        if (start == n) {
            result.push_back(path);
            return;
        }
        for (int end = start; end < n; end++) {
            if (!dp[start][end]) continue;  // 不是回文就跳过
            path.push_back(s.substr(start, end - start + 1));
            backtrack(end + 1);
            path.pop_back();
        }
    };
    backtrack(0);
    return result;
}
```

### LC 93: 复原 IP 地址

**约束：** 4 段，每段 0-255，不能有前导零

```cpp
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
```

### 分割问题的本质

> **分割 = 在字符串的各个位置做"切或不切"的选择**

与组合类似，`start` 表示当前切割起点，`end` 表示切到哪里。

```
字符串切割的决策树（"abc"）：
         start=0
        /   |   \
     "a"  "ab" "abc"
      |     |
  start=1 start=2
   / \      |
 "b" "bc"  "c"
  |
start=2
  |
 "c"
```

---

## 9. 棋盘问题

### LC 51: N 皇后 ⛳

**规则：** 任意两个皇后不能同行、同列、同对角线

```cpp
vector<vector<string>> solveNQueens(int n) {
    vector<vector<string>> result;
    vector<string> board(n, string(n, '.'));
    
    // 记录哪些列和对角线已被占用
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
```

**对角线编码技巧：**

```
主对角线（↘）：同一条主对角线上 row - col 相同
副对角线（↙）：同一条副对角线上 row + col 相同

  col: 0  1  2  3
row 0: 0  1  2  3    row-col: 0,-1,-2,-3  → 加 n 偏移
row 1: 1  2  3  4    row+col: 1, 2, 3, 4
row 2: 2  3  4  5
row 3: 3  4  5  6
```

### LC 37: 解数独

```cpp
void solveSudoku(vector<vector<char>>& board) {
    function<bool(int, int)> backtrack = [&](int row, int col) -> bool {
        if (row == 9) return true;
        if (col == 9) return backtrack(row + 1, 0);
        if (board[row][col] != '.') return backtrack(row, col + 1);
        
        for (char c = '1'; c <= '9'; c++) {
            if (!isValid(board, row, col, c)) continue;
            board[row][col] = c;
            if (backtrack(row, col + 1)) return true;
            board[row][col] = '.';
        }
        return false;  // 所有数字都不行 → 回溯
    };
    backtrack(0, 0);
}

bool isValid(vector<vector<char>>& board, int row, int col, char c) {
    for (int i = 0; i < 9; i++) {
        if (board[row][i] == c) return false;  // 行
        if (board[i][col] == c) return false;  // 列
        // 3x3 宫格
        int r = 3 * (row / 3) + i / 3;
        int cl = 3 * (col / 3) + i % 3;
        if (board[r][cl] == c) return false;
    }
    return true;
}
```

### LC 79: 单词搜索

```cpp
bool exist(vector<vector<char>>& board, string word) {
    int m = board.size(), n = board[0].size();
    
    function<bool(int, int, int)> dfs = [&](int i, int j, int k) -> bool {
        if (k == (int)word.size()) return true;
        if (i < 0 || i >= m || j < 0 || j >= n) return false;
        if (board[i][j] != word[k]) return false;
        
        char tmp = board[i][j];
        board[i][j] = '#';  // 标记已访问
        
        bool found = dfs(i+1,j,k+1) || dfs(i-1,j,k+1)
                  || dfs(i,j+1,k+1) || dfs(i,j-1,k+1);
        
        board[i][j] = tmp;  // 回溯：恢复
        return found;
    };
    
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (dfs(i, j, 0)) return true;
    return false;
}
```

### 其他经典回溯题

#### LC 17: 电话号码的字母组合

```cpp
vector<string> letterCombinations(string digits) {
    if (digits.empty()) return {};
    vector<string> mapping = {"","","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
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
```

#### LC 22: 括号生成

```cpp
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
```

---

## 10. 排列/组合/子集速查对照表

### 完整对比

| 类型 | start | used[] | 去重 | 收集时机 | 可重复 |
|------|-------|--------|------|---------|--------|
| **排列** | 无 | ✅ | `!used[i-1]` | 叶子 | 否 |
| **排列（可重复）** | 无 | ✅ | 排序+`!used[i-1]` | 叶子 | 否 |
| **组合** | ✅ | 否 | 无需 | 叶子 | 否 |
| **组合（有重复元素）** | ✅ | 否 | 排序+`i>start` | 叶子 | 否 |
| **子集** | ✅ | 否 | 无需 | 所有节点 | 否 |
| **子集（有重复元素）** | ✅ | 否 | 排序+`i>start` | 所有节点 | 否 |
| **可重复组合** | ✅ | 否 | 无需 | 叶子 | 是（传 i） |

### 一图速记

```
                    排列/组合/子集
                   /      |      \
              排列(46)  组合(77)  子集(78)
              不用start  用start  用start
              用used[]   不用used  不用used
              叶子收集   叶子收集  全部收集
                |          |        |
            含重复(47)  含重复(40) 含重复(90)
            +排序去重   +排序去重  +排序去重
```

---

## 11. 剪枝优化策略

### 常见剪枝手段

| 剪枝类型 | 代码 | 适用场景 |
|---------|------|---------|
| **值剪枝** | `if (candidates[i] > remain) break;` | 组合总和 |
| **长度剪枝** | `i <= n-(k-path.size())+1` | 组合 |
| **重复剪枝** | `if (i > start && nums[i] == nums[i-1]) continue;` | 含重复元素 |
| **合法性剪枝** | `if (!isValid(...)) continue;` | N皇后、数独 |
| **提前终止** | `if (sum > target) return;` | 和相关问题 |

### 剪枝效果对比

以 LC 77: `combine(20, 5)` 为例：

```
不剪枝：遍历节点数 ≈ 无用分支很多
剪枝后：i ≤ 20-(5-path.size())+1 = 16
  第一层最多到 16，第二层最多到 17...
  减少约 40% 的节点访问
```

### 排序的作用

排序不仅是去重的前提，还能增强剪枝效果：

```
candidates = [7, 3, 2, 5], target = 7

未排序：必须遍历完才知道某分支无解
排序后   [2, 3, 5, 7]：
  当 candidates[i] > remain 时直接 break
  后面的更大，一定不行
```

---

## 12. 综合刷题清单

### 基础必做

| 序号 | 题目 | 难度 | 核心考点 |
|------|------|------|----------|
| 1 | LC 78: 子集 | Medium | 子集模板 |
| 2 | LC 46: 全排列 | Medium | 排列模板 |
| 3 | LC 77: 组合 | Medium | 组合模板 |
| 4 | LC 39: 组合总和 | Medium | 可重复选择 |
| 5 | LC 17: 电话号码字母组合 | Medium | 多选择列表 |

### 去重进阶

| 序号 | 题目 | 难度 | 核心考点 |
|------|------|------|----------|
| 6 | LC 90: 子集 II | Medium | 子集去重 |
| 7 | LC 47: 全排列 II | Medium | 排列去重 |
| 8 | LC 40: 组合总和 II | Medium | 组合去重 |
| 9 | LC 216: 组合总和 III | Medium | 固定范围组合 |

### 分割与棋盘

| 序号 | 题目 | 难度 | 核心考点 |
|------|------|------|----------|
| 10 | LC 131: 分割回文串 | Medium | 分割+回文 DP |
| 11 | LC 93: 复原 IP 地址 | Medium | 分割+约束 |
| 12 | LC 22: 括号生成 | Medium | 限制条件回溯 |
| 13 | LC 79: 单词搜索 | Medium | 网格回溯 |
| 14 | LC 51: N 皇后 | Hard | 棋盘+多维约束 |
| 15 | LC 37: 解数独 | Hard | 最复杂的回溯 |

### 推荐刷题顺序

```
子集(78) → 子集II(90) → 组合(77) → 组合总和(39) →
组合总和II(40) → 组合总和III(216) → 全排列(46) →
全排列II(47) → 电话号码(17) → 括号生成(22) →
分割回文串(131) → 复原IP(93) → 单词搜索(79) →
N皇后(51) → 解数独(37)
```

---

## 13. 易错点与调试技巧

### 🚫 常见错误 Top 5

| 排名 | 错误 | 正确做法 |
|------|------|---------|
| 1 | 忘记撤销选择（pop_back / used=false） | 每次做选择后**配对**撤销 |
| 2 | 组合/子集用了 used[] 而不用 start | 组合/子集用 `start` 控制范围 |
| 3 | 排列用了 start 而不用 used[] | 排列每次从 0 开始 + used[] |
| 4 | 去重时忘记排序 | `sort()` 必须在回溯前 |
| 5 | 可重复选择传了 i+1 | LC 39 传 `i`，LC 40 传 `i+1` |

### 调试三板斧

```
1. 打印决策树
   在 backtrack 开头加：
   cout << string(depth*2, ' ') << "path: " << path << endl;

2. 用小规模测试
   n=3 或 n=4，手动画出决策树对照

3. 检查"做选择-撤销"配对
   每一个 push_back 必须有 pop_back
   每一个 used[i]=true 必须有 used[i]=false
```

### 回溯 vs 动态规划的选择

| 特征 | 回溯 | DP |
|------|------|-----|
| 目标 | 求**所有**具体方案 | 求**最优值/方案数** |
| 方法 | 枚举+剪枝 | 状态转移 |
| 时间 | 指数级 | 多项式级 |
| 记忆化 | 可加（变成记忆化搜索） | 天然表格 |

> **经验法则：** 如果题目要求"列出所有方案"→ 回溯；要求"最优/计数"→ 先想 DP
