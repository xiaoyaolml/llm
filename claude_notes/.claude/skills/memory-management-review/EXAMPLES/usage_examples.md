# C++内存管理审查使用示例 (usage_examples.md)

## 基础使用场景

### 1. 全面内存管理审查
```bash
# 对交易引擎进行完整内存管理审查
claude /memory-management-review src/engine/
```

**输出说明**:
- 生成内存使用分析报告
- 识别内存泄漏和性能问题
- 提供内存优化建议
- 评估内存分配效率

### 2. 指定审查维度
```bash
# 审查智能指针使用
claude /memory-management-review src/ --dimension smart-pointers

# 审查内存泄漏
claude /memory-management-review src/ --dimension memory-leaks

# 审查内存池设计
claude /memory-management-review src/ --dimension memory-pool
```

**使用场景**:
- 发现内存持续增长问题时
- 优化高频分配场景时
- 代码审查阶段全面检查

### 3. 指定系统类型
```bash
# 高频交易系统（严格内存要求）
claude /memory-management-review src/ --system-type hft

# 风控系统（大数据量）
claude /memory-management-review src/ --system-type risk
```

## 金融特定场景示例

### 1. 市场数据缓存内存优化
```bash
# 检查市场数据缓存内存使用
claude /memory-management-review src/market_data/ --dimension memory-leaks --verbose
```

**重点检查**:
- 缓存对象生命周期管理
- 内存泄漏检测
- 缓存大小限制策略
- 内存碎片化问题

**示例问题**:
```markdown
### 🔴 内存泄漏问题
- **位置**: MarketDataCache::addData (Line 45)
- **问题**: 使用裸指针存储数据，析构时未释放
- **影响**: 每秒泄漏100KB内存，长期运行导致OOM
- **修复建议**: 使用智能指针或显式delete

// ❌ 泄漏代码
void addData(MarketData* data) {
    cache_[data->symbol] = data;  // 指针所有权不清晰
}

// ✅ 修复代码
void addData(std::unique_ptr<MarketData> data) {
    cache_[data->symbol] = std::move(data);
}
```

### 2. 交易对象内存池优化
```bash
# 评估交易对象池效率
claude /memory-management-review src/trading/ --dimension memory-pool
```

**关键检查项**:
- 预分配对象数量是否合理
- 对象重用率
- 内存池碎片化
- 并发访问效率

### 3. 高频场景内存分配优化
```bash
# 分析订单处理路径内存分配
claude /memory-management-review src/order/ --system-type hft --verbose
```

**优化目标**:
- 消除热路径上的动态分配
- 使用对象池替代new/delete
- 避免临时对象创建
- 优化数据结构内存布局

## 内存问题修复示例

### 1. 智能指针使用修复
```cpp
// ❌ 循环引用导致内存泄漏
class OrderListener {
    std::shared_ptr<OrderListener> partner_;
};

void link(std::shared_ptr<OrderListener> a, std::shared_ptr<OrderListener> b) {
    a->partner_ = b;
    b->partner_ = a;  // 循环引用，无法释放
}

// ✅ 使用weak_ptr打破循环
class OrderListener {
    std::weak_ptr<OrderListener> partner_;
};

void link(std::shared_ptr<OrderListener> a, std::shared_ptr<OrderListener> b) {
    a->partner_ = b;
    b->partner_ = a;  // weak_ptr不增加引用计数
}
```

### 2. 内存池实现示例
```cpp
// ❌ 频繁new/delete
Order* createOrder() {
    return new Order();  // 每次分配约100ns
}

// ✅ 对象池实现
template<typename T, size_t PoolSize>
class ObjectPool {
    alignas(64) T pool_[PoolSize];
    std::atomic<size_t> freeIndex_{PoolSize};
    
public:
    T* acquire() {
        size_t idx = freeIndex_.fetch_sub(1, std::memory_order_acquire);
        if (idx >= PoolSize) return nullptr;
        return &pool_[idx];
    }
    
    void release(T* obj) {
        // 复用对象，不释放内存
    }
};

// 使用
ObjectPool<Order, 10000> orderPool;
Order* order = orderPool.acquire();  // 约5ns
orderPool.release(order);
```

### 3. 缓存行对齐优化
```cpp
// ❌ 伪共享问题
struct alignas(1) OrderStats {
    uint64_t orderCount;
    uint64_t cancelCount;
    uint64_t fillCount;
};

class OrderProcessor {
    OrderStats stats[16];  // 多个线程竞争同一缓存行
};

// ✅ 缓存行对齐
struct alignas(64) OrderStats {
    uint64_t orderCount;
    uint64_t cancelCount;
    uint64_t fillCount;
    char padding[64 - sizeof(uint64_t) * 3];
};

class OrderProcessor {
    alignas(64) OrderStats stats[16];  // 每个stats独立缓存行
};
```

## RAII最佳实践示例

### 资源获取即初始化
```cpp
// ❌ 手动资源管理
void processTrade(TradeData* data) {
    auto* buffer = allocateBuffer();
    process(buffer, data);
    // 异常时资源泄漏
    if (hasError) return;
    freeBuffer(buffer);
}

// ✅ RAII资源管理
class BufferGuard {
    char* buffer_;
public:
    BufferGuard(size_t size) : buffer_(allocateBuffer(size)) {}
    ~BufferGuard() { freeBuffer(buffer_); }
    char* get() { return buffer_; }
};

void processTrade(TradeData* data) {
    BufferGuard buffer(1024);
    process(buffer.get(), data);
    // 异常安全：离开作用域自动释放
}
```

### Scope Guard模式
```cpp
// 使用ScopeGuard确保资源释放
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

template<typename F>
struct ScopeGuard {
    F f;
    bool dismissed;
    ScopeGuard(F&& f) : f(std::move(f)), dismissed(false) {}
    ~ScopeGuard() { if (!dismissed) f(); }
    void dismiss() { dismissed = true; }
};

template<typename F>
ScopeGuard<F> makeGuard(F&& f) {
    return ScopeGuard<F>(std::move(f));
}

// 使用示例
auto guard = makeGuard([&]() { unlock(mutex); });
lock(mutex);
// ... 受保护的操作
guard.dismiss();  // 正常路径不解锁
```

## 内存性能指标监控

### 关键指标
```bash
# 评估内存分配性能
claude /memory-management-review src/ --dimension performance-metrics
```

**指标目标**:
| 指标 | 高频交易 | 支付系统 | 风控系统 |
|------|----------|----------|----------|
| 堆分配次数/秒 | < 1,000 | < 10,000 | < 50,000 |
| 平均分配延迟 | < 10ns | < 100ns | < 500ns |
| 内存碎片率 | < 5% | < 10% | < 15% |
| 内存使用峰值 | < 1GB | < 4GB | < 16GB |

## CI/CD集成示例

### 内存泄漏检测
```yaml
- name: Memory Management Review
  run: claude /memory-management-review src/ --output json > memory_report.json
- name: Check Memory Issues
  run: |
    LEAKS=$(jq '.issues.memory_leaks' memory_report.json)
    if [ $LEAKS -gt 0 ]; then
      echo "❌ 发现内存泄漏问题"
      cat memory_report.json
      exit 1
    fi
    echo "✅ 无内存泄漏问题"
```

### 定期内存基准测试
```bash
# 每日内存性能检查
0 4 * * * claude /memory-management-review src/ --output json > /var/log/memory_weekly.log
```

## 内存调试工具集成

### Valgrind使用
```bash
# 检测内存泄漏
valgrind --leak-check=full --show-leak-kinds=all ./trading_engine

# 内存性能分析
valgrind --tool=massif ./trading_engine
ms_print massif.out > memory_usage.txt
```

### AddressSanitizer
```bash
# 编译时启用ASan
clang++ -fsanitize=address -g src/*.cpp -o trading_engine

# 运行检测内存错误
./trading_engine
```