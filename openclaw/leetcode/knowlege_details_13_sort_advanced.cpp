/**
 * 专题十三（下）：排序进阶应用 — 非比较排序与LeetCode实战 示例代码
 * 作者：大胖超 😜
 *
 * 编译: g++ -std=c++17 -o knowlege_details_13_sort_advanced knowlege_details_13_sort_advanced.cpp
 * 运行: ./knowlege_details_13_sort_advanced
 *
 * 覆盖内容：
 *   Demo1: 计数排序（标准版 + 简化版）
 *   Demo2: 桶排序（浮点版 + 整数版）+ LC 164 最大间距
 *   Demo3: 基数排序
 *   Demo4: LC 56 合并区间
 *   Demo5: LC 75 颜色分类（荷兰国旗三向切分）
 *   Demo6: LC 179 最大数（自定义比较器）
 *   Demo7: LC 148 排序链表（链表归并排序）
 *   Demo8: LC 315 右侧小于当前元素的个数（归并+索引追踪）
 *   Demo9: LC 347 前K个高频元素（桶排序法）
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <functional>
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

// ==================== Demo1: 计数排序 ====================
void demo1_countingSort() {
    cout << "===== Demo1: 计数排序 =====\n\n";

    // --- 标准计数排序（稳定） ---
    {
        cout << "--- 标准计数排序（稳定版） ---\n";
        vector<int> nums = {4, 2, 2, 8, 3, 3, 1};
        printVecLn(nums, "原始: ");

        int minVal = *min_element(nums.begin(), nums.end());
        int maxVal = *max_element(nums.begin(), nums.end());
        int range = maxVal - minVal + 1;

        vector<int> count(range, 0);
        for (int x : nums) count[x - minVal]++;

        cout << "计数: ";
        for (int i = 0; i < range; i++)
            if (count[i] > 0)
                cout << (i + minVal) << "×" << count[i] << " ";
        cout << "\n";

        // 前缀和
        for (int i = 1; i < range; i++)
            count[i] += count[i - 1];

        // 反向填充保证稳定性
        vector<int> output(nums.size());
        for (int i = nums.size() - 1; i >= 0; i--) {
            output[count[nums[i] - minVal] - 1] = nums[i];
            count[nums[i] - minVal]--;
        }
        printVecLn(output, "结果: ");
        cout << "\n";
    }

    // --- 简化版（不需稳定时） ---
    {
        cout << "--- 简化计数排序 ---\n";
        vector<int> nums = {5, 3, 1, 3, 2, 5, 1};
        printVecLn(nums, "原始: ");

        int maxVal = *max_element(nums.begin(), nums.end());
        vector<int> count(maxVal + 1, 0);
        for (int x : nums) count[x]++;

        int idx = 0;
        for (int i = 0; i <= maxVal; i++)
            while (count[i]-- > 0)
                nums[idx++] = i;
        printVecLn(nums, "结果: ");
        cout << "\n";
    }
}

// ==================== Demo2: 桶排序 + LC 164 ====================
void demo2_bucketSort() {
    cout << "===== Demo2: 桶排序 =====\n\n";

    // --- 浮点桶排序 ---
    {
        cout << "--- 浮点桶排序 [0,1) ---\n";
        vector<float> nums = {0.42f, 0.32f, 0.33f, 0.52f, 0.37f, 0.47f, 0.51f};
        cout << "原始: [";
        for (int i = 0; i < (int)nums.size(); i++)
            cout << nums[i] << (i < (int)nums.size()-1 ? ", " : "");
        cout << "]\n";

        int n = nums.size();
        vector<vector<float>> buckets(n);
        for (float x : nums) {
            int idx = (int)(x * n);
            if (idx >= n) idx = n - 1;
            buckets[idx].push_back(x);
        }

        cout << "桶分配:\n";
        for (int i = 0; i < n; i++)
            if (!buckets[i].empty()) {
                cout << "  桶" << i << ": ";
                for (float x : buckets[i]) cout << x << " ";
                cout << "\n";
            }

        for (auto& b : buckets) sort(b.begin(), b.end());
        int idx = 0;
        for (auto& b : buckets)
            for (float x : b) nums[idx++] = x;

        cout << "结果: [";
        for (int i = 0; i < (int)nums.size(); i++)
            cout << nums[i] << (i < (int)nums.size()-1 ? ", " : "");
        cout << "]\n\n";
    }

    // --- LC 164: 最大间距 ---
    {
        cout << "--- LC 164: 最大间距（桶排序 + 鸽巢原理） ---\n";
        auto maximumGap = [](vector<int> nums) -> int {
            int n = nums.size();
            if (n < 2) return 0;
            int minVal = *min_element(nums.begin(), nums.end());
            int maxVal = *max_element(nums.begin(), nums.end());
            if (minVal == maxVal) return 0;

            int bucketSize = max(1, (maxVal - minVal) / (n - 1));
            int bucketCount = (maxVal - minVal) / bucketSize + 1;

            vector<int> bMin(bucketCount, INT_MAX);
            vector<int> bMax(bucketCount, INT_MIN);
            vector<bool> used(bucketCount, false);

            for (int x : nums) {
                int idx = (x - minVal) / bucketSize;
                used[idx] = true;
                bMin[idx] = min(bMin[idx], x);
                bMax[idx] = max(bMax[idx], x);
            }

            int maxGap = 0, prevMax = minVal;
            cout << "  桶信息:\n";
            for (int i = 0; i < bucketCount; i++) {
                if (!used[i]) {
                    cout << "    桶" << i << ": 空\n";
                    continue;
                }
                cout << "    桶" << i << ": [" << bMin[i] << ".." << bMax[i]
                     << "], 间距=" << bMin[i] - prevMax << "\n";
                maxGap = max(maxGap, bMin[i] - prevMax);
                prevMax = bMax[i];
            }
            return maxGap;
        };

        vector<int> nums = {3, 6, 9, 1};
        printVecLn(nums, "数组: ");
        cout << "  排序后: [1, 3, 6, 9]\n";
        cout << "  最大间距: " << maximumGap(nums) << " (期望3)\n\n";
    }
}

// ==================== Demo3: 基数排序 ====================
void demo3_radixSort() {
    cout << "===== Demo3: 基数排序 =====\n\n";

    vector<int> nums = {170, 45, 75, 90, 802, 24, 2, 66};
    printVecLn(nums, "原始: ");

    int maxVal = *max_element(nums.begin(), nums.end());

    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        vector<int> output(nums.size());
        int count[10] = {0};

        for (int x : nums) count[(x / exp) % 10]++;
        for (int i = 1; i < 10; i++) count[i] += count[i - 1];
        for (int i = nums.size() - 1; i >= 0; i--) {
            int digit = (nums[i] / exp) % 10;
            output[count[digit] - 1] = nums[i];
            count[digit]--;
        }
        nums = output;

        string pos = (exp == 1) ? "个" : (exp == 10 ? "十" : "百");
        cout << "按" << pos << "位排: ";
        printVecLn(nums);
    }

    printVecLn(nums, "结果: ");
    cout << "\n";
}

// ==================== Demo4: LC 56 合并区间 ====================
void demo4_mergeIntervals() {
    cout << "===== Demo4: LC 56 合并区间 =====\n\n";

    auto merge = [](vector<vector<int>> intervals) -> vector<vector<int>> {
        sort(intervals.begin(), intervals.end());
        vector<vector<int>> result;
        for (auto& intv : intervals) {
            if (!result.empty() && intv[0] <= result.back()[1]) {
                result.back()[1] = max(result.back()[1], intv[1]);
            } else {
                result.push_back(intv);
            }
        }
        return result;
    };

    vector<vector<int>> intervals = {{1,3},{2,6},{8,10},{15,18}};
    cout << "输入: [[1,3],[2,6],[8,10],[15,18]]\n";

    auto result = merge(intervals);
    cout << "合并: [";
    for (int i = 0; i < (int)result.size(); i++) {
        cout << "[" << result[i][0] << "," << result[i][1] << "]";
        if (i < (int)result.size()-1) cout << ",";
    }
    cout << "]\n";
    cout << "过程:\n";
    cout << "  排序后: [1,3],[2,6],[8,10],[15,18]\n";
    cout << "  [1,3]+[2,6] → 2≤3 合并 → [1,6]\n";
    cout << "  [1,6]+[8,10] → 8>6 不合并\n";
    cout << "  [8,10]+[15,18] → 15>10 不合并\n\n";

    // 测试2: 完全重叠
    vector<vector<int>> intervals2 = {{1,4},{0,4}};
    cout << "输入: [[1,4],[0,4]]\n";
    auto result2 = merge(intervals2);
    cout << "合并: [[" << result2[0][0] << "," << result2[0][1] << "]]\n\n";
}

// ==================== Demo5: LC 75 荷兰国旗 ====================
void demo5_sortColors() {
    cout << "===== Demo5: LC 75 颜色分类（荷兰国旗） =====\n\n";

    auto sortColors = [](vector<int> nums) -> vector<int> {
        int lo = 0, hi = nums.size() - 1, i = 0;
        cout << "过程:\n";
        while (i <= hi) {
            cout << "  i=" << i << " lo=" << lo << " hi=" << hi
                 << " arr="; printVec(nums);
            if (nums[i] == 0) {
                cout << " nums[" << i << "]=0 → swap(lo)\n";
                swap(nums[i++], nums[lo++]);
            } else if (nums[i] == 2) {
                cout << " nums[" << i << "]=2 → swap(hi), i不动\n";
                swap(nums[i], nums[hi--]);
            } else {
                cout << " nums[" << i << "]=1 → i++\n";
                i++;
            }
        }
        return nums;
    };

    vector<int> nums = {2, 0, 2, 1, 1, 0};
    printVecLn(nums, "输入: ");
    auto result = sortColors(nums);
    printVecLn(result, "结果: ");
    cout << "\n";
}

// ==================== Demo6: LC 179 最大数 ====================
void demo6_largestNumber() {
    cout << "===== Demo6: LC 179 最大数 =====\n\n";

    auto largestNumber = [](vector<int> nums) -> string {
        vector<string> strs;
        for (int x : nums) strs.push_back(to_string(x));

        sort(strs.begin(), strs.end(),
            [](const string& a, const string& b) {
                return a + b > b + a;
            });

        if (strs[0] == "0") return "0";

        string result;
        for (auto& s : strs) result += s;
        return result;
    };

    {
        vector<int> nums = {10, 2};
        printVecLn(nums, "输入: ");
        cout << "结果: " << largestNumber(nums) << "\n";
        cout << "比较: \"210\" vs \"102\" → \"2\"+\"10\"=\"210\" > \"10\"+\"2\"=\"102\"\n\n";
    }

    {
        vector<int> nums = {3, 30, 34, 5, 9};
        printVecLn(nums, "输入: ");
        string res = largestNumber(nums);
        cout << "结果: " << res << "\n";
        cout << "排序后: 9 > 5 > 34 > 3 > 30\n";
        cout << "验证: \"3\"+\"30\"=\"330\" > \"30\"+\"3\"=\"303\" → 3在30前\n";
        cout << "        \"34\"+\"3\"=\"343\" > \"3\"+\"34\"=\"334\" → 34在3前\n\n";
    }

    {
        vector<int> nums = {0, 0};
        printVecLn(nums, "输入: ");
        cout << "结果: " << largestNumber(nums) << " (应该是\"0\"而不是\"00\")\n\n";
    }
}

// ==================== Demo7: LC 148 排序链表 ====================
void demo7_sortList() {
    cout << "===== Demo7: LC 148 排序链表（归并） =====\n\n";

    struct ListNode {
        int val;
        ListNode* next;
        ListNode(int x) : val(x), next(nullptr) {}
    };

    // 构建链表
    auto buildList = [](vector<int>& v) -> ListNode* {
        ListNode dummy(0);
        ListNode* tail = &dummy;
        for (int x : v) {
            tail->next = new ListNode(x);
            tail = tail->next;
        }
        return dummy.next;
    };

    auto printList = [](ListNode* h) {
        while (h) {
            cout << h->val;
            if (h->next) cout << " → ";
            h = h->next;
        }
        cout << "\n";
    };

    // 归并排序链表
    function<ListNode*(ListNode*)> sortList = [&](ListNode* head) -> ListNode* {
        if (!head || !head->next) return head;

        // 快慢指针找中点
        ListNode* slow = head, *fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        ListNode* mid = slow->next;
        slow->next = nullptr;

        ListNode* left = sortList(head);
        ListNode* right = sortList(mid);

        // 合并
        ListNode dummy(0);
        ListNode* tail = &dummy;
        while (left && right) {
            if (left->val <= right->val) {
                tail->next = left;
                left = left->next;
            } else {
                tail->next = right;
                right = right->next;
            }
            tail = tail->next;
        }
        tail->next = left ? left : right;
        return dummy.next;
    };

    vector<int> v = {4, 2, 1, 3};
    ListNode* head = buildList(v);
    cout << "原链表: "; printList(head);

    head = sortList(head);
    cout << "排序后: "; printList(head);

    vector<int> v2 = {-1, 5, 3, 4, 0};
    ListNode* head2 = buildList(v2);
    cout << "\n原链表: "; printList(head2);
    head2 = sortList(head2);
    cout << "排序后: "; printList(head2);

    // 清理内存
    auto freeList = [](ListNode* h) {
        while (h) { auto t = h; h = h->next; delete t; }
    };
    freeList(head);
    freeList(head2);
    cout << "\n";
}

// ==================== Demo8: LC 315 右侧小于当前元素 ====================
void demo8_countSmaller() {
    cout << "===== Demo8: LC 315 右侧小于当前元素的个数 =====\n\n";

    auto countSmaller = [](vector<int> nums) -> vector<int> {
        int n = nums.size();
        vector<int> result(n, 0);
        vector<int> index(n);
        iota(index.begin(), index.end(), 0);

        function<void(int, int)> mergeSort = [&](int left, int right) {
            if (left >= right) return;
            int mid = left + (right - left) / 2;
            mergeSort(left, mid);
            mergeSort(mid + 1, right);

            vector<int> tempIdx;
            int i = left, j = mid + 1;
            while (i <= mid && j <= right) {
                if (nums[index[i]] <= nums[index[j]]) {
                    result[index[i]] += (j - mid - 1);
                    tempIdx.push_back(index[i++]);
                } else {
                    tempIdx.push_back(index[j++]);
                }
            }
            while (i <= mid) {
                result[index[i]] += (j - mid - 1);
                tempIdx.push_back(index[i++]);
            }
            while (j <= right)
                tempIdx.push_back(index[j++]);
            copy(tempIdx.begin(), tempIdx.end(), index.begin() + left);
        };

        mergeSort(0, n - 1);
        return result;
    };

    vector<int> nums = {5, 2, 6, 1};
    printVecLn(nums, "输入: ");

    auto result = countSmaller(nums);
    printVecLn(result, "输出: ");
    cout << "解释:\n";
    cout << "  5的右侧小于5的: [2, 1] → 2个\n";
    cout << "  2的右侧小于2的: [1] → 1个\n";
    cout << "  6的右侧小于6的: [1] → 1个\n";
    cout << "  1的右侧: 无 → 0个\n\n";

    vector<int> nums2 = {-1, -1};
    printVecLn(nums2, "输入: ");
    auto result2 = countSmaller(nums2);
    printVecLn(result2, "输出: ");
    cout << "\n";
}

// ==================== Demo9: LC 347 前K个高频元素 ====================
void demo9_topKFrequent() {
    cout << "===== Demo9: LC 347 前K个高频元素（桶排序） =====\n\n";

    auto topKFrequent = [](vector<int> nums, int k) -> vector<int> {
        unordered_map<int, int> freq;
        for (int x : nums) freq[x]++;

        int n = nums.size();
        vector<vector<int>> buckets(n + 1);
        for (auto& [val, cnt] : freq)
            buckets[cnt].push_back(val);

        vector<int> result;
        for (int i = n; i >= 0 && (int)result.size() < k; i--)
            for (int x : buckets[i]) {
                result.push_back(x);
                if ((int)result.size() == k) break;
            }
        return result;
    };

    {
        vector<int> nums = {1, 1, 1, 2, 2, 3};
        int k = 2;
        printVecLn(nums, "输入: ");
        cout << "k=" << k << "\n";

        // 显示频率
        unordered_map<int, int> freq;
        for (int x : nums) freq[x]++;
        cout << "频率: ";
        for (auto& [val, cnt] : freq) cout << val << "×" << cnt << " ";
        cout << "\n";

        auto result = topKFrequent(nums, k);
        cout << "前" << k << "高频: ";
        printVecLn(result);

        cout << "桶排序思路: 频率做桶下标\n";
        int n = nums.size();
        cout << "  桶3: [1]  (出现3次)\n";
        cout << "  桶2: [2]  (出现2次)\n";
        cout << "  桶1: [3]  (出现1次)\n";
        cout << "  从高频桶向低频取k个\n\n";
    }

    {
        vector<int> nums = {1};
        int k = 1;
        printVecLn(nums, "输入: ");
        auto result = topKFrequent(nums, k);
        printVecLn(result, "前1高频: ");
        cout << "\n";
    }
}

// ==================== main ====================
int main() {
    cout << "╔════════════════════════════════════════════════════════╗\n";
    cout << "║  专题十三（下）：非比较排序 + LeetCode排序实战        ║\n";
    cout << "║  作者：大胖超 😜                                       ║\n";
    cout << "╚════════════════════════════════════════════════════════╝\n\n";

    demo1_countingSort();
    demo2_bucketSort();
    demo3_radixSort();
    demo4_mergeIntervals();
    demo5_sortColors();
    demo6_largestNumber();
    demo7_sortList();
    demo8_countSmaller();
    demo9_topKFrequent();

    cout << "========================================\n";
    cout << "全部 Demo 运行完毕！\n";
    cout << "覆盖算法: 计数排序, 桶排序, 基数排序,\n";
    cout << "         荷兰国旗, 自定义比较器, 链表归并, 归并索引追踪\n";
    cout << "覆盖题目: LC 56, 75, 148, 164, 179, 315, 347\n";
    cout << "共 9 个 Demo, 7 道 LeetCode 题\n";
    return 0;
}
