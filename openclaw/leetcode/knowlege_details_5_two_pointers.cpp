/**
 * ============================================================
 *   专题五：双指针（Two Pointers）示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_5_two_pointers knowlege_details_5_two_pointers.cpp
 *   配合 knowlege_details_5_two_pointers.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
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

void printVecVec(const vector<vector<int>>& vv, const string& label = "") {
    if (!label.empty()) cout << label << ":" << endl;
    for (auto& v : vv) {
        cout << "  [";
        for (int i = 0; i < (int)v.size(); i++) {
            if (i) cout << ", ";
            cout << v[i];
        }
        cout << "]" << endl;
    }
}

// ============================================================
//  Demo 1: 对撞指针 — 有序数组两数之和 & 回文
//  LC 167: Two Sum II
//  LC 125: Valid Palindrome
//  LC 680: Valid Palindrome II
// ============================================================

// LC 167: 两数之和 II（有序数组）
vector<int> twoSumSorted(vector<int>& numbers, int target) {
    int left = 0, right = numbers.size() - 1;
    while (left < right) {
        int sum = numbers[left] + numbers[right];
        if (sum == target) return {left + 1, right + 1};  // 1-indexed
        else if (sum < target) left++;
        else right--;
    }
    return {};
}

// LC 125: 验证回文串
bool isPalindrome(string s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        while (left < right && !isalnum(s[left])) left++;
        while (left < right && !isalnum(s[right])) right--;
        if (tolower(s[left]) != tolower(s[right])) return false;
        left++; right--;
    }
    return true;
}

// LC 680: 验证回文串 II（最多删一个字符）
bool validPalindrome(string s) {
    auto check = [&](int l, int r) {
        while (l < r) {
            if (s[l] != s[r]) return false;
            l++; r--;
        }
        return true;
    };
    int left = 0, right = s.size() - 1;
    while (left < right) {
        if (s[left] != s[right])
            return check(left + 1, right) || check(left, right - 1);
        left++; right--;
    }
    return true;
}

void demo_collision_pointers() {
    cout << "\n===== Demo 1: 对撞指针 =====" << endl;

    // LC 167
    cout << "\n--- LC 167: 两数之和 II ---" << endl;
    vector<int> nums1 = {2, 7, 11, 15};
    int target1 = 9;
    printVec(nums1, "numbers");
    cout << "target = " << target1 << endl;
    auto res1 = twoSumSorted(nums1, target1);
    printVec(res1, "结果 (1-indexed)");  // [1, 2]

    vector<int> nums2 = {2, 3, 4};
    int target2 = 6;
    printVec(nums2, "numbers");
    cout << "target = " << target2 << endl;
    auto res2 = twoSumSorted(nums2, target2);
    printVec(res2, "结果 (1-indexed)");  // [1, 3]

    // LC 125
    cout << "\n--- LC 125: 验证回文串 ---" << endl;
    cout << "\"A man, a plan, a canal: Panama\" → "
         << (isPalindrome("A man, a plan, a canal: Panama") ? "true" : "false") << endl;  // true
    cout << "\"race a car\" → "
         << (isPalindrome("race a car") ? "true" : "false") << endl;  // false

    // LC 680
    cout << "\n--- LC 680: 验证回文串 II ---" << endl;
    cout << "\"abca\" → "
         << (validPalindrome("abca") ? "true" : "false") << endl;   // true (删 b 或 c)
    cout << "\"abc\" → "
         << (validPalindrome("abc") ? "true" : "false") << endl;    // false
    cout << "\"deeee\" → "
         << (validPalindrome("deeee") ? "true" : "false") << endl;  // true (删 d)
}

// ============================================================
//  Demo 2: 快慢指针 — 数组原地操作
//  LC 26: Remove Duplicates
//  LC 27: Remove Element
//  LC 283: Move Zeroes
// ============================================================

// LC 26: 删除有序数组中的重复项
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;
    for (int fast = 1; fast < (int)nums.size(); fast++) {
        if (nums[fast] != nums[slow]) {
            slow++;
            nums[slow] = nums[fast];
        }
    }
    return slow + 1;
}

// LC 80: 删除有序数组中的重复项 II（保留最多两个）
int removeDuplicatesII(vector<int>& nums) {
    if ((int)nums.size() <= 2) return nums.size();
    int slow = 2;
    for (int fast = 2; fast < (int)nums.size(); fast++) {
        if (nums[fast] != nums[slow - 2]) {
            nums[slow] = nums[fast];
            slow++;
        }
    }
    return slow;
}

// LC 27: 移除元素
int removeElement(vector<int>& nums, int val) {
    int slow = 0;
    for (int fast = 0; fast < (int)nums.size(); fast++) {
        if (nums[fast] != val) {
            nums[slow] = nums[fast];
            slow++;
        }
    }
    return slow;
}

// LC 283: 移动零
void moveZeroes(vector<int>& nums) {
    int slow = 0;
    for (int fast = 0; fast < (int)nums.size(); fast++) {
        if (nums[fast] != 0) {
            swap(nums[slow], nums[fast]);
            slow++;
        }
    }
}

void demo_fast_slow_array() {
    cout << "\n===== Demo 2: 快慢指针（数组原地操作）=====" << endl;

    // LC 26
    cout << "\n--- LC 26: 删除有序数组重复项 ---" << endl;
    vector<int> a1 = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    printVec(a1, "原数组");
    int len1 = removeDuplicates(a1);
    cout << "去重后长度: " << len1 << endl;  // 5
    a1.resize(len1);
    printVec(a1, "去重结果");  // [0, 1, 2, 3, 4]

    // LC 80
    cout << "\n--- LC 80: 删除重复项 II（保留≤2个）---" << endl;
    vector<int> a2 = {1, 1, 1, 2, 2, 3};
    printVec(a2, "原数组");
    int len2 = removeDuplicatesII(a2);
    cout << "保留后长度: " << len2 << endl;  // 5
    a2.resize(len2);
    printVec(a2, "结果");  // [1, 1, 2, 2, 3]

    // LC 27
    cout << "\n--- LC 27: 移除元素 ---" << endl;
    vector<int> a3 = {3, 2, 2, 3};
    int val = 3;
    printVec(a3, "原数组");
    cout << "移除 " << val << endl;
    int len3 = removeElement(a3, val);
    a3.resize(len3);
    printVec(a3, "结果");  // [2, 2]

    // LC 283
    cout << "\n--- LC 283: 移动零 ---" << endl;
    vector<int> a4 = {0, 1, 0, 3, 12};
    printVec(a4, "原数组");
    moveZeroes(a4);
    printVec(a4, "移动零后");  // [1, 3, 12, 0, 0]
}

// ============================================================
//  Demo 3: 三数之和 & 四数之和
//  LC 15: 3Sum
//  LC 18: 4Sum
// ============================================================

// LC 15: 三数之和
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;

    for (int i = 0; i < (int)nums.size() - 2; i++) {
        if (nums[i] > 0) break;                            // 剪枝
        if (i > 0 && nums[i] == nums[i - 1]) continue;     // 去重

        int left = i + 1, right = nums.size() - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                while (left < right && nums[left] == nums[left + 1]) left++;
                while (left < right && nums[right] == nums[right - 1]) right--;
                left++; right--;
            } else if (sum < 0) {
                left++;
            } else {
                right--;
            }
        }
    }
    return result;
}

// LC 18: 四数之和
vector<vector<int>> fourSum(vector<int>& nums, int target) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    int n = nums.size();

    for (int i = 0; i < n - 3; i++) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;
        for (int j = i + 1; j < n - 2; j++) {
            if (j > i + 1 && nums[j] == nums[j - 1]) continue;

            int left = j + 1, right = n - 1;
            while (left < right) {
                long sum = (long)nums[i] + nums[j] + nums[left] + nums[right];
                if (sum == target) {
                    result.push_back({nums[i], nums[j], nums[left], nums[right]});
                    while (left < right && nums[left] == nums[left + 1]) left++;
                    while (left < right && nums[right] == nums[right - 1]) right--;
                    left++; right--;
                } else if (sum < target) {
                    left++;
                } else {
                    right--;
                }
            }
        }
    }
    return result;
}

void demo_n_sum() {
    cout << "\n===== Demo 3: N 数之和 =====" << endl;

    // LC 15
    cout << "\n--- LC 15: 三数之和 ---" << endl;
    vector<int> nums1 = {-1, 0, 1, 2, -1, -4};
    printVec(nums1, "nums");
    auto res1 = threeSum(nums1);
    printVecVec(res1, "三元组（和=0）");
    // [-1,-1,2], [-1,0,1]

    // LC 18
    cout << "\n--- LC 18: 四数之和 ---" << endl;
    vector<int> nums2 = {1, 0, -1, 0, -2, 2};
    int target = 0;
    printVec(nums2, "nums");
    cout << "target = " << target << endl;
    auto res2 = fourSum(nums2, target);
    printVecVec(res2, "四元组（和=0）");
    // [-2,-1,1,2], [-2,0,0,2], [-1,0,0,1]
}

// ============================================================
//  Demo 4: 盛水问题
//  LC 11: Container With Most Water
//  LC 42: Trapping Rain Water
// ============================================================

// LC 11: 盛最多水的容器
int maxArea(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int maxWater = 0;
    while (left < right) {
        int w = right - left;
        int h = min(height[left], height[right]);
        maxWater = max(maxWater, w * h);
        if (height[left] < height[right])
            left++;
        else
            right--;
    }
    return maxWater;
}

// LC 42: 接雨水（双指针）
int trap(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int leftMax = 0, rightMax = 0;
    int water = 0;

    while (left < right) {
        if (height[left] < height[right]) {
            leftMax = max(leftMax, height[left]);
            water += leftMax - height[left];
            left++;
        } else {
            rightMax = max(rightMax, height[right]);
            water += rightMax - height[right];
            right--;
        }
    }
    return water;
}

void demo_water() {
    cout << "\n===== Demo 4: 盛水问题 =====" << endl;

    // LC 11
    cout << "\n--- LC 11: 盛最多水的容器 ---" << endl;
    vector<int> h1 = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    printVec(h1, "height");
    cout << "最大水量: " << maxArea(h1) << endl;  // 49
    cout << "解释: 选 h[1]=8 和 h[8]=7，宽 7，高 min(8,7)=7 → 7*7=49" << endl;

    // LC 42
    cout << "\n--- LC 42: 接雨水 ---" << endl;
    vector<int> h2 = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    printVec(h2, "height");
    cout << "接水量: " << trap(h2) << endl;  // 6

    vector<int> h3 = {4, 2, 0, 3, 2, 5};
    printVec(h3, "height");
    cout << "接水量: " << trap(h3) << endl;  // 9

    cout << "\n双指针原理:" << endl;
    cout << "  左指针较矮 → 水量由 leftMax 决定" << endl;
    cout << "  右指针较矮 → 水量由 rightMax 决定" << endl;
    cout << "  每步累加 Max - height[i]" << endl;
}

// ============================================================
//  Demo 5: 合并有序数组 & 颜色分类
//  LC 88: Merge Sorted Array
//  LC 75: Sort Colors
// ============================================================

// LC 88: 合并两个有序数组
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    int p1 = m - 1, p2 = n - 1, p = m + n - 1;
    while (p1 >= 0 && p2 >= 0) {
        if (nums1[p1] > nums2[p2])
            nums1[p--] = nums1[p1--];
        else
            nums1[p--] = nums2[p2--];
    }
    while (p2 >= 0)
        nums1[p--] = nums2[p2--];
}

// LC 75: 颜色分类（荷兰国旗问题）
void sortColors(vector<int>& nums) {
    int lo = 0, mid = 0, hi = nums.size() - 1;
    while (mid <= hi) {
        if (nums[mid] == 0) {
            swap(nums[lo], nums[mid]);
            lo++; mid++;
        } else if (nums[mid] == 1) {
            mid++;
        } else {
            swap(nums[mid], nums[hi]);
            hi--;
            // mid 不动！交换过来的可能是 0
        }
    }
}

void demo_merge_sort_colors() {
    cout << "\n===== Demo 5: 合并有序数组 & 颜色分类 =====" << endl;

    // LC 88
    cout << "\n--- LC 88: 合并两个有序数组 ---" << endl;
    vector<int> n1 = {1, 2, 3, 0, 0, 0};
    vector<int> n2 = {2, 5, 6};
    cout << "nums1 = [1,2,3,0,0,0], m=3" << endl;
    cout << "nums2 = [2,5,6], n=3" << endl;
    merge(n1, 3, n2, 3);
    printVec(n1, "合并后");  // [1, 2, 2, 3, 5, 6]

    // LC 75
    cout << "\n--- LC 75: 颜色分类 ---" << endl;
    vector<int> colors = {2, 0, 2, 1, 1, 0};
    printVec(colors, "原数组");
    sortColors(colors);
    printVec(colors, "分类后");  // [0, 0, 1, 1, 2, 2]

    cout << "\n荷兰国旗三指针原理:" << endl;
    cout << "  [0..lo-1] = 0 (红区)" << endl;
    cout << "  [lo..mid-1] = 1 (白区)" << endl;
    cout << "  [mid..hi] = 待处理" << endl;
    cout << "  [hi+1..n-1] = 2 (蓝区)" << endl;
}

// ============================================================
//  Demo 6: 链表双指针
//  LC 141: Linked List Cycle
//  LC 142: Linked List Cycle II
//  LC 876: Middle of the Linked List
//  LC 160: Intersection of Two Linked Lists
// ============================================================

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v) : val(v), next(nullptr) {}
};

// LC 141: 环形链表
bool hasCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

// LC 142: 环形链表 II（找入环节点）
ListNode* detectCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            ListNode* p = head;
            while (p != slow) {
                p = p->next;
                slow = slow->next;
            }
            return p;
        }
    }
    return nullptr;
}

// LC 876: 链表的中间结点
ListNode* middleNode(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// LC 160: 相交链表
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    ListNode *pA = headA, *pB = headB;
    while (pA != pB) {
        pA = pA ? pA->next : headB;
        pB = pB ? pB->next : headA;
    }
    return pA;
}

void demo_linked_list() {
    cout << "\n===== Demo 6: 链表双指针 =====" << endl;

    // 构建链表: 1→2→3→4→5
    ListNode* n1 = new ListNode(1);
    ListNode* n2 = new ListNode(2);
    ListNode* n3 = new ListNode(3);
    ListNode* n4 = new ListNode(4);
    ListNode* n5 = new ListNode(5);
    n1->next = n2; n2->next = n3; n3->next = n4; n4->next = n5;

    // LC 876: 中间结点
    cout << "\n--- LC 876: 链表的中间结点 ---" << endl;
    cout << "链表: 1→2→3→4→5" << endl;
    ListNode* mid = middleNode(n1);
    cout << "中间结点: " << mid->val << endl;  // 3

    // LC 141: 无环
    cout << "\n--- LC 141: 环形链表 ---" << endl;
    cout << "1→2→3→4→5 有环? " << (hasCycle(n1) ? "true" : "false") << endl;  // false

    // 制造环: 5→3
    n5->next = n3;
    cout << "1→2→3→4→5→3(环) 有环? " << (hasCycle(n1) ? "true" : "false") << endl;  // true

    // LC 142: 找入环节点
    cout << "\n--- LC 142: 环形链表 II ---" << endl;
    ListNode* entry = detectCycle(n1);
    cout << "入环节点: " << (entry ? to_string(entry->val) : "null") << endl;  // 3

    // 断开环
    n5->next = nullptr;

    // LC 160: 相交链表
    cout << "\n--- LC 160: 相交链表 ---" << endl;
    // A: 10→20→n3→n4→n5
    // B: 30→n3→n4→n5
    ListNode* a1 = new ListNode(10);
    ListNode* a2 = new ListNode(20);
    ListNode* b1 = new ListNode(30);
    a1->next = a2; a2->next = n3;
    b1->next = n3;
    cout << "A: 10→20→3→4→5" << endl;
    cout << "B: 30→3→4→5" << endl;
    ListNode* inter = getIntersectionNode(a1, b1);
    cout << "交点: " << (inter ? to_string(inter->val) : "null") << endl;  // 3

    cout << "\n原理: pA 走完 A 走 B, pB 走完 B 走 A" << endl;
    cout << "  pA: 10→20→3→4→5→30→3 (长度 a+c+b)" << endl;
    cout << "  pB: 30→3→4→5→10→20→3 (长度 b+c+a)" << endl;
    cout << "  同时到达交点 3!" << endl;

    // 清理
    delete a1; delete a2; delete b1;
    delete n1; delete n2; delete n3; delete n4; delete n5;
}

// ============================================================
//  Demo 7: 字符串双指针
//  LC 344: Reverse String
//  LC 345: Reverse Vowels
//  LC 392: Is Subsequence
// ============================================================

// LC 344: 反转字符串
void reverseString(vector<char>& s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        swap(s[left], s[right]);
        left++; right--;
    }
}

// LC 345: 反转字符串中的元音字母
string reverseVowels(string s) {
    auto isVowel = [](char c) {
        c = tolower(c);
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
    };
    int left = 0, right = s.size() - 1;
    while (left < right) {
        while (left < right && !isVowel(s[left])) left++;
        while (left < right && !isVowel(s[right])) right--;
        swap(s[left], s[right]);
        left++; right--;
    }
    return s;
}

// LC 392: 判断子序列
bool isSubsequence(string s, string t) {
    int i = 0, j = 0;
    while (i < (int)s.size() && j < (int)t.size()) {
        if (s[i] == t[j]) i++;
        j++;
    }
    return i == (int)s.size();
}

void demo_string_pointers() {
    cout << "\n===== Demo 7: 字符串双指针 =====" << endl;

    // LC 344
    cout << "\n--- LC 344: 反转字符串 ---" << endl;
    vector<char> s1 = {'h', 'e', 'l', 'l', 'o'};
    cout << "原: ";
    for (char c : s1) cout << c;
    cout << endl;
    reverseString(s1);
    cout << "反: ";
    for (char c : s1) cout << c;
    cout << endl;  // olleh

    // LC 345
    cout << "\n--- LC 345: 反转元音字母 ---" << endl;
    cout << "\"hello\" → \"" << reverseVowels("hello") << "\"" << endl;     // holle
    cout << "\"leetcode\" → \"" << reverseVowels("leetcode") << "\"" << endl; // leotcede

    // LC 392
    cout << "\n--- LC 392: 判断子序列 ---" << endl;
    cout << "\"abc\" 是 \"ahbgdc\" 的子序列? "
         << (isSubsequence("abc", "ahbgdc") ? "true" : "false") << endl;     // true
    cout << "\"axc\" 是 \"ahbgdc\" 的子序列? "
         << (isSubsequence("axc", "ahbgdc") ? "true" : "false") << endl;     // false
}

// ============================================================
//  Demo 8: 综合演练 — 接雨水详解 & 压缩字符串
//  LC 42: Trapping Rain Water（前后缀法 对比理解）
//  LC 443: String Compression
// ============================================================

// LC 42: 接雨水（前后缀数组法 — 更好理解）
int trap_prefix(vector<int>& height) {
    int n = height.size();
    if (n == 0) return 0;
    vector<int> leftMax(n), rightMax(n);

    leftMax[0] = height[0];
    for (int i = 1; i < n; i++)
        leftMax[i] = max(leftMax[i - 1], height[i]);

    rightMax[n - 1] = height[n - 1];
    for (int i = n - 2; i >= 0; i--)
        rightMax[i] = max(rightMax[i + 1], height[i]);

    int water = 0;
    for (int i = 0; i < n; i++)
        water += min(leftMax[i], rightMax[i]) - height[i];

    return water;
}

// LC 443: 压缩字符串
int compress(vector<char>& chars) {
    int write = 0, read = 0;
    while (read < (int)chars.size()) {
        char ch = chars[read];
        int count = 0;
        while (read < (int)chars.size() && chars[read] == ch) {
            read++;
            count++;
        }
        chars[write++] = ch;
        if (count > 1) {
            string cnt = to_string(count);
            for (char c : cnt) chars[write++] = c;
        }
    }
    return write;
}

void demo_advanced() {
    cout << "\n===== Demo 8: 综合演练 =====" << endl;

    // LC 42 前后缀法
    cout << "\n--- LC 42: 接雨水（前后缀法）---" << endl;
    vector<int> h = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    printVec(h, "height");
    cout << "接水量: " << trap_prefix(h) << endl;  // 6

    cout << "\n前后缀法详解:" << endl;
    cout << "  每个位置的水 = min(左边最高, 右边最高) - 自身高度" << endl;
    cout << "  leftMax:  [0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3]" << endl;
    cout << "  rightMax: [3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1]" << endl;
    cout << "  water[i]: [0, 0, 1, 0, 1, 2, 1, 0, 0, 1, 0, 0] 总和=6" << endl;

    // LC 443
    cout << "\n--- LC 443: 压缩字符串 ---" << endl;
    vector<char> chars = {'a', 'a', 'b', 'b', 'c', 'c', 'c'};
    cout << "原: ";
    for (char c : chars) cout << c;
    cout << endl;
    int newLen = compress(chars);
    cout << "压缩后 (长度=" << newLen << "): ";
    for (int i = 0; i < newLen; i++) cout << chars[i];
    cout << endl;  // a2b2c3

    vector<char> chars2 = {'a', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'};
    cout << "原: ";
    for (char c : chars2) cout << c;
    cout << endl;
    int newLen2 = compress(chars2);
    cout << "压缩后 (长度=" << newLen2 << "): ";
    for (int i = 0; i < newLen2; i++) cout << chars2[i];
    cout << endl;  // ab12
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题五：双指针 Two Pointers 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_collision_pointers();    // Demo 1: 对撞指针
    demo_fast_slow_array();      // Demo 2: 快慢指针（数组）
    demo_n_sum();                // Demo 3: N 数之和
    demo_water();                // Demo 4: 盛水问题
    demo_merge_sort_colors();    // Demo 5: 合并 & 颜色分类
    demo_linked_list();          // Demo 6: 链表双指针
    demo_string_pointers();      // Demo 7: 字符串双指针
    demo_advanced();             // Demo 8: 综合演练

    cout << "\n============================================" << endl;
    cout << "  所有双指针 Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 11, 15, 18, 26, 27, 42," << endl;
    cout << "  75, 80, 88, 125, 141, 142, 160, 167," << endl;
    cout << "  283, 344, 345, 392, 443, 680, 876" << endl;
    cout << "============================================" << endl;

    return 0;
}
