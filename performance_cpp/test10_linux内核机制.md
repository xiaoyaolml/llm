# 深入理解 Linux 内核机制： 调度、内存管理、网络栈、 驱动等，请你给出详细指导教程

> The user is asking for a detailed tutorial on Linux kernel mechanisms covering scheduling, memory management, network stack, drivers, etc. Based on the pattern, this should go into test10.cpp. Let me check the current file.


Summarized conversation history已创建 test10.cpp，共 **24 章**，覆盖 Linux 内核六大子系统：

| 篇                     | 章 | 主题                   | 核心内容                                   |
| ---------------------- | -- | ---------------------- | ------------------------------------------ |
| **一、调度**     | 1  | CFS 完全公平调度器     | vruntime、红黑树、权重计算、模拟器         |
|                        | 2  | 实时调度策略           | SCHED_FIFO/RR/DEADLINE、RT 限制            |
|                        | 3  | CPU 亲和性与 NUMA      | `sched_setaffinity`、NUMA 拓扑、远程延迟 |
|                        | 4  | cgroups CPU 限制       | v1/v2 对比、Docker 映射                    |
| **二、内存**     | 5  | 虚拟内存与分页         | 四级页表、TLB、大页、进程地址空间演示      |
|                        | 6  | 伙伴系统               | 完整 Buddy Allocator 模拟（分配/合并）     |
|                        | 7  | SLAB/SLUB 分配器       | per-CPU 缓存、简化 Slab 模拟               |
|                        | 8  | mmap 与零拷贝          | MAP 标志、madvise、匿名映射演示            |
|                        | 9  | MMIO 与 DMA            | 端口 I/O → MMIO → DMA 演进、IOMMU        |
|                        | 10 | OOM Killer             | oom_score、overcommit、PSI 内存压力        |
| **三、文件系统** | 11 | VFS 虚拟文件系统       | superblock/inode/dentry/file 四大结构      |
|                        | 12 | Page Cache 与 I/O 调度 | 脏页回写参数、I/O 调度器对比               |
|                        | 13 | io_uring               | SQ/CQ 共享环、零系统调用、liburing 示例    |
| **四、网络栈**   | 14 | TCP/IP 协议栈          | 完整 RX/TX 路径、NAPI/GRO、拥塞控制        |
|                        | 15 | sk_buff                | 内存布局、skb_push/pull/put 操作           |
|                        | 16 | Netfilter/iptables     | 五个钩子、四张表、conntrack                |
|                        | 17 | epoll 内核实现         | 红黑树+就绪链表、ep_poll_callback、ET/LT   |
|                        | 18 | 零拷贝网络             | sendfile/splice/MSG_ZEROCOPY 对比          |
| **五、驱动**     | 19 | Linux 设备模型         | 字符/块/网络设备、cdev 注册流程、udev      |
|                        | 20 | 中断处理               | 上/下半部、softirq/tasklet/workqueue       |
| **六、同步**     | 21 | 内核同步原语           | spinlock/RCU/seqlock/mutex 对比表          |
| **七、实战**     | 22 | /proc 与 /sys 信息采集 | CPU/内存/网络/进程信息读取                 |
|                        | 23 | 性能分析工具链         | perf/ftrace/bpftrace/strace 速查表         |
|                        | 24 | 内核调优参数速查       | 调度/内存/网络/文件系统 sysctl 参数        |

编译（Linux）：

```bash
g++ -std=c++17 -O2 -pthread -o test10 test10.cpp
```

Windows 下也可编译（Linux 特有 API 的部分会输出占位信息），核心原理讲解和模拟器（CFS、伙伴系统、Slab）均可正常运行。
