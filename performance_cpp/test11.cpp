// =============================================================================
// 熟练掌握无锁编程、内存顺序、原子操作及并发数据结构 — 完全教程
// =============================================================================
//
// 编译：g++ -std=c++17 -O2 -pthread -o test11 test11.cpp
//       cl /std:c++17 /O2 /EHsc test11.cpp
//
// 目录：
// ═══════════════════════════════════════════════════════════════
// 一、原子操作基础篇
//   1.  std::atomic 基础：load/store/exchange/CAS
//   2.  原子类型完全手册：整型、指针、浮点、atomic_ref
//   3.  fetch_add / fetch_or 等 RMW 操作详解
//   4.  CAS 循环模式与 ABA 问题
//
// 二、内存顺序篇
//   5.  CPU 指令重排与编译器重排
//   6.  六种 memory_order 逐一精讲
//   7.  Acquire-Release 语义：生产者-消费者详解
//   8.  Release Sequence 与传递性
//   9.  memory_order_consume 与数据依赖
//  10.  volatile vs atomic：本质区别
//  11.  内存屏障 (fence) 独立使用
//
// 三、无锁算法篇
//  12.  自旋锁三种实现：TAS / TTAS / Ticket Lock
//  13.  无锁栈 (Treiber Stack)
//  14.  无锁队列 (Michael-Scott Queue)
//  15.  无锁哈希表 (分桶 + 原子链表)
//  16.  无锁环形缓冲区 (SPSC / MPMC)
//
// 四、内存回收篇
//  17.  Hazard Pointer 详解与实现
//  18.  Epoch-Based Reclamation (EBR)
//  19.  引用计数回收 (split reference count)
//
// 五、高级并发数据结构篇
//  20.  SeqLock（序列锁）完整实现
//  21.  Read-Copy-Update (RCU) 用户态模拟
//  22.  并发跳表 (Concurrent Skip List)
//
// 六、实战与调试篇
//  23.  常见 Bug 模式与修复
//  24.  性能基准测试：锁 vs 无锁 vs 分片
//  25.  工具链：TSan / ASAN / Relacy / CDSChecker
// ═══════════════════════════════════════════════════════════════

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <deque>
#include <algorithm>
#include <numeric>
#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <any>
#include <bitset>

#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <random>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <new> // std::hardware_destructive_interference_size

using namespace std::chrono_literals;

// ─── 工具函数 ────────────────────────────────────────────────

static void print_header(const char* title) {
    std::cout << "\n╔══════════════════════════════════════════════════╗\n";
    std::cout << "║ " << std::left << std::setw(48) << title << " ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n\n";
}

static void print_section(const char* title) {
    std::cout << "  ── " << title << " ──\n";
}

class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point start_ = clk::now();
public:
    double elapsed_ms() const {
        return std::chrono::duration<double, std::milli>(clk::now() - start_).count();
    }
    void reset() { start_ = clk::now(); }
};

// 缓存行大小 (避免 false sharing)
#ifdef __cpp_lib_hardware_interference_size
    constexpr size_t CACHE_LINE = std::hardware_destructive_interference_size;
#else
    constexpr size_t CACHE_LINE = 64;
#endif


// =============================================================================
// ██ 一、原子操作基础篇
// =============================================================================

// =============================================================================
// 第1章：std::atomic 基础
// =============================================================================
//
// std::atomic<T> 保证对 T 的操作是原子的（不可分割的），
// 且可以指定内存顺序来控制操作的可见性。
//
// 核心操作：
//   load()     — 原子读取
//   store()    — 原子写入
//   exchange() — 原子交换（返回旧值）
//   compare_exchange_weak/strong() — CAS
//
// lock_free 保证：
//   is_lock_free()  — 是否是真正的硬件原子（不使用内部锁）
//   is_always_lock_free — 编译期常量
// =============================================================================

namespace ch1 {

void demo_atomic_basics() {
    print_section("std::atomic 基础操作");

    // --- 1. load / store ---
    std::atomic<int> x(0);
    x.store(42, std::memory_order_relaxed);
    int val = x.load(std::memory_order_relaxed);
    std::cout << "  store(42), load() = " << val << "\n";

    // 默认 memory_order = seq_cst
    x = 100;      // 等价于 x.store(100, memory_order_seq_cst)
    val = x;       // 等价于 x.load(memory_order_seq_cst)
    std::cout << "  x = 100, 读取 = " << val << "\n";

    // --- 2. exchange ---
    // 将新值写入，返回旧值。常用于 spinlock。
    int old = x.exchange(200);
    std::cout << "  exchange(200), 旧值 = " << old << ", 新值 = " << x.load() << "\n";

    // --- 3. compare_exchange_strong (CAS) ---
    // if (x == expected) { x = desired; return true; }
    // else { expected = x; return false; }
    int expected = 200;
    bool ok = x.compare_exchange_strong(expected, 300);
    std::cout << "  CAS(200→300) " << (ok ? "成功" : "失败")
              << ", x=" << x.load() << "\n";

    expected = 999; // 故意给错
    ok = x.compare_exchange_strong(expected, 400);
    std::cout << "  CAS(999→400) " << (ok ? "成功" : "失败")
              << ", expected 被更新为 " << expected << "\n";

    // --- 4. compare_exchange_weak ---
    // 可能假失败 (spurious failure)，但在循环中更高效（某些架构）
    // 典型用法：
    int cur = x.load();
    while (!x.compare_exchange_weak(cur, cur + 1)) {
        // cur 被自动更新为最新值，继续尝试
    }
    std::cout << "  weak CAS 循环 +1: x=" << x.load() << "\n";

    // --- 5. lock_free 检查 ---
    std::cout << "\n  lock_free 检查:\n";
    std::cout << "    atomic<int>:      " << std::atomic<int>{}.is_lock_free() << "\n";
    std::cout << "    atomic<long long>:" << std::atomic<long long>{}.is_lock_free() << "\n";
    std::cout << "    atomic<int*>:     " << std::atomic<int*>{}.is_lock_free() << "\n";

    // 128-bit 结构体（可能不是 lock-free）
    struct Big { long a, b; };
    std::atomic<Big> big;
    std::cout << "    atomic<{long,long}>: " << big.is_lock_free()
              << " (16 bytes, 平台相关)\n";

    // --- 6. 多线程并发递增 (正确性验证) ---
    std::atomic<int> counter(0);
    constexpr int N = 100000;
    constexpr int T = 4;

    std::vector<std::thread> threads;
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&counter] {
            for (int i = 0; i < N; ++i) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    for (auto& t : threads) t.join();

    std::cout << "\n  " << T << " 线程各 +" << N << " 次, 结果 = "
              << counter.load() << " (期望 " << T * N << ")\n";
}

} // namespace ch1


// =============================================================================
// 第2章：原子类型完全手册
// =============================================================================

namespace ch2 {

void demo_atomic_types() {
    print_section("原子类型完全手册");

    std::cout << R"(
  ┌──────────────────────────────────┬────────────┬───────────┐
  │ 类型                            │ 支持操作    │ lock-free │
  ├──────────────────────────────────┼────────────┼───────────┤
  │ atomic<bool>                    │ load/store/ │ ✅ 通常   │
  │  = atomic_bool                  │ exchange/CAS│           │
  ├──────────────────────────────────┼────────────┼───────────┤
  │ atomic<int/long/...>            │ + fetch_add │ ✅ 通常   │
  │  = atomic_int, atomic_long...   │ fetch_sub   │           │
  │                                  │ fetch_and/or│           │
  │                                  │ fetch_xor   │           │
  ├──────────────────────────────────┼────────────┼───────────┤
  │ atomic<T*>                      │ + fetch_add │ ✅ 通常   │
  │                                  │ fetch_sub   │           │
  │                                  │ (指针算术)  │           │
  ├──────────────────────────────────┼────────────┼───────────┤
  │ atomic<float/double> (C++20)    │ + fetch_add │ 平台相关  │
  │                                  │ fetch_sub   │           │
  ├──────────────────────────────────┼────────────┼───────────┤
  │ atomic<用户定义类型>             │ load/store/ │ 取决于    │
  │ (要求 trivially copyable)        │ exchange/CAS│ sizeof(T) │
  ├──────────────────────────────────┼────────────┼───────────┤
  │ atomic_flag                     │ test_and_set│ ✅ 保证   │
  │ (唯一保证 lock-free 的类型)     │ clear       │           │
  ├──────────────────────────────────┼────────────┼───────────┤
  │ atomic_ref<T> (C++20)           │ 同上       │ 同上      │
  │ (对已有变量的原子访问视图)       │            │           │
  └──────────────────────────────────┴────────────┴───────────┘
)";

    // atomic_flag — 最基础的原子类型
    std::atomic_flag flag = ATOMIC_FLAG_INIT; // 初始为 clear
    bool was_set = flag.test_and_set(); // 设置并返回旧值
    std::cout << "  atomic_flag: test_and_set() = " << was_set << " (旧值)\n";
    was_set = flag.test_and_set();
    std::cout << "  atomic_flag: test_and_set() = " << was_set << " (已被设置)\n";
    flag.clear();
    std::cout << "  atomic_flag: clear() 后 test_and_set() = "
              << flag.test_and_set() << "\n";

    // 指针原子操作
    int arr[] = {10, 20, 30, 40, 50};
    std::atomic<int*> ptr(arr);
    int* old_ptr = ptr.fetch_add(2); // 指针前移 2 个元素
    std::cout << "\n  atomic<int*>: *old=" << *old_ptr
              << ", *new=" << *ptr.load() << "\n";

    // 用户定义类型
    struct Point { float x, y; };
    static_assert(std::is_trivially_copyable_v<Point>);
    std::atomic<Point> ap({1.0f, 2.0f});
    Point p = ap.load();
    std::cout << "  atomic<Point>: {" << p.x << ", " << p.y << "}\n";
    std::cout << "    is_lock_free = " << ap.is_lock_free() << "\n";
}

} // namespace ch2


// =============================================================================
// 第3章：RMW (Read-Modify-Write) 操作详解
// =============================================================================

namespace ch3 {

void demo_rmw() {
    print_section("RMW 操作——fetch_add / fetch_or / ...");

    // fetch_add / fetch_sub — 原子加减
    std::atomic<int> x(10);
    int old = x.fetch_add(5);  // x: 10→15, 返回10
    std::cout << "  fetch_add(5): old=" << old << ", new=" << x.load() << "\n";

    old = x.fetch_sub(3);      // x: 15→12, 返回15
    std::cout << "  fetch_sub(3): old=" << old << ", new=" << x.load() << "\n";

    // operator++ / -- 也是原子的
    x++;                        // 等价于 fetch_add(1)
    ++x;                        // 等价于 fetch_add(1)
    std::cout << "  x++ 两次: " << x.load() << "\n";

    // fetch_and / fetch_or / fetch_xor — 位操作
    std::atomic<unsigned> bits(0xFF00);
    unsigned old_bits = bits.fetch_and(0x0FF0); // AND: 保留共同位
    std::cout << "\n  fetch_and: 0x" << std::hex << old_bits
              << " & 0x0FF0 = 0x" << bits.load() << std::dec << "\n";

    bits.store(0xFF00);
    bits.fetch_or(0x00FF);     // OR: 设置位
    std::cout << "  fetch_or:  0x" << std::hex << bits.load() << std::dec << "\n";

    bits.fetch_xor(0xFFFF);    // XOR: 翻转位
    std::cout << "  fetch_xor: 0x" << std::hex << bits.load() << std::dec << "\n";

    // 实际应用：原子位标志
    std::cout << "\n  实际应用——原子位标志:\n";
    std::atomic<uint32_t> flags(0);
    constexpr uint32_t FLAG_INIT   = 1 << 0;
    constexpr uint32_t FLAG_READY  = 1 << 1;
    constexpr uint32_t FLAG_ERROR  = 1 << 2;

    flags.fetch_or(FLAG_INIT);                        // 设置 INIT
    flags.fetch_or(FLAG_READY);                       // 设置 READY
    std::cout << "    设置 INIT|READY: 0b"
              << std::bitset<8>(flags.load()) << "\n";

    bool was_error = flags.fetch_or(FLAG_ERROR) & FLAG_ERROR; // 检查并设置
    std::cout << "    设置 ERROR (之前有? " << was_error << "): 0b"
              << std::bitset<8>(flags.load()) << "\n";

    flags.fetch_and(~FLAG_ERROR);                     // 清除 ERROR
    std::cout << "    清除 ERROR: 0b" << std::bitset<8>(flags.load()) << "\n";

    // RMW 与 memory_order
    std::cout << R"(
  RMW 操作的 memory_order:
    fetch_add(val, order) — order 同时作用于读和写
    CAS 有两个 order: 成功时用 success, 失败时用 failure
      compare_exchange_weak(expected, desired,
                            memory_order_acq_rel,  // 成功
                            memory_order_acquire);  // 失败

  RMW 操作 always read the latest value (参与修改顺序 modification order)
  即使使用 relaxed，也保证对同一原子变量的 RMW 操作是序列化的。
)";
}

} // namespace ch3


// =============================================================================
// 第4章：CAS 循环与 ABA 问题
// =============================================================================

namespace ch4 {

// --- CAS 循环模式 ---
// 原子地将 x 变为 f(x)
template<typename T, typename Func>
T atomic_update(std::atomic<T>& a, Func f) {
    T old_val = a.load(std::memory_order_relaxed);
    T new_val;
    do {
        new_val = f(old_val);
    } while (!a.compare_exchange_weak(old_val, new_val,
                std::memory_order_release,
                std::memory_order_relaxed));
    // 失败时 old_val 被自动更新为当前值
    return old_val;
}

void demo_cas_loop() {
    print_section("CAS 循环模式");

    std::atomic<int> x(10);

    // 原子乘以2
    int old = atomic_update(x, [](int v) { return v * 2; });
    std::cout << "  atomic x*2: old=" << old << ", new=" << x.load() << "\n";

    // 原子 max
    std::atomic<int> max_val(50);
    auto atomic_max = [&max_val](int new_val) {
        int cur = max_val.load(std::memory_order_relaxed);
        while (new_val > cur) {
            if (max_val.compare_exchange_weak(cur, new_val,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                break;
            }
            // cur 已被更新为最新值
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&atomic_max, i] {
            atomic_max(i * 100);
        });
    }
    for (auto& t : threads) t.join();
    std::cout << "  atomic_max of {0,100,...,700} = " << max_val.load() << "\n";
}

void demo_aba_problem() {
    print_section("ABA 问题");

    std::cout << R"(
  ABA 问题：CAS 只比较值，无法检测值被改变又改回的情况。

  场景（无锁栈 pop）：
    初始栈: A → B → C

    线程1: 读到 top=A, next=B, 准备 CAS(top, A→B)
    线程1: [被调度出去]

    线程2: pop A  → 栈变成 B → C
    线程2: pop B  → 栈变成 C
    线程2: push A → 栈变成 A → C  (A 被重用!)

    线程1: [恢复] CAS(top, A→B) 成功！(因为 top 还是 A)
    但 B 已经被释放了！→ 崩溃 / 数据损坏

  解决方案：

  1. 带版本号的指针 (Tagged Pointer)
     将版本号嵌入指针（利用指针高位或对齐低位）
     ┌────────────┬──────────────────┐
     │ version(16)│   pointer(48)    │
     └────────────┴──────────────────┘
     CAS 同时比较版本号和指针，即使指针相同版本号也不同

  2. Double-width CAS (DWCAS / cmpxchg16b on x86-64)
     同时 CAS 128 位：{pointer, counter}

  3. Hazard Pointers
     读取前注册 "正在使用的指针"，其他线程不会释放它

  4. Epoch-Based Reclamation
     延迟释放，确保没有线程在访问旧数据时才释放
)";

    // Tagged Pointer 演示
    struct TaggedPtr {
        uintptr_t ptr_and_tag;

        static TaggedPtr make(void* p, uint16_t tag) {
            return { reinterpret_cast<uintptr_t>(p) | (static_cast<uintptr_t>(tag) << 48) };
        }

        void* ptr() const {
            return reinterpret_cast<void*>(ptr_and_tag & 0x0000FFFFFFFFFFFF);
        }

        uint16_t tag() const {
            return static_cast<uint16_t>(ptr_and_tag >> 48);
        }
    };

    int dummy = 42;
    auto tp1 = TaggedPtr::make(&dummy, 0);
    auto tp2 = TaggedPtr::make(&dummy, 1);

    std::cout << "  TaggedPtr 示例:\n";
    std::cout << "    tp1: ptr=" << tp1.ptr() << ", tag=" << tp1.tag() << "\n";
    std::cout << "    tp2: ptr=" << tp2.ptr() << ", tag=" << tp2.tag() << "\n";
    std::cout << "    同一指针, 但 tag 不同 → CAS 能区分\n";

    // 用 atomic<TaggedPtr> 进行 CAS
    std::atomic<TaggedPtr> atp;
    atp.store(tp1);
    TaggedPtr expected = tp1;
    bool ok = atp.compare_exchange_strong(expected, tp2);
    std::cout << "    CAS(tag0→tag1): " << (ok ? "成功" : "失败") << "\n";
}

} // namespace ch4


// =============================================================================
// ██ 二、内存顺序篇
// =============================================================================

// =============================================================================
// 第5章：CPU 与编译器重排
// =============================================================================

namespace ch5 {

void demo_reordering() {
    print_section("CPU 指令重排与编译器重排");

    std::cout << R"(
  为什么需要内存顺序？因为你写的代码可能不按你写的顺序执行！

  ═══ 编译器重排 ═══
  编译器在 as-if 规则下，可以重新排列不相关的指令。
  例如：
    a = 1;    编译器可能生成:   b = 2;
    b = 2;                      a = 1;
  在单线程下等价，但多线程下另一个线程可能看到 b=2 但 a=0！

  ═══ CPU 重排 (硬件重排) ═══
  即使编译器没有重排，CPU 也可能乱序执行。

  x86/x64 (TSO - Total Store Order):
    Store-Store: ❌ 不重排 (STore 顺序保证)
    Load-Load:   ❌ 不重排
    Load-Store:  ❌ 不重排
    Store-Load:  ✅ 可能重排！(最常见的坑)
    → x86 相对安全，但 Store-Load 重排仍可导致 bug

  ARM / POWER (弱内存模型):
    所有组合都可能重排！必须显式用 barrier。
    → 在 ARM 上不加 fence 的无锁代码几乎一定会出 bug

  ═══ Store Buffer (存储缓冲区) ═══
  CPU 把 store 先写入 store buffer (对本核可见)，
  再异步刷新到缓存（对其他核可见）。
  这导致其他核看到 store 的延迟（Store-Load 重排的根本原因）。

  ═══ MESI 缓存一致性协议 ═══
  保证最终一致，但不保证即时一致。
  Modified → 本核已修改，其他核无效
  Exclusive → 本核独占
  Shared → 多核共享未修改
  Invalid → 无效

  解决方案: std::atomic + memory_order
)";

    // 经典示例: 没有同步 → 可能看到"不可能"的结果
    std::cout << "  Store-Load 重排演示（不使用原子操作时可能出错）:\n";

    std::atomic<int> x(0), y(0);
    std::atomic<int> r1(0), r2(0);

    constexpr int ITERS = 100000;
    int both_zero = 0;

    for (int iter = 0; iter < ITERS; ++iter) {
        x = 0; y = 0;

        std::thread t1([&] {
            x.store(1, std::memory_order_relaxed);
            r1.store(y.load(std::memory_order_relaxed), std::memory_order_relaxed);
        });

        std::thread t2([&] {
            y.store(1, std::memory_order_relaxed);
            r2.store(x.load(std::memory_order_relaxed), std::memory_order_relaxed);
        });

        t1.join(); t2.join();

        if (r1 == 0 && r2 == 0) ++both_zero;
    }

    std::cout << "    " << ITERS << " 次中 r1=0 && r2=0 出现 " << both_zero << " 次\n";
    std::cout << "    (这在顺序一致模型下是不可能的！但 relaxed 下可以)\n";
}

} // namespace ch5


// =============================================================================
// 第6章：六种 memory_order 逐一精讲
// =============================================================================

namespace ch6 {

void demo_memory_orders() {
    print_section("六种 memory_order 详解");

    std::cout << R"(
  C++ 定义了六种内存顺序：

  ┌───────────────────────────────────────────────────────────────┐
  │ 顺序                │ 简称    │ 约束           │ 性能  │
  ├─────────────────────┼─────────┼────────────────┼──────┤
  │ memory_order_relaxed│ relaxed │ 仅保证原子性    │ 最快  │
  │ memory_order_consume│ consume │ 数据依赖链     │ (少用)│
  │ memory_order_acquire│ acquire │ 读之后不前移    │ 快    │
  │ memory_order_release│ release │ 写之前不后移    │ 快    │
  │ memory_order_acq_rel│ acq_rel │ acquire+release│ 中    │
  │ memory_order_seq_cst│ seq_cst │ 全局全序       │ 最慢  │
  └─────────────────────┴─────────┴────────────────┴──────┘

  ═══ 1. relaxed ═══
  仅保证操作本身是原子的，不提供任何排序保证。
  其他线程可能以任意顺序看到 relaxed 操作。
  用途：计数器、统计数据（不需要与其他数据同步时）。
    counter.fetch_add(1, memory_order_relaxed);

  ═══ 2. acquire ═══
  用于 load 操作。
  保证：此 load 之后的所有读写，不会被重排到此 load 之前。
  "获取"其他线程 release 发布的数据。
    ┌──────────────┐
    │ load(acquire)│ ← 屏障：之后的操作不能移到这之前
    │ read a       │
    │ read b       │
    └──────────────┘

  ═══ 3. release ═══
  用于 store 操作。
  保证：此 store 之前的所有读写，不会被重排到此 store 之后。
  "发布"数据，供其他线程 acquire 获取。
    ┌──────────────┐
    │ write a      │
    │ write b      │
    │ store(release)│ ← 屏障：之前的操作不能移到这之后
    └──────────────┘

  ═══ 4. acquire + release 配对 ═══
  线程 A: 准备数据, flag.store(true, release)
  线程 B: flag.load(acquire), 使用数据
  A 的 release 之前的所有写入，对 B 的 acquire 之后都可见。
  这是最常用的同步模式！

  ═══ 5. acq_rel ═══
  用于 RMW 操作（如 CAS、fetch_add）。
  同时具有 acquire 和 release 语义。

  ═══ 6. seq_cst (顺序一致性) ═══
  最强约束：所有 seq_cst 操作形成一个全局全序。
  所有线程看到完全相同的操作顺序。
  是默认的 memory_order。性能最低（x86 上 store 会加 MFENCE）。
)";

    // --- acquire-release 演示 ---
    std::cout << "  acquire-release 经典模式演示:\n";

    std::atomic<bool> ready(false);
    int data = 0;

    std::thread producer([&] {
        data = 42;                                      // ① 准备数据
        ready.store(true, std::memory_order_release);   // ② 发布
    });

    std::thread consumer([&] {
        while (!ready.load(std::memory_order_acquire))  // ③ 获取
            ; // 自旋等待
        assert(data == 42);                              // ④ 安全使用
        std::cout << "    consumer 读到 data = " << data << " ✓\n";
    });

    producer.join();
    consumer.join();

    // --- seq_cst vs relaxed 性能差异 ---
    std::cout << "\n  性能对比 (1000万次递增):\n";
    constexpr int N = 10'000'000;

    {
        std::atomic<int> c(0);
        Timer t;
        for (int i = 0; i < N; ++i)
            c.fetch_add(1, std::memory_order_relaxed);
        std::cout << "    relaxed: " << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << " ms\n";
    }
    {
        std::atomic<int> c(0);
        Timer t;
        for (int i = 0; i < N; ++i)
            c.fetch_add(1, std::memory_order_seq_cst);
        std::cout << "    seq_cst: " << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << " ms\n";
    }
}

} // namespace ch6


// =============================================================================
// 第7章：Acquire-Release 深入：生产者-消费者
// =============================================================================

namespace ch7 {

void demo_acquire_release_deep() {
    print_section("Acquire-Release 生产者-消费者");

    // 环形缓冲区使用 acquire-release 同步
    constexpr int CAPACITY = 1024;

    struct SPSCQueue {
        std::array<int, CAPACITY> buffer_;
        alignas(CACHE_LINE) std::atomic<size_t> head_{0}; // 消费者拥有
        alignas(CACHE_LINE) std::atomic<size_t> tail_{0}; // 生产者拥有

        bool push(int val) {
            size_t t = tail_.load(std::memory_order_relaxed);
            size_t next = (t + 1) % CAPACITY;
            if (next == head_.load(std::memory_order_acquire)) // ① 获取 head
                return false; // 满
            buffer_[t] = val;                                   // ② 写数据
            tail_.store(next, std::memory_order_release);       // ③ 发布 tail
            return true;
        }

        bool pop(int& val) {
            size_t h = head_.load(std::memory_order_relaxed);
            if (h == tail_.load(std::memory_order_acquire))     // ④ 获取 tail
                return false; // 空
            val = buffer_[h];                                    // ⑤ 读数据
            head_.store((h + 1) % CAPACITY, std::memory_order_release); // ⑥ 发布 head
            return true;
        }
    };

    std::cout << "  SPSC 无锁队列 (acquire-release 同步):\n";

    SPSCQueue q;
    constexpr int COUNT = 1'000'000;
    std::atomic<bool> done(false);
    long long sum_produced = 0, sum_consumed = 0;

    std::thread producer([&] {
        for (int i = 1; i <= COUNT; ++i) {
            while (!q.push(i)) std::this_thread::yield();
            sum_produced += i;
        }
        done.store(true, std::memory_order_release);
    });

    std::thread consumer([&] {
        int val;
        while (true) {
            if (q.pop(val)) {
                sum_consumed += val;
            } else if (done.load(std::memory_order_acquire)) {
                while (q.pop(val)) sum_consumed += val;
                break;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    std::cout << "    生产者 sum = " << sum_produced << "\n";
    std::cout << "    消费者 sum = " << sum_consumed << "\n";
    std::cout << "    匹配: " << (sum_produced == sum_consumed ? "✓" : "✗") << "\n";

    std::cout << R"(
  同步分析:
    生产者:
      buffer_[t] = val;        // 先写数据
      tail_.store(release);    // 然后发布 (release 保证数据写在 store 前完成)

    消费者:
      tail_.load(acquire);     // 先获取 (acquire 保证后续读在 load 后执行)
      val = buffer_[h];        // 然后读数据

    release-acquire 配对确保消费者一定能看到生产者写入的数据！
)";
}

} // namespace ch7


// =============================================================================
// 第8章：Release Sequence 与传递性
// =============================================================================

namespace ch8 {

void demo_release_sequence() {
    print_section("Release Sequence 与传递性");

    std::cout << R"(
  Release Sequence (释放序列):
  一个 release store 之后的连续 RMW 操作(即使是relaxed)
  仍然是 release sequence 的一部分。

  线程 A: data = 42; count.store(5, release);   // ① release
  线程 B: count.fetch_sub(1, relaxed);           // ② RMW (在 release sequence 中)
  线程 C: count.fetch_sub(1, relaxed);           // ③ RMW (在 release sequence 中)
  线程 D: v = count.load(acquire);               // ④ acquire
          if (v == 3) use(data);                  // 此时 data=42 对 D 可见！

  因为 ②③ 是 ① 之后对 count 的 RMW，它们属于 ① 的 release sequence。
  ④ 的 acquire 与 ① 的 release 配对（通过 release sequence）。

  典型应用：shared_ptr 引用计数
    拷贝: ref.fetch_add(1, relaxed)         — 只需原子性
    销毁: if (ref.fetch_sub(1, release) == 1) {
            atomic_thread_fence(acquire);    — 同步所有之前的 release
            delete ptr;
          }
)";

    // 演示 release sequence
    int data = 0;
    std::atomic<int> count(0);
    constexpr int NUM_WORKERS = 3;

    std::thread producer([&] {
        data = 42;
        count.store(NUM_WORKERS, std::memory_order_release); // release
    });

    std::vector<std::thread> workers;
    std::atomic<int> finished(0);

    for (int i = 0; i < NUM_WORKERS; ++i) {
        workers.emplace_back([&, i] {
            // 等待生产者
            while (count.load(std::memory_order_relaxed) == 0)
                std::this_thread::yield();

            // RMW（在 release sequence 中）
            int remaining = count.fetch_sub(1, std::memory_order_acq_rel);
            if (remaining == 1) {
                // 最后一个 worker，通过 acq_rel 确保看到 data
                assert(data == 42);
                finished.store(1, std::memory_order_release);
            }
        });
    }

    producer.join();
    for (auto& w : workers) w.join();
    std::cout << "  release sequence 验证: data = " << data << " ✓\n";

    // --- Transitivity 传递性 ---
    std::cout << R"(
  传递性 (Transitivity / Happens-Before 传递):

  线程A: x.store(1, release)
  线程B: x.load(acquire)  "同步于" 线程A的 release
         y.store(1, release)
  线程C: y.load(acquire)  "同步于" 线程B的 release

  由传递性: 线程A 的 x.store happens-before 线程C 的 y.load
  → 线程 C 能看到线程 A 的所有副作用（即使没有直接同步）

  ⚠️ relaxed 没有传递性！relaxed 只保证同一变量的原子性。
)";
}

} // namespace ch8


// =============================================================================
// 第9章：memory_order_consume
// =============================================================================

namespace ch9 {

void demo_consume() {
    print_section("memory_order_consume 与数据依赖");

    std::cout << R"(
  consume 是比 acquire 更弱的约束，只保证"数据依赖链"上的可见性。

  含义:
    acquire: load 之后的所有操作都不能重排到 load 之前
    consume: 只有 依赖于此 load 值 的操作才不能重排

  示例:
    struct Node { int data; Node* next; };
    atomic<Node*> head;

    线程A: node->data = 42; head.store(node, release);
    线程B: Node* p = head.load(consume);
            p->data;   // ✅ 依赖于 p → 保证看到 42
            global_var; // ❌ 不依赖于 p → 不保证（acquire 才保证）

  为什么 consume 有用？
    在 ARM/POWER 等弱序架构上，consume 不需要 memory barrier，
    而 acquire 需要。consume 利用 CPU 的数据依赖保证即可。

  ⚠️ 现状 (C++17/20):
    consume 的语义对编译器实现太困难（依赖链追踪问题）。
    所有主流编译器都将 consume 升级为 acquire。
    C++ 委员会正在重新设计（可能在未来标准中修改）。
    → 实际代码中，直接用 acquire 即可。
)";
}

} // namespace ch9


// =============================================================================
// 第10章：volatile vs atomic
// =============================================================================

namespace ch10 {

void demo_volatile_vs_atomic() {
    print_section("volatile vs atomic 本质区别");

    std::cout << R"(
  ┌──────────────┬──────────────────────┬──────────────────────┐
  │              │ volatile             │ std::atomic           │
  ├──────────────┼──────────────────────┼──────────────────────┤
  │ 原子性       │ ❌ 不保证            │ ✅ 保证              │
  │ 内存顺序     │ ❌ 不保证            │ ✅ 可指定            │
  │ 编译器重排   │ ✅ 不重排(自身)      │ ✅ 不重排            │
  │ CPU 重排     │ ❌ 不阻止            │ ✅ 有序列化语义      │
  │ 优化         │ 不优化掉读写         │ 不优化掉读写         │
  │ CAS 等操作   │ ❌ 没有              │ ✅ 提供              │
  │ 用途         │ MMIO, 信号处理       │ 多线程同步           │
  └──────────────┴──────────────────────┴──────────────────────┘

  volatile 的含义: "别优化掉这个访问"
    编译器不会缓存 volatile 变量的值、不会消除冗余读写。
    但不提供线程安全性！

  什么时候用 volatile:
    1. MMIO — 访问硬件寄存器 (每次必须真正读/写硬件)
    2. 信号处理 — volatile sig_atomic_t
    3. longjmp — 与 setjmp 配合

  什么时候用 atomic:
    1. 多线程共享变量 — 几乎所有多线程场景
    2. lock-free 数据结构

  经典错误:
    volatile bool flag = false;
    // 线程 A: flag = true;
    // 线程 B: while (!flag);
    // 在 x86 上"碰巧"能工作，在 ARM 上可能死循环！
    // 正确做法: std::atomic<bool> flag;

  C++20: volatile 的部分操作被 deprecated
    volatile int x; x++; // C++20 deprecated
    → 如果需要原子操作，用 atomic
    → 如果需要 volatile 语义（抑制优化），保持 volatile 但不做复合操作
)";
}

} // namespace ch10


// =============================================================================
// 第11章：独立 fence
// =============================================================================

namespace ch11 {

void demo_fences() {
    print_section("独立内存屏障 (atomic_thread_fence)");

    std::cout << R"(
  std::atomic_thread_fence 提供独立于原子操作的屏障。

  fence vs 原子操作的 memory_order:
    原子操作: 屏障"附着"在特定原子变量上
    fence:    屏障"独立"，影响所有内存操作

  类型:
    atomic_thread_fence(memory_order_release)
      确保 fence 之前的所有写操作对其他线程可见
      等效于"所有之前的 store 都变成 release"

    atomic_thread_fence(memory_order_acquire)
      确保 fence 之后的所有读操作能看到其他线程的写入
      等效于"所有之后的 load 都变成 acquire"

    atomic_thread_fence(memory_order_seq_cst)
      全序屏障（最强，x86 上编译为 MFENCE）

    atomic_thread_fence(memory_order_acq_rel)
      同时 acquire + release

    atomic_signal_fence(order)
      仅防止编译器重排，不生成 CPU 指令（同线程内信号处理用）
)";

    // acquire fence 的用法
    std::atomic<bool> ready(false);
    int payload = 0;

    std::thread writer([&] {
        payload = 42;
        std::atomic_thread_fence(std::memory_order_release); // release fence
        ready.store(true, std::memory_order_relaxed);
    });

    std::thread reader([&] {
        while (!ready.load(std::memory_order_relaxed))
            ;
        std::atomic_thread_fence(std::memory_order_acquire); // acquire fence
        assert(payload == 42);
        std::cout << "  fence 同步验证: payload = " << payload << " ✓\n";
    });

    writer.join();
    reader.join();

    std::cout << R"(
  fence 的优势:
    当有多个 relaxed 操作需要统一加 barrier 时，
    用一个 fence 比每个操作都加 acquire/release 更高效。

    例如（批量同步）:
      data1 = 1;
      data2 = 2;
      data3 = 3;
      atomic_thread_fence(release);   // 一个 fence 保护三个 store
      flag.store(true, relaxed);

    等效于:
      data1 = 1;
      data2 = 2;
      flag.store(true, release);  // 只保护flag之前的store
    但如果 data3 在 flag 之后，fence 版本更灵活。

  x86 编译结果:
    release fence → 编译器屏障（x86 不需要 CPU 屏障，因为 TSO）
    acquire fence → 编译器屏障
    seq_cst fence → MFENCE 指令
)";
}

} // namespace ch11


// =============================================================================
// ██ 三、无锁算法篇
// =============================================================================

// =============================================================================
// 第12章：自旋锁
// =============================================================================

namespace ch12 {

// 1. TAS (Test-And-Set) — 最简单
class TASLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire))
            ; // 忙等
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};

// 2. TTAS (Test-and-Test-And-Set) — 减少总线流量
class TTASLock {
    std::atomic<bool> flag_{false};
public:
    void lock() {
        for (;;) {
            // 先 test (只读，不产生 cache line 独占)
            if (!flag_.load(std::memory_order_relaxed)) {
                // 再 test-and-set (写操作)
                if (!flag_.exchange(true, std::memory_order_acquire))
                    return; // 获取成功
            }
            // 可加退避: std::this_thread::yield()
        }
    }
    void unlock() {
        flag_.store(false, std::memory_order_release);
    }
};

// 3. Ticket Lock — FIFO 公平性保证
class TicketLock {
    alignas(CACHE_LINE) std::atomic<unsigned> next_ticket_{0};
    alignas(CACHE_LINE) std::atomic<unsigned> now_serving_{0};
public:
    void lock() {
        unsigned my_ticket = next_ticket_.fetch_add(1, std::memory_order_relaxed);
        while (now_serving_.load(std::memory_order_acquire) != my_ticket) {
            // 可加指数退避
#if defined(__x86_64__) || defined(_M_X64)
            // _mm_pause(); // 降低自旋功耗
#endif
        }
    }
    void unlock() {
        now_serving_.fetch_add(1, std::memory_order_release);
    }
};

void demo_spinlocks() {
    print_section("三种自旋锁对比");

    auto benchmark = [](auto& lock, const char* name) {
        constexpr int N = 1'000'000;
        constexpr int T = 4;
        long long counter = 0;

        Timer timer;
        std::vector<std::thread> threads;
        for (int t = 0; t < T; ++t) {
            threads.emplace_back([&] {
                for (int i = 0; i < N / T; ++i) {
                    lock.lock();
                    ++counter;
                    lock.unlock();
                }
            });
        }
        for (auto& th : threads) th.join();

        std::cout << "    " << std::setw(14) << std::left << name
                  << " counter=" << counter
                  << " time=" << std::fixed << std::setprecision(1)
                  << timer.elapsed_ms() << "ms\n";
    };

    TASLock tas;
    TTASLock ttas;
    TicketLock ticket;
    std::mutex mtx;

    benchmark(tas,    "TAS Lock");
    benchmark(ttas,   "TTAS Lock");
    benchmark(ticket, "Ticket Lock");
    benchmark(mtx,    "std::mutex");

    std::cout << R"(
  对比分析:
    TAS:    简单，但高竞争时大量无效写（cache line bouncing）
    TTAS:   先读后写，减少总线独占请求（推荐）
    Ticket: FIFO 公平，避免饥饿
    mutex:  内核级，含 futex 优化（低竞争时很快，高竞争时上下文切换）

  适用场景:
    极短临界区 (10-50ns): 自旋锁
    较长临界区 (>1μs):    mutex（避免浪费CPU）
    需要公平性:           Ticket Lock
)";
}

} // namespace ch12


// =============================================================================
// 第13章：无锁栈 (Treiber Stack)
// =============================================================================

namespace ch13 {

// Treiber Stack — 经典无锁栈
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};
    std::atomic<size_t> size_{0};

    // 简易延迟回收（生产环境用 Hazard Pointer 或 EBR）
    std::atomic<Node*> to_delete_{nullptr};

public:
    ~LockFreeStack() {
        // 清理
        Node* node = head_.load();
        while (node) { Node* next = node->next; delete node; node = next; }
        node = to_delete_.load();
        while (node) { Node* next = node->next; delete node; node = next; }
    }

    void push(const T& val) {
        Node* new_node = new Node(val);
        new_node->next = head_.load(std::memory_order_relaxed);

        // CAS 循环: 尝试把 head 指向新节点
        while (!head_.compare_exchange_weak(
                    new_node->next, new_node,
                    std::memory_order_release,
                    std::memory_order_relaxed))
            ; // new_node->next 被自动更新为当前 head

        size_.fetch_add(1, std::memory_order_relaxed);
    }

    bool pop(T& result) {
        Node* old_head = head_.load(std::memory_order_acquire);

        while (old_head) {
            // CAS: 尝试把 head 移到 next
            if (head_.compare_exchange_weak(
                        old_head, old_head->next,
                        std::memory_order_acq_rel,
                        std::memory_order_acquire)) {
                result = old_head->data;

                // 延迟删除（简化版：放入待删链表）
                old_head->next = to_delete_.load(std::memory_order_relaxed);
                while (!to_delete_.compare_exchange_weak(
                            old_head->next, old_head,
                            std::memory_order_release,
                            std::memory_order_relaxed))
                    ;

                size_.fetch_sub(1, std::memory_order_relaxed);
                return true;
            }
            // old_head 被自动更新为当前 head
        }
        return false; // 空栈
    }

    size_t size() const { return size_.load(std::memory_order_relaxed); }
};

void demo_treiber_stack() {
    print_section("无锁栈 (Treiber Stack)");

    LockFreeStack<int> stack;

    std::cout << "  Treiber Stack 原理:\n";
    std::cout << R"(
    push(val):
      1. new_node->next = head
      2. CAS(&head, new_node->next, new_node)
         成功 → 完成
         失败 → new_node->next 更新为最新 head, 重试

    pop():
      1. old = head
      2. CAS(&head, old, old->next)
         成功 → return old->data
         失败 → old 更新为最新 head, 重试

    ┌─────┐   ┌─────┐   ┌─────┐
    │  C  │──→│  B  │──→│  A  │──→ nullptr
    └─────┘   └─────┘   └─────┘
       ↑
      head
)";

    // 多线程测试
    constexpr int N = 100000;
    constexpr int T = 4;
    std::atomic<long long> push_sum(0), pop_sum(0);

    std::vector<std::thread> threads;

    // 推入线程
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&, t] {
            for (int i = t * N; i < (t + 1) * N; ++i) {
                stack.push(i);
                push_sum.fetch_add(i, std::memory_order_relaxed);
            }
        });
    }
    for (auto& th : threads) th.join();
    threads.clear();

    std::cout << "  推入 " << T * N << " 个元素, size = " << stack.size() << "\n";

    // 弹出线程
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&] {
            int val;
            while (stack.pop(val)) {
                pop_sum.fetch_add(val, std::memory_order_relaxed);
            }
        });
    }
    for (auto& th : threads) th.join();

    std::cout << "  push_sum = " << push_sum.load()
              << ", pop_sum = " << pop_sum.load()
              << " → " << (push_sum == pop_sum ? "匹配 ✓" : "不匹配 ✗") << "\n";
}

} // namespace ch13


// =============================================================================
// 第14章：无锁队列 (Michael-Scott Queue)
// =============================================================================

namespace ch14 {

// Michael-Scott Queue — 经典 MPMC 无锁队列
// 使用哨兵节点 (dummy node)
template<typename T>
class MSQueue {
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node() : next(nullptr) {}
        Node(const T& d) : data(d), next(nullptr) {}
    };

    alignas(CACHE_LINE) std::atomic<Node*> head_;
    alignas(CACHE_LINE) std::atomic<Node*> tail_;

public:
    MSQueue() {
        Node* dummy = new Node(); // 哨兵节点
        head_.store(dummy);
        tail_.store(dummy);
    }

    ~MSQueue() {
        T val;
        while (dequeue(val)) {}
        delete head_.load();
    }

    void enqueue(const T& val) {
        Node* new_node = new Node(val);

        while (true) {
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = tail->next.load(std::memory_order_acquire);

            if (tail == tail_.load(std::memory_order_relaxed)) {
                if (next == nullptr) {
                    // tail 确实是最后一个节点，尝试追加
                    if (tail->next.compare_exchange_weak(
                                next, new_node,
                                std::memory_order_release,
                                std::memory_order_relaxed)) {
                        // 成功追加，尝试移动 tail（失败也无妨，其他线程会帮忙）
                        tail_.compare_exchange_strong(
                            tail, new_node,
                            std::memory_order_release,
                            std::memory_order_relaxed);
                        return;
                    }
                } else {
                    // tail 落后了，帮助推进
                    tail_.compare_exchange_strong(
                        tail, next,
                        std::memory_order_release,
                        std::memory_order_relaxed);
                }
            }
        }
    }

    bool dequeue(T& result) {
        while (true) {
            Node* head = head_.load(std::memory_order_acquire);
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = head->next.load(std::memory_order_acquire);

            if (head == head_.load(std::memory_order_relaxed)) {
                if (head == tail) {
                    if (next == nullptr) return false; // 队列为空
                    // tail 落后了，推进
                    tail_.compare_exchange_strong(
                        tail, next,
                        std::memory_order_release,
                        std::memory_order_relaxed);
                } else {
                    result = next->data;
                    if (head_.compare_exchange_weak(
                                head, next,
                                std::memory_order_acq_rel,
                                std::memory_order_acquire)) {
                        delete head; // 删除旧 dummy
                        return true;
                    }
                }
            }
        }
    }
};

void demo_ms_queue() {
    print_section("Michael-Scott 无锁队列");

    std::cout << R"(
  MS Queue 特点:
    - MPMC (多生产者多消费者)
    - 使用哨兵节点 (dummy) 分离 head 和 tail 的竞争
    - "帮助推进" 机制：若发现 tail 落后，任何线程都可以帮忙推进

    结构:
      head ──→ [dummy] ──→ [A] ──→ [B] ──→ nullptr
                                              ↑
                                            tail

    enqueue(C):
      1. 读 tail 和 tail->next
      2. 如果 tail->next == null:
         CAS(tail->next, null→C)  — 追加
         CAS(tail, old→C)         — 推进 tail
      3. 如果 tail->next != null:
         CAS(tail, old→next)      — 帮助推进

    dequeue():
      1. 读 head 和 head->next
      2. 如果 head == tail && next == null: 空队列
      3. 否则: CAS(head, old→next), 读 next->data, delete old
)";

    MSQueue<int> q;
    constexpr int N = 50000;
    constexpr int P = 4, C = 4;

    std::atomic<long long> enq_sum(0), deq_sum(0);
    std::atomic<int> enq_count(0), deq_count(0);

    // 生产者
    std::vector<std::thread> producers;
    for (int p = 0; p < P; ++p) {
        producers.emplace_back([&, p] {
            for (int i = p * N; i < (p + 1) * N; ++i) {
                q.enqueue(i);
                enq_sum.fetch_add(i, std::memory_order_relaxed);
                enq_count.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 消费者
    std::atomic<bool> done(false);
    std::vector<std::thread> consumers;
    for (int c = 0; c < C; ++c) {
        consumers.emplace_back([&] {
            int val;
            while (true) {
                if (q.dequeue(val)) {
                    deq_sum.fetch_add(val, std::memory_order_relaxed);
                    deq_count.fetch_add(1, std::memory_order_relaxed);
                } else if (done.load(std::memory_order_acquire)) {
                    while (q.dequeue(val)) {
                        deq_sum.fetch_add(val, std::memory_order_relaxed);
                        deq_count.fetch_add(1, std::memory_order_relaxed);
                    }
                    break;
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& p : producers) p.join();
    done.store(true, std::memory_order_release);
    for (auto& c : consumers) c.join();

    std::cout << "  " << P << " 生产者 × " << C << " 消费者, 每组 " << N << " 个\n";
    std::cout << "  enqueue: " << enq_count.load() << " 个, sum=" << enq_sum.load() << "\n";
    std::cout << "  dequeue: " << deq_count.load() << " 个, sum=" << deq_sum.load() << "\n";
    std::cout << "  匹配: " << (enq_sum == deq_sum ? "✓" : "✗") << "\n";
}

} // namespace ch14


// =============================================================================
// 第15章：无锁哈希表
// =============================================================================

namespace ch15 {

// 分桶 + 原子链表 无锁哈希表
// 支持 lock-free 插入和查找
template<typename K, typename V, size_t NUM_BUCKETS = 64>
class LockFreeHashMap {
    struct Node {
        K key;
        V value;
        std::atomic<Node*> next;
        Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    std::array<std::atomic<Node*>, NUM_BUCKETS> buckets_;

    size_t bucket_index(const K& key) const {
        return std::hash<K>{}(key) % NUM_BUCKETS;
    }

public:
    LockFreeHashMap() {
        for (auto& b : buckets_) b.store(nullptr, std::memory_order_relaxed);
    }

    ~LockFreeHashMap() {
        for (auto& b : buckets_) {
            Node* node = b.load();
            while (node) { Node* next = node->next.load(); delete node; node = next; }
        }
    }

    // 插入（key 存在则不更新）
    bool insert(const K& key, const V& value) {
        size_t idx = bucket_index(key);
        Node* new_node = new Node(key, value);

        Node* head = buckets_[idx].load(std::memory_order_acquire);
        do {
            // 检查 key 是否已存在
            Node* curr = head;
            while (curr) {
                if (curr->key == key) {
                    delete new_node;
                    return false; // 已存在
                }
                curr = curr->next.load(std::memory_order_relaxed);
            }
            new_node->next.store(head, std::memory_order_relaxed);
        } while (!buckets_[idx].compare_exchange_weak(
                     head, new_node,
                     std::memory_order_release,
                     std::memory_order_acquire));

        return true;
    }

    // 查找
    std::optional<V> find(const K& key) const {
        size_t idx = bucket_index(key);
        Node* curr = buckets_[idx].load(std::memory_order_acquire);
        while (curr) {
            if (curr->key == key) {
                return curr->value;
            }
            curr = curr->next.load(std::memory_order_acquire);
        }
        return std::nullopt;
    }
};

void demo_lockfree_hashmap() {
    print_section("无锁哈希表 (分桶原子链表)");

    LockFreeHashMap<int, std::string, 32> map;

    // 多线程插入
    constexpr int N = 10000;
    constexpr int T = 4;
    std::atomic<int> success_count(0);

    std::vector<std::thread> threads;
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&, t] {
            for (int i = t * N; i < (t + 1) * N; ++i) {
                if (map.insert(i, "val_" + std::to_string(i)))
                    success_count.fetch_add(1);
            }
        });
    }
    for (auto& th : threads) th.join();

    std::cout << "  插入 " << success_count.load() << " 个不重复键\n";

    // 验证查找
    int found = 0;
    for (int i = 0; i < T * N; ++i) {
        if (map.find(i).has_value()) ++found;
    }
    std::cout << "  查找验证: " << found << " / " << T * N << " ✓\n";
}

} // namespace ch15


// =============================================================================
// 第16章：无锁环形缓冲区
// =============================================================================

namespace ch16 {

// SPSC (Single-Producer Single-Consumer) 无锁环形缓冲区
template<typename T, size_t Capacity>
class SPSCRingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

    std::array<T, Capacity> buffer_;
    alignas(CACHE_LINE) std::atomic<size_t> write_pos_{0};
    alignas(CACHE_LINE) std::atomic<size_t> read_pos_{0};

public:
    bool push(const T& val) {
        size_t wp = write_pos_.load(std::memory_order_relaxed);
        size_t next_wp = (wp + 1) & (Capacity - 1);

        if (next_wp == read_pos_.load(std::memory_order_acquire))
            return false; // 满

        buffer_[wp] = val;
        write_pos_.store(next_wp, std::memory_order_release);
        return true;
    }

    bool pop(T& val) {
        size_t rp = read_pos_.load(std::memory_order_relaxed);
        if (rp == write_pos_.load(std::memory_order_acquire))
            return false; // 空

        val = buffer_[rp];
        read_pos_.store((rp + 1) & (Capacity - 1), std::memory_order_release);
        return true;
    }

    size_t size() const {
        size_t wp = write_pos_.load(std::memory_order_relaxed);
        size_t rp = read_pos_.load(std::memory_order_relaxed);
        return (wp - rp + Capacity) & (Capacity - 1);
    }
};

// MPMC 无锁环形缓冲区 (Vyukov 风格, 使用 sequence 序号)
template<typename T, size_t Capacity>
class MPMCRingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    std::array<Cell, Capacity> buffer_;
    alignas(CACHE_LINE) std::atomic<size_t> enqueue_pos_{0};
    alignas(CACHE_LINE) std::atomic<size_t> dequeue_pos_{0};

public:
    MPMCRingBuffer() {
        for (size_t i = 0; i < Capacity; ++i)
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
    }

    bool push(const T& val) {
        size_t pos;
        Cell* cell;
        for (;;) {
            pos = enqueue_pos_.load(std::memory_order_relaxed);
            cell = &buffer_[pos & (Capacity - 1)];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)pos;

            if (diff == 0) {
                // 空槽，尝试占用
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1,
                        std::memory_order_relaxed))
                    break;
            } else if (diff < 0) {
                return false; // 满
            }
            // else: 其他线程正在使用此位置，重试
        }
        cell->data = val;
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& val) {
        size_t pos;
        Cell* cell;
        for (;;) {
            pos = dequeue_pos_.load(std::memory_order_relaxed);
            cell = &buffer_[pos & (Capacity - 1)];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

            if (diff == 0) {
                // 有数据，尝试消费
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1,
                        std::memory_order_relaxed))
                    break;
            } else if (diff < 0) {
                return false; // 空
            }
        }
        val = cell->data;
        cell->sequence.store(pos + Capacity, std::memory_order_release);
        return true;
    }
};

void demo_ring_buffers() {
    print_section("无锁环形缓冲区 (SPSC / MPMC)");

    // SPSC 测试
    {
        SPSCRingBuffer<int, 1024> rb;
        constexpr int N = 1'000'000;
        long long sum_in = 0, sum_out = 0;

        Timer t;
        std::thread producer([&] {
            for (int i = 1; i <= N; ++i) {
                while (!rb.push(i)) std::this_thread::yield();
                sum_in += i;
            }
        });

        std::thread consumer([&] {
            int val, count = 0;
            while (count < N) {
                if (rb.pop(val)) { sum_out += val; ++count; }
                else std::this_thread::yield();
            }
        });

        producer.join(); consumer.join();
        std::cout << "  SPSC: " << N << " 个元素, "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << "ms, "
                  << (sum_in == sum_out ? "✓" : "✗") << "\n";
    }

    // MPMC 测试
    {
        MPMCRingBuffer<int, 1024> rb;
        constexpr int N = 200000;
        constexpr int T = 4;
        std::atomic<long long> sum_in(0), sum_out(0);

        Timer t;
        std::vector<std::thread> producers, consumers;

        for (int i = 0; i < T; ++i) {
            producers.emplace_back([&, i] {
                for (int j = i * N; j < (i + 1) * N; ++j) {
                    while (!rb.push(j)) std::this_thread::yield();
                    sum_in.fetch_add(j, std::memory_order_relaxed);
                }
            });
        }

        std::atomic<int> consumed(0);
        for (int i = 0; i < T; ++i) {
            consumers.emplace_back([&] {
                int val;
                while (consumed.load(std::memory_order_relaxed) < T * N) {
                    if (rb.pop(val)) {
                        sum_out.fetch_add(val, std::memory_order_relaxed);
                        consumed.fetch_add(1, std::memory_order_relaxed);
                    } else {
                        std::this_thread::yield();
                    }
                }
            });
        }

        for (auto& p : producers) p.join();
        for (auto& c : consumers) c.join();

        std::cout << "  MPMC: " << T * N << " 个元素, "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << "ms, "
                  << (sum_in == sum_out ? "✓" : "✗") << "\n";
    }
}

} // namespace ch16


// =============================================================================
// ██ 四、内存回收篇
// =============================================================================

// =============================================================================
// 第17章：Hazard Pointer
// =============================================================================

namespace ch17 {

// Hazard Pointer — 无锁数据结构的安全内存回收

// 原理：
// 每个线程在访问共享指针前，将其注册为 "hazard pointer"。
// 当要删除一个节点时，检查所有线程的 hazard pointer，
// 如果没有线程指向该节点，才安全删除。

class HazardPointerDomain {
    static constexpr int MAX_THREADS = 64;
    static constexpr int RETIRE_THRESHOLD = 128;

    struct HPRecord {
        std::atomic<std::thread::id> owner;
        std::atomic<void*> ptr;
    };

    std::array<HPRecord, MAX_THREADS> hp_records_;
    // 每线程的待删除列表
    struct RetireNode {
        void* ptr;
        std::function<void(void*)> deleter;
        RetireNode* next;
    };

    static thread_local std::vector<std::pair<void*, std::function<void(void*)>>> retire_list_;

public:
    HazardPointerDomain() {
        for (auto& r : hp_records_) {
            r.owner.store(std::thread::id{});
            r.ptr.store(nullptr);
        }
    }

    // 获取一个 HP 槽位
    int acquire_slot() {
        auto tid = std::this_thread::get_id();
        for (int i = 0; i < MAX_THREADS; ++i) {
            std::thread::id empty{};
            if (hp_records_[i].owner.compare_exchange_strong(empty, tid))
                return i;
        }
        throw std::runtime_error("No free HP slot");
    }

    // 释放槽位
    void release_slot(int slot) {
        hp_records_[slot].ptr.store(nullptr);
        hp_records_[slot].owner.store(std::thread::id{});
    }

    // 保护一个指针
    void protect(int slot, void* p) {
        hp_records_[slot].ptr.store(p, std::memory_order_release);
    }

    void clear(int slot) {
        hp_records_[slot].ptr.store(nullptr, std::memory_order_release);
    }

    // 标记删除
    template<typename T>
    void retire(T* ptr) {
        retire_list_.emplace_back(ptr, [](void* p) { delete static_cast<T*>(p); });
        if (retire_list_.size() >= RETIRE_THRESHOLD) {
            scan();
        }
    }

    // 扫描：删除不在 HP 中的节点
    void scan() {
        // 收集所有活跃的 hazard pointers
        std::set<void*> hazards;
        for (auto& r : hp_records_) {
            void* p = r.ptr.load(std::memory_order_acquire);
            if (p) hazards.insert(p);
        }

        // 删除不在 hazards 中的节点
        auto it = retire_list_.begin();
        while (it != retire_list_.end()) {
            if (hazards.find(it->first) == hazards.end()) {
                it->second(it->first); // 调用 deleter
                it = retire_list_.erase(it);
            } else {
                ++it;
            }
        }
    }

    // 强制清理
    void force_cleanup() {
        scan();
    }
};

thread_local std::vector<std::pair<void*, std::function<void(void*)>>>
    HazardPointerDomain::retire_list_;

void demo_hazard_pointer() {
    print_section("Hazard Pointer 详解");

    std::cout << R"(
  Hazard Pointer 工作流程:

  读取线程:
    1. hp.protect(slot, ptr)         ← 注册 "我在用这个指针"
    2. 验证 ptr 仍然有效 (double-check)
    3. 使用 ptr 指向的数据
    4. hp.clear(slot)                ← 取消保护

  删除线程:
    1. 将旧节点放入 retire_list
    2. 当 retire_list 超过阈值:
       a. 收集所有线程的 hazard pointers
       b. 对 retire_list 中不在 hazards 中的节点执行 delete
       c. 保留仍在 hazards 中的节点

  内存开销: O(H × T + R × T)
    H = 每线程最多保护的指针数
    T = 线程数
    R = 每线程 retire 阈值

  优点: 有界的内存开销
  缺点: 每次访问需要写 HP (cache line 写入)
)";

    // 简单演示
    HazardPointerDomain hp;

    struct Data {
        int value;
        Data(int v) : value(v) {}
    };

    std::atomic<Data*> shared_ptr_var(new Data(42));
    constexpr int ITERS = 10000;
    std::atomic<int> reads_ok(0);

    auto reader = [&] {
        int slot = hp.acquire_slot();
        for (int i = 0; i < ITERS; ++i) {
            Data* p;
            do {
                p = shared_ptr_var.load(std::memory_order_acquire);
                hp.protect(slot, p);
                // 重新检查：保护之后 ptr 可能已经被替换
            } while (p != shared_ptr_var.load(std::memory_order_acquire));

            if (p) {
                volatile int v = p->value; // 使用数据
                (void)v;
                reads_ok.fetch_add(1, std::memory_order_relaxed);
            }
            hp.clear(slot);
        }
        hp.release_slot(slot);
    };

    auto writer = [&] {
        for (int i = 0; i < ITERS; ++i) {
            Data* new_data = new Data(i);
            Data* old = shared_ptr_var.exchange(new_data, std::memory_order_acq_rel);
            if (old) hp.retire(old);
        }
    };

    std::thread r1(reader), r2(reader), w1(writer);
    r1.join(); r2.join(); w1.join();

    hp.force_cleanup();

    // 清理最后一个
    delete shared_ptr_var.load();

    std::cout << "  HP 演示: " << reads_ok.load() << " 次安全读取完成 ✓\n";
}

} // namespace ch17


// =============================================================================
// 第18章：Epoch-Based Reclamation (EBR)
// =============================================================================

namespace ch18 {

class EpochBasedReclamation {
    static constexpr int MAX_THREADS = 64;

    std::atomic<uint64_t> global_epoch_{0};

    struct ThreadState {
        std::atomic<uint64_t> local_epoch{0};
        std::atomic<bool> active{false};
        // 每个 epoch 的待删除列表
        std::vector<std::pair<void*, std::function<void(void*)>>> retire_lists[3];
    };

    std::array<ThreadState, MAX_THREADS> thread_states_;
    std::atomic<int> thread_count_{0};

public:
    int register_thread() {
        return thread_count_.fetch_add(1);
    }

    // 进入临界区
    void enter(int tid) {
        thread_states_[tid].active.store(true, std::memory_order_relaxed);
        thread_states_[tid].local_epoch.store(
            global_epoch_.load(std::memory_order_relaxed),
            std::memory_order_release);
    }

    // 离开临界区
    void leave(int tid) {
        thread_states_[tid].active.store(false, std::memory_order_release);
    }

    // 标记待删除
    template<typename T>
    void retire(int tid, T* ptr) {
        uint64_t epoch = global_epoch_.load(std::memory_order_relaxed);
        thread_states_[tid].retire_lists[epoch % 3].emplace_back(
            ptr, [](void* p) { delete static_cast<T*>(p); });
    }

    // 尝试推进 epoch
    void try_advance() {
        uint64_t cur = global_epoch_.load(std::memory_order_relaxed);
        int count = thread_count_.load(std::memory_order_relaxed);

        // 检查所有活跃线程是否已进入当前 epoch
        for (int i = 0; i < count; ++i) {
            if (thread_states_[i].active.load(std::memory_order_acquire)) {
                if (thread_states_[i].local_epoch.load(std::memory_order_acquire) != cur) {
                    return; // 有线程还在旧 epoch
                }
            }
        }

        // 推进 epoch
        uint64_t new_epoch = cur + 1;
        if (global_epoch_.compare_exchange_strong(cur, new_epoch)) {
            // 安全删除两个 epoch 前的节点
            uint64_t safe_epoch = new_epoch % 3;
            // (new_epoch - 2) % 3 的数据可以安全删除
            uint64_t delete_epoch = (new_epoch + 1) % 3; // = (new_epoch - 2) % 3
            for (int i = 0; i < count; ++i) {
                auto& list = thread_states_[i].retire_lists[delete_epoch];
                for (auto& [ptr, deleter] : list) deleter(ptr);
                list.clear();
            }
        }
    }

    // RAII guard
    class Guard {
        EpochBasedReclamation& ebr_;
        int tid_;
    public:
        Guard(EpochBasedReclamation& ebr, int tid) : ebr_(ebr), tid_(tid) {
            ebr_.enter(tid_);
        }
        ~Guard() { ebr_.leave(tid_); }
    };
};

void demo_ebr() {
    print_section("Epoch-Based Reclamation (EBR)");

    std::cout << R"(
  EBR 原理:
    维护全局 epoch 计数器（通常只需 3 个 epoch）。

    epoch 0 ──→ epoch 1 ──→ epoch 2 ──→ epoch 0 ...

    进入临界区: 记录当前 epoch
    退出临界区: 标记不活跃
    retire(ptr): 将 ptr 放入当前 epoch 的删除队列
    推进 epoch: 当所有活跃线程都已进入当前 epoch 时:
      1. epoch + 1
      2. 安全删除 (epoch - 2) 的所有节点

    为什么需要 3 个 epoch?
      epoch N:   当前活跃线程
      epoch N-1: 可能有线程刚进入临界区但还未更新 local_epoch
      epoch N-2: 所有线程已离开，安全删除

  vs Hazard Pointer:
    EBR: 更低的读开销（不需要每次写 HP），但内存回收可能延迟
    HP:  有界内存开销，但每次读需要注册 HP
)";

    // 演示
    EpochBasedReclamation ebr;
    struct Node { int data; };
    std::atomic<Node*> shared(new Node{42});

    int tid1 = ebr.register_thread();
    int tid2 = ebr.register_thread();

    std::atomic<int> reads(0);

    std::thread reader([&] {
        for (int i = 0; i < 10000; ++i) {
            EpochBasedReclamation::Guard guard(ebr, tid1);
            Node* p = shared.load(std::memory_order_acquire);
            if (p) {
                volatile int v = p->data;
                (void)v;
                reads.fetch_add(1);
            }
        }
    });

    std::thread writer([&] {
        for (int i = 0; i < 10000; ++i) {
            Node* new_node = new Node{i};
            Node* old = shared.exchange(new_node, std::memory_order_acq_rel);
            if (old) {
                EpochBasedReclamation::Guard guard(ebr, tid2);
                ebr.retire(tid2, old);
                ebr.try_advance();
            }
        }
    });

    reader.join();
    writer.join();

    delete shared.load();
    std::cout << "  EBR 演示: " << reads.load() << " 次安全读取 ✓\n";
}

} // namespace ch18


// =============================================================================
// 第19章：Split Reference Count
// =============================================================================

namespace ch19 {

void demo_split_refcount() {
    print_section("Split Reference Count (分离引用计数)");

    std::cout << R"(
  问题：无锁数据结构中，何时安全删除被替换的节点？
  方案之一：将引用计数分为内部计数和外部计数。

  struct CountedPtr {
      int external_count;  // 外部计数 (在指针中，与指针一起原子操作)
      Node* ptr;
  };

  struct Node {
      atomic<int> internal_count;  // 内部计数
      T data;
      CountedPtr next;
  };

  规则：
    external_count: 有多少线程正在尝试访问这个节点 (通过 CAS 增加)
    internal_count: 负值，表示已完成访问但尚未释放的线程数

    当线程访问完毕:
      internal_count += (external_borrowed - 2)
      如果 internal_count == 0: 没有其他线程在使用，安全删除

    为什么 -2?
      external 最初为 1 (持有者)
      每次借用 +1 (所以初始1不算)
      释放指针时 -1 (持有者)
      剩余归还到 internal

  优点：不需要额外的全局结构 (不像 HP 和 EBR)
  缺点：需要 double-width CAS (128-bit)，实现复杂

  std::shared_ptr 的引用计数就是一种简化版本:
    control_block {
        atomic<int> use_count;    // 强引用
        atomic<int> weak_count;   // 弱引用 + 1
    };
    use_count == 0 → 析构对象
    weak_count == 0 → 释放 control_block
)";

    // shared_ptr 线程安全性演示
    auto sp = std::make_shared<int>(42);
    std::atomic<int> sum(0);
    constexpr int N = 100000;
    constexpr int T = 4;

    std::vector<std::thread> threads;
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&] {
            for (int i = 0; i < N; ++i) {
                auto local = sp;               // 原子增加引用计数
                sum.fetch_add(*local, std::memory_order_relaxed);
            }                                   // 原子减少引用计数
        });
    }
    for (auto& th : threads) th.join();

    std::cout << "  shared_ptr 并发测试: sum = " << sum.load()
              << " (期望 " << (long long)42 * N * T << ") "
              << (sum.load() == (long long)42 * N * T ? "✓" : "✗") << "\n";
    std::cout << "  use_count = " << sp.use_count() << " (应为 1)\n";
}

} // namespace ch19


// =============================================================================
// ██ 五、高级并发数据结构篇
// =============================================================================

// =============================================================================
// 第20章：SeqLock
// =============================================================================

namespace ch20 {

// SeqLock — 写者极少时最优的读写同步
class SeqLock {
    std::atomic<unsigned> seq_{0}; // 偶数=无写者, 奇数=正在写

public:
    unsigned read_begin() const {
        unsigned s;
        do {
            s = seq_.load(std::memory_order_acquire);
        } while (s & 1); // 等待写者完成
        return s;
    }

    bool read_retry(unsigned start_seq) const {
        std::atomic_thread_fence(std::memory_order_acquire);
        return seq_.load(std::memory_order_relaxed) != start_seq;
    }

    void write_lock() {
        unsigned s = seq_.load(std::memory_order_relaxed);
        seq_.store(s + 1, std::memory_order_release); // 标记写入中
    }

    void write_unlock() {
        unsigned s = seq_.load(std::memory_order_relaxed);
        seq_.store(s + 1, std::memory_order_release); // 标记写入完成
    }
};

void demo_seqlock() {
    print_section("SeqLock 序列锁");

    struct TimestampedData {
        double x, y, z;
        uint64_t timestamp;
    };

    SeqLock lock;
    TimestampedData data{1.0, 2.0, 3.0, 0};
    std::atomic<bool> running(true);

    std::cout << R"(
  SeqLock 原理:
    维护一个序列号 (sequence counter):
      偶数 → 无写者
      奇数 → 正在写

    读者:
      1. s = seq.load()          (如果是奇数, 自旋等待)
      2. 拷贝数据
      3. if (seq != s) goto 1    (被写者干扰, 重读)

    写者:
      1. seq += 1  (变成奇数, 标记写入开始)
      2. 修改数据
      3. seq += 1  (变成偶数, 标记写入完成)

    特点:
      - 读者不使用任何锁或原子 RMW → 极快
      - 适合读极多、写极少的场景 (如系统时间 jiffies)
      - 读者可能"读到撕裂数据"，但会检测并重试
      - ⚠️ 不适合包含指针的数据（可能解引用无效指针）
)";

    // 写者线程
    std::thread writer([&] {
        for (uint64_t i = 1; i <= 100000 && running; ++i) {
            lock.write_lock();
            data.x = i * 1.0;
            data.y = i * 2.0;
            data.z = i * 3.0;
            data.timestamp = i;
            lock.write_unlock();
        }
        running = false;
    });

    // 读者线程
    std::atomic<long long> reads(0), retries(0);
    std::thread reader([&] {
        while (running) {
            TimestampedData local;
            unsigned seq;
            do {
                seq = lock.read_begin();
                local = data; // 可能读到不一致的数据
                if (lock.read_retry(seq)) {
                    retries.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }
                break;
            } while (true);

            // 验证一致性
            assert(local.y == local.x * 2.0);
            assert(local.z == local.x * 3.0);
            reads.fetch_add(1, std::memory_order_relaxed);
        }
    });

    writer.join();
    running = false;
    reader.join();

    std::cout << "  SeqLock 测试: " << reads.load() << " 次读取, "
              << retries.load() << " 次重试, 数据始终一致 ✓\n";
}

} // namespace ch20


// =============================================================================
// 第21章：用户态 RCU 模拟
// =============================================================================

namespace ch21 {

// 简化的用户态 RCU
template<typename T>
class SimpleRCU {
    std::atomic<T*> current_;
    std::mutex writer_lock_; // 写者互斥

public:
    explicit SimpleRCU(T* init) : current_(init) {}

    ~SimpleRCU() { delete current_.load(); }

    // 读者：获取当前指针（无锁）
    const T* read() const {
        return current_.load(std::memory_order_acquire);
    }

    // 写者：发布新版本
    template<typename Func>
    void update(Func modifier) {
        std::lock_guard<std::mutex> lock(writer_lock_);

        T* old = current_.load(std::memory_order_relaxed);
        T* new_data = new T(*old); // Copy
        modifier(*new_data);       // Update
        current_.store(new_data, std::memory_order_release); // Publish

        // 等待宽限期（简化：等足够长时间确保读者不再使用旧指针）
        std::this_thread::sleep_for(1ms);
        delete old;
    }
};

void demo_rcu() {
    print_section("用户态 RCU 模拟");

    std::cout << R"(
  RCU (Read-Copy-Update) 用户态模拟:

  读者:
    const T* p = rcu.read();  // 原子 load，极快
    // 使用 p (在临界区内)
    // 不需要释放

  写者:
    rcu.update([](T& data) {
        data.field = new_value;
    });
    // 内部: copy → modify → publish(atomic store) → wait → delete old

  关键特性:
    1. 读者无锁、无等待、无 cache line bouncing
    2. 适合读 >> 写 的场景
    3. 写者需要等待宽限期（读者都离开临界区后才删除旧数据）

  生产级实现: liburcu (user-space RCU library)
)";

    struct Config {
        int max_connections;
        int timeout_ms;
        std::string server_name;
    };

    SimpleRCU<Config> config(new Config{100, 5000, "server-v1"});

    std::atomic<bool> running(true);
    std::atomic<int> reads(0);

    // 多个读者
    std::vector<std::thread> readers;
    for (int i = 0; i < 4; ++i) {
        readers.emplace_back([&] {
            while (running) {
                const Config* cfg = config.read();
                volatile int mc = cfg->max_connections;
                volatile int to = cfg->timeout_ms;
                (void)mc; (void)to;
                reads.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 写者：定期更新配置
    std::thread writer([&] {
        for (int ver = 2; ver <= 5; ++ver) {
            std::this_thread::sleep_for(5ms);
            config.update([ver](Config& cfg) {
                cfg.max_connections = ver * 100;
                cfg.server_name = "server-v" + std::to_string(ver);
            });
        }
        running = false;
    });

    writer.join();
    for (auto& r : readers) r.join();

    std::cout << "  RCU 演示: " << reads.load() << " 次无锁读取, "
              << "最终版本: " << config.read()->server_name << " ✓\n";
}

} // namespace ch21


// =============================================================================
// 第22章：并发跳表
// =============================================================================

namespace ch22 {

// Lock-free Skip List (简化版)
template<typename K, typename V>
class ConcurrentSkipList {
    static constexpr int MAX_LEVEL = 16;

    struct Node {
        K key;
        V value;
        int level;
        std::atomic<Node*> nexts[MAX_LEVEL]; // 每层的下一个

        Node(const K& k, const V& v, int lv) : key(k), value(v), level(lv) {
            for (int i = 0; i < lv; ++i)
                nexts[i].store(nullptr, std::memory_order_relaxed);
        }

        // 头节点构造
        Node() : key{}, value{}, level(MAX_LEVEL) {
            for (int i = 0; i < MAX_LEVEL; ++i)
                nexts[i].store(nullptr, std::memory_order_relaxed);
        }
    };

    Node head_;
    std::atomic<int> max_level_{1};

    // 随机层数 (几何分布)
    int random_level() {
        static thread_local std::mt19937 gen(std::hash<std::thread::id>{}(
            std::this_thread::get_id()));
        int level = 1;
        while (level < MAX_LEVEL && (gen() & 1)) ++level;
        return level;
    }

    // 用 mutex 简化写入（完全无锁版本需要标记删除等复杂机制）
    std::mutex write_mutex_;

public:
    // 查找 (lock-free)
    std::optional<V> find(const K& key) const {
        const Node* curr = &head_;
        for (int i = max_level_.load(std::memory_order_relaxed) - 1; i >= 0; --i) {
            const Node* next = curr->nexts[i].load(std::memory_order_acquire);
            while (next && next->key < key) {
                curr = next;
                next = curr->nexts[i].load(std::memory_order_acquire);
            }
            if (next && next->key == key) {
                return next->value;
            }
        }
        return std::nullopt;
    }

    // 插入 (使用 mutex 保护写入)
    bool insert(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(write_mutex_);

        Node* preds[MAX_LEVEL];
        Node* curr = &head_;

        for (int i = max_level_.load(std::memory_order_relaxed) - 1; i >= 0; --i) {
            Node* next = curr->nexts[i].load(std::memory_order_relaxed);
            while (next && next->key < key) {
                curr = next;
                next = curr->nexts[i].load(std::memory_order_relaxed);
            }
            if (next && next->key == key) return false; // 已存在
            preds[i] = curr;
        }

        int level = random_level();
        // 如果新层数超过当前最大层数
        int cur_max = max_level_.load(std::memory_order_relaxed);
        if (level > cur_max) {
            for (int i = cur_max; i < level; ++i) {
                preds[i] = &head_;
            }
            max_level_.store(level, std::memory_order_relaxed);
        }

        Node* new_node = new Node(key, value, level);

        // 自底向上链接使读者能安全遍历
        for (int i = 0; i < level; ++i) {
            new_node->nexts[i].store(
                preds[i]->nexts[i].load(std::memory_order_relaxed),
                std::memory_order_relaxed);
        }
        // release 确保 new_node 的内容对读者可见
        for (int i = 0; i < level; ++i) {
            preds[i]->nexts[i].store(new_node, std::memory_order_release);
        }

        return true;
    }

    ~ConcurrentSkipList() {
        Node* curr = head_.nexts[0].load();
        while (curr) {
            Node* next = curr->nexts[0].load();
            delete curr;
            curr = next;
        }
    }
};

void demo_skip_list() {
    print_section("并发跳表");

    std::cout << R"(
  跳表 (Skip List):
    level 3: head ──────────────────────────→ [50] ────→ nil
    level 2: head ──────→ [20] ─────────────→ [50] ────→ nil
    level 1: head → [10] → [20] → [30] → [40] → [50] → nil

    查找 O(log n) — 从高层开始，逐层降低
    插入 O(log n) — 随机层数（概率 1/2 升层）

  并发跳表的关键:
    - 查找: 只需 acquire load 遍历，天然 lock-free
    - 插入: 需要处理多层链接的一致性
    - 删除: 最复杂，需要标记节点为"逻辑删除"再物理删除
)";

    ConcurrentSkipList<int, std::string> sl;

    // 并发插入
    constexpr int N = 10000;
    constexpr int T = 4;
    std::atomic<int> success(0);

    std::vector<std::thread> threads;
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&, t] {
            for (int i = t * N; i < (t + 1) * N; ++i) {
                if (sl.insert(i, "v" + std::to_string(i)))
                    success.fetch_add(1);
            }
        });
    }
    for (auto& th : threads) th.join();

    // 并发查找
    std::atomic<int> found(0);
    threads.clear();
    for (int t = 0; t < T; ++t) {
        threads.emplace_back([&, t] {
            for (int i = t * N; i < (t + 1) * N; ++i) {
                if (sl.find(i).has_value())
                    found.fetch_add(1);
            }
        });
    }
    for (auto& th : threads) th.join();

    std::cout << "  并发插入 " << success.load() << " 个键\n";
    std::cout << "  并发查找 " << found.load() << " / " << T * N << " ✓\n";
}

} // namespace ch22


// =============================================================================
// ██ 六、实战与调试篇
// =============================================================================

// =============================================================================
// 第23章：常见 Bug 模式
// =============================================================================

namespace ch23 {

void demo_common_bugs() {
    print_section("常见无锁编程 Bug 模式");

    std::cout << R"(
  ═══ Bug 1: 遗漏 memory_order ═══
  ❌ flag.store(true);  data 可能还没写完
  ✅ 先写 data, 再 flag.store(true, release)
     对应: flag.load(acquire) 后再读 data

  ═══ Bug 2: ABA (参见第4章) ═══
  ❌ CAS 只比较指针值 → 无法检测 A→B→A
  ✅ 使用 tagged pointer / hazard pointer / EBR

  ═══ Bug 3: CAS 后使用过时值 ═══
  ❌ Node* n = head.load();
     use(n->data);         // n 可能已被其他线程释放!
  ✅ 先用 HP 保护 n, 再使用

  ═══ Bug 4: relaxed 导致的不可见 ═══
  ❌ 线程 A: x.store(1, relaxed); y.store(1, relaxed);
     线程 B: if (y.load(relaxed) == 1) assert(x.load(relaxed) == 1);
     → assert 可能失败! relaxed 不保证跨变量的顺序
  ✅ 使用 release/acquire 配对

  ═══ Bug 5: false sharing (伪共享) ═══
  ❌ struct { atomic<int> a; atomic<int> b; };  // 同一 cache line
     线程1 写 a, 线程2 写 b → cache line 反复失效
  ✅ alignas(64) atomic<int> a;
     alignas(64) atomic<int> b;

  ═══ Bug 6: compare_exchange_weak 在非循环中使用 ═══
  ❌ if (x.compare_exchange_weak(expected, desired)) { ... }
     → weak 可能假失败!
  ✅ 非循环用 strong, 循环用 weak

  ═══ Bug 7: memory_order 不匹配 ═══
  ❌ store(release) + load(relaxed)  → 不构成同步
  ✅ store(release) + load(acquire)  → 正确的同步对

  ═══ Bug 8: 数据竞争 (Data Race) ═══
  两个线程无同步地访问同一内存位置，且至少一个是写:
  ❌ int x = 0;  // 非原子
     线程1: x = 1;
     线程2: cout << x;
  → 未定义行为! (不仅仅是读到旧值，而是 UB)
  ✅ atomic<int> x 或使用 mutex
)";
}

} // namespace ch23


// =============================================================================
// 第24章：性能基准测试
// =============================================================================

namespace ch24 {

void demo_benchmark() {
    print_section("性能基准: 锁 vs 无锁 vs 分片");

    constexpr int TOTAL_OPS = 4'000'000;
    constexpr int T = 4;
    constexpr int OPS_PER_THREAD = TOTAL_OPS / T;

    // 1. std::mutex
    {
        std::mutex mtx;
        long long counter = 0;
        Timer t;

        std::vector<std::thread> threads;
        for (int i = 0; i < T; ++i) {
            threads.emplace_back([&] {
                for (int j = 0; j < OPS_PER_THREAD; ++j) {
                    std::lock_guard<std::mutex> lock(mtx);
                    ++counter;
                }
            });
        }
        for (auto& th : threads) th.join();

        std::cout << "  std::mutex:        "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms()
                  << "ms (counter=" << counter << ")\n";
    }

    // 2. std::atomic (seq_cst)
    {
        std::atomic<long long> counter(0);
        Timer t;

        std::vector<std::thread> threads;
        for (int i = 0; i < T; ++i) {
            threads.emplace_back([&] {
                for (int j = 0; j < OPS_PER_THREAD; ++j) {
                    counter.fetch_add(1, std::memory_order_seq_cst);
                }
            });
        }
        for (auto& th : threads) th.join();

        std::cout << "  atomic(seq_cst):   "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms()
                  << "ms (counter=" << counter.load() << ")\n";
    }

    // 3. std::atomic (relaxed)
    {
        std::atomic<long long> counter(0);
        Timer t;

        std::vector<std::thread> threads;
        for (int i = 0; i < T; ++i) {
            threads.emplace_back([&] {
                for (int j = 0; j < OPS_PER_THREAD; ++j) {
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& th : threads) th.join();

        std::cout << "  atomic(relaxed):   "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms()
                  << "ms (counter=" << counter.load() << ")\n";
    }

    // 4. per-thread + 合并 (sharding)
    {
        struct alignas(CACHE_LINE) PaddedCounter { long long val = 0; };
        std::array<PaddedCounter, 16> local_counters{};
        Timer t;

        std::vector<std::thread> threads;
        for (int i = 0; i < T; ++i) {
            threads.emplace_back([&, i] {
                for (int j = 0; j < OPS_PER_THREAD; ++j) {
                    ++local_counters[i].val;
                }
            });
        }
        for (auto& th : threads) th.join();

        long long total = 0;
        for (int i = 0; i < T; ++i) total += local_counters[i].val;

        std::cout << "  per-thread sharded:"
                  << std::fixed << std::setprecision(1) << t.elapsed_ms()
                  << "ms (counter=" << total << ")\n";
    }

    // 5. thread_local + 合并
    {
        std::atomic<long long> total(0);
        Timer t;

        std::vector<std::thread> threads;
        for (int i = 0; i < T; ++i) {
            threads.emplace_back([&] {
                long long local = 0;
                for (int j = 0; j < OPS_PER_THREAD; ++j) {
                    ++local;
                }
                total.fetch_add(local, std::memory_order_relaxed);
            });
        }
        for (auto& th : threads) th.join();

        std::cout << "  thread_local:      "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms()
                  << "ms (counter=" << total.load() << ")\n";
    }

    std::cout << R"(
  结论:
    mutex:           最慢 (上下文切换 + 锁竞争)
    atomic(seq_cst): 较慢 (硬件 MFENCE / LOCK 前缀)
    atomic(relaxed): 快 (仅 LOCK 前缀, 无 barrier)
    sharded:         很快 (无竞争, 各自核心写自己的 cache line)
    thread_local:    最快 (完全无共享, 最后一次合并)

  选择指南:
    能用 thread_local ─→ 首选 (零竞争)
    需要实时共享 ─→ atomic (尽量用 relaxed)
    临界区较长 ─→ mutex
    读多写少 ─→ RCU / SeqLock
    单一计数器 ─→ atomic (考虑分片)
)";
}

} // namespace ch24


// =============================================================================
// 第25章：工具链
// =============================================================================

namespace ch25 {

void demo_tools() {
    print_section("工具链: TSan / ASAN / Relacy");

    std::cout << R"(
  ═══ ThreadSanitizer (TSan) ═══
  检测数据竞争 (Data Race)。

  编译: g++ -fsanitize=thread -g -O1 test11.cpp
  运行: ./a.out

  输出示例:
    WARNING: ThreadSanitizer: data race (pid=12345)
      Write of size 4 at 0x7f... by thread T1:
        #0 func1 test.cpp:42
      Previous read of size 4 at 0x7f... by thread T2:
        #0 func2 test.cpp:58

  优点: 非常准确, 极少误报
  缺点: 程序变慢 5-15x, 内存占用增加 5-10x

  ═══ AddressSanitizer (ASAN) ═══
  检测内存错误 (use-after-free, buffer overflow, ...)

  编译: g++ -fsanitize=address -g -O1 test11.cpp

  检测:
    - heap-use-after-free
    - heap-buffer-overflow
    - stack-buffer-overflow
    - use-after-scope
    - double-free / invalid-free

  ═══ 静态分析工具 ═══
  clang-tidy:
    clang-tidy test11.cpp -- -std=c++17
    检查: bugprone-*, performance-*, concurrency-*

  cppcheck:
    cppcheck --enable=all test11.cpp

  ═══ Relacy (验证工具) ═══
  专门用于测试无锁算法的模型检查器。
  探索所有可能的线程交错，发现 memory ordering bug。
  https://github.com/dvyukov/relacy

  使用:
    #include <relacy/relacy.hpp>
    struct test : rl::test_suite<test, 2> {
        rl::atomic<int> x;
        void thread(unsigned idx) { ... }
    };
    rl::simulate<test>();

  ═══ CDSChecker / GenMC ═══
  形式化验证工具，枚举所有 C++ 内存模型允许的行为。
  适合验证小型无锁算法的正确性。

  ═══ perf + cache 事件 ═══
    perf stat -e cache-misses,cache-references \
              -e L1-dcache-load-misses,L1-dcache-loads ./test11

  ═══ 核心检查清单 ═══
  □ 所有共享可变数据都通过 atomic 或 mutex 访问
  □ release-acquire 配对正确
  □ CAS 失败时正确处理 (expected 被更新)
  □ 没有 ABA 问题 (用 tagged pointer 或 HP)
  □ 无锁结构有正确的内存回收策略
  □ 避免 false sharing (alignas(64))
  □ TSan 零警告
)";
}

} // namespace ch25


// =============================================================================
// main
// =============================================================================

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "================================================================\n";
    std::cout << " 无锁编程、内存顺序、原子操作及并发数据结构 — 完全教程\n";
    std::cout << "================================================================\n";

    // 一、原子操作基础
    print_header("一、原子操作基础篇");
    ch1::demo_atomic_basics();
    ch2::demo_atomic_types();
    ch3::demo_rmw();
    ch4::demo_cas_loop();
    ch4::demo_aba_problem();

    // 二、内存顺序
    print_header("二、内存顺序篇");
    ch5::demo_reordering();
    ch6::demo_memory_orders();
    ch7::demo_acquire_release_deep();
    ch8::demo_release_sequence();
    ch9::demo_consume();
    ch10::demo_volatile_vs_atomic();
    ch11::demo_fences();

    // 三、无锁算法
    print_header("三、无锁算法篇");
    ch12::demo_spinlocks();
    ch13::demo_treiber_stack();
    ch14::demo_ms_queue();
    ch15::demo_lockfree_hashmap();
    ch16::demo_ring_buffers();

    // 四、内存回收
    print_header("四、内存回收篇");
    ch17::demo_hazard_pointer();
    ch18::demo_ebr();
    ch19::demo_split_refcount();

    // 五、高级并发数据结构
    print_header("五、高级并发数据结构篇");
    ch20::demo_seqlock();
    ch21::demo_rcu();
    ch22::demo_skip_list();

    // 六、实战与调试
    print_header("六、实战与调试篇");
    ch23::demo_common_bugs();
    ch24::demo_benchmark();
    ch25::demo_tools();

    std::cout << "\n================================================================\n";
    std::cout << " 演示完成\n";
    std::cout << "================================================================\n";

    return 0;
}
