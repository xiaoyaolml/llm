/**
 * ============================================================================
 *  专题一：字符串（String）— 完整示例代码
 *  配合 knowlege_details_1_string.md 食用
 *  编译: g++ -std=c++17 -o knowlege_details_1_string knowlege_details_1_string.cpp
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <cctype>
#include <cstring>
#include <climits>
#include <stack>
#include <cassert>
using namespace std;

// ============================================================================
//  第 1 章：C 风格字符串
// ============================================================================

void demo_c_string() {
    cout << "\n========== 1. C 风格字符串 ==========\n";

    // 声明与初始化
    char str1[] = "Hello";          // 自动加 \0，大小为 6
    char str2[10] = "World";        // 后续位置填 \0

    cout << "str1: " << str1 << endl;
    cout << "str1 sizeof: " << sizeof(str1) << endl;    // 6（含 \0）
    cout << "str1 strlen: " << strlen(str1) << endl;    // 5（不含 \0）

    // C 字符串函数
    char dest[20];
    strcpy(dest, str1);              // 拷贝 "Hello"
    strcat(dest, " ");               // "Hello "
    strcat(dest, str2);              // "Hello World"
    cout << "strcat 结果: " << dest << endl;

    // 比较（返回值：<0, 0, >0）
    cout << "strcmp(\"abc\",\"abd\"): " << strcmp("abc", "abd") << endl;  // < 0

    // 查找子串
    const char* found = strstr(dest, "World");
    if (found) cout << "找到 \"World\" 在位置: " << (found - dest) << endl;

    // ⚠️ C 风格字符串的危险性
    // char tiny[3] = "Hi";   // 正好 3 字节: 'H', 'i', '\0'
    // strcat(tiny, "!!!");    // 缓冲区溢出！未定义行为！
    cout << "⚠️ C 字符串不检查越界，现代 C++ 推荐使用 std::string\n";
}

// ============================================================================
//  第 2 章：std::string 基础
// ============================================================================

void demo_string_basics() {
    cout << "\n========== 2. std::string 基础 ==========\n";

    // --- 2.1 多种创建方式 ---
    string s1;                        // ""（空串）
    string s2 = "Hello";             // 从字面量
    string s3("World");               // 构造函数
    string s4(5, '*');                // "*****"（5 个 '*'）
    string s5(s2);                    // 拷贝 "Hello"
    string s6(s2, 1, 3);            // 从 s2 下标 1 取 3 个 → "ell"
    string s7 = s2 + " " + s3;      // 拼接 → "Hello World"

    cout << "s1 (空): \"" << s1 << "\"" << endl;
    cout << "s2: " << s2 << endl;
    cout << "s4: " << s4 << endl;
    cout << "s6 (s2的子串): " << s6 << endl;
    cout << "s7 (拼接): " << s7 << endl;

    // --- 2.2 大小和容量 ---
    cout << "\ns7.size() = " << s7.size() << endl;        // 11
    cout << "s7.length() = " << s7.length() << endl;      // 11（同 size）
    cout << "s7.capacity() = " << s7.capacity() << endl;   // >= 11
    cout << "s7.empty() = " << s7.empty() << endl;        // 0 (false)
    cout << "s1.empty() = " << s1.empty() << endl;        // 1 (true)
}

// ============================================================================
//  第 3 章：增删改查
// ============================================================================

void demo_string_crud() {
    cout << "\n========== 3. string 增删改查 ==========\n";

    // --- 增加 ---
    string s = "Hello";
    s += " World";                   // "Hello World"
    s.append("!");                   // "Hello World!"
    s.push_back('~');               // "Hello World!~"
    s.insert(5, ",");               // "Hello, World!~"
    cout << "增加后: " << s << endl;

    // --- 删除 ---
    s.pop_back();                    // 删尾部 '~' → "Hello, World!"
    s.erase(5, 1);                  // 删下标 5 的 ',' → "Hello World!"
    cout << "删除后: " << s << endl;

    // --- 修改 ---
    s[0] = 'h';                     // "hello World!"
    s.at(0) = 'H';                  // "Hello World!"（带越界检查）
    s.replace(6, 5, "C++");        // "Hello C++!"
    cout << "修改后: " << s << endl;

    // --- 大小写转换 ---
    string upper = s;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    cout << "大写: " << upper << endl;

    string lower = s;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    cout << "小写: " << lower << endl;

    // --- 查询 ---
    cout << "\nfront: '" << s.front() << "'" << endl;
    cout << "back: '" << s.back() << "'" << endl;
    cout << "substr(6, 3): \"" << s.substr(6, 3) << "\"" << endl;
}

// ============================================================================
//  第 4 章：字符操作
// ============================================================================

void demo_char_operations() {
    cout << "\n========== 4. 字符操作 ==========\n";

    // --- ASCII 值 ---
    cout << "'A' = " << (int)'A' << endl;    // 65
    cout << "'a' = " << (int)'a' << endl;    // 97
    cout << "'0' = " << (int)'0' << endl;    // 48

    // --- 字符类型判断 ---
    cout << "isalpha('A') = " << isalpha('A') << endl;
    cout << "isdigit('3') = " << isdigit('3') << endl;
    cout << "isalnum('a') = " << isalnum('a') << endl;
    cout << "isspace(' ') = " << isspace(' ') << endl;

    // --- 字符 ↔ 数字转换 ---
    int digit = '7' - '0';           // 7
    char ch = 3 + '0';               // '3'
    int pos = 'c' - 'a';             // 2
    cout << "'7' - '0' = " << digit << endl;
    cout << "3 + '0' = '" << ch << "'" << endl;
    cout << "'c' - 'a' = " << pos << endl;

    // --- 频率统计（LeetCode 超常用技巧！）---
    string word = "banana";
    int freq[26] = {};
    for (char c : word) {
        freq[c - 'a']++;   // 'b'-'a'=1, 'a'-'a'=0, 'n'-'a'=13
    }
    cout << "\n\"" << word << "\" 的字符频率:" << endl;
    for (int i = 0; i < 26; i++) {
        if (freq[i] > 0) {
            cout << "  '" << (char)('a' + i) << "': " << freq[i] << endl;
        }
    }
}

// ============================================================================
//  第 5 章：字符串转换
// ============================================================================

void demo_string_conversion() {
    cout << "\n========== 5. 字符串转换 ==========\n";

    // --- 字符串 → 数字 ---
    int n1 = stoi("42");
    long long n2 = stoll("9999999999");
    double d1 = stod("3.14159");
    int hex = stoi("ff", nullptr, 16);    // 255
    int bin = stoi("1010", nullptr, 2);   // 10

    cout << "stoi(\"42\") = " << n1 << endl;
    cout << "stoll(\"9999999999\") = " << n2 << endl;
    cout << "stod(\"3.14159\") = " << d1 << endl;
    cout << "stoi(\"ff\", 16) = " << hex << endl;
    cout << "stoi(\"1010\", 2) = " << bin << endl;

    // --- 数字 → 字符串 ---
    string s1 = to_string(42);
    string s2 = to_string(3.14);
    cout << "to_string(42) = \"" << s1 << "\"" << endl;
    cout << "to_string(3.14) = \"" << s2 << "\"" << endl;

    // --- stringstream ---
    ostringstream oss;
    oss << "Name: " << "Alice" << ", Score: " << 98.5;
    cout << "ostringstream: " << oss.str() << endl;

    // --- 按空格分割 ---
    string line = "hello world foo bar";
    istringstream iss(line);
    string word;
    cout << "按空格分割: ";
    while (iss >> word) {
        cout << "[" << word << "] ";
    }
    cout << endl;

    // --- 按逗号分割 ---
    string csv = "apple,banana,cherry";
    istringstream iss2(csv);
    string token;
    cout << "按逗号分割: ";
    while (getline(iss2, token, ',')) {
        cout << "[" << token << "] ";
    }
    cout << endl;
}

// ============================================================================
//  第 6 章：查找与替换
// ============================================================================

void demo_find_replace() {
    cout << "\n========== 6. 查找与替换 ==========\n";

    string s = "Hello World Hello C++";

    // --- 查找 ---
    cout << "find(\"Hello\"): " << s.find("Hello") << endl;        // 0
    cout << "find(\"Hello\", 1): " << s.find("Hello", 1) << endl;   // 12
    cout << "rfind(\"Hello\"): " << s.rfind("Hello") << endl;       // 12
    cout << "find('o'): " << s.find('o') << endl;                  // 4

    // 找不到返回 npos
    size_t pos = s.find("xyz");
    cout << "find(\"xyz\"): " << (pos == string::npos ? "没找到" : "找到了") << endl;

    // --- find_first_of / find_last_of ---
    string vowels = "aeiouAEIOU";
    cout << "第一个元音位置: " << s.find_first_of(vowels) << endl;
    cout << "最后一个元音位置: " << s.find_last_of(vowels) << endl;

    // --- 替换所有出现 ---
    string text = "foo bar foo baz foo";
    string from = "foo", to = "***";
    size_t p = 0;
    while ((p = text.find(from, p)) != string::npos) {
        text.replace(p, from.size(), to);
        p += to.size();
    }
    cout << "替换所有 \"foo\" → \"***\": " << text << endl;

    // --- C++20: starts_with / ends_with ---
    #if __cplusplus >= 202002L
    string greeting = "Hello World";
    cout << "starts_with(\"Hello\"): " << greeting.starts_with("Hello") << endl;
    cout << "ends_with(\"World\"): " << greeting.ends_with("World") << endl;
    #else
    cout << "(starts_with/ends_with 需要 C++20)\n";
    #endif
}

// ============================================================================
//  第 7 章：比较与排序
// ============================================================================

void demo_compare_sort() {
    cout << "\n========== 7. 字符串比较与排序 ==========\n";

    // --- 比较（字典序）---
    string a = "apple", b = "banana";
    cout << "\"apple\" < \"banana\": " << (a < b ? "true" : "false") << endl;
    cout << "\"apple\" == \"apple\": " << (a == "apple" ? "true" : "false") << endl;

    // --- 字符串内部排序 ---
    string word = "dcba";
    sort(word.begin(), word.end());
    cout << "排序 \"dcba\" → \"" << word << "\"" << endl;  // "abcd"

    // --- 字符串数组排序 ---
    vector<string> words = {"banana", "apple", "cherry", "date"};

    // 默认字典序
    sort(words.begin(), words.end());
    cout << "字典序: ";
    for (auto& w : words) cout << w << " ";
    cout << endl;

    // 按长度排序
    sort(words.begin(), words.end(), [](const string& a, const string& b) {
        return a.size() < b.size();
    });
    cout << "按长度: ";
    for (auto& w : words) cout << w << " ";
    cout << endl;
}

// ============================================================================
//  第 8 章：string_view (C++17)
// ============================================================================

// 零拷贝字符串引用
void processView(string_view sv) {
    cout << "string_view: \"" << sv << "\" size=" << sv.size() << endl;
}

void demo_string_view() {
    cout << "\n========== 8. string_view ==========\n";

    // 从不同来源创建，都是零拷贝
    processView("Hello");             // 从字面量

    string s = "World";
    processView(s);                    // 从 string

    string_view sv = "Hello World";
    string_view sub = sv.substr(6, 5); // "World"，零拷贝！
    cout << "substr: " << sub << endl;

    // string_view 的操作和 string 类似
    cout << "front: " << sv.front() << endl;
    cout << "back: " << sv.back() << endl;
    cout << "find('W'): " << sv.find('W') << endl;

    // ⚠️ 注意：string_view 不拥有数据，要确保底层数据的生命周期
    // ⚠️ string_view 没有 c_str()（不保证 null-terminated）
}

// ============================================================================
//  第 9 章：双指针在字符串中的应用
// ============================================================================

// LC 344: 反转字符串
void reverseString(vector<char>& s) {
    int left = 0, right = (int)s.size() - 1;
    while (left < right) {
        swap(s[left++], s[right--]);
    }
}

// LC 125: 验证回文串（只考虑字母数字，忽略大小写）
bool isPalindrome(string s) {
    int left = 0, right = (int)s.size() - 1;
    while (left < right) {
        // 跳过非字母数字
        while (left < right && !isalnum(s[left])) left++;
        while (left < right && !isalnum(s[right])) right--;

        if (tolower(s[left]) != tolower(s[right]))
            return false;
        left++;
        right--;
    }
    return true;
}

// LC 345: 反转字符串中的元音字母
string reverseVowels(string s) {
    string vowels = "aeiouAEIOU";
    int left = 0, right = (int)s.size() - 1;

    while (left < right) {
        while (left < right && vowels.find(s[left]) == string::npos) left++;
        while (left < right && vowels.find(s[right]) == string::npos) right--;
        if (left < right) swap(s[left++], s[right--]);
    }
    return s;
}

void demo_two_pointers_string() {
    cout << "\n========== 9. 双指针-字符串 ==========\n";

    // 反转字符串
    vector<char> chars = {'h', 'e', 'l', 'l', 'o'};
    reverseString(chars);
    cout << "反转: ";
    for (char c : chars) cout << c;
    cout << endl;  // "olleh"

    // 验证回文
    cout << "\"A man, a plan, a canal: Panama\" 是回文? "
         << (isPalindrome("A man, a plan, a canal: Panama") ? "是" : "否") << endl;
    cout << "\"race a car\" 是回文? "
         << (isPalindrome("race a car") ? "是" : "否") << endl;

    // 反转元音
    cout << "reverseVowels(\"hello\"): " << reverseVowels("hello") << endl;  // "holle"
}

// ============================================================================
//  第 10 章：滑动窗口在字符串中的应用
// ============================================================================

// LC 3: 无重复字符的最长子串
int lengthOfLongestSubstring(string s) {
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
            if (window[c] == need[c]) valid++;
        }

        // 窗口已包含所有目标字符
        while (valid == (int)need.size()) {
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

// LC 242: 有效的字母异位词
bool isAnagram(string s, string t) {
    if (s.size() != t.size()) return false;
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

        // 窗口超过 p 的长度，移除左边
        if (right >= (int)p.size())
            window[s[right - p.size()] - 'a']--;

        // 频率完全匹配
        if (window == need)
            result.push_back(right - (int)p.size() + 1);
    }
    return result;
}

void demo_sliding_window_string() {
    cout << "\n========== 10. 滑动窗口-字符串 ==========\n";

    // 无重复字符的最长子串
    cout << "最长无重复子串 \"abcabcbb\": "
         << lengthOfLongestSubstring("abcabcbb") << endl;  // 3 ("abc")
    cout << "最长无重复子串 \"bbbbb\": "
         << lengthOfLongestSubstring("bbbbb") << endl;     // 1 ("b")
    cout << "最长无重复子串 \"pwwkew\": "
         << lengthOfLongestSubstring("pwwkew") << endl;    // 3 ("wke")

    // 最小覆盖子串
    cout << "minWindow(\"ADOBECODEBANC\", \"ABC\"): "
         << minWindow("ADOBECODEBANC", "ABC") << endl;  // "BANC"

    // 字母异位词
    cout << "isAnagram(\"anagram\",\"nagaram\"): "
         << (isAnagram("anagram", "nagaram") ? "是" : "否") << endl;

    // 所有字母异位词位置
    auto positions = findAnagrams("cbaebabacd", "abc");
    cout << "findAnagrams(\"cbaebabacd\",\"abc\"): ";
    for (int p : positions) cout << p << " ";  // 0 6
    cout << endl;
}

// ============================================================================
//  第 11 章：回文字符串
// ============================================================================

// 判断回文（双指针）
bool isPalin(const string& s, int l, int r) {
    while (l < r) {
        if (s[l++] != s[r--]) return false;
    }
    return true;
}

// LC 5: 最长回文子串（中心扩展法）
string longestPalindrome(string s) {
    int start = 0, maxLen = 1;
    int n = s.size();

    // 从 (left, right) 向两边扩展
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
        expand(i, i);       // 奇数长度：以 i 为中心
        expand(i, i + 1);   // 偶数长度：以 i 和 i+1 为中心
    }

    return s.substr(start, maxLen);
}

// LC 647: 回文子串计数
int countSubstrings(string s) {
    int count = 0;
    int n = s.size();

    auto expand = [&](int left, int right) {
        while (left >= 0 && right < n && s[left] == s[right]) {
            count++;        // 每次扩展成功都是一个回文子串
            left--;
            right++;
        }
    };

    for (int i = 0; i < n; i++) {
        expand(i, i);       // 奇数
        expand(i, i + 1);   // 偶数
    }
    return count;
}

void demo_palindrome() {
    cout << "\n========== 11. 回文字符串 ==========\n";

    cout << "\"racecar\" 是回文? " << (isPalin("racecar", 0, 6) ? "是" : "否") << endl;
    cout << "\"hello\" 是回文? " << (isPalin("hello", 0, 4) ? "是" : "否") << endl;

    cout << "最长回文子串 \"babad\": " << longestPalindrome("babad") << endl;  // "bab" 或 "aba"
    cout << "最长回文子串 \"cbbd\": " << longestPalindrome("cbbd") << endl;    // "bb"

    cout << "回文子串数 \"aaa\": " << countSubstrings("aaa") << endl;   // 6
    cout << "回文子串数 \"abc\": " << countSubstrings("abc") << endl;   // 3
}

// ============================================================================
//  第 12 章：字符串匹配算法
// ============================================================================

// 暴力匹配 O(n*m)
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

// KMP 算法 O(n+m)
// 构建 next 数组（前缀函数）
vector<int> buildNext(const string& pattern) {
    int m = pattern.size();
    vector<int> next(m, 0);
    int j = 0;  // 前缀指针

    for (int i = 1; i < m; i++) {
        // 不匹配时，依据已匹配信息回退
        while (j > 0 && pattern[i] != pattern[j])
            j = next[j - 1];
        // 匹配时，前缀指针前进
        if (pattern[i] == pattern[j])
            j++;
        next[i] = j;
    }
    return next;
}

// KMP 匹配
int kmpSearch(const string& text, const string& pattern) {
    vector<int> next = buildNext(pattern);
    int j = 0;

    for (int i = 0; i < (int)text.size(); i++) {
        while (j > 0 && text[i] != pattern[j])
            j = next[j - 1];
        if (text[i] == pattern[j])
            j++;
        if (j == (int)pattern.size())
            return i - j + 1;   // 找到匹配
    }
    return -1;
}

// 查找所有匹配位置
vector<int> kmpSearchAll(const string& text, const string& pattern) {
    vector<int> result;
    vector<int> next = buildNext(pattern);
    int j = 0;

    for (int i = 0; i < (int)text.size(); i++) {
        while (j > 0 && text[i] != pattern[j])
            j = next[j - 1];
        if (text[i] == pattern[j])
            j++;
        if (j == (int)pattern.size()) {
            result.push_back(i - j + 1);
            j = next[j - 1];   // 继续查找下一个匹配
        }
    }
    return result;
}

void demo_string_matching() {
    cout << "\n========== 12. 字符串匹配算法 ==========\n";

    string text = "ababababca";
    string pattern = "abababca";

    // 暴力匹配
    cout << "暴力匹配 \"" << pattern << "\" 在 \"" << text << "\": "
         << bruteForceSearch(text, pattern) << endl;

    // KMP 匹配
    cout << "KMP 匹配: " << kmpSearch(text, pattern) << endl;

    // next 数组
    string pat2 = "abcabd";
    auto next = buildNext(pat2);
    cout << "\n\"" << pat2 << "\" 的 next 数组: ";
    for (int x : next) cout << x << " ";
    cout << endl;
    // next = [0, 0, 0, 1, 2, 0]

    // 查找所有匹配
    string text2 = "abcabcabc";
    string pat3 = "abc";
    auto allPos = kmpSearchAll(text2, pat3);
    cout << "\"" << pat3 << "\" 在 \"" << text2 << "\" 的所有位置: ";
    for (int p : allPos) cout << p << " ";
    cout << endl;  // 0 3 6
}

// ============================================================================
//  第 13 章：字符串哈希
// ============================================================================

class StringHash {
    vector<long long> hashVal, pw;
    long long MOD = 1e9 + 7, BASE = 131;

public:
    StringHash(const string& s) {
        int n = s.size();
        hashVal.resize(n + 1, 0);
        pw.resize(n + 1, 1);

        for (int i = 0; i < n; i++) {
            hashVal[i + 1] = (hashVal[i] * BASE + s[i]) % MOD;
            pw[i + 1] = (pw[i] * BASE) % MOD;
        }
    }

    // 获取 s[l..r] 的哈希值
    long long getHash(int l, int r) {
        return (hashVal[r + 1] - hashVal[l] * pw[r - l + 1] % MOD + MOD) % MOD;
    }
};

void demo_string_hash() {
    cout << "\n========== 13. 字符串哈希 ==========\n";

    string s = "abcabc";
    StringHash sh(s);

    // 比较 s[0..2] ("abc") 和 s[3..5] ("abc")
    long long h1 = sh.getHash(0, 2);
    long long h2 = sh.getHash(3, 5);
    cout << "hash(\"abc\"[0..2]) = " << h1 << endl;
    cout << "hash(\"abc\"[3..5]) = " << h2 << endl;
    cout << "二者相等: " << (h1 == h2 ? "是" : "否") << endl;

    // 比较 s[0..2] ("abc") 和 s[1..3] ("bca")
    long long h3 = sh.getHash(1, 3);
    cout << "hash(\"bca\"[1..3]) = " << h3 << endl;
    cout << "\"abc\" == \"bca\": " << (h1 == h3 ? "是" : "否") << endl;
}

// ============================================================================
//  第 14 章：经典 LeetCode 题
// ============================================================================

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
            strStack.push(current);   // 保存当前上下文
            numStack.push(num);
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

// LC 14: 最长公共前缀
string longestCommonPrefix(vector<string>& strs) {
    if (strs.empty()) return "";
    string prefix = strs[0];

    for (int i = 1; i < (int)strs.size(); i++) {
        // 不断缩短 prefix，直到它是 strs[i] 的前缀
        while (strs[i].find(prefix) != 0) {
            prefix.pop_back();
            if (prefix.empty()) return "";
        }
    }
    return prefix;
}

// LC 49: 字母异位词分组
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> groups;
    for (string& s : strs) {
        string key = s;
        sort(key.begin(), key.end());  // 排序后作为 key
        groups[key].push_back(s);
    }
    vector<vector<string>> result;
    for (auto& [key, group] : groups) {
        result.push_back(group);
    }
    return result;
}

// LC 72: 编辑距离
int minDistance(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    // 初始化
    for (int i = 0; i <= m; i++) dp[i][0] = i;  // 删除 i 次
    for (int j = 0; j <= n; j++) dp[0][j] = j;  // 插入 j 次

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];      // 相同，不操作
            } else {
                dp[i][j] = min({
                    dp[i - 1][j] + 1,              // 删除
                    dp[i][j - 1] + 1,              // 插入
                    dp[i - 1][j - 1] + 1           // 替换
                });
            }
        }
    }
    return dp[m][n];
}

// LC 139: 单词拆分
bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> wordSet(wordDict.begin(), wordDict.end());
    int n = s.size();
    // dp[i] = s[0..i-1] 能否被拆分
    vector<bool> dp(n + 1, false);
    dp[0] = true;

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            if (dp[j] && wordSet.count(s.substr(j, i - j))) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[n];
}

void demo_classic_string_problems() {
    cout << "\n========== 14. 经典字符串题 ==========\n";

    // 字符串解码
    cout << "decode \"3[a2[c]]\": " << decodeString("3[a2[c]]") << endl;
    cout << "decode \"2[abc]3[cd]ef\": " << decodeString("2[abc]3[cd]ef") << endl;

    // 最长公共前缀
    vector<string> strs = {"flower", "flow", "flight"};
    cout << "最长公共前缀: \"" << longestCommonPrefix(strs) << "\"" << endl;  // "fl"

    // 字母异位词分组
    vector<string> anagrams = {"eat", "tea", "tan", "ate", "nat", "bat"};
    auto groups = groupAnagrams(anagrams);
    cout << "字母异位词分组: " << groups.size() << " 组" << endl;
    for (auto& group : groups) {
        cout << "  [";
        for (int i = 0; i < (int)group.size(); i++) {
            if (i > 0) cout << ", ";
            cout << group[i];
        }
        cout << "]" << endl;
    }

    // 编辑距离
    cout << "编辑距离(\"horse\",\"ros\"): " << minDistance("horse", "ros") << endl;  // 3
    cout << "编辑距离(\"intention\",\"execution\"): "
         << minDistance("intention", "execution") << endl;  // 5

    // 单词拆分
    string s = "leetcode";
    vector<string> dict = {"leet", "code"};
    cout << "单词拆分 \"leetcode\" [\"leet\",\"code\"]: "
         << (wordBreak(s, dict) ? "true" : "false") << endl;
}

// ============================================================================
//  第 15 章：常见陷阱演示
// ============================================================================

void demo_pitfalls() {
    cout << "\n========== 15. 常见陷阱 ==========\n";

    // 陷阱 1：空字符串操作
    string empty;
    cout << "空串 size: " << empty.size() << endl;
    // empty[0];    // ❌ 未定义行为！
    // empty.front(); // ❌ 未定义行为！
    if (!empty.empty()) {
        cout << "first char: " << empty.front() << endl;
    } else {
        cout << "✅ 空串不能直接访问 front/back/[0]" << endl;
    }

    // 陷阱 2：substr 的参数
    string s = "Hello";
    cout << "substr(3, 100): \"" << s.substr(3, 100) << "\"" << endl;  // "lo"（不越界，取到末尾）
    // s.substr(10);  // ❌ 会抛 out_of_range 异常

    // 陷阱 3：字符和整数隐式转换
    string s2 = "A";
    s2 += 66;       // 追加 ASCII 66 = 'B'，不是追加 "66"！
    cout << "\"A\" + (int)66 = \"" << s2 << "\"" << endl;  // "AB"

    // 要追加数字字符串：
    s2 += to_string(66);
    cout << "再 + to_string(66) = \"" << s2 << "\"" << endl;  // "AB66"

    // 陷阱 4：中文字符的大小
    string chinese = "你好";
    cout << "\"你好\" size: " << chinese.size() << "(UTF-8: 一个汉字约3字节)" << endl;
    cout << "⚠️ 处理中文需要专门的 Unicode 库" << endl;

    // 性能建议
    cout << "\n--- 性能最佳实践 ---" << endl;
    cout << "✅ 传参用 const string& 或 string_view" << endl;
    cout << "✅ 大量拼接先 reserve" << endl;
    cout << "✅ 用 push_back('c') 而非 += \"c\"" << endl;
    cout << "✅ 用 int freq[26] 代替 unordered_map<char,int>" << endl;
    cout << "✅ 用 s.compare(i, len, t) 代替 s.substr(i, len) == t" << endl;
}

// ============================================================================
//  主函数：运行所有示例
// ============================================================================

int main() {
    cout << "╔══════════════════════════════════════════╗\n";
    cout << "║   专题一：字符串 (String) 完整示例代码     ║\n";
    cout << "╚══════════════════════════════════════════╝\n";

    demo_c_string();                  // 1. C 风格字符串
    demo_string_basics();             // 2. string 基础
    demo_string_crud();               // 3. 增删改查
    demo_char_operations();           // 4. 字符操作
    demo_string_conversion();         // 5. 字符串转换
    demo_find_replace();              // 6. 查找与替换
    demo_compare_sort();              // 7. 比较与排序
    demo_string_view();               // 8. string_view
    demo_two_pointers_string();       // 9. 双指针
    demo_sliding_window_string();     // 10. 滑动窗口
    demo_palindrome();                // 11. 回文
    demo_string_matching();           // 12. 字符串匹配
    demo_string_hash();               // 13. 字符串哈希
    demo_classic_string_problems();   // 14. 经典题
    demo_pitfalls();                  // 15. 陷阱

    cout << "\n✅ 所有示例运行完毕！\n";
    return 0;
}
