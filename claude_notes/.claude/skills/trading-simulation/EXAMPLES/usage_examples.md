# 交易模拟器审查使用示例

## 示例1: 基本压力测试审查

```bash
# 审查压力测试配置
claude /trading-simulation stress-tests/ --test-type stress

# 输出详细报告
claude /trading-simulation stress-tests/ --output markdown
```

## 示例2: 指定测试类型

```bash
# 只验证性能基准
claude /trading-simulation benchmarks/ --test-type benchmark

# 验证故障恢复测试
claude /trading-simulation failover-tests/ --test-type failover

# 验证容错能力
claude /trading-simulation fault-tolerance-tests/ --test-type fault-tolerance

# 验证所有测试类型
claude /trading-simulation all-tests/ --validate-all
```

## 示例3: 指定性能指标

```bash
# 只验证关键性能指标
claude /trading-simulation stress-tests/ --benchmark response-time,throughput,cpu

# 验证资源使用情况
claude /trading-simulation stress-tests/ --benchmark memory,network,storage

# 验证交易处理能力
claude /trading-simulation stress-tests/ --benchmark tps,latency,success-rate
```

## 示例4: JSON格式输出

```bash
# 输出JSON格式便于程序处理
claude /trading-simulation stress-tests/ --output json

# 将结果保存到文件
claude /trading-simulation stress-tests/ --output json > stress_test_report.json
```

## 示例5: 详细模式

```bash
# 显示详细验证过程
claude /trading-simulation stress-tests/ --verbose

# 显示所有警告和错误
claude /trading-simulation stress-tests/ --strict
```

## 示例6: CI/CD集成

```yaml
# GitHub Actions 示例
name: Trading System Simulation

on: [push, pull_request]

jobs:
  trading-simulation:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup Testing Environment
      run: |
        # 配置测试环境
        echo "Setting up trading simulation environment..."
    
    - name: Run Stress Tests
      run: |
        claude /trading-simulation stress-tests/ --output json > stress_report.json
    
    - name: Run Performance Benchmarks
      run: |
        claude /trading-simulation benchmarks/ --output json > benchmark_report.json
    
    - name: Run Failover Tests
      run: |
        claude /trading-simulation failover-tests/ --output json > failover_report.json
    
    - name: Check Critical Issues
      run: |
        # 检查是否有严重问题
        if jq '.test_results | any(.status == "fail")' stress_report.json; then
          echo "❌ 压力测试存在严重问题"
          exit 1
        elif jq '.test_results | any(.status == "fail")' benchmark_report.json; then
          echo "❌ 性能基准测试存在严重问题"
          exit 1
        else
          echo "✅ 交易系统模拟测试通过"
        fi
```

## 示例7: 高级用法

```bash
# 审查多个测试目录
claude /trading-simulation tests/ stress/ benchmarks/ failover/ --recursive

# 审查特定文件类型
claude /trading-simulation tests/ --file-pattern "*.yaml"

# 设置自定义阈值
claude /trading-simulation stress-tests/ --latency-threshold 100 --throughput-threshold 10000
```

## 示例8: 与现有技能组合使用

```bash
# 首先进行通用代码审查
/review src/

# 然后进行金融性能优化检查
/finance-optim src/

# 最后进行交易模拟器审查
/trading-simulation simulation-config/ --test-type stress --validate-performance

# 验证合规性
/compliance-review src/
```

## 示例9: 批量验证

```bash
# 验证整个测试套件
claude /trading-simulation test-suite/ --recursive

# 验证特定测试环境
claude /trading-simulation environments/ --environment prod,staging,test

# 验证特定组件
claude /trading-simulation trading-system/ --component engine,api,db,cache
```

## 示例10: 与金融开发技能结合

```bash
# 先审查交易系统性能
/trading-simulation trading-system/ --test-type benchmark --validate-latency

# 再审查并发性能
/concurrency-review trading-system/ --validate-thread-safety

# 最后审查风险计算
/risk-check trading-system/src/risk/

# 验证市场数据质量
/market-data-validate trading-system/src/data/
```

## 示例11: 定期审查设置

```bash
# 创建每日压力测试任务
# 在 crontab 中添加
0 9 * * * cd /path/to/project && claude /trading-simulation stress-tests/ --output json > daily_stress_$(date +%Y%m%d).json

# 创建每周性能基准测试
0 9 * * 1 cd /path/to/project && claude /trading-simulation benchmarks/ --output json > weekly_benchmark_$(date +%Y%m%d).json

# 创建每月全面测试
0 9 1 * * cd /path/to/project && claude /trading-simulation all-tests/ --validate-all --output json > monthly_comprehensive_$(date +%Y%m%d).json
```

## 示例12: 生产环境审查

```bash
# 审查生产环境模拟
claude /trading-simulation prod-simulation/ --environment production --validate-stability

# 重点关注性能指标
claude /trading-simulation prod-simulation/ --module performance --output json > performance_report.json

# 重点关注故障恢复
claude /trading-simulation prod-simulation/ --module fault-tolerance --output json > fault_tolerance_report.json
```

## 示例13: 量化交易系统测试

```bash
# 验证量化策略模拟
/trading-simulation quant-strategies/ --test-type stress --validate-execution

# 检查回测结果
/quant-backtest quant-strategies/ --validate-simulation

# 验证交易执行效率
/trading-simulation quant-strategies/ --benchmark tps,latency,accuracy

# 检查策略稳定性
/trading-simulation quant-strategies/ --test-type fault-tolerance --validate-resilience
```

这些示例展示了如何在不同的场景下使用交易模拟器审查技能，从基础验证到复杂的CI/CD集成，以及与其他金融开发技能的协同使用。