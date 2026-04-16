# 交易模拟器审查技能 (trading-simulation)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/trading-simulation/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行交易模拟器审查
claude /trading-simulation src/simulator/

# 指定审查维度
claude /trading-simulation src/simulator/ --dimension fault-recovery

# 输出JSON格式报告
claude /trading-simulation src/simulator/ --output json

# 详细模式
claude /trading-simulation src/simulator/ --verbose
```

## ✨ 核心功能

- **压力测试**：验证高并发场景下的系统稳定性
- **性能基准**：评估交易处理能力与延迟指标
- **故障恢复**：检查系统故障后的恢复能力
- **容错能力**：测试异常情况下的数据一致性
- **交易流程**：验证交易路径的完整性与正确性
- **极限负载**：评估系统在峰值压力下的表现

## 📚 使用示例

### 1. 压力测试验证
```bash
# 验证10000订单/秒处理能力
claude /trading-simulation src/load_test/ --dimension pressure-test --load 10000
```

### 2. 故障恢复测试
```bash
# 模拟节点故障后的恢复测试
claude /trading-simulation src/failover/ --dimension fault-recovery --scenario node-failure
```

### 3. 容错能力评估
```bash
# 测试异常交易场景处理能力
claude /trading-simulation src/edge_cases/ --dimension fault-tolerance --cases timeout,partial_fill
```

## 🔌 集成指南

### 测试流水线集成
```yaml
- name: Trading Simulation
  run: claude /trading-simulation src/simulator/ --output json > trading_sim_report.json
```

### 定期验证配置
```bash
# 每日自动运行交易模拟测试
0 8 * * * claude /trading-simulation src/simulator/ --load 5000
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含基础交易模拟器审查功能