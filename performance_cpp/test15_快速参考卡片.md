# 高性能网络开发 - 快速参考卡片

## 🚀 核心技术对比速查表

### 延迟对比 (单向，微秒 μs)

```
传统 Socket:     5-15 μs    ████████████████
DPDK:            0.5-2 μs   ██
Solarflare:      0.3-1 μs   █
RDMA:            0.7-2 μs   ██
XDP:             1-3 μs     ███
AF_XDP:          1-3 μs     ███
io_uring:        2-5 μs     █████
FPGA:            0.1-0.5 μs ▌
```

### 吞吐对比 (Mpps - 百万包/秒)

```
传统 Socket:     1-2 Mpps
DPDK:            10-100 Mpps    ← 最高吞吐
Solarflare:      10-50 Mpps
RDMA:            100 Gbps+
XDP:             24-40 Mpps
AF_XDP:          20-30 Mpps
io_uring:        5-10 Mpps
```

---

## 📖 35章速查导航

### 一、基础理论 (第1-5章)

| 章 | 主题 | 关键概念 |
|----|------|----------|
| 1 | 传统网络栈延迟 | syscall(300ns) + 拷贝(500ns) + TCP(2μs) + SKB(500ns) |
| 2 | 内核旁路技术 | Kernel Bypass, PMD, 零拷贝, 用户态驱动 |
| 3 | 零拷贝原理 | sendfile, MSG_ZEROCOPY, mmap, DMA 直接映射 |
| 4 | DMA 环形缓冲区 | RX/TX Ring, Descriptor Queue, Head/Tail |
| 5 | 轮询 vs 中断 | Polling(1μs 专核), Interrupt(5μs 节能), NAPI(混合) |

**记忆点**：
- ✅ 传统栈延迟 = **系统调用 + 拷贝 + 协议栈 + 中断**
- ✅ 内核旁路 = **跳过内核 + 轮询 + 零拷贝**

---

### 二、DPDK 核心 (第6-14章)

| 章 | 主题 | API/命令 |
|----|------|----------|
| 6 | DPDK 架构 | EAL, Mempool, Mbuf, PMD |
| 7 | 环境配置 | `dpdk-devbind.py`, 大页内存, VFIO |
| 8 | 内存管理 | `rte_mempool_create()`, per-core cache |
| 9 | Mbuf 操作 | `rte_pktmbuf_alloc/free/clone()` |
| 10 | PMD 驱动 | ixgbe, i40e, mlx5, SR-IOV |
| 11 | 收发包 | `rte_eth_rx_burst()`, `tx_burst()` |
| 12 | 多核并行 | RTC 模型, Pipeline 模型, `rte_ring` |
| 13 | RSS | 接收侧扩展, Hash到队列, RETA |
| 14 | Flow Director | RTE Flow API, 硬件流分类 |

**核心代码**：
```cpp
// DPDK 最小循环
struct rte_mbuf *bufs[32];
while (1) {
    uint16_t n = rte_eth_rx_burst(port, 0, bufs, 32);
    // 处理 bufs[0..n-1]
    rte_eth_tx_burst(port, 0, bufs, n);
}
```

**性能调优**：
```bash
# Burst Size: 32-64 (经验值)
# Ring Size: 1024-2048 (高吞吐), 256-512 (低延迟)
# CPU: 专用核心, 100% 轮询
# 内存: 1GB 大页 > 2MB 大页
```

---

### 三、Solarflare (第15-19章)

| 章 | 主题 | 关键技术 |
|----|------|----------|
| 15 | OpenOnload 原理 | LD_PRELOAD, 用户态TCP栈 |
| 16 | ef_vi API | 直接网卡访问, 零拷贝 |
| 17 | Onload 加速 | Socket API 劫持 |
| 18 | PTP 时间戳 | IEEE 1588, <100ns 精度 |
| 19 | TCPDirect | 用户态零拷贝TCP, `zf_*` API |

**使用速查**：
```bash
# 透明加速 (无需修改代码)
onload --profile=latency ./app

# 延迟优化
onload --profile=latency-best ./app

# ef_vi 示例
ef_vi_transmit(&vi, dma_addr, len, 0);
```

**适用场景**：
- ✅ HFT 交易 → 延迟 300-700ns
- ✅ 市场数据 → PTP 时间戳 <100ns
- ✅ 交易所 → 透明加速现有系统

---

### 四、RDMA (第20-25章)

| 章 | 主题 | 核心对象 |
|----|------|----------|
| 20 | RDMA 基础 | QP, CQ, MR, WR |
| 21 | 协议对比 | InfiniBand (1μs), RoCE v2 (1-2μs), iWARP (2-4μs) |
| 22 | ibverbs API | `ibv_create_qp()`, `ibv_reg_mr()` |
| 23 | 单边 WRITE/READ | 远端 CPU 零参与 |
| 24 | 双边 SEND/RECV | 消息语义 |
| 25 | RDMA CM | 连接管理, 类 Socket API |

**编程流程**：
```cpp
// 1. 打开设备
ibv_context *ctx = ibv_open_device(dev);

// 2. 分配 PD + 注册内存
ibv_pd *pd = ibv_alloc_pd(ctx);
ibv_mr *mr = ibv_reg_mr(pd, buf, 4096, IBV_ACCESS_REMOTE_WRITE);

// 3. 创建 CQ + QP
ibv_cq *cq = ibv_create_cq(ctx, 10, NULL, NULL, 0);
ibv_qp *qp = ibv_create_qp(pd, &attr);

// 4. RDMA WRITE
ibv_send_wr wr = { .opcode = IBV_WR_RDMA_WRITE, ... };
ibv_post_send(qp, &wr, &bad_wr);

// 5. Poll CQ
ibv_wc wc;
ibv_poll_cq(cq, 1, &wc);
```

**性能优势**：
```
TCP:
  App → Kernel → NIC  →  NIC → Kernel → App
  延迟: 10-20μs, 远端 CPU 40%

RDMA WRITE:
  App → RNIC  →  远端内存
  延迟: 1-2μs, 远端 CPU 0%!
```

---

### 五、高级技术 (第26-30章)

| 章 | 主题 | 使用场景 |
|----|------|----------|
| 26 | XDP | eBPF 早期过滤, DDoS 防护 |
| 27 | AF_XDP | 用户态 XDP Socket |
| 28 | io_uring | 异步 I/O 环, 批量提交 |
| 29 | SmartNIC | NVIDIA BlueField, 网卡运行OVS |
| 30 | FPGA | <100ns 超低延迟 |

**XDP 示例**：
```c
SEC("xdp")
int xdp_filter(struct xdp_md *ctx) {
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
```

**io_uring 示例**：
```cpp
struct io_uring ring;
io_uring_queue_init(256, &ring, 0);

// 异步 recv
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
io_uring_prep_recv(sqe, fd, buf, len, 0);
io_uring_submit(&ring);

// 等待完成
struct io_uring_cqe *cqe;
io_uring_wait_cqe(&ring, &cqe);
```

---

### 六、性能优化 (第31-35章)

| 章 | 主题 | 关键操作 |
|----|------|----------|
| 31 | 延迟测量 | P50/P99/P999, 硬件时间戳 |
| 32 | 多队列优化 | RSS, IRQ 绑定, RPS/RFS |
| 33 | CPU 绑核 | `isolcpus`, `taskset`, NUMA |
| 34 | 内存池 | 预分配, 大页, 零 malloc |
| 35 | 调优清单 | 100+ sysctl/ethtool 参数 |

**一键调优脚本**：
```bash
#!/bin/bash
# 高性能网络调优

# ===== 内核参数 =====
sysctl -w net.core.rmem_max=536870912
sysctl -w net.core.wmem_max=536870912
sysctl -w net.core.netdev_max_backlog=300000
sysctl -w net.ipv4.tcp_timestamps=0
sysctl -w net.ipv4.tcp_no_metrics_save=1

# ===== 网卡参数 =====
ethtool -G eth0 rx 4096 tx 4096           # Ring size
ethtool -K eth0 gro off lro off           # 禁用合并 (低延迟)
ethtool -C eth0 rx-usecs 0                # 禁用中断合并
ethtool -L eth0 combined 8                # 8 队列

# ===== CPU 隔离 (修改 /etc/default/grub) =====
# GRUB_CMDLINE_LINUX="isolcpus=2-7 nohz_full=2-7 rcu_nocbs=2-7"
# update-grub && reboot

# ===== IRQ 绑定 =====
for i in {125..132}; do
    echo 2 > /proc/irq/$i/smp_affinity
done

# ===== 大页内存 =====
echo 2048 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
mkdir -p /mnt/huge
mount -t hugetlbfs nodev /mnt/huge

echo "✅ 优化完成"
```

---

## 🎯 场景选型决策树

```
需要高性能网络？
│
├─ 通用应用 (Web/数据库/中间件)
│  ├─ 延迟要求 <10μs? → io_uring + TCP优化
│  └─ 吞吐优先? → 多队列 + RSS + RPS
│
├─ 数据平面 (路由/NFV/包处理)
│  ├─ 需要完全控制? → DPDK (10-100 Mpps)
│  └─ 早期过滤? → XDP/AF_XDP (20-40 Mpps)
│
├─ 低延迟 (金融/HFT)
│  ├─ 最低延迟 (<1μs)? → Solarflare OpenOnload
│  ├─ 透明加速? → OpenOnload (LD_PRELOAD)
│  └─ 硬件时间戳? → Solarflare PTP
│
├─ 跨节点通信 (存储/HPC/AI)
│  ├─ 需要高带宽 (>50Gbps)? → RDMA (100Gbps+)
│  ├─ 远端 CPU 参与? → RDMA WRITE (0% CPU)
│  └─ 小消息? → RDMA SEND/RECV
│
└─ 极致性能 (证券/交易所)
   ├─ 延迟 <100ns? → FPGA 网卡
   └─ 定制协议? → FPGA + Verilog
```

---

## 📊 性能指标速查

### 延迟分布 (微秒 μs)

| 技术 | P50 | P99 | P99.9 |
|------|-----|-----|-------|
| 传统 Socket | 8-12 | 30-50 | 100+ |
| DPDK | 1-2 | 5-8 | 15-20 |
| Solarflare | 0.3-0.7 | 1.5-3 | 5-10 |
| RDMA | 0.7-1.5 | 3-5 | 10-15 |
| XDP | 1-2 | 5-10 | 15-25 |
| io_uring | 2-4 | 10-15 | 25-40 |

### CPU 占用 (10Gbps 负载)

```
传统 Socket: ████████ 80% (中断风暴)
DPDK:        ██████████ 100% (1核专用轮询)
OpenOnload:  ████████ 85% (用户态TCP栈)
RDMA:        ██ 20% (硬件卸载)
XDP:         ███ 30% (早期过滤)
io_uring:    ████ 40% (异步批量)
```

---

## 🔧 常见问题速查

### DPDK

**Q**: `EAL: Cannot allocate memory`  
**A**: `echo 2048 > /sys/kernel/mm/hugepages/.../nr_hugepages`

**Q**: `No Ethernet ports`  
**A**: `dpdk-devbind.py --bind=vfio-pci 0000:03:00.0`

**Q**: 收发包为0  
**A**: 检查链路状态 `rte_eth_link_get()`, 启用混杂模式 `rte_eth_promiscuous_enable()`

### RDMA

**Q**: `ibv_devices` 无输出  
**A**: `modprobe mlx5_core mlx5_ib ib_uverbs`

**Q**: RDMA CM 连接超时  
**A**: 检查防火墙 TCP 18515, 配置 RoCE IP

**Q**: QP 状态转换失败  
**A**: 顺序: RESET → INIT → RTR → RTS

### 系统调优

**Q**: 网卡丢包  
**A**: 增大 Ring size `ethtool -G eth0 rx 4096`, 增大 backlog

**Q**: 延迟抖动大  
**A**: CPU 隔离 `isolcpus`, 禁用 C-states, 关闭节能

**Q**: 多队列不均衡  
**A**: 检查 RSS hash key, 调整 RETA 表

---

## 💡 最佳实践速记

### DPDK 三要素
1. **大页内存**: 1GB > 2MB
2. **CPU 绑核**: 专用核心 (isolcpus)
3. **批量操作**: Burst=32-64

### RDMA 三原则
1. **注册内存**: 提前 `ibv_reg_mr()`
2. **批量 Post**: 减少 doorbell 开销
3. **Inline 数据**: 小包 (<128B) 用 inline

### 低延迟三板斧
1. **轮询**: 禁用中断, 100% CPU
2. **绑核**: 避免迁移, NUMA local
3. **零拷贝**: DMA 直接映射

### 监控三指标
1. **延迟**: P50 + P99 + P999 (不要只看平均!)
2. **吞吐**: pps + Gbps
3. **丢包**: `ethtool -S eth0 | grep drop`

---

## 📝 快速命令备忘录

```bash
# ===== DPDK =====
dpdk-devbind.py --status                      # 查看设备
dpdk-devbind.py --bind=vfio-pci 03:00.0       # 绑定网卡
dpdk-testpmd -c 0xf -n 4 -- -i               # 测试程序

# ===== RDMA =====
ibv_devices                                   # 列出设备
ibv_devinfo                                   # 设备信息
ib_write_bw                                   # 带宽测试
ib_write_lat                                  # 延迟测试

# ===== 系统优化 =====
ethtool -S eth0                               # 网卡统计
ethtool -g eth0                               # Ring size
ethtool -k eth0                               # Offload 特性
ethtool -C eth0                               # 中断合并
cat /proc/interrupts                          # 中断统计
mpstat -P ALL 1                               # CPU 监控

# ===== 性能测试 =====
iperf3 -s                                     # TCP 吞吐
sockperf ping-pong -i 192.168.1.1 -p 5001    # 延迟
netperf -H 192.168.1.1 -t TCP_RR              # 请求/响应
```

---

## 🎓 学习检查清单

### 基础 ✅
- [ ] 理解传统网络栈延迟来源 (第1章)
- [ ] 掌握 DMA 与 Ring Buffer 原理 (第4章)
- [ ] 理解轮询 vs 中断权衡 (第5章)

### DPDK ✅
- [ ] 搭建 DPDK 环境 (大页/绑卡) (第7章)
- [ ] 实现简单收发包程序 (第11章)
- [ ] 理解 Mempool 与 Mbuf (第8-9章)
- [ ] 实现多核并行 (RTC/Pipeline) (第12章)

### RDMA ✅
- [ ] 配置 RDMA 环境 (ibverbs) (第22章)
- [ ] 实现 RDMA WRITE/READ (第23章)
- [ ] 对比 TCP vs RDMA 性能 (第20-21章)

### 高级 ✅
- [ ] 实现 XDP eBPF 过滤 (第26章)
- [ ] 使用 io_uring 异步 I/O (第28章)
- [ ] 完成生产环境调优 (第35章)

---

**最后更新**: 2024-01  
**用途**: 配合 `test15.cpp` 与 `test15_readme.md` 快速查阅  
**提示**: 打印本文档作为桌面参考速查卡片！
