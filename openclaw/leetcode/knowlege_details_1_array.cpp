/**
 * ============================================================================
 *  专题一：数组（Array）— 完整示例代码
 *  配合 knowlege_details_1_array.md 食用
 *  编译: g++ -std=c++17 -o knowlege_details_1_array knowlege_details_1_array.cpp
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <cassert>
using namespace std;

// ============================================================================
//  第 1 章：C++ 原生数组基础
// ============================================================================

void demo_raw_array() {
    cout << "\n========== 1. 原生数组基础 ==========\n";

    // --- 1.1 声明与初始化 ---
    int arr1[5] = {1, 2, 3, 4, 5};       // 完整初始化
    int arr2[5] = {1, 2};                 // 部分初始化，其余为 0
    int arr3[5] = {};                     // 全部为 0
    int arr4[] = {10, 20, 30};            // 编译器推导大小为 3

    // 打印 arr1
    cout << "arr1: ";
    for (int i = 0; i < 5; i++) {
        cout << arr1[i] << " ";
    }
    cout << endl;

    // 打印 arr2，验证部分初始化
    cout << "arr2 (部分初始化): ";
    for (int i = 0; i < 5; i++) {
        cout << arr2[i] << " ";  // 输出: 1 2 0 0 0
    }
    cout << endl;

    // --- 1.2 获取数组大小 ---
    // 方法 1: sizeof（仅在数组未退化为指针时有效）
    int size_method1 = sizeof(arr1) / sizeof(arr1[0]);
    cout << "arr1 大小 (sizeof): " << size_method1 << endl;

    // 方法 2: std::size (C++17)
    cout << "arr4 大小 (std::size): " << std::size(arr4) << endl;  // 3

    // --- 1.3 二维数组 ---
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    cout << "matrix[1][2] = " << matrix[1][2] << endl;  // 6

    // 二维数组的内存是行优先连续存储的
    // 可以通过指针验证：
    int* ptr = &matrix[0][0];
    cout << "二维数组连续内存验证: ";
    for (int i = 0; i < 6; i++) {
        cout << ptr[i] << " ";  // 输出: 1 2 3 4 5 6
    }
    cout << endl;
}

// ============================================================================
//  第 2 章：STL vector 动态数组
// ============================================================================

void demo_vector_basics() {
    cout << "\n========== 2. vector 基础操作 ==========\n";

    // --- 2.1 创建 ---
    vector<int> v1;                     // 空
    vector<int> v2(5, 0);              // 5 个 0
    vector<int> v3 = {3, 1, 4, 1, 5};  // 初始化列表
    vector<int> v4(v3.begin(), v3.begin() + 3);  // 取 v3 前 3 个

    cout << "v3: ";
    for (int x : v3) cout << x << " ";  // 3 1 4 1 5
    cout << endl;

    cout << "v4 (v3 前3个): ";
    for (int x : v4) cout << x << " ";  // 3 1 4
    cout << endl;

    // --- 2.2 增删改查 ---
    vector<int> v = {10, 20, 30};

    // 增
    v.push_back(40);          // {10, 20, 30, 40}
    v.emplace_back(50);       // {10, 20, 30, 40, 50}    比 push_back 少一次拷贝
    v.insert(v.begin() + 2, 25);  // 在下标 2 处插入 25 → {10, 20, 25, 30, 40, 50}

    cout << "增加后: ";
    for (int x : v) cout << x << " ";
    cout << endl;

    // 删
    v.pop_back();                   // 删尾部 → {10, 20, 25, 30, 40}
    v.erase(v.begin() + 2);        // 删下标 2 → {10, 20, 30, 40}

    cout << "删除后: ";
    for (int x : v) cout << x << " ";
    cout << endl;

    // 改
    v[0] = 100;                    // {100, 20, 30, 40}
    v.at(1) = 200;                 // {100, 200, 30, 40}  带越界检查

    // 查
    cout << "front=" << v.front() << " back=" << v.back() << endl;
    cout << "size=" << v.size() << " empty=" << v.empty() << endl;
}

void demo_vector_memory() {
    cout << "\n========== 3. vector 内存机制 ==========\n";

    vector<int> v;
    cout << "初始: size=" << v.size() << " capacity=" << v.capacity() << endl;

    // 逐个添加，观察 capacity 变化
    for (int i = 1; i <= 10; i++) {
        v.push_back(i);
        cout << "push_back(" << i << "): "
             << "size=" << v.size()
             << " capacity=" << v.capacity() << endl;
    }
    // 可以看到 capacity 会在不够时翻倍增长（1→2→4→8→16...）

    // --- reserve：预分配内存 ---
    vector<int> v2;
    v2.reserve(100);  // 预分配 100 个元素的空间
    cout << "\nreserve(100)后: size=" << v2.size()
         << " capacity=" << v2.capacity() << endl;
    // size 仍为 0，但 capacity=100，后续 push_back 不会频繁扩容

    // --- shrink_to_fit：释放多余内存 ---
    v.shrink_to_fit();
    cout << "shrink_to_fit后: size=" << v.size()
         << " capacity=" << v.capacity() << endl;
}

// ============================================================================
//  第 3 章：std::array 静态数组
// ============================================================================

void demo_std_array() {
    cout << "\n========== 4. std::array ==========\n";

    array<int, 5> arr = {1, 2, 3, 4, 5};

    // 与 vector 类似的接口
    cout << "size=" << arr.size() << endl;     // 编译期常量 5
    cout << "front=" << arr.front() << endl;
    cout << "back=" << arr.back() << endl;

    // fill: 全部填充
    arr.fill(42);
    cout << "fill(42): ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    // 可以直接比较和赋值（原生数组不行！）
    array<int, 3> a1 = {1, 2, 3};
    array<int, 3> a2 = {1, 2, 3};
    cout << "a1 == a2: " << (a1 == a2 ? "true" : "false") << endl;  // true

    array<int, 3> a3 = a1;  // 直接赋值拷贝
    cout << "a3 (拷贝自 a1): ";
    for (int x : a3) cout << x << " ";
    cout << endl;
}

// ============================================================================
//  第 4 章：遍历方式大全
// ============================================================================

void demo_traversal() {
    cout << "\n========== 5. 数组遍历方式 ==========\n";

    vector<int> v = {10, 20, 30, 40, 50};

    // 方式 1：下标遍历
    cout << "下标遍历: ";
    for (int i = 0; i < (int)v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << endl;

    // 方式 2：范围 for（值拷贝）
    cout << "范围 for: ";
    for (int x : v) {
        cout << x << " ";
    }
    cout << endl;

    // 方式 3：范围 for（引用，可修改）
    for (int& x : v) x *= 2;  // 全部翻倍
    cout << "翻倍后: ";
    for (const int& x : v) cout << x << " ";  // const 引用，只读
    cout << endl;

    // 方式 4：迭代器
    cout << "迭代器: ";
    for (auto it = v.begin(); it != v.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    // 方式 5：反向迭代
    cout << "反向: ";
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
}

// ============================================================================
//  第 5 章：STL 算法配合 vector
// ============================================================================

void demo_stl_algorithms() {
    cout << "\n========== 6. STL 算法 ==========\n";

    vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

    // --- 排序 ---
    sort(v.begin(), v.end());
    cout << "升序排序: ";
    for (int x : v) cout << x << " ";
    cout << endl;

    sort(v.begin(), v.end(), greater<int>());
    cout << "降序排序: ";
    for (int x : v) cout << x << " ";
    cout << endl;

    // --- 查找 ---
    sort(v.begin(), v.end());  // 先排序
    bool found = binary_search(v.begin(), v.end(), 5);
    cout << "二分查找 5: " << (found ? "找到" : "没找到") << endl;

    auto lb = lower_bound(v.begin(), v.end(), 5);
    cout << "lower_bound(5) 位置: " << (lb - v.begin()) << endl;  // 第一个 >= 5

    // --- 最值 ---
    auto maxIt = max_element(v.begin(), v.end());
    auto minIt = min_element(v.begin(), v.end());
    cout << "最大值=" << *maxIt << " 最小值=" << *minIt << endl;

    // --- 求和 ---
    int sum = accumulate(v.begin(), v.end(), 0);
    cout << "总和=" << sum << endl;

    // --- 去重（前提：已排序）---
    vector<int> v2 = {1, 1, 2, 2, 3, 3, 4};
    auto last = unique(v2.begin(), v2.end());
    v2.erase(last, v2.end());
    cout << "去重后: ";
    for (int x : v2) cout << x << " ";  // 1 2 3 4
    cout << endl;

    // --- 翻转 ---
    reverse(v2.begin(), v2.end());
    cout << "翻转后: ";
    for (int x : v2) cout << x << " ";
    cout << endl;

    // --- 计数 ---
    vector<int> v3 = {1, 2, 3, 1, 2, 1};
    cout << "1 出现次数: " << count(v3.begin(), v3.end(), 1) << endl;  // 3
    cout << ">2 的个数: " << count_if(v3.begin(), v3.end(),
                                       [](int x) { return x > 2; }) << endl;  // 1

    // --- iota: 填充连续序列 ---
    vector<int> seq(5);
    iota(seq.begin(), seq.end(), 1);  // 1, 2, 3, 4, 5
    cout << "iota 序列: ";
    for (int x : seq) cout << x << " ";
    cout << endl;
}

// ============================================================================
//  第 6 章：双指针技巧
// ============================================================================

// LC 26: 删除有序数组中的重复项
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;
    // slow: 指向最后一个不重复元素的位置
    // fast: 遍历数组的指针
    for (int fast = 1; fast < (int)nums.size(); fast++) {
        if (nums[fast] != nums[slow]) {
            // 发现新元素，放到 slow 的下一个位置
            nums[++slow] = nums[fast];
        }
        // 如果 nums[fast] == nums[slow]，跳过（fast 继续前进）
    }
    return slow + 1;  // 新数组的长度
}

// LC 283: 移动零
void moveZeroes(vector<int>& nums) {
    int slow = 0;  // slow 指向下一个非零元素该放的位置
    for (int fast = 0; fast < (int)nums.size(); fast++) {
        if (nums[fast] != 0) {
            swap(nums[slow++], nums[fast]);
            // swap 保证 [0, slow) 全是非零，[slow, fast] 全是零
        }
    }
}

// LC 11: 盛最多水的容器
int maxArea(vector<int>& height) {
    int left = 0, right = (int)height.size() - 1;
    int maxWater = 0;

    while (left < right) {
        // 面积 = 较短的边 × 两边距离
        int water = min(height[left], height[right]) * (right - left);
        maxWater = max(maxWater, water);

        // 关键洞察：移动较短的那边，才可能找到更大的容器
        // 因为如果移动较长的那边，宽度减小，高度不会增大
        if (height[left] < height[right])
            left++;
        else
            right--;
    }
    return maxWater;
}

// LC 15: 三数之和
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());  // 排序是前提
    vector<vector<int>> result;
    int n = nums.size();

    for (int i = 0; i < n - 2; i++) {
        if (nums[i] > 0) break;  // 最小的数都 > 0，不可能凑出 0
        if (i > 0 && nums[i] == nums[i - 1]) continue;  // 跳过重复

        int left = i + 1, right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                while (left < right && nums[left] == nums[left + 1]) left++;
                while (left < right && nums[right] == nums[right - 1]) right--;
                left++;
                right--;
            } else if (sum < 0) {
                left++;
            } else {
                right--;
            }
        }
    }
    return result;
}

void demo_two_pointers() {
    cout << "\n========== 7. 双指针技巧 ==========\n";

    // 测试 removeDuplicates
    vector<int> nums1 = {1, 1, 2, 2, 3, 4, 4, 5};
    int newLen = removeDuplicates(nums1);
    cout << "去重后 (前 " << newLen << " 个): ";
    for (int i = 0; i < newLen; i++) cout << nums1[i] << " ";
    cout << endl;

    // 测试 moveZeroes
    vector<int> nums2 = {0, 1, 0, 3, 12};
    moveZeroes(nums2);
    cout << "移动零: ";
    for (int x : nums2) cout << x << " ";
    cout << endl;

    // 测试 maxArea
    vector<int> heights = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    cout << "盛最多水: " << maxArea(heights) << endl;  // 49

    // 测试 threeSum
    vector<int> nums3 = {-1, 0, 1, 2, -1, -4};
    auto triplets = threeSum(nums3);
    cout << "三数之和: ";
    for (auto& t : triplets) {
        cout << "[" << t[0] << "," << t[1] << "," << t[2] << "] ";
    }
    cout << endl;
}

// ============================================================================
//  第 7 章：前缀和技巧
// ============================================================================

// 构建一维前缀和数组
vector<int> buildPrefixSum(vector<int>& nums) {
    int n = nums.size();
    vector<int> prefix(n + 1, 0);
    // prefix[i] = nums[0] + nums[1] + ... + nums[i-1]
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + nums[i];
    }
    return prefix;
}

// 查询区间 [l, r] 的和
int rangeSum(vector<int>& prefix, int l, int r) {
    return prefix[r + 1] - prefix[l];
}

// LC 560: 和为 K 的子数组
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> prefixCount;
    prefixCount[0] = 1;  // 空前缀和为 0

    int sum = 0, count = 0;
    for (int num : nums) {
        sum += num;
        // 如果 sum - k 在之前出现过，说明存在子数组和为 k
        // 因为 prefix[j] - prefix[i] = k → prefix[i] = prefix[j] - k
        if (prefixCount.count(sum - k))
            count += prefixCount[sum - k];
        prefixCount[sum]++;
    }
    return count;
}

void demo_prefix_sum() {
    cout << "\n========== 8. 前缀和 ==========\n";

    vector<int> nums = {2, 1, 5, 3, 4};
    auto prefix = buildPrefixSum(nums);

    cout << "原数组: ";
    for (int x : nums) cout << x << " ";
    cout << endl;

    cout << "前缀和: ";
    for (int x : prefix) cout << x << " ";
    cout << endl;

    cout << "区间 [1,3] 的和: " << rangeSum(prefix, 1, 3) << endl;  // 1+5+3=9
    cout << "区间 [0,4] 的和: " << rangeSum(prefix, 0, 4) << endl;  // 2+1+5+3+4=15

    // 测试 subarraySum
    vector<int> nums2 = {1, 1, 1};
    cout << "和为 2 的子数组数: " << subarraySum(nums2, 2) << endl;  // 2

    vector<int> nums3 = {1, 2, 3, -3, 1, 2};
    cout << "和为 3 的子数组数: " << subarraySum(nums3, 3) << endl;  // 4
}

// ============================================================================
//  第 8 章：差分数组技巧
// ============================================================================

class DiffArray {
    vector<int> diff;
public:
    DiffArray(int n) : diff(n + 1, 0) {}

    // 对区间 [l, r] 加 val
    // 差分思想：只修改端点，最后求前缀和还原
    void add(int l, int r, int val) {
        diff[l] += val;
        if (r + 1 < (int)diff.size())
            diff[r + 1] -= val;
    }

    // 还原为结果数组
    vector<int> result() {
        int n = diff.size() - 1;
        vector<int> res(n);
        res[0] = diff[0];
        for (int i = 1; i < n; i++) {
            res[i] = res[i - 1] + diff[i];
        }
        return res;
    }
};

// LC 1094: 拼车
bool carPooling(vector<vector<int>>& trips, int capacity) {
    vector<int> diff(1001, 0);
    for (auto& t : trips) {
        diff[t[1]] += t[0];   // 在 t[1] 站上车 t[0] 人
        diff[t[2]] -= t[0];   // 在 t[2] 站下车 t[0] 人
    }
    int passengers = 0;
    for (int d : diff) {
        passengers += d;
        if (passengers > capacity) return false;
    }
    return true;
}

void demo_diff_array() {
    cout << "\n========== 9. 差分数组 ==========\n";

    DiffArray da(6);
    da.add(1, 4, 3);   // [1,4] 全部 +3
    da.add(2, 5, 2);   // [2,5] 全部 +2

    auto res = da.result();
    cout << "差分还原: ";
    for (int x : res) cout << x << " ";
    cout << endl;
    // 期望: [0, 3, 5, 5, 5, 2]

    // 测试拼车
    vector<vector<int>> trips = {{2, 1, 5}, {3, 3, 7}};
    cout << "拼车(capacity=4): " << (carPooling(trips, 4) ? "可以" : "不行") << endl;
    cout << "拼车(capacity=5): " << (carPooling(trips, 5) ? "可以" : "不行") << endl;
}

// ============================================================================
//  第 9 章：滑动窗口
// ============================================================================

// LC 209: 长度最小的子数组
// 找满足 sum >= target 的最短子数组
int minSubArrayLen(int target, vector<int>& nums) {
    int left = 0, sum = 0;
    int minLen = INT_MAX;

    for (int right = 0; right < (int)nums.size(); right++) {
        sum += nums[right];  // 扩大窗口

        // 只要窗口内的和 >= target，就尝试缩小窗口
        while (sum >= target) {
            minLen = min(minLen, right - left + 1);
            sum -= nums[left];  // 缩小窗口
            left++;
        }
    }
    return minLen == INT_MAX ? 0 : minLen;
}

// LC 53: 最大子数组和 (Kadane 算法)
int maxSubArray(vector<int>& nums) {
    int maxSum = nums[0];
    int curSum = nums[0];
    // curSum 记录以当前元素结尾的最大子数组和
    for (int i = 1; i < (int)nums.size(); i++) {
        // 要么延续前面的子数组，要么从当前元素重新开始
        curSum = max(nums[i], curSum + nums[i]);
        maxSum = max(maxSum, curSum);
    }
    return maxSum;
}

void demo_sliding_window() {
    cout << "\n========== 10. 滑动窗口 ==========\n";

    vector<int> nums1 = {2, 3, 1, 2, 4, 3};
    cout << "最短子数组(sum>=7): " << minSubArrayLen(7, nums1) << endl;  // 2

    vector<int> nums2 = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    cout << "最大子数组和: " << maxSubArray(nums2) << endl;  // 6
}

// ============================================================================
//  第 10 章：二分查找
// ============================================================================

// 标准二分：在有序数组中找 target
int binarySearch(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;  // 防 int 溢出
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// 左边界二分：找第一个 >= target 的位置
int lowerBound(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size();
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) left = mid + 1;
        else right = mid;
    }
    return left;
}

// LC 33: 搜索旋转排序数组
int searchRotated(vector<int>& nums, int target) {
    int left = 0, right = (int)nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;

        // 判断 mid 在哪一段有序部分
        if (nums[left] <= nums[mid]) {
            // 左半段有序
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;  // target 在左半段
            else
                left = mid + 1;   // target 在右半段
        } else {
            // 右半段有序
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }
    return -1;
}

void demo_binary_search() {
    cout << "\n========== 11. 二分查找 ==========\n";

    vector<int> sorted = {1, 3, 5, 7, 9, 11, 13};
    cout << "查找 7: index=" << binarySearch(sorted, 7) << endl;    // 3
    cout << "查找 6: index=" << binarySearch(sorted, 6) << endl;    // -1
    cout << "lowerBound(6): " << lowerBound(sorted, 6) << endl;     // 3 (第一个>=6是7)

    vector<int> rotated = {4, 5, 6, 7, 0, 1, 2};
    cout << "旋转数组找 0: " << searchRotated(rotated, 0) << endl;  // 4
    cout << "旋转数组找 3: " << searchRotated(rotated, 3) << endl;  // -1
}

// ============================================================================
//  第 11 章：矩阵操作
// ============================================================================

// LC 48: 旋转图像（顺时针 90°）
void rotateMatrix(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // 第一步：转置（沿主对角线翻转）
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            swap(matrix[i][j], matrix[j][i]);

    // 第二步：每行水平翻转
    for (auto& row : matrix)
        reverse(row.begin(), row.end());
}

// LC 54: 螺旋矩阵
vector<int> spiralOrder(vector<vector<int>>& matrix) {
    vector<int> result;
    int top = 0, bottom = matrix.size() - 1;
    int left = 0, right = matrix[0].size() - 1;

    while (top <= bottom && left <= right) {
        // 向右遍历上边
        for (int j = left; j <= right; j++)
            result.push_back(matrix[top][j]);
        top++;

        // 向下遍历右边
        for (int i = top; i <= bottom; i++)
            result.push_back(matrix[i][right]);
        right--;

        // 向左遍历下边（注意要检查还有没有下边）
        if (top <= bottom) {
            for (int j = right; j >= left; j--)
                result.push_back(matrix[bottom][j]);
            bottom--;
        }

        // 向上遍历左边（注意要检查还有没有左边）
        if (left <= right) {
            for (int i = bottom; i >= top; i--)
                result.push_back(matrix[i][left]);
            left++;
        }
    }
    return result;
}

// LC 240: 搜索二维矩阵 II（每行每列递增）
bool searchMatrix2D(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    // 从右上角开始搜索
    int i = 0, j = n - 1;
    while (i < m && j >= 0) {
        if (matrix[i][j] == target) return true;
        else if (matrix[i][j] > target) j--;  // 排除当前列
        else i++;                               // 排除当前行
    }
    return false;
}

void demo_matrix() {
    cout << "\n========== 12. 矩阵操作 ==========\n";

    // 旋转图像
    vector<vector<int>> mat = {{1,2,3},{4,5,6},{7,8,9}};
    cout << "旋转前:\n";
    for (auto& row : mat) {
        for (int x : row) cout << x << " ";
        cout << endl;
    }

    rotateMatrix(mat);
    cout << "顺时针旋转90°后:\n";
    for (auto& row : mat) {
        for (int x : row) cout << x << " ";
        cout << endl;
    }

    // 螺旋矩阵
    vector<vector<int>> mat2 = {{1,2,3},{4,5,6},{7,8,9}};
    auto spiral = spiralOrder(mat2);
    cout << "螺旋顺序: ";
    for (int x : spiral) cout << x << " ";
    cout << endl;  // 1 2 3 6 9 8 7 4 5

    // 搜索二维矩阵
    vector<vector<int>> mat3 = {
        {1,  4,  7, 11, 15},
        {2,  5,  8, 12, 19},
        {3,  6,  9, 16, 22},
        {10, 13, 14, 17, 24},
        {18, 21, 23, 26, 30}
    };
    cout << "搜索 5: " << (searchMatrix2D(mat3, 5) ? "找到" : "没找到") << endl;
    cout << "搜索 20: " << (searchMatrix2D(mat3, 20) ? "找到" : "没找到") << endl;
}

// ============================================================================
//  第 12 章：排序相关
// ============================================================================

// LC 56: 合并区间
vector<vector<int>> mergeIntervals(vector<vector<int>>& intervals) {
    // 按起始点排序
    sort(intervals.begin(), intervals.end());
    vector<vector<int>> result;

    for (auto& interval : intervals) {
        // 如果结果为空，或当前区间不与上一个重叠
        if (result.empty() || result.back()[1] < interval[0]) {
            result.push_back(interval);
        } else {
            // 重叠，合并（取较大的右端点）
            result.back()[1] = max(result.back()[1], interval[1]);
        }
    }
    return result;
}

// LC 88: 合并两个有序数组（从后往前填）
void mergeSortedArrays(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    int i = m - 1, j = n - 1, k = m + n - 1;

    // 从后往前比较，大的放后面
    // 这样不会覆盖 nums1 中还没处理的元素
    while (i >= 0 && j >= 0) {
        if (nums1[i] > nums2[j])
            nums1[k--] = nums1[i--];
        else
            nums1[k--] = nums2[j--];
    }
    // nums2 还有剩余的要补上
    while (j >= 0) nums1[k--] = nums2[j--];
}

void demo_sorting() {
    cout << "\n========== 13. 排序相关 ==========\n";

    // 合并区间
    vector<vector<int>> intervals = {{1,3},{2,6},{8,10},{15,18}};
    auto merged = mergeIntervals(intervals);
    cout << "合并区间: ";
    for (auto& v : merged) cout << "[" << v[0] << "," << v[1] << "] ";
    cout << endl;  // [1,6] [8,10] [15,18]

    // 合并有序数组
    vector<int> nums1 = {1, 2, 3, 0, 0, 0};
    vector<int> nums2 = {2, 5, 6};
    mergeSortedArrays(nums1, 3, nums2, 3);
    cout << "合并有序数组: ";
    for (int x : nums1) cout << x << " ";
    cout << endl;  // 1 2 2 3 5 6
}

// ============================================================================
//  第 13 章：其他经典题
// ============================================================================

// LC 136: 只出现一次的数字
// 异或: a^a=0, a^0=a
int singleNumber(vector<int>& nums) {
    int result = 0;
    for (int x : nums) result ^= x;
    return result;
}

// LC 169: 多数元素（摩尔投票法）
// 核心思想：抵消——不同的两个数互相消掉，最后剩下的就是多数
int majorityElement(vector<int>& nums) {
    int candidate = 0, count = 0;
    for (int num : nums) {
        if (count == 0) candidate = num;
        count += (num == candidate) ? 1 : -1;
    }
    return candidate;
}

// LC 238: 除自身以外数组的乘积
// 不能用除法！用前缀积和后缀积
vector<int> productExceptSelf(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, 1);

    // 第一遍：计算左边所有元素的乘积
    int leftProd = 1;
    for (int i = 0; i < n; i++) {
        result[i] = leftProd;
        leftProd *= nums[i];
    }

    // 第二遍：乘上右边所有元素的乘积
    int rightProd = 1;
    for (int i = n - 1; i >= 0; i--) {
        result[i] *= rightProd;
        rightProd *= nums[i];
    }

    return result;
}

// LC 121: 买卖股票的最佳时机
int maxProfit(vector<int>& prices) {
    int minPrice = INT_MAX;
    int maxProfit = 0;
    for (int price : prices) {
        minPrice = min(minPrice, price);         // 记录历史最低价
        maxProfit = max(maxProfit, price - minPrice);  // 今天卖出的利润
    }
    return maxProfit;
}

// LC 42: 接雨水（双指针法）
int trap(vector<int>& height) {
    int left = 0, right = (int)height.size() - 1;
    int leftMax = 0, rightMax = 0;
    int water = 0;

    while (left < right) {
        if (height[left] < height[right]) {
            // 左边较低，水量由左边最高决定
            leftMax = max(leftMax, height[left]);
            water += leftMax - height[left];
            left++;
        } else {
            // 右边较低，水量由右边最高决定
            rightMax = max(rightMax, height[right]);
            water += rightMax - height[right];
            right--;
        }
    }
    return water;
}

void demo_classic_problems() {
    cout << "\n========== 14. 经典题 ==========\n";

    // 只出现一次的数字
    vector<int> nums1 = {2, 2, 1, 3, 3};
    cout << "只出现一次: " << singleNumber(nums1) << endl;  // 1

    // 多数元素
    vector<int> nums2 = {3, 2, 3};
    cout << "多数元素: " << majorityElement(nums2) << endl;  // 3

    // 除自身以外数组的乘积
    vector<int> nums3 = {1, 2, 3, 4};
    auto products = productExceptSelf(nums3);
    cout << "除自身乘积: ";
    for (int x : products) cout << x << " ";  // 24 12 8 6
    cout << endl;

    // 买卖股票
    vector<int> prices = {7, 1, 5, 3, 6, 4};
    cout << "最大利润: " << maxProfit(prices) << endl;  // 5

    // 接雨水
    vector<int> height = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    cout << "接雨水: " << trap(height) << endl;  // 6
}

// ============================================================================
//  第 14 章：常见陷阱演示
// ============================================================================

void demo_pitfalls() {
    cout << "\n========== 15. 常见陷阱 ==========\n";

    // 陷阱 1：size() 返回无符号数
    vector<int> empty_v;
    // if (empty_v.size() - 1 >= 0)  // ⚠️ 永远为 true！因为无符号 0-1 = 超大正数
    cout << "空 vector 的 size()-1 (无符号): " << empty_v.size() - 1 << endl;
    cout << "正确写法: (int)size()-1 = " << (int)empty_v.size() - 1 << endl;

    // 陷阱 2：整数溢出
    int a = 100000, b = 100000;
    // int product = a * b;  // 溢出！
    long long safeProduct = (long long)a * b;
    cout << "安全乘法: " << safeProduct << endl;

    // 陷阱 3：mid 溢出
    int left = INT_MAX - 1, right = INT_MAX;
    // int mid = (left + right) / 2;  // 溢出！
    int safeMid = left + (right - left) / 2;
    cout << "安全 mid: " << safeMid << endl;

    // 提示：at() 比 [] 更安全
    vector<int> v = {1, 2, 3};
    try {
        int val = v.at(10);  // 抛出 out_of_range 异常
        (void)val;
    } catch (const out_of_range& e) {
        cout << "at(10) 越界异常: " << e.what() << endl;
    }
}

// ============================================================================
//  主函数：运行所有示例
// ============================================================================

int main() {
    cout << "╔══════════════════════════════════════════╗\n";
    cout << "║   专题一：数组 (Array) 完整示例代码       ║\n";
    cout << "╚══════════════════════════════════════════╝\n";

    demo_raw_array();          // 1. 原生数组
    demo_vector_basics();      // 2. vector 基础
    demo_vector_memory();      // 3. vector 内存
    demo_std_array();          // 4. std::array
    demo_traversal();          // 5. 遍历方式
    demo_stl_algorithms();     // 6. STL 算法
    demo_two_pointers();       // 7. 双指针
    demo_prefix_sum();         // 8. 前缀和
    demo_diff_array();         // 9. 差分数组
    demo_sliding_window();     // 10. 滑动窗口
    demo_binary_search();      // 11. 二分查找
    demo_matrix();             // 12. 矩阵
    demo_sorting();            // 13. 排序
    demo_classic_problems();   // 14. 经典题
    demo_pitfalls();           // 15. 陷阱

    cout << "\n✅ 所有示例运行完毕！\n";
    return 0;
}
