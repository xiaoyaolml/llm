/**
 * ============================================================
 *   专题七：二分查找（Binary Search）示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_7_binary_search knowlege_details_7_binary_search.cpp
 *   配合 knowlege_details_7_binary_search.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <climits>
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
//  Demo 1: 三种基础模板
//  模板一: 标准二分（找精确值）
//  模板二: lower_bound / upper_bound（找边界）
//  模板三: 红蓝染色法（万能）
// ============================================================

// 模板一：标准二分 — 闭区间 [left, right]
int binarySearch(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size() - 1;
    while (left <= right) {                       // <=
        int mid = left + (right - left) / 2;      // 防溢出
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// 模板二：lower_bound — 左闭右开 [left, right)
// 返回第一个 >= target 的下标
int lowerBound(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size();       // right = n
    while (left < right) {                         // <
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) left = mid + 1;
        else right = mid;                          // 不是 mid-1
    }
    return left;  // left == right
}

// 模板二变体：upper_bound
// 返回第一个 > target 的下标
int upperBound(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size();
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] <= target) left = mid + 1;  // <= 而非 <
        else right = mid;
    }
    return left;
}

// 模板三：红蓝染色法 — 开区间 (blue, red)
// 返回第一个 >= target 的下标（等价 lower_bound）
int binarySearchColor(vector<int>& nums, int target) {
    int blue = -1, red = (int)nums.size();
    while (blue + 1 < red) {
        int mid = blue + (red - blue) / 2;
        if (nums[mid] < target)
            blue = mid;     // 涂蓝：太小
        else
            red = mid;      // 涂红：够大
    }
    return red;
}

void demo_templates() {
    cout << "\n===== Demo 1: 三种基础模板 =====" << endl;

    vector<int> nums = {1, 3, 5, 7, 9, 11, 13, 15};
    printVec(nums, "有序数组");

    // 模板一
    cout << "\n--- 模板一: 标准二分 ---" << endl;
    for (int t : {7, 1, 15, 6}) {
        int idx = binarySearch(nums, t);
        cout << "  查找 " << t << " → " << idx
             << (idx >= 0 ? " (找到)" : " (不存在)") << endl;
    }

    // 模板二
    cout << "\n--- 模板二: lower_bound / upper_bound ---" << endl;
    vector<int> nums2 = {1, 3, 5, 5, 5, 7, 9};
    printVec(nums2, "数组");
    cout << "  lower_bound(5) = " << lowerBound(nums2, 5) << " (第一个≥5)" << endl;   // 2
    cout << "  upper_bound(5) = " << upperBound(nums2, 5) << " (第一个>5)" << endl;    // 5
    cout << "  5 的个数 = " << upperBound(nums2, 5) - lowerBound(nums2, 5) << endl;   // 3
    cout << "  lower_bound(4) = " << lowerBound(nums2, 4) << " (插入位置)" << endl;   // 2
    cout << "  lower_bound(10) = " << lowerBound(nums2, 10) << " (越界=n)" << endl;   // 7

    // 模板三
    cout << "\n--- 模板三: 红蓝染色法 ---" << endl;
    cout << "  binarySearchColor(5) = " << binarySearchColor(nums2, 5)
         << " (等价 lower_bound)" << endl;  // 2
    cout << "  binarySearchColor(4) = " << binarySearchColor(nums2, 4) << endl;  // 2
    cout << "  binarySearchColor(6) = " << binarySearchColor(nums2, 6) << endl;  // 5
}

// ============================================================
//  Demo 2: 有序数组查找
//  LC 704: 二分查找
//  LC 34: 查找首末位置
//  LC 35: 搜索插入位置
// ============================================================

// LC 34: 在排序数组中查找元素的第一个和最后一个位置
vector<int> searchRange(vector<int>& nums, int target) {
    int first = lowerBound(nums, target);
    if (first == (int)nums.size() || nums[first] != target)
        return {-1, -1};
    int last = upperBound(nums, target) - 1;
    return {first, last};
}

// LC 35: 搜索插入位置（就是 lower_bound）
int searchInsert(vector<int>& nums, int target) {
    return lowerBound(nums, target);
}

void demo_sorted_search() {
    cout << "\n===== Demo 2: 有序数组查找 =====" << endl;

    // LC 704
    cout << "\n--- LC 704: 二分查找 ---" << endl;
    vector<int> nums = {-1, 0, 3, 5, 9, 12};
    printVec(nums, "nums");
    cout << "查找 9 → " << binarySearch(nums, 9) << " (期望=4)" << endl;
    cout << "查找 2 → " << binarySearch(nums, 2) << " (期望=-1)" << endl;

    // LC 34
    cout << "\n--- LC 34: 查找首末位置 ---" << endl;
    vector<int> nums2 = {5, 7, 7, 8, 8, 10};
    printVec(nums2, "nums");
    auto r1 = searchRange(nums2, 8);
    printVec(r1, "查找 8");  // [3, 4]
    auto r2 = searchRange(nums2, 6);
    printVec(r2, "查找 6");  // [-1, -1]

    vector<int> nums3 = {1};
    auto r3 = searchRange(nums3, 1);
    printVec(r3, "查找 1 in [1]");  // [0, 0]

    // LC 35
    cout << "\n--- LC 35: 搜索插入位置 ---" << endl;
    vector<int> nums4 = {1, 3, 5, 6};
    printVec(nums4, "nums");
    for (int t : {5, 2, 7, 0}) {
        cout << "  target=" << t << " → 位置 " << searchInsert(nums4, t) << endl;
    }
    // 5→2, 2→1, 7→4, 0→0
}

// ============================================================
//  Demo 3: 旋转排序数组
//  LC 33: 搜索旋转排序数组
//  LC 81: 搜索旋转排序数组 II（有重复）
//  LC 153: 旋转数组中的最小值
// ============================================================

// LC 33: 搜索旋转排序数组
int searchRotated(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;

        if (nums[left] <= nums[mid]) {
            // 左半段 [left..mid] 有序
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;
            else
                left = mid + 1;
        } else {
            // 右半段 [mid..right] 有序
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }
    return -1;
}

// LC 81: 搜索旋转排序数组 II（有重复）
bool searchRotatedII(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return true;

        // 关键：去重
        if (nums[left] == nums[mid]) {
            left++;
            continue;
        }

        if (nums[left] <= nums[mid]) {
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;
            else
                left = mid + 1;
        } else {
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }
    return false;
}

// LC 153: 旋转数组中的最小值
int findMin(vector<int>& nums) {
    int left = 0, right = (int)nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] > nums[right])
            left = mid + 1;    // 最小值在右半
        else
            right = mid;       // 最小值在左半（含 mid）
    }
    return nums[left];
}

void demo_rotated() {
    cout << "\n===== Demo 3: 旋转排序数组 =====" << endl;

    // LC 33
    cout << "\n--- LC 33: 搜索旋转排序数组 ---" << endl;
    vector<int> r1 = {4, 5, 6, 7, 0, 1, 2};
    printVec(r1, "nums");
    cout << "查找 0 → " << searchRotated(r1, 0) << " (期望=4)" << endl;
    cout << "查找 3 → " << searchRotated(r1, 3) << " (期望=-1)" << endl;
    cout << "查找 4 → " << searchRotated(r1, 4) << " (期望=0)" << endl;
    cout << "查找 2 → " << searchRotated(r1, 2) << " (期望=6)" << endl;

    // 未旋转情况
    vector<int> r1b = {1, 2, 3, 4, 5};
    printVec(r1b, "未旋转");
    cout << "查找 3 → " << searchRotated(r1b, 3) << " (期望=2)" << endl;

    // LC 81
    cout << "\n--- LC 81: 旋转数组 II（有重复）---" << endl;
    vector<int> r2 = {2, 5, 6, 0, 0, 1, 2};
    printVec(r2, "nums");
    cout << "查找 0 → " << (searchRotatedII(r2, 0) ? "true" : "false")
         << " (期望=true)" << endl;
    cout << "查找 3 → " << (searchRotatedII(r2, 3) ? "true" : "false")
         << " (期望=false)" << endl;

    // LC 153
    cout << "\n--- LC 153: 旋转数组最小值 ---" << endl;
    vector<int> r3 = {3, 4, 5, 1, 2};
    printVec(r3, "nums");
    cout << "最小值: " << findMin(r3) << " (期望=1)" << endl;

    vector<int> r4 = {4, 5, 6, 7, 0, 1, 2};
    printVec(r4, "nums");
    cout << "最小值: " << findMin(r4) << " (期望=0)" << endl;

    vector<int> r5 = {1, 2, 3};
    printVec(r5, "nums（未旋转）");
    cout << "最小值: " << findMin(r5) << " (期望=1)" << endl;
}

// ============================================================
//  Demo 4: 山脉数组与峰值
//  LC 852: 山脉数组的峰顶索引
//  LC 162: 寻找峰值
// ============================================================

// LC 852: 山脉数组的峰顶索引
int peakIndexInMountainArray(vector<int>& arr) {
    int left = 0, right = (int)arr.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] < arr[mid + 1])
            left = mid + 1;   // 上坡 → 峰在右边
        else
            right = mid;      // 下坡或峰顶 → 收缩
    }
    return left;
}

// LC 162: 寻找峰值
int findPeakElement(vector<int>& nums) {
    int left = 0, right = (int)nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] < nums[mid + 1])
            left = mid + 1;   // 右边更高 → 右边有峰
        else
            right = mid;      // 左边更高或平 → 左边有峰
    }
    return left;
}

void demo_peak() {
    cout << "\n===== Demo 4: 山脉数组与峰值 =====" << endl;

    // LC 852
    cout << "\n--- LC 852: 山脉数组峰顶 ---" << endl;
    vector<int> m1 = {0, 2, 1, 0};
    printVec(m1, "arr");
    cout << "峰顶索引: " << peakIndexInMountainArray(m1) << " (期望=1)" << endl;

    vector<int> m2 = {0, 10, 5, 2};
    printVec(m2, "arr");
    cout << "峰顶索引: " << peakIndexInMountainArray(m2) << " (期望=1)" << endl;

    vector<int> m3 = {3, 5, 3, 2, 0};
    printVec(m3, "arr");
    cout << "峰顶索引: " << peakIndexInMountainArray(m3) << " (期望=1)" << endl;

    vector<int> m4 = {0, 1, 2, 3, 4, 3, 1};
    printVec(m4, "arr");
    cout << "峰顶索引: " << peakIndexInMountainArray(m4) << " (期望=4)" << endl;

    // LC 162
    cout << "\n--- LC 162: 寻找峰值 ---" << endl;
    vector<int> p1 = {1, 2, 3, 1};
    printVec(p1, "nums");
    cout << "峰值索引: " << findPeakElement(p1) << " (期望=2)" << endl;

    vector<int> p2 = {1, 2, 1, 3, 5, 6, 4};
    printVec(p2, "nums");
    int pk = findPeakElement(p2);
    cout << "峰值索引: " << pk << " (期望=1或5, 任一峰即可)" << endl;

    cout << "\n💡 为什么二分找峰值是正确的？" << endl;
    cout << "  nums[-1] = nums[n] = -∞，所以边界也算'下坡'" << endl;
    cout << "  若 nums[mid] < nums[mid+1] → 右边一定有峰" << endl;
    cout << "  否则 → 左边(含mid)一定有峰" << endl;
}

// ============================================================
//  Demo 5: 二分答案 — LC 69, LC 875
//  LC 69: x 的平方根
//  LC 875: 爱吃香蕉的珂珂
// ============================================================

// LC 69: x 的平方根
int mySqrt(int x) {
    if (x <= 1) return x;
    long left = 1, right = (long)x / 2;
    while (left <= right) {
        long mid = left + (right - left) / 2;
        if (mid <= x / mid && (mid + 1) > x / (mid + 1))
            return (int)mid;
        else if (mid > x / mid)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return (int)left;
}

// LC 875: 爱吃香蕉的珂珂
int minEatingSpeed(vector<int>& piles, int h) {
    int left = 1;
    int right = *max_element(piles.begin(), piles.end());

    while (left < right) {
        int mid = left + (right - left) / 2;

        // check: 速度为 mid 时需要多少小时？
        long hours = 0;
        for (int p : piles)
            hours += (p + mid - 1) / mid;  // 向上取整

        if (hours <= h)
            right = mid;       // 吃得完 → 尝试更慢
        else
            left = mid + 1;    // 吃不完 → 必须更快
    }
    return left;
}

void demo_binary_answer() {
    cout << "\n===== Demo 5: 二分答案 =====" << endl;

    // LC 69
    cout << "\n--- LC 69: x 的平方根 ---" << endl;
    for (int x : {0, 1, 4, 8, 16, 100, 2147395599}) {
        cout << "  sqrt(" << x << ") = " << mySqrt(x) << endl;
    }
    // 0→0, 1→1, 4→2, 8→2, 16→4, 100→10, 2147395599→46339

    // LC 875
    cout << "\n--- LC 875: 爱吃香蕉的珂珂 ---" << endl;
    {
        vector<int> piles = {3, 6, 7, 11};
        int h = 8;
        printVec(piles, "piles");
        cout << "h=" << h << ", 最小速度: " << minEatingSpeed(piles, h)
             << " (期望=4)" << endl;
    }
    {
        vector<int> piles = {30, 11, 23, 4, 20};
        int h = 5;
        printVec(piles, "piles");
        cout << "h=" << h << ", 最小速度: " << minEatingSpeed(piles, h)
             << " (期望=30)" << endl;
    }
    {
        vector<int> piles = {30, 11, 23, 4, 20};
        int h = 6;
        printVec(piles, "piles");
        cout << "h=" << h << ", 最小速度: " << minEatingSpeed(piles, h)
             << " (期望=23)" << endl;
    }

    cout << "\n💡 二分答案模板:" << endl;
    cout << "  1. 确定答案范围 [lo, hi]" << endl;
    cout << "  2. 写 check(mid) 判定函数" << endl;
    cout << "  3. check 通过 → right = mid (尝试更优)" << endl;
    cout << "  4. check 不过 → left = mid + 1" << endl;
}

// ============================================================
//  Demo 6: 二分答案 — LC 1011 在 D 天内送达包裹的能力
// ============================================================

int shipWithinDays(vector<int>& weights, int days) {
    int left = *max_element(weights.begin(), weights.end());
    int right = accumulate(weights.begin(), weights.end(), 0);

    while (left < right) {
        int mid = left + (right - left) / 2;

        // check: 载重为 mid 时需要几天？
        int need = 1, curLoad = 0;
        for (int w : weights) {
            if (curLoad + w > mid) {
                need++;
                curLoad = 0;
            }
            curLoad += w;
        }

        if (need <= days)
            right = mid;       // 够了 → 尝试更小载重
        else
            left = mid + 1;    // 不够 → 增大载重
    }
    return left;
}

void demo_ship() {
    cout << "\n===== Demo 6: LC 1011 D 天内送达包裹 =====" << endl;

    {
        vector<int> w = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int d = 5;
        printVec(w, "weights");
        cout << "days=" << d << ", 最小载重: " << shipWithinDays(w, d)
             << " (期望=15)" << endl;
        cout << "  载重15: [1,2,3,4,5][6,7][8][9][10] → 5天 ✓" << endl;
    }
    {
        vector<int> w = {3, 2, 2, 4, 1, 4};
        int d = 3;
        printVec(w, "weights");
        cout << "days=" << d << ", 最小载重: " << shipWithinDays(w, d)
             << " (期望=6)" << endl;
    }
    {
        vector<int> w = {1, 2, 3, 1, 1};
        int d = 4;
        printVec(w, "weights");
        cout << "days=" << d << ", 最小载重: " << shipWithinDays(w, d)
             << " (期望=3)" << endl;
    }
}

// ============================================================
//  Demo 7: 矩阵二分
//  LC 74: 搜索二维矩阵
//  LC 240: 搜索二维矩阵 II
// ============================================================

// LC 74: 搜索二维矩阵（行行严格递增）
bool searchMatrix74(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int left = 0, right = m * n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int val = matrix[mid / n][mid % n];  // 一维 → 二维

        if (val == target) return true;
        else if (val < target) left = mid + 1;
        else right = mid - 1;
    }
    return false;
}

// LC 240: 搜索二维矩阵 II（每行每列有序）
bool searchMatrix240(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int row = 0, col = n - 1;  // 从右上角出发

    while (row < m && col >= 0) {
        if (matrix[row][col] == target) return true;
        else if (matrix[row][col] > target) col--;
        else row++;
    }
    return false;
}

void demo_matrix() {
    cout << "\n===== Demo 7: 矩阵二分 =====" << endl;

    // LC 74
    cout << "\n--- LC 74: 搜索二维矩阵 ---" << endl;
    vector<vector<int>> mat1 = {
        {1,  3,  5,  7},
        {10, 11, 16, 20},
        {23, 30, 34, 60}
    };
    cout << "矩阵:" << endl;
    for (auto& row : mat1) printVec(row, "  ");
    cout << "查找 3 → " << (searchMatrix74(mat1, 3) ? "true" : "false")
         << " (期望=true)" << endl;
    cout << "查找 13 → " << (searchMatrix74(mat1, 13) ? "true" : "false")
         << " (期望=false)" << endl;
    cout << "查找 60 → " << (searchMatrix74(mat1, 60) ? "true" : "false")
         << " (期望=true)" << endl;

    cout << "\n关键: 一维下标 mid → 二维 (mid/n, mid%n)" << endl;
    cout << "  例: m=3, n=4, mid=5 → (5/4, 5%4) = (1, 1) → mat[1][1]=11" << endl;

    // LC 240
    cout << "\n--- LC 240: 搜索二维矩阵 II ---" << endl;
    vector<vector<int>> mat2 = {
        {1,  4,  7,  11, 15},
        {2,  5,  8,  12, 19},
        {3,  6,  9,  16, 22},
        {10, 13, 14, 17, 24},
        {18, 21, 23, 26, 30}
    };
    cout << "5×5 矩阵（每行每列有序）" << endl;
    cout << "查找 5 → " << (searchMatrix240(mat2, 5) ? "true" : "false")
         << " (期望=true)" << endl;
    cout << "查找 20 → " << (searchMatrix240(mat2, 20) ? "true" : "false")
         << " (期望=false)" << endl;

    cout << "\n右上角出发法:" << endl;
    cout << "  当前值 > target → 左移（排除当前列）" << endl;
    cout << "  当前值 < target → 下移（排除当前行）" << endl;
    cout << "  O(m + n) 时间" << endl;
}

// ============================================================
//  Demo 8: STL 二分函数 & 综合对比
// ============================================================

void demo_stl_and_summary() {
    cout << "\n===== Demo 8: STL 二分 & 综合 =====" << endl;

    vector<int> nums = {1, 2, 4, 4, 4, 7, 9};
    printVec(nums, "数组");

    cout << "\n--- STL 二分函数 ---" << endl;

    // lower_bound
    auto it1 = lower_bound(nums.begin(), nums.end(), 4);
    cout << "lower_bound(4) → 下标 " << (it1 - nums.begin())
         << " (值=" << *it1 << ")" << endl;  // 下标2

    // upper_bound
    auto it2 = upper_bound(nums.begin(), nums.end(), 4);
    cout << "upper_bound(4) → 下标 " << (it2 - nums.begin())
         << " (值=" << *it2 << ")" << endl;  // 下标5

    // 出现次数
    int cnt = (int)(upper_bound(nums.begin(), nums.end(), 4) -
                    lower_bound(nums.begin(), nums.end(), 4));
    cout << "4 出现次数: " << cnt << endl;  // 3

    // binary_search
    cout << "binary_search(4) → "
         << (binary_search(nums.begin(), nums.end(), 4) ? "true" : "false") << endl;
    cout << "binary_search(5) → "
         << (binary_search(nums.begin(), nums.end(), 5) ? "true" : "false") << endl;

    // equal_range
    auto [lo, hi] = equal_range(nums.begin(), nums.end(), 4);
    cout << "equal_range(4) → [" << (lo - nums.begin())
         << ", " << (hi - nums.begin()) << ")" << endl;  // [2, 5)

    // 不存在的值
    auto it3 = lower_bound(nums.begin(), nums.end(), 5);
    cout << "lower_bound(5) → 下标 " << (it3 - nums.begin())
         << " (第一个≥5的值=" << *it3 << ")" << endl;  // 下标5, 值7

    auto it4 = lower_bound(nums.begin(), nums.end(), 10);
    cout << "lower_bound(10) → 下标 " << (it4 - nums.begin())
         << " (越界=end)" << endl;  // 下标7=n

    // 综合对比
    cout << "\n--- 手写 vs STL 对比 ---" << endl;
    for (int t : {1, 4, 5, 9, 10}) {
        int myLB = lowerBound(nums, t);
        int stlLB = (int)(lower_bound(nums.begin(), nums.end(), t) - nums.begin());
        int myUB = upperBound(nums, t);
        int stlUB = (int)(upper_bound(nums.begin(), nums.end(), t) - nums.begin());
        cout << "  target=" << t
             << "  LB: 手写=" << myLB << " STL=" << stlLB
             << "  UB: 手写=" << myUB << " STL=" << stlUB
             << (myLB == stlLB && myUB == stlUB ? " ✓" : " ✗") << endl;
    }

    // 总结
    cout << "\n--- 题型速查 ---" << endl;
    cout << "  有序查找: 模板一/STL (LC 704, 34, 35)" << endl;
    cout << "  旋转数组: 模板一变体 (LC 33, 81, 153)" << endl;
    cout << "  峰值/山脉: 邻居比较 (LC 162, 852)" << endl;
    cout << "  二分答案: 模板二 + check() (LC 69, 875, 1011)" << endl;
    cout << "  矩阵搜索: 一维映射/右上角 (LC 74, 240)" << endl;
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题七：二分查找 Binary Search 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_templates();         // Demo 1: 三种模板
    demo_sorted_search();     // Demo 2: 有序数组查找
    demo_rotated();           // Demo 3: 旋转排序数组
    demo_peak();              // Demo 4: 山脉 & 峰值
    demo_binary_answer();     // Demo 5: 二分答案 (69, 875)
    demo_ship();              // Demo 6: 二分答案 (1011)
    demo_matrix();            // Demo 7: 矩阵二分
    demo_stl_and_summary();   // Demo 8: STL & 总结

    cout << "\n============================================" << endl;
    cout << "  所有二分查找 Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 33, 34, 35, 69, 74, 81," << endl;
    cout << "  153, 162, 240, 704, 852, 875, 1011" << endl;
    cout << "============================================" << endl;

    return 0;
}
