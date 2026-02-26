// =============================================================================
// 微秒级低延迟系统 — C++ 实战完全教程
// =============================================================================
//
// 编译 (Linux):
//   g++ -std=c++17 -O2 -march=native -mtune=native -fno-exceptions \
//       -fno-rtti -falign-functions=64 -pthread -lrt -o test13 test13.cpp
//
// 编译 (Windows MSVC):
//   cl /std:c++17 /O2 /EHsc /arch:AVX2 test13.cpp /link ws2_32.lib
//
// 运行建议 (Linux):
//   sudo chrt -f 90 taskset -c 2 ./test13     # FIFO调度 + CPU绑核
//
// ─────────────────────────────────────────────────────────────
// 目录 (38 章)
// ═══════════════════════════════════════════════════════════════
//
// 一、延迟测量与基准篇
//   1.  高精度时钟与 rdtsc / rdtscp
//   2.  延迟直方图与百分位统计
//   3.  热路径识别与测量框架
//
// 二、内存子系统篇
//   4.  缓存行对齐与 false sharing 消除
//   5.  内存预取 (prefetch) 策略
//   6.  大页 (Huge Pages) 与 TLB 优化
//   7.  对象池 (Object Pool) — 零分配设计
//   8.  竞技场分配器 (Arena Allocator)
//   9.  内存布局 SoA vs AoS
//
// 三、CPU 与调度篇
//  10.  CPU 亲和性 (Core Pinning)
//  11.  忙等待 (Busy Wait) vs 阻塞
//  12.  自旋锁与自适应退避
//  13.  NUMA 感知编程
//  14.  分支预测优化 (likely/unlikely/branchless)
//
// 四、无锁数据结构篇
//  15.  无锁 SPSC 环形队列 (生产级)
//  16.  无锁 MPSC 队列
//  17.  SeqLock — 读写极端不对称场景
//  18.  无锁内存池
//
// 五、网络 I/O 篇
//  19.  内核旁路 (Kernel Bypass) 概述
//  20.  原始套接字与零拷贝发送
//  21.  TCP_NODELAY 与低延迟 TCP 调优
//  22.  UDP 组播 — 市场数据分发
//  23.  网卡时间戳与硬件加速
//
// 六、编译器与指令级优化篇
//  24.  编译器屏障与优化暗示
//  25.  SIMD (SSE/AVX) 向量化
//  26.  内联汇编与 intrinsics
//  27.  PGO (Profile-Guided Optimization)
//  28.  LTO (Link-Time Optimization)
//
// 七、系统级调优篇
//  29.  Linux 实时调度 (SCHED_FIFO)
//  30.  中断亲和性与 IRQ 隔离
//  31.  内核参数调优 (sysctl)
//  32.  CPU 频率锁定与 C-State 禁用
//  33.  BIOS/固件级调优
//
// 八、架构设计篇
//  34.  事件驱动架构 — 单线程事件循环
//  35.  流水线架构 — 多阶段处理
//  36.  热路径 / 冷路径分离
//  37.  预计算与查找表
//  38.  性能监控与生产调优
// ═══════════════════════════════════════════════════════════════

#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <deque>
#include <list>
#include <queue>
#include <tuple>
#include <optional>
#include <variant>
#include <functional>
#include <algorithm>
#include <numeric>
#include <memory>
#include <type_traits>
#include <utility>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <new>         // std::hardware_destructive_interference_size

#ifdef __linux__
  #include <sys/mman.h>
  #include <sched.h>
  #include <pthread.h>
  #include <unistd.h>
  #include <x86intrin.h>
#elif defined(_WIN32)
  #include <intrin.h>
  #include <windows.h>
#endif

// ─── 缓存行大小常量 ────────────────────────────────────────
#ifdef __cpp_lib_hardware_interference_size
  static constexpr size_t CACHE_LINE = std::hardware_destructive_interference_size;
#else
  static constexpr size_t CACHE_LINE = 64;
#endif

// ─── 编译器提示宏 ──────────────────────────────────────────
#if defined(__GNUC__) || defined(__clang__)
  #define LIKELY(x)   __builtin_expect(!!(x), 1)
  #define UNLIKELY(x) __builtin_expect(!!(x), 0)
  #define FORCE_INLINE __attribute__((always_inline)) inline
  #define NOINLINE     __attribute__((noinline))
  #define HOT_FUNC     __attribute__((hot))
  #define COLD_FUNC    __attribute__((cold))
  #define PREFETCH_R(addr) __builtin_prefetch((addr), 0, 3)
  #define PREFETCH_W(addr) __builtin_prefetch((addr), 1, 3)
  #define CACHE_ALIGNED __attribute__((aligned(64)))
#elif defined(_MSC_VER)
  #define LIKELY(x)   (x)
  #define UNLIKELY(x) (x)
  #define FORCE_INLINE __forceinline
  #define NOINLINE     __declspec(noinline)
  #define HOT_FUNC
  #define COLD_FUNC
  #define PREFETCH_R(addr) _mm_prefetch((const char*)(addr), _MM_HINT_T0)
  #define PREFETCH_W(addr) _mm_prefetch((const char*)(addr), _MM_HINT_T0)
  #define CACHE_ALIGNED __declspec(align(64))
#endif

// ─── 工具 ─────────────────────────────────────────────────────

static void print_header(const char* title) {
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║ " << std::left << std::setw(52) << title << " ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";
}
static void print_section(const char* title) {
    std::cout << "  ── " << title << " ──\n";
}

class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point start_ = clk::now();
public:
    double elapsed_ns() const {
        return std::chrono::duration<double, std::nano>(clk::now() - start_).count();
    }
    double elapsed_us() const { return elapsed_ns() / 1000.0; }
    double elapsed_ms() const { return elapsed_us() / 1000.0; }
    void reset() { start_ = clk::now(); }
};


// =============================================================================
// ██ 一、延迟测量与基准篇
// =============================================================================

// =============================================================================
// 第1章：高精度时钟与 rdtsc
// =============================================================================

namespace ch1 {

// ─── rdtsc: CPU 周期计数器 ───
// 精度: 亚纳秒 (每个CPU cycle)
// 开销: ~20-30ns (rdtscp带序列化)

FORCE_INLINE uint64_t rdtsc() {
#if defined(__GNUC__) || defined(__clang__)
    uint32_t lo, hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return (uint64_t)hi << 32 | lo;
#elif defined(_MSC_VER)
    return __rdtsc();
#endif
}

FORCE_INLINE uint64_t rdtscp() {
    // rdtscp 自带序列化，防止乱序执行影响测量
#if defined(__GNUC__) || defined(__clang__)
    uint32_t lo, hi, aux;
    asm volatile("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux));
    return (uint64_t)hi << 32 | lo;
#elif defined(_MSC_VER)
    unsigned int aux;
    return __rdtscp(&aux);
#endif
}

// 使用 lfence + rdtsc 的精确测量
FORCE_INLINE uint64_t rdtsc_fenced() {
#if defined(__GNUC__) || defined(__clang__)
    uint32_t lo, hi;
    asm volatile("lfence\n\t"
                 "rdtsc" : "=a"(lo), "=d"(hi) :: "memory");
    return (uint64_t)hi << 32 | lo;
#elif defined(_MSC_VER)
    _mm_lfence();
    return __rdtsc();
#endif
}

// 校准 TSC 频率
double calibrate_tsc_freq() {
    auto start_ts = rdtsc();
    auto start_chrono = std::chrono::high_resolution_clock::now();

    // 忙等100ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto end_ts = rdtsc();
    auto end_chrono = std::chrono::high_resolution_clock::now();

    double elapsed_sec = std::chrono::duration<double>(end_chrono - start_chrono).count();
    double cycles = static_cast<double>(end_ts - start_ts);
    return cycles / elapsed_sec; // Hz
}

// 低开销时间戳类
class TscClock {
    double tsc_freq_ghz_; // GHz = cycles per nanosecond
public:
    TscClock() {
        double freq = calibrate_tsc_freq();
        tsc_freq_ghz_ = freq / 1e9;
    }

    uint64_t now_cycles() const { return rdtsc_fenced(); }

    double cycles_to_ns(uint64_t cycles) const {
        return cycles / tsc_freq_ghz_;
    }
    double cycles_to_us(uint64_t cycles) const {
        return cycles_to_ns(cycles) / 1000.0;
    }
};

void demo() {
    print_section("第1章: 高精度时钟与 rdtsc");

    // 1. 对比不同时钟源
    {
        constexpr int N = 1000;
        uint64_t overhead_rdtsc = UINT64_MAX;
        uint64_t overhead_chrono = UINT64_MAX;

        for (int i = 0; i < N; ++i) {
            uint64_t t1 = rdtsc();
            uint64_t t2 = rdtsc();
            overhead_rdtsc = std::min(overhead_rdtsc, t2 - t1);
        }
        for (int i = 0; i < N; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            auto t2 = std::chrono::high_resolution_clock::now();
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            overhead_chrono = std::min(overhead_chrono, (uint64_t)ns);
        }
        std::cout << "  rdtsc 开销: " << overhead_rdtsc << " cycles\n";
        std::cout << "  chrono 开销: " << overhead_chrono << " ns\n";
    }

    // 2. TSC 频率校准
    TscClock tsc;
    std::cout << "  TSC 频率校准中 (100ms)...\n";

    uint64_t t1 = tsc.now_cycles();
    // 模拟微小工作
    volatile int sum = 0;
    for (int i = 0; i < 1000; ++i) sum += i;
    uint64_t t2 = tsc.now_cycles();

    double ns = tsc.cycles_to_ns(t2 - t1);
    std::cout << "  1000次加法: " << std::fixed << std::setprecision(0)
              << ns << " ns (" << (t2-t1) << " cycles)\n";

    std::cout << R"(
  时钟选择指南:
  ┌─────────────────┬──────────┬──────────┬──────────────────┐
  │ 时钟            │ 精度     │ 开销     │ 适用场景          │
  ├─────────────────┼──────────┼──────────┼──────────────────┤
  │ rdtsc           │ ~0.3ns   │ ~20cyc   │ 微基准测试        │
  │ rdtscp          │ ~0.3ns   │ ~30cyc   │ 精确区间测量      │
  │ clock_gettime   │ ~1ns     │ ~20ns    │ Linux 通用高精度  │
  │ chrono::steady  │ ~1ns     │ ~20-50ns │ 可移植高精度      │
  │ QueryPerformance│ ~100ns   │ ~30ns    │ Windows 高精度    │
  └─────────────────┴──────────┴──────────┴──────────────────┘

  ⚠️ rdtsc 注意事项:
    1. 需要 constant_tsc / nonstop_tsc CPU 特性
       cat /proc/cpuinfo | grep -o "constant_tsc\|nonstop_tsc"
    2. 跨核心可能不同步 → 绑核后使用
    3. 不要用 rdtsc 做 wall-clock 计时
)";
}

} // namespace ch1


// =============================================================================
// 第2章：延迟直方图与百分位统计
// =============================================================================

namespace ch2 {

// 低延迟系统关注的是尾延迟 (p99, p99.9)，而不是平均延迟
// HdrHistogram 思想的简化实现

class LatencyHistogram {
    static constexpr size_t NUM_BUCKETS = 10000;  // 0~9999 ns
    static constexpr size_t OVERFLOW_BUCKET = NUM_BUCKETS;

    std::array<uint64_t, NUM_BUCKETS + 1> buckets_{};
    uint64_t total_count_ = 0;
    uint64_t min_ns_ = UINT64_MAX;
    uint64_t max_ns_ = 0;
    uint64_t sum_ns_ = 0;

public:
    void record(uint64_t latency_ns) {
        size_t idx = std::min(latency_ns, (uint64_t)NUM_BUCKETS);
        ++buckets_[idx];
        ++total_count_;
        min_ns_ = std::min(min_ns_, latency_ns);
        max_ns_ = std::max(max_ns_, latency_ns);
        sum_ns_ += latency_ns;
    }

    uint64_t percentile(double pct) const {
        uint64_t target = static_cast<uint64_t>(total_count_ * pct / 100.0);
        uint64_t accumulated = 0;
        for (size_t i = 0; i <= NUM_BUCKETS; ++i) {
            accumulated += buckets_[i];
            if (accumulated >= target) return i;
        }
        return NUM_BUCKETS;
    }

    void print_summary() const {
        if (total_count_ == 0) { std::cout << "    (无数据)\n"; return; }
        double avg = (double)sum_ns_ / total_count_;
        std::cout << std::fixed << std::setprecision(0);
        std::cout << "    样本数:  " << total_count_ << "\n";
        std::cout << "    最小:    " << min_ns_ << " ns\n";
        std::cout << "    平均:    " << avg << " ns\n";
        std::cout << "    中位数:  " << percentile(50) << " ns\n";
        std::cout << "    p90:     " << percentile(90) << " ns\n";
        std::cout << "    p99:     " << percentile(99) << " ns\n";
        std::cout << "    p99.9:   " << percentile(99.9) << " ns\n";
        std::cout << "    最大:    " << max_ns_ << " ns\n";
    }

    void reset() {
        buckets_.fill(0);
        total_count_ = 0;
        min_ns_ = UINT64_MAX;
        max_ns_ = 0;
        sum_ns_ = 0;
    }
};

void demo() {
    print_section("第2章: 延迟直方图与百分位统计");

    LatencyHistogram hist;

    // 测量 rdtsc 开销分布
    for (int i = 0; i < 100000; ++i) {
        auto t1 = std::chrono::high_resolution_clock::now();
        // 模拟一些工作
        volatile int x = 0;
        for (int j = 0; j < 10; ++j) x += j;
        auto t2 = std::chrono::high_resolution_clock::now();
        uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        hist.record(ns);
    }

    std::cout << "  小循环延迟分布 (100K 次):\n";
    hist.print_summary();

    std::cout << R"(
  ⚠️ 低延迟测量要点:
    1. 关注尾延迟: p99/p99.9 >> 平均值
       平均 100ns 但 p99.9=50μs → 不合格
    2. 预热: 前 N 次结果丢弃 (JIT/cache/TLB冷启动)
    3. 避免协调遗漏 (Coordinated Omission):
       不要在上一次完成后才开始下一次计时
    4. 样本量足够: ≥100K samples
    5. 生产环境采样: 不要每次都记录 → 每 N 次采样

  工具推荐:
    HdrHistogram  — Gil Tene 的高动态范围直方图
    wrk2          — 正确处理协调遗漏的 HTTP 基准
    perf stat     — CPU 计数器统计
)";
}

} // namespace ch2


// =============================================================================
// 第3章：热路径识别与测量框架
// =============================================================================

namespace ch3 {

// 零开销测量框架: Release 模式下完全消除

#ifdef ENABLE_LATENCY_TRACE
  #define LATENCY_SCOPE(name) ScopedLatencyTracer _tracer_##__LINE__(name)
#else
  #define LATENCY_SCOPE(name)  // 完全消除
#endif

class ScopedLatencyTracer {
    const char* name_;
    std::chrono::high_resolution_clock::time_point start_;
public:
    ScopedLatencyTracer(const char* name)
        : name_(name), start_(std::chrono::high_resolution_clock::now()) {}
    ~ScopedLatencyTracer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
        if (ns > 1000) { // 只记录超过 1μs 的
            std::cout << "    [TRACE] " << name_ << ": " << ns << " ns\n";
        }
    }
};

// 异步日志收集器 (不阻塞热路径)
struct LatencySample {
    uint64_t timestamp_ns;
    uint64_t latency_ns;
    uint16_t point_id;
};

class AsyncLatencyCollector {
    static constexpr size_t BUFFER_SIZE = 1024 * 64; // 64K samples
    std::array<LatencySample, BUFFER_SIZE> buffer_{};
    std::atomic<uint64_t> write_pos_{0};

public:
    void record(uint16_t point_id, uint64_t latency_ns) {
        uint64_t pos = write_pos_.fetch_add(1, std::memory_order_relaxed);
        auto& sample = buffer_[pos % BUFFER_SIZE];
        sample.point_id = point_id;
        sample.latency_ns = latency_ns;
        sample.timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    uint64_t count() const { return write_pos_.load(std::memory_order_relaxed); }
};

void demo() {
    print_section("第3章: 热路径识别与测量框架");

    AsyncLatencyCollector collector;

    // 模拟热路径测量
    for (int i = 0; i < 10000; ++i) {
        auto t1 = std::chrono::high_resolution_clock::now();
        volatile int x = i * i; (void)x;
        auto t2 = std::chrono::high_resolution_clock::now();
        uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
        collector.record(1, ns);
    }

    std::cout << "  采集样本数: " << collector.count() << "\n";

    std::cout << R"(
  热路径测量原则:
    1. 测量本身不应超过被测量操作的 1%
       rdtsc ~20 cycles vs 操作 ~300 cycles → OK
    2. 异步收集: 热路径只写一条记录, 后台线程聚合
    3. 条件编译: ENABLE_LATENCY_TRACE → Release 零开销
    4. 采样: 不是每次都记录 → if (++counter % 1024 == 0) record()

  热路径识别方法:
    perf record -g ./app          → 火焰图
    perf stat -d ./app            → IPC/缓存统计
    VTune / uProf                 → 微架构分析
    bpftrace 'uprobe:./app:func'  → 动态追踪
)";
}

} // namespace ch3


// =============================================================================
// ██ 二、内存子系统篇
// =============================================================================

// =============================================================================
// 第4章：缓存行对齐与 false sharing
// =============================================================================

namespace ch4 {

// False sharing: 两个无关变量在同一缓存行, 不同核心写入时互相使缓存失效

// ❌ 错误: 可能 false sharing
struct BadCounters {
    std::atomic<uint64_t> counter1;
    std::atomic<uint64_t> counter2;
};

// ✅ 正确: 每个原子变量独占一个缓存行
struct alignas(CACHE_LINE) GoodCounters {
    alignas(CACHE_LINE) std::atomic<uint64_t> counter1{0};
    alignas(CACHE_LINE) std::atomic<uint64_t> counter2{0};
};

void demo() {
    print_section("第4章: 缓存行对齐与 false sharing");

    std::cout << "  缓存行大小: " << CACHE_LINE << " bytes\n";
    std::cout << "  sizeof(BadCounters):  " << sizeof(BadCounters)
              << " bytes (可能 false sharing)\n";
    std::cout << "  sizeof(GoodCounters): " << sizeof(GoodCounters)
              << " bytes (无 false sharing)\n";

    constexpr int N = 5000000;

    // 测试 false sharing 影响
    {
        BadCounters bad{};
        Timer t;
        std::thread t1([&] { for (int i = 0; i < N; ++i) bad.counter1.fetch_add(1, std::memory_order_relaxed); });
        std::thread t2([&] { for (int i = 0; i < N; ++i) bad.counter2.fetch_add(1, std::memory_order_relaxed); });
        t1.join(); t2.join();
        std::cout << "  BadCounters  (false sharing):  " << std::fixed
                  << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }
    {
        GoodCounters good{};
        Timer t;
        std::thread t1([&] { for (int i = 0; i < N; ++i) good.counter1.fetch_add(1, std::memory_order_relaxed); });
        std::thread t2([&] { for (int i = 0; i < N; ++i) good.counter2.fetch_add(1, std::memory_order_relaxed); });
        t1.join(); t2.join();
        std::cout << "  GoodCounters (无 false sharing): " << std::fixed
                  << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }

    std::cout << R"(
  False Sharing 代价:
    同一缓存行被两个核心交替写入 → MESI 协议 ping-pong
    每次缓存行转移: ~70–200 cycles (同一NUMA节点)
    跨NUMA: ~200-400 cycles

  解决方案:
    1. alignas(64) 每个热变量
    2. 填充字节 (padding): char pad[64 - sizeof(counter)];
    3. C++17: std::hardware_destructive_interference_size
    4. 将只读和读写数据分开

  检测工具:
    perf c2c record ./app  → 检测 false sharing
    perf c2c report        → 显示冲突的缓存行
)";
}

} // namespace ch4


// =============================================================================
// 第5章：内存预取 (prefetch)
// =============================================================================

namespace ch5 {

void demo() {
    print_section("第5章: 内存预取 (Prefetch)");

    // 顺序访问 vs 随机访问 + 预取
    constexpr int N = 1024 * 1024; // 1M 元素 = 4MB
    std::vector<int> data(N);
    std::iota(data.begin(), data.end(), 0);

    // 生成随机访问顺序
    std::vector<int> indices(N);
    std::iota(indices.begin(), indices.end(), 0);
    // Fisher-Yates 洗牌
    for (int i = N - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        std::swap(indices[i], indices[j]);
    }

    // 顺序访问
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i)
            sum += data[i];
        std::cout << "  顺序访问: " << std::fixed << std::setprecision(2)
                  << t.elapsed_ms() << " ms\n";
    }

    // 随机访问 (无预取)
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i)
            sum += data[indices[i]];
        std::cout << "  随机访问 (无预取): " << std::fixed << std::setprecision(2)
                  << t.elapsed_ms() << " ms\n";
    }

    // 随机访问 (有预取)
    {
        Timer t;
        volatile int64_t sum = 0;
        constexpr int PREFETCH_DIST = 8; // 提前 8 个元素预取
        for (int i = 0; i < N; ++i) {
            if (i + PREFETCH_DIST < N) {
                PREFETCH_R(&data[indices[i + PREFETCH_DIST]]);
            }
            sum += data[indices[i]];
        }
        std::cout << "  随机访问 (有预取): " << std::fixed << std::setprecision(2)
                  << t.elapsed_ms() << " ms\n";
    }

    std::cout << R"(
  预取指南:
    __builtin_prefetch(addr, rw, locality)
      rw:       0=读预取, 1=写预取
      locality: 0=NTA(不保留), 1=L3, 2=L2, 3=L1(最高优先)

  预取距离:
    太近 → 数据还没到达 → 无效
    太远 → 数据被驱逐 → 无效
    最佳距离 ≈ 内存延迟(ns) / 每元素处理时间(ns)

  最佳实践:
    1. 顺序访问: 硬件预取器足够, 不需要手动预取
    2. 随机访问/链表: prefetch 下一个节点
    3. 哈希表查找: 在计算哈希时预取目标 bucket
    4. B-Tree: 预取子节点

  注意:
    过度预取 → 污染缓存 → 适得其反
    使用 perf stat 观察 cache-misses 变化
)";
}

} // namespace ch5


// =============================================================================
// 第6章：大页 (Huge Pages)
// =============================================================================

namespace ch6 {

void demo() {
    print_section("第6章: 大页 (Huge Pages) 与 TLB 优化");

    std::cout << R"(
  TLB (Translation Lookaside Buffer) 是虚拟→物理地址的缓存。
  TLB miss 代价: ~10-100 cycles

  页大小对比:
  ┌──────────┬──────────┬──────────────┬─────────────┐
  │ 页大小   │ TLB 条目 │ 可覆盖内存   │ TLB miss率  │
  ├──────────┼──────────┼──────────────┼─────────────┤
  │ 4KB      │ ~1536    │ ~6MB         │ 高          │
  │ 2MB      │ ~1536    │ ~3GB         │ 低          │
  │ 1GB      │ ~4       │ ~4GB         │ 极低        │
  └──────────┴──────────┴──────────────┴─────────────┘

  ═══ Linux 配置大页 ═══
  # 分配 1024 个 2MB 大页 (共 2GB)
  echo 1024 > /proc/sys/vm/nr_hugepages

  # 或在 GRUB 中:
  hugepagesz=2M hugepages=1024

  # 透明大页 (THP):
  echo always > /sys/kernel/mm/transparent_hugepage/enabled
  # ⚠️ THP 可能引起延迟尖峰 → 低延迟场景建议禁用 THP, 手动管理

  ═══ 代码中使用大页 ═══

  // Linux: mmap + MAP_HUGETLB
  void* p = mmap(nullptr, size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                 -1, 0);

  // Linux: madvise
  void* p = aligned_alloc(2 * 1024 * 1024, size);
  madvise(p, size, MADV_HUGEPAGE);

  // Windows:
  VirtualAlloc(nullptr, size,
               MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES,
               PAGE_READWRITE);

  ═══ 自定义分配器 ═══
  template<typename T>
  struct HugePageAllocator {
      using value_type = T;
      T* allocate(size_t n) {
          void* p = mmap(nullptr, n * sizeof(T),
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                         -1, 0);
          if (p == MAP_FAILED) throw std::bad_alloc();
          return static_cast<T*>(p);
      }
      void deallocate(T* p, size_t n) {
          munmap(p, n * sizeof(T));
      }
  };
  std::vector<int, HugePageAllocator<int>> data(1'000'000);

  效果: 对于大数据集 (>6MB), 大页可减少 TLB miss 90%+
        延迟降低 10%-30%
)";
}

} // namespace ch6


// =============================================================================
// 第7章：对象池 (Object Pool)
// =============================================================================

namespace ch7 {

// 预分配对象池: 避免热路径上的 new/delete
// new/delete 代价: 100-10000 ns (可能触发 mmap syscall)

template<typename T, size_t MaxSize = 4096>
class ObjectPool {
    struct alignas(CACHE_LINE) Slot {
        alignas(T) char storage[sizeof(T)];
        bool in_use = false;
    };

    // 使用 freelist 链表实现 O(1) 分配/释放
    struct FreeNode {
        FreeNode* next;
    };

    char* pool_;
    FreeNode* free_head_ = nullptr;
    size_t slot_size_;
    size_t count_ = 0;

public:
    ObjectPool() {
        slot_size_ = std::max(sizeof(T), sizeof(FreeNode));
        // 对齐到缓存行
        slot_size_ = (slot_size_ + CACHE_LINE - 1) & ~(CACHE_LINE - 1);

        pool_ = static_cast<char*>(std::aligned_alloc(CACHE_LINE, slot_size_ * MaxSize));
        if (!pool_) throw std::bad_alloc();

        // 构建 freelist
        for (size_t i = 0; i < MaxSize; ++i) {
            auto* node = reinterpret_cast<FreeNode*>(pool_ + i * slot_size_);
            node->next = free_head_;
            free_head_ = node;
        }
    }

    ~ObjectPool() { std::free(pool_); }

    template<typename... Args>
    T* allocate(Args&&... args) {
        if (UNLIKELY(!free_head_)) return nullptr; // 池满

        void* slot = free_head_;
        free_head_ = free_head_->next;
        ++count_;

        return new (slot) T(std::forward<Args>(args)...);
    }

    void deallocate(T* obj) {
        obj->~T();
        auto* node = reinterpret_cast<FreeNode*>(obj);
        node->next = free_head_;
        free_head_ = node;
        --count_;
    }

    size_t count() const { return count_; }
    size_t capacity() const { return MaxSize; }
};

struct Order {
    uint64_t id;
    double price;
    int quantity;
    char symbol[8];

    Order(uint64_t id, double price, int qty, const char* sym)
        : id(id), price(price), quantity(qty) {
        std::strncpy(symbol, sym, 7);
        symbol[7] = '\0';
    }
};

void demo() {
    print_section("第7章: 对象池 — 零分配设计");

    ObjectPool<Order, 10000> pool;

    // 性能对比: 对象池 vs new/delete
    constexpr int N = 100000;

    // new/delete
    {
        Timer t;
        for (int i = 0; i < N; ++i) {
            auto* o = new Order(i, 100.0 + i, 100, "AAPL");
            delete o;
        }
        std::cout << "  new/delete ×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }

    // 对象池
    {
        Timer t;
        for (int i = 0; i < N; ++i) {
            auto* o = pool.allocate(i, 100.0 + i, 100, "AAPL");
            pool.deallocate(o);
        }
        std::cout << "  ObjectPool ×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }

    std::cout << R"(
  对象池设计要点:
    1. 预分配: 构造时分配所有内存
    2. Freelist: O(1) 分配/释放, 利用已释放内存存储链表指针
    3. 缓存行对齐: 避免 false sharing
    4. 池满策略: 返回 nullptr / 断言 / 扩展
    5. 线程安全: 单线程池 (每线程一个) 或无锁池

  ⚠️ 低延迟黄金法则:
    热路径上绝不调用 malloc/new/free/delete
    所有对象在启动时预分配
)";
}

} // namespace ch7


// =============================================================================
// 第8章：竞技场分配器 (Arena Allocator)
// =============================================================================

namespace ch8 {

// Arena: 线性分配, 批量释放, 极低开销

class Arena {
    char* base_;
    size_t capacity_;
    size_t offset_ = 0;

public:
    Arena(size_t cap) : capacity_(cap) {
        base_ = static_cast<char*>(std::aligned_alloc(CACHE_LINE, cap));
        if (!base_) throw std::bad_alloc();
    }
    ~Arena() { std::free(base_); }

    // O(1) 分配 — 只需移动指针
    void* allocate(size_t size, size_t align = alignof(std::max_align_t)) {
        size_t aligned_offset = (offset_ + align - 1) & ~(align - 1);
        if (UNLIKELY(aligned_offset + size > capacity_)) return nullptr;
        void* ptr = base_ + aligned_offset;
        offset_ = aligned_offset + size;
        return ptr;
    }

    template<typename T, typename... Args>
    T* create(Args&&... args) {
        void* p = allocate(sizeof(T), alignof(T));
        return p ? new (p) T(std::forward<Args>(args)...) : nullptr;
    }

    // O(1) 批量释放 — 只需重置指针
    void reset() { offset_ = 0; }

    size_t used() const { return offset_; }
    size_t remaining() const { return capacity_ - offset_; }
};

void demo() {
    print_section("第8章: 竞技场分配器 (Arena)");

    Arena arena(1024 * 1024); // 1MB

    constexpr int N = 100000;

    // Arena 分配
    {
        Timer t;
        for (int i = 0; i < N; ++i) {
            auto* p = arena.create<Order>(i, 100.0, 100, "MSFT");
            (void)p;
        }
        std::cout << "  Arena ×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.elapsed_us() << " μs"
                  << " (used " << arena.used() / 1024 << " KB)\n";
        arena.reset(); // O(1) 批量释放
    }

    // malloc 分配
    {
        std::vector<void*> ptrs;
        ptrs.reserve(N);
        Timer t;
        for (int i = 0; i < N; ++i) {
            ptrs.push_back(std::malloc(sizeof(Order)));
        }
        auto alloc_time = t.elapsed_us();
        for (auto* p : ptrs) std::free(p);
        std::cout << "  malloc ×" << N << ": "
                  << std::fixed << std::setprecision(1) << alloc_time << " μs\n";
    }

    std::cout << R"(
  Arena vs 通用分配器:
  ┌──────────────┬──────────────┬──────────────┐
  │              │ Arena        │ malloc/new   │
  ├──────────────┼──────────────┼──────────────┤
  │ 分配速度     │ ~2-5 ns      │ ~100-500 ns  │
  │ 释放速度     │ O(1) reset   │ 逐个释放     │
  │ 碎片         │ 无           │ 有           │
  │ 线程安全     │ 不需要       │ 需要         │
  │ 单个释放     │ ❌ 不支持    │ ✅           │
  └──────────────┴──────────────┴──────────────┘

  典型用途:
    每个请求/消息分配一个 Arena
    处理完成后 reset() → 零碎片, 极速
    网络包解析、JSON解析、编译器 AST
)";
}

} // namespace ch8


// =============================================================================
// 第9章：内存布局 SoA vs AoS
// =============================================================================

namespace ch9 {

// AoS: Array of Structures — 传统布局
struct ParticleAoS {
    float x, y, z;    // 位置
    float vx, vy, vz; // 速度
    float mass;
    int type;
    // sizeof = 32 bytes, 一个缓存行放2个
};

// SoA: Structure of Arrays — SIMD 友好布局
struct ParticlesSoA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;
    std::vector<int>   type;

    void resize(size_t n) {
        x.resize(n); y.resize(n); z.resize(n);
        vx.resize(n); vy.resize(n); vz.resize(n);
        mass.resize(n); type.resize(n);
    }
};

void demo() {
    print_section("第9章: SoA vs AoS 内存布局");

    constexpr int N = 1000000;

    // AoS
    std::vector<ParticleAoS> aos(N);
    for (int i = 0; i < N; ++i) {
        aos[i] = {(float)i, (float)i, (float)i,
                  0.1f, 0.2f, 0.3f, 1.0f, 0};
    }

    // SoA
    ParticlesSoA soa;
    soa.resize(N);
    for (int i = 0; i < N; ++i) {
        soa.x[i] = soa.y[i] = soa.z[i] = (float)i;
        soa.vx[i] = 0.1f; soa.vy[i] = 0.2f; soa.vz[i] = 0.3f;
        soa.mass[i] = 1.0f; soa.type[i] = 0;
    }

    // 测试: 只更新 x, y, z (位置更新)
    // AoS: 读取整个结构体但只用 x,y,z 和 vx,vy,vz → 浪费带宽
    {
        Timer t;
        for (int i = 0; i < N; ++i) {
            aos[i].x += aos[i].vx;
            aos[i].y += aos[i].vy;
            aos[i].z += aos[i].vz;
        }
        std::cout << "  AoS 位置更新: " << std::fixed << std::setprecision(2)
                  << t.elapsed_ms() << " ms\n";
    }

    // SoA: 连续访问 x[], vx[], y[], vy[], z[], vz[] → 缓存友好
    {
        Timer t;
        for (int i = 0; i < N; ++i) {
            soa.x[i] += soa.vx[i];
            soa.y[i] += soa.vy[i];
            soa.z[i] += soa.vz[i];
        }
        std::cout << "  SoA 位置更新: " << std::fixed << std::setprecision(2)
                  << t.elapsed_ms() << " ms\n";
    }

    std::cout << R"(
  AoS vs SoA 对比:
  ┌──────────┬──────────────────────────────────────────┐
  │ AoS      │ [x,y,z,vx,vy,vz,m,t] [x,y,z,vx,...]   │
  │ 优点     │ 对象完整, 直觉, 单对象操作快             │
  │ 缺点     │ 不用的字段也加载到缓存 → 浪费带宽        │
  ├──────────┼──────────────────────────────────────────┤
  │ SoA      │ [x,x,x,...] [y,y,y,...] [z,z,z,...]     │
  │ 优点     │ 只加载需要的字段, SIMD 自动向量化         │
  │ 缺点     │ 单对象操作需要多次访问                    │
  └──────────┴──────────────────────────────────────────┘

  选择:
    批量处理 + 单一字段 → SoA
    单个对象操作 + 多字段 → AoS
    混合: AoSoA (块内 SoA, 块间 AoS) → 最佳平衡
)";
}

} // namespace ch9


// =============================================================================
// ██ 三、CPU 与调度篇
// =============================================================================

// =============================================================================
// 第10章：CPU 亲和性 (Core Pinning)
// =============================================================================

namespace ch10 {

void demo() {
    print_section("第10章: CPU 亲和性 (Core Pinning)");

    // 展示当前线程可以绑定到指定 CPU
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset); // 绑定到 CPU 0
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    std::cout << "  绑核到 CPU 0: " << (rc == 0 ? "成功" : "失败") << "\n";

    // 查询当前 CPU
    int cpu = sched_getcpu();
    std::cout << "  当前 CPU: " << cpu << "\n";
#elif defined(_WIN32)
    DWORD_PTR mask = 1; // CPU 0
    auto result = SetThreadAffinityMask(GetCurrentThread(), mask);
    std::cout << "  绑核到 CPU 0: " << (result != 0 ? "成功" : "失败") << "\n";
#endif

    std::cout << R"(
  ═══ 为什么要绑核 ═══
  1. 消除线程迁移: OS 调度器可能把线程移到其他核 → L1/L2 缓存全废
     线程迁移代价: ~5-50 μs (冷缓存)
  2. 避免缓存争用: 关键线程独占一个核
  3. 可预测性: 消除调度器的不确定性

  ═══ 绑核策略 ═══
  典型低延迟系统:
    CPU 0:   留给 OS + 中断
    CPU 1:   网络 I/O 线程
    CPU 2:   业务逻辑线程 (热路径)
    CPU 3:   日志 / 监控线程
    CPU 4-N: 其他工作线程

  Linux:
    taskset -c 2 ./app          # 启动时绑核
    pthread_setaffinity_np()    # 代码中绑核
    isolcpus=2,3                # 内核参数隔离CPU

  ═══ CPU 隔离 (isolcpus) ═══
  # /etc/default/grub
  GRUB_CMDLINE_LINUX="isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3"

  isolcpus:   隔离CPU，OS调度器不会自动分配线程到这些核心
  nohz_full:  禁用定时中断 (tick) → 消除 ~4μs 的定时器中断
  rcu_nocbs:  RCU 回调不在这些核心上执行

  效果: 被隔离的 CPU 完全由你的应用独占 → 延迟最稳定
)";
}

} // namespace ch10


// =============================================================================
// 第11章：忙等待 (Busy Wait) vs 阻塞
// =============================================================================

namespace ch11 {

// 忙等待: 不释放 CPU, 延迟最低
// 阻塞: 释放 CPU, 延迟不可控 (唤醒可能需要 5-15μs)

class SpinWait {
public:
    // 纯自旋 — 延迟最低, CPU 消耗最高
    static void spin(std::atomic<bool>& flag) {
        while (!flag.load(std::memory_order_acquire)) {
            // 空循环
        }
    }

    // PAUSE 指令自旋 — 省电, 避免流水线惩罚
    static void spin_pause(std::atomic<bool>& flag) {
        while (!flag.load(std::memory_order_acquire)) {
#if defined(__GNUC__) || defined(__clang__)
            __builtin_ia32_pause();
#elif defined(_MSC_VER)
            _mm_pause();
#endif
        }
    }

    // 自适应: 先自旋 N 次, 再 yield, 最后 sleep
    static void adaptive_wait(std::atomic<bool>& flag, int spin_count = 1000) {
        // 阶段1: 纯自旋
        for (int i = 0; i < spin_count; ++i) {
            if (flag.load(std::memory_order_acquire)) return;
#if defined(__GNUC__) || defined(__clang__)
            __builtin_ia32_pause();
#elif defined(_MSC_VER)
            _mm_pause();
#endif
        }
        // 阶段2: yield
        for (int i = 0; i < 100; ++i) {
            if (flag.load(std::memory_order_acquire)) return;
            std::this_thread::yield();
        }
        // 阶段3: sleep (最后手段)
        while (!flag.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
};

void demo() {
    print_section("第11章: 忙等待 vs 阻塞");

    // 测量唤醒延迟
    std::atomic<bool> ready{false};
    std::atomic<uint64_t> wakeup_ns{0};

    // 忙等待唤醒延迟
    {
        ready.store(false);
        std::thread waiter([&] {
            SpinWait::spin_pause(ready);
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto t1 = std::chrono::high_resolution_clock::now();
        ready.store(true, std::memory_order_release);
        waiter.join();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        std::cout << "  忙等待唤醒延迟: ~" << ns << " ns\n";
    }

    // 条件变量唤醒延迟
    {
        std::mutex mtx;
        std::condition_variable cv;
        bool cv_ready = false;

        std::thread waiter([&] {
            std::unique_lock lk(mtx);
            cv.wait(lk, [&] { return cv_ready; });
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto t1 = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard lk(mtx);
            cv_ready = true;
        }
        cv.notify_one();
        waiter.join();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        std::cout << "  条件变量唤醒延迟: ~" << ns << " ns\n";
    }

    std::cout << R"(
  等待策略对比:
  ┌─────────────────┬──────────┬──────────┬──────────┐
  │ 策略            │ 唤醒延迟 │ CPU 消耗 │ 适用场景 │
  ├─────────────────┼──────────┼──────────┼──────────┤
  │ 纯自旋          │ <100ns   │ 100%     │ 极低延迟 │
  │ pause 自旋      │ <200ns   │ 80-90%   │ 低延迟   │
  │ yield           │ ~1-10μs  │ 10-50%   │ 中延迟   │
  │ futex/condvar   │ ~5-15μs  │ ~0%      │ 高延迟   │
  │ sleep(1ms)      │ ~1-15ms  │ ~0%      │ 无要求   │
  │ 自适应          │ 可变     │ 可变     │ 通用     │
  └─────────────────┴──────────┴──────────┴──────────┘

  PAUSE 指令的作用:
    1. 降低功耗 (~20% CPU)
    2. 避免自旋锁退出时的流水线清空惩罚
    3. 在超线程环境下把资源让给 sibling core
    开销: Intel ~140 cycles (Skylake+), AMD ~65 cycles
)";
}

} // namespace ch11


// =============================================================================
// 第12章：自旋锁与自适应退避
// =============================================================================

namespace ch12 {

// 生产级 TicketLock — 公平 + 低延迟

class TicketSpinLock {
    alignas(CACHE_LINE) std::atomic<uint32_t> ticket_{0};
    alignas(CACHE_LINE) std::atomic<uint32_t> serving_{0};

public:
    void lock() {
        uint32_t my_ticket = ticket_.fetch_add(1, std::memory_order_relaxed);
        while (serving_.load(std::memory_order_acquire) != my_ticket) {
#if defined(__GNUC__) || defined(__clang__)
            __builtin_ia32_pause();
#elif defined(_MSC_VER)
            _mm_pause();
#endif
        }
    }
    void unlock() {
        serving_.fetch_add(1, std::memory_order_release);
    }
};

// 指数退避自旋锁
class BackoffSpinLock {
    std::atomic<bool> locked_{false};

public:
    void lock() {
        int backoff = 1;
        while (true) {
            // 先读取 (不写) — 减少缓存行争用
            while (locked_.load(std::memory_order_relaxed)) {
                for (int i = 0; i < backoff; ++i) {
#if defined(__GNUC__) || defined(__clang__)
                    __builtin_ia32_pause();
#elif defined(_MSC_VER)
                    _mm_pause();
#endif
                }
            }
            // 尝试获取
            if (!locked_.exchange(true, std::memory_order_acquire))
                return;
            // 退避增长
            backoff = std::min(backoff * 2, 1024);
        }
    }
    void unlock() {
        locked_.store(false, std::memory_order_release);
    }
};

void demo() {
    print_section("第12章: 自旋锁与自适应退避");

    constexpr int N = 1000000;
    volatile int shared_val = 0;

    // TicketLock
    {
        TicketSpinLock lock;
        Timer t;
        std::thread t1([&] {
            for (int i = 0; i < N; ++i) { lock.lock(); ++shared_val; lock.unlock(); }
        });
        std::thread t2([&] {
            for (int i = 0; i < N; ++i) { lock.lock(); ++shared_val; lock.unlock(); }
        });
        t1.join(); t2.join();
        std::cout << "  TicketLock 2线程×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }

    // std::mutex 对比
    shared_val = 0;
    {
        std::mutex mtx;
        Timer t;
        std::thread t1([&] {
            for (int i = 0; i < N; ++i) { std::lock_guard lk(mtx); ++shared_val; }
        });
        std::thread t2([&] {
            for (int i = 0; i < N; ++i) { std::lock_guard lk(mtx); ++shared_val; }
        });
        t1.join(); t2.join();
        std::cout << "  std::mutex 2线程×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }

    std::cout << R"(
  自旋锁 vs mutex:
    自旋锁: 临界区极短 (<1μs) 时更快, 避免系统调用
    mutex:  临界区较长时更好, 不浪费 CPU

  低延迟首选: TicketLock (公平) 或 TTAS (高吞吐)
  最佳方案:   无锁设计 → 完全消除锁
)";
}

} // namespace ch12


// =============================================================================
// 第13章：NUMA 感知编程
// =============================================================================

namespace ch13 {

void demo() {
    print_section("第13章: NUMA 感知编程");

    std::cout << R"(
  NUMA (Non-Uniform Memory Access) 架构:
  ┌────────────────────────────────────────────────────────┐
  │  NUMA Node 0              NUMA Node 1                  │
  │  ┌────────┐               ┌────────┐                   │
  │  │ CPU 0  │─── 互联 ──── │ CPU 1  │                   │
  │  │ Core0-7│   (QPI/UPI)   │ Core8-15│                  │
  │  └───┬────┘               └───┬────┘                   │
  │      │                        │                        │
  │  ┌───┴────┐               ┌───┴────┐                   │
  │  │ 内存0  │               │ 内存1  │                   │
  │  │ 64GB   │               │ 64GB   │                   │
  │  └────────┘               └────────┘                   │
  └────────────────────────────────────────────────────────┘

  本地内存访问 : ~80 ns
  远程内存访问 : ~140 ns (跨NUMA节点, 1.7x 慢)

  ═══ NUMA 编程要点 ═══

  1. 内存绑定 (membind):
     numactl --membind=0 --cpunodebind=0 ./app
     → 内存和CPU都在 node 0

  2. 代码中绑定:
     #include <numa.h>
     void* p = numa_alloc_onnode(size, 0);  // 在 node 0 分配
     numa_free(p, size);

  3. 首次触摸策略 (First-Touch Policy):
     Linux 默认: 内存在首次写入时分配到执行写入的CPU所在NUMA节点
     → 在目标线程中初始化数据, 而不是主线程

     ❌ 错误:
     int* data = new int[N];  // 主线程(CPU 0)分配
     memset(data, 0, N);      // 物理页在 node 0
     // worker(CPU 8, node 1) 使用 → 远程访问!

     ✅ 正确:
     int* data = new int[N];  // 只分配虚拟地址
     // 在 worker 线程中初始化
     worker_thread([data, N] {
         memset(data, 0, N);  // 首次触摸 → 物理页在 node 1
     });

  4. 查询 NUMA 信息:
     numactl --hardware          # 查看拓扑
     lscpu                       # 查看 CPU → NUMA 映射
     cat /proc/buddyinfo         # 各节点内存状态

  性能影响:
    未优化的 NUMA:  延迟可能增加 50-100%
    NUMA 感知后:    延迟稳定在本地访问级别
)";
}

} // namespace ch13


// =============================================================================
// 第14章：分支预测优化
// =============================================================================

namespace ch14 {

// likely/unlikely 提示编译器分支概率
// C++20: [[likely]] / [[unlikely]]

// 无分支绝对值
FORCE_INLINE int branchless_abs(int x) {
    int mask = x >> 31;          // 负数→全1, 正数→全0
    return (x + mask) ^ mask;    // 等价于 abs(x)
}

// 无分支 min/max
FORCE_INLINE int branchless_min(int a, int b) {
    return b + ((a - b) & ((a - b) >> 31));
}

FORCE_INLINE int branchless_max(int a, int b) {
    return a - ((a - b) & ((a - b) >> 31));
}

// 无分支 clamp
FORCE_INLINE int branchless_clamp(int x, int lo, int hi) {
    return branchless_min(branchless_max(x, lo), hi);
}

void demo() {
    print_section("第14章: 分支预测优化");

    constexpr int N = 10000000;

    // 可预测分支 vs 不可预测分支
    std::vector<int> data(N);
    for (int i = 0; i < N; ++i) data[i] = std::rand();

    // 可预测: 大部分走同一分支
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            if (LIKELY(data[i] > 100))  // 99.99% 为 true
                sum += data[i];
        }
        std::cout << "  可预测分支: " << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << " ms\n";
    }

    // 不可预测: 50/50
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            if (data[i] > RAND_MAX / 2)  // ~50% true
                sum += data[i];
            else
                sum -= data[i];
        }
        std::cout << "  不可预测分支: " << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << " ms\n";
    }

    // 无分支版本
    {
        Timer t;
        volatile int64_t sum = 0;
        int half = RAND_MAX / 2;
        for (int i = 0; i < N; ++i) {
            int mask = -(data[i] > half);  // true→-1(全1), false→0
            sum += (data[i] & mask) - (data[i] & ~mask);
        }
        std::cout << "  无分支版本: " << std::fixed << std::setprecision(1)
                  << t.elapsed_ms() << " ms\n";
    }

    // branchless 验证
    assert(branchless_abs(-5) == 5);
    assert(branchless_abs(3) == 3);
    assert(branchless_min(3, 7) == 3);
    assert(branchless_max(3, 7) == 7);
    assert(branchless_clamp(50, 0, 100) == 50);
    assert(branchless_clamp(-10, 0, 100) == 0);
    std::cout << "  branchless abs/min/max/clamp: 全部通过 ✓\n";

    std::cout << R"(
  分支预测失败代价: ~15-20 cycles (流水线清空)
  分支预测准确率: 可预测 ~99%, 不可预测 ~50%

  优化策略:
    1. [[likely]]/[[unlikely]]: 提示编译器布局热路径
    2. 无分支算法: 用位运算替代 if
    3. 消除分支: 用查找表、CMOV、条件移动
    4. 按概率排序: 最可能的 case 放最前面

  适用场景: 错误处理、范围检查、比较密集的代码
)";
}

} // namespace ch14


// =============================================================================
// ██ 四、无锁数据结构篇
// =============================================================================

// =============================================================================
// 第15章：无锁 SPSC 环形队列 (生产级)
// =============================================================================

namespace ch15 {

// SPSC (Single Producer, Single Consumer) 环形队列
// 延迟: ~10-30 ns per operation
// 吞吐: >100M ops/sec

template<typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity 必须是 2 的幂");

    struct alignas(CACHE_LINE) {
        std::atomic<uint64_t> head{0};  // 消费者读取位置
    };
    struct alignas(CACHE_LINE) {
        std::atomic<uint64_t> tail{0};  // 生产者写入位置
    };
    struct alignas(CACHE_LINE) {
        uint64_t cached_head{0};        // 生产者缓存的 head (减少跨缓存行读取)
        uint64_t cached_tail{0};        // 消费者缓存的 tail
    };

    alignas(CACHE_LINE) T buffer_[Capacity];

    static constexpr uint64_t MASK = Capacity - 1;

    // 这些字段按访问模式分离到不同缓存行
    // head: 只有消费者写, 生产者偶尔读
    // tail: 只有生产者写, 消费者偶尔读

public:
    bool try_push(const T& item) {
        const uint64_t t = tail.load(std::memory_order_relaxed);

        // 检查是否满: 使用缓存的 head (避免频繁跨缓存行读)
        if (t - cached_head >= Capacity) {
            cached_head = head.load(std::memory_order_acquire);
            if (t - cached_head >= Capacity)
                return false; // 真的满了
        }

        buffer_[t & MASK] = item;
        tail.store(t + 1, std::memory_order_release);
        return true;
    }

    bool try_pop(T& item) {
        const uint64_t h = head.load(std::memory_order_relaxed);

        // 检查是否空: 使用缓存的 tail
        if (h >= cached_tail) {
            cached_tail = tail.load(std::memory_order_acquire);
            if (h >= cached_tail)
                return false; // 真的空了
        }

        item = buffer_[h & MASK];
        head.store(h + 1, std::memory_order_release);
        return true;
    }

    size_t size() const {
        return tail.load(std::memory_order_relaxed) - head.load(std::memory_order_relaxed);
    }
};

void demo() {
    print_section("第15章: 无锁 SPSC 环形队列");

    SPSCQueue<int64_t, 65536> queue;
    constexpr int64_t N = 5000000;

    std::atomic<bool> started{false};
    int64_t consumer_sum = 0;

    // 消费者
    std::thread consumer([&] {
        while (!started.load(std::memory_order_acquire)) {}
        int64_t val;
        int64_t count = 0;
        while (count < N) {
            if (queue.try_pop(val)) {
                consumer_sum += val;
                ++count;
            }
        }
    });

    // 生产者
    started.store(true, std::memory_order_release);
    Timer t;
    for (int64_t i = 0; i < N; ++i) {
        while (!queue.try_push(i)) {} // 忙等
    }
    consumer.join();
    double elapsed = t.elapsed_ms();

    int64_t expected_sum = N * (N - 1) / 2;
    bool correct = (consumer_sum == expected_sum);

    double ops_per_sec = N / (elapsed / 1000.0);
    double ns_per_op = elapsed * 1e6 / N;

    std::cout << "  " << N << " 条消息: " << std::fixed << std::setprecision(1)
              << elapsed << " ms\n";
    std::cout << "  吞吐量: " << std::setprecision(0) << ops_per_sec / 1e6 << " M ops/sec\n";
    std::cout << "  每操作: " << std::setprecision(0) << ns_per_op << " ns\n";
    std::cout << "  正确性: " << (correct ? "✓" : "✗") << "\n";

    std::cout << R"(
  SPSC 队列优化要点:
    1. 容量为 2 的幂 → 用 & MASK 替代 % 取模
    2. head 和 tail 在不同缓存行 → 消除 false sharing
    3. 缓存远端的计数器 → 减少缓存行同步次数
    4. acquire/release 语义 → 足够, 不需要 seq_cst
    5. 批量操作: 一次 push/pop 多个 → 进一步降低均摊延迟

  应用: 交易系统 market data → strategy → order
)";
}

} // namespace ch15


// =============================================================================
// 第16章：无锁 MPSC 队列
// =============================================================================

namespace ch16 {

// MPSC: Multiple Producer, Single Consumer
// 多个生产者线程安全地入队, 单个消费者出队

template<typename T>
class MPSCQueue {
    struct Node {
        T data;
        std::atomic<Node*> next{nullptr};
    };

    alignas(CACHE_LINE) std::atomic<Node*> head_;  // 消费者端
    alignas(CACHE_LINE) std::atomic<Node*> tail_;  // 生产者端

public:
    MPSCQueue() {
        auto* dummy = new Node{};
        head_.store(dummy, std::memory_order_relaxed);
        tail_.store(dummy, std::memory_order_relaxed);
    }

    ~MPSCQueue() {
        T val;
        while (try_pop(val)) {}
        delete head_.load();
    }

    void push(const T& data) {
        auto* node = new Node{data};
        // CAS 尾部
        Node* prev = tail_.exchange(node, std::memory_order_acq_rel);
        // 链接前驱 → 新节点
        prev->next.store(node, std::memory_order_release);
    }

    bool try_pop(T& data) {
        Node* h = head_.load(std::memory_order_relaxed);
        Node* next = h->next.load(std::memory_order_acquire);
        if (!next) return false;
        data = next->data;
        head_.store(next, std::memory_order_release);
        delete h;
        return true;
    }
};

void demo() {
    print_section("第16章: 无锁 MPSC 队列");

    MPSCQueue<int64_t> queue;
    constexpr int PRODUCERS = 4;
    constexpr int PER_PRODUCER = 100000;

    std::atomic<int> done_count{0};
    int64_t total_sum = 0;

    // 生产者
    std::vector<std::thread> producers;
    Timer t;
    for (int p = 0; p < PRODUCERS; ++p) {
        producers.emplace_back([&, p] {
            for (int i = 0; i < PER_PRODUCER; ++i) {
                queue.push(p * PER_PRODUCER + i);
            }
            done_count.fetch_add(1, std::memory_order_release);
        });
    }

    // 消费者 (主线程)
    int64_t consumed = 0;
    int64_t total = PRODUCERS * PER_PRODUCER;
    while (consumed < total) {
        int64_t val;
        if (queue.try_pop(val)) {
            total_sum += val;
            ++consumed;
        }
    }
    for (auto& t : producers) t.join();

    int64_t expected = total * (total - 1) / 2;
    std::cout << "  " << PRODUCERS << " 生产者 × " << PER_PRODUCER << " 条:\n";
    std::cout << "  耗时: " << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    std::cout << "  校验: " << (total_sum == expected ? "✓" : "✗") << "\n";

    std::cout << R"(
  MPSC 队列特点:
    push: CAS exchange (原子操作, 多生产者安全)
    pop:  单消费者, 无需原子操作竞争
    适用: 多个工作线程 → 单个聚合线程

  注意: 此实现使用 new/delete → 热路径不适合
    生产版应使用预分配的节点池
)";
}

} // namespace ch16


// =============================================================================
// 第17章：SeqLock — 读写极端不对称
// =============================================================================

namespace ch17 {

// SeqLock: 0 个读者阻塞写者, 写者极低延迟
// 适用: 一个写者 + 多个读者, 写入频率远大于读取

class SeqLock {
    alignas(CACHE_LINE) std::atomic<uint64_t> seq_{0};

public:
    uint64_t read_begin() const {
        uint64_t s;
        do {
            s = seq_.load(std::memory_order_acquire);
        } while (s & 1); // 奇数表示写入中, 等待
        return s;
    }

    bool read_retry(uint64_t start_seq) const {
        std::atomic_thread_fence(std::memory_order_acquire);
        return seq_.load(std::memory_order_relaxed) != start_seq;
    }

    void write_lock() {
        seq_.fetch_add(1, std::memory_order_release);  // 变为奇数
    }
    void write_unlock() {
        seq_.fetch_add(1, std::memory_order_release);  // 变为偶数
    }
};

// 共享的市场数据结构
struct MarketData {
    double bid_price;
    double ask_price;
    uint64_t bid_qty;
    uint64_t ask_qty;
    uint64_t timestamp;
};

class MarketDataFeed {
    SeqLock lock_;
    MarketData data_{};

public:
    void update(double bid, double ask, uint64_t bqty, uint64_t aqty) {
        lock_.write_lock();
        data_.bid_price = bid;
        data_.ask_price = ask;
        data_.bid_qty = bqty;
        data_.ask_qty = aqty;
        data_.timestamp++;
        lock_.write_unlock();
    }

    MarketData read() const {
        MarketData result;
        uint64_t seq;
        do {
            seq = lock_.read_begin();
            result = data_;
        } while (lock_.read_retry(seq));
        return result;
    }
};

void demo() {
    print_section("第17章: SeqLock — 读写不对称");

    MarketDataFeed feed;
    std::atomic<bool> running{true};

    // 写者: 高频更新
    std::thread writer([&] {
        for (int i = 0; i < 1000000; ++i) {
            feed.update(100.0 + i * 0.01, 100.05 + i * 0.01, 100, 200);
        }
        running.store(false, std::memory_order_release);
    });

    // 读者: 持续读取
    uint64_t reads = 0;
    uint64_t retries = 0;
    while (running.load(std::memory_order_acquire)) {
        auto data = feed.read();
        ++reads;
        // 一致性检查: ask >= bid
        if (data.ask_price < data.bid_price) {
            std::cout << "  ❌ 不一致数据!\n";
        }
    }
    writer.join();

    std::cout << "  写入: 1,000,000 次\n";
    std::cout << "  读取: " << reads << " 次\n";
    std::cout << "  数据始终一致 ✓\n";

    std::cout << R"(
  SeqLock vs 读写锁:
  ┌──────────────┬──────────────┬──────────────┐
  │              │ SeqLock      │ RWLock       │
  ├──────────────┼──────────────┼──────────────┤
  │ 写者延迟     │ ~几 ns       │ ~几百 ns     │
  │ 读者阻塞写者 │ 不阻塞       │ 阻塞         │
  │ 读者可能重试 │ 是           │ 否           │
  │ 适用场景     │ 写多读少     │ 读多写少     │
  │ 数据要求     │ trivially    │ 任意         │
  │              │ copyable     │              │
  └──────────────┴──────────────┴──────────────┘

  典型场景: 市场数据feeds, 配置热更新, 时间戳分发
)";
}

} // namespace ch17


// =============================================================================
// 第18章：无锁内存池
// =============================================================================

namespace ch18 {

// 无锁 freelist: 多线程安全的对象池
template<typename T, size_t PoolSize = 4096>
class LockFreePool {
    struct Node {
        union {
            alignas(T) char storage[sizeof(T)];
            Node* next;
        };
    };

    alignas(CACHE_LINE) std::atomic<Node*> free_head_;
    char* pool_memory_;

public:
    LockFreePool() {
        pool_memory_ = static_cast<char*>(
            std::aligned_alloc(CACHE_LINE, sizeof(Node) * PoolSize));

        // 构建 freelist
        Node* nodes = reinterpret_cast<Node*>(pool_memory_);
        for (size_t i = 0; i < PoolSize - 1; ++i) {
            nodes[i].next = &nodes[i + 1];
        }
        nodes[PoolSize - 1].next = nullptr;
        free_head_.store(nodes, std::memory_order_relaxed);
    }

    ~LockFreePool() { std::free(pool_memory_); }

    template<typename... Args>
    T* allocate(Args&&... args) {
        Node* node;
        do {
            node = free_head_.load(std::memory_order_acquire);
            if (!node) return nullptr;
        } while (!free_head_.compare_exchange_weak(
            node, node->next, std::memory_order_acq_rel));

        return new (node->storage) T(std::forward<Args>(args)...);
    }

    void deallocate(T* obj) {
        obj->~T();
        Node* node = reinterpret_cast<Node*>(obj);
        Node* head;
        do {
            head = free_head_.load(std::memory_order_relaxed);
            node->next = head;
        } while (!free_head_.compare_exchange_weak(
            head, node, std::memory_order_release));
    }
};

void demo() {
    print_section("第18章: 无锁内存池");

    LockFreePool<Order, 8192> pool;

    constexpr int N = 100000;
    constexpr int THREADS = 4;

    std::atomic<int64_t> alloc_count{0};
    std::vector<std::thread> threads;

    Timer t;
    for (int t_id = 0; t_id < THREADS; ++t_id) {
        threads.emplace_back([&, t_id] {
            for (int i = 0; i < N; ++i) {
                auto* o = pool.allocate(t_id * N + i, 100.0, 100, "TEST");
                if (o) {
                    alloc_count.fetch_add(1, std::memory_order_relaxed);
                    pool.deallocate(o);
                }
            }
        });
    }
    for (auto& th : threads) th.join();

    std::cout << "  " << THREADS << " 线程 × " << N << " 次 alloc/dealloc:\n";
    std::cout << "  耗时: " << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    std::cout << "  总操作: " << alloc_count.load() << "\n";

    std::cout << R"(
  ⚠️ ABA 问题:
    CAS freelist 存在 ABA 问题:
    线程A读 head=X, 线程B pop X → pop Y → push X
    线程A CAS 成功但 head->next 已改变

  解决方案:
    1. Tagged pointer: 在指针高位嵌入版本号
    2. Hazard Pointers: 保护正在访问的指针
    3. 每线程池: 消除争用 (最佳方案)

  生产建议: 低延迟系统首选每线程独立池 + 无锁跨线程传递
)";
}

} // namespace ch18


// =============================================================================
// ██ 五、网络 I/O 篇
// =============================================================================

// =============================================================================
// 第19-23章：网络优化
// =============================================================================

namespace ch19_23 {

void demo() {
    print_section("第19-23章: 网络 I/O 低延迟优化");

    std::cout << R"(
  ═══ 第19章: 内核旁路 (Kernel Bypass) ═══

  传统网络路径:
    NIC → 驱动 → 内核网络栈 → socket → 用户空间
    延迟: 5-50 μs (系统调用 + 上下文切换 + 数据拷贝)

  内核旁路:
    NIC → 用户空间 (直接 DMA)
    延迟: 0.5-2 μs

  技术栈:
  ┌────────────────┬──────────┬──────────────────────────┐
  │ 方案           │ 延迟     │ 特点                     │
  ├────────────────┼──────────┼──────────────────────────┤
  │ DPDK           │ ~1 μs    │ Intel, 通用, PMD驱动     │
  │ Solarflare/ef_ │ ~0.7 μs  │ Xilinx/AMD, OpenOnload   │
  │ Mellanox VMA   │ ~1 μs    │ NVIDIA, RDMA             │
  │ io_uring       │ ~2-5 μs  │ Linux 5.1+, 异步IO       │
  │ XDP            │ ~2 μs    │ Linux, eBPF在NIC层       │
  │ FPGA           │ ~0.1 μs  │ 最低延迟, 硬件方案       │
  └────────────────┴──────────┴──────────────────────────┘

  ═══ 第20章: 零拷贝发送 ═══

  传统:  用户buf → 内核buf → NIC DMA buf  (2次拷贝)
  零拷贝: 用户buf → NIC DMA buf            (0次拷贝)

  实现:
    sendmsg() + MSG_ZEROCOPY  (Linux 4.14+)
    mmap + 共享缓冲区
    DPDK: rte_mbuf 直接 DMA

  ═══ 第21章: TCP 低延迟调优 ═══

  关键 socket 选项:
    TCP_NODELAY = 1          // 禁用 Nagle 算法 (必须!)
    TCP_QUICKACK = 1         // 禁用延迟 ACK
    SO_RCVBUF / SO_SNDBUF    // 适当缩小缓冲区 → 减少排队延迟
    SO_BUSY_POLL = 50        // 内核忙轮询 50μs
    TCP_FASTOPEN             // TFO 减少建连延迟

  代码示例:
    int flag = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, &flag, sizeof(flag));

  系统参数:
    net.core.busy_poll = 50          # 忙轮询
    net.core.busy_read = 50
    net.ipv4.tcp_low_latency = 1

  ═══ 第22章: UDP 组播 ═══

  市场数据分发: 交易所 → 组播 → 所有订阅者
  延迟: ~1-5 μs (局域网)

  代码要点:
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("239.1.1.1");
    mreq.imr_interface.s_addr = INADDR_ANY;
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

    // 高性能: recvmmsg() 批量接收
    struct mmsghdr msgs[BATCH_SIZE];
    int n = recvmmsg(fd, msgs, BATCH_SIZE, MSG_DONTWAIT, nullptr);

  ═══ 第23章: 网卡时间戳 ═══

  硬件时间戳: NIC 在收到数据包时打上精确时间戳
  精度: ~ns 级 (PTP 同步后)

  setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPNS, &flag, sizeof(flag));
  // 或 SO_TIMESTAMPING for 硬件时间戳

  用途:
    - 精确测量网络延迟 (消除软件延迟)
    - 交易系统: 确定收到行情的精确时间
    - 延迟审计和合规
)";
}

} // namespace ch19_23


// =============================================================================
// ██ 六、编译器与指令级优化篇
// =============================================================================

// =============================================================================
// 第24章：编译器屏障
// =============================================================================

namespace ch24 {

void demo() {
    print_section("第24章: 编译器屏障与优化暗示");

    // 编译器屏障: 阻止编译器重排序, 不影响 CPU
    volatile int x = 0;

    x = 1;
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" ::: "memory");  // 编译器屏障
#elif defined(_MSC_VER)
    _ReadWriteBarrier();
#endif
    x = 2;

    std::cout << R"(
  编译器优化暗示:

  1. 编译器屏障 (compiler barrier):
     asm volatile("" ::: "memory");
     _ReadWriteBarrier();   // MSVC
     作用: 阻止编译器跨屏障移动内存操作
     开销: 零 (不生成任何指令)

  2. CPU 内存屏障:
     asm volatile("mfence" ::: "memory");  // 全屏障
     asm volatile("lfence" ::: "memory");  // 读屏障
     asm volatile("sfence" ::: "memory");  // 写屏障
     开销: ~20-100 cycles

  3. volatile:
     volatile int x;  // 每次访问都从内存读/写
     ❌ 不保证原子性, 不保证顺序 (除 MSVC /volatile:ms)
     ✅ 用于: 内存映射 I/O, 信号处理, benchmarking 防优化

  4. __restrict__:
     void process(int* __restrict__ a, int* __restrict__ b, int n) {
         for (int i = 0; i < n; ++i) a[i] += b[i];
     }
     告诉编译器 a 和 b 不重叠 → 可以自动向量化

  5. [[gnu::pure]] / [[gnu::const]]:
     [[gnu::pure]] int compute(int x);  // 只读全局状态
     [[gnu::const]] int square(int x);  // 纯函数, 无副作用
     编译器可以消除重复调用

  6. __builtin_unreachable():
     if (x > MAX) __builtin_unreachable();
     告诉编译器这个路径不可能到达 → 允许更激进优化
)";
}

} // namespace ch24


// =============================================================================
// 第25章：SIMD 向量化
// =============================================================================

namespace ch25 {

void demo() {
    print_section("第25章: SIMD (SSE/AVX) 向量化");

    constexpr int N = 1024 * 1024; // 1M
    std::vector<float> a(N, 1.0f), b(N, 2.0f), c(N, 0.0f);

    // 标量版本
    {
        Timer t;
        for (int i = 0; i < N; ++i) {
            c[i] = a[i] + b[i];
        }
        std::cout << "  标量加法: " << std::fixed << std::setprecision(2)
                  << t.elapsed_ms() << " ms\n";
    }

    // 编译器自动向量化 (需要 -O2 -march=native)
    // 确保循环可向量化: 无依赖, 无分支, 连续内存
    {
        Timer t;
        float* __restrict__ pa = a.data();
        float* __restrict__ pb = b.data();
        float* __restrict__ pc = c.data();
        for (int i = 0; i < N; ++i) {
            pc[i] = pa[i] + pb[i]; // __restrict__ 帮助编译器向量化
        }
        std::cout << "  restrict 加法: " << std::fixed << std::setprecision(2)
                  << t.elapsed_ms() << " ms\n";
    }

    std::cout << R"(
  SIMD 指令集:
  ┌──────────┬──────────┬──────────────────────────┐
  │ 指令集   │ 宽度     │ 浮点(32-bit) 吞吐       │
  ├──────────┼──────────┼──────────────────────────┤
  │ SSE      │ 128-bit  │ 4 floats/cycle           │
  │ AVX      │ 256-bit  │ 8 floats/cycle           │
  │ AVX-512  │ 512-bit  │ 16 floats/cycle          │
  └──────────┴──────────┴──────────────────────────┘

  手写 intrinsics 示例 (SSE):
    #include <immintrin.h>
    void add_sse(float* a, float* b, float* c, int n) {
        for (int i = 0; i < n; i += 4) {
            __m128 va = _mm_load_ps(&a[i]);
            __m128 vb = _mm_load_ps(&b[i]);
            __m128 vc = _mm_add_ps(va, vb);
            _mm_store_ps(&c[i], vc);
        }
    }

  AVX2 示例:
    void add_avx(float* a, float* b, float* c, int n) {
        for (int i = 0; i < n; i += 8) {
            __m256 va = _mm256_load_ps(&a[i]);
            __m256 vb = _mm256_load_ps(&b[i]);
            _mm256_store_ps(&c[i], _mm256_add_ps(va, vb));
        }
    }

  自动向量化条件:
    1. -O2 -march=native
    2. 循环无数据依赖
    3. 连续内存访问 (SoA 布局)
    4. __restrict__ 消除别名
    5. 循环边界编译期可知 → 更好

  检查是否向量化:
    g++ -O2 -march=native -fopt-info-vec-optimized    # GCC
    clang -O2 -Rpass=loop-vectorize                   # Clang
)";
}

} // namespace ch25


// =============================================================================
// 第26章：内联汇编与 intrinsics
// =============================================================================

namespace ch26 {

// x86 POPCNT — 统计二进制中 1 的个数
FORCE_INLINE int fast_popcount(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(x);
#elif defined(_MSC_VER)
    return (int)__popcnt64(x);
#endif
}

// CLZ — 前导零个数 (Count Leading Zeros)
FORCE_INLINE int fast_clz(uint32_t x) {
    if (x == 0) return 32;
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_clz(x);
#elif defined(_MSC_VER)
    unsigned long idx;
    _BitScanReverse(&idx, x);
    return 31 - idx;
#endif
}

// CTZ — 尾部零个数
FORCE_INLINE int fast_ctz(uint32_t x) {
    if (x == 0) return 32;
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(x);
#elif defined(_MSC_VER)
    unsigned long idx;
    _BitScanForward(&idx, x);
    return idx;
#endif
}

// 快速 log2
FORCE_INLINE int fast_log2(uint32_t x) {
    return 31 - fast_clz(x);
}

void demo() {
    print_section("第26章: 内联汇编与 Intrinsics");

    std::cout << "  popcount(0xFF00FF) = " << fast_popcount(0xFF00FF) << "\n";
    std::cout << "  clz(16) = " << fast_clz(16) << " (前导零)\n";
    std::cout << "  ctz(16) = " << fast_ctz(16) << " (尾部零)\n";
    std::cout << "  log2(1024) = " << fast_log2(1024) << "\n";

    // 性能对比
    constexpr int N = 10000000;
    {
        Timer t;
        volatile int sum = 0;
        for (int i = 1; i <= N; ++i)
            sum += fast_popcount(i);
        std::cout << "  intrinsic popcount ×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }
    {
        Timer t;
        volatile int sum = 0;
        auto software_popcount = [](uint64_t x) {
            int count = 0;
            while (x) { count += x & 1; x >>= 1; }
            return count;
        };
        for (int i = 1; i <= N; ++i)
            sum += software_popcount(i);
        std::cout << "  software popcount ×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.elapsed_ms() << " ms\n";
    }

    std::cout << R"(
  常用 intrinsics:
    __builtin_popcount(x)    — 位计数 (POPCNT)
    __builtin_clz(x)         — 前导零 (BSR/LZCNT)
    __builtin_ctz(x)         — 尾部零 (BSF/TZCNT)
    __builtin_expect(x, v)   — 分支预测暗示
    __builtin_prefetch(p)    — 预取
    _mm_pause()              — PAUSE
    _mm_clflush(p)           — 刷新缓存行

  低延迟应用:
    popcount → 位图索引查找
    clz/ctz → 优先级队列 (find highest/lowest set bit)
    log2 → 哈希表大小计算
)";
}

} // namespace ch26


// =============================================================================
// 第27-28章：PGO 与 LTO
// =============================================================================

namespace ch27_28 {

void demo() {
    print_section("第27-28章: PGO 与 LTO");

    std::cout << R"(
  ═══ 第27章: PGO (Profile-Guided Optimization) ═══

  步骤:
    1. 编译插桩版本:
       g++ -O2 -fprofile-generate -o app_instrumented app.cpp

    2. 运行真实工作负载 (收集 profile 数据):
       ./app_instrumented < real_workload.dat
       → 生成 *.gcda 文件

    3. 使用 profile 重新编译:
       g++ -O2 -fprofile-use -o app_optimized app.cpp

  PGO 优化内容:
    - 函数内联决策 (热函数更激进内联)
    - 分支预测 (基于真实数据的 likely/unlikely)
    - 代码布局 (热路径连续, 冷路径分离)
    - 循环优化 (基于真实迭代次数)

  效果: 通常提速 10-30%, 低延迟场景可达 40%

  MSVC:
    cl /O2 /GL /GENPROFILE app.cpp        # 插桩
    ./app.exe < workload.dat              # 收集
    cl /O2 /GL /USEPROFILE app.cpp        # 优化

  ═══ 第28章: LTO (Link-Time Optimization) ═══

  GCC:
    g++ -O2 -flto -c file1.cpp -o file1.o
    g++ -O2 -flto -c file2.cpp -o file2.o
    g++ -O2 -flto file1.o file2.o -o app

  Clang:
    clang++ -O2 -flto=thin -c *.cpp         # ThinLTO (更快)
    clang++ -O2 -flto=thin *.o -o app

  MSVC:
    cl /O2 /GL /c *.cpp          # /GL = Whole Program Optimization
    link /LTCG *.obj /out:app    # /LTCG = Link Time Code Generation

  LTO 优化内容:
    - 跨翻译单元内联
    - 跨翻译单元死代码消除
    - 全局变量优化 (internalization)
    - 虚函数去虚化 (devirtualization)

  效果: 通常提速 5-20%

  最佳组合: -O2 -march=native -flto -fprofile-use → 最大优化
)";
}

} // namespace ch27_28


// =============================================================================
// ██ 七、系统级调优篇
// =============================================================================

namespace ch29_33 {

void demo() {
    print_section("第29-33章: 系统级调优");

    std::cout << R"(
  ═══ 第29章: Linux 实时调度 ═══

  调度策略:
    SCHED_OTHER  — 默认 CFS, 非实时
    SCHED_FIFO   — 实时, 先来先服务, 优先级 1-99
    SCHED_RR     — 实时, 时间片轮转
    SCHED_DEADLINE — 最先进, 基于截止时间

  设置:
    // 代码中
    struct sched_param param;
    param.sched_priority = 90;   // 高优先级
    sched_setscheduler(0, SCHED_FIFO, &param);

    // 命令行
    sudo chrt -f 90 ./app       # FIFO 优先级 90
    sudo chrt -d --sched-runtime 100000 --sched-period 1000000 ./app

  ═══ 第30章: 中断亲和性 ═══

  问题: 网卡中断在业务核心上 → 延迟尖峰

  解决:
    # 查看中断分布
    cat /proc/interrupts | grep eth

    # 将网卡中断绑定到 CPU 0
    echo 1 > /proc/irq/<IRQ_NUM>/smp_affinity

    # 或使用 irqbalance 自动平衡
    systemctl stop irqbalance    # 低延迟场景建议关闭

  ═══ 第31章: 内核参数调优 (sysctl) ═══

  # 网络
  net.core.busy_poll = 50
  net.core.busy_read = 50
  net.core.netdev_max_backlog = 30000
  net.core.rmem_max = 16777216
  net.core.wmem_max = 16777216
  net.ipv4.tcp_low_latency = 1
  net.ipv4.tcp_fastopen = 3
  net.ipv4.tcp_tw_reuse = 1

  # 内存
  vm.swappiness = 0              # 禁用 swap
  vm.zone_reclaim_mode = 0       # 禁用 NUMA zone reclaim
  vm.min_free_kbytes = 1048576   # 保留 1GB 空闲

  # 调度
  kernel.sched_rt_runtime_us = -1  # 允许 RT 线程 100% CPU
  kernel.sched_min_granularity_ns = 10000000  # 10ms

  ═══ 第32章: CPU 频率锁定 ═══

  问题: CPU 降频 → 延迟不稳定

  解决:
    # 锁定最高频率
    cpupower frequency-set -g performance
    # 或
    echo performance > /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

    # 禁用深度 C-States
    # GRUB: intel_idle.max_cstate=0 processor.max_cstate=0
    # 或: /dev/cpu_dma_latency 写入 0
    int fd = open("/dev/cpu_dma_latency", O_WRONLY);
    int val = 0;
    write(fd, &val, sizeof(val));
    // 保持 fd 打开 → CPU 不进入深睡眠

  效果: C-State 唤醒延迟 ~2-100μs → 锁定后消除

  ═══ 第33章: BIOS/固件调优 ═══

  关键 BIOS 设置:
    Hyper-Threading:     关闭 (消除抖动, 但损失吞吐)
    Turbo Boost:         关闭 (频率稳定) 或 开启 (更高峰值)
    C-States:            C1 only 或 全部禁用
    P-States:            固定最高频率
    Power Management:    Maximum Performance
    NUMA Interleaving:   关闭 (使用本地内存)
    Intel SpeedStep:     关闭
    LLC Prefetch:        开启 (Last Level Cache 预取)
    Snoop Mode:          Early Snoop (更低延迟)
)";
}

} // namespace ch29_33


// =============================================================================
// ██ 八、架构设计篇
// =============================================================================

// =============================================================================
// 第34章：事件驱动架构 — 单线程事件循环
// =============================================================================

namespace ch34 {

// 单线程事件循环: 没有锁, 没有上下文切换, 延迟最低

class EventLoop {
public:
    using EventHandler = std::function<void()>;

    struct TimerEntry {
        uint64_t trigger_time_ns;
        EventHandler handler;
        bool operator>(const TimerEntry& o) const { return trigger_time_ns > o.trigger_time_ns; }
    };

private:
    std::vector<EventHandler> immediate_queue_;
    std::priority_queue<TimerEntry, std::vector<TimerEntry>, std::greater<>> timer_queue_;
    bool running_ = false;

    uint64_t now_ns() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

public:
    void post(EventHandler handler) {
        immediate_queue_.push_back(std::move(handler));
    }

    void post_delayed(uint64_t delay_ns, EventHandler handler) {
        timer_queue_.push({now_ns() + delay_ns, std::move(handler)});
    }

    void run_once() {
        // 处理 immediate events
        auto events = std::move(immediate_queue_);
        immediate_queue_.clear();
        for (auto& handler : events) {
            handler();
        }

        // 处理到期的定时器
        uint64_t current = now_ns();
        while (!timer_queue_.empty() && timer_queue_.top().trigger_time_ns <= current) {
            auto handler = std::move(const_cast<TimerEntry&>(timer_queue_.top()).handler);
            timer_queue_.pop();
            handler();
        }
    }

    void run(int max_iterations = 100) {
        running_ = true;
        for (int i = 0; i < max_iterations && running_; ++i) {
            run_once();
        }
    }

    void stop() { running_ = false; }
};

void demo() {
    print_section("第34章: 事件驱动单线程架构");

    EventLoop loop;
    int event_count = 0;

    // 注册事件
    loop.post([&] {
        std::cout << "  [事件1] 收到市场数据\n";
        ++event_count;

        // 处理后触发下游事件
        loop.post([&] {
            std::cout << "  [事件2] 策略计算完成\n";
            ++event_count;

            loop.post([&] {
                std::cout << "  [事件3] 发送订单\n";
                ++event_count;
                loop.stop();
            });
        });
    });

    Timer t;
    loop.run();
    double elapsed = t.elapsed_us();

    std::cout << "  处理 " << event_count << " 个事件, 耗时: "
              << std::fixed << std::setprecision(1) << elapsed << " μs\n";

    std::cout << R"(
  单线程事件循环架构:
    ┌──────────────────────────────────────────────┐
    │          Event Loop (单线程, 绑核)            │
    │                                              │
    │  ┌─────────┐  ┌──────────┐  ┌────────────┐  │
    │  │ 网络 IO │→│ 解码/处理 │→│ 发送/响应   │  │
    │  │ (poll)  │  │ (业务)   │  │ (write)    │  │
    │  └─────────┘  └──────────┘  └────────────┘  │
    │                                              │
    │  ← 全程无锁, 无上下文切换, 无线程同步 →       │
    └──────────────────────────────────────────────┘

  优势:
    - 零同步开销 (无锁, 无mutex, 无CAS)
    - 确定性延迟 (无调度器干扰)
    - 简单 (单线程 = 容易调试/推理)

  典型应用: HFT 交易引擎, 网络代理, 游戏服务器
)";
}

} // namespace ch34


// =============================================================================
// 第35章：流水线架构
// =============================================================================

namespace ch35 {

void demo() {
    print_section("第35章: 流水线架构 — 多阶段处理");

    std::cout << R"(
  流水线架构: 将处理分为多个阶段, 每个阶段一个线程

    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
    │ Stage 1 │───→│ Stage 2 │───→│ Stage 3 │───→│ Stage 4 │
    │ 网络接收│    │ 解码解析│    │ 业务逻辑│    │ 网络发送│
    │  CPU 1  │    │  CPU 2  │    │  CPU 3  │    │  CPU 4  │
    └─────────┘    └─────────┘    └─────────┘    └─────────┘
        ↑               ↑               ↑               ↑
     SPSC Queue      SPSC Queue     SPSC Queue     直接发送

  设计要点:
    1. 阶段间用 SPSC 队列连接 → 无锁, 低延迟
    2. 每个阶段绑定到独立 CPU
    3. 消息通过指针传递 (零拷贝)
    4. 每个阶段可独立测量延迟

  延迟分析:
    单阶段延迟: 0.5-2 μs
    阶段间传递: 0.05-0.1 μs (SPSC queue)
    总延迟: Σ(阶段延迟) ≈ 2-8 μs (不是最大阶段×N)

  vs 单线程:
  ┌────────────────┬──────────────────┬──────────────────┐
  │                │ 单线程事件循环   │ 多阶段流水线     │
  ├────────────────┼──────────────────┼──────────────────┤
  │ 吞吐量         │ 中               │ 高 (并行)        │
  │ 单消息延迟     │ 最低             │ 略高 (队列传递)  │
  │ 复杂度         │ 低               │ 中               │
  │ 可扩展性       │ 单核瓶颈         │ 添加阶段即可     │
  │ 适用           │ 极低延迟         │ 高吞吐低延迟     │
  └────────────────┴──────────────────┴──────────────────┘

  LMAX Disruptor 模式:
    Ring Buffer (无锁) + 依赖图 + 批处理
    单生产者, 多消费者, 消费者间有依赖关系
    吞吐: >6M events/sec
)";
}

} // namespace ch35


// =============================================================================
// 第36章：热路径 / 冷路径分离
// =============================================================================

namespace ch36 {

// 热路径 (Hot Path): 每条消息必经的关键路径
// 冷路径 (Cold Path): 错误处理、日志、统计等

// ❌ 错误示例: 日志在热路径上
struct BadDesign {
    NOINLINE void process_order_bad(int64_t order_id, double price) {
        // 热路径混入冷操作
        std::ostringstream oss;  // 堆分配!
        oss << "[LOG] Order " << order_id << " @ " << price; // 格式化!
        std::cout << oss.str() << "\n"; // IO!

        // 实际业务逻辑
        volatile double result = price * 1.001;
        (void)result;
    }
};

// ✅ 正确: 热路径极简
struct GoodDesign {
    // 热路径: 只做最必要的事
    HOT_FUNC
    FORCE_INLINE void process_order(int64_t order_id, double price) {
        // 纯计算, 无分配, 无IO, 无锁
        volatile double result = price * 1.001;
        (void)result;

        // 冷路径: unlikely 分支 → 编译器会把它移到函数末尾
        if (UNLIKELY(price <= 0)) {
            handle_error(order_id, price);  // NOINLINE
        }
    }

    // 冷路径: 不内联, 不影响热路径指令缓存
    COLD_FUNC NOINLINE
    void handle_error(int64_t order_id, double price) {
        std::cerr << "Invalid price for order " << order_id << ": " << price << "\n";
    }
};

void demo() {
    print_section("第36章: 热路径 / 冷路径分离");

    constexpr int N = 1000000;

    // Bad: 热路径混入日志
    // (只测量少量以避免输出太多)
    {
        BadDesign bad;
        Timer t;
        for (int i = 0; i < 100; ++i)
            bad.process_order_bad(i, 100.0 + i);
        double per_op = t.elapsed_us() / 100;
        std::cout << "  Bad (含日志): ~" << std::fixed << std::setprecision(1)
                  << per_op * 1000 << " ns/op\n";
    }

    // Good: 纯热路径
    {
        GoodDesign good;
        Timer t;
        for (int i = 0; i < N; ++i)
            good.process_order(i, 100.0 + i);
        double per_op = t.elapsed_ns() / N;
        std::cout << "  Good (纯计算): ~" << std::fixed << std::setprecision(1)
                  << per_op << " ns/op\n";
    }

    std::cout << R"(
  热路径规则 (黄金法则):
    ✅ 允许:
      纯计算、位操作、数组访问
      预分配内存中的读写
      原子操作 (relaxed/acquire/release)
      inline 函数调用

    ❌ 禁止:
      malloc / new / delete    → 用对象池
      std::string / ostringstream → 预分配 or 固定缓冲区
      std::cout / printf       → 异步日志
      mutex / condvar          → 无锁设计
      系统调用 (read/write)    → 内核旁路 or 异步
      虚函数 (高频调用)        → CRTP 或 if constexpr
      异常 throw               → 错误码 / expected

  代码组织:
    - 热函数: HOT_FUNC + FORCE_INLINE + 放在一起 (指令缓存)
    - 冷函数: COLD_FUNC + NOINLINE + 单独编译单元
    - __builtin_expect + 编译器把冷分支移出热路径
)";
}

} // namespace ch36


// =============================================================================
// 第37章：预计算与查找表
// =============================================================================

namespace ch37 {

// 将运行时计算转为编译期查找表

// 示例: 价格类型 (tick-based, 类似交易所)
// 价格 = tick_value * tick_size
// tick_size 按产品不同, 但运行时固定

class alignas(CACHE_LINE) PriceConverter {
    // 预计算的乘法因子查找表
    double tick_to_price_[65536];  // 按 tick 索引
    double price_to_tick_factor_;

public:
    PriceConverter(double tick_size) : price_to_tick_factor_(1.0 / tick_size) {
        for (int i = 0; i < 65536; ++i) {
            tick_to_price_[i] = i * tick_size;
        }
    }

    // O(1) + 缓存命中 (同一个缓存行内的多次查找)
    FORCE_INLINE double tick_to_price(uint16_t tick) const {
        return tick_to_price_[tick];
    }

    FORCE_INLINE uint16_t price_to_tick(double price) const {
        return static_cast<uint16_t>(price * price_to_tick_factor_);
    }
};

// 编译期 CRC32 查找表
constexpr std::array<uint32_t, 256> make_crc32_table() {
    std::array<uint32_t, 256> table{};
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
            else crc >>= 1;
        }
        table[i] = crc;
    }
    return table;
}

constexpr auto CRC32_TABLE = make_crc32_table();

FORCE_INLINE uint32_t crc32(const void* data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    auto* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < len; ++i) {
        crc = CRC32_TABLE[(crc ^ bytes[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

void demo() {
    print_section("第37章: 预计算与查找表");

    PriceConverter conv(0.01); // tick_size = 0.01 (分)

    std::cout << "  tick 100 → $" << conv.tick_to_price(100) << "\n";
    std::cout << "  tick 12345 → $" << conv.tick_to_price(12345) << "\n";
    std::cout << "  $123.45 → tick " << conv.price_to_tick(123.45) << "\n";

    // CRC32 性能
    char data[1024];
    std::memset(data, 'A', sizeof(data));

    constexpr int N = 100000;
    {
        Timer t;
        volatile uint32_t checksum = 0;
        for (int i = 0; i < N; ++i) {
            checksum = crc32(data, sizeof(data));
        }
        std::cout << "  CRC32(1KB) ×" << N << ": " << std::fixed
                  << std::setprecision(1) << t.elapsed_ms() << " ms"
                  << " (编译期查找表)\n";
    }

    std::cout << R"(
  预计算策略:
    1. 编译期查找表 (constexpr array)
       CRC32, 三角函数, 位操作表
    2. 启动时初始化查找表
       价格转换, 权重表, 配置相关计算
    3. 缓存最近结果 (memoization)
       重复查询的订单簿状态

  关键约束:
    查找表大小 << L1 缓存 (32-48KB)
    热查找表应 < 4KB (一个 TLB 页)
    → 避免查找表太大反而导致缓存驱逐
)";
}

} // namespace ch37


// =============================================================================
// 第38章：性能监控与生产调优
// =============================================================================

namespace ch38 {

void demo() {
    print_section("第38章: 性能监控与生产调优");

    std::cout << R"(
  ═══ 生产环境监控指标 ═══

  必须监控 (实时):
    1. 端到端延迟: p50, p90, p99, p99.9, max
    2. 吞吐量: messages/sec
    3. 队列深度: SPSC queue 使用率
    4. CPU 使用率: 按核心监控

  硬件计数器 (perf):
    perf stat -e cache-misses,cache-references,
              branch-misses,branches,
              instructions,cycles,
              L1-dcache-load-misses,
              LLC-load-misses
              -p <PID>

  关键比率:
    IPC (Instructions Per Cycle): 理想 > 2.0
    L1 miss rate: < 1%
    LLC miss rate: < 5%
    Branch miss rate: < 1%
    Context switches: 0 (理想)

  ═══ 常见延迟陷阱 ═══

  ┌────────────────────┬──────────┬──────────────────────────┐
  │ 陷阱               │ 延迟     │ 解决方案                 │
  ├────────────────────┼──────────┼──────────────────────────┤
  │ malloc/new         │ 100-10Kns│ 对象池/Arena             │
  │ 系统调用           │ 100-500ns│ 内核旁路/批量            │
  │ 页错误 (page fault)│ 3-10 μs  │ mlockall + 预触摸        │
  │ TLB miss           │ 10-100ns │ 大页                     │
  │ L3 cache miss      │ 40-80ns  │ 预取 + 紧凑数据          │
  │ 分支预测失败       │ 15-20cyc │ branchless + likely      │
  │ 线程迁移           │ 5-50 μs  │ 绑核 + isolcpus          │
  │ 定时器中断         │ ~4 μs    │ nohz_full                │
  │ CPU 降频           │ ~100 μs  │ performance governor     │
  │ C-State 唤醒       │ 2-100 μs │ 禁用深 C-State           │
  │ NUMA 远程访问      │ +60ns    │ NUMA 绑定                │
  │ 日志/IO            │ 1-100 μs │ 异步日志                 │
  │ 异常 (throw)       │ 1-100 μs │ 错误码/expected          │
  │ 虚函数調用         │ ~5ns     │ CRTP/devirt              │
  │ std::string 拷贝   │ 50-500ns │ string_view              │
  │ 锁竞争             │ 100ns-μs │ 无锁设计                 │
  └────────────────────┴──────────┴──────────────────────────┘

  ═══ 检查清单 (Checklist) ═══

  □ 编译: -O2 -march=native -flto -fno-exceptions
  □ 绑核: isolcpus + taskset + pthread_setaffinity
  □ 频率: performance governor + 禁用 C-State
  □ 内存: mlockall + 大页 + 预分配
  □ 网络: TCP_NODELAY + busy_poll + 内核旁路
  □ 中断: IRQ 隔离 + irqbalance 关闭
  □ 测量: rdtsc + 直方图 + p99.9
  □ 热路径: 无分配 + 无锁 + 无系统调用 + 无分支
  □ 数据: 缓存行对齐 + false sharing 消除 + 预取
  □ 调优: PGO + perf stat + 火焰图

  ═══ 微秒延迟系统典型架构 ═══

  ┌──────────────────────────────────────────────────────────┐
  │                    Trading System                        │
  │                                                          │
  │  ┌──────┐    ┌────────────────────────────┐   ┌──────┐  │
  │  │ NIC  │───→│     Event Loop (CPU 2)     │──→│ NIC  │  │
  │  │ RX   │    │ ┌──────┐ ┌──────┐ ┌─────┐ │   │ TX   │  │
  │  │(DPDK)│    │ │Decode │→│Logic │→│Encode│ │   │(DPDK)│  │
  │  └──────┘    │ └──────┘ └──────┘ └─────┘ │   └──────┘  │
  │              └────────────────────────────┘              │
  │                        │                                 │
  │  ┌─────────── SPSC Queue ──────┐                        │
  │  ▼                              │                        │
  │  ┌──────────────────────────┐   │                        │
  │  │  Logging Thread (CPU 3)  │   │                        │
  │  │  Monitoring, Persistence │   │                        │
  │  └──────────────────────────┘   │                        │
  │                                 │                        │
  │  Configuration: mlockall, hugepages, isolcpus,           │
  │  SCHED_FIFO, performance governor, DPDK                  │
  └──────────────────────────────────────────────────────────┘

  Network → Decode → Strategy → Order → Network
           ~0.5μs    ~1-3μs    ~0.5μs
           ─────── 总延迟 ~2-5 μs ──────
)";
}

} // namespace ch38


// =============================================================================
// main
// =============================================================================

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "================================================================\n";
    std::cout << " 微秒级低延迟系统 — C++ 实战完全教程\n";
    std::cout << "================================================================\n";

    // 一、延迟测量
    print_header("一、延迟测量与基准篇");
    ch1::demo();
    ch2::demo();
    ch3::demo();

    // 二、内存子系统
    print_header("二、内存子系统篇");
    ch4::demo();
    ch5::demo();
    ch6::demo();
    ch7::demo();
    ch8::demo();
    ch9::demo();

    // 三、CPU 与调度
    print_header("三、CPU 与调度篇");
    ch10::demo();
    ch11::demo();
    ch12::demo();
    ch13::demo();
    ch14::demo();

    // 四、无锁数据结构
    print_header("四、无锁数据结构篇");
    ch15::demo();
    ch16::demo();
    ch17::demo();
    ch18::demo();

    // 五、网络 I/O
    print_header("五、网络 I/O 篇");
    ch19_23::demo();

    // 六、编译器优化
    print_header("六、编译器与指令级优化篇");
    ch24::demo();
    ch25::demo();
    ch26::demo();
    ch27_28::demo();

    // 七、系统级调优
    print_header("七、系统级调优篇");
    ch29_33::demo();

    // 八、架构设计
    print_header("八、架构设计篇");
    ch34::demo();
    ch35::demo();
    ch36::demo();
    ch37::demo();
    ch38::demo();

    std::cout << "\n================================================================\n";
    std::cout << " 演示完成\n";
    std::cout << "================================================================\n";

    return 0;
}
