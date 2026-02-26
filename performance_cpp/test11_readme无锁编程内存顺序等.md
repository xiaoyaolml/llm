# 无锁编程、内存顺序、原子操作及并发数据结构 — 完全教程

> **配套代码**: `test11.cpp` (2929 行, 25 章, C++17)
>
> **编译**:
> ```bash
> # Linux / macOS
> g++ -std=c++17 -O2 -pthread -o test11 test11.cpp
>
> # Windows MSVC
> cl /std:c++17 /O2 /EHsc test11.cpp
>
> # 带 ThreadSanitizer（调试数据竞争）
> g++ -std=c++17 -fsanitize=thread -g -O1 -pthread -o test11_tsan test11.cpp
>
> # 带 AddressSanitizer（调试内存错误）
> g++ -std=c++17 -fsanitize=address -g -O1 -pthread -o test11_asan test11.cpp
> ```

---

## 目录

| 篇 | 章 | 主题 | 核心内容 | 难度 |
|---|---|---|---|---|
| **一、原子操作基础** | 1 | `std::atomic` 基础 | load/store/exchange/CAS/lock_free 检查 | ⭐⭐ |
| | 2 | 原子类型完全手册 | 整型/指针/浮点/`atomic_flag`/`atomic_ref`/用户定义类型 | ⭐⭐ |
| | 3 | RMW 操作详解 | fetch_add/sub/and/or/xor + 原子位标志 | ⭐⭐⭐ |
| | 4 | CAS 循环与 ABA 问题 | `atomic_update` 通用模板、TaggedPtr 解决 ABA | ⭐⭐⭐⭐ |
| **二、内存顺序** | 5 | CPU/编译器重排 | Store-Load 重排演示、x86 TSO vs ARM 弱序 | ⭐⭐⭐⭐ |
| | 6 | 六种 memory_order | relaxed→seq_cst 逐一精讲 + 性能对比 | ⭐⭐⭐⭐⭐ |
| | 7 | Acquire-Release 深入 | SPSC 无锁队列 + 同步路径详细标注 | ⭐⭐⭐⭐ |
| | 8 | Release Sequence 与传递性 | shared_ptr 引用计数应用 + 多 worker 传递 | ⭐⭐⭐⭐⭐ |
| | 9 | memory_order_consume | 数据依赖链、ARM 优势、编译器现状 | ⭐⭐⭐ |
| | 10 | volatile vs atomic | 对比表、经典错误、C++20 弃用 | ⭐⭐⭐ |
| | 11 | 独立内存屏障 (fence) | `atomic_thread_fence` + x86 编译结果分析 | ⭐⭐⭐⭐ |
| **三、无锁算法** | 12 | 自旋锁三种实现 | TAS / TTAS / Ticket Lock + mutex 基准对比 | ⭐⭐⭐ |
| | 13 | Treiber Stack | 完整无锁栈 push/pop + 多线程验证 | ⭐⭐⭐⭐ |
| | 14 | Michael-Scott Queue | MPMC 无锁队列 + 帮助推进 (help-advance) | ⭐⭐⭐⭐⭐ |
| | 15 | 无锁哈希表 | 分桶原子链表、CAS 插入 + acquire 查找 | ⭐⭐⭐⭐ |
| | 16 | 无锁环形缓冲区 | SPSC + MPMC Vyukov 两种实现 | ⭐⭐⭐⭐ |
| **四、内存回收** | 17 | Hazard Pointer | 完整域实现 + 并发读写演示 | ⭐⭐⭐⭐⭐ |
| | 18 | Epoch-Based Reclamation | 三 epoch 轮转 + RAII Guard | ⭐⭐⭐⭐⭐ |
| | 19 | Split Reference Count | 外部/内部计数原理 + shared_ptr 并发测试 | ⭐⭐⭐⭐ |
| **五、高级并发结构** | 20 | SeqLock (序列锁) | 写者极少场景最优、一致性 + 重试验证 | ⭐⭐⭐⭐ |
| | 21 | 用户态 RCU | SimpleRCU 模板 + Config 热更新演示 | ⭐⭐⭐⭐⭐ |
| | 22 | 并发跳表 | lock-free 查找 + mutex 写入 + 多线程测试 | ⭐⭐⭐⭐ |
| **六、实战与调试** | 23 | 常见 Bug 模式 | 8 大 Bug 模式与修复方案 | ⭐⭐⭐ |
| | 24 | 性能基准测试 | mutex/atomic/sharded/thread_local 五档对比 | ⭐⭐⭐ |
| | 25 | 工具链 | TSan/ASAN/Relacy/CDSChecker + 检查清单 | ⭐⭐⭐ |

---

## 文件关系

| 文件 | 主题 | 行数 | 章节 | 与本文件关系 |
|---|---|---|---|---|
| `test6.cpp` / `test6_readme` | 多线程基础 | ~2400 | 18 章 | **前置依赖** — 线程/互斥锁/条件变量基础 |
| `test9.cpp` / `test9_readme` | 高并发设计 | - | - | **并行参考** — 并发服务器/连接池/IO |
| **`test11.cpp`** / **本文件** | **无锁编程** | **2929** | **25 章** | **当前** |
| `test12.cpp` / `test12_readme` | 零开销抽象 | - | - | **进阶** — 编译期+运行期极致优化 |
| `test13.cpp` / `test13_readme` | 微秒系统 | - | - | **目标应用** — 低延迟实战 |
| `test14.cpp` / `test14_readme` | CPU微架构 | - | - | **底层支撑** — cache/pipeline/分支预测 |

---

# 一、原子操作基础篇

---

## 第 1 章：`std::atomic` 基础 — load/store/exchange/CAS

### 1.1 核心概念

`std::atomic<T>` 是 C++11 引入的**原子类型模板**，保证对 T 的操作在任何平台上都是**不可分割的** (indivisible)——其他线程要么看到操作完成前的状态，要么看到完成后的状态，绝不会看到**中间态** (torn read/write)。

**关键特性：**

| 特性 | 说明 |
|---|---|
| **原子性** | 整个操作不可被中断（硬件级保证） |
| **可见性** | 配合 memory_order 控制跨核可见性 |
| **有序性** | 建立 happens-before 关系 |
| **无锁可能** | `is_lock_free()` 检查是否在硬件级无锁 |

### 1.2 五种基本操作

```cpp
std::atomic<int> x(0);

// 1. load — 原子读取
int val = x.load(std::memory_order_acquire);

// 2. store — 原子写入
x.store(42, std::memory_order_release);

// 3. exchange — 原子交换，返回旧值
int old = x.exchange(100);

// 4. compare_exchange_strong — 强 CAS
int expected = 100;
bool ok = x.compare_exchange_strong(expected, 200);
// ok=true → x 从 100 变为 200
// ok=false → expected 被更新为 x 的当前值

// 5. compare_exchange_weak — 弱 CAS (可伪失败，用于循环)
while (!x.compare_exchange_weak(expected, desired)) {
    // expected 会被自动更新
}
```

### 1.3 `is_lock_free()` 检查

```cpp
struct Big { int data[100]; };

std::atomic<int> ai;
std::atomic<long long> all;
std::atomic<int*> ap;
std::atomic<Big> ab;

// x86-64 典型结果:
ai.is_lock_free();   // true  — 4字节, 单条 MOV
all.is_lock_free();  // true  — 8字节, 单条 MOV (64位)
ap.is_lock_free();   // true  — 8字节指针
ab.is_lock_free();   // false — 400字节, 内部用 mutex!
```

> **硬件原理**: x86-64 上，**≤8 字节**且**自然对齐**的 load/store 天然是原子的。超过此大小的 `std::atomic<T>` 回退为内部锁实现。

### 1.4 代码示例 — 4 线程原子自增验证

```cpp
// test11.cpp ch1::demo_atomic_basics()
std::atomic<int> counter(0);
constexpr int N = 100000;
constexpr int T = 4;

std::vector<std::thread> threads;
for (int i = 0; i < T; ++i) {
    threads.emplace_back([&] {
        for (int j = 0; j < N; ++j)
            counter.fetch_add(1, std::memory_order_relaxed);
    });
}
for (auto& t : threads) t.join();

// counter == 400000 ✓ (无锁, 无丢失)
```

### 1.5 深入扩展

**CAS 的 strong vs weak 选择策略：**

| 场景 | 选择 | 原因 |
|---|---|---|
| 循环内 CAS | `weak` | 伪失败代价低，循环会重试；ARM 上 LL/SC 更高效 |
| 单次判断 | `strong` | 不允许伪失败；x86 上两者无区别（都是 LOCK CMPXCHG） |
| 嵌套 CAS | `strong` | 伪失败可能导致外层逻辑错误 |

**x86 vs ARM 原子操作的硬件差异：**

```
x86:  LOCK CMPXCHG [mem], reg    // 一条指令完成 CAS
ARM:  LDXR  → 计算 → STXR        // LL/SC 指令对, STXR 可能失败 (weak 自然)
      ↑                   ↑
    Load-Exclusive   Store-Exclusive (可被其他核 invalidate)
```

---

## 第 2 章：原子类型完全手册

### 2.1 类型总览

| 类型 | 支持操作 | lock_free | 典型用途 |
|---|---|---|---|
| `atomic<bool>` | load/store/exchange/CAS | ✅ | 标志位、状态开关 |
| `atomic<int>` | 全部 + fetch_add/sub/and/or/xor | ✅ | 计数器、位标志 |
| `atomic<int*>` | 全部 + fetch_add/sub (指针算术) | ✅ | 无锁链表指针 |
| `atomic<float>` (C++20) | load/store/exchange/CAS | ✅ | 浮点累加器 |
| `atomic<UserType>` | load/store/exchange/CAS | 取决于大小 | 复合状态 |
| `atomic_flag` | test_and_set/clear | ✅ 保证 | 最简自旋锁 |
| `atomic_ref<T>` (C++20) | 给非原子变量加原子访问 | 取决于 T | 遗留代码升级 |

### 2.2 `atomic_flag` — 唯一保证 lock-free 的类型

```cpp
std::atomic_flag flag = ATOMIC_FLAG_INIT;  // 初始 clear

// test_and_set: 设为 true, 返回旧值
bool was_set = flag.test_and_set(std::memory_order_acquire);

// clear: 设为 false
flag.clear(std::memory_order_release);

// 最简自旋锁:
while (flag.test_and_set(std::memory_order_acquire))
    ; // 忙等
// ... 临界区 ...
flag.clear(std::memory_order_release);
```

### 2.3 指针原子运算

```cpp
int arr[10] = {};
std::atomic<int*> ptr(arr);

int* old = ptr.fetch_add(3);  // ptr 向前移动 3 个 int
// old == arr, ptr 现在指向 arr[3]

ptr.fetch_sub(1);  // 回退 1 个, 现在指向 arr[2]
```

### 2.4 用户定义类型

```cpp
struct Point { float x, y; };  // trivially copyable, 8 字节

std::atomic<Point> pos({0.0f, 0.0f});

Point p = pos.load();
pos.store({1.0f, 2.0f});
pos.compare_exchange_strong(p, {3.0f, 4.0f});

// 注意: sizeof(Point) <= 8, x86-64 上 lock-free
//       如果 sizeof > 16, 回退为 mutex
```

### 2.5 深入扩展

**`atomic_ref<T>` (C++20) 的价值——无入侵式原子化：**

```cpp
int plain_var = 0;  // 已有的非原子变量

// 不修改 plain_var 的声明，临时获得原子能力
std::atomic_ref<int> ref(plain_var);
ref.fetch_add(1, std::memory_order_relaxed);

// 使用场景: 老代码中的 int 变量需要线程安全但无法改声明
// 要求: plain_var 必须满足 alignment 要求 (alignof(atomic_ref<int>))
```

**C++20 `atomic<float>/atomic<double>` 注意事项：**
- 支持 `fetch_add`/`fetch_sub`（浮点原子累加）
- 不支持 `fetch_and`/`fetch_or`/`fetch_xor`（位操作对浮点无意义）
- x86 上实现为 CAS 循环（硬件无浮点原子指令）

---

## 第 3 章：RMW 操作详解 — fetch_add/sub/and/or/xor

### 3.1 Read-Modify-Write (RMW) 概念

RMW 操作在**单个原子步骤**中完成：读旧值 → 计算新值 → 写入新值。其他线程无法在读和写之间插入。

```cpp
std::atomic<int> x(10);

int old;
old = x.fetch_add(5);    // old=10, x=15
old = x.fetch_sub(3);    // old=15, x=12
old = x.fetch_and(0xFF); // old=12, x=12 (12 & 0xFF = 12)
old = x.fetch_or(0x100); // old=12, x=268 (12 | 0x100)
old = x.fetch_xor(0xFF); // old=268, x=243 (268 ^ 0xFF)

// 便捷运算符:
x++;   // 等价于 x.fetch_add(1)
x--;   // 等价于 x.fetch_sub(1)
x += 5;  // 等价于 x.fetch_add(5)
```

### 3.2 原子位标志 — 多状态管理

```cpp
// test11.cpp ch3 — 原子位标志
constexpr int FLAG_INIT  = 1 << 0;  // 0x01
constexpr int FLAG_READY = 1 << 1;  // 0x02
constexpr int FLAG_ERROR = 1 << 2;  // 0x04

std::atomic<int> status(0);

// 设置标志 (不影响其他位):
status.fetch_or(FLAG_INIT, std::memory_order_release);

// 清除标志:
status.fetch_and(~FLAG_ERROR, std::memory_order_release);

// 检测标志:
if (status.load(std::memory_order_acquire) & FLAG_READY) {
    // 就绪
}

// 原子地设置新标志 + 检查旧标志:
int old = status.fetch_or(FLAG_READY, std::memory_order_acq_rel);
if (old & FLAG_ERROR) {
    // 在设置 READY 之前已经有 ERROR
}
```

### 3.3 RMW 的 memory_order 语义

| memory_order | RMW 行为 |
|---|---|
| `relaxed` | 仅保证原子性，不保证顺序 |
| `acquire` | RMW 的**读**部分带 acquire |
| `release` | RMW 的**写**部分带 release |
| `acq_rel` | 读 acquire + 写 release（最常用） |
| `seq_cst` | 全序，最安全也最慢 |

```cpp
// CAS 有两个 memory_order: 成功和失败
x.compare_exchange_strong(expected, desired,
    std::memory_order_acq_rel,   // 成功时: 读 acquire + 写 release
    std::memory_order_acquire);  // 失败时: 仅读 (acquire)
```

### 3.4 深入扩展

**为什么 RMW 不能拆成 load + compute + store？**

```
线程A: load(x) → 42       线程B: load(x) → 42
线程A: 42 + 1 = 43         线程B: 42 + 1 = 43
线程A: store(x, 43)        线程B: store(x, 43)
// 结果: x = 43, 丢失了一次加!

// fetch_add 保证:
线程A: fetch_add(1) → 42, x=43
线程B: fetch_add(1) → 43, x=44  ✓
```

**x86 上的 RMW 汇编（`fetch_add(1)`）：**
```asm
lock add DWORD PTR [rdi], 1     ; 单条指令, LOCK 前缀锁住 cache line
; 或 (当需要返回旧值时):
lock xadd DWORD PTR [rdi], eax  ; 交换并相加
```

---

## 第 4 章：CAS 循环模式与 ABA 问题

### 4.1 通用 CAS 循环模板

CAS (Compare-And-Swap) 循环是无锁编程的**核心模式**——当没有直接的原子操作可用时，用 CAS 构造任意原子更新：

```cpp
// test11.cpp ch4 — 通用 CAS 循环
template<typename T, typename Func>
T atomic_update(std::atomic<T>& target, Func func) {
    T old_val = target.load(std::memory_order_relaxed);
    T new_val;
    do {
        new_val = func(old_val);
    } while (!target.compare_exchange_weak(old_val, new_val,
                std::memory_order_release,
                std::memory_order_relaxed));
    return old_val;
}

// 使用: 原子最大值
std::atomic<int> max_val(0);
atomic_update(max_val, [](int cur) { return std::max(cur, 42); });
```

**CAS 循环的必要性分析:**

```
                        ┌──────────────┐
                        │ load old_val │
                        └──────┬───────┘
                               │
                        ┌──────▼───────┐
                        │ compute new  │
                        │ = func(old)  │
                        └──────┬───────┘
                               │
                    ┌──────────▼──────────┐
                    │ CAS(old → new)?     │
                    │ (其他线程是否已修改?)│
                    └──────┬──────┬───────┘
                     成功 │      │ 失败
                     ┌────▼──┐   │ (old 自动更新)
                     │ 返回  │   │
                     └───────┘   └────→ 重试循环
```

### 4.2 ABA 问题详解

**问题场景 — 无锁栈的 pop()：**

```
初始状态: Stack → A → B → C

线程1: 开始 pop(), 读到 head = A, next = B
        (被挂起)

线程2: pop() A               → Stack: B → C
线程2: pop() B               → Stack: C
线程2: push(A)               → Stack: A → C
        (A 被重用, 但 A->next 现在是 C, 不是 B!)

线程1: 恢复, CAS(head, A → B)
        → CAS 成功! (head 确实还是 A)
        → 但 head 现在指向 B, 而 B 已经被 free!
        → 💥 Use-after-free
```

### 4.3 解决方案 — Tagged Pointer

```cpp
// test11.cpp ch4 — TaggedPtr 解决 ABA
struct TaggedPtr {
    uintptr_t ptr  : 48;  // 指针 (x86-64 只用 48 位)
    uintptr_t tag  : 16;  // 版本号 (0~65535)
};

std::atomic<TaggedPtr> head;

// push:
TaggedPtr old_head = head.load();
TaggedPtr new_head = {
    reinterpret_cast<uintptr_t>(new_node),
    old_head.tag + 1                        // 版本号递增!
};
while (!head.compare_exchange_weak(old_head, new_head)) {
    new_head.tag = old_head.tag + 1;  // 更新版本号
}
// CAS 同时比较 ptr + tag, 防止 ABA
```

### 4.4 ABA 解决方案对比

| 方案 | 原理 | 优点 | 缺点 |
|---|---|---|---|
| **Tagged Pointer** | 指针高位存版本号 | 简单高效 | 版本号可溢出; x86-64 限 16 位 |
| **DWCAS** | CAS 128 位 (ptr+counter) | 版本空间大 | 不是所有平台支持 |
| **Hazard Pointer** | 保护正在使用的指针 | 无版本号溢出 | 实现复杂 (第 17 章) |
| **EBR** | 基于 epoch 延迟回收 | 高吞吐 | 写者可能等待迟钝读者 (第 18 章) |

### 4.5 深入扩展

**Double-Width CAS (DWCAS) 在 x86-64 上：**

```asm
; CMPXCHG16B — 比较并交换 128 位
lock cmpxchg16b [rdi]
; 需要: -mcx16 编译选项
; RDX:RAX = expected, RCX:RBX = desired
```

```cpp
// GCC/Clang 128-bit CAS:
struct PtrWithCounter {
    void* ptr;
    uint64_t counter;
} __attribute__((aligned(16)));

std::atomic<PtrWithCounter> head;  // 需要 lock-free 128-bit CAS
```

---

# 二、内存顺序篇

---

## 第 5 章：CPU 指令重排与编译器重排

### 5.1 两种重排来源

**编译器重排** — 在 as-if 规则下，编译器可以重排不相关的操作：

```cpp
int a = 1;    // 编译器可能把这两条
int b = 2;    // 调换顺序 (单线程无区别)

// 但多线程中:
data = 42;
ready = true;
// 如果编译器重排为 ready=true; data=42;
// 另一个线程看到 ready==true 时 data 可能还是 0!
```

**CPU 重排** — 不同架构允许不同程度的重排：

| 架构 | 允许的重排 | 模型强度 |
|---|---|---|
| **x86/x86-64** | 仅 Store→Load 可重排 | TSO (强序) |
| **ARM/AArch64** | Load→Load, Load→Store, Store→Store, Store→Load | 弱序 |
| **RISC-V** | 与 ARM 类似 (默认弱序) | 弱序 + 可选 TSO |
| **POWER** | 全部四种 + 更多 | 极弱序 |

### 5.2 x86 TSO 模型详解

```
            ┌─────────────┐      ┌─────────────┐
            │    Core 0   │      │    Core 1   │
            │  ┌───────┐  │      │  ┌───────┐  │
            │  │ L1/L2 │  │      │  │ L1/L2 │  │
            │  └───┬───┘  │      │  └───┬───┘  │
            │  ┌───▼───┐  │      │  ┌───▼───┐  │
            │  │ Store  │  │      │  │ Store  │  │
            │  │ Buffer │  │      │  │ Buffer │  │
            │  └───┬───┘  │      │  └───┬───┘  │
            └──────┼──────┘      └──────┼──────┘
                   │        MESI        │
                   └───────┬────────────┘
                           │
                    ┌──────▼──────┐
                    │ 共享 L3/内存 │
                    └─────────────┘

TSO 规则:
  ✅ Load 不会越过 Load    (Load→Load 有序)
  ✅ Store 不会越过 Store   (Store→Store 有序)
  ✅ Load 不会越过 Store    (Load→Store 有序)
  ❌ Store 可能被后续 Load 越过 (Store→Load 可重排!)
     → 原因: Store 还在 Store Buffer 里, 但 Load 已经执行了
```

### 5.3 Store-Load 重排演示

```cpp
// test11.cpp ch5 — Store-Load 重排实验
std::atomic<int> x(0), y(0);
std::atomic<int> both_zero(0);

for (int iter = 0; iter < 100000; ++iter) {
    x.store(0, std::memory_order_relaxed);
    y.store(0, std::memory_order_relaxed);

    std::thread t1([&] {
        x.store(1, std::memory_order_relaxed);  // Store x
        int r1 = y.load(std::memory_order_relaxed);  // Load y
        if (r1 == 0) /* ... */;
    });

    std::thread t2([&] {
        y.store(1, std::memory_order_relaxed);  // Store y
        int r2 = x.load(std::memory_order_relaxed);  // Load x
        if (r2 == 0) /* ... */;
    });

    // 如果 r1==0 && r2==0 → Store-Load 重排发生!
    // 在 seq_cst 下不可能, 在 relaxed 下可能!
}
```

### 5.4 深入扩展

**为什么 x86 只有 Store-Load 重排？**
- **Store Buffer** 是唯一原因：写入先进 Store Buffer，对本核 Load 可见（store forwarding），但对其他核**不可见**
- **解决**: `MFENCE` 指令强制刷空 Store Buffer
- `seq_cst` store 在 x86 上编译为 `MOV + MFENCE` 或 `XCHG`（隐含 MFENCE 语义）

**MESI 协议 cache line 状态：**

| 状态 | 含义 |
|---|---|
| **M** (Modified) | 本核独有且已修改，需回写 |
| **E** (Exclusive) | 本核独有，未修改 |
| **S** (Shared) | 多核共享，只读 |
| **I** (Invalid) | 无效，需从其他核/内存获取 |

---

## 第 6 章：六种 memory_order 逐一精讲

### 6.1 总览表

| memory_order | 含义 | 开销 (x86) | 开销 (ARM) | 适用场景 |
|---|---|---|---|---|
| `relaxed` | 仅原子性 | 零 (普通 MOV) | 零 | 计数器、统计 |
| `consume` | 数据依赖链 acquire | 零 | 零 | (极少用, 见第 9 章) |
| `acquire` | 本线程后续读写不前移 | 零 (x86 自带) | DMB ISH LD | load 端 |
| `release` | 本线程之前读写不后移 | 零 (x86 自带) | DMB ISH ST | store 端 |
| `acq_rel` | acquire + release | 零 (x86 自带) | DMB ISH | RMW 操作 |
| `seq_cst` | 全序 (所有线程看到相同顺序) | MFENCE / XCHG | DMB ISH | 默认, 最安全 |

### 6.2 Acquire-Release 经典配对

```cpp
// test11.cpp ch6 — acquire-release 经典
std::atomic<bool> ready(false);
int data = 0;

// 生产者:
data = 42;                                    // ① 普通写
ready.store(true, std::memory_order_release); // ② release store
// release 保证: ① 在 ② 之前对其他线程可见

// 消费者:
while (!ready.load(std::memory_order_acquire))  // ③ acquire load
    ;
assert(data == 42);                            // ④ 保证看到 42

// 因为 ②→③ 构成 synchronizes-with 关系:
// ① happens-before ② (程序顺序)
// ② synchronizes-with ③ (release-acquire)
// ③ happens-before ④ (程序顺序)
// 传递: ① happens-before ④ ✓
```

### 6.3 seq_cst vs relaxed 性能对比

```cpp
// test11.cpp ch6 — 10M 次单线程 fetch_add
std::atomic<long long> counter(0);

// seq_cst:
for (int i = 0; i < 10'000'000; ++i)
    counter.fetch_add(1, std::memory_order_seq_cst);
// x86: LOCK XADD (隐含 full barrier)

// relaxed:
for (int i = 0; i < 10'000'000; ++i)
    counter.fetch_add(1, std::memory_order_relaxed);
// x86: LOCK XADD (同样! x86 的 LOCK 指令本身就是 full barrier)
```

> **x86 的"秘密"**: 在 x86 上，`LOCK` 前缀指令（用于所有 RMW）本身就提供了 full barrier 语义。因此 `relaxed` 和 `seq_cst` 的 RMW 在 x86 上**性能几乎相同**。差异主要体现在 ARM 等弱序架构上。

### 6.4 深入扩展

**memory_order 影响的是 fence，不是操作本身的原子性：**
- 所有 `std::atomic` 操作**无论用什么 memory_order**都保证原子性
- memory_order 只控制**操作之间的可见性和顺序**
- `relaxed` 不是"不安全"——只是"不保证其他变量的可见性"

**Dekker's Algorithm 为什么需要 seq_cst：**

```cpp
// 两个线程试图同时进入临界区
// 线程0:                  线程1:
flag[0].store(true);       flag[1].store(true);
if (!flag[1].load()) {     if (!flag[0].load()) {
    // 临界区                  // 临界区
}                          }

// 如果用 acquire-release: 两个 store-load 之间无顺序!
// 可能两个线程都进入临界区!
// 必须用 seq_cst 保证全局一致顺序
```

---

## 第 7 章：Acquire-Release 深入 — SPSC 无锁队列

### 7.1 SPSC 队列实现与同步标注

这是一个**单生产者-单消费者**无锁队列，仅靠 acquire-release 语义保证正确性：

```cpp
// test11.cpp ch7 — SPSC 无锁队列 (简化版)
template<typename T, int Cap>
class SPSCQueue {
    T buffer_[Cap];
    std::atomic<int> write_pos_{0};  // 只有生产者写
    std::atomic<int> read_pos_{0};   // 只有消费者写

public:
    bool push(const T& val) {
        int w = write_pos_.load(std::memory_order_relaxed);      // ① 生产者读自己的指针
        int next = (w + 1) % Cap;
        if (next == read_pos_.load(std::memory_order_acquire))   // ② acquire: 获取消费者的进度
            return false;  // 满
        buffer_[w] = val;                                         // ③ 写入数据
        write_pos_.store(next, std::memory_order_release);       // ④ release: 发布给消费者
        return true;
    }

    bool pop(T& val) {
        int r = read_pos_.load(std::memory_order_relaxed);       // ⑤ 消费者读自己的指针
        if (r == write_pos_.load(std::memory_order_acquire))     // ⑥ acquire: 获取生产者的进度
            return false;  // 空
        val = buffer_[r];                                         // ⑦ 读取数据
        read_pos_.store((r + 1) % Cap, std::memory_order_release); // ⑧ release: 通知生产者
        return true;
    }
};
```

### 7.2 同步路径分析

```
生产者:                                消费者:
  ③ buffer_[w] = val                     
  ④ write_pos_.store(release) ──sync──→ ⑥ write_pos_.load(acquire)
                                         ⑦ val = buffer_[r]
  ② read_pos_.load(acquire) ←──sync──── ⑧ read_pos_.store(release)

  ④ release 保证: ③ 对消费者在 ⑥ acquire 后可见 → ⑦ 读到正确数据
  ⑧ release 保证: ⑦ 的读取完成后才更新 read_pos → ② 看到新空位时旧数据已被消费
```

### 7.3 深入扩展

**为什么 ① 和 ⑤ 用 `relaxed`？**
- 生产者只有自己写 `write_pos_`，读自己写的值不需要跨线程同步
- 消费者同理只有自己写 `read_pos_`
- **原则**: 只有跨线程通信的变量才需要 acquire/release

**SPSC vs MPMC 的同步复杂度对比：**

| 类型 | 同步方式 | 原因 |
|---|---|---|
| SPSC | acquire-release 足够 | 只有一个写者，不需要 CAS |
| MPSC | 生产端需要 CAS | 多个生产者竞争写入位置 |
| SPMC | 消费端需要 CAS | 多个消费者竞争读取位置 |
| MPMC | 双端都需要 CAS | 见第 16 章 Vyukov 环形缓冲区 |

---

## 第 8 章：Release Sequence 与传递性

### 8.1 Release Sequence 定义

**Release Sequence (释放序列)** 是 C++ 内存模型中最微妙的概念之一：

> 如果线程 A 做了一个 release store，之后其他线程对**同一原子变量**进行了一系列 RMW 操作（任意 memory_order），这些 RMW 操作**延续**了 A 的 release 关系。最终，任何线程对该变量做 acquire load 仍然与 A 的 release store 构成 synchronizes-with。

```
线程A: x.store(release)         ─┐
线程B: x.fetch_add(1, relaxed)   │ release sequence
线程C: x.fetch_add(1, relaxed)   │
线程D: x.load(acquire) ←─────────┘ 与 A 的 release synchronizes-with
```

### 8.2 shared_ptr 引用计数应用

这正是 `shared_ptr` 引用计数的工作原理：

```cpp
// test11.cpp ch8 — release sequence 演示
std::atomic<int> ref_count(1);
int* data = new int(42);

// 多个线程 acquire ref:
ref_count.fetch_add(1, std::memory_order_relaxed);  // RMW: 不需要 acquire

// 释放 ref:
if (ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    // 最后一个释放者: acq_rel 确保看到所有之前 release 的数据
    delete data;  // 安全: 构成 release sequence
}
```

### 8.3 传递性 (Transitivity)

```
线程A: data=42; x.store(1, release)     // A release
线程B: x.load(acquire); y.store(1, release)  // B 中继
线程C: y.load(acquire); use(data)       // C 看到 data=42 ✓

A→B (release-acquire on x)
B→C (release-acquire on y)
传递性: A→C, C 看到 A 写的 data
```

### 8.4 深入扩展

**为什么 `shared_ptr` 的 `fetch_sub` 用 `acq_rel` 而不只是 `release`？**
- `release` 部分：确保释放前对数据的修改对最后一个释放者可见
- `acquire` 部分：最后一个释放者（`count==1` 时）需要**看到所有其他线程释放前的修改**
- 如果只用 `release`：最后一个释放者可能**看不到**其他线程对 data 的修改就 delete 了

---

## 第 9 章：memory_order_consume 与数据依赖

### 9.1 概念

`consume` 是 `acquire` 的**弱化版**——只保证**有数据依赖关系**的操作有序，不影响无关操作：

```cpp
std::atomic<Data*> ptr;

// 生产者:
Data* d = new Data{42, "hello"};
ptr.store(d, std::memory_order_release);

// 消费者:
Data* p = ptr.load(std::memory_order_consume);
// p->value 有数据依赖 (通过 p 解引用) → 保证看到 42 ✓
// other_var 无数据依赖 → 不保证可见
```

### 9.2 ARM 上的理论优势

在 ARM 上：
- `acquire` → DMB ISH LD（完整的 load barrier）
- `consume` → **无 barrier!** 只依赖 CPU 的地址依赖保证
- 理论性能收益：免费的顺序保证

### 9.3 编译器现状

**所有主流编译器都将 `consume` 提升为 `acquire`!**

原因：
- 编译器需要跟踪**数据依赖链** (dependency chain)——这对优化器来说极其复杂
- "dependency chain" 可能穿越函数调用、模板实例化等，难以自动分析
- 标准委员会在 P0735R0 中承认目前的定义不够好

> **建议**: 不要使用 `consume`，直接用 `acquire`。等未来标准修订后再考虑。

---

## 第 10 章：volatile vs atomic — 本质区别

### 10.1 对比表

| 特性 | `volatile` | `std::atomic` |
|---|---|---|
| **原子性** | ❌ 不保证 | ✅ 保证 |
| **内存顺序** | ❌ 不提供 | ✅ 提供 (6 种) |
| **编译器优化** | 阻止优化（保留每次读写） | 按 memory_order 允许优化 |
| **CAS 操作** | ❌ 不支持 | ✅ compare_exchange |
| **硬件 fence** | ❌ 不生成 | ✅ 按需生成 |
| **用途** | MMIO 寄存器、信号处理 | 多线程同步 |

### 10.2 经典错误 — 用 volatile 做线程同步

```cpp
// ❌ 错误: volatile 不保证跨线程可见性!
volatile bool ready = false;
int data = 0;

// 线程1:
data = 42;
ready = true;  // 可能被 CPU 重排到 data=42 之前!

// 线程2:
while (!ready) ;
assert(data == 42);  // 可能失败!

// ✅ 正确:
std::atomic<bool> ready(false);
// ready.store(true, release) + ready.load(acquire)
```

### 10.3 C++20 弃用

C++20 弃用了 `volatile` 的**复合操作** (`volatile int x; x++;`)，因为这既不是原子的也不是有意义的。保留的用途仅限于 MMIO 和信号处理。

---

## 第 11 章：独立内存屏障 (fence)

### 11.1 `atomic_thread_fence` 类型

```cpp
// release fence: 之前的所有写 不被重排到 fence 之后
std::atomic_thread_fence(std::memory_order_release);

// acquire fence: 之后的所有读 不被重排到 fence 之前
std::atomic_thread_fence(std::memory_order_acquire);

// seq_cst fence: 完整的内存屏障
std::atomic_thread_fence(std::memory_order_seq_cst);

// signal fence: 仅对信号处理函数 (不插入 CPU barrier)
std::atomic_signal_fence(std::memory_order_seq_cst);
```

### 11.2 fence vs 每操作 memory_order

```cpp
// 方式1: 每个操作指定 order
a.store(1, std::memory_order_release);
b.store(2, std::memory_order_release);
c.store(3, std::memory_order_release);

// 方式2: 一个 fence 保护多个操作 (可能更优)
a.store(1, std::memory_order_relaxed);
b.store(2, std::memory_order_relaxed);
c.store(3, std::memory_order_relaxed);
std::atomic_thread_fence(std::memory_order_release);
flag.store(true, std::memory_order_relaxed);

// 方式2 的优势: 批量同步, 一个 fence 保护所有之前的写
// x86: 两种方式通常编译结果相同
// ARM: fence 版本可能生成更少的 DMB 指令
```

### 11.3 x86 上的编译结果

```
release fence → 仅编译器屏障 (x86 TSO 已保证 Store-Store)
acquire fence → 仅编译器屏障 (x86 TSO 已保证 Load-Load)
seq_cst fence → MFENCE 指令 (阻止 Store-Load 重排)
```

### 11.4 代码示例

```cpp
// test11.cpp ch11 — writer/reader fence 配对
std::atomic<bool> flag(false);
int payload = 0;

// Writer:
payload = 42;
std::atomic_thread_fence(std::memory_order_release);
flag.store(true, std::memory_order_relaxed);

// Reader:
while (!flag.load(std::memory_order_relaxed)) ;
std::atomic_thread_fence(std::memory_order_acquire);
assert(payload == 42);  // ✓
```

### 11.5 深入扩展

**fence 的本质——切断重排路径：**

```
release fence 的语义:
  ────────────────────────
  之前的任何 load/store
  ──── release fence ─────  ← 不可越过的屏障
  之后的任何 store
  ────────────────────────

acquire fence 的语义:
  ────────────────────────
  之前的任何 load
  ──── acquire fence ─────  ← 不可越过的屏障
  之后的任何 load/store
  ────────────────────────
```

---

# 三、无锁算法篇

---

## 第 12 章：自旋锁三种实现

### 12.1 TAS Lock (Test-And-Set)

```cpp
// test11.cpp ch12 — TAS 自旋锁
class TASLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire))
            ;  // 忙等, 每次 test_and_set 都是写操作 → 频繁 invalidate cache line
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};
```

**问题**: 每次 `test_and_set` 都是**写操作**（即使锁被占着），导致 cache line 在各核之间反复 invalidate → **bus 风暴**。

### 12.2 TTAS Lock (Test-Test-And-Set)

```cpp
// test11.cpp ch12 — TTAS 优化
class TTASLock {
    std::atomic<bool> locked_{false};
public:
    void lock() {
        while (true) {
            // 先 test (只读) — 在本地 cache 中 spin
            while (locked_.load(std::memory_order_relaxed))
                ;  // 只读, 不 invalidate 其他核

            // 再 test_and_set (写) — 尝试获取
            if (!locked_.exchange(true, std::memory_order_acquire))
                return;  // 成功获取
        }
    }
    void unlock() {
        locked_.store(false, std::memory_order_release);
    }
};
```

**改进**: 先**只读**spin（不产生总线流量），只有发现锁可能可用时才尝试写 → 大幅减少 cache line bouncing。

### 12.3 Ticket Lock (FIFO 公平)

```cpp
// test11.cpp ch12 — Ticket Lock
class TicketLock {
    std::atomic<unsigned> next_ticket_{0};
    std::atomic<unsigned> now_serving_{0};
public:
    void lock() {
        unsigned my_ticket = next_ticket_.fetch_add(1, std::memory_order_relaxed);
        while (now_serving_.load(std::memory_order_acquire) != my_ticket)
            ;  // 等待叫号
    }
    void unlock() {
        now_serving_.fetch_add(1, std::memory_order_release);
    }
};
```

**特点**: FIFO 顺序，无饥饿。类似银行取号——每个线程取号后等待叫号。

### 12.4 基准测试对比

```
// test11.cpp ch12 — 4 线程 × N 次 lock/unlock
  TAS Lock:     ~Xms  (最差 — bus 风暴)
  TTAS Lock:    ~Yms  (好转 — 减少写操作)
  Ticket Lock:  ~Zms  (公平但不一定最快)
  std::mutex:   ~Wms  (有上下文切换但无忙等)
```

### 12.5 深入扩展

**选择指南：**

| 场景 | 推荐 | 原因 |
|---|---|---|
| 临界区 < 1μs | TTAS + backoff | 忙等代价小于上下文切换 |
| 需要公平性 | Ticket Lock | FIFO 无饥饿 |
| 临界区 ≥ 10μs | `std::mutex` | 让出 CPU 更高效 |
| 实时系统 | Ticket Lock | 可预测的延迟 |

**进阶优化 — MCS Lock / CLH Lock:**
- 每个线程在**自己的 cache line** 上 spin（不是共享变量）
- 完全消除 cache line bouncing
- Linux 内核 `qspinlock` 就是基于 MCS 的变体

---

## 第 13 章：Treiber Stack — 无锁栈

### 13.1 核心结构

```cpp
// test11.cpp ch13 — 无锁栈
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
    };

    std::atomic<Node*> head_{nullptr};
    std::atomic<size_t> size_{0};

    // 简化版延迟删除
    std::atomic<Node*> to_delete_{nullptr};
```

### 13.2 Push — CAS 替换 head

```cpp
    void push(const T& val) {
        Node* new_node = new Node{val, nullptr};
        new_node->next = head_.load(std::memory_order_relaxed);

        // CAS 循环: 尝试将 head 从 old_head 替换为 new_node
        while (!head_.compare_exchange_weak(
                    new_node->next,     // expected (失败时自动更新为当前 head)
                    new_node,           // desired
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
            // new_node->next 已被自动更新, 直接重试
        }
        size_.fetch_add(1, std::memory_order_relaxed);
    }
```

```
Push 过程:
  Before:  head → [A] → [B] → [C]
  new_node → [X, next=A]
  CAS(head, A → X) 成功:
  After:   head → [X] → [A] → [B] → [C]
```

### 13.3 Pop — CAS + 延迟删除

```cpp
    std::optional<T> pop() {
        Node* old_head = head_.load(std::memory_order_acquire);
        while (old_head) {
            if (head_.compare_exchange_weak(
                    old_head,
                    old_head->next,
                    std::memory_order_acq_rel,
                    std::memory_order_acquire)) {
                T val = old_head->data;
                // 延迟删除: 把 old_head 放入 to_delete 链
                old_head->next = to_delete_.load(std::memory_order_relaxed);
                while (!to_delete_.compare_exchange_weak(
                    old_head->next, old_head,
                    std::memory_order_release,
                    std::memory_order_relaxed));
                size_.fetch_sub(1, std::memory_order_relaxed);
                return val;
            }
            // old_head 自动更新, 重试
        }
        return std::nullopt;
    }
```

### 13.4 多线程验证

```cpp
// test11.cpp ch13 — 4线程 × 100K push + pop
// 验证: 所有 push 的元素之和 == 所有 pop 的元素之和
constexpr int T = 4, N = 100000;
// 总和 = T * N * (N-1) / 2 = 预期值
```

### 13.5 深入扩展

**Treiber Stack 的 ABA 风险分析：**
- `pop()` 中 `old_head->next` 可能已被其他线程修改 → 经典 ABA
- 解决: Hazard Pointer（第 17 章）或 Tagged Pointer（第 4 章）
- 本实现用**延迟删除**（to_delete_ 链）简化——避免立即释放

**与 `std::stack + mutex` 对比：**

| 指标 | Treiber Stack | mutex + stack |
|---|---|---|
| 吞吐 (低竞争) | 🏆 更高 | 较低 |
| 吞吐 (高竞争) | CAS 重试多 | mutex 排队等 |
| 延迟确定性 | 差 (CAS 重试不确定) | 差 (等锁不确定) |
| 内存回收 | 复杂! | 简单 |
| 代码复杂度 | 高 | 低 |

---

## 第 14 章：Michael-Scott Queue — MPMC 无锁队列

### 14.1 核心设计

Michael-Scott Queue 是最经典的 **MPMC (多生产者-多消费者)** 无锁队列，使用 **dummy node** 分离 head 和 tail 的竞争：

```cpp
// test11.cpp ch14 — Michael-Scott Queue
template<typename T>
class MSQueue {
    struct Node {
        std::optional<T> data;
        std::atomic<Node*> next{nullptr};
    };

    std::atomic<Node*> head_;  // 指向 dummy node
    std::atomic<Node*> tail_;  // 指向最后一个节点

    MSQueue() {
        Node* dummy = new Node{};
        head_.store(dummy);
        tail_.store(dummy);
    }
```

### 14.2 Enqueue — CAS + 帮助推进

```cpp
    void enqueue(const T& val) {
        Node* new_node = new Node{val};
        while (true) {
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = tail->next.load(std::memory_order_acquire);

            if (next == nullptr) {
                // tail->next 为空, 尝试链接新节点
                if (tail->next.compare_exchange_weak(next, new_node,
                        std::memory_order_release)) {
                    // 成功! 推进 tail
                    tail_.compare_exchange_strong(tail, new_node,
                        std::memory_order_release);
                    return;
                }
            } else {
                // tail 落后了! 帮助推进 (help-advance)
                tail_.compare_exchange_weak(tail, next,
                    std::memory_order_release);
            }
        }
    }
```

**帮助推进 (Help-Advance) 机制：**

```
正常状态:  head → [D] → [A] → [B] ← tail

enqueue(C) 分两步:
  ① CAS: tail->next = new_node     → head → [D] → [A] → [B] → [C]  ← tail 还在 B!
  ② CAS: tail = new_node           → head → [D] → [A] → [B] → [C] ← tail

如果线程在 ① 和 ② 之间被抢占:
  其他线程发现 tail->next != null → 帮助推进 tail!
  → 保证 tail 不会无限落后
```

### 14.3 Dequeue

```cpp
    std::optional<T> dequeue() {
        while (true) {
            Node* head = head_.load(std::memory_order_acquire);
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = head->next.load(std::memory_order_acquire);

            if (next == nullptr)
                return std::nullopt;  // 队列空

            if (head == tail) {
                // head == tail 但 next != null → tail 落后, 帮助推进
                tail_.compare_exchange_weak(tail, next);
                continue;
            }

            T val = *next->data;
            if (head_.compare_exchange_weak(head, next,
                    std::memory_order_acq_rel)) {
                delete head;  // 删除旧 dummy
                return val;
            }
        }
    }
```

### 14.4 深入扩展

**为什么需要 Dummy Node？**
- 如果没有 dummy node：enqueue 写 tail，dequeue 写 head。当队列只有一个元素时，head == tail，两个操作**竞争同一个指针** → 复杂
- 有 dummy node：head 总是指向 dummy，dequeue 操作的是 `head->next`，与 tail 指向的不同位置 → **分离竞争**

**MSQueue vs 分区队列：**

| 特性 | MSQueue | 分区队列 |
|---|---|---|
| 模型 | MPMC | Per-producer queue + stealing |
| CAS 竞争 | head/tail 各一个 | 近乎零竞争 |
| 延迟 | 低 | 更低 |
| 公平性 | 全局 FIFO | 近似 FIFO |
| 实现复杂度 | 中 | 高 |

---

## 第 15 章：无锁哈希表 — 分桶原子链表

### 15.1 设计思路

```cpp
// test11.cpp ch15 — Lock-free HashMap
template<typename K, typename V, int NUM_BUCKETS = 64>
class LockFreeHashMap {
    struct Node {
        K key;
        V value;
        std::atomic<Node*> next{nullptr};
    };

    std::array<std::atomic<Node*>, NUM_BUCKETS> buckets_{};

    int bucket_for(const K& key) const {
        return std::hash<K>{}(key) % NUM_BUCKETS;
    }
```

**设计**: 64 个桶，每个桶是一个原子链表。不同桶之间**完全无竞争**。

### 15.2 CAS 插入

```cpp
    bool insert(const K& key, const V& value) {
        int idx = bucket_for(key);
        Node* new_node = new Node{key, value};

        Node* old_head = buckets_[idx].load(std::memory_order_acquire);
        do {
            // 检查是否已存在
            Node* curr = old_head;
            while (curr) {
                if (curr->key == key) {
                    delete new_node;
                    return false;  // 已存在
                }
                curr = curr->next.load(std::memory_order_acquire);
            }
            new_node->next.store(old_head, std::memory_order_relaxed);
        } while (!buckets_[idx].compare_exchange_weak(
                    old_head, new_node,
                    std::memory_order_release,
                    std::memory_order_acquire));

        return true;
    }
```

### 15.3 Acquire 遍历查找

```cpp
    std::optional<V> find(const K& key) const {
        int idx = bucket_for(key);
        Node* curr = buckets_[idx].load(std::memory_order_acquire);
        while (curr) {
            if (curr->key == key)
                return curr->value;
            curr = curr->next.load(std::memory_order_acquire);
        }
        return std::nullopt;
    }
```

### 15.4 深入扩展

**分桶数量选择：**
- 桶太少 → 链表长，竞争多
- 桶太多 → 浪费内存，cache 不友好
- **经验**: 桶数 ≈ 期望最大并发线程数 × 8~16
- 使用**质数个桶**可减少 hash 碰撞

**无锁 delete 的复杂性：**

本实现只 insert + find，没有 delete。无锁 delete 需要：
1. **逻辑删除**: 标记节点为已删除（但不物理移除）
2. **物理删除**: 在后续 insert/find 中顺路清理
3. **内存回收**: HP 或 EBR 保护删除节点

这就是为什么真正的无锁 HashMap（如 `ConcurrentHashMap`）实现非常复杂。

---

## 第 16 章：无锁环形缓冲区 — SPSC + MPMC

### 16.1 SPSC Ring Buffer

```cpp
// test11.cpp ch16 — SPSC 环形缓冲区
template<typename T, size_t Capacity>
class SPSCRingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity 须为 2 的幂");
    std::array<T, Capacity> buffer_;
    alignas(64) std::atomic<size_t> write_pos_{0};  // 生产者独占
    alignas(64) std::atomic<size_t> read_pos_{0};   // 消费者独占

public:
    bool push(const T& val) {
        size_t w = write_pos_.load(std::memory_order_relaxed);
        size_t next = (w + 1) & (Capacity - 1);  // 位运算代替取模
        if (next == read_pos_.load(std::memory_order_acquire))
            return false;
        buffer_[w] = val;
        write_pos_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& val) {
        size_t r = read_pos_.load(std::memory_order_relaxed);
        if (r == write_pos_.load(std::memory_order_acquire))
            return false;
        val = buffer_[r];
        read_pos_.store((r + 1) & (Capacity - 1), std::memory_order_release);
        return true;
    }
};
```

**关键设计点：**
- `Capacity` 必须是 2 的幂 → 用 `& (Capacity-1)` 代替 `% Capacity` (更快)
- `alignas(64)` 分离 write_pos 和 read_pos → 消除 false sharing
- 只用 acquire-release → 比 seq_cst 更高效

### 16.2 MPMC Ring Buffer (Vyukov 方式)

```cpp
// test11.cpp ch16 — Vyukov MPMC 环形缓冲区
template<typename T, size_t Capacity>
class MPMCRingBuffer {
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    std::array<Cell, Capacity> buffer_;
    alignas(64) std::atomic<size_t> enqueue_pos_{0};
    alignas(64) std::atomic<size_t> dequeue_pos_{0};

public:
    MPMCRingBuffer() {
        for (size_t i = 0; i < Capacity; ++i)
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
    }

    bool enqueue(const T& val) {
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        Cell& cell = buffer_[pos & (Capacity - 1)];
        size_t seq = cell.sequence.load(std::memory_order_acquire);
        intptr_t diff = (intptr_t)seq - (intptr_t)pos;

        if (diff == 0) {
            // 槽位可用
            if (enqueue_pos_.compare_exchange_weak(pos, pos + 1,
                    std::memory_order_relaxed))
            {
                cell.data = val;
                cell.sequence.store(pos + 1, std::memory_order_release);
                return true;
            }
        }
        return false;  // diff < 0 → 满; CAS 失败 → 重试
    }

    bool dequeue(T& val) {
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        Cell& cell = buffer_[pos & (Capacity - 1)];
        size_t seq = cell.sequence.load(std::memory_order_acquire);
        intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

        if (diff == 0) {
            if (dequeue_pos_.compare_exchange_weak(pos, pos + 1,
                    std::memory_order_relaxed))
            {
                val = cell.data;
                cell.sequence.store(pos + Capacity, std::memory_order_release);
                return true;
            }
        }
        return false;
    }
};
```

### 16.3 Vyukov MPMC 原理

```
Cell sequence 数字的含义:
  sequence == pos      → 槽位空, 可写入 (enqueue)
  sequence == pos + 1  → 槽位满, 可读取 (dequeue)
  sequence == pos + Cap → 回收完成, 等待下一轮写入

enqueue(pos=0):
  seq=0, diff=0 → 写入 → seq=1
dequeue(pos=0):
  seq=1, diff=0 → 读取 → seq=0+Cap

这个精巧的序列号方案实现了无需 mutex 的 MPMC 队列。
```

### 16.4 深入扩展

**SPSC vs MPMC 性能差距的根本原因：**

| | SPSC Ring | MPMC Ring |
|---|---|---|
| 核心操作 | `store(release)` | `CAS(relaxed)` |
| CAS 竞争 | 无 | 有 |
| 吞吐量 | ~100M ops/sec | ~10-30M ops/sec |
| 延迟 | ~10-50ns | ~50-200ns |
| 适用 | 固定 1P-1C | 任意 MP-MC |

---

# 四、内存回收篇

---

## 第 17 章：Hazard Pointer 详解与实现

### 17.1 核心问题

无锁数据结构的最大难题：**何时安全释放被替换的节点？**

```
线程A: old = head.load()    // 读到指向节点 X 的指针
线程B: CAS 移除 X; delete X  // 替换并释放 X
线程A: use(old->data)        // 💥 Use-after-free!
```

**Hazard Pointer 思路**: 线程在使用指针前**声明** "我正在使用这个指针"，其他线程在释放前**检查**是否有人声明了该指针。

### 17.2 完整实现

```cpp
// test11.cpp ch17 — Hazard Pointer Domain
class HazardPointerDomain {
    static constexpr int MAX_THREADS = 64;
    static constexpr int RETIRE_THRESHOLD = 128;

    struct HPRecord {
        std::atomic<std::thread::id> owner{};
        std::atomic<void*> ptr{nullptr};
    };

    std::array<HPRecord, MAX_THREADS> hp_records_;
    static thread_local std::vector<void*> retire_list_;
    static thread_local std::function<void(void*)> deleter_;

    // 获取一个 HP 槽位
    HPRecord* acquire_slot() {
        auto tid = std::this_thread::get_id();
        for (auto& rec : hp_records_) {
            std::thread::id empty{};
            if (rec.owner.compare_exchange_strong(empty, tid))
                return &rec;
        }
        throw std::runtime_error("HP slots exhausted");
    }

    // 保护指针 (读者调用)
    void protect(HPRecord* slot, void* ptr) {
        slot->ptr.store(ptr, std::memory_order_release);
    }

    // 退休 (写者调用) — 延迟回收
    void retire(void* ptr, std::function<void(void*)> del) {
        retire_list_.push_back(ptr);
        deleter_ = del;
        if (retire_list_.size() >= RETIRE_THRESHOLD)
            scan();
    }

    // 扫描 — 找出可安全删除的节点
    void scan() {
        // 收集所有活跃的 hazard pointer
        std::set<void*> hazard_set;
        for (auto& rec : hp_records_) {
            void* p = rec.ptr.load(std::memory_order_acquire);
            if (p) hazard_set.insert(p);
        }

        // 不在 hazard_set 中的退休节点可以安全删除
        auto it = retire_list_.begin();
        while (it != retire_list_.end()) {
            if (hazard_set.count(*it) == 0) {
                deleter_(*it);
                it = retire_list_.erase(it);
            } else {
                ++it;
            }
        }
    }
};
```

### 17.3 读者保护模式 (双重检查)

```cpp
// 读者必须用双重检查!
HPRecord* slot = domain.acquire_slot();

Data* ptr;
do {
    ptr = shared.load(std::memory_order_acquire);   // ① 读指针
    domain.protect(slot, ptr);                      // ② 声明保护
} while (ptr != shared.load(std::memory_order_acquire));  // ③ 重新检查
// 为什么需要 ③? 因为在 ① 和 ② 之间 ptr 可能已被删除!

use(ptr->data);  // 安全!
domain.clear(slot);  // 使用完毕, 清除保护
```

### 17.4 深入扩展

**Hazard Pointer 的开销分析：**

| 操作 | 开销 |
|---|---|
| `protect` | 一次 store(release) + 一次 load(acquire) 重检 |
| `clear` | 一次 store(release) |
| `retire` | 加入列表, O(1) |
| `scan` | O(P × R) — P=线程数, R=退休列表大小 |

**HP vs EBR 对比：**

| | Hazard Pointer | EBR |
|---|---|---|
| 读者开销 | 每次访问: protect + clear | 每次访问: enter + leave |
| 写者开销 | retire + 异步 scan | retire + epoch advance |
| 延迟回收 | 最多延迟 P 个节点 | 最多延迟 2 个 epoch 的节点 |
| 迟钝读者 | 不影响其他线程 | 阻塞 epoch 推进! |
| 实现复杂度 | 中 | 中 |
| C++ 标准 | C++26 `<hazard_pointer>` | 无 |

---

## 第 18 章：Epoch-Based Reclamation (EBR)

### 18.1 三 Epoch 轮转原理

```
Epoch 0  →  Epoch 1  →  Epoch 2  →  Epoch 0  → ...
  ↑                                     ↑
  这个 epoch 的退休节点             回到 0 时, epoch 0
  在 epoch 2 结束时可安全删除      的退休节点已无人引用
```

**规则**: 当全局 epoch 从 E 推进到 E+1 时，说明所有线程都至少经历了 epoch E。因此 epoch (E-2) 的退休节点可安全删除——因为没有线程还处于 epoch (E-2) 的临界区中。

### 18.2 核心实现

```cpp
// test11.cpp ch18 — EBR
class EpochBasedReclamation {
    static constexpr int MAX_THREADS = 64;
    static constexpr int NUM_EPOCHS = 3;

    std::atomic<uint64_t> global_epoch_{0};

    struct ThreadState {
        std::atomic<uint64_t> local_epoch{0};
        std::atomic<bool> active{false};
        std::vector<void*> retire_lists[NUM_EPOCHS];
    };

    std::array<ThreadState, MAX_THREADS> thread_states_;

    // 进入临界区
    void enter(int tid) {
        thread_states_[tid].active.store(true, std::memory_order_relaxed);
        thread_states_[tid].local_epoch.store(
            global_epoch_.load(std::memory_order_relaxed),
            std::memory_order_release);
    }

    // 离开临界区
    void leave(int tid) {
        thread_states_[tid].active.store(false, std::memory_order_release);
    }

    // 退休节点
    void retire(int tid, void* ptr) {
        uint64_t epoch = global_epoch_.load(std::memory_order_relaxed);
        thread_states_[tid].retire_lists[epoch % NUM_EPOCHS].push_back(ptr);
        try_advance();
    }

    // 尝试推进 epoch
    void try_advance() {
        uint64_t cur = global_epoch_.load(std::memory_order_relaxed);
        // 检查所有活跃线程是否都在当前 epoch
        for (auto& ts : thread_states_) {
            if (ts.active.load(std::memory_order_acquire) &&
                ts.local_epoch.load(std::memory_order_acquire) != cur) {
                return;  // 有线程还在旧 epoch, 不能推进
            }
        }
        // 推进 epoch
        if (global_epoch_.compare_exchange_strong(cur, cur + 1)) {
            // 删除 (cur-1) epoch 的退休节点
            uint64_t old_epoch = (cur + 1) % NUM_EPOCHS;
            for (auto& ts : thread_states_) {
                for (void* p : ts.retire_lists[old_epoch])
                    delete static_cast<char*>(p);
                ts.retire_lists[old_epoch].clear();
            }
        }
    }
};
```

### 18.3 RAII Guard

```cpp
// test11.cpp ch18 — RAII Guard
class Guard {
    EpochBasedReclamation& ebr_;
    int tid_;
public:
    Guard(EpochBasedReclamation& ebr, int tid)
        : ebr_(ebr), tid_(tid) { ebr_.enter(tid_); }
    ~Guard() { ebr_.leave(tid_); }
};

// 使用:
{
    Guard g(ebr, tid);
    Data* p = shared.load(std::memory_order_acquire);
    use(p->data);  // Guard 保证 p 不会被删除
}  // ~Guard → leave
```

### 18.4 深入扩展

**EBR 的"迟钝读者"问题：**
- 如果一个线程进入临界区后**长时间不退出**（如 sleep），epoch 无法推进
- 所有退休列表都在增长 → 内存泄漏
- 解决: 限制临界区长度, 或混合 HP+EBR

---

## 第 19 章：引用计数回收 (Split Reference Count)

### 19.1 概念

Split Reference Count 将引用计数拆为**外部计数**和**内部计数**：

```
┌────────────────────────┐
│ CountedPtr              │
│  external_count (正在   │ ← 每次有新访问者: +1
│    访问此节点的线程数)   │    访问结束: external→internal 转移
│  ptr → Node             │
└────────┬───────────────┘
         │
         ▼
┌────────────────────┐
│ Node                │
│  internal_count     │ ← 每次有线程结束访问: +1
│  data               │    当 internal == -(external-1) 时
└────────────────────┘      → 所有访问者都结束, 可安全删除
```

**关键**: `external_count` 增加意味着"有新访问者"；当访问者离开时，不减 external 而是增加 internal。当 `internal + external == 1` 时，说明最后一个访问者已离开。

### 19.2 与 shared_ptr 的关系

`std::shared_ptr` 的 `control_block` 就是 split reference count 的工业级实现：
- `use_count` (strong ref) ≈ external count
- `weak_count` (weak ref) ≈ internal count
- 当 `use_count == 0` → 删除管理的对象
- 当 `use_count == 0 && weak_count == 0` → 删除 control_block

### 19.3 深入扩展

**三种内存回收方案对比：**

| | Hazard Pointer | EBR | Split RefCount |
|---|---|---|---|
| 读者开销 | 声明 + 清除 | enter + leave | 增/减计数 |
| 空间开销 | O(P) HP 槽 | O(P×N) 退休列表 | 每节点两个计数 |
| 延迟确定性 | 好 | 差 (迟钝读者) | 好 |
| 适用结构 | 通用 | 通用 | 特定 (如栈) |
| 实现难度 | 中 | 中 | 高 |

---

# 五、高级并发数据结构篇

---

## 第 20 章：SeqLock — 序列锁

### 20.1 设计思路

SeqLock 适用于**写者极少、读者极多**的场景。读者无锁，但可能需要**重试**：

```
写者:
  ① sequence++ (变为奇数 → 正在写)
  ② 修改数据
  ③ sequence++ (变为偶数 → 写完)

读者:
  ① seq1 = sequence (如果是奇数 → 正在写, 重试)
  ② 读取数据
  ③ seq2 = sequence (如果 seq1 != seq2 → 数据被修改, 重试)
```

### 20.2 实现

```cpp
// test11.cpp ch20 — SeqLock
class SeqLock {
    std::atomic<unsigned> seq_{0};
    // 数据区域不需要是 atomic

    unsigned read_begin() const {
        unsigned s;
        do {
            s = seq_.load(std::memory_order_acquire);
        } while (s & 1);  // 奇数 → 写者正在写
        return s;
    }

    bool read_retry(unsigned start_seq) const {
        std::atomic_thread_fence(std::memory_order_acquire);
        return seq_.load(std::memory_order_relaxed) != start_seq;
    }

    void write_lock() {
        seq_.fetch_add(1, std::memory_order_release);  // 奇数
    }

    void write_unlock() {
        seq_.fetch_add(1, std::memory_order_release);  // 偶数
    }
};
```

### 20.3 一致性验证

```cpp
// test11.cpp ch20 — TimestampedData 一致性测试
struct TimestampedData {
    int x, y, z;
    uint64_t timestamp;
};

// 写者: x=N, y=N*2, z=N*3, timestamp=N
// 读者: 验证 y==x*2, z==x*3 (一致性)
// 如果读到不一致数据 → read_retry 检测到, 重试
```

### 20.4 深入扩展

**SeqLock vs RWLock：**

| | SeqLock | shared_mutex (RWLock) |
|---|---|---|
| 读者锁 | 无 (零开销) | 有 (shared lock) |
| 读者阻塞写者 | 否 | 是 |
| 数据一致性 | 重试保证 | 锁保证 |
| 适用 | 小数据, 写极少 | 通用 |
| 缺点 | 只能用于可安全复制的 POD 数据 | 原子引用计数开销 |

---

## 第 21 章：Read-Copy-Update (RCU)

### 21.1 RCU 核心思想

RCU 是 **Linux 内核**中最重要的并发原语之一，核心思想是：
- **读者**: 原子 load 指针，直接读（零开销）
- **写者**: 复制旧数据 → 修改副本 → 原子替换指针 → 等待旧读者结束 → 删除旧数据

### 21.2 用户态 SimpleRCU

```cpp
// test11.cpp ch21 — SimpleRCU
template<typename T>
class SimpleRCU {
    std::atomic<T*> current_;
    std::mutex writer_mutex_;

public:
    SimpleRCU(T* init) : current_(init) {}

    // 读者 (lock-free):
    const T* read() const {
        return current_.load(std::memory_order_acquire);
    }

    // 写者: copy-modify-publish-wait-delete
    template<typename Func>
    void update(Func modify_fn) {
        std::lock_guard<std::mutex> lock(writer_mutex_);
        T* old = current_.load(std::memory_order_relaxed);
        T* new_data = new T(*old);         // copy
        modify_fn(*new_data);              // modify
        current_.store(new_data, std::memory_order_release);  // publish
        std::this_thread::sleep_for(1ms);  // wait (简化的 grace period)
        delete old;                        // delete
    }
};
```

### 21.3 Config 热更新演示

```cpp
// test11.cpp ch21 — Config 热更新
struct Config {
    int max_connections;
    int timeout_ms;
    std::string server_name;
};

SimpleRCU<Config> config(new Config{100, 5000, "server-v1"});

// 4 个读者线程: 不断 read() 获取配置
// 1 个写者线程: 定期 update() 更新配置
// 读者**永远不会**被写者阻塞!
```

### 21.4 深入扩展

**RCU 的 Grace Period 问题：**
- 简化实现中用 `sleep(1ms)` 等待旧读者——这不精确
- Linux 内核的 RCU 利用**调度点** (quiescent state) 精确判断
- 用户态替代: 结合 EBR 判断读者是否已离开
- `libcds` 和 `userspace-rcu (URCU)` 都提供了高效的用户态 RCU

**RCU 适用场景：**
- 配置管理 (热更新)
- 路由表
- 读多写少的全局数据
- Linux 内核: 文件系统目录项、网络路由表、模块列表

---

## 第 22 章：并发跳表 (Concurrent Skip List)

### 22.1 跳表结构

```
跳表 (Skip List):
  level 3: head ──────────────────────────→ [50] ────→ nil
  level 2: head ──────→ [20] ─────────────→ [50] ────→ nil
  level 1: head → [10] → [20] → [30] → [40] → [50] → nil

  查找 O(log n) — 从高层开始，逐层降低
  插入 O(log n) — 随机层数（概率 1/2 升层）
```

### 22.2 并发设计

```cpp
// test11.cpp ch22 — ConcurrentSkipList
template<typename K, typename V>
class ConcurrentSkipList {
    static constexpr int MAX_LEVEL = 16;

    struct Node {
        K key;
        V value;
        int level;
        std::atomic<Node*> nexts[MAX_LEVEL];  // 每层原子指针
    };

    Node head_;                      // 哨兵头节点
    std::atomic<int> max_level_{1};
    std::mutex write_mutex_;         // 写入用 mutex 简化

    int random_level() {
        // 几何分布: P(level=k) = 1/2^k
        static thread_local std::mt19937 gen(...);
        int level = 1;
        while (level < MAX_LEVEL && (gen() & 1)) ++level;
        return level;
    }
```

### 22.3 Lock-free 查找

```cpp
    // 查找是 lock-free 的:
    std::optional<V> find(const K& key) const {
        const Node* curr = &head_;
        for (int i = max_level_.load() - 1; i >= 0; --i) {
            const Node* next = curr->nexts[i].load(std::memory_order_acquire);
            while (next && next->key < key) {
                curr = next;
                next = curr->nexts[i].load(std::memory_order_acquire);
            }
            if (next && next->key == key)
                return next->value;
        }
        return std::nullopt;
    }
```

### 22.4 写入 (mutex 简化)

```cpp
    bool insert(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(write_mutex_);

        // 找到每层的前驱节点
        Node* preds[MAX_LEVEL];
        Node* curr = &head_;
        for (int i = max_level_ - 1; i >= 0; --i) {
            // ... 遍历找前驱
            preds[i] = curr;
        }

        int level = random_level();
        Node* new_node = new Node(key, value, level);

        // 自底向上链接 + release 保证新节点内容对读者可见
        for (int i = 0; i < level; ++i) {
            new_node->nexts[i].store(preds[i]->nexts[i].load(), relaxed);
        }
        for (int i = 0; i < level; ++i) {
            preds[i]->nexts[i].store(new_node, std::memory_order_release);
        }
        return true;
    }
```

### 22.5 深入扩展

**完全无锁跳表的复杂性：**
- 本实现：查找 lock-free + 插入 mutex → **读写并发安全**，但写者之间串行
- 完全无锁版本需要：
  1. **逻辑删除标记**: 在 next 指针中嵌入删除位
  2. **帮助机制**: 其他线程帮助完成被打断的操作
  3. **多层 CAS 一致性**: 插入/删除时多层链接的原子性
- 参考: Fraser's lock-free skip list (Fraser & Harris, 2004)
- Java: `ConcurrentSkipListMap` 是完全无锁的

---

# 六、实战与调试篇

---

## 第 23 章：常见 Bug 模式 (8 大模式)

### Bug 1: 遗漏 memory_order

```cpp
// ❌ flag.store(true);  data 可能还没写完
// ✅ 先写 data, 再 flag.store(true, release)
//    对应: flag.load(acquire) 后再读 data
```

### Bug 2: ABA 问题

```cpp
// ❌ CAS 只比较指针值 → 无法检测 A→B→A
// ✅ 使用 tagged pointer / hazard pointer / EBR
```

### Bug 3: CAS 后使用过时值 (Use-After-Free)

```cpp
// ❌ Node* n = head.load(); use(n->data);  // n 可能已被释放!
// ✅ 先用 HP 保护 n, 再使用
```

### Bug 4: relaxed 导致的不可见

```cpp
// ❌ 线程A: x.store(1, relaxed); y.store(1, relaxed);
//    线程B: if (y.load(relaxed)==1) assert(x.load(relaxed)==1);
//    → assert 可能失败! relaxed 不保证跨变量顺序
// ✅ 使用 release/acquire 配对
```

### Bug 5: False Sharing (伪共享)

```cpp
// ❌ struct { atomic<int> a; atomic<int> b; };
//    线程1 写 a, 线程2 写 b → 同一 cache line 反复 invalidate
// ✅ alignas(64) atomic<int> a;
//    alignas(64) atomic<int> b;
```

### Bug 6: CAS weak 在非循环中使用

```cpp
// ❌ if (x.compare_exchange_weak(expected, desired)) { ... }
//    → weak 可能假失败!
// ✅ 非循环用 strong, 循环用 weak
```

### Bug 7: memory_order 不匹配

```cpp
// ❌ store(release) + load(relaxed)  → 不构成同步
// ✅ store(release) + load(acquire)  → 正确的同步对
```

### Bug 8: 数据竞争 (UB!)

```cpp
// ❌ int x = 0;  // 非原子
//    线程1: x = 1; 线程2: cout << x;  → Undefined Behavior!
// ✅ atomic<int> x 或使用 mutex
```

---

## 第 24 章：性能基准测试 — 五档对比

### 24.1 测试设置

4 线程 × 1M 次递增 = 4M 总操作

### 24.2 结果

```
// test11.cpp ch24 — 4M ops benchmark
  std::mutex:        最慢 (上下文切换 + 锁竞争)
  atomic(seq_cst):   较慢 (LOCK XADD + MFENCE)
  atomic(relaxed):   快   (仅 LOCK 前缀)
  per-thread sharded: 很快 (无竞争, 各核写各自 cache line)
  thread_local:      最快 (完全无共享, 最后合并)
```

### 24.3 性能阶梯

```
  thread_local ──→ sharded ──→ atomic(relaxed) ──→ atomic(seq_cst) ──→ mutex
    最快              快            较快                较慢             最慢
  (零竞争)     (各自 cache line) (LOCK 前缀)     (full barrier)   (上下文切换)

  选择指南:
    能用 thread_local  → 首选 (零竞争)
    需实时共享         → atomic (尽量用 relaxed)
    临界区较长         → mutex
    读多写少           → RCU / SeqLock
    单一计数器         → atomic (考虑分片)
```

### 24.4 深入扩展

**per-thread sharding 实现要点：**

```cpp
// test11.cpp ch24 — alignas 消除 false sharing
struct alignas(64) PaddedCounter { long long val = 0; };  // 64字节=1 cache line
std::array<PaddedCounter, 16> local_counters{};

// 每个线程写自己的 counter → 零 cache line 竞争
// 最后一次性合并 → 仅 1 次跨核操作
```

**`thread_local` 最快的原因：**
- 完全不写共享变量（编译器可能优化为寄存器操作）
- 只在最后用一次 `fetch_add(relaxed)` 合并
- 适用于: 统计计数、采样点收集、线程本地缓存

---

## 第 25 章：工具链 — TSan / ASAN / Relacy / CDSChecker

### 25.1 ThreadSanitizer (TSan)

```bash
# 编译
g++ -fsanitize=thread -g -O1 test11.cpp

# 检测到数据竞争时输出:
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 4 at 0x7f... by thread T1:
    #0 func1 test.cpp:42
  Previous read of size 4 at 0x7f... by thread T2:
    #0 func2 test.cpp:58
```

- **优点**: 非常准确, 极少误报
- **缺点**: 程序慢 5-15x, 内存增 5-10x
- **建议**: CI 中必备, 每次提交都跑

### 25.2 AddressSanitizer (ASAN)

```bash
g++ -fsanitize=address -g -O1 test11.cpp
```

检测: heap-use-after-free, buffer-overflow, double-free, use-after-scope

### 25.3 Relacy — 无锁算法模型检查器

```cpp
#include <relacy/relacy.hpp>

struct test : rl::test_suite<test, 2> {
    rl::atomic<int> x;
    void thread(unsigned idx) {
        if (idx == 0) x.store(1, rl::mo_release);
        else          int v = x.load(rl::mo_acquire);
    }
};

// rl::simulate<test>() 枚举所有调度, 验证无 bug
```

### 25.4 核心检查清单

```
□ 所有共享可变数据都通过 atomic 或 mutex 访问
□ release-acquire 配对正确
□ CAS 失败时正确处理 (expected 被更新)
□ 没有 ABA 问题 (用 tagged pointer 或 HP/EBR)
□ 无锁结构有正确的内存回收策略
□ 避免 false sharing (alignas(64))
□ TSan/ASAN 零警告
□ perf stat 确认无异常 cache-miss
```

---

## 附录 A：内存顺序速查表

| memory_order | 适用操作 | 保证 | x86 开销 | ARM 开销 |
|---|---|---|---|---|
| `relaxed` | load/store/RMW | 仅原子性 | 零 | 零 |
| `consume` | load | 数据依赖 (实际=acquire) | 零 | 零→DMB |
| `acquire` | load/fence | 后续读写不前移 | 零 | DMB ISH LD |
| `release` | store/fence | 之前读写不后移 | 零 | DMB ISH ST |
| `acq_rel` | RMW/fence | acquire + release | 零 | DMB ISH |
| `seq_cst` | 全部 | 全序一致性 | MFENCE/LOCK | DMB ISH |

## 附录 B：无锁数据结构选型

| 数据结构 | 适用模型 | 核心技术 | 内存回收 | 章节 |
|---|---|---|---|---|
| Treiber Stack | MPMC | head CAS | HP/EBR | 13 |
| Michael-Scott Queue | MPMC | head/tail CAS + help | HP/EBR | 14 |
| Lock-free HashMap | MPMC (分桶) | per-bucket CAS | HP/EBR | 15 |
| SPSC Ring Buffer | 1P-1C | acquire-release | 无需 | 16 |
| MPMC Ring Buffer | MPMC | Vyukov seq number | 无需 | 16 |
| SeqLock | 1W-NR | 序列号重试 | 无需 | 20 |
| RCU | NR-少W | copy-modify-publish | Grace period | 21 |
| Concurrent Skip List | NR-少W | atomic 指针遍历 | HP/EBR | 22 |

## 附录 C：内存回收方案选型

| 方案 | 读者开销 | 写者开销 | 迟钝读者影响 | C++ 标准 | 适用场景 |
|---|---|---|---|---|---|
| **Hazard Pointer** | protect+clear | retire+scan | 不影响 | C++26 | 通用 |
| **EBR** | enter+leave | retire+advance | 阻塞回收 | 无 | 短临界区 |
| **Split RefCount** | 增/减计数 | 计数归零删除 | 不影响 | 无 | 特定结构 |
| **RCU** | load(acquire) | copy+publish+wait | 不影响 | 无 | 读多写少 |

---

## 运行输出示例

```
================================================================
 无锁编程、内存顺序、原子操作及并发数据结构 — 完全教程
================================================================

╔══════════════════════════════════════════════════╗
║ 一、原子操作基础篇                                ║
╚══════════════════════════════════════════════════╝

  ── std::atomic 基础 ──
  lock_free 检查:
    atomic<int>       : lock_free = true
    atomic<long long> : lock_free = true
    atomic<int*>      : lock_free = true
    atomic<Big>       : lock_free = false
  4 线程 × 100000 次原子自增: counter = 400000 ✓

  ── 原子类型手册 ──
  atomic_flag: set 前 was_set = false
  指针原子运算: fetch_add(3) → arr, 现在指向 arr[3]
  用户定义 Point: CAS 成功 → {3, 4} ✓

  ── RMW 操作 ──
  fetch_add(5): old=0, new=5
  fetch_or(FLAG_READY): 状态变化 ✓
  4 线程 fetch_add: counter = 400000 ✓

  ── CAS 循环 ──
  atomic_max via CAS: 42 ✓
  ── ABA 问题 ──
  TaggedPtr CAS: tag 版本递增防 ABA ✓

╔══════════════════════════════════════════════════╗
║ 二、内存顺序篇                                    ║
╚══════════════════════════════════════════════════╝

  ── Store-Load 重排 ──
  100000 轮中 both_zero 出现 N 次 (relaxed 下可能 > 0)

  ── 六种 memory_order ──
  acquire-release 同步: data = 42 ✓
  seq_cst 10M fetch_add: Xms
  relaxed 10M fetch_add: Yms

  ── Acquire-Release SPSC ──
  1M 项传输: sum = 499999500000 ✓

  ...

╔══════════════════════════════════════════════════╗
║ 六、实战与调试篇                                  ║
╚══════════════════════════════════════════════════╝

  ── 性能基准: 锁 vs 无锁 vs 分片 ──
  std::mutex:        XXms (counter=4000000)
  atomic(seq_cst):   XXms (counter=4000000)
  atomic(relaxed):   XXms (counter=4000000)
  per-thread sharded: Xms (counter=4000000)
  thread_local:       Xms (counter=4000000)

  ── 工具链 ──
  TSan / ASAN / Relacy 使用指南 ✓

================================================================
 演示完成
================================================================
```

---

> **核心原则**: 无锁编程的本质是**用硬件 CAS 指令替代操作系统级锁**——减少上下文切换，但增加了编程复杂度。选择无锁方案前，先确认 `std::mutex` 确实是瓶颈。记住：**正确性 > 性能**，先用 mutex 写对，再用 atomic 优化。
