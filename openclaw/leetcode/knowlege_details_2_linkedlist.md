# 📖 专题二：链表（Linked List）— 从指针零基础到精通

*作者：大胖超 😜 | 配合 leetcode_knowledge_base.md 食用更佳*

---

## 目录

1. [指针基础：理解内存与地址](#1-指针基础理解内存与地址)
2. [指针进阶：动态内存与常见操作](#2-指针进阶动态内存与常见操作)
3. [指针与引用的区别](#3-指针与引用的区别)
4. [智能指针简介](#4-智能指针简介)
5. [链表基本概念](#5-链表基本概念)
6. [单链表：节点定义与基本操作](#6-单链表节点定义与基本操作)
7. [虚拟头节点（Dummy Node）](#7-虚拟头节点dummy-node)
8. [链表遍历与查找](#8-链表遍历与查找)
9. [链表反转（超高频！）](#9-链表反转超高频)
10. [快慢指针](#10-快慢指针)
11. [链表中的双指针技巧](#11-链表中的双指针技巧)
12. [链表排序](#12-链表排序)
13. [双向链表](#13-双向链表)
14. [链表与其他数据结构结合](#14-链表与其他数据结构结合)
15. [经典 LeetCode 链表题精讲](#15-经典-leetcode-链表题精讲)
16. [链表调试技巧](#16-链表调试技巧)
17. [常见陷阱与最佳实践](#17-常见陷阱与最佳实践)

---

# 1. 指针基础：理解内存与地址

> 🎯 链表的核心就是指针。不理解指针，就无法真正掌握链表。

## 1.1 什么是内存？

计算机内存可以想象成一栋超级大楼：
- 每个房间有一个**唯一编号**（地址），从 0 开始
- 每个房间可以存放**一份数据**
- 相邻房间的编号连续

```
地址:    0x1000  0x1004  0x1008  0x100C  ...
内容:    [  42 ] [ 100 ] [  -1 ] [  0  ] ...
         ↑ int a  ↑ int b
```

## 1.2 什么是指针？

**指针 = 变量，只不过它存储的是「另一个变量的地址」**。

```cpp
int a = 42;        // a 是一个 int，值为 42，假设它住在 0x1000
int* p = &a;       // p 是一个指针，值为 0x1000（a 的地址）
```

图解：
```
变量 a:   [  42  ]    地址 0x1000
变量 p:   [0x1000]    地址 0x2000
           ↓
           指向 a
```

## 1.3 指针的两个核心操作

### 取地址 `&`：获取变量的地址

```cpp
int x = 10;
int* ptr = &x;           // ptr = x 的地址
cout << &x;               // 输出 x 的地址：0x7ffc...
cout << ptr;              // 同上
```

### 解引用 `*`：通过地址访问变量的值

```cpp
int x = 10;
int* ptr = &x;
cout << *ptr;             // 输出 10（通过 ptr 找到 x 的值）
*ptr = 20;                // 通过指针修改 x 的值
cout << x;                // 输出 20！
```

> 💡 口诀：`&` 取地址，`*` 解引用。`*` 和 `&` 是一对逆运算。

## 1.4 指针的类型

指针的类型决定了解引用时读取多少字节：

```cpp
int* p;          // 指向 int 的指针，解引用时读 4 字节
char* q;         // 指向 char 的指针，解引用时读 1 字节
double* r;       // 指向 double 的指针，解引用时读 8 字节

// 所有指针本身的大小相同：
// 32 位系统：4 字节
// 64 位系统：8 字节
```

## 1.5 指针运算

```cpp
int arr[] = {10, 20, 30, 40, 50};
int* p = arr;           // p 指向 arr[0]

cout << *p;             // 10
cout << *(p + 1);       // 20（p 偏移一个 int = 4 字节）
cout << *(p + 2);       // 30

p++;                    // p 现在指向 arr[1]
cout << *p;             // 20

// 两个指针相减 = 之间元素的个数
int* q = &arr[4];
cout << (q - p);        // 3
```

> 指针 +1 不是地址 +1，而是 **地址 + sizeof(类型)** ！

## 1.6 nullptr 与空指针

```cpp
int* p = nullptr;       // C++11 推荐用 nullptr
// int* p = NULL;       // C 风格，不推荐
// int* p = 0;          // 也是空指针，不推荐

// ⚠️ 永远不要解引用空指针！
if (p != nullptr) {
    cout << *p;          // 安全
}
// *p;   // ❌ 段错误！程序崩溃！

// 在链表中，nullptr 代表「链表的结束」
```

---

# 2. 指针进阶：动态内存与常见操作

## 2.1 栈内存 vs 堆内存

```
┌──────────────────────────────────────┐
│           内存布局                    │
├──────────────────────────────────────┤
│  栈 (Stack)                          │
│  - 自动管理，函数退出时自动释放        │
│  - int x = 10;                       │
│  - 大小有限（通常 1-8 MB）            │
├──────────────────────────────────────┤
│  堆 (Heap)                           │
│  - 手动管理，需要 new/delete          │
│  - int* p = new int(10);             │
│  - 大小几乎无限                       │
│  - ⚠️ 忘记释放 = 内存泄漏！            │
└──────────────────────────────────────┘
```

## 2.2 new 和 delete

```cpp
// 分配单个变量
int* p = new int(42);       // 在堆上创建一个 int，值为 42
cout << *p;                  // 42
delete p;                    // 释放内存
p = nullptr;                 // 好习惯：释放后置 nullptr

// 分配数组
int* arr = new int[5]{1, 2, 3, 4, 5};
cout << arr[2];              // 3
delete[] arr;                // ⚠️ 数组要用 delete[]！
arr = nullptr;

// 分配链表节点（最常用！）
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* node = new ListNode(42);   // 创建一个值为 42 的节点
delete node;                          // 用完释放
```

## 2.3 指针的指针 (`**`)

```cpp
int val = 42;
int* p = &val;           // p 指向 val
int** pp = &p;           // pp 指向 p

cout << **pp;            // 42（两次解引用）

// 链表中的应用：修改头指针
void insertHead(ListNode** headRef, int val) {
    ListNode* newNode = new ListNode(val);
    newNode->next = *headRef;
    *headRef = newNode;
}
// 调用方式：insertHead(&head, 42);
```

## 2.4 结构体指针与 `->` 操作符

```cpp
struct ListNode {
    int val;
    ListNode* next;
};

ListNode node;
node.val = 10;                // 对象用 .

ListNode* p = &node;
p->val = 20;                  // 指针用 ->（等价于 (*p).val）
p->next = nullptr;

// 在链表操作中，几乎总是通过指针操作节点：
// curr->next   比   (*curr).next   更常用
```

## 2.5 函数中的指针参数

```cpp
// 值传递：修改不影响外部
void changeVal(int x) {
    x = 100;    // 改的是副本
}

// 指针传递：通过地址修改外部变量
void changeRef(int* p) {
    *p = 100;   // 通过指针修改原变量
}

// 引用传递：C++ 推荐
void changeRef2(int& x) {
    x = 100;    // 直接修改原变量
}

int a = 42;
changeVal(a);    // a 仍是 42
changeRef(&a);   // a 变成 100
changeRef2(a);   // a 变成 100
```

---

# 3. 指针与引用的区别

| 特性 | 指针 (`*`) | 引用 (`&`) |
|------|-----------|-----------|
| 初始化 | 可以不初始化（危险） | **必须初始化** |
| 是否可为空 | 可以是 `nullptr` | **不可为空** |
| 是否可重新绑定 | 可以指向别的变量 | 一旦绑定，终身不变 |
| 是否可算术运算 | 可以 `p++`、`p+1` | 不可 |
| 语法 | `*p` 解引用 | 像变量一样使用 |
| 链表场景 | 主要用指针（需要 nullptr, 重指向） | 函数参数传递 |

```cpp
int a = 10, b = 20;

// 指针
int* p = &a;      // p 指向 a
p = &b;            // p 可以改指向 b
*p = 30;           // b 变成 30

// 引用
int& ref = a;      // ref 是 a 的别名
ref = 50;          // a 变成 50
// int& ref2;      // ❌ 编译错误，引用必须初始化
```

---

# 4. 智能指针简介

> 实际工程中推荐智能指针，但 LeetCode 题目用裸指针即可。

```cpp
#include <memory>

// unique_ptr：独占所有权，自动释放
{
    auto p = make_unique<ListNode>(42);
    cout << p->val;    // 42
}   // 出作用域自动 delete，不会内存泄漏

// shared_ptr：共享所有权，引用计数归零时释放
{
    auto p1 = make_shared<ListNode>(10);
    auto p2 = p1;      // 引用计数 = 2
    cout << p1.use_count();  // 2
}   // 两个都超出作用域后才释放

// weak_ptr：不增加引用计数，避免循环引用
```

**LeetCode 场景**：题目使用裸指针 `new ListNode(val)`，返回前不需要 `delete`。

---

# 5. 链表基本概念

## 5.1 链表 vs 数组

```
数组（连续内存）：
┌───┬───┬───┬───┬───┐
│ 1 │ 3 │ 5 │ 7 │ 9 │   → O(1) 随机访问
└───┴───┴───┴───┴───┘      O(n) 插入/删除（需移动）

链表（离散内存）：
┌───┬──┐   ┌───┬──┐   ┌───┬──┐
│ 1 │ ─┼──→│ 3 │ ─┼──→│ 5 │╲ │
└───┴──┘   └───┴──┘   └───┴──┘
  节点1       节点2       节点3     → O(n) 查找
                                     O(1) 插入/删除（已知位置）
```

## 5.2 链表的分类

### 单链表

```
head → [1|→] → [2|→] → [3|→] → nullptr
```

每个节点只有一个 `next` 指针，只能正向遍历。

### 双向链表

```
nullptr ← [←|1|→] ⇄ [←|2|→] ⇄ [←|3|→] → nullptr
```

每个节点有 `prev` 和 `next` 两个指针，可双向遍历。

### 循环链表

```
head → [1|→] → [2|→] → [3|→] ──┐
        ↑                       │
        └───────────────────────┘
```

尾节点的 `next` 指向头节点，形成环。

## 5.3 核心操作复杂度

| 操作 | 数组 | 链表 |
|------|------|------|
| 按下标访问 | **O(1)** | O(n) |
| 头部插入 | O(n) | **O(1)** |
| 尾部插入（无尾指针） | O(1) | O(n) |
| 尾部插入（有尾指针） | O(1) | **O(1)** |
| 中间插入（已知位置） | O(n) | **O(1)** |
| 中间删除（已知位置） | O(n) | **O(1)** |
| 查找 | O(n)/O(logn) | O(n) |
| 内存开销 | 小（连续） | 大（指针开销） |
| 缓存友好性 | **好** | 差 |

> 选择依据：频繁插入删除 → 链表，频繁随机访问 → 数组。

---

# 6. 单链表：节点定义与基本操作

## 6.1 节点定义

```cpp
// LeetCode 标准定义
struct ListNode {
    int val;              // 数据域
    ListNode* next;       // 指针域，指向下一个节点

    // 构造函数
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};
```

## 6.2 创建链表

```cpp
// 方法 1: 逐个创建连接
ListNode* node1 = new ListNode(1);
ListNode* node2 = new ListNode(2);
ListNode* node3 = new ListNode(3);
node1->next = node2;
node2->next = node3;
ListNode* head = node1;   // head → 1 → 2 → 3 → nullptr

// 方法 2: 从数组快速构建（刷题常用辅助函数）
ListNode* buildList(vector<int>& arr) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (int val : arr) {
        tail->next = new ListNode(val);
        tail = tail->next;
    }
    return dummy.next;
}

// 方法 3: 一行链式构建
ListNode* head = new ListNode(1, new ListNode(2, new ListNode(3)));
```

## 6.3 头插法 vs 尾插法

```cpp
// 头插法：新节点插到开头，结果是逆序
ListNode* headInsert(vector<int>& arr) {
    ListNode* head = nullptr;
    for (int val : arr) {
        ListNode* node = new ListNode(val);
        node->next = head;    // 新节点指向旧 head
        head = node;           // head 更新为新节点
    }
    return head;
    // arr = {1,2,3} → 链表: 3 → 2 → 1
}

// 尾插法：新节点接到末尾，结果是正序
ListNode* tailInsert(vector<int>& arr) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (int val : arr) {
        tail->next = new ListNode(val);
        tail = tail->next;
    }
    return dummy.next;
    // arr = {1,2,3} → 链表: 1 → 2 → 3
}
```

## 6.4 插入节点（在位置 pos 之后插入）

```cpp
void insertAfter(ListNode* pos, int val) {
    if (!pos) return;
    ListNode* newNode = new ListNode(val);
    newNode->next = pos->next;     // 新节点指向 pos 的后继
    pos->next = newNode;           // pos 指向新节点
}

// 图解：在 B 后面插入 X
// 原始:   A → B → C
// 步骤 1: A → B → C,  X → C      (newNode->next = pos->next)
// 步骤 2: A → B → X → C          (pos->next = newNode)
// ⚠️ 顺序不能反！先连后面，再断前面
```

## 6.5 删除节点

```cpp
// 删除 pos 的下一个节点
void deleteNext(ListNode* pos) {
    if (!pos || !pos->next) return;
    ListNode* toDelete = pos->next;
    pos->next = pos->next->next;    // 跳过被删节点
    delete toDelete;                 // 释放内存
}

// 图解：删除 B（已知前驱 A）
// 原始:   A → B → C
// 步骤 1: toDelete = B
// 步骤 2: A → C        (pos->next = pos->next->next)
// 步骤 3: delete B       (释放内存)
```

## 6.6 链表长度

```cpp
int getLength(ListNode* head) {
    int len = 0;
    while (head) {
        len++;
        head = head->next;
    }
    return len;
}
```

---

# 7. 虚拟头节点（Dummy Node）

> 🎯 链表中最重要的技巧之一！消除对头节点的特殊处理。

## 7.1 为什么需要？

当操作可能会修改头节点时（比如删除头节点），需要额外判断：

```cpp
// ❌ 不用 dummy，需要特判头节点
ListNode* removeElements_bad(ListNode* head, int val) {
    // 特殊处理：头节点就是要删除的
    while (head && head->val == val) {
        ListNode* tmp = head;
        head = head->next;
        delete tmp;
    }
    // 处理中间节点
    ListNode* curr = head;
    while (curr && curr->next) {
        if (curr->next->val == val) {
            ListNode* tmp = curr->next;
            curr->next = curr->next->next;
            delete tmp;
        } else {
            curr = curr->next;
        }
    }
    return head;
}
```

## 7.2 使用 dummy 统一逻辑

```cpp
// ✅ LC 203: 移除链表元素
ListNode* removeElements(ListNode* head, int val) {
    ListNode dummy(0);         // dummy → head → ...
    dummy.next = head;
    ListNode* prev = &dummy;   // prev 从 dummy 开始

    while (prev->next) {
        if (prev->next->val == val) {
            ListNode* toDelete = prev->next;
            prev->next = prev->next->next;
            delete toDelete;
        } else {
            prev = prev->next;
        }
    }
    return dummy.next;          // 返回 dummy 的后继（真正的头）
}
```

## 7.3 dummy 的两种创建方式

```cpp
// 方式 1: 栈上创建（推荐，自动回收）
ListNode dummy(0);
dummy.next = head;
// ...
return dummy.next;

// 方式 2: 堆上创建（需要手动 delete）
ListNode* dummy = new ListNode(0);
dummy->next = head;
// ...
ListNode* result = dummy->next;
delete dummy;
return result;
```

## 7.4 常用 dummy 的场景

| 场景 | 例题 |
|------|------|
| 删除节点 | LC 203, 82, 237 |
| 合并链表 | LC 21, 23 |
| 分割链表 | LC 86, 328 |
| 链表排序 | LC 148 |
| 任何可能修改头节点的操作 | — |

> 📌 记住：**只要链表头可能改变，就用 dummy！**

---

# 8. 链表遍历与查找

## 8.1 基本遍历

```cpp
// 打印链表所有值
void printList(ListNode* head) {
    ListNode* curr = head;
    while (curr) {
        cout << curr->val;
        if (curr->next) cout << " → ";
        curr = curr->next;
    }
    cout << " → nullptr" << endl;
}
// 输出: 1 → 2 → 3 → nullptr
```

## 8.2 查找特定值

```cpp
// 返回第一个值为 target 的节点，不存在返回 nullptr
ListNode* findNode(ListNode* head, int target) {
    while (head) {
        if (head->val == target) return head;
        head = head->next;
    }
    return nullptr;
}
```

## 8.3 获取第 n 个节点（0-indexed）

```cpp
ListNode* getNthNode(ListNode* head, int n) {
    while (head && n > 0) {
        head = head->next;
        n--;
    }
    return head;  // n 超出范围时返回 nullptr
}
```

## 8.4 获取倒数第 n 个节点（双指针）

```cpp
// 快指针先走 n 步，然后两个一起走
ListNode* getNthFromEnd(ListNode* head, int n) {
    ListNode* fast = head;
    ListNode* slow = head;

    // fast 先走 n 步
    for (int i = 0; i < n; i++) {
        if (!fast) return nullptr;
        fast = fast->next;
    }

    // 两个一起走，fast 到末尾时 slow 就是倒数第 n 个
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }
    return slow;
}
```

---

# 9. 链表反转（超高频！）

> 🔥 反转链表是面试和 LeetCode 出现频率最高的链表题型之一。

## 9.1 迭代法反转（推荐默写！）

```cpp
// LC 206: 反转链表
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;   // 1️⃣ 保存后继
        curr->next = prev;             // 2️⃣ 反转指向
        prev = curr;                   // 3️⃣ prev 前进
        curr = next;                   // 4️⃣ curr 前进
    }
    return prev;                       // prev 就是新头
}
```

**逐步图解**：

```
初始:   nullptr    1 → 2 → 3 → nullptr
        prev      curr

Step 1: nullptr ← 1    2 → 3 → nullptr
                prev  curr

Step 2: nullptr ← 1 ← 2    3 → nullptr
                      prev  curr

Step 3: nullptr ← 1 ← 2 ← 3   nullptr
                           prev  curr

返回 prev = 3（新头节点）
结果:   3 → 2 → 1 → nullptr
```

> 💡 三变量模板：**prev, curr, next**，记住四步操作顺序。

## 9.2 递归法反转

```cpp
ListNode* reverseListRecursive(ListNode* head) {
    // 基本情况
    if (!head || !head->next) return head;

    // 递归反转后面的部分
    ListNode* newHead = reverseListRecursive(head->next);

    // 当前节点的后继反指向自己
    head->next->next = head;
    head->next = nullptr;

    return newHead;
}
```

**递归过程图解**：

```
reverseList(1→2→3→null)
  reverseList(2→3→null)
    reverseList(3→null) → 返回 3
  此时: 3→null, head=2
  2->next->next = 2  →  3→2
  2->next = null      →  3→2→null
  返回 3
此时: 3→2→null, head=1
1->next->next = 1  →  3→2→1
1->next = null      →  3→2→1→null
返回 3
```

## 9.3 反转前 N 个节点

```cpp
// 反转链表的前 n 个节点
ListNode* reverseBetween_N(ListNode* head, int n) {
    if (n == 1) return head;

    ListNode* prev = nullptr;
    ListNode* curr = head;
    ListNode* tail = head;  // 反转后 tail 成为尾部

    for (int i = 0; i < n; i++) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    tail->next = curr;  // tail 接上剩余部分
    return prev;
}
```

## 9.4 反转链表的一部分

```cpp
// LC 92: 反转链表 II（反转 [left, right]）
ListNode* reverseBetween(ListNode* head, int left, int right) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode* pre = &dummy;

    // 1. 找到 left 的前驱
    for (int i = 1; i < left; i++)
        pre = pre->next;

    // 2. 反转 [left, right] 这一段
    ListNode* curr = pre->next;
    for (int i = 0; i < right - left; i++) {
        ListNode* next = curr->next;
        curr->next = next->next;
        next->next = pre->next;
        pre->next = next;
    }

    return dummy.next;
}
```

## 9.5 K 个一组翻转

```cpp
// LC 25: K 个一组翻转链表
ListNode* reverseKGroup(ListNode* head, int k) {
    // 1. 先检查是否有 k 个节点
    ListNode* check = head;
    for (int i = 0; i < k; i++) {
        if (!check) return head;   // 不足 k 个，不翻转
        check = check->next;
    }

    // 2. 翻转前 k 个
    ListNode* prev = nullptr;
    ListNode* curr = head;
    for (int i = 0; i < k; i++) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    // 3. head 变成了这段的尾部，递归处理后续
    head->next = reverseKGroup(curr, k);

    return prev;  // prev 是新头
}
```

---

# 10. 快慢指针

> 🎯 链表中最优雅的技巧，利用两个不同速度的指针解决一类问题。

## 10.1 找中间节点

```cpp
// LC 876: 链表的中间结点
ListNode* middleNode(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;           // 慢指针走 1 步
        fast = fast->next->next;     // 快指针走 2 步
    }
    return slow;  // 快到终点时，慢正好在中间
}

// 偶数节点时返回中间偏右的节点：
// 1→2→3→4   → 返回 3
// 1→2→3→4→5 → 返回 3
```

**如果要返回中间偏左的节点**：

```cpp
ListNode* middleNodeLeft(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
    // 1→2→3→4 → 返回 2
}
```

## 10.2 检测环

```cpp
// LC 141: 环形链表
bool hasCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;  // 相遇 = 有环
    }
    return false;  // fast 到头 = 无环
}
```

**为什么一定会相遇？**

```
想象操场跑圈：
- 慢人每次跑 1 米
- 快人每次跑 2 米
- 每一步，快人和慢人的距离减少 1
- 因此在环上一定会相遇！
```

## 10.3 找入环点

```cpp
// LC 142: 环形链表 II
ListNode* detectCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            // 相遇后，一个从 head 出发，一个从相遇点出发
            // 两者再次相遇的位置就是入环点
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
```

**数学证明**：

```
设：
- a = head 到入环点的距离
- b = 入环点到相遇点的距离
- c = 环的长度

相遇时：
- slow 走了 a + b
- fast 走了 a + b + nc（绕了 n 圈）
- 因为 fast 速度是 slow 的 2 倍：2(a+b) = a+b+nc
- 化简：a + b = nc → a = nc - b = (n-1)c + (c-b)
- 即从 head 走 a 步 = 从相遇点走 (c-b) + (n-1)圈
- 两者必然在入环点相遇！
```

---

# 11. 链表中的双指针技巧

## 11.1 删除倒数第 N 个节点

```cpp
// LC 19: 删除链表的倒数第 N 个结点
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0);
    dummy.next = head;

    ListNode* fast = &dummy;
    ListNode* slow = &dummy;

    // fast 先走 n+1 步（多走一步，这样 slow 指向被删节点的前驱）
    for (int i = 0; i <= n; i++)
        fast = fast->next;

    // 同速前进
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    // slow->next 就是要删的节点
    ListNode* toDelete = slow->next;
    slow->next = slow->next->next;
    delete toDelete;

    return dummy.next;
}
```

## 11.2 链表相交

```cpp
// LC 160: 相交链表
// 核心思路：两个指针分别遍历两个链表，到末尾后跳到另一个链表头
// 走过的总路程相同 → 如果相交，一定在交点相遇
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    ListNode* a = headA;
    ListNode* b = headB;

    // a 和 b 要么同时到 nullptr（不相交），要么在交点相遇
    while (a != b) {
        a = a ? a->next : headB;    // a 到头就跳到 B
        b = b ? b->next : headA;    // b 到头就跳到 A
    }
    return a;
}

// 图解：
// A: a1→a2 ↘
//           c1→c2→c3
// B: b1→b2→b3 ↗
//
// 指针 a: a1→a2→c1→c2→c3→(跳到B)→b1→b2→b3→c1
// 指针 b: b1→b2→b3→c1→c2→c3→(跳到A)→a1→a2→c1
// 都走了相同总长度后在 c1 相遇！
```

## 11.3 回文链表

```cpp
// LC 234: 回文链表
// 思路：找中点 → 反转后半 → 比较
bool isPalindrome(ListNode* head) {
    if (!head || !head->next) return true;

    // 1. 快慢指针找中点
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 反转后半部分
    ListNode* secondHalf = slow->next;
    slow->next = nullptr;
    ListNode* prev = nullptr;
    while (secondHalf) {
        ListNode* next = secondHalf->next;
        secondHalf->next = prev;
        prev = secondHalf;
        secondHalf = next;
    }

    // 3. 比较前半和反转后的后半
    ListNode* p1 = head;
    ListNode* p2 = prev;
    while (p2) {
        if (p1->val != p2->val) return false;
        p1 = p1->next;
        p2 = p2->next;
    }
    return true;
}
```

---

# 12. 链表排序

## 12.1 归并排序（推荐！适合链表）

```cpp
// LC 148: 排序链表
// 归并排序：O(n log n) 时间，O(log n) 空间（递归栈）
ListNode* sortList(ListNode* head) {
    // 基本情况
    if (!head || !head->next) return head;

    // 1. 快慢指针找中点
    ListNode* slow = head;
    ListNode* fast = head->next;      // 注意这里 fast 从 head->next 开始
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 断开两半
    ListNode* mid = slow->next;
    slow->next = nullptr;

    // 3. 递归排序
    ListNode* left = sortList(head);
    ListNode* right = sortList(mid);

    // 4. 合并
    return mergeTwoLists(left, right);
}

// 合并两个有序链表
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (l1 && l2) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;

    return dummy.next;
}
```

> 🤔 为什么链表排序推荐归并而不是快排？
> - 链表不支持随机访问，快排需要随机下标的操作在链表上低效
> - 归并排序只需要顺序访问和分割，天然适合链表
> - 链表归并无需额外 O(n) 数组空间（不像数组归并）

## 12.2 插入排序

```cpp
// LC 147: 对链表进行插入排序
ListNode* insertionSortList(ListNode* head) {
    ListNode dummy(0);
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;

        // 在 dummy 链表中找到插入位置
        ListNode* prev = &dummy;
        while (prev->next && prev->next->val < curr->val)
            prev = prev->next;

        curr->next = prev->next;
        prev->next = curr;
        curr = next;
    }

    return dummy.next;
}
```

---

# 13. 双向链表

## 13.1 节点定义

```cpp
struct DListNode {
    int val;
    DListNode* prev;
    DListNode* next;
    DListNode(int x) : val(x), prev(nullptr), next(nullptr) {}
};
```

## 13.2 基本操作

```cpp
class DoublyLinkedList {
    DListNode* head;
    DListNode* tail;
    int size;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    // 头部插入
    void pushFront(int val) {
        DListNode* node = new DListNode(val);
        node->next = head;
        if (head) head->prev = node;
        head = node;
        if (!tail) tail = node;
        size++;
    }

    // 尾部插入
    void pushBack(int val) {
        DListNode* node = new DListNode(val);
        node->prev = tail;
        if (tail) tail->next = node;
        tail = node;
        if (!head) head = node;
        size++;
    }

    // 删除指定节点（O(1)！因为有 prev 指针）
    void remove(DListNode* node) {
        if (node->prev) node->prev->next = node->next;
        else head = node->next;

        if (node->next) node->next->prev = node->prev;
        else tail = node->prev;

        delete node;
        size--;
    }
};
```

## 13.3 LRU Cache（双向链表 + 哈希表）

```cpp
// LC 146: LRU 缓存
class LRUCache {
    int capacity;
    list<pair<int, int>> cache;                          // 双向链表
    unordered_map<int, list<pair<int,int>>::iterator> map;  // key → 链表迭代器

public:
    LRUCache(int cap) : capacity(cap) {}

    int get(int key) {
        if (map.find(key) == map.end()) return -1;

        // 移到最前面（最近使用）
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
            // 淘汰最久未使用的（链表尾部）
            int oldKey = cache.back().first;
            cache.pop_back();
            map.erase(oldKey);
        }
        cache.push_front({key, value});
        map[key] = cache.begin();
    }
};
```

---

# 14. 链表与其他数据结构结合

## 14.1 链表 + 哈希表

```cpp
// LC 138: 随机链表的复制
// 节点有一个 random 指针可以指向任意节点
struct Node {
    int val;
    Node* next;
    Node* random;
    Node(int x) : val(x), next(nullptr), random(nullptr) {}
};

Node* copyRandomList(Node* head) {
    if (!head) return nullptr;

    // 哈希记录：旧节点 → 新节点
    unordered_map<Node*, Node*> map;

    // 第一遍：创建所有新节点
    Node* curr = head;
    while (curr) {
        map[curr] = new Node(curr->val);
        curr = curr->next;
    }

    // 第二遍：设置 next 和 random
    curr = head;
    while (curr) {
        map[curr]->next = map[curr->next];
        map[curr]->random = map[curr->random];
        curr = curr->next;
    }

    return map[head];
}
```

## 14.2 链表 + 堆

```cpp
// LC 23: 合并 K 个升序链表
// 用最小堆维护 k 个链表的当前头节点
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) {
        return a->val > b->val;   // 小顶堆
    };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);

    for (auto* list : lists) {
        if (list) pq.push(list);
    }

    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (!pq.empty()) {
        ListNode* min = pq.top();
        pq.pop();
        tail->next = min;
        tail = tail->next;
        if (min->next) pq.push(min->next);
    }

    return dummy.next;
}
// 时间复杂度: O(N log K)，N 是总节点数，K 是链表数
```

---

# 15. 经典 LeetCode 链表题精讲

## 必做题清单

| 题号 | 题目 | 难度 | 核心技巧 | 频率 |
|------|------|------|----------|------|
| 206 | 反转链表 | Easy | 迭代/递归 | ⭐⭐⭐⭐⭐ |
| 21 | 合并两个有序链表 | Easy | dummy + 归并 | ⭐⭐⭐⭐⭐ |
| 141 | 环形链表 | Easy | 快慢指针 | ⭐⭐⭐⭐⭐ |
| 876 | 链表的中间结点 | Easy | 快慢指针 | ⭐⭐⭐⭐ |
| 203 | 移除链表元素 | Easy | dummy | ⭐⭐⭐⭐ |
| 160 | 相交链表 | Easy | 双指针等距 | ⭐⭐⭐⭐ |
| 234 | 回文链表 | Easy | 中点+反转+比较 | ⭐⭐⭐⭐ |
| 83 | 删除排序链表重复元素 | Easy | 遍历去重 | ⭐⭐⭐ |
| 92 | 反转链表 II | Medium | 局部反转 | ⭐⭐⭐⭐⭐ |
| 19 | 删除倒数第N个 | Medium | 双指针+dummy | ⭐⭐⭐⭐⭐ |
| 142 | 环形链表 II | Medium | 快慢指针+数学 | ⭐⭐⭐⭐ |
| 2 | 两数相加 | Medium | 模拟进位 | ⭐⭐⭐⭐ |
| 328 | 奇偶链表 | Medium | 拆分连接 | ⭐⭐⭐ |
| 138 | 随机链表的复制 | Medium | 哈希映射 | ⭐⭐⭐⭐ |
| 148 | 排序链表 | Medium | 归并排序 | ⭐⭐⭐⭐ |
| 146 | LRU 缓存 | Medium | 双向链表+哈希 | ⭐⭐⭐⭐⭐ |
| 25 | K 个一组翻转 | Hard | 分段反转 | ⭐⭐⭐⭐⭐ |
| 23 | 合并K个升序链表 | Hard | 堆/分治 | ⭐⭐⭐⭐⭐ |

## 解题思路速查

```
链表问题 → 先考虑：
  ├─ 头节点可能改变？ → 用 dummy
  ├─ 涉及倒数/中间/环？ → 快慢指针
  ├─ 需要反转？ → prev/curr/next 三指针
  ├─ 需要合并？ → dummy + 尾插
  ├─ 涉及 K 个/多个链表？ → 堆或分治
  └─ 需要 O(1) 删除已知节点？ → 双向链表
```

---

# 16. 链表调试技巧

## 16.1 辅助函数集

```cpp
// 从数组创建链表
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
void printList(ListNode* head) {
    while (head) {
        cout << head->val;
        if (head->next) cout << " → ";
        head = head->next;
    }
    cout << " → null" << endl;
}

// 链表转数组（方便 assert 验证）
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
```

## 16.2 常见调试方法

```
1. 在关键步骤打印链表状态
2. 用小规模输入（1-3 个节点）手动推演
3. 特别测试边界：空链表、单节点、两个节点
4. 画图！画图！画图！链表题一定要画图！
```

---

# 17. 常见陷阱与最佳实践

## 17.1 经典陷阱

### 陷阱 1：丢失指针

```cpp
// ❌ 错误：直接修改 next，丢失后续节点
curr->next = prev;    // 此时 curr 原来的 next 丢了！
curr = curr->next;    // 现在 curr 指向 prev！不是预期的下一个节点

// ✅ 正确：先保存
ListNode* next = curr->next;   // 先保存
curr->next = prev;             // 再修改
curr = next;                   // 用保存的值前进
```

### 陷阱 2：空指针解引用

```cpp
// ❌ 直接访问 next->next，不检查 next 是否为空
if (curr->next->val == target) { ... }

// ✅ 先检查
if (curr->next && curr->next->val == target) { ... }

// ❌ 快慢指针经典错误
while (fast->next) {
    fast = fast->next->next;   // fast->next->next 可能是空指针之后的解引用
}

// ✅ 同时检查 fast 和 fast->next
while (fast && fast->next) {
    fast = fast->next->next;
}
```

### 陷阱 3：忘记更新 head

```cpp
// ❌ 删除头节点后返回旧 head
head = head->next;
// 如果函数返回局部 head，调用者的 head 不变

// ✅ 用 dummy 或返回新 head
ListNode dummy(0);
dummy.next = head;
// ...
return dummy.next;
```

### 陷阱 4：环形链表导致死循环

```cpp
// ❌ 遍历有环链表，永远不会结束
while (curr) {
    curr = curr->next;   // 如果有环，永远不会到 nullptr
}

// ✅ 用快慢指针检测环
```

### 陷阱 5：内存泄漏

```cpp
// ❌ 删除节点不释放内存
prev->next = curr->next;   // curr 节点成了野内存

// ✅ 先保存再释放
ListNode* toDelete = curr;
prev->next = curr->next;
delete toDelete;

// 注意：LeetCode 不检查内存泄漏，但面试中会被问到
```

### 陷阱 6：死循环——忘记断链

```cpp
// ❌ 合并或分割链表时，忘记断开尾部
// 如果 slow->next 不置 nullptr，链表未断开，后续处理出错

// ✅ 分割链表时一定要断开
ListNode* mid = slow->next;
slow->next = nullptr;      // 断开！
```

## 17.2 最佳实践

```
✅ 画图：链表题第一步永远是画图
✅ Dummy：凡是头节点可能变化，必用 dummy
✅ 保存 next：反转/删除前，先保存 curr->next
✅ 边界测试：空链表、单节点、两节点、奇偶长度
✅ 释放内存：delete 被删除的节点
✅ 短路求值：访问 fast->next->next 前先 check
✅ 尾插法创建：避免头插法的逆序问题
✅ 递归/迭代选择：面试通常 pass 迭代版本即可
```

## 17.3 链表技巧速查卡

```
┌──────────────────────────────────────────────────┐
│               链表技巧速查                        │
├──────────────────────────────────────────────────┤
│  Dummy Node    → 统一头部操作                     │
│  快慢指针       → 中点/环/倒数第k                  │
│  prev/curr/next → 反转链表标配                    │
│  哈希表辅助      → 复制随机链表/交点               │
│  递归           → 反转/合并的简洁实现              │
│  分治           → 归并排序/合并K个                 │
│  堆             → 合并K个升序链表                  │
│  双向链表+哈希   → LRU Cache                     │
└──────────────────────────────────────────────────┘
```

---

*📝 下一步：完成专题二的示例代码 `knowlege_details_2_linkedlist.cpp`，然后继续专题三。*

*💪 链表是算法面试的基本功，务必手写每个反转变体！*
