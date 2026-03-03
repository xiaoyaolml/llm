/**
 * 专题十四：堆与优先队列 — 从原理到LeetCode实战 示例代码
 * 作者：大胖超 😜
 *
 * 编译: g++ -std=c++17 -o knowlege_details_14_heap knowlege_details_14_heap.cpp
 * 运行: ./knowlege_details_14_heap
 *
 * 覆盖内容：
 *   Demo1: 手写最小堆（完整实现 + 操作追踪）
 *   Demo2: priority_queue 用法与自定义比较器
 *   Demo3: Top-K 问题 — LC 215 第K大 + LC 347 前K高频 + LC 973 最近K点
 *   Demo4: 动态中位数 — LC 295 数据流中位数
 *   Demo5: 多路归并 — LC 23 合并K链表 + LC 378 有序矩阵第K小
 *   Demo6: 任务调度 — LC 621 任务调度器 + LC 767 重组字符串
 *   Demo7: 懒删除堆（滑动窗口最大值示例）
 *   Demo8: LC 264 丑数II（三路归并）
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <tuple>
#include <set>
#include <climits>
using namespace std;

// ==================== 辅助函数 ====================
void printVec(const vector<int>& v, const string& label = "") {
    if (!label.empty()) cout << label;
    cout << "[";
    for (int i = 0; i < (int)v.size(); i++)
        cout << v[i] << (i < (int)v.size()-1 ? ", " : "");
    cout << "]";
}
void printVecLn(const vector<int>& v, const string& label = "") {
    printVec(v, label); cout << "\n";
}

// ==================== Demo1: 手写最小堆 ====================
void demo1_handwrittenHeap() {
    cout << "===== Demo1: 手写最小堆 =====\n\n";

    class MinHeap {
        vector<int> data;

        void siftUp(int i) {
            while (i > 0) {
                int parent = (i - 1) / 2;
                if (data[i] < data[parent]) {
                    swap(data[i], data[parent]);
                    i = parent;
                } else break;
            }
        }

        void siftDown(int i) {
            int n = data.size();
            while (2 * i + 1 < n) {
                int child = 2 * i + 1;
                if (child + 1 < n && data[child + 1] < data[child])
                    child++;
                if (data[i] <= data[child]) break;
                swap(data[i], data[child]);
                i = child;
            }
        }

    public:
        void push(int val) {
            data.push_back(val);
            siftUp(data.size() - 1);
        }
        int top() { return data[0]; }
        void pop() {
            data[0] = data.back();
            data.pop_back();
            if (!data.empty()) siftDown(0);
        }
        bool empty() { return data.empty(); }
        int size() { return data.size(); }
        void print() {
            cout << "[";
            for (int i = 0; i < (int)data.size(); i++)
                cout << data[i] << (i < (int)data.size()-1 ? ", " : "");
            cout << "]";
        }
    };

    MinHeap heap;
    vector<int> inputs = {5, 3, 8, 1, 7, 2};
    cout << "依次插入: ";
    printVecLn(inputs);

    for (int x : inputs) {
        heap.push(x);
        cout << "  push(" << x << ") → 堆: ";
        heap.print();
        cout << ", top=" << heap.top() << "\n";
    }

    cout << "\n依次弹出:\n";
    while (!heap.empty()) {
        cout << "  pop() = " << heap.top();
        heap.pop();
        if (!heap.empty()) {
            cout << " → 堆: ";
            heap.print();
        }
        cout << "\n";
    }
    cout << "\n";
}

// ==================== Demo2: priority_queue 用法 ====================
void demo2_pqUsage() {
    cout << "===== Demo2: priority_queue 用法 =====\n\n";

    // --- 最大堆(默认) vs 最小堆 ---
    {
        cout << "--- 最大堆 vs 最小堆 ---\n";
        priority_queue<int> maxPQ;
        priority_queue<int, vector<int>, greater<int>> minPQ;

        vector<int> data = {3, 1, 4, 1, 5, 9};
        cout << "数据: "; printVecLn(data);

        for (int x : data) { maxPQ.push(x); minPQ.push(x); }

        cout << "最大堆 pop 顺序: ";
        while (!maxPQ.empty()) { cout << maxPQ.top() << " "; maxPQ.pop(); }
        cout << "\n最小堆 pop 顺序: ";
        while (!minPQ.empty()) { cout << minPQ.top() << " "; minPQ.pop(); }
        cout << "\n\n";
    }

    // --- 自定义比较器 ---
    {
        cout << "--- 自定义比较器 (按pair.second排序) ---\n";

        // 按 second 做最小堆
        auto cmp = [](const pair<string,int>& a, const pair<string,int>& b) {
            return a.second > b.second;  // second大的沉底 → second小的在顶
        };
        priority_queue<pair<string,int>,
                       vector<pair<string,int>>,
                       decltype(cmp)> pq(cmp);

        pq.push({"Alice", 90});
        pq.push({"Bob", 70});
        pq.push({"Charlie", 85});
        pq.push({"Diana", 60});

        cout << "按分数升序 pop:\n";
        while (!pq.empty()) {
            auto [name, score] = pq.top(); pq.pop();
            cout << "  " << name << ": " << score << "\n";
        }
        cout << "\n";
    }

    // --- 比较器方向演示 ---
    {
        cout << "--- 比较器方向: pq vs sort ---\n";
        vector<int> data = {3, 1, 4, 1, 5};

        // sort: cmp(a,b)=true → a在前
        auto sortData = data;
        sort(sortData.begin(), sortData.end(), [](int a, int b) { return a > b; });
        cout << "sort(a>b → 降序): "; printVecLn(sortData);

        // pq: cmp(a,b)=true → a沉底(优先级低)
        auto cmp = [](int a, int b) { return a > b; };
        priority_queue<int, vector<int>, decltype(cmp)> pq(cmp, data);
        cout << "pq (a>b → 最小堆) pop: ";
        while (!pq.empty()) { cout << pq.top() << " "; pq.pop(); }
        cout << "\n⚠️ 同一个 a>b: sort=降序, pq=升序(最小堆)，方向相反！\n\n";
    }
}

// ==================== Demo3: Top-K 问题 ====================
void demo3_topK() {
    cout << "===== Demo3: Top-K 问题 =====\n\n";

    // --- LC 215: 第K个最大元素 ---
    {
        cout << "--- LC 215: 第K大元素 ---\n";
        auto findKthLargest = [](vector<int> nums, int k) -> int {
            priority_queue<int, vector<int>, greater<int>> minHeap;
            for (int num : nums) {
                minHeap.push(num);
                if ((int)minHeap.size() > k) minHeap.pop();
            }
            return minHeap.top();
        };

        vector<int> nums = {3, 2, 1, 5, 6, 4};
        printVecLn(nums, "数组: ");
        cout << "第2大 = " << findKthLargest(nums, 2) << " (期望5)\n";

        cout << "过程(最小堆大小2):\n";
        priority_queue<int, vector<int>, greater<int>> demo;
        for (int x : nums) {
            demo.push(x);
            cout << "  push(" << x << ")";
            if ((int)demo.size() > 2) {
                cout << " → 大小" << demo.size() << ">2, pop " << demo.top();
                demo.pop();
            }
            cout << " → top=" << demo.top() << "\n";
        }
        cout << "\n";
    }

    // --- LC 347: 前K高频元素 ---
    {
        cout << "--- LC 347: 前K高频元素 ---\n";
        auto topKFrequent = [](vector<int> nums, int k) -> vector<int> {
            unordered_map<int, int> freq;
            for (int x : nums) freq[x]++;

            auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
                return a.second > b.second;
            };
            priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);

            for (auto& [val, cnt] : freq) {
                pq.push({val, cnt});
                if ((int)pq.size() > k) pq.pop();
            }

            vector<int> result;
            while (!pq.empty()) { result.push_back(pq.top().first); pq.pop(); }
            return result;
        };

        vector<int> nums = {1,1,1,2,2,3};
        printVecLn(nums, "数组: ");
        cout << "频率: 1→3次, 2→2次, 3→1次\n";
        auto res = topKFrequent(nums, 2);
        cout << "前2高频: "; printVecLn(res);
        cout << "\n";
    }

    // --- LC 973: K个最近点 ---
    {
        cout << "--- LC 973: 最接近原点的K个点 ---\n";
        vector<vector<int>> points = {{3,3},{5,-1},{-2,4}};
        int k = 2;
        cout << "点: ";
        for (auto& p : points)
            cout << "(" << p[0] << "," << p[1]
                 << ")[d²=" << p[0]*p[0]+p[1]*p[1] << "] ";
        cout << "\n";

        // 最大堆按距离
        auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
            return (a.first*a.first+a.second*a.second) < (b.first*b.first+b.second*b.second);
        };
        priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);

        for (auto& p : points) {
            pq.push({p[0], p[1]});
            if ((int)pq.size() > k) pq.pop();
        }

        cout << "最近" << k << "个: ";
        while (!pq.empty()) {
            auto [x, y] = pq.top(); pq.pop();
            cout << "(" << x << "," << y << ") ";
        }
        cout << "\n\n";
    }
}

// ==================== Demo4: LC 295 动态中位数 ====================
void demo4_median() {
    cout << "===== Demo4: LC 295 数据流中位数 =====\n\n";

    class MedianFinder {
        priority_queue<int> maxHeap;
        priority_queue<int, vector<int>, greater<int>> minHeap;
    public:
        void addNum(int num) {
            maxHeap.push(num);
            minHeap.push(maxHeap.top());
            maxHeap.pop();
            if (minHeap.size() > maxHeap.size()) {
                maxHeap.push(minHeap.top());
                minHeap.pop();
            }
        }
        double findMedian() {
            if (maxHeap.size() > minHeap.size())
                return maxHeap.top();
            return (maxHeap.top() + minHeap.top()) / 2.0;
        }
        void printState() {
            // 复制堆来打印
            auto mx = maxHeap, mn = minHeap;
            vector<int> left, right;
            while (!mx.empty()) { left.push_back(mx.top()); mx.pop(); }
            while (!mn.empty()) { right.push_back(mn.top()); mn.pop(); }
            cout << "  maxHeap(左半): ";
            for (int x : left) cout << x << " ";
            cout << "| minHeap(右半): ";
            for (int x : right) cout << x << " ";
        }
    };

    MedianFinder mf;
    vector<int> stream = {5, 2, 8, 3, 7, 1, 6};
    cout << "数据流: "; printVecLn(stream);
    cout << "\n";

    for (int x : stream) {
        mf.addNum(x);
        cout << "加入 " << x << ": ";
        mf.printState();
        cout << "→ 中位数=" << mf.findMedian() << "\n";
    }
    cout << "\n";
}

// ==================== Demo5: 多路归并 ====================
void demo5_kWayMerge() {
    cout << "===== Demo5: 多路归并 =====\n\n";

    // --- LC 23变体: 合并K个有序数组(用数组替代链表演示) ---
    {
        cout << "--- 合并K个有序数组 ---\n";
        vector<vector<int>> lists = {{1,4,7}, {2,5,8}, {3,6,9}};

        cout << "输入:\n";
        for (int i = 0; i < (int)lists.size(); i++) {
            cout << "  L" << i << ": ";
            printVecLn(lists[i]);
        }

        // {值, 列表编号, 下标}
        auto cmp = [](tuple<int,int,int>& a, tuple<int,int,int>& b) {
            return get<0>(a) > get<0>(b);
        };
        priority_queue<tuple<int,int,int>,
                       vector<tuple<int,int,int>>,
                       decltype(cmp)> pq(cmp);

        for (int i = 0; i < (int)lists.size(); i++)
            if (!lists[i].empty())
                pq.push({lists[i][0], i, 0});

        vector<int> result;
        cout << "过程:\n";
        while (!pq.empty()) {
            auto [val, li, idx] = pq.top(); pq.pop();
            result.push_back(val);
            cout << "  pop " << val << "(L" << li << "[" << idx << "])";
            if (idx + 1 < (int)lists[li].size()) {
                pq.push({lists[li][idx + 1], li, idx + 1});
                cout << " → push " << lists[li][idx + 1];
            }
            cout << "\n";
        }
        printVecLn(result, "结果: ");
        cout << "\n";
    }

    // --- LC 378: 有序矩阵第K小 ---
    {
        cout << "--- LC 378: 有序矩阵第K小 ---\n";
        vector<vector<int>> matrix = {
            {1,  5,  9},
            {10, 11, 13},
            {12, 13, 15}
        };
        int k = 5;

        cout << "矩阵:\n";
        for (auto& row : matrix) {
            cout << "  ";
            for (int x : row) cout << x << "\t";
            cout << "\n";
        }

        auto cmp = [](tuple<int,int,int>& a, tuple<int,int,int>& b) {
            return get<0>(a) > get<0>(b);
        };
        priority_queue<tuple<int,int,int>,
                       vector<tuple<int,int,int>>,
                       decltype(cmp)> pq(cmp);

        int n = matrix.size();
        for (int i = 0; i < n; i++)
            pq.push({matrix[i][0], i, 0});

        int result = 0;
        for (int i = 0; i < k; i++) {
            auto [val, row, col] = pq.top(); pq.pop();
            result = val;
            cout << "  第" << i+1 << "小: " << val
                 << " (matrix[" << row << "][" << col << "])\n";
            if (col + 1 < n)
                pq.push({matrix[row][col + 1], row, col + 1});
        }
        cout << "第" << k << "小 = " << result << "\n\n";
    }
}

// ==================== Demo6: 任务调度 ====================
void demo6_taskScheduler() {
    cout << "===== Demo6: 任务调度 =====\n\n";

    // --- LC 621: 任务调度器 ---
    {
        cout << "--- LC 621: 任务调度器 ---\n";
        auto leastInterval = [](vector<char> tasks, int n) -> int {
            vector<int> freq(26, 0);
            for (char c : tasks) freq[c - 'A']++;

            priority_queue<int> pq;
            for (int f : freq) if (f > 0) pq.push(f);

            int time = 0;
            cout << "  调度过程:\n";
            while (!pq.empty()) {
                vector<int> temp;
                int cycle = n + 1;
                cout << "  周期" << time/max(1,cycle)+1 << ": ";
                for (int i = 0; i < cycle; i++) {
                    if (!pq.empty()) {
                        int f = pq.top(); pq.pop();
                        cout << "[任务×" << f << "] ";
                        if (f > 1) temp.push_back(f - 1);
                    } else if (!temp.empty()) {
                        cout << "[idle] ";
                    }
                    time++;
                    if (pq.empty() && temp.empty()) break;
                }
                cout << "\n";
                for (int f : temp) pq.push(f);
            }
            return time;
        };

        vector<char> tasks = {'A','A','A','B','B','B'};
        cout << "任务: A×3, B×3, 冷却n=2\n";
        int result = leastInterval(tasks, 2);
        cout << "最短时间: " << result << "\n\n";
    }

    // --- LC 767: 重组字符串 ---
    {
        cout << "--- LC 767: 重组字符串 ---\n";
        auto reorganizeString = [](string s) -> string {
            vector<int> freq(26, 0);
            for (char c : s) freq[c - 'a']++;

            priority_queue<pair<int,char>> pq;
            for (int i = 0; i < 26; i++)
                if (freq[i] > 0) pq.push({freq[i], 'a' + i});

            string result;
            while (pq.size() > 1) {
                auto [f1, c1] = pq.top(); pq.pop();
                auto [f2, c2] = pq.top(); pq.pop();
                result += c1;
                result += c2;
                if (f1 > 1) pq.push({f1 - 1, c1});
                if (f2 > 1) pq.push({f2 - 1, c2});
            }
            if (!pq.empty()) {
                auto [f, c] = pq.top();
                if (f > 1) return "";
                result += c;
            }
            return result;
        };

        string s1 = "aab";
        cout << "\"" << s1 << "\" → \"" << reorganizeString(s1) << "\"\n";

        string s2 = "aaab";
        string r2 = reorganizeString(s2);
        cout << "\"" << s2 << "\" → " << (r2.empty() ? "不可能 \"\"" : "\"" + r2 + "\"") << "\n";

        string s3 = "vvvlo";
        cout << "\"" << s3 << "\" → \"" << reorganizeString(s3) << "\"\n\n";
    }
}

// ==================== Demo7: 懒删除堆 ====================
void demo7_lazyDeletion() {
    cout << "===== Demo7: 懒删除堆 =====\n\n";

    cout << "--- 滑动窗口最大值（堆+懒删除） ---\n";

    // LC 239 滑动窗口最大值 — 用堆+懒删除实现
    auto maxSlidingWindow = [](vector<int>& nums, int k) -> vector<int> {
        // 最大堆 {值, 下标}
        priority_queue<pair<int,int>> pq;
        vector<int> result;

        for (int i = 0; i < (int)nums.size(); i++) {
            pq.push({nums[i], i});
            // 懒删除: 如果堆顶下标不在窗口内，pop
            while (pq.top().second <= i - k)
                pq.pop();
            if (i >= k - 1)
                result.push_back(pq.top().first);
        }
        return result;
    };

    vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    printVecLn(nums, "数组: ");
    cout << "k=" << k << "\n";

    auto result = maxSlidingWindow(nums, k);

    cout << "窗口 → 最大值:\n";
    for (int i = 0; i <= (int)nums.size() - k; i++) {
        cout << "  [";
        for (int j = i; j < i + k; j++)
            cout << nums[j] << (j < i+k-1 ? ", " : "");
        cout << "] → " << result[i] << "\n";
    }
    printVecLn(result, "结果: ");
    cout << "\n懒删除要点:\n";
    cout << "  ✓ 入堆时带下标 {nums[i], i}\n";
    cout << "  ✓ 取堆顶时检查下标是否在窗口 [i-k+1, i]\n";
    cout << "  ✓ 不在窗口的直接pop，不影响正确性\n\n";
}

// ==================== Demo8: LC 264 丑数II ====================
void demo8_uglyNumber() {
    cout << "===== Demo8: LC 264 丑数II（三路归并） =====\n\n";

    auto nthUglyNumber = [](int n) -> int {
        // 丑数: 质因子只含 2,3,5 的正整数
        // 最小堆 + set去重
        priority_queue<long, vector<long>, greater<long>> pq;
        set<long> visited;
        pq.push(1);
        visited.insert(1);

        long result = 1;
        vector<int> factors = {2, 3, 5};
        for (int i = 0; i < n; i++) {
            result = pq.top(); pq.pop();
            for (int f : factors) {
                long next = result * f;
                if (!visited.count(next)) {
                    visited.insert(next);
                    pq.push(next);
                }
            }
        }
        return (int)result;
    };

    cout << "前15个丑数:\n  ";
    for (int i = 1; i <= 15; i++)
        cout << nthUglyNumber(i) << " ";
    cout << "\n  (应为: 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 16, 18, 20, 24)\n";

    cout << "\n第10个丑数 = " << nthUglyNumber(10) << " (期望12)\n";

    // 也展示DP解法（更优）
    cout << "\n--- DP三指针法（更优 O(n)） ---\n";
    {
        int n = 15;
        vector<int> dp(n);
        dp[0] = 1;
        int p2 = 0, p3 = 0, p5 = 0;
        for (int i = 1; i < n; i++) {
            int v2 = dp[p2] * 2, v3 = dp[p3] * 3, v5 = dp[p5] * 5;
            dp[i] = min({v2, v3, v5});
            if (dp[i] == v2) p2++;
            if (dp[i] == v3) p3++;
            if (dp[i] == v5) p5++;
        }
        cout << "前15个: ";
        printVecLn(dp);
    }
    cout << "\n";
}

// ==================== main ====================
int main() {
    cout << "╔════════════════════════════════════════════════════╗\n";
    cout << "║  专题十四：堆与优先队列 — 完整演示               ║\n";
    cout << "║  作者：大胖超 😜                                   ║\n";
    cout << "╚════════════════════════════════════════════════════╝\n\n";

    demo1_handwrittenHeap();
    demo2_pqUsage();
    demo3_topK();
    demo4_median();
    demo5_kWayMerge();
    demo6_taskScheduler();
    demo7_lazyDeletion();
    demo8_uglyNumber();

    cout << "========================================\n";
    cout << "全部 Demo 运行完毕！\n";
    cout << "覆盖算法: 手写堆(上浮/下沉/建堆), priority_queue,\n";
    cout << "         Top-K, 双堆中位数, 多路归并, 任务调度,\n";
    cout << "         懒删除堆, 三路归并\n";
    cout << "覆盖题目: LC 215, 239, 264, 295, 347, 378, 621,\n";
    cout << "         767, 973\n";
    cout << "共 8 个 Demo, 9 道 LeetCode 题\n";
    return 0;
}
