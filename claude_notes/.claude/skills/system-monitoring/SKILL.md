---
name: system-monitoring
description: 系统监控审查技能 - 审查金融系统的监控指标完整性、告警规则合理性、日志收集完整性和性能阈值设置
---

# 📈 系统监控审查技能 (system-monitoring)

## 🎯 功能概述

此技能专门用于金融系统的监控体系审查，确保关键业务系统具备完善的监控指标、合理的告警机制、完整的日志收集和科学的性能阈值设置，特别关注金融行业对系统稳定性、可靠性和合规性的严格要求。

## 🔧 支持的监控维度

- 监控指标完整性 (Monitoring Metrics)
- 告警规则合理性 (Alerting Rules)
- 日志收集完整性 (Logging)
- 性能阈值设置 (Performance Thresholds)
- 仪表板设计 (Dashboards)
- 审计追踪 (Auditing)

## 📋 主要审查功能

### 1. 监控指标完整性
- 检查关键指标是否覆盖所有系统组件
- 验证业务指标与技术指标的关联性
- 审查指标数据采集频率和准确性
- 确保指标能够反映系统健康状态

### 2. 告警规则合理性
- 检查告警阈值设置是否合理
- 验证告警优先级和分级
- 审查告警通知机制和渠道
- 确保告警准确性和及时性

### 3. 日志收集完整性
- 检查日志采集范围是否完整
- 验证日志格式和标准化
- 审查日志存储周期和保留策略
- 确保日志可检索性和分析能力

### 4. 性能阈值设置
- 检查性能阈值是否基于业务需求
- 验证阈值的动态调整机制
- 审查阈值与系统容量的关系
- 确保阈值能够预测潜在问题

### 5. 仪表板设计
- 检查仪表板是否突出关键指标
- 验证数据可视化合理性和有效性
- 审查仪表板的可操作性和实用性
- 确保仪表板支持快速问题诊断

### 6. 审计追踪
- 检查系统变更的审计日志
- 验证安全事件的追踪能力
- 审查用户操作的记录完整性
- 确保符合监管审计要求

## 🛠 使用方法

```bash
# 基本使用
claude /system-monitoring monitoring-config/

# 指定监控维度
claude /system-monitoring monitoring-config/ --dimension metrics

# 指定审查范围
claude /system-monitoring monitoring-config/ --scope production

# 输出JSON格式报告
claude /system-monitoring monitoring-config/ --output json

# 指定特定审查模块
claude /system-monitoring monitoring-config/ --module alerts,logs

# 详细模式
claude /system-monitoring monitoring-config/ --verbose
```

## 📊 输出格式

### Markdown 输出
```markdown
# 系统监控审查报告

## 概述
- 系统名称: trading_platform
- 审查时间: 2026-04-09 10:30:00
- 监控平台: Prometheus + Grafana

## 审查结果

### 监控指标
- ✅ 关键指标覆盖全面
- ⚠️ 业务指标缺少部分关键指标
- ✅ 指标采集频率合理
- ⚠️ 指标准确性有待验证

### 告警规则
- ✅ 阈值设置合理
- ✅ 优先级分级明确
- ⚠️ 告警通知渠道不完整
- ✅ 告警准确率良好

### 日志收集
- ✅ 采集范围完整
- ⚠️ 日志格式部分不规范
- ✅ 存储策略合理
- ✅ 可检索性良好

### 性能阈值
- ✅ 阈值基于业务需求
- ⚠️ 动态调整机制缺失
- ✅ 与容量关系明确
- ✅ 预测能力良好

### 仪表板
- ✅ 关键指标突出显示
- ⚠️ 可视化效果有待提升
- ✅ 可操作性良好
- ✅ 问题诊断快速

### 审计追踪
- ✅ 系统变更记录完整
- ✅ 安全事件追踪有效
- ✅ 用户操作记录完整
- ✅ 审计合规性良好

## 建议
1. 补充缺失的业务关键指标
2. 规范日志格式
3. 增加告警通知渠道
4. 添加阈值动态调整机制
```

### JSON 输出
```json
{
  "system_name": "trading_platform",
  "timestamp": "2026-04-09T10:30:00Z",
  "monitoring_platform": "Prometheus + Grafana",
  "review_results": {
    "metrics": {
      "status": "warning",
      "checks": [
        {"name": "coverage", "status": "pass"},
        {"name": "business_metrics", "status": "warning"},
        {"name": "collection_frequency", "status": "pass"},
        {"name": "accuracy", "status": "warning"}
      ]
    },
    "alerts": {
      "status": "warning",
      "checks": [
        {"name": "threshold_settings", "status": "pass"},
        {"name": "priority_leveling", "status": "pass"},
        {"name": "notification_channels", "status": "warning"},
        {"name": "accuracy", "status": "pass"}
      ]
    },
    "logs": {
      "status": "warning",
      "checks": [
        {"name": "collection_scope", "status": "pass"},
        {"name": "format_standardization", "status": "warning"},
        {"name": "storage_policy", "status": "pass"},
        {"name": "retrievability", "status": "pass"}
      ]
    },
    "thresholds": {
      "status": "warning",
      "checks": [
        {"name": "business_alignment", "status": "pass"},
        {"name": "dynamic_adjustment", "status": "warning"},
        {"name": "capacity_relationship", "status": "pass"},
        {"name": "predictive_capability", "status": "pass"}
      ]
    },
    "dashboards": {
      "status": "warning",
      "checks": [
        {"name": "key_metrics_emphasis", "status": "pass"},
        {"name": "visualization_effectiveness", "status": "warning"},
        {"name": "actionability", "status": "pass"},
        {"name": "diagnostic_speed", "status": "pass"}
      ]
    },
    "auditing": {
      "status": "pass",
      "checks": [
        {"name": "system_changes", "status": "pass"},
        {"name": "security_events", "status": "pass"},
        {"name": "user_operations", "status": "pass"},
        {"name": "compliance", "status": "pass"}
      ]
    }
  },
  "recommendations": [
    "补充缺失的业务关键指标",
    "规范日志格式",
    "增加告警通知渠道",
    "添加阈值动态调整机制"
  ]
}
```

## 🔍 审查规则

### 监控指标规则
- ✅ 关键业务指标必须监控
- ✅ 系统组件关键指标必须覆盖
- ✅ 指标采集频率应合理
- ✅ 指标应支持历史趋势分析
- ✅ 指标应可关联到业务影响

### 告警规则规则
- ✅ 告警阈值应基于历史数据
- ✅ 应设置合理的告警优先级
- ✅ 告警应提供明确的处理建议
- ✅ 告警应支持静默和自动解除
- ✅ 应避免告警风暴

### 日志收集规则
- ✅ 日志应覆盖所有系统组件
- ✅ 日志格式应标准化
- ✅ 日志应包含足够上下文信息
- ✅ 日志保留周期应符合合规要求
- ✅ 应支持实时日志分析

### 性能阈值规则
- ✅ 阈值应基于业务需求
- ✅ 阈值应考虑系统季节性变化
- ✅ 阈值应支持动态调整
- ✅ 阈值应与容量规划关联
- ✅ 应设置预警阈值和严重阈值

### 仪表板规则
- ✅ 仪表板应突出关键指标
- ✅ 数据可视化应清晰直观
- ✅ 仪表板应支持快速问题诊断
- ✅ 仪表板应避免信息过载
- ✅ 应支持不同角色视图

### 审计追踪规则
- ✅ 系统变更必须记录
- ✅ 安全事件必须追踪
- ✅ 用户操作必须审计
- ✅ 审计日志必须不可篡改
- ✅ 审计数据必须满足合规要求

## 🧪 示例用法

### 验证生产环境监控配置
```bash
claude /system-monitoring production/ --scope production --dimension all
```

### 审查告警规则
```bash
claude /system-monitoring alerts/ --dimension alerts --output json
```

### 检查日志收集配置
```bash
claude /system-monitoring logs/ --dimension logs --validate-completeness
```

### 验证性能阈值设置
```bash
claude /system-monitoring thresholds/ --dimension thresholds --validate-settings
```

## 📚 注意事项

1. **监控环境**：需要访问监控系统API和配置
2. **权限要求**：需要查看监控和日志数据的权限
3. **依赖项**：需要连接到监控系统（Prometheus、Grafana等）
4. **性能考虑**：大规模数据查询可能需要较长时间
5. **合规要求**：确保审查过程符合金融行业安全规范

## 🚀 适用场景

### 1. 监控系统搭建
- 评估监控系统设计
- 验证指标覆盖范围
- 检查告警规则设置
- 审查仪表板设计

### 2. 监控系统优化
- 识别监控盲点
- 优化告警准确性
- 提升仪表板实用性
- 调整性能阈值

### 3. 合规性审计
- 检查审计日志完整性
- 验证监控数据留存
- 审查安全事件追踪
- 确保符合监管要求

### 4. 事故预防
- 识别潜在监控缺陷
- 优化阈值设置
- 提前预警系统问题
- 增强系统可靠性

## 📈 与金融开发技能组的协同

此技能与以下金融开发技能配合使用效果最佳：

- `/code-review` - 通用代码审查
- `/finance-optim` - 金融性能优化
- `/concurrency-review` - 高频并发审查
- `/compliance-review` - 合规性审查
- `/market-data-validate` - 市场数据质量验证
- `/trading-simulation` - 交易模拟器审查

通过组合使用这些技能，可以建立完整的金融系统监控和运维体系。