# C++后端专项审查使用示例

## 示例1: 基本C++后端代码审查

```bash
# 审查C++代码
claude /cpp-backend-review src/

# 输出详细报告
claude /cpp-backend-review src/ --output markdown
```

## 示例2: 指定审查维度

```bash
# 只验证内存管理
claude /cpp-backend-review src/ --dimension memory

# 审查线程安全
claude /cpp-backend-review src/ --dimension thread

# 审查模板与泛型编程
claude /cpp-backend-review src/ --dimension templates

# 验证移动语义
claude /cpp-backend-review src/ --dimension move-semantics

# 验证所有维度
claude /cpp-backend-review src/ --validate-all
```

## 示例3: 指定C++标准

```bash
# 指定C++17标准
claude /cpp-backend-review src/ --standard c++17

# 指定C++20标准
claude /cpp-backend-review src/ --standard c++20

# 指定C++14标准并验证
claude /cpp-backend-review src/ --standard c++14
```

## 示例4: 指定审查范围

```bash
# 审查特定模块
claude /cpp-backend-review src/ --component trade-engine,market-data,api

# 审查交易引擎模块
claude /cpp-backend-review src/trade-engine/ --component engine

# 审查内存管理模块
claude /cpp-backend-review src/ --component memory-manager
```

## 示例5: JSON格式输出

```bash
# 输出JSON格式便于程序处理
claude /cpp-backend-review src/ --output json

# 将结果保存到文件
claude /cpp-backend-review src/ --output json > cpp_backend_report.json
```

## 示例6: 详细模式

```bash
# 显示详细验证过程
claude /cpp-backend-review src/ --verbose

# 显示所有警告和错误
claude /cpp-backend-review src/ --strict
```

## 示例7: CI/CD集成

```yaml
# GitHub Actions 示例
name: C++ Backend Review

on: [push, pull_request]

jobs:
  cpp-backend-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
  
    - name: Setup C++ Build Environment
      run: |
        # 配置C++开发环境
        echo "Setting up C++ development environment..."
  
    - name: Review C++ Code
      run: |
        claude /cpp-backend-review src/ --output json > review.json
  
    - name: Check Critical Issues
      run: |
        # 检查是否有严重问题
        if jq '.review_results | any(.status == "fail")' review.json; then
          echo "❌ C++代码存在严重问题"
          exit 1
        else
          echo "✅ C++代码通过审查"
        fi
```

## 示例8: 与现有技能组合使用

```bash
# 首先进行通用代码审查
/review src/

# 然后进行C++后端专项审查
/cpp-backend-review src/

# 再进行金融性能优化检查
/finance-optim src/

# 最后验证合规性
/compliance-review src/
```

## 示例9: 批量验证

```bash
# 验证整个代码库
claude /cpp-backend-review src/ --recursive

# 验证特定模块
claude /cpp-backend-review src/ --component trade-engine

# 验证特定文件类型
claude /cpp-backend-review src/ --file-pattern "*.cpp"
```

## 示例10: 与金融开发技能结合

```bash
# 先审查C++代码质量
/cpp-backend-review src/ --dimension all

# 再审查并发性能
/concurrency-review src/ --validate-thread-safety

# 验证高频交易性能
/finance-optim src/ --validate-latency

# 验证市场数据质量
/market-data-validate src/data/
```

## 示例11: 定期审查设置

```bash
# 创建每日C++代码审查任务
# 在 crontab 中添加
0 9 * * * cd /path/to/project && claude /cpp-backend-review src/ --output json > daily_cpp_$(date +%Y%m%d).json

# 创建每周全面审查
0 9 * * 1 cd /path/to/project && claude /cpp-backend-review src/ --validate-all --output json > weekly_cpp_$(date +%Y%m%d).json

# 创建每月审查
0 9 1 * * cd /path/to/project && claude /cpp-backend-review src/ --validate-all --output json > monthly_cpp_$(date +%Y%m%d).json
```

## 示例12: 金融特定场景审查

```bash
# 审查交易引擎C++代码
claude /cpp-backend-review src/trade-engine/ --dimension all

# 重点验证内存管理
claude /cpp-backend-review src/trade-engine/ --dimension memory --output json > memory_report.json

# 重点验证并发性能
claude /cpp-backend-review src/trade-engine/ --dimension concurrency --output json > concurrency_report.json

# 重点验证性能优化
claude /cpp-backend-review src/trade-engine/ --dimension performance --output json > performance_report.json
```

## 示例13: 模板元编程审查

```bash
# 验证模板元编程安全性
/cpp-backend-review src/ --dimension templates

# 检查SFINAE使用
/cpp-backend-review src/ --dimension templates --module sfinae

# 审查模板特化
/cpp-backend-review src/ --dimension templates --module specialization

# 验证类型推导
/cpp-backend-review src/ --dimension templates --module type-deduction
```

这些示例展示了如何在不同的场景下使用C++后端专项审查技能，从基础验证到复杂的CI/CD集成，以及与其他金融开发技能的协同使用。
