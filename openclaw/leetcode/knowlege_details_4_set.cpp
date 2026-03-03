/**
 * ============================================================
 *   专题四（下）：集合（Set）示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_4_set knowlege_details_4_set.cpp
 *   配合 knowlege_details_4_set.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include <bitset>
#include <algorithm>
#include <iterator>
#include <numeric>
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

template<typename T>
void printSet(const T& s, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    cout << "{";
    bool first = true;
    for (auto& x : s) {
        if (!first) cout << ", ";
        cout << x;
        first = false;
    }
    cout << "}" << endl;
}

// ============================================================
//  Demo 1: unordered_set 基础用法
//  演示增删查、遍历、去重
// ============================================================
void demo_unordered_set_basics() {
    cout << "\n===== Demo 1: unordered_set 基础 =====" << endl;

    // ---------- 创建与初始化 ----------
    unordered_set<int> s = {5, 3, 1, 4, 2};
    printSet(s, "初始（顺序不确定）");

    // 从数组构造（自动去重）
    vector<int> v = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    unordered_set<int> s2(v.begin(), v.end());
    printVec(v, "原数组");
    printSet(s2, "去重后");

    // ---------- 插入 ----------
    cout << "\n--- 插入操作 ---" << endl;
    s.insert(10);
    s.insert(20);
    auto [it, ok] = s.insert(5);  // 5 已存在
    cout << "insert(5): " << (ok ? "成功" : "已存在，跳过") << endl;
    s.emplace(30);
    printSet(s, "插入 10,20,30 后");

    // ---------- 查找 ----------
    cout << "\n--- 查找操作 ---" << endl;
    cout << "count(10): " << s.count(10) << " (存在)" << endl;
    cout << "count(99): " << s.count(99) << " (不存在)" << endl;

    auto it2 = s.find(20);
    if (it2 != s.end())
        cout << "find(20): 找到 " << *it2 << endl;

    // ---------- 删除 ----------
    cout << "\n--- 删除操作 ---" << endl;
    s.erase(10);
    s.erase(20);
    cout << "erase 10, 20 → size: " << s.size() << endl;

    // ---------- 其他 API ----------
    cout << "\n--- 其他 API ---" << endl;
    cout << "size:         " << s.size() << endl;
    cout << "empty:        " << (s.empty() ? "true" : "false") << endl;
    cout << "bucket_count: " << s.bucket_count() << endl;
    cout << "load_factor:  " << s.load_factor() << endl;
}

// ============================================================
//  Demo 2: set（有序集合）
//  演示有序遍历、lower_bound、upper_bound、前驱后继
// ============================================================
void demo_ordered_set() {
    cout << "\n===== Demo 2: set 有序集合 =====" << endl;

    set<int> s = {50, 20, 80, 10, 30, 60, 90, 40, 70};
    printSet(s, "自动升序排列");
    // {10, 20, 30, 40, 50, 60, 70, 80, 90}

    // ---------- 最小/最大 ----------
    cout << "\n最小元素: " << *s.begin() << endl;   // 10
    cout << "最大元素: " << *s.rbegin() << endl;    // 90

    // ---------- lower_bound / upper_bound ----------
    cout << "\n--- lower_bound / upper_bound ---" << endl;
    auto lo = s.lower_bound(35);  // >= 35 的第一个
    cout << "lower_bound(35): " << *lo << endl;  // 40

    auto hi = s.upper_bound(60);  // > 60 的第一个
    cout << "upper_bound(60): " << *hi << endl;  // 70

    // 范围查询: [30, 70]
    cout << "范围 [30, 70]: ";
    for (auto it = s.lower_bound(30); it != s.upper_bound(70); ++it)
        cout << *it << " ";
    cout << endl;  // 30 40 50 60 70

    // ---------- prev / next ----------
    cout << "\n--- prev / next ---" << endl;
    auto it = s.find(50);
    cout << "50 的前驱: " << *prev(it) << endl;   // 40
    cout << "50 的后继: " << *next(it) << endl;    // 60

    // ---------- 降序 set ----------
    cout << "\n--- 降序 set ---" << endl;
    set<int, greater<int>> desc = {1, 5, 3, 7, 2};
    printSet(desc, "降序 set");
    // {7, 5, 3, 2, 1}
}

// ============================================================
//  Demo 3: multiset（允许重复）
//  演示重复元素、count、精确删除单个
// ============================================================
void demo_multiset() {
    cout << "\n===== Demo 3: multiset =====" << endl;

    multiset<int> ms;
    ms.insert(3);
    ms.insert(1);
    ms.insert(3);
    ms.insert(2);
    ms.insert(3);
    ms.insert(2);

    printSet(ms, "multiset");
    // {1, 2, 2, 3, 3, 3}

    cout << "count(3): " << ms.count(3) << endl;  // 3
    cout << "count(2): " << ms.count(2) << endl;  // 2

    // ⚠️ erase(val) 删除所有等于 val 的！
    cout << "\n--- erase 的坑 ---" << endl;
    multiset<int> ms2 = ms;  // 拷贝一份

    ms2.erase(3);  // 删掉所有的 3！
    printSet(ms2, "erase(3) 后");  // {1, 2, 2}

    // ✅ 只删一个：用迭代器
    cout << "\n--- 只删一个 3 ---" << endl;
    auto it = ms.find(3);
    if (it != ms.end()) ms.erase(it);  // 只删一个
    printSet(ms, "erase(find(3)) 后");  // {1, 2, 2, 3, 3}

    // equal_range
    cout << "\n--- equal_range ---" << endl;
    auto [lo, hi] = ms.equal_range(2);
    cout << "2 的范围: ";
    for (auto it2 = lo; it2 != hi; ++it2) cout << *it2 << " ";
    cout << "(个数: " << distance(lo, hi) << ")" << endl;
}

// ============================================================
//  Demo 4: bitset（位集合）
//  演示位操作和集合运算
// ============================================================
void demo_bitset() {
    cout << "\n===== Demo 4: bitset =====" << endl;

    bitset<16> bs;
    bs.set(1);   // 第 1 位 = 1
    bs.set(3);   // {1, 3}
    bs.set(5);   // {1, 3, 5}
    bs.set(7);   // {1, 3, 5, 7}

    cout << "bs = " << bs << endl;
    cout << "count (1的个数): " << bs.count() << endl;   // 4
    cout << "test(3): " << bs.test(3) << endl;           // 1
    cout << "test(4): " << bs.test(4) << endl;           // 0

    bs.flip(5);   // 取反第 5 位 → {1, 3, 7}
    cout << "flip(5): " << bs << ", count: " << bs.count() << endl;

    // 集合运算
    cout << "\n--- bitset 集合运算 ---" << endl;
    bitset<16> a, b;
    a.set(1); a.set(3); a.set(5); a.set(7);   // a = {1,3,5,7}
    b.set(3); b.set(5); b.set(9); b.set(11);  // b = {3,5,9,11}

    cout << "a      = " << a << " {1,3,5,7}" << endl;
    cout << "b      = " << b << " {3,5,9,11}" << endl;
    cout << "a & b  = " << (a & b) << " 交集 {3,5}" << endl;
    cout << "a | b  = " << (a | b) << " 并集 {1,3,5,7,9,11}" << endl;
    cout << "a ^ b  = " << (a ^ b) << " 对称差 {1,7,9,11}" << endl;

    cout << "\n交集元素个数: " << (a & b).count() << endl;
    cout << "并集元素个数: " << (a | b).count() << endl;
}

// ============================================================
//  Demo 5: 去重与判重
//  LC 217: 存在重复元素
//  LC 128: 最长连续序列
//  LC 202: 快乐数
// ============================================================

// LC 217
bool containsDuplicate(vector<int>& nums) {
    unordered_set<int> seen;
    for (int num : nums) {
        if (seen.count(num)) return true;
        seen.insert(num);
    }
    return false;
}

// LC 128
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> numSet(nums.begin(), nums.end());
    int maxLen = 0;

    for (int num : numSet) {
        if (!numSet.count(num - 1)) {  // 只从起点开始
            int current = num, len = 1;
            while (numSet.count(current + 1)) {
                current++;
                len++;
            }
            maxLen = max(maxLen, len);
        }
    }
    return maxLen;
}

// LC 202
bool isHappy(int n) {
    unordered_set<int> seen;
    while (n != 1) {
        if (seen.count(n)) return false;
        seen.insert(n);
        int sum = 0;
        while (n > 0) {
            int d = n % 10;
            sum += d * d;
            n /= 10;
        }
        n = sum;
    }
    return true;
}

void demo_dedup_detect() {
    cout << "\n===== Demo 5: 去重与判重 =====" << endl;

    // LC 217
    cout << "\n--- LC 217: 存在重复元素 ---" << endl;
    vector<int> n1 = {1, 2, 3, 1};
    vector<int> n2 = {1, 2, 3, 4};
    printVec(n1, "nums");
    cout << "存在重复: " << (containsDuplicate(n1) ? "true" : "false") << endl;  // true
    printVec(n2, "nums");
    cout << "存在重复: " << (containsDuplicate(n2) ? "true" : "false") << endl;  // false

    // LC 128
    cout << "\n--- LC 128: 最长连续序列 ---" << endl;
    vector<int> n3 = {100, 4, 200, 1, 3, 2};
    printVec(n3, "nums");
    cout << "最长连续序列长度: " << longestConsecutive(n3) << endl;  // 4 (1,2,3,4)

    vector<int> n4 = {0, 3, 7, 2, 5, 8, 4, 6, 0, 1};
    printVec(n4, "nums");
    cout << "最长连续序列长度: " << longestConsecutive(n4) << endl;  // 9 (0~8)

    // LC 202
    cout << "\n--- LC 202: 快乐数 ---" << endl;
    cout << "19 是快乐数? " << (isHappy(19) ? "true" : "false") << endl;  // true
    cout << "  19 → 1²+9²=82 → 8²+2²=68 → 6²+8²=100 → 1²+0²+0²=1 ✅" << endl;
    cout << "2 是快乐数?  " << (isHappy(2) ? "true" : "false") << endl;   // false
}

// ============================================================
//  Demo 6: 集合运算
//  LC 349: 两个数组的交集
//  + STL set_intersection / set_union / set_difference
// ============================================================

// LC 349
vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
    unordered_set<int> s1(nums1.begin(), nums1.end());
    vector<int> result;
    unordered_set<int> added;  // 防重复
    for (int x : nums2) {
        if (s1.count(x) && !added.count(x)) {
            result.push_back(x);
            added.insert(x);
        }
    }
    return result;
}

void demo_set_operations() {
    cout << "\n===== Demo 6: 集合运算 =====" << endl;

    // LC 349
    cout << "\n--- LC 349: 两个数组的交集 ---" << endl;
    vector<int> a1 = {1, 2, 2, 1}, b1 = {2, 2};
    printVec(a1, "nums1");
    printVec(b1, "nums2");
    auto r1 = intersection(a1, b1);
    printVec(r1, "交集");  // [2]

    vector<int> a2 = {4, 9, 5}, b2 = {9, 4, 9, 8, 4};
    printVec(a2, "nums1");
    printVec(b2, "nums2");
    auto r2 = intersection(a2, b2);
    printVec(r2, "交集");  // [9, 4]

    // STL 集合运算（要求有序）
    cout << "\n--- STL 集合运算 ---" << endl;
    set<int> s1 = {1, 2, 3, 4, 5};
    set<int> s2 = {3, 4, 5, 6, 7};
    printSet(s1, "s1");
    printSet(s2, "s2");

    // 交集
    vector<int> inter;
    set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                     back_inserter(inter));
    printVec(inter, "交集 s1∩s2");  // [3, 4, 5]

    // 并集
    vector<int> uni;
    set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
              back_inserter(uni));
    printVec(uni, "并集 s1∪s2");  // [1, 2, 3, 4, 5, 6, 7]

    // 差集
    vector<int> diff;
    set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
                   back_inserter(diff));
    printVec(diff, "差集 s1-s2");  // [1, 2]

    // 对称差集
    vector<int> symDiff;
    set_symmetric_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
                              back_inserter(symDiff));
    printVec(symDiff, "对称差 s1△s2");  // [1, 2, 6, 7]
}

// ============================================================
//  Demo 7: 滑动窗口 + 集合
//  LC 3: 无重复字符的最长子串
//  LC 219: 存在重复元素 II
// ============================================================

// LC 3
int lengthOfLongestSubstring(string s) {
    unordered_set<char> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < (int)s.size(); right++) {
        while (window.count(s[right])) {
            window.erase(s[left]);
            left++;
        }
        window.insert(s[right]);
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

// LC 219
bool containsNearbyDuplicate(vector<int>& nums, int k) {
    unordered_set<int> window;
    for (int i = 0; i < (int)nums.size(); i++) {
        if (window.count(nums[i])) return true;
        window.insert(nums[i]);
        if ((int)window.size() > k)
            window.erase(nums[i - k]);
    }
    return false;
}

void demo_sliding_window_set() {
    cout << "\n===== Demo 7: 滑动窗口 + 集合 =====" << endl;

    // LC 3
    cout << "\n--- LC 3: 无重复字符的最长子串 ---" << endl;
    string s1 = "abcabcbb";
    cout << "\"" << s1 << "\" → " << lengthOfLongestSubstring(s1) << endl;  // 3 ("abc")

    string s2 = "bbbbb";
    cout << "\"" << s2 << "\" → " << lengthOfLongestSubstring(s2) << endl;  // 1 ("b")

    string s3 = "pwwkew";
    cout << "\"" << s3 << "\" → " << lengthOfLongestSubstring(s3) << endl;  // 3 ("wke")

    // LC 219
    cout << "\n--- LC 219: 存在重复元素 II ---" << endl;
    vector<int> n1 = {1, 2, 3, 1};
    int k1 = 3;
    printVec(n1, "nums");
    cout << "k = " << k1 << " → " << (containsNearbyDuplicate(n1, k1) ? "true" : "false") << endl;  // true

    vector<int> n2 = {1, 0, 1, 1};
    int k2 = 1;
    printVec(n2, "nums");
    cout << "k = " << k2 << " → " << (containsNearbyDuplicate(n2, k2) ? "true" : "false") << endl;  // true

    vector<int> n3 = {1, 2, 3, 1, 2, 3};
    int k3 = 2;
    printVec(n3, "nums");
    cout << "k = " << k3 << " → " << (containsNearbyDuplicate(n3, k3) ? "true" : "false") << endl;  // false
}

// ============================================================
//  Demo 8: set + lower_bound（有序操作）
//  LC 220: 存在重复元素 III
//  + 有效数独、手写 HashSet
// ============================================================

// LC 220: 存在重复元素 III
bool containsNearbyAlmostDuplicate(vector<int>& nums, int indexDiff, int valueDiff) {
    set<long> window;

    for (int i = 0; i < (int)nums.size(); i++) {
        // 查找 >= nums[i] - valueDiff 的最小元素
        auto it = window.lower_bound((long)nums[i] - valueDiff);

        if (it != window.end() && *it <= (long)nums[i] + valueDiff)
            return true;

        window.insert(nums[i]);

        if ((int)window.size() > indexDiff)
            window.erase(nums[i - indexDiff]);
    }
    return false;
}

// LC 36: 有效的数独
bool isValidSudoku(vector<vector<char>>& board) {
    // 每行、每列、每个 3x3 宫格用 set 判重
    unordered_set<char> rows[9], cols[9], boxes[9];

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            char c = board[i][j];
            if (c == '.') continue;

            int boxIdx = (i / 3) * 3 + j / 3;

            if (rows[i].count(c) || cols[j].count(c) || boxes[boxIdx].count(c))
                return false;

            rows[i].insert(c);
            cols[j].insert(c);
            boxes[boxIdx].insert(c);
        }
    }
    return true;
}

// 手写 HashSet（简化版，LC 705）
class MyHashSet {
    static const int SIZE = 1009;  // 质数大小
    vector<vector<int>> buckets;

public:
    MyHashSet() : buckets(SIZE) {}

    int hash(int key) { return key % SIZE; }

    void add(int key) {
        int h = hash(key);
        for (int x : buckets[h])
            if (x == key) return;  // 已存在
        buckets[h].push_back(key);
    }

    void remove(int key) {
        int h = hash(key);
        auto& bucket = buckets[h];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (*it == key) { bucket.erase(it); return; }
        }
    }

    bool contains(int key) {
        int h = hash(key);
        for (int x : buckets[h])
            if (x == key) return true;
        return false;
    }
};

void demo_advanced_set() {
    cout << "\n===== Demo 8: set 高级应用 =====" << endl;

    // LC 220
    cout << "\n--- LC 220: 存在重复元素 III ---" << endl;
    vector<int> n1 = {1, 2, 3, 1};
    cout << "nums = [1,2,3,1], indexDiff=3, valueDiff=0" << endl;
    cout << "结果: "
         << (containsNearbyAlmostDuplicate(n1, 3, 0) ? "true" : "false") << endl;  // true

    vector<int> n2 = {1, 5, 9, 1, 5, 9};
    cout << "nums = [1,5,9,1,5,9], indexDiff=2, valueDiff=3" << endl;
    cout << "结果: "
         << (containsNearbyAlmostDuplicate(n2, 2, 3) ? "true" : "false") << endl;  // false

    cout << "\n原理: set.lower_bound(nums[i]-t) 找到 >= nums[i]-t 的最小值" << endl;
    cout << "       如果该值 <= nums[i]+t，说明差值在 t 以内" << endl;

    // LC 36: 有效的数独
    cout << "\n--- LC 36: 有效的数独 ---" << endl;
    vector<vector<char>> board = {
        {'5','3','.','.','7','.','.','.','.'},
        {'6','.','.','1','9','5','.','.','.'},
        {'.','9','8','.','.','.','.','6','.'},
        {'8','.','.','.','6','.','.','.','3'},
        {'4','.','.','8','.','3','.','.','1'},
        {'7','.','.','.','2','.','.','.','6'},
        {'.','6','.','.','.','.','2','8','.'},
        {'.','.','.','4','1','9','.','.','5'},
        {'.','.','.','.','8','.','.','7','9'}
    };
    cout << "数独有效: " << (isValidSudoku(board) ? "true" : "false") << endl;  // true

    // 手写 HashSet (LC 705)
    cout << "\n--- LC 705: 手写 HashSet ---" << endl;
    MyHashSet hs;
    hs.add(1);
    hs.add(2);
    cout << "add(1), add(2)" << endl;
    cout << "contains(1): " << (hs.contains(1) ? "true" : "false") << endl;  // true
    cout << "contains(3): " << (hs.contains(3) ? "true" : "false") << endl;  // false
    hs.add(2);
    cout << "add(2) (重复)" << endl;
    cout << "contains(2): " << (hs.contains(2) ? "true" : "false") << endl;  // true
    hs.remove(2);
    cout << "remove(2)" << endl;
    cout << "contains(2): " << (hs.contains(2) ? "true" : "false") << endl;  // false
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题四（下）：集合 Set 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_unordered_set_basics();   // Demo 1: unordered_set 基础
    demo_ordered_set();            // Demo 2: set 有序集合
    demo_multiset();               // Demo 3: multiset
    demo_bitset();                 // Demo 4: bitset
    demo_dedup_detect();           // Demo 5: 去重与判重
    demo_set_operations();         // Demo 6: 集合运算
    demo_sliding_window_set();     // Demo 7: 滑动窗口 + 集合
    demo_advanced_set();           // Demo 8: 高级应用

    cout << "\n============================================" << endl;
    cout << "  所有 Set Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 3, 36, 128, 202, 217," << endl;
    cout << "  219, 220, 349, 705" << endl;
    cout << "============================================" << endl;

    return 0;
}
