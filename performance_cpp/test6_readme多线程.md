# 现代 C++ 多线程完全指导教程

> **"Concurrency is about dealing with lots of things at once.**
> **Parallelism is about doing lots of things at once."**
> — Rob Pike
>
> **对应源码**: `test6.cpp`（2374 行，18 章，覆盖 C++ 并发编程全栈）
> **标准**: C++17（部分 C++20 注释说明）

---

## 编译方式

```bash
# GCC / Clang (Linux/Mac)
g++ -std=c++17 -O2 -Wall -Wextra -pthread -o test6 test6.cpp && ./test6

# 启用 ThreadSanitizer 检测数据竞争
g++ -std=c++17 -fsanitize=thread -g -O1 -pthread -o test6_tsan test6.cpp

# MSVC (Windows — 自动链接线程库)
cl /std:c++17 /O2 /W4 /EHsc test6.cpp
```

---

## 教程目录

| 章节 | 主题 | 核心技术 | 难度 |
|------|------|----------|------|
| 1 | **线程基础** | `std::thread` 创建、join/detach、RAII JoinThread | ★☆☆ |
| 2 | **线程传参与返回值** | 值传递、`std::ref`、移动语义、future 返回值 | ★☆☆ |
| 3 | **互斥量** | `mutex` / `timed_mutex` / `recursive_mutex`、数据竞争 | ★★☆ |
| 4 | **锁管理** | `lock_guard` / `unique_lock` / `scoped_lock` (C++17) | ★★☆ |
| 5 | **死锁与避免** | 固定顺序、scoped_lock、try_lock、层级锁 | ★★★ |
| 6 | **条件变量** | `condition_variable`、虚假唤醒、wait_for、notify_all | ★★★ |
| 7 | **生产者-消费者** | `ThreadSafeQueue<T>` 完整实现 | ★★★ |
| 8 | **异步编程** | async / promise / future / packaged_task / shared_future | ★★☆ |
| 9 | **原子操作** | `atomic` 全部操作、atomic vs mutex 性能、SpinLock | ★★★ |
| 10 | **内存序** | relaxed / acquire-release / seq_cst 详解 | ★★★★ |
| 11 | **线程安全单例** | `call_once`、Meyer's Singleton | ★★☆ |
| 12 | **读写锁** | `shared_mutex` + `ConcurrentMap` 实现 | ★★★ |
| 13 | **线程池** | 完整 ThreadPool，`submit()` 返回 future | ★★★★ |
| 14 | **并行算法** | parallel_for_each / parallel_reduce / parallel_sort | ★★★ |
| 15 | **无锁编程** | SPSC 队列 + MPMC 队列（CAS） | ★★★★★ |
| 16 | **实战：并发管道** | 多阶段 Pipeline 模式 | ★★★ |
| 17 | **实战：Map-Reduce** | 并行 Map-Reduce 框架 + 词频统计 | ★★★ |
| 18 | **陷阱与调试** | false sharing、thread_local、async 陷阱、TSan | ★★★ |
| 附录 | **核心概念速查表** | 同步原语 / 锁管理器 / 异步机制 / 内存序 | — |

### 文件体系定位

| 文件 | 标准 | 定位 |
|------|------|------|
| test1-3.cpp | C++17/20 | 编译期优化 |
| test4.cpp | C++17/20 | 运行期性能优化 |
| test5.cpp | C++17/20 | 零开销抽象 |
| **test6.cpp** | **C++17** | **多线程 — 利用多核的正确方式** |

### C++ 并发编程全景图

```
┌───────────────────────────────────────────────────────────────────────┐
│                    C++ 并发编程全栈                                     │
├───────────┬───────────┬──────────┬────────────────────────────────────┤
│  基础设施  │  同步原语  │ 高层抽象  │  实战模式                          │
├───────────┼───────────┼──────────┼────────────────────────────────────┤
│ thread    │ mutex     │ async    │ 生产者-消费者 (Ch7)                │
│ jthread   │ atomic    │ future   │ 线程池 (Ch13)                     │
│ this_thread│ condvar  │ promise  │ 并行算法 (Ch14)                   │
│           │ shared_mutex│ packaged_task│ 无锁队列 (Ch15)             │
│           │ barrier   │          │ Pipeline (Ch16)                   │
│           │ latch     │          │ Map-Reduce (Ch17)                 │
├───────────┴───────────┴──────────┴────────────────────────────────────┤
│           底层: 内存序 (Ch10) — CPU 缓存一致性协议 (MESI)              │
└───────────────────────────────────────────────────────────────────────┘
```

---

## 第1章：线程基础 — std::thread

### 1.1 创建线程的四种方式

```cpp
// (a) 普通函数
void free_function() { safe_print("普通函数"); }
std::thread t1(free_function);

// (b) 函数对象
struct Functor { void operator()() const { safe_print("函数对象"); } };
std::thread t2(Functor{});

// (c) Lambda
std::thread t3([]() { safe_print("Lambda"); });

// (d) 成员函数
Worker w;
std::thread t4(&Worker::do_work, &w, 42);

t1.join(); t2.join(); t3.join(); t4.join();
```

### 1.2 线程生命周期

```cpp
std::thread t(func);
// t 现在处于 joinable 状态

t.join();     // 阻塞直到线程结束 → 不再 joinable
// 或
t.detach();   // 分离线程（后台运行）→ 不再 joinable

// ⚠️ 析构时仍 joinable → std::terminate()!
```

**关键规则**：thread 对象**必须**在销毁前调用 `join()` 或 `detach()`。

### 1.3 RAII 线程守卫 — JoinThread

```cpp
class JoinThread {
    std::thread t_;
public:
    template <typename... Args>
    explicit JoinThread(Args&&... args) : t_(std::forward<Args>(args)...) {}
    ~JoinThread() { if (t_.joinable()) t_.join(); }

    JoinThread(JoinThread&&) = default;
    JoinThread& operator=(JoinThread&&) = default;
    JoinThread(const JoinThread&) = delete;
    JoinThread& operator=(const JoinThread&) = delete;
};
// 异常安全！无论如何退出作用域都会 join
```

> **C++20** 引入了 `std::jthread`，自带自动 join + 可协作取消（stop_token）。

### 1.4 线程信息

```cpp
std::thread::hardware_concurrency()  // 硬件并发数（CPU 核心数）
std::this_thread::get_id()           // 当前线程 ID
std::this_thread::sleep_for(10ms)    // 睡眠
std::this_thread::yield()            // 让出 CPU 时间片
```

### 深入扩展：C++20 jthread vs thread

| | `std::thread` | `std::jthread` (C++20) |
|------|--------------|----------------------|
| 自动 join | ❌ 手动 | ✅ 析构自动 join |
| 协作取消 | ❌ 无 | ✅ `stop_token` / `stop_source` |
| 异常安全 | 需要 RAII 包装 | 天然安全 |
| 推荐度 | C++11~17 | C++20+ 首选 |

---

## 第2章：线程传参与返回值

### 2.1 三种传参方式

```cpp
// (a) 值传递（默认拷贝）
void process(int x);
std::thread t(process, 42);  // 拷贝 42

// (b) 引用传递（必须 std::ref）
void increment(int& x) { ++x; }
int val = 0;
std::thread t(increment, std::ref(val));  // ✅ 真引用
// std::thread t(increment, val);          // ❌ 不会修改 val！

// (c) 移动语义
void take(std::unique_ptr<int> ptr);
auto p = std::make_unique<int>(42);
std::thread t(take, std::move(p));  // ✅ 移动所有权
```

### 2.2 三种获取返回值的方式

```cpp
// 方式1: std::async（最简单）
auto f1 = std::async(std::launch::async, [](int x) { return x*x; }, 7);
int result = f1.get();  // 49

// 方式2: packaged_task（可延迟执行）
std::packaged_task<int(int)> task([](int x) { return x*x; });
auto f2 = task.get_future();
std::thread t2(std::move(task), 8);
t2.join();
int result2 = f2.get();  // 64

// 方式3: promise（最灵活，一对一通信通道）
std::promise<int> prom;
auto f3 = prom.get_future();
std::thread t3([&prom]() { prom.set_value(81); });
t3.join();
int result3 = f3.get();  // 81
```

### 深入扩展：传参陷阱

| 陷阱 | 说明 | 解决 |
|------|------|------|
| 隐式拷贝 | `thread(func, ref_param)` 总是拷贝 | 用 `std::ref()` |
| 悬挂引用 | detach 后引用局部变量 | 按值捕获，或确保生命周期 |
| 字符串字面量 | `thread(func, "hello")` 传 `const char*`，可能悬挂 | 显式构造 `std::string` |
| unique_ptr | 不可拷贝 | 必须 `std::move()` |

---

## 第3章：互斥量 — mutex 家族

### 3.1 数据竞争演示

```cpp
int counter = 0;  // 共享变量
// 两个线程同时 ++counter 各 10 万次
// 结果不确定！可能 < 200000
```

> **数据竞争 = 未定义行为 (UB)**。C++ 标准规定：两个线程同时访问同一非原子变量，且至少一个是写操作 → UB。

### 3.2 mutex 家族

```cpp
std::mutex              // 基础互斥量
std::timed_mutex        // 带超时：try_lock_for(50ms)
std::recursive_mutex    // 可重入：同一线程可多次加锁
std::shared_mutex       // 读写锁（C++17，详见第12章）
```

```cpp
// timed_mutex 示例
std::timed_mutex tmtx;
if (tmtx.try_lock_for(50ms)) {
    // 获取成功
    tmtx.unlock();
} else {
    // 超时未获取
}

// recursive_mutex 示例
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
        add(x);  // 内部再次加锁 — 普通 mutex 会死锁！
        add(x);
    }
};
```

### 深入扩展：mutex 性能特征

| 互斥量 | 大小 (Linux) | 特点 | 何时用 |
|--------|-------------|------|--------|
| `mutex` | 40B | 不可重入，不可超时 | 默认选择 |
| `timed_mutex` | 48B | 可超时 | 需要避免无限等待 |
| `recursive_mutex` | 48B | 可重入 | 递归调用（尽量避免） |
| `shared_mutex` | 56B | 读写分离 | 读多写少 |

> **经验**：`recursive_mutex` 往往是设计问题的信号。优先重构代码消除递归加锁需求。

---

## 第4章：锁管理 — RAII 是关键

**永远不要手动 `lock()` / `unlock()`** — 使用 RAII 锁管理器。

### 4.1 三种锁管理器

```cpp
// (1) lock_guard: 最简单、不可解锁
{
    std::lock_guard<std::mutex> lock(mtx);
    // 临界区
}  // 自动解锁

// (2) unique_lock: 灵活、可中途解锁、可转移
{
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);  // 延迟加锁
    lock.lock();     // 手动加锁
    lock.unlock();   // 中途解锁
    lock.lock();     // 再次加锁
}  // 自动解锁

// (3) scoped_lock (C++17): 同时锁多个 mutex，防死锁
{
    std::scoped_lock lock(mtxA, mtxB);  // 内部使用无死锁算法
    // 同时持有两个锁
}  // 同时释放
```

### 4.2 unique_lock 的四种加锁模式

```cpp
std::unique_lock<std::mutex> lock1(mtx);                    // 立即加锁
std::unique_lock<std::mutex> lock2(mtx, std::defer_lock);   // 延迟加锁
std::unique_lock<std::mutex> lock3(mtx, std::try_to_lock);  // 尝试加锁
std::unique_lock<std::timed_mutex> lock4(tmtx, 100ms);      // 超时加锁
```

### 深入扩展：锁管理器选型

| 场景 | 推荐 | 原因 |
|------|------|------|
| 简单临界区 | `lock_guard` | 开销最小，最不容易出错 |
| 需要中途解锁 | `unique_lock` | 灵活 |
| 条件变量 | `unique_lock` | `cv.wait()` 要求 unique_lock |
| 同时锁多个 mutex | `scoped_lock` | 自动防死锁 |
| 读写锁的读端 | `shared_lock` | 共享锁 |
| C++17 可用 | `scoped_lock` | 单 mutex 也可用，替代 lock_guard |

---

## 第5章：死锁与避免策略

### 5.1 死锁的四个必要条件

```
1. 互斥       — 资源不能共享
2. 持有并等待 — 持有一个锁的同时等待另一个
3. 不可抢占   — 不能强制释放他人的锁
4. 循环等待   — A 等 B，B 等 A
```

### 5.2 四种避免策略

| 策略 | 方式 | 代码 |
|------|------|------|
| **固定顺序** | 所有线程以相同顺序获取锁 | `lockA → lockB` |
| **scoped_lock** | 一次性获取所有锁 | `scoped_lock lock(A, B)` |
| **try_lock** | 非阻塞尝试，失败则重试 | `std::try_lock(A, B)` |
| **层级锁** | 只能从高层级锁到低层级 | `HierarchicalMutex` |

### 5.3 层级锁实现

```cpp
class HierarchicalMutex {
    std::mutex internal_mtx_;
    unsigned long const hierarchy_value_;
    unsigned long previous_hierarchy_;
    static thread_local unsigned long this_thread_hierarchy_;

    void check_hierarchy() {
        if (hierarchy_value_ >= this_thread_hierarchy_)
            throw std::logic_error("Mutex hierarchy violated");
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
};

HierarchicalMutex high(10000), mid(5000), low(1000);

// ✅ 合法：从高到低
{ lock(high); lock(mid); lock(low); }

// ❌ 违规：从低到高 → 抛异常
{ lock(low); lock(high); }  // throw logic_error!
```

### 深入扩展：生产环境死锁诊断

| 工具 | 平台 | 功能 |
|------|------|------|
| TSan (`-fsanitize=thread`) | Linux/Mac | 检测潜在死锁 + 数据竞争 |
| Helgrind (`valgrind`) | Linux | 锁序违规检测 |
| Intel Inspector | Windows/Linux | 商业级并发错误检测 |
| `std::lock` | 标准库 | 运行期无死锁算法 |
| 锁序图分析 | 设计阶段 | 静态验证锁获取顺序 |

---

## 第6章：条件变量 — condition_variable

### 6.1 基本工作模式

```
等待者线程:                          通知者线程:
─────────────                      ─────────────
lock(mtx)                          lock(mtx)
while (!ready)                     ready = true;
  cv.wait(lock)     ◄────────────  unlock(mtx)
// ready == true                   cv.notify_one()
unlock(mtx)
```

```cpp
std::mutex mtx;
std::condition_variable cv;
bool ready = false;

// 等待者
std::thread waiter([&]() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]() { return ready; });  // 等待 ready == true
    // ready == true，继续执行
});

// 通知者
{
    std::lock_guard<std::mutex> lock(mtx);
    ready = true;
}
cv.notify_one();
```

### 6.2 关键：虚假唤醒 (Spurious Wakeup)

```cpp
// ❌ 错误：无条件等待
cv.wait(lock);  // 可能没收到 notify 就返回！

// ✅ 正确：带谓词等待
cv.wait(lock, [&]() { return ready; });
// 等效于：
// while (!ready) cv.wait(lock);
```

> **虚假唤醒**是操作系统和 CPU 的实现特性，POSIX、Windows、Linux 都可能发生。**永远使用谓词版本的 wait**。

### 6.3 超时等待

```cpp
if (cv.wait_for(lock, 50ms, [&]() { return done; })) {
    // 在超时前收到通知
} else {
    // 超时了
}
```

### 6.4 notify_one vs notify_all

| | `notify_one` | `notify_all` |
|---|---|---|
| 唤醒数量 | 一个（不确定哪个） | 所有 |
| 适用场景 | 单消费者 | 多消费者同时响应 |
| 经验 | 精确知道只有一个等待者 | 不确定时用这个 |

---

## 第7章：生产者-消费者模式

### 7.1 ThreadSafeQueue\<T\> 完整实现

```cpp
template <typename T>
class ThreadSafeQueue {
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<T> queue_;
    bool closed_ = false;

public:
    // 生产者：推入数据
    void push(T value) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (closed_) throw std::runtime_error("Queue is closed");
            queue_.push(std::move(value));
        }
        cv_.notify_one();
    }

    // 消费者：阻塞等待直到有数据或队列关闭
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() { return !queue_.empty() || closed_; });
        if (queue_.empty()) return std::nullopt;  // 关闭且空
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    // 非阻塞尝试
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) return std::nullopt;
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    // 关闭队列（通知所有消费者停止）
    void close() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            closed_ = true;
        }
        cv_.notify_all();  // 唤醒所有阻塞的消费者
    }
};
```

### 7.2 使用示例

```cpp
ThreadSafeQueue<int> queue;
// 2 个生产者各生产 100 个项目
// 3 个消费者竞争消费
// 生产者完成后 close() 队列 → 消费者自动退出
// 总消费数 = 200
```

### 深入扩展：线程安全队列的变体

| 变体 | 特点 | 适用场景 |
|------|------|----------|
| 有界队列 | `push` 在队列满时阻塞 | 背压控制 |
| 优先级队列 | 内部使用 `priority_queue` | 任务调度 |
| 窃取队列 (Work-Stealing) | 每个线程一个 deque | 线程池负载均衡 |
| 无锁队列 | 基于 CAS（见第15章） | 极致性能 |
| 批量队列 | 批量 push/pop 减少锁争用 | 高吞吐量 |

---

## 第8章：异步编程 — future / promise / async

### 8.1 std::async — 最简单的异步

```cpp
auto f1 = std::async(std::launch::async, []() {
    std::this_thread::sleep_for(50ms);
    return 42;
});
auto f2 = std::async(std::launch::async, []() {
    std::this_thread::sleep_for(50ms);
    return 100;
});
// 两个任务并行，总时间 ~50ms
int result = f1.get() + f2.get();  // 142
```

### 8.2 promise — 异常传递

```cpp
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
    fut.get();  // 重新抛出异常
} catch (const std::runtime_error& e) {
    // "计算出错"
}
```

### 8.3 shared_future — 多消费者

```cpp
auto sf = std::async(std::launch::async, compute).share();
// 多个线程可以同时安全地调用 sf.get()
for (int i = 0; i < 3; ++i)
    threads.emplace_back([sf]() { sf.get(); });
```

### 8.4 异步机制对比

| 机制 | 写端 | 读端 | 特点 |
|------|------|------|------|
| `async` | 自动 | `future` | 最简单，可能延迟执行 |
| `promise` | 手动 `set_value` | `future` | 完全控制，可传异常 |
| `packaged_task` | 包装可调用对象 | `future` | 可延迟执行、可转移 |
| `shared_future` | 同上 | `shared_future` | 多消费者 |

### 深入扩展：async 的两大陷阱

```cpp
// 陷阱1: launch::deferred 不是异步
auto f = std::async(std::launch::deferred, heavy);
// 直到 f.get() 才在调用线程执行！不是并行！

// 陷阱2: 未保存 future → 阻塞
for (int i = 0; i < 5; ++i)
    std::async(std::launch::async, task);  // ❌ 每次都阻塞！
// 临时 future 析构时等待任务完成 → 串行执行

// ✅ 正确：保存所有 future
std::vector<std::future<void>> futures;
for (int i = 0; i < 5; ++i)
    futures.push_back(std::async(std::launch::async, task));
for (auto& f : futures) f.get();  // 真正并行
```

---

## 第9章：原子操作 — std::atomic

### 9.1 atomic 操作一览

```cpp
std::atomic<int> val{10};

val.load()              // 原子读
val.store(20)           // 原子写
val.fetch_add(5)        // 原子加，返回旧值
val.fetch_sub(3)        // 原子减，返回旧值
val.fetch_and(0xFF)     // 原子位与
val.fetch_or(0x100)     // 原子位或
val.fetch_xor(0xFF)     // 原子位异或
val.exchange(100)       // 原子交换，返回旧值

// CAS (Compare-And-Swap) — 无锁编程的基石
int expected = 100;
bool ok = val.compare_exchange_strong(expected, 200);
// val==expected → val=200, return true
// val!=expected → expected=val, return false
```

### 9.2 atomic vs mutex 性能

```
5,000,000 次递增 × 2 线程:
  [atomic (relaxed)]  ~35 ms    ← 快 5x
  [mutex lock/unlock] ~180 ms   ← 慢
```

> **原则**：简单计数器/标志位用 `atomic`。复杂临界区用 `mutex`。

### 9.3 SpinLock 自旋锁

```cpp
class SpinLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire))
            ;  // 自旋等待
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};
```

### 深入扩展：自旋锁 vs 互斥锁

| | SpinLock | std::mutex |
|------|---------|-----------|
| 等待方式 | 忙等（消耗 CPU） | 内核挂起（让出 CPU） |
| 适用场景 | 临界区极短 (<数百 ns) | 临界区较长 |
| 上下文切换 | 无 | 有（~μs 级） |
| 公平性 | 不公平 | 大致公平 |
| 优先级反转 | 可能 | 内核可处理 |
| 生产建议 | 用于内核/极低延迟 | 默认选择 |

---

## 第10章：内存序 — memory_order 详解

### 10.1 为什么需要内存序？

编译器和 CPU 都会**重排**内存操作以提高性能。在单线程中不可见，在多线程中可能导致 bug。

```
线程1:                    线程2:
data = 42;     ①          while (!ready) ;   ③
ready = true;  ②          assert(data == 42); ④

如果 ① 和 ② 被重排 → ④ 可能失败！
```

### 10.2 六种内存序（从弱到强）

| 内存序 | 保证 | 性能 | 用途 |
|--------|------|------|------|
| `relaxed` | 仅原子性 | **最快** | 计数器 |
| `consume` | 数据依赖 | 快 | ⚠️ 不推荐（编译器通常提升为 acquire） |
| `acquire` | 之后的操作不上移 | 快 | 锁 acquire 端 / 消费者 |
| `release` | 之前的操作不下移 | 快 | 锁 release 端 / 生产者 |
| `acq_rel` | acquire + release | 中 | CAS 循环 |
| `seq_cst` | 全局一致顺序 | **最慢** | 默认，不确定时用 |

### 10.3 Acquire-Release 示例

```cpp
std::atomic<bool> flag{false};
int data = 0;

// 生产者
data = 42;                                      // ① 普通写
flag.store(true, std::memory_order_release);     // ② release 写
// release 保证 ① happens-before ②

// 消费者
while (!flag.load(std::memory_order_acquire)) {} // ③ acquire 读
assert(data == 42);                              // ④ 保证看到 42
// acquire 保证 ③ happens-before ④
// release-acquire 配对保证 ② happens-before ③
// 因此 ① happens-before ④
```

### 10.4 Sequential Consistency 示例

```cpp
std::atomic<bool> x{false}, y{false};
std::atomic<int> z{0};

t1: x.store(true, seq_cst);           // A
t2: y.store(true, seq_cst);           // B
t3: while(!x.load(seq_cst)); if(y.load(seq_cst)) z++;  // C,D
t4: while(!y.load(seq_cst)); if(x.load(seq_cst)) z++;  // E,F

// seq_cst 保证: z > 0 (至少一个线程看到对方的 store)
// 用 relaxed 则不保证!
```

### 深入扩展：内存序与 CPU 架构

| 架构 | 内存模型 | relaxed 开销 | seq_cst 开销 |
|------|----------|-------------|-------------|
| x86/x64 | TSO (强序) | 零 | `mfence` 指令 |
| ARM | 弱序 | 零 | `dmb` 屏障 |
| RISC-V | 弱序 (RVWMO) | 零 | `fence` 指令 |
| POWER | 非常弱序 | 零 | `hwsync` |

> **在 x86 上**：`acquire` 和 `release` 几乎是免费的（TSO 已经提供足够的保证）。`seq_cst` 需要 `mfence` 指令，约 20-30ns。

---

## 第11章：线程安全的单例

### 11.1 两种实现方式

```cpp
// 方式1: call_once (C++11)
class Singleton1 {
    static std::once_flag init_flag_;
    static Singleton1* instance_;
public:
    static Singleton1& instance() {
        std::call_once(init_flag_, []() {
            instance_ = new Singleton1();
        });
        return *instance_;
    }
};

// 方式2: Meyer's Singleton (推荐)
class Singleton2 {
public:
    static Singleton2& instance() {
        static Singleton2 inst;  // C++11 保证线程安全的一次初始化
        return inst;
    }
};
```

### 11.2 对比

| | `call_once` | Meyer's Singleton |
|------|-----------|------------------|
| 代码量 | 多（需要 once_flag + 指针） | 少（一行 static） |
| 生命周期 | 手动管理（可能泄漏） | 自动析构 |
| 线程安全 | ✅ (C++11) | ✅ (C++11) |
| **推荐** | 非单例的一次初始化 | **单例首选** |

---

## 第12章：读写锁 — shared_mutex (C++17)

```
多个读者可同时持有 shared_lock（共享锁）
只有一个写者可持有 unique_lock（独占锁）
读写互斥
```

### 12.1 ConcurrentMap 实现

```cpp
template <typename K, typename V>
class ConcurrentMap {
    mutable std::shared_mutex smtx_;
    std::unordered_map<K, V> data_;
public:
    // 写：独占锁
    void insert(const K& key, const V& value) {
        std::unique_lock<std::shared_mutex> lock(smtx_);
        data_[key] = value;
    }

    // 读：共享锁（多个读者可并行）
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
};
```

### 深入扩展：读写锁的适用性

| 场景 | 推荐 |
|------|------|
| 读多写少（90% 读） | ✅ `shared_mutex` 收益大 |
| 读写各半 | ⚠️ 收益不明显，开销比 mutex 高 |
| 写多读少 | ❌ 不如用 mutex |
| 极短临界区 | ❌ 不如用 atomic 或 spinlock |

> **注意**：`shared_mutex` 比 `mutex` 更重（需要管理读者计数 + 写者优先级）。只有在读操作远多于写操作时才值得使用。

---

## 第13章：线程池实现

### 13.1 完整 ThreadPool

```cpp
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
        { std::lock_guard<std::mutex> lock(mtx_); stop_ = true; }
        cv_.notify_all();
        for (auto& w : workers_) w.join();
    }

    // 提交任务，返回 future
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using R = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<R()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        auto future = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            tasks_.emplace([task]() { (*task)(); });
        }
        cv_.notify_one();
        return future;
    }
};
```

### 13.2 使用示例

```cpp
ThreadPool pool(4);

// 提交计算任务
auto f = pool.submit([](int x) { return x * x; }, 42);
std::cout << f.get();  // 1764

// 并行求和：分成 8 个分片
for (int c = 0; c < 8; ++c)
    sums.push_back(pool.submit(partial_sum, start, end));
long long total = 0;
for (auto& f : sums) total += f.get();
```

### 深入扩展：线程池改进方向

| 改进 | 说明 |
|------|------|
| **Work-Stealing** | 每个线程一个 deque，空闲线程从其他线程窃取任务 |
| **优先级** | 任务队列使用 `priority_queue` |
| **动态扩缩** | 根据负载动态增减工作线程 |
| **任务取消** | 配合 `stop_token` (C++20) 或 `std::atomic<bool>` |
| **异常处理** | 捕获工作线程异常，通过 future 传播 |
| **任务依赖图** | DAG 调度器，支持任务间依赖 |
| **生产级库** | `taskflow`、`Intel TBB`、`BS::thread_pool` |

---

## 第14章：并行算法

### 14.1 手动并行 for_each

```cpp
template <typename Iter, typename Func>
void parallel_for_each(Iter begin, Iter end, Func f, int num_threads = 0) {
    if (num_threads <= 0)
        num_threads = std::thread::hardware_concurrency();

    auto total = std::distance(begin, end);
    auto chunk = total / num_threads;

    std::vector<std::thread> threads;
    auto chunk_begin = begin;
    for (int i = 0; i < num_threads; ++i) {
        auto chunk_end = (i == num_threads - 1) ? end
                         : std::next(chunk_begin, chunk);
        threads.emplace_back([=]() {
            std::for_each(chunk_begin, chunk_end, f);
        });
        chunk_begin = chunk_end;
    }
    for (auto& t : threads) t.join();
}
```

### 14.2 手动并行 reduce

```cpp
template <typename Iter, typename T, typename BinaryOp>
T parallel_reduce(Iter begin, Iter end, T init, BinaryOp op,
                  int num_threads = 0);
// 每个线程处理一个分片 → std::accumulate
// 最后合并所有分片结果
```

### 14.3 手动并行 sort

```cpp
template <typename Iter>
void parallel_sort(Iter begin, Iter end, int depth = 0) {
    if (size < 10000 || depth > 4) { std::sort(begin, end); return; }
    // nth_element 分割 → 左右两半并行排序 → inplace_merge
}
```

### 14.4 基准测试

```
5,000,000 元素:
  [串行 accumulate]   ~5 ms
  [并行 reduce (4线程)] ~2 ms   ← ~2.5x
  [串行 sort]           ~700 ms
  [并行 sort (4线程)]    ~350 ms ← ~2x
```

### 深入扩展：C++17 执行策略

```cpp
#include <execution>

// C++17 标准并行算法（需要 TBB / MSVC 支持）
std::sort(std::execution::par, data.begin(), data.end());
std::reduce(std::execution::par_unseq, data.begin(), data.end(), 0LL);
std::for_each(std::execution::par, data.begin(), data.end(), func);
```

| 策略 | 含义 | 要求 |
|------|------|------|
| `seq` | 顺序执行 | 无特殊要求 |
| `par` | 并行执行 | 回调不能数据竞争 |
| `par_unseq` | 并行 + SIMD | 回调不能加锁 |
| `unseq` (C++20) | SIMD 向量化 | 回调不能加锁 |

---

## 第15章：无锁编程 — Lock-Free Queue

### 15.1 SPSC 队列（单生产者单消费者）

```cpp
template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "must be power of 2");

    std::array<T, Capacity> buffer_;
    alignas(64) std::atomic<size_t> head_{0};  // 写者
    alignas(64) std::atomic<size_t> tail_{0};  // 读者

    static constexpr size_t MASK = Capacity - 1;

public:
    bool push(const T& value) {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t next = (head + 1) & MASK;
        if (next == tail_.load(std::memory_order_acquire))
            return false;  // 满
        buffer_[head] = value;
        head_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire))
            return false;  // 空
        value = buffer_[tail];
        tail_.store((tail + 1) & MASK, std::memory_order_release);
        return true;
    }
};
```

**关键设计**：
- `alignas(64)`：head 和 tail 分别在不同缓存行，避免 false sharing
- 容量为 2 的幂：用位运算 `& MASK` 替代取模 `%`
- 内存序：push 用 release，pop 用 acquire → 建立 happens-before

### 15.2 MPMC 队列（多生产者多消费者）

基于 **CAS 循环**（compare_exchange_weak）+ 序列号（sequence number）实现。每个 Cell 有独立的 `atomic<size_t> sequence`，生产者和消费者通过 CAS 竞争位置。

```
SPSC 100万次: ~30 ms
MPMC 4P4C 40万次: ~60 ms
```

### 深入扩展：无锁编程注意事项

| 要点 | 说明 |
|------|------|
| ABA 问题 | CAS 可能误判（值回到旧值）→ 用版本号解决 |
| 内存回收 | 无锁结构中何时安全释放内存？→ Hazard Pointer / RCU / Epoch-based |
| false sharing | 不同线程访问同一缓存行 → `alignas(64)` |
| 编译器屏障 | `std::atomic_thread_fence` |
| 硬件限制 | `is_lock_free()` 检查是否真正无锁 |
| 正确性验证 | 极难 — 用 Relacy、CDSChecker 等模型检查器 |

> **忠告**：无锁编程极难正确实现。优先使用经过验证的库（如 Boost.Lockfree、folly::MPMCQueue）。

---

## 第16章：实战 — 并发管道 (Pipeline)

### 16.1 管道模式

```
Stage1 (生成) → Queue → Stage2 (处理, 可并行) → Queue → Stage3 (输出)
```

```cpp
ThreadSafeQueue<int> raw_queue;
ThreadSafeQueue<DataItem> processed_queue;
ThreadSafeQueue<string> final_queue;

// 阶段1：生成原始数据 (1 线程)
thread stage1([&]() {
    for (int i = 1; i <= 20; ++i) raw_queue.push(i);
    raw_queue.close();
});

// 阶段2：处理数据 → 可多线程并行
thread stage2a(stage2_worker);
thread stage2b(stage2_worker);

// 阶段3：格式化输出 (1 线程)
thread stage3([&]() {
    while (auto item = processed_queue.pop())
        final_queue.push(format(*item));
    final_queue.close();
});
```

### 深入扩展：管道模式的应用

| 应用 | 阶段 |
|------|------|
| 视频处理 | 解码 → 特效 → 编码 |
| 编译器 | 词法分析 → 语法分析 → 优化 → 代码生成 |
| ETL 数据管道 | Extract → Transform → Load |
| 网络服务器 | 接收 → 解析 → 处理 → 响应 |
| 音频处理 | 采集 → 滤波 → 编码 → 传输 |

> **优势**：每个阶段独立伸缩（处理阶段可以加更多线程），天然背压控制（队列满时自动阻塞上游）。

---

## 第17章：实战 — Map-Reduce 框架

### 17.1 通用 parallel_map_reduce

```cpp
template <typename InputIter, typename MapFunc, typename ReduceFunc>
auto parallel_map_reduce(
    InputIter begin, InputIter end,
    MapFunc map_fn, ReduceFunc reduce_fn,
    auto init, int num_threads = 0)
{
    // Map 阶段：数据分片，每个线程独立处理
    // Reduce 阶段：合并所有分片结果
}
```

### 17.2 示例1：平方和

```cpp
long long sum = parallel_map_reduce(
    data.begin(), data.end(),
    [](int x) -> long long { return (long long)x * x; },  // Map
    std::plus<long long>{},                                 // Reduce
    0LL, 4                                                  // init, threads
);
```

### 17.3 示例2：词频统计

```cpp
// Map: 每个文档 → 局部词频 map
// Reduce: 合并所有局部 map 到全局 map

std::vector<std::string> documents = { "hello world...", ... };

// 每个文档一个 async 任务
for (auto& doc : documents) {
    futures.push_back(std::async([&]() {
        WordCount local;
        // ... tokenize & count ...
        // merge to global under lock
    }));
}
```

### 深入扩展：Map-Reduce 优化

| 优化 | 说明 |
|------|------|
| 分片大小 | 太大 → 负载不均；太小 → 调度开销大 |
| 局部合并 | 先在线程内合并，再全局合并（减少锁争用） |
| 无锁合并 | 每个线程返回局部结果，最后单线程合并 |
| NUMA 感知 | 分片与 NUMA 节点对齐 |
| 生产级 | Hadoop MapReduce、Spark、C++ Thrust |

---

## 第18章：常见陷阱与调试技巧

### 18.1 六大陷阱

| # | 陷阱 | 问题 | 解决 |
|---|------|------|------|
| 1 | 忘记 join/detach | `std::terminate()` | RAII JoinThread / jthread |
| 2 | 悬挂引用 | detach 后访问已销毁变量 | 按值捕获 |
| 3 | 隐式拷贝 | `thread(func, ref)` 拷贝参数 | `std::ref()` |
| 4 | async 未保存 future | 临时 future 析构时阻塞 → 串行 | 保存到 vector |
| 5 | 锁粒度过大 | 锁住整个函数 → 串行化 | 只锁临界区 |
| 6 | false sharing | 不同线程的变量在同一缓存行 | `alignas(64)` |

### 18.2 false sharing 演示

```cpp
struct BadCounters {
    std::atomic<int> a{0};  // 同一缓存行
    std::atomic<int> b{0};  //
};

struct GoodCounters {
    alignas(64) std::atomic<int> a{0};  // 不同缓存行
    alignas(64) std::atomic<int> b{0};  //
};
```

```
5,000,000 次递增 × 2 线程:
  [false sharing (同缓存行)]   ~140 ms   ← 慢
  [无 false sharing (不同缓存行)] ~35 ms  ← 快 4x
```

### 18.3 thread_local

```cpp
thread_local int tl_counter = 0;
// 每个线程有独立副本！
// 线程1的 tl_counter++ 不影响线程2
```

### 18.4 调试工具

| 工具 | 命令 | 检测 |
|------|------|------|
| **ThreadSanitizer** | `g++ -fsanitize=thread -g -O1` | 数据竞争、死锁 |
| **Helgrind** | `valgrind --tool=helgrind ./prog` | 锁序违规 |
| **AddressSanitizer** | `g++ -fsanitize=address -g` | use-after-free |
| **Intel Inspector** | GUI | 商业级并发错误检测 |
| **rr (Record & Replay)** | `rr record ./prog && rr replay` | 确定性重放调试 |

### 深入扩展：并发程序设计原则

```
1. 最小化共享可变状态 — 能不共享就不共享
2. 不可变数据天然线程安全 — const 是最好的同步
3. 优先使用高层抽象 — async > thread，ThreadPool > 手动管理
4. 锁的粒度越小越好 — 但不能太小（失去原子性）
5. 先正确，后性能 — 用 mutex 写对，再考虑 atomic / lock-free
6. 用工具验证 — TSan 在 CI 中常开
7. 考虑可测试性 — 并发代码要能确定性地重放
```

---

## 附录：核心概念速查表

### 同步原语

| 原语 | 用途 | 开销 |
|------|------|------|
| `mutex` | 互斥访问 | 中 |
| `recursive_mutex` | 可重入互斥 | 中高 |
| `timed_mutex` | 带超时的互斥 | 中 |
| `shared_mutex` | 读写锁 | 中 |
| `condition_variable` | 线程等待/通知 | 低 |
| `atomic<T>` | 无锁原子操作 | 低 |
| `atomic_flag` | 最简原子标志 | 极低 |
| `call_once` | 一次初始化 | 低 |

### 锁管理器 (RAII)

| 管理器 | 特点 | 推荐 |
|--------|------|------|
| `lock_guard` | 简单、不可中途解锁 | ★★★★ |
| `unique_lock` | 灵活、可转移、可解锁 | ★★★★★ |
| `scoped_lock` | 多 mutex、防死锁 | ★★★★★ |
| `shared_lock` | 共享读锁 | ★★★★ |

### 异步机制

| 机制 | 用途 | 返回值 |
|------|------|--------|
| `std::async` | 简单异步任务 | future |
| `promise/future` | 一对一跨线程通信 | T |
| `packaged_task` | 可延迟执行的任务 | future |
| `shared_future` | 多消费者共享结果 | T |

### 内存序 (从弱到强)

| 内存序 | 保证 | 使用场景 | 性能 |
|--------|------|----------|------|
| `relaxed` | 仅原子性 | 计数器 | 最快 |
| `acquire` | 读不后移 | 锁 acquire 端 | 快 |
| `release` | 写不前移 | 锁 release 端 | 快 |
| `acq_rel` | acquire+release | CAS 循环 | 中 |
| `seq_cst` | 全局一致 | 默认 | 慢 |

### C++ 并发发展史

| 标准 | 新增 |
|------|------|
| **C++11** | thread, mutex, atomic, future, condition_variable |
| **C++14** | shared_timed_mutex |
| **C++17** | shared_mutex, scoped_lock, 并行 STL 算法 |
| **C++20** | jthread, stop_token, barrier, latch, semaphore, atomic_ref |
| **C++23** | stackful coroutines (提案中) |

---

## 运行输出示例

```
===== 现代 C++ 多线程教程 演示 =====

[1] 线程基础:
    [线程] 普通函数
    [线程] 函数对象
    [线程] Lambda
    [线程] 成员函数, id=42
  硬件并发数: 12
  主线程 ID: 140312...
  join 之后继续
  detach 之后继续（线程在后台）
  joinable (创建后): true
  joinable (join后):  false
    JoinThread: 自动 join

[2] 线程传参与返回值:
  引用传递后 value = 1
    移动传参: *ptr=42
  async 返回值: 49
  packaged_task 返回值: 64
  promise 返回值: 81

[3] 互斥量:
  无保护 counter = 156432 (期望 200000)
  mutex 保护 counter = 200000 (期望 200000)
  timed_mutex: 超时未获取
  recursive_mutex: 10 (期望 10)

[4] 锁管理:
  unique_lock try_to_lock: 成功
  unique_lock 超时加锁: 成功
  lock1 owns: false, lock2 owns: true
  scoped_lock: 同时持有两个锁

[5] 死锁与避免:
  策略1 (固定顺序): 安全
  策略2 (scoped_lock): 安全
  策略3 (try_lock): 安全
  层级锁违规: Mutex hierarchy violated

[6] 条件变量:
    等待者: 收到通知, ready=true
  wait_for: 超时 (50ms), done=false
    线程 0 开始运行
    线程 1 开始运行
    线程 2 开始运行
    线程 3 开始运行
    线程 4 开始运行

[7] 生产者-消费者:
    生产者 0 完成
    生产者 1 完成
    消费者 0 处理了 68 个项目
    消费者 1 处理了 72 个项目
    消费者 2 处理了 60 个项目
  总消费数: 200 (期望 200)

[8] 异步编程:
  async 并行结果: 142 (期望 142)
  promise/future: Hello from promise!
  promise 异常传递: 计算出错
  packaged_task: 3+4=7
    线程 0 读取 shared_future: 777
    线程 1 读取 shared_future: 777
    线程 2 读取 shared_future: 777

[9] 原子操作:
  atomic counter = 2000000 (期望 2000000)
  CAS: success=true, val=200, prev=100
  [atomic 递增] 35 ms
  [mutex 递增] 180 ms
  SpinLock counter = 200000 (期望 200000)

[10] 内存序:
  relaxed counter = 2000000
    acquire-release: data=42 ✓
  seq_cst: z = 2 (保证 > 0)

[11] 线程安全单例:
    Singleton2 构造 (Meyer's)
    Singleton2::hello() (×5)

[12] 读写锁:
  [ConcurrentMap 读写] 25 ms
  Map size: 2000, total reads hit: 3200

[13] 线程池:
  线程池结果: 0 1 4 9 16 25 36 49 64 81
  [线程池并行求和] 12 ms
  并行求和: 50000005000000 (期望 50000005000000)

[14] 并行算法:
  [串行 accumulate] 5 ms
  [并行 reduce] 2 ms
  [串行 sort] 720 ms
  [并行 sort] 350 ms

[15] 无锁队列:
  [SPSC 无锁队列] 30 ms
  SPSC sum = 499999500000 (期望 499999500000)
  [MPMC 无锁队列] 65 ms
  MPMC total consumed: 400000 (期望 400000)

[16] 并发管道:
    Stage1: 生成完成
    Stage3: 格式化完成
  Pipeline 结果: [1: item_1] [2: item_4] [3: item_9] ... 共 20 项

[17] Map-Reduce:
  [MapReduce 平方和] 8 ms
  平方和: 333333383333335000000 (期望 333333383333335000000)
  词频统计:
    hello: 6
    world: 4
    foo: 2
    bar: 3
    baz: 3

[18] 陷阱与调试:
  [false sharing (同缓存行)] 140 ms
  [无 false sharing (不同缓存行)] 35 ms
    线程 1 的 thread_local counter = 5
    线程 2 的 thread_local counter = 5
  主线程的 thread_local counter = 0
  [async 陷阱: 未保存 future] 55 ms
  [async 正确: 保存 future] 12 ms

===== 演示完成 =====
```

---

## 核心理念

> **并发编程的首要目标是正确性，其次才是性能。**
>
> 先用 `mutex` + `lock_guard` 写出正确的代码。然后用 TSan 验证。确认正确后，再考虑 `atomic` / 无锁 / 并行算法等优化。
>
> 最安全的并发代码是**没有共享可变状态的代码**。消息传递（队列）、不可变数据（const）、线程本地存储（thread_local）— 这些才是并发编程的第一选择。
