# 熟悉 CPU 微架构：缓存层级、分支预测、乱序执行、 流水线优化，请你给出详细指导教程

> The user wants a detailed tutorial on CPU microarchitecture topics: cache hierarchy, branch prediction, out-of-order execution, and pipeline optimization. They want this in test14.cpp, following the same pattern as previous tutorials.

test14.cpp 已创建完成，共 40 章，覆盖 7 大专题：

**一、CPU 流水线基础篇** (ch1-4)

- 经典 5 级流水线、超标量多发射、流水线冒险、气泡与停顿实测

**二、缓存层级篇** (ch5-13)

- L1/L2/L3 延迟实测、缓存行、关联度与冲突、替换策略 (LRU/PLRU/RRIP)、写策略 (Write-Back/NT Store)、MESI/MOESI 协议、False Sharing 实测、缓存友好数据结构、Cache Thrashing 与工作集

**三、TLB 与虚拟内存篇** (ch14-16)

- 4 级页表与 Page Walk、大页对 TLB 影响、PCID 优化

**四、分支预测篇** (ch17-24)

- BHT/BTB 原理、2-bit 计数器、两级自适应/GShare、TAGE 预测器、间接分支预测 (虚函数)、预测失败代价实测、无分支编程技术库、likely/unlikely/PGO

**五、乱序执行篇** (ch25-32)

- OoO 引擎全景、寄存器重命名 (RAT)、保留站、ROB、Store Buffer 与内存消歧、执行端口调度、ILP 最大化实测、循环携带依赖优化 (多路累加器)

**六、前端优化篇** (ch33-36)

- I-Cache 优化、μop Cache (DSB)、循环流检测器 (LSD)、代码对齐与 BOLT

**七、性能计数器与分析篇** (ch37-40)

- PMU 硬件计数器、Top-Down 分析法 (TMAM)、VTune/μProf/FlameGraph、微架构优化检查清单与延迟速查表

所有章节包含可编译运行的 benchmark 代码 + 详细的中文图表说明。

Made changes.
