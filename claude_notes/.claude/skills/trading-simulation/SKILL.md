---
name: trading-simulation
description: 交易模拟器审查技能 - 审查金融交易系统的压力测试、性能基准测试、故障恢复测试和容错能力评估
---

# 🤖 交易模拟器审查技能 (trading-simulation)

## 🎯 功能概述

此技能专门用于金融交易系统的模拟器审查，确保交易系统在各种压力场景下能够稳定运行，特别关注高频交易和量化交易环境中的性能、可靠性、容错能力等关键指标。

## 🔧 支持的测试类型

- 压力测试 (Stress Testing)
- 性能基准测试 (Benchmark Testing)
- 故障恢复测试 (Failover Testing)
- 容错能力评估 (Fault Tolerance Assessment)
- 交易流仿真 (Trading Flow Simulation)

## 📋 主要审查功能

### 1. 压力测试审查
- 检查交易负载是否合理设置
- 验证系统在峰值流量下的表现
- 审查资源使用情况（CPU、内存、网络）
- 确保系统具备足够的处理能力

### 2. 性能基准测试审查
- 检查响应时间指标是否达标
- 验证吞吐量是否满足业务要求
- 审查延迟分布情况
- 确保关键交易路径的性能表现

### 3. 故障恢复测试审查
- 检查系统故障检测机制
- 验证自动恢复流程的有效性
- 审查数据一致性保障措施
- 确保故障隔离和容错能力

### 4. 容错能力评估
- 检查系统对网络分区的处理
- 验证服务降级机制
- 审查异常处理和错误恢复
- 确保系统在部分组件失效时仍能运行

### 5. 交易流仿真
- 检查交易流程的完整性和正确性
- 验证交易执行的原子性
- 审查订单处理的时序正确性
- 确保交易状态的一致性

## 🛠 使用方法

```bash
# 基本使用
claude /trading-simulation simulation-config/

# 指定测试类型
claude /trading-simulation simulation-config/ --test-type stress

# 指定性能指标
claude /trading-simulation simulation-config/ --benchmark cpu,mem,latency

# 输出JSON格式报告
claude /trading-simulation simulation-config/ --output json

# 指定特定审查模块
claude /trading-simulation simulation-config/ --module performance,fault-tolerance

# 详细模式
claude /trading-simulation simulation-config/ --verbose
```

## 📊 输出格式

### Markdown 输出
```markdown
# 交易模拟器审查报告

## 概述
- 系统名称: high_frequency_trading_system
- 测试时间: 2026-04-09 10:30:00
- 测试类型: 压力测试
- 测试规模: 10,000 TPS

## 测试结果

### 性能基准
- ✅ 响应时间 < 1ms (99%)
- ✅ 吞吐量 50,000 TPS
- ✅ CPU使用率 < 70%
- ⚠️ 内存使用率 85% (接近上限)

### 压力测试
- ✅ 系统在10,000 TPS下稳定运行
- ✅ 无连接超时或丢包
- ⚠️ 在峰值时出现轻微延迟
- ✅ 数据一致性保持良好

### 故障恢复
- ✅ 故障检测时间 < 1秒
- ✅ 自动恢复时间 < 30秒
- ✅ 数据一致性验证通过
- ✅ 无数据丢失

### 容错能力
- ✅ 网络分区处理正常
- ✅ 服务降级机制有效
- ✅ 异常处理完整
- ✅ 部分组件失效不影响整体

## 建议
1. 优化内存使用，降低使用率
2. 提升峰值处理能力
3. 完善故障恢复流程细节
4. 加强异常处理逻辑
```

### JSON 输出
```json
{
  "system_name": "high_frequency_trading_system",
  "timestamp": "2026-04-09T10:30:00Z",
  "test_type": "stress",
  "test_scale": "10000 TPS",
  "test_results": {
    "performance_benchmark": {
      "status": "warning",
      "metrics": [
        {"name": "response_time", "value": "< 1ms", "status": "pass"},
        {"name": "throughput", "value": "50,000 TPS", "status": "pass"},
        {"name": "cpu_usage", "value": "< 70%", "status": "pass"},
        {"name": "memory_usage", "value": "85%", "status": "warning"}
      ]
    },
    "stress_test": {
      "status": "warning",
      "metrics": [
        {"name": "system_stability", "value": "10,000 TPS", "status": "pass"},
        {"name": "connection_timeout", "value": "0", "status": "pass"},
        {"name": "latency_peak", "value": "1.2ms", "status": "warning"},
        {"name": "data_consistency", "value": "pass", "status": "pass"}
      ]
    },
    "failover_test": {
      "status": "pass",
      "metrics": [
        {"name": "failure_detection", "value": "< 1s", "status": "pass"},
        {"name": "auto_recovery", "value": "< 30s", "status": "pass"},
        {"name": "data_integrity", "value": "pass", "status": "pass"},
        {"name": "data_loss", "value": "0", "status": "pass"}
      ]
    },
    "fault_tolerance": {
      "status": "pass",
      "metrics": [
        {"name": "network_partition", "value": "handled", "status": "pass"},
        {"name": "service_degradation", "value": "effective", "status": "pass"},
        {"name": "exception_handling", "value": "complete", "status": "pass"},
        {"name": "component_failure", "value": "isolated", "status": "pass"}
      ]
    }
  },
  "recommendations": [
    "优化内存使用，降低使用率",
    "提升峰值处理能力",
    "完善故障恢复流程细节",
    "加强异常处理逻辑"
  ]
}
```

## 🔍 审查规则

### 压力测试规则
- ✅ 系统应能承受预期的最大交易负载
- ✅ 在峰值负载下应保持稳定的性能指标
- ✅ 应避免系统过载导致的崩溃
- ✅ 应有明确的容量规划和扩展策略

### 性能基准规则
- ✅ 响应时间应符合业务SLA要求
- ✅ 吞吐量应满足峰值业务需求
- ✅ 系统资源使用率应合理分布
- ✅ 应具备性能监控和告警机制

### 故障恢复规则
- ✅ 应有快速的故障检测机制
- ✅ 应具备自动恢复能力
- ✅ 应保证数据一致性和完整性
- ✅ 应进行故障恢复演练

### 容错能力规则
- ✅ 应具备网络分区处理能力
- ✅ 应有服务降级和熔断机制
- ✅ 应完善异常处理和错误恢复
- ✅ 应确保部分组件失效不影响整体系统

### 交易流规则
- ✅ 交易流程应完整且逻辑正确
- ✅ 交易执行应具备原子性
- ✅ 交易时序应保持正确
- ✅ 交易状态应一致可靠

## 🧪 示例用法

### 验证交易系统压力测试
```bash
claude /trading-simulation stress-tests/ --test-type stress --benchmark all
```

### 审查系统性能基准
```bash
claude /trading-simulation benchmarks/ --module performance --output json
```

### 检查故障恢复能力
```bash
claude /trading-simulation failover-tests/ --module fault-tolerance --validate-recovery
```

### 完整交易流程仿真
```bash
claude /trading-simulation trading-flows/ --validate-all --output markdown
```

## 📚 注意事项

1. **环境准备**：需要准备模拟测试环境
2. **测试数据**：需要准备真实或模拟的交易数据
3. **监控配置**：需要配置实时监控和告警
4. **性能考虑**：大规模测试可能消耗大量系统资源
5. **合规要求**：确保测试过程符合金融监管要求

## 🚀 适用场景

### 1. 交易系统上线前验证
- 验证系统承载能力
- 确保性能达标
- 检查故障恢复能力
- 评估容错能力

### 2. 系统升级后验证
- 验证升级后的性能改进
- 检查兼容性
- 验证稳定性
- 确认故障处理机制

### 3. 重大变更审查
- 验证新增功能影响
- 检查系统边界条件
- 评估风险和脆弱性
- 确保业务连续性

### 4. 定期回归测试
- 持续监控系统性能
- 验证长期运行稳定性
- 检查容量规划
- 确保持续改进

## 📈 与金融开发技能组的协同

此技能与以下金融开发技能配合使用效果最佳：

- `/code-review` - 通用代码审查
- `/finance-optim` - 金融性能优化
- `/concurrency-review` - 高频并发审查
- `/risk-check` - 风险计算验证
- `/market-data-validate` - 市场数据质量验证
- `/quant-backtest` - 量化策略回测验证

通过组合使用这些技能，可以建立完整的金融交易系统开发和验证流程。