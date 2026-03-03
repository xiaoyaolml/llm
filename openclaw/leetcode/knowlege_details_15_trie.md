# 专题十五：字典树 Trie — 前缀树原理与LeetCode实战

> 作者：大胖超 😜
> 
> Trie（字典树/前缀树）是处理字符串集合查询、前缀匹配、自动补全等问题的利器。面试中 LC 208/211/212/720 等高频出现，掌握 Trie 的实现和变体是拿下字符串类难题的关键。

---

## 目录

1. [Trie 基本概念](#1-trie-基本概念)
2. [Trie 核心实现](#2-trie-核心实现)
3. [Trie 操作详解](#3-trie-操作详解)
4. [Trie 变体](#4-trie-变体)
5. [Trie + DFS/回溯](#5-trie--dfs回溯)
6. [Trie 的空间优化](#6-trie-的空间优化)
7. [LeetCode 经典题解](#7-leetcode-经典题解)
8. [Trie vs 其他数据结构](#8-trie-vs-其他数据结构)
9. [题目总结与易错点](#9-题目总结与易错点)

---

## 1. Trie 基本概念

### 1.1 什么是 Trie

Trie（发音为 "try"）是一种**多叉树**，每条边代表一个字符，从根到某节点的路径构成一个字符串前缀。

```
插入 "apple", "app", "apply", "bat", "bad":

            root
           /    \
          a      b
          |      |
          p      a
          |     / \
          p*   t*  d*
         / \
        l   y*
        |
        e*

  * = isEnd 标记（这里是一个完整单词的结尾）

路径含义：
  root→a→p→p = "app" (isEnd=true)
  root→a→p→p→l→e = "apple" (isEnd=true)
  root→b→a→t = "bat" (isEnd=true)
```

### 1.2 Trie 的性质

| 性质 | 说明 |
|------|------|
| 根节点 | 不存储字符，只有children |
| 每条边 | 代表一个字符 |
| 每个节点 | 最多26个子节点（小写英文） |
| 共享前缀 | "app"和"apple"共享"app"路径 |
| isEnd标记 | 区分前缀和完整单词 |

### 1.3 复杂度

| 操作 | 时间 | 空间 |
|------|------|------|
| 插入 | O(L) | O(L×字符集) |
| 查找 | O(L) | — |
| 前缀查找 | O(L) | — |
| 删除 | O(L) | — |

> L = 字符串长度。相比哈希表，Trie 在**前缀查询**方面有独特优势。

### 1.4 Trie 的典型应用

```
应用场景:
├── 自动补全 / 搜索提示
├── 拼写检查
├── IP 路由表（最长前缀匹配）
├── 词频统计
├── 字符串排序
├── 单词搜索（Boggle/Word Search）
└── XOR 最大值（01-Trie）
```

---

## 2. Trie 核心实现

### 2.1 节点结构

```cpp
struct TrieNode {
    TrieNode* children[26] = {};  // 26个子节点初始化为nullptr
    bool isEnd = false;           // 是否是某个单词的结尾
    
    // 可选扩展:
    // int count = 0;             // 经过此节点的单词数
    // int wordCount = 0;         // 以此节点为结尾的单词数
    // string word;               // 存储完整单词(方便取出)
};
```

### 2.2 完整 Trie 类（LC 208）

```cpp
class Trie {
    struct TrieNode {
        TrieNode* children[26] = {};
        bool isEnd = false;
    };
    TrieNode* root;
    
public:
    Trie() : root(new TrieNode()) {}
    
    // 插入单词
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
    
    // 精确查找
    bool search(const string& word) {
        TrieNode* node = find(word);
        return node && node->isEnd;
    }
    
    // 前缀查找
    bool startsWith(const string& prefix) {
        return find(prefix) != nullptr;
    }
    
private:
    // 共用的查找逻辑
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
```

---

## 3. Trie 操作详解

### 3.1 插入过程图解

```
插入 "app":
  root → 'a': 不存在,创建 → 'p': 不存在,创建 → 'p': 不存在,创建
  标记 isEnd = true

  root
   |
   a
   |
   p
   |
   p*  ← isEnd=true

再插入 "apple":
  root → 'a': 存在 → 'p': 存在 → 'p': 存在 → 'l': 不存在,创建 → 'e': 不存在,创建
  标记 isEnd = true

  root
   |
   a
   |
   p
   |
   p* ← "app" 的结尾
   |
   l
   |
   e* ← "apple" 的结尾
```

### 3.2 查找过程

```
search("app"):
  root→a(√)→p(√)→p(√), isEnd=true → 返回 true ✓

search("ap"):
  root→a(√)→p(√), isEnd=false → 返回 false ✗

startsWith("ap"):
  root→a(√)→p(√), 节点存在 → 返回 true ✓

search("abc"):
  root→a(√)→b(nullptr) → 返回 false ✗
```

### 3.3 删除操作（进阶）

Trie 的删除比较复杂，需要递归判断节点是否还有其他用途：

```cpp
bool remove(TrieNode* node, const string& word, int depth) {
    if (!node) return false;
    
    if (depth == (int)word.size()) {
        if (!node->isEnd) return false;  // 单词不存在
        node->isEnd = false;
        // 如果没有子节点，可以删除
        return !hasChildren(node);
    }
    
    int idx = word[depth] - 'a';
    bool shouldDelete = remove(node->children[idx], word, depth + 1);
    
    if (shouldDelete) {
        delete node->children[idx];
        node->children[idx] = nullptr;
        // 当前节点无子节点且非单词结尾 → 可删
        return !node->isEnd && !hasChildren(node);
    }
    return false;
}

bool hasChildren(TrieNode* node) {
    for (int i = 0; i < 26; i++)
        if (node->children[i]) return true;
    return false;
}
```

### 3.4 统计前缀出现次数

```cpp
struct TrieNode {
    TrieNode* children[26] = {};
    int prefixCount = 0;   // 经过此节点的单词数
    int wordCount = 0;     // 以此节点结尾的单词数
};

void insert(const string& word) {
    TrieNode* node = root;
    for (char c : word) {
        int idx = c - 'a';
        if (!node->children[idx])
            node->children[idx] = new TrieNode();
        node = node->children[idx];
        node->prefixCount++;  // 每经过一个节点+1
    }
    node->wordCount++;
}

int countWordsWithPrefix(const string& prefix) {
    TrieNode* node = root;
    for (char c : prefix) {
        int idx = c - 'a';
        if (!node->children[idx]) return 0;
        node = node->children[idx];
    }
    return node->prefixCount;
}
```

---

## 4. Trie 变体

### 4.1 通配符搜索（LC 211）

支持 `.` 匹配任意单个字符：

```cpp
// LC 211: 添加与搜索单词
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
            for (int i = 0; i < 26; i++)
                if (dfs(node->children[i], word, pos + 1))
                    return true;
            return false;
        } else {
            return dfs(node->children[word[pos] - 'a'], word, pos + 1);
        }
    }
};
```

### 4.2 01-Trie（位运算Trie）

用于解决**最大异或值**问题：

```
每个数字按二进制存入 Trie:
  5 = 101
  3 = 011
  7 = 111

       root
      /    \
     0      1
     |     / \
     1    0   1
     |    |   |
     1*   1*  1*
     (3)  (5) (7)

查询最大异或：贪心选择，每一位尽量走相反方向
```

```cpp
// LC 421: 数组中两个数的最大异或值
class TrieXOR {
    struct Node {
        Node* children[2] = {};
    };
    Node* root = new Node();
    
public:
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
            int want = 1 - bit;  // 贪心: 想走相反方向
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
```

### 4.3 HashMap 实现的 Trie

当字符集很大（不止26个字母）时使用 unordered_map：

```cpp
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEnd = false;
};

// 插入
void insert(TrieNode* root, const string& word) {
    TrieNode* node = root;
    for (char c : word) {
        if (!node->children.count(c))
            node->children[c] = new TrieNode();
        node = node->children[c];
    }
    node->isEnd = true;
}
```

> 优点：字符集不限于小写字母，支持 Unicode  
> 缺点：比数组慢（哈希开销），内存碎片

---

## 5. Trie + DFS/回溯

### 5.1 LC 212: 单词搜索 II（Trie + 回溯 经典！）

```
给定二维字符网格和单词列表，找出所有出现在网格中的单词。
每个单词必须通过相邻单元格（上下左右）的字母序列构成。

board = [['o','a','a','n'],
         ['e','t','a','e'],
         ['i','h','k','r'],
         ['i','f','l','v']]
words = ["oath","pea","eat","rain"]

输出: ["eat","oath"]
```

**思路**：将所有单词插入 Trie，然后从网格每个位置开始 DFS，沿着 Trie 走。

```cpp
class Solution {
    struct TrieNode {
        TrieNode* children[26] = {};
        string word;  // 到达时存完整单词
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
                node->word.clear();  // 防止重复
            }
            
            board[r][c] = '#';  // 标记访问
            dfs(r+1, c, node);
            dfs(r-1, c, node);
            dfs(r, c+1, node);
            dfs(r, c-1, node);
            board[r][c] = ch;   // 恢复
        };
        
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                dfs(i, j, root);
        
        return result;
    }
};
```

> **为什么用 Trie 而不是逐个单词搜索？**
> - 逐个搜索：每个单词 DFS 一次 → 总复杂度 O(W × M×N × 4^L)
> - Trie：一次 DFS 可以同时尝试所有单词的前缀 → 大幅剪枝

### 5.2 自动补全 / 前缀搜索所有单词

```cpp
// 返回所有以 prefix 为前缀的单词
vector<string> autocomplete(TrieNode* root, const string& prefix) {
    TrieNode* node = root;
    for (char c : prefix) {
        int idx = c - 'a';
        if (!node->children[idx]) return {};
        node = node->children[idx];
    }
    
    // 从当前节点 DFS 收集所有单词
    vector<string> result;
    string current = prefix;
    function<void(TrieNode*)> dfs = [&](TrieNode* n) {
        if (n->isEnd) result.push_back(current);
        for (int i = 0; i < 26; i++) {
            if (n->children[i]) {
                current += ('a' + i);
                dfs(n->children[i]);
                current.pop_back();
            }
        }
    };
    dfs(node);
    return result;
}
```

---

## 6. Trie 的空间优化

### 6.1 空间问题分析

标准 Trie 每个节点分配 26 个指针（每个 8 字节在 64 位系统上），非常浪费：

```
1000 个平均长度 10 的单词:
  节点数 ≈ 10000
  每节点 26×8 = 208 字节
  总空间 ≈ 2 MB

实际使用的指针 << 26 → 大量 nullptr 浪费空间
```

### 6.2 优化方案

| 方案 | 空间 | 速度 | 适用 |
|------|------|------|------|
| 数组 children[26] | 最浪费 | 最快 | 竞赛/面试 |
| unordered_map | 节省 | 较慢 | 大字符集 |
| 数组池 (静态分配) | 节省 new 开销 | 快 | 竞赛 |
| 压缩Trie | 最省 | 中等 | 实际工程 |

### 6.3 数组池优化（竞赛常用）

```cpp
// 用一个大数组替代 new，避免动态分配
const int MAXN = 100005;  // 最大节点数

int trie[MAXN][26];       // trie[i][c] = 下一节点编号
bool isEnd[MAXN];
int cnt = 0;              // 当前分配的节点编号

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
```

### 6.4 压缩 Trie（Radix Tree / Patricia Tree）

```
普通 Trie:              压缩 Trie:
    root                   root
    |                      |
    a                      app
    |                     / \
    p                   le   ly
    |                   ↓     ↓
    p*                apple* apply*
   / \
  l   y*
  |
  e*

只有一个子节点的路径压缩成一条边。
实际工程（如路由表、数据库索引）广泛使用。
```

---

## 7. LeetCode 经典题解

### 7.1 LC 14: 最长公共前缀

```
虽然可以不用 Trie（两两比较更简单），但 Trie 是优雅解法:
将所有字符串插入 Trie，从根开始走到第一个分叉点。
```

```cpp
string longestCommonPrefix(vector<string>& strs) {
    if (strs.empty()) return "";
    
    struct Node {
        Node* ch[26] = {};
        int childCount = 0;
        bool isEnd = false;
    };
    Node* root = new Node();
    
    // 插入所有字符串
    for (auto& s : strs) {
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
    
    // 从根走到分叉点
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
```

### 7.2 LC 648: 单词替换

```
用 Trie 存储所有词根,遍历句子中每个单词,
在 Trie 中找到最短前缀匹配的词根替换。

dictionary = ["cat","bat","rat"]
sentence = "the cattle was rattled by the battery"
输出: "the cat was rat by the bat"
```

```cpp
string replaceWords(vector<string>& dictionary, string sentence) {
    // 建 Trie
    struct Node {
        Node* ch[26] = {};
        bool isEnd = false;
    };
    Node* root = new Node();
    for (auto& w : dictionary) {
        Node* cur = root;
        for (char c : w) {
            if (!cur->ch[c-'a']) cur->ch[c-'a'] = new Node();
            cur = cur->ch[c-'a'];
        }
        cur->isEnd = true;
    }
    
    // 对句子中每个单词查找最短词根
    string result, word;
    istringstream iss(sentence);
    while (iss >> word) {
        if (!result.empty()) result += ' ';
        Node* cur = root;
        string prefix;
        bool found = false;
        for (char c : word) {
            if (!cur->ch[c-'a']) break;
            cur = cur->ch[c-'a'];
            prefix += c;
            if (cur->isEnd) { found = true; break; }
        }
        result += found ? prefix : word;
    }
    return result;
}
```

### 7.3 LC 720: 词典中最长的单词

```
找字典中能由其他单词一次添加一个字母构成的最长单词。
即: 对于单词 "apple", "a","ap","app","appl" 都必须在字典中。

思路: 插入 Trie 后 BFS/DFS, 只走 isEnd=true 的路径。
```

```cpp
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
            if (!cur->ch[c-'a']) cur->ch[c-'a'] = new Node();
            cur = cur->ch[c-'a'];
        }
        cur->isEnd = true;
    }
    
    // DFS: 只沿 isEnd=true 的节点走
    string result, current;
    function<void(Node*)> dfs = [&](Node* node) {
        if (current.size() > result.size() ||
            (current.size() == result.size() && current < result))
            result = current;
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
```

### 7.4 LC 677: 键值映射（MapSum）

```
插入 key-value 对, 查询所有以 prefix 为前缀的 key 的 value 之和。

insert("apple", 3)
insert("app", 2)
sum("ap") → 3+2 = 5
```

```cpp
class MapSum {
    struct Node {
        Node* ch[26] = {};
        int val = 0;
    };
    Node* root;
    unordered_map<string, int> map;  // 记录旧值以处理更新
    
public:
    MapSum() : root(new Node()) {}
    
    void insert(string key, int val) {
        int delta = val - map[key];
        map[key] = val;
        Node* cur = root;
        for (char c : key) {
            if (!cur->ch[c-'a']) cur->ch[c-'a'] = new Node();
            cur = cur->ch[c-'a'];
            cur->val += delta;  // 路径上每个节点都加 delta
        }
    }
    
    int sum(string prefix) {
        Node* cur = root;
        for (char c : prefix) {
            if (!cur->ch[c-'a']) return 0;
            cur = cur->ch[c-'a'];
        }
        return cur->val;
    }
};
```

---

## 8. Trie vs 其他数据结构

### 8.1 对比表

| 场景 | Trie | HashSet/Map | 排序数组 |
|------|------|------------|---------|
| 精确查找 | O(L) | O(L) 均摊 | O(Llogn) |
| 前缀查找 | O(L) ✅ | O(n×L) ❌ | O(Llogn) |
| 前缀计数 | O(L) ✅ | O(n×L) ❌ | O(Llogn) |
| 通配符搜 | O(26^dots × L) | O(n×L) | 不支持 |
| 自动补全 | O(L+结果) ✅ | 不支持 | O(Llogn) |
| 空间 | 大（每节点26指针）❌ | 小 ✅ | 小 ✅ |
| 实现复杂度 | 中 | 低 | 低 |

### 8.2 什么时候用 Trie

```
必须用 Trie 的场景:
  ✓ 明确要求"前缀匹配"
  ✓ 通配符搜索 (.)
  ✓ 多单词同时在网格中搜索 (LC 212)
  ✓ XOR 最大值 (01-Trie)
  ✓ 前缀频率统计

可以不用 Trie 的场景:
  ✗ 只需要精确匹配 → HashSet
  ✗ 最长公共前缀 → 直接比较更简单
  ✗ 单词量很少 → 暴力即可
```

---

## 9. 题目总结与易错点

### 9.1 题目分类

#### 基础 Trie
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 208 | 实现 Trie | 中等 | 基础实现 |
| 14 | 最长公共前缀 | 简单 | Trie/直接比较 |
| 720 | 词典中最长单词 | 中等 | Trie + DFS |
| 1268 | 搜索推荐系统 | 中等 | Trie + 排序 |

#### 通配符/模式匹配
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 211 | 添加与搜索单词 | 中等 | 通配符 '.' DFS |
| 212 | 单词搜索 II | 困难 | Trie + 网格回溯 |
| 648 | 单词替换 | 中等 | 最短前缀匹配 |

#### 前缀相关
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 677 | 键值映射 | 中等 | 前缀和 + Trie |
| 745 | 前缀和后缀搜索 | 困难 | 双Trie/组合key |
| 1032 | 字符流 | 困难 | 反向Trie |

#### 位运算 Trie
| 题号 | 题目 | 难度 | 考点 |
|------|------|------|------|
| 421 | 最大异或值 | 中等 | 01-Trie 贪心 |
| 1707 | 与数组中元素的最大异或 | 困难 | 01-Trie + 排序 |

### 9.2 高频易错点

```
❌ 1. 忘记标记 isEnd
   只插入了路径但忘记 node->isEnd = true
   → search("app") 在只有 "apple" 时错误返回 true
   ✅ 插入结束后标记 isEnd

❌ 2. search 和 startsWith 混淆
   search 要求 isEnd=true
   startsWith 只要路径存在即可
   ✅ search: return node && node->isEnd
   ✅ startsWith: return node != nullptr

❌ 3. LC 212 重复收集单词
   同一单词被找到多次
   ✅ 找到后 node->word.clear() 或用 set 去重

❌ 4. LC 211 通配符搜索忘记尝试所有分支
   '.' 应该遍历所有 26 个子节点
   ✅ for (int i = 0; i < 26; i++) if (dfs(...)) return true

❌ 5. 内存泄漏
   用 new 创建节点但没有 delete
   ✅ 面试中通常可以忽略，实际工程中用 unique_ptr 或内存池

❌ 6. 字符集超出26个字母
   数字、大写字母、特殊字符 → children[26] 越界
   ✅ 根据实际字符集调整大小或用 map

❌ 7. 01-Trie 中忘记处理高位
   应从最高位(31或30)开始插入
   ✅ for (int i = 31; i >= 0; i--)
```

### 9.3 面试高频问答

> **Q: Trie 空间太大怎么办？**  
> A: 用压缩 Trie (Patricia/Radix Tree)，或用 HashMap 代替数组，或用数组池。

> **Q: Trie 和 Hash 什么时候选哪个？**  
> A: 需要前缀查询/匹配/统计 → Trie。只需要精确查找 → HashSet 更简单高效。

> **Q: 线上系统中 Trie 常用来做什么？**  
> A: 搜索引擎自动补全、输入法候选词、IP路由表最长前缀匹配、字符串去重过滤。

---

> 📌 **配套代码**：[knowlege_details_15_trie.cpp](knowlege_details_15_trie.cpp) 包含上述所有算法的可运行演示。
