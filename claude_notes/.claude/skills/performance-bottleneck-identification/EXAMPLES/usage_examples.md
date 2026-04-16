# 性能瓶颈识别技能使用示例

## 示例 1: 基本使用

```bash
# 审查整个系统的性能瓶颈
claude /performance-bottleneck-identification src/

# 输出结果将包含：
# - CPU瓶颈分析
# - 内存瓶颈分析
# - I/O瓶颈分析
# - 网络瓶颈分析
# - 数据库瓶颈分析
# - 算法瓶颈分析
# - 并发瓶颈分析
# - 缓存瓶颈分析
```

## 示例 2: 指定审查维度

```bash
# 仅审查CPU瓶颈
claude /performance-bottleneck-identification src/ --dimension cpu

# 仅审查内存瓶颈
claude /performance-bottleneck-identification src/ --dimension memory

# 审查多个维度
claude /performance-bottleneck-identification src/ --dimension cpu,memory,database
```

## 示例 3: 指定性能场景

```bash
# 高频交易场景
claude /performance-bottleneck-identification src/ --scenario high-frequency-trading

# 支付系统场景
claude /performance-bottleneck-identification src/ --scenario payment-system

# 报表系统场景
claude /performance-bottleneck-identification src/ --scenario reporting-system
```

## 示例 4: 输出不同格式

```bash
# Markdown格式输出（默认）
claude /performance-bottleneck-identification src/

# JSON格式输出
claude /performance-bottleneck-identification src/ --output json

# HTML格式输出
claude /performance-bottleneck-identification src/ --output html

# 输出到文件
claude /performance-bottleneck-identification src/ --output json > bottleneck-report.json
```

## 示例 5: 详细模式

```bash
# 详细输出，包含所有检查项
claude /performance-bottleneck-identification src/ --verbose

# 简洁输出，只显示瓶颈和建议
claude /performance-bottleneck-identification src/ --quiet
```

## 示例 6: 针对性审查

```cpp
// 代码示例：待审查的CPU密集型代码
class TradingEngine {
public:
    void processMarketData(const MarketData& data) {
        // CPU密集型计算
        for (const auto& order : orders) {
            if (order.price == data.price) {
                // 复杂计算
                double result = 0;
                for (int i = 0; i < 1000; i++) {
                    result += std::sin(data.price * i) * std::cos(data.volume);
                }
                updateOrder(order, result);
            }
        }
    }

private:
    std::vector<Order> orders;
};
```

```bash
# 审查上述代码的CPU瓶颈
claude /performance-bottleneck-identification src/trading_engine.cpp --dimension cpu --verbose
```

## 示例 7: 批量审查

```bash
# 审查整个项目的性能瓶颈
claude /performance-bottleneck-identification src/ --recursive

# 指定文件模式
claude /performance-bottleneck-identification src/ --pattern "*_engine.cpp"
```

## 示例 8: 集成到CI/CD

```yaml
# GitHub Actions 示例
name: Performance Bottleneck Check

on: [push, pull_request]

jobs:
  bottleneck-check:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Run Performance Bottleneck Check
      run: |
        claude /performance-bottleneck-identification src/ --output json > bottleneck.json

    - name: Check Critical Bottlenecks
      run: |
        if jq '.bottlenecks | any(.status == "critical")' bottleneck.json; then
          echo "发现严重性能瓶颈，需要立即修复"
          exit 1
        fi
```

## 示例 9: 性能监控集成

```bash
# 定期性能瓶颈检测脚本
#!/bin/bash
# daily_bottleneck_check.sh

DATE=$(date +%Y-%m-%d)
REPORT_DIR="reports/bottlenecks"
mkdir -p $REPORT_DIR

# 执行瓶颈检测
claude /performance-bottleneck-identification src/ \
  --output json \
  --verbose \
  > $REPORT_DIR/bottleneck-$DATE.json

# 提取关键信息
jq '.bottlenecks | to_entries | map({
  dimension: .key,
  status: .value.status,
  issues_count: (.value.issues | length)
})' $REPORT_DIR/bottleneck-$DATE.json > $REPORT_DIR/summary-$DATE.json

echo "性能瓶颈检测报告已生成: $REPORT_DIR/bottleneck-$DATE.json"
```

## 示例 10: 瓶颈修复验证

```bash
# 修复前检测
claude /performance-bottleneck-identification src/ --output json > before-fix.json

# 修复代码...

# 修复后检测
claude /performance-bottleneck-identification src/ --output json > after-fix.json

# 对比结果
diff before-fix.json after-fix.json
```

## 常见使用场景

### 1. 系统性能下降时
```bash
# 系统变慢时进行瓶颈分析
claude /performance-bottleneck-identification src/ --scenario production
```

### 2. 新功能开发
```bash
# 新功能开发时检查性能影响
claude /performance-bottleneck-identification src/features/new-feature/
```

### 3. 上线前检查
```bash
# 上线前全面性能检查
claude /performance-bottleneck-identification src/ --verbose --output json
```

### 4. 定期维护
```bash
# 每周定期性能检查
0 2 * * 1 claude /performance-bottleneck-identification src/ --output json > /tmp/weekly-bottleneck.json
```

### 5. 性能优化项目
```bash
# 性能优化项目开始时的基线检测
claude /performance-bottleneck-identification src/ --baseline baseline.json
```

## 输出示例

### Markdown输出示例
```markdown
# 性能瓶颈识别报告

## 概述
- 系统名称: trading-engine
- 审查时间: 2026-04-09 10:30:00
- 性能场景: 高频交易系统
- 审查范围: 全部维度

## 瓶颈识别结果

### CPU瓶颈 (⚠️ 警告)
- 🔍 发现问题: 市场数据处理函数CPU使用率过高
- 📍 位置: TradingEngine::processMarketData
- ⚡ 影响程度: 中等
- 💡 建议优化: 使用预计算表格优化三角函数计算

### 内存瓶颈 (❌ 严重)
- 🔍 发现问题: 订单列表内存使用持续增长
- 📍 位置: OrderManager::orders
- ⚡ 影响程度: 严重
- 💡 建议优化: 实现订单清理机制，移除已完成订单

### 数据库瓶颈 (⚠️ 警告)
- 🔍 发现问题: 历史订单查询存在慢查询
- 📍 位置: OrderRepository::findHistoricalOrders
- ⚡ 影响程度: 中等
- 💡 建议优化: 添加时间范围索引

### 网络瓶颈 (✅ 正常)
- ✅ 网络延迟在可接受范围内
- ✅ 带宽使用率正常
- ✅ 连接管理良好

## 优化优先级
1. **P0 - 立即修复**: 内存泄漏问题
2. **P1 - 本版本**: CPU热点优化
3. **P2 - 下版本**: 数据库查询优化

## 预期效果
- 修复内存问题后，系统稳定性提升80%
- 优化CPU热点后，处理能力提升30%
- 优化数据库后，查询速度提升50%
```

### JSON输出示例
```json
{
  "system": "trading-engine",
  "review_time": "2026-04-09 10:30:00",
  "performance_scenario": "high-frequency-trading",
  "overall_status": "warning",
  "bottlenecks": {
    "cpu": {
      "status": "warning",
      "score": 65,
      "issues": [
        {
          "location": "TradingEngine::processMarketData",
          "function": "sin/cos计算循环",
          "issue": "CPU使用率过高",
          "severity": "medium",
          "impact": "处理延迟增加20%",
          "recommendation": "使用预计算表格优化三角函数计算"
        }
      ]
    },
    "memory": {
      "status": "critical",
      "score": 35,
      "issues": [
        {
          "location": "OrderManager::orders",
          "function": "订单列表管理",
          "issue": "内存使用持续增长",
          "severity": "high",
          "impact": "系统稳定性严重下降",
          "recommendation": "实现订单清理机制，移除已完成订单"
        }
      ]
    },
    "database": {
      "status": "warning",
      "score": 70,
      "issues": [
        {
          "location": "OrderRepository::findHistoricalOrders",
          "function": "历史订单查询",
          "issue": "慢查询",
          "severity": "medium",
          "impact": "查询响应时间增加50%",
          "recommendation": "添加时间范围索引"
        }
      ]
    },
    "network": {
      "status": "pass",
      "score": 90,
      "issues": []
    }
  },
  "overall_score": 65,
  "optimization_priority": [
    {
      "priority": "P0",
      "bottleneck": "memory",
      "description": "修复内存泄漏问题",
      "estimated_improvement": "80%"
    },
    {
      "priority": "P1",
      "bottleneck": "cpu",
      "description": "优化CPU热点",
      "estimated_improvement": "30%"
    },
    {
      "priority": "P2",
      "bottleneck": "database",
      "description": "优化数据库查询",
      "estimated_improvement": "50%"
    }
  ],
  "recommendations": [
    "立即修复内存泄漏问题",
    "优化三角函数计算使用预计算表格",
    "为历史订单查询添加索引",
    "定期监控内存使用情况"
  ]
}
```

## 高级用法

### 自定义瓶颈阈值
```bash
# 使用自定义性能阈值
claude /performance-bottleneck-identification src/ --thresholds custom-thresholds.json
```

### 基线对比
```bash
# 与性能基线对比
claude /performance-bottleneck-identification src/ --baseline performance-baseline.json
```

### 瓶颈趋势分析
```bash
# 分析瓶颈趋势
claude /performance-bottleneck-identification src/ --trend-analysis --history reports/
```

### 实时监控集成
```bash
# 集成实时监控数据
claude /performance-bottleneck-identification src/ --monitoring-data prometheus-metrics.json
```

## 性能优化建议模板

### CPU优化建议
```cpp
// 优化前
for (int i = 0; i < 1000; i++) {
    result += std::sin(angle * i) * std::cos(angle * i);
}

// 优化后
// 使用预计算表格
static const std::array<double, 1000> sin_table = [] {
    std::array<double, 1000> table;
    for (int i = 0; i < 1000; i++) {
        table[i] = std::sin(angle * i) * std::cos(angle * i);
    }
    return table;
}();

for (int i = 0; i < 1000; i++) {
    result += sin_table[i];
}
```

### 内存优化建议
```cpp
// 优化前
class OrderManager {
private:
    std::vector<Order> orders;  // 只增不减
public:
    void addOrder(const Order& order) {
        orders.push_back(order);
    }
};

// 优化后
class OrderManager {
private:
    std::deque<Order> orders;
    static constexpr size_t MAX_ORDERS = 10000;
public:
    void addOrder(const Order& order) {
        orders.push_back(order);
        if (orders.size() > MAX_ORDERS) {
            orders.pop_front();  // 清理旧订单
        }
    }
};
```

---

通过这些示例，您可以了解如何在不同场景下使用性能瓶颈识别技能，快速定位和解决系统性能问题。