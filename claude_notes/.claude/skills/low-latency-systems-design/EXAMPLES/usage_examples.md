# 低延迟系统设计使用示例 (usage_examples.md)

## 基础使用场景

### 1. 全面低延迟系统设计审查
```bash
# 对交易引擎进行完整低延迟设计审查
claude /low-latency-systems-design src/engine/
```

**输出说明**:
- 生成端到端延迟预算分析
- 识别关键路径上的延迟瓶颈
- 提供延迟优化建议和预期收益
- 评估系统是否满足延迟目标

### 2. 指定延迟目标
```bash
# 检查10微秒延迟目标达成情况
claude /low-latency-systems-design src/engine/ --latency-target 10us

# 检查1毫秒延迟目标
claude /low-latency-systems-design src/gateway/ --latency-target 1ms
```

**使用场景**:
- 高频交易系统：10-100微秒
- 交易所网关：50-500微秒
- 支付系统：100毫秒以内

### 3. 指定审查维度
```bash
# 审查内存架构设计
claude /low-latency-systems-design src/engine/ --dimension memory-architecture

# 审查网络栈优化
claude /low-latency-systems-design src/network/ --dimension network-stack

# 审查并发模型
claude /low-latency-systems-design src/concurrency/ --dimension concurrency-model
```

## 金融特定场景示例

### 1. 高频交易引擎优化
```bash
# 审查高频交易引擎延迟
claude /low-latency-systems-design src/hft_engine/ --latency-target 10us --verbose
```

**重点检查**:
- 订单处理路径延迟（目标<5us）
- 市场数据处理延迟（目标<2us）
- 风控检查延迟（目标<2us）
- 网络发送延迟（目标<1us）

**示例问题**:
```markdown
### 🔴 关键路径延迟超标
- **位置**: OrderProcessor::processOrder (Line 128)
- **当前延迟**: 8500ns
- **目标延迟**: 5000ns
- **问题**: 动态内存分配导致延迟抖动
- **建议**: 使用对象池预分配订单对象
```

### 2. 网络栈优化评估
```bash
# 评估DPDK集成效果
claude /low-latency-systems-design src/network/ --dimension kernel-bypass
```

**优化建议**:
- 使用DPDK绕过内核网络栈
- 实施零拷贝数据包处理
- 配置网卡多队列和RSS
- 优化中断处理策略

### 3. 内存布局优化
```bash
# 检查缓存行对齐和内存池设计
claude /low-latency-systems-design src/memory/ --dimension cache-alignment
```

**关键检查项**:
- 是否使用alignas(64)对齐数据结构
- 是否避免伪共享问题
- 是否使用对象池预分配
- 是否优化数据局部性

## 延迟预算分析示例

### 端到端延迟分解
```markdown
## 交易处理延迟预算 (目标: 10μs)

| 组件 | 预算 | 实际 | 状态 |
|------|------|------|------|
| 网络接收 | 1.0μs | 0.8μs | ✅ |
| 协议解析 | 1.5μs | 1.8μs | ⚠️ |
| 订单校验 | 0.5μs | 0.4μs | ✅ |
| 风控检查 | 1.0μs | 1.2μs | ⚠️ |
| 订单匹配 | 3.0μs | 3.5μs | ❌ |
| 状态更新 | 0.5μs | 0.4μs | ✅ |
| 网络发送 | 1.5μs | 1.3μs | ✅ |
| **总计** | **9.0μs** | **9.4μs** | ⚠️ |
```

### 优化建议优先级
1. **P0 - 订单匹配优化**: 使用无锁队列，预期改善500ns
2. **P1 - 协议解析优化**: 切换到SBE二进制协议，预期改善400ns
3. **P2 - 风控检查优化**: 预计算风控因子，预期改善200ns

## 性能优化示例

### 1. 无锁队列优化
```cpp
// ❌ 阻塞队列导致延迟
std::queue<Order> orderQueue;
std::mutex mutex;

void processOrders() {
    while (true) {
        std::lock_guard<std::mutex> lock(mutex);
        if (orderQueue.empty()) break;
        auto order = orderQueue.front();
        orderQueue.pop();
        process(order);
    }
}

// ✅ 无锁队列实现
template<typename T, size_t Size>
class LockFreeQueue {
    alignas(64) std::atomic<size_t> head{0};
    alignas(64) std::atomic<size_t> tail{0};
    alignas(64) T buffer[Size];
    
public:
    bool push(const T& item) {
        size_t t = tail.load(std::memory_order_relaxed);
        size_t h = head.load(std::memory_order_acquire);
        if (t - h >= Size) return false;
        buffer[t % Size] = item;
        tail.store(t + 1, std::memory_order_release);
        return true;
    }
};
```

### 2. 缓存行对齐优化
```cpp
// ❌ 伪共享问题
struct TradeData {
    double price;
    double volume;
    int status;
    int flags;  // 与其他线程共享同一缓存行
};

// ✅ 缓存行对齐
struct alignas(64) TradeData {
    double price;
    double volume;
    int status;
    int flags;
    // 填充到64字节边界
    char padding[64 - sizeof(double)*2 - sizeof(int)*2];
};
```

## CI/CD集成示例

### 延迟基准测试
```yaml
- name: Low-Latency Design Review
  run: claude /low-latency-systems-design src/ --latency-target 10us --output json > latency_report.json
- name: Check Latency Budget
  run: |
    LATENCY=$(jq '.budget.remaining_ns' latency_report.json)
    if [ $LATENCY -lt 0 ]; then
      echo "❌ 延迟预算超出"
      cat latency_report.json
      exit 1
    fi
    echo "✅ 延迟预算满足: ${LATENCY}ns 余量"
```

## 实时延迟监控

### 设置延迟监控
```bash
# 每5分钟检查延迟指标
/loop 5m claude /low-latency-systems-design src/engine/ --latency-target 10us
```

**监控指标**:
- P50, P90, P99, P99.9延迟
- 延迟抖动(标准差)
- 延迟超标次数
- 优化效果趋势