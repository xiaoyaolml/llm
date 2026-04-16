# 机器学习模型验证使用示例

## 示例1: 基本模型验证

```bash
# 验证一个简单的信贷风险模型
claude /ml-model-validate credit_risk_model.pkl

# 输出详细报告
claude /ml-model-validate credit_risk_model.pkl --output markdown
```

## 示例2: 指定验证模块

```bash
# 只验证公平性检查
claude /ml-model-validate fraud_model.pkl --module fairness

# 验证输入输出和漂移检测
claude /ml-model-validate churn_model.pkl --module input-output,drift

# 验证所有模块
claude /ml-model-validate risk_model.pkl --validate-all
```

## 示例3: 指定测试数据

```bash
# 使用指定的测试数据集
claude /ml-model-validate model.pkl --dataset test_data.csv

# 指定验证参数
claude /ml-model-validate model.pkl --dataset data.csv --threshold 0.05
```

## 示例4: JSON格式输出

```bash
# 输出JSON格式便于程序处理
claude /ml-model-validate model.pkl --output json

# 将结果保存到文件
claude /ml-model-validate model.pkl --output json > validation_report.json
```

## 示例5: 详细模式

```bash
# 显示详细验证过程
claude /ml-model-validate model.pkl --verbose

# 显示所有警告和错误
claude /ml-model-validate model.pkl --strict
```

## 示例6: CI/CD集成

```yaml
# GitHub Actions 示例
name: ML Model Validation

on: [push, pull_request]

jobs:
  validate-model:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.9'
    
    - name: Install dependencies
      run: |
        pip install scikit-learn pandas numpy
    
    - name: Validate ML Model
      run: |
        claude /ml-model-validate model.pkl --output json > model_validation.json
    
    - name: Check validation results
      run: |
        # 检查是否有严重问题
        if jq '.validation_results | any(.status == "fail")' model_validation.json; then
          echo "❌ 模型验证失败"
          exit 1
        else
          echo "✅ 模型验证通过"
        fi
```

## 示例7: 高级用法

```bash
# 验证多个模型
claude /ml-model-validate model1.pkl model2.pkl model3.pkl

# 验证特定类型的模型
claude /ml-model-validate model.pkl --model-type classification

# 设置自定义阈值
claude /ml-model-validate model.pkl --drift-threshold 0.03 --bias-threshold 0.05
```

## 示例8: 与现有技能组合使用

```bash
# 首先进行通用代码审查
/review src/model_training.py

# 然后进行金融性能优化检查
/finance-optim src/model_training.py

# 最后进行机器学习模型验证
/ml-model-validate trained_model.pkl --dataset validation_data.csv

# 验证合规性
/compliance-review src/model_training.py
```

## 示例9: 批量验证

```bash
# 验证整个模型目录
claude /ml-model-validate models/ --recursive

# 验证特定格式的模型
claude /ml-model-validate models/ --file-pattern "*.pkl"
```

## 示例10: 与量化回测结合

```bash
# 先验证量化策略
/quant-backtest strategies/

# 再验证模型
/ml-model-validate strategy_model.pkl

# 最后验证市场数据
/market-data-validate data/
```

这些示例展示了如何在不同的场景下使用机器学习模型验证技能，从基础验证到复杂的CI/CD集成，以及与其他金融开发技能的协同使用。