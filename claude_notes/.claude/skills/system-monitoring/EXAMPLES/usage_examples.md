# 系统监控审查使用示例

## 示例1: 基本监控配置审查

```bash
# 审查监控配置文件
claude /system-monitoring monitoring-config/ --dimension metrics

# 输出详细报告
claude /system-monitoring monitoring-config/ --output markdown
```

## 示例2: 指定审查维度

```bash
# 只验证告警规则
claude /system-monitoring monitoring-config/ --dimension alerts

# 审查日志收集
claude /system-monitoring monitoring-config/ --dimension logs

# 验证性能阈值
claude /system-monitoring monitoring-config/ --dimension thresholds

# 验证所有维度
claude /system-monitoring monitoring-config/ --validate-all
```

## 示例3: 指定审查范围

```bash
# 审查生产环境配置
claude /system-monitoring production/ --scope production

# 审查测试环境配置
claude /system-monitoring test/ --scope testing

# 审查特定组件
claude /system-monitoring monitoring-config/ --component database,api,gateway
```

## 示例4: JSON格式输出

```bash
# 输出JSON格式便于程序处理
claude /system-monitoring monitoring-config/ --output json

# 将结果保存到文件
claude /system-monitoring monitoring-config/ --output json > monitoring_report.json
```

## 示例5: 详细模式

```bash
# 显示详细验证过程
claude /system-monitoring monitoring-config/ --verbose

# 显示所有警告和错误
claude /system-monitoring monitoring-config/ --strict
```

## 示例6: CI/CD集成

```yaml
# GitHub Actions 示例
name: System Monitoring Review

on: [push, pull_request]

jobs:
  monitoring-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup Monitoring Tools
      run: |
        # 安装必要的监控工具
        echo "Installing monitoring tools..."
    
    - name: Review Monitoring Configuration
      run: |
        claude /system-monitoring monitoring-config/ --output json > review.json
    
    - name: Check for Critical Issues
      run: |
        # 检查是否有严重问题
        if jq '.review_results | any(.status == "fail")' review.json; then
          echo "❌ 监控配置存在严重问题"
          exit 1
        else
          echo "✅ 监控配置通过审查"
        fi
```

## 示例7: 高级用法

```bash
# 审查多个监控配置目录
claude /system-monitoring monitoring-config/ logs/ alerts/ dashboards/ --recursive

# 审查特定文件类型
claude /system-monitoring monitoring-config/ --file-pattern "*.yaml"

# 设置自定义阈值
claude /system-monitoring monitoring-config/ --latency-threshold 100 --error-rate-threshold 0.01
```

## 示例8: 与现有技能组合使用

```bash
# 首先进行通用代码审查
/review src/

# 然后进行金融性能优化检查
/finance-optim src/

# 最后进行系统监控审查
/system-monitoring monitoring-config/ --dimension all

# 验证合规性
/compliance-review monitoring-config/
```

## 示例9: 批量验证

```bash
# 验证整个监控体系
claude /system-monitoring all-configs/ --recursive

# 验证特定环境
claude /system-monitoring environments/ --scope dev,staging,prod

# 验证特定组件
claude /system-monitoring trading-system/ --component engine,api,db,cache
```

## 示例10: 与金融开发技能结合

```bash
# 先审查金融系统性能
/system-monitoring trading-system/ --dimension thresholds --validate-latency

# 再审查并发性能
/concurrency-review trading-system/ --validate-thread-safety

# 最后审查风险计算
/risk-check trading-system/src/risk/

# 验证市场数据质量
/market-data-validate trading-system/src/data/
```

## 示例11: 定期审查设置

```bash
# 创建每日监控配置任务
# 在 crontab 中添加
0 9 * * * cd /path/to/project && claude /system-monitoring monitoring-config/ --output json > daily_monitoring_$(date +%Y%m%d).json

# 创建每周完整性检查
0 9 * * 1 cd /path/to/project && claude /system-monitoring monitoring-config/ --output json > weekly_monitoring_$(date +%Y%m%d).json

# 创建每月全面检查
0 9 1 * * cd /path/to/project && claude /system-monitoring monitoring-config/ --validate-all --output json > monthly_monitoring_$(date +%Y%m%d).json
```

## 示例12: 生产环境审查

```bash
# 审查生产环境监控
claude /system-monitoring production/ --scope production --validate-completeness

# 重点关注告警规则
claude /system-monitoring production/ --dimension alerts --output json > alerts_report.json

# 重点关注日志收集
claude /system-monitoring production/ --dimension logs --output json > logs_report.json
```

## 示例13: 合规性审计

```bash
# 验证合规性监控
/system-monitoring compliance/ --dimension auditing --validate-compliance

# 检查审计追踪
/system-monitoring compliance/ --module auditing --validate-logs

# 验证数据保留策略
/system-monitoring compliance/ --module logs --validate-retention

# 检查安全事件监控
/system-monitoring compliance/ --dimension alerts --validate-security
```

这些示例展示了如何在不同的场景下使用系统监控审查技能，从基础验证到复杂的CI/CD集成，以及与其他金融开发技能的协同使用。