/**
 * 专题十三（上）：排序算法基础 — 比较排序全解 示例代码
 * 作者：大胖超 😜
 *
 * 编译: g++ -std=c++17 -o knowlege_details_13_sort_basics knowlege_details_13_sort_basics.cpp
 * 运行: ./knowlege_details_13_sort_basics
 *
 * 覆盖内容：
 *   Demo1: 冒泡 / 选择 / 插入排序（三种基础排序对比）
 *   Demo2: 希尔排序
 *   Demo3: 快速排序（Lomuto + Hoare + 三向切分 + 随机化）
 *   Demo4: 归并排序 + 逆序对计数
 *   Demo5: 堆排序（手写 heapify + 完整流程）
 *   Demo6: 快速选择 — LC 215 第K大元素
 *   Demo7: 排序稳定性验证
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <functional>
#include <cassert>
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
    printVec(v, label);
    cout << "\n";
}

// ==================== Demo1: 冒泡 / 选择 / 插入 ====================
void demo1_basicSorts() {
    cout << "===== Demo1: 冒泡 / 选择 / 插入排序 =====\n\n";

    // --- 冒泡排序 ---
    {
        cout << "--- 冒泡排序 ---\n";
        vector<int> nums = {5, 3, 8, 1, 2};
        printVecLn(nums, "原始: ");

        int n = nums.size();
        for (int i = 0; i < n - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < n - 1 - i; j++) {
                if (nums[j] > nums[j + 1]) {
                    swap(nums[j], nums[j + 1]);
                    swapped = true;
                }
            }
            cout << "  第" << i+1 << "轮: ";
            printVec(nums);
            if (!swapped) { cout << " (无交换,提前终止)"; }
            cout << "\n";
            if (!swapped) break;
        }
        printVecLn(nums, "结果: ");
        cout << "\n";
    }

    // --- 选择排序 ---
    {
        cout << "--- 选择排序 ---\n";
        vector<int> nums = {3, 5, 1, 8, 2};
        printVecLn(nums, "原始: ");

        int n = nums.size();
        for (int i = 0; i < n - 1; i++) {
            int minIdx = i;
            for (int j = i + 1; j < n; j++)
                if (nums[j] < nums[minIdx])
                    minIdx = j;
            cout << "  第" << i+1 << "轮: 最小值=" << nums[minIdx]
                 << "(下标" << minIdx << "), ";
            if (minIdx != i) {
                swap(nums[i], nums[minIdx]);
                cout << "swap → ";
            } else {
                cout << "不动 → ";
            }
            printVecLn(nums);
        }
        printVecLn(nums, "结果: ");
        cout << "\n";
    }

    // --- 插入排序 ---
    {
        cout << "--- 插入排序 ---\n";
        vector<int> nums = {5, 3, 8, 1, 2};
        printVecLn(nums, "原始: ");

        int n = nums.size();
        for (int i = 1; i < n; i++) {
            int key = nums[i];
            int j = i - 1;
            cout << "  i=" << i << ": 插入key=" << key << " → ";
            while (j >= 0 && nums[j] > key) {
                nums[j + 1] = nums[j];
                j--;
            }
            nums[j + 1] = key;
            printVecLn(nums);
        }
        printVecLn(nums, "结果: ");
        cout << "\n";
    }
}

// ==================== Demo2: 希尔排序 ====================
void demo2_shellSort() {
    cout << "===== Demo2: 希尔排序 =====\n\n";

    vector<int> nums = {8, 5, 9, 1, 7, 3, 6, 2};
    printVecLn(nums, "原始: ");

    int n = nums.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {
        cout << "  gap=" << gap << ": ";
        for (int i = gap; i < n; i++) {
            int key = nums[i];
            int j = i - gap;
            while (j >= 0 && nums[j] > key) {
                nums[j + gap] = nums[j];
                j -= gap;
            }
            nums[j + gap] = key;
        }
        printVecLn(nums);
    }
    printVecLn(nums, "结果: ");
    cout << "\n";
}

// ==================== Demo3: 快速排序 ====================
void demo3_quickSort() {
    cout << "===== Demo3: 快速排序 =====\n\n";

    // --- Lomuto Partition ---
    {
        cout << "--- Lomuto Partition ---\n";
        vector<int> nums = {3, 6, 8, 1, 5, 2};
        printVecLn(nums, "原始: ");

        int step = 0;
        function<void(int, int)> quickSortLomuto = [&](int lo, int hi) {
            if (lo >= hi) return;
            int pivot = nums[hi];
            int i = lo;
            step++;
            cout << "  步骤" << step << ": partition [" << lo << ".." << hi
                 << "], pivot=" << pivot << "\n";
            for (int j = lo; j < hi; j++) {
                if (nums[j] < pivot) {
                    swap(nums[i], nums[j]);
                    i++;
                }
            }
            swap(nums[i], nums[hi]);
            cout << "    → pivot在下标" << i << ": ";
            printVecLn(nums);
            quickSortLomuto(lo, i - 1);
            quickSortLomuto(i + 1, hi);
        };

        quickSortLomuto(0, nums.size() - 1);
        printVecLn(nums, "结果: ");
        cout << "\n";
    }

    // --- Hoare Partition （推荐写法） ---
    {
        cout << "--- Hoare Partition (推荐) ---\n";
        vector<int> nums = {5, 3, 8, 6, 2, 7, 1, 4};
        printVecLn(nums, "原始: ");

        function<void(int, int)> quickSortHoare = [&](int left, int right) {
            if (left >= right) return;
            int pivot = nums[left + (right - left) / 2];
            int i = left, j = right;
            while (i <= j) {
                while (nums[i] < pivot) i++;
                while (nums[j] > pivot) j--;
                if (i <= j) {
                    swap(nums[i], nums[j]);
                    i++;
                    j--;
                }
            }
            quickSortHoare(left, j);
            quickSortHoare(i, right);
        };

        quickSortHoare(0, nums.size() - 1);
        printVecLn(nums, "结果: ");
        cout << "\n";
    }

    // --- 三向切分 (Dutch National Flag) ---
    {
        cout << "--- 三向切分 (大量重复元素) ---\n";
        vector<int> nums = {3, 1, 2, 3, 3, 1, 2, 3, 2, 1};
        printVecLn(nums, "原始: ");

        function<void(int, int)> quickSort3Way = [&](int lo, int hi) {
            if (lo >= hi) return;
            int pivot = nums[lo];
            int lt = lo, i = lo + 1, gt = hi;
            while (i <= gt) {
                if (nums[i] < pivot)
                    swap(nums[lt++], nums[i++]);
                else if (nums[i] > pivot)
                    swap(nums[i], nums[gt--]);
                else
                    i++;
            }
            quickSort3Way(lo, lt - 1);
            quickSort3Way(gt + 1, hi);
        };

        quickSort3Way(0, nums.size() - 1);
        printVecLn(nums, "结果: ");
        cout << "\n";
    }

    // --- 随机化 Pivot ---
    {
        cout << "--- 随机化 Pivot ---\n";
        // 构造一个快排最坏情况：有序数组
        vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        cout << "有序数组(快排最坏情况): ";
        printVecLn(nums);

        mt19937 rng(42);
        function<void(int, int)> quickSortRandom = [&](int lo, int hi) {
            if (lo >= hi) return;
            // 随机选 pivot 避免 O(n²)
            int ri = lo + rng() % (hi - lo + 1);
            swap(nums[lo], nums[ri]);
            int pivot = nums[lo];
            int lt = lo, i = lo + 1, gt = hi;
            while (i <= gt) {
                if (nums[i] < pivot) swap(nums[lt++], nums[i++]);
                else if (nums[i] > pivot) swap(nums[i], nums[gt--]);
                else i++;
            }
            quickSortRandom(lo, lt - 1);
            quickSortRandom(gt + 1, hi);
        };

        quickSortRandom(0, nums.size() - 1);
        printVecLn(nums, "随机化快排结果: ");
        cout << "\n";
    }
}

// ==================== Demo4: 归并排序 + 逆序对 ====================
void demo4_mergeSort() {
    cout << "===== Demo4: 归并排序 + 逆序对计数 =====\n\n";

    // --- 归并排序（带追踪） ---
    {
        cout << "--- 归并排序（带合并过程追踪） ---\n";
        vector<int> nums = {5, 3, 8, 1, 2, 7};
        printVecLn(nums, "原始: ");

        int mergeCount = 0;
        function<void(int, int)> mergeSort = [&](int left, int right) {
            if (left >= right) return;
            int mid = left + (right - left) / 2;
            mergeSort(left, mid);
            mergeSort(mid + 1, right);

            if (nums[mid] <= nums[mid + 1]) return;  // 优化

            mergeCount++;
            vector<int> temp(right - left + 1);
            int i = left, j = mid + 1, k = 0;
            while (i <= mid && j <= right) {
                if (nums[i] <= nums[j]) temp[k++] = nums[i++];
                else temp[k++] = nums[j++];
            }
            while (i <= mid) temp[k++] = nums[i++];
            while (j <= right) temp[k++] = nums[j++];
            copy(temp.begin(), temp.end(), nums.begin() + left);

            cout << "  合并#" << mergeCount << " ["
                 << left << ".." << mid << "] + [" << mid+1 << ".." << right << "] → ";
            for (int x = left; x <= right; x++) cout << nums[x] << " ";
            cout << "\n";
        };

        mergeSort(0, nums.size() - 1);
        printVecLn(nums, "结果: ");
        cout << "共执行 " << mergeCount << " 次合并\n\n";
    }

    // --- 逆序对计数（剑指 Offer 51） ---
    {
        cout << "--- 逆序对计数（归并变体） ---\n";
        vector<int> nums = {7, 5, 6, 4};
        cout << "数组: ";
        printVecLn(nums);

        long long totalCount = 0;
        function<void(int, int)> mergeSortCount = [&](int left, int right) {
            if (left >= right) return;
            int mid = left + (right - left) / 2;
            mergeSortCount(left, mid);
            mergeSortCount(mid + 1, right);

            vector<int> temp;
            int i = left, j = mid + 1;
            while (i <= mid && j <= right) {
                if (nums[i] <= nums[j]) {
                    temp.push_back(nums[i++]);
                } else {
                    long long cnt = mid - i + 1;
                    totalCount += cnt;
                    cout << "  " << nums[j] << " < [";
                    for (int x = i; x <= mid; x++) cout << nums[x] << (x<mid?",":"");
                    cout << "] → +" << cnt << " 个逆序对\n";
                    temp.push_back(nums[j++]);
                }
            }
            while (i <= mid) temp.push_back(nums[i++]);
            while (j <= right) temp.push_back(nums[j++]);
            copy(temp.begin(), temp.end(), nums.begin() + left);
        };

        mergeSortCount(0, nums.size() - 1);
        cout << "逆序对总数: " << totalCount << "\n";
        cout << "验证: (7,5)(7,6)(7,4)(5,4)(6,4) = 5 个\n\n";
    }
}

// ==================== Demo5: 堆排序 ====================
void demo5_heapSort() {
    cout << "===== Demo5: 堆排序 =====\n\n";

    vector<int> nums = {3, 5, 1, 8, 2, 7};
    printVecLn(nums, "原始: ");

    int n = nums.size();

    // heapify: 下沉调整
    function<void(int, int)> heapify = [&](int size, int i) {
        int largest = i;
        int left = 2 * i + 1, right = 2 * i + 2;
        if (left < size && nums[left] > nums[largest]) largest = left;
        if (right < size && nums[right] > nums[largest]) largest = right;
        if (largest != i) {
            swap(nums[i], nums[largest]);
            heapify(size, largest);
        }
    };

    // 建堆
    cout << "建大顶堆:\n";
    for (int i = n / 2 - 1; i >= 0; i--) {
        cout << "  heapify(i=" << i << ", val=" << nums[i] << "): ";
        heapify(n, i);
        printVecLn(nums);
    }

    // 排序
    cout << "排序过程:\n";
    for (int i = n - 1; i > 0; i--) {
        cout << "  swap(堆顶" << nums[0] << ", 末尾" << nums[i] << "): ";
        swap(nums[0], nums[i]);
        heapify(i, 0);
        printVecLn(nums);
    }
    printVecLn(nums, "结果: ");
    cout << "\n";
}

// ==================== Demo6: 快速选择 LC 215 ====================
void demo6_quickSelect() {
    cout << "===== Demo6: 快速选择 — LC 215 第K大元素 =====\n\n";

    auto findKthLargest = [](vector<int> nums, int k) -> int {
        // 转换：第 k 大 = 第 (n-k) 小（0-based）
        int target = nums.size() - k;
        mt19937 rng(42);

        function<int(int, int)> quickSelect = [&](int lo, int hi) -> int {
            if (lo == hi) return nums[lo];
            int ri = lo + rng() % (hi - lo + 1);
            swap(nums[lo], nums[ri]);
            int pivot = nums[lo];
            int lt = lo, gt = hi, i = lo + 1;
            while (i <= gt) {
                if (nums[i] < pivot) swap(nums[lt++], nums[i++]);
                else if (nums[i] > pivot) swap(nums[i], nums[gt--]);
                else i++;
            }
            // [lo..lt-1] < pivot, [lt..gt] == pivot, [gt+1..hi] > pivot
            if (target < lt) return quickSelect(lo, lt - 1);
            if (target > gt) return quickSelect(gt + 1, hi);
            return nums[lt];
        };

        return quickSelect(0, nums.size() - 1);
    };

    vector<int> nums1 = {3, 2, 1, 5, 6, 4};
    cout << "数组: "; printVecLn(nums1);
    cout << "第2大: " << findKthLargest(nums1, 2) << " (期望5)\n";

    vector<int> nums2 = {3, 2, 3, 1, 2, 4, 5, 5, 6};
    cout << "数组: "; printVecLn(nums2);
    cout << "第4大: " << findKthLargest(nums2, 4) << " (期望4)\n\n";
}

// ==================== Demo7: 排序稳定性验证 ====================
void demo7_stability() {
    cout << "===== Demo7: 排序稳定性验证 =====\n\n";

    struct Student {
        string name;
        int score;
    };

    vector<Student> students = {
        {"Alice", 90}, {"Bob", 90}, {"Charlie", 85}, {"Diana", 95}, {"Eve", 85}
    };

    cout << "原始数据:\n";
    for (auto& s : students)
        cout << "  " << s.name << " - " << s.score << "\n";

    // stable_sort — 稳定排序
    {
        auto data = students;
        stable_sort(data.begin(), data.end(),
                    [](auto& a, auto& b) { return a.score < b.score; });
        cout << "\nstd::stable_sort (稳定 ✓):\n";
        for (auto& s : data)
            cout << "  " << s.name << " - " << s.score << "\n";
        // 相同分数的 Alice 和 Bob 保持原始顺序
        cout << "  → 同分数85: Charlie在Eve前(原始顺序保持)\n";
        cout << "  → 同分数90: Alice在Bob前(原始顺序保持)\n";
    }

    // sort — 不稳定排序
    {
        auto data = students;
        sort(data.begin(), data.end(),
             [](auto& a, auto& b) { return a.score < b.score; });
        cout << "\nstd::sort (不保证稳定):\n";
        for (auto& s : data)
            cout << "  " << s.name << " - " << s.score << "\n";
        cout << "  → 同分数学生顺序可能改变\n";
    }

    // --- 演示: 多关键字排序利用稳定性 ---
    cout << "\n--- 多关键字排序策略 ---\n";
    cout << "先按姓名排，再用 stable_sort 按分数排\n";
    cout << "→ 同分数的学生按姓名字母序排列\n";
    {
        auto data = students;
        // 第一步：按姓名排（字母序）
        sort(data.begin(), data.end(),
             [](auto& a, auto& b) { return a.name < b.name; });
        cout << "按姓名排: ";
        for (auto& s : data) cout << s.name << "(" << s.score << ") ";
        cout << "\n";

        // 第二步：按分数 stable_sort
        stable_sort(data.begin(), data.end(),
                    [](auto& a, auto& b) { return a.score < b.score; });
        cout << "再按分数stable_sort:\n";
        for (auto& s : data)
            cout << "  " << s.name << " - " << s.score << "\n";
        cout << "  → 同分数85: Charlie在Eve前(字母序)\n";
        cout << "  → 同分数90: Alice在Bob前(字母序)\n";
    }
    cout << "\n";
}

// ==================== main ====================
int main() {
    cout << "╔════════════════════════════════════════════════════╗\n";
    cout << "║  专题十三（上）：比较排序全解 — 7大排序算法演示    ║\n";
    cout << "║  作者：大胖超 😜                                   ║\n";
    cout << "╚════════════════════════════════════════════════════╝\n\n";

    demo1_basicSorts();
    demo2_shellSort();
    demo3_quickSort();
    demo4_mergeSort();
    demo5_heapSort();
    demo6_quickSelect();
    demo7_stability();

    cout << "========================================\n";
    cout << "全部 Demo 运行完毕！\n";
    cout << "覆盖算法: 冒泡排序, 选择排序, 插入排序, 希尔排序,\n";
    cout << "         快速排序(Lomuto/Hoare/三向切分/随机化),\n";
    cout << "         归并排序(+逆序对), 堆排序, 快速选择\n";
    cout << "覆盖题目: LC 215, 剑指Offer 51\n";
    return 0;
}
