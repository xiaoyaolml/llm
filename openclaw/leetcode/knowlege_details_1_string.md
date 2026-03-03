# 📖 专题一：字符串（String）— 从零基础到精通

*作者：大胖超 😜 | C++ 字符串完全教学*

---

## 目录

1. [C 风格字符串（char 数组）](#1-c-风格字符串char-数组)
2. [std::string 基础](#2-stdstring-基础)
3. [string 的增删改查](#3-string-的增删改查)
4. [字符串遍历与字符操作](#4-字符串遍历与字符操作)
5. [字符串转换](#5-字符串转换)
6. [子串与查找](#6-子串与查找)
7. [字符串比较与排序](#7-字符串比较与排序)
8. [string 的内存机制](#8-string-的内存机制)
9. [string_view（C++17）](#9-string_viewc17)
10. [双指针在字符串中的应用](#10-双指针在字符串中的应用)
11. [滑动窗口在字符串中的应用](#11-滑动窗口在字符串中的应用)
12. [回文字符串](#12-回文字符串)
13. [字符串匹配算法](#13-字符串匹配算法)
14. [字符串哈希](#14-字符串哈希)
15. [正则表达式基础](#15-正则表达式基础)
16. [LeetCode 经典字符串题精讲](#16-leetcode-经典字符串题精讲)
17. [常见陷阱与最佳实践](#17-常见陷阱与最佳实践)

---

# 1. C 风格字符串（char 数组）

## 1.1 什么是 C 风格字符串？

C 风格字符串就是一个**以 `\0`（空字符）结尾的 char 数组**。

```
"Hello" 在内存中：
┌─────┬─────┬─────┬─────┬─────┬─────┐
│ 'H' │ 'e' │ 'l' │ 'l' │ 'o' │ '\0'│
└─────┴─────┴─────┴─────┴─────┴─────┘
  [0]   [1]   [2]   [3]   [4]   [5]
                                 ↑ 结尾标志
```

## 1.2 声明与初始化

```cpp
// 方式 1：手动指定大小
char str1[6] = {'H', 'e', 'l', 'l', 'o', '\0'};

// 方式 2：字符串字面量（自动加 \0）
char str2[] = "Hello";         // 大小自动为 6（含 \0）

// 方式 3：指定大小+字面量
char str3[10] = "Hello";      // 后面的都是 \0

// ⚠️ 常见错误
char str4[5] = "Hello";       // 大小 5 放不下 \0！未定义行为
```

## 1.3 C 字符串函数（`<cstring>`）

```cpp
#include <cstring>

char s1[20] = "Hello";
char s2[20] = "World";

strlen(s1);            // 5（不含 \0）
strcmp(s1, s2);         // < 0（H < W，按字典序）
strcpy(s1, "Hi");      // s1 = "Hi"
strcat(s1, " World");  // s1 = "Hi World"
strstr(s1, "Wor");     // 找子串，返回指针

// ⚠️ 这些函数都不检查缓冲区溢出！非常危险
// 现代 C++ 中应优先使用 std::string
```

## 1.4 为什么不用 C 风格字符串？

```
❌ 手动管理内存，容易缓冲区溢出
❌ 没有大小信息，strlen 是 O(n)
❌ 不能直接用 + 拼接
❌ 不能直接用 == 比较（那是比较指针地址）
❌ 常量字符串字面量不可修改

✅ 现代 C++ 答案：std::string
```

---

# 2. std::string 基础

## 2.1 创建与初始化

```cpp
#include <string>
using namespace std;

// ========== 创建方式 ==========
string s1;                   // 空字符串 ""
string s2 = "Hello";        // 从字符串字面量
string s3("World");          // 构造函数
string s4(5, 'a');           // "aaaaa"（5 个 'a'）
string s5(s2);               // 拷贝 s2
string s6(s2, 1, 3);        // 从 s2 下标 1 开始取 3 个 → "ell"
string s7 = s2 + " " + s3;  // 拼接 → "Hello World"

// C++14: 字符串字面量后缀 s
using namespace std::string_literals;
auto s8 = "Hello"s;          // 类型是 std::string，不是 const char*
```

## 2.2 string 底层原理

`std::string` 本质上就是一个**动态 char 数组**（类似 `vector<char>`），但做了很多优化：

```
std::string 内部结构（简化版）：

class string {
    char* _data;     // 指向字符数据
    size_t _size;    // 当前长度
    size_t _capacity; // 已分配容量
};

小字符串优化 (SSO - Small String Optimization)：
- 短字符串（通常 ≤ 15~22 字节）直接存在栈上，不分配堆内存
- 这样 "hi"、"test" 这种短字符串就不用 new/delete，非常快

┌──────────────────────┐
│ SSO buffer (栈上)     │  ← 短字符串直接存这里
│ "Hello\0          "  │
│ size = 5             │
│ 标志位: 使用SSO      │
└──────────────────────┘

┌──────────────────────┐
│ _data → [堆上的数据]  │  ← 长字符串在堆上
│ size = 100           │
│ capacity = 128       │
│ 标志位: 使用堆内存    │
└──────────────────────┘
```

---

# 3. string 的增删改查

## 3.1 增加/拼接

```cpp
string s = "Hello";

// 拼接
s += " World";          // "Hello World"
s.append("!");          // "Hello World!"
s.append(3, '?');       // "Hello World!???"
s.push_back('~');       // "Hello World!???~"

// 插入
s.insert(5, ",");       // "Hello, World!???~"
s.insert(0, ">>> ");    // ">>> Hello, World!???~"

// ⚠️ 性能：拼接操作在尾部是 O(1) 摊销，在中间是 O(n)
```

## 3.2 删除

```cpp
string s = "Hello World";

s.erase(5, 1);          // 删除下标 5 开始的 1 个字符 → "HelloWorld"
s.erase(5);             // 删除下标 5 及之后所有 → "Hello"
s.pop_back();           // 删除最后一个字符 → "Hell"
s.clear();              // 清空 → ""
```

## 3.3 修改

```cpp
string s = "Hello World";

s[0] = 'h';             // "hello World"（不检查越界）
s.at(0) = 'H';          // "Hello World"（检查越界）
s.replace(6, 5, "C++"); // 从下标6替换5个字符 → "Hello C++"

// 全部转大写/小写
transform(s.begin(), s.end(), s.begin(), ::toupper);  // "HELLO C++"
transform(s.begin(), s.end(), s.begin(), ::tolower);  // "hello c++"
```

## 3.4 查询

```cpp
string s = "Hello World Hello";

s.size();              // 17（= length()）
s.length();            // 17
s.empty();             // false
s.front();             // 'H'
s.back();              // 'o'

// 子串
s.substr(6, 5);        // "World"（从下标 6 开始取 5 个）
s.substr(6);           // "World Hello"（从下标 6 到末尾）

// 查找
s.find("Hello");       // 0（第一次出现的位置）
s.find("Hello", 1);    // 12（从下标 1 开始找）
s.rfind("Hello");      // 12（从右往左找）
s.find_first_of("aeiou");   // 1（第一个元音的位置）
s.find_last_of("aeiou");    // 16

// 如果找不到，返回 string::npos（一个很大的数）
if (s.find("xyz") == string::npos) {
    cout << "没找到";
}
```

---

# 4. 字符串遍历与字符操作

## 4.1 遍历

```cpp
string s = "Hello";

// 方式 1：下标
for (int i = 0; i < (int)s.size(); i++) {
    cout << s[i];
}

// 方式 2：范围 for（推荐）
for (char c : s) {
    cout << c;
}

// 方式 3：引用修改
for (char& c : s) {
    c = toupper(c);  // 转大写
}

// 方式 4：反向遍历
for (int i = (int)s.size() - 1; i >= 0; i--) {
    cout << s[i];
}
// 或用反向迭代器
for (auto it = s.rbegin(); it != s.rend(); ++it) {
    cout << *it;
}
```

## 4.2 字符操作函数（`<cctype>`）

```cpp
#include <cctype>

// 判断字符类型
isalpha('A');    // true —— 是否是字母
isdigit('3');    // true —— 是否是数字
isalnum('a');    // true —— 是否是字母或数字
isspace(' ');    // true —— 是否是空白字符
isupper('A');    // true —— 是否大写
islower('a');    // true —— 是否小写
ispunct('!');    // true —— 是否是标点

// 转换
toupper('a');    // 'A'
tolower('A');    // 'a'

// ⚠️ 中文字符不适用，这些函数只处理 ASCII
```

## 4.3 字符的 ASCII 值

```cpp
// 字符本质上是整数（ASCII 码）
// 'A' = 65, 'Z' = 90
// 'a' = 97, 'z' = 122
// '0' = 48, '9' = 57

// 常用技巧
int digit = '5' - '0';         // 字符 → 数字：5
char ch = 3 + '0';             // 数字 → 字符：'3'
int pos = 'c' - 'a';           // 字母在字母表中的位置：2
char upper = 'a' - 32;         // 小写→大写（不推荐，用 toupper）
bool isLower = (c >= 'a' && c <= 'z');  // 手动判断小写

// 字母频率统计数组（刷题超常用！）
int freq[26] = {};
for (char c : s) {
    freq[c - 'a']++;  // 将 'a'-'z' 映射到 0-25
}
```

---

# 5. 字符串转换

## 5.1 数字 ↔ 字符串

```cpp
// ========== 字符串 → 数字 ==========
int    n1 = stoi("42");       // string to int
long   n2 = stol("123456");   // string to long
long long n3 = stoll("99999999999");  // string to long long
float  f1 = stof("3.14");     // string to float
double d1 = stod("3.14159");  // string to double

// 带进制转换
int hex = stoi("ff", nullptr, 16);   // 16 进制 → 255
int bin = stoi("1010", nullptr, 2);  // 2 进制 → 10

// ========== 数字 → 字符串 ==========
string s1 = to_string(42);        // "42"
string s2 = to_string(3.14);      // "3.140000"
string s3 = to_string(-99);       // "-99"
```

## 5.2 string ↔ C 字符串

```cpp
string s = "Hello";

// string → const char*
const char* cstr = s.c_str();   // 返回 null-terminated 的 C 字符串
const char* data = s.data();    // C++11 起等同于 c_str()

// char* → string
char arr[] = "World";
string s2(arr);       // 自动转换
string s3 = arr;      // 也可以

// ⚠️ c_str() 返回的指针在 string 被修改后可能失效！
```

## 5.3 stringstream（格式化利器）

```cpp
#include <sstream>

// 拼接格式化字符串
ostringstream oss;
oss << "Name: " << "Alice" << ", Age: " << 25 << ", Score: " << 98.5;
string result = oss.str();
// result = "Name: Alice, Age: 25, Score: 98.5"

// 分割字符串（按空格）
string line = "hello world foo bar";
istringstream iss(line);
string word;
while (iss >> word) {
    cout << "[" << word << "] ";
}
// 输出: [hello] [world] [foo] [bar]

// 按特定分隔符分割
string csv = "apple,banana,cherry";
istringstream iss2(csv);
string token;
while (getline(iss2, token, ',')) {
    cout << "[" << token << "] ";
}
// 输出: [apple] [banana] [cherry]
```

---

# 6. 子串与查找

## 6.1 提取子串

```cpp
string s = "Hello, World!";

// substr(pos, len) —— 从 pos 开始取 len 个字符
string sub1 = s.substr(0, 5);    // "Hello"
string sub2 = s.substr(7);       // "World!"（到末尾）
```

## 6.2 查找

```cpp
string s = "abcabc";

// find 族
s.find("abc");           // 0   ← 首次出现位置
s.find("abc", 1);        // 3   ← 从下标 1 开始找
s.rfind("abc");          // 3   ← 最后一次出现位置
s.find('b');             // 1   ← 查找单个字符

// find_first_of / find_last_of —— 查找字符集合中任意一个
string vowels = "aeiou";
s = "Hello World";
s.find_first_of(vowels);     // 1（'e'）
s.find_last_of(vowels);      // 7（'o'）
s.find_first_not_of("Helo"); // 5（' '，第一个不是 H/e/l/o 的）

// 判断是否包含子串
bool contains = (s.find("World") != string::npos);

// C++23: contains, starts_with, ends_with
// C++20 已支持:
s.starts_with("Hello");  // true
s.ends_with("World");    // true
// C++23:
// s.contains("llo");    // true
```

## 6.3 替换

```cpp
string s = "Hello World World";

// replace(pos, len, new_str) —— 替换一次
s.replace(6, 5, "C++");   // "Hello C++ World"

// 替换所有出现（标准库没有直接函数，手动实现）
void replaceAll(string& s, const string& from, const string& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}
```

---

# 7. 字符串比较与排序

## 7.1 比较

```cpp
string s1 = "apple", s2 = "banana";

// 直接用比较运算符（按字典序比较）
bool eq = (s1 == s2);    // false
bool lt = (s1 < s2);     // true（"apple" < "banana"）
bool gt = (s1 > s2);     // false

// compare 函数（更细粒度，返回 <0, 0, >0）
int cmp = s1.compare(s2);     // < 0
int cmp2 = s1.compare(0, 3, s2, 0, 3);  // 比较 "app" 和 "ban"

// ⚠️ C 风格的 == 比较的是指针地址，不是内容！
const char* c1 = "hello";
const char* c2 = "hello";
// c1 == c2   可能是 true 也可能 false（取决于编译器是否合并字面量）
// strcmp(c1, c2) == 0  ← 这才是正确的 C 风格比较
```

## 7.2 排序

```cpp
// 字符串内部排序
string s = "dcba";
sort(s.begin(), s.end());    // "abcd"

// 字符串数组排序
vector<string> words = {"banana", "apple", "cherry"};

// 默认字典序
sort(words.begin(), words.end());
// {"apple", "banana", "cherry"}

// 按长度排序
sort(words.begin(), words.end(), [](const string& a, const string& b) {
    return a.size() < b.size();
});

// 忽略大小写排序
sort(words.begin(), words.end(), [](string a, string b) {
    transform(a.begin(), a.end(), a.begin(), ::tolower);
    transform(b.begin(), b.end(), b.begin(), ::tolower);
    return a < b;
});
```

---

# 8. string 的内存机制

## 8.1 与 vector 的相似性

```cpp
string s = "Hello";

s.size();          // 5  —— 当前长度
s.capacity();      // ≥ 5 —— 已分配容量（通常带有 SSO 或预分配）
s.reserve(100);    // 预分配 100 字节
s.shrink_to_fit(); // 释放多余的内存
s.resize(10, '!'); // 调整大小为 10，用 '!' 填充
```

## 8.2 SSO（小字符串优化）

```
大多数 std::string 实现对短字符串做了优化：

短字符串（≤ 15 字节左右）：
┌──────────────────────────────┐
│ ['H']['e']['l']['l']['o'][0] │  ← 直接存在对象内部，零堆分配
│          ...padding...        │
│ size = 5                      │
│ SSO 标志                      │
└──────────────────────────────┘

长字符串（> 15 字节）：
┌──────────────────────────────┐
│ _data → 堆上: [很长的字符串]  │  ← 在堆上分配
│ size = 1000                   │
│ capacity = 1024               │
└──────────────────────────────┘

性能含义：
- 短字符串创建/拷贝几乎零开销（栈上操作）
- 长字符串可能触发堆分配
- 频繁创建短字符串时 string 比看起来的要快
```

## 8.3 移动语义

```cpp
// C++11 移动语义避免深拷贝
string createLongString() {
    string s(1000, 'x');  // 1000 个 'x'
    return s;              // 移动，不拷贝！（编译器 RVO/NRVO）
}

string s = createLongString();  // 零拷贝

// 显式移动
string s1 = "Hello World";
string s2 = std::move(s1);  // s2 获得 s1 的资源，s1 变成空字符串
```

---

# 9. string_view（C++17）

## 9.1 什么是 string_view？

`string_view` 是一个**不拥有数据的字符串引用**，只包含指针和长度，零拷贝。

```cpp
#include <string_view>

// 传统做法：每次传 string 参数可能触发拷贝
void process(const string& s);     // 如果传 "hello"，先创建临时 string

// 更好的做法：用 string_view，零拷贝
void process(string_view sv);      // 不管传什么都不拷贝

string_view sv1 = "Hello";         // 直接引用字面量
string s = "World";
string_view sv2 = s;               // 引用 string 的内存
string_view sv3 = sv1.substr(1, 3); // "ell"，还是零拷贝！

cout << sv1.size() << endl;        // 5
cout << sv1[0] << endl;            // 'H'
```

## 9.2 注意事项

```cpp
// ⚠️ string_view 不拥有数据，不保证生命周期！
string_view dangerous() {
    string s = "Hello";
    return string_view(s);  // ❌ s 被销毁，string_view 悬空！
}

// ⚠️ string_view 没有 c_str()（不保证 null-terminated）
// 如果需要 C 字符串，要先转成 string
string str(sv);  // 从 string_view 创建 string
```

---

# 10. 双指针在字符串中的应用

## 10.1 反转字符串

```cpp
// LC 344: 反转字符串
void reverseString(vector<char>& s) {
    int left = 0, right = (int)s.size() - 1;
    while (left < right) {
        swap(s[left++], s[right--]);
    }
}
// 或直接用 reverse(s.begin(), s.end());
```

## 10.2 验证回文

```cpp
// LC 125: 验证回文串
// 只考虑字母和数字，忽略大小写
bool isPalindrome(string s) {
    int left = 0, right = (int)s.size() - 1;
    while (left < right) {
        // 跳过非字母数字字符
        while (left < right && !isalnum(s[left])) left++;
        while (left < right && !isalnum(s[right])) right--;
        
        if (tolower(s[left]) != tolower(s[right]))
            return false;
        left++;
        right--;
    }
    return true;
}
```

## 10.3 字符串压缩

```cpp
// LC 443: 压缩字符串
// "aabbccc" → "a2b2c3"
int compress(vector<char>& chars) {
    int write = 0;  // 写指针
    int read = 0;   // 读指针
    
    while (read < (int)chars.size()) {
        char current = chars[read];
        int count = 0;
        
        // 计算连续相同字符的个数
        while (read < (int)chars.size() && chars[read] == current) {
            read++;
            count++;
        }
        
        // 写字符
        chars[write++] = current;
        
        // 写数量（> 1 时）
        if (count > 1) {
            string countStr = to_string(count);
            for (char c : countStr) {
                chars[write++] = c;
            }
        }
    }
    return write;
}
```

---

# 11. 滑动窗口在字符串中的应用

## 11.1 无重复字符的最长子串

```cpp
// LC 3: 无重复字符的最长子串
int lengthOfLongestSubstring(string s) {
    // 思路：滑动窗口，用 set/map 记录窗口内的字符
    unordered_map<char, int> window;  // 字符 → 出现次数
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < (int)s.size(); right++) {
        window[s[right]]++;
        
        // 有重复字符，收缩左边界
        while (window[s[right]] > 1) {
            window[s[left]]--;
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

// 优化版：记录字符最后出现的位置，直接跳过
int lengthOfLongestSubstringOptimized(string s) {
    unordered_map<char, int> lastSeen;  // 字符 → 最后出现的位置
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < (int)s.size(); right++) {
        if (lastSeen.count(s[right]) && lastSeen[s[right]] >= left) {
            left = lastSeen[s[right]] + 1;  // 直接跳到重复字符的下一位
        }
        lastSeen[s[right]] = right;
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

## 11.2 最小覆盖子串

```cpp
// LC 76: 最小覆盖子串
string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    
    int left = 0, valid = 0;
    int start = 0, minLen = INT_MAX;
    
    for (int right = 0; right < (int)s.size(); right++) {
        char c = s[right];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;  // 该字符满足了
        }
        
        // 窗口已包含所有目标字符，尝试收缩
        while (valid == (int)need.size()) {
            // 更新最小窗口
            if (right - left + 1 < minLen) {
                start = left;
                minLen = right - left + 1;
            }
            
            char d = s[left];
            left++;
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;
                window[d]--;
            }
        }
    }
    return minLen == INT_MAX ? "" : s.substr(start, minLen);
}
```

## 11.3 字母异位词

```cpp
// LC 242: 有效的字母异位词
bool isAnagram(string s, string t) {
    if (s.size() != t.size()) return false;
    
    // 方法 1：排序比较
    // sort(s.begin(), s.end());
    // sort(t.begin(), t.end());
    // return s == t;
    
    // 方法 2：频率统计（更快 O(n)）
    int freq[26] = {};
    for (char c : s) freq[c - 'a']++;
    for (char c : t) freq[c - 'a']--;
    for (int f : freq) if (f != 0) return false;
    return true;
}

// LC 438: 找到字符串中所有字母异位词
vector<int> findAnagrams(string s, string p) {
    vector<int> result;
    if (s.size() < p.size()) return result;
    
    vector<int> need(26, 0), window(26, 0);
    for (char c : p) need[c - 'a']++;
    
    for (int right = 0; right < (int)s.size(); right++) {
        window[s[right] - 'a']++;
        
        // 窗口大小超过 p.size()，移除左边
        if (right >= (int)p.size())
            window[s[right - p.size()] - 'a']--;
        
        // 窗口内频率和目标一致
        if (window == need)
            result.push_back(right - (int)p.size() + 1);
    }
    return result;
}
```

---

# 12. 回文字符串

## 12.1 判断回文

```cpp
// 方法 1：双指针
bool isPalin(const string& s) {
    int l = 0, r = (int)s.size() - 1;
    while (l < r) {
        if (s[l++] != s[r--]) return false;
    }
    return true;
}

// 方法 2：反转比较
bool isPalin2(const string& s) {
    string rev(s.rbegin(), s.rend());
    return s == rev;
}
```

## 12.2 最长回文子串

```cpp
// LC 5: 最长回文子串
// 方法：中心扩展法
// 思路：从每个位置向两边扩展，检查回文
string longestPalindrome(string s) {
    int start = 0, maxLen = 1;
    int n = s.size();
    
    // 从位置 (left, right) 向两边扩展
    auto expand = [&](int left, int right) {
        while (left >= 0 && right < n && s[left] == s[right]) {
            if (right - left + 1 > maxLen) {
                start = left;
                maxLen = right - left + 1;
            }
            left--;
            right++;
        }
    };
    
    for (int i = 0; i < n; i++) {
        expand(i, i);      // 奇数长度回文（以 i 为中心）
        expand(i, i + 1);  // 偶数长度回文（以 i 和 i+1 为中心）
    }
    
    return s.substr(start, maxLen);
}
```

## 12.3 回文子串计数

```cpp
// LC 647: 回文子串（计数）
int countSubstrings(string s) {
    int count = 0;
    int n = s.size();
    
    auto expand = [&](int left, int right) {
        while (left >= 0 && right < n && s[left] == s[right]) {
            count++;
            left--;
            right++;
        }
    };
    
    for (int i = 0; i < n; i++) {
        expand(i, i);      // 奇数
        expand(i, i + 1);  // 偶数
    }
    return count;
}
```

## 12.4 最长回文子序列（DP）

```cpp
// LC 516: 最长回文子序列
// 注意：子序列可以不连续，子串必须连续
int longestPalindromeSubseq(string s) {
    int n = s.size();
    // dp[i][j] = s[i..j] 中最长回文子序列的长度
    vector<vector<int>> dp(n, vector<int>(n, 0));
    
    for (int i = n - 1; i >= 0; i--) {
        dp[i][i] = 1;  // 单个字符是回文
        for (int j = i + 1; j < n; j++) {
            if (s[i] == s[j])
                dp[i][j] = dp[i + 1][j - 1] + 2;  // 首尾相同，加 2
            else
                dp[i][j] = max(dp[i + 1][j], dp[i][j - 1]);  // 去掉一端
        }
    }
    return dp[0][n - 1];
}
```

---

# 13. 字符串匹配算法

## 13.1 暴力匹配

```cpp
// 暴力 O(n*m)
int bruteForceSearch(const string& text, const string& pattern) {
    int n = text.size(), m = pattern.size();
    for (int i = 0; i <= n - m; i++) {
        bool match = true;
        for (int j = 0; j < m; j++) {
            if (text[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) return i;
    }
    return -1;
}
```

## 13.2 KMP 算法

KMP 的核心思想：利用**已匹配的部分信息**，避免重复比较。

```cpp
// 构建 next 数组（前缀函数/失败函数）
// next[i] = pattern[0..i] 中最长的相等前后缀长度
vector<int> buildNext(const string& pattern) {
    int m = pattern.size();
    vector<int> next(m, 0);
    int j = 0;  // 前缀指针
    
    for (int i = 1; i < m; i++) {
        while (j > 0 && pattern[i] != pattern[j])
            j = next[j - 1];  // 回退
        if (pattern[i] == pattern[j])
            j++;
        next[i] = j;
    }
    return next;
}

// KMP 匹配 O(n+m)
int kmpSearch(const string& text, const string& pattern) {
    vector<int> next = buildNext(pattern);
    int j = 0;
    
    for (int i = 0; i < (int)text.size(); i++) {
        while (j > 0 && text[i] != pattern[j])
            j = next[j - 1];
        if (text[i] == pattern[j])
            j++;
        if (j == (int)pattern.size())
            return i - j + 1;  // 找到匹配
    }
    return -1;
}

// KMP 的 next 数组图解：
// pattern = "abcabd"
// 
// i=0: next[0] = 0  (单个字符没有前后缀)
// i=1: next[1] = 0  "ab" 无相等前后缀
// i=2: next[2] = 0  "abc" 无相等前后缀
// i=3: next[3] = 1  "abca" 前缀 "a" = 后缀 "a"
// i=4: next[4] = 2  "abcab" 前缀 "ab" = 后缀 "ab"
// i=5: next[5] = 0  "abcabd" 无相等前后缀
// next = [0, 0, 0, 1, 2, 0]
```

## 13.3 Rabin-Karp（滚动哈希）

```cpp
// 滚动哈希匹配
// 思路：用哈希值快速比较，冲突时再逐字符比较
int rabinKarp(const string& text, const string& pattern) {
    int n = text.size(), m = pattern.size();
    if (m > n) return -1;
    
    const long long MOD = 1e9 + 7;
    const long long BASE = 31;
    
    // 计算 pattern 的哈希值
    long long patHash = 0, textHash = 0, power = 1;
    for (int i = 0; i < m; i++) {
        patHash = (patHash * BASE + pattern[i]) % MOD;
        textHash = (textHash * BASE + text[i]) % MOD;
        if (i > 0) power = (power * BASE) % MOD;
    }
    
    // 滑动窗口比较哈希值
    for (int i = 0; i <= n - m; i++) {
        if (textHash == patHash) {
            // 哈希匹配，验证实际字符（防止哈希冲突）
            if (text.substr(i, m) == pattern) return i;
        }
        // 滚动计算下一个窗口的哈希
        if (i < n - m) {
            textHash = (textHash - (long long)text[i] * power % MOD + MOD) % MOD;
            textHash = (textHash * BASE + text[i + m]) % MOD;
        }
    }
    return -1;
}
```

---

# 14. 字符串哈希

## 14.1 为什么需要字符串哈希？

当需要快速比较两个子串是否相等时，逐字符比较是 O(n)，而预计算哈希后比较只需 O(1)。

```cpp
// 字符串哈希模板
class StringHash {
    vector<long long> hashVal, power;
    long long MOD = 1e9 + 7, BASE = 131;
    
public:
    StringHash(const string& s) {
        int n = s.size();
        hashVal.resize(n + 1, 0);
        power.resize(n + 1, 1);
        
        for (int i = 0; i < n; i++) {
            hashVal[i + 1] = (hashVal[i] * BASE + s[i]) % MOD;
            power[i + 1] = (power[i] * BASE) % MOD;
        }
    }
    
    // 获取 s[l..r] 的哈希值
    long long getHash(int l, int r) {
        return (hashVal[r + 1] - hashVal[l] * power[r - l + 1] % MOD + MOD) % MOD;
    }
};
```

---

# 15. 正则表达式基础

```cpp
#include <regex>

string text = "Email: alice@example.com, Phone: 123-456-7890";

// 简单匹配
regex emailRegex(R"(\w+@\w+\.\w+)");
smatch match;
if (regex_search(text, match, emailRegex)) {
    cout << "找到邮箱: " << match[0] << endl;  // alice@example.com
}

// 查找所有匹配
regex phoneRegex(R"(\d{3}-\d{3}-\d{4})");
auto begin = sregex_iterator(text.begin(), text.end(), phoneRegex);
auto end = sregex_iterator();
for (auto it = begin; it != end; ++it) {
    cout << "电话: " << it->str() << endl;  // 123-456-7890
}

// 替换
string result = regex_replace(text, emailRegex, "[REDACTED]");

// ⚠️ C++ 的 regex 性能较差，竞赛/高性能场景慎用
```

---

# 16. LeetCode 经典字符串题精讲

## 必做题清单

### Easy
| 题号 | 题目 | 核心考点 |
|------|------|---------|
| 13 | 罗马数字转整数 | 模拟 |
| 14 | 最长公共前缀 | 纵向扫描 |
| 20 | 有效的括号 | 栈 |
| 125 | 验证回文串 | 双指针 |
| 242 | 有效的字母异位词 | 频率统计 |
| 344 | 反转字符串 | 双指针 |
| 387 | 字符串中第一个唯一字符 | 哈希表 |

### Medium
| 题号 | 题目 | 核心考点 |
|------|------|---------|
| 3 | 无重复字符最长子串 | 滑动窗口 |
| 5 | 最长回文子串 | 中心扩展/DP |
| 22 | 括号生成 | 回溯 |
| 49 | 字母异位词分组 | 排序+哈希 |
| 76 | 最小覆盖子串 | 滑动窗口 |
| 139 | 单词拆分 | DP |
| 394 | 字符串解码 | 栈 |
| 438 | 所有字母异位词 | 固定窗口 |
| 647 | 回文子串 | 中心扩展 |

### Hard
| 题号 | 题目 | 核心考点 |
|------|------|---------|
| 10 | 正则表达式匹配 | DP |
| 32 | 最长有效括号 | 栈/DP |
| 44 | 通配符匹配 | DP |
| 72 | 编辑距离 | DP |

## 重点题详解

### 编辑距离（DP 经典）

```cpp
// LC 72: 编辑距离
// dp[i][j] = word1 前 i 个字符变成 word2 前 j 个字符的最少操作数
int minDistance(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    
    // 初始化：空串到长度为 j 的串需要 j 次插入
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];  // 相同，不操作
            } else {
                dp[i][j] = min({
                    dp[i - 1][j] + 1,      // 删除
                    dp[i][j - 1] + 1,      // 插入
                    dp[i - 1][j - 1] + 1   // 替换
                });
            }
        }
    }
    return dp[m][n];
}
```

### 字符串解码（栈）

```cpp
// LC 394: 字符串解码
// 输入: "3[a2[c]]" → "accaccacc"
string decodeString(string s) {
    stack<string> strStack;
    stack<int> numStack;
    string current;
    int num = 0;
    
    for (char c : s) {
        if (isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c == '[') {
            strStack.push(current);  // 保存当前字符串
            numStack.push(num);       // 保存重复次数
            current = "";
            num = 0;
        } else if (c == ']') {
            string prev = strStack.top(); strStack.pop();
            int repeat = numStack.top(); numStack.pop();
            string temp;
            for (int i = 0; i < repeat; i++) temp += current;
            current = prev + temp;
        } else {
            current += c;
        }
    }
    return current;
}
```

### 单词拆分（DP）

```cpp
// LC 139: 单词拆分
// "leetcode" → ["leet", "code"] → true
bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> wordSet(wordDict.begin(), wordDict.end());
    int n = s.size();
    // dp[i] = s[0..i-1] 是否可以被拆分
    vector<bool> dp(n + 1, false);
    dp[0] = true;  // 空串
    
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            // s[0..j-1] 可以拆分 且 s[j..i-1] 在字典中
            if (dp[j] && wordSet.count(s.substr(j, i - j))) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[n];
}
```

---

# 17. 常见陷阱与最佳实践

## 17.1 常见陷阱

```cpp
// ⚠️ 陷阱 1：遍历时修改 string
string s = "abc";
for (int i = 0; i < s.size(); i++) {
    // 如果在循环中 erase 或 insert，s.size() 会变化！
    // 确保使用正确的索引管理
}

// ⚠️ 陷阱 2：substr 越界
string s = "Hello";
// s.substr(10);  // 抛出 out_of_range 异常
// s.substr(3, 100);  // 不会异常，取到末尾 → "lo"

// ⚠️ 陷阱 3：字符和字符串的区别
string s2 = "Hello";
// s2 += 'A';     // ✅ 正确，追加字符
// s2 += "A";     // ✅ 正确，追加字符串
// s2 += 65;      // ⚠️ 追加 ASCII 65 = 'A'，但容易误用

// ⚠️ 陷阱 4：空字符串操作
string empty;
// empty[0];      // 未定义行为！
// empty.front(); // 未定义行为！
// empty.back();  // 未定义行为！
if (!empty.empty()) {  // 先检查
    char first = empty[0];
    (void)first;
}

// ⚠️ 陷阱 5：中文字符
string chinese = "你好";
// chinese.size()  不是 2！UTF-8 编码下每个中文字符占 3 字节
// chinese[0] 也不是 '你'，是第一个字节
// 处理中文需要用 wstring 或第三方库
```

## 17.2 性能最佳实践

```cpp
// ✅ 传递用 const string& 或 string_view
void process(const string& s);           // ✅ 不拷贝
void process(string_view sv);            // ✅ 更灵活，零拷贝

// ✅ 大量拼接用 reserve 或 ostringstream
string result;
result.reserve(10000);  // 预分配，避免频繁扩容
for (int i = 0; i < 1000; i++) {
    result += "hello";
}

// ✅ 字符操作比字符串操作快
s += 'a';          // ✅ O(1) 追加字符
s += "a";          // ❓ 创建临时字符串（但编译器通常会优化）
s.push_back('a');  // ✅ 显式追加字符

// ✅ 频率统计用 int[26] 而非 unordered_map
int freq[26] = {};   // ✅ 栈上分配，cache友好，访问 O(1)
// unordered_map<char, int> freq;  // ❓ 堆分配，hash 有开销

// ✅ 比较子串时，避免创建临时 string
// 差的写法：
// if (s.substr(i, 3) == "abc")  // 创建临时 string
// 好的写法：
if (s.compare(i, 3, "abc") == 0) {}  // 零分配比较
```

## 17.3 字符串刷题模板

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // 常用工具函数

    // 判断回文
    bool isPalin(const string& s, int l, int r) {
        while (l < r) {
            if (s[l++] != s[r--]) return false;
        }
        return true;
    }
    
    // 翻转单词
    string reverseWords(string s) {
        // 去前后空格、中间多余空格
        // 整体翻转 + 单词翻转
        // ... 实现 ...
        return s;
    }
    
    // 字符频率统计
    vector<int> charFreq(const string& s) {
        vector<int> freq(26, 0);
        for (char c : s) freq[c - 'a']++;
        return freq;
    }
};
```

---

*最后更新：2026-03-02*
*字符串是面试的常客，练好基本功！✍️*
