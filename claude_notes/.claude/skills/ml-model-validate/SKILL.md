---
name: ml-model-validate
description: 机器学习模型验证技能 - 验证金融AI模型的输入输出、特征工程、模型漂移、公平性等
---

# 🤖 机器学习模型验证技能 (ml-model-validate)

## 🎯 功能概述

此技能专门用于金融领域机器学习模型的全面验证，包括模型输入输出验证、特征工程检查、模型漂移检测、公平性和偏见检查等功能，确保金融AI系统的可靠性和合规性。

## 🔧 支持的模型类型

- 分类模型 (Classification)
- 回归模型 (Regression)
- 时间序列预测模型 (Time Series Forecasting)
- 异常检测模型 (Anomaly Detection)
- 多任务学习模型 (Multi-task Learning)

## 📋 主要验证功能

### 1. 输入输出验证
- 检查输入数据格式和类型
- 验证输出值范围和合理性
- 确保特征维度匹配
- 检查缺失值和异常值处理

### 2. 特征工程检查
- 验证特征构建逻辑
- 检查特征重要性合理性
- 确保特征选择符合业务逻辑
- 检查特征缩放和标准化

### 3. 模型漂移检测
- 检测训练数据和生产数据分布差异
- 监控特征分布变化
- 检测目标变量分布漂移
- 识别潜在的数据质量问题

### 4. 公平性和偏见检查
- 检查模型对不同群体的公平性
- 检测潜在的偏见来源
- 验证性别、种族、年龄等敏感属性的影响
- 确保模型决策的一致性

### 5. 性能验证
- 检查模型准确率和召回率
- 验证F1分数和AUC指标
- 检查模型在不同数据集上的稳定性
- 确保模型收敛性

## 🛠 使用方法

```bash
# 基本使用
claude /ml-model-validate model.pkl

# 指定验证范围
claude /ml-model-validate model.pkl --validate-input-output

# 指定数据集进行验证
claude /ml-model-validate model.pkl --dataset data.csv

# 输出JSON格式报告
claude /ml-model-validate model.pkl --output json

# 指定特定验证模块
claude /ml-model-validate model.pkl --module fairness,drift

# 详细模式
claude /ml-model-validate model.pkl --verbose
```

## 📊 输出格式

### Markdown 输出
```markdown
# 机器学习模型验证报告

## 概述
- 模型名称: credit_risk_model
- 模型类型: 二分类模型
- 验证时间: 2026-04-09 10:30:00

## 验证结果

### 输入输出验证
- ✅ 输入数据格式正确
- ✅ 输出值范围合理
- ❌ 检测到异常值
- ⚠️ 特征维度不匹配

### 特征工程
- ✅ 特征构建逻辑合理
- ⚠️ 特征重要性偏低
- ✅ 特征缩放正确

### 模型漂移
- ⚠️ 特征分布轻微变化
- ⚠️ 目标变量分布有变化

### 公平性检查
- ✅ 无明显群体偏见
- ✅ 敏感属性影响在合理范围内

## 建议
1. 修复异常值问题
2. 重新评估特征重要性
3. 关注特征分布变化趋势
```

### JSON 输出
```json
{
  "model_name": "credit_risk_model",
  "model_type": "binary_classification",
  "timestamp": "2026-04-09T10:30:00Z",
  "validation_results": {
    "input_output": {
      "status": "partial_failure",
      "checks": [
        {"name": "input_format", "status": "pass"},
        {"name": "output_range", "status": "pass"},
        {"name": "anomalies", "status": "fail"},
        {"name": "feature_dimensions", "status": "warning"}
      ]
    },
    "feature_engineering": {
      "status": "warning",
      "checks": [
        {"name": "feature_logic", "status": "pass"},
        {"name": "feature_importance", "status": "warning"},
        {"name": "scaling", "status": "pass"}
      ]
    },
    "drift_detection": {
      "status": "warning",
      "checks": [
        {"name": "feature_distribution", "status": "warning"},
        {"name": "target_distribution", "status": "warning"}
      ]
    },
    "fairness": {
      "status": "pass",
      "checks": [
        {"name": "group_bias", "status": "pass"},
        {"name": "sensitive_attributes", "status": "pass"}
      ]
    }
  },
  "recommendations": [
    "修复异常值问题",
    "重新评估特征重要性",
    "关注特征分布变化趋势"
  ]
}
```

## 🔍 验证规则

### 输入输出验证规则
- ✅ 输入必须是数值型数据
- ✅ 输出值应在合理范围内
- ✅ 特征维度必须与模型匹配
- ✅ 不允许空值或无穷大值

### 特征工程规则
- ✅ 特征构建必须符合业务逻辑
- ✅ 特征重要性应合理分配
- ✅ 特征缩放应保持一致性
- ✅ 不存在高度相关的特征

### 漂移检测规则
- ✅ 特征分布变化不超过5%
- ✅ 目标变量分布变化不超过3%
- ✅ 异常值比例不超过1%

### 公平性规则
- ✅ 不同群体的预测结果差异不超过5%
- ✅ 敏感属性的影响系数应在正常范围内
- ✅ 不存在明显的歧视性偏差

## 🧪 示例用法

### 验证一个信用评分模型
```bash
claude /ml-model-validate credit_scoring_model.pkl --dataset training_data.csv
```

### 验证交易欺诈检测模型
```bash
claude /ml-model-validate fraud_detection_model.pkl --validate-input-output --validate-drift
```

### 验证客户流失预测模型
```bash
claude /ml-model-validate customer_churn_model.pkl --module fairness --output json
```

## 📚 注意事项

1. **数据准备**：验证需要准备相应的测试数据集
2. **模型格式**：支持多种模型格式（pickle, joblib, ONNX等）
3. **依赖项**：需要安装相关Python包（scikit-learn, pandas, numpy等）
4. **性能考虑**：大模型验证可能需要较长时间
5. **合规要求**：确保验证过程符合金融监管要求

## 🚀 适用场景

### 1. 风险管理
- 信用评分模型验证
- 欠款风险模型验证
- 市场风险模型验证

### 2. 交易系统
- 交易信号模型验证
- 市场预测模型验证
- 异常交易检测模型验证

### 3. 合规审计
- AI模型合规性验证
- 公平性审查
- 偏见检测

### 4. 模型生命周期管理
- 模型部署前验证
- 模型更新后验证
- 模型退役前评估

## 📈 与金融开发技能组的协同

此技能与以下金融开发技能配合使用效果最佳：

- `/risk-check` - 风险计算验证
- `/compliance-review` - 合规性审查
- `/quant-backtest` - 量化策略回测验证
- `/market-data-validate` - 市场数据质量验证

通过组合使用这些技能，可以建立完整的金融AI模型开发和验证流程。