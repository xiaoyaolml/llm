# 金融级安全标准代码审查技能 (security-review)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/security-review/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行安全审查
claude /security-review src/

# 指定审查维度
claude /security-review src/ --dimension authentication

# 指定安全标准
claude /security-review src/ --standard pci-dss

# 输出JSON格式报告
claude /security-review src/ --output json

# 详细模式
claude /security-review src/ --verbose
```

## ✨ 核心功能

- **输入验证安全**: 检测SQL注入、XSS、命令注入漏洞
- **身份认证安全**: 验证密码策略、MFA、JWT安全使用
- **访问控制安全**: 审查RBAC权限模型、越权漏洞
- **数据保护安全**: 检查加密存储、传输安全、数据脱敏
- **加密安全**: 验证算法选择、密钥管理安全
- **会话管理安全**: 检查会话超时、固定攻击防护
- **业务逻辑安全**: 检测交易逻辑漏洞、金额计算问题
- **合规性检查**: PCI-DSS、GDPR、SOX等标准合规验证

## 📚 使用示例

### 1. 支付系统安全审查
```bash
# 审查支付系统PCI-DSS合规性
claude /security-review src/payments/ --standard pci-dss --verbose
```

### 2. 身份认证安全验证
```bash
# 检查JWT令牌实现安全性
claude /security-review src/auth/ --dimension authentication --target jwt
```

### 3. 敏感数据保护检查
```bash
# 验证用户密码存储加密方式
claude /security-review src/users/ --dimension data-protection --target password-storage
```

## 🔌 集成指南

### CI/CD 集成
```yaml
- name: Security Review
  run: claude /security-review src/ --output json > security_report.json
```

### 预提交钩子
```bash
#!/bin/sh
claude /security-review --high-priority-only || exit 1
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含金融系统安全审查核心功能