# C++ 金融代码性能检查清单

## 必查项 (高危)

- [ ] **动态内存分配**：高频路径是否避免 `new`/`delete`
```cpp
// ❌ 禁止在高频路径使用
auto result = new PricingResult();

// ✅ 使用对象池
static thread_local ObjectPool<PricingResult> pool;
auto result = pool.allocate();
```

- [ ] **虚函数调用**：关键路径是否避免虚函数
```cpp
// ❌ 高频路径避免
virtual double calculate() const;

// ✅ 使用 CRTP
template<typename Derived>
class PricingBase {
    double calculate() const {
        return static_cast<Derived*>(this)->calculate_impl();
    }
};
```

- [ ] **锁竞争**：高频操作是否使用无锁编程
```cpp
// ❌ 避免互斥锁
std::mutex mtx;
std::lock_guard lock(mtx);

// ✅ 使用原子操作
std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);
```

- [ ] **货币计算**：是否使用整数运算
```cpp
// ❌ 禁止使用 double
double price = base_price * 1.001;

// ✅ 使用整数（单位：分）
int64_t price_cents = base_cents * 1001 / 1000;
```

## 优化建议 (中危)

- [ ] **数据结构优化**：
```cpp
// ❌ 避免 std::map 用于高频查找
std::map<int, Order> orders;

// ✅ 使用 std::unordered_map
std::unordered_map<int, Order> orders;
```

- [ ] **SIMD 优化**：
```cpp
// ❌ 标量计算
for (size_t i = 0; i < prices.size(); ++i) {
    returns[i] = (prices[i] - prev_prices[i]) / prev_prices[i];
}

// ✅ SIMD 优化（使用 Eigen 或手写）
Eigen::ArrayXd prices_arr = Eigen::Map<Eigen::ArrayXd>(prices.data(), prices.size());
Eigen::ArrayXd prev_arr = Eigen::Map<Eigen::ArrayXd>(prev_prices.data(), prev_prices.size());
Eigen::ArrayXd returns_arr = (prices_arr - prev_arr) / prev_arr;
```

- [ ] **内存对齐**：
```cpp
// ❌ 未对齐
struct MarketData {
    double price;
    int volume;
};

// ✅ 缓存行对齐
struct alignas(64) MarketData {
    double price;
    int volume;
    char padding[52]; // 填充到 64 字节
};
```

## 金融特定模式

### 1. 交易引擎优化

| 模式 | 风险 | 优化建议 |
|------|------|----------|
| `std::map` 价格档位 | O(log n) 查找 | 改用 `std::unordered_map` 或数组索引 |
| 互斥锁保护订单簿 | 锁竞争延迟 | 无锁订单簿 + CAS 操作 |
| 单线程处理 | 吞吐量瓶颈 | 多生产者-单消费者队列 |
| 未预热缓存 | 冷启动延迟 | 启动时预热关键数据结构 |

### 2. 风险计算优化

| 模式 | 风险 | 优化建议 |
|------|------|----------|
| 串行蒙特卡洛 | 计算慢 | 并行计算 + SIMD 向量化 |
| 动态内存分配 | 碎片化 | 预分配路径存储空间 |
| 未缓存希腊字母 | 重复计算 | 缓存常见参数的希腊字母 |
| 单精度浮点 | 精度损失 | 双精度或定点数 |

### 3. 市场数据优化

| 模式 | 风险 | 优化建议 |
|------|------|----------|
| 逐条处理 tick | 高延迟 | 批量处理 + 向量化 |
| 未压缩存储 | 内存浪费 | Delta 编码 + RLE 压缩 |
| 随机访问 | 缓存未命中 | 连续内存布局 + 预取 |
| 未分片处理 | 瓶颈 | 按 symbol 分片 + 并行处理 |

## 代码示例对比

### 差的实现 (延迟 250μs)
```cpp
class PricingEngine {
public:
    virtual ~PricingEngine() = default;
    virtual double price(const Option& option) {
        auto result = new PricingResult(); // 动态分配
        // 复杂计算...
        return result->value;
    }
};

// 使用 double 计算货币
double calculate_pnl(double initial, double final) {
    return final - initial;
}
```

### 优化的实现 (延迟 85μs)
```cpp
template<typename Model>
class PricingEngine {
public:
    double price(const Option& option) {
        static thread_local ObjectPool<PricingResult> pool;
        auto result = pool.allocate(); // 对象池
        // 复杂计算...
        return result->value;
    }
};

// 使用整数计算货币
int64_t calculate_pnl_cents(int64_t initial_cents, int64_t final_cents) {
    return final_cents - initial_cents;
}
```

## 编译器优化提示

### GCC/Clang 优化
```cpp
// 内联提示
inline __attribute__((always_inline)) double fast_sqrt(double x) {
    return __builtin_sqrt(x);
}

// 分支预测
if (__builtin_expect(condition, 1)) { // 预期为 true
    // 快速路径
}

// 缓存预取
void process_data(const std::vector<double>& data) {
    __builtin_prefetch(&data[16], 0, 3); // 预取 16 个元素后
    // 处理 data[0]
}
```

### MSVC 优化
```cpp
// 内联提示
__forceinline double fast_sqrt(double x) {
    return sqrt(x);
}

// 循环展开
#pragma loop(ivdep)
for (int i = 0; i < N; ++i) {
    // 独立操作
}
```

## 性能测试框架

```cpp
// 性能测试模板
template<typename Func>
double benchmark(Func func, int iterations = 1000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return static_cast<double>(duration.count()) / iterations;
}

// 使用示例
double avg_latency = benchmark([&]() {
    engine.price(option);
}, 10000);
std::cout << "Average latency: " << avg_latency << "μs" << std::endl;
```

## 金融系统架构检查

### 1. 低延迟架构
- [ ] CPU 亲和性设置
- [ ] 网络绑核优化
- [ ] 内存池预分配
- [ ] 无锁通信队列

### 2. 高吞吐架构
- [ ] 负载均衡分片
- [ ] 异步 I/O 处理
- [ ] 批量处理优化
- [ ] SIMD 向量化

### 3. 高可用架构
- [ ] 热备切换机制
- [ ] 状态同步延迟
- [ ] 故障恢复时间
- [ ] 数据一致性保证

## 更新日志

- **v1.0** (2026-04-08): 初始版本
  - C++ 金融代码性能检查清单
  - 交易引擎、风险计算、市场数据优化
  - 编译器优化提示和性能测试框架