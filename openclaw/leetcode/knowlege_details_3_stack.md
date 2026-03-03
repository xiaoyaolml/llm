# 📖 专题三（上）：栈（Stack）— 从零基础到精通

*作者：大胖超 😜 | 配合 leetcode_knowledge_base.md 食用更佳*

---

## 目录

1. [栈的基本概念](#1-栈的基本概念)
2. [C++ 中的栈实现](#2-c-中的栈实现)
3. [手写栈（加深理解）](#3-手写栈加深理解)
4. [括号匹配问题](#4-括号匹配问题)
5. [最小栈](#5-最小栈)
6. [单调栈（Monotonic Stack）](#6-单调栈monotonic-stack)
7. [栈在表达式求值中的应用](#7-栈在表达式求值中的应用)
8. [栈与递归的关系](#8-栈与递归的关系)
9. [栈模拟题](#9-栈模拟题)
10. [栈在 DFS 中的应用](#10-栈在-dfs-中的应用)
11. [经典 LeetCode 栈题精讲](#11-经典-leetcode-栈题精讲)
12. [常见陷阱与最佳实践](#12-常见陷阱与最佳实践)

---

# 1. 栈的基本概念

## 1.1 什么是栈？

**栈 (Stack)** = **后进先出 (LIFO: Last In, First Out)**

生活类比：
- 🥞 叠煎饼：最后放上去的，最先被吃
- 📦 箱子里叠书：先拿出最上面的
- ↩️ 浏览器回退：最近访问的页面先返回
- 🔄 撤销操作 (Ctrl+Z)：最近的操作先撤销

```
           ┌─────┐
  push →   │  3  │ ← top（栈顶）    pop → 取出 3
           ├─────┤
           │  2  │
           ├─────┤
           │  1  │ ← bottom（栈底）
           └─────┘

  操作：push(入栈)  pop(出栈)  top(查看栈顶)  empty(是否为空)
```

## 1.2 核心操作与复杂度

| 操作 | 说明 | 时间复杂度 |
|------|------|-----------|
| `push(x)` | 元素入栈 | O(1) |
| `pop()` | 栈顶元素出栈 | O(1) |
| `top()` | 查看栈顶（不弹出） | O(1) |
| `empty()` | 判断是否为空 | O(1) |
| `size()` | 获取元素个数 | O(1) |
| 随机访问 | ❌ 不支持 | — |

## 1.3 栈的应用场景

```
栈的使用场景：
├─ 括号匹配 → LC 20, 32
├─ 表达式求值 → LC 224, 227, 772
├─ 单调栈 → LC 42, 84, 496, 503, 739
├─ DFS 的迭代实现 → 树的前/中/后序遍历
├─ 撤销/回退 → 浏览器历史、编辑器undo
├─ 函数调用栈 → 递归底层机制
└─ 解码/嵌套结构 → LC 394, 341, 385
```

---

# 2. C++ 中的栈实现

## 2.1 `std::stack`

```cpp
#include <stack>

stack<int> stk;                // 默认底层是 deque
stack<int, vector<int>> stk2;  // 底层用 vector

stk.push(10);     // 入栈
stk.push(20);
stk.push(30);

cout << stk.top();  // 30（查看栈顶，不弹出）
stk.pop();          // 弹出 30（⚠️ 返回值是 void！）
cout << stk.top();  // 20

cout << stk.size();  // 2
cout << stk.empty(); // false
```

> ⚠️ **关键注意**：`pop()` 不返回值！要先 `top()` 再 `pop()`。

## 2.2 底层容器选择

```cpp
// stack 是适配器，可以选择底层容器：
stack<int>                 stk1;  // 默认 deque（推荐）
stack<int, vector<int>>    stk2;  // vector（保证连续内存）
stack<int, list<int>>      stk3;  // list（双向链表）

// deque vs vector:
// deque: 两端操作都是 O(1)，不会整体重分配
// vector: 连续内存，缓存友好，但 push_back 偶尔重分配
```

## 2.3 用 `vector` 模拟栈

LeetCode 中经常直接用 `vector` 当栈（更灵活）：

```cpp
vector<int> stk;

stk.push_back(10);   // push
stk.push_back(20);   // push
stk.back();           // top → 20
stk.pop_back();       // pop
stk.size();           // size
stk.empty();          // empty

// 优点：可以遍历、随机访问、转为其他结构
```

---

# 3. 手写栈（加深理解）

## 3.1 数组实现

```cpp
class ArrayStack {
    vector<int> data;

public:
    void push(int val) {
        data.push_back(val);
    }

    void pop() {
        if (data.empty()) throw runtime_error("Stack underflow");
        data.pop_back();
    }

    int top() {
        if (data.empty()) throw runtime_error("Stack is empty");
        return data.back();
    }

    bool empty() { return data.empty(); }
    int size() { return data.size(); }
};
```

## 3.2 链表实现

```cpp
class LinkedStack {
    struct Node {
        int val;
        Node* next;
        Node(int v, Node* n = nullptr) : val(v), next(n) {}
    };
    Node* head = nullptr;
    int count = 0;

public:
    void push(int val) {
        head = new Node(val, head);  // 头插
        count++;
    }

    void pop() {
        if (!head) throw runtime_error("Stack underflow");
        Node* old = head;
        head = head->next;
        delete old;
        count--;
    }

    int top() {
        if (!head) throw runtime_error("Stack is empty");
        return head->val;
    }

    bool empty() { return head == nullptr; }
    int size() { return count; }

    ~LinkedStack() {
        while (head) { Node* t = head; head = head->next; delete t; }
    }
};
```

---

# 4. 括号匹配问题

> 🎯 栈最经典的应用，面试必考。

## 4.1 有效的括号

```cpp
// LC 20: 有效的括号
// 思路：左括号入栈，右括号检查栈顶是否配对
bool isValid(string s) {
    stack<char> stk;
    unordered_map<char, char> pairs = {
        {')', '('}, {']', '['}, {'}', '{'}
    };

    for (char c : s) {
        if (pairs.count(c)) {
            // 右括号：检查栈顶
            if (stk.empty() || stk.top() != pairs[c])
                return false;
            stk.pop();
        } else {
            // 左括号：入栈
            stk.push(c);
        }
    }
    return stk.empty();  // 栈必须为空才合法
}

// 输入: "([]{})" → true
// 输入: "([)]"   → false
// 输入: "((("    → false（栈不为空）
```

## 4.2 最长有效括号

```cpp
// LC 32: 最长有效括号
// 思路：栈中存下标，用下标计算长度
int longestValidParentheses(string s) {
    stack<int> stk;
    stk.push(-1);     // 哨兵：作为计算长度的基准
    int maxLen = 0;

    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '(') {
            stk.push(i);    // 左括号的下标入栈
        } else {
            stk.pop();       // 尝试匹配
            if (stk.empty()) {
                stk.push(i); // 当前右括号无法匹配，作为新基准
            } else {
                maxLen = max(maxLen, i - stk.top());
            }
        }
    }
    return maxLen;
}

// ")()())" → 4
// "(()"    → 2
```

## 4.3 括号生成

```cpp
// LC 22: 括号生成（回溯）
// 生成所有 n 对合法括号
void generateHelper(int n, int open, int close, string& curr, vector<string>& result) {
    if ((int)curr.size() == 2 * n) {
        result.push_back(curr);
        return;
    }
    if (open < n) {                    // 还能放左括号
        curr.push_back('(');
        generateHelper(n, open + 1, close, curr, result);
        curr.pop_back();               // 回溯
    }
    if (close < open) {                // 右括号不能多于左括号
        curr.push_back(')');
        generateHelper(n, open, close + 1, curr, result);
        curr.pop_back();               // 回溯
    }
}

vector<string> generateParenthesis(int n) {
    vector<string> result;
    string curr;
    generateHelper(n, 0, 0, curr, result);
    return result;
}
```

---

# 5. 最小栈

```cpp
// LC 155: 最小栈
// 在 O(1) 时间内获取栈中最小值
class MinStack {
    stack<int> dataStack;
    stack<int> minStack;   // 辅助栈，同步维护最小值

public:
    void push(int val) {
        dataStack.push(val);
        // 当前值 <= 最小值时，也压入 minStack
        if (minStack.empty() || val <= minStack.top())
            minStack.push(val);
    }

    void pop() {
        if (dataStack.top() == minStack.top())
            minStack.pop();
        dataStack.pop();
    }

    int top() {
        return dataStack.top();
    }

    int getMin() {
        return minStack.top();
    }
};

// 另一种方法：用一个栈，存 pair<val, currentMin>
class MinStack2 {
    stack<pair<int,int>> stk;   // {值, 当前最小值}

public:
    void push(int val) {
        int curMin = stk.empty() ? val : min(val, stk.top().second);
        stk.push({val, curMin});
    }

    void pop() { stk.pop(); }
    int top() { return stk.top().first; }
    int getMin() { return stk.top().second; }
};
```

---

# 6. 单调栈（Monotonic Stack）

> 🔥 解决「下一个更大/更小元素」类问题的利器。

## 6.1 核心思想

**单调栈** = 栈内元素始终保持单调递增或单调递减的栈。

```
单调递减栈（栈底到栈顶递减）：
  push(3): [3]
  push(2): [3, 2]
  push(1): [3, 2, 1]
  push(5): 弹出 1,2,3（因为 5 比它们都大），[5]

每个被弹出的元素，其「下一个更大元素」就是当前要入栈的元素。
```

## 6.2 下一个更大元素

```cpp
// LC 496: 下一个更大元素 I
// 对于数组中每个元素，找到右边第一个比它大的元素
vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    unordered_map<int, int> nextGreater;
    stack<int> stk;  // 单调递减栈

    for (int num : nums2) {
        // 当前元素比栈顶大 → 栈顶元素的「下一个更大」就是当前元素
        while (!stk.empty() && stk.top() < num) {
            nextGreater[stk.top()] = num;
            stk.pop();
        }
        stk.push(num);
    }

    // 栈中剩余元素没有更大的
    while (!stk.empty()) {
        nextGreater[stk.top()] = -1;
        stk.pop();
    }

    vector<int> result;
    for (int num : nums1)
        result.push_back(nextGreater[num]);
    return result;
}
```

## 6.3 每日温度

```cpp
// LC 739: 每日温度
// 等几天才能等到更暖和的温度
vector<int> dailyTemperatures(vector<int>& temperatures) {
    int n = temperatures.size();
    vector<int> result(n, 0);
    stack<int> stk;   // 存下标，单调递减栈

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && temperatures[stk.top()] < temperatures[i]) {
            int prevIndex = stk.top();
            stk.pop();
            result[prevIndex] = i - prevIndex;  // 等了多少天
        }
        stk.push(i);
    }
    return result;
}

// temperatures = [73, 74, 75, 71, 69, 72, 76, 73]
// result       = [ 1,  1,  4,  2,  1,  1,  0,  0]
```

## 6.4 下一个更大元素 II（循环数组）

```cpp
// LC 503: 下一个更大元素 II
// 循环数组：最后一个元素的下一个是第一个
vector<int> nextGreaterElements(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;

    // 遍历两遍（模拟循环）
    for (int i = 0; i < 2 * n; i++) {
        int idx = i % n;
        while (!stk.empty() && nums[stk.top()] < nums[idx]) {
            result[stk.top()] = nums[idx];
            stk.pop();
        }
        if (i < n) stk.push(i);  // 只有第一遍才入栈
    }
    return result;
}
```

## 6.5 柱状图中最大的矩形

```cpp
// LC 84: 柱状图中最大的矩形（Hard 经典！）
// 对每根柱子，找它向左向右能延伸的最远距离
int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    stack<int> stk;
    int maxArea = 0;

    for (int i = 0; i <= n; i++) {
        int h = (i == n) ? 0 : heights[i];  // 虚拟哨兵

        while (!stk.empty() && heights[stk.top()] > h) {
            int height = heights[stk.top()];
            stk.pop();
            int width = stk.empty() ? i : (i - stk.top() - 1);
            maxArea = max(maxArea, height * width);
        }
        stk.push(i);
    }
    return maxArea;
}

// heights = [2, 1, 5, 6, 2, 3]
// maxArea = 10（高 5，宽 2：柱子 5 和 6）
```

## 6.6 接雨水

```cpp
// LC 42: 接雨水（用单调栈解法）
int trap(vector<int>& height) {
    stack<int> stk;
    int water = 0;

    for (int i = 0; i < (int)height.size(); i++) {
        while (!stk.empty() && height[stk.top()] < height[i]) {
            int bottom = stk.top();
            stk.pop();
            if (stk.empty()) break;

            int width = i - stk.top() - 1;
            int h = min(height[i], height[stk.top()]) - height[bottom];
            water += width * h;
        }
        stk.push(i);
    }
    return water;
}

// height = [0,1,0,2,1,0,1,3,2,1,2,1]
// water  = 6
```

## 6.7 单调栈模板总结

```
问题类型               栈的单调性          何时弹出
─────────────────────────────────────────────────
下一个更大元素          递减栈             当前 > 栈顶
下一个更小元素          递增栈             当前 < 栈顶
前一个更大元素          递减栈             入栈时栈顶就是答案
前一个更小元素          递增栈             入栈时栈顶就是答案

口诀：求更大 → 递减栈，求更小 → 递增栈
```

---

# 7. 栈在表达式求值中的应用

## 7.1 逆波兰表达式

```cpp
// LC 150: 逆波兰表达式求值
// 后缀表达式：遇到数字入栈，遇到运算符弹出两个数计算后入栈
int evalRPN(vector<string>& tokens) {
    stack<int> stk;

    for (const string& token : tokens) {
        if (token == "+" || token == "-" || token == "*" || token == "/") {
            int b = stk.top(); stk.pop();  // 注意顺序！b 先出
            int a = stk.top(); stk.pop();
            if (token == "+") stk.push(a + b);
            else if (token == "-") stk.push(a - b);
            else if (token == "*") stk.push(a * b);
            else stk.push(a / b);          // 整数除法
        } else {
            stk.push(stoi(token));
        }
    }
    return stk.top();
}

// ["2","1","+","3","*"] → (2+1)*3 = 9
```

## 7.2 基本计算器

```cpp
// LC 224: 基本计算器（含括号、加减、空格）
int calculate(string s) {
    stack<int> signs;
    signs.push(1);
    int result = 0, num = 0, sign = 1;

    for (char c : s) {
        if (isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c == '+' || c == '-') {
            result += sign * num;
            num = 0;
            sign = signs.top() * (c == '+' ? 1 : -1);
        } else if (c == '(') {
            signs.push(sign);
        } else if (c == ')') {
            signs.pop();
        }
    }
    result += sign * num;
    return result;
}
```

## 7.3 基本计算器 II（含乘除）

```cpp
// LC 227: 基本计算器 II
// 含 +, -, *, / 无括号
int calculate2(string s) {
    stack<int> stk;
    int num = 0;
    char prevOp = '+';

    for (int i = 0; i < (int)s.size(); i++) {
        char c = s[i];
        if (isdigit(c)) {
            num = num * 10 + (c - '0');
        }
        // 遇到运算符或末尾时处理
        if ((!isdigit(c) && c != ' ') || i == (int)s.size() - 1) {
            if (prevOp == '+') stk.push(num);
            else if (prevOp == '-') stk.push(-num);
            else if (prevOp == '*') {
                int top = stk.top(); stk.pop();
                stk.push(top * num);
            }
            else if (prevOp == '/') {
                int top = stk.top(); stk.pop();
                stk.push(top / num);
            }
            prevOp = c;
            num = 0;
        }
    }

    int result = 0;
    while (!stk.empty()) {
        result += stk.top();
        stk.pop();
    }
    return result;
}
```

---

# 8. 栈与递归的关系

## 8.1 递归的本质就是栈

```
每次函数调用 → 压入系统调用栈
每次函数返回 → 从调用栈弹出

factorial(4) 的调用栈：
┌─────────────────┐
│ factorial(1) = 1 │ ← 栈顶（最后调用，最先返回）
├─────────────────┤
│ factorial(2)     │  → 等 factorial(1) 返回后算 2*1
├─────────────────┤
│ factorial(3)     │  → 等 factorial(2) 返回后算 3*2
├─────────────────┤
│ factorial(4)     │  → 等 factorial(3) 返回后算 4*6
└─────────────────┘    栈底（最先调用，最后返回）
```

## 8.2 用栈消除递归

任何递归都可以用显式栈改写为迭代：

```cpp
// 递归版 DFS
void dfs_recursive(TreeNode* root) {
    if (!root) return;
    visit(root);
    dfs_recursive(root->left);
    dfs_recursive(root->right);
}

// 栈版 DFS（等价）
void dfs_iterative(TreeNode* root) {
    stack<TreeNode*> stk;
    if (root) stk.push(root);

    while (!stk.empty()) {
        TreeNode* node = stk.top(); stk.pop();
        visit(node);
        if (node->right) stk.push(node->right);  // 先右后左
        if (node->left) stk.push(node->left);     // 左先出栈
    }
}
```

---

# 9. 栈模拟题

## 9.1 字符串解码

```cpp
// LC 394: 字符串解码
// "3[a2[c]]" → "accaccacc"
string decodeString(string s) {
    stack<string> strStack;
    stack<int> numStack;
    string current;
    int num = 0;

    for (char c : s) {
        if (isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c == '[') {
            strStack.push(current);
            numStack.push(num);
            current = "";
            num = 0;
        } else if (c == ']') {
            string prev = strStack.top(); strStack.pop();
            int repeat = numStack.top(); numStack.pop();
            string temp;
            for (int i = 0; i < repeat; i++) temp += current;
            current = prev + temp;
        } else {
            current += c;
        }
    }
    return current;
}
```

## 9.2 简化路径

```cpp
// LC 71: 简化路径
string simplifyPath(string path) {
    stack<string> stk;
    istringstream ss(path);
    string token;

    while (getline(ss, token, '/')) {
        if (token == "" || token == ".") continue;   // 空或当前目录
        if (token == "..") {
            if (!stk.empty()) stk.pop();             // 返回上一级
        } else {
            stk.push(token);
        }
    }

    string result;
    while (!stk.empty()) {
        result = "/" + stk.top() + result;
        stk.pop();
    }
    return result.empty() ? "/" : result;
}

// "/a/./b/../../c/" → "/c"
```

## 9.3 去除重复字母

```cpp
// LC 316: 去除重复字母（最小字典序）
string removeDuplicateLetters(string s) {
    int lastOccurrence[26] = {};
    bool inStack[26] = {};

    // 记录每个字符最后出现的位置
    for (int i = 0; i < (int)s.size(); i++)
        lastOccurrence[s[i] - 'a'] = i;

    string stk;  // 用 string 模拟栈
    for (int i = 0; i < (int)s.size(); i++) {
        char c = s[i];
        if (inStack[c - 'a']) continue;  // 已在栈中

        // 栈顶字符比当前大，且后面还会出现 → 弹出
        while (!stk.empty() && stk.back() > c &&
               lastOccurrence[stk.back() - 'a'] > i) {
            inStack[stk.back() - 'a'] = false;
            stk.pop_back();
        }

        stk.push_back(c);
        inStack[c - 'a'] = true;
    }
    return stk;
}

// "bcabc" → "abc"
// "cbacdcbc" → "acdb"
```

---

# 10. 栈在 DFS 中的应用

## 10.1 岛屿数量（DFS 栈版）

```cpp
// LC 200: 岛屿数量
int numIslands(vector<vector<char>>& grid) {
    int m = grid.size(), n = grid[0].size();
    int count = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') {
                count++;
                // DFS 用栈
                stack<pair<int,int>> stk;
                stk.push({i, j});
                grid[i][j] = '0';

                while (!stk.empty()) {
                    auto [r, c] = stk.top(); stk.pop();
                    int dirs[] = {-1,0,1,0,-1};
                    for (int d = 0; d < 4; d++) {
                        int nr = r + dirs[d], nc = c + dirs[d+1];
                        if (nr >= 0 && nr < m && nc >= 0 && nc < n
                            && grid[nr][nc] == '1') {
                            grid[nr][nc] = '0';
                            stk.push({nr, nc});
                        }
                    }
                }
            }
        }
    }
    return count;
}
```

---

# 11. 经典 LeetCode 栈题精讲

## 必做题清单

| 题号 | 题目 | 难度 | 核心技巧 | 频率 |
|------|------|------|----------|------|
| 20 | 有效的括号 | Easy | 括号匹配 | ⭐⭐⭐⭐⭐ |
| 155 | 最小栈 | Medium | 辅助栈 | ⭐⭐⭐⭐⭐ |
| 150 | 逆波兰表达式 | Medium | 后缀求值 | ⭐⭐⭐⭐ |
| 232 | 用栈实现队列 | Easy | 两栈倒腾 | ⭐⭐⭐⭐⭐ |
| 394 | 字符串解码 | Medium | 双栈模拟 | ⭐⭐⭐⭐⭐ |
| 71 | 简化路径 | Medium | 栈模拟 | ⭐⭐⭐⭐ |
| 739 | 每日温度 | Medium | 单调栈 | ⭐⭐⭐⭐⭐ |
| 496 | 下一个更大元素 I | Easy | 单调栈+哈希 | ⭐⭐⭐⭐ |
| 503 | 下一个更大元素 II | Medium | 单调栈+循环 | ⭐⭐⭐⭐ |
| 84 | 柱状图最大矩形 | Hard | 单调栈 | ⭐⭐⭐⭐⭐ |
| 42 | 接雨水 | Hard | 单调栈/双指针 | ⭐⭐⭐⭐⭐ |
| 224 | 基本计算器 | Hard | 栈+递归 | ⭐⭐⭐⭐ |
| 227 | 基本计算器 II | Medium | 栈+优先级 | ⭐⭐⭐⭐ |
| 32 | 最长有效括号 | Hard | 栈/DP | ⭐⭐⭐⭐ |
| 316 | 去除重复字母 | Medium | 单调栈+贪心 | ⭐⭐⭐⭐ |
| 22 | 括号生成 | Medium | 回溯 | ⭐⭐⭐⭐⭐ |

## 解题思路速查

```
看到栈相关问题 → 先分类：
├─ 括号/嵌套结构？ → 直接匹配入栈出栈
├─ 求下一个更大/更小？ → 单调栈
├─ 表达式求值？ → 数字栈 + 运算符栈
├─ 解码/嵌套？ → 双栈（数据栈 + 辅助栈）
├─ 路径/历史？ → 栈模拟
└─ DFS 迭代版？ → 显式栈替代递归
```

---

# 12. 常见陷阱与最佳实践

## 12.1 经典陷阱

### 陷阱 1：空栈操作

```cpp
// ❌ 不检查就操作
stk.top();   // 空栈 → 未定义行为！
stk.pop();   // 空栈 → 未定义行为！

// ✅ 先检查
if (!stk.empty()) {
    int val = stk.top();
    stk.pop();
}
```

### 陷阱 2：pop() 不返回值

```cpp
// ❌ 期望 pop 返回值
int val = stk.pop();  // 编译错误！pop 返回 void

// ✅ 先 top 再 pop
int val = stk.top();
stk.pop();
```

### 陷阱 3：单调栈的单调方向搞反

```cpp
// 求「下一个更大」→ 栈内递减（从底到顶递减）
// 求「下一个更小」→ 栈内递增（从底到顶递增）

// ⚠️ 别搞反了！
```

### 陷阱 4：逆波兰表达式操作数顺序

```cpp
// ❌ a 和 b 顺序反了
int a = stk.top(); stk.pop();
int b = stk.top(); stk.pop();
result = a - b;  // 应该是 b - a（b 先入栈）

// ✅ 正确顺序
int b = stk.top(); stk.pop();  // 后操作数先出
int a = stk.top(); stk.pop();  // 先操作数后出
result = a - b;                 // a op b
```

## 12.2 最佳实践

```
✅ 操作前检查 empty()
✅ 理解 pop() 返回 void
✅ 单调栈先画图确定单调方向
✅ 用 vector 模拟栈（可遍历、更灵活）
✅ 考虑哨兵值简化边界处理
✅ 栈中存下标（比存值更灵活）
```

## 12.3 栈技巧速查卡

```
┌──────────────────────────────────────────────────┐
│               栈技巧速查                          │
├──────────────────────────────────────────────────┤
│  括号匹配     → 左入栈，右检查顶                   │
│  最小栈       → 辅助栈同步维护 min                 │
│  单调栈       → 更大用递减栈，更小用递增栈          │
│  表达式求值   → 数字栈 + 运算符栈                  │
│  字符串解码   → str栈 + num栈                     │
│  DFS 迭代版   → 显式栈替代递归                     │
│  栈存下标     → 比存值更灵活（可算距离）            │
│  哨兵技巧     → 末尾/初始加哨兵简化边界              │
└──────────────────────────────────────────────────┘
```

---

*📝 下一步：完成队列详解 `knowlege_details_3_queue.md`*

*💪 单调栈是栈中最难也最常考的，务必画图理解每道题！*
