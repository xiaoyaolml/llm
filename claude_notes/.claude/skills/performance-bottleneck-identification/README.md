# 性能瓶颈识别技能 (performance-bottleneck-identification)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/performance-bottleneck-identification/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行性能瓶颈识别
claude /performance-bottleneck-identification src/

# 指定审查维度
claude /performance-bottleneck-identification src/ --dimension memory

# 输出JSON格式报告
claude /performance-bottleneck-identification src/ --output json

# 详细模式
claude /performance-bottleneck-identification src/ --verbose
```

## ✨ 核心功能

- **CPU瓶颈**：识别热点函数、评估算法复杂度
- **内存瓶颈**：检测内存泄漏、分析内存碎片
- **I/O瓶颈**：评估磁盘性能、优化文件访问模式
- **网络瓶颈**：检查延迟和带宽、优化协议使用
- **数据库瓶颈**：分析慢查询、验证索引使用
- **并发瓶颈**：检查线程竞争、优化锁使用
- **缓存瓶颈**：评估命中率、优化缓存策略

## 📚 使用示例

### 1. 高频交易系统分析
```bash
# 识别交易引擎性能瓶颈
claude /performance-bottleneck-identification src/engine/ --system-type hft --verbose
```

### 2. 内存泄漏检测
```bash
# 检测市场数据缓存内存问题
claude /performance-bottleneck-identification src/cache/ --dimension memory --scenario leak-detection
```

### 3. 数据库性能分析
```bash
# 优化订单查询性能
claude /performance-bottleneck-identification src/db/ --dimension database --target order-query
```

## 🔌 集成指南

### CI/CD 集成
```yaml
- name: Performance Bottleneck Identification
  run: claude /performance-bottleneck-identification src/ --output json > perf_report.json
```

### 定期性能监控
```bash
# 每日自动运行瓶颈识别
0 3 * * * claude /performance-bottleneck-identification src/ --high-priority-only
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含核心性能瓶颈识别功能