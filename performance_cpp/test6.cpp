// =============================================================================
// 现代 C++ 多线程 完全指导教程与代码示例
// =============================================================================
// C++11 引入了标准线程库，C++17/20 持续增强。
// 本教程从基础到高级，完整覆盖多线程编程的每个方面。
//
// 标准：C++17（部分 C++20 注释说明）
// 编译：
//   Linux/Mac: g++ -std=c++17 -O2 -pthread -o test6 test6.cpp
//   Windows:   cl /std:c++17 /O2 /EHsc test6.cpp
//   (MSVC 自动链接线程库)
//
// 目录：
//   1.  线程基础 — std::thread
//   2.  线程传参与返回值
//   3.  互斥量 — mutex 家族
//   4.  锁管理 — lock_guard / unique_lock / scoped_lock
//   5.  死锁与避免策略
//   6.  条件变量 — condition_variable
//   7.  生产者-消费者模式
//   8.  异步编程 — future / promise / async
//   9.  原子操作 — std::atomic
//  10.  内存序 — memory_order 详解
//  11.  线程安全的单例 (call_once / Meyer's Singleton)
//  12.  读写锁 — shared_mutex
//  13.  线程池实现
//  14.  并行算法 (C++17 Execution Policies)
//  15.  无锁编程 — Lock-Free Queue
//  16.  实战：并发 Web 爬虫模型
//  17.  实战：Map-Reduce 框架
//  18.  常见陷阱与调试技巧
// =============================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include <queue>
#include <deque>
#include <string>
#include <string_view>
#include <functional>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <map>
#include <unordered_map>
#include <sstream>
#include <cassert>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <climits>

using namespace std::chrono_literals; // 允许 100ms, 1s 等字面量


// =============================================================================
// 工具
// =============================================================================
class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    Timer(const char* label) : start_(Clock::now()), label_(label) {}
    ~Timer() {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            Clock::now() - start_).count();
        std::cout << "  [" << label_ << "] " << ms << " ms\n";
    }
};

// 线程安全输出（避免多线程 cout 交错）
std::mutex g_cout_mutex;

template <typename... Args>
void safe_print(Args&&... args) {
    std::lock_guard<std::mutex> lock(g_cout_mutex);
    (std::cout << ... << std::forward<Args>(args)) << "\n";
}


// =============================================================================
// 第1章：线程基础 — std::thread
// =============================================================================
//
// std::thread 是 C++11 引入的线程封装。
// 关键规则：
//   1. thread 对象必须在销毁前 join() 或 detach()
//   2. 不 join 也不 detach → 析构时 std::terminate()
//   3. thread 不可拷贝，只可移动

namespace ch1 {

// --- 1.1 创建线程的多种方式 ---

void free_function() {
    safe_print("    [线程] 普通函数");
}

struct Functor {
    void operator()() const {
        safe_print("    [线程] 函数对象");
    }
};

class Worker {
public:
    void do_work(int id) {
        safe_print("    [线程] 成员函数, id=", id);
    }
};

void demo_thread_creation() {
    // (a) 普通函数
    std::thread t1(free_function);

    // (b) 函数对象
    Functor f;
    std::thread t2(f);

    // (c) Lambda
    std::thread t3([]() {
        safe_print("    [线程] Lambda");
    });

    // (d) 成员函数
    Worker w;
    std::thread t4(&Worker::do_work, &w, 42);

    t1.join(); t2.join(); t3.join(); t4.join();
}

// --- 1.2 线程信息 ---

void demo_thread_info() {
    std::cout << "  硬件并发数: " << std::thread::hardware_concurrency() << "\n";
    std::cout << "  主线程 ID:  " << std::this_thread::get_id() << "\n";

    std::thread t([]() {
        safe_print("    子线程 ID: ", std::this_thread::get_id());
    });
    t.join();
}

// --- 1.3 线程的生命周期 ---

void demo_thread_lifecycle() {
    // join: 等待线程完成
    {
        std::thread t([]() {
            std::this_thread::sleep_for(10ms);
            safe_print("    join 示例: 线程完成");
        });
        t.join(); // 阻塞直到线程结束
        std::cout << "  join 之后继续\n";
    }

    // detach: 分离线程（后台运行）
    {
        std::thread t([]() {
            std::this_thread::sleep_for(5ms);
            // 分离后的线程在后台运行，不要访问局部变量
        });
        t.detach();
        std::cout << "  detach 之后继续（线程在后台）\n";
    }

    // joinable 检查
    {
        std::thread t([]() {});
        std::cout << "  joinable (创建后): " << std::boolalpha << t.joinable() << "\n";
        t.join();
        std::cout << "  joinable (join后):  " << t.joinable() << "\n";
    }
}

// --- 1.4 RAII 线程守卫 ---

class JoinThread {
    std::thread t_;
public:
    template <typename... Args>
    explicit JoinThread(Args&&... args) : t_(std::forward<Args>(args)...) {}

    ~JoinThread() {
        if (t_.joinable()) t_.join();
    }

    JoinThread(JoinThread&&) = default;
    JoinThread& operator=(JoinThread&&) = default;
    JoinThread(const JoinThread&) = delete;
    JoinThread& operator=(const JoinThread&) = delete;

    std::thread& get() { return t_; }
};

void demo_join_thread() {
    JoinThread jt([]() {
        safe_print("    JoinThread: 自动 join");
    });
    // 不需要手动 join — 析构时自动 join
    // 异常安全！
}

} // namespace ch1


// =============================================================================
// 第2章：线程传参与返回值
// =============================================================================

namespace ch2 {

// --- 2.1 值传递（默认拷贝）---

void process_value(int x) {
    safe_print("    值传递: x=", x);
}

// --- 2.2 引用传递（必须用 std::ref）---

void increment(int& x) {
    ++x;
}

void demo_ref_pass() {
    int value = 0;
    // std::thread t(increment, value);      // ❌ 不会修改 value（拷贝了）
    std::thread t(increment, std::ref(value)); // ✅ 真正的引用
    t.join();
    std::cout << "  引用传递后 value = " << value << "\n"; // 1
}

// --- 2.3 移动语义传参 ---

void take_ownership(std::unique_ptr<int> ptr) {
    safe_print("    移动传参: *ptr=", *ptr);
}

void demo_move_pass() {
    auto ptr = std::make_unique<int>(42);
    // std::thread t(take_ownership, ptr);             // ❌ unique_ptr 不可拷贝
    std::thread t(take_ownership, std::move(ptr));      // ✅ 移动
    t.join();
    // ptr 现在为 nullptr
}

// --- 2.4 返回值：通过 future ---

int compute_heavy(int x) {
    std::this_thread::sleep_for(10ms);
    return x * x;
}

void demo_return_value() {
    // 方式1: async
    auto future1 = std::async(std::launch::async, compute_heavy, 7);
    std::cout << "  async 返回值: " << future1.get() << "\n"; // 49

    // 方式2: packaged_task
    std::packaged_task<int(int)> task(compute_heavy);
    auto future2 = task.get_future();
    std::thread t(std::move(task), 8);
    t.join();
    std::cout << "  packaged_task 返回值: " << future2.get() << "\n"; // 64

    // 方式3: promise
    std::promise<int> prom;
    auto future3 = prom.get_future();
    std::thread t2([&prom]() {
        prom.set_value(9 * 9);
    });
    t2.join();
    std::cout << "  promise 返回值: " << future3.get() << "\n"; // 81
}

} // namespace ch2


// =============================================================================
// 第3章：互斥量 — mutex 家族
// =============================================================================

namespace ch3 {

// C++ 提供多种互斥量：
//   std::mutex              — 基础互斥量
//   std::timed_mutex        — 带超时的互斥量
//   std::recursive_mutex    — 可重入互斥量（同一线程可多次加锁）
//   std::shared_mutex       — 读写锁（C++17）

// --- 3.1 无保护的数据竞争 ---

void demo_data_race() {
    int counter = 0;
    constexpr int N = 100000;

    auto increment = [&]() {
        for (int i = 0; i < N; ++i) ++counter;
    };

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join(); t2.join();

    // 结果不确定！可能小于 200000
    std::cout << "  无保护 counter = " << counter
              << " (期望 " << 2 * N << ")\n";
}

// --- 3.2 mutex 保护 ---

void demo_mutex_protection() {
    int counter = 0;
    std::mutex mtx;
    constexpr int N = 100000;

    auto increment = [&]() {
        for (int i = 0; i < N; ++i) {
            mtx.lock();
            ++counter;
            mtx.unlock();
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join(); t2.join();

    std::cout << "  mutex 保护 counter = " << counter
              << " (期望 " << 2 * N << ")\n";
}

// --- 3.3 timed_mutex ---

void demo_timed_mutex() {
    std::timed_mutex tmtx;

    std::thread t([&]() {
        tmtx.lock();
        std::this_thread::sleep_for(100ms);
        tmtx.unlock();
    });

    std::this_thread::sleep_for(10ms); // 让 t 先获得锁

    // 尝试在 50ms 内获取锁
    if (tmtx.try_lock_for(50ms)) {
        std::cout << "  timed_mutex: 获取成功\n";
        tmtx.unlock();
    } else {
        std::cout << "  timed_mutex: 超时未获取\n";
    }

    t.join();
}

// --- 3.4 recursive_mutex ---

class RecursiveExample {
    std::recursive_mutex rmtx_;
    int value_ = 0;

public:
    void add(int x) {
        std::lock_guard<std::recursive_mutex> lock(rmtx_);
        value_ += x;
    }

    void add_twice(int x) {
        std::lock_guard<std::recursive_mutex> lock(rmtx_);
        add(x); // 内部再次加锁 — 普通 mutex 会死锁！
        add(x);
    }

    int get() const { return value_; }
};

void demo_recursive_mutex() {
    RecursiveExample obj;
    obj.add_twice(5);
    std::cout << "  recursive_mutex: " << obj.get() << " (期望 10)\n";
}

} // namespace ch3


// =============================================================================
// 第4章：锁管理 — lock_guard / unique_lock / scoped_lock
// =============================================================================

namespace ch4 {

// 永远不要手动 lock/unlock — 使用 RAII 锁管理器。

// --- 4.1 lock_guard: 最简单的 RAII 锁 ---

class SafeCounter {
    mutable std::mutex mtx_;
    int count_ = 0;
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx_);
        ++count_;
        // 异常安全：即使这里抛异常，锁也会释放
    }

    int get() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return count_;
    }
};

// --- 4.2 unique_lock: 灵活的锁管理 ---

void demo_unique_lock() {
    std::mutex mtx;

    // (a) 延迟加锁
    {
        std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
        // 此时未加锁
        lock.lock(); // 手动加锁
        // ... 临界区 ...
    } // 自动解锁

    // (b) 尝试加锁
    {
        std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
        if (lock.owns_lock()) {
            std::cout << "  unique_lock try_to_lock: 成功\n";
        }
    }

    // (c) 超时加锁
    {
        std::timed_mutex tmtx;
        std::unique_lock<std::timed_mutex> lock(tmtx, 100ms);
        if (lock.owns_lock()) {
            std::cout << "  unique_lock 超时加锁: 成功\n";
        }
    }

    // (d) 锁的转移
    {
        std::unique_lock<std::mutex> lock1(mtx);
        std::unique_lock<std::mutex> lock2 = std::move(lock1);
        // lock1 不再持有锁，lock2 持有
        std::cout << "  lock1 owns: " << std::boolalpha << lock1.owns_lock()
                  << ", lock2 owns: " << lock2.owns_lock() << "\n";
    }
}

// --- 4.3 scoped_lock (C++17): 同时锁定多个互斥量 ---

void demo_scoped_lock() {
    std::mutex mtx1, mtx2;

    // scoped_lock 使用 std::lock 算法避免死锁
    {
        std::scoped_lock lock(mtx1, mtx2); // 同时锁两个
        std::cout << "  scoped_lock: 同时持有两个锁\n";
    } // 同时释放

    // 等效于 C++11 的写法（更麻烦）：
    // std::lock(mtx1, mtx2);
    // std::lock_guard<std::mutex> lg1(mtx1, std::adopt_lock);
    // std::lock_guard<std::mutex> lg2(mtx2, std::adopt_lock);
}

} // namespace ch4


// =============================================================================
// 第5章：死锁与避免策略
// =============================================================================

namespace ch5 {

// 死锁的四个必要条件：
//   1. 互斥          — 资源不能共享
//   2. 持有并等待    — 持有一个锁的同时等待另一个
//   3. 不可抢占      — 不能强制释放他人的锁
//   4. 循环等待      — A 等 B，B 等 A

// --- 5.1 经典死锁示例 ---

void demo_deadlock_danger() {
    std::mutex mtxA, mtxB;

    // ⚠️ 以下代码可能死锁（已注释）
    /*
    auto thread1 = [&]() {
        std::lock_guard<std::mutex> lockA(mtxA);
        std::this_thread::sleep_for(1ms);
        std::lock_guard<std::mutex> lockB(mtxB);  // 等 B
    };

    auto thread2 = [&]() {
        std::lock_guard<std::mutex> lockB(mtxB);
        std::this_thread::sleep_for(1ms);
        std::lock_guard<std::mutex> lockA(mtxA);  // 等 A → 死锁!
    };
    */

    std::cout << "  (死锁示例已注释，避免实际死锁)\n";
}

// --- 5.2 避免策略 ---

// 策略1：固定锁的顺序
void strategy_fixed_order() {
    std::mutex mtxA, mtxB;

    auto safe_thread = [&]() {
        std::lock_guard<std::mutex> lockA(mtxA); // 总是先锁 A
        std::lock_guard<std::mutex> lockB(mtxB); // 再锁 B
    };

    std::thread t1(safe_thread);
    std::thread t2(safe_thread);
    t1.join(); t2.join();
    std::cout << "  策略1 (固定顺序): 安全\n";
}

// 策略2：std::lock + scoped_lock（推荐）
void strategy_std_lock() {
    std::mutex mtxA, mtxB;

    auto safe_thread = [&]() {
        std::scoped_lock lock(mtxA, mtxB); // 内部使用无死锁算法
        // ... 安全地使用两个资源 ...
    };

    std::thread t1(safe_thread);
    std::thread t2(safe_thread);
    t1.join(); t2.join();
    std::cout << "  策略2 (scoped_lock): 安全\n";
}

// 策略3：try_lock 避免阻塞
void strategy_try_lock() {
    std::mutex mtxA, mtxB;

    auto safe_thread = [&]() {
        while (true) {
            std::unique_lock<std::mutex> lockA(mtxA, std::defer_lock);
            std::unique_lock<std::mutex> lockB(mtxB, std::defer_lock);

            if (std::try_lock(lockA, lockB) == -1) {
                // 成功获取两个锁
                break;
            }
            // 失败则重试
            std::this_thread::yield();
        }
    };

    std::thread t1(safe_thread);
    std::thread t2(safe_thread);
    t1.join(); t2.join();
    std::cout << "  策略3 (try_lock): 安全\n";
}

// --- 5.3 层级锁 (Hierarchical Lock) ---

class HierarchicalMutex {
    std::mutex internal_mtx_;
    unsigned long const hierarchy_value_;
    unsigned long previous_hierarchy_;
    static thread_local unsigned long this_thread_hierarchy_;

    void check_hierarchy() {
        if (hierarchy_value_ >= this_thread_hierarchy_) {
            throw std::logic_error("Mutex hierarchy violated");
        }
    }

public:
    explicit HierarchicalMutex(unsigned long value)
        : hierarchy_value_(value), previous_hierarchy_(0) {}

    void lock() {
        check_hierarchy();
        internal_mtx_.lock();
        previous_hierarchy_ = this_thread_hierarchy_;
        this_thread_hierarchy_ = hierarchy_value_;
    }

    void unlock() {
        this_thread_hierarchy_ = previous_hierarchy_;
        internal_mtx_.unlock();
    }

    bool try_lock() {
        check_hierarchy();
        if (!internal_mtx_.try_lock()) return false;
        previous_hierarchy_ = this_thread_hierarchy_;
        this_thread_hierarchy_ = hierarchy_value_;
        return true;
    }
};

thread_local unsigned long
HierarchicalMutex::this_thread_hierarchy_ = ULONG_MAX;

void demo_hierarchical_mutex() {
    HierarchicalMutex high_level(10000);
    HierarchicalMutex mid_level(5000);
    HierarchicalMutex low_level(1000);

    // ✅ 合法：从高到低
    {
        std::lock_guard<HierarchicalMutex> lk1(high_level);
        std::lock_guard<HierarchicalMutex> lk2(mid_level);
        std::lock_guard<HierarchicalMutex> lk3(low_level);
    }

    // ❌ 非法：从低到高 → 抛异常
    try {
        std::lock_guard<HierarchicalMutex> lk1(low_level);
        std::lock_guard<HierarchicalMutex> lk2(high_level); // 违反层级！
    } catch (const std::logic_error& e) {
        std::cout << "  层级锁违规: " << e.what() << "\n";
    }
}

} // namespace ch5


// =============================================================================
// 第6章：条件变量 — condition_variable
// =============================================================================

namespace ch6 {

// 条件变量用于线程间的等待/通知机制。
// 必须与 mutex + unique_lock 一起使用。

// --- 6.1 基本用法 ---

void demo_basic_condvar() {
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;

    // 等待者
    std::thread waiter([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return ready; }); // 等待 ready == true
        safe_print("    等待者: 收到通知, ready=true");
    });

    // 通知者
    std::this_thread::sleep_for(50ms);
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one(); // 唤醒一个等待者

    waiter.join();
}

// --- 6.2 注意：虚假唤醒 (Spurious Wakeup) ---
//
// cv.wait() 可能在没收到 notify 的情况下返回！
// 这是操作系统底层的行为。
// 因此必须用 predicate 版本的 wait：
//
//   cv.wait(lock, predicate);
//   等效于：
//   while (!predicate()) cv.wait(lock);
//
// 永远不要用无条件的 cv.wait(lock);

// --- 6.3 wait_for / wait_until ---

void demo_condvar_timeout() {
    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;

    std::thread t([&]() {
        std::this_thread::sleep_for(200ms);
        {
            std::lock_guard<std::mutex> lock(mtx);
            done = true;
        }
        cv.notify_one();
    });

    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, 50ms, [&]() { return done; })) {
            std::cout << "  wait_for: 在超时前收到通知\n";
        } else {
            std::cout << "  wait_for: 超时 (50ms), done=" << done << "\n";
        }
    }

    t.join();
}

// --- 6.4 notify_one vs notify_all ---
//
// notify_one: 唤醒一个等待线程（不确定哪个）
// notify_all: 唤醒所有等待线程
//
// 经验法则：
//   - 只有一个等待者 → notify_one
//   - 多个等待者需要同时响应 → notify_all
//   - 不确定 → notify_all（安全但可能多余唤醒）

void demo_notify_all() {
    std::mutex mtx;
    std::condition_variable cv;
    bool go = false;

    auto runner = [&](int id) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return go; });
        safe_print("    线程 ", id, " 开始运行");
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
        threads.emplace_back(runner, i);

    std::this_thread::sleep_for(50ms);
    {
        std::lock_guard<std::mutex> lock(mtx);
        go = true;
    }
    cv.notify_all(); // 所有5个线程同时开始

    for (auto& t : threads) t.join();
}

} // namespace ch6


// =============================================================================
// 第7章：生产者-消费者模式
// =============================================================================

namespace ch7 {

// 经典的线程协调模式。
// 一个或多个生产者往队列放数据，一个或多个消费者取数据。

template <typename T>
class ThreadSafeQueue {
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<T> queue_;
    bool closed_ = false;

public:
    // 生产者调用
    void push(T value) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (closed_) throw std::runtime_error("Queue is closed");
            queue_.push(std::move(value));
        }
        cv_.notify_one();
    }

    // 消费者调用：阻塞直到有数据或队列关闭
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() { return !queue_.empty() || closed_; });

        if (queue_.empty()) return std::nullopt; // 队列关闭且为空

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    // 非阻塞尝试取
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) return std::nullopt;
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    // 关闭队列（通知所有消费者停止等待）
    void close() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            closed_ = true;
        }
        cv_.notify_all();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }
};

void demo_producer_consumer() {
    ThreadSafeQueue<int> queue;
    std::atomic<int> consumed{0};

    constexpr int NUM_ITEMS = 100;
    constexpr int NUM_PRODUCERS = 2;
    constexpr int NUM_CONSUMERS = 3;

    // 生产者
    auto producer = [&](int id) {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            queue.push(id * 1000 + i);
        }
        safe_print("    生产者 ", id, " 完成");
    };

    // 消费者
    auto consumer = [&](int id) {
        int count = 0;
        while (auto val = queue.pop()) {
            ++count;
            consumed.fetch_add(1, std::memory_order_relaxed);
        }
        safe_print("    消费者 ", id, " 处理了 ", count, " 个项目");
    };

    std::vector<std::thread> threads;

    // 启动消费者
    for (int i = 0; i < NUM_CONSUMERS; ++i)
        threads.emplace_back(consumer, i);

    // 启动生产者
    for (int i = 0; i < NUM_PRODUCERS; ++i)
        threads.emplace_back(producer, i);

    // 等待生产者完成
    for (int i = NUM_CONSUMERS; i < NUM_CONSUMERS + NUM_PRODUCERS; ++i)
        threads[i].join();

    // 关闭队列，让消费者退出
    queue.close();

    // 等待消费者完成
    for (int i = 0; i < NUM_CONSUMERS; ++i)
        threads[i].join();

    std::cout << "  总消费数: " << consumed.load()
              << " (期望 " << NUM_PRODUCERS * NUM_ITEMS << ")\n";
}

} // namespace ch7


// =============================================================================
// 第8章：异步编程 — future / promise / async
// =============================================================================

namespace ch8 {

// --- 8.1 std::async ---
//
// std::launch::async   — 保证新线程执行
// std::launch::deferred — 延迟到 get() 时在调用线程执行
// 默认 (async|deferred) — 由实现决定

void demo_async() {
    // 并行计算
    auto f1 = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(50ms);
        return 42;
    });

    auto f2 = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(50ms);
        return 100;
    });

    // 两个任务并行执行，总时间约 50ms 而不是 100ms
    int result = f1.get() + f2.get();
    std::cout << "  async 并行结果: " << result << " (期望 142)\n";
}

// --- 8.2 std::promise + std::future ---
//
// promise 是写端,  future 是读端。
// 一对一的单次通信通道。

void demo_promise_future() {
    std::promise<std::string> prom;
    std::future<std::string> fut = prom.get_future();

    std::thread producer([&prom]() {
        std::this_thread::sleep_for(30ms);
        prom.set_value("Hello from promise!");
    });

    // future.get() 阻塞直到 promise 设置值
    std::cout << "  promise/future: " << fut.get() << "\n";
    producer.join();
}

// --- 8.3 promise 异常传递 ---

void demo_promise_exception() {
    std::promise<int> prom;
    auto fut = prom.get_future();

    std::thread t([&prom]() {
        try {
            throw std::runtime_error("计算出错");
        } catch (...) {
            prom.set_exception(std::current_exception());
        }
    });

    try {
        fut.get();
    } catch (const std::runtime_error& e) {
        std::cout << "  promise 异常传递: " << e.what() << "\n";
    }
    t.join();
}

// --- 8.4 std::packaged_task ---
//
// 将可调用对象包装成异步任务，可以延迟执行。

void demo_packaged_task() {
    // 创建任务（但不执行）
    std::packaged_task<int(int, int)> task([](int a, int b) {
        return a + b;
    });

    auto future = task.get_future();

    // 可以在任意线程执行
    std::thread t(std::move(task), 3, 4);
    t.join();

    std::cout << "  packaged_task: 3+4=" << future.get() << "\n";
}

// --- 8.5 std::shared_future ---
//
// future 只能 get() 一次。
// shared_future 可以被多个线程 get()。

void demo_shared_future() {
    auto sf = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(30ms);
        return 777;
    }).share(); // 转为 shared_future

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([sf, i]() {
            int val = sf.get(); // 多个线程可以安全调用
            safe_print("    线程 ", i, " 读取 shared_future: ", val);
        });
    }

    for (auto& t : threads) t.join();
}

} // namespace ch8


// =============================================================================
// 第9章：原子操作 — std::atomic
// =============================================================================

namespace ch9 {

// atomic 提供无锁（或低锁）的线程安全操作。
// 对于简单的计数器/标志位，atomic 远快于 mutex。

// --- 9.1 atomic 基础 ---

void demo_atomic_basic() {
    std::atomic<int> counter{0};
    constexpr int N = 1000000;

    auto increment = [&]() {
        for (int i = 0; i < N; ++i)
            counter.fetch_add(1, std::memory_order_relaxed);
    };

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join(); t2.join();

    std::cout << "  atomic counter = " << counter.load()
              << " (期望 " << 2 * N << ")\n";
}

// --- 9.2 atomic 操作一览 ---

void demo_atomic_operations() {
    std::atomic<int> val{10};

    // load / store
    int v = val.load();                     // 读
    val.store(20);                          // 写

    // fetch_xxx: 返回旧值
    int old = val.fetch_add(5);             // old=20, val=25
    old = val.fetch_sub(3);                 // old=25, val=22
    old = val.fetch_and(0xFF);              // 位与
    old = val.fetch_or(0x100);              // 位或
    old = val.fetch_xor(0xFF);             // 位异或
    (void)old;

    // exchange: 设置新值，返回旧值
    int prev = val.exchange(100);          // prev=旧值, val=100

    // compare_exchange: CAS (Compare-And-Swap)
    int expected = 100;
    bool success = val.compare_exchange_strong(expected, 200);
    // 如果 val==expected → val=200, 返回 true
    // 如果 val!=expected → expected=val的值, 返回 false

    std::cout << "  CAS: success=" << std::boolalpha << success
              << ", val=" << val.load() << ", prev=" << prev << "\n";
}

// --- 9.3 atomic vs mutex 性能 ---

void demo_atomic_vs_mutex() {
    constexpr int N = 5000000;

    // atomic
    {
        std::atomic<int> counter{0};
        Timer t("atomic 递增");
        auto inc = [&]() {
            for (int i = 0; i < N; ++i)
                counter.fetch_add(1, std::memory_order_relaxed);
        };
        std::thread t1(inc), t2(inc);
        t1.join(); t2.join();
    }

    // mutex
    {
        int counter = 0;
        std::mutex mtx;
        Timer t("mutex 递增");
        auto inc = [&]() {
            for (int i = 0; i < N; ++i) {
                std::lock_guard<std::mutex> lock(mtx);
                ++counter;
            }
        };
        std::thread t1(inc), t2(inc);
        t1.join(); t2.join();
    }
}

// --- 9.4 atomic_flag: 最简单的原子类型 ---

class SpinLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 自旋等待
            // C++20: flag_.wait(true); // 更高效的等待
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};

void demo_spinlock() {
    SpinLock spin;
    int counter = 0;
    constexpr int N = 100000;

    auto increment = [&]() {
        for (int i = 0; i < N; ++i) {
            spin.lock();
            ++counter;
            spin.unlock();
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join(); t2.join();

    std::cout << "  SpinLock counter = " << counter
              << " (期望 " << 2 * N << ")\n";
}

} // namespace ch9


// =============================================================================
// 第10章：内存序 — memory_order 详解
// =============================================================================

namespace ch10 {

// 编译器和 CPU 可能会重排内存操作以提高性能。
// memory_order 控制这种重排行为。
//
// 从弱到强：
//
// memory_order_relaxed   — 无顺序保证，仅保证原子性
// memory_order_consume   — (不推荐使用，编译器通常提升为 acquire)
// memory_order_acquire   — 读操作：此操作之后的读写不会被重排到之前
// memory_order_release   — 写操作：此操作之前的读写不会被重排到之后
// memory_order_acq_rel   — 同时具有 acquire 和 release 语义
// memory_order_seq_cst   — 顺序一致（默认，最强，最慢）

// --- 10.1 Relaxed: 仅保证原子性 ---

void demo_relaxed() {
    std::atomic<int> counter{0};
    constexpr int N = 1000000;

    // relaxed 足以做简单计数
    auto inc = [&]() {
        for (int i = 0; i < N; ++i)
            counter.fetch_add(1, std::memory_order_relaxed);
    };

    std::thread t1(inc), t2(inc);
    t1.join(); t2.join();
    std::cout << "  relaxed counter = " << counter << "\n";
}

// --- 10.2 Acquire-Release: 建立 happens-before 关系 ---
//
// Release 写 "发布" 之前的所有修改。
// Acquire 读 "获取" 这些修改。
// 适用于：生产者-消费者、锁实现。

void demo_acquire_release() {
    std::atomic<bool> flag{false};
    int data = 0;

    std::thread producer([&]() {
        data = 42;                                          // (1) 普通写
        flag.store(true, std::memory_order_release);         // (2) release 写
        // release 保证 (1) 在 (2) 之前完成（对其他线程可见）
    });

    std::thread consumer([&]() {
        while (!flag.load(std::memory_order_acquire)) {}     // (3) acquire 读
        // acquire 保证 (3) 之后的读能看到 (2) 之前的写
        assert(data == 42);                                  // (4) 保证看到 42
        safe_print("    acquire-release: data=", data, " ✓");
    });

    producer.join();
    consumer.join();
}

// --- 10.3 Sequential Consistency (默认) ---
//
// 所有线程看到的操作顺序完全相同。
// 最直观，但也最慢（需要完全的内存屏障）。

void demo_seq_cst() {
    std::atomic<bool> x{false}, y{false};
    std::atomic<int> z{0};

    // seq_cst 保证全局一致的顺序
    std::thread t1([&]() {
        x.store(true, std::memory_order_seq_cst);          // A
    });
    std::thread t2([&]() {
        y.store(true, std::memory_order_seq_cst);          // B
    });
    std::thread t3([&]() {
        while (!x.load(std::memory_order_seq_cst)) {}      // C: 看到 A
        if (y.load(std::memory_order_seq_cst))              // D
            z.fetch_add(1, std::memory_order_relaxed);
    });
    std::thread t4([&]() {
        while (!y.load(std::memory_order_seq_cst)) {}      // E: 看到 B
        if (x.load(std::memory_order_seq_cst))              // F
            z.fetch_add(1, std::memory_order_relaxed);
    });

    t1.join(); t2.join(); t3.join(); t4.join();

    // seq_cst 保证 z != 0
    // (用 relaxed 则不能保证)
    std::cout << "  seq_cst: z = " << z << " (保证 > 0)\n";
}

// --- 10.4 选择指南 ---
//
// | 场景                  | 推荐内存序              |
// |----------------------|------------------------|
// | 简单计数             | relaxed                |
// | 锁的实现             | acquire (lock) + release (unlock) |
// | 生产者-消费者标志     | acquire + release      |
// | 不确定               | seq_cst (默认)          |
// | 高性能要求明确场景    | 针对性使用弱内存序      |

} // namespace ch10


// =============================================================================
// 第11章：线程安全的单例
// =============================================================================

namespace ch11 {

// --- 11.1 std::call_once (C++11) ---

class Singleton1 {
    static std::once_flag init_flag_;
    static Singleton1* instance_;

    Singleton1() {
        safe_print("    Singleton1 构造 (call_once)");
    }

public:
    static Singleton1& instance() {
        std::call_once(init_flag_, []() {
            instance_ = new Singleton1();
        });
        return *instance_;
    }

    void hello() { safe_print("    Singleton1::hello()"); }

    Singleton1(const Singleton1&) = delete;
    Singleton1& operator=(const Singleton1&) = delete;
};

std::once_flag Singleton1::init_flag_;
Singleton1* Singleton1::instance_ = nullptr;

// --- 11.2 Meyer's Singleton (推荐 - C++11 起线程安全) ---
//
// C++11 标准保证：static 局部变量的初始化是线程安全的。
// 这是最简洁、最推荐的方式。

class Singleton2 {
    Singleton2() { safe_print("    Singleton2 构造 (Meyer's)"); }
public:
    static Singleton2& instance() {
        static Singleton2 inst; // 线程安全的一次初始化
        return inst;
    }

    void hello() { safe_print("    Singleton2::hello()"); }

    Singleton2(const Singleton2&) = delete;
    Singleton2& operator=(const Singleton2&) = delete;
};

void demo_singleton() {
    std::vector<std::thread> threads;

    // 多线程同时访问 — 只构造一次
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([]() {
            Singleton2::instance().hello();
        });
    }

    for (auto& t : threads) t.join();
}

} // namespace ch11


// =============================================================================
// 第12章：读写锁 — shared_mutex (C++17)
// =============================================================================

namespace ch12 {

// 读写锁允许：
//   多个读者同时读（shared lock）
//   只有一个写者写（exclusive lock）
//   读写互斥

template <typename K, typename V>
class ConcurrentMap {
    mutable std::shared_mutex smtx_;
    std::unordered_map<K, V> data_;

public:
    // 写操作：独占锁
    void insert(const K& key, const V& value) {
        std::unique_lock<std::shared_mutex> lock(smtx_);
        data_[key] = value;
    }

    // 读操作：共享锁（多个读者可并行）
    std::optional<V> find(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(smtx_);
        auto it = data_.find(key);
        if (it != data_.end()) return it->second;
        return std::nullopt;
    }

    // 删除：独占锁
    bool erase(const K& key) {
        std::unique_lock<std::shared_mutex> lock(smtx_);
        return data_.erase(key) > 0;
    }

    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(smtx_);
        return data_.size();
    }
};

void demo_concurrent_map() {
    ConcurrentMap<std::string, int> map;

    constexpr int N_WRITERS = 2;
    constexpr int N_READERS = 4;
    constexpr int N_ITEMS = 1000;

    std::atomic<int> reads_done{0};

    // 写者
    auto writer = [&](int id) {
        for (int i = 0; i < N_ITEMS; ++i)
            map.insert("key_" + std::to_string(id * N_ITEMS + i), i);
    };

    // 读者
    auto reader = [&](int id) {
        int found = 0;
        for (int i = 0; i < N_ITEMS * N_WRITERS; ++i) {
            if (map.find("key_" + std::to_string(i)))
                ++found;
        }
        reads_done.fetch_add(found, std::memory_order_relaxed);
    };

    {
        Timer t("ConcurrentMap 读写");
        std::vector<std::thread> threads;

        for (int i = 0; i < N_WRITERS; ++i)
            threads.emplace_back(writer, i);
        for (int i = 0; i < N_READERS; ++i)
            threads.emplace_back(reader, i);

        for (auto& t : threads) t.join();
    }

    std::cout << "  Map size: " << map.size()
              << ", total reads hit: " << reads_done << "\n";
}

} // namespace ch12


// =============================================================================
// 第13章：线程池实现
// =============================================================================

namespace ch13 {

class ThreadPool {
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;

public:
    explicit ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx_);
                        cv_.wait(lock, [this]() {
                            return stop_ || !tasks_.empty();
                        });

                        if (stop_ && tasks_.empty()) return;

                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& w : workers_) w.join();
    }

    // 提交任务，返回 future
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using ReturnType = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        auto future = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (stop_) throw std::runtime_error("ThreadPool is stopped");
            tasks_.emplace([task]() { (*task)(); });
        }
        cv_.notify_one();

        return future;
    }

    // 禁止拷贝
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};

void demo_thread_pool() {
    ThreadPool pool(4); // 4个工作线程

    // 提交多个任务
    std::vector<std::future<int>> results;

    for (int i = 0; i < 10; ++i) {
        results.push_back(pool.submit([i]() {
            std::this_thread::sleep_for(10ms);
            return i * i;
        }));
    }

    // 收集结果
    std::cout << "  线程池结果: ";
    for (auto& f : results) {
        std::cout << f.get() << " ";
    }
    std::cout << "\n";

    // 更复杂的示例：并行求和
    constexpr int N = 10000000;
    std::vector<int> data(N);
    std::iota(data.begin(), data.end(), 1);

    {
        Timer t("线程池并行求和");
        constexpr int CHUNKS = 8;
        int chunk_size = N / CHUNKS;

        std::vector<std::future<long long>> sums;
        for (int c = 0; c < CHUNKS; ++c) {
            int start = c * chunk_size;
            int end = (c == CHUNKS - 1) ? N : start + chunk_size;
            sums.push_back(pool.submit([&data, start, end]() {
                long long s = 0;
                for (int i = start; i < end; ++i) s += data[i];
                return s;
            }));
        }

        long long total = 0;
        for (auto& f : sums) total += f.get();

        long long expected = (long long)N * (N + 1) / 2;
        std::cout << "  并行求和: " << total << " (期望 " << expected << ")\n";
    }
}

} // namespace ch13


// =============================================================================
// 第14章：并行算法 (C++17 Execution Policies)
// =============================================================================
//
// C++17 为 STL 算法添加了执行策略：
//
//   std::execution::seq        — 顺序执行（与不加策略相同）
//   std::execution::par        — 并行执行
//   std::execution::par_unseq  — 并行+向量化
//
// 注意：
//   1. 需要编译器支持（GCC 需要 TBB: -ltbb）
//   2. MSVC 原生支持
//   3. 并行算法中不能有数据竞争
//
// 由于编译器支持差异，这里用手动并行模拟展示概念。

namespace ch14 {

// --- 14.1 手动并行 for_each ---

template <typename Iter, typename Func>
void parallel_for_each(Iter begin, Iter end, Func f, int num_threads = 0) {
    // NOTE: 空区间直接返回，避免后续分块逻辑产生无意义线程。
    if (begin == end) return;

    // NOTE: hardware_concurrency() 允许返回 0，需兜底到 1。
    if (num_threads <= 0)
        num_threads = static_cast<int>(std::thread::hardware_concurrency());
    if (num_threads <= 0)
        num_threads = 1;

    auto total = std::distance(begin, end);
    if (total < num_threads)
        num_threads = static_cast<int>(total);
    auto chunk = total / num_threads;

    std::vector<std::thread> threads;
    auto chunk_begin = begin;

    for (int i = 0; i < num_threads; ++i) {
        auto chunk_end = (i == num_threads - 1)
            ? end
            : std::next(chunk_begin, chunk);

        threads.emplace_back([=]() {
            std::for_each(chunk_begin, chunk_end, f);
        });

        chunk_begin = chunk_end;
    }

    for (auto& t : threads) t.join();
}

// --- 14.2 手动并行 reduce ---

template <typename Iter, typename T, typename BinaryOp>
T parallel_reduce(Iter begin, Iter end, T init, BinaryOp op,
                  int num_threads = 0) {
    // NOTE: 空区间返回初始值，语义与串行 reduce 一致。
    if (begin == end) return init;

    if (num_threads <= 0)
        num_threads = static_cast<int>(std::thread::hardware_concurrency());
    if (num_threads <= 0)
        num_threads = 1;

    auto total = std::distance(begin, end);
    if (total < num_threads)
        num_threads = static_cast<int>(total);
    auto chunk = total / num_threads;

    std::vector<std::future<T>> futures;
    auto chunk_begin = begin;

    for (int i = 0; i < num_threads; ++i) {
        auto chunk_end = (i == num_threads - 1)
            ? end
            : std::next(chunk_begin, chunk);

        futures.push_back(std::async(std::launch::async,
            [=]() {
                return std::accumulate(chunk_begin, chunk_end, T{}, op);
            }
        ));

        chunk_begin = chunk_end;
    }

    T result = init;
    for (auto& f : futures) result = op(result, f.get());
    return result;
}

// --- 14.3 手动并行 sort ---

template <typename Iter>
void parallel_sort(Iter begin, Iter end, int depth = 0) {
    auto size = std::distance(begin, end);
    if (size < 10000 || depth > 4) {
        std::sort(begin, end);
        return;
    }

    auto mid = begin + size / 2;
    std::nth_element(begin, mid, end);

    auto f = std::async(std::launch::async,
        [=]() { parallel_sort(begin, mid, depth + 1); });
    parallel_sort(mid, end, depth + 1);
    f.wait();

    std::inplace_merge(begin, mid, end);
}

void demo_parallel_algos() {
    constexpr int N = 5000000;
    std::vector<int> data(N);
    std::iota(data.begin(), data.end(), 1);

    // 串行求和
    {
        Timer t("串行 accumulate");
        long long sum = std::accumulate(data.begin(), data.end(), 0LL);
        (void)sum;
    }

    // 并行求和
    {
        Timer t("并行 reduce");
        long long sum = parallel_reduce(data.begin(), data.end(), 0LL,
                                         std::plus<long long>{}, 4);
        (void)sum;
    }

    // 串行排序
    {
        auto d = data;
        std::reverse(d.begin(), d.end());
        Timer t("串行 sort");
        std::sort(d.begin(), d.end());
    }

    // 并行排序
    {
        auto d = data;
        std::reverse(d.begin(), d.end());
        Timer t("并行 sort");
        parallel_sort(d.begin(), d.end());
    }
}

} // namespace ch14


// =============================================================================
// 第15章：无锁编程 — Lock-Free SPSC Queue
// =============================================================================

namespace ch15 {

// SPSC = Single Producer Single Consumer
// 最简单的无锁结构：只需 atomic 做索引同步。

template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be power of 2");

    std::array<T, Capacity> buffer_;
    alignas(64) std::atomic<size_t> head_{0}; // 写者使用
    alignas(64) std::atomic<size_t> tail_{0}; // 读者使用

    static constexpr size_t MASK = Capacity - 1;

public:
    bool push(const T& value) {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t next = (head + 1) & MASK;

        if (next == tail_.load(std::memory_order_acquire))
            return false; // 队列满

        buffer_[head] = value;
        head_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        size_t tail = tail_.load(std::memory_order_relaxed);

        if (tail == head_.load(std::memory_order_acquire))
            return false; // 队列空

        value = buffer_[tail];
        tail_.store((tail + 1) & MASK, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }
};

void demo_spsc_queue() {
    SPSCQueue<int, 1024> queue;

    constexpr int N = 1000000;
    std::atomic<long long> sum{0};

    // 生产者
    std::thread producer([&]() {
        for (int i = 0; i < N; ++i) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
        }
    });

    // 消费者
    std::thread consumer([&]() {
        long long s = 0;
        for (int i = 0; i < N; ++i) {
            int val;
            while (!queue.pop(val)) {
                std::this_thread::yield();
            }
            s += val;
        }
        sum.store(s, std::memory_order_relaxed);
    });

    {
        Timer t("SPSC 无锁队列");
        producer.join();
        consumer.join();
    }

    long long expected = (long long)(N - 1) * N / 2;
    std::cout << "  SPSC sum = " << sum.load()
              << " (期望 " << expected << ")\n";
}

// --- MPMC 无锁队列 (多生产者多消费者) ---
// 更复杂，基于 CAS 循环

template <typename T, size_t Capacity>
class MPMCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be power of 2");

    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    alignas(64) std::array<Cell, Capacity> buffer_;
    alignas(64) std::atomic<size_t> enqueue_pos_{0};
    alignas(64) std::atomic<size_t> dequeue_pos_{0};

    static constexpr size_t MASK = Capacity - 1;

public:
    MPMCQueue() {
        for (size_t i = 0; i < Capacity; ++i)
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
    }

    bool push(const T& value) {
        Cell* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);

        while (true) {
            cell = &buffer_[pos & MASK];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)pos;

            if (diff == 0) {
                if (enqueue_pos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed))
                    break;
            } else if (diff < 0) {
                return false; // 队列满
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }

        cell->data = value;
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        Cell* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);

        while (true) {
            cell = &buffer_[pos & MASK];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

            if (diff == 0) {
                if (dequeue_pos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed))
                    break;
            } else if (diff < 0) {
                return false; // 队列空
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }

        value = cell->data;
        cell->sequence.store(pos + MASK + 1, std::memory_order_release);
        return true;
    }
};

void demo_mpmc_queue() {
    MPMCQueue<int, 1024> queue;

    constexpr int N_PRODUCERS = 4;
    constexpr int N_CONSUMERS = 4;
    constexpr int N_PER_PRODUCER = 100000;

    std::atomic<int> total_consumed{0};

    auto producer = [&](int id) {
        for (int i = 0; i < N_PER_PRODUCER; ++i) {
            while (!queue.push(id * N_PER_PRODUCER + i))
                std::this_thread::yield();
        }
    };

    std::atomic<bool> done{false};

    auto consumer = [&]() {
        int count = 0;
        while (!done.load(std::memory_order_relaxed) || true) {
            int val;
            if (queue.pop(val)) {
                ++count;
                if (total_consumed.fetch_add(1, std::memory_order_relaxed) + 1
                    >= N_PRODUCERS * N_PER_PRODUCER) {
                    done.store(true, std::memory_order_relaxed);
                    break;
                }
            } else if (done.load(std::memory_order_relaxed)) {
                break;
            } else {
                std::this_thread::yield();
            }
        }
    };

    {
        Timer t("MPMC 无锁队列");

        std::vector<std::thread> threads;
        for (int i = 0; i < N_CONSUMERS; ++i)
            threads.emplace_back(consumer);
        for (int i = 0; i < N_PRODUCERS; ++i)
            threads.emplace_back(producer, i);

        for (auto& t : threads) t.join();
    }

    std::cout << "  MPMC total consumed: " << total_consumed
              << " (期望 " << N_PRODUCERS * N_PER_PRODUCER << ")\n";
}

} // namespace ch15


// =============================================================================
// 第16章：实战 — 并发任务管道 (Pipeline)
// =============================================================================

namespace ch16 {

// 管道模式：数据流经多个处理阶段，每个阶段在不同线程。
// Stage1 → Queue → Stage2 → Queue → Stage3

using DataItem = std::pair<int, std::string>;

void demo_pipeline() {
    ch7::ThreadSafeQueue<int> raw_queue;
    ch7::ThreadSafeQueue<DataItem> processed_queue;
    ch7::ThreadSafeQueue<std::string> final_queue;

    // 阶段1：生成原始数据
    std::thread stage1([&]() {
        for (int i = 1; i <= 20; ++i) {
            raw_queue.push(i);
            std::this_thread::sleep_for(2ms);
        }
        raw_queue.close();
        safe_print("    Stage1: 生成完成");
    });

    // 阶段2：处理数据（可并行）
    auto stage2_worker = [&]() {
        while (auto val = raw_queue.pop()) {
            DataItem item{*val, "item_" + std::to_string(*val * *val)};
            processed_queue.push(std::move(item));
        }
    };

    std::thread stage2a(stage2_worker);
    std::thread stage2b(stage2_worker);

    // 阶段3：格式化输出
    std::thread stage3([&]() {
        while (auto item = processed_queue.pop()) {
            final_queue.push("[" + std::to_string(item->first) + ": "
                            + item->second + "]");
        }
        final_queue.close();
        safe_print("    Stage3: 格式化完成");
    });

    // 等待阶段1完成，然后关闭后续队列
    stage1.join();
    stage2a.join();
    stage2b.join();
    processed_queue.close();
    stage3.join();

    // 收集结果
    std::cout << "  Pipeline 结果: ";
    int count = 0;
    while (auto result = final_queue.pop()) {
        if (count < 5) std::cout << *result << " ";
        ++count;
    }
    std::cout << "... 共 " << count << " 项\n";
}

} // namespace ch16


// =============================================================================
// 第17章：实战 — Map-Reduce 框架
// =============================================================================

namespace ch17 {

// Map-Reduce: 分解 → 并行处理 → 合并结果

template <typename InputIter, typename MapFunc, typename ReduceFunc>
auto parallel_map_reduce(
    InputIter begin, InputIter end,
    MapFunc map_fn, ReduceFunc reduce_fn,
    typename std::iterator_traits<InputIter>::value_type init,
    int num_threads = 0)
{
    // NOTE: 空区间直接返回 init，避免首元素解引用风险。
    if (begin == end) {
        using EmptyT = decltype(map_fn(*begin));
        return static_cast<EmptyT>(init);
    }

    using T = decltype(map_fn(*begin));

    if (num_threads <= 0)
        num_threads = static_cast<int>(std::thread::hardware_concurrency());
    if (num_threads <= 0)
        num_threads = 1;

    auto total = std::distance(begin, end);
    if (total < num_threads)
        num_threads = static_cast<int>(total);
    auto chunk_size = total / num_threads;

    // Map 阶段：每个线程处理一个分片
    std::vector<std::future<T>> futures;
    auto chunk_begin = begin;

    for (int i = 0; i < num_threads; ++i) {
        auto chunk_end = (i == num_threads - 1) ? end
            : std::next(chunk_begin, chunk_size);

        futures.push_back(std::async(std::launch::async,
            [=]() {
                T local_result = map_fn(*chunk_begin);
                auto it = std::next(chunk_begin);
                while (it != chunk_end) {
                    local_result = reduce_fn(local_result, map_fn(*it));
                    ++it;
                }
                return local_result;
            }
        ));

        chunk_begin = chunk_end;
    }

    // Reduce 阶段：合并所有分片结果
    T result = static_cast<T>(init);
    for (auto& f : futures)
        result = reduce_fn(result, f.get());

    return result;
}

void demo_map_reduce() {
    // 示例1：并行计算平方和
    {
        constexpr int N = 10000000;
        std::vector<int> data(N);
        std::iota(data.begin(), data.end(), 1);

        Timer t("MapReduce 平方和");
        long long sum_of_squares = parallel_map_reduce(
            data.begin(), data.end(),
            [](int x) -> long long { return (long long)x * x; },
            std::plus<long long>{},
            0, 4
        );

        // 1^2 + 2^2 + ... + N^2 = N*(N+1)*(2N+1)/6
        long long expected = (long long)N * (N + 1) * (2 * N + 1) / 6;
        std::cout << "  平方和: " << sum_of_squares
                  << " (期望 " << expected << ")\n";
    }

    // 示例2：并行词频统计（模拟）
    {
        std::vector<std::string> documents = {
            "hello world hello foo bar",
            "world bar baz hello world",
            "foo bar hello baz world",
            "hello hello world world baz",
        };

        // Map: 每个文档 → 词频 map
        // Reduce: 合并所有词频 map

        using WordCount = std::unordered_map<std::string, int>;
        std::mutex merge_mtx;
        WordCount global_counts;

        std::vector<std::future<void>> futures;
        for (const auto& doc : documents) {
            futures.push_back(std::async(std::launch::async,
                [&doc, &global_counts, &merge_mtx]() {
                    // Map
                    WordCount local;
                    std::istringstream iss(doc);
                    std::string word;
                    while (iss >> word) local[word]++;

                    // Reduce (合并到全局)
                    std::lock_guard<std::mutex> lock(merge_mtx);
                    for (auto& [w, c] : local)
                        global_counts[w] += c;
                }
            ));
        }

        for (auto& f : futures) f.get();

        std::cout << "  词频统计:\n";
        for (auto& [word, count] : global_counts)
            std::cout << "    " << word << ": " << count << "\n";
    }
}

} // namespace ch17


// =============================================================================
// 第18章：常见陷阱与调试技巧
// =============================================================================

namespace ch18 {

// --- 18.1 陷阱总结 ---
//
// 陷阱1: 忘记 join/detach → std::terminate
//   ❌ { std::thread t(func); } // 析构时 terminate!
//   ✅ { std::thread t(func); t.join(); }
//
// 陷阱2: 引用悬挂
//   ❌ {
//       int x = 42;
//       std::thread t([&x]() { /* 使用 x */ });
//       t.detach();
//   } // x 已销毁，线程仍在访问!
//   ✅ 按值捕获，或确保生命周期
//
// 陷阱3: 隐式拷贝
//   ❌ void func(int& x) { x++; }
//      std::thread t(func, my_var); // 拷贝了 my_var!
//   ✅ std::thread t(func, std::ref(my_var));
//
// 陷阱4: std::async 返回值未保存
//   ❌ std::async(std::launch::async, func); // 阻塞！
//      // 临时 future 立即析构，等待任务完成
//   ✅ auto f = std::async(std::launch::async, func);
//
// 陷阱5: 锁的粒度
//   ❌ 锁住整个函数 → 串行化，失去并行性
//   ✅ 只锁住必要的临界区
//
// 陷阱6: false sharing
//   ❌ struct { atomic<int> a; atomic<int> b; };
//      // a 和 b 在同一缓存行，互相'干扰'
//   ✅ struct { alignas(64) atomic<int> a; alignas(64) atomic<int> b; };

// --- 18.2 false sharing 演示 ---

struct BadCounters {
    std::atomic<int> a{0};
    std::atomic<int> b{0};
    // a 和 b 在同一个 64 字节缓存行
};

struct GoodCounters {
    alignas(64) std::atomic<int> a{0};
    alignas(64) std::atomic<int> b{0};
    // a 和 b 在不同缓存行
};

template <typename Counters>
void bench_counters(const char* label) {
    Counters c;
    constexpr int N = 5000000;

    Timer t(label);
    std::thread t1([&]() {
        for (int i = 0; i < N; ++i)
            c.a.fetch_add(1, std::memory_order_relaxed);
    });
    std::thread t2([&]() {
        for (int i = 0; i < N; ++i)
            c.b.fetch_add(1, std::memory_order_relaxed);
    });
    t1.join(); t2.join();
}

void demo_false_sharing() {
    bench_counters<BadCounters>("false sharing (同缓存行)");
    bench_counters<GoodCounters>("无 false sharing (不同缓存行)");
}

// --- 18.3 调试建议 ---
//
// 工具：
//   1. ThreadSanitizer (TSan):
//      g++ -fsanitize=thread -g -O1 test6.cpp -o test6
//      检测数据竞争、死锁
//
//   2. Valgrind (Helgrind):
//      valgrind --tool=helgrind ./test6
//
//   3. AddressSanitizer (ASan):
//      g++ -fsanitize=address -g test6.cpp -o test6
//      检测内存错误（use-after-free 等）
//
// 技巧：
//   - 用 assert + 日志定位问题
//   - 减少共享可变状态
//   - 优先使用不可变数据
//   - 尽早建立正确的抽象（ThreadSafeQueue, ThreadPool）
//   - 用 scoped_lock 而非手动 lock/unlock
//   - 考虑线程本地存储 (thread_local)

// --- 18.4 thread_local ---

thread_local int tl_counter = 0;

void demo_thread_local() {
    auto worker = [](int id) {
        for (int i = 0; i < 5; ++i) {
            ++tl_counter;
        }
        safe_print("    线程 ", id, " 的 thread_local counter = ", tl_counter);
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join(); t2.join();

    std::cout << "  主线程的 thread_local counter = " << tl_counter << "\n";
    // 每个线程都有独立的副本!
}

// --- 18.5 async 陷阱演示 ---

void demo_async_trap() {
    // ⚠️ 未保存 future → 阻塞!
    {
        Timer t("async 陷阱: 未保存 future");
        for (int i = 0; i < 5; ++i) {
            // 每次循环都阻塞等待! 因为临时 future 立即析构
            std::async(std::launch::async, []() {
                std::this_thread::sleep_for(10ms);
            });
        }
    }

    // ✅ 保存 future → 真正并行
    {
        Timer t("async 正确: 保存 future");
        std::vector<std::future<void>> futures;
        for (int i = 0; i < 5; ++i) {
            futures.push_back(std::async(std::launch::async, []() {
                std::this_thread::sleep_for(10ms);
            }));
        }
        for (auto& f : futures) f.get();
    }
}

} // namespace ch18


// =============================================================================
// 附录：多线程核心概念速查表
// =============================================================================
//
// 同步原语：
// ───────────────────────────────────────────────────────────────────
// | 原语              | 用途                    | 开销   |
// |-------------------|------------------------|--------|
// | mutex             | 互斥访问                | 中     |
// | recursive_mutex   | 可重入互斥              | 中高   |
// | timed_mutex       | 带超时的互斥            | 中     |
// | shared_mutex      | 读写锁                  | 中     |
// | condition_variable| 线程等待/通知            | 低     |
// | atomic<T>         | 无锁原子操作            | 低     |
// | atomic_flag       | 最简原子标志            | 极低   |
// | call_once         | 一次初始化              | 低     |
//
// 锁管理器（RAII）：
// ───────────────────────────────────────────────────────────────────
// | 管理器          | 特点                         | 推荐度 |
// |----------------|------------------------------|--------|
// | lock_guard     | 简单、不可中途解锁            | ★★★★  |
// | unique_lock    | 灵活、可转移、可中途解锁       | ★★★★★ |
// | scoped_lock    | 同时锁多个 mutex、防死锁       | ★★★★★ |
// | shared_lock    | 共享读锁                     | ★★★★  |
//
// 异步机制：
// ───────────────────────────────────────────────────────────────────
// | 机制             | 用途                     | 返回值 |
// |-----------------|--------------------------|--------|
// | std::async      | 简单异步任务              | future |
// | promise/future  | 一对一跨线程通信           | T      |
// | packaged_task   | 可延迟执行的任务          | future |
// | shared_future   | 多个消费者共享结果         | T      |
//
// 内存序 (从弱到强)：
// ───────────────────────────────────────────────────────────────────
// | 内存序         | 保证         | 使用场景           | 性能  |
// |---------------|-------------|-------------------|-------|
// | relaxed       | 仅原子性     | 计数器             | 最快  |
// | acquire       | 读不后移     | 锁 acquire 端      | 快    |
// | release       | 写不前移     | 锁 release 端      | 快    |
// | acq_rel       | acquire+release | CAS 循环       | 中    |
// | seq_cst       | 全局一致     | 默认,不确定时用     | 慢    |
// =============================================================================


// =============================================================================
// main: 运行演示
// =============================================================================
int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "===== 现代 C++ 多线程教程 演示 =====\n\n";

    // 1. 线程基础
    std::cout << "[1] 线程基础:\n";
    ch1::demo_thread_creation();
    ch1::demo_thread_info();
    ch1::demo_thread_lifecycle();
    ch1::demo_join_thread();
    std::cout << "\n";

    // 2. 线程传参
    std::cout << "[2] 线程传参与返回值:\n";
    ch2::demo_ref_pass();
    ch2::demo_move_pass();
    ch2::demo_return_value();
    std::cout << "\n";

    // 3. 互斥量
    std::cout << "[3] 互斥量:\n";
    ch3::demo_data_race();
    ch3::demo_mutex_protection();
    ch3::demo_timed_mutex();
    ch3::demo_recursive_mutex();
    std::cout << "\n";

    // 4. 锁管理
    std::cout << "[4] 锁管理:\n";
    ch4::demo_unique_lock();
    ch4::demo_scoped_lock();
    std::cout << "\n";

    // 5. 死锁
    std::cout << "[5] 死锁与避免:\n";
    ch5::strategy_fixed_order();
    ch5::strategy_std_lock();
    ch5::strategy_try_lock();
    ch5::demo_hierarchical_mutex();
    std::cout << "\n";

    // 6. 条件变量
    std::cout << "[6] 条件变量:\n";
    ch6::demo_basic_condvar();
    ch6::demo_condvar_timeout();
    ch6::demo_notify_all();
    std::cout << "\n";

    // 7. 生产者消费者
    std::cout << "[7] 生产者-消费者:\n";
    ch7::demo_producer_consumer();
    std::cout << "\n";

    // 8. 异步编程
    std::cout << "[8] 异步编程:\n";
    ch8::demo_async();
    ch8::demo_promise_future();
    ch8::demo_promise_exception();
    ch8::demo_packaged_task();
    ch8::demo_shared_future();
    std::cout << "\n";

    // 9. 原子操作
    std::cout << "[9] 原子操作:\n";
    ch9::demo_atomic_basic();
    ch9::demo_atomic_operations();
    ch9::demo_atomic_vs_mutex();
    ch9::demo_spinlock();
    std::cout << "\n";

    // 10. 内存序
    std::cout << "[10] 内存序:\n";
    ch10::demo_relaxed();
    ch10::demo_acquire_release();
    ch10::demo_seq_cst();
    std::cout << "\n";

    // 11. 单例
    std::cout << "[11] 线程安全单例:\n";
    ch11::demo_singleton();
    std::cout << "\n";

    // 12. 读写锁
    std::cout << "[12] 读写锁:\n";
    ch12::demo_concurrent_map();
    std::cout << "\n";

    // 13. 线程池
    std::cout << "[13] 线程池:\n";
    ch13::demo_thread_pool();
    std::cout << "\n";

    // 14. 并行算法
    std::cout << "[14] 并行算法:\n";
    ch14::demo_parallel_algos();
    std::cout << "\n";

    // 15. 无锁队列
    std::cout << "[15] 无锁队列:\n";
    ch15::demo_spsc_queue();
    ch15::demo_mpmc_queue();
    std::cout << "\n";

    // 16. 管道
    std::cout << "[16] 并发管道:\n";
    ch16::demo_pipeline();
    std::cout << "\n";

    // 17. Map-Reduce
    std::cout << "[17] Map-Reduce:\n";
    ch17::demo_map_reduce();
    std::cout << "\n";

    // 18. 陷阱与调试
    std::cout << "[18] 陷阱与调试:\n";
    ch18::demo_false_sharing();
    ch18::demo_thread_local();
    ch18::demo_async_trap();

    std::cout << "\n===== 演示完成 =====\n";
    return 0;
}
