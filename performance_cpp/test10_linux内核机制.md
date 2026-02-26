# 深入理解 Linux 内核机制：调度、内存管理、网络栈、驱动 — 详细指导教程

> **定位**：本教程对应 `test10.cpp`（2278 行，24 章），从用户态 C++ 视角深入剖析 Linux 内核七大子系统的核心原理、数据结构与算法。通过可编译运行的代码示例（CFS 模拟器、伙伴系统、Slab 分配器、epoll 演示等）将抽象的内核知识具象化。与 `test7.cpp`（网络编程用户态）、`test9.cpp`（高并发架构）互补，聚焦"内核到底在做什么"。

---

## 编译与运行

```bash
# Linux — GCC / Clang（完整功能）
g++ -std=c++17 -O2 -pthread -o test10 test10.cpp && ./test10

# Windows — MSVC（自动跳过 Linux API，模拟器正常运行）
cl /std:c++17 /O2 /EHsc test10.cpp && test10.exe
```

> Windows 下通过 `#ifdef _WIN32` / `LINUX_ONLY` 宏自动切换：内核原理讲解全量输出，CFS / Buddy / Slab 模拟器可正常运行，`/proc` 读取等 Linux 特有 API 输出占位信息。

---

## 目录总览

| 篇 | 章 | 主题 | 核心技术 |
|---|---|---|---|
| **一、进程调度** | 1 | CFS 完全公平调度器 | vruntime / 红黑树 / 权重计算 / 模拟器 |
| | 2 | 实时调度策略 | SCHED_FIFO / RR / DEADLINE / RT 限制 |
| | 3 | CPU 亲和性与 NUMA | sched_setaffinity / NUMA 拓扑 / 远程延迟 |
| | 4 | cgroups CPU 限制 | v1/v2 对比 / cpu.max / Docker 映射 |
| **二、内存管理** | 5 | 虚拟内存与分页 | 四级页表 / TLB / 大页 / 地址空间 |
| | 6 | 伙伴系统 | Buddy Allocator 模拟 / 分配 / 合并 |
| | 7 | SLAB/SLUB 分配器 | per-CPU 缓存 / SimpleSlab 模拟 |
| | 8 | mmap 与零拷贝 | MAP 标志 / madvise / 匿名映射 |
| | 9 | MMIO 与 DMA | 端口 I/O → MMIO → DMA / IOMMU |
| | 10 | OOM Killer | oom_score / overcommit / PSI |
| **三、文件系统与 I/O** | 11 | VFS 虚拟文件系统 | superblock / inode / dentry / file |
| | 12 | Page Cache 与 I/O 调度 | 脏页回写 / 调度器对比 |
| | 13 | io_uring | SQ/CQ 共享环 / 零系统调用 / liburing |
| **四、网络栈** | 14 | TCP/IP 协议栈 | RX/TX 完整路径 / NAPI / GRO / 拥塞控制 |
| | 15 | sk_buff | 内存布局 / push/pull/put 操作 |
| | 16 | Netfilter/iptables | 五钩子 / 四表 / conntrack |
| | 17 | epoll 内核实现 | 红黑树 + 就绪链表 / ep_poll_callback / ET/LT |
| | 18 | 零拷贝网络 | sendfile / splice / MSG_ZEROCOPY |
| **五、设备驱动** | 19 | Linux 设备模型 | 字符/块/网络设备 / cdev / udev |
| | 20 | 中断处理 | 上半部/下半部 / softirq / tasklet / workqueue |
| **六、内核同步** | 21 | 内核同步原语 | spinlock / RCU / seqlock / mutex 对比 |
| **七、综合实战** | 22 | /proc 与 /sys 信息采集 | CPU / 内存 / 网络 / 进程信息 |
| | 23 | 性能分析工具链 | perf / ftrace / bpftrace / strace |
| | 24 | 内核调优参数速查 | 调度 / 内存 / 网络 / 文件系统 sysctl |

---

## 文件关系与知识图谱

```
test6.cpp (多线程基础)          test9.cpp (高并发架构)
    │ mutex/thread/async            │ lock-free/pool/actor
    │                               │
    └───────────┬───────────────────┘
                │
                ▼
        test10.cpp (Linux 内核机制) ◄── 本文件
                │
    ┌───────────┼───────────────┬──────────────┐
    │           │               │              │
    ▼           ▼               ▼              ▼
 test7.cpp   test11.cpp    test14.cpp     test13.cpp
 (网络编程)  (无锁深入)   (CPU微架构)   (亚10μs系统)
   │           │               │              │
   │ epoll     │ RCU/         │ cache/       │ 低延迟
   │ TCP       │ memory       │ pipeline     │ 实时
   │ sendfile  │ ordering     │ NUMA         │ scheduling
   └───────────┴───────────────┴──────────────┘

内核子系统关系:
┌──────────────────────────────────────────────────────────┐
│                    用户空间                               │
│  系统调用接口 (syscall)                                   │
├──────────────────────────────────────────────────────────┤
│           VFS (Ch11)        │ Socket Layer (Ch14-17)     │
│              │              │       │                     │
│         Page Cache (Ch12)   │   TCP/IP Stack (Ch14)      │
│              │              │       │                     │
│     I/O 调度器 → io_uring   │   Netfilter (Ch16)        │
│           (Ch12-13)         │       │                     │
├──────────────────────────────────────────────────────────┤
│  内存管理 (Ch5-10)          │ 设备驱动 (Ch19-20)         │
│  虚拟内存 → 伙伴系统       │ 字符/块/网络设备            │
│  → SLAB/SLUB               │ 中断半部 → softirq          │
├──────────────────────────────────────────────────────────┤
│  进程调度 (Ch1-4)    │ 内核同步 (Ch21)                   │
│  CFS/RT/DEADLINE     │ spinlock/RCU/seqlock              │
├──────────────────────────────────────────────────────────┤
│                    硬件层                                 │
│  CPU (多核/NUMA) │ 内存 (DRAM/TLB) │ NIC/磁盘/GPU       │
└──────────────────────────────────────────────────────────┘
```

---

## 公共工具

```cpp
#ifdef _WIN32
#define LINUX_ONLY 0
#else
#define LINUX_ONLY 1
// Linux 特有头文件: unistd.h, sys/mman.h, sys/epoll.h, sched.h, ...
#endif

using namespace std::chrono_literals;

static void print_header(const char* title) {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║ " << std::left << std::setw(40) << title << " ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";
}

static std::string read_proc_file(const std::string& path) {
#if LINUX_ONLY
    std::ifstream ifs(path);
    if (!ifs) return "(无法读取 " + path + ")";
    return std::string((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
#else
    return "(仅 Linux 可用)";
#endif
}

static std::string read_first_line(const std::string& path) {
    std::string content = read_proc_file(path);
    auto pos = content.find('\n');
    if (pos != std::string::npos) content = content.substr(0, pos);
    return content;
}
```

> **跨平台策略**：`LINUX_ONLY` 宏控制条件编译。原理讲解（`std::cout << R"(...)"` 原始字符串）和算法模拟器在所有平台运行；`/proc` 读取、`epoll` 演示等仅在 Linux 执行。

---

## 第1章：CFS 完全公平调度器 — 原理与模拟

### 1.1 CFS 核心原理

```
CFS (Completely Fair Scheduler) — Linux 2.6.23+ 默认调度器

核心思想:
  每个进程维护 vruntime (虚拟运行时间)
  vruntime 最小的进程获得 CPU → 红黑树最左节点 O(1) 选取

vruntime 计算:
  vruntime += delta_exec × (NICE_0_WEIGHT / weight)

  nice = 0   → weight = 1024  → vruntime 正常增长
  nice = -20 → weight = 88761 → vruntime 增长很慢 (更多 CPU)
  nice = 19  → weight = 15    → vruntime 增长很快 (更少 CPU)

内核数据结构:
  struct sched_entity {
      u64 vruntime;                    // 虚拟运行时间
      struct rb_node run_node;         // 红黑树节点
  };
  struct cfs_rq {
      struct rb_root_cached tasks_timeline; // 红黑树
      u64 min_vruntime;                     // 单调递增基准
  };
```

### 1.2 CFS 模拟器实现

```cpp
namespace ch1 {

class CFSSimulator {
public:
    struct Task {
        int pid;
        std::string name;
        int nice;              // -20 ~ 19
        double weight;         // 由 nice 计算
        double vruntime;       // 虚拟运行时间
        double total_runtime;  // 实际运行总时间
        bool operator>(const Task& o) const { return vruntime > o.vruntime; }
    };

private:
    // 优先队列模拟红黑树（最小 vruntime 在顶部）
    std::priority_queue<Task, std::vector<Task>, std::greater<Task>> rq_;
    double min_vruntime_ = 0;

    static double nice_to_weight(int nice) {
        // 简化: weight = 1024 × 1.25^(-nice)
        return 1024.0 * std::pow(1.25, -nice);
    }

    double calc_timeslice(const Task& task, double total_weight, int nr_running) {
        double sched_period = std::max(6.0, (double)nr_running * 0.75); // ms
        return sched_period * (task.weight / total_weight);
    }

public:
    void add_task(int pid, const std::string& name, int nice) {
        Task t{pid, name, nice, nice_to_weight(nice), min_vruntime_, 0};
        rq_.push(t);
    }

    void simulate(int total_ticks) {
        // 每个 tick：选最小 vruntime 任务 → 运行时间片 → 更新 vruntime → 放回
        for (int tick = 0; tick < total_ticks; ++tick) {
            Task current = rq_.top(); rq_.pop();
            int nr = rq_.size() + 1;
            double timeslice = calc_timeslice(current, /*...*/);
            current.total_runtime += timeslice;
            current.vruntime += timeslice * (1024.0 / current.weight);
            rq_.push(current);
        }
        // 输出各进程实际运行时间占比
    }
};

void demo_cfs() {
    CFSSimulator cfs;
    cfs.add_task(1001, "web-server",  0);   // 普通优先级
    cfs.add_task(1002, "database",   -5);   // 稍高优先级
    cfs.add_task(1003, "logger",     10);   // 低优先级
    cfs.add_task(1004, "critical",  -15);   // 很高优先级
    cfs.add_task(1005, "background", 15);   // 很低优先级
    cfs.simulate(500);
}

} // namespace ch1
```

### 1.3 CFS 调度可视化

```
红黑树 (按 vruntime 排序):
                    [web:50ms]
                   /           \
         [critical:12ms]    [logger:120ms]
              /                     \
    [db:28ms]                [background:180ms]
        ↑                              ↑
    vruntime 最小               vruntime 最大
    下一个运行                  等待最久

vruntime 增长速度 (500 个调度周期后):
  critical (nice=-15): ████                     12%  → 最多 CPU
  database (nice=-5):  ████████                 28%
  web-server (nice=0): ████████████             35%
  logger (nice=10):    ██                        8%
  background (nice=15):█                         4%  → 最少 CPU
```

### 1.4 CFS 可调参数

```
/proc/sys/kernel/sched_latency_ns = 6000000 (6ms)
  调度延迟目标：所有可运行进程在此时间内至少运行一次
  进程越多 → 每个时间片越短

/proc/sys/kernel/sched_min_granularity_ns = 750000 (0.75ms)
  最小时间片：即使进程很多也不低于此值
  当 nr_running × min_gran > latency → 周期自动延长

/proc/sys/kernel/sched_wakeup_granularity_ns = 1000000 (1ms)
  唤醒粒度：新唤醒进程 vruntime 必须比当前进程小至少此值才能抢占
  减少不必要的上下文切换

低延迟调优:
  echo 1000000 > /proc/sys/kernel/sched_latency_ns
  echo 100000  > /proc/sys/kernel/sched_min_granularity_ns
```

### 深入扩展

1. **EEVDF 调度器 (Linux 6.6+)**：Earliest Eligible Virtual Deadline First，替代纯 CFS，引入 deadline 概念，对延迟敏感任务更友好
2. **调度域 (Sched Domain)**：多级域结构（SMT → Core → Package → NUMA），负载均衡在域间逐级迁移
3. **组调度 (Group Scheduling)**：cgroup + CFS 实现容器间公平，每个 cgroup 作为一个调度实体参与 vruntime 竞争
4. **`sched_ext` (Linux 6.12+)**：用 eBPF 编写自定义调度器，可在用户态快速迭代调度策略
5. **NUMA 调度平衡**：`task_numa_work()` 定期扫描内存访问模式，自动迁移任务到数据所在 NUMA 节点

---

## 第2章：实时调度策略

### 2.1 调度策略层次

```
Linux 调度策略层次:
┌──────────────────────────────────────────────────────────┐
│ 优先级 99 ── SCHED_FIFO / SCHED_RR      (实时 RT 类)   │
│   ...                                                    │
│ 优先级  1 ── SCHED_FIFO / SCHED_RR      (实时 RT 类)   │
│ ──────── 实时分界线 ──────────────────────────────────── │
│ nice -20 ── SCHED_OTHER (CFS)            (普通 FAIR 类) │
│   ...                                                    │
│ nice  19 ── SCHED_OTHER (CFS)            (普通 FAIR 类) │
│ ──────── 空闲分界线 ──────────────────────────────────── │
│          ── SCHED_IDLE / SCHED_BATCH     (批处理/空闲)  │
└──────────────────────────────────────────────────────────┘
```

### 2.2 三种实时策略

| 策略 | 原理 | 关键参数 | 适用场景 |
|---|---|---|---|
| `SCHED_FIFO` | 先进先出，同优先级不抢占 | priority (1-99) | 音频、工控 |
| `SCHED_RR` | 同优先级按时间片轮转 | 默认 100ms 时间片 | 多 RT 任务 |
| `SCHED_DEADLINE` | 基于 EDF 保证截止时间 | runtime/deadline/period | 硬实时 |

```
SCHED_DEADLINE 参数示例:
  runtime  = 10ms   每周期内保证的 CPU 时间
  deadline = 30ms   必须在此之前获得 runtime
  period   = 100ms  调度周期

  时间轴: |───10ms运行───|─────等待─────|──────────|
          ← deadline 30ms →
          ←────────── period 100ms ──────────────→
```

**RT 安全机制**：

```
/proc/sys/kernel/sched_rt_runtime_us = 950000 (默认)
/proc/sys/kernel/sched_rt_period_us  = 1000000

→ RT 进程每 1 秒最多使用 0.95 秒 CPU
→ 预留 5% 给普通进程，防止 RT 进程饿死系统
```

### 深入扩展

1. **PREEMPT_RT 补丁**：将 spinlock 替换为 rt_mutex（可睡眠），中断线程化，实现微秒级确定性延迟
2. **CPU 隔离 (`isolcpus`)**：从调度器完全隔离特定核心，仅运行绑定任务，消除调度抖动
3. **`chrt` 工具**：`chrt -f 50 ./app` 以 SCHED_FIFO 优先级 50 运行程序
4. **cyclictest 基准**：RT 延迟测试工具，衡量调度确定性（目标 < 10μs）
5. **优先级反转防护**：rt_mutex 支持优先级继承（低优先级持锁时临时提升到等待者优先级）

---

## 第3章：CPU 亲和性与 NUMA

### 3.1 CPU 亲和性 API

```cpp
#if LINUX_ONLY
    // 获取 CPU 数量
    int ncpus = sysconf(_SC_NPROCESSORS_ONLN);

    // 获取当前进程 CPU 亲和性
    cpu_set_t mask;
    CPU_ZERO(&mask);
    sched_getaffinity(0, sizeof(mask), &mask);

    // 绑定到 CPU 0
    cpu_set_t new_mask;
    CPU_ZERO(&new_mask);
    CPU_SET(0, &new_mask);
    sched_setaffinity(0, sizeof(new_mask), &new_mask);

    // 线程级: pthread_setaffinity_np(thread, ...)
#endif
```

### 3.2 NUMA 架构

```
NUMA (Non-Uniform Memory Access):
┌──────────────┐          ┌──────────────┐
│  NUMA Node 0 │───QPI───│  NUMA Node 1 │
│ CPU 0,1,2,3  │          │ CPU 4,5,6,7  │
│  内存 32GB   │          │  内存 32GB   │
└──────────────┘          └──────────────┘

本地内存: ~80ns    远程内存: ~130ns  (延迟增加 60%)

NUMA 感知编程:
  numactl --cpunodebind=0 --membind=0 ./myapp   # 绑定 node 0
  set_mempolicy(MPOL_BIND, ...)                  # 代码中设置
  mbind(addr, len, MPOL_BIND, ...)               # 绑定内存区域

查看拓扑: numactl -H 或 lscpu
```

### 深入扩展

1. **`taskset` vs `numactl`**：`taskset` 仅绑 CPU，`numactl` 同时绑 CPU + 内存，NUMA 场景必须用后者
2. **IRQ 亲和性**：`/proc/irq/<N>/smp_affinity` 控制中断分发到哪些核心，网卡多队列 + IRQ 绑核是万兆网络必需
3. **`perf stat -e node-loads,node-load-misses`**：量化 NUMA 远程访问比例
4. **AutoNUMA (numa_balancing)**：内核自动检测内存访问模式并迁移页面，`/proc/sys/kernel/numa_balancing`
5. **内存交错 (`interleave`)**：对不确定访问模式的数据，`numactl --interleave=all` 均匀分布到所有节点

---

## 第4章：cgroups CPU 限制

### 4.1 cgroup v2 CPU 控制

```
/sys/fs/cgroup/mygroup/
  cpu.max       — "quota period"        例: "50000 100000" → 50% CPU
  cpu.weight    — 相对权重 (1-10000)    默认 100
  cgroup.procs  — 属于此组的进程 PID
  cpu.stat      — 统计 (usage_usec, ...)

操作:
  mkdir /sys/fs/cgroup/mygroup
  echo "50000 100000" > /sys/fs/cgroup/mygroup/cpu.max    # 50% CPU
  echo $PID > /sys/fs/cgroup/mygroup/cgroup.procs          # 加入进程

Docker 映射:
  docker run --cpus=1.5      → cpu.max = "150000 100000"
  docker run --cpu-shares=512 → cpu.weight ≈ 50
```

### 4.2 cgroup v1 vs v2

```
           cgroup v1                      cgroup v2
┌──────────────────────────────┬──────────────────────────────┐
│ 多层级树 (每个控制器一棵树)   │ 单统一层级树                  │
│ cpu/memory/io 独立            │ 所有控制器在同一树             │
│ /sys/fs/cgroup/cpu/           │ /sys/fs/cgroup/               │
│ cpu.cfs_quota_us              │ cpu.max                       │
│ cpu.shares                    │ cpu.weight                    │
│ 可能冲突/不一致               │ 原子性配置                    │
└──────────────────────────────┴──────────────────────────────┘
```

### 深入扩展

1. **cgroup v2 PSI (Pressure Stall Information)**：`/sys/fs/cgroup/mygroup/cpu.pressure` 查看 CPU 压力百分比
2. **内存 cgroup**：`memory.max` 限制内存上限，`memory.swap.max` 限制 swap
3. **I/O cgroup**：`io.max` 限制磁盘带宽，`io.weight` 设置 I/O 权重
4. **Kubernetes 资源限制**：`resources.limits.cpu: "2"` → cgroup cpu.max，`requests.cpu` → cpu.weight
5. **systemd slice**：systemd 自动为每个服务创建 cgroup，`systemd-cgtop` 实时监控

---

## 第5章：虚拟内存与分页

### 5.1 x86-64 四级页表

```
虚拟地址 (48位有效):
┌────────┬────────┬────────┬────────┬────────────┐
│ PML4   │ PDPT   │  PD    │  PT    │  Offset    │
│ 9 bits │ 9 bits │ 9 bits │ 9 bits │  12 bits   │
└───┬────┴───┬────┴───┬────┴───┬────┴────────────┘
    │        │        │        │
    ▼        ▼        ▼        ▼
  PML4    PDPT表    PD表    PT表 ──→ 4KB 物理页
 (512项)  (512项)  (512项) (512项)

可寻址: 2^48 = 256 TB (用户态 128TB + 内核态 128TB)

Linux 5.x+: 五级页表 (PML5) → 2^57 = 128 PB
```

### 5.2 TLB 层次

```
TLB (Translation Lookaside Buffer):
  L1 TLB: ~64 条目,  1 cycle
  L2 TLB: ~1536 条目, ~7 cycles
  TLB miss → 走页表 → 20-200 cycles (最坏4次内存访问)

大页 (Huge Pages):
  2MB 大页 → 少一级页表 → TLB 利用率提升 512 倍
  1GB 大页 → 更少 TLB miss，适合大内存数据库
```

### 5.3 进程地址空间演示

```cpp
#if LINUX_ONLY
    int stack_var = 42;
    static int data_var = 42;
    int* heap_var = new int(42);

    // 地址空间分布:
    //   main()     代码段: 0x55xxxx...
    //   data_var   数据段: 0x55xxxx...
    //   heap_var   堆:     0x55xxxx...
    //   stack_var  栈:     0x7fxxxx...

    // /proc/self/maps 查看完整映射
    // /proc/self/statm 查看内存统计 (VmSize, RSS, Shared)
#endif
```

### 深入扩展

1. **透明大页 (THP)**：`/sys/kernel/mm/transparent_hugepage/enabled`，内核自动合并 4KB 页为 2MB 大页，但可能导致延迟抖动
2. **KPTI (Kernel Page Table Isolation)**：Meltdown 补丁，内核和用户态使用不同页表，每次 syscall 切换页表增加 ~5% 开销
3. **`/proc/<pid>/pagemap`**：查看虚拟页到物理帧的映射，可用于分析 NUMA 页面分布
4. **`userfaultfd`**：用户态缺页处理，VM 热迁移利器
5. **Memory Protection Keys (MPK)**：x86 硬件级内存保护，无需修改页表即可更改权限

---

## 第6章：伙伴系统 (Buddy System) 模拟

### 6.1 完整模拟实现

```cpp
namespace ch6 {

class BuddyAllocator {
    static constexpr int MAX_ORDER = 10; // 2^10 = 1024 页 = 4MB
    std::vector<std::set<size_t>> free_lists_; // 每阶空闲链表
    size_t total_pages_;

public:
    explicit BuddyAllocator(size_t total_pages) : total_pages_(total_pages) {
        free_lists_.resize(MAX_ORDER + 1);
        // 所有页面放入最大阶
        for (size_t i = 0; i + (1 << MAX_ORDER) <= total_pages; i += (1 << MAX_ORDER))
            free_lists_[MAX_ORDER].insert(i);
    }

    int allocate(int order) {
        // 从 order 向上查找空闲块
        for (int i = order; i <= MAX_ORDER; ++i) {
            if (!free_lists_[i].empty()) {
                size_t block = *free_lists_[i].begin();
                free_lists_[i].erase(free_lists_[i].begin());
                // 拆分：大块拆成两个伙伴，低半留用，高半放入低一阶链表
                while (i > order) {
                    --i;
                    size_t buddy = block + (1 << i);
                    free_lists_[i].insert(buddy);
                }
                return block;
            }
        }
        return -1; // OOM
    }

    void deallocate(size_t block, int order) {
        // 尝试合并伙伴
        while (order < MAX_ORDER) {
            size_t buddy = block ^ (1 << order); // XOR 计算伙伴地址
            if (free_lists_[order].count(buddy)) {
                free_lists_[order].erase(buddy);
                block = std::min(block, buddy); // 合并取低地址
                ++order;
            } else {
                break;
            }
        }
        free_lists_[order].insert(block);
    }
};

} // namespace ch6
```

### 6.2 分配与合并过程

```
初始: 1024 页 = 1 个 order-10 块

分配 order-0 (1页=4KB):
  order-10 拆分: [0..511] [512..1023]   → 512 放入 order-9
  order-9  拆分: [0..255] [256..511]    → 256 放入 order-8
  ...一路拆分...
  order-0: 返回页 0                     → 页 1 放入 order-0

释放页 0 (order-0):
  伙伴 = 0 XOR 1 = 1
  页 1 空闲? ✓ → 合并为 order-1 块 [0..1]
  伙伴 = 0 XOR 2 = 2
  页 2 空闲? ✓ → 合并为 order-2 块 [0..3]
  ...递归合并直到伙伴不空闲...

查看: cat /proc/buddyinfo
  Node 0, zone Normal  128 64 32 16 8 4 2 1 0 0 1
  → order 0: 128 × 4KB 空闲, order 1: 64 × 8KB 空闲, ...
```

### 6.3 伙伴地址计算

```
关键公式: buddy_addr = block_addr XOR (1 << order)

示例 (order=2, 即 4 页为一块):
  块 0  的伙伴: 0  XOR 4  = 4   → [0..3] 和 [4..7] 是伙伴
  块 4  的伙伴: 4  XOR 4  = 0   → 对称
  块 8  的伙伴: 8  XOR 4  = 12  → [8..11] 和 [12..15] 是伙伴

性质: XOR 保证两个伙伴合并后的起始地址是 2^(order+1) 对齐的
```

### 深入扩展

1. **内存区域 (Zone)**：ZONE_DMA (0-16MB) / ZONE_DMA32 (0-4GB) / ZONE_NORMAL / ZONE_MOVABLE，伙伴系统按 zone 独立管理
2. **内存碎片化**：长时间运行后外碎片增加，内核使用 compaction（kcompactd）迁移可移动页合并大块
3. **MIGRATE 类型**：UNMOVABLE / MOVABLE / RECLAIMABLE，内核按迁移特性归类页面，延缓碎片化
4. **GFP 标志**：`GFP_KERNEL`（可睡眠）/ `GFP_ATOMIC`（中断上下文）/ `GFP_DMA` 等控制分配行为
5. **`/proc/pagetypeinfo`**：查看每个 zone 每种迁移类型的伙伴系统状态

---

## 第7章：SLAB/SLUB 分配器

### 7.1 架构层次

```
用户接口:  kmalloc(64) / kfree()
               │
               ▼
对象缓存:  kmem_cache
           ┌────────────────────────┐
           │ name: "task_struct"    │
           │ object_size: 6592     │
           │ per-cpu: slub_page[]  │ ← 每 CPU 本地缓存 (无锁!)
           │ partial: list         │ ← 部分使用的 slab
           │ full: list            │ ← 完全使用的 slab
           └────────────────────────┘
               │
               ▼
页面分配:  Buddy System (伙伴系统)
```

### 7.2 SLUB 分配流程

```
分配:
  ① per-cpu 空闲链表取对象 → 无锁 O(1) ✓
  ② 空了 → 从 partial list 取一个 slab → 加锁
  ③ 还空 → 从伙伴系统申请新页面

SLAB vs SLUB vs SLOB:
┌──────┬──────────────────┬──────────────────┬──────────────┐
│      │ SLAB (经典)       │ SLUB (现代默认)   │ SLOB (嵌入式) │
├──────┼──────────────────┼──────────────────┼──────────────┤
│ 复杂度│ 高 (着色对齐)     │ 中 (简化设计)     │ 最简          │
│ 调试  │ 有限             │ 优秀 (SLUB_DEBUG) │ 无            │
│ 适用  │ 已弃用           │ 所有系统          │ RAM<64MB      │
└──────┴──────────────────┴──────────────────┴──────────────┘

查看: cat /proc/slabinfo 或 slabtop -s c
```

### 7.3 简化 Slab 模拟

```cpp
class SimpleSlab {
    size_t obj_size_, slab_size_, objs_per_slab_;
    std::vector<std::vector<char>> slabs_;
    std::vector<void*> free_list_;

public:
    SimpleSlab(size_t obj_size, size_t slab_pages = 1)
        : obj_size_(obj_size), slab_size_(slab_pages * 4096) {
        objs_per_slab_ = slab_size_ / obj_size_;
        grow();
    }

    void* alloc() {
        if (free_list_.empty()) grow();
        void* obj = free_list_.back();
        free_list_.pop_back();
        return obj;
    }

    void free(void* obj) { free_list_.push_back(obj); }

private:
    void grow() {
        slabs_.emplace_back(slab_size_);
        char* base = slabs_.back().data();
        for (size_t i = 0; i < objs_per_slab_; ++i)
            free_list_.push_back(base + i * obj_size_);
    }
};

// 使用:
SimpleSlab slab_128(128);   // 模拟 kmalloc-128
void* obj = slab_128.alloc();
slab_128.free(obj);
```

### 深入扩展

1. **SLUB 合并 (slab_merge)**：多个相同大小的 kmem_cache 共享底层 slab，减少内存浪费
2. **SLAB 着色 (Coloring)**：不同 slab 的对象错开 cache line 偏移，减少缓存集合冲突
3. **`kmalloc` 大小类**：8, 16, 32, 64, 96, 128, 192, 256, 512, 1024, 2048, 4096, 8192——每种大小一个缓存
4. **用户态 Slab**：Jemalloc / TCMalloc 采用类似思想，per-thread 缓存 + 中央池
5. **KASAN (Kernel Address Sanitizer)**：利用 SLUB 红区检测内核内存越界

---

## 第8章：mmap 与零拷贝

### 8.1 mmap vs read 对比

```
传统 read():
  磁盘 → DMA → 内核缓冲区 (Page Cache) → CPU拷贝 → 用户缓冲区
  ① 一次数据拷贝 + ② 两次上下文切换

mmap():
  磁盘 → DMA → Page Cache ← 用户直接访问 (虚拟地址映射)
  零拷贝！按需触发缺页异常加载
```

### 8.2 mmap 标志

| 标志 | 含义 | 典型场景 |
|---|---|---|
| `MAP_PRIVATE` | 私有映射 (COW) | 读取文件 |
| `MAP_SHARED` | 共享映射（写回文件） | 进程间共享 |
| `MAP_ANONYMOUS` | 匿名映射（无文件） | malloc 大块分配 |
| `MAP_HUGETLB` | 使用大页 | 数据库 |
| `MAP_POPULATE` | 预加载所有页面 | 避免后续缺页 |

```cpp
#if LINUX_ONLY
    // 文件映射
    void* addr = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
    // 匿名映射
    void* anon = mmap(nullptr, 4096, PROT_READ|PROT_WRITE,
                       MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    // madvise 优化提示
    madvise(addr, length, MADV_SEQUENTIAL);  // 顺序访问 → 多预读
    madvise(addr, length, MADV_RANDOM);      // 随机访问 → 少预读
    madvise(addr, length, MADV_WILLNEED);    // 提前加载
    madvise(addr, length, MADV_DONTNEED);    // 可以释放
    madvise(addr, length, MADV_HUGEPAGE);    // 建议大页
#endif
```

### 深入扩展

1. **`MAP_POPULATE` + `MAP_LOCKED`**：预加载且锁定在内存中，防止被换出，适合实时系统
2. **`remap_file_pages`（已废弃）**：非线性映射，被 `MAP_FIXED` 多次映射替代
3. **`memfd_create` + 密封 (Sealing)**：创建匿名内存文件，可通过 fd 传递给其他进程
4. **DAX (Direct Access)**：NVMe/持久内存设备直接映射，绕过 Page Cache，`mmap` 直接操作存储介质
5. **Copy-on-Write 优化**：`fork()` 后父子共享所有页面，写时才拷贝，`madvise(MADV_WIPEONFORK)` 安全清除敏感数据

---

## 第9章：MMIO 与 DMA

### 9.1 I/O 方式演进

```
1. 端口 I/O (Port I/O) — x86 传统
   in/out 指令，独立 I/O 地址空间 (0x0000-0xFFFF)

2. MMIO (内存映射 I/O) — 现代主流
   设备寄存器映射到物理地址空间，CPU 用 load/store 访问
   ┌──────────────────────────────────────┐
   │ 0x0000_0000 ─ 0x3FFF_FFFF: DRAM     │
   │ 0xFE00_0000 ─ 0xFEFF_FFFF: PCIe BAR │ ← MMIO
   │ 0xFF00_0000 ─ 0xFFFF_FFFF: 固件     │
   └──────────────────────────────────────┘

3. DMA (Direct Memory Access) — 高吞吐
   设备直接读写主存，不经 CPU
   CPU ← → RAM ← DMA → NIC/GPU
```

### 9.2 DMA 关键概念

```
一致性 DMA (coherent):   CPU 和设备看到相同内存
流式 DMA (streaming):    需要手动同步 (dma_map/unmap)
IOMMU:                   设备的"MMU"，隔离地址空间
Scatter-Gather DMA:      一次传输多个不连续内存块

内核 DMA API:
  dma_alloc_coherent(dev, size, &handle, GFP_KERNEL)
  dma_map_single(dev, vaddr, size, DMA_TO_DEVICE)

用户态 DMA:
  DPDK  — 网络零拷贝
  SPDK  — 存储零拷贝
  RDMA  — InfiniBand/RoCE 远程 DMA
```

### 深入扩展

1. **VFIO (Virtual Function I/O)**：安全地将设备直通给用户态进程/VM，配合 IOMMU 实现隔离
2. **DMA 映射一致性问题**：ARM 等弱一致性架构需要显式 `dma_sync_*` 操作
3. **PCIe BAR 映射**：通过 `pci_iomap()` 将设备 BAR 空间映射到内核虚拟地址
4. **CMA (Contiguous Memory Allocator)**：为 DMA 设备预留大块连续物理内存
5. **P2P DMA**：GPU ↔ NVMe 直接数据传输，不经 CPU/主存（GPUDirect Storage）

---

## 第10章：OOM Killer 与内存压力

### 10.1 OOM 评分机制

```
/proc/<pid>/oom_score      — 当前分数 (越高越容易被杀)
/proc/<pid>/oom_score_adj  — 调整值 (-1000 ~ 1000)
  -1000 = 永不被杀
   1000 = 优先被杀
      0 = 默认

评分因素:
  ① 进程 RSS (越大分越高)
  ② 子进程内存
  ③ oom_score_adj 调整
  ④ root 进程 3% 优惠

保护关键进程:
  echo -1000 > /proc/<pid>/oom_score_adj   # 永不被杀 (如数据库)
  echo  500  > /proc/<pid>/oom_score_adj   # 优先被杀 (如缓存)
```

### 10.2 Overcommit 策略

```
/proc/sys/vm/overcommit_memory:
  0 — 启发式 (默认, 允许适度 overcommit)
  1 — 总是允许 (malloc 永不失败, 可能 OOM)
  2 — 严格: 不超过 swap + ratio% × RAM

/proc/sys/vm/overcommit_ratio = 50
  模式2: 可分配 = swap + 50% × RAM
```

### 10.3 PSI 内存压力

```
/proc/pressure/memory (Linux 4.20+):
  some avg10=0.50 avg60=0.30 avg300=0.10 total=123456
  full avg10=0.00 avg60=0.00 avg300=0.00 total=0

  some: 至少一个任务因内存而阻塞
  full: 所有任务都因内存而阻塞
  avg10/60/300: 10秒/60秒/300秒内压力百分比
```

### 深入扩展

1. **earlyoom / systemd-oomd**：用户态 OOM 守护进程，基于 PSI 提前杀进程，避免内核 OOM 的"太晚"问题
2. **`memory.oom.group` (cgroup v2)**：OOM 时杀掉整个 cgroup（如容器），而非随机挑一个进程
3. **`vm.panic_on_oom`**：OOM 时直接 panic（嵌入式/关键系统偏好已知故障而非未知行为）
4. **DAMON (Data Access MONitor)**：Linux 5.15+，基于采样的内存访问模式监控，指导主动回收
5. **`ksm` (Kernel Same-page Merging)**：合并相同内容的物理页（虚拟化场景省内存），`/sys/kernel/mm/ksm/`

---

## 第11章：VFS 虚拟文件系统

### 11.1 四大核心数据结构

```
1. superblock — 文件系统全局信息
   struct super_block {
       struct file_system_type *s_type;  // ext4, xfs, btrfs...
       unsigned long s_blocksize;
       struct dentry *s_root;            // 根目录
   };

2. inode — 文件元数据 (一个文件一个 inode)
   struct inode {
       umode_t i_mode;          // 权限
       loff_t  i_size;          // 大小
       unsigned long i_ino;     // inode 号
       struct inode_operations *i_op;
       struct file_operations *i_fop;
   };

3. dentry — 目录项缓存 (路径 → inode 映射)
   struct dentry {
       struct inode *d_inode;
       struct dentry *d_parent;
       struct qstr d_name;
   };
   路径查找: /usr/local/bin/gcc
     → dentry("/") → dentry("usr") → dentry("local") → dentry("bin") → dentry("gcc")
     每级查 dentry 缓存，miss 才读磁盘

4. file — 打开的文件实例 (进程级)
   struct file {
       struct inode *f_inode;
       const struct file_operations *f_op;
       loff_t f_pos;            // 文件偏移
       unsigned int f_flags;    // O_RDONLY, O_NONBLOCK...
   };
```

### 11.2 VFS 调用链

```
用户: read(fd, buf, len)
  → 系统调用: sys_read()
  → VFS: file->f_op->read()
  → 具体文件系统: ext4_file_read_iter()
  → Page Cache 查找
  → 未命中: 磁盘 I/O

文件系统注册:
  register_filesystem(&ext4_fs_type);
  ext4_fs_type.mount = ext4_mount;
```

### 深入扩展

1. **FUSE (Filesystem in Userspace)**：用户态实现文件系统内核接口，sshfs、s3fs 等基于此
2. **overlayfs**：Docker 默认存储驱动，upper + lower 层叠，实现镜像分层
3. **dentry 缓存 (dcache)**：全局哈希表 + LRU，是 VFS 性能的关键，`/proc/sys/fs/dentry-state` 查看统计
4. **inode 缓存**：同理用 `/proc/sys/fs/inode-state` 查看，内存压力时由 kswapd 回收
5. **文件锁**：`flock()` (整文件) / `fcntl()` (区间锁) / `open(O_EXCL)` 三种机制

---

## 第12章：Page Cache 与 I/O 调度

### 12.1 Page Cache 读写流程

```
读取:
  read(fd) → Page Cache 查找
  ├── 命中 → 直接返回 (~10ns)
  └── 未命中 → 磁盘读取 → 放入 Page Cache → 返回

写入:
  write(fd) → 写入 Page Cache (标记 dirty)
  → 后台 writeback 线程定期刷盘
  → 或 fsync(fd) / fdatasync(fd) 强制刷盘
```

### 12.2 脏页回写参数

```
/proc/sys/vm/dirty_ratio = 20
  进程脏页超过可用内存 20% → 同步写磁盘 (阻塞!)

/proc/sys/vm/dirty_background_ratio = 10
  后台脏页超过 10% → 启动后台回写 (不阻塞)

/proc/sys/vm/dirty_expire_centisecs = 3000
  脏页超过 30 秒自动回写

/proc/sys/vm/dirty_writeback_centisecs = 500
  回写线程每 5 秒检查一次
```

### 12.3 I/O 调度器

| 调度器 | 原理 | 适用 |
|---|---|---|
| `none/noop` | 不排序，直接提交 | NVMe SSD |
| `mq-deadline` | 保证延迟上限 | HDD / SATA SSD |
| `bfq` | 公平队列 | 桌面交互 |
| `kyber` | 低延迟+高吞吐自动平衡 | 混合负载 |

```bash
cat /sys/block/sda/queue/scheduler
echo "mq-deadline" > /sys/block/sda/queue/scheduler
```

### 深入扩展

1. **`O_DIRECT` 绕过 Page Cache**：数据库自管缓存时使用，避免双缓存开销
2. **预读调优**：`/sys/block/sda/queue/read_ahead_kb`，顺序大文件可加大到 2048KB
3. **`sync_file_range()`**：精确控制特定字节范围的回写，比 `fsync` 更细粒度
4. **BIO 层**：Page Cache 下面的块 I/O 层，将读写请求打包为 `struct bio` 提交给调度器
5. **writeback 节流**：当脏页比例接近 dirty_ratio 时，进程 write 会被节流（balance_dirty_pages）

---

## 第13章：io_uring 异步 I/O

### 13.1 核心设计 — 共享环形缓冲区

```
┌──────── 用户态 ────────┐    ┌──────── 内核态 ────────┐
│                        │    │                         │
│  Submission Queue (SQ) │────│──→ 处理请求              │
│  ┌─┬─┬─┬─┬─┬─┐       │    │                         │
│  │1│2│3│ │ │ │       │    │  Completion Queue (CQ)  │
│  └─┴─┴─┴─┴─┴─┘       │    │  ┌─┬─┬─┬─┬─┬─┐        │
│       ↑ 应用写入       │    │  │✓│✓│ │ │ │ │ ← 内核  │
│                        │    │  └─┴─┴─┴─┴─┴─┘        │
│  CQ: 应用消费 ↓        │────│──←                      │
└────────────────────────┘    └─────────────────────────┘

SQ/CQ 通过 mmap 共享 → 提交/完成可以不需系统调用！
```

### 13.2 性能对比

```
传统 read():      ~4 μs/次 (含系统调用)
io_uring batched: ~0.3 μs/次 (大批量)
io_uring sqpoll:  ~0.1 μs/次 (内核轮询模式，零系统调用)
```

### 13.3 liburing 伪代码

```cpp
struct io_uring ring;
io_uring_queue_init(256, &ring, 0);       // 队列深度 256

// 提交读请求
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
io_uring_prep_read(sqe, fd, buf, len, offset);
sqe->user_data = 42;                      // 完成时原样返回
io_uring_submit(&ring);

// 等待完成
struct io_uring_cqe *cqe;
io_uring_wait_cqe(&ring, &cqe);
int result = cqe->res;                    // 读取的字节数
io_uring_cqe_seen(&ring, cqe);

io_uring_queue_exit(&ring);
```

### 13.4 支持的操作 (50+)

| 类别 | 操作 |
|---|---|
| 文件 | read, write, fsync, fallocate, openat, close |
| 网络 | accept, connect, recv, send, recvmsg, sendmsg |
| 高级 | timeout, link (链式请求), cancel, poll_add |
| 零拷贝 | send_zc (5.19+), recv_multishot |

### 深入扩展

1. **SQPOLL 模式**：内核线程持续 poll SQ，应用直接写 SQE 无需 `io_uring_enter()`，适合超低延迟
2. **Fixed Buffers / Fixed Files**：`io_uring_register()` 预注册缓冲区和文件，减少每次 I/O 的引用计数开销
3. **链式请求 (IOSQE_IO_LINK)**：多个 SQE 链式依赖执行，如 read → process → write
4. **多短消息接收 (recv_multishot)**：一次注册，持续接收多条消息，适合高连接数 UDP
5. **io_uring + 协程**：配合 C++20 coroutine，`co_await` 一个 io_uring 完成事件，实现极简异步

---

## 第14章：TCP/IP 协议栈分层

### 14.1 接收路径 (RX)

```
┌─── 应用层 ─────────────────────────────────────────────────┐
│ read() / recv() / recvmsg()                                │
│  ↑ 从 socket 缓冲区拷贝到用户缓冲区                        │
├─── 传输层 ─────────────────────────────────────────────────┤
│ tcp_v4_rcv() / udp_rcv()                                   │
│  ↑ TCP: 序号检查, ACK, 流控, 拥塞控制                       │
│  ↑ 数据放入 sk->sk_receive_queue                           │
├─── 网络层 ─────────────────────────────────────────────────┤
│ ip_rcv() → ip_local_deliver()                              │
│  ↑ IP 头解析, 路由查找, Netfilter 钩子                      │
├─── 链路层 ─────────────────────────────────────────────────┤
│ netif_receive_skb()                                         │
│  ↑ 协议分发 (ETH_P_IP → ip_rcv)                            │
├─── 驱动层 ─────────────────────────────────────────────────┤
│ NAPI poll → napi_gro_receive()                              │
│  ↑ 从 DMA ring buffer 取包, 构造 sk_buff                    │
├─── 硬件 ───────────────────────────────────────────────────┤
│ NIC → DMA → ring buffer → 中断/NAPI                        │
└────────────────────────────────────────────────────────────┘
```

### 14.2 关键优化

```
NAPI (New API):
  传统: 每个包一个硬中断 → 高速网络中断风暴
  NAPI: 中断触发后切换到 poll 模式，批量处理
  → 大幅降低高吞吐下的 CPU 开销

GRO (Generic Receive Offload):
  在驱动层合并多个小包为大包，减少协议栈处理次数

TCP 拥塞控制:
  cubic  — 默认, 高 BDP 友好
  bbr    — Google, 基于带宽和 RTT
  reno   — 经典

  sysctl -w net.ipv4.tcp_congestion_control=bbr
```

### 深入扩展

1. **XDP (eXpress Data Path)**：在驱动层 `NAPI poll` 之前决定包的命运（DROP/TX/PASS），无需 `sk_buff` 分配
2. **TCP Fast Open (TFO)**：SYN 包携带数据，减少一次 RTT，`net.ipv4.tcp_fastopen=3`
3. **GSO/TSO/GRO 协作**：发送端 TSO 分段由网卡硬件完成，接收端 GRO 合并，协议栈只看大包
4. **`SO_BUSY_POLL`**：让应用线程 poll 网卡而非让内核中断驱动，适合低延迟交易系统
5. **TCP 内存管理**：`tcp_rmem / tcp_wmem` 三元组 (min, default, max) 自动调节缓冲区大小

---

## 第15章：sk_buff 结构

### 15.1 内存布局

```
head────────┐
            │ headroom (预留空间, 供添加头部)
data────────┤ ← skb_push() 向前扩展
            │ 实际数据
tail────────┤ ← skb_put() 向后扩展
            │ tailroom
end─────────┘
```

### 15.2 层层剥离与添加

```
接收方向 (层层剥离):
  驱动:  data → 以太帧头部
  链路层: skb_pull(ETH_HLEN)    → data 移到 IP 头
  网络层: skb_pull(IP header)   → data 移到 TCP 头
  传输层: skb_pull(TCP header)  → data 移到应用数据

发送方向 (层层添加):
  应用:  放入 payload
  传输层: skb_push(TCP header)
  网络层: skb_push(IP header)
  链路层: skb_push(ETH header)

关键操作:
  skb_put(skb, len)    — 向 tail 扩展
  skb_push(skb, len)   — 向 head 扩展
  skb_pull(skb, len)   — 从 data 剥离
  skb_reserve(skb, n)  — 预留 headroom
```

### 深入扩展

1. **skb_clone vs skb_copy**：clone 共享数据区（引用计数），copy 完整复制（独立修改）
2. **Scatter-Gather**：`skb_shinfo(skb)->frags[]` 指向多个物理页面，避免大包连续内存分配
3. **skb 池化**：NAPI 使用 page_pool 预分配页面，减少高频收包时的 alloc/free 开销
4. **`struct xdp_buff`**：XDP 使用更轻量的缓冲区结构，替代 sk_buff 节省开销
5. **网络命名空间**：sk_buff 关联到特定 netns，容器网络隔离的基础

---

## 第16章：Netfilter / iptables

### 16.1 五个钩子点

```
网络报文 →① PREROUTING → 路由判断
                            │
           ┌────────────────┴────────────────┐
           ↓ (目标是本机)                     ↓ (转发)
      ② INPUT                          ③ FORWARD
           ↓                                  ↓
       本地进程                         ④ POSTROUTING → 出站
           ↓
      ⑤ OUTPUT → 路由判断 → POSTROUTING → 出站
```

### 16.2 四表 × 五链

```
┌──────────┬──────────┬──────┬────────┬──────┬───────────┐
│          │PREROUTING│INPUT │FORWARD │OUTPUT│POSTROUTING│
├──────────┼──────────┼──────┼────────┼──────┼───────────┤
│ raw      │    ✓     │      │        │  ✓   │           │
│ mangle   │    ✓     │  ✓   │   ✓    │  ✓   │     ✓     │
│ nat      │    ✓     │  ✓   │        │  ✓   │     ✓     │
│ filter   │          │  ✓   │   ✓    │  ✓   │           │
└──────────┴──────────┴──────┴────────┴──────┴───────────┘

conntrack (连接跟踪):
  状态: NEW / ESTABLISHED / RELATED / INVALID
  cat /proc/net/nf_conntrack
  sysctl net.nf_conntrack_max=262144
```

### 深入扩展

1. **nftables**：iptables 的现代替代 (Linux 3.13+)，统一 ip/ip6/arp/bridge 过滤，虚拟机编译优化
2. **eBPF 替代 iptables**：Cilium 使用 eBPF 实现 L3/L4/L7 策略，性能远超 iptables 连接跟踪
3. **conntrack 性能问题**：高连接数下 conntrack 表成为瓶颈，可用 `NOTRACK` 或 `CT --notrack` 跳过
4. **NAT 穿透**：DNAT / SNAT / MASQUERADE 原理，Docker 桥接网络的 iptables 规则
5. **`ipset`**：O(1) 查找大量 IP/端口匹配，比逐条 iptables 规则快几个数量级

---

## 第17章：epoll 内核实现原理

### 17.1 核心数据结构

```
struct eventpoll {
    struct rb_root_cached rbr;     // 红黑树: 所有监听的 fd
    struct list_head rdllist;      // 就绪链表: 有事件的 fd
    wait_queue_head_t wq;          // 等待队列: 阻塞在 epoll_wait 的进程
};

struct epitem {
    struct rb_node rbn;            // 红黑树节点
    struct list_head rdllink;      // 就绪链表节点
    struct epoll_filefd ffd;       // {file*, fd}
    struct epoll_event event;      // 关注的事件
};
```

### 17.2 工作流程

```
1. epoll_create()
   → 创建 eventpoll (红黑树 + 就绪链表 + 等待队列)

2. epoll_ctl(ADD, fd)
   → 创建 epitem，插入红黑树 O(log n)
   → 在 fd 的设备等待队列注册回调 ep_poll_callback

3. fd 有事件到达 (如 socket 收到数据)
   → 设备中断/软中断 → 唤醒等待队列
   → ep_poll_callback() 被调用
   → 将 epitem 加入就绪链表 rdllist (O(1))
   → 唤醒 epoll_wait 的进程

4. epoll_wait()
   → 检查就绪链表: 非空则返回
   → 空则睡眠 → 被 ep_poll_callback 唤醒
   → 就绪事件拷贝到用户空间 → 返回
```

### 17.3 select vs poll vs epoll

| | select | poll | epoll |
|---|---|---|---|
| 数据结构 | fd_set 位图 | pollfd[] | 红黑树 + 链表 |
| fd 上限 | 1024 | 无限 | 无限 |
| 复杂度 | O(n) | O(n) | O(1) 就绪通知 |
| 拷贝 | 每次全量 | 每次全量 | 仅 ctl 时 |
| 触发模式 | LT | LT | LT + ET |

```
ET (Edge Triggered):  状态变化时通知一次 → 必须一次读完
LT (Level Triggered): 只要可读就一直通知 → 简单但可能惊群

EPOLLONESHOT:    事件触发后自动移除，防止多线程竞争
EPOLLEXCLUSIVE:  防止惊群 (Linux 4.5+)
```

### 17.4 epoll Demo

```cpp
#if LINUX_ONLY
    int epfd = epoll_create1(0);
    int pipefd[2];
    pipe(pipefd);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;  // 边缘触发
    ev.data.fd = pipefd[0];
    epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[0], &ev);

    write(pipefd[1], "hello", 5);

    struct epoll_event events[10];
    int nfds = epoll_wait(epfd, events, 10, 100);
    // nfds=1, 读到 "hello"
#endif
```

### 深入扩展

1. **epoll + 线程池**：主线程 epoll_wait → 分发 fd 到工作线程，Nginx 采用此模型
2. **io_uring 替代 epoll**：io_uring 统一 I/O 和事件通知，减少系统调用次数
3. **`EPOLL_CLOEXEC`**：fork 后子进程自动关闭 epoll fd，防止 fd 泄漏
4. **嵌套 epoll**：将 epoll fd 加入另一个 epoll，实现多级事件分发
5. **惊群优化**：`EPOLLEXCLUSIVE` 确保同一事件只唤醒一个等待线程

---

## 第18章：零拷贝网络

### 18.1 拷贝次数对比

```
传统 read+write (4次拷贝, 4次上下文切换):
  磁盘 → 内核缓冲区 → 用户缓冲区 → 内核 socket → NIC

sendfile (2次拷贝, 2次切换):
  磁盘 → 内核缓冲区 → ✗ → (DMA gather) → NIC
  数据不经过用户空间！

sendfile + DMA gather (真零拷贝):
  磁盘 DMA → Page Cache
  只拷贝描述符到 socket 缓冲区
  NIC DMA 直接从 Page Cache 取数据

性能 (发送 1GB 文件):
  read+write: ~2000ms
  sendfile:   ~800ms
  splice:     ~750ms
```

### 18.2 零拷贝 API

| API | 内核版本 | 原理 | 适用 |
|---|---|---|---|
| `sendfile` | 2.2+ | 文件 → socket | 静态文件服务 |
| `splice` | 2.6.17+ | 任意 fd 间通过 pipe | 代理转发 |
| `tee` | 2.6.17+ | 复制 pipe 不消费 | 日志镜像 |
| `MSG_ZEROCOPY` | 4.14+ | 用户 buf → NIC DMA | 大包发送(>10KB) |

### 深入扩展

1. **`MSG_ZEROCOPY` 完成通知**：通过 `recvmsg(MSG_ERRQUEUE)` 获取完成通知，确认缓冲区可释放
2. **Nginx sendfile**：`sendfile on;` + `tcp_nopush on;` 配合，一次 syscall 发送完整 HTTP 响应
3. **`copy_file_range()`**：Linux 4.5+，文件间零拷贝，同文件系统可利用 reflink
4. **DPDK 用户态网络栈**：完全绕过内核 TCP/IP 栈，DMA 直接收发，适合 10G/25G/100G 网络
5. **AF_XDP**：内核 XDP 与用户态零拷贝的桥梁，性能接近 DPDK 但集成内核生态

---

## 第19章：Linux 设备模型

### 19.1 三类设备

```
1. 字符设备 (char device)
   按字节流访问, 不支持随机寻址
   /dev/tty, /dev/null, /dev/random
   struct cdev + file_operations

2. 块设备 (block device)
   按块访问, 支持随机寻址, 经 I/O 调度器和 Page Cache
   /dev/sda, /dev/nvme0n1
   struct gendisk + block_device_operations

3. 网络设备 (network device)
   不在 /dev 下, 通过 socket API 访问
   eth0, wlan0, lo
   struct net_device + net_device_ops
```

### 19.2 字符设备注册流程

```c
// 1. 分配主次设备号
alloc_chrdev_region(&devno, 0, 1, "mydev");

// 2. 初始化 cdev
struct cdev my_cdev;
cdev_init(&my_cdev, &my_fops);
cdev_add(&my_cdev, devno, 1);

// 3. 创建 /dev 节点
class = class_create(THIS_MODULE, "myclass");
device_create(class, NULL, devno, NULL, "mydev");

// 4. file_operations
struct file_operations my_fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .read    = my_read,
    .write   = my_write,
    .release = my_close,
    .ioctl   = my_ioctl,
};
```

### 19.3 sysfs 与 udev

```
/sys/class/    — 按类别 (net, block, input...)
/sys/bus/      — 按总线 (pci, usb, i2c...)
/sys/devices/  — 物理设备树
/sys/block/    — 块设备

udev 流程:
  内核发现设备 → uevent → udev 守护进程 → 创建 /dev 节点
  规则: /etc/udev/rules.d/
```

### 深入扩展

1. **platform device**：无总线枚举能力的设备（如 SoC 外设），通过设备树 (DTS) 描述和匹配驱动
2. **device driver model**：bus_type / device / device_driver 三者的 probe/match 机制
3. **misc device**：简化的字符设备注册 (`misc_register`)，主设备号 10，省略繁琐的 cdev 流程
4. **`/dev/mem` 直接访问物理内存**：危险但有用，BIOS/固件调试场景
5. **virtio**：虚拟化 I/O 框架，guest 与 hypervisor 通过 virtqueue 高效通信

---

## 第20章：中断处理 — 上半部/下半部

### 20.1 分层架构

```
┌─────── 上半部 (硬中断 / top half) ─────────────────────────┐
│ - 中断上下文, 不可睡眠                                     │
│ - 关闭当前中断线 (其他中断可嵌套)                           │
│ - 只做最少工作: 确认硬件 / 拷贝数据 / 调度下半部            │
│ - 时间: 几微秒                                             │
└────────────────────────────────────────────────────────────┘
                         ↓ 调度
┌─────── 下半部 (bottom half) ──────────────────────────────┐
│                                                            │
│  softirq (软中断):                                         │
│    编译时静态注册, 优先级最高                                │
│    同一 softirq 可在多 CPU 同时运行                          │
│    种类: NET_TX, NET_RX, TIMER, TASKLET, SCHED, RCU        │
│                                                            │
│  tasklet (基于 softirq):                                   │
│    动态注册, 同一 tasklet 不会多 CPU 并行                    │
│    比 softirq 简单, 大多驱动用此                             │
│                                                            │
│  workqueue (工作队列):                                      │
│    进程上下文, 可以睡眠!                                    │
│    用内核线程池执行 (kworker/*)                              │
│    适合需要长时间或可能阻塞的工作                             │
│                                                            │
│  threaded IRQ (线程化中断, 2.6.30+):                       │
│    专用内核线程, 可睡眠/使用 mutex                           │
│    request_threaded_irq(irq, quick_handler, thread_fn, .) │
└────────────────────────────────────────────────────────────┘
```

### 20.2 网络收包完整流程

```
1. NIC 收到包 → DMA 写入 ring buffer
2. 硬中断 → 驱动上半部 → 关闭 NIC 中断 → 调度 NAPI softirq
3. ksoftirqd 处理 NET_RX softirq
4. NAPI poll 函数批量收包 → 协议栈处理
5. 处理完 → 重新开启 NIC 中断

查看:
  /proc/interrupts   — 中断统计
  /proc/softirqs     — softirq 统计
```

### 深入扩展

1. **中断亲和性**：`/proc/irq/<N>/smp_affinity` 绑定中断到特定核心，配合 irqbalance
2. **RPS/RFS (Receive Packet Steering)**：软件级多队列，将收包处理分散到多 CPU
3. **NAPI 权重 (budget)**：每次 poll 最多处理 N 个包，避免 softirq 独占 CPU 太久
4. **ksoftirqd 线程**：当 softirq 持续积压时，内核将处理从中断上下文移到此内核线程
5. **`irq_chip` 框架**：中断控制器抽象，支持 GIC (ARM) / APIC (x86) / MSI-X (PCIe)

---

## 第21章：内核同步原语

### 21.1 同步原语对比表

| 原语 | 能否睡眠 | 上下文 | 开销 | 场景 |
|---|---|---|---|---|
| `raw_spinlock` | 否 | 中断/进程 | 最低 | RT 锁 |
| `spinlock` | 否 | 中断/进程 | 低 | 短临界区 |
| `rwlock` | 否 | 中断/进程 | 低 | 读多写少 |
| `seqlock` | 读否写否 | 中断/进程 | 低 | 读极多 |
| `mutex` | 是 | 仅进程 | 中 | 通用 |
| `rt_mutex` | 是 | 仅进程 | 中 | 优先级继承 |
| `rw_semaphore` | 是 | 仅进程 | 中 | 读多大段 |
| `RCU` | 读否写是 | 中断/进程 | 极低 | 读极多 |
| `percpu counter` | 否 | 任何 | 极低 | 统计 |
| `atomic` | 否 | 任何 | 极低 | 计数器 |

### 21.2 RCU 核心原理

```
读者:
  rcu_read_lock();                // 仅标记, 几乎零开销
  ptr = rcu_dereference(p);       // 带屏障的读指针
  // 使用 ptr...
  rcu_read_unlock();

写者:
  new = kmalloc(...);
  *new = *old;                    // 拷贝
  new->field = new_val;           // 修改
  rcu_assign_pointer(p, new);     // 原子替换指针
  synchronize_rcu();              // 等待所有读者完成
  kfree(old);                     // 释放旧数据

宽限期 (Grace Period):
  ┌──读者A──┐
  │ rcu_lock │ rcu_unlock
  ┌─────────────────────────┐
  │ 写者: 更新指针            │ synchronize_rcu() → kfree(old)
  └─────────────────────────┘
               ┌──读者B──┐
               │ 看到新数据 │

Linux 内核 RCU 使用超过 10000 处！
应用: 路由表, dcache, 进程列表...
```

### 21.3 spinlock 变种选择

```
spin_lock()              — 普通 (仅进程上下文访问)
spin_lock_irq()          — 自旋 + 关本地中断
spin_lock_irqsave()      — 自旋 + 关中断 + 保存状态
spin_lock_bh()           — 自旋 + 关下半部

选择规则:
  只有进程上下文: spin_lock()
  中断也会访问:   spin_lock_irqsave()
  softirq 也访问: spin_lock_bh()
```

### 深入扩展

1. **SRCU (Sleepable RCU)**：允许 RCU 读者睡眠，适合文件系统等长持有场景
2. **qspinlock**：Linux 4.2+ 默认 spinlock 实现，MCS 锁思想，NUMA 友好
3. **lockdep**：内核运行时死锁检测，`CONFIG_PROVE_LOCKING` 开启
4. **LKMM (Linux Kernel Memory Model)**：形式化内核内存模型，精确定义 barrier 语义
5. **per-CPU 变量**：`DEFINE_PER_CPU(type, name)`，通过禁抢占保护，零锁竞争

---

## 第22章：/proc 与 /sys 信息采集

### 22.1 关键采集点

```cpp
#if LINUX_ONLY
    // 系统信息
    struct utsname uts;
    uname(&uts);  // sysname, release, machine

    // CPU
    /proc/cpuinfo      — 型号、核数、频率
    /proc/loadavg      — 1/5/15 分钟负载

    // 内存
    /proc/meminfo      — 总量/可用/缓存/脏页
    /proc/self/statm   — 进程内存 (VmSize, RSS)

    // 网络
    /proc/net/dev      — 接口收发统计
    /proc/net/tcp      — TCP 连接列表

    // 进程
    /proc/self/status  — 名称/VmPeak/Threads/上下文切换
    /proc/self/maps    — 内存映射
    /proc/self/fd/     — 打开的文件描述符

    // 运行时间
    /proc/uptime       — 系统运行秒数
#endif
```

### 深入扩展

1. **`/proc/<pid>/smaps_rollup`**：进程内存使用汇总（PSS/RSS/Swap），比逐行读 smaps 快
2. **`/sys/fs/cgroup/**/memory.stat`**：cgroup 级内存统计，容器监控必备
3. **`/proc/net/sockstat`**：socket 内存和数量统计，检测连接泄漏
4. **`/proc/vmstat`**：pgfault / pgmajfault / pswpin 等 VM 事件计数
5. **BPF 替代 /proc 轮询**：用 bpf 程序 hook 内核事件推送数据，避免高频 readfile 开销

---

## 第23章：性能分析工具链

### 23.1 工具速查表

| 工具 | 用途 | 关键命令 |
|---|---|---|
| `perf stat` | 硬件计数器统计 | `perf stat -e cache-misses,cycles ./app` |
| `perf record` | 采样 profiling | `perf record -F 99 -g ./app` |
| `perf top` | 实时热点 | `perf top -p $PID` |
| `ftrace` | 内核函数跟踪 | `echo function_graph > current_tracer` |
| `bpftrace` | eBPF 动态跟踪 | `bpftrace -e 'kprobe:tcp_connect {...}'` |
| `strace` | 系统调用跟踪 | `strace -c ./app` |
| `vmstat` | 内存/CPU/IO 概览 | `vmstat 1` |
| `iostat` | 磁盘 I/O | `iostat -x 1` |
| `flame graph` | 火焰图 | `perf script \| stackcollapse-perf.pl \| flamegraph.pl` |

### 23.2 perf 使用示例

```bash
# 硬件事件统计
perf stat -e cache-misses,cache-references,instructions,cycles ./app

# CPU 热点采样 + 火焰图
perf record -F 99 -g ./app
perf script | stackcollapse-perf.pl | flamegraph.pl > flame.svg

# 调度分析
perf sched record && perf sched latency

# 内存访问分析
perf mem record && perf mem report
```

### 23.3 bpftrace 示例

```bash
# 统计系统调用
bpftrace -e 'tracepoint:raw_syscalls:sys_enter { @[comm] = count(); }'

# TCP 连接跟踪
bpftrace -e 'kprobe:tcp_connect { printf("%s -> %s\n", comm, str(arg0)); }'

# 磁盘延迟直方图
bpftrace -e 'kprobe:blk_account_io_done { @usecs = hist(nsecs); }'
```

### 深入扩展

1. **perf c2c**：检测 false sharing 的利器，`perf c2c record` + `perf c2c report`
2. **Intel VTune / AMD uProf**：厂商提供的深度微架构分析工具，比 perf 更详细的流水线分析
3. **eBPF CO-RE (Compile Once, Run Everywhere)**：`libbpf` + BTF 实现跨内核版本兼容的 BPF 程序
4. **Continuous Profiling**：将 perf/pprof 接入 Grafana Pyroscope 等平台，持续分析生产环境
5. **`perf inject --jit`**：支持 JIT 编译代码（JVM/V8）的符号解析

---

## 第24章：内核调优参数速查

### 24.1 CPU 调度

```
kernel.sched_latency_ns        = 6000000     # CFS 调度延迟目标
kernel.sched_min_granularity_ns = 750000      # 最小时间片
kernel.sched_wakeup_granularity_ns = 1000000  # 唤醒粒度
kernel.sched_rt_runtime_us     = 950000       # RT 最大占比
```

### 24.2 内存管理

```
vm.swappiness                = 10     # swap 倾向 (低=少swap)
vm.dirty_ratio               = 20     # 同步写脏页比例
vm.dirty_background_ratio    = 10     # 后台写脏页比例
vm.overcommit_memory         = 0      # overcommit 策略
vm.min_free_kbytes           = 67584  # 最低空闲内存
vm.zone_reclaim_mode         = 0      # NUMA 回收策略

透明大页:
  /sys/kernel/mm/transparent_hugepage/enabled = madvise
  always:  全局启用 (可能延迟抖动)
  madvise: 仅按需 (推荐)
  never:   禁用 (低延迟推荐)
```

### 24.3 网络栈

```
net.core.somaxconn           = 4096      # listen backlog
net.core.netdev_max_backlog  = 5000      # 网卡接收队列
net.core.rmem_max            = 16777216  # 最大接收缓冲
net.core.wmem_max            = 16777216  # 最大发送缓冲
net.ipv4.tcp_max_syn_backlog = 4096      # SYN 队列
net.ipv4.tcp_fin_timeout     = 30        # FIN_WAIT2 超时
net.ipv4.tcp_tw_reuse        = 1         # 复用 TIME_WAIT
net.ipv4.tcp_keepalive_time  = 600       # keepalive
net.ipv4.tcp_congestion_control = bbr    # 拥塞控制

TCP 缓冲区:
net.ipv4.tcp_rmem = 4096 87380 16777216  # min default max
net.ipv4.tcp_wmem = 4096 65536 16777216

net.ipv4.ip_local_port_range = 1024 65535
net.nf_conntrack_max = 262144
```

### 24.4 文件系统

```
fs.file-max                    = 1048576  # 系统级最大 fd
fs.nr_open                     = 1048576  # 进程级最大 fd
fs.inotify.max_user_watches    = 524288   # inotify 上限
```

### 24.5 高并发 Web 服务器一键调优

```bash
sysctl -w net.core.somaxconn=65535
sysctl -w net.ipv4.tcp_max_syn_backlog=65535
sysctl -w net.core.netdev_max_backlog=65535
sysctl -w net.ipv4.tcp_fin_timeout=10
sysctl -w net.ipv4.tcp_tw_reuse=1
sysctl -w net.ipv4.tcp_congestion_control=bbr
sysctl -w vm.swappiness=10
ulimit -n 1048576

# 持久化
echo "net.core.somaxconn=65535" >> /etc/sysctl.conf
sysctl -p
```

---

## 附录 A：test10.cpp 扩展建议总结

| 章节 | 扩展方向 | 优先级 | 关联 |
|---|---|---|---|
| Ch1 | EEVDF 调度器 / sched_ext eBPF / 调度域 | 高 | test13 |
| Ch2 | PREEMPT_RT / CPU 隔离 / cyclictest | 高 | test13 |
| Ch3 | IRQ 亲和性 / AutoNUMA / perf node-loads | 高 | test14 |
| Ch4 | PSI 监控 / K8s 资源限制 / systemd slice | 中 | — |
| Ch5 | THP 性能影响 / KPTI / userfaultfd / MPK | 高 | test14 |
| Ch6 | 内存碎片 compaction / GFP 标志 / Zone 管理 | 中 | — |
| Ch7 | SLUB 合并 / KASAN / 用户态对照 (jemalloc) | 中 | — |
| Ch8 | DAX 持久内存 / memfd_create / COW 优化 | 中 | — |
| Ch9 | VFIO 设备直通 / CMA / P2P DMA | 中 | — |
| Ch10 | earlyoom / DAMON / cgroup OOM / KSM | 高 | — |
| Ch11 | FUSE / overlayfs / dcache 优化 | 中 | — |
| Ch12 | O_DIRECT / sync_file_range / writeback 节流 | 高 | — |
| Ch13 | SQPOLL / Fixed Buffers / 链式请求 / 协程集成 | 高 | test9 |
| Ch14 | XDP / TCP Fast Open / SO_BUSY_POLL / AF_XDP | 高 | test7 |
| Ch15 | skb_clone / page_pool / xdp_buff | 中 | — |
| Ch16 | nftables / eBPF Cilium / ipset | 高 | — |
| Ch17 | io_uring 替代 / EPOLLEXCLUSIVE / 嵌套 epoll | 中 | test7 |
| Ch18 | MSG_ZEROCOPY / copy_file_range / DPDK | 高 | test7 |
| Ch19 | platform device / DTS / virtio | 中 | — |
| Ch20 | IRQ 亲和性 / RPS/RFS / ksoftirqd | 中 | — |
| Ch21 | SRCU / qspinlock / lockdep / per-CPU 变量 | 高 | test11 |
| Ch22 | smaps_rollup / cgroup stat / BPF 推送 | 中 | — |
| Ch23 | perf c2c / VTune / eBPF CO-RE / Continuous Profiling | 高 | test14 |
| Ch24 | 场景化调优模板 (数据库/Web/实时/批处理) | 高 | — |

---

## 附录 B：典型输出示例

```
================================================================
     深入理解 Linux 内核机制 — 完全教程
================================================================

╔══════════════════════════════════════════╗
║ 一、进程调度篇                            ║
╚══════════════════════════════════════════╝

  ── CFS 完全公平调度器模拟 ──
  CFS 模拟 (500 个调度周期):

     PID        Name    Nice   Runtime(ms)       占比(%)
  ------------------------------------------------------
    1004    critical     -15          180.5         36.1%
    1002    database      -5          120.3         24.1%
    1001  web-server       0           95.2         19.0%
    1003      logger      10           60.8         12.2%
    1005  background      15           43.2          8.6%

  结论：nice 值越低（优先级越高）获得的 CPU 时间越多

  ── CFS 可调参数 ──
  sched_latency_ns = 6000000
  sched_min_granularity_ns = 750000
  ...

  ── 实时调度策略 ──
  当前进程调度策略: SCHED_OTHER
  RT 运行时限制: 950000 us
  ...

╔══════════════════════════════════════════╗
║ 二、内存管理篇                            ║
╚══════════════════════════════════════════╝

  ── 伙伴系统模拟 ──
  初始状态:
    order 10 (每块 1024 页 = 4096 KB): 1 块空闲

  分配后 (4KB + 32KB + 128KB):
    块 A (order 0) 起始页: 0
    块 B (order 3) 起始页: 8
    块 C (order 5) 起始页: 32
    order  0: 1 块, order 1: 1 块, order 2: 1 块, ...

  释放块 B 后（触发伙伴合并）:
    order 3: 合并...

  全部释放后（完全合并回 order 10）:
    order 10: 1 块空闲 ✓

  ── SLAB/SLUB 分配器 ──
    kmalloc-128 (分配50个): 对象大小: 128 B, 每 slab: 32 个, slab 数: 2
    ...

╔══════════════════════════════════════════╗
║ 四、网络栈篇                              ║
╚══════════════════════════════════════════╝

  ── epoll 内核实现原理 ──
  epoll API 演示:
    就绪 fd 数: 1
    fd=3 读到: "hello" (5 bytes)
  ...

╔══════════════════════════════════════════╗
║ 七、综合实战篇                            ║
╚══════════════════════════════════════════╝

  ── /proc 与 /sys 系统信息采集 ──
  系统: Linux 6.1.0-amd64 x86_64
  CPU: Intel(R) Core(TM) i7-12700H
  负载: 0.35 0.28 0.22
  ...

================================================================
     演示完成
================================================================
```

---

## 附录 C：核心设计原则

> **Linux 内核的精髓在于分层抽象与可插拔设计。** VFS 抽象文件系统、Netfilter 抽象包处理、设备模型抽象硬件。每一层通过函数指针表（`*_operations`）实现多态——虽然是 C 语言，却体现了面向对象的思想。**理解内核，就是理解这些分层边界、数据结构和回调链。** 从用户态看，`read()` 是一个系统调用；从内核看，它穿越 VFS → 文件系统 → Page Cache → 块设备 → DMA 的完整路径。掌握这条路径上的每一环，是编写高性能系统软件的基础。
