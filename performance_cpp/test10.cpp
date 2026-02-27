// =============================================================================
// 深入理解 Linux 内核机制：调度、内存管理、网络栈、驱动
// =============================================================================
//
// 本教程从用户态 C++ 视角出发，深入剖析 Linux 内核核心子系统的原理、
// 数据结构与算法，并通过可编译运行的代码示例，演示如何与内核交互、
// 模拟内核算法、以及编写高性能系统级程序。
//
// ‼️ 说明：本文件仅在 Linux 上编译运行。
// 编译：g++ -std=c++17 -O2 -pthread -o test10 test10.cpp
//
// 目录：
// ═══════════════════════════════════════════════════════════════
// 一、进程调度篇
//   1.  CFS 完全公平调度器原理与模拟
//   2.  实时调度策略 (SCHED_FIFO / SCHED_RR / SCHED_DEADLINE)
//   3.  CPU 亲和性 (CPU Affinity) 与 NUMA 感知
//   4.  cgroups CPU 限制原理
//
// 二、内存管理篇
//   5.  虚拟内存与分页机制
//   6.  伙伴系统 (Buddy System) 模拟
//   7.  SLAB/SLUB 分配器原理
//   8.  mmap 与零拷贝 (Zero-Copy)
//   9.  内存映射 I/O 与 DMA 概念
//  10.  OOM Killer 与内存压力处理
//
// 三、文件系统与 I/O 篇
//  11.  VFS 虚拟文件系统抽象
//  12.  Page Cache 与 I/O 调度
//  13.  io_uring 异步 I/O（Linux 5.1+）
//
// 四、网络栈篇
//  14.  TCP/IP 协议栈分层架构
//  15.  Socket 缓冲区 (sk_buff) 原理
//  16.  Netfilter / iptables 钩子链
//  17.  epoll 内核实现原理
//  18.  零拷贝网络：sendfile / splice
//
// 五、设备驱动篇
//  19.  Linux 设备模型：字符/块/网络设备
//  20.  中断处理：上半部/下半部 (softirq, tasklet, workqueue)
//
// 六、内核同步篇
//  21.  内核同步原语：spinlock, rwlock, RCU, seqlock
//
// 七、综合实战篇
//  22.  /proc 与 /sys 文件系统信息采集
//  23.  性能分析工具链 (perf, ftrace, bpf)
//  24.  内核调优参数速查手册
// ═══════════════════════════════════════════════════════════════

#ifdef _WIN32
// Windows 下仅编译框架，跳过 Linux 特有 API
#define LINUX_ONLY 0
#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <random>
#include <optional>
#include <bitset>
#include <cmath>
#else
#define LINUX_ONLY 1
#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <random>
#include <optional>
#include <bitset>
#include <cmath>

// Linux 特有头文件
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sched.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#endif

using namespace std::chrono_literals;

// 工具
static void print_header(const char* title) {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║ " << std::left << std::setw(40) << title << " ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";
}

static void print_section(const char* title) {
    std::cout << "  ── " << title << " ──\n";
}

static std::string read_proc_file(const std::string& path) {
#if LINUX_ONLY
    std::ifstream ifs(path);
    if (!ifs) return "(无法读取 " + path + ")";
    std::string content((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
    return content;
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


// =============================================================================
// ██ 一、进程调度篇
// =============================================================================

// =============================================================================
// 第1章：CFS 完全公平调度器 — 原理与模拟
// =============================================================================
//
// CFS (Completely Fair Scheduler) 是 Linux 2.6.23+ 的默认调度器。
//
// 核心思想：
//   - 每个进程维护一个"虚拟运行时间" (vruntime)
//   - vruntime 最小的进程获得 CPU
//   - 高优先级进程的 vruntime 增长更慢（获得更多实际 CPU 时间）
//   - 使用红黑树管理所有可运行进程，O(log n) 选择下一个
//
// 关键数据结构（内核源码）：
//   struct sched_entity {
//       u64 vruntime;        // 虚拟运行时间
//       struct rb_node run_node; // 红黑树节点
//       ...
//   };
//   struct cfs_rq {
//       struct rb_root_cached tasks_timeline; // 红黑树
//       u64 min_vruntime;    // 最小vruntime（单调递增）
//       ...
//   };
//
// vruntime 计算：
//   vruntime += delta_exec * (NICE_0_WEIGHT / weight)
//   nice=0  → weight=1024 → vruntime 以正常速度增长
//   nice=-20 → weight=88761 → vruntime 增长很慢（更多CPU）
//   nice=19  → weight=15    → vruntime 增长很快（更少CPU）
// =============================================================================

namespace ch1 {

// 模拟 CFS 调度器
class CFSSimulator {
public:
    struct Task {
        int pid;
        std::string name;
        int nice;          // -20 ~ 19
        double weight;     // 由 nice 计算
        double vruntime;   // 虚拟运行时间
        double total_runtime; // 实际运行总时间

        bool operator>(const Task& o) const { return vruntime > o.vruntime; }
    };

private:
    // 用优先队列模拟红黑树（最小 vruntime 在顶部）
    std::priority_queue<Task, std::vector<Task>, std::greater<Task>> rq_;
    double min_vruntime_ = 0;

    // nice → weight 映射表（简化版，对应内核 sched_prio_to_weight[]）
    static double nice_to_weight(int nice) {
        // 简化公式：weight = 1024 * 1.25^(-nice)
        return 1024.0 * std::pow(1.25, -nice);
    }

    // 计算时间片（基于权重比例）
    double calc_timeslice(const Task& task, double total_weight, int nr_running) {
        // sched_slice = sched_period * (weight / total_weight)
        double sched_period = std::max(6.0, (double)nr_running * 0.75); // ms
        return sched_period * (task.weight / total_weight);
    }

public:
    void add_task(int pid, const std::string& name, int nice) {
        Task t;
        t.pid = pid;
        t.name = name;
        t.nice = nice;
        t.weight = nice_to_weight(nice);
        t.vruntime = min_vruntime_; // 新进程从 min_vruntime 开始
        t.total_runtime = 0;
        rq_.push(t);
    }

      double current_total_weight() const {
        auto copy = rq_;
        double total = 0.0;
        while (!copy.empty()) {
          total += copy.top().weight;
          copy.pop();
        }
        return total;
      }

    void simulate(int total_ticks) {
        std::cout << "  CFS 模拟 (" << total_ticks << " 个调度周期):\n\n";
      // NOTE: 这是教学级近似模型，重点演示 vruntime/weight 关系，
      // 并不覆盖内核 CFS 的全部细节（如睡眠补偿、负载跟踪、调度域迁移等）。

        std::map<int, double> runtime_map;

        for (int tick = 0; tick < total_ticks; ++tick) {
            if (rq_.empty()) break;

            // 选择 vruntime 最小的任务（O(1)，红黑树最左节点）
            Task current = rq_.top();
            rq_.pop();

            // 计算总权重
            double total_weight = current.weight + current_total_weight();
            int nr_running = rq_.size() + 1;

            // 计算时间片
            double timeslice = calc_timeslice(current, total_weight, nr_running);

            // 运行一个时间片
            double delta_exec = timeslice;
            current.total_runtime += delta_exec;
            runtime_map[current.pid] += delta_exec;

            // 更新 vruntime
            // vruntime += delta_exec * (NICE_0_WEIGHT / weight)
            double nice_0_weight = 1024.0;
            current.vruntime += delta_exec * (nice_0_weight / current.weight);

            // 更新 min_vruntime
            if (!rq_.empty())
                min_vruntime_ = std::max(min_vruntime_, rq_.top().vruntime);

            // 放回队列
            rq_.push(current);
        }

        // 输出结果
        std::cout << "  " << std::setw(6) << "PID"
                  << std::setw(12) << "Name"
                  << std::setw(8) << "Nice"
                  << std::setw(14) << "Runtime(ms)"
                  << std::setw(14) << "占比(%)" << "\n";
        std::cout << "  " << std::string(54, '-') << "\n";

        double total = 0;
        for (auto& [pid, rt] : runtime_map) total += rt;

        // 从队列中取出所有任务信息
        std::vector<Task> tasks;
        while (!rq_.empty()) { tasks.push_back(rq_.top()); rq_.pop(); }

        for (auto& t : tasks) {
            auto it = runtime_map.find(t.pid);
            double rt = it != runtime_map.end() ? it->second : 0;
            std::cout << "  " << std::setw(6) << t.pid
                      << std::setw(12) << t.name
                      << std::setw(8) << t.nice
                      << std::setw(14) << std::fixed << std::setprecision(1) << rt
                      << std::setw(13) << std::setprecision(1) << (rt / total * 100) << "%\n";
        }

        std::cout << "\n  结论：nice 值越低（优先级越高）获得的 CPU 时间越多\n";
    }
};

void demo_cfs() {
    print_section("CFS 完全公平调度器模拟");

    CFSSimulator cfs;
    cfs.add_task(1001, "web-server",  0);    // 普通优先级
    cfs.add_task(1002, "database",   -5);    // 稍高优先级
    cfs.add_task(1003, "logger",     10);    // 低优先级
    cfs.add_task(1004, "critical",  -15);    // 很高优先级
    cfs.add_task(1005, "background", 15);    // 很低优先级

    cfs.simulate(500);
}

// --- CFS 相关内核参数 ---
void demo_cfs_tunables() {
    print_section("CFS 可调参数");

    std::cout << R"(
  /proc/sys/kernel/sched_latency_ns
    调度延迟目标（默认 6ms）。所有可运行进程在此时间内至少运行一次。
    进程越多，每个分到的时间片越短。

  /proc/sys/kernel/sched_min_granularity_ns
    最小时间片（默认 0.75ms）。即使进程很多也不低于此值。
    当 nr_running * min_gran > latency 时，调度周期自动延长。

  /proc/sys/kernel/sched_wakeup_granularity_ns
    唤醒粒度（默认 1ms）。新唤醒的进程 vruntime 必须比当前进程
    小至少这么多才能抢占，减少不必要的上下文切换。

  调优示例（低延迟场景）：
    echo 1000000 > /proc/sys/kernel/sched_latency_ns
    echo 100000 > /proc/sys/kernel/sched_min_granularity_ns
)";
}

} // namespace ch1


// =============================================================================
// 第2章：实时调度策略
// =============================================================================

namespace ch2 {

void demo_realtime_scheduling() {
    print_section("实时调度策略");

    std::cout << R"(
  Linux 调度策略层次：
  ┌─────────────────────────────────────────────────────────────┐
  │  优先级 99 ── SCHED_FIFO / SCHED_RR     (实时, RT 类)     │
  │    ...                                                      │
  │  优先级  1 ── SCHED_FIFO / SCHED_RR     (实时, RT 类)     │
  │  ─────────── 实时分界线 ───────────────────────────────── │
  │  nice -20  ── SCHED_OTHER (CFS)          (普通, FAIR 类)   │
  │    ...                                                      │
  │  nice  19  ── SCHED_OTHER (CFS)          (普通, FAIR 类)   │
  │  ─────────── 空闲分界线 ───────────────────────────────── │
  │            ── SCHED_IDLE / SCHED_BATCH   (批处理/空闲)     │
  └─────────────────────────────────────────────────────────────┘

  SCHED_FIFO：先进先出，同优先级不抢占，高优先级立即抢占低优先级
  SCHED_RR：  轮转调度，同优先级按时间片轮转（默认100ms）
  SCHED_DEADLINE (Linux 3.14+)：基于 EDF，保证截止时间
    三个参数: runtime / deadline / period
    例: runtime=10ms, deadline=30ms, period=100ms
    → 每 100ms 周期内，保证在 30ms 前获得 10ms CPU

  ⚠️ 实时进程会饿死普通进程！需谨慎使用。
  /proc/sys/kernel/sched_rt_runtime_us = 950000 (默认)
  → RT 进程最多使用 95% CPU，预留 5% 给普通进程
)";

#if LINUX_ONLY
    // 查看当前进程调度策略
    int policy = sched_getscheduler(0);
    const char* policies[] = {"OTHER", "FIFO", "RR", "BATCH", "", "IDLE", "DEADLINE"};
    std::cout << "  当前进程调度策略: SCHED_"
              << (policy >= 0 && policy <= 6 ? policies[policy] : "UNKNOWN") << "\n";

    // 设置 CPU 亲和性示例
    struct sched_param param;
    param.sched_priority = 0;
    // sched_setscheduler(0, SCHED_OTHER, &param); // 设为普通调度

    // 查看 RT 限制
    std::cout << "  RT 运行时限制: " << read_first_line("/proc/sys/kernel/sched_rt_runtime_us") << " us\n";
    std::cout << "  RT 周期: " << read_first_line("/proc/sys/kernel/sched_rt_period_us") << " us\n";
#endif
}

} // namespace ch2


// =============================================================================
// 第3章：CPU 亲和性与 NUMA
// =============================================================================

namespace ch3 {

void demo_cpu_affinity() {
    print_section("CPU 亲和性 (CPU Affinity)");

    std::cout << R"(
  CPU 亲和性：将进程/线程绑定到特定 CPU 核心。

  为什么需要：
    1. 避免跨核迁移 → 保持 L1/L2 缓存热度
    2. 避免跨 NUMA 节点 → 减少远程内存访问延迟
    3. 隔离关键任务 → 避免被其他进程抢占

  内核数据结构：
    struct task_struct {
        cpumask_t cpus_mask;  // 允许运行的 CPU 掩码
        int nr_cpus_allowed;
        ...
    };

  API：
    sched_setaffinity(pid, sizeof(cpu_set_t), &mask) — 设置亲和性
    sched_getaffinity(pid, sizeof(cpu_set_t), &mask) — 获取亲和性
    pthread_setaffinity_np(thread, ...) — 线程级别
)";

#if LINUX_ONLY
    // 获取 CPU 数量
    int ncpus = sysconf(_SC_NPROCESSORS_ONLN);
  if (ncpus <= 0) {
    std::cout << "  无法获取在线 CPU 数量\n";
    return;
  }
    std::cout << "  在线 CPU 数: " << ncpus << "\n";

    // 获取当前进程 CPU 亲和性
    cpu_set_t mask;
    CPU_ZERO(&mask);
    if (sched_getaffinity(0, sizeof(mask), &mask) != 0) {
      std::cout << "  获取亲和性失败: " << strerror(errno) << "\n";
      return;
    }

    std::cout << "  当前亲和性: {";
    for (int i = 0; i < ncpus; ++i) {
        if (CPU_ISSET(i, &mask)) std::cout << " " << i;
    }
    std::cout << " }\n";

    // 绑定到 CPU 0（演示）
    cpu_set_t new_mask;
    CPU_ZERO(&new_mask);
    CPU_SET(0, &new_mask);
    if (sched_setaffinity(0, sizeof(new_mask), &new_mask) == 0) {
        std::cout << "  已绑定到 CPU 0\n";
    } else {
      std::cout << "  绑定 CPU 0 失败: " << strerror(errno) << "\n";
    }

    // 恢复
    if (sched_setaffinity(0, sizeof(mask), &mask) == 0) {
      std::cout << "  已恢复原亲和性\n";
    }
#endif

    // NUMA 概念
    std::cout << R"(

  NUMA (Non-Uniform Memory Access):
  ┌──────────────┐          ┌──────────────┐
  │   NUMA Node 0│───QPI───│   NUMA Node 1│
  │ CPU 0,1,2,3  │          │ CPU 4,5,6,7  │
  │   内存 32GB  │          │   内存 32GB  │
  └──────────────┘          └──────────────┘

  本地内存访问 ~80ns，远程内存访问 ~130ns (延迟增加 60%)

  NUMA 感知编程：
    numactl --cpunodebind=0 --membind=0 ./myapp  # 绑定 node 0
    set_mempolicy(MPOL_BIND, ...)                 # 代码中设置
    mbind(addr, len, MPOL_BIND, ...)              # 绑定内存区域

  查看 NUMA 拓扑: numactl -H 或 lscpu
)";
}

} // namespace ch3


// =============================================================================
// 第4章：cgroups CPU 限制
// =============================================================================

namespace ch4 {

void demo_cgroups() {
    print_section("cgroups CPU 限制");

    std::cout << R"(
  cgroups (Control Groups) 是 Linux 内核的资源隔离机制。
  Docker/K8s 等容器技术的基础。

  cgroup v2 CPU 控制器：
  ┌─────────────────────────────────────────────────────────┐
  │ /sys/fs/cgroup/mygroup/                                 │
  │   cpu.max        — "quota period"                       │
  │                    例: "50000 100000" → 50% CPU          │
  │   cpu.weight     — 相对权重 (1-10000, 默认100)           │
  │   cgroup.procs   — 属于此组的进程 PID                    │
  │   cpu.stat       — 统计信息 (usage_usec, ...)           │
  └─────────────────────────────────────────────────────────┘

  操作示例：
    # 创建 cgroup
    mkdir /sys/fs/cgroup/mygroup

    # 限制 CPU 为 50%
    echo "50000 100000" > /sys/fs/cgroup/mygroup/cpu.max

    # 将进程加入
    echo $PID > /sys/fs/cgroup/mygroup/cgroup.procs

    # 查看使用量
    cat /sys/fs/cgroup/mygroup/cpu.stat

  cgroup v1 (旧版) 路径：/sys/fs/cgroup/cpu/mygroup/
    cpu.cfs_quota_us  — CPU 配额 (微秒)
    cpu.cfs_period_us — 周期 (默认 100000us = 100ms)
    cpu.shares        — 相对权重 (默认 1024)

  Docker 映射：
    docker run --cpus=1.5        → cpu.max = "150000 100000"
    docker run --cpu-shares=512   → cpu.weight ≈ 50
)";

#if LINUX_ONLY
    // 读取当前 cgroup 信息
    std::cout << "  当前进程 cgroup:\n";
    std::string cgroup = read_proc_file("/proc/self/cgroup");
    // 只显示前几行
    std::istringstream iss(cgroup);
    std::string line;
    int n = 0;
    while (std::getline(iss, line) && n++ < 5) {
        std::cout << "    " << line << "\n";
    }
#endif
}

} // namespace ch4


// =============================================================================
// ██ 二、内存管理篇
// =============================================================================

// =============================================================================
// 第5章：虚拟内存与分页
// =============================================================================

namespace ch5 {

void demo_virtual_memory() {
    print_section("虚拟内存与分页机制");

    std::cout << R"(
  x86-64 四级页表结构 (4KB 页)：
  ┌─────────────────────────────────────────────────────────────────┐
  │ 虚拟地址 (48位有效):                                            │
  │ ┌────────┬────────┬────────┬────────┬────────────┐             │
  │ │ PML4   │ PDPT   │  PD    │  PT    │  Offset    │             │
  │ │ 9 bits │ 9 bits │ 9 bits │ 9 bits │  12 bits   │             │
  │ └────┬───┴───┬────┴───┬────┴───┬────┴────────────┘             │
  │      │       │        │        │                                │
  │      ▼       ▼        ▼        ▼                                │
  │    PML4    PDPT表    PD表     PT表 ──→ 4KB 物理页               │
  │   (512项)  (512项)  (512项)  (512项)                            │
  └─────────────────────────────────────────────────────────────────┘

  可寻址空间: 2^48 = 256 TB (用户态 128TB + 内核态 128TB)

  大页 (Huge Pages)：
    2MB 大页 → 少一级页表 → TLB 利用率提升
    1GB 大页 → 更少 TLB 未命中，适合大内存数据库

  TLB (Translation Lookaside Buffer)：
    L1 TLB: ~64 条目, 1 cycle
    L2 TLB: ~1536 条目, ~7 cycles
    TLB miss → 走页表 → ~20-200 cycles (最坏4次内存访问)
)";

#if LINUX_ONLY
    // 查看进程内存映射
    std::cout << "\n  当前进程内存布局 (/proc/self/maps 前15行):\n";
    std::ifstream maps("/proc/self/maps");
    std::string line;
    int count = 0;
    while (std::getline(maps, line) && count++ < 15) {
        std::cout << "    " << line << "\n";
    }

    // 页面大小
    long page_size = sysconf(_SC_PAGESIZE);
    std::cout << "\n  系统页面大小: " << page_size << " bytes\n";

    // 查看物理内存信息
    struct sysinfo si;
    sysinfo(&si);
    std::cout << "  总内存: " << (si.totalram * si.mem_unit) / (1024*1024) << " MB\n";
    std::cout << "  可用内存: " << (si.freeram * si.mem_unit) / (1024*1024) << " MB\n";

    // 演示虚拟地址
    int stack_var = 42;
    static int data_var = 42;
    int* heap_var = new int(42);

    std::cout << "\n  地址空间分布:\n";
    std::cout << "    main()  代码段: " << (void*)demo_virtual_memory << "\n";
    std::cout << "    data_var 数据段: " << (void*)&data_var << "\n";
    std::cout << "    heap_var 堆:     " << (void*)heap_var << "\n";
    std::cout << "    stack_var 栈:    " << (void*)&stack_var << "\n";

    delete heap_var;

    // /proc/self/statm
    std::cout << "\n  内存统计 (/proc/self/statm):\n";
    std::ifstream statm("/proc/self/statm");
    long vm_size, vm_rss, shared, text, lib, data, dirty;
    statm >> vm_size >> vm_rss >> shared >> text >> lib >> data >> dirty;
    std::cout << "    虚拟内存: " << vm_size * page_size / 1024 << " KB\n";
    std::cout << "    常驻内存 (RSS): " << vm_rss * page_size / 1024 << " KB\n";
    std::cout << "    共享页面: " << shared * page_size / 1024 << " KB\n";
#endif
}

} // namespace ch5


// =============================================================================
// 第6章：伙伴系统 (Buddy System) 模拟
// =============================================================================

namespace ch6 {

// 伙伴系统分配器模拟
// Linux 内核使用伙伴系统管理物理页面
// order 0 = 1页(4KB), order 1 = 2页(8KB), ..., order 10 = 1024页(4MB)

class BuddyAllocator {
    static constexpr int MAX_ORDER = 10; // 最大阶 → 2^10 = 1024 页

    // 每个阶的空闲链表
    std::vector<std::set<size_t>> free_lists_;
    size_t total_pages_;
    std::vector<int> page_order_; // 每个页面所属的块的阶

public:
    explicit BuddyAllocator(size_t total_pages) : total_pages_(total_pages) {
        free_lists_.resize(MAX_ORDER + 1);
        page_order_.resize(total_pages, -1);

        // 初始化：将所有页面放入最大阶的空闲链表
        for (size_t i = 0; i + (1 << MAX_ORDER) <= total_pages; i += (1 << MAX_ORDER)) {
            free_lists_[MAX_ORDER].insert(i);
        }
    }

    // 分配 2^order 个页面，返回起始页号，-1 表示失败
    int allocate(int order) {
        if (order > MAX_ORDER) return -1;

        // 从 order 开始向上查找空闲块
        for (int i = order; i <= MAX_ORDER; ++i) {
            if (!free_lists_[i].empty()) {
                size_t block = *free_lists_[i].begin();
                free_lists_[i].erase(free_lists_[i].begin());

                // 如果找到的块比需要的大，拆分
                while (i > order) {
                    --i;
                    // 拆分：伙伴放入低一阶的空闲链表
                    size_t buddy = block + (1 << i);
                    free_lists_[i].insert(buddy);
                }

                page_order_[block] = order;
                return block;
            }
        }
        return -1; // 无空闲内存
    }

    // 释放
    void deallocate(size_t block, int order) {
        page_order_[block] = -1;

        // 尝试合并伙伴
        while (order < MAX_ORDER) {
            size_t buddy = block ^ (1 << order); // XOR 计算伙伴地址

            // 检查伙伴是否空闲
            if (free_lists_[order].count(buddy)) {
                free_lists_[order].erase(buddy);
                block = std::min(block, buddy); // 合并，取较小地址
                ++order;
            } else {
                break;
            }
        }

        free_lists_[order].insert(block);
    }

    void print_state() const {
        std::cout << "  伙伴系统状态:\n";
        for (int i = 0; i <= MAX_ORDER; ++i) {
            if (!free_lists_[i].empty()) {
                std::cout << "    order " << std::setw(2) << i
                          << " (每块 " << std::setw(5) << (1 << i)
                          << " 页 = " << std::setw(6) << ((1 << i) * 4)
                          << " KB): " << free_lists_[i].size() << " 块空闲\n";
            }
        }
    }
};

void demo_buddy_system() {
    print_section("伙伴系统模拟");

    BuddyAllocator buddy(1024); // 1024 页 = 4MB

    std::cout << "  初始状态:\n";
    buddy.print_state();

    // 分配
    int a = buddy.allocate(0);  // 1 页 = 4KB
    int b = buddy.allocate(3);  // 8 页 = 32KB
    int c = buddy.allocate(5);  // 32 页 = 128KB

    std::cout << "\n  分配后 (4KB + 32KB + 128KB):\n";
    std::cout << "    块 A (order 0) 起始页: " << a << "\n";
    std::cout << "    块 B (order 3) 起始页: " << b << "\n";
    std::cout << "    块 C (order 5) 起始页: " << c << "\n";
    buddy.print_state();

    // 释放 B，观察伙伴合并
    buddy.deallocate(b, 3);
    std::cout << "\n  释放块 B 后（可能触发伙伴合并）:\n";
    buddy.print_state();

    // 释放 A
    buddy.deallocate(a, 0);
    std::cout << "\n  释放块 A 后（更多合并）:\n";
    buddy.print_state();

    // 释放 C
    buddy.deallocate(c, 5);
    std::cout << "\n  全部释放后（完全合并回 order 10）:\n";
    buddy.print_state();

    std::cout << R"(
  内核伙伴系统查看: cat /proc/buddyinfo
    Node 0, zone   Normal  128  64  32  16  8  4  2  1  0  0  1
    ──→ order 0 有 128 个空闲块, order 1 有 64 个, ...
)";
}

} // namespace ch6


// =============================================================================
// 第7章：SLAB/SLUB 分配器
// =============================================================================

namespace ch7 {

void demo_slab() {
    print_section("SLAB/SLUB 分配器原理");

    std::cout << R"(
  问题：伙伴系统最小分配 1 页 (4KB)，但内核对象通常很小
        (task_struct ~6KB, inode ~500B, dentry ~200B)。

  解决：SLAB 分配器 —— 在伙伴系统之上的对象缓存层。

  架构：
  ┌──────────────────────────────────────────────────────────┐
  │  kmalloc(64) / kfree()           用户接口               │
  ├──────────────────────────────────────────────────────────┤
  │  kmem_cache                      对象缓存               │
  │  ┌──────────────────────┐                               │
  │  │ name: "task_struct"  │                               │
  │  │ object_size: 6592    │                               │
  │  │ per-cpu: slub_page[] │ ← 每个CPU有本地缓存(无锁!)   │
  │  │ partial: list        │ ← 部分使用的 slab            │
  │  │ full: list           │ ← 完全使用的 slab            │
  │  └──────────────────────┘                               │
  ├──────────────────────────────────────────────────────────┤
  │  Buddy System                    页面分配               │
  └──────────────────────────────────────────────────────────┘

  SLAB vs SLUB vs SLOB:
    SLAB: 经典实现，复杂，维护着色(coloring)对齐
    SLUB: 现代默认，简化设计，更好的调试支持 (Linux 2.6.23+)
    SLOB: 嵌入式，极简，适合内存 <64MB 的系统

  SLUB 的 per-CPU 缓存避免了锁竞争：
    分配: 先从 per-cpu 空闲链表取 → 无锁 O(1)
    空了: 从 partial list 取一个 slab → 加锁
    还空: 从伙伴系统申请新页面

  查看 SLAB 信息:
    cat /proc/slabinfo
    slabtop -s c   (按缓存大小排序)
)";

#if LINUX_ONLY
    // 读取 slabinfo 前几行
    std::cout << "  /proc/slabinfo 前 10 行:\n";
    std::ifstream slabinfo("/proc/slabinfo");
    std::string line;
    int n = 0;
    while (std::getline(slabinfo, line) && n++ < 10) {
        std::cout << "    " << line << "\n";
    }
#endif

    // 简单的 Slab 分配器模拟
    std::cout << "\n  简化 Slab 分配器模拟:\n";

    class SimpleSlab {
        size_t obj_size_;
        size_t slab_size_;       // 一个 slab 的大小（页面对齐）
        size_t objs_per_slab_;
        std::vector<std::vector<char>> slabs_;
        std::vector<void*> free_list_;
        int alloc_count_ = 0;

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
            ++alloc_count_;
            return obj;
        }

        void free(void* obj) {
            free_list_.push_back(obj);
            --alloc_count_;
        }

        void print_stats() const {
            std::cout << "    对象大小: " << obj_size_ << " B"
                      << ", 每 slab: " << objs_per_slab_ << " 个对象"
                      << ", slab 数: " << slabs_.size()
                      << ", 已分配: " << alloc_count_
                      << ", 空闲: " << free_list_.size() << "\n";
        }

    private:
        void grow() {
            slabs_.emplace_back(slab_size_);
            char* base = slabs_.back().data();
            for (size_t i = 0; i < objs_per_slab_; ++i) {
                free_list_.push_back(base + i * obj_size_);
            }
        }
    };

    SimpleSlab slab_128(128);   // 模拟 kmalloc-128 缓存
    SimpleSlab slab_512(512);   // 模拟 kmalloc-512 缓存

    // 分配一些对象
    std::vector<void*> objs;
    for (int i = 0; i < 50; ++i) objs.push_back(slab_128.alloc());

    std::cout << "    kmalloc-128 (分配50个): ";
    slab_128.print_stats();

    // 释放一半
    for (int i = 0; i < 25; ++i) { slab_128.free(objs.back()); objs.pop_back(); }
    std::cout << "    kmalloc-128 (释放25个): ";
    slab_128.print_stats();
}

} // namespace ch7


// =============================================================================
// 第8章：mmap 与零拷贝
// =============================================================================

namespace ch8 {

void demo_mmap() {
    print_section("mmap 与零拷贝");

    std::cout << R"(
  mmap() 将文件或设备映射到进程地址空间：

  传统 read():
    磁盘 → 内核缓冲区 (Page Cache) → 用户缓冲区
    需要一次数据拷贝 + 两次上下文切换

  mmap():
    磁盘 → Page Cache ← 用户直接访问
    零拷贝！进程直接读写 Page Cache

  用法:
    void* addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
    // 读取 addr[0..length-1]，按需触发缺页异常加载
    munmap(addr, length);

  mmap 标志:
    MAP_PRIVATE   — 私有映射（copy-on-write）
    MAP_SHARED    — 共享映射（多进程可见，可写回文件）
    MAP_ANONYMOUS — 匿名映射（不关联文件，malloc 大块用这个）
    MAP_HUGETLB   — 使用大页
    MAP_POPULATE   — 预加载所有页面（避免后续缺页）

  应用场景:
    1. 大文件读取 (数据库, 日志分析)
    2. 进程间共享内存
    3. 内存分配 (malloc 大于 128KB 时使用 mmap)
    4. 动态链接器加载 .so 文件
)";

#if LINUX_ONLY
    // 演示：用 mmap 读取 /proc/self/status
    const char* path = "/proc/self/status";
    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        struct stat st;
      if (fstat(fd, &st) != 0) {
        std::cout << "  fstat(" << path << ") 失败: " << strerror(errno) << "\n";
        close(fd);
      } else if (st.st_size > 0) {
            void* addr = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (addr != MAP_FAILED) {
                std::cout << "  mmap 读取 " << path << " (前 200 字节):\n    ";
                std::string content((char*)addr, std::min((size_t)200, (size_t)st.st_size));
                std::cout << content.substr(0, content.rfind('\n')) << "\n";
                munmap(addr, st.st_size);
            }
      } else {
        std::cout << "  mmap 读取 " << path << "：该文件在 procfs 中可能报告大小为 0，改用 read() 路径更可靠\n";
        std::cout << "    " << read_first_line(path) << "\n";
        }
        close(fd);
    }

    // 匿名 mmap
    void* anon = mmap(nullptr, 4096, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (anon != MAP_FAILED) {
        memcpy(anon, "mmap anonymous works!", 22);
        std::cout << "  匿名 mmap: " << (char*)anon << "\n";
        munmap(anon, 4096);
    }

    // madvise 提示内核
    std::cout << R"(
  madvise() 优化提示:
    MADV_SEQUENTIAL — 顺序访问 → 预读取更多
    MADV_RANDOM     — 随机访问 → 减少预读
    MADV_WILLNEED   — 即将访问 → 提前加载
    MADV_DONTNEED   — 不再需要 → 可以释放
    MADV_HUGEPAGE   — 建议使用透明大页
)";
#endif
}

} // namespace ch8


// =============================================================================
// 第9章：内存映射 I/O 与 DMA
// =============================================================================

namespace ch9 {

void demo_mmio_dma() {
    print_section("MMIO 与 DMA");

    std::cout << R"(
  I/O 访问方式演进：

  1. 端口 I/O (Port I/O) — x86 传统
     in/out 指令，独立的 I/O 地址空间 (0x0000-0xFFFF)
     例: inb(0x60) 读取键盘

  2. 内存映射 I/O (MMIO) — 现代主流
     设备寄存器映射到物理地址空间
     CPU 用普通 load/store 指令访问
     ┌──────────────────────────────────────┐
     │ 物理地址空间                         │
     │ 0x0000_0000 ─ 0x3FFF_FFFF: DRAM     │
     │ 0xFE00_0000 ─ 0xFEFF_FFFF: PCIe BAR │ ← MMIO
     │ 0xFF00_0000 ─ 0xFFFF_FFFF: 固件     │
     └──────────────────────────────────────┘

  3. DMA (Direct Memory Access) — 高吞吐
     设备直接读写主存，不经过 CPU
     ┌───┐        总线        ┌──────┐
     │CPU│◄─────────────────►│ RAM  │
     └───┘                    └──┬───┘
                                 │ DMA
                            ┌────┴────┐
                            │ NIC/GPU │
                            └─────────┘

  DMA 关键概念：
    - 一致性 DMA (coherent): CPU 和设备看到相同内存内容
    - 流式 DMA (streaming): 需要手动同步 (dma_map/unmap)
    - IOMMU: 设备的"MMU"，隔离设备地址空间（安全+虚拟化）
    - Scatter-Gather DMA: 一次传输多个不连续内存块

  内核 DMA API:
    dma_alloc_coherent(dev, size, &dma_handle, GFP_KERNEL)
    dma_map_single(dev, vaddr, size, DMA_TO_DEVICE)
    dma_unmap_single(dev, dma_handle, size, DMA_TO_DEVICE)

  用户态 DMA:
    DPDK (Data Plane Development Kit) — 网络零拷贝
    SPDK (Storage Performance Development Kit) — 存储零拷贝
    RDMA (Remote DMA) — 网络 InfiniBand/RoCE
)";
}

} // namespace ch9


// =============================================================================
// 第10章：OOM Killer 与内存压力
// =============================================================================

namespace ch10 {

void demo_oom() {
    print_section("OOM Killer 与内存压力处理");

    std::cout << R"(
  当系统物理内存耗尽，Linux OOM (Out-Of-Memory) Killer 选择
  并杀死进程以释放内存。

  OOM 评分机制：
    /proc/<pid>/oom_score     — 当前 OOM 分数 (越高越容易被杀)
    /proc/<pid>/oom_score_adj — 调整值 (-1000 ~ 1000)
      -1000 = 永不被 OOM 杀死
       1000 = 优先被杀
         0  = 默认

  评分因素：
    1. 进程内存使用量（RSS 越大分越高）
    2. 子进程内存
    3. oom_score_adj 调整
    4. root 进程有 3% 的优惠

  保护关键进程:
    echo -1000 > /proc/<pid>/oom_score_adj   # 永不被杀
    echo  500  > /proc/<pid>/oom_score_adj   # 优先被杀

  内存 overcommit 策略：
    /proc/sys/vm/overcommit_memory
      0 — 启发式 (默认, 允许适度 overcommit)
      1 — 总是允许 (可能 OOM)
      2 — 严格，不超过 swap + ratio% × RAM

    /proc/sys/vm/overcommit_ratio = 50 (默认)
      模式2下: 可分配 = swap + 50% × RAM
)";

#if LINUX_ONLY
    // 读取当前进程的 OOM 分数
    std::cout << "  当前进程 OOM 分数: "
              << read_first_line("/proc/self/oom_score") << "\n";
    std::cout << "  当前 OOM 调整值: "
              << read_first_line("/proc/self/oom_score_adj") << "\n";
    std::cout << "  overcommit_memory: "
              << read_first_line("/proc/sys/vm/overcommit_memory") << "\n";

    // 内存压力 (PSI - Pressure Stall Information, Linux 4.20+)
    std::string psi = read_proc_file("/proc/pressure/memory");
    if (psi.find("无法") == std::string::npos) {
        std::cout << "\n  内存压力 (/proc/pressure/memory):\n";
        std::istringstream iss(psi);
        std::string line;
        while (std::getline(iss, line)) {
            std::cout << "    " << line << "\n";
        }
        std::cout << R"(
    some: 至少一个任务因内存而阻塞
    full: 所有任务都因内存而阻塞
    avg10/60/300: 10秒/60秒/300秒内的压力百分比
)";
    }
#endif
}

} // namespace ch10


// =============================================================================
// ██ 三、文件系统与 I/O 篇
// =============================================================================

// =============================================================================
// 第11章：VFS 虚拟文件系统
// =============================================================================

namespace ch11 {

void demo_vfs() {
    print_section("VFS 虚拟文件系统抽象");

    std::cout << R"(
  VFS (Virtual File System) 是 Linux 的文件系统抽象层。

  核心数据结构：

  1. superblock — 文件系统全局信息
     struct super_block {
         struct file_system_type *s_type;  // ext4, xfs, btrfs...
         struct super_operations *s_op;
         unsigned long s_blocksize;
         struct dentry *s_root;            // 根目录
         ...
     };

  2. inode — 文件元数据（一个文件一个 inode）
     struct inode {
         umode_t i_mode;         // 权限 (rwxrwxrwx)
         uid_t   i_uid;          // 所有者
         loff_t  i_size;         // 大小
         struct timespec64 i_atime, i_mtime, i_ctime;
         struct inode_operations *i_op;    // 操作表
         struct file_operations *i_fop;
         unsigned long i_ino;    // inode 号
         ...
     };

  3. dentry — 目录项缓存（路径 → inode 的映射）
     struct dentry {
         struct inode *d_inode;
         struct dentry *d_parent;
         struct qstr d_name;     // 文件名
         ...
     };
     路径查找: /usr/local/bin/gcc
       → dentry("/") → dentry("usr") → dentry("local") → dentry("bin") → dentry("gcc")
       每一级都查 dentry 缓存，miss 才读磁盘

  4. file — 打开的文件实例（进程级）
     struct file {
         struct path f_path;         // dentry + mount
         struct inode *f_inode;
         const struct file_operations *f_op;  // read/write/mmap/ioctl...
         loff_t f_pos;               // 文件偏移
         unsigned int f_flags;       // O_RDONLY, O_NONBLOCK...
         ...
     };

  调用链：
    用户 read(fd, buf, len)
    → 系统调用 sys_read()
    → VFS: file->f_op->read()
    → 具体文件系统: ext4_file_read_iter()
    → Page Cache 查找
    → 未命中: 磁盘 I/O

  文件系统注册:
    register_filesystem(&ext4_fs_type);
    ext4_fs_type.mount = ext4_mount;
)";

#if LINUX_ONLY
    // 查看已挂载的文件系统
    std::cout << "  已挂载的文件系统 (/proc/mounts 前10行):\n";
    std::ifstream mounts("/proc/mounts");
    std::string line;
    int n = 0;
    while (std::getline(mounts, line) && n++ < 10) {
        std::cout << "    " << line << "\n";
    }

    // 支持的文件系统
    std::cout << "\n  内核支持的文件系统:\n    ";
    std::string fs = read_proc_file("/proc/filesystems");
    int count = 0;
    std::istringstream iss(fs);
    std::string token;
    while (iss >> token) {
        if (token != "nodev") {
            std::cout << token << " ";
            if (++count % 8 == 0) std::cout << "\n    ";
        }
    }
    std::cout << "\n";
#endif
}

} // namespace ch11


// =============================================================================
// 第12章：Page Cache 与 I/O 调度
// =============================================================================

namespace ch12 {

void demo_page_cache() {
    print_section("Page Cache 与 I/O 调度");

    std::cout << R"(
  Page Cache 是内核的文件数据缓存，位于内存中。

  读取流程：
    read(fd) → 检查 Page Cache
    ├── 命中 → 直接返回 (内存速度 ~10ns)
    └── 未命中 → 从磁盘读取 → 放入 Page Cache → 返回

  写入流程：
    write(fd) → 写入 Page Cache (标记为 dirty)
    → 后台 writeback 线程定期刷盘
    → 或 fsync(fd) 强制刷盘

  脏页回写参数：
    /proc/sys/vm/dirty_ratio = 20
      进程写脏页超过可用内存 20% 时，同步写磁盘（阻塞）
    /proc/sys/vm/dirty_background_ratio = 10
      后台脏页超过 10% 时，启动后台回写（不阻塞）
    /proc/sys/vm/dirty_expire_centisecs = 3000
      脏页超过 30 秒自动回写
    /proc/sys/vm/dirty_writeback_centisecs = 500
      回写线程每 5 秒检查一次

  I/O 调度器 (块设备层)：
    - none/noop：不排序，直接提交（NVMe SSD 推荐）
    - mq-deadline：保证请求延迟上限
    - bfq：公平队列，适合桌面交互
    - kyber：低延迟+高吞吐自动平衡

    查看/修改:
      cat /sys/block/sda/queue/scheduler
      echo "mq-deadline" > /sys/block/sda/queue/scheduler

  预读 (Readahead):
    /sys/block/sda/queue/read_ahead_kb = 128
    内核自动检测顺序读取模式并预读后续页面
)";

#if LINUX_ONLY
    // 查看 Page Cache 状态
    std::cout << "  内存信息 (/proc/meminfo 摘要):\n";
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("Cached:") == 0 ||
            line.find("Buffers:") == 0 ||
            line.find("Dirty:") == 0 ||
            line.find("Writeback:") == 0 ||
            line.find("MemTotal:") == 0 ||
            line.find("MemAvailable:") == 0) {
            std::cout << "    " << line << "\n";
        }
    }

    // dirty 参数
    std::cout << "\n  脏页参数:\n";
    std::cout << "    dirty_ratio: " << read_first_line("/proc/sys/vm/dirty_ratio") << "%\n";
    std::cout << "    dirty_background_ratio: " << read_first_line("/proc/sys/vm/dirty_background_ratio") << "%\n";
#endif
}

} // namespace ch12


// =============================================================================
// 第13章：io_uring 异步 I/O
// =============================================================================

namespace ch13 {

void demo_io_uring() {
    print_section("io_uring 异步 I/O (Linux 5.1+)");

    std::cout << R"(
  io_uring 是 Linux 最新的异步 I/O 接口（取代 AIO）。

  为什么需要 io_uring:
    - select/poll/epoll: 只通知就绪，读写本身仍是同步的
    - AIO (libaio): 仅支持 O_DIRECT，不支持 buffered I/O
    - io_uring: 统一异步接口，支持所有 I/O 操作

  核心设计——共享环形缓冲区（避免系统调用开销）：

    ┌──────── 用户态 ────────┐  ┌──────── 内核态 ────────┐
    │                        │  │                         │
    │  Submission Queue (SQ) │──│──→  处理请求            │
    │  ┌─┬─┬─┬─┬─┬─┐       │  │                         │
    │  │1│2│3│ │ │ │       │  │  Completion Queue (CQ)  │
    │  └─┴─┴─┴─┴─┴─┘       │  │  ┌─┬─┬─┬─┬─┬─┐        │
    │        ↑ 应用写入      │  │  │✓│✓│ │ │ │ │ ←kernel │
    │                        │  │  └─┴─┴─┴─┴─┴─┘        │
    │  CQ 应用消费 ↓         │──│──←                      │
    └────────────────────────┘  └─────────────────────────┘

    SQ/CQ 通过 mmap 共享，提交/完成可以不需要系统调用！

  API:
    io_uring_setup()         — 创建 io_uring 实例
    io_uring_enter()         — 提交请求 / 等待完成
    io_uring_register()      — 注册文件/缓冲区（减少引用开销）

  支持的操作 (50+):
    - 文件: read, write, fsync, fallocate, openat, close
    - 网络: accept, connect, recv, send, recvmsg, sendmsg
    - 高级: timeout, link, cancel, poll_add
    - 零拷贝: send_zc (5.19+), recv_multishot

  SQE (Submission Queue Entry) 关键字段:
    struct io_uring_sqe {
        __u8  opcode;     // IORING_OP_READ, IORING_OP_WRITE...
        __u8  flags;      // IOSQE_IO_LINK (链式请求)
        __s32 fd;
        __u64 off;        // 文件偏移
        __u64 addr;       // 缓冲区地址
        __u32 len;
        __u64 user_data;  // 完成时原样返回
    };

  性能数据 (参考):
    - 传统 read(): ~4μs/次 (含系统调用)
    - io_uring batched: ~0.3μs/次 (大批量时)
    - io_uring sqpoll: ~0.1μs/次 (内核轮询模式，无需系统调用)

  推荐库: liburing (简化 io_uring 使用)
)";

    // io_uring 伪代码
    std::cout << R"(
  liburing 使用示例 (伪代码):

    struct io_uring ring;
    io_uring_queue_init(256, &ring, 0);   // 队列深度 256

    // 提交读请求
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, len, offset);
    sqe->user_data = 42;
    io_uring_submit(&ring);

    // 等待完成
    struct io_uring_cqe *cqe;
    io_uring_wait_cqe(&ring, &cqe);
    int result = cqe->res;           // 读取的字节数
    uint64_t id = cqe->user_data;    // 42
    io_uring_cqe_seen(&ring, cqe);

    io_uring_queue_exit(&ring);
)";
}

} // namespace ch13


// =============================================================================
// ██ 四、网络栈篇
// =============================================================================

// =============================================================================
// 第14章：TCP/IP 协议栈分层
// =============================================================================

namespace ch14 {

void demo_tcp_stack() {
    print_section("TCP/IP 协议栈分层架构");

    std::cout << R"(
  Linux 网络报文处理路径（接收方向 RX）：

  ┌─────────────────────────────────────────────────────────────┐
  │  应用层: read()/recv()/recvmsg()                           │
  │   ↑ 从 socket 缓冲区拷贝到用户缓冲区                       │
  ├─────────────────────────────────────────────────────────────┤
  │  传输层: tcp_v4_rcv() / udp_rcv()                         │
  │   ↑ TCP: 序号检查, ACK, 流控, 拥塞控制                     │
  │   ↑ 数据放入 socket 接收队列 (sk->sk_receive_queue)        │
  ├─────────────────────────────────────────────────────────────┤
  │  网络层: ip_rcv() → ip_local_deliver()                     │
  │   ↑ IP 头部解析, 路由查找, Netfilter 钩子                  │
  ├─────────────────────────────────────────────────────────────┤
  │  链路层: netif_receive_skb() → __netif_receive_skb()       │
  │   ↑ 协议分发 (ETH_P_IP → ip_rcv)                          │
  ├─────────────────────────────────────────────────────────────┤
  │  驱动层: NAPI poll → napi_gro_receive()                    │
  │   ↑ 从 DMA ring buffer 取包，构造 sk_buff                  │
  ├─────────────────────────────────────────────────────────────┤
  │  硬件: NIC → DMA → ring buffer → 中断/NAPI                │
  └─────────────────────────────────────────────────────────────┘

  发送方向 (TX)：
    send() → tcp_sendmsg() → ip_queue_xmit() → dev_queue_xmit()
    → NIC 驱动 → DMA → 网线

  NAPI (New API):
    传统: 每个包一个中断 → 高速网络中断风暴
    NAPI: 中断触发后切换到 poll 模式，批量处理
    → 大幅降低高吞吐下的 CPU 开销

  GRO (Generic Receive Offload):
    在驱动层合并多个小包为大包，减少协议栈处理次数

  TCP 拥塞控制算法:
    cat /proc/sys/net/ipv4/tcp_congestion_control
    - cubic (默认, 高BDP友好)
    - bbr   (Google, 基于带宽和RTT)
    - reno  (经典)

    切换: sysctl -w net.ipv4.tcp_congestion_control=bbr
)";

#if LINUX_ONLY
    // 读取网络参数
    std::cout << "  当前网络参数:\n";
    std::cout << "    tcp_congestion: " << read_first_line("/proc/sys/net/ipv4/tcp_congestion_control") << "\n";
    std::cout << "    tcp_wmem: " << read_first_line("/proc/sys/net/ipv4/tcp_wmem") << "\n";
    std::cout << "    tcp_rmem: " << read_first_line("/proc/sys/net/ipv4/tcp_rmem") << "\n";
    std::cout << "    somaxconn: " << read_first_line("/proc/sys/net/core/somaxconn") << "\n";
    std::cout << "    tcp_max_syn_backlog: " << read_first_line("/proc/sys/net/ipv4/tcp_max_syn_backlog") << "\n";
#endif
}

} // namespace ch14


// =============================================================================
// 第15章：sk_buff 结构
// =============================================================================

namespace ch15 {

void demo_skbuff() {
    print_section("Socket 缓冲区 (sk_buff)");

    std::cout << R"(
  sk_buff 是 Linux 网络栈最核心的数据结构，代表一个网络报文。

  struct sk_buff {
      // 指针区域
      unsigned char *head;  // 缓冲区起始
      unsigned char *data;  // 当前层有效数据起始
      unsigned char *tail;  // 当前层有效数据结束
      unsigned char *end;   // 缓冲区结束

      // 头部指针
      struct {
          struct iphdr    *iph;        // IP 头
          struct tcphdr   *th;         // TCP 头
          struct ethhdr   *eth;        // 以太头
          ...
      } headers;

      struct sock *sk;                 // 关联的 socket
      struct net_device *dev;          // 网络设备
      unsigned int len;                // 数据长度
      ...
  };

  内存布局:
  head────────┐
              │ headroom (预留空间，供添加头部)
  data────────┤ ← skb_push() 向前扩展
              │ 实际数据
  tail────────┤ ← skb_put() 向后扩展
              │ tailroom
  end─────────┘

  接收时的层层剥离:
    驱动: data → 以太帧头部
    链路层: skb_pull(ETH_HLEN) → data 移到 IP 头
    网络层: skb_pull(IP header) → data 移到 TCP 头
    传输层: skb_pull(TCP header) → data 移到应用数据

  发送时的层层添加:
    应用: 放入 payload
    传输层: skb_push(TCP header)
    网络层: skb_push(IP header)
    链路层: skb_push(ETH header)

  关键操作:
    skb_put(skb, len)   — 向 tail 方向扩展
    skb_push(skb, len)  — 向 head 方向扩展
    skb_pull(skb, len)  — 从 data 处剥离
    skb_reserve(skb, n) — 预留 headroom

  性能优化:
    - skb 由 SLAB 缓存管理（快速分配/释放）
    - 零拷贝: 多个 skb 可以共享数据区域 (skb_clone)
    - scatter-gather: skb_shinfo(skb)->frags[] 指向多个页面
)";
}

} // namespace ch15


// =============================================================================
// 第16章：Netfilter / iptables
// =============================================================================

namespace ch16 {

void demo_netfilter() {
    print_section("Netfilter / iptables 钩子链");

    std::cout << R"(
  Netfilter 是 Linux 内核的包过滤框架。
  iptables/nftables 是其用户态前端。

  Netfilter 五个钩子点:

  网络报文 ─→ ① PREROUTING ─→ 路由判断
                                  │
                ┌─────────────────┴─────────────────┐
                ↓ (目标是本机)                       ↓ (转发)
           ② INPUT                            ③ FORWARD
                ↓                                    ↓
            本地进程                            ④ POSTROUTING ──→ 出站
                ↓
           ⑤ OUTPUT ──→ 路由判断 ──→ POSTROUTING ──→ 出站

  钩子优先级 (数值越小越先执行):
    NF_IP_PRI_CONNTRACK_DEFRAG = -400  (碎片重组)
    NF_IP_PRI_RAW             = -300  (raw 表)
    NF_IP_PRI_CONNTRACK       = -200  (连接跟踪)
    NF_IP_PRI_MANGLE          = -150  (mangle 表)
    NF_IP_PRI_NAT_DST         = -100  (DNAT)
    NF_IP_PRI_FILTER          =  0    (filter 表)
    NF_IP_PRI_NAT_SRC         =  100  (SNAT)

  四张表 × 五个链:
    ┌──────────┬──────────┬──────┬────────┬─────────────┐
    │          │PREROUTING│INPUT │FORWARD │OUTPUT│POSTROUTING│
    ├──────────┼──────────┼──────┼────────┼──────┼───────────┤
    │ raw      │    ✓     │      │        │  ✓   │           │
    │ mangle   │    ✓     │  ✓   │   ✓    │  ✓   │     ✓     │
    │ nat      │    ✓     │  ✓   │        │  ✓   │     ✓     │
    │ filter   │          │  ✓   │   ✓    │  ✓   │           │
    └──────────┴──────────┴──────┴────────┴──────┴───────────┘

  conntrack (连接跟踪):
    跟踪每个连接的状态: NEW, ESTABLISHED, RELATED, INVALID
    是 NAT 和有状态防火墙的基础
    cat /proc/net/nf_conntrack          # 查看连接表
    sysctl net.nf_conntrack_max=262144  # 调整最大连接数

  nftables (iptables 的替代品, Linux 3.13+):
    nft add table ip filter
    nft add chain ip filter input { type filter hook input priority 0 \; }
    nft add rule ip filter input tcp dport 80 accept
)";

#if LINUX_ONLY
    // 查看 conntrack 最大数
    std::cout << "  nf_conntrack_max: "
              << read_first_line("/proc/sys/net/nf_conntrack_max") << "\n";
#endif
}

} // namespace ch16


// =============================================================================
// 第17章：epoll 内核实现原理
// =============================================================================

namespace ch17 {

void demo_epoll_internals() {
    print_section("epoll 内核实现原理");

    std::cout << R"(
  epoll 是 Linux 高性能事件通知机制，O(1) 复杂度。

  内核数据结构：

  struct eventpoll {
      struct rb_root_cached rbr;    // 红黑树：所有被监听的 fd
      struct list_head rdllist;     // 就绪链表：有事件的 fd
      wait_queue_head_t wq;         // 等待 epoll_wait 的进程
      ...
  };

  struct epitem {
      struct rb_node rbn;           // 红黑树节点
      struct list_head rdllink;     // 就绪链表节点
      struct epoll_filefd ffd;      // {file*, fd}
      struct epoll_event event;     // 关注的事件
      struct eventpoll *ep;         // 所属 eventpoll
      ...
  };

  工作流程:

  1. epoll_create()
     → 创建 eventpoll 结构（红黑树 + 就绪链表 + 等待队列）

  2. epoll_ctl(ADD, fd)
     → 创建 epitem，插入红黑树 O(log n)
     → 在 fd 对应的设备等待队列上注册回调函数 ep_poll_callback

  3. 当 fd 有事件到达（如 socket 收到数据）
     → 设备中断/软中断 → 唤醒等待队列
     → ep_poll_callback() 被调用
     → 将 epitem 加入就绪链表 rdllist（O(1)）
     → 唤醒 epoll_wait 的进程

  4. epoll_wait()
     → 检查就绪链表，非空则返回
     → 空则睡眠等待 → 被 ep_poll_callback 唤醒
     → 将就绪事件拷贝到用户空间
     → 返回就绪 fd 数量

  select vs poll vs epoll:
  ┌─────────┬────────────┬──────────┬──────────────┐
  │         │  select    │  poll    │  epoll       │
  ├─────────┼────────────┼──────────┼──────────────┤
  │ 数据结构│ fd_set位图 │ pollfd[] │ 红黑树+链表  │
  │ fd上限  │ 1024       │ 无限     │ 无限         │
  │ 复杂度  │ O(n)       │ O(n)    │ O(1)就绪通知 │
  │ 拷贝    │ 每次全量   │ 每次全量│ 只在ctl时    │
  │ 触发模式│ LT         │ LT      │ LT + ET      │
  └─────────┴────────────┴──────────┴──────────────┘

  ET (Edge Triggered) vs LT (Level Triggered)：
    LT: 只要可读就一直通知（简单但可能惊群）
    ET: 状态变化时通知一次（高效但必须一次读完）

  EPOLLONESHOT: 事件触发后自动移除，防止多线程竞争
  EPOLLEXCLUSIVE: 防止惊群 (Linux 4.5+)
)";

#if LINUX_ONLY
    // 简易 epoll 使用演示
    std::cout << "  epoll API 演示:\n";

    int epfd = epoll_create1(0);
    if (epfd >= 0) {
        // 创建一个 pipe 作为演示 fd
        int pipefd[2];
        pipe(pipefd);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;     // 读事件 + 边缘触发
        ev.data.fd = pipefd[0];
        epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[0], &ev);

        // 写入一些数据
        write(pipefd[1], "hello", 5);

        // epoll_wait
        struct epoll_event events[10];
        int nfds = epoll_wait(epfd, events, 10, 100); // 100ms 超时

        std::cout << "    就绪 fd 数: " << nfds << "\n";
        for (int i = 0; i < nfds; ++i) {
            char buf[64] = {};
            int n = read(events[i].data.fd, buf, sizeof(buf));
            std::cout << "    fd=" << events[i].data.fd
                      << " 读到: \"" << buf << "\" (" << n << " bytes)\n";
        }

        close(pipefd[0]); close(pipefd[1]);
        close(epfd);
    }
#endif
}

} // namespace ch17


// =============================================================================
// 第18章：零拷贝网络
// =============================================================================

namespace ch18 {

void demo_zero_copy_network() {
    print_section("零拷贝网络: sendfile / splice");

    std::cout << R"(
  传统文件发送 (4次拷贝, 4次上下文切换):
    1. read(file_fd, buf, len)  → 磁盘 → 内核缓冲区 → 用户缓冲区
    2. write(sock_fd, buf, len) → 用户缓冲区 → 内核 socket 缓冲区 → NIC

  sendfile (2次拷贝, 2次切换) — Linux 2.2+:
    sendfile(sock_fd, file_fd, &offset, len)
    → 磁盘 → 内核缓冲区 → ✗ ─(DMA gather)→ NIC
    数据不经过用户空间！

  带 DMA gather copy 的 sendfile (零拷贝):
    → 磁盘 DMA → Page Cache
    → 只拷贝描述符 (位置+长度) 到 socket 缓冲区
    → NIC DMA 直接从 Page Cache 取数据
    真正的零 CPU 拷贝！

  splice (Linux 2.6.17+) — 更通用:
    splice(pipe_rd, NULL, sock_fd, NULL, len, 0)
    在任意两个 fd 之间移动数据（通过 pipe 中转）
    适用于: 文件→socket, socket→文件, socket→socket

  tee — 复制 pipe 数据而不消费:
    tee(pipe_rd, pipe_wr, len, 0)
    用于: 日志镜像、流量复制

  MSG_ZEROCOPY (Linux 4.14+):
    send(fd, buf, len, MSG_ZEROCOPY)
    → 用户缓冲区直接被 NIC DMA 读取
    → 完成后通过 errqueue 通知应用可以释放缓冲区
    适用于: 大包发送 (>10KB)

  性能对比 (发送 1GB 文件):
    read+write: ~2000ms
    sendfile:   ~800ms
    splice:     ~750ms
    (具体取决于硬件和 DMA 支持)
)";
}

} // namespace ch18


// =============================================================================
// ██ 五、设备驱动篇
// =============================================================================

// =============================================================================
// 第19章：Linux 设备模型
// =============================================================================

namespace ch19 {

void demo_device_model() {
    print_section("Linux 设备模型");

    std::cout << R"(
  三类设备：

  1. 字符设备 (char device)
     - 按字节流访问，不支持随机寻址
     - /dev/tty, /dev/null, /dev/random, /dev/input/*
     - 操作: open, read, write, ioctl, close
     - 内核: struct cdev + file_operations

  2. 块设备 (block device)
     - 按块访问，支持随机寻址
     - /dev/sda, /dev/nvme0n1, /dev/loop0
     - 经过 I/O 调度器和 Page Cache
     - 内核: struct gendisk + block_device_operations

  3. 网络设备 (network device)
     - 不在 /dev 下，通过 socket API 访问
     - eth0, wlan0, lo
     - 内核: struct net_device + net_device_ops

  设备驱动注册流程 (字符设备):

    // 1. 分配主次设备号
    alloc_chrdev_region(&devno, 0, 1, "mydev");

    // 2. 初始化 cdev 并关联 file_operations
    struct cdev my_cdev;
    cdev_init(&my_cdev, &my_fops);
    cdev_add(&my_cdev, devno, 1);

    // 3. 创建设备节点 (/dev/mydev)
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

  sysfs 设备模型 (/sys):
    /sys/class/         — 按类别组织（net, block, input...）
    /sys/bus/           — 按总线组织（pci, usb, i2c...）
    /sys/devices/       — 物理设备树
    /sys/block/         — 块设备

  udev (用户态设备管理):
    内核发现设备 → uevent → udev 守护进程 → 创建 /dev 节点
    规则文件: /etc/udev/rules.d/
)";

#if LINUX_ONLY
    // 查看一些设备信息
    std::cout << "  块设备:\n";
    std::ifstream partitions("/proc/partitions");
    std::string line;
    int n = 0;
    while (std::getline(partitions, line) && n++ < 8) {
        std::cout << "    " << line << "\n";
    }
#endif
}

} // namespace ch19


// =============================================================================
// 第20章：中断处理
// =============================================================================

namespace ch20 {

void demo_interrupts() {
    print_section("中断处理：上半部/下半部");

    std::cout << R"(
  中断处理的矛盾：
    - 需要快速响应（不能关中断太久）
    - 但处理工作可能很多（如网络包解析）

  解决：分为上半部（top half）和下半部（bottom half）

  ┌─────── 上半部 (硬中断) ────────────────────────────────────┐
  │ - 中断上下文，不可睡眠                                     │
  │ - 关闭当前中断线（其他中断可嵌套）                          │
  │ - 只做最少工作：确认硬件、拷贝数据到缓冲区、调度下半部      │
  │ - 时间：几微秒                                             │
  └───────────────────────────────────────────────────────────┘
                              ↓ 调度
  ┌─────── 下半部 (延迟处理) ─────────────────────────────────┐
  │                                                            │
  │  softirq (软中断)                                          │
  │  - 编译时静态注册，优先级最高                               │
  │  - 同一 softirq 可以在多个CPU 上同时运行                   │
  │  - 种类: NET_TX, NET_RX, TIMER, TASKLET, SCHED, RCU...    │
  │                                                            │
  │  tasklet (基于 softirq)                                    │
  │  - 动态注册，同一 tasklet 不会在多个 CPU 上并行             │
  │  - 比 softirq 简单，大多数驱动用这个                       │
  │                                                            │
  │  workqueue (工作队列)                                      │
  │  - 进程上下文，可以睡眠！                                  │
  │  - 用内核线程池执行 (kworker/*)                            │
  │  - 适合需要长时间或可能阻塞的工作                          │
  │                                                            │
  │  threaded IRQ (线程化中断, Linux 2.6.30+)                  │
  │  - 中断处理在专用内核线程中运行                             │
  │  - 可以睡眠、使用 mutex                                    │
  │  - request_threaded_irq(irq, quick_handler, thread_fn,.)  │
  └────────────────────────────────────────────────────────────┘

  网络收包流程（完整版）：
    1. NIC 收到包 → DMA 写入 ring buffer
    2. 硬中断 → 驱动上半部 → 关闭 NIC 中断 → 调度 NAPI softirq
    3. ksoftirqd 处理 NET_RX softirq
    4. NAPI poll 函数批量收包 → 协议栈处理
    5. 处理完 → 重新开启 NIC 中断
)";

#if LINUX_ONLY
    // 查看中断统计
    std::cout << "  中断统计 (/proc/interrupts 前15行):\n";
    std::ifstream irq("/proc/interrupts");
    std::string line;
    int n = 0;
    while (std::getline(irq, line) && n++ < 15) {
        std::cout << "    " << line << "\n";
    };

    // softirq 统计
    std::cout << "\n  softirq 统计 (/proc/softirqs 前10行):\n";
    std::ifstream softirq("/proc/softirqs");
    n = 0;
    while (std::getline(softirq, line) && n++ < 10) {
        std::cout << "    " << line << "\n";
    }
#endif
}

} // namespace ch20


// =============================================================================
// ██ 六、内核同步篇
// =============================================================================

// =============================================================================
// 第21章：内核同步原语
// =============================================================================

namespace ch21 {

void demo_kernel_sync() {
    print_section("内核同步原语");

    std::cout << R"(
  内核中的锁比用户态更丰富，因为：
    - 有中断上下文（不能睡眠）
    - 有抢占（需要禁抢占）
    - SMP 多核需要缓存一致性

  ┌─────────────────────────────────────────────────────────────────┐
  │ 原语              │ 能否睡眠 │ 上下文         │ 开销  │ 场景  │
  ├───────────────────┼──────────┼───────────────┼──────┼───────┤
  │ raw_spinlock      │ ❌       │ 中断/进程     │ 最低 │ RT 锁 │
  │ spinlock          │ ❌       │ 中断/进程     │ 低   │ 短临界│
  │ rwlock            │ ❌       │ 中断/进程     │ 低   │ 读多  │
  │ seqlock           │ 读❌写❌ │ 中断/进程     │ 低   │ 读极多│
  │ mutex             │ ✅       │ 仅进程        │ 中   │ 通用  │
  │ rt_mutex          │ ✅       │ 仅进程        │ 中   │ 优先级│
  │ rw_semaphore      │ ✅       │ 仅进程        │ 中   │ 读多  │
  │ RCU               │ 读❌写✅ │ 中断/进程     │ 极低 │ 读极多│
  │ percpu counter    │ ❌       │ 任何          │ 极低 │ 统计  │
  │ atomic            │ ❌       │ 任何          │ 极低 │ 计数器│
  └───────────────────┴──────────┴───────────────┴──────┴───────┘

  RCU (Read-Copy-Update) — 内核最重要的同步机制:

  原理：读者无锁直接访问，写者拷贝-修改-替换指针-等待宽限期-释放旧版本

  读者:
    rcu_read_lock();           // 仅标记，几乎零开销
    ptr = rcu_dereference(p);  // 带内存屏障的读指针
    // 使用 ptr...
    rcu_read_unlock();

  写者:
    new_data = kmalloc(...);
    *new_data = *old_data;     // 拷贝
    new_data->field = new_val; // 修改
    rcu_assign_pointer(p, new_data); // 替换指针（原子）
    synchronize_rcu();         // 等待所有读者完成
    kfree(old_data);           // 释放旧数据

  宽限期 (Grace Period):
    ┌──读者A──┐
    │ rcu_lock │ rcu_unlock
    ┌─────────────────────┐
    │   写者更新指针        │ synchronize_rcu()  → kfree(old)
    └─────────────────────┘
                 ┌──读者B──┐
                 │ 看到新数据 │

  RCU 应用：路由表、文件系统 dcache、进程列表...
  Linux 内核中 RCU 使用超过 10000 处！

  spinlock 变种：
    spin_lock()              — 普通自旋
    spin_lock_irq()          — 自旋 + 关本地中断
    spin_lock_irqsave()      — 自旋 + 关中断 + 保存中断状态
    spin_lock_bh()           — 自旋 + 关下半部

    何时用哪个？
    - 只有进程上下文访问: spin_lock()
    - 中断也会访问: spin_lock_irqsave()
    - softirq 也会访问: spin_lock_bh()
)";
}

} // namespace ch21


// =============================================================================
// ██ 七、综合实战篇
// =============================================================================

// =============================================================================
// 第22章：/proc 与 /sys 信息采集
// =============================================================================

namespace ch22 {

void demo_proc_sys() {
    print_section("/proc 与 /sys 系统信息采集");

#if LINUX_ONLY
    // --- 系统基本信息 ---
    struct utsname uts;
    uname(&uts);
    std::cout << "  系统: " << uts.sysname << " " << uts.release
              << " " << uts.machine << "\n";

    // /proc/cpuinfo 摘要
    std::cout << "\n  CPU 信息:\n";
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    int cpu_count = 0;
    std::string model_name;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") == 0 && model_name.empty()) {
            model_name = line.substr(line.find(':') + 2);
        }
        if (line.find("processor") == 0) ++cpu_count;
    }
    std::cout << "    型号: " << model_name << "\n";
    std::cout << "    逻辑核数: " << cpu_count << "\n";

    // 负载
    std::cout << "    负载: " << read_first_line("/proc/loadavg") << "\n";

    // 运行时间
    std::string uptime_str = read_first_line("/proc/uptime");
    double uptime = 0.0;
    try {
      uptime = std::stod(uptime_str);
    } catch (...) {
      std::cout << "    运行时间: (无法解析 /proc/uptime)\n";
      uptime = -1.0;
    }
    if (uptime >= 0.0) {
    int days = (int)(uptime / 86400);
    int hours = ((int)uptime % 86400) / 3600;
    int mins = ((int)uptime % 3600) / 60;
    std::cout << "    运行时间: " << days << "天 " << hours << "时 " << mins << "分\n";
    }

    // 网络统计
    std::cout << "\n  网络接口统计:\n";
    std::ifstream netdev("/proc/net/dev");
    int n = 0;
    while (std::getline(netdev, line)) {
        if (n++ < 2) { std::cout << "    " << line << "\n"; continue; } // 头部
        if (n > 6) break;
        std::cout << "    " << line << "\n";
    }

    // 进程信息
    std::cout << "\n  当前进程:\n";
    std::cout << "    PID: " << getpid() << "\n";
    std::cout << "    PPID: " << getppid() << "\n";

    std::string status = read_proc_file("/proc/self/status");
    std::istringstream iss(status);
    while (std::getline(iss, line)) {
        if (line.find("VmPeak") == 0 || line.find("VmRSS") == 0 ||
            line.find("Threads") == 0 || line.find("Name") == 0 ||
            line.find("voluntary_ctxt_switches") == 0) {
            std::cout << "    " << line << "\n";
        }
    }
#else
    std::cout << "  (此章需要在 Linux 系统上运行)\n";
#endif
}

} // namespace ch22


// =============================================================================
// 第23章：性能分析工具链
// =============================================================================

namespace ch23 {

void demo_perf_tools() {
    print_section("性能分析工具链");

    std::cout << R"(
  ┌─────────────────────────────────────────────────────────────────┐
  │ 工具         │ 用途                     │ 命令示例              │
  ├──────────────┼─────────────────────────┼───────────────────────┤
  │ perf stat    │ 硬件计数器统计           │ perf stat ./app       │
  │ perf record  │ 采样 profiling           │ perf record ./app     │
  │ perf report  │ 分析采样结果             │ perf report           │
  │ perf top     │ 实时热点函数             │ perf top -p $PID      │
  │ perf sched   │ 调度分析                 │ perf sched record     │
  │ perf mem     │ 内存访问分析             │ perf mem record       │
  ├──────────────┼─────────────────────────┼───────────────────────┤
  │ ftrace       │ 内核函数跟踪             │ trace-cmd record -e.. │
  │ function_graph│ 函数调用图              │ echo ... > tracing/.. │
  ├──────────────┼─────────────────────────┼───────────────────────┤
  │ bpftrace     │ eBPF 动态跟踪           │ bpftrace -e '...'     │
  │ bcc tools    │ eBPF 工具集             │ biolatency, tcplife...│
  ├──────────────┼─────────────────────────┼───────────────────────┤
  │ strace       │ 系统调用跟踪             │ strace -c ./app       │
  │ ltrace       │ 库函数跟踪              │ ltrace ./app          │
  │ lsof         │ 打开的文件/socket        │ lsof -p $PID          │
  ├──────────────┼─────────────────────────┼───────────────────────┤
  │ vmstat       │ 内存/CPU/IO 概览        │ vmstat 1              │
  │ iostat       │ 磁盘 I/O 统计           │ iostat -x 1           │
  │ mpstat       │ 多核 CPU 统计           │ mpstat -P ALL 1       │
  │ pidstat      │ 进程级统计              │ pidstat -d 1          │
  │ sar          │ 历史性能数据             │ sar -u 1 10           │
  ├──────────────┼─────────────────────────┼───────────────────────┤
  │ flame graph  │ 火焰图                  │ perf script | stack.. │
  │ pprof        │ Go/C++ 性能分析         │ pprof ./app prof.out  │
  └──────────────┴─────────────────────────┴───────────────────────┘

  perf 使用示例:

    # 统计 cache miss 等硬件事件
    perf stat -e cache-misses,cache-references,instructions,cycles ./app

    # 采样 CPU 热点 (采样频率 99Hz)
    perf record -F 99 -g ./app
    perf report --stdio

    # 生成火焰图
    perf script | stackcollapse-perf.pl | flamegraph.pl > flame.svg

  ftrace 使用:
    cd /sys/kernel/debug/tracing
    echo function_graph > current_tracer
    echo tcp_sendmsg > set_graph_function
    echo 1 > tracing_on
    # ... 运行测试 ...
    echo 0 > tracing_on
    cat trace

  bpftrace 示例:
    # 统计系统调用
    bpftrace -e 'tracepoint:raw_syscalls:sys_enter { @[comm] = count(); }'

    # 跟踪 TCP 连接
    bpftrace -e 'kprobe:tcp_connect { printf("%s -> %s\n", comm, str(arg0)); }'
)";
}

} // namespace ch23


// =============================================================================
// 第24章：内核调优参数速查
// =============================================================================

namespace ch24 {

void demo_kernel_tuning() {
    print_section("内核调优参数速查手册");

    std::cout << R"(
  ═══ CPU 调度 ═══════════════════════════════════════════════════
  kernel.sched_latency_ns = 6000000        # CFS 调度延迟目标
  kernel.sched_min_granularity_ns = 750000 # 最小时间片
  kernel.sched_wakeup_granularity_ns = 1e6 # 唤醒粒度
  kernel.sched_rt_runtime_us = 950000      # RT 进程最大占比

  ═══ 内存管理 ═══════════════════════════════════════════════════
  vm.swappiness = 10                       # swap 倾向 (0-100, 低=少swap)
  vm.dirty_ratio = 20                      # 同步写的脏页比例
  vm.dirty_background_ratio = 10           # 后台写的脏页比例
  vm.overcommit_memory = 0                 # 内存 overcommit 策略
  vm.min_free_kbytes = 67584               # 最低空闲内存 (OOM 前预留)
  vm.zone_reclaim_mode = 0                 # NUMA 内存回收策略

  透明大页 (THP):
  /sys/kernel/mm/transparent_hugepage/enabled = madvise
    always:  全局启用 (可能导致延迟抖动)
    madvise: 仅 madvise(MADV_HUGEPAGE) 请求的区域
    never:   禁用 (低延迟场景推荐)

  ═══ 网络栈 ═══════════════════════════════════════════════════
  net.core.somaxconn = 4096                # listen backlog 上限
  net.core.netdev_max_backlog = 5000       # 网卡接收队列长度
  net.core.rmem_max = 16777216             # 最大接收缓冲区
  net.core.wmem_max = 16777216             # 最大发送缓冲区
  net.ipv4.tcp_max_syn_backlog = 4096      # SYN 队列长度
  net.ipv4.tcp_fin_timeout = 30            # FIN_WAIT2 超时
  net.ipv4.tcp_tw_reuse = 1               # 复用 TIME_WAIT
  net.ipv4.tcp_keepalive_time = 600        # keepalive 间隔
  net.ipv4.tcp_congestion_control = bbr    # 拥塞控制算法

  TCP 缓冲区自动调优:
  net.ipv4.tcp_rmem = 4096 87380 16777216  # min default max
  net.ipv4.tcp_wmem = 4096 65536 16777216

  net.ipv4.ip_local_port_range = 1024 65535 # 可用端口范围
  net.nf_conntrack_max = 262144            # conntrack 最大连接数

  ═══ 文件系统 ═══════════════════════════════════════════════════
  fs.file-max = 1048576                    # 系统级最大 fd 数
  fs.nr_open = 1048576                     # 进程级最大 fd 数
  fs.inotify.max_user_watches = 524288     # inotify 监控上限

  ═══ 一键调优脚本 ═══════════════════════════════════════════════
  # 高并发 Web 服务器示例:
  sysctl -w net.core.somaxconn=65535
  sysctl -w net.ipv4.tcp_max_syn_backlog=65535
  sysctl -w net.core.netdev_max_backlog=65535
  sysctl -w net.ipv4.tcp_fin_timeout=10
  sysctl -w net.ipv4.tcp_tw_reuse=1
  sysctl -w net.ipv4.tcp_congestion_control=bbr
  sysctl -w vm.swappiness=10
  ulimit -n 1048576

  持久化: 写入 /etc/sysctl.conf 然后 sysctl -p
)";

#if LINUX_ONLY
    // 显示当前关键参数
    std::cout << "\n  ═══ 当前系统关键参数 ═══\n";

    auto show = [](const char* name, const char* path) {
        std::string val = read_first_line(path);
        std::cout << "    " << std::setw(35) << std::left << name << " = " << val << "\n";
    };

    show("somaxconn", "/proc/sys/net/core/somaxconn");
    show("tcp_max_syn_backlog", "/proc/sys/net/ipv4/tcp_max_syn_backlog");
    show("swappiness", "/proc/sys/vm/swappiness");
    show("dirty_ratio", "/proc/sys/vm/dirty_ratio");
    show("file-max", "/proc/sys/fs/file-max");
    show("tcp_congestion_control", "/proc/sys/net/ipv4/tcp_congestion_control");
    show("tcp_fin_timeout", "/proc/sys/net/ipv4/tcp_fin_timeout");
    show("ip_local_port_range", "/proc/sys/net/ipv4/ip_local_port_range");
#endif
}

} // namespace ch24


// =============================================================================
// main
// =============================================================================

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "================================================================\n";
    std::cout << "     深入理解 Linux 内核机制 — 完全教程\n";
    std::cout << "================================================================\n";

    // 一、进程调度
    print_header("一、进程调度篇");
    ch1::demo_cfs();
    ch1::demo_cfs_tunables();
    ch2::demo_realtime_scheduling();
    ch3::demo_cpu_affinity();
    ch4::demo_cgroups();

    // 二、内存管理
    print_header("二、内存管理篇");
    ch5::demo_virtual_memory();
    ch6::demo_buddy_system();
    ch7::demo_slab();
    ch8::demo_mmap();
    ch9::demo_mmio_dma();
    ch10::demo_oom();

    // 三、文件系统与 I/O
    print_header("三、文件系统与 I/O 篇");
    ch11::demo_vfs();
    ch12::demo_page_cache();
    ch13::demo_io_uring();

    // 四、网络栈
    print_header("四、网络栈篇");
    ch14::demo_tcp_stack();
    ch15::demo_skbuff();
    ch16::demo_netfilter();
    ch17::demo_epoll_internals();
    ch18::demo_zero_copy_network();

    // 五、设备驱动
    print_header("五、设备驱动篇");
    ch19::demo_device_model();
    ch20::demo_interrupts();

    // 六、内核同步
    print_header("六、内核同步篇");
    ch21::demo_kernel_sync();

    // 七、综合实战
    print_header("七、综合实战篇");
    ch22::demo_proc_sys();
    ch23::demo_perf_tools();
    ch24::demo_kernel_tuning();

    std::cout << "\n================================================================\n";
    std::cout << "     演示完成\n";
    std::cout << "================================================================\n";

    return 0;
}
