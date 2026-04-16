# 机器学习模型验证技能 (ml-model-validate)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/ml-model-validate/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行模型验证
claude /ml-model-validate src/models/

# 指定验证维度
claude /ml-model-validate src/models/ --dimension feature-engineering

# 输出JSON格式报告
claude /ml-model-validate src/models/ --output json

# 详细模式
claude /ml-model-validate src/models/ --verbose
```

## ✨ 核心功能

- **模型验证**：检查模型输入输出规范
- **特征工程**：验证特征选择和处理流程
- **模型漂移**：检测数据分布变化
- **公平性检查**：评估模型偏见风险
- **过拟合检测**：识别过度拟合风险
- **精度验证**：确认模型预测精度

## 📚 使用示例

### 1. 基础模型验证
```bash
# 检查所有模型
claude /ml-model-validate src/models/
```

### 2. 特征工程审查
```bash
# 详细审查特征工程实现
claude /ml-model-validate src/features/ --dimension feature-engineering --verbose
```

### 3. 模型漂移检测
```bash
# 监控生产环境模型漂移
claude /ml-model-validate production/models/ --dimension drift-detection --baseline historical_data
```

## 🔌 集成指南

### CI/CD 集成
```yaml
- name: Run Model Validation
  run: claude /ml-model-validate src/models/ --output json > model_report.json
```

### 预提交钩子
```bash
#!/bin/sh
claude /ml-model-validate --high-priority-only || exit 1
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含基础模型验证功能