# 金融API接口设计规范审查技能 (api-design-review)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/api-design-review/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行API设计审查
claude /api-design-review src/api/

# 指定审查维度
claude /api-design-review src/api/ --dimension versioning

# 输出JSON格式报告
claude /api-design-review src/api/ --output json

# 详细模式
claude /api-design-review src/api/ --verbose
```

## ✨ 核心功能

- **RESTful设计**：验证资源命名、HTTP方法使用
- **版本管理**：检查API版本控制策略
- **错误处理**：评估错误码规范、错误信息标准化
- **安全性**：审查认证授权机制、敏感信息保护
- **兼容性**：验证向后兼容性设计
- **文档完整性**：评估OpenAPI/Swagger文档质量
- **限流策略**：检查速率限制机制

## 📚 使用示例

### 1. RESTful规范审查
```bash
# 检查RESTful API设计合规性
claude /api-design-review src/rest/ --dimension restful-standards --verbose
```

### 2. 版本管理验证
```bash
# 评估API版本控制实施
claude /api-design-review src/version/ --dimension versioning
```

### 3. 金融交易API安全审查
```bash
# 验证订单API安全性
claude /api-design-review src/orders/ --dimension security --target trading
```

## 🔌 集成指南

### CI/CD 集成
```yaml
- name: API Design Review
  run: claude /api-design-review src/ --output json > api_report.json
```

### 预提交钩子
```bash
#!/bin/sh
claude /api-design-review --high-priority-only || exit 1
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含金融API设计核心审查功能