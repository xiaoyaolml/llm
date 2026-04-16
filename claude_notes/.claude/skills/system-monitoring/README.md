# 系统监控审查技能 (system-monitoring)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/system-monitoring/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行系统监控审查
claude /system-monitoring src/monitoring/

# 指定审查维度
claude /system-monitoring src/monitoring/ --dimension alert-rules

# 输出JSON格式报告
claude /system-monitoring src/monitoring/ --output json

# 详细模式
claude /system-monitoring src/monitoring/ --verbose
```

## ✨ 核心功能

- **监控指标**：验证系统指标完整性与准确性
- **告警规则**：检查阈值设置合理性与告警有效性
- **日志收集**：评估日志完整性与结构化程度
- **性能阈值**：验证关键路径性能监控设置
- **仪表板**：审查可视化配置与关键指标覆盖
- **历史数据**：评估数据存储策略与分析能力

## 📚 使用示例

### 1. 监控指标审查
```bash
# 验证核心交易指标是否完整
claude /system-monitoring src/metrics/ --dimension metrics-completeness --target trading
```

### 2. 告警规则验证
```bash
# 检查高频交易系统告警阈值
claude /system-monitoring src/alerts/ --dimension alert-rules --system-type hft
```

### 3. 日志收集评估
```bash
# 验证关键操作日志是否完整
claude /system-monitoring src/logs/ --dimension log-completeness --critical-operations
```

## 🔌 集成指南

### Prometheus/Grafana 集成
```yaml
- name: System Monitoring Review
  run: claude /system-monitoring src/monitoring/ --output json > monitoring_report.json
```

### 定期审查配置
```bash
# 每日自动运行监控审查
0 6 * * * claude /system-monitoring src/monitoring/ --high-priority-only
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含基础系统监控审查功能