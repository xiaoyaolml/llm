/**
 * ============================================================================
 *  专题三（上）：栈（Stack）— 完整示例代码
 *  配合 knowlege_details_3_stack.md 食用
 *  编译: g++ -std=c++17 -o knowlege_details_3_stack knowlege_details_3_stack.cpp
 * ============================================================================
 */

#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <climits>
#include <cassert>
using namespace std;

// ============================================================================
//  第 1 章：std::stack 基础与 vector 模拟栈
// ============================================================================

void demo_stack_basics() {
    cout << "\n========== 1. 栈基础 ==========\n";

    // --- std::stack ---
    stack<int> stk;
    stk.push(10);
    stk.push(20);
    stk.push(30);

    cout << "栈顶: " << stk.top() << endl;    // 30
    cout << "大小: " << stk.size() << endl;    // 3

    stk.pop();   // 弹出 30  ⚠️ pop() 返回 void！
    cout << "pop 后栈顶: " << stk.top() << endl;  // 20

    // 遍历并清空
    cout << "依次弹出: ";
    while (!stk.empty()) {
        cout << stk.top() << " ";
        stk.pop();
    }
    cout << endl;  // 20 10

    // --- 用 vector 模拟栈（更灵活）---
    cout << "\nvector 模拟栈:" << endl;
    vector<int> vecStk;
    vecStk.push_back(1);
    vecStk.push_back(2);
    vecStk.push_back(3);

    cout << "栈顶: " << vecStk.back() << endl;  // 3
    vecStk.pop_back();
    cout << "pop 后栈顶: " << vecStk.back() << endl;  // 2

    // vector 优势：可以遍历
    cout << "遍历: ";
    for (int x : vecStk) cout << x << " ";
    cout << endl;
}

// ============================================================================
//  第 2 章：手写栈
// ============================================================================

class ArrayStack {
    vector<int> data;
public:
    void push(int val) { data.push_back(val); }

    void pop() {
        if (data.empty()) throw runtime_error("Stack underflow");
        data.pop_back();
    }

    int top() {
        if (data.empty()) throw runtime_error("Stack is empty");
        return data.back();
    }

    bool empty() { return data.empty(); }
    int size() { return (int)data.size(); }
};

class LinkedStack {
    struct Node {
        int val;
        Node* next;
        Node(int v, Node* n = nullptr) : val(v), next(n) {}
    };
    Node* head = nullptr;
    int count = 0;

public:
    void push(int val) { head = new Node(val, head); count++; }

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

void demo_custom_stack() {
    cout << "\n========== 2. 手写栈 ==========\n";

    // 数组栈
    ArrayStack as;
    as.push(10); as.push(20); as.push(30);
    cout << "ArrayStack top: " << as.top() << endl;  // 30
    as.pop();
    cout << "ArrayStack pop 后 top: " << as.top() << endl;  // 20

    // 链表栈
    LinkedStack ls;
    ls.push(100); ls.push(200); ls.push(300);
    cout << "LinkedStack top: " << ls.top() << endl;  // 300
    ls.pop();
    cout << "LinkedStack pop 后 top: " << ls.top() << endl;  // 200
}

// ============================================================================
//  第 3 章：括号匹配
// ============================================================================

// LC 20: 有效的括号
bool isValid(string s) {
    stack<char> stk;
    unordered_map<char, char> pairs = {
        {')', '('}, {']', '['}, {'}', '{'}
    };
    for (char c : s) {
        if (pairs.count(c)) {
            if (stk.empty() || stk.top() != pairs[c]) return false;
            stk.pop();
        } else {
            stk.push(c);
        }
    }
    return stk.empty();
}

// LC 32: 最长有效括号
int longestValidParentheses(string s) {
    stack<int> stk;
    stk.push(-1);   // 哨兵
    int maxLen = 0;

    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '(') {
            stk.push(i);
        } else {
            stk.pop();
            if (stk.empty()) {
                stk.push(i);    // 新基准
            } else {
                maxLen = max(maxLen, i - stk.top());
            }
        }
    }
    return maxLen;
}

// LC 22: 括号生成
void generateHelper(int n, int open, int close, string& curr, vector<string>& result) {
    if ((int)curr.size() == 2 * n) {
        result.push_back(curr);
        return;
    }
    if (open < n) {
        curr.push_back('(');
        generateHelper(n, open + 1, close, curr, result);
        curr.pop_back();
    }
    if (close < open) {
        curr.push_back(')');
        generateHelper(n, open, close + 1, curr, result);
        curr.pop_back();
    }
}

vector<string> generateParenthesis(int n) {
    vector<string> result;
    string curr;
    generateHelper(n, 0, 0, curr, result);
    return result;
}

void demo_parentheses() {
    cout << "\n========== 3. 括号匹配 ==========\n";

    // 有效的括号
    cout << "\"()[]{}\" 有效? " << (isValid("()[]{}") ? "是" : "否") << endl;
    cout << "\"([)]\" 有效? " << (isValid("([)]") ? "是" : "否") << endl;
    cout << "\"((\" 有效? " << (isValid("((") ? "是" : "否") << endl;
    cout << "\"{[]}\" 有效? " << (isValid("{[]}") ? "是" : "否") << endl;

    // 最长有效括号
    cout << "\n最长有效括号 \")()())\": " << longestValidParentheses(")()())") << endl;  // 4
    cout << "最长有效括号 \"(()\": " << longestValidParentheses("(()") << endl;         // 2

    // 括号生成
    auto result = generateParenthesis(3);
    cout << "\n生成 3 对括号:" << endl;
    for (const string& s : result)
        cout << "  " << s << endl;
}

// ============================================================================
//  第 4 章：最小栈
// ============================================================================

class MinStack {
    stack<int> dataStack;
    stack<int> minStack;

public:
    void push(int val) {
        dataStack.push(val);
        if (minStack.empty() || val <= minStack.top())
            minStack.push(val);
    }

    void pop() {
        if (dataStack.top() == minStack.top())
            minStack.pop();
        dataStack.pop();
    }

    int top() { return dataStack.top(); }
    int getMin() { return minStack.top(); }
};

// 方法 2：pair 法
class MinStack2 {
    stack<pair<int,int>> stk;  // {值, 当前最小}

public:
    void push(int val) {
        int curMin = stk.empty() ? val : min(val, stk.top().second);
        stk.push({val, curMin});
    }
    void pop() { stk.pop(); }
    int top() { return stk.top().first; }
    int getMin() { return stk.top().second; }
};

void demo_min_stack() {
    cout << "\n========== 4. 最小栈 ==========\n";

    MinStack ms;
    ms.push(-2);
    ms.push(0);
    ms.push(-3);
    cout << "getMin: " << ms.getMin() << endl;  // -3
    ms.pop();
    cout << "top: " << ms.top() << endl;        // 0
    cout << "getMin: " << ms.getMin() << endl;  // -2

    cout << "\n--- MinStack2 (pair 法) ---" << endl;
    MinStack2 ms2;
    ms2.push(5);
    ms2.push(3);
    ms2.push(7);
    cout << "getMin: " << ms2.getMin() << endl;  // 3
    ms2.pop();
    cout << "getMin: " << ms2.getMin() << endl;  // 3
    ms2.pop();
    cout << "getMin: " << ms2.getMin() << endl;  // 5
}

// ============================================================================
//  第 5 章：单调栈
// ============================================================================

// LC 739: 每日温度
vector<int> dailyTemperatures(vector<int>& temperatures) {
    int n = temperatures.size();
    vector<int> result(n, 0);
    stack<int> stk;  // 单调递减栈（存下标）

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && temperatures[stk.top()] < temperatures[i]) {
            int prevIndex = stk.top(); stk.pop();
            result[prevIndex] = i - prevIndex;
        }
        stk.push(i);
    }
    return result;
}

// LC 496: 下一个更大元素 I
vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    unordered_map<int, int> ng;
    stack<int> stk;

    for (int num : nums2) {
        while (!stk.empty() && stk.top() < num) {
            ng[stk.top()] = num;
            stk.pop();
        }
        stk.push(num);
    }
    while (!stk.empty()) { ng[stk.top()] = -1; stk.pop(); }

    vector<int> result;
    for (int num : nums1) result.push_back(ng[num]);
    return result;
}

// LC 503: 下一个更大元素 II（循环数组）
vector<int> nextGreaterElements(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> stk;

    for (int i = 0; i < 2 * n; i++) {
        int idx = i % n;
        while (!stk.empty() && nums[stk.top()] < nums[idx]) {
            result[stk.top()] = nums[idx];
            stk.pop();
        }
        if (i < n) stk.push(i);
    }
    return result;
}

// LC 84: 柱状图中最大的矩形
int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    stack<int> stk;
    int maxArea = 0;

    for (int i = 0; i <= n; i++) {
        int h = (i == n) ? 0 : heights[i];
        while (!stk.empty() && heights[stk.top()] > h) {
            int height = heights[stk.top()]; stk.pop();
            int width = stk.empty() ? i : (i - stk.top() - 1);
            maxArea = max(maxArea, height * width);
        }
        stk.push(i);
    }
    return maxArea;
}

// LC 42: 接雨水（单调栈法）
int trap(vector<int>& height) {
    stack<int> stk;
    int water = 0;

    for (int i = 0; i < (int)height.size(); i++) {
        while (!stk.empty() && height[stk.top()] < height[i]) {
            int bottom = stk.top(); stk.pop();
            if (stk.empty()) break;
            int width = i - stk.top() - 1;
            int h = min(height[i], height[stk.top()]) - height[bottom];
            water += width * h;
        }
        stk.push(i);
    }
    return water;
}

void demo_monotonic_stack() {
    cout << "\n========== 5. 单调栈 ==========\n";

    // 每日温度
    vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    auto days = dailyTemperatures(temps);
    cout << "每日温度: ";
    for (int d : days) cout << d << " ";
    cout << endl;  // 1 1 4 2 1 1 0 0

    // 下一个更大元素 I
    vector<int> nums1 = {4, 1, 2}, nums2 = {1, 3, 4, 2};
    auto nge = nextGreaterElement(nums1, nums2);
    cout << "下一个更大元素: ";
    for (int x : nge) cout << x << " ";
    cout << endl;  // -1 3 -1

    // 下一个更大元素 II（循环）
    vector<int> nums3 = {1, 2, 1};
    auto nge2 = nextGreaterElements(nums3);
    cout << "循环-下一个更大: ";
    for (int x : nge2) cout << x << " ";
    cout << endl;  // 2 -1 2

    // 柱状图最大矩形
    vector<int> heights = {2, 1, 5, 6, 2, 3};
    cout << "最大矩形面积: " << largestRectangleArea(heights) << endl;  // 10

    // 接雨水
    vector<int> h = {0,1,0,2,1,0,1,3,2,1,2,1};
    cout << "接雨水: " << trap(h) << endl;  // 6
}

// ============================================================================
//  第 6 章：表达式求值
// ============================================================================

// LC 150: 逆波兰表达式求值
int evalRPN(vector<string>& tokens) {
    stack<int> stk;
    for (const string& t : tokens) {
        if (t == "+" || t == "-" || t == "*" || t == "/") {
            int b = stk.top(); stk.pop();
            int a = stk.top(); stk.pop();
            if (t == "+") stk.push(a + b);
            else if (t == "-") stk.push(a - b);
            else if (t == "*") stk.push(a * b);
            else stk.push(a / b);
        } else {
            stk.push(stoi(t));
        }
    }
    return stk.top();
}

// LC 224: 基本计算器
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
    return result + sign * num;
}

// LC 227: 基本计算器 II
int calculate2(string s) {
    stack<int> stk;
    int num = 0;
    char prevOp = '+';
    for (int i = 0; i < (int)s.size(); i++) {
        char c = s[i];
        if (isdigit(c)) num = num * 10 + (c - '0');
        if ((!isdigit(c) && c != ' ') || i == (int)s.size() - 1) {
            if (prevOp == '+') stk.push(num);
            else if (prevOp == '-') stk.push(-num);
            else if (prevOp == '*') { int t = stk.top(); stk.pop(); stk.push(t * num); }
            else if (prevOp == '/') { int t = stk.top(); stk.pop(); stk.push(t / num); }
            prevOp = c;
            num = 0;
        }
    }
    int result = 0;
    while (!stk.empty()) { result += stk.top(); stk.pop(); }
    return result;
}

void demo_expression() {
    cout << "\n========== 6. 表达式求值 ==========\n";

    // 逆波兰
    vector<string> rpn = {"2", "1", "+", "3", "*"};
    cout << "RPN [2,1,+,3,*] = " << evalRPN(rpn) << endl;  // 9

    vector<string> rpn2 = {"4", "13", "5", "/", "+"};
    cout << "RPN [4,13,5,/,+] = " << evalRPN(rpn2) << endl;  // 6

    // 基本计算器
    cout << "\"(1+(4+5+2)-3)+(6+8)\" = " << calculate("(1+(4+5+2)-3)+(6+8)") << endl;  // 23

    // 基本计算器 II
    cout << "\"3+2*2\" = " << calculate2("3+2*2") << endl;           // 7
    cout << "\" 3/2 \" = " << calculate2(" 3/2 ") << endl;          // 1
    cout << "\" 3+5 / 2 \" = " << calculate2(" 3+5 / 2 ") << endl; // 5
}

// ============================================================================
//  第 7 章：栈模拟题
// ============================================================================

// LC 394: 字符串解码
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

// LC 71: 简化路径
string simplifyPath(string path) {
    stack<string> stk;
    istringstream ss(path);
    string token;

    while (getline(ss, token, '/')) {
        if (token.empty() || token == ".") continue;
        if (token == "..") {
            if (!stk.empty()) stk.pop();
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

// LC 316: 去除重复字母
string removeDuplicateLetters(string s) {
    int lastOcc[26] = {};
    bool inStack[26] = {};
    for (int i = 0; i < (int)s.size(); i++) lastOcc[s[i]-'a'] = i;

    string stk;
    for (int i = 0; i < (int)s.size(); i++) {
        char c = s[i];
        if (inStack[c-'a']) continue;
        while (!stk.empty() && stk.back() > c && lastOcc[stk.back()-'a'] > i) {
            inStack[stk.back()-'a'] = false;
            stk.pop_back();
        }
        stk.push_back(c);
        inStack[c-'a'] = true;
    }
    return stk;
}

// LC 232: 用栈实现队列
class MyQueue {
    stack<int> inStack, outStack;
    void transfer() {
        while (!inStack.empty()) {
            outStack.push(inStack.top());
            inStack.pop();
        }
    }
public:
    void push(int x) { inStack.push(x); }
    int pop() {
        if (outStack.empty()) transfer();
        int val = outStack.top(); outStack.pop();
        return val;
    }
    int peek() {
        if (outStack.empty()) transfer();
        return outStack.top();
    }
    bool empty() { return inStack.empty() && outStack.empty(); }
};

void demo_stack_simulation() {
    cout << "\n========== 7. 栈模拟题 ==========\n";

    // 字符串解码
    cout << "decode \"3[a2[c]]\": " << decodeString("3[a2[c]]") << endl;        // accaccacc
    cout << "decode \"2[abc]3[cd]ef\": " << decodeString("2[abc]3[cd]ef") << endl;  // abcabccdcdcdef

    // 简化路径
    cout << "simplify \"/a/./b/../../c/\": " << simplifyPath("/a/./b/../../c/") << endl;  // /c
    cout << "simplify \"/home//foo/\": " << simplifyPath("/home//foo/") << endl;          // /home/foo

    // 去除重复字母
    cout << "removeDup \"bcabc\": " << removeDuplicateLetters("bcabc") << endl;      // abc
    cout << "removeDup \"cbacdcbc\": " << removeDuplicateLetters("cbacdcbc") << endl;  // acdb

    // 用栈实现队列
    cout << "\n--- 用栈实现队列 ---" << endl;
    MyQueue q;
    q.push(1); q.push(2);
    cout << "peek: " << q.peek() << endl;     // 1
    cout << "pop: " << q.pop() << endl;        // 1
    cout << "empty: " << q.empty() << endl;    // 0
}

// ============================================================================
//  第 8 章：常见陷阱
// ============================================================================

void demo_pitfalls() {
    cout << "\n========== 8. 常见陷阱 ==========\n";

    // 陷阱 1：空栈 top/pop
    stack<int> stk;
    cout << "陷阱 1: 空栈操作" << endl;
    if (stk.empty()) {
        cout << "  ✅ stk 为空，不能调用 top()/pop()" << endl;
    }

    // 陷阱 2：pop() 返回 void
    stk.push(42);
    // int val = stk.pop();  // ❌ 编译错误！
    int val = stk.top(); stk.pop();  // ✅
    cout << "陷阱 2: pop()返回void → 先top()再pop(), val=" << val << endl;

    // 陷阱 3：RPN 操作数顺序
    cout << "陷阱 3: RPN \"a-b\" → b先出栈, a后出栈, 结果是 a-b" << endl;

    // 陷阱 4：单调栈方向
    cout << "陷阱 4: 下一个更大→递减栈, 下一个更小→递增栈" << endl;

    cout << "\n--- 最佳实践 ---" << endl;
    cout << "  ✅ 操作前检查 empty()" << endl;
    cout << "  ✅ 用 vector 模拟栈更灵活" << endl;
    cout << "  ✅ 栈中存下标比存值更灵活" << endl;
    cout << "  ✅ 哨兵简化边界（如 push(-1)）" << endl;
}

// ============================================================================
//  主函数
// ============================================================================

int main() {
    cout << "╔══════════════════════════════════════════╗\n";
    cout << "║   专题三（上）：栈 (Stack) 完整示例代码    ║\n";
    cout << "╚══════════════════════════════════════════╝\n";

    demo_stack_basics();          // 1. 栈基础
    demo_custom_stack();          // 2. 手写栈
    demo_parentheses();           // 3. 括号匹配
    demo_min_stack();             // 4. 最小栈
    demo_monotonic_stack();       // 5. 单调栈
    demo_expression();            // 6. 表达式求值
    demo_stack_simulation();      // 7. 栈模拟题
    demo_pitfalls();              // 8. 陷阱

    cout << "\n✅ 所有栈示例运行完毕！\n";
    return 0;
}
