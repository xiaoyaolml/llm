/**
 * ============================================================================
 *  专题二：链表（Linked List）— 完整示例代码
 *  配合 knowlege_details_2_linkedlist.md 食用
 *  编译: g++ -std=c++17 -o knowlege_details_2_linkedlist knowlege_details_2_linkedlist.cpp
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>       // priority_queue
#include <list>        // std::list (双向链表)
#include <cassert>
#include <functional>
using namespace std;

// ============================================================================
//  链表节点定义（LeetCode 标准）
// ============================================================================

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* n) : val(x), next(n) {}
};

// ============================================================================
//  辅助函数：创建/打印/转换/释放链表
// ============================================================================

// 从数组构建链表（尾插法）
ListNode* buildList(const vector<int>& arr) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (int val : arr) {
        tail->next = new ListNode(val);
        tail = tail->next;
    }
    return dummy.next;
}

// 打印链表
void printList(ListNode* head, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    while (head) {
        cout << head->val;
        if (head->next) cout << " → ";
        head = head->next;
    }
    cout << " → null" << endl;
}

// 链表转数组（便于验证）
vector<int> toVector(ListNode* head) {
    vector<int> res;
    while (head) {
        res.push_back(head->val);
        head = head->next;
    }
    return res;
}

// 释放链表内存
void freeList(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

// 打印数组
void printVec(const vector<int>& v, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    cout << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        if (i > 0) cout << ", ";
        cout << v[i];
    }
    cout << "]" << endl;
}

// ============================================================================
//  第 1 章：指针基础演示
// ============================================================================

void demo_pointer_basics() {
    cout << "\n========== 1. 指针基础 ==========\n";

    // --- 1.1 取地址与解引用 ---
    int a = 42;
    int* p = &a;         // p 存储 a 的地址

    cout << "a 的值: " << a << endl;
    cout << "a 的地址: " << &a << endl;
    cout << "p 的值 (=a的地址): " << p << endl;
    cout << "*p (解引用): " << *p << endl;   // 42

    // 通过指针修改变量
    *p = 100;
    cout << "修改 *p=100 后，a = " << a << endl;  // 100

    // --- 1.2 指针运算 ---
    int arr[] = {10, 20, 30, 40, 50};
    int* q = arr;        // 数组名就是首元素地址

    cout << "\n数组指针运算:" << endl;
    cout << "*q = " << *q << endl;        // 10
    cout << "*(q+1) = " << *(q+1) << endl;  // 20
    cout << "*(q+2) = " << *(q+2) << endl;  // 30

    q += 2;   // q 现在指向 arr[2]
    cout << "q += 2 后，*q = " << *q << endl;  // 30

    // --- 1.3 指针的大小 ---
    cout << "\nsizeof(int*) = " << sizeof(int*) << " 字节" << endl;
    cout << "sizeof(char*) = " << sizeof(char*) << " 字节" << endl;
    cout << "sizeof(double*) = " << sizeof(double*) << " 字节" << endl;
    // 在 64 位系统上全部是 8 字节

    // --- 1.4 nullptr ---
    int* null_ptr = nullptr;
    cout << "\nnullptr 检查: " << (null_ptr == nullptr ? "是空指针" : "非空") << endl;
    // *null_ptr;  // ❌ 千万不要解引用空指针！
}

// ============================================================================
//  第 2 章：动态内存与结构体指针
// ============================================================================

void demo_dynamic_memory() {
    cout << "\n========== 2. 动态内存与结构体指针 ==========\n";

    // --- new / delete ---
    int* p = new int(42);
    cout << "new int(42): *p = " << *p << endl;
    delete p;
    p = nullptr;   // 好习惯

    // --- new[] / delete[] ---
    int* arr = new int[5]{10, 20, 30, 40, 50};
    cout << "new int[5]: ";
    for (int i = 0; i < 5; i++) cout << arr[i] << " ";
    cout << endl;
    delete[] arr;   // ⚠️ 数组必须用 delete[]
    arr = nullptr;

    // --- 结构体指针与 -> ---
    ListNode* node = new ListNode(42);
    cout << "\nListNode: node->val = " << node->val << endl;
    cout << "ListNode: node->next = " << node->next << " (nullptr)" << endl;

    // -> 等价于 (*node).val
    cout << "(*node).val = " << (*node).val << endl;

    // 链接两个节点
    ListNode* node2 = new ListNode(99);
    node->next = node2;
    cout << "node->next->val = " << node->next->val << endl;  // 99

    delete node2;
    delete node;

    // --- 值传递 vs 指针传递 ---
    int x = 10;
    auto changeByValue = [](int n) { n = 999; };
    auto changeByPointer = [](int* p) { *p = 999; };

    changeByValue(x);
    cout << "\n值传递后 x = " << x << " (没变)" << endl;

    changeByPointer(&x);
    cout << "指针传递后 x = " << x << " (变了!)" << endl;
}

// ============================================================================
//  第 3 章：链表基本操作（创建/插入/删除/长度）
// ============================================================================

void demo_list_basic_ops() {
    cout << "\n========== 3. 链表基本操作 ==========\n";

    // --- 创建链表 ---
    ListNode* list1 = buildList({1, 2, 3, 4, 5});
    printList(list1, "创建链表");

    // --- 链表长度 ---
    auto getLength = [](ListNode* head) -> int {
        int len = 0;
        while (head) { len++; head = head->next; }
        return len;
    };
    cout << "链表长度: " << getLength(list1) << endl;

    // --- 头插法（逆序） ---
    auto headInsert = [](const vector<int>& arr) -> ListNode* {
        ListNode* head = nullptr;
        for (int val : arr) {
            ListNode* node = new ListNode(val);
            node->next = head;
            head = node;
        }
        return head;
    };
    ListNode* list2 = headInsert({1, 2, 3});
    printList(list2, "头插法{1,2,3}");  // 3 → 2 → 1

    // --- 在节点后插入 ---
    // 在 list1 的第 2 个节点后面插入 99
    ListNode* pos = list1->next;  // 节点 2
    ListNode* newNode = new ListNode(99);
    newNode->next = pos->next;
    pos->next = newNode;
    printList(list1, "在2后插入99");  // 1→2→99→3→4→5

    // --- 删除节点（删除值为 99 的节点） ---
    // 已知前驱是 pos (值为 2)
    ListNode* toDelete = pos->next;
    pos->next = pos->next->next;
    delete toDelete;
    printList(list1, "删除99后");  // 1→2→3→4→5

    freeList(list1);
    freeList(list2);
}

// ============================================================================
//  第 4 章：虚拟头节点（Dummy Node）
// ============================================================================

// LC 203: 移除链表元素
ListNode* removeElements(ListNode* head, int val) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode* prev = &dummy;

    while (prev->next) {
        if (prev->next->val == val) {
            ListNode* toDelete = prev->next;
            prev->next = prev->next->next;
            delete toDelete;
        } else {
            prev = prev->next;
        }
    }
    return dummy.next;
}

// LC 83: 删除排序链表中的重复元素
ListNode* deleteDuplicates(ListNode* head) {
    ListNode* curr = head;
    while (curr && curr->next) {
        if (curr->val == curr->next->val) {
            ListNode* toDelete = curr->next;
            curr->next = curr->next->next;
            delete toDelete;
        } else {
            curr = curr->next;
        }
    }
    return head;
}

// LC 82: 删除排序链表中的所有重复元素（保留 0 次）
ListNode* deleteDuplicates2(ListNode* head) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode* prev = &dummy;

    while (prev->next) {
        ListNode* curr = prev->next;
        // 检查从 curr 开始是否有连续重复
        while (curr->next && curr->val == curr->next->val)
            curr = curr->next;

        if (prev->next == curr) {
            // 没有重复，正常前进
            prev = prev->next;
        } else {
            // 有重复，跳过所有重复节点
            prev->next = curr->next;
            // (简化版不 delete，LeetCode 不检查)
        }
    }
    return dummy.next;
}

void demo_dummy_node() {
    cout << "\n========== 4. 虚拟头节点 ==========\n";

    // LC 203: 移除链表元素
    ListNode* list1 = buildList({1, 2, 6, 3, 4, 5, 6});
    list1 = removeElements(list1, 6);
    printList(list1, "移除6");  // 1→2→3→4→5
    freeList(list1);

    // LC 83: 删除重复 (保留一个)
    ListNode* list2 = buildList({1, 1, 2, 3, 3});
    list2 = deleteDuplicates(list2);
    printList(list2, "去重(保留1个)");  // 1→2→3
    freeList(list2);

    // LC 82: 删除全部重复
    ListNode* list3 = buildList({1, 2, 3, 3, 4, 4, 5});
    list3 = deleteDuplicates2(list3);
    printList(list3, "去重(删全部)");  // 1→2→5
    freeList(list3);
}

// ============================================================================
//  第 5 章：链表反转
// ============================================================================

// LC 206: 反转链表 — 迭代法
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;  // 1. 保存后继
        curr->next = prev;            // 2. 反转指向
        prev = curr;                  // 3. prev 前进
        curr = next;                  // 4. curr 前进
    }
    return prev;
}

// LC 206: 反转链表 — 递归法
ListNode* reverseListRecursive(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode* newHead = reverseListRecursive(head->next);
    head->next->next = head;   // 后继反指
    head->next = nullptr;      // 断开原指向
    return newHead;
}

// LC 92: 反转链表 II（反转 [left, right]）
ListNode* reverseBetween(ListNode* head, int left, int right) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode* pre = &dummy;

    // 找到 left 的前驱
    for (int i = 1; i < left; i++)
        pre = pre->next;

    // 头插法反转 [left, right]
    ListNode* curr = pre->next;
    for (int i = 0; i < right - left; i++) {
        ListNode* next = curr->next;
        curr->next = next->next;
        next->next = pre->next;
        pre->next = next;
    }

    return dummy.next;
}

// LC 25: K 个一组翻转链表
ListNode* reverseKGroup(ListNode* head, int k) {
    // 检查是否有 k 个节点
    ListNode* check = head;
    for (int i = 0; i < k; i++) {
        if (!check) return head;
        check = check->next;
    }

    // 翻转前 k 个
    ListNode* prev = nullptr;
    ListNode* curr = head;
    for (int i = 0; i < k; i++) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    // head 变成了尾部，递归处理后续
    head->next = reverseKGroup(curr, k);
    return prev;
}

void demo_reverse() {
    cout << "\n========== 5. 链表反转 ==========\n";

    // --- 完整反转（迭代）---
    ListNode* list1 = buildList({1, 2, 3, 4, 5});
    printList(list1, "原始");
    list1 = reverseList(list1);
    printList(list1, "反转(迭代)");  // 5→4→3→2→1
    freeList(list1);

    // --- 完整反转（递归）---
    ListNode* list2 = buildList({1, 2, 3, 4, 5});
    list2 = reverseListRecursive(list2);
    printList(list2, "反转(递归)");  // 5→4→3→2→1
    freeList(list2);

    // --- 区间反转 ---
    ListNode* list3 = buildList({1, 2, 3, 4, 5});
    list3 = reverseBetween(list3, 2, 4);
    printList(list3, "反转[2,4]");   // 1→4→3→2→5
    freeList(list3);

    // --- K 个一组翻转 ---
    ListNode* list4 = buildList({1, 2, 3, 4, 5});
    list4 = reverseKGroup(list4, 2);
    printList(list4, "K=2翻转");    // 2→1→4→3→5
    freeList(list4);

    ListNode* list5 = buildList({1, 2, 3, 4, 5, 6});
    list5 = reverseKGroup(list5, 3);
    printList(list5, "K=3翻转");    // 3→2→1→6→5→4
    freeList(list5);
}

// ============================================================================
//  第 6 章：快慢指针
// ============================================================================

// LC 876: 链表的中间结点
ListNode* middleNode(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// LC 141: 环形链表
bool hasCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

// LC 142: 环形链表 II（找入环点）
ListNode* detectCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
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

void demo_fast_slow() {
    cout << "\n========== 6. 快慢指针 ==========\n";

    // --- 找中间节点 ---
    ListNode* list1 = buildList({1, 2, 3, 4, 5});
    cout << "中间节点 {1,2,3,4,5}: " << middleNode(list1)->val << endl;  // 3
    freeList(list1);

    ListNode* list2 = buildList({1, 2, 3, 4, 5, 6});
    cout << "中间节点 {1,2,3,4,5,6}: " << middleNode(list2)->val << endl;  // 4
    freeList(list2);

    // --- 检测环 ---
    // 手动创建环形链表: 1→2→3→4→5→(回到3)
    ListNode* n1 = new ListNode(1);
    ListNode* n2 = new ListNode(2);
    ListNode* n3 = new ListNode(3);
    ListNode* n4 = new ListNode(4);
    ListNode* n5 = new ListNode(5);
    n1->next = n2; n2->next = n3; n3->next = n4;
    n4->next = n5; n5->next = n3;   // 环！

    cout << "有环链表 hasCycle: " << (hasCycle(n1) ? "是" : "否") << endl;

    // 找入环点
    ListNode* entry = detectCycle(n1);
    cout << "入环点: " << (entry ? to_string(entry->val) : "无") << endl;  // 3

    // 断开环再释放（避免内存问题）
    n5->next = nullptr;
    freeList(n1);

    // 无环链表
    ListNode* list3 = buildList({1, 2, 3});
    cout << "无环链表 hasCycle: " << (hasCycle(list3) ? "是" : "否") << endl;
    freeList(list3);
}

// ============================================================================
//  第 7 章：双指针技巧
// ============================================================================

// LC 19: 删除链表的倒数第 N 个结点
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode* fast = &dummy;
    ListNode* slow = &dummy;

    for (int i = 0; i <= n; i++)
        fast = fast->next;

    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    ListNode* toDelete = slow->next;
    slow->next = slow->next->next;
    delete toDelete;
    return dummy.next;
}

// LC 160: 相交链表
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    ListNode* a = headA;
    ListNode* b = headB;
    while (a != b) {
        a = a ? a->next : headB;
        b = b ? b->next : headA;
    }
    return a;
}

// LC 234: 回文链表
bool isPalindromeList(ListNode* head) {
    if (!head || !head->next) return true;

    // 找中点
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 反转后半
    ListNode* second = slow->next;
    slow->next = nullptr;
    ListNode* prev = nullptr;
    while (second) {
        ListNode* next = second->next;
        second->next = prev;
        prev = second;
        second = next;
    }

    // 比较
    ListNode* p1 = head;
    ListNode* p2 = prev;
    bool result = true;
    while (p2) {
        if (p1->val != p2->val) { result = false; break; }
        p1 = p1->next;
        p2 = p2->next;
    }

    // 恢复（可选）并释放
    // ... 省略恢复步骤 ...
    return result;
}

void demo_two_pointers() {
    cout << "\n========== 7. 双指针技巧 ==========\n";

    // --- 删除倒数第 N 个 ---
    ListNode* list1 = buildList({1, 2, 3, 4, 5});
    list1 = removeNthFromEnd(list1, 2);
    printList(list1, "删除倒数第2个");  // 1→2→3→5
    freeList(list1);

    ListNode* list1b = buildList({1});
    list1b = removeNthFromEnd(list1b, 1);
    printList(list1b, "单节点删除倒数第1个");  // null
    freeList(list1b);

    // --- 相交链表 ---
    // 构建: A: 1→2→↘
    //              3→4→5
    //       B:   9→↗
    ListNode* common = buildList({3, 4, 5});
    ListNode* headA = new ListNode(1, new ListNode(2, common));
    ListNode* headB = new ListNode(9, common);

    ListNode* inter = getIntersectionNode(headA, headB);
    cout << "相交节点: " << (inter ? to_string(inter->val) : "无") << endl;  // 3

    // 不能简单 freeList，因为共享节点
    delete headA->next; delete headA;  // 只释放 A 独有的
    delete headB;                      // 只释放 B 独有的
    freeList(common);                  // 释放公共部分

    // --- 回文链表 ---
    ListNode* list3 = buildList({1, 2, 2, 1});
    cout << "回文 {1,2,2,1}: " << (isPalindromeList(list3) ? "是" : "否") << endl;

    ListNode* list4 = buildList({1, 2, 3});
    cout << "回文 {1,2,3}: " << (isPalindromeList(list4) ? "是" : "否") << endl;
}

// ============================================================================
//  第 8 章：合并链表
// ============================================================================

// LC 21: 合并两个有序链表
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (l1 && l2) {
        if (l1->val <= l2->val) {
            tail->next = l1; l1 = l1->next;
        } else {
            tail->next = l2; l2 = l2->next;
        }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;
    return dummy.next;
}

// LC 23: 合并 K 个升序链表
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) {
        return a->val > b->val;  // 小顶堆
    };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);

    for (auto* list : lists) {
        if (list) pq.push(list);
    }

    ListNode dummy(0);
    ListNode* tail = &dummy;
    while (!pq.empty()) {
        ListNode* min = pq.top(); pq.pop();
        tail->next = min;
        tail = tail->next;
        if (min->next) pq.push(min->next);
    }
    return dummy.next;
}

void demo_merge() {
    cout << "\n========== 8. 合并链表 ==========\n";

    // --- 合并两个有序链表 ---
    ListNode* a = buildList({1, 3, 5, 7});
    ListNode* b = buildList({2, 4, 6, 8});
    ListNode* merged = mergeTwoLists(a, b);
    printList(merged, "合并两个有序");  // 1→2→3→4→5→6→7→8
    freeList(merged);

    // --- 合并 K 个升序链表 ---
    vector<ListNode*> lists = {
        buildList({1, 4, 7}),
        buildList({2, 5, 8}),
        buildList({3, 6, 9})
    };
    ListNode* kMerged = mergeKLists(lists);
    printList(kMerged, "合并K=3个");  // 1→2→3→4→5→6→7→8→9
    freeList(kMerged);
}

// ============================================================================
//  第 9 章：链表排序
// ============================================================================

// 前向声明
ListNode* mergeSort(ListNode* head);

// LC 148: 排序链表（归并排序）
ListNode* sortList(ListNode* head) {
    if (!head || !head->next) return head;

    // 快慢指针找中点
    ListNode* slow = head;
    ListNode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 断开
    ListNode* mid = slow->next;
    slow->next = nullptr;

    // 递归排序两半
    ListNode* left = sortList(head);
    ListNode* right = sortList(mid);

    // 合并
    return mergeTwoLists(left, right);
}

// LC 147: 插入排序链表
ListNode* insertionSortList(ListNode* head) {
    ListNode dummy(0);
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;
        ListNode* prev = &dummy;

        while (prev->next && prev->next->val < curr->val)
            prev = prev->next;

        curr->next = prev->next;
        prev->next = curr;
        curr = next;
    }
    return dummy.next;
}

void demo_sort() {
    cout << "\n========== 9. 链表排序 ==========\n";

    // --- 归并排序 ---
    ListNode* list1 = buildList({4, 2, 1, 3, 5});
    list1 = sortList(list1);
    printList(list1, "归并排序");  // 1→2→3→4→5
    freeList(list1);

    // --- 插入排序 ---
    ListNode* list2 = buildList({4, 2, 1, 3});
    list2 = insertionSortList(list2);
    printList(list2, "插入排序");  // 1→2→3→4
    freeList(list2);
}

// ============================================================================
//  第 10 章：经典 LeetCode 题
// ============================================================================

// LC 2: 两数相加
// 输入: l1 = 2→4→3 (342), l2 = 5→6→4 (465)
// 输出: 7→0→8 (807)
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    int carry = 0;

    while (l1 || l2 || carry) {
        int sum = carry;
        if (l1) { sum += l1->val; l1 = l1->next; }
        if (l2) { sum += l2->val; l2 = l2->next; }

        carry = sum / 10;
        tail->next = new ListNode(sum % 10);
        tail = tail->next;
    }
    return dummy.next;
}

// LC 328: 奇偶链表
// 把所有奇数位置的节点排在前面，偶数位置的排在后面
ListNode* oddEvenList(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* odd = head;
    ListNode* even = head->next;
    ListNode* evenHead = even;

    while (even && even->next) {
        odd->next = even->next;     // 奇数连奇数
        odd = odd->next;
        even->next = odd->next;     // 偶数连偶数
        even = even->next;
    }

    odd->next = evenHead;           // 奇数尾接偶数头
    return head;
}

// LC 86: 分隔链表
// 把所有小于 x 的节点排在前面，>= x 的排在后面
ListNode* partition(ListNode* head, int x) {
    ListNode smallDummy(0), bigDummy(0);
    ListNode* small = &smallDummy;
    ListNode* big = &bigDummy;

    while (head) {
        if (head->val < x) {
            small->next = head;
            small = small->next;
        } else {
            big->next = head;
            big = big->next;
        }
        head = head->next;
    }

    big->next = nullptr;             // 大链表尾置空
    small->next = bigDummy.next;     // 小链表尾接大链表头
    return smallDummy.next;
}

// LC 143: 重排链表
// 1→2→3→4→5 → 1→5→2→4→3
void reorderList(ListNode* head) {
    if (!head || !head->next) return;

    // 1. 找中点
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 反转后半
    ListNode* second = slow->next;
    slow->next = nullptr;
    ListNode* prev = nullptr;
    while (second) {
        ListNode* next = second->next;
        second->next = prev;
        prev = second;
        second = next;
    }

    // 3. 交替合并
    ListNode* first = head;
    second = prev;
    while (second) {
        ListNode* tmp1 = first->next;
        ListNode* tmp2 = second->next;
        first->next = second;
        second->next = tmp1;
        first = tmp1;
        second = tmp2;
    }
}

void demo_classic_problems() {
    cout << "\n========== 10. 经典链表题 ==========\n";

    // LC 2: 两数相加
    ListNode* l1 = buildList({2, 4, 3});     // 342
    ListNode* l2 = buildList({5, 6, 4});     // 465
    ListNode* sum = addTwoNumbers(l1, l2);
    printList(sum, "两数相加 342+465");       // 7→0→8 (807)
    freeList(l1); freeList(l2); freeList(sum);

    // LC 328: 奇偶链表
    ListNode* list1 = buildList({1, 2, 3, 4, 5});
    list1 = oddEvenList(list1);
    printList(list1, "奇偶链表");  // 1→3→5→2→4
    freeList(list1);

    // LC 86: 分隔链表
    ListNode* list2 = buildList({1, 4, 3, 2, 5, 2});
    list2 = partition(list2, 3);
    printList(list2, "分隔(x=3)");  // 1→2→2→4→3→5
    freeList(list2);

    // LC 143: 重排链表
    ListNode* list3 = buildList({1, 2, 3, 4, 5});
    reorderList(list3);
    printList(list3, "重排链表");  // 1→5→2→4→3
    freeList(list3);
}

// ============================================================================
//  第 11 章：LRU Cache（双向链表 + 哈希表）
// ============================================================================

class LRUCache {
    int capacity;
    list<pair<int, int>> cache;                           // 双向链表: front=最新, back=最旧
    unordered_map<int, list<pair<int,int>>::iterator> map; // key → 链表迭代器

public:
    LRUCache(int cap) : capacity(cap) {}

    int get(int key) {
        if (map.find(key) == map.end()) return -1;
        auto it = map[key];
        int val = it->second;
        cache.erase(it);
        cache.push_front({key, val});
        map[key] = cache.begin();
        return val;
    }

    void put(int key, int value) {
        if (map.find(key) != map.end()) {
            cache.erase(map[key]);
        } else if ((int)cache.size() >= capacity) {
            int oldKey = cache.back().first;
            cache.pop_back();
            map.erase(oldKey);
        }
        cache.push_front({key, value});
        map[key] = cache.begin();
    }
};

void demo_lru_cache() {
    cout << "\n========== 11. LRU Cache ==========\n";

    LRUCache lru(2);  // 容量为 2

    lru.put(1, 1);
    lru.put(2, 2);
    cout << "get(1) = " << lru.get(1) << endl;  // 1
    lru.put(3, 3);                                // 淘汰 key=2
    cout << "get(2) = " << lru.get(2) << endl;  // -1（已被淘汰）
    lru.put(4, 4);                                // 淘汰 key=1（因为 key=1 比 key=3 更旧？不对，get(1) 后 key=1 是最新的）
    // 实际淘汰 key=3（因为 get(1) 把 1 移到了最前面）
    // 此时缓存: [4, 1]
    cout << "get(1) = " << lru.get(1) << endl;  // 1
    cout << "get(3) = " << lru.get(3) << endl;  // -1
    cout << "get(4) = " << lru.get(4) << endl;  // 4
}

// ============================================================================
//  第 12 章：常见陷阱演示
// ============================================================================

void demo_pitfalls() {
    cout << "\n========== 12. 常见陷阱 ==========\n";

    // --- 陷阱 1：丢失指针 ---
    cout << "陷阱 1: 反转时先保存 next 再修改\n";
    cout << "  ❌ curr->next = prev; curr = curr->next; // next 已被改成 prev!\n";
    cout << "  ✅ ListNode* next = curr->next; curr->next = prev; curr = next;\n\n";

    // --- 陷阱 2：空指针解引用 ---
    cout << "陷阱 2: 访问 ->next 前必须检查非空\n";
    ListNode* empty = nullptr;
    // empty->val;  // ❌ 崩溃!
    if (empty) cout << "非空" << endl;
    else cout << "  ✅ 先检查后访问\n\n";

    // --- 陷阱 3：忘记断链 ---
    cout << "陷阱 3: 分割链表时必须 slow->next = nullptr 断开\n\n";

    // --- 陷阱 4：快慢指针检查条件 ---
    cout << "陷阱 4: 快慢指针\n";
    cout << "  ❌ while (fast->next)     // fast 本身可能是 null\n";
    cout << "  ✅ while (fast && fast->next)\n\n";

    // --- 陷阱 5：delete 后继续使用 ---
    cout << "陷阱 5: delete 后指针变为悬垂指针\n";
    ListNode* node = new ListNode(42);
    delete node;
    node = nullptr;  // ✅ 好习惯
    cout << "  ✅ delete 后立即置 nullptr\n\n";

    // --- 陷阱 6：链表 vs 数组下标 ---
    cout << "陷阱 6: 链表没有下标，不能用 list[i]\n";
    cout << "  ✅ 只能用 curr = curr->next 遍历\n\n";

    // --- 性能建议 ---
    cout << "--- 最佳实践 ---\n";
    cout << "  ✅ 头节点可能变 → 用 dummy\n";
    cout << "  ✅ 要反转 → prev/curr/next 三指针\n";
    cout << "  ✅ 要找中点/环/倒数 → 快慢指针\n";
    cout << "  ✅ 边界测试：空链表、单节点、双节点\n";
    cout << "  ✅ 画图！画图！画图！\n";
}

// ============================================================================
//  主函数：运行所有示例
// ============================================================================

int main() {
    cout << "╔══════════════════════════════════════════╗\n";
    cout << "║  专题二：链表 (Linked List) 完整示例代码   ║\n";
    cout << "╚══════════════════════════════════════════╝\n";

    demo_pointer_basics();        // 1. 指针基础
    demo_dynamic_memory();        // 2. 动态内存
    demo_list_basic_ops();        // 3. 链表基本操作
    demo_dummy_node();            // 4. 虚拟头节点
    demo_reverse();               // 5. 链表反转
    demo_fast_slow();             // 6. 快慢指针
    demo_two_pointers();          // 7. 双指针技巧
    demo_merge();                 // 8. 合并链表
    demo_sort();                  // 9. 链表排序
    demo_classic_problems();      // 10. 经典题
    demo_lru_cache();             // 11. LRU Cache
    demo_pitfalls();              // 12. 陷阱

    cout << "\n✅ 所有链表示例运行完毕！\n";
    return 0;
}
