# 微秒低延迟系统 — C++ 实战完全教程

> **定位**：本教程对应 `test13.cpp`（3048 行，38 章），为超低延迟 C++ 系统开发提供从**测量方法论 → 内存/CPU/无锁优化 → 网络 I/O → 编译器/系统级调优 → 架构设计**的完整实战路径。与 `test14.cpp`（CPU 微架构）聚焦硬件特性不同，本文件聚焦**工程实践**——如何在真实系统中持续压低端到端延迟（在合适场景中可逼近或进入 10μs 量级）。

---

## 编译与运行

```bash
# ═══ Linux (推荐) ═══
g++ -std=c++17 -O2 -march=native -mtune=native -fno-exceptions -fno-rtti \
    -falign-functions=64 -pthread -lrt -o test13 test13.cpp

# 最佳运行方式 (FIFO 实时调度 + CPU 绑核)
sudo chrt -f 90 taskset -c 2 ./test13

# ═══ Windows MSVC ═══
cl /std:c++17 /O2 /EHsc /arch:AVX2 test13.cpp /link ws2_32.lib
test13.exe
```

> **编译选项说明**：`-fno-exceptions -fno-rtti` 消除异常和 RTTI 开销；`-march=native` 启用当前 CPU 全部指令集（SSE/AVX/POPCNT 等）；`-falign-functions=64` 将函数对齐到缓存行边界以改善 I-cache。

---

## 目录总览（38 章 × 8 篇）

| 篇                       | 章 | 主题               | 核心技术                                      |
| ------------------------ | -- | ------------------ | --------------------------------------------- |
| **一、延迟测量**   | 1  | 高精度时钟与 rdtsc | rdtsc/rdtscp, TSC 校准, 时钟选择指南          |
|                          | 2  | 延迟直方图         | p50/p90/p99/p99.9, HdrHistogram, 协调遗漏     |
|                          | 3  | 热路径识别         | 零开销宏, 异步采集, perf/bpftrace             |
| **二、内存子系统** | 4  | 缓存行对齐         | alignas(64), false sharing, perf c2c, MESI    |
|                          | 5  | 内存预取           | prefetch(addr, rw, locality), 预取距离公式    |
|                          | 6  | 大页               | 2MB/1GB, mmap MAP_HUGETLB, TLB miss 消除      |
|                          | 7  | 对象池             | Freelist O(1), 零分配热路径, 预分配           |
|                          | 8  | 竞技场分配器       | 线性分配 ~2ns, O(1) reset, 零碎片             |
|                          | 9  | SoA vs AoS         | 内存布局基准, SIMD 友好, 带宽效率             |
| **三、CPU 调度**   | 10 | CPU 亲和性         | isolcpus, nohz_full, rcu_nocbs                |
|                          | 11 | 忙等待 vs 阻塞     | spin/pause/yield/condvar 唤醒延迟实测         |
|                          | 12 | 自旋锁             | TicketLock, 指数退避, TTAS, vs mutex          |
|                          | 13 | NUMA 感知          | 首次触摸策略, membind, 远程 +60ns             |
|                          | 14 | 分支预测           | likely/unlikely, branchless abs/min/max/clamp |
| **四、无锁结构**   | 15 | SPSC 队列          | 缓存行分离, cached head/tail, >100M ops/sec   |
|                          | 16 | MPSC 队列          | exchange-based push, 多生产者无锁入队         |
|                          | 17 | SeqLock            | 写者零阻塞, 读者乐观重试, MarketData          |
|                          | 18 | 无锁内存池         | CAS freelist, ABA 问题, Tagged pointer        |
| **五、网络 I/O**   | 19 | 内核旁路           | DPDK ~1μs, Solarflare ~0.7μs, FPGA ~0.1μs  |
|                          | 20 | 零拷贝             | MSG_ZEROCOPY, DMA 直传, rte_mbuf              |
|                          | 21 | TCP 低延迟         | TCP_NODELAY, QUICKACK, SO_BUSY_POLL           |
|                          | 22 | UDP 组播           | 市场数据分发, recvmmsg 批量接收               |
|                          | 23 | 网卡时间戳         | 硬件 timestamp, PTP 同步, 延迟审计            |
| **六、编译器优化** | 24 | 编译器屏障         | asm volatile, volatile, restrict, pure/const  |
|                          | 25 | SIMD 向量化        | SSE/AVX/AVX-512, restrict, 自动向量化         |
|                          | 26 | Intrinsics         | popcount, clz, ctz, 位操作加速                |
|                          | 27 | PGO                | 插桩→收集→优化, 10-40% 提速                 |
|                          | 28 | LTO                | 跨翻译单元内联, ThinLTO                       |
| **七、系统调优**   | 29 | 实时调度           | SCHED_FIFO, SCHED_DEADLINE                    |
|                          | 30 | 中断亲和性         | IRQ 隔离, irqbalance 禁用                     |
|                          | 31 | 内核参数           | sysctl 网络/内存/调度参数                     |
|                          | 32 | CPU 频率锁定       | performance governor, C-State 禁用            |
|                          | 33 | BIOS 调优          | HT/Turbo/C-State/Snoop Mode                   |
| **八、架构设计**   | 34 | 事件循环           | 单线程无锁, HFT 引擎, EventLoop               |
|                          | 35 | 流水线架构         | 多阶段 SPSC, Disruptor 模式                   |
|                          | 36 | 热/冷分离          | HOT_FUNC/COLD_FUNC, 热路径黄金法则            |
|                          | 37 | 预计算查找表       | 编译期 CRC32, 价格转换 LUT                    |
|                          | 38 | 生产调优           | 延迟陷阱清单, 监控指标, 系统架构              |

---

## 文件关系与知识图谱

```
             test14.cpp (CPU微架构)
              │ cache/pipeline/SIMD/分支预测
              │
test11.cpp ──┼── test13.cpp (本文件) ──── test9.cpp
(无锁编程)   │   微秒低延迟系统         (高并发)
 atomic/RCU  │   完整工程实践               epoll/io_uring
 memory order│                              thread pool
              │
              ├── test7.cpp (网络编程)
              │    TCP/UDP/socket
              │
              ├── test6.cpp (多线程)
              │    mutex/condvar/future
              │
              └── test12.cpp (零开销抽象)
                   CRTP/Policy/TypeErasure

知识依赖链:
┌─────────────────────────────────────────────────────────────┐
│  第八篇: 架构设计 (Ch34-38)                                  │
│    事件循环 │ 流水线 │ 热冷分离 │ 查找表 │ 生产调优          │
│  ← 综合运用以下所有技术 →                                    │
├─────────────────────────────────────────────────────────────┤
│  第七篇: 系统级调优 (Ch29-33)    │  第六篇: 编译器优化       │
│    SCHED_FIFO │ IRQ │ sysctl    │   SIMD │ PGO │ LTO │ 屏障│
│    频率锁定 │ BIOS              │   intrinsics             │
├─────────────────────────────────────────────────────────────┤
│  第五篇: 网络 (Ch19-23)         │  第四篇: 无锁 (Ch15-18)  │
│    DPDK │ 零拷贝 │ TCP调优      │   SPSC │ MPSC │ SeqLock  │
│    组播 │ 硬件时间戳            │   无锁池 │ ABA            │
├─────────────────────────────────────────────────────────────┤
│  第三篇: CPU/调度 (Ch10-14)     │  第二篇: 内存 (Ch4-9)    │
│    绑核 │ 忙等待 │ 自旋锁      │   对齐 │ 预取 │ 大页      │
│    NUMA │ 分支预测              │   对象池 │ Arena │ SoA    │
├─────────────────────────────────────────────────────────────┤
│  第一篇: 延迟测量 (Ch1-3)                                    │
│    rdtsc │ 直方图 │ p99.9 │ 热路径识别 │ 基准方法论         │
└─────────────────────────────────────────────────────────────┘
```

---

## 延迟量级参考（心中有数的数字）

| 操作                | 延迟          | 备注             |
| ------------------- | ------------- | ---------------- |
| L1 cache hit        | ~1 ns         | 最快的内存访问   |
| L2 cache hit        | ~4 ns         |                  |
| L3 cache hit        | ~12 ns        |                  |
| DRAM 访问           | ~80 ns        | 本地 NUMA 节点   |
| DRAM 远程 (NUMA)    | ~140 ns       | 跨节点 +60ns     |
| 原子 CAS (无竞争)   | ~5-10 ns      |                  |
| 原子 CAS (有竞争)   | ~20-100 ns    | 缓存行 ping-pong |
| 分支预测失败        | ~15-20 cycles | 流水线清空       |
| syscall (getpid)    | ~100 ns       | 最轻量级         |
| mutex lock (无竞争) | ~25 ns        |                  |
| mutex lock (有竞争) | ~100-10000 ns | futex 唤醒       |
| malloc (小对象)     | ~100-500 ns   | 可能触发 mmap    |
| 线程上下文切换      | ~1-5 μs      | 含缓存冷启动     |
| 条件变量唤醒        | ~5-15 μs     | 含调度延迟       |
| page fault          | ~3-10 μs     |                  |
| TCP 发送 (loopback) | ~5-10 μs     | 内核协议栈       |
| DPDK 收发           | ~0.5-2 μs    | 内核旁路         |
| FPGA 数据通路       | ~0.1 μs      | 纯硬件           |

---

## 公共工具与编译器宏

```cpp
// ─── 缓存行大小 ───
#ifdef __cpp_lib_hardware_interference_size
  static constexpr size_t CACHE_LINE = std::hardware_destructive_interference_size;
#else
  static constexpr size_t CACHE_LINE = 64;
#endif

// ─── 编译器提示 ───
#define LIKELY(x)     __builtin_expect(!!(x), 1)
#define UNLIKELY(x)   __builtin_expect(!!(x), 0)
#define FORCE_INLINE   __attribute__((always_inline)) inline
#define NOINLINE       __attribute__((noinline))
#define HOT_FUNC       __attribute__((hot))     // 热函数：优化布局
#define COLD_FUNC      __attribute__((cold))    // 冷函数：移出热路径
#define PREFETCH_R(addr)  __builtin_prefetch((addr), 0, 3)  // 读预取到 L1
#define PREFETCH_W(addr)  __builtin_prefetch((addr), 1, 3)  // 写预取到 L1
#define CACHE_ALIGNED  __attribute__((aligned(64)))

// ─── 高精度计时器 ───
class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point start_ = clk::now();
public:
    double elapsed_ns() const;  // 纳秒
    double elapsed_us() const;  // 微秒
    double elapsed_ms() const;  // 毫秒
    void reset();
};
```

---

# 一、延迟测量与基准篇

---

## 第1章：高精度时钟与 rdtsc

### 1.1 三种 rdtsc 变体

```cpp
// rdtsc — 最低开销 (~20 cycles)，不序列化
FORCE_INLINE uint64_t rdtsc() {
    uint32_t lo, hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return (uint64_t)hi << 32 | lo;
}

// rdtscp — 自带序列化 (~30 cycles)，防止乱序执行影响测量
FORCE_INLINE uint64_t rdtscp() {
    uint32_t lo, hi, aux;
    asm volatile("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux));
    return (uint64_t)hi << 32 | lo;
}

// lfence + rdtsc — 显式序列化，最精确
FORCE_INLINE uint64_t rdtsc_fenced() {
    uint32_t lo, hi;
    asm volatile("lfence\n\trdtsc" : "=a"(lo), "=d"(hi) :: "memory");
    return (uint64_t)hi << 32 | lo;
}
```

### 1.2 TSC 频率校准

```cpp
class TscClock {
    double tsc_freq_ghz_;  // GHz = cycles per nanosecond
public:
    TscClock() {
        // 忙等 100ms，用 chrono 和 rdtsc 交叉校准
        auto start_ts = rdtsc();
        auto start_chrono = high_resolution_clock::now();
        sleep_for(100ms);
        double elapsed_sec = duration<double>(now() - start_chrono).count();
        tsc_freq_ghz_ = (rdtsc() - start_ts) / elapsed_sec / 1e9;
    }
    double cycles_to_ns(uint64_t c) const { return c / tsc_freq_ghz_; }
};
```

### 1.3 时钟选择指南

```
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
  1. 需要 constant_tsc + nonstop_tsc CPU 特性
  2. 跨核心可能不同步 → 绑核后使用
  3. 不适合做 wall-clock 计时
```

### 深入扩展

1. **`clock_gettime(CLOCK_MONOTONIC_RAW)`**：比 `CLOCK_MONOTONIC` 更无 NTP 校正干扰，适合纯延迟测量
2. **RDPMC (Performance Monitor Counter)**：读取 CPU 硬件计数器（L1 miss 等），比 `perf` 更低开销——需要 `rdpmc` 权限
3. **TSC 与超线程**：同一物理核心的两个逻辑核共享 TSC——但跨物理核需要 invariant TSC 保证
4. **ARM 平台时钟**：`CNTVCT_EL0` 计数器（ARM generic timer），精度通常低于 x86 rdtsc
5. **时钟源漂移校正**：长时间运行时 TSC 与 NTP 时间源的漂移——定期重校准策略

---

## 第2章：延迟直方图与百分位统计

### 2.1 LatencyHistogram 实现

```cpp
class LatencyHistogram {
    static constexpr size_t NUM_BUCKETS = 10000;  // 0~9999 ns
    std::array<uint64_t, NUM_BUCKETS + 1> buckets_{};
    uint64_t total_count_ = 0, min_ns_ = UINT64_MAX, max_ns_ = 0, sum_ns_ = 0;
public:
    void record(uint64_t latency_ns) {
        ++buckets_[std::min(latency_ns, (uint64_t)NUM_BUCKETS)];
        ++total_count_;
        min_ns_ = std::min(min_ns_, latency_ns);
        max_ns_ = std::max(max_ns_, latency_ns);
        sum_ns_ += latency_ns;
    }
    uint64_t percentile(double pct) const {
        uint64_t target = total_count_ * pct / 100.0;
        uint64_t acc = 0;
        for (size_t i = 0; i <= NUM_BUCKETS; ++i) {
            acc += buckets_[i];
            if (acc >= target) return i;
        }
        return NUM_BUCKETS;
    }
    // print_summary() → min/avg/median/p90/p99/p99.9/max
};
```

### 2.2 低延迟测量要点

```
1. 关注尾延迟: p99/p99.9 >> 平均值
   平均 100ns 但 p99.9=50μs → 不合格!
2. 预热: 前 N 次结果丢弃 (JIT/cache/TLB 冷启动)
3. 协调遗漏 (Coordinated Omission):
   不要在上一次完成后才开始下一次计时
   → wrk2 / HdrHistogram 正确处理
4. 样本量: ≥100K samples
5. 生产采样: 不要每次都记录 → 每 N 次采样
```

### 深入扩展

1. **HdrHistogram 实现原理**：对数分桶 + 指数压缩，用 2KB 内存覆盖 1ns~1h 动态范围
2. **协调遗漏详解**：在固定速率调度的测试中，如果一次请求阻塞 10ms，后续 10000 个请求的排队延迟也应记录
3. **多直方图合并**：跨线程各自维护独立直方图，冷路径上合并——避免原子操作
4. **自适应采样率**：延迟 > 阈值时全量采样，正常时 1/1024 采样——兼顾精度和开销
5. **perf + BPF 直方图**：`bpftrace -e 'uprobe:./app:func { @hist = hist(nsecs - @start); }'` 内核态直方图

---

## 第3章：热路径识别与测量框架

### 3.1 零开销条件编译

```cpp
#ifdef ENABLE_LATENCY_TRACE
  #define LATENCY_SCOPE(name) ScopedLatencyTracer _tracer_##__LINE__(name)
#else
  #define LATENCY_SCOPE(name)  // Release 下完全消除
#endif

// ScopedLatencyTracer: RAII 记录 [start, end] 延迟
// 只记录超过 1μs 的（避免无意义数据）
```

### 3.2 异步采集器

```cpp
class AsyncLatencyCollector {
    static constexpr size_t BUFFER_SIZE = 64 * 1024;
    std::array<LatencySample, BUFFER_SIZE> buffer_{};
    std::atomic<uint64_t> write_pos_{0};
public:
    void record(uint16_t point_id, uint64_t latency_ns) {
        uint64_t pos = write_pos_.fetch_add(1, std::memory_order_relaxed);
        buffer_[pos % BUFFER_SIZE] = {now_ns(), latency_ns, point_id};
    }
};
// 热路径只写一条记录 → 后台线程聚合分析
```

### 3.3 热路径识别方法

```
perf record -g ./app             → 火焰图 (热函数可视化)
perf stat -d ./app               → IPC / 缓存统计
VTune / AMD uProf                → 微架构分析
bpftrace 'uprobe:./app:func'     → 动态追踪 (无需重编译)

原则: 测量本身不超过被测操作的 1%
  rdtsc ~20 cycles vs 操作 ~300 cycles → OK (6.7%)
```

### 深入扩展

1. **eBPF 延迟检测**：`libbpf` + `USDT probe` 实现用户态零开销追踪点——无需修改代码
2. **火焰图差分 (diff flame graph)**：优化前后火焰图对比，直观看到哪些函数延迟减少
3. **内核态 tracing (ftrace)**：`trace-cmd record -p function_graph -g schedule` 追踪调度延迟
4. **硬件 PMU 事件**：`perf stat -e 'cpu/event=0xc4,umask=0x00/'` 自定义硬件计数器
5. **连续集成延迟回归测试**：每次提交自动运行基准测试，p99.9 超标即阻止合入

---

# 二、内存子系统篇

---

## 第4章：缓存行对齐与 false sharing

### 4.1 问题与解决

```cpp
// ❌ false sharing: 两个原子变量共享缓存行
struct BadCounters {
    std::atomic<uint64_t> counter1;  // 同一 64B
    std::atomic<uint64_t> counter2;  // ← MESI ping-pong
};

// ✅ 每个变量独占一个缓存行
struct alignas(CACHE_LINE) GoodCounters {
    alignas(CACHE_LINE) std::atomic<uint64_t> counter1{0};
    alignas(CACHE_LINE) std::atomic<uint64_t> counter2{0};
};

// sizeof(BadCounters) = 16 bytes → 可能 false sharing
// sizeof(GoodCounters) = 128 bytes → 彻底消除
```

### 4.2 实测结果（代码中实测）

```
两线程各递增 5M 次:
  BadCounters  (false sharing):  ~120ms
  GoodCounters (无 false sharing): ~30ms
→ false sharing 导致 4x 性能下降!

false sharing 代价:
  同一 NUMA 节点: ~70-200 cycles/次缓存行转移
  跨 NUMA 节点: ~200-400 cycles

检测: perf c2c record ./app → perf c2c report
```

### 深入扩展

1. **`std::hardware_destructive_interference_size` (C++17)**：编译期获取缓存行大小，但许多编译器未实现——保险起见硬编码 64
2. **MESI 协议状态分析**：M→I→S 转移路径的具体延迟——Intel Xeon 上测量为 ~42ns（同 socket）
3. **struct 成员排列优化**：热成员放前面、读写分离——`__attribute__((packed))` 的陷阱
4. **perf c2c 实战**：`perf c2c record -F 60000 -p PID` → `perf c2c report --stdio` 精确定位冲突行
5. **C++20 `destructive_interference_size` vs `constructive_interference_size`**：前者避免共享（padding），后者鼓励共享（数组元素）

---

## 第5章：内存预取 (Prefetch)

### 5.1 实测结果（代码中基准）

```
1M 个 int (4MB) 访问:
  顺序访问:          ~X ms (硬件预取器自动处理)
  随机访问 (无预取):  ~Y ms (大量 cache miss)
  随机访问 (有预取):  ~Z ms (预取减少 30-50% miss)

prefetch 距离 = 提前 8 个元素
__builtin_prefetch(addr, rw, locality)
  rw:       0=读, 1=写
  locality: 0=NTA(不保留), 1=L3, 2=L2, 3=L1
```

### 5.2 预取距离公式

```
最佳距离 ≈ 内存延迟(ns) / 每元素处理时间(ns)

例: DRAM ~80ns, 每元素处理 ~10ns → 提前 8 个预取

太近 → 数据还没到达 → 无效
太远 → 数据被驱逐 → 无效
过多 → 污染缓存 → 适得其反
```

### 深入扩展

1. **硬件预取器类型**：L1 Stream Prefetcher / L2 Adjacent Line / L2 Stride Prefetcher——不同处理器有不同组合
2. **`_mm_prefetch` + `_MM_HINT_NTA`**：Non-Temporal Access 预取，不污染 L1/L2——适合只读一次的数据
3. **链表 / B-Tree 预取**：`prefetch(current->next)` 在处理当前节点时预取下一个——Redis 跳表使用此技术
4. **批量预取 (batch prefetch)**：先预取一批地址，再批量处理——摊低预取的开销
5. **perf stat 验证**：`perf stat -e L1-dcache-load-misses,L1-dcache-loads` 对比预取前后的 miss 率变化

---

## 第6章：大页 (Huge Pages) 与 TLB 优化

### 6.1 TLB 覆盖分析

```
┌──────────┬──────────┬──────────────┬─────────────┐
│ 页大小   │ TLB 条目 │ 可覆盖内存   │ TLB miss率  │
├──────────┼──────────┼──────────────┼─────────────┤
│ 4KB      │ ~1536    │ ~6MB         │ 高          │
│ 2MB      │ ~1536    │ ~3GB         │ 低          │
│ 1GB      │ ~4       │ ~4GB         │ 极低        │
└──────────┴──────────┴──────────────┴─────────────┘

TLB miss 代价: ~10-100 cycles
效果: 对于大数据集 (>6MB), 大页常可显著降低 TLB miss（具体幅度依赖工作集与内核配置）
```

### 6.2 使用方式

```cpp
// Linux: mmap + MAP_HUGETLB
void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);

// Linux: madvise
madvise(p, size, MADV_HUGEPAGE);

// Windows:
VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES,
             PAGE_READWRITE);

// 自定义 STL 分配器
template<typename T>
struct HugePageAllocator {
    T* allocate(size_t n) { return (T*)mmap(... | MAP_HUGETLB); }
    void deallocate(T* p, size_t n) { munmap(p, n * sizeof(T)); }
};
std::vector<int, HugePageAllocator<int>> data(1'000'000);
```

### 深入扩展

1. **透明大页 (THP) 的陷阱**：THP 压缩/拆分可能引起 ~ms 级延迟尖峰——低延迟系统建议禁用 THP，手动管理
2. **1GB 页的适用场景**：数据库缓冲池、大型哈希表——GRUB `hugepagesz=1G hugepages=4` 分配
3. **NUMA + 大页**：`numactl --membind=0 --cpunodebind=0 ./app` 确保大页分配在本地 NUMA 节点
4. **大页预分配时机**：系统启动时分配最可靠，运行时可能因内存碎片失败
5. **`libhugetlbfs`**：用户态库，透明替换 malloc 为大页——无需修改代码

---

## 第7章：对象池 (Object Pool)

### 7.1 Freelist 设计

```cpp
template<typename T, size_t MaxSize = 4096>
class ObjectPool {
    struct FreeNode { FreeNode* next; };
    char* pool_;
    FreeNode* free_head_ = nullptr;

public:
    ObjectPool() {
        pool_ = aligned_alloc(CACHE_LINE, slot_size * MaxSize);
        // 构建 freelist: 利用未使用的内存空间存储链表指针
        for (size_t i = 0; i < MaxSize; ++i) {
            auto* node = reinterpret_cast<FreeNode*>(pool_ + i * slot_size);
            node->next = free_head_;
            free_head_ = node;
        }
    }

    template<typename... Args>
    T* allocate(Args&&... args) {
        if (UNLIKELY(!free_head_)) return nullptr;
        void* slot = free_head_;
        free_head_ = free_head_->next;   // O(1)
        return new (slot) T(std::forward<Args>(args)...);
    }

    void deallocate(T* obj) {
        obj->~T();
        auto* node = reinterpret_cast<FreeNode*>(obj);
        node->next = free_head_;
        free_head_ = node;  // O(1)
    }
};
```

### 7.2 性能对比（代码中实测）

```
Order 对象 (32 bytes) ×100K:
  new/delete:  ~X ms (堆管理 + 可能的 syscall)
  ObjectPool:  ~Y ms (纯指针操作)
→ 对象池在热路径上常有显著收益（具体倍数取决于分配器实现与负载）

黄金法则: 热路径上绝不调用 malloc/new/free/delete
```

### 深入扩展

1. **SLAB 分配器**：Linux 内核的对象池实现——按类型缓存已初始化的对象，避免频繁构造/析构
2. **每线程对象池 (thread-local pool)**：消除跨线程竞争——最佳低延迟方案
3. **固定大小 vs 可变大小池**：固定大小用 freelist O(1)，可变大小用 buddy algorithm O(log n)
4. **`std::pmr::monotonic_buffer_resource`**：C++17 标准的竞技场分配器——搭配 `polymorphic_allocator`
5. **内存池 + 大页**：对象池底层用 `mmap(MAP_HUGETLB)` 分配——同时消除 TLB miss 和 malloc 开销

---

## 第8章：竞技场分配器 (Arena Allocator)

### 8.1 极简设计

```cpp
class Arena {
    char* base_;
    size_t capacity_, offset_ = 0;
public:
    void* allocate(size_t size, size_t align = alignof(max_align_t)) {
        size_t aligned = (offset_ + align - 1) & ~(align - 1);
        if (aligned + size > capacity_) return nullptr;
        void* ptr = base_ + aligned;
        offset_ = aligned + size;
        return ptr;                    // O(1): 只是移动指针
    }
    void reset() { offset_ = 0; }     // O(1): 批量释放
};
```

### 8.2 Arena vs malloc 基准（代码中实测）

```
Order ×100K:
  Arena:  ~X μs (指针移动 ~2-5ns/次)
  malloc: ~Y μs (堆管理 ~100-500ns/次)
→ Arena 在批量分配/批量释放场景常可显著快于通用分配器

> **NOTE（补充说明）**：`aligned_alloc` 需要“size 是 alignment 的整数倍”，且在 Windows 下通常使用 `_aligned_malloc/_aligned_free` 成对接口；跨平台工程建议封装统一分配器以避免释放接口不匹配。

Arena 不支持单个释放 → 只能批量 reset()
典型用途: 每请求/每消息一个 Arena，处理完 reset()
```

### 深入扩展

1. **分页 Arena**：用完一页分配下一页 (linked list of pages)——避免预估大小
2. **`std::pmr::monotonic_buffer_resource`**：标准库 Arena——搭配 `pmr::vector/string` 零改代码
3. **Arena + RAII**：`ArenaScope` 在析构时自动 reset——确保不泄漏
4. **多线程 Arena**：每线程私有 Arena（最佳）或 `atomic<offset>` 无锁 Arena
5. **Arena 在编译器/解析器中的应用**：LLVM/Clang 大量使用 `BumpPtrAllocator`——AST 节点全部竞技场分配

---

## 第9章：SoA vs AoS 内存布局

### 9.1 布局对比

```
AoS: [x,y,z,vx,vy,vz,m,t] [x,y,z,vx,vy,vz,m,t] ...
     对象完整，单对象操作直觉
     不用的字段也加载到缓存 → 浪费带宽

SoA: [x,x,x,...] [y,y,y,...] [z,z,z,...] [vx,vx,vx,...] ...
     只加载需要的字段，SIMD 自动向量化
     单对象操作需要多次内存访问
```

### 9.2 实测结果（代码中基准）

```
1M 粒子位置更新 (x += vx, y += vy, z += vz):
  AoS: ~X ms (读 32B 只用 24B → 带宽利用 75%)
  SoA: ~Y ms (连续读取 → 带宽利用 100% + 自动向量化)
→ SoA 通常快 2-4x (尤其批量处理)
```

### 深入扩展

1. **AoSoA (混合布局)**：块内 SoA，块间 AoS——`struct Block { float x[8], y[8], z[8]; }`——兼顾缓存局部性和向量化
2. **Entity Component System (ECS)**：游戏引擎 (Unity DOTS / Bevy) 的 SoA 架构——按组件类型分离存储
3. **`std::simd` (P1928)**：标准库 SIMD 抽象——SoA 布局的自然搭配
4. **数据库列存储 (Columnar)**：ClickHouse/Arrow 的列式存储本质就是 SoA——OLAP 查询性能数量级提升
5. **自动 SoA 变换工具**：Intel ISPC (Implicit SPMD Program Compiler) 自动将 AoS 代码变换为 SoA 执行

---

# 三、CPU 与调度篇

---

## 第10章：CPU 亲和性 (Core Pinning)

### 10.1 代码绑核

```cpp
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);   // 绑定到 CPU 2
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
#elif defined(_WIN32)
    SetThreadAffinityMask(GetCurrentThread(), 1 << 2);  // CPU 2
#endif
```

### 10.2 CPU 隔离策略

```
典型低延迟系统 CPU 分配:
  CPU 0:   OS + 中断 (不隔离)
  CPU 1:   网络 I/O 线程
  CPU 2:   业务逻辑 (热路径) ← isolcpus 隔离
  CPU 3:   日志/监控
  CPU 4-N: 其他工作线程

Linux 内核参数:
  GRUB_CMDLINE_LINUX="isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3"

  isolcpus:   OS 调度器不自动分配线程到这些核心
  nohz_full:  禁用定时中断 (~4μs/tick 消除)
  rcu_nocbs:  RCU 回调不在这些核心上执行

效果: 被隔离 CPU 可显著减少调度噪声，通常有助于延迟稳定
线程迁移代价: ~5-50μs (L1/L2 缓存全废) → 绑核后消除
```

### 深入扩展

1. **`cset shield`**：cpuset 工具简化 CPU 和中断隔离——比手动 isolcpus 更灵活
2. **超线程 (HT) 与绑核**：低延迟系统通常禁用 HT——或绑核到物理核心（避免 sibling 干扰）
3. **CPU 热插拔**：在线隔离/恢复 CPU——`echo 0 > /sys/devices/system/cpu/cpu2/online`
4. **容器 (Docker/K8s) 绑核**：`--cpuset-cpus=2,3` / K8s `cpuManager: static`
5. **Windows 处理器组 (Processor Groups)**：超过 64 个逻辑核时需要 `SetThreadGroupAffinity`

---

## 第11章：忙等待 (Busy Wait) vs 阻塞

### 11.1 等待策略

```cpp
class SpinWait {
    // 纯自旋: 延迟最低，CPU 100%
    static void spin(atomic<bool>& flag) {
        while (!flag.load(acquire)) {}
    }
    // PAUSE 自旋: 省电，避免流水线惩罚
    static void spin_pause(atomic<bool>& flag) {
        while (!flag.load(acquire)) __builtin_ia32_pause();
    }
    // 自适应: 自旋 N 次 → yield → sleep
    static void adaptive_wait(atomic<bool>& flag, int spin_count = 1000);
};
```

### 11.2 唤醒延迟实测（代码中实测）

```
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

PAUSE 指令 (Intel Skylake+): ~140 cycles
  降低功耗 ~20%
  避免自旋退出的流水线清空惩罚
  超线程环境让资源给 sibling core
```

### 深入扩展

1. **`UMWAIT` / `TPAUSE` (Intel Tremont+)**：用户态低功耗等待指令——比 PAUSE 更省电，延迟可控
2. **Linux `futex` 系统调用细节**：`FUTEX_WAIT/WAKE` 的内核路径——从 `do_futex` 到唤醒的精确延迟链
3. **`eventfd` + `epoll` 组合**：跨线程唤醒的替代方案——比 condvar 更灵活
4. **忙等待 + CPU 频率**：100% 占用 → 维持最高频率 → 延迟更低（C-State 不会触发）
5. **等待策略与功耗平衡**：数据中心按 CPU 利用率计费——过度忙等待 = 浪费电费

---

## 第12章：自旋锁与自适应退避

### 12.1 TicketLock（公平）

```cpp
class TicketSpinLock {
    alignas(CACHE_LINE) std::atomic<uint32_t> ticket_{0};   // 取号
    alignas(CACHE_LINE) std::atomic<uint32_t> serving_{0};  // 叫号
public:
    void lock() {
        uint32_t my = ticket_.fetch_add(1, relaxed);
        while (serving_.load(acquire) != my) __builtin_ia32_pause();
    }
    void unlock() { serving_.fetch_add(1, release); }
};
```

### 12.2 指数退避（高吞吐）

```cpp
class BackoffSpinLock {
    std::atomic<bool> locked_{false};
public:
    void lock() {
        int backoff = 1;
        while (true) {
            while (locked_.load(relaxed)) {       // TTAS: 先读再尝试
                for (int i = 0; i < backoff; ++i) __builtin_ia32_pause();
            }
            if (!locked_.exchange(true, acquire)) return;
            backoff = std::min(backoff * 2, 1024); // 指数退避
        }
    }
};
```

### 12.3 自旋锁 vs mutex 实测（代码中基准）

```
2 线程 ×1M 次 lock/unlock:
  TicketSpinLock: ~X ms (纯用户态，无 syscall)
  std::mutex:     ~Y ms (竞争时有 futex syscall)

自旋锁: 临界区 <1μs 时更快
mutex:  临界区较长时更好 (不浪费 CPU)
最佳:   无锁设计 → 完全消除锁
```

### 深入扩展

1. **MCS Lock**：每个等待者在自己的缓存行上自旋——消除 TicketLock 的 serving_ 缓存行争用
2. **CLH Lock**：类似 MCS 但基于隐式链表——Linux 内核 `qspinlock` 的实现基础
3. **读写自旋锁**：分离读取和写入路径——读操作不竞争
4. **锁粒度分析**：粗粒度 vs 细粒度 vs 无锁——Trade-off 分析方法
5. **`std::atomic_flag` 实现**：最轻量的自旋锁——`test_and_set` 只需一条指令

---

## 第13章：NUMA 感知编程

### 13.1 NUMA 架构

```
NUMA Node 0              NUMA Node 1
┌────────┐               ┌────────┐
│ CPU 0  │─── QPI/UPI ──│ CPU 1  │
│ Core0-7│               │ Core8-15│
└───┬────┘               └───┬────┘
    │                        │
┌───┴────┐               ┌───┴────┐
│ 内存0  │               │ 内存1  │
│ 64GB   │               │ 64GB   │
└────────┘               └────────┘

本地访问: ~80 ns    远程访问: ~140 ns (+75%)
```

### 13.2 首次触摸策略

```cpp
// ❌ 错误: 主线程分配 → 物理页在 node 0
int* data = new int[N];
memset(data, 0, N);  // 所有页在 node 0
// worker (node 1) 使用 → 远程访问!

// ✅ 正确: 在目标线程中首次触摸
int* data = new int[N];  // 只分配虚拟地址
worker_thread([=] {
    memset(data, 0, N);  // 首次触摸 → 物理页在本地
});

// 或使用 numactl
numactl --membind=0 --cpunodebind=0 ./app
```

### 深入扩展

1. **`libnuma` API**：`numa_alloc_onnode(size, node)` / `numa_set_membind()` 精细控制内存分配
2. **NUMA balancing (Linux 自动)**：内核自动迁移页面到访问它的 CPU 所在节点——延迟场景建议关闭 `numa_balancing=0`
3. **Zone reclaim 禁用**：`vm.zone_reclaim_mode=0` 避免跨 NUMA 回收导致的延迟尖峰
4. **NUMA 拓扑发现**：`hwloc` / `lstopo` 可视化 NUMA/Cache/PCI 拓扑——精确规划内存和线程亲和性
5. **CXL (Compute Express Link)**：未来的 NUMA 扩展——CXL 设备的内存延迟约 ~300-500ns

---

## 第14章：分支预测优化

### 14.1 无分支算法

```cpp
// 无分支绝对值
FORCE_INLINE int branchless_abs(int x) {
    int mask = x >> 31;        // 负数→全1, 正数→全0
    return (x + mask) ^ mask;  // abs(x)
}

// 无分支 min/max
FORCE_INLINE int branchless_min(int a, int b) {
    return b + ((a - b) & ((a - b) >> 31));
}

// 无分支 clamp
FORCE_INLINE int branchless_clamp(int x, int lo, int hi) {
    return branchless_min(branchless_max(x, lo), hi);
}
```

### 14.2 实测对比（代码中基准）

```
10M 次分支跳转:
  可预测 (99.99% true):  ~X ms (分支预测 ~99.99% 命中)
  不可预测 (50/50):       ~Y ms (每次预测失败 ~15-20 cycles)
  无分支位操作:           ~Z ms (无流水线清空)

分支预测失败代价: ~15-20 cycles (流水线清空)
```

### 深入扩展

1. **`[[likely]]` / `[[unlikely]]` (C++20)**：标准化的分支提示——比 `__builtin_expect` 更可移植
2. **CMOV 指令**：编译器自动生成条件移动——`-O2` 下简单三元运算符通常编译为 CMOV
3. **查找表替代分支**：`action_table[state]()` 替代 `switch(state)`
4. **分支目标缓冲器 (BTB)**：间接跳转预测——虚函数调用的瓶颈，BTB miss ~20 cycles
5. **perf stat 分支统计**：`perf stat -e branch-misses,branches` 精确判断分支优化是否有效

---

# 四、无锁数据结构篇

---

## 第15章：SPSC 环形队列（生产级）

### 15.1 设计要点

```cpp
template<typename T, size_t Capacity>  // Capacity 必须是 2 的幂
class SPSCQueue {
    // 访问模式分离到不同缓存行
    struct alignas(CACHE_LINE) { std::atomic<uint64_t> head{0}; };  // 消费者写
    struct alignas(CACHE_LINE) { std::atomic<uint64_t> tail{0}; };  // 生产者写
    struct alignas(CACHE_LINE) {
        uint64_t cached_head{0};  // 生产者缓存的 head → 减少跨缓存行读
        uint64_t cached_tail{0};  // 消费者缓存的 tail
    };
    alignas(CACHE_LINE) T buffer_[Capacity];
    static constexpr uint64_t MASK = Capacity - 1;

    bool try_push(const T& item) {
        const uint64_t t = tail.load(relaxed);
        if (t - cached_head >= Capacity) {
            cached_head = head.load(acquire);  // 仅在可能满时才跨缓存行读
            if (t - cached_head >= Capacity) return false;
        }
        buffer_[t & MASK] = item;
        tail.store(t + 1, release);
        return true;
    }
    // try_pop() 对称实现
};
```

### 15.2 性能实测（代码中基准）

```
5M 条 int64_t 消息:
  吞吐: >100M ops/sec
  每操作: ~10-15 ns
  校验: 生产者写入 [0, N) 求和 vs 消费者累加

优化要点:
  1. & MASK 替代 % 取模 (位运算 vs 除法)
  2. head/tail 不同缓存行 (消除 false sharing)
  3. cached head/tail (减少跨缓存行同步)
  4. acquire/release (不需要 seq_cst)
  5. 批量操作: 一次 push/pop 多个 → 均摊更低
```

### 深入扩展

1. **LMAX Disruptor (Java)**：单生产者 Ring Buffer + 多消费者依赖图——C++ 移植版 `folly::ProducerConsumerQueue`
2. **批量 push/pop**：`try_push_n(T* items, size_t n)` 一次写入多条——减少原子操作次数
3. **等待策略集成**：`WaitStrategy` 模板参数——BusySpinWait / YieldingWait / SleepingWait
4. **SPSC 队列的 Lamport 证明**：只需要单调递增的 head/tail + release/acquire → 无需 CAS
5. **`boost::lockfree::spsc_queue`**：生产级实现，支持批量操作和动态大小

---

## 第16章：MPSC 队列

### 16.1 Exchange-based 设计

```cpp
template<typename T>
class MPSCQueue {
    struct Node { T data; std::atomic<Node*> next{nullptr}; };
    alignas(CACHE_LINE) std::atomic<Node*> head_;  // 消费者端
    alignas(CACHE_LINE) std::atomic<Node*> tail_;  // 生产者端

    void push(const T& data) {
        auto* node = new Node{data};
        Node* prev = tail_.exchange(node, acq_rel);  // 原子交换尾指针
        prev->next.store(node, release);              // 链接前驱→新节点
    }
    bool try_pop(T& data) {
        Node* h = head_.load(relaxed);
        Node* next = h->next.load(acquire);
        if (!next) return false;
        data = next->data;
        head_.store(next, release);
        delete h;  // ← 生产版应使用对象池!
        return true;
    }
};
```

### 深入扩展

1. **MPMC 队列**：多生产者多消费者——`folly::MPMCQueue` 使用 ticket+slot 设计
2. **无分配 MPSC 队列**：预分配节点池 + intrusive linked list——消除 `new/delete`
3. **Vyukov MPSC 队列**：`Dmitry Vyukov` 的经典设计——Linux 内核 `llist` 基于类似思想
4. **Vyukov MPMC 有界队列**：基于 sequence counter 的等待——比 CAS 竞争更少
5. **CAS 与 exchange 的性能差异**：`exchange` 是单次原子写（无重试），`CAS` 可能需要重试——MPSC push 用 exchange 更优

---

## 第17章：SeqLock — 读写极端不对称

### 17.1 核心实现

```cpp
class SeqLock {
    alignas(CACHE_LINE) std::atomic<uint64_t> seq_{0};
public:
    uint64_t read_begin() const {
        uint64_t s;
        do { s = seq_.load(acquire); } while (s & 1); // 奇数=写入中
        return s;
    }
    bool read_retry(uint64_t start_seq) const {
        atomic_thread_fence(acquire);
        return seq_.load(relaxed) != start_seq;
    }
    void write_lock()   { seq_.fetch_add(1, release); } // → 奇数
    void write_unlock() { seq_.fetch_add(1, release); } // → 偶数
};

// MarketData 示例:
class MarketDataFeed {
    SeqLock lock_;
    MarketData data_{};
public:
    void update(double bid, double ask, ...) {
        lock_.write_lock();
        data_ = ...; // 写入新数据
        lock_.write_unlock();
    }
    MarketData read() const {
        MarketData result;
        uint64_t seq;
        do {
            seq = lock_.read_begin();
            result = data_;              // 可能读到撕裂数据
        } while (lock_.read_retry(seq)); // 如果被撕裂则重试
        return result;
    }
};
```

### 17.2 SeqLock vs RWLock

```
┌──────────────┬──────────────┬──────────────┐
│              │ SeqLock      │ RWLock       │
├──────────────┼──────────────┼──────────────┤
│ 写者延迟     │ ~几 ns       │ ~几百 ns     │
│ 读者阻塞写者 │ 不阻塞       │ 阻塞         │
│ 读者可能重试 │ 是           │ 否           │
│ 数据要求     │ trivially    │ 任意         │
│              │ copyable     │              │
│ 适用场景     │ 1写多读      │ 少写多读     │
└──────────────┴──────────────┴──────────────┘

典型场景: 市场数据 feeds, 配置热更新, 时间戳分发
```

### 深入扩展

1. **SeqLock + SIMD Copy**：用 `_mm256_load/store` 原子宽度拷贝——减少撕裂重试概率
2. **双缓冲 (Double Buffering)**：SeqLock 的替代——写者写后台缓冲区，原子切换指针
3. **Linux 内核 SeqLock**：`read_seqbegin() / read_seqretry()`——用于 `jiffies` 时间更新
4. **version clock 扩展**：seq 号同时作为数据版本——读者可以检测"新了多少版本"
5. **SeqLock 与 store buffer**：x86 上 seq_.store 后紧接数据写是安全的（TSO 保证写入顺序）

---

## 第18章：无锁内存池

### 18.1 CAS Freelist

```cpp
template<typename T, size_t PoolSize = 4096>
class LockFreePool {
    struct Node { union { char storage[sizeof(T)]; Node* next; }; };
    alignas(CACHE_LINE) std::atomic<Node*> free_head_;

    T* allocate(...) {
        Node* node;
        do {
            node = free_head_.load(acquire);
            if (!node) return nullptr;
        } while (!free_head_.compare_exchange_weak(node, node->next, acq_rel));
        return new (node->storage) T(...);
    }
    void deallocate(T* obj) {
        obj->~T();
        Node* node = reinterpret_cast<Node*>(obj);
        Node* head;
        do {
            head = free_head_.load(relaxed);
            node->next = head;
        } while (!free_head_.compare_exchange_weak(head, node, release));
    }
};
```

### 18.2 ABA 问题

```
线程A 读 head=X
线程B pop X → pop Y → push X (X 回来了)
线程A CAS(head, X, X->next) 成功 → 但 X->next 已改变!

解决方案:
  1. Tagged pointer: 高 16 位存版本号 → 每次操作 +1
  2. Hazard Pointers: 保护正在访问的指针
  3. 每线程池 (最佳): 消除跨线程竞争
```

### 深入扩展

1. **Tagged pointer (Double-Width CAS)**：`atomic<pair<uint64_t, uint64_t>>` 或 `__uint128_t`——但 DWCAS 开销更高
2. **Hazard Pointers (P2530)**：C++26 标准提案——`std::hazard_pointer` 安全延迟回收
3. **Epoch-Based Reclamation (EBR)**：`crossbeam` (Rust) / `folly::RCU` 的实现基础
4. **`std::atomic<shared_ptr>` (C++20)**：引用计数的原子操作——简单但开销高（128-bit CAS）
5. **jemalloc thread cache**：每线程缓存 + 跨线程无锁传递——生产级无锁分配器

---

# 五、网络 I/O 篇

---

## 第19章：内核旁路 (Kernel Bypass)

### 19.1 传统 vs 旁路

```
传统: NIC → 驱动 → 内核协议栈 → socket → 用户空间
      延迟: 5-50 μs (syscall + 上下文切换 + 数据拷贝)

旁路: NIC → 用户空间 (直接 DMA)
      延迟: 0.5-2 μs
```

### 19.2 方案对比

```
┌────────────────┬──────────┬──────────────────────────┐
│ 方案           │ 延迟     │ 特点                     │
├────────────────┼──────────┼──────────────────────────┤
│ DPDK           │ ~1 μs    │ Intel PMD, 轮询驱动      │
│ Solarflare/ef_ │ ~0.7 μs  │ Xilinx/AMD, OpenOnload   │
│ Mellanox VMA   │ ~1 μs    │ NVIDIA, RDMA 加速        │
│ io_uring       │ ~2-5 μs  │ Linux 5.1+, 异步 IO      │
│ XDP            │ ~2 μs    │ eBPF 在 NIC 层           │
│ FPGA           │ ~0.1 μs  │ 最低延迟, 纯硬件         │
└────────────────┴──────────┴──────────────────────────┘
```

### 深入扩展

1. **DPDK 架构**：Poll Mode Driver (PMD) + `rte_eth_rx_burst()` + 大页 + 核心隔离——完整的用户态网络栈
2. **io_uring 零拷贝 (IORING_OP_SEND_ZC)**：Linux 6.0+ 支持——不需要 DPDK 复杂度就能接近内核旁路性能
3. **AF_XDP socket**：Linux XDP + `AF_XDP` socket——在内核 eBPF 层直接到用户态，比 DPDK 更易集成
4. **Solarflare OpenOnload**：透明拦截 socket API，无需修改代码——延迟 ~0.7μs
5. **FPGA 交易系统**：Xilinx Alveo / Intel Stratix——纯硬件解析 FIX/ITCH 协议 + 决策 + 发送

---

## 第20-23章：零拷贝、TCP 调优、UDP 组播、硬件时间戳

### 20. 零拷贝

```
传统:  用户buf → 内核buf → NIC DMA buf  (2次拷贝)
零拷贝: 用户buf → NIC DMA buf            (0次拷贝)
实现: sendmsg() + MSG_ZEROCOPY / DPDK rte_mbuf
```

### 21. TCP 低延迟常用项

```cpp
int flag = 1;
setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));   // 禁 Nagle
setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, &flag, sizeof(flag));  // 禁延迟 ACK
int busy = 50;
setsockopt(fd, SOL_SOCKET, SO_BUSY_POLL, &busy, sizeof(busy));   // 忙轮询 50μs

sysctl:
  net.core.busy_poll = 50
  net.core.busy_read = 50
  net.ipv4.tcp_low_latency = 1
```

### 22. UDP 组播

```cpp
struct ip_mreq mreq;
mreq.imr_multiaddr.s_addr = inet_addr("239.1.1.1");
mreq.imr_interface.s_addr = INADDR_ANY;
setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
// 高性能批量接收: recvmmsg() 一次收多个包
```

### 23. 硬件时间戳

```
setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPNS, ...);
// 或 SO_TIMESTAMPING for 硬件时间戳 (NIC 打时间戳)
精度: ~ns 级 (PTP 同步后)
用途: 精确测量网络延迟、交易审计与合规
```

### 深入扩展（网络篇综合）

1. **TCP BBR 拥塞控制**：Google 的基于带宽估计的拥塞控制——比 CUBIC 更低延迟
2. **QUIC / HTTP3**：基于 UDP 的传输协议——0-RTT 建连，多路复用无队头阻塞
3. **PTP (IEEE 1588) 时间同步**：硬件时间戳 + PTP 协议——跨机器 <1μs 时钟同步
4. **RSS (Receive Side Scaling)**：多队列网卡 + 哈希分流——每个 CPU 核独立处理一个队列
5. **TCP/UDP 校验和卸载**：网卡硬件计算校验和——释放 CPU 资源

---

# 六、编译器与指令级优化篇

---

## 第24章：编译器屏障与优化暗示

### 24.1 关键机制

```
1. 编译器屏障 (零运行时开销):
   asm volatile("" ::: "memory");    // GCC/Clang
   _ReadWriteBarrier();               // MSVC
   → 阻止编译器跨屏障重排内存操作

2. CPU 内存屏障 (~20-100 cycles):
   mfence  — 全屏障
   lfence  — 读屏障
   sfence  — 写屏障

3. volatile:
   ❌ 不保证原子性，不保证多线程可见性
   ✅ 用于: MMIO, 信号处理, benchmark 防优化

4. __restrict__:
   告诉编译器指针不重叠 → 启用自动向量化

5. [[gnu::pure]] / [[gnu::const]]:
   纯函数标记 → 编译器消除重复调用

6. __builtin_unreachable():
   标记不可达路径 → 更激进的优化
```

### 深入扩展

1. **`std::atomic_signal_fence` vs `atomic_thread_fence`**：前者只影响编译器、后者影响 CPU——信号处理场景用 signal_fence
2. **`asm volatile` 的 clobber list 细节**：`"memory"` 有多重——还有 `"cc"` (条件码)，具体寄存器
3. **`__attribute__((optimize("O3")))` 函数级优化**：对热函数单独提升优化等级
4. **PGO + ICP (Indirect Call Promotion)**：将虚函数调用转为直接调用——基于 profile 自动去虚化
5. **Control Flow Integrity (CFI)**：安全 vs 性能——`-fsanitize=cfi` 检测间接调用劫持

---

## 第25章：SIMD 向量化

### 25.1 手写 Intrinsics

```cpp
// SSE: 4 floats/cycle
void add_sse(float* a, float* b, float* c, int n) {
    for (int i = 0; i < n; i += 4) {
        __m128 va = _mm_load_ps(&a[i]);
        __m128 vb = _mm_load_ps(&b[i]);
        _mm_store_ps(&c[i], _mm_add_ps(va, vb));
    }
}
// AVX: 8 floats/cycle
void add_avx(float* a, float* b, float* c, int n) {
    for (int i = 0; i < n; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        _mm256_store_ps(&c[i], _mm256_add_ps(va, vb));
    }
}
```

### 25.2 自动向量化条件

```
1. -O2 -march=native
2. 循环无数据依赖
3. 连续内存 + __restrict__
4. 循环边界编译期可知
5. 无分支 (或可转为 blend)

检查:
  gcc -O2 -march=native -fopt-info-vec-optimized
  clang -O2 -Rpass=loop-vectorize
```

### 深入扩展

1. **AVX-512 频率降低**：AVX-512 可能触发 CPU 降频 ~100-200MHz——量化延迟时需关注
2. **Highway / xsimd / Vc 库**：可移植的 SIMD 抽象库——一套代码支持 SSE/AVX/NEON/SVE
3. **SIMD JSON 解析 (simdjson)**：利用 SIMD 并行处理 JSON——比常规解析器快 10x
4. **`std::experimental::simd` (Parallelism TS v2)**：标准化 SIMD 类型——`simd<float, 8>` 自动选择最佳指令集
5. **Masking (AVX-512)**：`_mm512_mask_add_ps(src, mask, a, b)` 条件向量化——替代分支

---

## 第26章：内联汇编与 Intrinsics

### 26.1 位操作加速

```cpp
fast_popcount(0xFF00FF)  = 16   // POPCNT: 1 cycle
fast_clz(16)              = 27   // LZCNT: 前导零
fast_ctz(16)              = 4    // TZCNT: 尾部零
fast_log2(1024)           = 10   // 31 - clz

性能: intrinsic vs 软件实现
  popcount ×10M: intrinsic ~X ms vs software ~Y ms
→ 硬件指令快 10-50x (单 cycle vs 循环)
```

### 深入扩展

1. **`_pdep` / `_pext` (BMI2)**：位存取 (parallel bit deposit/extract)——棋盘类程序、位图索引的加速利器
2. **`_tzcnt_u64` (BMI1)**：比 `__builtin_ctz` 更明确——`ctz(0)` 未定义但 `tzcnt(0)=64`
3. **CRC32 硬件指令**：`_mm_crc32_u8/u32/u64` (SSE4.2)——比查找表快且通用
4. **AES-NI**：`_mm_aesenc_si128` 硬件 AES 加密——加密吞吐 >10 GB/s
5. **RDRAND / RDSEED**：硬件真随机数生成器——比 `std::mt19937` 更安全（但更慢）

---

## 第27-28章：PGO 与 LTO

### 27. PGO（Profile-Guided Optimization）

```bash
# 步骤 1: 插桩编译
g++ -O2 -fprofile-generate -o app app.cpp
# 步骤 2: 运行真实工作负载
./app < real_workload.dat    # → *.gcda
# 步骤 3: 使用 profile 优化编译
g++ -O2 -fprofile-use -o app app.cpp

优化内容: 热函数激进内联, 分支基于真实数据, 代码布局更贴近热路径
效果: 在代表性 workload 下常有可观收益，具体幅度依赖代码与数据分布
```

### 28. LTO（Link-Time Optimization）

```bash
g++ -O2 -flto -c *.cpp && g++ -O2 -flto *.o -o app
clang++ -O2 -flto=thin *.cpp -o app   # ThinLTO 更快

优化: 跨翻译单元内联, 死代码消除, 全局变量优化, 去虚化
效果: 5-20%

最佳组合: -O2 -march=native -flto -fprofile-use → 最大优化
```

### 深入扩展

1. **BOLT (Binary Optimization and Layout Tool)**：Meta 的后链接优化工具——重排代码布局，减少 I-cache miss
2. **`-fprofile-sample-use` (AutoFDO)**：使用 `perf record` 采样数据替代插桩——生产环境可用
3. **ThinLTO vs Full LTO**：ThinLTO 并行编译（快 2-5x），Full LTO 优化更彻底（小文件更好）
4. **Propeller**：Google 的函数级代码布局优化——比 BOLT 更细粒度
5. **LTO 与模板**：跨翻译单元去重模板实例化——减少二进制大小和 I-cache 压力

---

# 七、系统级调优篇

---

## 第29-33章：系统级调优总览

### 29. Linux 实时调度

```
SCHED_FIFO:     先来先服务, 优先级 1-99
SCHED_DEADLINE: 基于截止时间 (最先进)

sudo chrt -f 90 ./app        # FIFO 优先级 90
kernel.sched_rt_runtime_us = -1  # 允许 RT 线程 100% CPU
```

### 30. 中断亲和性

```bash
echo 1 > /proc/irq/<IRQ_NUM>/smp_affinity  # 网卡中断绑 CPU 0
systemctl stop irqbalance                    # 禁用自动平衡
```

### 31. 内核参数 (sysctl)

```
# 网络
net.core.busy_poll = 50
net.core.busy_read = 50
net.ipv4.tcp_low_latency = 1
net.ipv4.tcp_fastopen = 3

# 内存
vm.swappiness = 0           # 禁 swap
vm.zone_reclaim_mode = 0    # 禁 NUMA zone reclaim
vm.min_free_kbytes = 1048576 # 保留 1GB

# 调度
kernel.sched_rt_runtime_us = -1
```

### 32. CPU 频率锁定

```bash
cpupower frequency-set -g performance
# 或
echo performance > /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# 禁用深 C-State:
# GRUB: intel_idle.max_cstate=0 processor.max_cstate=0
# 或: 写入 /dev/cpu_dma_latency (保持 fd 打开)
int fd = open("/dev/cpu_dma_latency", O_WRONLY);
int val = 0;
write(fd, &val, sizeof(val));  // CPU 不进入深睡眠

C-State 唤醒延迟: C1 ~2μs, C3 ~50μs, C6 ~100μs → 全部消除
```

### 33. BIOS 调优

```
关键设置:
  Hyper-Threading:  关闭 (消除抖动)
  Turbo Boost:      关闭 (频率稳定) 或开启 (峰值更高)
  C-States:         C1 only 或全部禁用
  P-States:         固定最高频率
  Power:            Maximum Performance
  NUMA Interleaving: 关闭
  Intel SpeedStep:   关闭
  LLC Prefetch:      开启
  Snoop Mode:        Early Snoop (更低延迟)
```

### 深入扩展（系统篇综合）

1. **Linux PREEMPT_RT 内核**：完全抢占式内核——最坏情况延迟从 ~ms 降至 ~μs
2. **`mlockall(MCL_CURRENT | MCL_FUTURE)`**：锁定所有页面到物理内存——消除 page fault（~3-10μs）
3. **`/dev/cpu_dma_latency` 生产实践**：系统服务配置——确保进程退出时 fd 自动关闭
4. **tuned-adm latency-performance**：RHEL 的一键优化 profile——自动设置大部分参数
5. **cgroup v2 CPU 控制**：`cpu.max` 限制其他 cgroup 的 CPU 使用——保护低延迟进程

---

# 八、架构设计篇

---

## 第34章：事件驱动 — 单线程事件循环

### 34.1 EventLoop 实现

```cpp
class EventLoop {
    std::vector<EventHandler> immediate_queue_;
    std::priority_queue<TimerEntry, ..., greater<>> timer_queue_;

    void run_once() {
        // 1. 处理即时事件
        auto events = std::move(immediate_queue_);
        for (auto& h : events) h();
        // 2. 处理到期定时器
        while (!timer_queue_.empty() && timer_queue_.top().trigger_time <= now())
            timer_queue_.pop().handler();
    }
};

// 事件链:
loop.post([&] {
    /* 收到市场数据 */ → loop.post([&] {
        /* 策略计算 */ → loop.post([&] {
            /* 发送订单 */
        });
    });
});
```

### 34.2 单线程架构优势

```
┌──────────────────────────────────────────────┐
│          Event Loop (单线程, 绑核)            │
│                                              │
│  ┌─────────┐ → ┌──────────┐ → ┌────────────┐│
│  │ 网络 IO │   │ 解码/处理│   │ 发送/响应  ││
│  └─────────┘   └──────────┘   └────────────┘│
│                                              │
│ 全程无锁, 无上下文切换, 无线程同步 → 延迟最低 │
└──────────────────────────────────────────────┘
```

### 深入扩展

1. **`epoll` + 单线程事件循环**：nginx / Redis 的核心架构——单线程处理 100K+ 并发连接
2. **io_uring 事件循环**：`liburing` 异步提交——比 epoll 减少一半系统调用
3. **无分配事件系统**：事件对象预分配在对象池中——消除 `std::function` 的堆分配
4. **定时器轮 (Timing Wheel)**：比 `priority_queue` 更高效的定时器——O(1) 插入/删除，Kafka/Netty 使用
5. **异步日志集成**：事件循环只写异步 SPSC 队列 → 日志线程消费并写文件——零影响热路径

---

## 第35章：流水线架构

### 35.1 多阶段设计

```
┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
│ Stage 1 │───→│ Stage 2 │───→│ Stage 3 │───→│ Stage 4 │
│ 网络接收│    │ 解码解析│    │ 业务逻辑│    │ 网络发送│
│  CPU 1  │    │  CPU 2  │    │  CPU 3  │    │  CPU 4  │
└─────────┘    └─────────┘    └─────────┘    └─────────┘
    ↑               ↑               ↑               ↑
  SPSC Queue     SPSC Queue     SPSC Queue     直接发送

总延迟 = Σ(每阶段延迟 + 队列传递)
       = (0.5 + 0.05) + (1.0 + 0.05) + (2.0 + 0.05) + 0.5
       ≈ 4.15 μs
```

### 35.2 单线程 vs 流水线

```
┌────────────────┬──────────────────┬──────────────────┐
│                │ 单线程事件循环   │ 多阶段流水线     │
├────────────────┼──────────────────┼──────────────────┤
│ 吞吐量         │ 中 (单核瓶颈)   │ 高 (并行)        │
│ 单消息延迟     │ 最低             │ 略高 (队列)      │
│ 复杂度         │ 低               │ 中               │
│ 可扩展性       │ 差               │ 好 (加阶段)      │
└────────────────┴──────────────────┴──────────────────┘
```

### 深入扩展

1. **LMAX Disruptor 模式**：Ring Buffer + 消费者依赖图 + 批处理——>6M events/sec
2. **流水线反压 (Backpressure)**：队列满时上游阻塞——防止消息丢失
3. **动态负载均衡**：监控每阶段队列深度——运行时调整阶段的 CPU 分配
4. **Seastar 框架**：每核一个 shard，shard 间无共享——share-nothing 架构
5. **流水线 + 批处理**：每个阶段攒批处理——减少非均摊开销

---

## 第36章：热路径 / 冷路径分离

### 36.1 正反示例

```cpp
// ❌ 热路径混入冷操作
void process_order_bad(int64_t id, double price) {
    ostringstream oss;      // 堆分配!
    oss << "[LOG] " << id;  // 格式化!
    cout << oss.str();      // IO!
    /* ...业务逻辑... */
}

// ✅ 热路径极简
HOT_FUNC FORCE_INLINE
void process_order(int64_t id, double price) {
    volatile double result = price * 1.001;  // 纯计算
    if (UNLIKELY(price <= 0))
        handle_error(id, price);  // NOINLINE, COLD_FUNC
}
```

### 36.2 热路径黄金法则

```
✅ 允许:
  纯计算、位操作、数组访问
  预分配内存的读写
  原子操作 (relaxed/acquire/release)
  inline 函数调用

❌ 禁止:
  malloc/new/delete    → 对象池/Arena
  string/ostringstream → 固定缓冲区
  cout/printf          → 异步日志
  mutex/condvar        → 无锁设计
  syscall (read/write) → 内核旁路/异步
  虚函数 (高频)        → CRTP/if constexpr
  throw                → 错误码/expected
```

### 深入扩展

1. **`__attribute__((section(".hot")))`**：将热函数放入特定 ELF section——与 BOLT 配合进一步优化 I-cache
2. **`__attribute__((flatten))`**：将函数内所有调用内联——hot 函数使用，消除调用开销
3. **函数多版本 (Function Multi-Versioning)**：`__attribute__((target("avx2")))` 热路径用 AVX2，冷路径保持 SSE
4. **冷路径异常处理**：`-fno-exceptions` + 手动 `setjmp/longjmp`——仅在冷路径上使用
5. **编译单元分离**：热路径代码编译为独立 `.o`——不同优化选项 + 链接时独立控制

---

## 第37章：预计算与查找表

### 37.1 运行时 LUT

```cpp
class PriceConverter {
    double tick_to_price_[65536];  // 启动时预计算
public:
    PriceConverter(double tick_size) {
        for (int i = 0; i < 65536; ++i)
            tick_to_price_[i] = i * tick_size;
    }
    FORCE_INLINE double tick_to_price(uint16_t tick) const {
        return tick_to_price_[tick];  // O(1) + 缓存命中
    }
};
```

### 37.2 编译期 LUT

```cpp
constexpr auto CRC32_TABLE = [] {
    std::array<uint32_t, 256> t{};
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j)
            crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
        t[i] = crc;
    }
    return t;
}();
// CRC32(1KB) ×100K → 编译期查找表零运行时初始化开销
```

### 37.3 查找表大小约束

```
查找表大小 << L1 缓存 (32-48 KB)
热查找表 < 4KB (一个 TLB 页)
→ 过大的查找表反而导致缓存驱逐，适得其反
```

### 深入扩展

1. **多层查找表**：`table1[field1]` → `table2[field2]` 两级索引——trading symbol → exchange → handler
2. **`consteval` 强化 (C++20)**：确保查找表构造在编译期完成——不可能退化为运行时
3. **Sin/Cos 查找表 + 插值**：360 条目 + 线性插值——精度足够且零 math.h 调用
4. **PerfectHash + LUT**：编译期完美哈希——O(1) 字符串查找，无冲突
5. **DPDK `rte_hash`**：基于 Cuckoo Hashing 的高性能查找表——用于 flow classification

---

## 第38章：性能监控与生产调优

### 38.1 监控指标

```
必须实时监控:
  端到端延迟: p50, p90, p99, p99.9, max
  吞吐量: messages/sec
  队列深度: SPSC queue 使用率
  CPU 使用率: 按核心

关键 perf 比率:
  IPC: > 2.0 (理想)
  L1 miss rate: < 1%
  LLC miss rate: < 5%
  Branch miss rate: < 1%
  Context switches: 0 (理想)
```

### 38.2 延迟陷阱清单

```
┌────────────────────┬──────────┬──────────────────────────┐
│ 陷阱               │ 延迟     │ 解决方案                 │
├────────────────────┼──────────┼──────────────────────────┤
│ malloc/new         │ 100ns-10K│ 对象池/Arena             │
│ 系统调用           │ 100-500ns│ 内核旁路/批量            │
│ page fault         │ 3-10 μs  │ mlockall + 预触摸        │
│ TLB miss           │ 10-100ns │ 大页                     │
│ L3 miss            │ 40-80ns  │ 预取 + 紧凑数据          │
│ 分支预测失败       │ 15-20cyc │ branchless + likely      │
│ 线程迁移           │ 5-50 μs  │ 绑核 + isolcpus          │
│ 定时器中断         │ ~4 μs    │ nohz_full                │
│ CPU 降频           │ ~100 μs  │ performance governor     │
│ C-State 唤醒       │ 2-100 μs │ 禁用深 C-State           │
│ NUMA 远程访问      │ +60ns    │ NUMA 绑定                │
│ 日志/IO            │ 1-100 μs │ 异步日志                 │
│ throw              │ 1-100 μs │ 错误码/expected          │
│ 虚函数             │ ~5ns     │ CRTP/去虚化              │
│ string 拷贝        │ 50-500ns │ string_view              │
│ 锁竞争             │ 100ns-μs │ 无锁设计                 │
└────────────────────┴──────────┴──────────────────────────┘
```

### 38.3 生产系统架构

```
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
│              ┌─ SPSC Queue ─┐                            │
│              ▼              │                             │
│  ┌──────────────────────────┐                            │
│  │  Logging (CPU 3)         │                            │
│  │  Monitor, Persistence    │                            │
│  └──────────────────────────┘                            │
│                                                          │
│  Config: mlockall, hugepages, isolcpus,                  │
│  SCHED_FIFO, performance governor, DPDK                  │
└──────────────────────────────────────────────────────────┘

延迟分解:
  Decode ~0.5μs → Strategy ~1-3μs → Encode ~0.5μs
  总延迟: 2-5μs
```

### 38.4 生产检查清单

```
□ 编译: -O2 -march=native -flto -fno-exceptions
□ 绑核: isolcpus + taskset + pthread_setaffinity
□ 频率: performance governor + C-State 禁用
□ 内存: mlockall + 大页 + 预分配 + 对象池
□ 网络: TCP_NODELAY + busy_poll + 内核旁路
□ 中断: IRQ 绑定 + irqbalance 关闭
□ 测量: rdtsc + 直方图 + p99.9
□ 热路径: 无分配 + 无锁 + 无 syscall + 无分支
□ 数据: 缓存行对齐 + false sharing 消除 + 预取
□ 调优: PGO + perf stat + 火焰图  
```

### 深入扩展

1. **Prometheus + Grafana 延迟监控**：p99.9 实时面板 + 告警——阈值 >10μs 触发调查
2. **A/B 延迟对比框架**：灰度发布时对比新旧版本的延迟分布——不只看 p50
3. **`perf kvm stat`**：虚拟化环境下的 VM-Exit 延迟——尽量不用虚拟机跑低延迟系统
4. **延迟热力图 (latency heatmap)**：时间 × 延迟 × 颜色深度——直观展示延迟毛刺模式
5. **故障注入测试**：主动注入 page fault / 网络延迟——验证系统在异常下的 p99.9 表现

---

## 附录 A：test13.cpp 扩展建议总结

| 章      | 扩展方向                                            | 优先级 | 关联   |
| ------- | --------------------------------------------------- | ------ | ------ |
| Ch1     | RDPMC 硬件计数器 / ARM CNTVCT / TSC 漂移校正        | 高     | test14 |
| Ch2     | HdrHistogram 完整实现 / 协调遗漏检测 / 多直方图合并 | 高     | —     |
| Ch3     | eBPF USDT probe / 火焰图差分 / CI 延迟回归          | 高     | —     |
| Ch4     | MESI 状态转移延迟实测 / perf c2c 实战               | 中     | test14 |
| Ch5     | 批量预取 / B-Tree 预取 / NTA 非临时预取             | 中     | —     |
| Ch6     | 1GB 大页 / THP 陷阱量化 / NUMA+大页                 | 高     | Ch13   |
| Ch7     | SLAB 分配器 / thread-local pool / pmr 标准分配器    | 高     | Ch8    |
| Ch8     | 分页 Arena / pmr::monotonic_buffer_resource         | 中     | Ch7    |
| Ch9     | AoSoA 混合布局 / ECS 架构 / std::simd               | 高     | Ch25   |
| Ch10    | cset shield / HT 分析 / K8s cpuManager              | 中     | —     |
| Ch11    | UMWAIT/TPAUSE / futex 内核路径 / eventfd            | 中     | —     |
| Ch12    | MCS Lock / CLH Lock / std::atomic_flag              | 高     | test11 |
| Ch13    | libnuma API / CXL 延迟 / NUMA balancing 分析        | 中     | —     |
| Ch14    | CMOV 生成分析 / BTB miss / perf branch-misses       | 中     | test14 |
| Ch15    | 批量 push_n/pop_n / 等待策略模板 / Disruptor        | 高     | Ch35   |
| Ch16    | 无分配 MPSC / Vyukov 队列 / MPMC bounded            | 高     | —     |
| Ch17    | SIMD 宽拷贝 / 双缓冲替代 / SeqLock + version        | 中     | —     |
| Ch18    | Tagged pointer / Hazard Pointers / EBR              | 高     | test11 |
| Ch19    | DPDK 完整示例 / AF_XDP / OpenOnload                 | 高     | test7  |
| Ch20-23 | BBR / QUIC / PTP / RSS / 校验和卸载                 | 中     | test7  |
| Ch24    | signal_fence / clobber 细节 / 函数级优化属性        | 中     | —     |
| Ch25    | AVX-512 降频 / Highway 库 / simdjson / SIMD TS      | 高     | test14 |
| Ch26    | PDEP/PEXT / CRC32 硬件 / AES-NI                     | 中     | —     |
| Ch27-28 | BOLT / AutoFDO / ThinLTO vs Full / Propeller        | 高     | —     |
| Ch29-33 | PREEMPT_RT / mlockall / tuned-adm / cgroup          | 高     | —     |
| Ch34    | io_uring 循环 / 无分配事件 / 定时器轮               | 高     | test9  |
| Ch35    | Disruptor 完整实现 / 反压 / Seastar                 | 高     | Ch15   |
| Ch36    | section(".hot") / flatten / 多版本函数              | 中     | —     |
| Ch37    | consteval LUT / PerfectHash / multi-level LUT       | 中     | test12 |
| Ch38    | Prometheus/Grafana / 延迟热力图 / 故障注入          | 高     | —     |

---

## 附录 B：典型输出示例

```
================================================================
 微秒低延迟系统 — C++ 实战完全教程
================================================================

╔══════════════════════════════════════════════════════╗
║ 一、延迟测量与基准篇                                  ║
╚══════════════════════════════════════════════════════╝

  ── 第1章: 高精度时钟与 rdtsc ──
  rdtsc 开销: 24 cycles
  chrono 开销: 21 ns
  TSC 频率校准中 (100ms)...
  1000次加法: 312 ns (856 cycles)

  ── 第2章: 延迟直方图与百分位统计 ──
  小循环延迟分布 (100K 次):
    样本数:  100000
    最小:    30 ns
    中位数:  52 ns
    p90:     68 ns
    p99:     125 ns
    p99.9:   850 ns
    最大:    9200 ns

╔══════════════════════════════════════════════════════╗
║ 二、内存子系统篇                                      ║
╚══════════════════════════════════════════════════════╝

  ── 第4章: 缓存行对齐与 false sharing ──
  sizeof(BadCounters):  16 bytes (可能 false sharing)
  sizeof(GoodCounters): 128 bytes (无 false sharing)
  BadCounters  (false sharing):  120.5 ms
  GoodCounters (无 false sharing): 28.3 ms

  ── 第7章: 对象池 — 零分配 ──
  new/delete ×100000: 12.4 ms
  ObjectPool  ×100000:  0.8 ms

  ── 第9章: SoA vs AoS ──
  AoS 位置更新: 3.52 ms
  SoA 位置更新: 1.23 ms

╔══════════════════════════════════════════════════════╗
║ 四、无锁数据结构篇                                    ║
╚══════════════════════════════════════════════════════╝

  ── 第15章: SPSC 环形队列 ──
  5000000 条消息: 45.2 ms
  吞吐量: 111 M ops/sec
  每操作: 9 ns
  正确性: ✓

================================================================
 演示完成
================================================================
```

---

## 附录 C：核心原则

> **热路径上绝不做：** `malloc` / `new` / `delete` / `std::string` / `cout` / `mutex` / 系统调用 / `throw` / 虚函数（高频）

> **目标延迟分解：** Network Decode (~0.5μs) → Strategy Logic (~1-3μs) → Order Encode (~0.5μs) = **总延迟 2-5μs**

> **低延迟系统的本质**：将所有可以在编译期/启动期/空闲期做的事移出热路径。热路径上只保留纯计算和预分配内存的读写。一切不确定延迟的来源——堆分配、系统调用、锁竞争、缓存 miss、分支预测失败、CPU 降频、操作系统中断——都必须被识别、量化、消除或隔离。测量是一切优化的前提：没有 p99.9 延迟数据的优化是盲人摸象。
