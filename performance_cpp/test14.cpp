// =============================================================================
// CPU 微架构深度解析 — 缓存层级·分支预测·乱序执行·流水线优化
// =============================================================================
//
// 编译 (Linux):
//   g++ -std=c++17 -O2 -march=native -pthread -o test14 test14.cpp
//
// 编译 (Windows MSVC):
//   cl /std:c++17 /O2 /EHsc /arch:AVX2 test14.cpp
//
// ─────────────────────────────────────────────────────────────
// 目录 (46 章)
// ═══════════════════════════════════════════════════════════════
//
// 一、CPU 流水线基础篇
//   1.  经典 5 级流水线
//   2.  超标量与多发射
//   3.  流水线冒险 (Hazards)
//   4.  流水线气泡与停顿测量
//
// 二、缓存层级篇
//   5.  L1/L2/L3 缓存层级与延迟
//   6.  缓存行 (Cache Line) 详解
//   7.  缓存关联度 (Associativity) 与冲突
//   8.  缓存替换策略 (LRU/PLRU/RRIP)
//   9.  写策略: Write-Back vs Write-Through
//  10.  缓存一致性协议 (MESI/MOESI)
//  11.  False Sharing 深度解析与实测
//  12.  缓存友好的数据结构
//  13.  缓存 Thrashing 与工作集分析
//
// 三、TLB 与虚拟内存篇
//  14.  TLB 层级与 Page Walk
//  15.  大页 (Huge Pages) 对 TLB 的影响
//  16.  PCID 与 TLB 刷新优化
//
// 四、分支预测篇
//  17.  分支预测器原理 (BHT/BTB)
//  18.  静态预测 vs 动态预测
//  19.  两级自适应预测器 (2-bit Counter)
//  20.  TAGE 预测器 (现代架构)
//  21.  间接分支预测 (虚函数/函数指针)
//  22.  分支预测失败代价实测
//  23.  无分支编程技术 (Branchless)
//  24.  分支提示: likely/unlikely/PGO
//
// 五、乱序执行篇
//  25.  乱序执行引擎 (OoO Engine)
//  26.  寄存器重命名与 RAT
//  27.  保留站 (Reservation Station)
//  28.  重排序缓冲区 (ROB)
//  29.  存储缓冲区 (Store Buffer) 与内存消歧
//  30.  执行端口与调度
//  31.  指令级并行 (ILP) 最大化
//  32.  循环携带依赖与优化
//
// 六、前端优化篇
//  33.  指令缓存 (I-Cache) 优化
//  34.  μop Cache (DSB) 与解码器
//  35.  循环流检测器 (LSD)
//  36.  代码对齐与 I-Cache 利用
//
// 七、性能计数器与分析篇
//  37.  硬件性能计数器 (PMU)
//  38.  perf 实战: Top-Down 分析法
//  39.  Intel VTune / AMD μProf
//  40.  微架构优化检查清单
//
// 八、扩展专题篇
//  41.  投机执行安全 (Spectre/Meltdown)
//  42.  硬件预取器行为深度分析
//  43.  SIMD 指令集与微架构交互 (AVX/AVX-512)
//  44.  内存带宽分析与 Roofline 模型
//  45.  多路 NUMA 深度分析
//  46.  编译器对微架构的认知与精细控制
// ═══════════════════════════════════════════════════════════════

#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <deque>
#include <list>
#include <algorithm>
#include <numeric>
#include <memory>
#include <functional>
#include <type_traits>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <new>

#ifdef __linux__
  #include <x86intrin.h>
  #include <sched.h>
  #include <pthread.h>
  #include <unistd.h>
#elif defined(_WIN32)
  #include <intrin.h>
  #include <windows.h>
#endif

// ─── 缓存行大小 ────────────────────────────────
#ifdef __cpp_lib_hardware_interference_size
  static constexpr size_t CACHE_LINE = std::hardware_destructive_interference_size;
#else
  static constexpr size_t CACHE_LINE = 64;
#endif

// ─── 编译器提示 ──────────────────────────────────
#if defined(__GNUC__) || defined(__clang__)
  #define LIKELY(x)      __builtin_expect(!!(x), 1)
  #define UNLIKELY(x)    __builtin_expect(!!(x), 0)
  #define FORCE_INLINE   __attribute__((always_inline)) inline
  #define NOINLINE        __attribute__((noinline))
  #define PREFETCH_R(a)  __builtin_prefetch((a), 0, 3)
  #define PREFETCH_W(a)  __builtin_prefetch((a), 1, 3)
#elif defined(_MSC_VER)
  #define LIKELY(x)      (x)
  #define UNLIKELY(x)    (x)
  #define FORCE_INLINE   __forceinline
  #define NOINLINE        __declspec(noinline)
  #define PREFETCH_R(a)  _mm_prefetch((const char*)(a), _MM_HINT_T0)
  #define PREFETCH_W(a)  _mm_prefetch((const char*)(a), _MM_HINT_T0)
#endif

// ─── rdtsc ─────────────────────────────────────────
FORCE_INLINE uint64_t rdtsc_fence() {
#if defined(__GNUC__) || defined(__clang__)
    uint32_t lo, hi;
    asm volatile("lfence\n\trdtsc" : "=a"(lo), "=d"(hi) :: "memory");
    return (uint64_t)hi << 32 | lo;
#elif defined(_MSC_VER)
    _mm_lfence();
    return __rdtsc();
#endif
}

FORCE_INLINE void pause_cpu() {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_ia32_pause();
#elif defined(_MSC_VER)
    _mm_pause();
#endif
}

// ─── 工具 ──────────────────────────────────────────
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
// ██  一、CPU 流水线基础篇
// =============================================================================

// =====================================================================
// 第 1 章  经典 5 级流水线
// =====================================================================
namespace ch1 {
void demo() {
    print_section("第1章: 经典 5 级流水线");

    std::cout << R"(
  ═══ 经典 RISC 5 级流水线 ═══

  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐
  │  IF  │→│  ID  │→│  EX  │→│  MEM │→│  WB  │
  │ 取指 │ │ 译码 │ │ 执行 │ │访存  │ │ 写回 │
  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘

  IF  (Instruction Fetch)  — 从 I-Cache 取指令
  ID  (Instruction Decode) — 译码 + 读寄存器
  EX  (Execute)            — ALU 计算 / 地址计算
  MEM (Memory Access)      — 访问 D-Cache (Load/Store)
  WB  (Write Back)         — 写结果到寄存器

  理想情况: 每 cycle 完成一条指令 (CPI = 1)

  ═══ 现代 CPU 流水线深度 ═══

  ┌────────────────┬──────────────┬───────────────────────┐
  │ 处理器          │ 流水线级数   │ 特点                  │
  ├────────────────┼──────────────┼───────────────────────┤
  │ Classic RISC    │ 5            │ 教科书模型            │
  │ Pentium 4       │ 31           │ 过深 → 分支惩罚太大   │
  │ Sandy Bridge    │ 14-19        │ 平衡深度与 IPC         │
  │ Zen 4           │ 19           │ AMD 现代架构          │
  │ Golden Cove     │ ~20          │ Intel 12th Gen+        │
  │ Apple M2 (P)    │ ~16          │ 宽发射, 浅流水线       │
  └────────────────┴──────────────┴───────────────────────┘

  流水线更深:
    ✅ 更高主频 (每级工作更少 → 时钟周期更短)
    ❌ 分支预测失败惩罚更大 (~15-25 cycles)
    ❌ 更多前递 (forwarding) 路径

  现代趋势: 适度深度 (14-20) + 超宽发射 (6-8 wide)
)";
}
} // namespace ch1


// =====================================================================
// 第 2 章  超标量与多发射
// =====================================================================
namespace ch2 {
void demo() {
    print_section("第2章: 超标量与多发射");

    std::cout << R"(
  超标量 (Superscalar): 每 cycle 发射多条指令

  ┌──────────────────────────────────────────────────────────┐
  │                  Cycle 1  Cycle 2  Cycle 3  Cycle 4     │
  │ 标量:     inst1     IF      ID      EX      MEM         │
  │           inst2             IF      ID      EX          │
  │           inst3                     IF      ID          │
  │                                                         │
  │ 4-wide:   inst1     IF      ID      EX      MEM         │
  │ 超标量    inst2     IF      ID      EX      MEM         │
  │           inst3     IF      ID      EX      MEM         │
  │           inst4     IF      ID      EX      MEM         │
  │           → 同一拍取+译+执 4 条!                        │
  └──────────────────────────────────────────────────────────┘

  现代 CPU 发射宽度:
  ┌────────────────┬────────────┬──────────────────────┐
  │ 架构            │ 发射宽度   │ 执行端口             │
  ├────────────────┼────────────┼──────────────────────┤
  │ Zen 4           │ 6-wide     │ 4 ALU + 3 LD + 2 ST │
  │ Golden Cove     │ 6-wide     │ 5 ALU + 3 LD + 2 ST │
  │ Apple Firestorm │ 8-wide     │ 6 ALU + 3 LD + 2 ST │
  │ Neoverse V2     │ 10-wide    │ ARM 服务器            │
  └────────────────┴────────────┴──────────────────────┘

  IPC (Instructions Per Cycle):
    理想 IPC = 发射宽度 (如 6)
    实际 IPC ≈ 2-4 (受限于依赖 + 缓存 miss + 分支)

  IPC 限制因素:
    1. 数据依赖 → 无法并行
    2. 缓存 miss → 等待数据 (~100 cycles for DRAM)
    3. 分支预测失败 → 清空流水线
    4. 执行端口冲突 → 端口饱和
)";

    // 实测 IPC: 独立操作 vs 依赖链
    constexpr int N = 100000000;

    // 独立操作: 可以充分利用超标量
    {
        volatile int a=0, b=0, c=0, d=0;
        Timer t;
        for (int i = 0; i < N; ++i) {
            a += i;
            b += i * 2;
            c += i * 3;
            d += i * 4;
        }
        std::cout << "  4 路独立累加 ×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.ms() << " ms (高 IPC)\n";
    }
    // 依赖链: 每步依赖上一步结果
    {
        volatile int a = 0;
        Timer t;
        for (int i = 0; i < N; ++i) {
            a += i;
            a ^= (a >> 3);
            a += (a << 2);
            a ^= (a >> 7);
        }
        std::cout << "  串行依赖链 ×" << N << ": "
                  << std::fixed << std::setprecision(1) << t.ms() << " ms (低 IPC)\n";
    }
}
} // namespace ch2


// =====================================================================
// 第 3 章  流水线冒险
// =====================================================================
namespace ch3 {
void demo() {
    print_section("第3章: 流水线冒险 (Hazards)");

    std::cout << R"(
  流水线冒险 = 阻止下一条指令按计划执行的情况

  ═══ 1. 数据冒险 (Data Hazard) ═══

  RAW (Read After Write) — 最常见:
    ADD R1, R2, R3     ; R1 = R2 + R3
    SUB R4, R1, R5     ; 需要 R1 → 但 R1 还没写回!

  解决: 前递 (Forwarding / Bypassing)
    EX 阶段的结果直接转发给下一条指令, 不等 WB

    ┌──────────────────────────────────────┐
    │  ADD: IF → ID → EX ──┐→ MEM → WB   │
    │                      ↓              │
    │  SUB: IF → ID → EX  (前递得到 R1)   │
    └──────────────────────────────────────┘

  WAR (Write After Read):
    乱序执行中可能出现, 通过寄存器重命名解决

  WAW (Write After Write):
    两条指令写同一寄存器, 顺序不能乱 → 寄存器重命名解决

  ═══ 2. 控制冒险 (Control Hazard) ═══

  分支指令: 需要知道分支方向才能取下一条指令
    BEQ R1, R2, target
    ??? ← 下一条取哪个? 要到 EX 才知道!

  解决: 分支预测 (Branch Prediction)
    预测分支方向 → 推测执行 → 如果错了清空流水线

  清空代价 = 流水线深度: ~15-25 cycles (现代 CPU)

  ═══ 3. 结构冒险 (Structural Hazard) ═══

  两条指令同时需要同一硬件单元:
    - 单端口内存: IF 和 MEM 同时访问 → 冲突
    - 单个除法器: 两条除法指令 → 等待

  解决: 增加硬件资源 (多端口缓存, 多执行单元)
  现代 CPU: 几乎不存在结构冒险 (资源充足)
)";
}
} // namespace ch3


// =====================================================================
// 第 4 章  流水线气泡与停顿测量
// =====================================================================
namespace ch4 {
void demo() {
    print_section("第4章: 流水线气泡与停顿测量");

    // 展示依赖链长度对性能的实际影响
    constexpr int N = 50000000;

    // 依赖链长度 1 (独立)
    {
        volatile int64_t a=1,b=1,c=1,d=1;
        Timer t;
        for (int i = 0; i < N; ++i) { a+=1; b+=1; c+=1; d+=1; }
        double cyc_per_iter = (t.ns() / N);
        std::cout << "  链长1 (4路独立): " << std::fixed << std::setprecision(1)
                  << cyc_per_iter << " ns/iter\n";
    }
    // 依赖链长度 2
    {
        volatile int64_t a=1;
        Timer t;
        for (int i = 0; i < N; ++i) { a = a + 1; a = a + 1; }
        double cyc_per_iter = (t.ns() / N);
        std::cout << "  链长2 (串行):    " << std::fixed << std::setprecision(1)
                  << cyc_per_iter << " ns/iter\n";
    }
    // 依赖链长度 4
    {
        volatile int64_t a=1;
        Timer t;
        for (int i = 0; i < N; ++i) { a=a+1; a=a+1; a=a+1; a=a+1; }
        double cyc_per_iter = (t.ns() / N);
        std::cout << "  链长4 (串行):    " << std::fixed << std::setprecision(1)
                  << cyc_per_iter << " ns/iter\n";
    }

    std::cout << R"(
  观察:
    链长1: 4 路独立 → 4 条指令 1 cycle 完成 ≈ 0.25 cycle/inst
    链长2: 每条等前一条 → 2 cycle / iter
    链长4: 每条等前一条 → 4 cycle / iter

  perf 查看流水线停顿:
    perf stat -e cycles,instructions,stalled-cycles-frontend,
              stalled-cycles-backend ./app

  停顿分类:
    Frontend Stall: I-Cache miss, 译码瓶颈
    Backend Stall:  数据依赖, D-Cache miss, 端口饱和
)";
}
} // namespace ch4


// =============================================================================
// ██  二、缓存层级篇
// =============================================================================

// =====================================================================
// 第 5 章  L1/L2/L3 延迟与带宽
// =====================================================================
namespace ch5 {
void demo() {
    print_section("第5章: L1/L2/L3 缓存延迟实测");

    std::cout << R"(
  ═══ 缓存层级结构 ═══

          ┌──────────┐
          │   Core   │
          │ ┌──────┐ │
          │ │  L1D │ │  32-48 KB, ~4 cycles, ~1 ns
          │ │  L1I │ │  32 KB (指令缓存)
          │ └──┬───┘ │
          │ ┌──┴───┐ │
          │ │  L2   │ │  256KB-1.25MB, ~12 cycles, ~4 ns
          │ └──┬───┘ │
          └────┼─────┘
          ┌────┴─────┐
          │    L3     │  8-96 MB (共享), ~40 cycles, ~12 ns
          └────┬─────┘
          ┌────┴─────┐
          │   DRAM    │  ~200-300 cycles, ~65-100 ns
          └──────────┘

  ═══ 典型参数 (Intel Golden Cove / Zen 4) ═══
  ┌───────┬────────┬──────────┬──────────┬───────────────┐
  │ 层级  │ 大小   │ 延迟     │ 带宽     │ 关联度        │
  ├───────┼────────┼──────────┼──────────┼───────────────┤
  │ L1D   │ 48 KB  │ 4-5 cyc  │ 2×64B/cyc│ 12-way        │
  │ L1I   │ 32 KB  │ —        │ 32B/cyc  │ 8-way         │
  │ L2    │ 1.25MB │ 12 cyc   │ 64B/cyc  │ 10-way        │
  │ L3    │ 30 MB  │ 40-50 cyc│ 32B/cyc  │ 12-16-way     │
  │ DRAM  │ GBs    │ 200+ cyc │ 25GB/s   │ —             │
  └───────┴────────┴──────────┴──────────┴───────────────┘
)";

    // 实测: 不同大小数组的遍历延迟
    auto measure_latency = [](size_t size_kb) -> double {
        size_t count = size_kb * 1024 / sizeof(int);
        if (count < 16) count = 16;
        std::vector<int> data(count);
        // 指针追逐: 构造链表, 步长随机
        // 简化版: 固定步长略过缓存行
        size_t stride = CACHE_LINE / sizeof(int); // 16 ints = 64 bytes
        size_t steps = count / stride;
        if (steps == 0) steps = 1;

        // 预热
        volatile int sink = 0;
        for (size_t i = 0; i < count; i += stride) sink += data[i];

        // 测量
        constexpr int REPS = 10000;
        Timer t;
        for (int r = 0; r < REPS; ++r) {
            for (size_t i = 0; i < count; i += stride) {
                sink += data[i];
            }
        }
        return t.ns() / (REPS * steps);
    };

    struct TestCase { size_t kb; const char* expect; };
    TestCase cases[] = {
        { 4,    "L1" },
        { 32,   "L1" },
        { 64,   "L1/L2边界" },
        { 256,  "L2" },
        { 1024, "L2/L3边界" },
        { 4096, "L3" },
        { 16384,"L3/DRAM边界" },
    };
    for (auto& tc : cases) {
        double ns = measure_latency(tc.kb);
        std::cout << "  " << std::setw(6) << tc.kb << " KB: "
                  << std::fixed << std::setprecision(1) << std::setw(6) << ns
                  << " ns/access  (" << tc.expect << ")\n";
    }
}
} // namespace ch5


// =====================================================================
// 第 6 章  缓存行详解
// =====================================================================
namespace ch6 {
void demo() {
    print_section("第6章: 缓存行 (Cache Line) 详解");

    // 缓存行效应: 访问同一行内不同偏移 vs 跨行
    constexpr int N = 10000000;

    // 连续访问 (每次 +1 int)
    {
        std::vector<int> data(N, 1);
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) sum += data[i];
        std::cout << "  连续访问 (每元素): " << std::fixed << std::setprecision(2)
                  << t.ns() / N << " ns/elem\n";
    }
    // 步长 16 (恰好跳过一个缓存行, 64B/4B=16)
    {
        std::vector<int> data(N, 1);
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; i += 16) sum += data[i];
        std::cout << "  步长=16 (每缓存行1次): " << std::fixed << std::setprecision(2)
                  << t.ns() / (N/16) << " ns/access\n";
    }
    // 步长 256 (每次跳过 1KB, 缓存行冷)
    {
        constexpr int M = N / 4;
        std::vector<int> data(M, 1);
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < M; i += 256) sum += data[i];
        std::cout << "  步长=256 (跨页):       " << std::fixed << std::setprecision(2)
                  << t.ns() / (M/256) << " ns/access\n";
    }

    std::cout << R"(
  ═══ 缓存行结构 ═══

  地址分解 (64B 行, 8-way, 32KB L1):
  ┌──────────────┬──────────┬────────────┐
  │  Tag (高位)  │ Set Index│ Offset(6b) │
  │  确定唯一行  │ 定位组   │ 行内偏移   │
  └──────────────┴──────────┴────────────┘

  32KB = 512 行, 8-way → 64 组 → 6 bits index
  Offset: log2(64) = 6 bits
  Index:  log2(64) = 6 bits
  Tag:    剩余高位

  空间局部性 (Spatial Locality):
    访问 data[0] → 整个缓存行 (data[0..15]) 被加载
    → data[1] 到 data[15] 都是 "免费" 的

  时间局部性 (Temporal Locality):
    最近访问的数据倾向于很快再次被访问 → 留在缓存中

  ⚠️ 步长访问:
    步长 = 缓存行大小 → 最差: 每次都用一行中的一个元素
    步长 > 缓存行大小 → 极差: 空间局部性完全丧失
)";
}
} // namespace ch6


// =====================================================================
// 第 7 章  缓存关联度
// =====================================================================
namespace ch7 {
void demo() {
    print_section("第7章: 缓存关联度 (Associativity)");

    std::cout << R"(
  ═══ 关联度类型 ═══

  1. 直接映射 (Direct-Mapped, 1-way):
     每个地址只能放在一个位置
     ✅ 最简单, 最快查找
     ❌ 冲突率高

  2. 全相联 (Fully Associative):
     任何行可以放在任何位置
     ✅ 冲突率最低
     ❌ 查找最慢 (要比较所有 tag)

  3. 组相联 (Set Associative, N-way):
     每个地址映射到一个组, 组内有 N 个位置
     ✅ 平衡冲突率和查找速度

  ═══ 冲突 Miss 示例 ═══

  假设 L1D: 32KB, 8-way, 64B line → 64 组
  步长 = 32KB / 8 = 4KB → 每 4KB 映射到同一组

  如果你有 9 个间隔 4KB 的数据块:
    block[0], block[4096], block[8192], ... block[32768]
    → 全部映射到同一 set → 8-way 放不下 9 个 → 冲突驱逐!

  这就是 "2的幂步长问题":
    矩阵大小恰好是 2 的幂 → 列访问全冲突 → 性能悬崖
)";

    // 演示: 2 的幂步长 vs 略偏步长
    auto stride_test = [](int stride, const char* label) {
        constexpr int SETS = 256;
        constexpr int REPS = 100000;
        std::vector<char> data(stride * SETS + 64, 0);

        // 预热
        volatile char sink = 0;
        for (int s = 0; s < SETS; ++s) sink += data[s * stride];

        Timer t;
        for (int r = 0; r < REPS; ++r) {
            for (int s = 0; s < SETS; ++s) {
                sink += data[s * stride];
            }
        }
        std::cout << "  步长 " << std::setw(5) << stride
                  << " (" << std::setw(8) << label << "): "
                  << std::fixed << std::setprecision(1)
                  << t.ns() / (REPS * SETS) << " ns/access\n";
    };

    stride_test(4096, "冲突!");
    stride_test(4096 + 64, "错开");
    stride_test(8192, "冲突!");
    stride_test(8192 + 64, "错开");

    std::cout << R"(
  解决冲突 Miss:
    1. 避免 2 的幂大小的数组 → 加 padding
       float matrix[1024][1024];     // ❌ 冲突
       float matrix[1024][1024+16];  // ✅ 错开
    2. 分块访问 (tiling/blocking)
    3. 编译器: -fno-strict-aliasing 偶尔帮助
)";
}
} // namespace ch7


// =====================================================================
// 第 8 章  替换策略
// =====================================================================
namespace ch8 {
void demo() {
    print_section("第8章: 缓存替换策略");

    std::cout << R"(
  ═══ 替换策略对比 ═══

  当缓存组满了、需要换出一行时:

  1. LRU (Least Recently Used):
     ✅ 命中率最优
     ❌ 硬件实现复杂 (8-way 需要记录8!种顺序)
     用于: 小关联度缓存 (L1, 2-4 way)

  2. 伪 LRU (PLRU — Tree-based):
     ✅ 硬件简单 (二叉树决策, 每 way 1 bit)
     ✅ 接近 LRU 效果
     用于: L1 (4-8 way), 大多数现代 CPU

     二叉树 PLRU 示例 (8-way):
              [0]
             /   \
           [1]   [2]
           / \   / \
         [3][4][5][6]
         访问后翻转路径上的bit, 驱逐另一侧

  3. RRIP (Re-Reference Interval Prediction):
     ✅ 适合大容量 (LLC/L3)
     ✅ 抵抗 scanning (大数组遍历不会冲掉热数据)
     用于: Intel L3 (从 Ivy Bridge 开始)

     每行有 2-3 bit RRIP 值:
       高值 = 不太可能再次被访问
       miss 时驱逐 RRIP 最高的行
       hit 时降低 RRIP 值

  4. 随机替换 (Random):
     ✅ 最简单
     ✅ 对某些工作负载和LRU差不多
     用于: 某些 ARM 实现

  ═══ 对编程的影响 ═══

  工作集 > 缓存大小 → 替换策略决定谁被驱逐
  顺序扫描大数组 → RRIP 更好 (不会冲掉频繁访问的数据)
  随机访问 → LRU/PLRU 更好

  Intel: L1/L2 用 PLRU, L3 用 Adaptive RRIP
  AMD:   L1 用 PLRU, L2/L3 用 LRU 变体
)";
}
} // namespace ch8


// =====================================================================
// 第 9 章  写策略
// =====================================================================
namespace ch9 {
void demo() {
    print_section("第9章: 写策略 Write-Back vs Write-Through");

    std::cout << R"(
  ═══ Write-Back (回写) ═══

  写入时:
    1. 只写入缓存 → 标记为 "dirty"
    2. 被驱逐时才写回下一级/内存
  ✅ 减少写流量: 多次写入同一行 → 只回写一次
  ✅ 现代 CPU 默认策略 (L1/L2/L3 都是)

  ═══ Write-Through (直写) ═══

  写入时:
    1. 同时写入缓存和下一级
  ✅ 一致性简单
  ❌ 写流量大
  用于: 某些嵌入式 / GPU L1

  ═══ Write-Allocate (写分配) ═══

  写 miss 时:
    Write-Allocate:     先把行读入缓存, 再写入 (配合 Write-Back)
    No-Write-Allocate:  直接写到下一级, 不读入缓存

  现代 x86: Write-Back + Write-Allocate (默认)

  ═══ 非临时写 (Non-Temporal Store) ═══

  _mm_stream_si128(): 绕过缓存直接写内存
  ✅ 适合: 只写不读的大数据 (写了不会再读 → 不需要缓存)
  ✅ 不污染缓存
  ❌ 通常应尽量写满缓存行 (否则 partial write 常触发 RFO)

  void* p = aligned_alloc(64, size);
  for (int i = 0; i < N; i += 4) {
      __m128i val = _mm_set1_epi32(i);
      _mm_stream_si128((__m128i*)(buf + i), val);
  }
  _mm_sfence();  // 确保 NT store 可见

  Write Combining Buffer (WCB):
    CPU 在写缓冲区中合并多个 NT store → 一次 burst 写出
    Intel: 12 个 WCB entries
)";
}
} // namespace ch9


// =====================================================================
// 第10章  MESI 协议
// =====================================================================
namespace ch10 {
void demo() {
    print_section("第10章: 缓存一致性协议 (MESI/MOESI)");

    std::cout << R"(
  多核系统: 每个核有独立的 L1/L2 → 如何保证缓存一致?

  ═══ MESI 协议 (Intel) ═══

  每个缓存行有 4 种状态:

  M (Modified):  本核独占, 已修改, 与内存不一致
                 其他核无此行
  E (Exclusive): 本核独占, 未修改, 与内存一致
                 其他核无此行
  S (Shared):    多个核都有此行, 未修改
                 与内存一致
  I (Invalid):   无效, 需要重新获取

  状态转换:
  ┌──────┐  Read Hit   ┌──────┐
  │  I   │ ──────────→ │  E   │ (独占读)
  └──────┘             └──────┘
     ↑                    │ 其他核读
     │ 驱逐/失效          ↓
  ┌──────┐             ┌──────┐
  │  M   │ ←── 写入 ── │  S   │
  └──────┘             └──────┘

  ═══ 跨核缓存行转移延迟 ═══

  ┌─────────────────────┬───────────┐
  │ 场景                 │ 延迟      │
  ├─────────────────────┼───────────┤
  │ 同核 L1 hit         │ ~4 cyc    │
  │ 同 CCX/Tile L2 hit  │ ~12 cyc   │
  │ 跨核 (MESI snoop)   │ ~40 cyc   │
  │ 跨 CCD/Die (AMD)    │ ~60 cyc   │
  │ 跨 NUMA 节点         │ ~150 cyc  │
  └─────────────────────┴───────────┘

  核心写同一缓存行 → MESI 乒乓:
    Core A: M → (Core B 想读) → I, 发送数据给 B
    Core B: I → S → M → (Core A 想写) → I
    → 每次写入代价: ~40-70 cycles

  这就是 False Sharing 的根源!

  ═══ MOESI (AMD) ═══
  增加 O (Owned) 状态:
    Modified 但其他核有 Shared 副本
    → 不需要写回内存, 直接核间转发
    ✅ 减少内存带宽消耗
)";
}
} // namespace ch10


// =====================================================================
// 第11章  False Sharing 深度实测
// =====================================================================
namespace ch11 {

// 经典 false sharing 示例
struct NoPad {
    std::atomic<int64_t> a{0};
    std::atomic<int64_t> b{0};
    // a 和 b 在同一个 64B 缓存行中
};

struct WithPad {
    alignas(64) std::atomic<int64_t> a{0};
    alignas(64) std::atomic<int64_t> b{0};
    // a 和 b 各占一个缓存行
};

void demo() {
    print_section("第11章: False Sharing 深度实测");

    constexpr int N = 10000000;

    std::cout << "  sizeof(NoPad):  " << sizeof(NoPad) << " bytes\n";
    std::cout << "  sizeof(WithPad):" << sizeof(WithPad) << " bytes\n";

    // NoPad: 两个线程写同一缓存行的不同变量
    {
        NoPad data;
        Timer t;
        std::thread t1([&]{ for(int i=0;i<N;++i) data.a.fetch_add(1,std::memory_order_relaxed); });
        std::thread t2([&]{ for(int i=0;i<N;++i) data.b.fetch_add(1,std::memory_order_relaxed); });
        t1.join(); t2.join();
        std::cout << "  NoPad  (false sharing): "
                  << std::fixed << std::setprecision(1) << t.ms() << " ms\n";
    }
    // WithPad: 各自的缓存行
    {
        WithPad data;
        Timer t;
        std::thread t1([&]{ for(int i=0;i<N;++i) data.a.fetch_add(1,std::memory_order_relaxed); });
        std::thread t2([&]{ for(int i=0;i<N;++i) data.b.fetch_add(1,std::memory_order_relaxed); });
        t1.join(); t2.join();
        std::cout << "  WithPad (无 false sharing): "
                  << std::fixed << std::setprecision(1) << t.ms() << " ms\n";
    }

    std::cout << R"(
  False Sharing 检测:
    perf c2c record ./app      # 录制缓存行共享事件
    perf c2c report            # 报告共享缓存行

  设计模式:
    1. 线程数据结构: 每个字段 alignas(64)
    2. 线程局部计数器: thread_local → 最终聚合
    3. 分区数据: 每线程独立的数据区域
)";
}
} // namespace ch11


// =====================================================================
// 第12章  缓存友好的数据结构
// =====================================================================
namespace ch12 {

// 链表 vs 数组: 缓存友好性对比
struct ListNode {
    int value;
    ListNode* next;
};

void demo() {
    print_section("第12章: 缓存友好的数据结构");

    constexpr int N = 1000000;

    // 数组遍历
    {
        std::vector<int> arr(N);
        std::iota(arr.begin(), arr.end(), 0);
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) sum += arr[i];
        std::cout << "  vector 遍历: " << std::fixed << std::setprecision(2)
                  << t.ns()/N << " ns/elem\n";
    }

    // std::list 遍历 (随机堆位置)
    {
        std::list<int> lst;
        for (int i = 0; i < N; ++i) lst.push_back(i);
        Timer t;
        volatile int64_t sum = 0;
        for (auto& v : lst) sum += v;
        std::cout << "  list 遍历:   " << std::fixed << std::setprecision(2)
                  << t.ns()/N << " ns/elem\n";
    }

    // std::deque 遍历 (块状连续)
    {
        std::deque<int> dq;
        for (int i = 0; i < N; ++i) dq.push_back(i);
        Timer t;
        volatile int64_t sum = 0;
        for (auto& v : dq) sum += v;
        std::cout << "  deque 遍历:  " << std::fixed << std::setprecision(2)
                  << t.ns()/N << " ns/elem\n";
    }

    std::cout << R"(
  缓存友好程度:
    vector  >>>>  deque  >>>>>>  list / unordered_map

  原因:
    vector: 连续内存 → 硬件预取器完美工作
            每缓存行 16 个 int → 1 次 miss 得 16 个元素
    list:   每节点独立 new → 内存随机分布
            每个元素都可能 cache miss

  缓存友好数据结构设计:
  ┌────────────────┬──────────────────────────────────────┐
  │ 需求            │ 推荐                               │
  ├────────────────┼──────────────────────────────────────┤
  │ 顺序遍历       │ vector / array                      │
  │ 关联容器       │ flat_map (排序 vector)              │
  │ 哈希表         │ open addressing (Robin Hood/Swiss)  │
  │ 树结构         │ B-Tree (多 key/节点 → 缓存行利用)  │
  │ 图             │ CSR (Compressed Sparse Row)         │
  │ 队列           │ ring buffer                         │
  │ 有序集合       │ van Emde Boas / 分层跳表            │
  └────────────────┴──────────────────────────────────────┘

  反模式: 指针追逐 (pointer chasing)
    链表、树、图: 每个节点是独立分配的指针
    → 将节点存储在连续池中, 用索引代替指针
)";
}
} // namespace ch12


// =====================================================================
// 第13章  Cache Thrashing 与工作集
// =====================================================================
namespace ch13 {
void demo() {
    print_section("第13章: Cache Thrashing 与工作集");

    // 工作集大小 vs 缓存大小 → 性能悬崖
    auto bench_working_set = [](size_t kb) -> double {
        size_t count = kb * 1024 / sizeof(int);
        if (count < 64) count = 64;
        std::vector<int> data(count, 1);

        // 随机访问模式
        std::mt19937 rng(42);
        std::vector<size_t> indices(10000);
        for (auto& idx : indices) idx = rng() % count;

        volatile int sink = 0;
        // 预热
        for (auto idx : indices) sink += data[idx];

        Timer t;
        constexpr int REPS = 100;
        for (int r = 0; r < REPS; ++r)
            for (auto idx : indices) sink += data[idx];

        return t.ns() / (REPS * indices.size());
    };

    std::cout << "  工作集大小 → 随机访问延迟:\n";
    size_t sizes[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 32768 };
    for (auto kb : sizes) {
        double ns = bench_working_set(kb);
        std::cout << "    " << std::setw(6) << kb << " KB: "
                  << std::fixed << std::setprecision(1) << std::setw(6) << ns
                  << " ns/access\n";
    }

    std::cout << R"(
  性能悬崖:
    工作集 < L1 (32-48 KB) → ~1ns
    工作集 < L2 (256-1.25MB) → ~4ns
    工作集 < L3 (8-96 MB) → ~12ns
    工作集 > L3 → ~80-100ns  (DRAM)

  Thrashing:
    工作集略大于缓存 → 频繁驱逐+加载 → 性能突然恶化
    尤其在关联度边界: N+1 个映射到同一组的行

  解决:
    1. 减小工作集: 压缩数据结构
    2. 分块处理 (Tiling): 让每块适合缓存
    3. 改善局部性: 重排数据访问顺序
    4. cache-oblivious 算法: 自动适应任意缓存大小
)";
}
} // namespace ch13


// =============================================================================
// ██  三、TLB 与虚拟内存篇
// =============================================================================

// =====================================================================
// 第14章  TLB 层级与 Page Walk
// =====================================================================
namespace ch14 {
void demo() {
    print_section("第14章: TLB 层级与 Page Walk");

    std::cout << R"(
  ═══ 4级页表结构 (x86-64) ═══

  虚拟地址 (48 bit):
  ┌────────┬─────────┬─────────┬─────────┬──────────────┐
  │ PML4   │ PDPT    │ PD      │ PT      │ Page Offset  │
  │ 9 bits │ 9 bits  │ 9 bits  │ 9 bits  │ 12 bits      │
  └────────┴─────────┴─────────┴─────────┴──────────────┘

  Page Walk: 虚拟地址 → 物理地址 (4 次内存访问)
    1. PML4 表 → 找到 PDPT
    2. PDPT 表 → 找到 PD
    3. PD 表   → 找到 PT
    4. PT 表   → 找到物理页帧

  每次内存访问都需要地址翻译 → 太昂贵!
  TLB (Translation Lookaside Buffer) = 地址翻译缓存

  ═══ TLB 层级 ═══

  ┌───────────┬──────────┬──────────┬──────────┐
  │ TLB       │ 容量     │ 延迟     │ 关联度   │
  ├───────────┼──────────┼──────────┼──────────┤
  │ L1 DTLB   │ 64-72    │ 1 cyc    │ 全相联   │
  │ L1 ITLB   │ 128      │ 1 cyc    │ 8-way    │
  │ L2 STLB   │ 1536-2K  │ 6-8 cyc  │ 12-way   │
  └───────────┴──────────┴──────────┴──────────┘

  TLB miss → Page Walk → ~50-100 cycles
  有 Page Walk Cache (PMH): 缓存中间页表项

  ═══ TLB 覆盖范围 ═══

  4KB 页: 1536 entries × 4KB = ~6 MB
  2MB 页: 1536 entries × 2MB = ~3 GB
  1GB 页: 4 entries × 1GB   = ~4 GB

  → 工作集 > 6MB 且随机访问 → TLB miss 频繁

  ═══ Page Walk 优化 ═══

  1. 大页 (2MB/1GB): 覆盖范围增大 512x/262144x
  2. PCID: 避免上下文切换时刷新 TLB
  3. PGE (Page Global Enable): 内核页常驻 TLB
  4. Page Walk Cache: 缓存中间级页表
  5. 减少 VMA 数量: 合并内存映射区域
)";
}
} // namespace ch14


// =====================================================================
// 第15章  大页对 TLB 的影响
// =====================================================================
namespace ch15 {
void demo() {
    print_section("第15章: 大页对 TLB 的影响");

    // 模拟 TLB miss: 大步长随机访问大块内存
    auto tlb_bench = [](size_t total_mb, size_t stride_kb) -> double {
        size_t total = total_mb * 1024 * 1024;
        size_t stride = stride_kb * 1024;
        size_t count = total / stride;
        if (count == 0) return 0;

        std::vector<char> data(total, 0);
        volatile char sink = 0;

        // 预热
        for (size_t i = 0; i < total; i += stride) sink += data[i];

        Timer t;
        constexpr int REPS = 200;
        for (int r = 0; r < REPS; ++r)
            for (size_t i = 0; i < total; i += stride)
                sink += data[i];

        return t.ns() / (REPS * count);
    };

    std::cout << "  在不同内存大小下, 每页访问一次的延迟:\n";
    std::cout << "  (步长=4KB, 每次触摸不同的页)\n";

    size_t mbs[] = { 1, 4, 8, 16, 64, 256 };
    for (auto mb : mbs) {
        double ns = tlb_bench(mb, 4);
        size_t pages = mb * 256; // mb * 1M / 4K
        std::cout << "    " << std::setw(4) << mb << " MB ("
                  << std::setw(6) << pages << " pages): "
                  << std::fixed << std::setprecision(1) << ns << " ns/access\n";
    }

    std::cout << R"(
  结果分析:
    ≤6 MB: STLB 能覆盖 (~1536 页 × 4KB = 6MB) → TLB hit
    >6 MB: TLB miss 增多 → Page Walk → 延迟上升

  大页效果:
    2MB 页: 覆盖 1536 × 2MB = 3 GB → 几乎不会 TLB miss
    1GB 页: 覆盖 4 × 1GB = 4 GB

  使用场景:
    数据库缓冲池 (InnoDB buffer pool)
    交易系统的行情/订单数据
    大规模科学计算的矩阵

  配置:
    echo 1024 > /proc/sys/vm/nr_hugepages
    mmap(MAP_HUGETLB) 或 libhugetlbfs
)";
}
} // namespace ch15


// =====================================================================
// 第16章  PCID
// =====================================================================
namespace ch16 {
void demo() {
    print_section("第16章: PCID 与 TLB 刷新优化");

    std::cout << R"(
  ═══ 问题: 上下文切换与 TLB 刷新 ═══

  传统:
    进程切换 → 加载新 CR3 (页表基址) → 全部 TLB 失效
    → 新进程启动时大量 TLB miss → 高延迟

  ═══ PCID (Process-Context Identifier) ═══

  每个 TLB 条目附加 12-bit PCID (4096 个)
  切换 CR3 时:
    旧: 全部刷新 TLB
    新: 只查找 PCID 匹配的条目 → 旧条目保留

  效果:
    上下文切换后: TLB 仍然有上次运行时的条目
    → 减少 cold-start TLB miss
    → 上下文切换延迟降低 30-50%

  Linux 自 4.15 起启用 PCID (also for Meltdown: KPTI)

  ═══ INVPCID 指令 ═══

  精细控制 TLB 刷新:
    INVPCID 0: 刷新特定 PCID 的特定地址
    INVPCID 1: 刷新特定 PCID 的所有条目
    INVPCID 2: 刷新所有 PCID 的所有条目
    INVPCID 3: 刷新除 Global 外的所有条目

  ═══ 对低延迟的影响 ═══

  场景: 交易系统进程切换
    无 PCID: 切换后 ~10-50μs TLB 重建
    有 PCID: 切换后 TLB 仍然"温暖" → ~1-5μs

  最佳实践:
    1. 尽量避免上下文切换 (绑核 + isolcpus)
    2. 内核启用 PCID (默认已启用)
    3. 减少 KPTI 开销: 如果没有侧信道攻击风险 → nopti
)";
}
} // namespace ch16


// =============================================================================
// ██  四、分支预测篇
// =============================================================================

// =====================================================================
// 第17章  分支预测器原理
// =====================================================================
namespace ch17 {
void demo() {
    print_section("第17章: 分支预测器原理");

    std::cout << R"(
  ═══ 为什么需要分支预测 ═══

  流水线中:
    IF  ID  EX  MEM  WB
        IF  ID  EX   MEM  WB
            IF  ID   EX   MEM  WB
               BEQ ← 分支方向在 EX 才确定!
                IF  ← 取哪条指令?

  不预测: 等到分支解析 → 白白浪费 ~15-25 cycles
  预测:   猜一个方向 → 推测执行 → 90-99% 对了 → 流水线满载

  ═══ 分支预测器组件 ═══

  1. BHT (Branch History Table / PHT):
     预测分支方向 (taken / not-taken)
     用分支 PC 的低位索引

  2. BTB (Branch Target Buffer):
     缓存分支目标地址
     命中 → 可以在 IF 阶段就开始取目标指令

  3. RAS (Return Address Stack):
     专门预测函数返回地址 (CALL/RET)
     CALL → 压栈; RET → 弹栈
      准确率通常很高 (深递归或异常控制流会降低命中)

  4. Loop Predictor:
     识别循环分支 (N 次 taken, 1 次 not-taken)
     记录循环计数 → 预测精确的退出点

  ═══ 分支预测器容量 ═══

  ┌────────────────┬────────────────────┐
  │ 组件            │ 典型大小           │
  ├────────────────┼────────────────────┤
  │ BTB             │ 4096-12288 entries │
  │ BHT/PHT        │ ~16K-64K entries   │
  │ RAS             │ 16-32 entries      │
  │ Loop Predictor  │ 64-256 entries     │
  └────────────────┴────────────────────┘
)";
}
} // namespace ch17


// =====================================================================
// 第18章  静态预测 vs 动态预测
// =====================================================================
namespace ch18 {
void demo() {
    print_section("第18章: 静态预测 vs 动态预测");

    std::cout << R"(
  ═══ 静态预测 (编译器/硬件固定规则) ═══

  1. 向后跳转 → 预测 taken (循环)
     for (int i = 0; i < N; ++i)  ← 向后跳转
       → 预测: 继续循环 (usually correct)

  2. 向前跳转 → 预测 not-taken (if 分支)
     if (error) goto handle;    ← 向前跳转
       → 预测: 不跳转 (error is rare)

  3. 编译器提示:
     [[likely]]   → 编译器排列代码使 likely 路径是 fall-through
     [[unlikely]] → 编译器把 unlikely 代码移到远处

  ═══ 动态预测 (运行时统计) ═══

  1. 1-bit 预测器:
     上次 taken → 预测 taken; 上次 not-taken → 预测 not-taken
     缺点: 循环末尾翻转 → 下次循环开头也预测错

  2. 2-bit 饱和计数器:
     4 个状态: 强不跳转(00) → 弱不跳转(01) → 弱跳转(10) → 强跳转(11)
     需要连续两次预测错才翻转 → 循环末尾不会立即翻转

     ┌─────────┐ not-taken  ┌─────────┐ not-taken  ┌─────────┐
     │ 强跳转  │ ─────────→ │ 弱跳转  │ ─────────→ │ 弱不跳  │
     │  (11)   │            │  (10)   │            │  (01)   │
     │         │ ←───────── │         │ ←───────── │         │
     └─────────┘   taken    └─────────┘   taken    └─────────┘
                                                       │ not-taken
                                                       ↓
                                                   ┌─────────┐
                                                   │ 强不跳  │
                                                   │  (00)   │
                                                   └─────────┘
)";
}
} // namespace ch18


// =====================================================================
// 第19章  两级自适应预测器
// =====================================================================
namespace ch19 {
void demo() {
    print_section("第19章: 两级自适应预测器");

    std::cout << R"(
  ═══ 两级预测器 (Correlated / Two-Level) ═══

  基本 2-bit 计数器的局限:
    分支模式 TNTNTNT (交替) → 2-bit 计数器全错!
    因为它只看"上一次结果", 不看"序列模式"

  两级预测器:
    第一级: 全局/局部历史寄存器 (BHR)
            记录最近 N 次分支的 taken/not-taken 序列
    第二级: 模式历史表 (PHT)
            用历史序列作为索引, 每个条目是 2-bit 计数器

  ┌─────────────┐
  │ BHR (8-bit) │ = 10110101 (最近 8 次分支结果)
  └──────┬──────┘
         │ 作为索引
         ↓
  ┌──────────────────────────────────────┐
  │ PHT[256]  (每个 2-bit saturating)    │
  │ [00] [01] [10] ... [10110101] → 11   │ ← 预测: taken
  └──────────────────────────────────────┘

  变体:
    GAp: 全局历史, 每分支一个 PHT
    GAg: 全局历史, 全局 PHT
    PAp: 每分支历史, 每分支 PHT  (开销大)
    PAg: 每分支历史, 全局 PHT

  ═══ GShare 预测器 ═══

  改进: 全局历史 XOR 分支 PC → 作为 PHT 索引
  → 减少 aliasing (不同分支不会共享同一 PHT 条目)

    index = BHR ⊕ PC[low bits]
    PHT[index] → 2-bit 计数器 → 预测

  效果: 95-97% 准确率 (一般程序)
)";
}
} // namespace ch19


// =====================================================================
// 第20章  TAGE 预测器
// =====================================================================
namespace ch20 {
void demo() {
    print_section("第20章: TAGE 预测器 (现代架构)");

    std::cout << R"(
  ═══ TAGE: TAgged GEometric history length ═══

  现代 CPU (Intel, AMD, ARM) 使用的最先进预测器

  核心思想:
    多个表, 每个表使用不同长度的全局历史
    历史长度按几何级数增长: 2, 4, 8, 16, 32, 64, 128, ...

  结构:
  ┌──────────┐
  │ 基础表 T0│ (无历史, 用 2-bit 计数器)
  └──────────┘
  ┌──────────┐
  │ T1       │ 历史长度 = 2    index = hash(PC, H[0:2])
  └──────────┘
  ┌──────────┐
  │ T2       │ 历史长度 = 4    index = hash(PC, H[0:4])
  └──────────┘
  ┌──────────┐
  │ T3       │ 历史长度 = 8    index = hash(PC, H[0:8])
  └──────────┘
      ...
  ┌──────────┐
  │ T7       │ 历史长度 = 128  index = hash(PC, H[0:128])
  └──────────┘

  预测过程:
    1. 并行查找所有表
    2. 找到 tag 匹配的最长历史表 → 使用其预测
    3. 如果没有任何表匹配 → 使用基础表 T0

  关键属性:
    - 几何级历史长度: 捕捉短期和长期模式
    - Tagged: 每个条目有 tag → 减少 aliasing
    - 动态适应: 统计有用性 → 自动分配表项

  准确率: ~97-99% (SPEC benchmarks)

  ═══ 现代 CPU 分支预测器 ═══

  ┌────────────────┬──────────────────────────────┐
  │ CPU            │ 预测器                       │
  ├────────────────┼──────────────────────────────┤
  │ Intel (Willow  │ TAGE + 循环预测 +             │
  │ Cove / Golden) │ 间接预测 + 统计修正           │
  │                │ ~96-99% 准确                  │
  │                │                              │
  │ AMD Zen 4      │ TAGE-like                    │
  │                │ + Perceptron (机器学习)       │
  │                │ 大历史深度                    │
  │                │                              │
  │ Apple M2       │ TAGE + 大 BTB (>8K)          │
  │                │ 极大预测器面积                 │
  └────────────────┴──────────────────────────────┘
)";
}
} // namespace ch20


// =====================================================================
// 第21章  间接分支预测
// =====================================================================
namespace ch21 {

struct Base {
    virtual int work(int x) = 0;
    virtual ~Base() = default;
};
struct DerivedA : Base { int work(int x) override { return x + 1; } };
struct DerivedB : Base { int work(int x) override { return x * 2; } };
struct DerivedC : Base { int work(int x) override { return x ^ 37; } };

void demo() {
    print_section("第21章: 间接分支预测 (虚函数)");

    constexpr int N = 5000000;

    // 单态 (monomorphic): 虚函数总是同一类型
    {
        std::vector<std::unique_ptr<Base>> objs;
        for (int i = 0; i < 64; ++i) objs.push_back(std::make_unique<DerivedA>());
        Timer t;
        volatile int sum = 0;
        for (int i = 0; i < N; ++i) sum += objs[i % 64]->work(i);
        std::cout << "  单态虚调用: " << std::fixed << std::setprecision(2)
                  << t.ns() / N << " ns/call\n";
    }

    // 多态 (polymorphic): 随机类型
    {
        std::vector<std::unique_ptr<Base>> objs;
        std::mt19937 rng(42);
        for (int i = 0; i < 64; ++i) {
            switch (rng() % 3) {
                case 0: objs.push_back(std::make_unique<DerivedA>()); break;
                case 1: objs.push_back(std::make_unique<DerivedB>()); break;
                case 2: objs.push_back(std::make_unique<DerivedC>()); break;
            }
        }
        Timer t;
        volatile int sum = 0;
        for (int i = 0; i < N; ++i) sum += objs[i % 64]->work(i);
        std::cout << "  多态虚调用 (3种): " << std::fixed << std::setprecision(2)
                  << t.ns() / N << " ns/call\n";
    }

    std::cout << R"(
  间接分支 = 目标地址在运行时才确定
    虚函数调用: call [vtable + offset]
    函数指针:   call [reg]
    switch:     jmp [table + idx]

  间接分支预测器 (ITTAGE):
    类似 TAGE, 但预测的是目标地址而不是方向
    用全局历史 + PC → 索引表 → 得到预测的目标地址

  预测准确率:
    单态 (monomorphic): ~99%  一个callsite总是调同一个类
    少态 (megamorphic): ~90%  2-3 种类型, 有规律
    多态 (polymorphic): ~50-70%  随机类型 → 预测困难

  优化:
    1. 避免随机多态: 按类型分组处理
       for (auto& a : type_A) a.work();
       for (auto& b : type_B) b.work();
    2. 去虚化 (devirtualization):
       CRTP / if constexpr / std::variant
    3. LTO/PGO: 编译器可以去虚化单态调用点
)";
}
} // namespace ch21


// =====================================================================
// 第22章  分支预测失败代价实测
// =====================================================================
namespace ch22 {
void demo() {
    print_section("第22章: 分支预测失败代价实测");

    constexpr int N = 10000000;

    // 准备数据
    std::vector<int> data(N);
    std::mt19937 rng(42);
    for (auto& v : data) v = rng() % 256;

    // 排序后的数据: 分支高度可预测
    auto sorted = data;
    std::sort(sorted.begin(), sorted.end());

    // 未排序: 分支不可预测
    auto test = [&](const std::vector<int>& arr, const char* label) {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            if (arr[i] >= 128)  // ~50% taken
                sum += arr[i];
        }
        std::cout << "  " << label << ": " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms (" << t.ns()/N << " ns/iter)\n";
    };

    test(sorted, "排序数据 (可预测)  ");
    test(data,   "随机数据 (不可预测)");

    // 无分支版本
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            int mask = -(data[i] >= 128); // true→-1, false→0
            sum += data[i] & mask;
        }
        std::cout << "  无分支版本 (随机)  : " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms (" << t.ns()/N << " ns/iter)\n";
    }

    std::cout << R"(
  分析:
    排序数据: 前半全 <128 (not-taken), 后半全 >=128 (taken)
              → 仅边界附近会出现少量误预测，整体命中率通常较高

    随机数据: ~50% 概率 → 接近随机猜测
              → 大量 misprediction → 每次代价 ~15-25 cycles

    无分支: 用位运算消除分支 → 无预测失败

  perf 查看:
    perf stat -e branches,branch-misses ./app
    分支 miss rate: 排序 <0.1%, 随机 ~25%
)";
}
} // namespace ch22


// =====================================================================
// 第23章  无分支编程技术
// =====================================================================
namespace ch23 {

// CMOV 等价操作
FORCE_INLINE int branchless_abs(int x) {
    int mask = x >> 31;
    return (x + mask) ^ mask;
}

FORCE_INLINE int branchless_min(int a, int b) {
    return b + ((a - b) & ((a - b) >> 31));
}

FORCE_INLINE int branchless_max(int a, int b) {
    return a - ((a - b) & ((a - b) >> 31));
}

// 无分支条件选择
FORCE_INLINE int branchless_select(bool cond, int a, int b) {
    return b ^ ((a ^ b) & -static_cast<int>(cond));
}

// 无分支 clamp
FORCE_INLINE int branchless_clamp(int x, int lo, int hi) {
    return branchless_min(branchless_max(x, lo), hi);
}

// 无分支符号函数
FORCE_INLINE int branchless_sign(int x) {
    return (x > 0) - (x < 0);
}

void demo() {
    print_section("第23章: 无分支编程技术");

    // 验证
    assert(branchless_abs(-42) == 42);
    assert(branchless_abs(7) == 7);
    assert(branchless_min(3, 7) == 3);
    assert(branchless_max(3, 7) == 7);
    assert(branchless_select(true, 10, 20) == 10);
    assert(branchless_select(false, 10, 20) == 20);
    assert(branchless_clamp(50, 0, 100) == 50);
    assert(branchless_clamp(-10, 0, 100) == 0);
    assert(branchless_clamp(200, 0, 100) == 100);
    assert(branchless_sign(-5) == -1);
    assert(branchless_sign(0) == 0);
    assert(branchless_sign(5) == 1);
    std::cout << "  所有 branchless 函数验证通过 ✓\n";

    // 性能对比: 条件累加
    constexpr int N = 10000000;
    std::vector<int> data(N);
    std::mt19937 rng(42);
    for (auto& v : data) v = rng() % 256;

    // 有分支
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            if (data[i] > 128) sum += data[i];
            else sum -= data[i];
        }
        std::cout << "  分支版本:   " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }
    // 无分支
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            int mask = -(data[i] > 128); // true → -1 (all 1s), false → 0
            int val = data[i];
            sum += (val & mask) - (val & ~mask);
        }
        std::cout << "  无分支版本: " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }

    std::cout << R"(
  无分支技巧汇总:
  ┌──────────────────┬──────────────────────────────────┐
  │ 需求              │ 无分支实现                       │
  ├──────────────────┼──────────────────────────────────┤
  │ abs(x)           │ mask=x>>31; (x+mask)^mask        │
  │ min(a,b)         │ b+((a-b)&((a-b)>>31))           │
  │ max(a,b)         │ a-((a-b)&((a-b)>>31))           │
  │ clamp(x,lo,hi)  │ min(max(x,lo),hi)               │
  │ select(c,a,b)   │ b^((a^b)&-int(c))               │
  │ sign(x)         │ (x>0)-(x<0)                      │
  │ cond add         │ sum += val & -(val>thresh)       │
  └──────────────────┴──────────────────────────────────┘

  什么时候用无分支:
    ✅ 分支不可预测 (>5% miss rate)
    ✅ 数据驱动的分支 (不是控制流)
    ❌ 分支高度可预测 → 不需要
    ❌ 分支两侧代码量差异大 → likely/unlikely 更好
)";
}
} // namespace ch23


// =====================================================================
// 第24章  分支提示
// =====================================================================
namespace ch24 {
void demo() {
    print_section("第24章: 分支提示 likely/unlikely/PGO");

    constexpr int N = 50000000;
    std::vector<int> data(N, 100);
    // 99.99% > 0, 极少 <= 0
    data[N/2] = -1;
    data[N/3] = -1;

    // 无提示
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            if (data[i] > 0) sum += data[i]; // 大多数 taken
            else sum -= 1;
        }
        std::cout << "  无提示:     " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }
    // 有提示
    {
        Timer t;
        volatile int64_t sum = 0;
        for (int i = 0; i < N; ++i) {
            if (LIKELY(data[i] > 0)) sum += data[i];
            else sum -= 1;
        }
        std::cout << "  LIKELY提示: " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }

    std::cout << R"(
  编译器分支提示:

  1. __builtin_expect (GCC/Clang):
     if (__builtin_expect(error, 0))   // error 预期为 0 (rare)
     if (__builtin_expect(success, 1)) // success 预期为 1 (common)

  2. C++20 属性:
     if (x > 0) [[likely]] { ... }
     else [[unlikely]] { ... }

  3. MSVC:
     无直接等价, 但 PGO 效果更好

  编译器做了什么:
    - 将 likely 路径安排为 fall-through (不跳转)
    - 将 unlikely 代码移到距离较远的位置
    - 不是把 likely 放到"最前面", 而是让 likely 是不跳转路径

  PGO (Profile-Guided Optimization):
    比手动提示更好:
    - 每个分支的精确概率 (不是 binary likely/unlikely)
    - 编译器自动排列代码布局
    - 内联决策基于真实调用频率

  PGO 流程:
    g++ -O2 -fprofile-generate -o app app.cpp
    ./app < real_data            # 收集 profile
    g++ -O2 -fprofile-use -o app app.cpp   # 用 profile 优化
)";
}
} // namespace ch24


// =============================================================================
// ██  五、乱序执行篇
// =============================================================================

// =====================================================================
// 第25章  乱序执行引擎
// =====================================================================
namespace ch25 {
void demo() {
    print_section("第25章: 乱序执行引擎 (OoO Engine)");

    std::cout << R"(
  ═══ 乱序执行: 为什么需要? ═══

  顺序执行:
    LOAD  R1, [mem]    ; ~100 cycles (cache miss!)
    ADD   R2, R1, R3   ; 等待 R1 → 停顿 100 cycles!
    MUL   R5, R6, R7   ; 独立 → 但也要等! → 浪费!

  乱序执行:
    LOAD  R1, [mem]    ; 发射
    MUL   R5, R6, R7   ; R5/R6/R7 和 R1 无关 → 先执行!
    ADD   R2, R1, R3   ; R1 就绪后再执行

  ═══ 乱序执行流水线 ═══

  ┌──────┐ ┌──────┐ ┌──────────┐ ┌────────────────────────┐ ┌──────┐
  │  IF  │→│  ID  │→│ Rename/  │→│   OoO 执行引擎          │→│Retire│
  │ 取指 │ │ 译码 │ │ Allocate │ │ ┌───────────────────┐   │ │ 提交 │
  └──────┘ └──────┘ │          │ │ │ Reservation Station│   │ └──────┘
                    │ 寄存器   │ │ │ (调度器)           │   │
                    │ 重命名   │ │ └─────────┬─────────┘   │
                    │          │ │           ↓             │
                    │ 分配ROB  │ │ ┌─────────────────────┐ │
                    │          │ │ │   执行单元 (EU)      │ │
                    └──────────┘ │ │ ALU ALU ALU LD ST ..│ │
                                 │ └─────────────────────┘ │
                                 │           ↓             │
                                 │ ┌─────────────────────┐ │
                                 │ │   ROB (重排序缓冲)   │ │
                                 │ │   按程序顺序提交     │ │
                                 │ └─────────────────────┘ │
                                 └────────────────────────┘

  核心组件:
    1. 寄存器重命名 → 消除 WAR/WAW 依赖
    2. Reservation Station → 操作数就绪时发射
    3. 执行单元 → 并行执行
    4. ROB → 维护程序顺序 (异常/中断精确)

  ═══ 乱序窗口大小 ═══

  ┌────────────────┬────────┬──────────────────┐
  │ 架构            │ ROB    │ 意义              │
  ├────────────────┼────────┼──────────────────┤
  │ Zen 4           │ 320    │ 可容纳 320 条指令 │
  │ Golden Cove     │ 512    │ Intel 12th Gen    │
  │ Firestorm (M2)  │ ~630   │ Apple, 最大        │
  │ Cortex-A710     │ 160    │ ARM 中端          │
  └────────────────┴────────┴──────────────────┘

  ROB 大小 = 乱序窗口 = 能"绕过"多少条指令去找独立工作
  → ROB 越大 → 越能容忍长延迟操作 (如 cache miss)
)";
}
} // namespace ch25


// =====================================================================
// 第26章  寄存器重命名
// =====================================================================
namespace ch26 {
void demo() {
    print_section("第26章: 寄存器重命名与 RAT");

    std::cout << R"(
  ═══ 为什么需要寄存器重命名 ═══

  程序:
    MUL R1, R2, R3     ; R1 = R2 × R3     ①
    ADD R4, R1, R5     ; R4 = R1 + R5     ② (RAW on R1)
    MOV R1, R6         ; R1 = R6          ③ (WAW on R1!, WAR on R1!)
    SUB R7, R1, R8     ; R7 = R1 - R8     ④

  名字依赖 (false dependency):
    ③ 和 ① 写同一个 R1 → WAW (但完全无关!)
    ③ 和 ② 都用 R1 → WAR (但 ② 读的是 ① 的结果)

  → ③ 必须等 ② 完成才能写 R1? NO! 用重命名!

  ═══ 重命名后 ═══

  RAT (Register Alias Table) 映射:
    架构寄存器 → 物理寄存器

  MUL P17, P2, P3    ; P17 = P2 × P3     R1 → P17
  ADD P18, P17, P5   ; P18 = P17 + P5    R4 → P18
  MOV P19, P6        ; P19 = P6          R1 → P19 (新的!)
  SUB P20, P19, P8   ; P20 = P19 - P8    R7 → P20

  现在:
    ① 和 ③ 完全独立! (P17 vs P19, 不同物理寄存器)
    → 可以并行执行!

  ═══ 物理寄存器数量 ═══

  ┌────────────────┬────────────┬──────────────┐
  │ 架构            │ 架构寄存器 │ 物理寄存器   │
  ├────────────────┼────────────┼──────────────┤
  │ x86-64 (GP)    │ 16         │ ~280 (Zen4)  │
  │ x86-64 (AVX)   │ 16 (32)   │ ~256          │
  │ ARM/AArch64    │ 31         │ ~384          │
  └────────────────┴────────────┴──────────────┘

  物理寄存器 >> 架构寄存器 → 大量并行度

  ═══ Move Elimination ═══

  MOV R1, R2
  → 不生成 μop, 只在 RAT 中修改映射: R1 → R2 的物理寄存器
  → 零延迟, 零执行资源!

  Intel: MOV 和某些 zeroing 操作 (XOR R, R) 被消除
)";

    // 展示寄存器重命名的效果
    constexpr int N = 100000000;

    // 使用不同的变量 (编译器帮助利用多物理寄存器)
    {
        volatile int a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0;
        Timer t;
        for (int i = 0; i < N; ++i) {
            a+=i; b+=i; c+=i; d+=i;
            e+=i; f+=i; g+=i; h+=i;
        }
        std::cout << "  8 路独立寄存器: " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }
    // 重用同一个变量 (强制串行)
    {
        volatile int a=0;
        Timer t;
        for (int i = 0; i < N; ++i) {
            a+=i; a+=i; a+=i; a+=i;
            a+=i; a+=i; a+=i; a+=i;
        }
        std::cout << "  1 变量串行×8:   " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }
}
} // namespace ch26


// =====================================================================
// 第27章  保留站
// =====================================================================
namespace ch27 {
void demo() {
    print_section("第27章: 保留站 (Reservation Station)");

    std::cout << R"(
  ═══ 保留站 (RS) / 调度器 (Scheduler) ═══

  寄存器重命名后, 指令进入保留站等待执行

  保留站的工作:
    1. 监听执行单元的结果总线
    2. 当一条指令的所有操作数就绪 → 标记为"可发射"
    3. 选择可发射的指令 → 发射到执行单元

  结构:
  ┌─────────────────────────────────────────────┐
  │ Reservation Station                          │
  │ ┌────┬────┬────┬────┬────┬─────────────────┐ │
  │ │ Op │ S1 │ Rdy│ S2 │ Rdy│ Dest            │ │
  │ ├────┼────┼────┼────┼────┼─────────────────┤ │
  │ │ADD │ P17│ ✗  │ P5 │ ✓  │ P18             │ │ ← 等 P17
  │ │MUL │ P2 │ ✓  │ P3 │ ✓  │ P17             │ │ ← 可发射!
  │ │SUB │ P19│ ✗  │ P8 │ ✓  │ P20             │ │ ← 等 P19
  │ │MOV │ P6 │ ✓  │ — │ — │ P19              │ │ ← 可发射!
  │ └────┴────┴────┴────┴────┴─────────────────┘ │
  └─────────────────────────────────────────────┘
       当 MUL 完成 → P17 就绪 → ADD 可发射
       当 MOV 完成 → P19 就绪 → SUB 可发射

  ═══ RS 容量 ═══

  ┌────────────────┬────────┐
  │ 架构            │ RS 大小│
  ├────────────────┼────────┤
  │ Zen 4 (INT)    │ 96     │
  │ Zen 4 (FP)     │ 64     │
  │ Golden Cove    │ 160+   │
  │ Firestorm      │ ~330   │
  └────────────────┴────────┘

  RS 满了 → 前端停顿 → 新指令无法进入
  → 要么 ROB 限制, 要么 RS 限制, 要么执行端口限制

  ═══ 统一 RS vs 分布式 RS ═══

  Intel: 统一调度器 (所有类型指令共享 RS)
  AMD:   分布式 (INT RS + FP RS 独立)
  各有优劣: 统一更灵活, 分布式更简单
)";
}
} // namespace ch27


// =====================================================================
// 第28章  重排序缓冲区 (ROB)
// =====================================================================
namespace ch28 {
void demo() {
    print_section("第28章: 重排序缓冲区 (ROB)");

    std::cout << R"(
  ═══ ROB 的作用 ═══

  乱序执行 → 指令完成顺序 ≠ 程序顺序
  但必须保证:
    1. 异常精确: 异常前的指令都已提交, 之后的都没有
    2. 中断精确: 同上
    3. 可见顺序: 内存操作按需呈现给其他核

  ROB = 环形缓冲区, 按程序顺序存储指令

  ┌────────────────────────────────────────┐
  │ ROB (环形)                             │
  │ ┌────┬────────────┬─────┬────────────┐ │
  │ │ #  │ 指令        │ 状态│ 结果       │ │
  │ ├────┼────────────┼─────┼────────────┤ │
  │ │ 0  │ MUL P17,.. │ ✓完成│ 42        │ │ ← 可退役 (头)
  │ │ 1  │ ADD P18,.. │ ✓完成│ 47        │ │ ← 可退役
  │ │ 2  │ LOAD P19,..│ ✗执行中│ —       │ │ ← 阻塞退役!
  │ │ 3  │ SUB P20,.. │ ✓完成│ 10        │ │ ← 等 #2
  │ │ 4  │ ...        │     │           │ │
  │ └────┴────────────┴─────┴────────────┘ │
  │  head →                    ← tail      │
  └────────────────────────────────────────┘

  退役 (Retire/Commit):
    从头部开始, 按顺序退役已完成的指令
    → 更新架构状态 (真正的寄存器文件)
    → 释放 ROB entry

  如果 #2 是 cache miss (等 100 cycles):
    #3 虽然完成了, 但不能退役 → ROB 逐渐填满
    ROB 满 → 前端停顿 → 性能下降

  ═══ ROB 与 Cache Miss ═══

  ROB 大小决定了能容忍多长的 cache miss:
    512 entry ROB, 6-wide 退役 → 可容忍 512/6 ≈ 85 cycles
    → 如果 cache miss > 85 cycles → ROB 满 → 停顿

  ROB 越大 → 越能"隐藏" cache miss 延迟
  这就是为什么现代 CPU ROB 越来越大 (320 → 512 → 630+)
)";
}
} // namespace ch28


// =====================================================================
// 第29章  存储缓冲区与内存消歧
// =====================================================================
namespace ch29 {
void demo() {
    print_section("第29章: 存储缓冲区 (Store Buffer)");

    std::cout << R"(
  ═══ Store Buffer ═══

  Store 指令执行后不立即写入缓存, 而是先进入 Store Buffer
  → Store Buffer 是 CPU 核心内部的小缓冲区

  ┌──────────────────────────────────┐
  │ Store Buffer (per core)          │
  │ ┌──────────────────────────────┐ │
  │ │ [addr: 0x1000] = 42         │ │ 最新
  │ │ [addr: 0x2000] = 100        │ │
  │ │ [addr: 0x1000] = 37         │ │ 较旧
  │ └──────────────────────────────┘ │
  │  退役时 → 写入 L1D Cache        │
  └──────────────────────────────────┘

  Store → Load Forwarding (Store 到 Load 转发):
    Store [0x1000] = 42
    Load  R1, [0x1000]  → 直接从 Store Buffer 读取 42!
    → 不需要访问缓存 → 快!

  ═══ 内存消歧 (Memory Disambiguation) ═══

  问题:
    Store [unknown_addr] = value
    Load  R1, [0x1000]
    → Load 能否越过 Store? 如果 unknown_addr == 0x1000 就不行!

  乱序执行中:
    CPU 推测 Store 和 Load 不冲突 → 先执行 Load
    → 如果后来发现冲突 → 回滚!

  Memory Disambiguator:
    记录哪些 Load 推测性地越过了 Store
    如果曾经违规 → 下次不推测 (保守执行)

  ═══ Store Buffer 大小 ═══

  ┌────────────────┬────────────────┐
  │ 架构            │ Store Buffer   │
  ├────────────────┼────────────────┤
  │ Zen 4           │ 64 entries     │
  │ Golden Cove     │ 72 entries     │
  │ Firestorm       │ ~128 entries   │
  └────────────────┴────────────────┘

  Store Buffer 满 → 新 Store 不能发射 → 停顿

  ═══ Store Buffer 对内存序的影响 ═══

  Store Buffer 意味着: 本核的 Store 对其他核不立即可见
  → 这就是为什么 x86 有 Store-Load 重排序 (TSO)
  → std::atomic + memory_order_seq_cst → MFENCE 刷新 Store Buffer
)";
}
} // namespace ch29


// =====================================================================
// 第30章  执行端口与调度
// =====================================================================
namespace ch30 {
void demo() {
    print_section("第30章: 执行端口与调度");

    std::cout << R"(
  ═══ Intel Golden Cove 执行端口 ═══

  ┌────────┬──────────────────────────────────┐
  │ Port   │ 功能单元                         │
  ├────────┼──────────────────────────────────┤
  │ Port 0 │ ALU, MUL(int), DIV, Branch       │
  │ Port 1 │ ALU, Fast LEA, 向量计算           │
  │ Port 5 │ ALU, 向量 shuffle, LEA            │
  │ Port 6 │ ALU, Branch                       │
  ├────────┼──────────────────────────────────┤
  │ Port 2 │ Load + AGU                        │
  │ Port 3 │ Load + AGU                        │
  │ Port 7 │ Store AGU                          │
  │ Port 8 │ Store AGU                          │
  │ Port 4 │ Store Data                         │
  │ Port 9 │ Store Data                         │
  └────────┴──────────────────────────────────┘

  每 cycle:
    最多 6 μops 发射
    最多 3 Load
    最多 2 Store
    最多 4 ALU 操作

  ═══ 端口争用 ═══

  如果大量指令都需要 Port 0 (如 MUL):
    → Port 0 饱和, 其他端口空闲
    → 吞吐量下降

  解决:
    混合不同类型的指令 → 均匀利用端口
    用 LLVM-MCA / uiCA 分析端口使用

  ═══ 指令延迟 vs 吞吐量 ═══

  ┌──────────┬─────────┬──────────┬─────────────────┐
  │ 指令      │ 延迟    │ 吞吐量   │ 端口            │
  ├──────────┼─────────┼──────────┼─────────────────┤
  │ ADD       │ 1 cyc   │ 4/cyc    │ 0,1,5,6         │
  │ MUL (int) │ 3 cyc   │ 1/cyc    │ 0               │
  │ IMUL 64b  │ 3 cyc   │ 1/cyc    │ 1               │
  │ DIV (int) │ 20-90   │ 1/20-90  │ 0               │
  │ 128b MUL  │ 4 cyc   │ 0.5/cyc  │ 0,1             │
  │ FP ADD    │ 4 cyc   │ 2/cyc    │ 0,1             │
  │ FP MUL    │ 4 cyc   │ 2/cyc    │ 0,1             │
  │ FP DIV    │ 11-15   │ 1/4      │ 0               │
  │ L1 Load   │ 4-5 cyc │ 3/cyc    │ 2,3,7           │
  └──────────┴─────────┴──────────┴─────────────────┘

  延迟: 一条指令从输入到输出
  吞吐量: 连续发射的间隔 (1/cyc = 每cycle一条)
  → 独立指令 → 关注吞吐量
  → 依赖链 → 关注延迟
)";
}
} // namespace ch30


// =====================================================================
// 第31章  指令级并行 (ILP)
// =====================================================================
namespace ch31 {
void demo() {
    print_section("第31章: 指令级并行 (ILP) 最大化");

    constexpr int N = 100000000;

    // 串行依赖: ILP = 1
    {
        volatile int a = 0;
        Timer t;
        for (int i = 0; i < N; ++i) a = a * 3 + i;
        std::cout << "  ILP=1 (串行MUL+ADD): " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }

    // 2 路并行: ILP = 2
    {
        volatile int a=0, b=0;
        Timer t;
        for (int i = 0; i < N; ++i) {
            a = a * 3 + i;
            b = b * 5 + i;
        }
        std::cout << "  ILP=2 (两路独立):    " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }

    // 4 路并行: ILP = 4
    {
        volatile int a=0, b=0, c=0, d=0;
        Timer t;
        for (int i = 0; i < N; ++i) {
            a = a * 3 + i;
            b = b * 5 + i;
            c = c * 7 + i;
            d = d * 11 + i;
        }
        std::cout << "  ILP=4 (四路独立):    " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }

    std::cout << R"(
  ILP 提升技术:
    1. 手动展开: 多个独立累加器
       for (i) { sum0 += a[i]; sum1 += a[i+1]; ... }
    2. 消除循环携带依赖:
       ❌ a[i] = a[i-1] * factor + data[i]  (依赖 a[i-1])
       ✅ 数学变换消除依赖
    3. 独立变量: 不同寄存器 → 不同 ROB slot → 并行执行
    4. 编译器选项: -funroll-loops -O2 -march=native

  理想 ILP = min(发射宽度, 物理寄存器/依赖, 内存带宽)
    实际受限于: 依赖链、缓存 miss、分支 miss
)";
}
} // namespace ch31


// =====================================================================
// 第32章  循环携带依赖
// =====================================================================
namespace ch32 {
void demo() {
    print_section("第32章: 循环携带依赖与优化");

    constexpr int N = 10000000;
    std::vector<double> data(N);
    for (int i = 0; i < N; ++i) data[i] = (i + 1) * 0.01;

    // 循环携带依赖: 每次迭代依赖上一次
    {
        double sum = 0;
        Timer t;
        for (int i = 0; i < N; ++i)
            sum += data[i]; // sum 依赖上一次的 sum → 串行!
        std::cout << "  单累加器:   " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms (sum=" << sum << ")\n";
    }

    // 多累加器打破依赖
    {
        double sum0=0, sum1=0, sum2=0, sum3=0;
        Timer t;
        int i = 0;
        for (; i + 3 < N; i += 4) {
            sum0 += data[i];
            sum1 += data[i+1];
            sum2 += data[i+2];
            sum3 += data[i+3];
        }
        for (; i < N; ++i) sum0 += data[i];
        double sum = sum0 + sum1 + sum2 + sum3;
        std::cout << "  4路累加器:  " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms (sum=" << sum << ")\n";
    }

    // 8 路累加器
    {
        double s0=0,s1=0,s2=0,s3=0,s4=0,s5=0,s6=0,s7=0;
        Timer t;
        int i = 0;
        for (; i + 7 < N; i += 8) {
            s0+=data[i]; s1+=data[i+1]; s2+=data[i+2]; s3+=data[i+3];
            s4+=data[i+4]; s5+=data[i+5]; s6+=data[i+6]; s7+=data[i+7];
        }
        for (; i < N; ++i) s0 += data[i];
        double sum = (s0+s1)+(s2+s3)+(s4+s5)+(s6+s7);
        std::cout << "  8路累加器:  " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms (sum=" << sum << ")\n";
    }

    std::cout << R"(
  FP ADD 延迟 ~4 cycles, 吞吐量 2/cycle (2 个 FP 端口)
  要打满吞吐: 需要 4 × 2 = 8 个独立的 FP ADD 链
  → 8 路累加器 ≈ 理论最优

  其他循环携带依赖优化:
    1. 前缀和 → 并行前缀和 (scan)
    2. 递推 a[i] = f(a[i-1]) → 查找数学封闭解
    3. 直方图 a[idx] += 1 → 多份直方图, 最后合并
    4. 链表遍历 → 预取下 N 个节点

  编译器帮助:
    -O3 -ffast-math → 允许重结合 FP → 自动多路累加
    ⚠️ -ffast-math 改变浮点语义 (结果可能略有不同)
)";
}
} // namespace ch32


// =============================================================================
// ██  六、前端优化篇
// =============================================================================

// =====================================================================
// 第33章  指令缓存优化
// =====================================================================
namespace ch33 {

// 展示大代码量 vs 紧凑代码
NOINLINE int64_t big_function(const int* data, int n) {
    int64_t sum = 0;
    // 重复展开制造"大函数"
    for (int i = 0; i < n; i += 8) {
        sum += data[i] * 1 + data[i+1] * 2 + data[i+2] * 3 + data[i+3] * 4;
        sum += data[i+4] * 5 + data[i+5] * 6 + data[i+6] * 7 + data[i+7] * 8;
        sum -= data[i] * 2 + data[i+1] * 3 + data[i+2] * 4 + data[i+3] * 5;
        sum -= data[i+4] * 6 + data[i+5] * 7 + data[i+6] * 8 + data[i+7] * 9;
    }
    return sum;
}

NOINLINE int64_t compact_function(const int* data, int n) {
    int64_t sum = 0;
    for (int i = 0; i < n; ++i) sum += data[i];
    return sum;
}

void demo() {
    print_section("第33章: 指令缓存 (I-Cache) 优化");

    constexpr int N = 1024;
    std::vector<int> data(N + 8, 1); // +8 防止越界

    constexpr int REPS = 500000;
    {
        Timer t;
        volatile int64_t s = 0;
        for (int r = 0; r < REPS; ++r) s = big_function(data.data(), N);
        std::cout << "  大函数:   " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }
    {
        Timer t;
        volatile int64_t s = 0;
        for (int r = 0; r < REPS; ++r) s = compact_function(data.data(), N);
        std::cout << "  紧凑函数: " << std::fixed << std::setprecision(1)
                  << t.ms() << " ms\n";
    }

    std::cout << R"(
  I-Cache 特点:
    L1 I-Cache: 32 KB, 8-way, 64B line
    → 能容纳 ~500-800 条 x86 指令 (平均 ~4-6 bytes/指令)
    → 热循环必须在 I-Cache 中

  I-Cache miss 代价:
    L2 hit: ~12 cycles
    L3 hit: ~40 cycles
    DRAM: ~200 cycles

  优化:
    1. 热代码保持紧凑 → 减少 I-Cache 占用
    2. 冷代码 NOINLINE → 不占用热路径的 I-Cache space
    3. PGO → 编译器把热代码放一起
    4. -Os (优化大小) vs -O2 (优化速度)
       有时 -Os 更快, 因为更小的代码 → 更少 I-Cache miss
    5. __attribute__((hot)) / __attribute__((cold))
    6. 避免过度内联: 3层以上的内联可能增大代码体积
)";
}
} // namespace ch33


// =====================================================================
// 第34章  μop Cache (DSB)
// =====================================================================
namespace ch34 {
void demo() {
    print_section("第34章: μop Cache (DSB) 与解码器");

    std::cout << R"(
  ═══ x86 译码器 ═══

  x86 指令: 变长 (1-15 bytes), 复杂
  → 必须先译码成固定长度的 μops (micro-operations)

  Legacy Decode Pipeline (MITE):
    取指 → 预译码(找边界) → 译码 → μops
    4-6 个译码器, 每 cycle 译码 4-6 条指令
    复杂指令: 用微码 ROM (15+ μops)

  ═══ μop Cache (Decoded Stream Buffer, DSB) ═══

  Intel 从 Sandy Bridge (2011) 引入:
    缓存已经译码过的 μops
    → 再次执行同一代码时, 跳过译码, 直接从 DSB 获取 μops

  ┌──────┐                    ┌──────────┐
  │ MITE │→ 译码 → μops ──→  │ μop Cache│ → 执行
  │ (IF) │                    │ (DSB)    │
  └──────┘    ↑ miss          └──────────┘
              └─────────── 命中 → 直接供给 ↑

  DSB 容量:
  ┌────────────────┬────────────────────────────┐
  │ 架构            │ μop Cache 大小             │
  ├────────────────┼────────────────────────────┤
  │ Sandy Bridge   │ 1536 μops, 32 sets, 8-way  │
  │ Skylake+       │ 2048+ μops                 │
  │ Golden Cove    │ 4096 μops                  │
  │ Zen 4          │ 6144 μops (Op Cache)       │
  └────────────────┴────────────────────────────┘

  DSB 优势:
    - 跳过复杂的 x86 译码 → 省功耗 + 更高带宽
    - 可以每 cycle 提供 6-8 μops (vs MITE 4-5)
    - 减少前端气泡

  DSB miss → 回退到 MITE 译码 → 前端带宽下降
  → 热循环应该适合 DSB (< 4K μops)

  ═══ DSB 对齐限制 ═══

  DSB 以 32B 边界组织 (与 I-Cache 行对齐)
  如果一条指令跨越 32B 边界 → 可能不适合 DSB
  → 函数/循环对齐到 32B 或 64B 可以改善 DSB 命中率

  检查:
    perf stat -e idq.dsb_uops,idq.mite_uops ./app
    DSB 比例 > 80% = 良好
)";
}
} // namespace ch34


// =====================================================================
// 第35章  循环流检测器 (LSD)
// =====================================================================
namespace ch35 {
void demo() {
    print_section("第35章: 循环流检测器 (LSD)");

    std::cout << R"(
  ═══ LSD (Loop Stream Detector) ═══

  当 CPU 检测到一个小循环 (< 256 μops):
    1. 锁定 μop Queue 中的循环 μops
    2. 不再从 DSB/MITE 取指 → 直接从 Queue 循环供给
    3. 关闭前端 → 省功耗 + 零前端气泡

  ┌────────────────────────────────────────────┐
  │      μop Queue                             │
  │ ┌─────┬─────┬─────┬─────┬─────┬─────┐     │
  │ │ μop1│ μop2│ μop3│ μop4│ μop5│ μop6│     │
  │ └──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┘     │
  │    └─────┴─────┴─────┴─────┴─────┘         │
  │              ↻ 循环供给                     │
  └────────────────────────────────────────────┘

  LSD 条件:
    - 循环体 < 64 μops (Intel) / <256 μops (AMD)
    - 没有错误预测的内部分支
    - 迭代次数足够 (>64 通常)

  ═══ LSD 在不同架构上 ═══

  Intel:
    Skylake: LSD 因漏洞被禁用 (微码更新)
    Alder Lake+: 重新启用, ~128 μops 容量
  AMD:
    Zen 4: Op Cache 足够大, LSD 需求相对较小

  ═══ 对编程的影响 ═══

  1. 热循环保持 < 64 μops → 可能进入 LSD
  2. 循环展开适度: 展开太多 → 超出 LSD → 可能反而变慢
  3. 内部分支少: 不可预测的分支破坏 LSD 效果

  实验: 逐步增大循环体, 观察 LSD hit 下降
    perf stat -e lsd.uops ./app
)";
}
} // namespace ch35


// =====================================================================
// 第36章  代码对齐
// =====================================================================
namespace ch36 {
void demo() {
    print_section("第36章: 代码对齐与 I-Cache 利用");

    std::cout << R"(
  ═══ 代码对齐的重要性 ═══

  CPU 以 16B/32B/64B 为单位从 I-Cache 取指令
  如果循环或函数起始地址没有对齐:
    → 循环头跨两个 cache line → 多取一行
    → DSB 32B window 效率下降
    → I-TLB 多用一个 entry

  ═══ 对齐指令 ═══

  GCC:
    -falign-functions=64    // 函数对齐到 64 字节
    -falign-loops=32        // 循环对齐到 32 字节
    -falign-jumps=16        // 跳转目标对齐
    -falign-labels=16       // 标签对齐

  代码属性:
    __attribute__((aligned(64))) void hot_function() { ... }

  ═══ 对齐 NOP ═══

  对齐使用 NOP 填充 → 增加代码大小
  过度对齐 → 大量 NOP → I-Cache 浪费

  最佳实践:
    热函数: align 64
    热循环: align 32
    普通函数: align 16 (默认)
    冷函数: 不需要对齐

  ═══ 性能敏感性 ═══

  LLVM 研究发现:
    仅改变代码对齐 → 性能可变化 ±10%!
    → 这就是 "性能测量噪音" 的来源之一

  解决:
    1. PGO 自动选择最佳对齐
    2. BOLT (Binary Optimization and Layout Tool):
       链接后二进制重排 → 根据 profile 选择最佳布局
       效果: +5-15% (大型应用)

  BOLT 使用:
    perf record -e cycles:u -j any,u -- ./app
    perf2bolt -p perf.data -o perf.fdata ./app
    llvm-bolt ./app -o ./app.bolt -data=perf.fdata -reorder-blocks=ext-tsp
)";
}
} // namespace ch36


// =============================================================================
// ██  七、性能计数器与分析篇
// =============================================================================

// =====================================================================
// 第37章  硬件性能计数器 (PMU)
// =====================================================================
namespace ch37 {
void demo() {
    print_section("第37章: 硬件性能计数器 (PMU)");

    std::cout << R"(
  ═══ PMU (Performance Monitoring Unit) ═══

  CPU 内置硬件计数器, 记录微架构事件
  每个核心有 4-8 个可编程计数器 + 3-4 个固定计数器

  ═══ 固定计数器 (Always Available) ═══
    INST_RETIRED.ANY        — 退役的指令数
    CPU_CLK_UNHALTED.THREAD — 线程活跃时钟周期
    CPU_CLK_UNHALTED.REF_TSC — 参考时钟周期

  ═══ 常用可编程计数器 ═══

  ┌──────────────────────────────┬──────────────────────────┐
  │ 计数器名                     │ 含义                     │
  ├──────────────────────────────┼──────────────────────────┤
  │ MEM_LOAD_RETIRED.L1_MISS    │ L1 缓存读 miss           │
  │ MEM_LOAD_RETIRED.L2_MISS    │ L2 缓存读 miss           │
  │ MEM_LOAD_RETIRED.L3_MISS    │ L3 缓存读 miss (→ DRAM) │
  │ BR_MISP_RETIRED.ALL_BRANCHES│ 分支预测失败             │
  │ MACHINE_CLEARS.COUNT        │ 流水线清空               │
  │ UOPS_ISSUED.ANY             │ 发射的 μops             │
  │ UOPS_RETIRED.SLOTS          │ 退役的 μop slots        │
  │ IDQ.DSB_UOPS               │ DSB 供给的 μops         │
  │ IDQ.MITE_UOPS              │ MITE 供给的 μops        │
  └──────────────────────────────┴──────────────────────────┘

  ═══ DCI 代码中读取 PMU ═══

  Linux perf_event_open() API:
    struct perf_event_attr attr;
    attr.type = PERF_TYPE_HARDWARE;
    attr.config = PERF_COUNT_HW_CACHE_MISSES;
    int fd = perf_event_open(&attr, 0, -1, -1, 0);
    read(fd, &count, sizeof(count));

  RDPMC 指令 (最低开销, ~20 cycles):
    // 需要内核允许: echo 2 > /proc/sys/kernel/perf_event_paranoid
    uint64_t count;
    asm volatile("rdpmc" : "=a"(count) : "c"(0));  // 读取计数器 0
)";
}
} // namespace ch37


// =====================================================================
// 第38章  Top-Down 分析法
// =====================================================================
namespace ch38 {
void demo() {
    print_section("第38章: perf 实战 — Top-Down 分析法");

    std::cout << R"(
  ═══ Ahmad Yasin's Top-Down Microarchitecture Analysis Method (TMAM) ═══

  将 CPU pipeline slots 分为 4 类:

  ┌────────────────────────────────────────────────┐
  │               Total Pipeline Slots              │
  │                                                │
  │   ┌──────────────┐   ┌──────────────────────┐  │
  │   │   Retiring    │   │     Not Retiring      │  │
  │   │   (有用工作)  │   │     (浪费)            │  │
  │   │              │   │ ┌─────────────────┐  │  │
  │   │              │   │ │  Bad Speculation │  │  │
  │   │              │   │ │  (错误推测)      │  │  │
  │   │              │   │ └─────────────────┘  │  │
  │   │              │   │ ┌─────────────────┐  │  │
  │   │              │   │ │  Frontend Bound  │  │  │
  │   │              │   │ │  (前端瓶颈)      │  │  │
  │   │              │   │ └─────────────────┘  │  │
  │   │              │   │ ┌─────────────────┐  │  │
  │   │              │   │ │  Backend Bound   │  │  │
  │   │              │   │ │  (后端瓶颈)      │  │  │
  │   │              │   │ └─────────────────┘  │  │
  │   └──────────────┘   └──────────────────────┘  │
  └────────────────────────────────────────────────┘

  1. Retiring (退役): 有用工作 — 越高越好
     理想: 100%, 实际优秀: >50%

  2. Bad Speculation (错误推测):
     分支预测失败 + Machine Clear
     → 优化分支: PGO, branchless, 排序数据

  3. Frontend Bound (前端瓶颈):
     I-Cache miss, DSB miss, 译码瓶颈
     → 优化: 代码紧凑, PGO 布局, BOLT

  4. Backend Bound (后端瓶颈):
     又分为:
     - Memory Bound: L1/L2/L3/DRAM miss, Store 争用
       → 预取, 大页, 数据结构优化
     - Core Bound: 执行端口饱和, 依赖链
       → 增加 ILP, 减少延迟, SIMD

  ═══ perf 命令 ═══

  Level 1:
    perf stat --topdown ./app         # 自动 Top-Down Level 1

  Level 2+:
    perf stat --topdown -v ./app      # 详细

  手动计数器:
    perf stat -e instructions,cycles,\
      L1-dcache-load-misses,L1-dcache-loads,\
      LLC-load-misses,LLC-loads,\
      branch-misses,branches \
      ./app

  计算:
    IPC = instructions / cycles
    L1 miss rate = L1-dcache-load-misses / L1-dcache-loads
    Branch miss rate = branch-misses / branches

  典型目标:
    IPC > 2.0       → 良好
    L1 miss < 2%    → 良好
    L3 miss < 0.5%  → 良好
    Branch miss < 1% → 良好
)";
}
} // namespace ch38


// =====================================================================
// 第39章  VTune / μProf
// =====================================================================
namespace ch39 {
void demo() {
    print_section("第39章: Intel VTune / AMD μProf");

    std::cout << R"(
  ═══ Intel VTune Profiler ═══

  安装: https://www.intel.com/vtune (免费)

  关键分析模式:
    1. Microarchitecture Exploration:
       自动 Top-Down 分析, 逐函数展示瓶颈类型
       → "这个函数 60% Backend Memory Bound"

    2. Memory Access:
       L1/L2/L3 miss 热点, NUMA 远程访问,
       False Sharing 检测

    3. Threading:
       锁竞争, 线程利用率, 等待时间分析

    4. Platform Profiler:
       系统级: 中断频率, C-State 转换, 内存带宽

  使用:
    vtune -collect uarch-exploration ./app
    vtune -collect memory-access ./app
    vtune -report summary

  ═══ AMD μProf ═══

  AMD 对标 VTune 的工具 (免费)

  特色:
    - IBS (Instruction-Based Sampling): AMD 独有
      比传统 PMU 采样更精确 (标记特定指令)
    - L3 Slice 分析: AMD CCD 架构下的 L3 使用情况
    - 功耗分析: 每核心/每 CCD 功耗

  ═══ perf + FlameGraph ═══

  最常用的 Linux 性能分析组合:

  # 录制 CPU 采样
  perf record -g -F 2000 -- ./app

  # 生成火焰图
  perf script | stackcollapse-perf.pl | flamegraph.pl > flame.svg

  # 查看热点函数
  perf report

  # 逐行分析
  perf annotate func_name

  ═══ LLVM-MCA (Machine Code Analyzer) ═══

  静态分析: 不需要运行程序, 分析汇编指令吞吐量

  echo "imulq %rax, %rbx
  addq %rbx, %rcx
  imulq %rcx, %rdx" | llvm-mca -mcpu=znver4

  输出:
    Throughput: 3.0 cycles per iteration
    Resource pressure per iteration:
    Port 0: 1.0  Port 1: 2.0  ...
)";
}
} // namespace ch39


// =====================================================================
// 第40章  微架构优化检查清单
// =====================================================================
namespace ch40 {
void demo() {
    print_section("第40章: 微架构优化检查清单");

    std::cout << R"(
  ═══════════════════════════════════════════════════════════
  CPU 微架构优化完整检查清单
  ═══════════════════════════════════════════════════════════

  □ 1. 缓存
    □ 数据结构紧凑 (减少 cache line 使用)
    □ 热数据和冷数据分离
    □ 避免 false sharing (alignas(64))
    □ 避免 2 的幂步长 (矩阵加 padding)
    □ 使用预取 (__builtin_prefetch)
    □ 优先 vector 而非 list/map
    □ 工作集适合 L1/L2 (分块处理)
    □ 使用大页减少 TLB miss

  □ 2. 分支预测
    □ 减少不可预测分支 (branchless)
    □ 按类型分组处理 (消除多态分支)
    □ likely/unlikely 标注
    □ switch case 按概率排序
    □ PGO 优化分支预测
    □ 排序化数据使分支可预测

  □ 3. 指令级并行
    □ 多路独立累加器
    □ 消除循环携带依赖
    □ 适度循环展开 (4-8 路)
    □ 混合不同类型指令 (ALU + Load)
    □ 减少长延迟操作 (除法 → 乘法)

  □ 4. 前端优化
    □ 热代码紧凑 (适合 DSB / I-Cache)
    □ 冷函数 NOINLINE
    □ 函数/循环对齐 (align 32/64)
    □ 避免过度内联
    □ BOLT 二进制重排 (大型应用)

  □ 5. 内存系统
    □ 顺序访问 > 随机访问
    □ SoA vs AoS 选择
    □ 减少指针追逐
    □ NT store 用于只写大数据
    □ NUMA 感知 (首次触摸策略)

  □ 6. 测量与验证
    □ perf stat: IPC, cache miss, branch miss
    □ perf top-down: 瓶颈分类
    □ perf record + flame graph: 热点定位
    □ VTune/μProf: 深度微架构分析
    □ LLVM-MCA: 关键循环静态分析

  ═══ 优化优先级 (投入产出比) ═══

  ┌────────────────────┬──────────────────────────────────┐
  │ 优先级              │ 优化内容                        │
  ├────────────────────┼──────────────────────────────────┤
  │ 1 (最高 ROI)       │ 算法复杂度 O(N²)→O(N log N)    │
  │ 2                  │ 数据结构选择 (缓存友好)          │
  │ 3                  │ 缓存优化 (分块, 预取, 对齐)     │
  │ 4                  │ 分支预测优化                     │
  │ 5                  │ ILP / 循环展开                   │
  │ 6                  │ SIMD 向量化                      │
  │ 7                  │ 代码布局 (PGO, BOLT)            │
  │ 8 (最低 ROI)       │ 指令级微调 (peephole)           │
  └────────────────────┴──────────────────────────────────┘

  黄金法则: 先测量, 再优化
    不要猜测瓶颈 → 用 perf 定位 → 针对性优化

  ═══ 延迟量级速查 ═══

  ┌────────────────────────────┬──────────────────────┐
  │ 操作                       │ 延迟 (cycles / ns)   │
  ├────────────────────────────┼──────────────────────┤
  │ 寄存器操作 (ADD/SUB/XOR)  │ 1 cycle              │
  │ 整数乘法 (IMUL)           │ 3 cycles             │
  │ 整数除法 (IDIV)           │ 20-90 cycles         │
  │ FP 加法/乘法              │ 4-5 cycles           │
  │ FP 除法                   │ 11-15 cycles         │
  │ FP 开方 (SQRT)            │ 15-20 cycles         │
  │ L1 Cache hit              │ 4-5 cycles, ~1ns     │
  │ L2 Cache hit              │ 12 cycles, ~4ns      │
  │ L3 Cache hit              │ 40-50 cycles, ~12ns  │
  │ DRAM 访问                 │ 200+ cycles, ~65ns   │
  │ 分支预测失败              │ 15-25 cycles         │
  │ 上下文切换                │ 5000-15000 cycles    │
  │ 系统调用 (getpid)         │ ~300 cycles          │
  │ Page Fault                │ ~10000-50000 cycles  │
  └────────────────────────────┴──────────────────────┘
)";
}
} // namespace ch40


// =============================================================================
// 八、扩展专题 (第 41–46 章)
// =============================================================================

// =============================================================================
// 第 41 章：投机执行安全 — Spectre / Meltdown 与缓解代价
// =============================================================================
namespace ch41 {

void demo() {
    std::cout << "\n[CH41] 投机执行安全 (Spectre/Meltdown)\n";
    std::cout << R"(
=== 第 41 章：投机执行安全 ===

【背景】现代 CPU 为了提高 IPC 而进行推测式(投机)执行:
  - 分支预测后继续取指/执行，结果可能被撤销
  - cache miss 期间推测性地执行后续指令
  - 乱序窗口内"可见但还未提交"的状态暴露了安全漏洞

【Spectre V1: 边界检查绕过 (CVE-2017-5753)】
  攻击原理:
  if (idx < array_size) {       //← 预测"taken"，在EX验证前已投机访问
      x = array1[idx];           //← 越界 idx → 读取越界数据
      y = array2[x * 64];        //← cache side channel: 这行被加载!
  }
  // 即使分支被撤销，array2 对应索引的 cache 行已被加载
  // 攻击者通过测量 array2[i] 的访问延迟推断越界数据

  缓解方案:
  1. LFENCE 序列化: if (idx < size) { _mm_lfence(); use(idx); }
     代价: ~10-50 cycles 额外延迟
  2. Index masking: idx = idx & (size-1)  (保证合法范围)
  3. Retpoline (return trampoline): 替换间接跳转
     __asm__(
       "call setup_target\n"
       "jmp retpoline_loop\n"      // 无限循环欺骗 BTB
       "setup_target:\n"
       "movq %%rax, (%%rsp)\n"     // 设置真实目标
       "ret\n"                       // 通过 RAS 预测 → 走 Retpoline loop
       "retpoline_loop:\n"
       "pause\n"
       "jmp retpoline_loop\n"
       : : "a"(target) :
     );

【Meltdown: 非法数据缓存加载 (CVE-2017-5754)】
  攻击原理: 投机性地读取"无权访问"的内核内存并通过 cache 泄露
  缓解: KPTI (Kernel Page Table Isolation) — 用户态/内核态分离页表

  KPTI 代价:
  ┌────────────────────────────────┬───────────────────────────────┐
  │ 场景                           │ 代价                          │
  ├────────────────────────────────┼───────────────────────────────┤
  │ 无 PCID + KPTI                 │ 每次 syscall 完全刷 TLB       │
  │                                │ → 开销常见较高 (取决于负载/内核/CPU) │
  │ PCID + KPTI (Broadwell+)      │ 进程 TLB 保留                 │
  │                                │ → 开销通常明显下降             │
  │ Intel 10nm+ (Ice Lake+)        │ 硬件修复 Meltdown              │
  │                                │ → 是否仍启用 KPTI 需看内核策略与平台配置 │
  └────────────────────────────────┴───────────────────────────────┘

【Spectre V2: 间接分支中毒 (CVE-2017-5715)】
  攻击: 训练 BTB (分支目标缓冲) 指向攻击者gadget → 受害进程投机执行
  缓解:
  1. IBRS (Indirect Branch Restricted Speculation): 内核入口设置
  2. Retpoline: 替换所有间接跳转 (GCC: -mindirect-branch=thunk)
  3. eIBRS (Enhanced IBRS, Skylake+): 硬件保护，perf 代价更小

【检查系统 Spectre 缓解状态】
  cat /sys/devices/system/cpu/vulnerabilities/spectre_v1
  cat /sys/devices/system/cpu/vulnerabilities/spectre_v2
  cat /sys/devices/system/cpu/vulnerabilities/meltdown

  典型输出:
   spectre_v1: Mitigation: usercopy/swapgs barriers and __user pointer sanitization
   spectre_v2: Mitigation: Retpolines, IBPB: conditional, IBRS_FW, ...
   meltdown:   Not affected (Intel 10nm+) 或 Mitigation: PTI

【代码实践: 安全 vs 性能权衡】
  // 关键路径禁用某些缓解 (仅可信输入!)
  prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_STORE_BYPASS,
        PR_SPEC_DISABLE, 0, 0);

  // 序列化屏障 (阻止推测执行越过它)
  _mm_lfence();    // Intel: Load Fence，阻止 Load 越过
  asm("isb");      // ARM: Instruction Synchronization Barrier
)";
}

} // namespace ch41

// =============================================================================
// 第 42 章：硬件预取器行为深度分析
// =============================================================================
namespace ch42 {

// 测量不同步长下硬件预取效果
static uint64_t measure_stride_access(size_t stride_bytes, size_t total_bytes) {
    const size_t n = total_bytes / sizeof(uint64_t);
    std::vector<uint64_t> data(n, 1);
    const size_t stride = stride_bytes / sizeof(uint64_t);
    if (stride == 0) return 0;

    volatile uint64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < n * stride; i += stride)
        sink += data[i % n];
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    return (uint64_t)std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
}

void demo() {
    std::cout << "\n[CH42] 硬件预取器行为分析\n";
    std::cout << R"(
=== 第 42 章：硬件预取器行为深度分析 ===

【Intel 4个硬件预取器 (L1/L2 各2个)】

  L1 Prefetchers:
  ┌─────────────────────┬────────────────────────────────────────────┐
  │ 名称                │ 触发条件                                    │
  ├─────────────────────┼────────────────────────────────────────────┤
  │ L1 Streamer         │ 2+ 次连续 cache miss, 步长 ≤ 2048B         │
  │ L1 Spatial Prefetch │ 同 4KB 页内相邻 cache line                 │
  └─────────────────────┴────────────────────────────────────────────┘

  L2 Prefetchers:
  ┌─────────────────────┬────────────────────────────────────────────┐
  │ 名称                │ 触发条件                                    │
  ├─────────────────────┼────────────────────────────────────────────┤
  │ L2 Streamer         │ 多次 L1 miss 形成流, 预取到 L2             │
  │ L2 Adjacent Line    │ L2 miss 时同时预取相邻缓存行               │
  └─────────────────────┴────────────────────────────────────────────┘

【预取器失效场景】
  1. 步长 > 2048B → L1 Streamer 停止预取 (看不出规律)
  2. 随机访问 (链表遍历) → 所有预取器失效
  3. 步长 = 缓存行 (64B) 但不规则 → Spatial 失效
  4. 访问多个独立流 (>8~16) → 预取器资源耗尽

【手动预取 vs 硬件预取】

  // __builtin_prefetch(addr, rw, locality)
  //   rw: 0=读预取, 1=写预取
  //   locality: 0=不缓存, 1=L3, 2=L2, 3=L1(最高优先级)

  // 典型用法: 提前 N 个 cache line 预取
  for (int i = 0; i < N; ++i) {
      __builtin_prefetch(&arr[i + 16], 0, 1);  // 16 cache lines = 1KB 提前
      process(arr[i]);
  }

  // 提前距离的选择:
  //   太早 → 预取出来已被驱逐(数据集太大)
  //   太晚 → 数据还未就绪(距离太短)
  //   最优 ≈ L2 延迟 / 单步处理时间 ≈ 10~30 个 cache line

  // 非规则访问: 软件预取优于硬件预取
  for (int i = 0; i < N; ++i) {
      __builtin_prefetch(&table[key[i+8]], 0, 1);  // 提前 8 步
      result += table[key[i]];
  }

【AMD 预取器差异】
  Zen 3/4: 额外拥有 Op Cache 级别的预测取指
  IP-based Stride Prefetcher: 基于 PC 的步长记录 → 每个指令独立的步长历史
  相比 Intel 可预测更短的步长 (甚至 1×) 和更复杂的模式

【关闭/控制预取器 (调试用)】
  # Intel MSR 0x1A4 控制预取器开关
  sudo wrmsr -a 0x1A4 0xF   # 关闭全部 4 个预取器
  sudo wrmsr -a 0x1A4 0x0   # 重新启用

  # 通过 PRFM 指令 (ARM)
  asm("prfm pldl1strm, [%0]" : : "r"(addr));   // Stream L1
  asm("prfm pldl2keep, [%0]" : : "r"(addr));   // Keep L2
)";

    // 实测不同步长的访问时间
    constexpr size_t TOTAL = 16 * 1024 * 1024;  // 16MB, 超出 L3
    std::cout << "\n  步长访问延迟测试 (16MB 数据集):\n";
    std::cout << "  注: 步长 <= 2048B 时硬件预取激活，延迟更低\n";
    size_t strides[] = {64, 128, 256, 512, 1024, 2048, 4096, 8192};
    for (size_t s : strides) {
        uint64_t us = measure_stride_access(s, TOTAL);
        std::cout << "  stride=" << std::setw(5) << s << "B: "
                  << std::setw(6) << us << " us"
                  << (s <= 2048 ? " ← 预取器可覆盖" : " ← 预取器失效") << "\n";
    }
}

} // namespace ch42

// =============================================================================
// 第 43 章：SIMD 指令集与微架构交互
// =============================================================================
namespace ch43 {

void demo() {
    std::cout << "\n[CH43] SIMD 指令集微架构交互 (AVX/AVX2/AVX-512)\n";
    std::cout << R"(
=== 第 43 章：SIMD 指令集与微架构交互 ===

【SIMD 寄存器层次】
  SSE:    XMM0~XMM15   128-bit  (16 bytes)
  AVX/2:  YMM0~YMM15   256-bit  (32 bytes)
  AVX-512: ZMM0~ZMM31  512-bit  (64 bytes) + 8 个 mask 寄存器 k0~k7

【AVX-512 频率降档 (Intel 重要陷阱!)】

  在 Skylake/Ice Lake/Rocket Lake 上:
  ┌────────────────────────────────────┬─────────────────────────────┐
  │ 场景                               │ 频率影响                    │
  ├────────────────────────────────────┼─────────────────────────────┤
  │ 无 AVX-512 指令                    │ 基础频率 (3.6 GHz)          │
  │ 执行 AVX-512 整数指令              │ AVX-512 Turbo (-300 MHz)    │
  │ 执行 AVX-512 FP 重型指令 (VFMA)   │ AVX-512 Heavy (-500 MHz)    │
  └────────────────────────────────────┴─────────────────────────────┘

  影响时长: 降频后 ~1ms 才能恢复 → 若只有几条 512b 指令，得不偿失!

  Intel Golden Cove (12th Gen) 改进: 减小了降频幅度 (~100-200 MHz)
  AMD Zen 4: 原生 AVX-512 支持，无频率降档问题

  最佳实践:
  1. 热循环完全使用 AVX-512 (不要混用 AVX2 和 AVX-512)
  2. 或者对 Skylake 目标使用 AVX2
  3. 用 -march=sapphirerapids 或 -march=znver4 编译

【执行端口与 SIMD 指令竞争】

  Golden Cove SIMD 端口分布:
  Port 0: FP MUL/FMA (256-bit), VEC SHUFFLE
  Port 1: FP ADD/FMA (256-bit), VEC ALU
  Port 5: VEC SHUFFLE, VPERMD, VPERMQ

  // FMA (Fused Multiply-Add): 一条指令完成 a*b+c
  // 延迟 4-5 cyc, 吞吐 2/cyc (Port 0+1)
  __m256 r = _mm256_fmadd_ps(a, b, c);  // r = a*b + c

  vs 分开写:
  __m256 t = _mm256_mul_ps(a, b);       // 3 cyc
  __m256 r = _mm256_add_ps(t, c);       // 4 cyc after t → 实际 7 cyc
  // FMA 不仅快，还更精确 (中间结果不舍入)

【数据重排惩罚 (Lane Crossing)】
  256-bit YMM 由两个 128-bit lane 组成:
  _mm256_permute2f128_ps  ← 跨 lane, 3 cyc on Port 5
  _mm256_shuffle_ps       ← 同 lane, 1 cyc on Port 0/1/5

  // 尽量保持数据在同一 lane 内操作
  // 必须跨 lane 时 VPERM2I128 可批量处理

【SIMD 对齐要求】
  // 对齐读 (比非对齐快 10-15% 在老 CPU)
  // 现代 CPU (Haswell+): 无明显差异，但跨 4KB 页边界仍有惩罚
  float* p = (float*)_mm_malloc(64, 32);   // 32字节对齐 for AVX
  __m256 v = _mm256_load_ps(p);            // 要求 32B 对齐
  __m256 u = _mm256_loadu_ps(p);           // 无对齐要求 (推荐)
  _mm_free(p);

【寄存器压力 (AVX-512 的优势)】
  普通 x86-64:  16 个 GP 寄存器 → 复杂循环 spill 到栈
  AVX2:         16 个 YMM (ymm0~15) 
  AVX-512:      32 个 ZMM (zmm0~31) → 减少 spill, 代码密度更高

  // 手动向量化 SAXPY (y = a*x + y)
  void saxpy_avx2(float a, float* x, float* y, int n) {
      __m256 va = _mm256_set1_ps(a);
      for (int i = 0; i < n; i += 8) {
          __m256 vx = _mm256_loadu_ps(x + i);
          __m256 vy = _mm256_loadu_ps(y + i);
          vy = _mm256_fmadd_ps(va, vx, vy);  // vy = va*vx + vy
          _mm256_storeu_ps(y + i, vy);
      }
  }

【SIMD 分支: vcmps + blend】
  // 无分支 SIMD: 向量比较 + 混合选择
  __m256 mask = _mm256_cmp_ps(a, threshold, _CMP_GT_OS);  // a > threshold?
  __m256 result = _mm256_blendv_ps(b, a, mask);            // mask? a : b
)";
}

} // namespace ch43

// =============================================================================
// 第 44 章：内存带宽分析与 Roofline 模型
// =============================================================================
namespace ch44 {

// STREAM-like benchmark: Triad: a[i] = b[i] + scalar * c[i]
static double measure_triad_bandwidth() {
    constexpr size_t N = 16 * 1024 * 1024;  // 16M doubles = 128MB per array
    std::vector<double> a(N, 1.0), b(N, 2.0), c(N, 3.0);
    const double scalar = 3.0;

    // warm up
    for (size_t i = 0; i < N; ++i) a[i] = b[i] + scalar * c[i];

    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i)
        a[i] = b[i] + scalar * c[i];
    auto t1 = std::chrono::high_resolution_clock::now();

    double secs = std::chrono::duration<double>(t1 - t0).count();
    double bytes = 3.0 * N * sizeof(double);  // 2 reads + 1 write
    return bytes / secs / 1e9;  // GB/s
}

void demo() {
    std::cout << "\n[CH44] 内存带宽分析与 Roofline 模型\n";

    double bw = measure_triad_bandwidth();
    std::cout << "  STREAM Triad 实测带宽: " << std::fixed
              << std::setprecision(1) << bw << " GB/s\n";

    std::cout << R"(
=== 第 44 章：内存带宽分析与 Roofline 模型 ===

【内存系统带宽层次】

  各级带宽 (Intel Core i9-13900K 参考):
  ┌───────────────────┬──────────────────┬────────────────────┐
  │ 层级              │ 带宽             │ 延迟               │
  ├───────────────────┼──────────────────┼────────────────────┤
  │ L1D               │ ~3 TB/s (实际)   │ 4-5 cycles         │
  │ L2                │ ~1 TB/s          │ 12 cycles          │
  │ L3                │ ~200 GB/s        │ 40 cycles          │
  │ DRAM (DDR5-5200×2)│ ~75-83 GB/s 理论 │ ~65 ns (~200 cyc)  │
  │ DRAM (实际)        │ ~50-65 GB/s     │ — (带宽饱和)       │
  └───────────────────┴──────────────────┴────────────────────┘

【STREAM Benchmark (内存带宽标准测试)】

  四种操作     代码                    读/写访存量
  Copy:        a[i] = b[i]            2N bytes
  Scale:       a[i] = s*b[i]          2N bytes
  Add:         a[i] = b[i]+c[i]       3N bytes
  Triad:       a[i] = b[i]+s*c[i]    3N bytes  ← 最常用

  关键要求: 数组大小 >> LLC 大小 (否则测的是 L3 带宽)

【Roofline 模型】

  基本思想: 性能受限于 计算能力 或 内存带宽 中较小的一个

  性能上界 = min(峰值FLOPS, 带宽 × 算术强度)

  算术强度 (Arithmetic Intensity, AI):
    AI = FLOPs / DRAM访问字节数  [单位: FLOP/Byte]

  示例:
  ┌──────────────────────┬────────────────┬───────────┬────────┐
  │ 算法                 │ FLOPs          │ AI        │ 瓶颈   │
  ├──────────────────────┼────────────────┼───────────┼────────┤
  │ 向量加法 a=b+c       │ N              │ 1/24      │ 内存   │
  │ SAXPY                │ 2N             │ 1/12      │ 内存   │
  │ Dense MatMul N×N     │ 2N³            │ N/12      │ 计算   │
  │ Sparse MatVec        │ 2nnz           │ ~0.25     │ 内存   │
  │ FFT N logN           │ 5N log₂N       │ ~1.5      │ 中间   │
  └──────────────────────┴────────────────┴───────────┴────────┘

  Roofline 分析步骤:
  1. 测量峰值 FLOPS: P = {cores} × {FMA/cyc} × 2 × {SIMD lanes} × frequency
     例: 8 cores × 2 FMA × 2 × 8(AVX-512 float) × 4GHz = 1024 GFLOPS
  2. 测量有效带宽 B (STREAM Triad)
  3. 山脊点 (Ridge Point) = P / B  [FLOP/Byte]
     例: 1000 GFLOPS / 50 GB/s = 20 FLOP/Byte
  4. AI < 山脊点 → 内存密集，优化: 减少 DRAM 访问、SIMD 宽度、数据重用
  5. AI > 山脊点 → 计算密集，优化: SIMD、FMA、ILP、低延迟循环

【带宽饱和与多线程】

  单核带宽通常达不到峰值, 需要多线程:
  // 使用 OpenMP 填满内存带宽
  #pragma omp parallel for schedule(static)
  for (int i = 0; i < N; ++i)
      a[i] = b[i] + scalar * c[i];

  // 一般 4~8 线程就能打满 DRAM 带宽
  // 更多线程: 带宽不再增加，但 NUMA 问题出现

【带宽限制优化策略】
  1. 降低工作数据集: 分块 (Tiling) + 缓存重用
  2. 压缩数据: float16/int8 减少带宽 2×/4×
  3. 增大算术强度: 融合多个操作 (kernel fusion)
  4. 非临时写 (NT Store): 绕过缓存直接写 → 节省 RFO (Read-For-Ownership)
  5. 预取: 软件预取减少延迟惩罚 (不增加带宽!)
)";
}

} // namespace ch44

// =============================================================================
// 第 45 章：多路 NUMA 深度分析
// =============================================================================
namespace ch45 {

void demo() {
    std::cout << "\n[CH45] 多路 NUMA 架构深度分析\n";
    std::cout << R"(
=== 第 45 章：多路 NUMA (Non-Uniform Memory Access) ===

【NUMA 产生原因】

  单 socket DDR 通道数有限 (通常 4~6 通道, ~100-200 GB/s)
  多 socket 系统: 每个 socket 有自己的 DDR → 累加带宽
  但访问远端 socket 的内存有额外延迟

  Intel 2-socket 系统:
  ┌──────────────────────┐        ┌──────────────────────┐
  │ Socket 0             │        │ Socket 1             │
  │ Core 0..27           │◄──────►│ Core 28..55          │
  │ L3: 52.5MB           │ UPI/QPI│ L3: 52.5MB           │
  │ DDR5 × 6 channels   │        │ DDR5 × 6 channels   │
  │ ~ 192 GB/s local     │        │ ~ 192 GB/s local     │
  └──────────────────────┘        └──────────────────────┘
                UPI 互连带宽: 单向 ~200 GB/s (双向 ~400 GB/s)

  延迟对比:
  ┌───────────────────────────┬────────────────┐
  │ 访问类型                  │ 延迟           │
  ├───────────────────────────┼────────────────┤
  │ 本地 L3 hit               │ ~40 cycles     │
  │ 本地 DRAM                 │ ~65 ns         │
  │ 跨 socket (remote DRAM)   │ ~120-150 ns    │
  │ 4-socket 最远             │ ~200-250 ns    │
  └───────────────────────────┴────────────────┘

【AMD EPYC NUMA 特殊性 (多 CCD 架构)】

  Zen 4 EPYC Rome: 8 CCD (Core Complex Die) × 8 cores = 64 cores
  每个 CCD 有独立的 32MB L3 Cache
  CCD 之间通过 Infinity Fabric 互连

  NUMA 域:
  ┌─────────────────────────────────────────────────────────┐
  │              EPYC 9654 (Genoa) 96-core                  │
  │  CCD0[8C,32MB-L3]  CCD1  CCD2  CCD3  (NUMA Node 0)     │
  │  CCD4[8C,32MB-L3]  CCD5  CCD6  CCD7  (仍 NUMA Node 0)  │
  │  DDR5 × 12 channels                                     │
  └─────────────────────────────────────────────────────────┘
  BIOS "NPS4 mode" → 4个 NUMA 子域, 更低的跨-CCD 延迟

  最优性能: 每个线程绑定到 CCD, 内存分配在本地 NUMA 域

【NUMA 感知编程技术】

  1. numactl 命令行绑定:
     numactl --cpunodebind=0 --membind=0 ./app    # 绑定 Node 0
     numactl --interleave=all ./app               # 内存跨 NUMA 轮询 (均匀带宽)

  2. C API (libnuma):
     #include <numa.h>
     // 检查 NUMA 可用性
     if (numa_available() < 0) { printf("No NUMA\n"); return; }

     // 分配本地内存
     void* p = numa_alloc_local(size);          // 在当前 CPU 的 NUMA 节点分配
     void* p = numa_alloc_onnode(size, node);   // 指定 node

     // 线程绑定
     numa_run_on_node(0);      // 当前线程绑定到 node 0
     numa_set_membind(mask);   // 内存分配限制在 mask 指定节点

  3. First-Touch 策略 (Linux 默认):
     // 内存 *第一次被触碰* 的线程决定它属于哪个 NUMA 节点
     // 陷阱: 初始化线程在 Node 0, 工作线程在 Node 1 → 全部远端访问!
     //
     // 最佳实践: 由工作线程自己初始化数据
     #pragma omp parallel for  // 工作线程同时初始化
     for (int i=0; i<N; ++i) data[i] = 0.0f;  // first touch = 本线程所在node

  4. 迁移已有内存页:
     // mbind() 迁移内存到指定节点
     mbind(addr, size, MPOL_BIND, &nodemask, maxnode, MPOL_MF_MOVE);

  5. 内存策略:
     MPOL_LOCAL:      优先本地 (推荐高性能)
     MPOL_BIND:       严格绑定指定节点 (低延迟, 但内存可能不足)
     MPOL_INTERLEAVE: 轮询分配 (高带宽工作负载)
     MPOL_PREFERRED:  首选某节点, 满时其他节点

【检测 NUMA 拓扑】
  numactl --hardware          # 显示 NUMA 节点、距离矩阵
  lstopo --of ascii           # 完整拓扑 (hwloc)
  cat /sys/devices/system/node/node0/distance  # NUMA 距离表

【NUMA 性能调优原则】
  1. 线程亲和性: 将相关线程绑定到同一 socket/CCD
  2. 数据本地性: First-touch 优化 + 使用 numa_alloc_local
  3. 带宽均衡: 有 NUMA interleave 均衡带宽 (适合 HPC/KV 场景)
  4. 容量规划: 预留 NUMA 本地内存余量 (防溢出到远端)
)";
}

} // namespace ch45

// =============================================================================
// 第 46 章：编译器对微架构的认知与精细控制
// =============================================================================
namespace ch46 {

// 演示不同优化属性的效果
FORCE_INLINE int hot_compute(int a, int b) {
    int result = 0;
    for (int i = 0; i < 64; ++i)
        result += (a * i + b * (i ^ 3)) & 0xFF;
    return result;
}

[[gnu::cold, gnu::noinline]]
static void cold_error_handler(const char* msg) {
    std::cout << "  Error: " << msg << "\n";
}

void demo() {
    std::cout << "\n[CH46] 编译器对微架构的认知与精细控制\n";
    std::cout << R"(
=== 第 46 章：编译器微架构感知编译优化 ===

【-march 目标架构选项】

  # 通用 (推荐发行版二进制)
  -march=x86-64-v3    # AVX2+FMA+BMI2, ~2013+ CPU
  -march=x86-64-v4    # AVX-512, ~2017+ Intel

  # 精确目标
  -march=znver4       # AMD Zen 4 (精确调度模型)
  -march=sapphirerapids  # Intel Sapphire Rapids (Xeon 4th Gen)
  -march=goldencove   # Intel Alder Lake P-core
  -march=native       # 本机 CPU (不可移植, 最高性能)

  # 查看 native 启用了哪些特性
  g++ -Q --help=target -march=native 2>&1 | grep enabled

【-mtune vs -march 区别】
  -march=X: 同时设置指令集 + 调度模型 (不能用于更旧的CPU)
  -mtune=X: 只调整调度策略 (指令集不变, 二进制可跑在旧CPU上)
  
  推荐:
  -march=x86-64-v3 -mtune=znver4   # 向量化用 AVX2, 但调度按 Zen4 优化

【关键编译器 flags】

  性能相关:
  -O3               # 开启全部优化 (含激进循环变换)
  -Ofast            # -O3 + -ffast-math + -fno-protect-parens
  -ffast-math       # 允许浮点重排 (不严格 IEEE754)
  -fno-math-errno   # 数学函数不设 errno (少一个写)
  -funroll-loops    # 循环展开 (增大代码体积!)
  -fomit-frame-pointer  # 释放 RBP 为通用寄存器

  调试/分析:
  -fno-inline       # 禁止内联 (方便 perf 归因)
  -pg               # gprof 插桩
  -fprofile-generate / -fprofile-use  # PGO 两步编译

【函数级别精细控制】

  // 标记热/冷函数 (影响代码布局)
  __attribute__((hot))   void hot_func() { /* 频繁调用 */ }
  __attribute__((cold))  void cold_func() { /* 错误处理 */ }

  // 指定函数级优化 (覆盖全局 -O 设置)
  __attribute__((optimize("O3,unroll-loops,tree-vectorize")))
  void critical_loop(float* a, float* b, int n) {
      for (int i=0; i<n; ++i) a[i] += b[i];
  }

  // 强制内联 / 禁止内联
  __attribute__((always_inline)) inline int fast_min(int a,int b){ return a<b?a:b; }
  __attribute__((noinline))      void isolated_func() { /* 不要内联我 */ }

  // 目标属性 (函数使用特定 ISA, 即使全局没有 -mavx2)
  __attribute__((target("avx2,fma")))
  void avx2_kernel(float* a, float* b, int n) {
      for (int i=0; i<n; i+=8) {
          __m256 va = _mm256_loadu_ps(a+i);
          __m256 vb = _mm256_loadu_ps(b+i);
          _mm256_storeu_ps(a+i, _mm256_add_ps(va, vb));
      }
  }

【PGO 完整流程 (Profile Guided Optimization)】

  步骤 1: 插桩编译
  g++ -O2 -fprofile-generate -fprofile-dir=./pgo_data \
      -march=native -o app app.cpp

  步骤 2: 代表性输入运行 (收集 profile)
  ./app < production_like_input1
  ./app < production_like_input2

  步骤 3: 利用 profile 优化
  g++ -O3 -fprofile-use=./pgo_data -fprofile-correction \
      -march=native -o app_pgo app.cpp

  PGO 带来的优化:
  ✓ 内联决策: 热函数优先内联
  ✓ 分支布局: 热路径 fall-through (无跳转)
  ✓ 代码布局: 热函数聚集 (I-Cache 友好)
  ✓ 循环展开: 热循环更激进展开
  典型收益: +5%~15%

【AutoFDO (基于 perf 的 PGO)】

  # 不需要插桩, 用生产环境 perf 数据!
  perf record -g -e cycles:u -o perf.data ./app
  create_llvm_prof --binary=./app --profile=perf.data --out=app.afdo
  clang++ -O3 -fprofile-sample-use=app.afdo -o app_afdo app.cpp

【LLVM-MCA 静态分析】

  # 分析一段汇编的理论 IPC 和瓶颈
  g++ -O3 -march=znver4 -S -o - inner_loop.cpp | \
      llvm-mca -mcpu=znver4 -bottleneck-analysis

  # 关键输出:
  # Timeline: 每条指令的执行时间线
  # Resource pressure: 哪个执行端口是瓶颈
  # 每次迭代的理论 cycles

  # 用注释标记分析范围:
  // clang: #pragma clang loop vectorize(assume_safety) unroll(full)
  asm volatile("# LLVM-MCA-BEGIN inner_loop");
  for (int i=0; i<N; ++i) c[i] = a[i] * b[i] + c[i];
  asm volatile("# LLVM-MCA-END inner_loop");

【Compiler Explorer (Godbolt) 工作流】
  1. 粘贴热循环代码到 godbolt.org
  2. 选择编译器+flags
  3. 查看生成汇编:
     - vmulps, vaddps → 已向量化 (AVX)
     - vfmadd231ps    → FMA 使用 ✓
     - rep movsb      → 字符串/memcpy 优化 ✓
     - imul           → 整数乘法 (3 cyc)
     - idiv           → 整数除法 (20-90 cyc!) ← 重点关注
  4. 在 godbolt 中 diff 两个版本的汇编

【pragma hint (GCC/Clang 向量化控制)】
  #pragma GCC optimize("O3,unroll-loops")
  #pragma GCC target("avx2,fma")

  // LLVM/Clang 专属
  #pragma clang loop vectorize(enable)
  #pragma clang loop unroll_count(4)
  #pragma clang loop interleave_count(2)

  // 标准 C++17/20
  for (int i=0; i<N; ++i)
      a[i] += b[i];  // 编译器会自动 SLP 向量化
)";

    // 演示 hot/cold 函数效果
    volatile int sink = 0;
    for (int i = 0; i < 1000; ++i)
        sink += hot_compute(i, i*2+1);
    (void)sink;
    std::cout << "  热函数执行示例完成 (FORCE_INLINE + 64次迭代)\n";
    cold_error_handler("演示冷路径函数 (cold attribute)");
}

} // namespace ch46

// =============================================================================
// main
// =============================================================================

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cout << "================================================================\n";
    std::cout << " CPU 微架构深度解析 — 缓存·分支预测·乱序执行·流水线优化\n";
    std::cout << "================================================================\n";

    // 一、流水线基础
    print_header("一、CPU 流水线基础篇");
    ch1::demo();
    ch2::demo();
    ch3::demo();
    ch4::demo();

    // 二、缓存层级
    print_header("二、缓存层级篇");
    ch5::demo();
    ch6::demo();
    ch7::demo();
    ch8::demo();
    ch9::demo();
    ch10::demo();
    ch11::demo();
    ch12::demo();
    ch13::demo();

    // 三、TLB
    print_header("三、TLB 与虚拟内存篇");
    ch14::demo();
    ch15::demo();
    ch16::demo();

    // 四、分支预测
    print_header("四、分支预测篇");
    ch17::demo();
    ch18::demo();
    ch19::demo();
    ch20::demo();
    ch21::demo();
    ch22::demo();
    ch23::demo();
    ch24::demo();

    // 五、乱序执行
    print_header("五、乱序执行篇");
    ch25::demo();
    ch26::demo();
    ch27::demo();
    ch28::demo();
    ch29::demo();
    ch30::demo();
    ch31::demo();
    ch32::demo();

    // 六、前端优化
    print_header("六、前端优化篇");
    ch33::demo();
    ch34::demo();
    ch35::demo();
    ch36::demo();

    // 七、性能分析
    print_header("七、性能计数器与分析篇");
    ch37::demo();
    ch38::demo();
    ch39::demo();
    ch40::demo();

    // 八、扩展专题
    print_header("八、扩展专题篇");
    ch41::demo();
    ch42::demo();
    ch43::demo();
    ch44::demo();
    ch45::demo();
    ch46::demo();

    std::cout << "\n================================================================\n";
    std::cout << " 演示完成\n";
    std::cout << "================================================================\n";

    return 0;
}
