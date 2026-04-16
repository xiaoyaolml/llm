# 云原生应用审查技能 (cloud-native-review)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/cloud-native-review/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行云原生应用审查
claude /cloud-native-review src/

# 指定审查维度
claude /cloud-native-review src/ --dimension containerization

# 输出JSON格式报告
claude /cloud-native-review src/ --output json

# 详细模式
claude /cloud-native-review src/ --verbose
```

## ✨ 核心功能

- **容器化**：验证Dockerfile最佳实践
- **微服务**：审查服务拆分合理性
- **自动扩缩容**：评估HPA配置优化
- **服务网格**：检查Istio/Linkerd集成
- **CI/CD流水线**：验证持续交付实践
- **配置管理**：审查ConfigMap/Secret使用
- **服务发现**：验证注册与发现机制
- **健康检查**：审查就绪/存活探针配置

## 📚 使用示例

### 1. 容器化检查
```bash
# 评估Docker配置优化
claude /cloud-native-review src/deploy/ --dimension containerization --verbose
```

### 2. 微服务架构审查
```bash
# 审查服务间依赖和边界
claude /cloud-native-review src/services/ --dimension microservices
```

### 3. 自动扩缩容评估
```bash
# 验证HPA配置合理性
claude /cloud-native-review src/deploy/hpa.yaml --dimension autoscaling
```

## 🔌 集成指南

### GitHub Actions 集成
```yaml
- name: Cloud Native Review
  run: claude /cloud-native-review src/ --output json > cloud_native_report.json
```

### 预提交钩子
```bash
#!/bin/sh
claude /cloud-native-review --high-priority-only || exit 1
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含云原生应用基础审查功能