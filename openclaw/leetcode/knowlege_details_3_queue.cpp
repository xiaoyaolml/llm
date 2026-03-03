/**
 * ============================================================
 *   专题三（下）：队列（Queue）示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_3_queue knowlege_details_3_queue.cpp
 *   配合 knowlege_details_3_queue.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <queue>
#include <deque>
#include <stack>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <stdexcept>
using namespace std;

// ============================================================
//  工具函数
// ============================================================
void printVec(const vector<int>& v, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    cout << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        if (i) cout << ", ";
        cout << v[i];
    }
    cout << "]" << endl;
}

// ============================================================
//  Demo 1: 队列基础用法
//  演示 std::queue 和 std::deque 的基本操作
// ============================================================
void demo_queue_basics() {
    cout << "\n===== Demo 1: 队列基础用法 =====" << endl;

    // ---------- std::queue ----------
    cout << "\n--- std::queue ---" << endl;
    queue<int> q;

    // 入队
    q.push(10);
    q.push(20);
    q.push(30);
    q.push(40);
    cout << "入队 10, 20, 30, 40" << endl;
    cout << "队头 front(): " << q.front() << endl;  // 10
    cout << "队尾 back():  " << q.back()  << endl;  // 40
    cout << "队列大小:     " << q.size()  << endl;  // 4

    // 出队
    q.pop();  // 弹出 10
    cout << "pop() 后队头: " << q.front() << endl;  // 20

    // 遍历（队列没有迭代器，只能逐个出队）
    cout << "遍历出队: ";
    while (!q.empty()) {
        cout << q.front() << " ";
        q.pop();
    }
    cout << endl;

    // ---------- std::deque ----------
    cout << "\n--- std::deque (双端队列) ---" << endl;
    deque<int> dq;

    dq.push_back(1);    // [1]
    dq.push_back(2);    // [1, 2]
    dq.push_front(0);   // [0, 1, 2]
    dq.push_front(-1);  // [-1, 0, 1, 2]

    cout << "deque: [";
    for (int i = 0; i < (int)dq.size(); i++) {
        if (i) cout << ", ";
        cout << dq[i];   // 支持随机访问
    }
    cout << "]" << endl;  // [-1, 0, 1, 2]

    dq.pop_front();  // 弹出 -1
    dq.pop_back();   // 弹出 2
    cout << "pop_front + pop_back 后: [";
    for (int i = 0; i < (int)dq.size(); i++) {
        if (i) cout << ", ";
        cout << dq[i];
    }
    cout << "]" << endl;  // [0, 1]
}

// ============================================================
//  Demo 2: 手写队列（链表 + 数组两种实现）
// ============================================================

// 链表队列
class LinkedQueue {
    struct Node {
        int val;
        Node* next;
        Node(int v) : val(v), next(nullptr) {}
    };
    Node* head = nullptr;  // 出队端
    Node* tail = nullptr;  // 入队端
    int count = 0;

public:
    void push(int val) {
        Node* node = new Node(val);
        if (tail) tail->next = node;
        tail = node;
        if (!head) head = node;  // 第一个元素
        count++;
    }

    void pop() {
        if (!head) throw runtime_error("Queue is empty");
        Node* old = head;
        head = head->next;
        if (!head) tail = nullptr;
        delete old;
        count--;
    }

    int front() const {
        if (!head) throw runtime_error("Queue is empty");
        return head->val;
    }

    bool empty() const { return head == nullptr; }
    int size() const { return count; }

    ~LinkedQueue() {
        while (head) {
            Node* t = head;
            head = head->next;
            delete t;
        }
    }
};

// 环形数组队列
class ArrayQueue {
    vector<int> data;
    int headIdx = 0;
    int count = 0;

public:
    ArrayQueue(int capacity) : data(capacity) {}

    void push(int val) {
        if (count == (int)data.size()) throw runtime_error("Queue full");
        int tailIdx = (headIdx + count) % data.size();
        data[tailIdx] = val;
        count++;
    }

    void pop() {
        if (count == 0) throw runtime_error("Queue empty");
        headIdx = (headIdx + 1) % data.size();
        count--;
    }

    int front() const {
        if (count == 0) throw runtime_error("Queue empty");
        return data[headIdx];
    }

    bool empty() const { return count == 0; }
    int size() const { return count; }
};

void demo_custom_queue() {
    cout << "\n===== Demo 2: 手写队列 =====" << endl;

    // 链表队列
    cout << "\n--- 链表队列 ---" << endl;
    LinkedQueue lq;
    lq.push(100);
    lq.push(200);
    lq.push(300);
    cout << "入队 100, 200, 300" << endl;
    cout << "front: " << lq.front() << endl;  // 100
    lq.pop();
    cout << "pop 后 front: " << lq.front() << endl;  // 200
    cout << "size: " << lq.size() << endl;  // 2

    // 数组队列
    cout << "\n--- 环形数组队列 ---" << endl;
    ArrayQueue aq(5);
    aq.push(1); aq.push(2); aq.push(3);
    cout << "入队 1, 2, 3 → front: " << aq.front() << endl;  // 1
    aq.pop();
    aq.push(4); aq.push(5); aq.push(6);
    // 环形特性：pop 了一个位置后可以再 push 一个
    cout << "pop 后再 push 4,5,6 → front: " << aq.front() << endl;  // 2
    cout << "size: " << aq.size() << endl;  // 5
}

// ============================================================
//  Demo 3: 用栈实现队列 / 用队列实现栈
//  LC 232: Implement Queue using Stacks
//  LC 225: Implement Stack using Queues
// ============================================================

// LC 232: 用两个栈实现队列
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
        int val = outStack.top();
        outStack.pop();
        return val;
    }

    int peek() {
        if (outStack.empty()) transfer();
        return outStack.top();
    }

    bool empty() { return inStack.empty() && outStack.empty(); }
};

// LC 225: 用一个队列实现栈
class MyStack {
    queue<int> q;

public:
    void push(int x) {
        q.push(x);
        int sz = q.size();
        // 把 x 前面的元素全部重新排到后面
        for (int i = 0; i < sz - 1; i++) {
            q.push(q.front());
            q.pop();
        }
    }

    int pop() { int v = q.front(); q.pop(); return v; }
    int top() { return q.front(); }
    bool empty() { return q.empty(); }
};

void demo_stack_queue_convert() {
    cout << "\n===== Demo 3: 栈/队列互相实现 =====" << endl;

    // 用栈实现队列
    cout << "\n--- LC 232: 用栈实现队列 ---" << endl;
    MyQueue mq;
    mq.push(1); mq.push(2); mq.push(3);
    cout << "push 1,2,3 → peek: " << mq.peek() << endl;  // 1 (FIFO)
    cout << "pop: " << mq.pop() << endl;   // 1
    cout << "pop: " << mq.pop() << endl;   // 2
    mq.push(4);
    cout << "push 4 → pop: " << mq.pop() << endl;  // 3 (不是4)
    cout << "pop: " << mq.pop() << endl;   // 4

    // 用队列实现栈
    cout << "\n--- LC 225: 用队列实现栈 ---" << endl;
    MyStack ms;
    ms.push(10); ms.push(20); ms.push(30);
    cout << "push 10,20,30 → top: " << ms.top() << endl;  // 30 (LIFO)
    cout << "pop: " << ms.pop() << endl;   // 30
    cout << "pop: " << ms.pop() << endl;   // 20
    cout << "pop: " << ms.pop() << endl;   // 10
}

// ============================================================
//  Demo 4: BFS — 二叉树层序遍历 & 岛屿数量
//  LC 102: Binary Tree Level Order Traversal
//  LC 200: Number of Islands
// ============================================================

// 简单树节点
struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

// LC 102: 层序遍历
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (!root) return result;

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int levelSize = q.size();  // 当前层的节点数
        vector<int> level;

        for (int i = 0; i < levelSize; i++) {
            TreeNode* node = q.front(); q.pop();
            level.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
        result.push_back(level);
    }
    return result;
}

// LC 200: 岛屿数量 (BFS 版)
int numIslands(vector<vector<char>>& grid) {
    if (grid.empty()) return 0;
    int m = grid.size(), n = grid[0].size();
    int count = 0;
    int dirs[] = {-1, 0, 1, 0, -1};

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') {
                count++;
                grid[i][j] = '0';  // 标记已访问
                queue<pair<int,int>> q;
                q.push({i, j});

                while (!q.empty()) {
                    auto [r, c] = q.front(); q.pop();
                    for (int d = 0; d < 4; d++) {
                        int nr = r + dirs[d], nc = c + dirs[d+1];
                        if (nr >= 0 && nr < m && nc >= 0 && nc < n
                            && grid[nr][nc] == '1') {
                            grid[nr][nc] = '0';
                            q.push({nr, nc});
                        }
                    }
                }
            }
        }
    }
    return count;
}

void demo_bfs() {
    cout << "\n===== Demo 4: BFS 应用 =====" << endl;

    // 构建二叉树:
    //       3
    //      / \
    //     9   20
    //        / \
    //       15  7
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(9);
    root->right = new TreeNode(20);
    root->right->left = new TreeNode(15);
    root->right->right = new TreeNode(7);

    cout << "\n--- LC 102: 层序遍历 ---" << endl;
    cout << "树:   3" << endl;
    cout << "     / \\" << endl;
    cout << "    9  20" << endl;
    cout << "      / \\" << endl;
    cout << "     15  7" << endl;

    auto levels = levelOrder(root);
    for (int i = 0; i < (int)levels.size(); i++) {
        cout << "第 " << i << " 层: ";
        printVec(levels[i]);
    }
    // 第 0 层: [3]
    // 第 1 层: [9, 20]
    // 第 2 层: [15, 7]

    // 释放内存
    delete root->right->right;
    delete root->right->left;
    delete root->right;
    delete root->left;
    delete root;

    // LC 200: 岛屿数量
    cout << "\n--- LC 200: 岛屿数量 (BFS) ---" << endl;
    vector<vector<char>> grid = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    cout << "网格:" << endl;
    for (auto& row : grid) {
        for (char c : row) cout << c << " ";
        cout << endl;
    }
    int islands = numIslands(grid);
    cout << "岛屿数量: " << islands << endl;  // 3
}

// ============================================================
//  Demo 5: 优先队列 / 堆
//  LC 215: 第K个最大元素
//  LC 347: 前K个高频元素
// ============================================================

// LC 215: 第K个最大元素
int findKthLargest(vector<int>& nums, int k) {
    // 小顶堆，维护 k 个最大元素
    priority_queue<int, vector<int>, greater<int>> minPQ;

    for (int num : nums) {
        minPQ.push(num);
        if ((int)minPQ.size() > k)
            minPQ.pop();  // 弹掉最小的
    }
    return minPQ.top();   // 堆顶 = 第 k 大
}

// LC 347: 前K个高频元素
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int num : nums) freq[num]++;

    // 小顶堆：按频率排序
    auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
        return a.second > b.second;
    };
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);

    for (auto& [num, cnt] : freq) {
        pq.push({num, cnt});
        if ((int)pq.size() > k) pq.pop();
    }

    vector<int> result;
    while (!pq.empty()) {
        result.push_back(pq.top().first);
        pq.pop();
    }
    return result;
}

void demo_priority_queue() {
    cout << "\n===== Demo 5: 优先队列 / 堆 =====" << endl;

    // 大顶堆 vs 小顶堆
    cout << "\n--- 大顶堆 vs 小顶堆 ---" << endl;
    priority_queue<int> maxPQ;             // 大顶堆（默认）
    priority_queue<int, vector<int>, greater<int>> minPQ;  // 小顶堆

    for (int x : {3, 1, 5, 2, 4}) {
        maxPQ.push(x);
        minPQ.push(x);
    }

    cout << "大顶堆出队: ";
    while (!maxPQ.empty()) { cout << maxPQ.top() << " "; maxPQ.pop(); }
    cout << endl;  // 5 4 3 2 1

    cout << "小顶堆出队: ";
    while (!minPQ.empty()) { cout << minPQ.top() << " "; minPQ.pop(); }
    cout << endl;  // 1 2 3 4 5

    // LC 215: 第K个最大元素
    cout << "\n--- LC 215: 第K个最大元素 ---" << endl;
    vector<int> nums1 = {3, 2, 1, 5, 6, 4};
    int k1 = 2;
    cout << "nums = "; printVec(nums1);
    cout << "第 " << k1 << " 个最大元素: " << findKthLargest(nums1, k1) << endl;  // 5

    // LC 347: 前K个高频元素
    cout << "\n--- LC 347: 前K个高频元素 ---" << endl;
    vector<int> nums2 = {1, 1, 1, 2, 2, 3};
    int k2 = 2;
    cout << "nums = "; printVec(nums2);
    auto freq = topKFrequent(nums2, k2);
    cout << "前 " << k2 << " 个高频元素: "; printVec(freq);  // [1, 2]（顺序可能不同）
}

// ============================================================
//  Demo 6: 单调队列 — 滑动窗口最大值
//  LC 239: Sliding Window Maximum
// ============================================================

vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;      // 存下标，维护单调递减
    vector<int> result;

    for (int i = 0; i < (int)nums.size(); i++) {
        // 1. 移除超出窗口的元素
        while (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        // 2. 维护单调递减：弹出所有比当前小的
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();

        dq.push_back(i);

        // 3. 窗口形成后（i >= k-1），队头就是最大值
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }
    return result;
}

void demo_monotonic_queue() {
    cout << "\n===== Demo 6: 单调队列 =====" << endl;

    cout << "\n--- LC 239: 滑动窗口最大值 ---" << endl;
    vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    cout << "nums = "; printVec(nums);
    cout << "k = " << k << endl;

    auto result = maxSlidingWindow(nums, k);
    printVec(result, "最大值序列");  // [3, 3, 5, 5, 6, 7]

    // 图解过程
    cout << "\n过程详解:" << endl;
    cout << "窗口 [1, 3, -1]  → 最大值 3" << endl;
    cout << "窗口 [3, -1, -3] → 最大值 3" << endl;
    cout << "窗口 [-1, -3, 5] → 最大值 5" << endl;
    cout << "窗口 [-3, 5, 3]  → 最大值 5" << endl;
    cout << "窗口 [5, 3, 6]   → 最大值 6" << endl;
    cout << "窗口 [3, 6, 7]   → 最大值 7" << endl;
}

// ============================================================
//  Demo 7: 循环队列
//  LC 622: Design Circular Queue
// ============================================================

class MyCircularQueue {
    vector<int> data;
    int head = 0, count = 0;

public:
    MyCircularQueue(int k) : data(k) {}

    bool enQueue(int val) {
        if (isFull()) return false;
        int tail = (head + count) % data.size();
        data[tail] = val;
        count++;
        return true;
    }

    bool deQueue() {
        if (isEmpty()) return false;
        head = (head + 1) % data.size();
        count--;
        return true;
    }

    int Front() { return isEmpty() ? -1 : data[head]; }

    int Rear() {
        if (isEmpty()) return -1;
        int tail = (head + count - 1) % data.size();
        return data[tail];
    }

    bool isEmpty() { return count == 0; }
    bool isFull()  { return count == (int)data.size(); }
};

void demo_circular_queue() {
    cout << "\n===== Demo 7: 循环队列 =====" << endl;

    cout << "\n--- LC 622: 设计循环队列 ---" << endl;
    MyCircularQueue cq(3);  // 容量 3

    cout << "enQueue(1): " << (cq.enQueue(1) ? "true" : "false") << endl;  // true
    cout << "enQueue(2): " << (cq.enQueue(2) ? "true" : "false") << endl;  // true
    cout << "enQueue(3): " << (cq.enQueue(3) ? "true" : "false") << endl;  // true
    cout << "enQueue(4): " << (cq.enQueue(4) ? "true" : "false") << endl;  // false（已满）

    cout << "Rear:  " << cq.Rear()  << endl;  // 3
    cout << "isFull: " << (cq.isFull() ? "true" : "false") << endl;  // true

    cout << "deQueue: " << (cq.deQueue() ? "true" : "false") << endl;  // true
    cout << "enQueue(4): " << (cq.enQueue(4) ? "true" : "false") << endl;  // true（环形复用）
    cout << "Front: " << cq.Front() << endl;  // 2
    cout << "Rear:  " << cq.Rear()  << endl;  // 4

    // 图解环形队列
    cout << "\n环形队列内部状态:" << endl;
    cout << "容量 3，下标 0,1,2 循环使用" << endl;
    cout << "enQueue(1,2,3): [1, 2, 3]  head=0" << endl;
    cout << "deQueue:        [_, 2, 3]  head=1" << endl;
    cout << "enQueue(4):     [4, 2, 3]  head=1, tail 回绕到 0" << endl;
}

// ============================================================
//  Demo 8: 综合演练 — 数据流中位数 & 任务调度
//  LC 295: Find Median from Data Stream
//  LC 621: Task Scheduler
// ============================================================

// LC 295: 数据流中位数（双堆法）
class MedianFinder {
    priority_queue<int> lo;               // 大顶堆：存较小的一半
    priority_queue<int, vector<int>,
                   greater<int>> hi;      // 小顶堆：存较大的一半

public:
    void addNum(int num) {
        lo.push(num);                     // 先入大顶堆

        // 保证大顶堆的最大值 ≤ 小顶堆的最小值
        hi.push(lo.top()); lo.pop();

        // 保证大顶堆的大小 ≥ 小顶堆的大小
        if (lo.size() < hi.size()) {
            lo.push(hi.top()); hi.pop();
        }
    }

    double findMedian() {
        if (lo.size() > hi.size())
            return lo.top();              // 奇数个，大顶堆多一个
        return (lo.top() + hi.top()) / 2.0;  // 偶数个，取平均
    }
};

// LC 621: 任务调度器（贪心法）
int leastInterval(vector<char>& tasks, int n) {
    vector<int> freq(26, 0);
    for (char t : tasks) freq[t - 'A']++;

    int maxFreq = *max_element(freq.begin(), freq.end());
    int maxCount = count(freq.begin(), freq.end(), maxFreq);

    // 框架: (maxFreq - 1) * (n + 1) + maxCount
    // 最终答案 = max(框架时间, 任务总数)
    return max((int)tasks.size(), (maxFreq - 1) * (n + 1) + maxCount);
}

void demo_advanced() {
    cout << "\n===== Demo 8: 综合演练 =====" << endl;

    // LC 295: 数据流中位数
    cout << "\n--- LC 295: 数据流中位数 ---" << endl;
    MedianFinder mf;
    mf.addNum(1);
    cout << "add(1)  → 中位数: " << mf.findMedian() << endl;  // 1
    mf.addNum(2);
    cout << "add(2)  → 中位数: " << mf.findMedian() << endl;  // 1.5
    mf.addNum(3);
    cout << "add(3)  → 中位数: " << mf.findMedian() << endl;  // 2
    mf.addNum(4);
    cout << "add(4)  → 中位数: " << mf.findMedian() << endl;  // 2.5
    mf.addNum(5);
    cout << "add(5)  → 中位数: " << mf.findMedian() << endl;  // 3

    cout << "\n原理: 大顶堆存较小一半，小顶堆存较大一半" << endl;
    cout << "       lo (大顶堆): [1,2,3]  hi (小顶堆): [4,5]" << endl;
    cout << "       中位数 = lo.top() = 3" << endl;

    // LC 621: 任务调度器
    cout << "\n--- LC 621: 任务调度器 ---" << endl;
    vector<char> tasks = {'A','A','A','B','B','B'};
    int n = 2;
    cout << "tasks = [A,A,A,B,B,B], n = " << n << endl;
    cout << "最少时间: " << leastInterval(tasks, n) << endl;  // 8
    cout << "调度: A B _ A B _ A B" << endl;
    cout << "公式: (maxFreq-1)*(n+1)+maxCount = 2*3+2 = 8" << endl;
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题三（下）：队列 Queue 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_queue_basics();         // Demo 1: 队列基础
    demo_custom_queue();         // Demo 2: 手写队列
    demo_stack_queue_convert();  // Demo 3: 栈/队列互相实现
    demo_bfs();                  // Demo 4: BFS
    demo_priority_queue();       // Demo 5: 优先队列/堆
    demo_monotonic_queue();      // Demo 6: 单调队列
    demo_circular_queue();       // Demo 7: 循环队列
    demo_advanced();             // Demo 8: 综合演练

    cout << "\n============================================" << endl;
    cout << "  所有队列 Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 102, 200, 215, 225, 232," << endl;
    cout << "  239, 295, 347, 621, 622, 752, 994" << endl;
    cout << "============================================" << endl;

    return 0;
}
