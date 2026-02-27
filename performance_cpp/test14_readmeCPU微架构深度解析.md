# CPU 微架构深度解析
## 缓存层级 · 分支预测 · 乱序执行 · 流水线优化

---

## 📚 概览

`test14.cpp` 是一份 **46 章、8 大专题** 的 CPU 微架构完整教程，每章包含：

- 精确的 ASCII 架构图 + 中文注释
- 可编译运行的基准测试代码
- 硬件参数数据表 (Intel / AMD / Apple Silicon)
- 调试工具命令 (perf / VTune / LLVM-MCA)

```bash
# 编译运行 (Linux)
g++ -std=c++17 -O2 -march=native -pthread -o test14 test14.cpp
./test14

# Windows MSVC
cl /std:c++17 /O2 /EHsc /arch:AVX2 test14.cpp
```

---

## 目录 (46 章 / 8 大专题)

| 专题 | 章节 | 核心主题 |
|------|------|----------|
| 一、流水线基础 | 1–4 | 5级流水线、超标量、冒险、气泡测量 |
| 二、缓存层级 | 5–13 | L1/L2/L3、缓存行、关联度、MESI、False Sharing |
| 三、TLB 与虚拟内存 | 14–16 | 4级页表、大页、PCID |
| 四、分支预测 | 17–24 | BHT/BTB、TAGE、间接分支、无分支技术 |
| 五、乱序执行 | 25–32 | ROB、RAT、保留站、Store Buffer、ILP |
| 六、前端优化 | 33–36 | I-Cache、μop Cache (DSB)、LSD、代码对齐 |
| 七、性能计数器 | 37–40 | PMU、Top-Down、VTune/μProf、优化清单 |
| 八、扩展专题 | 41–46 | 投机执行安全、预取器、SIMD、Roofline、NUMA、编译器控制 |

---

## 一、CPU 流水线基础篇 (第 1–4 章)

### 第 1 章：经典 5 级流水线

经典 RISC 流水线五个阶段：

```
IF → ID → EX → MEM → WB
取指   译码   执行   访存    写回
```

**理想状态**：每个 cycle 完成一条指令 (CPI = 1)。

**现代 CPU 流水线深度对比**：

| 处理器 | 级数 | 备注 |
|--------|------|------|
| 经典 RISC | 5 | 教科书模型 |
| Pentium 4 | 31 | NetBurst，分支惩罚极大 |
| Sandy Bridge | 14–19 | 平衡深度与 IPC |
| Zen 4 | ~19 | AMD 现代架构 |
| Golden Cove | ~20 | Intel 12th Gen+ |
| Apple M2 (P-core) | ~16 | 宽发射 + 浅流水线 |

**关键权衡**：流水线越深 → 主频越高，但分支惩罚越大 (~15–25 cycles)。  
**现代趋势**：适度深度 (14–20) + 超宽发射 (6–8 wide) 而非简单加深。

---

### 第 2 章：超标量与多发射

**超标量**：每 cycle 同时发射多条指令。

```
标量:   inst1 inst2 inst3 inst4 → 每条顺序进入流水线
4-wide: inst1+inst2+inst3+inst4 → 同一拍全部取/译/执！
```

**现代 CPU 发射宽度**：

| 架构 | 发射宽度 | 执行端口配置 |
|------|----------|-------------|
| Zen 4 | 6-wide | 4 ALU + 3 LD + 2 ST |
| Golden Cove | 6-wide | 5 ALU + 3 LD + 2 ST |
| Apple Firestorm | 8-wide | 6 ALU + 3 LD + 2 ST |
| Neoverse V2 | 10-wide | ARM 服务器 |

**实测发现**（test14 ch2 benchmark）：
- 4 路独立累加 vs 串行依赖链，常见可见明显差距（幅度依赖 CPU 与编译选项）
- IPC 实际约 2–4（受限于依赖、Cache miss、分支）

**IPC 四大限制因素**：
1. 数据依赖 → 无法并行执行
2. 缓存 miss → 等待 DRAM ~200 cycles
3. 分支预测失败 → 清空流水线
4. 执行端口冲突 → 某端口饱和

---

### 第 3 章：流水线冒险

三类冒险：

**1. 数据冒险 (RAW — Read After Write)**，最常见：

```asm
ADD R1, R2, R3   ; R1 还没写回
SUB R4, R1, R5   ; 但立刻需要 R1 → 停顿!
```

现代 CPU 解决方案：**前递 (Forwarding)**，EX 阶段结果直接旁路到下一条指令，无需等 WB。

**2. 控制冒险**：分支方向在 EX 才确定，前面已取了若干指令 → **分支预测**（详见第四篇）。

**3. 结构冒险**：两条指令竞争同一硬件单元。现代 CPU 资源充足，几乎不发生。

> 名字依赖 (WAW / WAR) 通过 **寄存器重命名** 消除（详见第 26 章）。

---

### 第 4 章：流水线气泡与停顿测量

**实测三种依赖链长度**（test14 ch4 benchmark）：

```
链长=1 (4路独立): ~0.25 ns/iter  ← 超标量并行
链长=2 (串行):   ~0.50 ns/iter  ← 每步等前一步
链长=4 (串行):   ~1.00 ns/iter  ← 依赖链深度×延迟
```

**perf 查看停顿**：

```bash
perf stat -e cycles,instructions,\
  stalled-cycles-frontend,stalled-cycles-backend \
  ./app
```

| 停顿类型 | 来源 | 优化方向 |
|----------|------|----------|
| Frontend Stall | I-Cache miss、DSB miss、译码瓶颈 | 代码紧凑、代码对齐 |
| Backend Stall | 数据依赖、D-Cache miss、端口饱和 | ILP、预取、SIMD |

---

## 二、缓存层级篇 (第 5–13 章)

### 第 5 章：L1/L2/L3 延迟实测

**缓存层级全景**：

```
    Core
   ┌─────────────┐
   │ L1D  48KB   │   4–5 cycles  (~1 ns)
   │ L1I  32KB   │
   ├─────────────┤
   │ L2   1.25MB │   ~12 cycles  (~4 ns)
   └──────┬──────┘
          │ (共享)
   ┌──────┴──────┐
   │ L3   30MB   │   40–50 cycles (~12 ns)
   └──────┬──────┘
          │
   ┌──────┴──────┐
   │   DRAM      │   200+ cycles (~65 ns)
   └─────────────┘
```

**Intel Golden Cove / Zen 4 参数**：

| 层级 | 大小 | 延迟 | 带宽 | 关联度 |
|------|------|------|------|--------|
| L1D | 48 KB | 4–5 cyc | 2×64B/cyc | 12-way |
| L1I | 32 KB | — | 32B/cyc | 8-way |
| L2 | 1.25 MB | 12 cyc | 64B/cyc | 10-way |
| L3 | 30 MB | 40–50 cyc | 32B/cyc | 12–16-way |
| DRAM | GBs | 200+ cyc | ~25 GB/s | — |

**实测规律**（test14 ch5 benchmark，步长 = 缓存行）：

```
    4 KB:  ~1 ns  → L1 命中
   64 KB:  ~4 ns  → L2 命中  (L1 放不下)
  256 KB:  ~5 ns  → L2 命中
 4096 KB: ~12 ns  → L3 命中
16384 KB: ~40 ns  → DRAM
```

> **核心结论**：DRAM 比 L1 慢 **40–50×**，数据局部性的重要性不可低估。

---

### 第 6 章：缓存行详解

缓存最小操作单位 = **64字节缓存行**。

地址分解 (32KB L1D, 8-way, 64B line)：

```
┌──────────────┬────────────┬──────────────┐
│  Tag (高位)  │ Index (6b) │ Offset (6b)  │
│  唯一标识行  │ 定位组     │ 行内偏移     │
└──────────────┴────────────┴──────────────┘
```

**空间局部性**：访问 `data[0]` → 整行 (`data[0..15]`) 自动加载。

**步长陷阱实测**（test14 ch6）：

```
连续访问 (stride=1):         ~0.25 ns/elem  ← 完美利用空间局部性
步长=16 (stride=cache line): ~0.5 ns/access ← 每行只用 1 个元素
步长=256:                    ~3 ns/access   ← 接近 L2 延迟
```

**优化原则**：
- 优先选择 `std::vector` 等连续存储
- 避免指针追逐 (linked list)
- SoA (Struct of Arrays) > AoS (Array of Structs) for SIMD

---

### 第 7 章：缓存关联度与冲突 Miss

**2的幂步长问题**（冲突 miss 实测，test14 ch7）：

```
步长 4096 (冲突!):    ~8 ns/access  ← 全打同一 set
步长 4096+64 (错开): ~1 ns/access  ← 分散到不同 set
```

**根因**：32KB L1D 8-way → 64 组，相邻 `4096B` 的地址映射到同一 set → 9 个数组就驱逐了 8-way。

**解决方案**：
```cpp
float matrix[1024][1024];      // 列访问全冲突
float matrix[1024][1024+16];   // +padding 错开 set
```

---

### 第 8 章：替换策略

| 策略 | 原理 | 使用场景 |
|------|------|----------|
| LRU | 驱逐最久未访问的行 | L1/L2 |
| PLRU (伪LRU) | 二叉树决策，每 way 1 bit | 现代 CPU L1/L2 |
| RRIP | 2–3 bit 再引用间隔预测 | **Intel L3** |
| Random | 随机选择 | 某些 ARM |

**RRIP 的优势**：大数组扫描不会冲掉热数据，适合 LLC。

---

### 第 9 章：写策略

**Write-Back（现代 CPU 默认）** + **非临时写 (Non-Temporal Store)**：

```cpp
// 绕过缓存直接写内存，适合只写不读的大块数据
_mm_stream_si128((__m128i*)ptr, val);
_mm_sfence();
```

适用：数据写完不会再读 + 写满整个缓存行 + 顺序访问（利用 WCB）。

---

### 第 10 章：MESI/MOESI 缓存一致性

**MESI 协议四状态**（Intel）：

| 状态 | 含义 |
|------|------|
| M (Modified) | 本核独占，已修改 |
| E (Exclusive) | 本核独占，未修改 |
| S (Shared) | 多核共有，未修改 |
| I (Invalid) | 无效 |

**跨核延迟**：

| 场景 | 延迟 |
|------|------|
| 同核 L1 hit | ~4 cycles |
| **跨核 MESI snoop** | **~40 cycles** |
| 跨 CCD/Die (AMD) | ~60 cycles |
| 跨 NUMA 节点 | ~150 cycles |

> **MESI 乒乓** = False Sharing 的根因。

---

### 第 11 章：False Sharing 深度实测

```cpp
struct NoPad {
    std::atomic<int64_t> a{0};
    std::atomic<int64_t> b{0};  // 同一 64B 行
};

struct WithPad {
    alignas(64) std::atomic<int64_t> a{0};
    alignas(64) std::atomic<int64_t> b{0};  // 独立缓存行
};
```

**实测**（2 线程，各递增 1000 万次）：

```
NoPad  (false sharing):    ~250 ms
WithPad (无 false sharing): ~30 ms
性能差异: ~8×
```

**检测**：`perf c2c record ./app && perf c2c report`

---

### 第 12 章：缓存友好的数据结构

**链表 vs 数组实测**（遍历 100 万节点）：

```
std::vector:  ~0.4 ms  ← 顺序加载，硬件预取
std::list:    ~8.0 ms  ← 指针随机跳跃，每次 cache miss
```

**AoS vs SoA**：
```cpp
// AoS: 遍历 x 需要跳过 y/z/mass
struct Particle { float x, y, z, mass; };

// SoA: 所有 x 连续 → SIMD 友好
struct Particles { std::vector<float> x, y, z, mass; };
```

---

### 第 13 章：缓存 Thrashing 与分块

**分块矩阵乘法** (TILE ≈ 64 或 128) → 3× 以上提升（N=512 时）：

```cpp
// 分块: 工作集 = 3×TILE²×sizeof(float) < L2 大小
for (ii+=TILE) for (jj+=TILE) for (kk+=TILE)
  for i in [ii..ii+TILE]: for j ...: for k ...:
    C[i][j] += A[i][k] * B[k][j];
```

---

## 三、TLB 与虚拟内存篇 (第 14–16 章)

### 第 14 章：TLB 层级与 Page Walk

**4 级页表 (x86-64, 4KB 页)**：

```
虚拟地址 48 bit:
┌────────┬────────┬────────┬────────┬────────────┐
│ PML4   │  PDP   │   PD   │   PT   │  Offset    │
│ 9 bits │ 9 bits │ 9 bits │ 9 bits │  12 bits   │
└────────┴────────┴────────┴────────┴────────────┘
```

| 层级 | 条目数 | 命中延迟 |
|------|--------|----------|
| L1 DTLB | 96 entries (4KB) | 1 cycle |
| L2 STLB | 2048 entries | ~8 cycles |
| Page Walk | — | **~100–200 cycles** |

---

### 第 15 章：大页对 TLB 的影响

| 页大小 | L2 STLB 覆盖 |
|--------|-------------|
| 4 KB | 2048 × 4KB = **8 MB** |
| **2 MB** | 1024 × 2MB = **2 GB** |
| 1 GB | 16 × 1GB = **16 GB** |

```bash
# 配置大页
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

# C++ 申请
void* p = mmap(NULL, size, PROT_READ|PROT_WRITE,
               MAP_ANON|MAP_PRIVATE|MAP_HUGETLB, -1, 0);
```

---

### 第 16 章：PCID 与 TLB 刷新

**PCID (Process Context ID)**（Intel Broadwell+）：每 TLB 条目带 12-bit 进程 ID → 进程切换无需完全刷新 TLB。

**KPTI（Spectre/Meltdown 缓解）无 PCID 时**：syscall 场景下开销常更明显；有 PCID 后通常可显著降低代价，具体幅度依赖 CPU、内核版本与负载。

---

## 四、分支预测篇 (第 17–24 章)

### 第 17 章：分支预测器原理

| 组件 | 大小 | 功能 |
|------|------|------|
| **BHT/PHT** | ~16K–64K entries | 2-bit 计数器预测 taken/not-taken |
| **BTB** | 4096–12288 entries | 缓存分支目标地址 |
| **RAS** | 16–32 entries | 专门预测 `RET` 返回地址 |
| **Loop Predictor** | 64–256 entries | 识别循环退出计数 |

---

### 第 18–19 章：2-bit 计数器与 GShare

**2-bit 饱和计数器**：

```
强跳(11) ←→ 弱跳(10) ←→ 弱不跳(01) ←→ 强不跳(00)
连续两次反向才翻转 → 循环末尾不立即误预测
```

**GShare 两级自适应**：`BHR XOR PC → PHT 索引 → 2-bit 计数器`，准确率 95–97%。

---

### 第 20 章：TAGE 预测器

**几何级数历史长度**：T0(无历史) + T1(2) + T2(4) + T3(8) + T4(16) + T5(32) + T6(64) + T7(128)。

| CPU | 准确率 (SPEC) |
|-----|--------------|
| 2-bit 计数器 | ~85–90% |
| GShare | ~95–97% |
| **TAGE** | **~97–99%** |

---

### 第 21 章：间接分支预测（虚函数）

多态分派 → 类型数量越多 → IBTB 预测失败率越高。

**优化**：按类型排序对象后批量处理，连续调用同类型通常可提高 BTB 命中率（提升幅度需实测）。

---

### 第 22–23 章：分支 Miss 代价与无分支技术

**分支 Miss 代价实测**：

```
排序数组: ~1.2 ms  (miss <0.1%)
随机数组: ~4.5 ms  (miss ~25%)  → 3.75× 差距
```

**无分支技术库**（高 miss rate >5% 时使用）：

```cpp
int branchless_abs(int x)    { int mask=x>>31; return (x+mask)^mask; }
int branchless_min(int a,int b) { return b+((a-b)&((a-b)>>31)); }
int branchless_select(bool c,int a,int b) { return b^((a^b)&-(int)c); }
```

---

### 第 24 章：likely/unlikely/PGO

```cpp
if (x > 0) [[likely]] { ... }   // C++20
if (LIKELY(x > 0)) ...           // GCC/Clang __builtin_expect

// PGO
g++ -O2 -fprofile-generate -o app app.cpp   // 插桩
./app < input                                // 收集
g++ -O2 -fprofile-use -o app app.cpp        // 优化
```

---

## 五、乱序执行篇 (第 25–32 章)

### 第 25 章：乱序执行引擎全景

```
取指 → 译码 → 寄存器重命名 → 分配ROB/RS
              ↓
        保留站 (RS) — 操作数就绪?
              ↓
        执行单元 (ALU×4, LD×3, ST×2)
              ↓
        ROB 标记完成 → 按序退役
```

**ROB 大小**：Zen 4=320, Golden Cove=512, Apple Firestorm=~630。

---

### 第 26 章：寄存器重命名 (RAT)

**消除 WAW/WAR 名字依赖**，物理寄存器 ~280 个 (Zen 4)。

**Move Elimination**：`MOV R1, R2` 仅更新 RAT 映射 → 零 μop，零延迟。

---

### 第 27–28 章：保留站与 ROB

| 架构 | RS | ROB |
|------|-----|-----|
| Zen 4 | 96 | 320 |
| Golden Cove | 160+ | 512 |
| Firestorm | ~330 | ~630 |

**DRAM miss (~200 cycles) 大于 ROB 窗口能隐藏的量 → ROB 填满 → 前端停顿**。这驱动了 ROB 容量持续增大。

---

### 第 29 章：Store Buffer 与内存消歧

- **Store-Load Forwarding**：Load 地址在 Store Buffer 中 → 直接转发，无需访问缓存
- x86 TSO：Store Buffer 导致其他核暂时不可见 → `MFENCE` 刷新 Store Buffer

---

### 第 30 章：执行端口与指令延迟

**Intel Golden Cove 关键指令**：

| 指令 | 延迟 | 吞吐 |
|------|------|------|
| ADD/XOR | 1 cyc | 4/cyc |
| IMUL (32b) | 3 cyc | 1/cyc |
| IDIV (64b) | 20–90 | — |
| FP ADD/MUL | 4–5 cyc | 2/cyc |
| L1 Load | 4–5 cyc | 3/cyc |

---

### 第 31–32 章：ILP 最大化

**多路累加器破解循环携带依赖**（FP ADD 延迟4cyc, 吞吐2/cyc → 需 8 路）：

```cpp
double s0=0,s1=0,s2=0,s3=0,s4=0,s5=0,s6=0,s7=0;
for (int i=0; i<N; i+=8) {
    s0+=data[i];   s1+=data[i+1]; s2+=data[i+2]; s3+=data[i+3];
    s4+=data[i+4]; s5+=data[i+5]; s6+=data[i+6]; s7+=data[i+7];
}
double sum = ((s0+s1)+(s2+s3)) + ((s4+s5)+(s6+s7));
```

---

## 六、前端优化篇 (第 33–36 章)

### 第 34 章：μop Cache (DSB)

| 架构 | DSB 容量 | 带宽 |
|------|----------|------|
| Sandy Bridge | 1536 μops | 4/cyc |
| Skylake | 2048 μops | 6/cyc |
| Golden Cove | 4096 μops | 6–8/cyc |
| Zen 4 (Op Cache) | 6144 μops | 8/cyc |

**检测**：`perf stat -e idq.dsb_uops,idq.mite_uops ./app`（目标 DSB 比例 > 80%）

---

### 第 35–36 章：LSD 与代码对齐

**LSD**：循环体较小且满足条件时，可能更多由 LSD/前端队列供给，从而减少前端取指与解码压力。

**BOLT** 二进制布局优化：
```bash
perf record -e cycles:u -j any,u -o perf.data -- ./app
perf2bolt ./app -p perf.data -o app.fdata
llvm-bolt ./app -o app.bolt -data app.fdata -reorder-blocks=cache+
```

---

## 七、性能计数器与分析篇 (第 37–40 章)

### 第 38 章：Top-Down 分析法 (TMAM)

```
Total Slots
├─ Frontend Bound  (I-Cache Miss / DSB Miss)
├─ Backend Bound
│  ├─ Memory Bound (L1/L2/L3/DRAM Miss)
│  └─ Core Bound  (端口饱和 / 依赖链)
├─ Bad Speculation (分支 Misprediction)
└─ Retiring        (理想状态，目标 >75%)
```

```bash
perf stat --topdown ./app          # Level 1
perf stat --topdown -v ./app       # Level 2
```

---

### 第 39 章：VTune / μProf / LLVM-MCA

| 工具 | 特色 |
|------|------|
| Intel VTune | Microarchitecture Exploration / Memory Access / Threading |
| AMD μProf | IBS 精确采样（AMD 独有）/ L3 Slice 分析 |
| LLVM-MCA | 静态分析理论吞吐，无需运行 |
| perf + FlameGraph | 最通用，火焰图定位热点 |

---

### 第 40 章：优化清单与延迟速查表

#### 延迟速查

| 操作 | 延迟 |
|------|------|
| ADD/XOR | 1 cycle |
| IMUL | 3 cycles |
| **IDIV** | **20–90 cycles** |
| FP ADD/MUL | 4–5 cycles |
| **L1 Cache** | **4–5 cyc (~1ns)** |
| **L2 Cache** | **~12 cyc (~4ns)** |
| **L3 Cache** | **~40 cyc (~12ns)** |
| **DRAM** | **~200 cyc (~65ns)** |
| **Branch Miss** | **15–25 cycles** |
| syscall | ~300 cycles |
| Page Fault | ~10K–50K cycles |

#### 优化优先级

| 优先级 | 优化内容 | 收益 |
|--------|----------|------|
| 最高 | 算法复杂度 O(N²)→O(N logN) | 指数级 |
| 高 | 缓存友好数据结构 | 5–20× |
| 中高 | 分块/预取/对齐 | 2–10× |
| 中 | 分支预测 / ILP / SIMD | 1.5–8× |
| 低 | PGO / BOLT | 1.1–1.3× |

**黄金法则：先测量 (`perf stat`)，再优化。**

---

## 八、扩展专题篇 (第 41–46 章)

### 第 41 章：投机执行安全

- Spectre V1/V2 与 Meltdown 的基本机理
- Retpoline / IBRS / KPTI 的工程权衡
- 缓解收益与性能开销受硬件代际、微码、内核策略影响

### 第 42 章：硬件预取器行为分析

- L1/L2 Streamer 与 Spatial 预取器的触发规律
- 步长、访问模式与预取器覆盖边界
- 软件预取在不规则访问场景的补充价值

### 第 43 章：SIMD 微架构交互

- AVX/AVX2/AVX-512 的寄存器与执行端口关系
- 部分平台上 AVX-512 重负载可能触发频率回退
- 向量宽度、混用策略与吞吐/频率权衡

### 第 44 章：内存带宽与 Roofline

- STREAM/Triad 测量有效带宽
- 算术强度 (AI) 与瓶颈判定
- 内存受限与计算受限的优化路径分离

### 第 45 章：NUMA 深度分析

- 本地/远端内存延迟差异与 first-touch 策略
- `numactl`/`libnuma` 绑定与迁移策略
- 带宽均衡与低延迟绑定的场景取舍

### 第 46 章：编译器微架构感知优化

- `-march`/`-mtune` 的目标与可移植性平衡
- PGO/AutoFDO/BOLT 的端到端流程
- 函数级属性与循环 pragma 的精细化控制

---

## 深入分析与扩展建议

当前 46 章已覆盖 CPU 微架构核心知识图谱。后续可继续深化以下方向：

| 扩展章节 | 主题 | 核心内容 |
|----------|------|----------|
| 第 41 章 | 投机执行安全 | Spectre/Meltdown, LFENCE, Retpoline, KPTI 代价 |
| 第 42 章 | 硬件预取器 | L1/L2 Streamer/Spatial, 步长限制, 手动预取时机 |
| 第 43 章 | SIMD 微架构交互 | AVX-512 频率降档, FMA, 寄存器压力 |
| 第 44 章 | 内存带宽分析 | STREAM benchmark, Roofline Model, 带宽饱和点 |
| 第 45 章 | 多路 NUMA | 跨 socket 延迟, numactl, first-touch 策略 |
| 第 46 章 | 编译器微架构认知 | -march/mtune, PGO 完整流程, BOLT |

---

## 推荐参考资料

- **Agner Fog** — *Optimizing Software in C++*（免费 PDF，最权威）
- **Intel** — *64 and IA-32 Architectures Optimization Reference Manual*
- **Hennessy & Patterson** — *Computer Architecture: A Quantitative Approach*
- [uops.info](https://uops.info) — x86 指令精确延迟/吞吐/端口数据
- [godbolt.org](https://godbolt.org) — 实时编译器输出
- [Brendan Gregg](https://www.brendangregg.com/linuxperf.html) — Linux perf 工具集

---

*参照 test14.cpp（约3579行，46章）编写*
