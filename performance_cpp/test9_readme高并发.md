# 现代C++高并发处理详细指导教程与代码示例教学

> **定位**：本教程对应 `test9.cpp`（2699 行，18 章），聚焦高并发架构模式与生产级并发组件的设计实现，涵盖从底层 CAS/自旋锁到上层 Actor/MapReduce 的全链路并发技术栈。与 `test6.cpp`（多线程基础）互补、与 `test11.cpp`（无锁编程深入）形成递进。

---

## 编译与运行

```bash
# Linux / macOS — GCC / Clang
g++ -std=c++17 -O2 -pthread -o test9 test9.cpp && ./test9

# Windows — MSVC
cl /std:c++17 /O2 /EHsc test9.cpp && test9.exe
```

> 全部示例仅依赖 C++17 标准库，无第三方依赖。部分章节（第17章）以伪代码/概念说明介绍 C++20 协程。

---

## 目录总览

| 篇 | 章 | 主题 | 核心技术 |
|---|---|---|---|
| **基础设施** | 1 | 并发基础设施 | CAS / SpinLock / 指数退避 / False Sharing / 内存序 |
| **无锁数据结构** | 2 | 无锁栈 | CAS push/pop / 延迟回收 / ABA 防护 |
| | 3 | MPMC 有界队列 | Vyukov 环形缓冲 / 序列号 CAS / 缓存行对齐 |
| **并发容器** | 4 | 并发哈希表 | 分段锁 / shared_mutex / compute_if_absent |
| | 15 | LRU 缓存 | 链表 + HashMap / O(1) 淘汰 / 命中率统计 |
| **线程池** | 5 | Work-Stealing 线程池 | 本地双端队列 / 全局队列 / 任务窃取 |
| **异步组合** | 6 | Future 组合器 | then 链式 / when_all / when_any |
| **并发模式** | 7 | Actor 模型 | 邮箱队列 / variant 消息 / 独立线程 |
| | 8 | Pipeline 流水线 | BoundedBlockingQueue / 多阶段串联 |
| | 9 | Fork-Join 分治 | 并行快排 / 并行求和 / 递归深度控制 |
| | 10 | MapReduce 框架 | Map→Shuffle→Reduce / 词频统计 |
| **流量控制** | 11 | 限流与背压 | 令牌桶 / 滑动窗口 / 高低水位背压 |
| **读优化** | 12 | SeqLock | 序列号读写 / 零读者开销 / atomic vs shared_mutex |
| **定时** | 13 | 定时器轮 | O(1) 添加取消 / 多圈支持 / 槽位链表 |
| **资源池** | 14 | 对象池/连接池 | RAII Lease / 动态扩容 / 阻塞借出 |
| **运维** | 16 | 优雅关停 | 双重检查 / RAII Guard / 超时强停 |
| **前沿** | 17 | C++20 协程概念 | co_await / co_yield / Generator / Task |
| **总结** | 18 | 最佳实践 | 锁层次 / 竞争策略 / thread_local / 调优清单 |

---

## 文件关系与知识图谱

```
test6.cpp (多线程基础)
    │
    ├─ mutex / condition_variable / thread / async
    │
    ▼
test9.cpp (高并发架构) ◄── 本文件
    │
    ├─ 第1章: CAS/SpinLock ──────────────┐
    ├─ 第2章: 无锁栈                      │ 基础层
    ├─ 第3章: MPMC队列                    │
    │                                     ▼
    ├─ 第4章: 并发HashMap ──┐         test11.cpp
    ├─ 第5章: Work-Stealing │ 容器层   (无锁深入)
    ├─ 第15章: LRU缓存  ───┘
    │
    ├─ 第6章: 异步组合器 ───┐
    ├─ 第7章: Actor         │ 架构层
    ├─ 第8章: Pipeline      │
    ├─ 第9章: Fork-Join     │
    ├─ 第10章: MapReduce ───┘
    │
    ├─ 第11章: 限流/背压 ──┐
    ├─ 第12章: SeqLock      │ 运行治理层
    ├─ 第13章: 定时器轮     │
    ├─ 第14章: 对象池    ───┘
    │
    ├─ 第16章: 优雅关停 ───── 运维层
    ├─ 第17章: 协程概念 ───── 前沿层
    └─ 第18章: 最佳实践 ───── 总结层
```

---

## 公共工具

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <functional>
#include <future>
#include <optional>
#include <variant>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstring>

using namespace std::chrono_literals;

// RAII 微秒计时器
struct Timer {
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
    Timer(std::string name) : name_(std::move(name)),
        start_(std::chrono::high_resolution_clock::now()) {}
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << "  [" << name_ << "] " << us << " μs\n";
    }
};

// 缓存行填充原子量 —— 消除 false sharing
constexpr size_t CACHE_LINE_SIZE = 64;

template <typename T>
struct alignas(CACHE_LINE_SIZE) PaddedAtomic {
    std::atomic<T> value{};
    // 填充至完整缓存行，避免相邻变量共享同一行
};
```

> **设计说明**：`PaddedAtomic` 利用 `alignas(64)` 保证每个原子变量独占一条缓存行。在多核高频写场景中，消除 false sharing 可带来 **5-20x** 的吞吐提升。

---

## 第1章：并发基础设施 — CAS / 自旋锁 / False Sharing / 内存序

### 1.1 CAS 原子递增

```cpp
namespace ch1 {

void demo_cas() {
    std::atomic<int> counter{0};
    constexpr int N = 100000;

    auto increment = [&]() {
        for (int i = 0; i < N; ++i) {
            int expected = counter.load(std::memory_order_relaxed);
            // CAS 循环：尝试将 expected → expected+1
            while (!counter.compare_exchange_weak(
                expected, expected + 1,
                std::memory_order_relaxed,
                std::memory_order_relaxed)) {
                // expected 被自动更新为当前值，继续重试
            }
        }
    };

    std::thread t1(increment), t2(increment);
    t1.join(); t2.join();
    std::cout << "  CAS counter: " << counter.load() << " (期望 " << 2 * N << ")\n";
}
```

**CAS 原理 (Compare-And-Swap)**：

```
线程A read → expected = 5
线程B read → expected = 5
线程B CAS(5→6)  ✓ 成功  counter = 6
线程A CAS(5→6)  ✗ 失败  expected 被更新为 6
线程A CAS(6→7)  ✓ 重试成功  counter = 7
```

> `compare_exchange_weak` vs `compare_exchange_strong`：weak 允许伪失败（spurious failure），在循环中性能更好；strong 保证不伪失败，适合单次检查。

### 1.2 自旋锁 — 基础版与指数退避版

```cpp
// 基础自旋锁 —— test_and_set 忙等
class SpinLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 忙等（burn CPU）
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};

// 指数退避自旋锁 —— 减少总线争用
class SpinLockBackoff {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        int backoff = 1;
        while (flag_.test_and_set(std::memory_order_acquire)) {
            for (int i = 0; i < backoff; ++i) {
                #if defined(__x86_64__) || defined(_M_X64)
                    __builtin_ia32_pause();  // x86 PAUSE 指令
                #else
                    std::this_thread::yield();
                #endif
            }
            backoff = std::min(backoff * 2, 1024); // 上限 1024 次 pause
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};
```

**指数退避原理**：

```
尝试1: spin 1 次  → 失败
尝试2: spin 2 次  → 失败
尝试3: spin 4 次  → 失败
尝试4: spin 8 次  → 成功获锁 ✓
      ↑                ↑
  退避时间指数增长    减少总线嗅探（bus snooping）流量
```

> **x86 PAUSE 指令**：告知 CPU "我在自旋等待"，CPU 可降低流水线功耗、减少超线程对兄弟核的干扰。ARM 上对应 `yield` 指令。

### 1.3 False Sharing 演示与消除

```cpp
void demo_false_sharing() {
    constexpr int N = 10000000;

    // ❌ 紧密排列 —— 两个 atomic 落在同一缓存行
    {
        std::atomic<long long> a{0}, b{0}; // 相邻 8 字节
        Timer t("紧密原子量 (false sharing)");
        std::thread t1([&]() { for (int i = 0; i < N; ++i) a.fetch_add(1, std::memory_order_relaxed); });
        std::thread t2([&]() { for (int i = 0; i < N; ++i) b.fetch_add(1, std::memory_order_relaxed); });
        t1.join(); t2.join();
    }

    // ✅ 缓存行填充 —— 各占独立缓存行
    {
        PaddedAtomic<long long> a, b; // 各 64B 对齐
        Timer t("填充原子量 (无 false sharing)");
        std::thread t1([&]() { for (int i = 0; i < N; ++i) a.value.fetch_add(1, std::memory_order_relaxed); });
        std::thread t2([&]() { for (int i = 0; i < N; ++i) b.value.fetch_add(1, std::memory_order_relaxed); });
        t1.join(); t2.join();
    }
}
```

**False Sharing 机制**：

```
缓存行 (64 bytes)
┌──────────────────────────────────┐
│ atomic<a> │ atomic<b> │ padding  │  ← 两变量共享同一行
└──────────────────────────────────┘
   Core0 写 a → 整行失效 → Core1 必须重新加载 b
   Core1 写 b → 整行失效 → Core0 必须重新加载 a
   → 乒乓效应（ping-pong），性能暴跌
```

**填充后**：

```
缓存行0 (64B)          缓存行1 (64B)
┌─────────────┐        ┌─────────────┐
│ PaddedAtomic│        │ PaddedAtomic│
│     <a>     │        │     <b>     │
└─────────────┘        └─────────────┘
Core0 写 a            Core1 写 b
互不干扰 ✓            互不干扰 ✓
```

### 1.4 内存序速查表

| 内存序 | 语义 | 开销 | 典型场景 |
|---|---|---|---|
| `relaxed` | 仅保证原子性 | 最小 | 计数器、标志位 |
| `acquire` | 读后操作不重排到读前 | 中 | 锁的获取、数据加载 |
| `release` | 写前操作不重排到写后 | 中 | 锁的释放、数据发布 |
| `acq_rel` | acquire + release 组合 | 较大 | CAS 读-修改-写 |
| `seq_cst` | 全局顺序一致 | 最大 | 默认（安全但慢） |

### 深入扩展

1. **TTAS (Test-and-Test-and-Set)**：先 `load()` 检查标志，仅当为 false 时才 `test_and_set()`，减少写操作对缓存行的独占
2. **MCS / CLH 队列锁**：每个等待线程在自己的节点上自旋，消除共享标志的竞争，适合 NUMA 架构
3. **C++20 `std::atomic<T>::wait()`**：操作系统级等待，取代纯忙等，减少 CPU 浪费
4. **伪代码 TTAS 扩展**：
   ```cpp
   void lock() {
       while (true) {
           while (flag_.test(std::memory_order_relaxed)) { pause(); } // 只读检查
           if (!flag_.test_and_set(std::memory_order_acquire)) return;  // 尝试获取
       }
   }
   ```
5. **`std::hardware_destructive_interference_size`**：C++17 提供的可移植缓存行大小常量（部分编译器尚未实现）

---

## 第2章：无锁栈 — CAS + 延迟回收

### 2.1 核心实现

```cpp
namespace ch2 {

template <typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(T val) : data(std::move(val)), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};

    // ─── 延迟回收机制 ───
    std::atomic<int> threads_in_pop_{0};   // 当前正在 pop 的线程数
    std::atomic<Node*> to_delete_{nullptr}; // 待删除链表

public:
    void push(T val) {
        auto* node = new Node(std::move(val));
        node->next = head_.load(std::memory_order_relaxed);
        // CAS: 若 head_ 未变则设为 node
        while (!head_.compare_exchange_weak(
            node->next, node,
            std::memory_order_release,
            std::memory_order_relaxed));
    }

    std::optional<T> pop() {
        threads_in_pop_.fetch_add(1, std::memory_order_relaxed);

        Node* old_head = head_.load(std::memory_order_relaxed);
        while (old_head && !head_.compare_exchange_weak(
            old_head, old_head->next,
            std::memory_order_acquire,
            std::memory_order_relaxed));

        std::optional<T> result;
        if (old_head) {
            result = std::move(old_head->data);
            try_reclaim(old_head);
        } else {
            threads_in_pop_.fetch_sub(1, std::memory_order_relaxed);
        }
        return result;
    }

private:
    void try_reclaim(Node* node) {
        if (threads_in_pop_.load(std::memory_order_relaxed) == 1) {
            // 唯一线程 → 安全回收全部待删除节点
            Node* pending = to_delete_.exchange(nullptr, std::memory_order_relaxed);
            if (threads_in_pop_.fetch_sub(1, std::memory_order_relaxed) == 1) {
                delete_nodes(pending);
            } else if (pending) {
                chain_pending_nodes(pending);
            }
            delete node;
        } else {
            // 还有其他线程在 pop → 延迟回收
            chain_pending_nodes(node);
            threads_in_pop_.fetch_sub(1, std::memory_order_relaxed);
        }
    }

    void chain_pending_nodes(Node* nodes) {
        Node* last = nodes;
        while (last->next) last = last->next;
        chain_pending_nodes(nodes, last);
    }

    void chain_pending_nodes(Node* first, Node* last) {
        last->next = to_delete_.load(std::memory_order_relaxed);
        while (!to_delete_.compare_exchange_weak(
            last->next, first, std::memory_order_release, std::memory_order_relaxed));
    }

    static void delete_nodes(Node* nodes) {
        while (nodes) {
            Node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }
};

} // namespace ch2
```

### 2.2 Push / Pop 并发时序

```
Push(10):                    Push(20):
  new Node(10)                 new Node(20)
  node->next = head(null)      node->next = head(null)
  CAS(null→10) ✓               CAS(null→10) ✗ (head已变)
                                node->next = head(10)
  head: [10]→null              CAS(10→20) ✓

  最终: head: [20]→[10]→null
```

**Pop 延迟回收流程**：

```
threads_in_pop = 0

Thread A: pop() → threads_in_pop = 1
  CAS head(20→10) ✓
  threads_in_pop == 1 → 安全回收
  delete Node(20)
  回收 to_delete_ 链表
  threads_in_pop = 0

Thread B: pop() → threads_in_pop = 2 (与 A 并发)
  CAS head → 成功弹出
  threads_in_pop = 2 > 1 → 不安全
  node 加入 to_delete_ 链表（延迟回收）
  threads_in_pop = 1
```

### 深入扩展

1. **Hazard Pointer**：每个线程发布"正在使用"的指针，回收时检查全局 hazard 列表，比延迟回收更精确
2. **Epoch-Based Reclamation (EBR)**：全局 epoch 计数器，节点在所有线程跨过 epoch 后才回收，libcds 采用此方案
3. **ABA 问题深入**：考虑加入版本号 `std::atomic<std::pair<Node*, uint64_t>>` 或使用 `std::atomic<std::shared_ptr<Node>>` (C++20)
4. **Tagged Pointer**：利用指针低位（对齐产生的冗余位）嵌入版本号，避免额外原子操作
5. **内存池优化**：为 Node 预分配固定大小内存池，避免 `new/delete` 在高并发下的锁争用

---

## 第3章：MPMC 有界队列 — Vyukov 环形缓冲

### 3.1 核心实现

```cpp
namespace ch3 {

template <typename T>
class MPMCBoundedQueue {
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    std::vector<Cell> buffer_;
    size_t mask_;   // capacity - 1，用于位运算取模

    // 生产/消费位置各占独立缓存行 → 消除 false sharing
    alignas(64) std::atomic<size_t> enqueue_pos_{0};
    alignas(64) std::atomic<size_t> dequeue_pos_{0};

public:
    explicit MPMCBoundedQueue(size_t capacity) : buffer_(capacity), mask_(capacity - 1) {
        assert((capacity & (capacity - 1)) == 0); // 必须是 2 的幂
        for (size_t i = 0; i < capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    bool try_enqueue(T data) {
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        for (;;) {
            Cell& cell = buffer_[pos & mask_];
            size_t seq = cell.sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)pos;

            if (diff == 0) {
                // 槽位空闲，尝试占据
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1,
                    std::memory_order_relaxed)) {
                    cell.data = std::move(data);
                    cell.sequence.store(pos + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false; // 队列已满
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed); // 重加载
            }
        }
    }

    bool try_dequeue(T& data) {
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        for (;;) {
            Cell& cell = buffer_[pos & mask_];
            size_t seq = cell.sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

            if (diff == 0) {
                // 数据就绪，尝试消费
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1,
                    std::memory_order_relaxed)) {
                    data = std::move(cell.data);
                    cell.sequence.store(pos + mask_ + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false; // 队列为空
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }
    }
};

} // namespace ch3
```

### 3.2 Vyukov 队列序列号状态机

```
容量 = 4，mask_ = 3

初始:  seq: [0] [1] [2] [3]    enqueue_pos=0  dequeue_pos=0

入队(A): pos=0, seq[0]=0, diff=0→CAS成功
  seq[0] = 0+1 = 1             → 标记"已填充"
  seq: [1] [1] [2] [3]         enqueue_pos=1

入队(B): pos=1, seq[1]=1, diff=0→CAS成功
  seq[1] = 1+1 = 2
  seq: [1] [2] [2] [3]         enqueue_pos=2

出队: pos=0, seq[0]=1, diff=1-(0+1)=0→CAS成功
  取走 A, seq[0] = 0+3+1 = 4  → 标记"可复用"
  seq: [4] [2] [2] [3]         dequeue_pos=1

  下次入队到 pos=4 (4&3=0), 检查 seq[0]=4==pos ✓ → 可写入
```

> **关键设计**：`sequence` 既充当槽位状态标记，又作为 ABA 计数器。生产者检查 `seq == pos` 表示空；消费者检查 `seq == pos+1` 表示有数据。

### 3.3 Demo — 4生产者 × 4消费者

```cpp
void demo_mpmc_queue() {
    MPMCBoundedQueue<int> q(1024);
    constexpr int PER_THREAD = 100000;
    constexpr int PRODUCERS = 4, CONSUMERS = 4;

    std::atomic<int> produced{0}, consumed{0};

    // 4 个生产者
    std::vector<std::thread> producers;
    for (int i = 0; i < PRODUCERS; ++i) {
        producers.emplace_back([&, i]() {
            for (int j = 0; j < PER_THREAD; ++j) {
                while (!q.try_enqueue(i * PER_THREAD + j))
                    std::this_thread::yield();
                produced.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 4 个消费者
    std::vector<std::thread> consumers;
    for (int i = 0; i < CONSUMERS; ++i) {
        consumers.emplace_back([&]() {
            int val;
            while (consumed.load(std::memory_order_relaxed) < PRODUCERS * PER_THREAD) {
                if (q.try_dequeue(val))
                    consumed.fetch_add(1, std::memory_order_relaxed);
                else
                    std::this_thread::yield();
            }
        });
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    std::cout << "  produced=" << produced.load()
              << " consumed=" << consumed.load() << "\n";
}
```

### 深入扩展

1. **SPSC 队列优化**：单生产者单消费者可去掉所有 CAS，仅用 `relaxed` load/store + `acquire/release` fence，吞吐量可达 **数亿 ops/s**
2. **动态扩容**：当前 `try_enqueue` 返回 false 时丢弃；可实现多段链表式无界队列（如 `boost::lockfree::queue`）
3. **`moodycamel::ConcurrentQueue`**：生产级 MPMC 队列，内部使用隐式生产者令牌 + 块链表，减少 CAS 争用
4. **批量入队/出队**：`try_enqueue_bulk()` 一次占据多个连续槽位，减少原子操作次数
5. **NUMA 感知**：在 NUMA 架构下为每个 NUMA 节点分配独立队列，跨节点交互走全局队列

---

## 第4章：并发哈希表 — 分段锁

### 4.1 核心实现

```cpp
namespace ch4 {

template <typename K, typename V, int NUM_STRIPES = 16>
class ConcurrentHashMap {
    struct Stripe {
        std::shared_mutex mutex;
        std::unordered_map<K, V> map;
    };

    std::array<Stripe, NUM_STRIPES> stripes_;

    Stripe& get_stripe(const K& key) {
        auto h = std::hash<K>{}(key);
        return stripes_[h % NUM_STRIPES];
    }

public:
    void put(const K& key, V value) {
        auto& stripe = get_stripe(key);
        std::unique_lock lock(stripe.mutex);
        stripe.map[key] = std::move(value);
    }

    std::optional<V> get(const K& key) {
        auto& stripe = get_stripe(key);
        std::shared_lock lock(stripe.mutex);  // 读锁 → 多读者并行
        auto it = stripe.map.find(key);
        if (it != stripe.map.end()) return it->second;
        return std::nullopt;
    }

    bool erase(const K& key) {
        auto& stripe = get_stripe(key);
        std::unique_lock lock(stripe.mutex);
        return stripe.map.erase(key) > 0;
    }

    // 先读锁查找，缺失时升级为写锁插入
    V compute_if_absent(const K& key, std::function<V()> factory) {
        auto& stripe = get_stripe(key);
        {
            std::shared_lock lock(stripe.mutex);
            auto it = stripe.map.find(key);
            if (it != stripe.map.end()) return it->second;
        }
        // 升级为写锁
        std::unique_lock lock(stripe.mutex);
        auto [it, inserted] = stripe.map.try_emplace(key, factory());
        return it->second;
    }

    // 快照遍历（非一致性）
    void for_each(std::function<void(const K&, const V&)> fn) {
        for (auto& stripe : stripes_) {
            std::shared_lock lock(stripe.mutex);
            for (auto& [k, v] : stripe.map) fn(k, v);
        }
    }

    size_t size() const {
        size_t total = 0;
        for (auto& stripe : stripes_) {
            std::shared_lock lock(const_cast<std::shared_mutex&>(stripe.mutex));
            total += stripe.map.size();
        }
        return total;
    }
};

} // namespace ch4
```

### 4.2 分段锁架构图

```
ConcurrentHashMap  (16 Stripes)
┌────────────────────────────────────────────────┐
│ Stripe 0      │ Stripe 1      │ ... │ Stripe 15│
│ shared_mutex   │ shared_mutex   │     │          │
│ unordered_map  │ unordered_map  │     │          │
└────────────────────────────────────────────────┘

hash("key_A") % 16 = 3  → 只锁 Stripe 3
hash("key_B") % 16 = 7  → 只锁 Stripe 7
→ A 和 B 操作完全并行，零竞争！

对比: 单锁 HashMap
┌─────────────────────────────────────────┐
│        一把 mutex 锁住整个 map           │
│    所有线程串行化 → 吞吐暴跌            │
└─────────────────────────────────────────┘
```

### 4.3 compute_if_absent 读写锁升级

```
Thread A: compute_if_absent("K")
  ① shared_lock → 查找 → 未找到
  ② 释放 shared_lock
  ③ unique_lock → try_emplace(K, factory())
     ↑ 双重检查：另一线程可能已插入

Thread B: compute_if_absent("K") (并发)
  ① shared_lock → 查找 → 未找到
  ② 释放 shared_lock
  ③ unique_lock (等待 A 完成)
  ④ try_emplace → 已存在 → 返回已有值 ✓
```

### 深入扩展

1. **`std::concurrent_unordered_map` (提案)**：C++ 标准委员会正在讨论的并发容器接口设计
2. **RCU (Read-Copy-Update)**：读者完全无锁、写者拷贝整个结构后原子替换，适合读写比 > 1000:1
3. **分段数量调优**：`NUM_STRIPES` 通常设为 `2 × hardware_concurrency`，过多浪费内存，过少增加竞争
4. **一致性迭代**：当前 `for_each` 逐段加锁，不保证全局一致性；如需一致快照，可对所有段加读锁后遍历
5. **动态扩容**：当单段 load factor > 0.75 时自动 rehash，需在写锁保护下执行

---

## 第5章：Work-Stealing 线程池

### 5.1 Work-Stealing 双端队列

```cpp
namespace ch5 {

class WorkStealingDeque {
    static constexpr int CAPACITY = 4096;
    std::array<std::function<void()>, CAPACITY> buffer_;
    std::atomic<int> top_{0};    // 窃取端
    std::atomic<int> bottom_{0}; // 持有者端

public:
    // 持有者从 bottom 端压入
    void push(std::function<void()> task) {
        int b = bottom_.load(std::memory_order_relaxed);
        buffer_[b % CAPACITY] = std::move(task);
        std::atomic_thread_fence(std::memory_order_release);
        bottom_.store(b + 1, std::memory_order_relaxed);
    }

    // 持有者从 bottom 端弹出 (LIFO — 缓存友好)
    std::function<void()> pop() {
        int b = bottom_.load(std::memory_order_relaxed) - 1;
        bottom_.store(b, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        int t = top_.load(std::memory_order_relaxed);

        if (t <= b) {
            auto task = std::move(buffer_[b % CAPACITY]);
            if (t == b) {
                // 最后一个元素 → CAS 竞争
                if (!top_.compare_exchange_strong(t, t + 1,
                    std::memory_order_seq_cst, std::memory_order_relaxed)) {
                    bottom_.store(b + 1, std::memory_order_relaxed);
                    return nullptr;
                }
                bottom_.store(b + 1, std::memory_order_relaxed);
            }
            return task;
        }
        bottom_.store(b + 1, std::memory_order_relaxed);
        return nullptr;
    }

    // 窃取者从 top 端窃取 (FIFO — 大粒度任务优先)
    std::function<void()> steal() {
        int t = top_.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        int b = bottom_.load(std::memory_order_acquire);

        if (t < b) {
            auto task = buffer_[t % CAPACITY];
            if (top_.compare_exchange_strong(t, t + 1,
                std::memory_order_seq_cst, std::memory_order_relaxed)) {
                return task;
            }
        }
        return nullptr;
    }
};
```

### 5.2 线程池 — 三级任务获取

```cpp
class WorkStealingPool {
    int num_threads_;
    std::vector<std::thread> threads_;
    std::vector<std::unique_ptr<WorkStealingDeque>> local_queues_;

    // 全局后备队列
    std::queue<std::function<void()>> global_queue_;
    std::mutex global_mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{false};
    std::atomic<int> task_count_{0};

    static thread_local int thread_index_;

public:
    WorkStealingPool(int n = std::thread::hardware_concurrency())
        : num_threads_(n) {
        for (int i = 0; i < n; ++i)
            local_queues_.push_back(std::make_unique<WorkStealingDeque>());
        for (int i = 0; i < n; ++i)
            threads_.emplace_back([this, i]() { worker_loop(i); });
    }

    template <typename F>
    auto submit(F&& f) -> std::future<decltype(f())> {
        using R = decltype(f());
        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(f));
        auto future = task->get_future();
        task_count_.fetch_add(1, std::memory_order_relaxed);

        auto wrapper = [task, this]() {
            (*task)();
            task_count_.fetch_sub(1, std::memory_order_relaxed);
        };

        // 优先投递到提交线程的本地队列
        if (thread_index_ >= 0 && thread_index_ < num_threads_) {
            local_queues_[thread_index_]->push(std::move(wrapper));
        } else {
            std::lock_guard lock(global_mutex_);
            global_queue_.push(std::move(wrapper));
            cv_.notify_one();
        }
        return future;
    }

private:
    void worker_loop(int index) {
        thread_index_ = index;
        while (!stop_.load(std::memory_order_relaxed)) {
            std::function<void()> task;

            // ① 本地队列 (LIFO，缓存友好)
            task = local_queues_[index]->pop();

            // ② 全局队列
            if (!task) {
                std::lock_guard lock(global_mutex_);
                if (!global_queue_.empty()) {
                    task = std::move(global_queue_.front());
                    global_queue_.pop();
                }
            }

            // ③ 窃取其他线程的任务 (FIFO，大任务优先)
            if (!task) {
                for (int i = 0; i < num_threads_; ++i) {
                    if (i == index) continue;
                    task = local_queues_[i]->steal();
                    if (task) break;
                }
            }

            if (task) {
                task();
            } else {
                std::unique_lock lock(global_mutex_);
                cv_.wait_for(lock, 1ms);
            }
        }
    }
};

thread_local int WorkStealingPool::thread_index_ = -1;

} // namespace ch5
```

### 5.3 三级任务获取示意图

```
Worker 0                Worker 1                Worker 2
┌──────────┐           ┌──────────┐           ┌──────────┐
│ LocalDeq │           │ LocalDeq │           │ LocalDeq │
│ [T5,T4]  │           │ [T8]     │           │ []       │  ← 空！
└────┬─────┘           └────┬─────┘           └────┬─────┘
     │                      │                      │
     │ ① pop (LIFO)         │ ① pop               │ ① pop → null
     │ → T5 ✓               │ → T8 ✓              │
     │                      │                      │ ② global queue
     │                      │             ┌────────┤ → null
     │                      │             │        │
     │                      │    ┌────────▼──────┐ │ ③ steal from W0
     │                      │    │ Global Queue  │ │ → T4 ✓ (FIFO)
     │                      │    │ [T9, T10]     │ │
     │                      │    └───────────────┘ │
     ▼                      ▼                      ▼
```

> **为什么 LIFO pop + FIFO steal？**：持有者 pop 最近压入的任务（缓存热数据），窃取者 steal 最早的任务（通常粒度更大，可进一步拆分）。这是 Intel TBB / Java ForkJoinPool 的经典策略。

### 深入扩展

1. **动态扩容 Deque**：Chase-Lev 论文的 deque 支持 `grow()`，当 buffer 满时分配双倍数组并迁移
2. **优先级调度**：为任务附加优先级，本地队列改用优先队列
3. **亲和性绑定**：`pthread_setaffinity_np()` / `SetThreadAffinityMask()` 将 worker 绑定到特定核心，减少迁移开销
4. **等待策略优化**：空闲线程先自旋 N 次 → yield → 条件变量阻塞，三级降功耗
5. **分层窃取**：NUMA 架构下优先窃取同 NUMA 节点的 worker，跨节点窃取延迟高

---

## 第6章：异步组合器 — then / when_all / when_any

### 6.1 实现

```cpp
namespace ch6 {

// 简单线程池用于异步执行
class SimplePool {
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
public:
    SimplePool(int n = 4) {
        for (int i = 0; i < n; ++i)
            threads_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    { std::unique_lock lock(mutex_);
                      cv_.wait(lock, [this]{ return stop_ || !tasks_.empty(); });
                      if (stop_ && tasks_.empty()) return;
                      task = std::move(tasks_.front());
                      tasks_.pop(); }
                    task();
                }
            });
    }
    ~SimplePool() { { std::lock_guard l(mutex_); stop_ = true; } cv_.notify_all(); for(auto&t:threads_)t.join(); }

    template <typename F>
    void execute(F&& f) {
        { std::lock_guard lock(mutex_); tasks_.push(std::forward<F>(f)); }
        cv_.notify_one();
    }
};

// then: future<T> + (T→R) → future<R>
template <typename T, typename F>
auto then(std::future<T>& fut, F&& f) -> std::future<decltype(f(fut.get()))> {
    using R = decltype(f(std::declval<T>()));
    auto promise = std::make_shared<std::promise<R>>();
    auto future = promise->get_future();

    std::thread([p = std::move(promise), fut = std::move(fut), f = std::forward<F>(f)]() mutable {
        try {
            if constexpr (std::is_void_v<R>) {
                f(fut.get());
                p->set_value();
            } else {
                p->set_value(f(fut.get()));
            }
        } catch (...) {
            p->set_exception(std::current_exception());
        }
    }).detach();

    return future;
}

// when_all: vector<future<T>> → future<vector<T>>
template <typename T>
std::future<std::vector<T>> when_all(std::vector<std::future<T>>& futures) {
    auto promise = std::make_shared<std::promise<std::vector<T>>>();
    auto future = promise->get_future();

    std::thread([p = std::move(promise), futs = std::move(futures)]() mutable {
        std::vector<T> results;
        results.reserve(futs.size());
        for (auto& f : futs) results.push_back(f.get());
        p->set_value(std::move(results));
    }).detach();

    return future;
}

// when_any: vector<future<T>> → future<T> (第一个完成的)
template <typename T>
std::future<T> when_any(std::vector<std::future<T>>& futures) {
    auto promise = std::make_shared<std::promise<T>>();
    auto future = promise->get_future();
    auto done = std::make_shared<std::atomic<bool>>(false);

    for (auto& f : futures) {
        std::thread([p = promise, d = done, f = std::move(f)]() mutable {
            T result = f.get();
            if (!d->exchange(true)) {
                p->set_value(std::move(result));
            }
        }).detach();
    }
    return future;
}

} // namespace ch6
```

### 6.2 组合器数据流

```
then 链式:
  future<int>(10) ──then(×2)──► future<int>(20) ──then(+5)──► future<int>(25)

when_all 汇聚:
  future(1²=1)  ─┐
  future(2²=4)  ─┤
  future(3²=9)  ─┼──when_all──► future<vector>([1,4,9,16,25])
  future(4²=16) ─┤
  future(5²=25) ─┘

when_any 竞速:
  future(fast:  50ms) ─┐
  future(medium:150ms) ─┼──when_any──► future(first_result)
  future(slow:  300ms) ─┘
     ↑                        ↑
  atomic<bool> done       第一个完成的线程 set_value
  后续完成的线程 → exchange(true) → 发现已完成 → 跳过
```

### 深入扩展

1. **`std::experimental::when_all` / `when_any`**：Concurrency TS 提供的标准实现，返回 `when_any_result` 包含 index
2. **Continuation 池化**：当前 `then` 每次 detach 一个线程，生产代码应改用线程池执行续延
3. **异常传播**：`when_any` 中第一个 future 若抛异常，应传播给调用者而非静默丢弃
4. **超时组合器**：`when_any_or_timeout(futures, 100ms)` — 设置截止时间，超时返回 `std::nullopt`
5. **C++23 `std::execution` (P2300)**：Sender/Receiver 模型取代 future 组合器，零分配、更好的编译期优化

---

## 第7章：Actor 模型

### 7.1 核心实现

```cpp
namespace ch7 {

using Message = std::variant<std::string, int, std::pair<std::string, int>>;

class Actor {
    std::queue<Message> mailbox_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread thread_;
    std::atomic<bool> running_{false};

public:
    virtual ~Actor() { stop(); }

    void send(Message msg) {
        std::lock_guard lock(mutex_);
        mailbox_.push(std::move(msg));
        cv_.notify_one();
    }

    void start() {
        running_ = true;
        thread_ = std::thread([this]() {
            while (running_) {
                Message msg;
                {
                    std::unique_lock lock(mutex_);
                    cv_.wait(lock, [this]() { return !mailbox_.empty() || !running_; });
                    if (!running_ && mailbox_.empty()) return;
                    msg = std::move(mailbox_.front());
                    mailbox_.pop();
                }
                on_receive(msg);
            }
        });
    }

    void stop() {
        running_ = false;
        cv_.notify_all();
        if (thread_.joinable()) thread_.join();
    }

protected:
    virtual void on_receive(const Message& msg) = 0;
};

class CounterActor : public Actor {
    int count_ = 0;
protected:
    void on_receive(const Message& msg) override {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                if (arg == "increment") ++count_;
                else if (arg == "reset") count_ = 0;
                else if (arg == "print")
                    std::cout << "    CounterActor: " << count_ << "\n";
            } else if constexpr (std::is_same_v<T, std::pair<std::string, int>>) {
                if (arg.first == "add") count_ += arg.second;
            }
        }, msg);
    }
};

} // namespace ch7
```

### 7.2 Actor 消息流

```
Thread A                 CounterActor              LoggerActor
   │                    ┌──────────┐              ┌──────────┐
   │ send("increment")  │ Mailbox  │              │ Mailbox  │
   ├────────────────────►│ [incr]   │              │          │
   │ send("increment")  │ [incr]   │              │          │
   ├────────────────────►│ [incr,i] │              │          │
   │                    │          │              │          │
   │                    │ on_recv  │              │          │
   │                    │ count=1  │              │          │
   │                    │ count=2  │              │          │
   │ send(add, 5)       │          │              │          │
   ├────────────────────►│ [add:5]  │              │          │
   │                    │ count=7  │              │          │
   │ send("print")      │          │              │          │
   ├────────────────────►│ [print]  │              │          │
   │                    │ → "7"    │              │          │
   │                    └──────────┘              └──────────┘
```

> **核心思想**：每个 Actor 有自己的状态和邮箱，通过消息传递通信，内部无共享状态 → 天然线程安全。

### 深入扩展

1. **Actor 地址与路由**：为每个 Actor 分配唯一 ID，实现 `ActorSystem::send(actor_id, msg)`，支持跨进程/跨机器寻址
2. **监督树 (Supervision Tree)**：Erlang / Akka 风格，父 Actor 监控子 Actor 异常并重启
3. **背压集成**：邮箱容量限制，满时发送者阻塞或丢弃（结合第11章）
4. **批量消息处理**：一次取出多条消息批量处理，减少锁开销
5. **类型安全消息**：用模板参数化消息类型 `Actor<InMsg, OutMsg>`，编译期检查消息匹配

---

## 第8章：Pipeline 流水线

### 8.1 有界阻塞队列

```cpp
namespace ch8 {

template <typename T>
class BoundedBlockingQueue {
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    size_t capacity_;
    bool closed_ = false;

public:
    explicit BoundedBlockingQueue(size_t cap = 64) : capacity_(cap) {}

    void push(T item) {
        std::unique_lock lock(mutex_);
        not_full_.wait(lock, [this]() { return queue_.size() < capacity_ || closed_; });
        if (closed_) return;
        queue_.push(std::move(item));
        not_empty_.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock lock(mutex_);
        not_empty_.wait(lock, [this]() { return !queue_.empty() || closed_; });
        if (queue_.empty()) return std::nullopt;
        T item = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return item;
    }

    void close() {
        std::lock_guard lock(mutex_);
        closed_ = true;
        not_empty_.notify_all();
        not_full_.notify_all();
    }
};
```

### 8.2 流水线演示

```cpp
// Pipeline 演示：Producer → Stage1(×2) → Stage2(to_string) → Consumer
static void demo() {
    constexpr int N = 1000;
    BoundedBlockingQueue<int>         q1(32); // Producer → Stage1
    BoundedBlockingQueue<int>         q2(32); // Stage1   → Stage2
    BoundedBlockingQueue<std::string> q3(32); // Stage2   → Consumer

    std::thread stage1([&]() {
        while (auto val = q1.pop()) {
            q2.push(*val * 2);
        }
        q2.close();
    });

    std::thread stage2([&]() {
        while (auto val = q2.pop()) {
            q3.push("result_" + std::to_string(*val));
        }
        q3.close();
    });

    std::thread producer([&]() {
        for (int i = 0; i < N; ++i) q1.push(i);
        q1.close();
    });

    int count = 0;
    while (auto val = q3.pop()) { ++count; }

    producer.join(); stage1.join(); stage2.join();
    std::cout << "  Pipeline 处理: " << count << " 条\n";
}
```

### 8.3 流水线数据流

```
Producer        Stage 1 (×2)      Stage 2 (str)     Consumer
┌─────┐       ┌──────┐          ┌──────┐          ┌──────┐
│ 0   │──q1──►│ 0→0  │───q2───►│ 0→"r_0" │──q3──►│ count│
│ 1   │       │ 1→2  │         │ 2→"r_2" │       │  ++  │
│ 2   │       │ 2→4  │         │ 4→"r_4" │       │      │
│ ... │       │ ...  │         │ ...     │       │      │
│ 999 │       │999→1998│       │1998→"r_1998"│   │=1000 │
└──┬──┘       └──────┘          └──────┘          └──────┘
   │                                                  │
close() → q1.close() → stage1 退出 → q2.close() → stage2 退出 → q3.close()
   └──────────────── 级联关闭传播 ───────────────────┘
```

> **背压自动生效**：`BoundedBlockingQueue` 容量仅 32，当消费者慢于生产者时，`push()` 自动阻塞 — 天然背压。

### 深入扩展

1. **多线程阶段**：每个 Stage 可启动 N 个线程并行处理，适合 CPU 密集阶段
2. **分支/汇合拓扑**：一个阶段输出到多个下游（scatter）、多个阶段汇入一个（gather）
3. **批量传输**：每次传递 `vector<T>` 而非单个 T，减少同步开销
4. **动态管道**：运行时增删阶段，实现热更新处理逻辑
5. **错误处理**：传递 `variant<T, Error>` 而非裸值，支持错误沿管道传播

---

## 第9章：Fork-Join 并行分治

### 9.1 并行快速排序

```cpp
namespace ch9 {

template <typename Iter>
void parallel_sort(Iter begin, Iter end, int depth = 0) {
    auto len = std::distance(begin, end);
    if (len <= 1) return;

    // 小数组或递归过深 → 退化为串行
    if (len < 10000 || depth > 4) {
        std::sort(begin, end);
        return;
    }

    // 三路分区
    auto pivot = *std::next(begin, len / 2);
    auto mid1 = std::partition(begin, end, [pivot](const auto& x) { return x < pivot; });
    auto mid2 = std::partition(mid1, end, [pivot](const auto& x) { return !(pivot < x); });

    // Fork: 异步排序左半部分
    auto left = std::async(std::launch::async, [begin, mid1, depth]() {
        parallel_sort(begin, mid1, depth + 1);
    });

    // 当前线程排序右半部分
    parallel_sort(mid2, end, depth + 1);

    // Join: 等待左半部分完成
    left.get();
}
```

### 9.2 并行求和

```cpp
template <typename Iter>
auto parallel_sum(Iter begin, Iter end, int depth = 0)
    -> typename std::iterator_traits<Iter>::value_type
{
    using T = typename std::iterator_traits<Iter>::value_type;
    auto len = std::distance(begin, end);

    if (len == 0) return T{};
    if (len < 10000 || depth > 4) {
        return std::accumulate(begin, end, T{});
    }

    auto mid = std::next(begin, len / 2);
    auto left = std::async(std::launch::async, [begin, mid, depth]() {
        return parallel_sum(begin, mid, depth + 1);
    });

    T right = parallel_sum(mid, end, depth + 1);
    return left.get() + right;
}

} // namespace ch9
```

### 9.3 Fork-Join 递归树

```
parallel_sort([0..999999])
├── async: parallel_sort([0..499999])       ← Fork
│   ├── async: parallel_sort([0..249999])
│   │   ├── async: [0..124999]
│   │   └── [125000..249999]
│   └── [250000..499999]
└── parallel_sort([500000..999999])          ← 当前线程
    ├── async: [500000..749999]
    └── [750000..999999]
                    │
                    ▼
        left.get()  ← Join (等待所有子任务完成)

深度控制 (depth > 4 → 串行):
  避免创建过多线程（2^depth 个）
  depth=4 → 最多 16 个并发线程，与核心数匹配
```

### 9.4 Demo 对比

```cpp
void demo_fork_join() {
    constexpr int N = 1000000;
    std::vector<int> data(N);
    std::mt19937 rng(42);
    std::generate(data.begin(), data.end(), [&rng]() {
        return std::uniform_int_distribution<int>(0, N)(rng);
    });
    auto data_copy = data;

    { Timer t("std::sort 串行");       std::sort(data.begin(), data.end()); }
    { Timer t("parallel_sort 并行"); parallel_sort(data_copy.begin(), data_copy.end()); }

    std::cout << "  排序结果一致: " << std::boolalpha << (data == data_copy) << "\n";

    // 并行求和
    std::vector<long long> nums(N);
    std::iota(nums.begin(), nums.end(), 1LL);

    long long serial_sum, par_sum;
    { Timer t("串行求和"); serial_sum = std::accumulate(nums.begin(), nums.end(), 0LL); }
    { Timer t("并行求和"); par_sum = parallel_sum(nums.begin(), nums.end()); }

    std::cout << "  求和结果一致: " << (serial_sum == par_sum) << "\n";
}
```

### 深入扩展

1. **C++17 `std::execution::par`**：`std::sort(std::execution::par, begin, end)` 标准并行算法，内部使用 Fork-Join
2. **自适应阈值**：根据 `hardware_concurrency()` 动态计算切分阈值（`N / (4 × cores)`）
3. **Work-Stealing 集成**：将 Fork-Join 任务提交到第5章的 WorkStealingPool，避免 `std::async` 的线程创建开销
4. **并行归并排序**：归并阶段也可并行化，使用 `std::inplace_merge` + 并行
5. **内存局部性**：分治时保持数据连续性，避免缓存行失效，可用 `std::partition` 的 3-way 版本

---

## 第10章：MapReduce 框架

### 10.1 核心实现

```cpp
namespace ch10 {

template <typename InputT, typename KeyT, typename ValueT, typename ResultT>
class MapReduce {
public:
    using MapFunc = std::function<std::vector<std::pair<KeyT, ValueT>>(const InputT&)>;
    using ReduceFunc = std::function<ResultT(const KeyT&, const std::vector<ValueT>&)>;

    MapReduce(MapFunc mapper, ReduceFunc reducer, int num_threads = 4)
        : mapper_(mapper), reducer_(reducer), num_threads_(num_threads) {}

    std::map<KeyT, ResultT> execute(const std::vector<InputT>& inputs) {
        // 1. Map 阶段：并行分片处理
        size_t chunk_size = (inputs.size() + num_threads_ - 1) / num_threads_;
        std::vector<std::future<std::vector<std::pair<KeyT, ValueT>>>> map_futures;

        for (size_t i = 0; i < inputs.size(); i += chunk_size) {
            size_t end = std::min(i + chunk_size, inputs.size());
            map_futures.push_back(std::async(std::launch::async,
                [this, &inputs, i, end]() {
                    std::vector<std::pair<KeyT, ValueT>> results;
                    for (size_t j = i; j < end; ++j) {
                        auto pairs = mapper_(inputs[j]);
                        results.insert(results.end(),
                            std::make_move_iterator(pairs.begin()),
                            std::make_move_iterator(pairs.end()));
                    }
                    return results;
                }));
        }

        // 2. Shuffle 阶段：按 key 分组
        std::map<KeyT, std::vector<ValueT>> grouped;
        for (auto& future : map_futures) {
            for (auto& [key, value] : future.get()) {
                grouped[key].push_back(std::move(value));
            }
        }

        // 3. Reduce 阶段：并行聚合
        std::vector<std::future<std::pair<KeyT, ResultT>>> reduce_futures;
        std::vector<std::pair<KeyT, std::vector<ValueT>>> groups(
            grouped.begin(), grouped.end());

        for (auto& [key, values] : groups) {
            reduce_futures.push_back(std::async(std::launch::async,
                [this, &key, &values]() -> std::pair<KeyT, ResultT> {
                    return {key, reducer_(key, values)};
                }));
        }

        std::map<KeyT, ResultT> results;
        for (auto& f : reduce_futures) {
            auto [key, result] = f.get();
            results[key] = std::move(result);
        }
        return results;
    }

private:
    MapFunc mapper_;
    ReduceFunc reducer_;
    int num_threads_;
};

} // namespace ch10
```

### 10.2 MapReduce 执行流程

```
Input Documents:
  D1: "the quick brown fox"
  D2: "the fox the dog"
  D3: "a quick brown dog"

═══ Map 阶段 (并行) ═══
  Worker1 → D1 → [(the,1),(quick,1),(brown,1),(fox,1)]
  Worker2 → D2 → [(the,1),(fox,1),(the,1),(dog,1)]
  Worker3 → D3 → [(a,1),(quick,1),(brown,1),(dog,1)]

═══ Shuffle 阶段 (分组) ═══
  a:     [1]
  brown: [1, 1]
  dog:   [1, 1]
  fox:   [1, 1]
  quick: [1, 1]
  the:   [1, 1, 1]

═══ Reduce 阶段 (并行) ═══
  a:     sum([1])      = 1
  brown: sum([1,1])    = 2
  dog:   sum([1,1])    = 2
  fox:   sum([1,1])    = 2
  quick: sum([1,1])    = 2
  the:   sum([1,1,1])  = 3    ← 最高频词
```

### 10.3 词频统计 Demo

```cpp
void demo_mapreduce() {
    std::vector<std::string> documents = {
        "the quick brown fox jumps over the lazy dog",
        "the fox the dog the quick fox",
        "a quick brown dog outfoxes a lazy fox",
        "the dog sat on the fox and the fox sat on the dog",
    };

    using MR = MapReduce<std::string, std::string, int, int>;

    MR mr(
        // Map: 文本 → [(word, 1), ...]
        [](const std::string& doc) -> std::vector<std::pair<std::string, int>> {
            std::vector<std::pair<std::string, int>> result;
            std::istringstream iss(doc);
            std::string word;
            while (iss >> word) result.emplace_back(word, 1);
            return result;
        },
        // Reduce: (word, [1,1,...]) → count
        [](const std::string&, const std::vector<int>& values) -> int {
            return std::accumulate(values.begin(), values.end(), 0);
        }
    );

    auto results = mr.execute(documents);
    // 输出 Top 10
}
```

### 深入扩展

1. **Combiner 优化**：在 Map 阶段本地预聚合，减少 Shuffle 数据量（如 Hadoop 的 Combiner）
2. **分布式扩展**：将输入分片发送到不同机器，Shuffle 通过网络交换，Reduce 在目标机器执行
3. **内存映射输入**：用 `mmap` 读取大文件，避免整体加载到内存
4. **自定义 Partitioner**：控制 key 分配到哪个 Reduce 节点，优化数据局部性
5. **流式 MapReduce**：结合第8章 Pipeline，实现持续输入的增量处理

---

## 第11章：限流与背压

### 11.1 令牌桶限流器

```cpp
namespace ch11 {

class TokenBucketLimiter {
    double rate_;          // 每秒补充令牌数
    double capacity_;      // 桶容量（突发上限）
    double tokens_;        // 当前令牌数
    std::chrono::steady_clock::time_point last_time_;
    std::mutex mutex_;

public:
    TokenBucketLimiter(double rate, double capacity)
        : rate_(rate), capacity_(capacity), tokens_(capacity),
          last_time_(std::chrono::steady_clock::now()) {}

    bool try_acquire(int permits = 1) {
        std::lock_guard lock(mutex_);
        refill();
        if (tokens_ >= permits) {
            tokens_ -= permits;
            return true;
        }
        return false;
    }

    void acquire(int permits = 1) {
        while (!try_acquire(permits)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    void refill() {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - last_time_).count();
        tokens_ = std::min(capacity_, tokens_ + elapsed * rate_);
        last_time_ = now;
    }
};
```

**令牌桶原理**：

```
令牌桶 (capacity=100, rate=1000/s)
┌────────────────────────┐
│ ●●●●●●●● (tokens=100) │ ← 初始满桶
│                         │
│ 每秒补充 1000 个令牌    │
│ 超过 capacity 不补充    │
└────────────────────────┘

请求到达 → try_acquire()
  tokens >= 1 → 消耗1个令牌 → 通过 ✓
  tokens < 1  → 拒绝 ✗

特性：允许突发 (burst) 流量消耗存量令牌
     长期速率稳定在 rate/s
```

### 11.2 滑动窗口限流器

```cpp
class SlidingWindowLimiter {
    int max_requests_;
    std::chrono::milliseconds window_;
    std::deque<std::chrono::steady_clock::time_point> timestamps_;
    std::mutex mutex_;

public:
    SlidingWindowLimiter(int max_requests, std::chrono::milliseconds window)
        : max_requests_(max_requests), window_(window) {}

    bool try_acquire() {
        std::lock_guard lock(mutex_);
        auto now = std::chrono::steady_clock::now();

        // 清除窗口外的历史时间戳
        while (!timestamps_.empty() && now - timestamps_.front() > window_) {
            timestamps_.pop_front();
        }

        if ((int)timestamps_.size() < max_requests_) {
            timestamps_.push_back(now);
            return true;
        }
        return false;
    }
};
```

**滑动窗口 vs 令牌桶**：

```
滑动窗口 (max=5, window=1s):
  时间轴: ─────[────────1s窗口────────]─────
  请求:       * * * * *   (5个) → 第6个被拒绝
  移除窗口外: ← 过期的自动移出

令牌桶 (rate=5/s, cap=10):
  允许突发: 瞬间通过10个请求 (存量令牌)
  稳态: 每秒仅5个

适用场景:
  滑动窗口 → API 限流 (严格窗口内限制)
  令牌桶   → 网络流控 (允许合理突发)
```

### 11.3 背压队列

```cpp
template <typename T>
class BackpressureQueue {
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    size_t capacity_;
    size_t high_watermark_; // 高水位 → 通知减速
    size_t low_watermark_;  // 低水位 → 恢复生产
    std::atomic<bool> paused_{false};
    bool closed_ = false;

public:
    BackpressureQueue(size_t capacity = 1024,
                       size_t high_water = 768,
                       size_t low_water = 256)
        : capacity_(capacity), high_watermark_(high_water), low_watermark_(low_water) {}

    bool push(T item) {
        std::unique_lock lock(mutex_);
        not_full_.wait(lock, [this]() { return queue_.size() < capacity_ || closed_; });
        if (closed_) return false;
        queue_.push(std::move(item));
        if (queue_.size() >= high_watermark_) {
            paused_.store(true, std::memory_order_relaxed); // 触发背压
        }
        not_empty_.notify_one();
        return true;
    }

    std::optional<T> pop() {
        std::unique_lock lock(mutex_);
        not_empty_.wait(lock, [this]() { return !queue_.empty() || closed_; });
        if (queue_.empty()) return std::nullopt;
        T item = std::move(queue_.front());
        queue_.pop();
        if (queue_.size() <= low_watermark_) {
            paused_.store(false, std::memory_order_relaxed); // 解除背压
            not_full_.notify_all();
        }
        return item;
    }

    bool is_paused() const { return paused_.load(std::memory_order_relaxed); }

    void close() {
        std::lock_guard lock(mutex_);
        closed_ = true;
        not_full_.notify_all();
        not_empty_.notify_all();
    }
};
```

**高低水位机制**：

```
队列容量 = 1024
                  ┌───────────────── capacity (1024)
                  │
  ████████████████│█████████████ ← 高水位 (768): paused=true
  ██████████████  │               生产者减速/阻塞
  ████████████    │
  ██████████      │
  ████████        │
  ██████ ─────────┼───────────── ← 低水位 (256): paused=false
  ████            │               恢复生产
  ██              │
  ┴───────────────┼─── 0
                  │
```

> **迟滞效应**：高低水位之间的间隔避免了在边界值附近频繁切换暂停/恢复状态。

### 深入扩展

1. **分布式限流**：Redis + Lua 脚本实现集群级令牌桶，多节点共享限流状态
2. **漏桶算法 (Leaky Bucket)**：匀速处理，无突发，适合流量整形（如网卡 QoS）
3. **自适应限流**：根据系统负载（CPU/队列深度/延迟P99）动态调整限流阈值
4. **多级背压传播**：Service A → Queue → Service B → Queue → Service C，背压逐级向上游传播
5. **优先级限流**：高优先级请求分配更多令牌，低优先级请求节流

---

## 第12章：SeqLock — 读优化利器

### 12.1 核心实现

```cpp
namespace ch12 {

class SeqLock {
    std::atomic<uint64_t> seq_{0}; // 偶数 = 无写操作

public:
    void write_lock() {
        seq_.fetch_add(1, std::memory_order_release); // 奇数 → 正在写
    }
    void write_unlock() {
        seq_.fetch_add(1, std::memory_order_release); // 偶数 → 写完成
    }

    uint64_t read_begin() const {
        uint64_t s;
        do {
            s = seq_.load(std::memory_order_acquire);
        } while (s & 1); // 奇数表示正在写 → 等待
        return s;
    }

    bool read_validate(uint64_t start_seq) const {
        std::atomic_thread_fence(std::memory_order_acquire);
        return seq_.load(std::memory_order_relaxed) == start_seq;
    }
};

// SeqLock 保护的行情数据
struct MarketData {
    double price;
    double volume;
    uint64_t timestamp;
};

class MarketDataStore {
    SeqLock lock_;
    MarketData data_{0, 0, 0};

public:
    void update(double price, double volume, uint64_t ts) {
        lock_.write_lock();
        data_.price = price;
        data_.volume = volume;
        data_.timestamp = ts;
        lock_.write_unlock();
    }

    MarketData read() const {
        MarketData result;
        uint64_t seq;
        do {
            seq = lock_.read_begin();
            result = data_;                    // 拷贝数据
        } while (!lock_.read_validate(seq));   // 验证无并发写
        return result;
    }
};

} // namespace ch12
```

### 12.2 SeqLock 时序分析

```
seq: 0 (偶数=安全)

Writer:                         Reader:
  write_lock() → seq=1 (奇数)
  data.price = 100.5            read_begin() → seq=1 (奇数!)
  data.volume = 5000              → 等待... spin
  data.timestamp = 42
  write_unlock() → seq=2 (偶数)  read_begin() → seq=2 ✓
                                  result = data (拷贝)
                                  read_validate(2) → seq仍=2 ✓
                                  返回有效数据 ✓

并发写干扰:
  Reader: read_begin() → seq=2
          result = data (拷贝中...)
  Writer: write_lock() → seq=3 (正在写!)
          修改 data...
  Reader: read_validate(2) → seq=3 ≠ 2 ✗
          → 重试整个读操作
```

> **关键特性**：读者零开销（无锁、无原子写、无 CAS），写者不被读者阻塞。代价是读者可能重试，但在读多写少场景下摊销成本极低。

### 12.3 Atomic vs shared_mutex 基准对比

```cpp
void demo_read_counters() {
    constexpr int N = 5000000;

    // std::atomic (读写都有开销)
    {
        std::atomic<long long> counter{0};
        Timer t("atomic 读计数器");
        std::thread writer([&] {
            for (int i = 0; i < N; ++i) counter.store(i, std::memory_order_release);
        });
        std::thread reader([&] {
            long long sum = 0;
            for (int i = 0; i < N; ++i) sum += counter.load(std::memory_order_acquire);
        });
        writer.join(); reader.join();
    }

    // shared_mutex (读锁有开销)
    {
        long long counter = 0;
        std::shared_mutex mtx;
        Timer t("shared_mutex 读计数器");
        std::thread writer([&] {
            for (int i = 0; i < N; ++i) { std::unique_lock lk(mtx); counter = i; }
        });
        std::thread reader([&] {
            long long sum = 0;
            for (int i = 0; i < N; ++i) { std::shared_lock lk(mtx); sum += counter; }
        });
        writer.join(); reader.join();
    }
}
```

**对比总结**：

| 机制 | 读开销 | 写开销 | 适用场景 |
|---|---|---|---|
| `SeqLock` | ~0 (可能重试) | fetch_add × 2 | 小数据 + 读极多写极少 |
| `std::atomic` | load (cache line) | store | 单值读写 |
| `shared_mutex` | 内核调用 | 内核调用 | 大临界区、长持有时间 |

### 深入扩展

1. **多版本 SeqLock**：维护 N 个版本缓冲，写者写下一个版本，读者读最新完整版本，彻底消除重试
2. **LMAX Disruptor**：金融交易系统中的环形缓冲，基于序列号的无锁发布-订阅
3. **RCU (Read-Copy-Update)**：Linux 内核的读优化机制，读者完全无锁，写者拷贝+替换+延迟回收
4. **SeqLock 的局限**：不适合大数据结构（拷贝成本高）、不适合 string 等含指针类型（拷贝时指针可能已失效）
5. **`std::atomic_ref` (C++20)**：对非 atomic 变量进行原子操作，可用于 SeqLock 保护的 POD 优化

---

## 第13章：定时器轮 (Timer Wheel)

### 13.1 核心实现

```cpp
namespace ch13 {

class TimerWheel {
    using Callback = std::function<void()>;

    struct TimerTask {
        int id;
        int remaining_rounds; // 剩余圈数
        Callback callback;
    };

    int num_slots_;
    int current_slot_ = 0;
    int interval_ms_;
    int next_id_ = 0;

    std::vector<std::list<TimerTask>> slots_;
    std::mutex mutex_;
    std::thread ticker_;
    std::atomic<bool> running_{false};

public:
    TimerWheel(int num_slots = 60, int interval_ms = 100)
        : num_slots_(num_slots), interval_ms_(interval_ms),
          slots_(num_slots) {}

    ~TimerWheel() { stop(); }

    void start() {
        running_ = true;
        ticker_ = std::thread([this]() { tick_loop(); });
    }

    void stop() {
        running_ = false;
        if (ticker_.joinable()) ticker_.join();
    }

    // 添加定时器：delay_ms 后执行回调
    int add_timer(int delay_ms, Callback cb) {
        std::lock_guard lock(mutex_);
        int ticks = delay_ms / interval_ms_;
        int target_slot = (current_slot_ + ticks) % num_slots_;
        int rounds = ticks / num_slots_;

        int id = next_id_++;
        slots_[target_slot].push_back({id, rounds, std::move(cb)});
        return id;
    }

    // O(N) 取消 —— 遍历所有槽位
    bool cancel_timer(int id) {
        std::lock_guard lock(mutex_);
        for (auto& slot : slots_) {
            for (auto it = slot.begin(); it != slot.end(); ++it) {
                if (it->id == id) {
                    slot.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

private:
    void tick_loop() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms_));
            tick();
        }
    }

    void tick() {
        std::lock_guard lock(mutex_);
        current_slot_ = (current_slot_ + 1) % num_slots_;
        auto& slot = slots_[current_slot_];
        auto it = slot.begin();
        while (it != slot.end()) {
            if (it->remaining_rounds <= 0) {
                it->callback();       // 触发！
                it = slot.erase(it);
            } else {
                --it->remaining_rounds; // 还需等待更多圈
                ++it;
            }
        }
    }
};

} // namespace ch13
```

### 13.2 时间轮工作原理

```
TimerWheel (10 slots, interval=50ms, 总跨度=500ms)

     current_slot_ = 3
           ↓
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │
└───┴───┴───┴───┴─┬─┴───┴───┴───┴───┴───┘
                  │
         add_timer(200ms, cb)
         ticks = 200/50 = 4
         target = (3+4) % 10 = 7
         rounds = 4/10 = 0
                  │
                  ▼
         slot[7].push({id, rounds=0, cb})

     4 次 tick 后: current_slot_ = 7
         → 遍历 slot[7]
         → remaining_rounds == 0 → 触发 cb ✓

多圈示例:
    add_timer(700ms, cb2)
    ticks = 14, target = (3+14)%10 = 7
    rounds = 14/10 = 1
    → 第一次到达 slot[7]: rounds=1 → --rounds (不触发)
    → 第二次到达 slot[7]: rounds=0 → 触发 ✓
```

### 深入扩展

1. **多级时间轮 (Hierarchical Timer Wheel)**：Kafka / Netty 的实现，秒级轮 + 分钟级轮 + 小时级轮，支持超长延迟
2. **O(1) 取消**：用 `unordered_map<id, list::iterator>` 维护反向索引，取消操作从 O(N) 降为 O(1)
3. **高精度 tick**：用 `timerfd_create()` (Linux) 或 `CreateWaitableTimer()` (Windows) 替代 `sleep_for`
4. **无锁时间轮**：通过分离 tick 线程与定时器线程的操作路径，减少锁竞争
5. **惰性触发**：不使用后台 tick 线程，在 `add_timer()` 或 `check()` 时推进时间指针，适合事件驱动架构

---

## 第14章：对象池 / 连接池

### 14.1 核心实现

```cpp
namespace ch14 {

template <typename T>
class ObjectPool {
    std::vector<std::unique_ptr<T>> pool_;
    std::queue<T*> available_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::function<std::unique_ptr<T>()> factory_;
    size_t max_size_;

public:
    // RAII 自动归还
    class Lease {
        ObjectPool* pool_;
        T* obj_;
    public:
        Lease(ObjectPool* pool, T* obj) : pool_(pool), obj_(obj) {}
        ~Lease() { if (pool_ && obj_) pool_->release(obj_); }

        Lease(Lease&& other) noexcept : pool_(other.pool_), obj_(other.obj_) {
            other.pool_ = nullptr; other.obj_ = nullptr;
        }
        Lease& operator=(Lease&&) = delete;
        Lease(const Lease&) = delete;
        Lease& operator=(const Lease&) = delete;

        T* operator->() { return obj_; }
        T& operator*()  { return *obj_; }
        T* get()         { return obj_; }
    };

    ObjectPool(std::function<std::unique_ptr<T>()> factory,
               size_t initial_size = 4, size_t max_size = 16)
        : factory_(std::move(factory)), max_size_(max_size) {
        for (size_t i = 0; i < initial_size; ++i) {
            auto obj = factory_();
            available_.push(obj.get());
            pool_.push_back(std::move(obj));
        }
    }

    // 阻塞借出
    Lease acquire() {
        std::unique_lock lock(mutex_);
        if (available_.empty() && pool_.size() < max_size_) {
            auto obj = factory_();
            T* raw = obj.get();
            pool_.push_back(std::move(obj));
            return Lease(this, raw);
        }
        cv_.wait(lock, [this]() { return !available_.empty(); });
        T* obj = available_.front();
        available_.pop();
        return Lease(this, obj);
    }

    // 非阻塞尝试借出
    std::optional<Lease> try_acquire() {
        std::lock_guard lock(mutex_);
        if (available_.empty()) {
            if (pool_.size() < max_size_) {
                auto obj = factory_();
                T* raw = obj.get();
                pool_.push_back(std::move(obj));
                return Lease(this, raw);
            }
            return std::nullopt;
        }
        T* obj = available_.front();
        available_.pop();
        return Lease(this, obj);
    }

private:
    void release(T* obj) {
        std::lock_guard lock(mutex_);
        available_.push(obj);
        cv_.notify_one();
    }
};

} // namespace ch14
```

### 14.2 对象池生命周期

```
ObjectPool<DBConnection> (initial=2, max=8)

初始化:
  pool_: [conn#0, conn#1]
  available_: [conn#0, conn#1]

Thread A: acquire()
  available_: [conn#1]   → Lease(conn#0) 返回给 A

Thread B: acquire()
  available_: []         → Lease(conn#1) 返回给 B

Thread C: acquire()
  available_ 为空, pool_.size()=2 < max=8
  → factory_() 创建 conn#2
  pool_: [conn#0, conn#1, conn#2]
  → Lease(conn#2) 返回给 C

Thread A: ~Lease()  (RAII 自动归还)
  → release(conn#0)
  available_: [conn#0]
  cv_.notify_one()     → 唤醒等待中的线程
```

### 14.3 模拟数据库连接池 Demo

```cpp
struct DBConnection {
    int id;
    bool connected = true;
    static std::atomic<int> next_id;
    DBConnection() : id(next_id++) {
        std::this_thread::sleep_for(1ms); // 模拟连接耗时
    }
    std::string query(const std::string& sql) {
        return "Result from conn#" + std::to_string(id) + " for: " + sql;
    }
};

void demo_object_pool() {
    ObjectPool<DBConnection> pool(
        []() { return std::make_unique<DBConnection>(); }, 2, 8
    );

    // 8 线程 × 10 次查询
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&pool, i]() {
            for (int j = 0; j < 10; ++j) {
                auto conn = pool.acquire();   // 自动 RAII
                conn->query("SELECT * FROM t" + std::to_string(i));
                std::this_thread::sleep_for(1ms);
            } // ~Lease 自动归还
        });
    }
    for (auto& t : threads) t.join();
}
```

### 深入扩展

1. **健康检查**：借出前检查连接有效性（`ping()`），无效则销毁并重建
2. **空闲回收**：后台线程定期检查空闲对象，超过阈值时间未使用则销毁（缩池）
3. **预热 (Warmup)**：启动时异步创建 min_size 个对象，避免首次请求延迟
4. **统计指标**：记录等待时间、借出次数、活跃数量，暴露给监控系统
5. **分层池**：为不同类型的连接（读/写、不同数据库）维护独立池

---

## 第15章：并发 LRU 缓存

### 15.1 核心实现

```cpp
namespace ch15 {

template <typename K, typename V>
class ConcurrentLRUCache {
    struct Node {
        K key;
        V value;
    };

    size_t capacity_;
    std::list<Node> list_;                                         // 头 = 最近使用
    std::unordered_map<K, typename std::list<Node>::iterator> map_; // 快速查找
    mutable std::mutex mutex_;

    // 命中率统计
    std::atomic<uint64_t> hits_{0};
    std::atomic<uint64_t> misses_{0};

public:
    explicit ConcurrentLRUCache(size_t capacity) : capacity_(capacity) {}

    std::optional<V> get(const K& key) {
        std::lock_guard lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            misses_.fetch_add(1, std::memory_order_relaxed);
            return std::nullopt;
        }
        // 命中 → 移到头部
        list_.splice(list_.begin(), list_, it->second);
        hits_.fetch_add(1, std::memory_order_relaxed);
        return it->second->value;
    }

    void put(const K& key, V value) {
        std::lock_guard lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->value = std::move(value);
            list_.splice(list_.begin(), list_, it->second);
            return;
        }
        // 满了 → 淘汰尾部
        if (map_.size() >= capacity_) {
            auto& back = list_.back();
            map_.erase(back.key);
            list_.pop_back();
        }
        // 插入头部
        list_.push_front({key, std::move(value)});
        map_[key] = list_.begin();
    }

    double hit_rate() const {
        uint64_t h = hits_.load(), m = misses_.load();
        if (h + m == 0) return 0;
        return static_cast<double>(h) / (h + m) * 100.0;
    }
};

} // namespace ch15
```

### 15.2 LRU 操作图解

```
初始: capacity=4
  list: [D] → [C] → [B] → [A]
  map:  {A:→A, B:→B, C:→C, D:→D}

get(B) → 命中! 移到头部
  list: [B] → [D] → [C] → [A]

put(E, val) → 满了! 淘汰尾部 A
  map.erase(A)
  list.pop_back()
  list: [E] → [B] → [D] → [C]
  map:  {B:→B, C:→C, D:→D, E:→E}

get(A) → 未命中 (misses++)
```

> **O(1) 操作**：`list_.splice()` 只改指针，O(1)；`map_.find()` 哈希查找，摊销 O(1)；`list_.pop_back()` 淘汰尾部，O(1)。

### 15.3 命中率测试

```cpp
void demo_lru_cache() {
    ConcurrentLRUCache<int, std::string> cache(100);

    // 加载 200 个 → 只保留最近 100 个 (100-199)
    for (int i = 0; i < 200; ++i)
        cache.put(i, "value_" + std::to_string(i));

    // 80% 热数据 (100-199) + 20% 冷数据 (0-99)
    // 4 线程 × 10000 次查找
    // 预期命中率 ≈ 80%
}
```

### 深入扩展

1. **分段 LRU**：将 key 空间划分为 N 段，每段独立 LRU + 锁，并发度提升 N 倍
2. **LFU (Least Frequently Used)**：按访问频率淘汰，适合热点数据不稳定的场景
3. **2Q 算法**：两个队列（入口队列 + 频繁访问队列），防止扫描污染
4. **TTL 支持**：每个缓存项设置过期时间，结合第13章 TimerWheel 触发过期清理
5. **异步加载**：`get_or_load(key, loader)` — 缺失时异步加载并填充，避免缓存击穿

---

## 第16章：优雅关停 (Graceful Shutdown)

### 16.1 核心实现

```cpp
namespace ch16 {

class GracefulShutdown {
    std::atomic<bool> shutdown_requested_{false};
    std::atomic<int> active_requests_{0};
    std::condition_variable cv_;
    std::mutex mutex_;

public:
    void request_shutdown() {
        shutdown_requested_.store(true, std::memory_order_release);
        cv_.notify_all();
    }

    bool is_shutdown() const {
        return shutdown_requested_.load(std::memory_order_acquire);
    }

    // 进入请求处理
    bool enter() {
        if (is_shutdown()) return false; // 已关停 → 拒绝
        active_requests_.fetch_add(1, std::memory_order_relaxed);
        // 双重检查：防止 enter 和 request_shutdown 之间的竞态
        if (is_shutdown()) {
            active_requests_.fetch_sub(1, std::memory_order_relaxed);
            cv_.notify_all();
            return false;
        }
        return true;
    }

    void leave() {
        if (active_requests_.fetch_sub(1, std::memory_order_relaxed) == 1) {
            cv_.notify_all(); // 最后一个离开 → 通知等待者
        }
    }

    // RAII 守护
    class RequestGuard {
        GracefulShutdown* gs_;
        bool entered_;
    public:
        RequestGuard(GracefulShutdown& gs) : gs_(&gs), entered_(gs.enter()) {}
        ~RequestGuard() { if (entered_) gs_->leave(); }
        operator bool() const { return entered_; }
    };

    // 等待所有请求完成（带超时）
    bool wait_for_completion(std::chrono::milliseconds timeout) {
        std::unique_lock lock(mutex_);
        return cv_.wait_for(lock, timeout, [this]() {
            return active_requests_.load(std::memory_order_relaxed) == 0;
        });
    }
};

} // namespace ch16
```

### 16.2 关停时序

```
正常运行:
  Worker1: enter() ✓ → 处理请求 → leave()
  Worker2: enter() ✓ → 处理请求 → leave()
  Worker3: enter() ✓ → 处理中...

request_shutdown():
  shutdown_requested_ = true

  Worker4: enter() ✗ → 拒绝新请求
  Worker5: enter() ✗ → 拒绝新请求
  Worker3: ...处理完成 → leave() → active=0 → notify_all

wait_for_completion(1000ms):
  active_requests_ == 0 → 返回 true (干净关停)

超时场景:
  Worker3: 长时间处理...
  wait_for_completion(1000ms) → 超时 → 返回 false
  → 强制关停 (kill threads / exit)
```

### 16.3 双重检查竞态防护

```
时序问题 (无双重检查):
  Thread A:                      Main Thread:
    is_shutdown() → false
                                 request_shutdown() → true
    active_requests_++ (= 1)
    → 请求被接受但关停已开始!
    → wait_for_completion 永远等不到 0

双重检查解决:
  Thread A:
    is_shutdown() → false
    active_requests_++ (= 1)
                                 request_shutdown() → true
    is_shutdown() → true ← 第二次检查!
    active_requests_-- (= 0)
    notify_all()
    return false → 拒绝 ✓
```

### 深入扩展

1. **信号处理集成**：捕获 `SIGTERM / SIGINT` → `request_shutdown()`，配合 `signalfd` 或 `std::signal`
2. **分阶段关停**：① 停止接受连接 → ② 排空队列 → ③ 等待进行中请求 → ④ 关闭连接池 → ⑤ 刷新日志
3. **优先级排空**：高优先级请求优先完成，低优先级请求提前取消
4. **心跳监控**：关停过程中定期报告剩余请求数和等待时间
5. **强制清理**：超时后对仍在运行的任务发送取消令牌（`std::stop_token` C++20）

---

## 第17章：C++20 协程概念

### 17.1 线程 vs 协程

```
线程模型:
  1 连接 = 1 线程
  10000 连接 × 1MB 栈 = 10 GB  ← 不可行!

  线程切换: ~1-10 μs (内核态切换)

协程模型:
  1 协程 ≈ 100 字节 (仅保存恢复点)
  10000 协程 × 100B ≈ 1 MB     ← 轻松!

  协程切换: ~10-100 ns (用户态切换)

性能差距: 100x+
```

### 17.2 三个关键字

```
co_await expr   — 暂停当前协程，等待 expr 完成后恢复
co_yield expr   — 暂停，产出值给调用者（生成器模式）
co_return expr  — 结束协程，返回最终值
```

### 17.3 Generator 生成器 (伪代码)

```cpp
// C++20 协程生成器
Generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;        // 暂停，返回 a
        auto next = a + b;
        a = b;
        b = next;
    }
}

// 使用
for (auto val : fibonacci()) {
    if (val > 100) break;
    std::cout << val << " ";  // 0 1 1 2 3 5 8 13 21 34 55 89
}
```

### 17.4 异步 Task (伪代码)

```cpp
Task<string> fetch_url(string url) {
    auto conn = co_await async_connect(url);   // 暂停等连接
    auto response = co_await conn.read();       // 暂停等读取
    co_return response.body();
}

Task<void> process() {
    auto page1 = fetch_url("http://a.com");
    auto page2 = fetch_url("http://b.com");
    // 两个请求并发执行！
    auto r1 = co_await page1;
    auto r2 = co_await page2;
}
```

### 17.5 C++17 状态机模拟

```cpp
// C++17 版"协程"—— 用状态机模拟
struct FibStateMachine {
    int a = 0, b = 1;
    bool next(int& result) {
        result = a;
        int temp = a + b;
        a = b;
        b = temp;
        return true;
    }
};

FibStateMachine fib;
for (int i = 0; i < 12; ++i) {
    int val;
    fib.next(val);
    std::cout << val << " ";  // 0 1 1 2 3 5 8 13 21 34 55 89
}
```

### 17.6 协程调度器概念

```
Event Loop (协程调度器):

  ┌──────────────────────────────────────┐
  │ 就绪队列: [coroutine_A, coroutine_C] │
  │                                       │
  │ I/O 等待: coroutine_B (等待 socket)   │
  │           coroutine_D (等待 disk)     │
  └──────────────────────────────────────┘
         │
  循环:  │
    1. 从就绪队列取出 coroutine_A → resume()
    2. coroutine_A 执行到 co_await → 暂停
    3. 检查 I/O → socket 就绪 → coroutine_B 加入就绪队列
    4. 取出 coroutine_C → resume()
    5. ...

  类似 Python asyncio / Go goroutine scheduler / Rust tokio
```

### 深入扩展

1. **C++20 `<coroutine>` 实际实现**：`promise_type`、`coroutine_handle<>`、`co_await` 的 awaiter 协议
2. **cppcoro 库**：Lewis Baker 的协程库，提供 `task<T>`、`generator<T>`、`async_generator<T>`
3. **io_uring + 协程**：Linux 5.1+ 的异步 I/O + 协程，实现零拷贝高性能网络服务
4. **结构化并发 (Structured Concurrency)**：协程的生命周期绑定到父作用域，自动取消和清理
5. **协程与线程池混合**：CPU 密集任务 `co_await` 到线程池执行，I/O 任务在事件循环中运行

---

## 第18章：高并发最佳实践与性能调优

### 18.1 锁的选择层次

```
从快到慢（开销递增）:
  ┌─────────────────────────────────────────────────┐
  │ 1. 无锁 (atomic/CAS)    → 计数器、标志位        │ 最快
  │ 2. 自旋锁 (SpinLock)     → 极短临界区 (<100ns)  │
  │ 3. SeqLock              → 读多写少、小数据      │
  │ 4. shared_mutex          → 读多写少、大临界区    │
  │ 5. mutex                  → 通用互斥              │
  │ 6. 条件变量              → 需要等待条件          │
  │ 7. async/future          → 一次性异步结果        │ 最慢
  └─────────────────────────────────────────────────┘

选择决策树:
  需要同步?
    ├── 否 → 线程本地/不可变数据
    └── 是 → 临界区多长?
        ├── <100ns → SpinLock / atomic
        └── >100ns → 读多写少?
            ├── 是 → shared_mutex / SeqLock
            └── 否 → mutex / 条件变量
```

### 18.2 减少锁竞争策略

```
┌──────────────────────────────────────────────────┐
│ 策略              │ 实现                │ 章节   │
│──────────────────│────────────────────│───────│
│ 1. 分段锁         │ ConcurrentHashMap  │ Ch4   │
│ 2. thread_local   │ 计数器本地汇总     │ Ch18  │
│ 3. RCU            │ 读-拷贝-更新       │ (扩展) │
│ 4. 不可变数据     │ const / shared_ptr │ (扩展) │
│ 5. Actor 消息传递 │ 无共享内存         │ Ch7   │
│ 6. 无锁结构       │ CAS 队列/栈        │ Ch2,3 │
│ 7. 批处理         │ 减少锁获取频率     │ Ch18  │
└──────────────────────────────────────────────────┘
```

### 18.3 thread_local 计数器聚合

```cpp
namespace ch18 {

void demo_best_practices() {
    constexpr int N = 10000000;

    // 方法1: 直接原子累加（竞争严重）
    std::atomic<long long> global_sum{0};
    {
        Timer t("atomic 直接累加");
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&global_sum]() {
                for (int j = 0; j < N / 4; ++j) {
                    global_sum.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& t : threads) t.join();
    }
    // global_sum = 10000000

    // 方法2: thread_local + 最终汇总（几乎无竞争）
    std::atomic<long long> local_sum{0};
    {
        Timer t("local 累加 + flush");
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&local_sum]() {
                long long local = 0;
                for (int j = 0; j < N / 4; ++j) {
                    ++local;               // 纯本地操作，零竞争
                }
                local_sum.fetch_add(local, std::memory_order_relaxed); // 仅一次原子操作
            });
        }
        for (auto& t : threads) t.join();
    }
    // local_sum = 10000000，但快 10x+
}
```

**性能对比**：

```
方法1: atomic 直接累加
  4 × 2500000 次 fetch_add → 缓存行乒乓 × 10M 次
  典型耗时: ~200ms

方法2: local 累加 + flush
  4 × 2500000 次本地 ++ → 零同步
  4 × 1 次 fetch_add → 仅 4 次原子操作
  典型耗时: ~5ms     ← 40x 加速!
```

### 18.4 性能调优清单

```
□ 缓存行填充 — 消除 false sharing           (Ch1)
□ 预分配内存 — 避免锁内 malloc              (Ch14)
□ 减小临界区 — 锁住最少代码
□ 减少锁粒度 — 分段锁/无锁                 (Ch4, Ch2-3)
□ 避免锁嵌套 — 防止死锁
□ 选择正确内存序 — relaxed 够用就别用 seq_cst (Ch1)
□ 线程数 ≈ CPU 核心数 — 不要过多线程
□ 批处理 — 减少上下文切换
□ 背压 — 生产者过快时减速                   (Ch11)
□ 监控指标 — 队列深度、延迟 P99
```

### 18.5 并发模式总结

| 模式 | 适用场景 | 复杂度 | 章节 |
|---|---|---|---|
| 互斥锁 | 通用 | ★☆☆ | — |
| 读写锁 | 读多写少 | ★★☆ | Ch12 |
| 无锁队列 | 高吞吐消息传递 | ★★★ | Ch3 |
| 线程池 | 任务并行 | ★★☆ | — |
| Work-Stealing | 负载不均衡 | ★★★ | Ch5 |
| Actor | 分布式/服务通信 | ★★☆ | Ch7 |
| Pipeline | 流水线处理 | ★★☆ | Ch8 |
| Fork-Join | 分治算法 | ★★☆ | Ch9 |
| MapReduce | 大数据批处理 | ★★★ | Ch10 |
| SeqLock | 高频行情数据 | ★★★ | Ch12 |
| 协程 | I/O 密集高并发 | ★★★ | Ch17 |

---

## 附录 A：test9.cpp 扩展建议总结

| 章节 | 扩展方向 | 优先级 | 关联 |
|---|---|---|---|
| Ch1 | TTAS / MCS 队列锁 / C++20 atomic wait | 高 | test11 |
| Ch2 | Hazard Pointer / Epoch-Based 回收 | 高 | test11 |
| Ch3 | SPSC 特化 / 批量入队 / 动态扩容 | 中 | test11 |
| Ch4 | RCU 读优化 / 一致性迭代 | 中 | — |
| Ch5 | 动态 deque / NUMA 感知 / 优先级 | 高 | test6 |
| Ch6 | 线程池续延 / 超时组合器 / P2300 sender | 中 | — |
| Ch7 | ActorSystem 路由 / 监督树 / 类型安全 | 中 | — |
| Ch8 | 多线程阶段 / 分支汇合 / 错误传播 | 中 | — |
| Ch9 | C++17 par / 自适应阈值 / 归并排序并行 | 低 | — |
| Ch10 | Combiner / 分布式扩展 / 流式 MR | 中 | — |
| Ch11 | 漏桶 / 自适应限流 / 分布式限流 | 高 | — |
| Ch12 | 多版本 SeqLock / RCU / LMAX Disruptor | 高 | test11 |
| Ch13 | 多级时间轮 / O(1)取消 / 高精度 tick | 中 | — |
| Ch14 | 健康检查 / 空闲回收 / 预热 | 中 | — |
| Ch15 | 分段 LRU / LFU / TTL + TimerWheel | 高 | Ch13 |
| Ch16 | 信号处理 / 分阶段关停 / stop_token | 中 | — |
| Ch17 | 实际协程实现 / cppcoro / io_uring | 高 | — |
| Ch18 | perf/VTune profiling / NUMA 优化 | 中 | test14 |

---

## 附录 B：典型输出示例

```
========================================
  现代 C++ 高并发处理 完全教程
========================================

── 第1章：并发基础设施 ──
  CAS counter: 200000 (期望 200000)
  [SpinLock 4线程] 12345 μs
  [SpinLockBackoff 4线程] 8901 μs
  [紧密原子量 (false sharing)] 156000 μs
  [填充原子量 (无 false sharing)] 28000 μs

── 第2章：无锁栈 ──
  [无锁栈 push] 5600 μs
  [无锁栈 pop]  4200 μs
  Pop 成功: 40000

── 第3章：无锁 MPMC 有界队列 ──
  [MPMC Queue] 23000 μs
  produced=400000 consumed=400000

── 第4章：并发哈希表 ──
  [ConcurrentHashMap] 18500 μs
  最终大小: 40000

── 第5章：Work-Stealing 线程池 ──
  [Work-Stealing Pool 10000 tasks] 15000 μs
  结果 sum=49995000

── 第6章：异步组合器 ──
  then: 10 → ×2 → +5 = 25
  when_all: [1, 4, 9, 16, 25]
  when_any: (first result)

── 第7章：Actor 模型 ──
  CounterActor: 105

── 第8章：Pipeline 流水线 ──
  Pipeline 处理: 1000 条 (期望 1000)

── 第9章：Fork-Join 分治 ──
  [std::sort 串行] 95000 μs
  [parallel_sort 并行] 32000 μs
  排序结果一致: true
  [串行求和] 2100 μs
  [并行求和] 800 μs
  求和结果一致: true

── 第10章：MapReduce ──
  [MapReduce 词频统计] 1200 μs
  词频 Top 10:
           the: 10
           fox: 6
           dog: 5
         quick: 3
         brown: 2
          ...

── 第11章：背压与限流 ──
  令牌桶: accepted=123 rejected=1877
  背压队列: produced=1000 consumed=1000

── 第12章：SeqLock 与读优化 ──
  SeqLock: reads=4523678 final_price=199.99 final_ts=9999
  [atomic 读计数器] 35000 μs
  [shared_mutex 读计数器] 890000 μs

── 第13章：定时器轮 ──
  定时器触发数: 10 (期望 10, 取消1个不计)

── 第14章：对象池/连接池 ──
  初始连接数: 2 可用: 2
  [连接池 并发查询] 28000 μs
  查询总数: 80 连接数: 8 可用: 8

── 第15章：并发 LRU 缓存 ──
  缓存大小: 100
  [LRU 缓存并发读取] 12000 μs
  命中率: 79.8% (hits=31920 misses=8080)

── 第16章：优雅关停 ──
  请求关停, 活跃请求: 3
  关停干净, 完成请求: 478, 活跃: 0

── 第17章：协程式并发 ──
  17.1 线程 vs 协程: ...
  17.6 模拟生成器: 0 1 1 2 3 5 8 13 21 34 55 89

── 第18章：最佳实践 ──
  [atomic 直接累加] 210000 μs
  sum = 10000000
  [local 累加 + flush] 5200 μs
  sum = 10000000

========================================
  演示完成
========================================
```

---

## 附录 C：核心设计原则

> **高并发系统的本质是在吞吐量、延迟和正确性之间寻找平衡。** 无锁提供最低延迟但最难正确实现；锁简单可靠但可能成为瓶颈；消息传递（Actor/Pipeline）消除共享状态但引入通信开销。**没有银弹——理解每种模式的适用场景和代价，根据实际负载特征（读写比、竞争强度、数据大小）做出工程选择，始终以基准测试数据为决策依据。**
