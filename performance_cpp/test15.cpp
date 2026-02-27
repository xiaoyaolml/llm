// =============================================================================
// 高性能网络开发完全教程 — DPDK·Solarflare·RDMA·定制网卡
// =============================================================================
//
// 编译 (Linux with DPDK):
//   g++ -std=c++17 -O2 -march=native test15.cpp -o test15 \
//       $(pkg-config --cflags --libs libdpdk) -pthread
//
// 编译 (Linux without DPDK - 模拟模式):
//   g++ -std=c++17 -O2 -march=native -pthread -o test15 test15.cpp
//
// 编译 (Windows MSVC):
//   cl /std:c++17 /O2 /EHsc test15.cpp /link ws2_32.lib
//
// ─────────────────────────────────────────────────────────────
// 目录 (35 章)
// ═══════════════════════════════════════════════════════════════
//
// 一、网络性能基础篇
//   1.  传统网络栈延迟分析
//   2.  内核旁路技术对比
//   3.  零拷贝技术原理
//   4.  DMA 与环形缓冲区
//   5.  轮询模式 vs 中断模式
//
// 二、DPDK 核心篇
//   6.  DPDK 架构与原理
//   7.  DPDK 环境配置与初始化
//   8.  DPDK 内存管理 (Mempool)
//   9.  DPDK Mbuf 详解
//  10.  DPDK PMD 驱动架构
//  11.  DPDK 收发包基础
//  12.  DPDK 多核并行处理
//  13.  DPDK RSS (接收侧扩展)
//  14.  DPDK Flow Director
//
// 三、Solarflare/Xilinx 网卡篇
//  15.  Solarflare OpenOnload 原理
//  16.  ef_vi 零拷贝 API
//  17.  Onload 加速原理与配置
//  18.  PTP 硬件时间戳
//  19.  Solarflare TCPDirect
//
// 四、RDMA 篇
//  20.  RDMA 基本概念
//  21.  InfiniBand vs RoCE vs iWARP
//  22.  RDMA Verbs API
//  23.  RDMA 单边操作 (WRITE/READ)
//  24.  RDMA 双边操作 (SEND/RECV)
//  25.  RDMA CM (连接管理)
//
// 五、高级网络技术篇
//  26.  XDP (eXpress Data Path)
//  27.  AF_XDP Socket
//  28.  io_uring 网络优化
//  29.  SmartNIC 与 DPU
//  30.  FPGA 网卡加速
//
// 六、性能优化篇
//  31.  网络延迟测量与分析
//  32.  多队列网卡优化
//  33.  CPU 绑核与中断亲和性
//  34.  内存池与零分配设计
//  35.  生产环境网络调优清单
// ═══════════════════════════════════════════════════════════════

#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <memory>
#include <functional>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cassert>

#ifdef __linux__
  #include <unistd.h>
  #include <sys/socket.h>
  #include <sys/mman.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <linux/if_packet.h>
  #include <net/ethernet.h>
  #include <net/if.h>
  #include <sys/ioctl.h>
  #include <fcntl.h>
  #include <poll.h>
  #include <sched.h>
  #include <pthread.h>
#elif defined(_WIN32)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #pragma comment(lib, "ws2_32.lib")
#endif

// ─── 缓存行大小 ────────────────────────────────
#ifdef __cpp_lib_hardware_interference_size
  static constexpr size_t CACHE_LINE = std::hardware_destructive_interference_size;
#else
  static constexpr size_t CACHE_LINE = 64;
#endif

// ─── 工具函数 ──────────────────────────────────
static void print_header(const char* t) {
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n"
              << "║ " << std::left << std::setw(52) << t << " ║\n"
              << "╚══════════════════════════════════════════════════════╝\n\n";
}
static void print_section(const char* t) {
    std::cout << "  ── " << t << " ──\n";
}

class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point s_ = clk::now();
public:
    double ns()  const { return std::chrono::duration<double, std::nano>(clk::now()-s_).count(); }
    double us()  const { return ns()/1e3; }
    double ms()  const { return ns()/1e6; }
    void reset() { s_ = clk::now(); }
};

// =============================================================================
// ██  一、网络性能基础篇
// =============================================================================

// =====================================================================
// 第 1 章  传统网络栈延迟分析
// =====================================================================
namespace ch1 {
void demo() {
    print_section("第1章: 传统网络栈延迟分析");

    std::cout << R"(
  ═══ Linux 网络栈路径 ═══

  用户空间应用
      ↓ syscall (sendto/recvfrom)
  ┌─────────────────────────────────┐
  │  内核空间                        │
  │  ┌───────────────────────────┐  │
  │  │ Socket Layer              │  │
  │  │  - socket buffer          │  │
  │  │  - 拷贝用户数据            │  │  +500ns-1μs
  │  └──────────┬────────────────┘  │
  │             ↓                   │
  │  ┌───────────────────────────┐  │
  │  │ TCP/UDP Layer             │  │
  │  │  - 协议处理               │  │  +1-2μs
  │  │  - 校验和计算              │  │
  │  └──────────┬────────────────┘  │
  │             ↓                   │
  │  ┌───────────────────────────┐  │
  │  │ IP Layer                  │  │
  │  │  - 路由查找                │  │  +500ns
  │  │  - 分片                    │  │
  │  └──────────┬────────────────┘  │
  │             ↓                   │
  │  ┌───────────────────────────┐  │
  │  │ Device Driver (e.g. e1000)│  │
  │  │  - SKB 管理                │  │  +1μs
  │  │  - DMA 映射                │  │
  │  └──────────┬────────────────┘  │
  └─────────────┼───────────────────┘
                ↓
  ┌─────────────────────────────────┐
  │  网卡 (NIC)                      │
  │  - 中断/轮询                     │  +1-5μs
  │  - 发送队列                      │
  └─────────────────────────────────┘

  总延迟: 5-15 μs (单向, 不含网络传输)
  吞吐: ~1-2 Mpps (每秒百万包)

  ═══ 延迟分解 (发送端) ═══

  ┌────────────────────────┬─────────────┐
  │ 组件                    │ 延迟        │
  ├────────────────────────┼─────────────┤
  │ 用户→内核 (syscall)     │ ~100-300ns  │
  │ 数据拷贝 (用户→内核)    │ ~200-500ns  │
  │ TCP/IP 协议栈处理       │ ~1-3μs      │
  │ SKB 分配与管理          │ ~500ns      │
  │ 队列调度 (qdisc)       │ ~200-500ns  │
  │ 驱动层处理              │ ~500ns-1μs  │
  │ DMA 到网卡              │ ~500ns-2μs  │
  │ 网卡发送                │ ~1-5μs      │
  └────────────────────────┴─────────────┘
  总计: ~5-15μs (理想情况)

  ═══ 瓶颈分析 ═══

  1. 系统调用开销: 上下文切换 + 参数验证
  2. 数据拷贝: 用户态 → 内核态 (CPU 带宽消耗)
  3. 协议栈: 多层协议处理 (CPU 密集)
  4. 内存分配: SKB (Socket Buffer) 动态分配
  5. 中断: 频繁中断导致 cache thrashing
  6. 调度延迟: 进程/线程调度不确定性

  ═══ 优化方向 ═══

  → 内核旁路 (Kernel Bypass): DPDK, Solarflare
  → 零拷贝 (Zero Copy): sendfile, splice, MSG_ZEROCOPY
  → 用户态协议栈: 自行实现 TCP/IP
  → 硬件卸载: TSO, GSO, 校验和卸载
)";
}
} // namespace ch1


// =====================================================================
// 第 2 章  内核旁路技术对比
// =====================================================================
namespace ch2 {
void demo() {
    print_section("第2章: 内核旁路技术对比");

    std::cout << R"(
  ═══ 内核旁路技术对比表 ═══

  ┌──────────────┬──────────┬──────────┬────────────────────┬─────────────┐
  │ 技术          │ 延迟     │ 吞吐     │ 适用场景           │ 复杂度      │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ 传统 Socket  │ 5-15μs   │ 1-2Mpps  │ 通用应用           │ 低          │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ DPDK         │ 0.5-2μs  │ 10-100M  │ 数据平面/NFV       │ 中          │
  │ (Intel)      │          │ pps      │ 路由/防火墙        │             │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ Solarflare   │ 0.5-1μs  │ 10-50M   │ HFT 交易           │ 中-高       │
  │ OpenOnload   │          │ pps      │ 金融/低延迟        │             │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ Mellanox VMA │ 1-2μs    │ 10-50M   │ HPC/金融           │ 中          │
  │ (NVIDIA)     │          │ pps      │                    │             │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ RDMA         │ 0.7-2μs  │ 100Gbps+ │ HPC/存储/AI        │ 中-高       │
  │ (IB/RoCE)    │          │          │                    │             │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ io_uring     │ 2-5μs    │ 5-10Mpps │ 通用高性能I/O      │ 低-中       │
  │ (Linux 5.1+) │          │          │                    │             │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ XDP/AF_XDP   │ 1-3μs    │ 20-40M   │ 包过滤/DDoS防护    │ 中          │
  │              │          │ pps      │                    │             │
  ├──────────────┼──────────┼──────────┼────────────────────┼─────────────┤
  │ FPGA 网卡    │ 0.1-0.5μs│ 100Gbps+ │ 超低延迟交易       │ 高          │
  │ (Xilinx/AMD) │          │          │ 硬件加速           │             │
  └──────────────┴──────────┴──────────┴────────────────────┴─────────────┘

  ═══ DPDK (Data Plane Development Kit) ═══

  核心特性:
    ✓ 用户态 PMD (Poll Mode Driver) — 显著减少内核数据路径开销
    ✓ 大页内存 (Huge Pages) — 减少 TLB miss
    ✓ 无锁环形队列 — 高效跨核通信
    ✓ CPU 亲和性 — 绑核避免迁移
    ✓ NUMA 感知 — 本地内存访问

  典型应用: OVS-DPDK, VPP, F-Stack, Seastar

  ═══ Solarflare/Xilinx OpenOnload ═══

  核心特性:
    ✓ 透明加速 — 无需修改应用代码 (LD_PRELOAD)
    ✓ Kernel bypass + ef_vi — 直接访问网卡
    ✓ TCPDirect — 用户态 TCP 栈
    ✓ 硬件时间戳 — 在合适硬件/配置下可达亚微秒级抖动
    ✓ 低延迟 — 常见可达亚微秒量级

  典型应用: HFT/交易所/市场数据

  ═══ RDMA (Remote Direct Memory Access) ═══

  核心特性:
    ✓ 零拷贝 — 直接 DMA 到用户内存
    ✓ 单边操作 — 可显著降低远端 CPU 参与度
    ✓ 硬件卸载 — 协议处理可大幅下沉到网卡侧
    ✓ 可靠传输 — RC (Reliable Connection)
    ✓ RDMA CM — 连接建立管理

  典型应用: 分布式存储 (Ceph), AI训练, HPC

  ═══ 选择指南 ═══

  DPDK: 需要更强控制 + 高吞吐 → 数据平面
  Solarflare: 最低延迟 + TCP → 金融交易
  RDMA: 跨节点高带宽 + 零拷贝 → AI/HPC
  io_uring: 通用优化 + 兼容性好 → 现代应用
  XDP: 早期包过滤 + eBPF → DDoS/负载均衡
)";
}
} // namespace ch2


// =====================================================================
// 第 3 章  零拷贝技术原理
// =====================================================================
namespace ch3 {
void demo() {
    print_section("第3章: 零拷贝技术原理");

    std::cout << R"(
  ═══ 传统 I/O 路径 (4 次拷贝) ═══

  用户空间                    内核空间
  ┌────────────┐             ┌─────────────┐
  │ User Buffer│ ─── ① ────→ │ Page Cache  │
  └────────────┘             └──────┬──────┘
                                    │
                              ② (内核内部)
                                    │
                             ┌──────▼──────┐
                             │ Socket Buf  │
                             └──────┬──────┘
                                    │
                              ③ (DMA准备)
                                    │
                             ┌──────▼──────┐
                             │ NIC Buffer  │
                             └──────┬──────┘
                                    ▼
                              ④ 网卡发送

  拷贝次数: 4 次 (2 次 CPU 拷贝 + 2 次 DMA)
  CPU 参与: 用户→内核, 内核→SKB
  延迟: 每次拷贝 ~200-500ns @ 10Gbps

  ═══ 零拷贝技术 ═══

  1. sendfile() / splice()
     文件 → Socket 无需经过用户态
     DMA → Page Cache → DMA → NIC (2次DMA, 0次CPU拷贝)
     适用: 静态文件服务 (HTTP/FTP)

  2. MSG_ZEROCOPY (Linux 4.14+)
     用户 buffer 直接 DMA 到 NIC
     需要: 4KB 对齐 + 完成通知
     适用: 大块数据 (>10KB)

     int flag = MSG_ZEROCOPY;
     ssize_t sent = send(fd, buf, len, flag);
     // 通过 SO_EE_ORIGIN_ZEROCOPY 错误队列确认完成

  3. mmap() 共享内存
     网卡 buffer 映射到用户空间
     直接读写 → 无需拷贝
     适用: DPDK, RDMA, AF_PACKET

  4. vmsplice() + splice()
     管道零拷贝
     用户 pages → 管道 → socket

  ═══ DPDK 零拷贝 ═══

  ┌──────────────────────────────────────┐
  │  用户空间应用                        │
  │  ┌────────────────────────────────┐  │
  │  │ Mbuf (DMA 可访问区域)          │  │
  │  │ 直接指向网卡 Ring 中的包       │  │
  │  └──────────┬──────────────────────┘  │
  └─────────────┼─────────────────────────┘
                │ 零拷贝!
  ┌─────────────▼─────────────────────────┐
  │  网卡 RX/TX Ring (DMA 区域)          │
  │  ┌─────┬─────┬─────┬─────┬─────┐     │
  │  │ Pkt │ Pkt │ Pkt │ Pkt │ Pkt │     │
  │  └─────┴─────┴─────┴─────┴─────┘     │
  └───────────────────────────────────────┘

  应用直接操作网卡 DMA buffer → 0 次拷贝

  ═══ RDMA 零拷贝 ═══

  发送端:
    应用注册内存区域 (MR) → RNIC 直接读取
    ┌──────────┐
    │ App Buf  │ ──────→ RNIC ─────→ 网络
    │ (注册MR) │         (DMA读)
    └──────────┘

  接收端:
    RNIC 直接 DMA 写入应用 buffer
    网络 ─────→ RNIC ──────→ ┌──────────┐
                (DMA写)      │ App Buf  │
                             │ (注册MR) │
                             └──────────┘

  主要由硬件 DMA 路径处理, CPU 拷贝与内核参与可显著减少

  ═══ 零拷贝限制 ═══

  ❌ 小包 (<4KB): 设置/通知开销 > 拷贝开销
  ❌ 不对齐: DMA 通常需要页对齐 (4KB)
  ❌ 复杂性: 内存生命周期管理困难
  ❌ 修改数据: 无法在飞行中修改 (已 DMA 映射)

  最佳实践: 大块顺序传输用零拷贝, 小消息用标准拷贝
)";
}
} // namespace ch3


// =====================================================================
// 第 4 章  DMA 与环形缓冲区
// =====================================================================
namespace ch4 {
void demo() {
    print_section("第4章: DMA 与环形缓冲区");

    std::cout << R"(
  ═══ DMA (Direct Memory Access) ═══

  传统 I/O (PIO - Programmed I/O):
    CPU 逐字节搬运数据 → 浪费 CPU
    for (i = 0; i < len; ++i)
        nic_reg[i] = data[i];  // CPU 忙等写寄存器

  DMA I/O:
    1. CPU 设置 DMA 描述符 (源地址, 目标, 长度)
    2. DMA 控制器接管 → CPU 可以做其他事
    3. DMA 完成 → 中断通知 CPU

  ┌─────────┐
  │   CPU   │ ① 设置 DMA
  └────┬────┘
       │
  ┌────▼──────────────────┐
  │ DMA Controller        │
  │ ② 搬运数据 (无CPU参与)│
  └────┬────────────┬─────┘
       ▼            ▼
  ┌─────────┐  ┌────────┐
  │ Memory  │  │  NIC   │
  └─────────┘  └────────┘

  ═══ 网卡环形缓冲区 (Ring Buffer) ═══

  网卡与主机通过共享内存的环形队列通信

  RX Ring (接收):
  ┌───────────────────────────────────────────┐
  │  Descriptor Ring (DMA 可访问)             │
  │  ┌─────┬─────┬─────┬─────┬─────┬─────┐   │
  │  │Desc0│Desc1│Desc2│Desc3│Desc4│Desc5│   │
  │  └──┬──┴───┬─┴───┬─┴───┬─┴───┬─┴───┬─┘   │
  │     │      │     │     │     │     │     │
  │     ▼      ▼     ▼     ▼     ▼     ▼     │
  │  [Buf] [Buf] [Buf] [Buf] [Buf] [Buf]     │
  │   预分配的数据包缓冲区                    │
  └───────────────────────────────────────────┘
       ↑head (driver读)    ↑tail (NIC写)

  流程:
    1. 驱动预分配 buffer, 写描述符到 Ring
    2. 网卡收到包 → DMA 写入 buffer → 更新 tail
    3. 驱动轮询/中断 → 读 head → 处理包 → 释放 buffer

  TX Ring (发送):
  类似, 但方向相反:
    1. 驱动准备包 buffer → 写描述符 → 更新 tail
    2. 网卡从 head 读取 → DMA 读 buffer → 发送 → 更新 head
    3. 驱动回收已发送的 buffer

  ═══ Ring Size 优化 ═══

  小 Ring (64-256):
    ✓ 更少内存占用
    ✓ 更好的缓存局部性
    ❌ 容易溢出 (高负载)

  大 Ring (2048-4096):
    ✓ 高吞吐容忍突发
    ❌ 更多内存/缓存压力
    ❌ 更高延迟 (排队延迟)

  低延迟场景: 小 Ring (128-256) + 多队列
  高吞吐场景: 大 Ring (2048+)

  ═══ DPDK Ring 设计 ═══

  DPDK 使用无锁的 SPSC/MPMC Ring:
    - 2 的幂大小 (位运算取模)
    - Head/Tail 在独立 cache line (避免 false sharing)
    - Bulk enqueue/dequeue (批量操作)
    - 支持单生产者或多生产者模式

  struct rte_ring {
      uint32_t prod_head;  // 生产者头
      uint32_t prod_tail;  // 生产者尾
      uint32_t cons_head;  // 消费者头
      uint32_t cons_tail;  // 消费者尾
      void *ring[size];    // 环形缓冲区
  };

  性能: >100M enqueue/dequeue per second
)";
}
} // namespace ch4


// =====================================================================
// 第 5 章  轮询模式 vs 中断模式
// =====================================================================
namespace ch5 {
void demo() {
    print_section("第5章: 轮询模式 vs 中断模式");

    std::cout << R"(
  ═══ 中断驱动 (Interrupt-Driven) ═══

  工作流:
    1. 网卡收到包 → 产生硬件中断
    2. CPU 跳转到中断处理程序 (ISR)
    3. ISR 读取数据 → 唤醒应用线程
    4. 上下文切换 → 应用处理数据

  ┌─────────┐        中断        ┌──────┐
  │   CPU   │ ←─────────────── │ NIC  │
  │ 运行App │  保存上下文        └──────┘
  └────┬────┘  跳转ISR
       ▼       处理完成
  ┌─────────┐  恢复上下文
  │   ISR   │  返回App
  └─────────┘

  优点:
    ✓ CPU 利用率高 (无包时 CPU 空闲)
    ✓ 功耗低
    ✓ 适合低负载场景

  缺点:
    ❌ 中断延迟: ~1-5μs (上下文切换)
    ❌ Cache thrashing (ISR + App 交替执行)
    ❌ 高负载时中断风暴 (Interrupt Storm)
    ❌ 不确定性延迟

  ═══ 轮询模式 (Polling / Busy-Wait) ═══

  工作流:
    1. 应用线程持续检查网卡寄存器/Ring
    2. 发现新包 → 立即处理
    3. 无包 → 继续轮询 (不睡眠)

  while (true) {
      packets = check_rx_ring();  // 轮询
      if (packets > 0)
          process(packets);       // 处理
      // 不睡眠, 继续循环
  }

  优点:
    ✓ 极低延迟: ~0.5-2μs (无中断开销)
    ✓ 确定性高 (无调度延迟)
    ✓ 高吞吐 (无上下文切换)

  缺点:
    ❌ CPU 占用通常很高 (低流量时也可能接近满核)
    ❌ 功耗高
    ❌ 需要专用 CPU 核心

  ═══ 混合模式 (Adaptive/Hybrid) ═══

  低负载 → 中断模式 (省电)
  高负载 → 轮询模式 (低延迟)

  Linux NAPI (New API):
    - 正常: 中断模式
    - 收到包: 禁用中断 → 切换到轮询
    - 处理完一批包: 重新启用中断

  Intel Dynamic ITR (Interrupt Throttle Rate):
    - 自适应调整中断频率
    - 低负载: 立即中断
    - 高负载: 合并多个包一起中断

  ═══ DPDK 轮询策略 ═══

  DPDK 默认使用纯轮询:
    - 应用独占 CPU 核心 (绑核)
    - 持续调用 rte_eth_rx_burst()
    - 零中断 → 零延迟抖动

  优化轮询:
    1. 使用 PAUSE 指令降低功耗
       while (no_packets)
           _mm_pause();  // 降低空转功耗

    2. 多队列轮询
       for (q in queues)
           rte_eth_rx_burst(port, q, ...);

    3. 批量处理
       burst = rte_eth_rx_burst(port, q, pkts, 32);
       // 一次处理 32 个包 → 分摊开销

  ═══ 场景选择 ═══

  ┌─────────────────┬──────────┬──────────────┐
  │ 场景             │ 模式     │ 原因         │
  ├─────────────────┼──────────┼──────────────┤
  │ HFT 交易         │ 轮询     │ 最低延迟     │
  │ Web 服务器       │ 中断/混合│ CPU 利用率   │
  │ NFV 数据平面     │ 轮询     │ 高吞吐       │
  │ IoT 设备         │ 中断     │ 省电         │
  │ 游戏服务器       │ 混合     │ 平衡         │
  └─────────────────┴──────────┴──────────────┘
)";
}
} // namespace ch5


// =============================================================================
// ██  二、DPDK 核心篇
// =============================================================================

// =====================================================================
// 第 6 章  DPDK 架构与原理
// =====================================================================
namespace ch6 {
void demo() {
    print_section("第6章: DPDK 架构与原理");

    std::cout << R"(
  ═══ DPDK 核心理念 ═══

  1. 用户态驱动 (PMD - Poll Mode Driver)
     - 完全绕过内核
     - 应用直接访问网卡寄存器与 DMA 区域

  2. Run-to-Completion 模型
     - 单核心处理完整流程 (RX → 处理 → TX)
     - 无锁无阻塞

  3. 零拷贝
     - Mbuf 直接指向网卡 DMA buffer
     - 无内核态/用户态拷贝

  ═══ DPDK 架构图 ═══

  ┌────────────────────────────────────────────────────────┐
  │  用户应用                                              │
  │  ┌──────────┐  ┌──────────┐  ┌──────────┐            │
  │  │ Worker 0 │  │ Worker 1 │  │ Worker N │            │
  │  │ (Core 0) │  │ (Core 1) │  │ (Core N) │            │
  │  └────┬─────┘  └────┬─────┘  └────┬─────┘            │
  └───────┼─────────────┼─────────────┼───────────────────┘
          │             │             │
  ┌───────▼─────────────▼─────────────▼───────────────────┐
  │  DPDK 库                                               │
  │  ┌──────────────┐  ┌───────────────┐  ┌────────────┐ │
  │  │ EAL          │  │ Mempool       │  │ Mbuf       │ │
  │  │ (初始化)     │  │ (内存池)      │  │ (包缓冲)   │ │
  │  └──────────────┘  └───────────────┘  └────────────┘ │
  │  ┌──────────────┐  ┌───────────────┐  ┌────────────┐ │
  │  │ Ring         │  │ Hash          │  │ ACL        │ │
  │  │ (队列)       │  │ (哈希表)      │  │ (规则)     │ │
  │  └──────────────┘  └───────────────┘  └────────────┘ │
  │  ┌──────────────────────────────────────────────────┐ │
  │  │ PMD (Poll Mode Driver) — 用户态网卡驱动          │ │
  │  │ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────────┐ │ │
  │  │ │ ixgbe  │ │ i40e   │ │ mlx5   │ │ virtio-net │ │ │
  │  │ │(Intel) │ │(Intel) │ │(Mellanox│ │ (虚拟)     │ │ │
  │  │ └────────┘ └────────┘ └────────┘ └────────────┘ │ │
  │  └──────────────────────────────────────────────────┘ │
  └─────────────────────┬──────────────────────────────────┘
                        │ UIO / VFIO (用户态 I/O)
  ┌─────────────────────▼──────────────────────────────────┐
  │  物理网卡 (Intel 82599, X710, Mellanox CX-5/6...)     │
  └────────────────────────────────────────────────────────┘

  ═══ EAL (Environment Abstraction Layer) ═══

  核心初始化层, 负责:
    - CPU 亲和性设置 (绑核)
    - 大页内存分配
    - PCI 设备扫描与绑定
    - 日志系统
    - 多进程支持

  初始化代码:
    int argc = 4;
    char *argv[] = {
        "app",
        "-c", "0xff",      // CPU mask: 8 cores
        "-n", "4",         // Memory channels
        "--huge-dir", "/mnt/huge",
    };
    rte_eal_init(argc, argv);

  ═══ Mempool (内存池) ═══

  预分配的对象池, 避免 malloc:
    - 固定大小对象
    - NUMA 感知
    - 无锁访问 (per-core cache)

  创建:
    struct rte_mempool *pool = rte_pktmbuf_pool_create(
        "mbuf_pool",      // 名称
        8192,             // 元素个数
        256,              // per-core cache
        0,                // private size
        RTE_MBUF_DEFAULT_BUF_SIZE,
        rte_socket_id()   // NUMA node
    );

  ═══ Mbuf (消息缓冲区) ═══

  DPDK 的包封装, 包含:
    - 元数据: 长度, 端口号, 校验和
    - 数据指针: 指向实际包数据
    - 引用计数: 支持零拷贝克隆

  struct rte_mbuf {
      void *buf_addr;          // 数据 buffer 地址
      uint16_t data_off;       // 数据起始偏移
      uint16_t buf_len;        // buffer 总长度
      uint16_t pkt_len;        // 包总长度
      uint16_t data_len;       // 本 segment 数据长度
      struct rte_mbuf *next;   // 分段包链表
      uint16_t vlan_tci;       // VLAN tag
      uint32_t rss_hash;       // RSS hash
      // ... 更多字段
  };

  ═══ PMD 驱动架构 ═══

  支持的网卡:
    Intel: ixgbe (82599), i40e (X710), ice (E810)
    Mellanox: mlx4, mlx5 (ConnectX 系列)
    AMD/Xilinx: Solarflare
    Broadcom: bnxt
    Virtual: virtio, vmxnet3, netvsc

  驱动操作:
    rte_eth_dev_configure()  // 配置端口
    rte_eth_rx_queue_setup() // 设置 RX 队列
    rte_eth_tx_queue_setup() // 设置 TX 队列
    rte_eth_dev_start()      // 启动端口
    rte_eth_rx_burst()       // 接收包 (批量)
    rte_eth_tx_burst()       // 发送包 (批量)
)";
}
} // namespace ch6


// =====================================================================
// 第 7 章  DPDK 环境配置与初始化
// =====================================================================
namespace ch7 {
void demo() {
    print_section("第7章: DPDK 环境配置与初始化");

    std::cout << R"(
  ═══ 系统环境准备 ═══

  1. 安装 DPDK:
     # Ubuntu/Debian
     sudo apt install dpdk dpdk-dev

     # 从源码编译
     wget https://fast.dpdk.org/rel/dpdk-23.11.tar.xz
     tar xf dpdk-23.11.tar.xz && cd dpdk-23.11
     meson build && cd build && ninja
     sudo ninja install

  2. 设置大页内存:
     # 2MB 大页
     echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

     # 1GB 大页 (更好)
     echo 8 > /sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages

     # 挂载 hugetlbfs
     mkdir -p /mnt/huge
     mount -t hugetlbfs nodev /mnt/huge

  3. 绑定网卡到 DPDK:
     # 查看网卡
     dpdk-devbind.py --status

     # 加载 UIO/VFIO 驱动
     modprobe uio
     modprobe uio_pci_generic
     # 或者 VFIO (推荐, 支持 IOMMU)
     modprobe vfio-pci

     # 绑定网卡 (例: 0000:03:00.0)
     dpdk-devbind.py --bind=uio_pci_generic 0000:03:00.0
     # 或
     dpdk-devbind.py --bind=vfio-pci 0000:03:00.0

  ═══ 最小 DPDK 应用 ═══

  #include <rte_eal.h>
  #include <rte_ethdev.h>
  #include <rte_mbuf.h>

  #define RX_RING_SIZE 1024
  #define TX_RING_SIZE 1024
  #define NUM_MBUFS 8191
  #define MBUF_CACHE_SIZE 250
  #define BURST_SIZE 32

  int main(int argc, char *argv[]) {
      // ── 1. EAL 初始化 ──
      int ret = rte_eal_init(argc, argv);
      if (ret < 0)
          rte_exit(EXIT_FAILURE, "EAL init failed\n");
      
      argc -= ret;
      argv += ret;

      // ── 2. 创建 Mempool ──
      struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create(
          "MBUF_POOL",
          NUM_MBUFS,
          MBUF_CACHE_SIZE,
          0,
          RTE_MBUF_DEFAULT_BUF_SIZE,
          rte_socket_id()
      );
      if (!mbuf_pool)
          rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

      // ── 3. 配置端口 ──
      uint16_t nb_ports = rte_eth_dev_count_avail();
      if (nb_ports == 0)
          rte_exit(EXIT_FAILURE, "No Ethernet ports\n");

      uint16_t portid = 0;
      struct rte_eth_conf port_conf = {};
      port_conf.rxmode.max_rx_pkt_len = RTE_ETHER_MAX_LEN;

      ret = rte_eth_dev_configure(portid, 1, 1, &port_conf);
      if (ret < 0)
          rte_exit(EXIT_FAILURE, "Cannot configure port\n");

      // ── 4. 设置 RX 队列 ──
      ret = rte_eth_rx_queue_setup(portid, 0, RX_RING_SIZE,
                  rte_eth_dev_socket_id(portid),
                  NULL, mbuf_pool);
      if (ret < 0)
          rte_exit(EXIT_FAILURE, "RX queue setup failed\n");

      // ── 5. 设置 TX 队列 ──
      ret = rte_eth_tx_queue_setup(portid, 0, TX_RING_SIZE,
                  rte_eth_dev_socket_id(portid), NULL);
      if (ret < 0)
          rte_exit(EXIT_FAILURE, "TX queue setup failed\n");

      // ── 6. 启动端口 ──
      ret = rte_eth_dev_start(portid);
      if (ret < 0)
          rte_exit(EXIT_FAILURE, "Cannot start port\n");

      // ── 7. 主循环: 收发包 ──
      struct rte_mbuf *bufs[BURST_SIZE];
      while (1) {
          uint16_t nb_rx = rte_eth_rx_burst(portid, 0, bufs, BURST_SIZE);
          if (nb_rx == 0)
              continue;
          
          // 处理包 (这里简单回环)
          for (int i = 0; i < nb_rx; ++i) {
              // 交换 src/dst MAC
              struct rte_ether_hdr *eth = rte_pktmbuf_mtod(bufs[i], 
                                              struct rte_ether_hdr *);
              struct rte_ether_addr tmp = eth->d_addr;
              eth->d_addr = eth->s_addr;
              eth->s_addr = tmp;
          }

          uint16_t nb_tx = rte_eth_tx_burst(portid, 0, bufs, nb_rx);
          // 释放未发送的包
          for (int i = nb_tx; i < nb_rx; ++i)
              rte_pktmbuf_free(bufs[i]);
      }

      // ── 8. 清理 (实际不会到达) ──
      rte_eth_dev_stop(portid);
      rte_eth_dev_close(portid);
      rte_eal_cleanup();
      return 0;
  }

  ═══ 编译与运行 ═══

  # 编译
  gcc main.c -o dpdk_app \
      $(pkg-config --cflags --libs libdpdk) \
      -march=native

  # 运行 (需要 root 或 CAP_NET_ADMIN)
  sudo ./dpdk_app -c 0x3 -n 4 -- 

  EAL 参数:
    -c COREMASK: CPU 核心掩码 (0x3 = core 0,1)
    -l CORELIST: 核心列表 (0-3,8)
    -n CHANNELS: 内存通道数 (通常 2 或 4)
    --proc-type: primary/secondary (多进程)
    --file-prefix: 共享内存前缀 (多实例)
    --huge-dir: 大页目录

  ═══ 常见问题 ═══

  Q: EAL: Cannot open /dev/vfio/...
  A: 检查 IOMMU 是否启用
     # 内核参数
     intel_iommu=on iommu=pt
     # 或使用 uio_pci_generic

  Q: Cannot allocate memory
  A: 增加大页数量
     echo 2048 > /sys/kernel/mm/hugepages/.../nr_hugepages

  Q: No Ethernet ports
  A: 确认网卡已绑定到 DPDK
     dpdk-devbind.py --status
)";
}
} // namespace ch7


// =====================================================================
// 第 8 章  DPDK 内存管理 (Mempool)
// =====================================================================
namespace ch8 {
void demo() {
    print_section("第8章: DPDK 内存管理 (Mempool)");

    std::cout << R"(
  ═══ Mempool 设计理念 ═══

  问题: malloc/free 在高性能场景下的问题
    ❌ 系统调用开销
    ❌ 锁竞争 (多线程)
    ❌ 内存碎片
    ❌ 不确定性延迟

  DPDK Mempool:
    ✓ 预分配固定大小对象
    ✓ 无锁访问 (per-core cache)
    ✓ NUMA 感知
    ✓ 零碎片

  ═══ Mempool 架构 ═══

  ┌───────────────────────────────────────────┐
  │ Mempool (全局)                            │
  │  ┌─────────────────────────────────────┐  │
  │  │ Common Pool (共享对象)              │  │
  │  │ [obj][obj][obj][obj][obj]...        │  │
  │  └─────────────────────────────────────┘  │
  │         ↑                      ↑           │
  │         │ alloc (慢)          │ free       │
  │         │                      │           │
  │  ┌──────▼──────┐      ┌────────▼─────┐    │
  │  │Per-Core     │      │Per-Core      │    │
  │  │Cache (Core0)│      │Cache (Core1) │    │
  │  │[obj][obj]   │      │[obj][obj]    │    │
  │  └─────────────┘      └──────────────┘    │
  │   快速 alloc/free (无锁)                  │
  └───────────────────────────────────────────┘

  访问流程:
    1. alloc → 先从 per-core cache 取
    2. Cache 空 → 从 common pool 批量取 (带锁)
    3. free → 放回 per-core cache
    4. Cache 满 → 批量还给 common pool

  ═══ 创建 Mempool ═══

  // 通用 Mempool
  struct rte_mempool *mp = rte_mempool_create(
      "my_pool",             // 名称
      8192,                  // 元素数量
      128,                   // 每个元素大小 (bytes)
      256,                   // per-core cache 大小
      0,                     // private data size
      NULL, NULL,            // pool init 回调
      NULL, NULL,            // obj init 回调
      rte_socket_id(),       // NUMA socket
      0                      // flags
  );

  // 包专用 Mempool (推荐)
  struct rte_mempool *pkt_pool = rte_pktmbuf_pool_create(
      "mbuf_pool",
      8192,                  // 数量
      256,                   // cache
      0,
      RTE_MBUF_DEFAULT_BUF_SIZE,  // 每个 mbuf 大小
      rte_socket_id()
  );

  ═══ 使用 Mempool ═══

  // 分配对象
  void *obj = NULL;
  int ret = rte_mempool_get(mp, &obj);
  if (ret == 0) {
      // 使用 obj
  }

  // 批量分配 (更高效)
  void *objs[32];
  ret = rte_mempool_get_bulk(mp, objs, 32);

  // 释放对象
  rte_mempool_put(mp, obj);

  // 批量释放
  rte_mempool_put_bulk(mp, objs, 32);

  ═══ 高级特性 ═══

  1. 外部内存支持
     rte_mempool_create_empty() + rte_mempool_populate_*()

  2. 自定义对象初始化
     void my_obj_init(struct rte_mempool *mp, void *arg,
                      void *obj, unsigned i) {
         // 初始化每个对象
         my_obj_t *o = (my_obj_t*)obj;
         o->magic = 0xdeadbeef;
     }
     rte_mempool_create(..., my_obj_init, NULL, ...);

  3. 统计信息
     struct rte_mempool_ops *ops = rte_mempool_get_ops(mp->ops_index);
     // 可以获取 alloc/free 计数

  ═══ 性能优化技巧 ═══

  1. Cache 大小选择
     太小: 频繁访问 common pool (锁竞争)
     太大: 内存浪费 + cache miss
     推荐: 256-512 (10-20% 的总大小)

  2. NUMA 感知
     每个 NUMA 节点独立 mempool
     int socket_id = rte_socket_id();
     mp = rte_pktmbuf_pool_create(..., socket_id);

  3. 批量操作
     优先使用 _bulk 接口 (16-32 个对象)
     分摊 cache miss + 锁开销

  4. 预热 cache
     启动时预分配再释放
     for (int i = 0; i < cache_size; ++i) {
         void *obj;
         rte_mempool_get(mp, &obj);
         rte_mempool_put(mp, obj);
     }
)";
}
} // namespace ch8


// =====================================================================
// 第 9 章  DPDK Mbuf 详解
// =====================================================================
namespace ch9 {
void demo() {
    print_section("第9章: DPDK Mbuf 详解");

    std::cout << R"(
  ═══ Mbuf 结构 ═══

  struct rte_mbuf {
      // ── Cacheline 0 (第1个缓存行) ──
      void *buf_addr;         // Buffer 虚拟地址
      rte_iova_t buf_iova;    // Buffer 物理地址 (DMA)
      
      uint16_t data_off;      // 数据起始偏移
      uint16_t refcnt;        // 引用计数 (克隆用)
      uint16_t nb_segs;       // 分段数量
      uint16_t port;          // 收发端口号
      
      uint64_t ol_flags;      // Offload 标志位
      
      // ── Cacheline 1 (第2个缓存行) ──
      void *pool;             // 所属 Mempool
      struct rte_mbuf *next;  // 下一个 segment
      
      uint16_t data_len;      // 本 segment 长度
      uint16_t pkt_len;       // 完整包长度
      
      uint16_t vlan_tci;      // VLAN tag
      uint32_t rss_hash;      // RSS hash value
      
      // ── Offload 元数据 ──
      union {
          uint32_t packet_type;
          struct {
              uint32_t l2_type:4;
              uint32_t l3_type:4;
              uint32_t l4_type:4;
              uint32_t tun_type:4;
              // ...
          };
      };
      
      uint32_t tx_offload;    // TX offload 标志
      // ...更多字段
  };

  ═══ Mbuf 内存布局 ═══

  ┌──────────────────────────────────────┐
  │ rte_mbuf 结构 (128 bytes)            │ ← Metadata
  ├──────────────────────────────────────┤
  │ Headroom (RTE_PKTMBUF_HEADROOM)      │
  │ 默认 128 bytes                       │ ← 用于添加封装头
  ├──────────────────────────────────────┤
  │ Packet Data (实际数据)               │ ← data_off 指向这里
  │ ...                                  │
  │ ...                                  │
  ├──────────────────────────────────────┤
  │ Tailroom (剩余空间)                  │ ← 用于添加尾部数据
  └──────────────────────────────────────┘

  buf_addr → 指向 Headroom 起始
  data_off → 128 (跳过 Headroom)
  data_len → 实际数据长度

  ═══ Mbuf 基本操作 ═══

  1. 分配 Mbuf
     struct rte_mbuf *m = rte_pktmbuf_alloc(mbuf_pool);
     if (m == NULL) {
         // 分配失败 (pool 耗尽)
     }

  2. 访问数据
     char *data = rte_pktmbuf_mtod(m, char*);
     // 或指定类型
     struct rte_ether_hdr *eth = rte_pktmbuf_mtod(m, 
                                     struct rte_ether_hdr*);

  3. 添加/移除头部
     // 添加 14 字节以太网头
     char *eth_hdr = (char*)rte_pktmbuf_prepend(m, 14);
     // 移除头部
     char *ip_hdr = (char*)rte_pktmbuf_adj(m, 14);

  4. 添加尾部数据
     char *tail = (char*)rte_pktmbuf_append(m, 4); // CRC

  5. 释放 Mbuf
     rte_pktmbuf_free(m);

  ═══ 多段 Mbuf (Chained) ═══

  大包 > buffer size → 分段存储

  ┌─────────┐    next    ┌─────────┐    next    ┌─────────┐
  │ Mbuf 0  │ ────────→  │ Mbuf 1  │ ────────→  │ Mbuf 2  │
  │ (head)  │            │         │            │ (tail)  │
  ├─────────┤            ├─────────┤            ├─────────┤
  │ data    │            │ data    │            │ data    │
  │ 1500 B  │            │ 1500 B  │            │ 500 B   │
  └─────────┘            └─────────┘            └─────────┘
  pkt_len = 3500, nb_segs = 3

  遍历:
    struct rte_mbuf *seg = m;
    while (seg) {
        process_segment(seg);
        seg = seg->next;
    }

  ═══ Mbuf 克隆与引用 ═══

  零拷贝克隆 (增加 refcnt):
    struct rte_mbuf *clone = rte_pktmbuf_clone(m, clone_pool);
    // clone 与 m 共享同一个 data buffer
    // refcnt++

  使用场景: 组播 (一包发多个方向)

  释放:
    rte_pktmbuf_free(clone);  // refcnt--
    rte_pktmbuf_free(m);      // refcnt-- → 0 时才真正释放

  ═══ Mbuf Offload 特性 ═══

  1. 校验和卸载
     // TX 校验和计算卸载到网卡
     m->l2_len = sizeof(struct rte_ether_hdr);
     m->l3_len = sizeof(struct rte_ipv4_hdr);
     m->ol_flags |= PKT_TX_IP_CKSUM | PKT_TX_TCP_CKSUM;

  2. TSO (TCP Segmentation Offload)
     m->ol_flags |= PKT_TX_TCP_SEG;
     m->tso_segsz = 1460;  // MSS

  3. VLAN 插入
     m->ol_flags |= PKT_TX_VLAN_PKT;
     m->vlan_tci = 100;  // VLAN ID

  4. RSS Hash
     if (m->ol_flags & PKT_RX_RSS_HASH)
         uint32_t hash = m->rss_hash;

  ═══ 性能优化技巧 ═══

  1. 批量操作
     struct rte_mbuf *mbufs[32];
     int n = rte_pktmbuf_alloc_bulk(pool, mbufs, 32);

  2. 避免动态调整
     预留足够 headroom/tailroom, 避免 realloc

  3. Prefetch
     rte_prefetch0(rte_pktmbuf_mtod(m, void*));

  4. 重用 Mbuf
     rte_pktmbuf_reset(m);  // 重置元数据
)";
}
} // namespace ch9


// =====================================================================
// 第 10 章  DPDK PMD 驱动架构
// =====================================================================
namespace ch10 {
void demo() {
    print_section("第10章: DPDK PMD 驱动架构");

    std::cout << R"(
  ═══ PMD (Poll Mode Driver) 架构 ═══

  传统驱动 (Linux):
    网卡 → 中断 → 内核驱动 → 协议栈 → 用户空间
    ❌ 多次拷贝
    ❌ 上下文切换
    ❌ 不确定性延迟

  DPDK PMD:
    网卡 ← 直接映射 → 用户空间应用
    ✓ 零拷贝
    ✓ 无中断
    ✓ 无系统调用
    ✓ 轮询

  ═══ PMD 驱动列表 ═══

  Intel:
    ixgbe:  10GbE (82599ES, X520, X540)
    i40e:   10/25/40GbE (XL710, X710, XXV710)
    ice:    25/50/100GbE (E810)
    iavf:   虚拟功能 (SR-IOV VF)
    igc:    2.5GbE (I225/I226)
  
  Mellanox/NVIDIA:
    mlx4:   ConnectX-3
    mlx5:   ConnectX-4/5/6/7 (支持 RDMA)
  
  AMD/Xilinx:
    sfc:    Solarflare (SFN8xxx, X2xxx)
  
  Broadcom:
    bnxt:   NetXtreme-C/E
  
  Virtual:
    virtio: KVM/QEMU
    vmxnet3: VMware
    netvsc: Hyper-V
    tap:    TUN/TAP
  
  ═══ 端口操作 API ═══

  1. 探测设备
     uint16_t nb_ports = rte_eth_dev_count_avail();

  2. 配置端口
     struct rte_eth_conf port_conf = {};
     port_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
     port_conf.rxmode.max_rx_pkt_len = 9000;  // Jumbo frame
     port_conf.txmode.mq_mode = ETH_MQ_TX_NONE;
     
     rte_eth_dev_configure(port_id, nb_rx_q, nb_tx_q, &port_conf);

  3. 队列设置
     // RX queue
     struct rte_eth_rxconf rx_conf = {};
     rx_conf.rx_thresh.pthresh = 8;
     rx_conf.rx_thresh.hthresh = 8;
     rx_conf.rx_thresh.wthresh = 0;
     rx_conf.rx_free_thresh = 32;
     
     rte_eth_rx_queue_setup(port_id, queue_id, 
                            nb_rx_desc, socket_id,
                            &rx_conf, mbuf_pool);
     
     // TX queue
     struct rte_eth_txconf tx_conf = {};
     tx_conf.tx_thresh.pthresh = 32;
     tx_conf.tx_thresh.hthresh = 0;
     tx_conf.tx_thresh.wthresh = 0;
     tx_conf.tx_free_thresh = 32;
     
     rte_eth_tx_queue_setup(port_id, queue_id,
                            nb_tx_desc, socket_id,
                            &tx_conf);

  4. 启动/停止
     rte_eth_dev_start(port_id);
     rte_eth_promiscuous_enable(port_id);  // 混杂模式
     // ...
     rte_eth_dev_stop(port_id);
     rte_eth_dev_close(port_id);

  ═══ 收发包 API ═══

  // 接收 (批量)
  struct rte_mbuf *rx_pkts[BURST_SIZE];
  uint16_t nb_rx = rte_eth_rx_burst(port_id, queue_id,
                                     rx_pkts, BURST_SIZE);
  
  for (int i = 0; i < nb_rx; ++i) {
      process_packet(rx_pkts[i]);
  }

  // 发送 (批量)
  struct rte_mbuf *tx_pkts[BURST_SIZE];
  // ... 准备 tx_pkts
  uint16_t nb_tx = rte_eth_tx_burst(port_id, queue_id,
                                     tx_pkts, BURST_SIZE);
  
  // 释放未发送的包
  for (int i = nb_tx; i < BURST_SIZE; ++i) {
      rte_pktmbuf_free(tx_pkts[i]);
  }

  ═══ 设备信息查询 ═══

  struct rte_eth_dev_info dev_info;
  rte_eth_dev_info_get(port_id, &dev_info);
  
  printf("Driver: %s\n", dev_info.driver_name);
  printf("Max RX queues: %u\n", dev_info.max_rx_queues);
  printf("Max TX queues: %u\n", dev_info.max_tx_queues);
  printf("Max MTU: %u\n", dev_info.max_mtu);

  // 链路状态
  struct rte_eth_link link;
  rte_eth_link_get_nowait(port_id, &link);
  
  if (link.link_status == ETH_LINK_UP) {
      printf("Link up, %u Mbps, %s duplex\n",
             link.link_speed,
             link.link_duplex == ETH_LINK_FULL_DUPLEX ? 
                 "full" : "half");
  }

  // 统计信息
  struct rte_eth_stats stats;
  rte_eth_stats_get(port_id, &stats);
  
  printf("RX packets: %lu, bytes: %lu\n",
         stats.ipackets, stats.ibytes);
  printf("TX packets: %lu, bytes: %lu\n",
         stats.opackets, stats.obytes);
  printf("Errors: %lu\n", stats.ierrors + stats.oerrors);

  ═══ 高级特性 ═══

  1. SR-IOV (Single Root I/O Virtualization)
     一个物理网卡虚拟出多个 VF (Virtual Function)
     每个 VM 独立 VF → 接近原生性能
     
     # 启用 SR-IOV
     echo 4 > /sys/class/net/eth0/device/sriov_numvfs

  2. Flow Director / RTE Flow
     硬件流分类 → 特定流量到特定队列
     (详见第14章)

  3. 时间戳
     部分网卡支持硬件时间戳
     m->ol_flags & PKT_RX_TIMESTAMP
     uint64_t ts = m->timestamp;
)";
}
} // namespace ch10


// 继续第11-35章...
// 为了简洁，这里快速完成框架

// =====================================================================
// 第 11 章  DPDK 收发包基础
// =====================================================================
namespace ch11 {
void demo() {
    print_section("第11章: DPDK 收发包基础");
    std::cout << R"(
  ═══ 收包流程 ═══
  1. 网卡 DMA 写入 RX Ring
  2. rte_eth_rx_burst() 轮询
  3. 批量返回 mbuf 数组
  4. 应用处理
  5. rte_pktmbuf_free() 释放

  性能优化: Burst Size = 32 (经验值)
  ═══ 发包流程 ═══
  1. 应用准备 mbuf
  2. rte_eth_tx_burst() 批量发送
  3. 网卡 DMA 读取 TX Ring
  4. 释放已发送的 mbuf
)";
}
} // namespace ch11

// ... 省略 ch12-ch34 的详细实现 (保持结构一致) ...

// =====================================================================
// 第 12 章  DPDK 多核并行处理
// =====================================================================
namespace ch12 {
void demo() {
    print_section("第12章: DPDK 多核并行处理");

    std::cout << R"(
  ═══ 两种并行模型 ═══

  1. Run-to-Completion (RTC) 模型
     每个核心独立处理完整流程: RX → 处理 → TX
     ┌────────┐  ┌────────┐  ┌────────┐
     │ Core 0 │  │ Core 1 │  │ Core 2 │
     │  RX Q0 │  │  RX Q1 │  │  RX Q2 │
     │    ↓   │  │    ↓   │  │    ↓   │
     │ Process│  │ Process│  │ Process│
     │    ↓   │  │    ↓   │  │    ↓   │
     │  TX Q0 │  │  TX Q1 │  │  TX Q2 │
     └────────┘  └────────┘  └────────┘
     
     优点: 无锁, cache 友好, 延迟低
     缺点: 负载不均衡时某些核心闲置

  2. Pipeline 模型
     流水线式处理, 核心间传递包
     Core 0 (RX) → Core 1 (Process) → Core 2 (TX)
        │              │                  │
     RX Ring      rte_ring()         TX Ring

     优点: 更好的负载均衡, 易扩展
     缺点: 跨核通信延迟, 需要无锁队列

  ═══ Run-to-Completion 示例 ═══

  // 每个核心的工作函数
  static int lcore_main_rtc(void *arg) {
      uint16_t port = 0;
      uint16_t queue = rte_lcore_id();  // 队列 = 核心ID
      
      printf("Core %u processing queue %u\n",
             rte_lcore_id(), queue);
      
      struct rte_mbuf *bufs[BURST_SIZE];
      while (!force_quit) {
          // 1. 收包
          uint16_t nb_rx = rte_eth_rx_burst(port, queue,
                                            bufs, BURST_SIZE);
          if (unlikely(nb_rx == 0))
              continue;
          
          // 2. 处理
          for (int i = 0; i < nb_rx; ++i) {
              process_packet(bufs[i]);
          }
          
          // 3. 发包
          uint16_t nb_tx = rte_eth_tx_burst(port, queue,
                                            bufs, nb_rx);
          
          // 4. 释放未发送的
          for (int i = nb_tx; i < nb_rx; ++i)
              rte_pktmbuf_free(bufs[i]);
      }
      return 0;
  }

  // 主函数: 启动多个工作核心
  int main(int argc, char *argv[]) {
      rte_eal_init(argc, argv);
      
      // 配置网卡 (多队列)
      struct rte_eth_conf port_conf = {};
      port_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
      port_conf.txmode.mq_mode = ETH_MQ_TX_NONE;
      
      uint16_t nb_queues = rte_lcore_count() - 1;  // 主核心除外
      rte_eth_dev_configure(0, nb_queues, nb_queues, &port_conf);
      
      // 设置每个队列
      for (int q = 0; q < nb_queues; ++q) {
          rte_eth_rx_queue_setup(0, q, 1024, 
                                 rte_socket_id(), NULL, mbuf_pool);
          rte_eth_tx_queue_setup(0, q, 1024,
                                 rte_socket_id(), NULL);
      }
      
      rte_eth_dev_start(0);
      
      // 启动所有从核心
      rte_eal_mp_remote_launch(lcore_main_rtc, NULL, SKIP_MAIN);
      
      // 主核心等待
      rte_eal_mp_wait_lcore();
      return 0;
  }

  ═══ Pipeline 模型示例 ═══

  struct rte_ring *rx_to_worker, *worker_to_tx;

  // RX 核心
  static int lcore_rx(void *arg) {
      struct rte_mbuf *bufs[BURST_SIZE];
      while (!force_quit) {
          uint16_t nb_rx = rte_eth_rx_burst(0, 0, bufs, BURST_SIZE);
          
          // 送入 ring 传给 worker
          uint16_t sent = rte_ring_enqueue_burst(rx_to_worker,
                              (void**)bufs, nb_rx, NULL);
          
          // 释放放不进去的
          for (int i = sent; i < nb_rx; ++i)
              rte_pktmbuf_free(bufs[i]);
      }
      return 0;
  }

  // Worker 核心
  static int lcore_worker(void *arg) {
      struct rte_mbuf *bufs[BURST_SIZE];
      while (!force_quit) {
          uint16_t nb = rte_ring_dequeue_burst(rx_to_worker,
                            (void**)bufs, BURST_SIZE, NULL);
          if (nb == 0)
              continue;
          
          // 处理
          for (int i = 0; i < nb; ++i) {
              process_packet(bufs[i]);
          }
          
          // 传给 TX
          rte_ring_enqueue_burst(worker_to_tx,
                                 (void**)bufs, nb, NULL);
      }
      return 0;
  }

  // TX 核心
  static int lcore_tx(void *arg) {
      struct rte_mbuf *bufs[BURST_SIZE];
      while (!force_quit) {
          uint16_t nb = rte_ring_dequeue_burst(worker_to_tx,
                            (void**)bufs, BURST_SIZE, NULL);
          if (nb == 0)
              continue;
          
          uint16_t sent = rte_eth_tx_burst(0, 0, bufs, nb);
          for (int i = sent; i < nb; ++i)
              rte_pktmbuf_free(bufs[i]);
      }
      return 0;
  }

  ═══ rte_ring 无锁队列 ═══

  创建:
    rx_to_worker = rte_ring_create("rx_to_worker",
                                    4096,  // size (2的幂)
                                    rte_socket_id(),
                                    RING_F_SP_ENQ | RING_F_SC_DEQ);
    // SP = Single Producer, SC = Single Consumer

  性能: >100M ops/s (单生产者单消费者)

  ═══ CPU 绑核 ═══

  EAL 参数:
    -l 1-7        # 使用核心 1-7
    --lcores='(0-3)@(1,2,3,4),(4-7)@(5,6,7,8)'
                  # 逻辑核心到物理核心映射

  手动绑核:
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(lcore_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

  ═══ 性能对比 ═══

  RTC 模型: 
    吞吐: 10-15 Mpps/core
    延迟: 1-2 μs

  Pipeline 模型:
    吞吐: 8-12 Mpps/core (跨核开销)
    延迟: 2-4 μs (多次 ring 传递)
)";
}
} // namespace ch12


// =====================================================================
// 第 13 章  DPDK RSS (接收侧扩展)
// =====================================================================
namespace ch13 {
void demo() {
    print_section("第13章: DPDK RSS");

    std::cout << R"(
  ═══ RSS (Receive Side Scaling) 原理 ═══

  问题: 单队列成为瓶颈 (1 core处理全部流量)

  解决: 根据包头 hash, 分配到不同 RX 队列
        每个队列由独立 CPU 核心处理

  ┌────────────────────────────────────┐
  │  NIC                               │
  │  ┌──────┐                          │
  │  │Packet│                          │
  │  └───┬──┘                          │
  │      ▼                             │
  │  ┌────────────────┐                │
  │  │ RSS Hash       │                │
  │  │ (5-tuple)      │                │
  │  │ Src IP         │                │
  │  │ Dst IP         │                │
  │  │ Src Port       │                │
  │  │ Dst Port       │                │
  │  │ Protocol       │                │
  │  └───┬────────────┘                │
  │      │ hash % N                   │
  │      ├─────┬─────┬─────┐          │
  │      ▼     ▼     ▼     ▼          │
  │   Queue0 Q1   Q2   Q3             │
  └──────┼─────┼─────┼─────┼──────────┘
         │     │     │     │
     Core 0  Core 1  Core 2  Core 3

  优点:
    ✓ 负载均衡 (多核并行)
    ✓ 同一流的包进同一队列 (保序)
    ✓ Cache 局部性好

  ═══ RSS 配置 ═══

  struct rte_eth_conf port_conf = {};
  
  // 启用 RSS
  port_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
  
  // RSS 配置
  port_conf.rx_adv_conf.rss_conf.rss_key = NULL; // 默认密钥
  port_conf.rx_adv_conf.rss_conf.rss_key_len = 40;
  port_conf.rx_adv_conf.rss_conf.rss_hf = 
      ETH_RSS_IP | ETH_RSS_TCP | ETH_RSS_UDP;
  
  rte_eth_dev_configure(port_id, nb_queues, nb_queues, &port_conf);

  RSS Hash 类型:
    ETH_RSS_IPV4:     IPv4 (src/dst IP)
    ETH_RSS_TCP:      TCP (+ src/dst port)
    ETH_RSS_UDP:      UDP (+ src/dst port)
    ETH_RSS_IPV6:     IPv6
    ETH_RSS_SCTP:     SCTP
    ETH_RSS_L2_PAYLOAD: L2 负载
    
  ═══ 自定义 RSS 密钥 ═══

  uint8_t rss_key[40] = {
      0x6d, 0x5a, 0x56, 0xda, ...  // 自定义key
  };
  
  port_conf.rx_adv_conf.rss_conf.rss_key = rss_key;
  port_conf.rx_adv_conf.rss_conf.rss_key_len = 40;

  为什么自定义?
    - 避免 hash 碰撞攻击
    - 更好的负载均衡 (针对特定流量)

  ═══ RSS 重定向表 (RETA) ═══

  Hash 结果 → RETA 索引 → 队列号
  
  例: 8个队列, 128条RETA
    hash % 128 → RETA[idx] → queue_id

  自定义 RETA:
    struct rte_eth_rss_reta_entry64 reta_conf[2];
    memset(reta_conf, 0, sizeof(reta_conf));
    
    // 前64条映射到队列 0-3
    for (int i = 0; i < 64; ++i) {
        reta_conf[0].reta[i % 64] = i % 4;
    }
    reta_conf[0].mask = ~0ULL;
    
    // 后64条映射到队列 4-7
    for (int i = 0; i < 64; ++i) {
        reta_conf[1].reta[i] = 4 + (i % 4);
    }
    reta_conf[1].mask = ~0ULL;
    
    rte_eth_dev_rss_reta_update(port_id, reta_conf, 128);

  ═══ 查询 RSS Hash 值 ═══

  if (m->ol_flags & PKT_RX_RSS_HASH) {
      uint32_t hash = m->hash.rss;
      printf("RSS hash: 0x%08x\n", hash);
  }

  用途: 调试负载均衡, 自定义路由

  ═══ RSS vs Flow Director ═══

  RSS:
    ✓ 简单, 硬件自动
    ✓ 粗粒度 (基于 hash)
    ❌ 无法精确控制

  Flow Director / RTE Flow:
    ✓ 精确匹配 (5-tuple, VLAN, etc.)
    ✓ 可指定特定流到特定队列
    ❌ 规则有限 (硬件表项数)

  典型组合: RSS (通用流量) + Flow Director (关键流)

  ═══ 最佳实践 ═══

  1. 队列数 = CPU 核心数
     避免核心闲置或过载

  2. 使用对称 RSS (Symmetric RSS)
     保证 (A→B) 和 (B→A) 到同一队列
     需要自定义 key 或硬件支持

  3. 监控队列负载
     struct rte_eth_stats stats;
     rte_eth_stats_get(port_id, &stats);
     // 检查 q_ipackets[i] 是否均衡

  4. NUMA 感知
     队列的 mempool 应在同一 NUMA 节点
)";
}
} // namespace ch13


// =====================================================================
// 第 14 章  DPDK Flow Director / RTE Flow
// =====================================================================
namespace ch14 {
void demo() {
    print_section("第14章: DPDK Flow Director / RTE Flow");

    std::cout << R"(
  ═══ RTE Flow 概念 ═══

  硬件流分类与动作引擎
    - 匹配规则: 5-tuple, VLAN, MPLS, GTP...
    - 动作: 跳转队列, 丢弃, 打标记, 修改...

  ┌───────────┐
  │  Packet   │
  └─────┬─────┘
        ▼
  ┌─────────────────────────────┐
  │ RTE Flow 规则匹配 (硬件)   │
  │ 规则1: TCP 80 → Queue 5     │
  │ 规则2: UDP 53 → Drop        │
  │ 规则3: VLAN 100 → Queue 10  │
  └─────┬───────────────────────┘
        ▼
  ┌─────────────┐
  │ 执行动作    │
  └─────────────┘

  ═══ Flow 规则结构 ═══

  struct rte_flow_attr {
      uint32_t ingress:1;    // 入口流量
      uint32_t egress:1;     // 出口流量
      uint32_t transfer:1;   // 转发
      uint32_t priority;     // 优先级
  };

  struct rte_flow_item {
      enum rte_flow_item_type type;  // 匹配类型
      const void *spec;              // 期望值
      const void *mask;              // 掩码
      const void *last;              // 范围 [spec, last]
  };

  struct rte_flow_action {
      enum rte_flow_action_type type;  // 动作类型
      const void *conf;                // 配置
  };

  ═══ 示例: 匹配 TCP 端口 80 → Queue 5 ═══

  struct rte_flow_attr attr;
  struct rte_flow_item pattern[4];
  struct rte_flow_action action[2];
  struct rte_flow *flow = NULL;
  struct rte_flow_error error;

  // ── 1. 属性 ──
  memset(&attr, 0, sizeof(attr));
  attr.ingress = 1;
  attr.priority = 0;

  // ── 2. 模式: ETH / IPv4 / TCP dst=80 ──
  memset(pattern, 0, sizeof(pattern));
  
  pattern[0].type = RTE_FLOW_ITEM_TYPE_ETH;
  
  pattern[1].type = RTE_FLOW_ITEM_TYPE_IPV4;
  
  struct rte_flow_item_tcp tcp_spec, tcp_mask;
  memset(&tcp_spec, 0, sizeof(tcp_spec));
  memset(&tcp_mask, 0, sizeof(tcp_mask));
  tcp_spec.hdr.dst_port = htons(80);
  tcp_mask.hdr.dst_port = 0xffff;
  
  pattern[2].type = RTE_FLOW_ITEM_TYPE_TCP;
  pattern[2].spec = &tcp_spec;
  pattern[2].mask = &tcp_mask;
  
  pattern[3].type = RTE_FLOW_ITEM_TYPE_END;

  // ── 3. 动作: Queue 5 ──
  memset(action, 0, sizeof(action));
  
  struct rte_flow_action_queue queue_action = { .index = 5 };
  action[0].type = RTE_FLOW_ACTION_TYPE_QUEUE;
  action[0].conf = &queue_action;
  
  action[1].type = RTE_FLOW_ACTION_TYPE_END;

  // ── 4. 创建 Flow ──
  flow = rte_flow_create(port_id, &attr, pattern, action, &error);
  if (!flow) {
      printf("Flow create error: %s\n", error.message);
      return -1;
  }

  // ── 5. 销毁 Flow ──
  rte_flow_destroy(port_id, flow, &error);

  ═══ 常用匹配类型 ═══

  RTE_FLOW_ITEM_TYPE_ETH:      以太网
  RTE_FLOW_ITEM_TYPE_VLAN:     VLAN tag
  RTE_FLOW_ITEM_TYPE_IPV4:     IPv4
  RTE_FLOW_ITEM_TYPE_IPV6:     IPv6
  RTE_FLOW_ITEM_TYPE_TCP:      TCP
  RTE_FLOW_ITEM_TYPE_UDP:      UDP
  RTE_FLOW_ITEM_TYPE_SCTP:     SCTP
  RTE_FLOW_ITEM_TYPE_GTP:      GPRS Tunneling
  RTE_FLOW_ITEM_TYPE_VXLAN:    VXLAN
  RTE_FLOW_ITEM_TYPE_MPLS:     MPLS

  ═══ 常用动作类型 ═══

  RTE_FLOW_ACTION_TYPE_QUEUE:  指定队列
  RTE_FLOW_ACTION_TYPE_RSS:    指定 RSS 队列组
  RTE_FLOW_ACTION_TYPE_DROP:   丢弃
  RTE_FLOW_ACTION_TYPE_MARK:   标记 (m->hash.fdir.hi)
  RTE_FLOW_ACTION_TYPE_COUNT:  计数
  RTE_FLOW_ACTION_TYPE_JUMP:   跳转到表

  ═══ 示例: 丢弃特定 IP ═══

  struct rte_flow_item_ipv4 ipv4_spec, ipv4_mask;
  memset(&ipv4_spec, 0, sizeof(ipv4_spec));
  memset(&ipv4_mask, 0, sizeof(ipv4_mask));
  
  ipv4_spec.hdr.src_addr = htonl(0xc0a80101);  // 192.168.1.1
  ipv4_mask.hdr.src_addr = 0xffffffff;
  
  pattern[1].type = RTE_FLOW_ITEM_TYPE_IPV4;
  pattern[1].spec = &ipv4_spec;
  pattern[1].mask = &ipv4_mask;
  
  action[0].type = RTE_FLOW_ACTION_TYPE_DROP;

  ═══ 性能考虑 ═══

  1. 硬件表限制
     Intel X710: ~8K 规则
     Mellanox CX-5: ~64K 规则

  2. 优先级
     高优先级规则先匹配, 消耗更多资源

  3. 通配符匹配
     精确匹配 (mask = 0xff...) 更快
     范围匹配 (last) 较慢

  4. 动作组合
     多个动作 (QUEUE + COUNT + MARK) 可能降低性能
)";
}
} // namespace ch14


// =============================================================================
// ██  三、Solarflare/Xilinx 篇
// =============================================================================

namespace ch15 {
void demo() {
    print_section("第15章: Solarflare OpenOnload 原理");
    std::cout << R"(
  ═══ OpenOnload 架构 ═══

  传统: App → Socket API → 内核TCP/IP → 网卡
  Onload: App → Socket API → 用户态TCP/IP (Onload) → 网卡

  特点:
    ✓ 无需修改应用代码 (LD_PRELOAD)
    ✓ 绕过内核 (Kernel Bypass)
    ✓ 延迟 <1μs

  ═══ 使用方法 ═══

  onload --profile=latency ./myapp

  性能: 中位延迟 ~300-700ns
)";
}
} // namespace ch15

namespace ch16 {
void demo() {
    print_section("第16章: ef_vi 零拷贝 API");
    std::cout << R"(
  ═══ ef_vi (Etherfabric Virtual Interface) ═══

  直接访问 Solarflare 网卡 DMA 区域

  初始化:
    ef_driver_handle dh;
    ef_pd pd;
    ef_vi vi;
    
    ef_driver_open(&dh);
    ef_pd_alloc(&pd, dh, ifindex, EF_PD_DEFAULT);
    ef_vi_alloc_from_pd(&vi, dh, &pd, dh, -1, -1, -1, NULL, -1, 0);

  发送:
    ef_vi_transmit(&vi, dma_addr, len, 0);
    ef_eventq_poll(&vi, evs, EF_VI_EVENT_POLL_MAX);

  超低延迟: ~200-500ns
)";
}
} // namespace ch16

namespace ch17 {
void demo() {
    print_section("第17章: Onload 加速原理");
    std::cout << R"(
  ═══ LD_PRELOAD 劫持 ═══

  LD_PRELOAD=/usr/lib64/libonload.so ./app

  劫持的系统调用:
    socket(), bind(), connect(), send(), recv(), epoll()...

  透明加速: 应用无感知
)";
}
} // namespace ch17

namespace ch18 {
void demo() {
    print_section("第18章: PTP 硬件时间戳");
    std::cout << R"(
  ═══ PTP (Precision Time Protocol) ═══

  硬件打时间戳, 在合适网卡与时钟同步条件下可达亚微秒到百纳秒级精度

  Solarflare:
    int flag = SO_TIMESTAMPING;
    setsockopt(fd, SOL_SOCKET, flag, ...);
    
    // 接收时间戳
    struct cmsghdr *cmsg;
    cmsg = CMSG_FIRSTHDR(&msg);
    struct timespec *ts = (struct timespec*)CMSG_DATA(cmsg);

  应用: HFT, 时间同步
)";
}
} // namespace ch18

namespace ch19 {
void demo() {
    print_section("第19章: Solarflare TCPDirect");
    std::cout << R"(
  ═══ TCPDirect ═══

  用户态零拷贝 TCP 栈

  特点:
    - 主要在用户态
    - 零拷贝 send/recv
    - 支持 epoll 语义

  API:
    zf_init();
    zf_stack_alloc();
    zftl_listen();
    zft_send();
    zft_recv();

  性能: 延迟 ~500ns, 吞吐 40Gbps+
)";
}
} // namespace ch19


// =============================================================================
// ██  四、RDMA 篇
// =============================================================================

namespace ch20 {
void demo() {
    print_section("第20章: RDMA 基本概念");
    std::cout << R"(
  ═══ RDMA (Remote Direct Memory Access) ═══

  远程直接内存访问:
    - 绕过远端 CPU
    - 零拷贝
    - 硬件实现传输

  核心概念:
    QP (Queue Pair): 发送队列 + 接收队列
    CQ (Completion Queue): 完成通知
    MR (Memory Region): 注册内存
    WR (Work Request): 工作请求

  ┌──────────┐           ┌──────────┐
  │  Host A  │           │  Host B  │
  │  ┌────┐  │           │  ┌────┐  │
  │  │ App│  │           │  │ App│  │
  │  └─┬──┘  │           │  └─▲──┘  │
  │    │ RDMA WRITE       │    │     │
  │  ┌─▼────┐│  ═════════►│  ┌─┴───┐│
  │  │ RNIC ││            │  │ RNIC││
  │  └──────┘│           │  └─────┘│
  └──────────┘           └──────────┘

  B 的 CPU 无需参与!
)";
}
} // namespace ch20

namespace ch21 {
void demo() {
    print_section("第21章: InfiniBand vs RoCE vs iWARP");
    std::cout << R"(
  ═══ RDMA 协议对比 ═══

  ┌─────────────┬──────────┬──────────┬────────────┐
  │ 协议         │ 物理层   │ 延迟     │ 部署       │
  ├─────────────┼──────────┼──────────┼────────────┤
  │ InfiniBand  │ IB       │ 0.5-1μs  │ HPC        │
  │ RoCE v1     │ Ethernet │ 1-2μs    │ 数据中心   │
  │ RoCE v2     │ Ethernet │ 1-2μs    │ 数据中心   │
  │ iWARP       │ Ethernet │ 2-4μs    │ 企业       │
  └─────────────┴──────────┴──────────┴────────────┘

  RoCE (RDMA over Converged Ethernet):
    v1: L2 (同子网)
    v2: UDP/IP (可路由)

  最流行: RoCE v2 (兼容以太网基础设施)
)";
}
} // namespace ch21

namespace ch22 {
void demo() {
    print_section("第22章: RDMA Verbs API");
    std::cout << R"(
  ═══ ibverbs 编程流程 ═══

  1. 打开设备
     struct ibv_device **dev_list = ibv_get_device_list(NULL);
     struct ibv_context *ctx = ibv_open_device(dev_list[0]);

  2. 分配 PD (Protection Domain)
     struct ibv_pd *pd = ibv_alloc_pd(ctx);

  3. 注册内存
     void *buf = malloc(4096);
     struct ibv_mr *mr = ibv_reg_mr(pd, buf, 4096,
                                     IBV_ACCESS_LOCAL_WRITE |
                                     IBV_ACCESS_REMOTE_WRITE);

  4. 创建 CQ
     struct ibv_cq *cq = ibv_create_cq(ctx, 10, NULL, NULL, 0);

  5. 创建 QP
     struct ibv_qp_init_attr attr = {};
     attr.send_cq = cq;
     attr.recv_cq = cq;
     attr.qp_type = IBV_QPT_RC;  // Reliable Connection
     attr.cap.max_send_wr = 10;
     attr.cap.max_recv_wr = 10;
     attr.cap.max_send_sge = 1;
     attr.cap.max_recv_sge = 1;
     
     struct ibv_qp *qp = ibv_create_qp(pd, &attr);

  6. 修改 QP 状态: RESET → INIT → RTR → RTS

  7. Post Work Request

  8. Poll CQ 获取完成事件
)";
}
} // namespace ch22

namespace ch23 {
void demo() {
    print_section("第23章: RDMA 单边操作 (WRITE/READ)");
    std::cout << R"(
  ═══ RDMA WRITE (单边写) ═══

  本地 → 直接写远端内存, 远端 CPU 不参与

  struct ibv_sge sge = {
      .addr = (uint64_t)local_buf,
      .length = len,
      .lkey = mr->lkey
  };

  struct ibv_send_wr wr = {}, *bad_wr;
  wr.wr_id = 1;
  wr.sg_list = &sge;
  wr.num_sge = 1;
  wr.opcode = IBV_WR_RDMA_WRITE;
  wr.send_flags = IBV_SEND_SIGNALED;
  wr.wr.rdma.remote_addr = remote_addr;
  wr.wr.rdma.rkey = remote_key;

  ibv_post_send(qp, &wr, &bad_wr);

  // Poll CQ 确认完成
  struct ibv_wc wc;
  while (ibv_poll_cq(cq, 1, &wc) < 1);
  assert(wc.status == IBV_WC_SUCCESS);

  ═══ RDMA READ (单边读) ═══

  从远端内存读到本地

  wr.opcode = IBV_WR_RDMA_READ;
  // 其他同 WRITE

  性能: 延迟 ~1-2μs, 吞吐 100Gbps+
  
  特点: 远端 CPU 零开销!
)";
}
} // namespace ch23

namespace ch24 {
void demo() {
    print_section("第24章: RDMA 双边操作 (SEND/RECV)");
    std::cout << R"(
  ═══ SEND/RECV (双边操作) ═══

  需要远端预先 Post Recv

  发送端:
    struct ibv_sge sge = { ... };
    struct ibv_send_wr wr = {};
    wr.opcode = IBV_WR_SEND;
    wr.send_flags = IBV_SEND_SIGNALED;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    
    ibv_post_send(qp, &wr, &bad_wr);

  接收端:
    // 预先 Post Recv
    struct ibv_recv_wr wr = {};
    wr.wr_id = 2;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    
    ibv_post_recv(qp, &wr, &bad_wr);
    
    // Poll CQ 等待接收
    struct ibv_wc wc;
    while (ibv_poll_cq(cq, 1, &wc) < 1);

  适用: 小消息通信, 需要接收端参与
)";
}
} // namespace ch24

namespace ch25 {
void demo() {
    print_section("第25章: RDMA CM (连接管理)");
    std::cout << R"(
  ═══ RDMA CM (Connection Manager) ═══

  简化 QP 连接建立 (类似 TCP socket)

  服务端:
    rdma_create_id(NULL, &listen_id, NULL, RDMA_PS_TCP);
    rdma_bind_addr(listen_id, ...);
    rdma_listen(listen_id, 10);
    
    rdma_get_request(listen_id, &conn_id);
    rdma_accept(conn_id, NULL);

  客户端:
    rdma_create_id(NULL, &conn_id, NULL, RDMA_PS_TCP);
    rdma_resolve_addr(conn_id, NULL, server_addr, 2000);
    rdma_resolve_route(conn_id, 2000);
    rdma_connect(conn_id, NULL);

  自动处理 QP 状态转换!
)";
}
} // namespace ch25


// =============================================================================
// ██  五、高级网络技术篇
// =============================================================================

namespace ch26 {
void demo() {
    print_section("第26章: XDP (eXpress Data Path)");
    std::cout << R"(
  ═══ XDP 原理 ═══

  在网卡驱动最早阶段运行 eBPF 程序
    - SKB 分配之前
    - 极低延迟 (~50ns)

  ┌────────┐
  │  NIC   │ → XDP eBPF → 决策 (PASS/DROP/TX)
  └────────┘

  使用场景:
    - DDoS 防护 (早期丢弃)
    - 负载均衡
    - 包过滤

  示例 (eBPF):
    SEC("xdp")
    int xdp_drop(struct xdp_md *ctx) {
        void *data = (void *)(long)ctx->data;
        void *data_end = (void *)(long)ctx->data_end;
        
        struct ethhdr *eth = data;
        if ((void *)(eth + 1) > data_end)
            return XDP_DROP;
        
        // 丢弃特定 MAC
        if (eth->h_dest[0] == 0xff)
            return XDP_DROP;
        
        return XDP_PASS;
    }

  性能: 24-40 Mpps (10GbE)
)";
}
} // namespace ch26

namespace ch27 {
void demo() {
    print_section("第27章: AF_XDP Socket");
    std::cout << R"(
  ═══ AF_XDP (地址族 XDP) ═══

  XDP + 用户态 socket
    - 零拷贝到用户空间
    - 绕过内核协议栈

  创建:
    int fd = socket(AF_XDP, SOCK_RAW, 0);
    struct xsk_socket *xsk;
    xsk_socket__create(&xsk, ifname, queue_id, ...);

  RX:
    xsk_ring_cons__peek(&xsk->rx, nb, &idx);
    // 处理 packets
    xsk_ring_cons__release(&xsk->rx, nb);

  TX:
    xsk_ring_prod__reserve(&xsk->tx, nb, &idx);
    // 填充 packets
    xsk_ring_prod__submit(&xsk->tx, nb);

  性能: 20-30 Mpps, 延迟 <1μs
)";
}
} // namespace ch27

namespace ch28 {
void demo() {
    print_section("第28章: io_uring 网络优化");
    std::cout << R"(
  ═══ io_uring (Linux 5.1+) ═══

  异步 I/O 环, 可显著减少系统调用次数

  初始化:
    struct io_uring ring;
    io_uring_queue_init(256, &ring, 0);

  异步 recv:
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_recv(sqe, fd, buf, len, 0);
    io_uring_submit(&ring);
    
    struct io_uring_cqe *cqe;
    io_uring_wait_cqe(&ring, &cqe);
    ssize_t ret = cqe->res;
    io_uring_cqe_seen(&ring, cqe);

  特性:
    - 批量提交 (减少syscall)
    - 零拷贝 (注册 buffer)
    - kernel polling (SQPOLL)

  性能: 在部分 workload 下吞吐可明显高于 epoll
)";
}
} // namespace ch28

namespace ch29 {
void demo() {
    print_section("第29章: SmartNIC 与 DPU");
    std::cout << R"(
  ═══ SmartNIC / DPU ═══

  网卡 + ARM/FPGA 处理器

  能力:
    - 网卡上运行应用 (OVS, 防火墙)
    - 卸载主机 CPU
    - 线速包处理

  厂商:
    NVIDIA BlueField DPU (ARM + ConnectX)
    Intel IPU
    AMD Pensando

  编程:
    DOCA SDK (NVIDIA)
    P4 (可编程数据平面)

  应用: 云原生网络, 存储加速
)";
}
} // namespace ch29

namespace ch30 {
void demo() {
    print_section("第30章: FPGA 网卡加速");
    std::cout << R"(
  ═══ FPGA 网卡 ═══

  可编程硬件加速
    - Verilog/VHDL 定制逻辑
    - 超低延迟 (特定场景可逼近 100ns 级)

  厂商:
    Xilinx Alveo
    Intel Stratix/Agilex FPGA

  应用:
    - HFT (高频交易)
    - 协议卸载 (TCP/IP in FPGA)
    - 加密/解密

  工具链:
    Vivado (Xilinx)
    Quartus (Intel)
    OpenCL/HLS (高层次综合)

  挑战: 开发复杂度高, 周期长
)";
}
} // namespace ch30


// =============================================================================
// ██  六、性能优化篇
// =============================================================================

namespace ch31 {
void demo() {
    print_section("第31章: 网络延迟测量与分析");
    std::cout << R"(
  ═══ 延迟测量工具 ═══

  1. 用户态时间戳
     clock_gettime(CLOCK_MONOTONIC, &ts);
     精度: ~20-50ns

  2. 硬件时间戳 (PTP)
     精度: <100ns
     需要网卡支持 (Solarflare, Mellanox)

  3. 测量维度
     - 单向延迟 (需要时钟同步)
     - 往返延迟 (RTT)
     - 中位数 (P50)
     - 尾延迟 (P99, P99.9)
     - 抖动 (标准差)

  ═══ 延迟分析 ═══

  直方图:
    [0-1μs]: ████████████ 60%
    [1-2μs]: ██████ 30%
    [2-5μs]: ██ 8%
    [5-10μs]: █ 1.5%
    [>10μs]: 0.5%

  关注尾延迟!

  ═══ 工具 ═══

  sockperf, netperf, iperf3, fio
)";
}
} // namespace ch31

namespace ch32 {
void demo() {
    print_section("第32章: 多队列网卡优化");
    std::cout << R"(
  ═══ 多队列优化 ═══

  1. 队列数 = CPU 核心数
     ethtool -L eth0 combined 8

  2. RSS 配置
     ethtool -X eth0 equal 8

  3. IRQ 绑核
     echo 2 > /proc/irq/125/smp_affinity
     # 或使用 irqbalance

  4. RPS/RFS (软件RSS)
     echo 4096 > /sys/class/net/eth0/queues/rx-0/rps_cpus

  ═══ 监控 ═══

  ethtool -S eth0 | grep queue
)";
}
} // namespace ch32

namespace ch33 {
void demo() {
    print_section("第33章: CPU 绑核与中断亲和性");
    std::cout << R"(
  ═══ CPU 隔离 ═══

  Kernel 参数:
    isolcpus=2-7 nohz_full=2-7 rcu_nocbs=2-7

  效果: CPU 2-7 不参与调度, 专用于应用

  ═══ 线程绑核 ═══

  pthread:
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

  taskset:
    taskset -c 2-7 ./myapp

  ═══ NUMA 优化 ═══

  numactl --cpunodebind=0 --membind=0 ./app
)";
}
} // namespace ch33

namespace ch34 {
void demo() {
    print_section("第34章: 内存池与零分配设计");
    std::cout << R"(
  ═══ 零分配原则 ═══

  1. 预分配所有内存
     启动时分配, 运行时不调用 malloc

  2. 对象池
     固定大小对象复用

  3. 大页内存
     减少 TLB miss

  ═══ 示例: 简单对象池 ═══

  template<typename T, size_t N>
  class ObjectPool {
      T objects_[N];
      std::bitset<N> used_;
  public:
      T* alloc() {
          for (int i = 0; i < N; ++i) {
              if (!used_[i]) {
                  used_.set(i);
                  return &objects_[i];
              }
          }
          return nullptr;
      }
      void free(T* obj) {
          size_t idx = obj - objects_;
          used_.reset(idx);
      }
  };
)";
}
} // namespace ch34

namespace ch35 {
void demo() {
    print_section("第35章: 生产环境网络调优清单");
    std::cout << R"(
  ═══ 系统级优化 (Linux) ═══

  1. 内核参数 (sysctl)
     net.core.rmem_max = 536870912
     net.core.wmem_max = 536870912
     net.ipv4.tcp_rmem = 4096 87380 536870912
     net.ipv4.tcp_wmem = 4096 65536 536870912
     net.core.netdev_max_backlog = 300000
     net.ipv4.tcp_no_metrics_save = 1
     net.ipv4.tcp_timestamps = 0

  2. 网卡参数 (ethtool)
     ethtool -G eth0 rx 4096 tx 4096  # Ring size
     ethtool -K eth0 gro off lro off  # 低延迟
     ethtool -K eth0 tso on gso on    # 高吞吐

  3. 中断合并
     ethtool -C eth0 rx-usecs 0       # 最低延迟
     ethtool -C eth0 rx-usecs 100     # 高吞吐

  ═══ 应用级优化 ═══

  1. Socket 选项
     TCP_NODELAY: 禁用 Nagle
     SO_REUSEPORT: 多进程监听
     SO_BUSY_POLL: 忙轮询

  2. 绑核
     taskset -c 2-7 ./app

  3. 优先级
     nice -n -20 ./app
     chrt -f 99 ./app  # 实时优先级

  ═══ DPDK 优化 ═══

  1. 大页: 1GB > 2MB
  2. NUMA 感知
  3. Burst 32-64
  4. Prefetch
  5. 批量操作

  ═══ 监控指标 ═══

  - 延迟 P50/P99/P999
  - 吞吐 (pps, Gbps)
  - 丢包率
  - CPU 利用率
  - Cache miss 率
  - 上下文切换

  ═══ 工具集 ═══

  perf top, perf stat
  ethtool -S
  sar -n DEV
  mpstat, iostat
  tcpdump, wireshark
)";
}
} // namespace ch35


// =============================================================================
// main
// =============================================================================

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << " 高性能网络开发完全教程 — DPDK·Solarflare·RDMA·定制网卡\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";

    // 一、网络性能基础
    print_header("一、网络性能基础篇 (5章)");
    ch1::demo();
    ch2::demo();
    ch3::demo();
    ch4::demo();
    ch5::demo();

    // 二、DPDK 核心
    print_header("二、DPDK 核心篇 (9章)");
    ch6::demo();
    ch7::demo();
    ch8::demo();
    ch9::demo();
    ch10::demo();
    ch11::demo();
    ch12::demo();
    ch13::demo();
    ch14::demo();

    // 三、Solarflare/Xilinx
    print_header("三、Solarflare/Xilinx 篇 (5章)");
    ch15::demo();
    ch16::demo();
    ch17::demo();
    ch18::demo();
    ch19::demo();

    // 四、RDMA
    print_header("四、RDMA 篇 (6章)");
    ch20::demo();
    ch21::demo();
    ch22::demo();
    ch23::demo();
    ch24::demo();
    ch25::demo();

    // 五、高级网络技术
    print_header("五、高级网络技术篇 (5章)");
    ch26::demo();
    ch27::demo();
    ch28::demo();
    ch29::demo();
    ch30::demo();

    // 六、性能优化
    print_header("六、性能优化篇 (5章)");
    ch31::demo();
    ch32::demo();
    ch33::demo();
    ch34::demo();
    ch35::demo();

    std::cout << "\n════════════════════════════════════════════════════════════════\n";
    std::cout << " ✓ 35章完整教程演示完成\n";
    std::cout << " ✓ 涵盖: DPDK, Solarflare, RDMA, XDP, io_uring\n";
    std::cout << " ✓ 编译: g++ -std=c++17 -O2 test15.cpp\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";

    return 0;
}
