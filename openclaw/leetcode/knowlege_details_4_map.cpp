/**
 * ============================================================
 *   专题四（上）：映射（Map）与哈希表 示例代码
 *   作者：大胖超 😜
 *   编译：g++ -std=c++17 -o knowlege_details_4_map knowlege_details_4_map.cpp
 *   配合 knowlege_details_4_map.md 食用更佳
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <functional>
#include <chrono>
#include <list>
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

template<typename K, typename V>
void printMap(const unordered_map<K, V>& mp, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    cout << "{";
    bool first = true;
    for (auto& [k, v] : mp) {
        if (!first) cout << ", ";
        cout << k << ":" << v;
        first = false;
    }
    cout << "}" << endl;
}

// ============================================================
//  Demo 1: unordered_map 基础用法
//  演示增删查改、遍历、常用 API
// ============================================================
void demo_unordered_map_basics() {
    cout << "\n===== Demo 1: unordered_map 基础 =====" << endl;

    // ---------- 创建与初始化 ----------
    unordered_map<string, int> mp = {
        {"apple", 3},
        {"banana", 5},
        {"cherry", 2}
    };
    cout << "初始: ";
    for (auto& [k, v] : mp) cout << k << ":" << v << " ";
    cout << endl;

    // ---------- 插入 ----------
    mp["date"] = 7;                    // [] 插入/修改
    mp.insert({"elderberry", 1});      // insert
    mp.emplace("fig", 4);             // emplace

    // ---------- 查找 ----------
    cout << "\n--- 查找操作 ---" << endl;

    // count(): 返回 0 或 1
    cout << "count(\"apple\"): " << mp.count("apple") << endl;  // 1
    cout << "count(\"grape\"): " << mp.count("grape") << endl;  // 0

    // find(): 返回迭代器
    auto it = mp.find("banana");
    if (it != mp.end())
        cout << "find(\"banana\"): " << it->second << endl;  // 5

    // ⚠️ [] 的副作用：不存在的 key 会被自动插入！
    cout << "\nmp.size() before: " << mp.size() << endl;
    int val = mp["grape"];  // "grape" 不存在 → 自动插入 {"grape", 0}
    cout << "mp[\"grape\"] = " << val << " (自动插入!)" << endl;
    cout << "mp.size() after: " << mp.size() << endl;

    // ---------- 修改 ----------
    mp["apple"] = 100;
    cout << "\nmp[\"apple\"] 修改为: " << mp["apple"] << endl;

    // ---------- 删除 ----------
    mp.erase("grape");    // 删除刚才意外插入的
    mp.erase("date");
    cout << "erase grape, date → size: " << mp.size() << endl;

    // ---------- 遍历 ----------
    cout << "\n--- 遍历（C++17 结构化绑定）---" << endl;
    for (auto& [key, val] : mp) {
        cout << "  " << key << " → " << val << endl;
    }

    // ---------- 常用 API ----------
    cout << "\n--- 其他 API ---" << endl;
    cout << "size: " << mp.size() << endl;
    cout << "empty: " << (mp.empty() ? "true" : "false") << endl;
    cout << "bucket_count: " << mp.bucket_count() << endl;
    cout << "load_factor: " << mp.load_factor() << endl;
}

// ============================================================
//  Demo 2: map（有序映射）vs unordered_map
//  演示 map 的有序性、lower_bound / upper_bound
// ============================================================
void demo_ordered_map() {
    cout << "\n===== Demo 2: map 有序映射 =====" << endl;

    // map 按 key 升序自动排列
    map<string, int> m;
    m["cherry"] = 2;
    m["apple"]  = 3;
    m["banana"] = 5;
    m["date"]   = 7;

    cout << "map 遍历（按 key 字母序）:" << endl;
    for (auto& [k, v] : m) {
        cout << "  " << k << " → " << v << endl;
    }
    // apple → 3, banana → 5, cherry → 2, date → 7

    // ---------- lower_bound / upper_bound ----------
    cout << "\n--- lower_bound / upper_bound ---" << endl;
    map<int, string> scores = {{60,"C"}, {70,"B"}, {80,"B+"}, {90,"A"}, {100,"A+"}};

    // lower_bound(75): >= 75 的第一个
    auto lo = scores.lower_bound(75);
    cout << "lower_bound(75): " << lo->first << " → " << lo->second << endl; // 80 → B+

    // upper_bound(80): > 80 的第一个
    auto hi = scores.upper_bound(80);
    cout << "upper_bound(80): " << hi->first << " → " << hi->second << endl; // 90 → A

    // 范围查询: [70, 90]
    cout << "范围 [70, 90]: ";
    auto start = scores.lower_bound(70);
    auto end   = scores.upper_bound(90);
    for (auto it = start; it != end; ++it) {
        cout << it->first << " ";
    }
    cout << endl;  // 70 80 90

    // 最小/最大 key
    cout << "最小 key: " << scores.begin()->first << endl;   // 60
    cout << "最大 key: " << scores.rbegin()->first << endl;  // 100
}

// ============================================================
//  Demo 3: 频率计数
//  LC 383: 赎金信
//  LC 350: 两个数组的交集 II
// ============================================================

// LC 383: 判断 ransomNote 能否由 magazine 中的字符构成
bool canConstruct(string ransomNote, string magazine) {
    int freq[26] = {};
    for (char c : magazine) freq[c - 'a']++;
    for (char c : ransomNote) {
        freq[c - 'a']--;
        if (freq[c - 'a'] < 0) return false;
    }
    return true;
}

// LC 350: 两个数组的交集（含重复）
vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
    unordered_map<int, int> freq;
    for (int x : nums1) freq[x]++;

    vector<int> result;
    for (int x : nums2) {
        if (freq[x] > 0) {
            result.push_back(x);
            freq[x]--;
        }
    }
    return result;
}

void demo_frequency_count() {
    cout << "\n===== Demo 3: 频率计数 =====" << endl;

    // LC 383
    cout << "\n--- LC 383: 赎金信 ---" << endl;
    cout << "canConstruct(\"a\", \"b\"): "
         << (canConstruct("a", "b") ? "true" : "false") << endl;          // false
    cout << "canConstruct(\"aa\", \"aab\"): "
         << (canConstruct("aa", "aab") ? "true" : "false") << endl;       // true
    cout << "canConstruct(\"aa\", \"ab\"): "
         << (canConstruct("aa", "ab") ? "true" : "false") << endl;        // false

    // LC 350
    cout << "\n--- LC 350: 两个数组的交集 II ---" << endl;
    vector<int> n1 = {1, 2, 2, 1}, n2 = {2, 2};
    auto res = intersect(n1, n2);
    printVec(n1, "nums1");
    printVec(n2, "nums2");
    printVec(res, "交集");  // [2, 2]

    vector<int> n3 = {4,9,5}, n4 = {9,4,9,8,4};
    auto res2 = intersect(n3, n4);
    printVec(n3, "nums1");
    printVec(n4, "nums2");
    printVec(res2, "交集");  // [4, 9] 或 [9, 4]
}

// ============================================================
//  Demo 4: 两数之和系列
//  LC 1: Two Sum
//  LC 454: 4Sum II
// ============================================================

// LC 1: 两数之和
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> map;  // 值 → 下标
    for (int i = 0; i < (int)nums.size(); i++) {
        int complement = target - nums[i];
        if (map.count(complement))
            return {map[complement], i};
        map[nums[i]] = i;
    }
    return {};
}

// LC 454: 四数相加 II
int fourSumCount(vector<int>& A, vector<int>& B,
                 vector<int>& C, vector<int>& D) {
    unordered_map<int, int> sumAB;
    for (int a : A)
        for (int b : B)
            sumAB[a + b]++;

    int count = 0;
    for (int c : C)
        for (int d : D)
            if (sumAB.count(-(c + d)))
                count += sumAB[-(c + d)];
    return count;
}

void demo_two_sum() {
    cout << "\n===== Demo 4: 两数之和系列 =====" << endl;

    // LC 1
    cout << "\n--- LC 1: 两数之和 ---" << endl;
    vector<int> nums1 = {2, 7, 11, 15};
    int target1 = 9;
    printVec(nums1, "nums");
    cout << "target = " << target1 << endl;
    auto res1 = twoSum(nums1, target1);
    printVec(res1, "结果");  // [0, 1]

    vector<int> nums2 = {3, 2, 4};
    int target2 = 6;
    printVec(nums2, "nums");
    cout << "target = " << target2 << endl;
    auto res2 = twoSum(nums2, target2);
    printVec(res2, "结果");  // [1, 2]

    // LC 454
    cout << "\n--- LC 454: 四数相加 II ---" << endl;
    vector<int> A = {1, 2}, B = {-2, -1}, C = {-1, 2}, D = {0, 2};
    cout << "A = [1,2], B = [-2,-1], C = [-1,2], D = [0,2]" << endl;
    cout << "满足 A+B+C+D=0 的组合数: " << fourSumCount(A, B, C, D) << endl;  // 2
    cout << "解: (0,0,0,1) → 1+(-2)+(-1)+2=0" << endl;
    cout << "    (1,1,0,0) → 2+(-1)+(-1)+0=0" << endl;
}

// ============================================================
//  Demo 5: 字母异位词
//  LC 242: Valid Anagram
//  LC 49: Group Anagrams
// ============================================================

bool isAnagram(string s, string t) {
    if (s.size() != t.size()) return false;
    int cnt[26] = {};
    for (char c : s) cnt[c - 'a']++;
    for (char c : t) {
        cnt[c - 'a']--;
        if (cnt[c - 'a'] < 0) return false;
    }
    return true;
}

vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> groups;

    for (string& s : strs) {
        string key = s;
        sort(key.begin(), key.end());  // "eat" → "aet"
        groups[key].push_back(s);
    }

    vector<vector<string>> result;
    for (auto& [key, group] : groups)
        result.push_back(group);
    return result;
}

void demo_anagram() {
    cout << "\n===== Demo 5: 字母异位词 =====" << endl;

    // LC 242
    cout << "\n--- LC 242: 有效的字母异位词 ---" << endl;
    cout << "isAnagram(\"anagram\", \"nagaram\"): "
         << (isAnagram("anagram", "nagaram") ? "true" : "false") << endl;  // true
    cout << "isAnagram(\"rat\", \"car\"): "
         << (isAnagram("rat", "car") ? "true" : "false") << endl;         // false

    // LC 49
    cout << "\n--- LC 49: 字母异位词分组 ---" << endl;
    vector<string> strs = {"eat", "tea", "tan", "ate", "nat", "bat"};
    cout << "输入: [\"eat\",\"tea\",\"tan\",\"ate\",\"nat\",\"bat\"]" << endl;
    auto groups = groupAnagrams(strs);
    cout << "分组结果:" << endl;
    for (auto& group : groups) {
        cout << "  [";
        for (int i = 0; i < (int)group.size(); i++) {
            if (i) cout << ", ";
            cout << "\"" << group[i] << "\"";
        }
        cout << "]" << endl;
    }
}

// ============================================================
//  Demo 6: 前缀和 + 哈希表
//  LC 560: 和为K的子数组
//  LC 525: 连续数组
// ============================================================

// LC 560: 和为K的子数组
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> prefixCount;
    prefixCount[0] = 1;    // 前缀和本身 = k 的情况
    int sum = 0, count = 0;

    for (int num : nums) {
        sum += num;
        // 存在前缀和 = sum - k → 中间段的和 = k
        if (prefixCount.count(sum - k))
            count += prefixCount[sum - k];
        prefixCount[sum]++;
    }
    return count;
}

// LC 525: 连续数组（最长子数组 0 和 1 个数相等）
int findMaxLength(vector<int>& nums) {
    unordered_map<int, int> firstSeen;
    firstSeen[0] = -1;
    int sum = 0, maxLen = 0;

    for (int i = 0; i < (int)nums.size(); i++) {
        sum += (nums[i] == 0 ? -1 : 1);  // 把 0 变 -1
        if (firstSeen.count(sum)) {
            maxLen = max(maxLen, i - firstSeen[sum]);
        } else {
            firstSeen[sum] = i;
        }
    }
    return maxLen;
}

void demo_prefix_sum() {
    cout << "\n===== Demo 6: 前缀和 + 哈希表 =====" << endl;

    // LC 560
    cout << "\n--- LC 560: 和为K的子数组 ---" << endl;
    vector<int> nums1 = {1, 1, 1};
    int k1 = 2;
    printVec(nums1, "nums");
    cout << "k = " << k1 << " → 子数组个数: " << subarraySum(nums1, k1) << endl;  // 2 ([1,1] 两种)

    vector<int> nums2 = {1, 2, 3};
    int k2 = 3;
    printVec(nums2, "nums");
    cout << "k = " << k2 << " → 子数组个数: " << subarraySum(nums2, k2) << endl;  // 2 ([1,2] 和 [3])

    // LC 525
    cout << "\n--- LC 525: 连续数组（0和1等量最长子数组）---" << endl;
    vector<int> nums3 = {0, 1, 0};
    printVec(nums3, "nums");
    cout << "最长连续数组长度: " << findMaxLength(nums3) << endl;  // 2

    vector<int> nums4 = {0, 1, 0, 0, 1, 1, 0};
    printVec(nums4, "nums");
    cout << "最长连续数组长度: " << findMaxLength(nums4) << endl;  // 6
}

// ============================================================
//  Demo 7: LRU 缓存
//  LC 146: LRU Cache
// ============================================================

class LRUCache {
    struct Node {
        int key, val;
        Node *prev, *next;
        Node(int k, int v) : key(k), val(v), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    unordered_map<int, Node*> map;
    Node *head, *tail;  // 虚拟头尾

    void addToHead(Node* node) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }

    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(Node* node) {
        removeNode(node);
        addToHead(node);
    }

    Node* removeTail() {
        Node* node = tail->prev;
        removeNode(node);
        return node;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
    }

    int get(int key) {
        if (!map.count(key)) return -1;
        Node* node = map[key];
        moveToHead(node);
        return node->val;
    }

    void put(int key, int value) {
        if (map.count(key)) {
            Node* node = map[key];
            node->val = value;
            moveToHead(node);
        } else {
            Node* node = new Node(key, value);
            map[key] = node;
            addToHead(node);
            if ((int)map.size() > capacity) {
                Node* removed = removeTail();
                map.erase(removed->key);
                delete removed;
            }
        }
    }

    // 打印当前缓存（从头到尾 = 最近到最远）
    void print() {
        Node* curr = head->next;
        cout << "[";
        bool first = true;
        while (curr != tail) {
            if (!first) cout << " → ";
            cout << "(" << curr->key << ":" << curr->val << ")";
            first = false;
            curr = curr->next;
        }
        cout << "]" << endl;
    }

    ~LRUCache() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }
};

void demo_lru_cache() {
    cout << "\n===== Demo 7: LRU 缓存 =====" << endl;

    cout << "\n--- LC 146: LRU Cache (容量=2) ---" << endl;
    LRUCache cache(2);

    cache.put(1, 1);
    cout << "put(1,1)   → "; cache.print();  // [(1:1)]

    cache.put(2, 2);
    cout << "put(2,2)   → "; cache.print();  // [(2:2) → (1:1)]

    cout << "get(1) = " << cache.get(1) << endl;  // 1
    cout << "  缓存: "; cache.print();  // [(1:1) → (2:2)]  (1 被访问，移到头部)

    cache.put(3, 3);  // 容量满，淘汰 key=2
    cout << "put(3,3)   → "; cache.print();  // [(3:3) → (1:1)]

    cout << "get(2) = " << cache.get(2) << endl;  // -1（已被淘汰）

    cache.put(4, 4);  // 淘汰 key=1
    cout << "put(4,4)   → "; cache.print();  // [(4:4) → (3:3)]

    cout << "get(1) = " << cache.get(1) << endl;  // -1
    cout << "get(3) = " << cache.get(3) << endl;  // 3
    cout << "get(4) = " << cache.get(4) << endl;  // 4
}

// ============================================================
//  Demo 8: 自定义哈希 & 防 hack & 性能技巧
// ============================================================

// 安全哈希（防针对性 hack 导致的 TLE）
struct SafeHash {
    size_t operator()(int x) const {
        // 加入随机种子，让哈希不可预测
        static const size_t FIXED_RANDOM =
            chrono::steady_clock::now().time_since_epoch().count();
        x ^= FIXED_RANDOM;
        return x ^ (x >> 16);
    }
};

// pair<int,int> 作为 key
struct PairHash {
    size_t operator()(const pair<int,int>& p) const {
        size_t h1 = hash<int>{}(p.first);
        size_t h2 = hash<int>{}(p.second);
        return h1 ^ (h2 << 32);
    }
};

void demo_custom_hash() {
    cout << "\n===== Demo 8: 自定义哈希 & 技巧 =====" << endl;

    // 安全哈希
    cout << "\n--- 安全哈希（防 hack）---" << endl;
    unordered_map<int, int, SafeHash> safeMap;
    for (int i = 0; i < 10; i++) safeMap[i * 100] = i;
    cout << "安全哈希 map 大小: " << safeMap.size() << endl;

    // pair 作为 key
    cout << "\n--- pair<int,int> 作为 key ---" << endl;
    unordered_map<pair<int,int>, string, PairHash> coordMap;
    coordMap[{0, 0}] = "origin";
    coordMap[{1, 2}] = "point A";
    coordMap[{3, 4}] = "point B";
    for (auto& [coord, name] : coordMap) {
        cout << "  (" << coord.first << "," << coord.second << ") → " << name << endl;
    }

    // 字符串编码技巧
    cout << "\n--- 字符串编码代替自定义 hash ---" << endl;
    unordered_map<string, int> encoded;
    auto encode = [](int x, int y) {
        return to_string(x) + "," + to_string(y);
    };
    encoded[encode(1, 2)] = 100;
    encoded[encode(3, 4)] = 200;
    cout << "encode(1,2) = \"" << encode(1,2) << "\" → " << encoded[encode(1,2)] << endl;
    cout << "encode(3,4) = \"" << encode(3,4) << "\" → " << encoded[encode(3,4)] << endl;

    // 性能对比：数组 vs map
    cout << "\n--- 性能提示 ---" << endl;
    cout << "小范围 key (如 0~1000):" << endl;
    cout << "  int freq[1001] = {};  → 数组直接索引，比 map 快 3~5 倍" << endl;
    cout << "字符频率:" << endl;
    cout << "  int charFreq[128] = {};  → 代替 map<char,int>" << endl;

    // 演示 reserve 预分配
    cout << "\n--- reserve 预分配 ---" << endl;
    unordered_map<int,int> mp;
    cout << "默认 bucket_count: " << mp.bucket_count() << endl;
    mp.reserve(10000);
    cout << "reserve(10000) 后 bucket_count: " << mp.bucket_count() << endl;
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "============================================" << endl;
    cout << "  专题四（上）：映射 Map 与哈希表 示例代码" << endl;
    cout << "  作者：大胖超 😜" << endl;
    cout << "============================================" << endl;

    demo_unordered_map_basics();  // Demo 1: unordered_map 基础
    demo_ordered_map();           // Demo 2: map 有序映射
    demo_frequency_count();       // Demo 3: 频率计数
    demo_two_sum();               // Demo 4: 两数之和系列
    demo_anagram();               // Demo 5: 字母异位词
    demo_prefix_sum();            // Demo 6: 前缀和 + 哈希表
    demo_lru_cache();             // Demo 7: LRU 缓存
    demo_custom_hash();           // Demo 8: 自定义哈希

    cout << "\n============================================" << endl;
    cout << "  所有 Map Demo 运行完毕！" << endl;
    cout << "  涉及 LeetCode: 1, 49, 146, 242, 350," << endl;
    cout << "  383, 454, 525, 560" << endl;
    cout << "============================================" << endl;

    return 0;
}
