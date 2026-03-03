/*
 * 专题十五：字典树 Trie — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_15_trie.cpp -o trie
 * 运行：./trie
 *
 * Demo 列表：
 *   Demo1 — LC 208  实现 Trie（前缀树）
 *   Demo2 — LC 211  添加与搜索单词（通配符 '.'）
 *   Demo3 — LC 212  单词搜索 II（Trie + 网格回溯）
 *   Demo4 — LC 648  单词替换（最短前缀匹配）
 *   Demo5 — LC 720  词典中最长的单词（Trie + DFS）
 *   Demo6 — LC 677  键值映射 MapSum
 *   Demo7 — LC 421  数组中两个数的最大异或值（01-Trie）
 *   Demo8 — LC 1268 搜索推荐系统（Trie + DFS 自动补全）
 *   Demo9 — LC 14   最长公共前缀（Trie 解法）
 *   Demo10— 前缀计数 Trie / 数组池实现 / 删除操作 演示
 */

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <cstring>
using namespace std;

/* ============================================================
 * Demo1: LC 208 — 实现 Trie（前缀树）
 * 插入/精确查找/前缀查找
 * ============================================================ */
namespace Demo1 {

class Trie {
    struct TrieNode {
        TrieNode* children[26] = {};
        bool isEnd = false;
    };
    TrieNode* root;

public:
    Trie() : root(new TrieNode()) {}

    void insert(const string& word) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx])
                node->children[idx] = new TrieNode();
            node = node->children[idx];
        }
        node->isEnd = true;
    }

    bool search(const string& word) {
        TrieNode* node = find(word);
        return node && node->isEnd;
    }

    bool startsWith(const string& prefix) {
        return find(prefix) != nullptr;
    }

private:
    TrieNode* find(const string& s) {
        TrieNode* node = root;
        for (char c : s) {
            int idx = c - 'a';
            if (!node->children[idx]) return nullptr;
            node = node->children[idx];
        }
        return node;
    }
};

void run() {
    cout << "===== Demo1: LC 208 实现 Trie =====\n";
    Trie trie;
    trie.insert("apple");
    trie.insert("app");
    trie.insert("apply");
    trie.insert("bat");

    cout << "search(\"apple\"): " << trie.search("apple") << "  (期望 1)\n";
    cout << "search(\"app\"):   " << trie.search("app") << "  (期望 1)\n";
    cout << "search(\"ap\"):    " << trie.search("ap") << "  (期望 0)\n";
    cout << "startsWith(\"ap\"): " << trie.startsWith("ap") << "  (期望 1)\n";
    cout << "search(\"bat\"):   " << trie.search("bat") << "  (期望 1)\n";
    cout << "search(\"ba\"):    " << trie.search("ba") << "  (期望 0)\n";
    cout << "startsWith(\"ba\"): " << trie.startsWith("ba") << "  (期望 1)\n";
    cout << "search(\"xyz\"):   " << trie.search("xyz") << "  (期望 0)\n\n";
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 211 — 添加与搜索单词（支持通配符 '.'）
 * ============================================================ */
namespace Demo2 {

class WordDictionary {
    struct TrieNode {
        TrieNode* children[26] = {};
        bool isEnd = false;
    };
    TrieNode* root;

public:
    WordDictionary() : root(new TrieNode()) {}

    void addWord(const string& word) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx])
                node->children[idx] = new TrieNode();
            node = node->children[idx];
        }
        node->isEnd = true;
    }

    bool search(const string& word) {
        return dfs(root, word, 0);
    }

private:
    bool dfs(TrieNode* node, const string& word, int pos) {
        if (!node) return false;
        if (pos == (int)word.size()) return node->isEnd;

        if (word[pos] == '.') {
            // '.' 匹配任意字符 → 尝试所有子节点
            for (int i = 0; i < 26; i++) {
                if (dfs(node->children[i], word, pos + 1))
                    return true;
            }
            return false;
        } else {
            return dfs(node->children[word[pos] - 'a'], word, pos + 1);
        }
    }
};

void run() {
    cout << "===== Demo2: LC 211 添加与搜索单词 (通配符 '.') =====\n";
    WordDictionary wd;
    wd.addWord("bad");
    wd.addWord("dad");
    wd.addWord("mad");

    cout << "search(\"pad\"): " << wd.search("pad") << "  (期望 0)\n";
    cout << "search(\"bad\"): " << wd.search("bad") << "  (期望 1)\n";
    cout << "search(\".ad\"): " << wd.search(".ad") << "  (期望 1)\n";
    cout << "search(\"b..\"): " << wd.search("b..") << "  (期望 1)\n";
    cout << "search(\"...\"): " << wd.search("...") << "  (期望 1)\n";
    cout << "search(\".a.\"): " << wd.search(".a.") << "  (期望 1)\n";
    cout << "search(\"..d\"): " << wd.search("..d") << "  (期望 1)\n";
    cout << "search(\"..e\"): " << wd.search("..e") << "  (期望 0)\n\n";
}
} // namespace Demo2

/* ============================================================
 * Demo3: LC 212 — 单词搜索 II（Trie + 网格回溯）
 * 在二维字符网格中搜索多个单词
 * ============================================================ */
namespace Demo3 {

class Solution {
    struct TrieNode {
        TrieNode* children[26] = {};
        string word;  // 找到时直接取完整单词
    };

    TrieNode* buildTrie(vector<string>& words) {
        TrieNode* root = new TrieNode();
        for (auto& w : words) {
            TrieNode* node = root;
            for (char c : w) {
                int idx = c - 'a';
                if (!node->children[idx])
                    node->children[idx] = new TrieNode();
                node = node->children[idx];
            }
            node->word = w;
        }
        return root;
    }

public:
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        TrieNode* root = buildTrie(words);
        vector<string> result;
        int m = board.size(), n = board[0].size();

        function<void(int, int, TrieNode*)> dfs = [&](int r, int c, TrieNode* node) {
            if (r < 0 || r >= m || c < 0 || c >= n) return;
            char ch = board[r][c];
            if (ch == '#' || !node->children[ch - 'a']) return;

            node = node->children[ch - 'a'];
            if (!node->word.empty()) {
                result.push_back(node->word);
                node->word.clear();  // 防止重复收集
            }

            board[r][c] = '#';  // 标记已访问
            dfs(r + 1, c, node);
            dfs(r - 1, c, node);
            dfs(r, c + 1, node);
            dfs(r, c - 1, node);
            board[r][c] = ch;   // 恢复
        };

        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                dfs(i, j, root);

        return result;
    }
};

void run() {
    cout << "===== Demo3: LC 212 单词搜索 II (Trie+回溯) =====\n";
    vector<vector<char>> board = {
        {'o','a','a','n'},
        {'e','t','a','e'},
        {'i','h','k','r'},
        {'i','f','l','v'}
    };
    vector<string> words = {"oath", "pea", "eat", "rain"};

    Solution sol;
    auto result = sol.findWords(board, words);

    cout << "board:\n";
    for (auto& row : board) {
        for (char c : row) cout << c << ' ';
        cout << '\n';
    }
    cout << "words: [oath, pea, eat, rain]\n";
    cout << "找到: [";
    sort(result.begin(), result.end());
    for (int i = 0; i < (int)result.size(); i++) {
        if (i) cout << ", ";
        cout << result[i];
    }
    cout << "]  (期望: [eat, oath])\n\n";
}
} // namespace Demo3

/* ============================================================
 * Demo4: LC 648 — 单词替换（最短前缀匹配）
 * ============================================================ */
namespace Demo4 {

string replaceWords(vector<string>& dictionary, string sentence) {
    struct Node {
        Node* ch[26] = {};
        bool isEnd = false;
    };
    Node* root = new Node();

    // 建 Trie
    for (auto& w : dictionary) {
        Node* cur = root;
        for (char c : w) {
            if (!cur->ch[c - 'a']) cur->ch[c - 'a'] = new Node();
            cur = cur->ch[c - 'a'];
        }
        cur->isEnd = true;
    }

    // 对句中每个单词查找最短前缀
    string result, word;
    istringstream iss(sentence);
    while (iss >> word) {
        if (!result.empty()) result += ' ';
        Node* cur = root;
        string prefix;
        bool found = false;
        for (char c : word) {
            if (!cur->ch[c - 'a']) break;
            cur = cur->ch[c - 'a'];
            prefix += c;
            if (cur->isEnd) { found = true; break; }
        }
        result += found ? prefix : word;
    }
    return result;
}

void run() {
    cout << "===== Demo4: LC 648 单词替换 =====\n";
    vector<string> dict = {"cat", "bat", "rat"};
    string sentence = "the cattle was rattled by the battery";
    string res = replaceWords(dict, sentence);
    cout << "dictionary: [cat, bat, rat]\n";
    cout << "sentence: \"" << sentence << "\"\n";
    cout << "替换后: \"" << res << "\"\n";
    cout << "期望:   \"the cat was rat by the bat\"\n\n";
}
} // namespace Demo4

/* ============================================================
 * Demo5: LC 720 — 词典中最长的单词
 * 只沿 isEnd=true 的节点 DFS
 * ============================================================ */
namespace Demo5 {

string longestWord(vector<string>& words) {
    struct Node {
        Node* ch[26] = {};
        bool isEnd = false;
    };
    Node* root = new Node();
    root->isEnd = true;  // 空串也算

    for (auto& w : words) {
        Node* cur = root;
        for (char c : w) {
            if (!cur->ch[c - 'a']) cur->ch[c - 'a'] = new Node();
            cur = cur->ch[c - 'a'];
        }
        cur->isEnd = true;
    }

    // DFS: 只沿 isEnd=true 的路径走
    string result, current;
    function<void(Node*)> dfs = [&](Node* node) {
        if (current.size() > result.size() ||
            (current.size() == result.size() && current < result)) {
            result = current;
        }
        for (int i = 0; i < 26; i++) {
            if (node->ch[i] && node->ch[i]->isEnd) {
                current += ('a' + i);
                dfs(node->ch[i]);
                current.pop_back();
            }
        }
    };
    dfs(root);
    return result;
}

void run() {
    cout << "===== Demo5: LC 720 词典中最长的单词 =====\n";
    {
        vector<string> words = {"w","wo","wor","worl","world"};
        cout << "words: [w, wo, wor, worl, world]\n";
        cout << "结果: \"" << longestWord(words) << "\"  (期望 \"world\")\n";
    }
    {
        vector<string> words = {"a","banana","app","appl","ap","apply","apple"};
        cout << "words: [a, banana, app, appl, ap, apply, apple]\n";
        cout << "结果: \"" << longestWord(words) << "\"  (期望 \"apple\")\n";
    }
    cout << endl;
}
} // namespace Demo5

/* ============================================================
 * Demo6: LC 677 — 键值映射 MapSum
 * 前缀路径上累加 delta
 * ============================================================ */
namespace Demo6 {

class MapSum {
    struct Node {
        Node* ch[26] = {};
        int val = 0;  // 所有经过此节点的 value 之和
    };
    Node* root;
    unordered_map<string, int> mp;  // 记录旧值

public:
    MapSum() : root(new Node()) {}

    void insert(string key, int val) {
        int delta = val - mp[key];
        mp[key] = val;
        Node* cur = root;
        for (char c : key) {
            if (!cur->ch[c - 'a']) cur->ch[c - 'a'] = new Node();
            cur = cur->ch[c - 'a'];
            cur->val += delta;
        }
    }

    int sum(string prefix) {
        Node* cur = root;
        for (char c : prefix) {
            if (!cur->ch[c - 'a']) return 0;
            cur = cur->ch[c - 'a'];
        }
        return cur->val;
    }
};

void run() {
    cout << "===== Demo6: LC 677 键值映射 MapSum =====\n";
    MapSum ms;
    ms.insert("apple", 3);
    cout << "insert(\"apple\", 3)\n";
    cout << "sum(\"ap\"): " << ms.sum("ap") << "  (期望 3)\n";

    ms.insert("app", 2);
    cout << "insert(\"app\", 2)\n";
    cout << "sum(\"ap\"): " << ms.sum("ap") << "  (期望 5)\n";

    ms.insert("apple", 5);  // 更新 value
    cout << "insert(\"apple\", 5)  // 更新\n";
    cout << "sum(\"ap\"): " << ms.sum("ap") << "  (期望 7)\n\n";
}
} // namespace Demo6

/* ============================================================
 * Demo7: LC 421 — 数组中两个数的最大异或值（01-Trie）
 * 贪心：每一位尽量走相反方向
 * ============================================================ */
namespace Demo7 {

class TrieXOR {
    struct Node {
        Node* children[2] = {};
    };
    Node* root;

public:
    TrieXOR() : root(new Node()) {}

    void insert(int num) {
        Node* node = root;
        for (int i = 31; i >= 0; i--) {
            int bit = (num >> i) & 1;
            if (!node->children[bit])
                node->children[bit] = new Node();
            node = node->children[bit];
        }
    }

    int queryMaxXor(int num) {
        Node* node = root;
        int result = 0;
        for (int i = 31; i >= 0; i--) {
            int bit = (num >> i) & 1;
            int want = 1 - bit;  // 贪心：走相反方向
            if (node->children[want]) {
                result |= (1 << i);
                node = node->children[want];
            } else {
                node = node->children[bit];
            }
        }
        return result;
    }
};

int findMaximumXOR(vector<int>& nums) {
    TrieXOR trie;
    for (int x : nums) trie.insert(x);
    int maxXor = 0;
    for (int x : nums)
        maxXor = max(maxXor, trie.queryMaxXor(x));
    return maxXor;
}

void run() {
    cout << "===== Demo7: LC 421 最大异或值 (01-Trie) =====\n";
    {
        vector<int> nums = {3, 10, 5, 25, 2, 8};
        int res = findMaximumXOR(nums);
        cout << "nums: [3, 10, 5, 25, 2, 8]\n";
        cout << "最大异或值: " << res << "  (期望 28, 即 5^25)\n";
    }
    {
        vector<int> nums = {14, 70, 53, 83, 49, 91, 36, 80, 92, 51, 66, 70};
        int res = findMaximumXOR(nums);
        cout << "nums: [14,70,53,83,49,91,36,80,92,51,66,70]\n";
        cout << "最大异或值: " << res << "  (期望 127)\n";
    }
    cout << endl;
}
} // namespace Demo7

/* ============================================================
 * Demo8: LC 1268 — 搜索推荐系统
 * 每输入一个字符，返回最多3个字典序最小的推荐
 * 方法：Trie + 每个节点存最多3个推荐词
 * ============================================================ */
namespace Demo8 {

class Solution {
    struct TrieNode {
        TrieNode* children[26] = {};
        vector<string> suggestions;  // 最多保留3个
    };

public:
    vector<vector<string>> suggestedProducts(vector<string>& products,
                                              string searchWord) {
        // 先排序保证字典序
        sort(products.begin(), products.end());

        // 建 Trie，每个节点最多保留3个推荐
        TrieNode* root = new TrieNode();
        for (auto& p : products) {
            TrieNode* cur = root;
            for (char c : p) {
                int idx = c - 'a';
                if (!cur->children[idx])
                    cur->children[idx] = new TrieNode();
                cur = cur->children[idx];
                if ((int)cur->suggestions.size() < 3)
                    cur->suggestions.push_back(p);
            }
        }

        // 逐字符查询
        vector<vector<string>> result;
        TrieNode* cur = root;
        bool lost = false;
        for (char c : searchWord) {
            if (lost || !cur->children[c - 'a']) {
                lost = true;
                result.push_back({});
            } else {
                cur = cur->children[c - 'a'];
                result.push_back(cur->suggestions);
            }
        }
        return result;
    }
};

void run() {
    cout << "===== Demo8: LC 1268 搜索推荐系统 =====\n";
    vector<string> products = {"mobile","mouse","moneypot","monitor","mousepad"};
    string searchWord = "mouse";

    Solution sol;
    auto result = sol.suggestedProducts(products, searchWord);

    cout << "products: [mobile, mouse, moneypot, monitor, mousepad]\n";
    cout << "searchWord: \"mouse\"\n\n";
    for (int i = 0; i < (int)result.size(); i++) {
        cout << "  输入 \"" << searchWord.substr(0, i + 1) << "\" → [";
        for (int j = 0; j < (int)result[i].size(); j++) {
            if (j) cout << ", ";
            cout << result[i][j];
        }
        cout << "]\n";
    }
    cout << endl;
}
} // namespace Demo8

/* ============================================================
 * Demo9: LC 14 — 最长公共前缀 (Trie 解法)
 * 从根走到第一个分叉点或 isEnd 节点
 * ============================================================ */
namespace Demo9 {

string longestCommonPrefix(vector<string>& strs) {
    if (strs.empty()) return "";

    struct Node {
        Node* ch[26] = {};
        int childCount = 0;
        bool isEnd = false;
    };
    Node* root = new Node();

    for (auto& s : strs) {
        if (s.empty()) return "";  // 空字符串则公共前缀为空
        Node* cur = root;
        for (char c : s) {
            int idx = c - 'a';
            if (!cur->ch[idx]) {
                cur->ch[idx] = new Node();
                cur->childCount++;
            }
            cur = cur->ch[idx];
        }
        cur->isEnd = true;
    }

    // 沿唯一路径走
    string prefix;
    Node* cur = root;
    while (cur->childCount == 1 && !cur->isEnd) {
        for (int i = 0; i < 26; i++) {
            if (cur->ch[i]) {
                prefix += ('a' + i);
                cur = cur->ch[i];
                break;
            }
        }
    }
    return prefix;
}

void run() {
    cout << "===== Demo9: LC 14 最长公共前缀 (Trie) =====\n";
    {
        vector<string> strs = {"flower", "flow", "flight"};
        cout << "[flower, flow, flight] → \"" << longestCommonPrefix(strs) << "\"  (期望 \"fl\")\n";
    }
    {
        vector<string> strs = {"dog", "racecar", "car"};
        cout << "[dog, racecar, car] → \"" << longestCommonPrefix(strs) << "\"  (期望 \"\")\n";
    }
    {
        vector<string> strs = {"abc", "abcd", "abcde"};
        cout << "[abc, abcd, abcde] → \"" << longestCommonPrefix(strs) << "\"  (期望 \"abc\")\n";
    }
    cout << endl;
}
} // namespace Demo9

/* ============================================================
 * Demo10: 综合演示
 *   (a) 前缀计数 Trie（prefixCount / wordCount）
 *   (b) 数组池实现（竞赛常用）
 *   (c) 删除操作
 * ============================================================ */
namespace Demo10 {

// --- (a) 前缀计数 Trie ---
class PrefixCountTrie {
    struct Node {
        Node* ch[26] = {};
        int prefixCount = 0;
        int wordCount = 0;
    };
    Node* root;

public:
    PrefixCountTrie() : root(new Node()) {}

    void insert(const string& word) {
        Node* cur = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!cur->ch[idx]) cur->ch[idx] = new Node();
            cur = cur->ch[idx];
            cur->prefixCount++;
        }
        cur->wordCount++;
    }

    int countWordsEqualTo(const string& word) {
        Node* cur = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!cur->ch[idx]) return 0;
            cur = cur->ch[idx];
        }
        return cur->wordCount;
    }

    int countWordsStartingWith(const string& prefix) {
        Node* cur = root;
        for (char c : prefix) {
            int idx = c - 'a';
            if (!cur->ch[idx]) return 0;
            cur = cur->ch[idx];
        }
        return cur->prefixCount;
    }
};

// --- (b) 数组池 Trie（竞赛常用） ---
namespace ArrayPoolTrie {
    const int MAXN = 100005;
    int trie[MAXN][26];
    bool isEnd[MAXN];
    int cnt;

    void init() {
        cnt = 0;
        memset(trie[0], 0, sizeof(trie[0]));
        isEnd[0] = false;
    }

    void insert(const string& word) {
        int cur = 0;
        for (char c : word) {
            int idx = c - 'a';
            if (!trie[cur][idx]) {
                trie[cur][idx] = ++cnt;
                memset(trie[cnt], 0, sizeof(trie[cnt]));
                isEnd[cnt] = false;
            }
            cur = trie[cur][idx];
        }
        isEnd[cur] = true;
    }

    bool search(const string& word) {
        int cur = 0;
        for (char c : word) {
            int idx = c - 'a';
            if (!trie[cur][idx]) return false;
            cur = trie[cur][idx];
        }
        return isEnd[cur];
    }
}

// --- (c) 支持删除的 Trie ---
class DeletableTrie {
    struct Node {
        Node* ch[26] = {};
        bool isEnd = false;
    };
    Node* root;

    bool hasChildren(Node* node) {
        for (int i = 0; i < 26; i++)
            if (node->ch[i]) return true;
        return false;
    }

    // 返回 true 表示当前节点可以被删除
    bool removeHelper(Node* node, const string& word, int depth) {
        if (!node) return false;

        if (depth == (int)word.size()) {
            if (!node->isEnd) return false;  // 单词不存在
            node->isEnd = false;
            return !hasChildren(node);
        }

        int idx = word[depth] - 'a';
        bool shouldDelete = removeHelper(node->ch[idx], word, depth + 1);

        if (shouldDelete) {
            delete node->ch[idx];
            node->ch[idx] = nullptr;
            return !node->isEnd && !hasChildren(node);
        }
        return false;
    }

public:
    DeletableTrie() : root(new Node()) {}

    void insert(const string& word) {
        Node* cur = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!cur->ch[idx]) cur->ch[idx] = new Node();
            cur = cur->ch[idx];
        }
        cur->isEnd = true;
    }

    bool search(const string& word) {
        Node* cur = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!cur->ch[idx]) return false;
            cur = cur->ch[idx];
        }
        return cur->isEnd;
    }

    void remove(const string& word) {
        removeHelper(root, word, 0);
    }
};

void run() {
    cout << "===== Demo10: 综合 — 前缀计数 / 数组池 / 删除操作 =====\n";

    // (a) 前缀计数
    cout << "--- (a) 前缀计数 Trie ---\n";
    PrefixCountTrie pct;
    pct.insert("apple");
    pct.insert("app");
    pct.insert("apply");
    pct.insert("bat");
    cout << "插入: apple, app, apply, bat\n";
    cout << "countWordsEqualTo(\"app\"): " << pct.countWordsEqualTo("app") << "  (期望 1)\n";
    cout << "countWordsStartingWith(\"app\"): " << pct.countWordsStartingWith("app") << "  (期望 3)\n";
    cout << "countWordsStartingWith(\"b\"): " << pct.countWordsStartingWith("b") << "  (期望 1)\n";
    cout << "countWordsStartingWith(\"ap\"): " << pct.countWordsStartingWith("ap") << "  (期望 3)\n\n";

    // (b) 数组池
    cout << "--- (b) 数组池 Trie ---\n";
    ArrayPoolTrie::init();
    ArrayPoolTrie::insert("hello");
    ArrayPoolTrie::insert("help");
    ArrayPoolTrie::insert("world");
    cout << "插入: hello, help, world\n";
    cout << "search(\"hello\"): " << ArrayPoolTrie::search("hello") << "  (期望 1)\n";
    cout << "search(\"help\"):  " << ArrayPoolTrie::search("help") << "  (期望 1)\n";
    cout << "search(\"hel\"):   " << ArrayPoolTrie::search("hel") << "  (期望 0)\n";
    cout << "search(\"world\"): " << ArrayPoolTrie::search("world") << "  (期望 1)\n";
    cout << "总节点数: " << ArrayPoolTrie::cnt << "  (期望 12)\n\n";

    // (c) 删除
    cout << "--- (c) 支持删除的 Trie ---\n";
    DeletableTrie dt;
    dt.insert("apple");
    dt.insert("app");
    cout << "插入: apple, app\n";
    cout << "search(\"apple\"): " << dt.search("apple") << "  (期望 1)\n";
    cout << "search(\"app\"):   " << dt.search("app") << "  (期望 1)\n";

    dt.remove("apple");
    cout << "删除 \"apple\"\n";
    cout << "search(\"apple\"): " << dt.search("apple") << "  (期望 0)\n";
    cout << "search(\"app\"):   " << dt.search("app") << "  (期望 1)\n";

    dt.remove("app");
    cout << "删除 \"app\"\n";
    cout << "search(\"app\"):   " << dt.search("app") << "  (期望 0)\n";
    cout << endl;
}
} // namespace Demo10

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════╗\n";
    cout << "║   专题十五：字典树 Trie — 可运行示例代码      ║\n";
    cout << "║   作者：大胖超 😜                            ║\n";
    cout << "╚══════════════════════════════════════════════╝\n\n";

    Demo1::run();
    Demo2::run();
    Demo3::run();
    Demo4::run();
    Demo5::run();
    Demo6::run();
    Demo7::run();
    Demo8::run();
    Demo9::run();
    Demo10::run();

    cout << "===== 全部 Demo 运行完毕！=====\n";
    return 0;
}
