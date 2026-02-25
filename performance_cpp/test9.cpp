// =============================================================================
// 现代 C++ 高并发处理 完全指导教程与代码示例
// =============================================================================
// 本教程聚焦于高并发场景下的架构模式、无锁数据结构、高性能线程池、
// 并发设计模式以及实战级并发组件的实现。
//
// 与 test6 (多线程基础) 不同，本教程侧重：
//   - 高吞吐量并发数据结构
//   - 无锁/无等待算法
//   - 高性能线程池与任务调度
//   - 并发设计模式（Actor、Pipeline、MapReduce、Fork-Join）
//   - 背压、限流、优雅关停
//   - 实战级并发组件（连接池、定时器轮、并发缓存）
//
// 标准：C++17（部分 C++20 以注释说明）
// 编译：g++ -std=c++17 -O2 -pthread -o test9 test9.cpp
//       cl /std:c++17 /O2 /EHsc test9.cpp
//
// 目录：
//   1.  并发基础设施 — CAS / 内存序 / 缓存行填充
//   2.  无锁栈 (Lock-Free Stack)
//   3.  无锁 MPMC 有界队列 (Lock-Free Bounded Queue)
//   4.  并发哈希表 (Striped Lock HashMap)
//   5.  高性能线程池 — 任务窃取 (Work-Stealing ThreadPool)
//   6.  Future/Promise 链式调用与异步组合
//   7.  Actor 模型
//   8.  Pipeline 并行流水线
//   9.  Fork-Join 并行分治
//  10.  并行 MapReduce 框架
//  11.  背压 (Backpressure) 与限流 (Rate Limiter)
//  12.  读写锁优化 — 无锁读计数器 / SeqLock
//  13.  定时器轮 (Timer Wheel)
//  14.  对象池 / 连接池
//  15.  并发缓存 (Thread-Safe LRU Cache)
//  16.  优雅关停 (Graceful Shutdown)
//  17.  协程式并发简介 (C++20 Coroutines 概念)
//  18.  高并发最佳实践与性能调优
// =============================================================================

#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <type_traits>
#include <optional>
#include <variant>
#include <cassert>
#include <cstring>
#include <iomanip>

using namespace std::chrono_literals;

// =============================================================================
// 工具
// =============================================================================

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    Timer(const char* l) : start_(Clock::now()), label_(l) {}
    ~Timer() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
            Clock::now() - start_).count();
        std::cout << "  [" << label_ << "] " << us << " us\n";
    }
};

// 缓存行大小（典型 x86）
static constexpr size_t CACHE_LINE_SIZE = 64;

// 缓存行对齐的原子类型（防止 false sharing）
template <typename T>
struct alignas(CACHE_LINE_SIZE) PaddedAtomic {
    std::atomic<T> value{};

    PaddedAtomic() = default;
    explicit PaddedAtomic(T v) : value(v) {}

    T load(std::memory_order mo = std::memory_order_seq_cst) const {
        return value.load(mo);
    }
    void store(T v, std::memory_order mo = std::memory_order_seq_cst) {
        value.store(v, mo);
    }
    bool compare_exchange_weak(T& expected, T desired,
                                std::memory_order mo = std::memory_order_seq_cst) {
        return value.compare_exchange_weak(expected, desired, mo);
    }
    T fetch_add(T v, std::memory_order mo = std::memory_order_seq_cst) {
        return value.fetch_add(v, mo);
    }
};


// =============================================================================
// 第1章：并发基础设施 — CAS / 内存序 / 缓存行
// =============================================================================
// 高并发编程的三大基石：
//  1) CAS (Compare-And-Swap) — 无锁算法的核心原语
//  2) 内存序 (Memory Ordering) — 控制指令重排与可见性
//  3) 缓存行填充 (Cache Line Padding) — 消除 false sharing
// =============================================================================

namespace ch1 {

// --- 1.1 CAS 操作演示 ---
// CAS 是所有无锁算法的基石
// 语义：if (addr == expected) { addr = desired; return true; } else { expected = addr; return false; }

void demo_cas() {
    std::atomic<int> counter{0};

    // 用 CAS 实现无锁递增
    auto increment = [&counter]() {
        for (int i = 0; i < 100000; ++i) {
            int expected = counter.load(std::memory_order_relaxed);
            while (!counter.compare_exchange_weak(
                expected, expected + 1,
                std::memory_order_relaxed)) {
                // CAS 失败，expected 已被更新为最新值，重试
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(increment);
    for (auto& t : threads) t.join();

    std::cout << "  CAS counter: " << counter.load() << " (期望 400000)\n";
}

// --- 1.2 自旋锁 (SpinLock) —  CAS 实战 ---
// 适用场景：临界区极短（几十纳秒级）

class SpinLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        // test_and_set 返回旧值，如果旧值为 true 说明已被锁
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 自旋等待 — 可以加入退避策略
            // C++20: flag_.wait(true, std::memory_order_relaxed);
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
        // C++20: flag_.notify_one();
    }
};

// 带指数退避的自旋锁
class SpinLockBackoff {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        int backoff = 1;
        while (flag_.test_and_set(std::memory_order_acquire)) {
            for (int i = 0; i < backoff; ++i) {
                #if defined(__x86_64__) || defined(_M_X64)
                    // x86 pause 指令，减少流水线冲刷
                    #ifdef _MSC_VER
                        _mm_pause();
                    #else
                        __builtin_ia32_pause();
                    #endif
                #else
                    std::this_thread::yield();
                #endif
            }
            backoff = std::min(backoff * 2, 1024); // 指数退避，上限 1024
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};

void demo_spinlock() {
    SpinLock sl;
    int counter = 0;

    auto worker = [&]() {
        for (int i = 0; i < 100000; ++i) {
            sl.lock();
            ++counter;
            sl.unlock();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(worker);
    for (auto& t : threads) t.join();

    std::cout << "  SpinLock counter: " << counter << " (期望 400000)\n";
}

// --- 1.3 False Sharing 演示与解决 ---

void demo_false_sharing() {
    constexpr int N = 10000000;

    // ❌ 紧密排列 → false sharing
    struct Bad {
        std::atomic<long long> a{0};
        std::atomic<long long> b{0};
    } bad;

    {
        Timer t("false sharing (紧密)");
        auto t1 = std::thread([&]() { for (int i = 0; i < N; ++i) bad.a.fetch_add(1, std::memory_order_relaxed); });
        auto t2 = std::thread([&]() { for (int i = 0; i < N; ++i) bad.b.fetch_add(1, std::memory_order_relaxed); });
        t1.join(); t2.join();
    }

    // ✅ 缓存行填充 → 消除 false sharing
    PaddedAtomic<long long> good_a{0}, good_b{0};

    {
        Timer t("no false sharing (填充)");
        auto t1 = std::thread([&]() { for (int i = 0; i < N; ++i) good_a.value.fetch_add(1, std::memory_order_relaxed); });
        auto t2 = std::thread([&]() { for (int i = 0; i < N; ++i) good_b.value.fetch_add(1, std::memory_order_relaxed); });
        t1.join(); t2.join();
    }

    std::cout << "  结果: 紧密=" << bad.a << "+" << bad.b
              << " 填充=" << good_a.load() << "+" << good_b.load() << "\n";
}

// --- 1.4 内存序速查 ---
//
// | 内存序                | 语义              | 开销  | 用途                |
// |----------------------|-------------------|------|---------------------|
// | relaxed              | 无序保证           | 最低  | 计数器、统计        |
// | acquire              | 读后不上移         | 低    | 锁的获取            |
// | release              | 写前不下移         | 低    | 锁的释放            |
// | acq_rel              | 读acquire写release | 中    | CAS 读-改-写         |
// | seq_cst              | 全局一致序         | 最高  | 默认、简单场景      |

} // namespace ch1


// =============================================================================
// 第2章：无锁栈 (Lock-Free Stack)
// =============================================================================
//
// 使用 CAS 实现的无锁栈，支持多线程并发 push/pop。
// 注意 ABA 问题：本示例通过"延迟释放"降低风险。
// 生产环境建议使用 Hazard Pointer 或引用计数方案。
// =============================================================================

namespace ch2 {

template <typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(T val) : data(std::move(val)), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};
    std::atomic<size_t> size_{0};

    // 待释放节点列表（简单的延迟回收）
    std::atomic<Node*> to_delete_{nullptr};
    std::atomic<int> threads_in_pop_{0};

    void try_reclaim(Node* old_head) {
        if (threads_in_pop_ == 1) {
            // 只有当前线程在 pop，可以安全回收
            Node* nodes_to_delete = to_delete_.exchange(nullptr);
            if (--threads_in_pop_ == 0) {
                delete_nodes(nodes_to_delete);
            } else if (nodes_to_delete) {
                chain_pending_nodes(nodes_to_delete);
            }
            delete old_head;
        } else {
            // 其他线程也在 pop，加入待回收列表
            chain_pending_node(old_head);
            --threads_in_pop_;
        }
    }

    void chain_pending_nodes(Node* nodes) {
        Node* last = nodes;
        while (last->next) last = last->next;
        chain_pending_nodes(nodes, last);
    }

    void chain_pending_nodes(Node* first, Node* last) {
        last->next = to_delete_;
        while (!to_delete_.compare_exchange_weak(last->next, first));
    }

    void chain_pending_node(Node* n) {
        chain_pending_nodes(n, n);
    }

    static void delete_nodes(Node* nodes) {
        while (nodes) {
            Node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

public:
    ~LockFreeStack() {
        while (pop()) {} // 清空栈
        delete_nodes(to_delete_.load());
    }

    void push(T value) {
        Node* new_node = new Node(std::move(value));
        new_node->next = head_.load(std::memory_order_relaxed);
        // CAS：如果 head 还等于 new_node->next，则更新为 new_node
        while (!head_.compare_exchange_weak(
            new_node->next, new_node,
            std::memory_order_release,
            std::memory_order_relaxed)) {
            // 失败则 new_node->next 已被更新为最新 head，重试
        }
        size_.fetch_add(1, std::memory_order_relaxed);
    }

    std::optional<T> pop() {
        ++threads_in_pop_;
        Node* old_head = head_.load(std::memory_order_relaxed);
        while (old_head &&
               !head_.compare_exchange_weak(
                   old_head, old_head->next,
                   std::memory_order_acquire,
                   std::memory_order_relaxed)) {
            // old_head 已被更新
        }

        if (!old_head) {
            --threads_in_pop_;
            return std::nullopt;
        }

        T result = std::move(old_head->data);
        size_.fetch_sub(1, std::memory_order_relaxed);
        try_reclaim(old_head);
        return result;
    }

    size_t size() const { return size_.load(std::memory_order_relaxed); }
    bool empty() const { return size() == 0; }
};

void demo_lock_free_stack() {
    LockFreeStack<int> stack;
    constexpr int ITEMS_PER_THREAD = 10000;
    constexpr int NUM_THREADS = 4;

    // 多线程 push
    {
        std::vector<std::thread> threads;
        Timer t("无锁栈 push");
        for (int tid = 0; tid < NUM_THREADS; ++tid) {
            threads.emplace_back([&stack, tid]() {
                for (int i = 0; i < ITEMS_PER_THREAD; ++i)
                    stack.push(tid * ITEMS_PER_THREAD + i);
            });
        }
        for (auto& t : threads) t.join();
    }

    std::cout << "  栈大小: " << stack.size()
              << " (期望 " << NUM_THREADS * ITEMS_PER_THREAD << ")\n";

    // 多线程 pop
    std::atomic<int> pop_count{0};
    {
        std::vector<std::thread> threads;
        Timer t("无锁栈 pop");
        for (int tid = 0; tid < NUM_THREADS; ++tid) {
            threads.emplace_back([&stack, &pop_count]() {
                while (auto val = stack.pop()) {
                    pop_count.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& t : threads) t.join();
    }

    std::cout << "  pop 总数: " << pop_count.load() << "\n";
}

} // namespace ch2


// =============================================================================
// 第3章：无锁 MPMC 有界队列
// =============================================================================
//
// 多生产者多消费者 (MPMC) 有界环形队列。
// 基于 Dmitry Vyukov 的经典设计，使用序列号 + CAS。
// =============================================================================

namespace ch3 {

template <typename T>
class MPMCBoundedQueue {
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    std::vector<Cell> buffer_;
    size_t mask_;

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> enqueue_pos_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> dequeue_pos_{0};

public:
    // capacity 必须是 2 的幂
    explicit MPMCBoundedQueue(size_t capacity)
        : buffer_(capacity), mask_(capacity - 1) {
        assert((capacity & (capacity - 1)) == 0 && "capacity must be power of 2");
        for (size_t i = 0; i < capacity; ++i)
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
    }

    // 不可拷贝
    MPMCBoundedQueue(const MPMCBoundedQueue&) = delete;
    MPMCBoundedQueue& operator=(const MPMCBoundedQueue&) = delete;

    // 尝试入队，失败返回 false（队列满）
    bool try_enqueue(T value) {
        Cell* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);

        for (;;) {
            cell = &buffer_[pos & mask_];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

            if (diff == 0) {
                // cell 空闲，尝试占位
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1,
                    std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false; // 队列满
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }

        cell->data = std::move(value);
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    // 尝试出队，失败返回 nullopt（队列空）
    std::optional<T> try_dequeue() {
        Cell* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);

        for (;;) {
            cell = &buffer_[pos & mask_];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

            if (diff == 0) {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1,
                    std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return std::nullopt; // 队列空
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }

        T result = std::move(cell->data);
        cell->sequence.store(pos + mask_ + 1, std::memory_order_release);
        return result;
    }
};

void demo_mpmc_queue() {
    MPMCBoundedQueue<int> queue(1024);

    constexpr int ITEMS = 100000;
    constexpr int PRODUCERS = 4;
    constexpr int CONSUMERS = 4;

    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};
    std::atomic<bool> done{false};

    // 生产者
    std::vector<std::thread> producers;
    for (int i = 0; i < PRODUCERS; ++i) {
        producers.emplace_back([&, i]() {
            int count = ITEMS / PRODUCERS;
            for (int j = 0; j < count; ++j) {
                while (!queue.try_enqueue(i * count + j)) {
                    std::this_thread::yield(); // 队列满，让出 CPU
                }
                produced.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 消费者
    std::vector<std::thread> consumers;
    for (int i = 0; i < CONSUMERS; ++i) {
        consumers.emplace_back([&]() {
            while (!done.load(std::memory_order_relaxed) ||
                   consumed.load(std::memory_order_relaxed) < ITEMS) {
                if (auto val = queue.try_dequeue()) {
                    consumed.fetch_add(1, std::memory_order_relaxed);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& t : producers) t.join();
    done.store(true);
    for (auto& t : consumers) t.join();

    std::cout << "  MPMC 队列: produced=" << produced.load()
              << " consumed=" << consumed.load() << "\n";
}

} // namespace ch3


// =============================================================================
// 第4章：并发哈希表 (Striped Lock HashMap)
// =============================================================================
//
// 分段锁 (Striped Locking) 策略：
// 不是锁整个表，而是将表分成多个段，每段一个锁。
// 读写只锁自己所在的段，大幅提升并发度。
// =============================================================================

namespace ch4 {

template <typename K, typename V, size_t NUM_STRIPES = 16>
class ConcurrentHashMap {
    struct Bucket {
        std::shared_mutex mutex;
        std::unordered_map<K, V> data;
    };

    std::array<Bucket, NUM_STRIPES> buckets_;

    size_t stripe(const K& key) const {
        return std::hash<K>{}(key) % NUM_STRIPES;
    }

public:
    // 插入或更新
    void put(const K& key, V value) {
        auto& bucket = buckets_[stripe(key)];
        std::unique_lock lock(bucket.mutex);
        bucket.data[key] = std::move(value);
    }

    // 查找
    std::optional<V> get(const K& key) const {
        auto& bucket = buckets_[stripe(key)];
        std::shared_lock lock(bucket.mutex);
        auto it = bucket.data.find(key);
        if (it != bucket.data.end()) return it->second;
        return std::nullopt;
    }

    // 删除
    bool erase(const K& key) {
        auto& bucket = buckets_[stripe(key)];
        std::unique_lock lock(bucket.mutex);
        return bucket.data.erase(key) > 0;
    }

    // compute_if_absent: 不存在则计算并插入
    V compute_if_absent(const K& key, std::function<V()> supplier) {
        auto& bucket = buckets_[stripe(key)];

        // 先尝试读锁
        {
            std::shared_lock lock(bucket.mutex);
            auto it = bucket.data.find(key);
            if (it != bucket.data.end()) return it->second;
        }

        // 升级到写锁
        std::unique_lock lock(bucket.mutex);
        auto [it, inserted] = bucket.data.try_emplace(key, V{});
        if (inserted) it->second = supplier();
        return it->second;
    }

    // 遍历（快照式，不保证一致性）
    void for_each(std::function<void(const K&, const V&)> fn) const {
        for (auto& bucket : buckets_) {
            std::shared_lock lock(bucket.mutex);
            for (auto& [k, v] : bucket.data) fn(k, v);
        }
    }

    size_t size() const {
        size_t total = 0;
        for (auto& bucket : buckets_) {
            std::shared_lock lock(bucket.mutex);
            total += bucket.data.size();
        }
        return total;
    }
};

void demo_concurrent_hashmap() {
    ConcurrentHashMap<int, std::string> map;
    constexpr int N = 10000;

    // 多线程写入
    {
        Timer t("并发HashMap 写入");
        std::vector<std::thread> threads;
        for (int tid = 0; tid < 4; ++tid) {
            threads.emplace_back([&map, tid]() {
                for (int i = 0; i < N / 4; ++i) {
                    int key = tid * (N / 4) + i;
                    map.put(key, "val_" + std::to_string(key));
                }
            });
        }
        for (auto& t : threads) t.join();
    }

    std::cout << "  HashMap size: " << map.size() << " (期望 " << N << ")\n";

    // 多线程混合读写
    std::atomic<int> hits{0}, misses{0};
    {
        Timer t("并发HashMap 混合读写");
        std::vector<std::thread> threads;
        for (int tid = 0; tid < 8; ++tid) {
            threads.emplace_back([&map, &hits, &misses, tid]() {
                std::mt19937 rng(tid);
                std::uniform_int_distribution<int> dist(0, N * 2);
                for (int i = 0; i < 10000; ++i) {
                    int key = dist(rng);
                    if (auto val = map.get(key))
                        hits.fetch_add(1, std::memory_order_relaxed);
                    else
                        misses.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& t : threads) t.join();
    }

    std::cout << "  hits=" << hits.load() << " misses=" << misses.load() << "\n";
}

} // namespace ch4


// =============================================================================
// 第5章：高性能线程池 — 任务窃取 (Work-Stealing)
// =============================================================================
//
// 每个线程有自己的本地任务队列。
// 当自己的队列空了，从其他线程的队列"窃取"任务，实现负载均衡。
// =============================================================================

namespace ch5 {

// 工作窃取双端队列 (Work-Stealing Deque)
// 所有者从 bottom 端 push/pop，窃取者从 top 端 steal
class WorkStealingDeque {
    static constexpr int BUFFER_SIZE = 4096;
    std::array<std::function<void()>, BUFFER_SIZE> buffer_;
    alignas(CACHE_LINE_SIZE) std::atomic<int> top_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<int> bottom_{0};

public:
    // 所有者调用：添加任务到底部
    void push(std::function<void()> task) {
        int b = bottom_.load(std::memory_order_relaxed);
        buffer_[b % BUFFER_SIZE] = std::move(task);
        std::atomic_thread_fence(std::memory_order_release);
        bottom_.store(b + 1, std::memory_order_relaxed);
    }

    // 所有者调用：从底部取任务
    std::optional<std::function<void()>> pop() {
        int b = bottom_.load(std::memory_order_relaxed) - 1;
        bottom_.store(b, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);

        int t = top_.load(std::memory_order_relaxed);
        if (t <= b) {
            auto task = std::move(buffer_[b % BUFFER_SIZE]);
            if (t == b) {
                // 最后一个元素，需要和窃取者竞争
                if (!top_.compare_exchange_strong(t, t + 1,
                    std::memory_order_seq_cst, std::memory_order_relaxed)) {
                    bottom_.store(b + 1, std::memory_order_relaxed);
                    return std::nullopt;
                }
                bottom_.store(b + 1, std::memory_order_relaxed);
            }
            return task;
        }
        // 队列空
        bottom_.store(b + 1, std::memory_order_relaxed);
        return std::nullopt;
    }

    // 窃取者调用：从顶部偷任务
    std::optional<std::function<void()>> steal() {
        int t = top_.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        int b = bottom_.load(std::memory_order_acquire);

        if (t < b) {
            auto task = buffer_[t % BUFFER_SIZE];
            if (!top_.compare_exchange_strong(t, t + 1,
                std::memory_order_seq_cst, std::memory_order_relaxed)) {
                return std::nullopt; // 竞争失败
            }
            return task;
        }
        return std::nullopt;
    }

    bool empty() const {
        return top_.load(std::memory_order_relaxed) >=
               bottom_.load(std::memory_order_relaxed);
    }
};

// Work-Stealing 线程池
class WorkStealingPool {
    int num_threads_;
    std::vector<std::thread> threads_;
    std::vector<std::unique_ptr<WorkStealingDeque>> queues_;
    std::atomic<bool> stop_{false};
    std::atomic<int> task_count_{0};

    // 全局队列（用于外部提交）
    std::mutex global_mutex_;
    std::queue<std::function<void()>> global_queue_;
    std::condition_variable cv_;

    static thread_local int thread_index_;

public:
    explicit WorkStealingPool(int num_threads = std::thread::hardware_concurrency())
        : num_threads_(num_threads) {
        for (int i = 0; i < num_threads_; ++i)
            queues_.push_back(std::make_unique<WorkStealingDeque>());

        for (int i = 0; i < num_threads_; ++i) {
            threads_.emplace_back([this, i]() {
                thread_index_ = i;
                worker_loop(i);
            });
        }
    }

    ~WorkStealingPool() {
        stop_.store(true, std::memory_order_release);
        cv_.notify_all();
        for (auto& t : threads_) t.join();
    }

    // 提交任务
    template <typename F>
    auto submit(F&& f) -> std::future<decltype(f())> {
        using Result = decltype(f());
        auto task = std::make_shared<std::packaged_task<Result()>>(std::forward<F>(f));
        auto future = task->get_future();

        auto wrapper = [task]() { (*task)(); };

        // 如果当前在工作线程中，直接放入本地队列
        if (thread_index_ >= 0 && thread_index_ < num_threads_) {
            queues_[thread_index_]->push(std::move(wrapper));
        } else {
            std::lock_guard lock(global_mutex_);
            global_queue_.push(std::move(wrapper));
        }

        task_count_.fetch_add(1, std::memory_order_relaxed);
        cv_.notify_one();
        return future;
    }

    void wait_all() {
        while (task_count_.load(std::memory_order_relaxed) > 0) {
            std::this_thread::yield();
        }
    }

private:
    void worker_loop(int index) {
        while (!stop_.load(std::memory_order_relaxed)) {
            std::function<void()> task;

            // 1. 先从本地队列取
            if (auto t = queues_[index]->pop()) {
                task = std::move(*t);
            }
            // 2. 从全局队列取
            else {
                std::lock_guard lock(global_mutex_);
                if (!global_queue_.empty()) {
                    task = std::move(global_queue_.front());
                    global_queue_.pop();
                }
            }
            // 3. 从其他线程窃取
            if (!task) {
                for (int i = 0; i < num_threads_; ++i) {
                    if (i == index) continue;
                    if (auto t = queues_[i]->steal()) {
                        task = std::move(*t);
                        break;
                    }
                }
            }

            if (task) {
                task();
                task_count_.fetch_sub(1, std::memory_order_relaxed);
            } else {
                // 没任务，短暂等待
                std::unique_lock lock(global_mutex_);
                cv_.wait_for(lock, 1ms, [this]() {
                    return stop_.load() || !global_queue_.empty();
                });
            }
        }
    }
};

thread_local int WorkStealingPool::thread_index_ = -1;

void demo_work_stealing_pool() {
    WorkStealingPool pool(4);

    constexpr int N = 10000;
    std::atomic<long long> sum{0};

    {
        Timer t("Work-Stealing 线程池");
        std::vector<std::future<void>> futures;
        for (int i = 0; i < N; ++i) {
            futures.push_back(pool.submit([&sum, i]() {
                sum.fetch_add(i, std::memory_order_relaxed);
            }));
        }
        for (auto& f : futures) f.get();
    }

    long long expected = (long long)(N - 1) * N / 2;
    std::cout << "  求和: " << sum.load() << " (期望 " << expected << ")\n";
}

} // namespace ch5


// =============================================================================
// 第6章：Future/Promise 链式调用与异步组合
// =============================================================================
//
// 基于 std::future/promise 构建异步组合器：
// then (链式), when_all (全部完成), when_any (任一完成)
// =============================================================================

namespace ch6 {

// 简易线程池（用于异步执行）
class SimplePool {
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{false};

public:
    explicit SimplePool(int n = 4) {
        for (int i = 0; i < n; ++i) {
            threads_.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(mutex_);
                        cv_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    ~SimplePool() {
        stop_ = true;
        cv_.notify_all();
        for (auto& t : threads_) t.join();
    }

    template <typename F>
    auto submit(F&& f) -> std::future<decltype(f())> {
        using R = decltype(f());
        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(f));
        auto future = task->get_future();
        {
            std::lock_guard lock(mutex_);
            tasks_.push([task]() { (*task)(); });
        }
        cv_.notify_one();
        return future;
    }
};

// --- 6.1 then: 链式异步 ---
template <typename T, typename F>
auto then(SimplePool& pool, std::future<T> fut, F&& f) {
    using R = decltype(f(std::declval<T>()));
    auto shared_fut = std::make_shared<std::future<T>>(std::move(fut));
    return pool.submit([shared_fut = std::move(shared_fut), f = std::forward<F>(f)]() -> R {
        return f(shared_fut->get());
    });
}

// --- 6.2 when_all: 等待全部完成 ---
template <typename T>
std::future<std::vector<T>> when_all(SimplePool& pool,
                                      std::vector<std::future<T>> futures) {
    auto shared_futs = std::make_shared<std::vector<std::future<T>>>(std::move(futures));
    return pool.submit([shared_futs]() -> std::vector<T> {
        std::vector<T> results;
        results.reserve(shared_futs->size());
        for (auto& f : *shared_futs) results.push_back(f.get());
        return results;
    });
}

// --- 6.3 when_any: 等待任一完成 ---
template <typename T>
std::future<T> when_any(SimplePool& pool, std::vector<std::future<T>> futures) {
    auto shared_futs = std::make_shared<std::vector<std::future<T>>>(std::move(futures));
    auto result = std::make_shared<std::promise<T>>();
    auto future = result->get_future();
    auto done = std::make_shared<std::atomic<bool>>(false);

    for (size_t i = 0; i < shared_futs->size(); ++i) {
        pool.submit([shared_futs, result, done, i]() {
            try {
                T val = (*shared_futs)[i].get();
                bool expected = false;
                if (done->compare_exchange_strong(expected, true)) {
                    result->set_value(std::move(val));
                }
            } catch (...) {
                // 忽略非首个完成的异常
            }
        });
    }

    return future;
}

void demo_async_combinators() {
    SimplePool pool(4);

    // --- then 链式 ---
    auto f1 = pool.submit([]() { return 10; });
    auto f2 = then(pool, std::move(f1), [](int x) { return x * 2; });
    auto f3 = then(pool, std::move(f2), [](int x) { return x + 5; });
    std::cout << "  then 链: 10 → *2 → +5 = " << f3.get() << "\n";

    // --- when_all ---
    std::vector<std::future<int>> all_futures;
    for (int i = 0; i < 5; ++i) {
        all_futures.push_back(pool.submit([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return i * i;
        }));
    }
    auto all_result = when_all(pool, std::move(all_futures));
    auto results = all_result.get();
    std::cout << "  when_all: ";
    for (int v : results) std::cout << v << " ";
    std::cout << "\n";

    // --- when_any ---
    std::vector<std::future<std::string>> any_futures;
    any_futures.push_back(pool.submit([]() {
        std::this_thread::sleep_for(50ms); return std::string("slow");
    }));
    any_futures.push_back(pool.submit([]() {
        std::this_thread::sleep_for(10ms); return std::string("fast");
    }));
    any_futures.push_back(pool.submit([]() {
        std::this_thread::sleep_for(30ms); return std::string("medium");
    }));
    auto any_result = when_any(pool, std::move(any_futures));
    std::cout << "  when_any 最先完成: " << any_result.get() << "\n";
}

} // namespace ch6


// =============================================================================
// 第7章：Actor 模型
// =============================================================================
//
// Actor 模型核心思想：
//   - 每个 Actor 有独立的状态（不共享内存）
//   - 通过消息传递进行通信
//   - 一次只处理一条消息（天然线程安全）
// =============================================================================

namespace ch7 {

// 消息类型
using Message = std::variant<
    std::string,
    int,
    std::pair<std::string, int>
>;

// Actor 基类
class Actor {
public:
    virtual ~Actor() { stop(); }

    void send(Message msg) {
        {
            std::lock_guard lock(mutex_);
            mailbox_.push(std::move(msg));
        }
        cv_.notify_one();
    }

    void start() {
        running_ = true;
        thread_ = std::thread([this]() { run(); });
    }

    void stop() {
        running_ = false;
        cv_.notify_one();
        if (thread_.joinable()) thread_.join();
    }

protected:
    virtual void on_receive(const Message& msg) = 0;

private:
    void run() {
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
    }

    std::thread thread_;
    std::queue<Message> mailbox_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
};

// --- 计数器 Actor ---
class CounterActor : public Actor {
    int count_ = 0;
public:
    int get_count() const { return count_; }

protected:
    void on_receive(const Message& msg) override {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                if (arg == "increment") ++count_;
                else if (arg == "reset") count_ = 0;
            } else if constexpr (std::is_same_v<T, int>) {
                count_ += arg;
            }
        }, msg);
    }
};

// --- 日志 Actor ---
class LoggerActor : public Actor {
    std::vector<std::string> logs_;
    std::mutex log_mutex_;
public:
    std::vector<std::string> get_logs() {
        std::lock_guard lock(log_mutex_);
        return logs_;
    }

protected:
    void on_receive(const Message& msg) override {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                std::lock_guard lock(log_mutex_);
                logs_.push_back(arg);
            } else if constexpr (std::is_same_v<T, std::pair<std::string, int>>) {
                std::lock_guard lock(log_mutex_);
                logs_.push_back(arg.first + ": " + std::to_string(arg.second));
            }
        }, msg);
    }
};

void demo_actor() {
    CounterActor counter;
    LoggerActor logger;

    counter.start();
    logger.start();

    // 发送消息
    for (int i = 0; i < 100; ++i) {
        counter.send(std::string("increment"));
        if (i % 10 == 0)
            logger.send(std::string("tick " + std::to_string(i)));
    }

    counter.send(5); // 加 5

    // 等待消息处理完
    std::this_thread::sleep_for(100ms);

    counter.stop();
    logger.stop();

    std::cout << "  Counter: " << counter.get_count() << " (期望 105)\n";
    auto logs = logger.get_logs();
    std::cout << "  Logger 条目数: " << logs.size() << "\n";
}

} // namespace ch7


// =============================================================================
// 第8章：Pipeline 并行流水线
// =============================================================================
//
// 将处理分为多个阶段 (Stage)，每个阶段在独立线程中运行，
// 阶段之间通过有界队列传递数据。
// 优势：每个阶段可以并行处理不同数据，提高吞吐量。
// =============================================================================

namespace ch8 {

// 阶段间通信的有界阻塞队列
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

// 流水线
template <typename Input, typename Output>
class Pipeline {
    struct StageBase {
        virtual ~StageBase() = default;
        virtual void run() = 0;
    };

    std::vector<std::unique_ptr<StageBase>> stages_;
    std::vector<std::thread> threads_;

public:
    // 简化版本：直接用 BoundedBlockingQueue 串联

    // 两阶段演示
    static void demo() {
        constexpr int N = 1000;
        BoundedBlockingQueue<int> q1(32);      // input → stage1
        BoundedBlockingQueue<int> q2(32);      // stage1 → stage2
        BoundedBlockingQueue<std::string> q3(32); // stage2 → output

        // Stage 1: 乘以 2
        std::thread stage1([&]() {
            while (auto val = q1.pop()) {
                q2.push(*val * 2);
            }
            q2.close();
        });

        // Stage 2: 转为字符串
        std::thread stage2([&]() {
            while (auto val = q2.pop()) {
                q3.push("result_" + std::to_string(*val));
            }
            q3.close();
        });

        // 生产者
        std::thread producer([&]() {
            for (int i = 0; i < N; ++i) q1.push(i);
            q1.close();
        });

        // 消费者
        int count = 0;
        while (auto val = q3.pop()) {
            ++count;
        }

        producer.join();
        stage1.join();
        stage2.join();

        std::cout << "  Pipeline 处理: " << count << " 条 (期望 " << N << ")\n";
    }
};

void demo_pipeline() {
    Timer t("Pipeline 流水线");
    Pipeline<int, std::string>::demo();
}

} // namespace ch8


// =============================================================================
// 第9章：Fork-Join 并行分治
// =============================================================================
//
// 将任务递归拆分(fork)为子任务，子任务完成后合并结果(join)。
// 典型应用：并行排序、并行求和、并行树遍历。
// =============================================================================

namespace ch9 {

// 使用 async 实现 Fork-Join
// 并行快速排序
template <typename Iter>
void parallel_sort(Iter begin, Iter end, int depth = 0) {
    auto len = std::distance(begin, end);
    if (len <= 1) return;

    // 小数组直接排序
    if (len < 10000 || depth > 4) {
        std::sort(begin, end);
        return;
    }

    // 选择 pivot
    auto pivot = *std::next(begin, len / 2);

    // 三路分区
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

// 并行求和
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

void demo_fork_join() {
    constexpr int N = 1000000;

    // 并行排序
    std::vector<int> data(N);
    std::mt19937 rng(42);
    std::generate(data.begin(), data.end(), [&rng]() {
        return std::uniform_int_distribution<int>(0, N)(rng);
    });

    auto data_copy = data;

    {
        Timer t("std::sort 串行");
        std::sort(data.begin(), data.end());
    }

    {
        Timer t("parallel_sort 并行");
        parallel_sort(data_copy.begin(), data_copy.end());
    }

    std::cout << "  排序结果一致: " << std::boolalpha << (data == data_copy) << "\n";

    // 并行求和
    std::vector<long long> nums(N);
    std::iota(nums.begin(), nums.end(), 1LL);

    long long serial_sum, par_sum;
    {
        Timer t("串行求和");
        serial_sum = std::accumulate(nums.begin(), nums.end(), 0LL);
    }
    {
        Timer t("并行求和");
        par_sum = parallel_sum(nums.begin(), nums.end());
    }

    std::cout << "  求和结果一致: " << (serial_sum == par_sum)
              << " (" << serial_sum << ")\n";
}

} // namespace ch9


// =============================================================================
// 第10章：并行 MapReduce 框架
// =============================================================================
//
// 将数据分片 → Map(并行转换) → Shuffle(按 key 分组) → Reduce(并行聚合)
// =============================================================================

namespace ch10 {

template <typename InputT, typename KeyT, typename ValueT, typename ResultT>
class MapReduce {
public:
    using MapFunc = std::function<std::vector<std::pair<KeyT, ValueT>>(const InputT&)>;
    using ReduceFunc = std::function<ResultT(const KeyT&, const std::vector<ValueT>&)>;

    MapReduce(MapFunc mapper, ReduceFunc reducer, int num_threads = 4)
        : mapper_(mapper), reducer_(reducer), num_threads_(num_threads) {}

    std::map<KeyT, ResultT> execute(const std::vector<InputT>& inputs) {
        // 1. Map 阶段：并行
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

        // 3. Reduce 阶段：并行
        std::vector<std::pair<KeyT, std::vector<ValueT>>> groups(
            grouped.begin(), grouped.end());

        std::vector<std::future<std::pair<KeyT, ResultT>>> reduce_futures;
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

void demo_mapreduce() {
    // 词频统计
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
        // Reduce: (word, [1,1,1,...]) → count
        [](const std::string& /*key*/, const std::vector<int>& values) -> int {
            return std::accumulate(values.begin(), values.end(), 0);
        }
    );

    Timer t("MapReduce 词频统计");
    auto results = mr.execute(documents);

    std::cout << "  词频 Top 10:\n";
    std::vector<std::pair<std::string, int>> sorted(results.begin(), results.end());
    std::sort(sorted.begin(), sorted.end(),
        [](auto& a, auto& b) { return a.second > b.second; });

    int count = 0;
    for (auto& [word, freq] : sorted) {
        if (++count > 10) break;
        std::cout << "    " << std::setw(10) << word << ": " << freq << "\n";
    }
}

} // namespace ch10


// =============================================================================
// 第11章：背压 (Backpressure) 与限流 (Rate Limiter)
// =============================================================================
//
// 高并发系统必须控制流量，避免下游过载。
// 背压：生产者过快时自动减速。
// 限流：控制每秒处理量。
// =============================================================================

namespace ch11 {

// --- 11.1 令牌桶限流器 (Token Bucket Rate Limiter) ---
class TokenBucketLimiter {
    double rate_;          // 每秒补充的令牌数
    double capacity_;      // 桶容量
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

    // 阻塞等待直到获取令牌
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

// --- 11.2 滑动窗口限流器 ---
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

        // 移除窗口外的时间戳
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

// --- 11.3 背压队列 (Backpressure Queue) ---
template <typename T>
class BackpressureQueue {
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    size_t capacity_;
    size_t high_watermark_; // 高水位标记
    size_t low_watermark_;  // 低水位标记
    std::atomic<bool> paused_{false};
    bool closed_ = false;

public:
    BackpressureQueue(size_t capacity = 1024,
                       size_t high_water = 768,
                       size_t low_water = 256)
        : capacity_(capacity), high_watermark_(high_water), low_watermark_(low_water) {}

    // 生产者调用：当队列过满时阻塞
    bool push(T item) {
        std::unique_lock lock(mutex_);
        not_full_.wait(lock, [this]() { return queue_.size() < capacity_ || closed_; });
        if (closed_) return false;

        queue_.push(std::move(item));

        // 高水位：通知生产者减速
        if (queue_.size() >= high_watermark_) {
            paused_.store(true, std::memory_order_relaxed);
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

        // 低水位：恢复生产
        if (queue_.size() <= low_watermark_) {
            paused_.store(false, std::memory_order_relaxed);
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

void demo_rate_limiter() {
    // 令牌桶：每秒 1000 个令牌
    TokenBucketLimiter limiter(1000.0, 100.0);

    std::atomic<int> accepted{0}, rejected{0};

    {
        Timer t("令牌桶限流");
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&]() {
                for (int j = 0; j < 500; ++j) {
                    if (limiter.try_acquire())
                        accepted.fetch_add(1, std::memory_order_relaxed);
                    else
                        rejected.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& t : threads) t.join();
    }

    std::cout << "  令牌桶: accepted=" << accepted.load()
              << " rejected=" << rejected.load() << "\n";
}

void demo_backpressure() {
    BackpressureQueue<int> bq(100, 80, 20);
    std::atomic<int> produced{0}, consumed{0};

    // 快速生产者
    auto producer = std::thread([&]() {
        for (int i = 0; i < 1000; ++i) {
            bq.push(i);
            produced.fetch_add(1, std::memory_order_relaxed);
        }
        bq.close();
    });

    // 慢速消费者
    auto consumer = std::thread([&]() {
        while (auto val = bq.pop()) {
            consumed.fetch_add(1, std::memory_order_relaxed);
            // 模拟慢处理
            if (consumed.load(std::memory_order_relaxed) % 100 == 0) {
                std::this_thread::sleep_for(1ms);
            }
        }
    });

    producer.join();
    consumer.join();

    std::cout << "  背压队列: produced=" << produced.load()
              << " consumed=" << consumed.load() << "\n";
}

} // namespace ch11


// =============================================================================
// 第12章：读写锁优化 — SeqLock
// =============================================================================
//
// SeqLock (Sequence Lock)：
//   - 写者递增序列号（先奇后偶），读者检测序列号变化
//   - 适合读多写少 + 小数据场景（如高频行情数据）
//   - 读者零开销（无锁、无原子写）
//   - 读者可能重试但不阻塞写者
// =============================================================================

namespace ch12 {

class SeqLock {
    std::atomic<uint64_t> seq_{0}; // 偶数=无写操作

public:
    // 写者：获取写锁
    void write_lock() {
        seq_.fetch_add(1, std::memory_order_release); // 奇数 → 正在写
    }

    void write_unlock() {
        seq_.fetch_add(1, std::memory_order_release); // 偶数 → 写完成
    }

    // 读者：开始读取
    uint64_t read_begin() const {
        uint64_t s;
        do {
            s = seq_.load(std::memory_order_acquire);
        } while (s & 1); // 如果序列号为奇数（正在写），等待
        return s;
    }

    // 读者：验证读取是否有效
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
            result = data_; // 拷贝数据
        } while (!lock_.read_validate(seq)); // 验证无并发写
        return result;
    }
};

void demo_seqlock() {
    MarketDataStore store;
    std::atomic<bool> stop{false};

    // 写者线程（低频更新）
    auto writer = std::thread([&]() {
        for (int i = 0; i < 10000; ++i) {
            store.update(100.0 + i * 0.01, 1000.0 + i, i);
        }
        stop.store(true, std::memory_order_release);
    });

    // 读者线程（高频读取）
    std::atomic<int> reads{0};
    std::vector<std::thread> readers;
    for (int i = 0; i < 4; ++i) {
        readers.emplace_back([&]() {
            while (!stop.load(std::memory_order_relaxed)) {
                auto data = store.read();
                reads.fetch_add(1, std::memory_order_relaxed);
                (void)data;
            }
        });
    }

    writer.join();
    for (auto& t : readers) t.join();

    auto final_data = store.read();
    std::cout << "  SeqLock: reads=" << reads.load()
              << " final_price=" << final_data.price
              << " final_ts=" << final_data.timestamp << "\n";
}

// --- 无锁计数器对比 ---
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
            (void)sum;
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
            (void)sum;
        });
        writer.join(); reader.join();
    }
}

} // namespace ch12


// =============================================================================
// 第13章：定时器轮 (Timer Wheel)
// =============================================================================
//
// 高性能定时器，O(1) 添加/取消，适用于大量定时任务。
// 思路：时间划分到固定槽位，指针转动触发到期任务。
// 应用：网络超时、心跳检测、延迟任务。
// =============================================================================

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
    // num_slots: 槽位数, interval_ms: 每个槽位的时间间隔(毫秒)
    // 总时长 = num_slots * interval_ms
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

    // 添加定时器，delay_ms 毫秒后执行
    int add_timer(int delay_ms, Callback cb) {
        std::lock_guard lock(mutex_);
        int ticks = delay_ms / interval_ms_;
        int target_slot = (current_slot_ + ticks) % num_slots_;
        int rounds = ticks / num_slots_;

        int id = next_id_++;
        slots_[target_slot].push_back({id, rounds, std::move(cb)});
        return id;
    }

    // 取消定时器
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
                it->callback();
                it = slot.erase(it);
            } else {
                --it->remaining_rounds;
                ++it;
            }
        }
    }
};

void demo_timer_wheel() {
    TimerWheel wheel(10, 50); // 10 个槽，每槽 50ms

    std::atomic<int> fired{0};

    wheel.start();

    // 添加多个定时器
    for (int i = 0; i < 10; ++i) {
        wheel.add_timer((i + 1) * 100, [&fired, i]() {
            fired.fetch_add(1, std::memory_order_relaxed);
        });
    }

    // 添加并取消一个
    int cancel_id = wheel.add_timer(200, [&fired]() { fired.fetch_add(100); });
    wheel.cancel_timer(cancel_id);

    // 等待所有定时器触发
    std::this_thread::sleep_for(1500ms);
    wheel.stop();

    std::cout << "  定时器触发数: " << fired.load()
              << " (期望 10, 取消1个不计)\n";
}

} // namespace ch13


// =============================================================================
// 第14章：对象池 / 连接池
// =============================================================================
//
// 对象池避免频繁创建/销毁开销（如数据库连接、线程、缓冲区）。
// 核心：预创建对象，借出/归还，线程安全。
// =============================================================================

namespace ch14 {

// 通用对象池
template <typename T>
class ObjectPool {
    std::vector<std::unique_ptr<T>> pool_;
    std::queue<T*> available_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::function<std::unique_ptr<T>()> factory_;
    size_t max_size_;

public:
    // RAII 借出对象
    class Lease {
        ObjectPool* pool_;
        T* obj_;
    public:
        Lease(ObjectPool* pool, T* obj) : pool_(pool), obj_(obj) {}
        ~Lease() { if (pool_ && obj_) pool_->release(obj_); }

        Lease(Lease&& other) noexcept : pool_(other.pool_), obj_(other.obj_) {
            other.pool_ = nullptr;
            other.obj_ = nullptr;
        }
        Lease& operator=(Lease&&) = delete;
        Lease(const Lease&) = delete;
        Lease& operator=(const Lease&) = delete;

        T* operator->() { return obj_; }
        T& operator*() { return *obj_; }
        T* get() { return obj_; }
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

    // 借出对象（阻塞等待）
    Lease acquire() {
        std::unique_lock lock(mutex_);

        if (available_.empty() && pool_.size() < max_size_) {
            // 池未满，创建新对象
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

    // 尝试借出（非阻塞）
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

    size_t available_count() const {
        std::lock_guard lock(const_cast<std::mutex&>(mutex_));
        return available_.size();
    }

    size_t total_count() const {
        std::lock_guard lock(const_cast<std::mutex&>(mutex_));
        return pool_.size();
    }

private:
    void release(T* obj) {
        std::lock_guard lock(mutex_);
        available_.push(obj);
        cv_.notify_one();
    }
};

// 模拟数据库连接
struct DBConnection {
    int id;
    bool connected = true;

    static std::atomic<int> next_id;
    DBConnection() : id(next_id++) {
        // 模拟耗时连接
        std::this_thread::sleep_for(1ms);
    }

    std::string query(const std::string& sql) {
        return "Result from conn#" + std::to_string(id) + " for: " + sql;
    }
};

std::atomic<int> DBConnection::next_id{0};

void demo_object_pool() {
    // 连接池：初始 2 个，最大 8 个
    ObjectPool<DBConnection> pool(
        []() { return std::make_unique<DBConnection>(); },
        2, 8
    );

    std::cout << "  初始连接数: " << pool.total_count()
              << " 可用: " << pool.available_count() << "\n";

    std::atomic<int> queries{0};

    {
        Timer t("连接池 并发查询");
        std::vector<std::thread> threads;
        for (int i = 0; i < 8; ++i) {
            threads.emplace_back([&pool, &queries, i]() {
                for (int j = 0; j < 10; ++j) {
                    auto conn = pool.acquire();
                    auto result = conn->query("SELECT * FROM t" + std::to_string(i));
                    queries.fetch_add(1, std::memory_order_relaxed);
                    std::this_thread::sleep_for(1ms); // 模拟查询
                }
            });
        }
        for (auto& t : threads) t.join();
    }

    std::cout << "  查询总数: " << queries.load()
              << " 连接数: " << pool.total_count()
              << " 可用: " << pool.available_count() << "\n";
}

} // namespace ch14


// =============================================================================
// 第15章：并发缓存 (Thread-Safe LRU Cache)
// =============================================================================
//
// LRU (Least Recently Used) 缓存：O(1) 查找 + O(1) 淘汰。
// 线程安全版本使用分段锁提高并发度。
// =============================================================================

namespace ch15 {

template <typename K, typename V>
class ConcurrentLRUCache {
    struct Node {
        K key;
        V value;
    };

    size_t capacity_;
    std::list<Node> list_;  // 双向链表，头部=最近使用
    std::unordered_map<K, typename std::list<Node>::iterator> map_;
    mutable std::mutex mutex_;

    // 统计
    std::atomic<uint64_t> hits_{0};
    std::atomic<uint64_t> misses_{0};

public:
    explicit ConcurrentLRUCache(size_t capacity) : capacity_(capacity) {}

    // 查找
    std::optional<V> get(const K& key) {
        std::lock_guard lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            misses_.fetch_add(1, std::memory_order_relaxed);
            return std::nullopt;
        }
        // 移到头部（最近使用）
        list_.splice(list_.begin(), list_, it->second);
        hits_.fetch_add(1, std::memory_order_relaxed);
        return it->second->value;
    }

    // 插入
    void put(const K& key, V value) {
        std::lock_guard lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            // key 已存在，更新并移到头部
            it->second->value = std::move(value);
            list_.splice(list_.begin(), list_, it->second);
            return;
        }

        // 容量满，淘汰尾部
        if (map_.size() >= capacity_) {
            auto& back = list_.back();
            map_.erase(back.key);
            list_.pop_back();
        }

        // 插入头部
        list_.push_front({key, std::move(value)});
        map_[key] = list_.begin();
    }

    size_t size() const {
        std::lock_guard lock(mutex_);
        return map_.size();
    }

    double hit_rate() const {
        uint64_t h = hits_.load(), m = misses_.load();
        if (h + m == 0) return 0;
        return static_cast<double>(h) / (h + m) * 100.0;
    }

    std::pair<uint64_t, uint64_t> stats() const {
        return {hits_.load(), misses_.load()};
    }
};

void demo_lru_cache() {
    ConcurrentLRUCache<int, std::string> cache(100);

    // 预加载
    for (int i = 0; i < 200; ++i) {
        cache.put(i, "value_" + std::to_string(i));
    }
    // 只保留最近 100 个 (100-199)

    std::cout << "  缓存大小: " << cache.size() << "\n";

    // 并发读取
    {
        Timer t("LRU 缓存并发读取");
        std::vector<std::thread> threads;
        for (int tid = 0; tid < 4; ++tid) {
            threads.emplace_back([&cache, tid]() {
                std::mt19937 rng(tid);
                // 80% 热数据 (100-199), 20% 冷数据 (0-99)
                std::uniform_int_distribution<int> hot(100, 199);
                std::uniform_int_distribution<int> cold(0, 99);
                std::uniform_int_distribution<int> coin(0, 9);

                for (int i = 0; i < 10000; ++i) {
                    int key = (coin(rng) < 8) ? hot(rng) : cold(rng);
                    cache.get(key);
                }
            });
        }
        for (auto& t : threads) t.join();
    }

    auto [hits, misses] = cache.stats();
    std::cout << "  命中率: " << std::fixed << std::setprecision(1)
              << cache.hit_rate() << "% (hits=" << hits
              << " misses=" << misses << ")\n";
}

} // namespace ch15


// =============================================================================
// 第16章：优雅关停 (Graceful Shutdown)
// =============================================================================
//
// 高并发服务必须能够优雅关停：
//   1. 停止接受新请求
//   2. 等待正在处理的请求完成
//   3. 清理资源
//   4. 设置超时，超时后强制关停
// =============================================================================

namespace ch16 {

class GracefulShutdown {
    std::atomic<bool> shutdown_requested_{false};
    std::atomic<int> active_requests_{0};
    std::condition_variable cv_;
    std::mutex mutex_;

public:
    // 请求关停
    void request_shutdown() {
        shutdown_requested_.store(true, std::memory_order_release);
        cv_.notify_all();
    }

    // 是否已请求关停
    bool is_shutdown() const {
        return shutdown_requested_.load(std::memory_order_acquire);
    }

    // 进入请求处理（返回 false 表示已关停，拒绝新请求）
    bool enter() {
        if (is_shutdown()) return false;
        active_requests_.fetch_add(1, std::memory_order_relaxed);
        // double check
        if (is_shutdown()) {
            active_requests_.fetch_sub(1, std::memory_order_relaxed);
            cv_.notify_all();
            return false;
        }
        return true;
    }

    // 离开请求处理
    void leave() {
        if (active_requests_.fetch_sub(1, std::memory_order_relaxed) == 1) {
            cv_.notify_all();
        }
    }

    // RAII guard
    class RequestGuard {
        GracefulShutdown* gs_;
        bool entered_;
    public:
        RequestGuard(GracefulShutdown& gs) : gs_(&gs), entered_(gs.enter()) {}
        ~RequestGuard() { if (entered_) gs_->leave(); }
        operator bool() const { return entered_; }
    };

    // 等待所有请求完成
    bool wait_for_completion(std::chrono::milliseconds timeout) {
        std::unique_lock lock(mutex_);
        return cv_.wait_for(lock, timeout, [this]() {
            return active_requests_.load(std::memory_order_relaxed) == 0;
        });
    }

    int active_count() const {
        return active_requests_.load(std::memory_order_relaxed);
    }
};

void demo_graceful_shutdown() {
    GracefulShutdown gs;
    std::atomic<int> completed{0};

    // 模拟服务器处理请求
    std::vector<std::thread> workers;
    for (int i = 0; i < 8; ++i) {
        workers.emplace_back([&gs, &completed, i]() {
            for (int j = 0; j < 100; ++j) {
                GracefulShutdown::RequestGuard guard(gs);
                if (!guard) return; // 已关停，停止

                // 模拟处理
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                completed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 一段时间后请求关停
    std::this_thread::sleep_for(5ms);
    std::cout << "  请求关停, 活跃请求: " << gs.active_count() << "\n";
    gs.request_shutdown();

    // 等待完成（最多 1 秒）
    bool clean = gs.wait_for_completion(1000ms);

    for (auto& t : workers) t.join();

    std::cout << "  关停" << (clean ? "干净" : "超时")
              << ", 完成请求: " << completed.load()
              << ", 活跃: " << gs.active_count() << "\n";
}

} // namespace ch16


// =============================================================================
// 第17章：协程式并发简介 (C++20 Coroutines)
// =============================================================================
//
// C++20 引入语言级协程支持 (co_await, co_yield, co_return)。
// 协程是轻量级的"可暂停函数"，适合 I/O 密集型高并发。
//
// ⚠️ 下面是概念和伪代码说明（完整实现需要 C++20 编译器 + coroutine header）。
// =============================================================================

namespace ch17 {

void demo_coroutine_concepts() {
    std::cout << "  === C++20 协程概念说明 ===\n\n";

    // --- 17.1 为什么需要协程 ---
    std::cout << "  17.1 线程 vs 协程:\n";
    std::cout << "    线程: 1 MB 栈 × 10000 = 10 GB 内存 (不可行!)\n";
    std::cout << "    协程: ~100 字节 × 10000 = ~1 MB 内存 (轻松!)\n\n";

    // --- 17.2 协程核心概念 ---
    std::cout << "  17.2 三个关键字:\n";
    std::cout << "    co_await expr  — 暂停，等待 expr 完成后恢复\n";
    std::cout << "    co_yield expr  — 暂停，产出值给调用者\n";
    std::cout << "    co_return expr — 结束协程，返回最终值\n\n";

    // --- 17.3 生成器示例 (伪代码) ---
    //
    // Generator<int> fibonacci() {
    //     int a = 0, b = 1;
    //     while (true) {
    //         co_yield a;        // 暂停，返回 a
    //         auto next = a + b;
    //         a = b;
    //         b = next;
    //     }
    // }
    //
    // for (auto val : fibonacci()) {
    //     if (val > 100) break;
    //     cout << val << " ";  // 0 1 1 2 3 5 8 13 21 34 55 89
    // }

    std::cout << "  17.3 Generator<T> co_yield 示例:\n";
    std::cout << "    fibonacci() → 0 1 1 2 3 5 8 13 21 34 55 89...\n\n";

    // --- 17.4 异步 Task 示例 (伪代码) ---
    //
    // Task<string> fetch_url(string url) {
    //     auto conn = co_await async_connect(url);   // 暂停等待连接
    //     auto response = co_await conn.read();       // 暂停等待读取
    //     co_return response.body();
    // }
    //
    // Task<void> process() {
    //     auto page1 = fetch_url("http://a.com");
    //     auto page2 = fetch_url("http://b.com");
    //     // 两个请求并发执行！
    //     auto r1 = co_await page1;
    //     auto r2 = co_await page2;
    //     cout << r1.size() << " " << r2.size();
    // }

    std::cout << "  17.4 异步 Task<T> co_await 示例:\n";
    std::cout << "    并发 fetch 多个 URL，无需线程！\n\n";

    // --- 17.5 协程调度器概念 ---
    std::cout << "  17.5 协程调度器:\n";
    std::cout << "    event_loop → 检查 I/O 就绪 → 恢复对应协程\n";
    std::cout << "    类似 Python asyncio / Go goroutine 调度\n\n";

    // --- 17.6 用 C++17 模拟协程 ---
    // 使用状态机 + callback 可以模拟类似效果

    std::cout << "  17.6 C++17 状态机模拟:\n";

    // 简单的状态机式"协程"
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
    std::cout << "    模拟生成器: ";
    for (int i = 0; i < 12; ++i) {
        int val;
        fib.next(val);
        std::cout << val << " ";
    }
    std::cout << "\n";
}

} // namespace ch17


// =============================================================================
// 第18章：高并发最佳实践与性能调优
// =============================================================================

namespace ch18 {

void demo_best_practices() {
    std::cout << "  ===== 高并发最佳实践 =====\n\n";

    // --- 18.1 锁的层次 ---
    std::cout << "  18.1 锁的选择（从快到慢）:\n";
    std::cout << "    1. 无锁 (atomic/CAS)    → 计数器、标志位\n";
    std::cout << "    2. 自旋锁 (SpinLock)     → 极短临界区 (<100ns)\n";
    std::cout << "    3. shared_mutex          → 读多写少\n";
    std::cout << "    4. mutex                  → 通用互斥\n";
    std::cout << "    5. 条件变量              → 需要等待条件\n";
    std::cout << "    6. async/future          → 一次性异步结果\n\n";

    // --- 18.2 避免竞争的设计 ---
    std::cout << "  18.2 减少锁竞争的策略:\n";
    std::cout << "    1. 分段锁 (Striped Lock)       → HashMap\n";
    std::cout << "    2. 线程本地存储 (thread_local)  → 计数器汇总\n";
    std::cout << "    3. 读-拷贝-更新 (RCU)          → 读极多场景\n";
    std::cout << "    4. 不可变数据 (Immutable)       → 天然线程安全\n";
    std::cout << "    5. 消息传递 (Actor)             → 无共享内存\n\n";

    // --- 18.3 thread_local 计数器聚合 ---
    std::cout << "  18.3 thread_local 计数器聚合演示:\n";

    struct LocalCounter {
        static thread_local long long local;
        static std::atomic<long long> global;

        static void add(long long val) { local += val; }
        static void flush() { global.fetch_add(local, std::memory_order_relaxed); local = 0; }
    };

    // thread_local 变量只能在源文件中定义
    // 这里用lambda+原子量模拟
    constexpr int N = 10000000;
    std::atomic<long long> global_sum{0};

    // 方法1: 直接原子操作（竞争严重）
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
    std::cout << "    sum = " << global_sum.load() << "\n";

    // 方法2: thread_local + 最终汇总（几乎无竞争）
    std::atomic<long long> local_sum{0};
    {
        Timer t("local 累加 + flush");
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&local_sum]() {
                long long local = 0; // 线程本地变量
                for (int j = 0; j < N / 4; ++j) {
                    ++local;
                }
                local_sum.fetch_add(local, std::memory_order_relaxed); // 一次提交
            });
        }
        for (auto& t : threads) t.join();
    }
    std::cout << "    sum = " << local_sum.load() << "\n";

    // --- 18.4 性能调优清单 ---
    std::cout << "\n  18.4 性能调优清单:\n";
    std::cout << "    □ 缓存行填充 — 消除 false sharing\n";
    std::cout << "    □ 预分配内存 — 避免锁内 malloc\n";
    std::cout << "    □ 减小临界区 — 锁住最少代码\n";
    std::cout << "    □ 减少锁粒度 — 分段锁/无锁\n";
    std::cout << "    □ 避免锁嵌套 — 防止死锁\n";
    std::cout << "    □ 选择正确的内存序 — relaxed 够用就别用 seq_cst\n";
    std::cout << "    □ 线程数 ≈ CPU 核心数 — 不要过多线程\n";
    std::cout << "    □ 批处理 — 减少上下文切换\n";
    std::cout << "    □ 背压 — 生产者过快时减速\n";
    std::cout << "    □ 监控指标 — 队列深度、延迟 P99\n";

    // --- 18.5 总结表 ---
    std::cout << "\n  18.5 并发模式总结:\n";
    std::cout << "    | 模式           | 适用场景              | 复杂度 |\n";
    std::cout << "    |----------------|----------------------|--------|\n";
    std::cout << "    | 互斥锁         | 通用                 | ★☆☆   |\n";
    std::cout << "    | 读写锁         | 读多写少             | ★★☆   |\n";
    std::cout << "    | 无锁队列       | 高吞吐消息传递       | ★★★   |\n";
    std::cout << "    | 线程池         | 任务并行             | ★★☆   |\n";
    std::cout << "    | Work-Stealing  | 负载不均衡的并行任务 | ★★★   |\n";
    std::cout << "    | Actor          | 分布式/服务间通信    | ★★☆   |\n";
    std::cout << "    | Pipeline       | 流水线处理           | ★★☆   |\n";
    std::cout << "    | Fork-Join      | 分治算法             | ★★☆   |\n";
    std::cout << "    | MapReduce      | 大数据批处理         | ★★★   |\n";
    std::cout << "    | SeqLock        | 高频行情数据         | ★★★   |\n";
    std::cout << "    | 协程           | I/O 密集高并发       | ★★★   |\n";
}

} // namespace ch18


// =============================================================================
// main
// =============================================================================

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "========================================\n";
    std::cout << "  现代 C++ 高并发处理 完全教程\n";
    std::cout << "========================================\n\n";

    std::cout << "── 第1章：并发基础设施 ──\n";
    ch1::demo_cas();
    ch1::demo_spinlock();
    ch1::demo_false_sharing();
    std::cout << "\n";

    std::cout << "── 第2章：无锁栈 ──\n";
    ch2::demo_lock_free_stack();
    std::cout << "\n";

    std::cout << "── 第3章：无锁 MPMC 有界队列 ──\n";
    ch3::demo_mpmc_queue();
    std::cout << "\n";

    std::cout << "── 第4章：并发哈希表 ──\n";
    ch4::demo_concurrent_hashmap();
    std::cout << "\n";

    std::cout << "── 第5章：Work-Stealing 线程池 ──\n";
    ch5::demo_work_stealing_pool();
    std::cout << "\n";

    std::cout << "── 第6章：异步组合器 ──\n";
    ch6::demo_async_combinators();
    std::cout << "\n";

    std::cout << "── 第7章：Actor 模型 ──\n";
    ch7::demo_actor();
    std::cout << "\n";

    std::cout << "── 第8章：Pipeline 流水线 ──\n";
    ch8::demo_pipeline();
    std::cout << "\n";

    std::cout << "── 第9章：Fork-Join 分治 ──\n";
    ch9::demo_fork_join();
    std::cout << "\n";

    std::cout << "── 第10章：MapReduce ──\n";
    ch10::demo_mapreduce();
    std::cout << "\n";

    std::cout << "── 第11章：背压与限流 ──\n";
    ch11::demo_rate_limiter();
    ch11::demo_backpressure();
    std::cout << "\n";

    std::cout << "── 第12章：SeqLock 与读优化 ──\n";
    ch12::demo_seqlock();
    ch12::demo_read_counters();
    std::cout << "\n";

    std::cout << "── 第13章：定时器轮 ──\n";
    ch13::demo_timer_wheel();
    std::cout << "\n";

    std::cout << "── 第14章：对象池/连接池 ──\n";
    ch14::demo_object_pool();
    std::cout << "\n";

    std::cout << "── 第15章：并发 LRU 缓存 ──\n";
    ch15::demo_lru_cache();
    std::cout << "\n";

    std::cout << "── 第16章：优雅关停 ──\n";
    ch16::demo_graceful_shutdown();
    std::cout << "\n";

    std::cout << "── 第17章：协程式并发 ──\n";
    ch17::demo_coroutine_concepts();
    std::cout << "\n";

    std::cout << "── 第18章：最佳实践 ──\n";
    ch18::demo_best_practices();

    std::cout << "\n========================================\n";
    std::cout << "  演示完成\n";
    std::cout << "========================================\n";

    return 0;
}
