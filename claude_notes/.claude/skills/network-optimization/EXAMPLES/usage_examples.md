# 网络性能优化使用示例

## 示例1: 基本网络性能优化审查

```bash
# 基本使用
claude /network-optimization network-config/

# 输出详细报告
claude /network-optimization network-config/ --output markdown
```

## 示例2: 指定审查维度

```bash
# 只验证网络延迟
claude /network-optimization network-config/ --dimension latency

# 审查带宽使用
claude /network-optimization network-config/ --dimension bandwidth

# 验证连接稳定性
claude /network-optimization network-config/ --dimension connection-stability

# 验证数据传输效率
claude /network-optimization network-config/ --dimension data-efficiency
```

## 示例3: 指定审查范围

```bash
# 审查客户端组件
claude /network-optimization network-config/ --component client

# 审查服务器组件
claude /network-optimization network-config/ --component server

# 审查网关组件
claude /network-optimization network-config/ --component gateway

# 验证特定服务
claude /network-optimization network-config/ --service trade-engine
```

## 示例4: JSON格式输出

```bash
# 输出JSON格式便于程序处理
claude /network-optimization network-config/ --output json

# 将结果保存到文件
claude /network-optimization network-config/ --output json > network_optimization_report.json
```

## 示例5: 详细模式

```bash
# 显示详细验证过程
claude /network-optimization network-config/ --verbose

# 显示所有警告和错误
claude /network-optimization network-config/ --strict
```

## 示例6: CI/CD集成

```yaml
# GitHub Actions 示例
name: Network Optimization Review

on: [push, pull_request]

jobs:
  network-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup Network Tools
      run: |
        # 配置网络性能分析工具
        echo "Setting up network optimization tools..."
    
    - name: Review Network Configuration
      run: |
        claude /network-optimization network-config/ --output json > review.json
    
    - name: Check Critical Issues
      run: |
        # 检查是否有严重问题
        if jq '.review_results | any(.status == "fail")' review.json; then
          echo "❌ 网络性能存在严重问题"
          exit 1
        else
          echo "✅ 网络性能审查通过"
        fi
```

## 示例7: 高级用法

```bash
# 验证整个网络系统
claude /network-optimization network-config/ --validate-all

# 指定特定性能指标
claude /network-optimization network-config/ --metric latency --threshold 100

# 验证特定组件的性能
claude /network-optimization network-config/ --component api-gateway --verify-latency
```

## 示例8: 与现有技能组合使用

```bash
# 首先进行通用代码审查
/review src/

# 然后进行网络性能优化审查
/network-optimization network-config/ --dimension all

# 再进行性能优化
/finance-optim src/

# 验证合规性
/compliance-review src/
```

## 示例9: 批量验证

```bash
# 验证整个网络系统
claude /network-optimization network-config/ --recursive

# 验证特定网络组件
claude /network-optimization network-config/ --component load-balancer

# 验证特定网络类型
claude /network-optimization network-config/ --network-type high-frequency
```

## 示例10: 定期审查设置

```bash
# 创建每日网络审查任务
# 在 crontab 中添加
0 9 * * * cd /path/to/project && claude /network-optimization network-config/ --output json > daily_network_$(date +%Y%m%d).json

# 创建每周全面审查
0 9 * * 1 cd /path/to/project && claude /network-optimization network-config/ --validate-all --output json > weekly_network_$(date +%Y%m%d).json

# 创建每月详细审查
0 9 1 * * cd /path/to/project && claude /network-optimization network-config/ --validate-all --output json > monthly_network_$(date +%Y%m%d).json
```

## 示例11: 金融特定场景审查

```bash
# 高频交易系统网络审查
claude /network-optimization network-config/ --service trade-engine --dimension latency

# 支付系统网络审查
claude /network-optimization network-config/ --service payment-system --dimension bandwidth

# 挂牌系统网络审查
claude /network-optimization network-config/ --service listing-system --dimension connection-stability

# 账户系统网络审查
claude /network-optimization network-config/ --service account-system --dimension data-efficiency
```

## 示例12: 与金融开发技能结合

```bash
# 与金融性能优化协同
/finance-optim src/
/network-optimization network-config/ --dimension all

# 与并发审查结合
/concurrency-review src/
/network-optimization network-config/ --dimension all

# 与市场数据验证结合
/market-data-validate src/
/network-optimization network-config/ --dimension all
```

这些示例展示了如何在不同的场景下使用网络性能优化技能，从基础验证到复杂的CI/CD集成，以及与其他金融开发技能的协同使用。