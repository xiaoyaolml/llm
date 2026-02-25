# 亚10微秒级低延迟系统 — C++ 实战完全教程

## 编译

```bash
# Linux (推荐)
g++ -std=c++17 -O2 -march=native -mtune=native -fno-exceptions -fno-rtti \
    -falign-functions=64 -pthread -lrt -o test13 test13.cpp

# Windows MSVC
cl /std:c++17 /O2 /EHsc /arch:AVX2 test13.cpp /link ws2_32.lib
```

## 运行

```bash
# Linux 最佳运行方式 (FIFO 实时调度 + CPU绑核)
sudo chrt -f 90 taskset -c 2 ./test13

# Windows
test13.exe
```

## 目录 (38 章)

### 一、延迟测量与基准篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 1 | 高精度时钟与 rdtsc | rdtsc/rdtscp, TSC 校准, 时钟选择指南 |
| 2 | 延迟直方图与百分位统计 | p50/p90/p99/p99.9, HdrHistogram, 协调遗漏 |
| 3 | 热路径识别与测量框架 | 零开销宏, 异步采集, perf/bpftrace |

### 二、内存子系统篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 4 | 缓存行对齐与 false sharing | alignas(64), perf c2c, MESI 协议 |
| 5 | 内存预取 (Prefetch) | __builtin_prefetch, 预取距离, 随机访问优化 |
| 6 | 大页 (Huge Pages) | 2MB/1GB 大页, TLB miss 消除, mmap/VirtualAlloc |
| 7 | 对象池 (Object Pool) | Freelist, O(1) 分配, 零分配热路径 |
| 8 | 竞技场分配器 (Arena) | 线性分配 ~2ns, O(1) reset, 零碎片 |
| 9 | SoA vs AoS | 内存布局对比, SIMD 友好, 缓存效率 |

### 三、CPU 与调度篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 10 | CPU 亲和性 (Core Pinning) | isolcpus, nohz_full, rcu_nocbs |
| 11 | 忙等待 vs 阻塞 | spin/pause/yield 唤醒延迟对比 |
| 12 | 自旋锁与退避 | TicketLock, 指数退避, TTAS |
| 13 | NUMA 感知编程 | 首次触摸策略, membind, 跨节点开销 |
| 14 | 分支预测优化 | likely/unlikely, branchless abs/min/max/clamp |

### 四、无锁数据结构篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 15 | SPSC 环形队列 (生产级) | 缓存行分离, cached head/tail, >100M ops/sec |
| 16 | MPSC 队列 | exchange-based push, 多生产者无锁入队 |
| 17 | SeqLock | 读写不对称, 写者零阻塞, MarketData 示例 |
| 18 | 无锁内存池 | CAS freelist, ABA 问题与解决方案 |

### 五、网络 I/O 篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 19 | 内核旁路 (Kernel Bypass) | DPDK, Solarflare, io_uring, FPGA |
| 20 | 零拷贝发送 | MSG_ZEROCOPY, DMA 直传 |
| 21 | TCP 低延迟调优 | TCP_NODELAY, QUICKACK, SO_BUSY_POLL |
| 22 | UDP 组播 | 市场数据分发, recvmmsg 批量接收 |
| 23 | 网卡时间戳 | 硬件时间戳, PTP 同步, 延迟审计 |

### 六、编译器与指令级优化篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 24 | 编译器屏障与暗示 | asm volatile, volatile, restrict, pure/const |
| 25 | SIMD 向量化 | SSE/AVX/AVX-512, 自动向量化, intrinsics |
| 26 | 内联汇编与 intrinsics | popcount, clz, ctz, 位操作加速 |
| 27 | PGO | 插桩→收集→优化, 10-40% 提速 |
| 28 | LTO | 跨翻译单元优化, ThinLTO |

### 七、系统级调优篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 29 | Linux 实时调度 | SCHED_FIFO, SCHED_DEADLINE |
| 30 | 中断亲和性 | IRQ 隔离, irqbalance |
| 31 | 内核参数 (sysctl) | 网络/内存/调度参数 |
| 32 | CPU 频率锁定 | performance governor, C-State 禁用 |
| 33 | BIOS 调优 | HT/Turbo/C-State/Snoop Mode |

### 八、架构设计篇

| 章节 | 主题 | 核心内容 |
|------|------|----------|
| 34 | 单线程事件循环 | 无锁无切换, HFT 引擎架构 |
| 35 | 流水线架构 | 多阶段 + SPSC, Disruptor 模式 |
| 36 | 热路径/冷路径分离 | HOT/COLD 函数, 热路径黄金法则 |
| 37 | 预计算与查找表 | 编译期 CRC32 表, 价格转换 LUT |
| 38 | 性能监控与生产调优 | 延迟陷阱清单, 生产检查清单, 系统架构 |

## 核心原则

> **热路径上绝不做:**
> `malloc` / `new` / `delete` / `std::string` / `cout` / `mutex` / 系统调用 / `throw` / 虚函数(高频)

> **目标延迟分解:**
> Network Decode (~0.5μs) → Strategy Logic (~1-3μs) → Order Encode (~0.5μs) = **总延迟 2-5μs**

## 延迟量级参考

| 操作 | 延迟 |
|------|------|
| L1 cache hit | ~1 ns |
| L2 cache hit | ~4 ns |
| L3 cache hit | ~12 ns |
| DRAM 访问 | ~80 ns |
| 原子 CAS (无竞争) | ~5-10 ns |
| 原子 CAS (有竞争) | ~20-100 ns |
| syscall (getpid) | ~100 ns |
| mutex lock (无竞争) | ~25 ns |
| mutex lock (有竞争) | ~100-10000 ns |
| 线程上下文切换 | ~1-5 μs |
| 条件变量唤醒 | ~5-15 μs |
| malloc (小对象) | ~100-500 ns |
| page fault | ~3-10 μs |
| TCP 发送 (lo) | ~5-10 μs |
| DPDK 收发 | ~0.5-2 μs |
