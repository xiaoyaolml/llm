# API设计审查使用示例 (usage_examples.md)

## 基础使用场景

### 1. 全面API设计审查
```bash
# 对项目进行完整API设计审查
claude /api-design-review src/api/
```

**输出说明**:
- 生成包含所有API设计维度的综合报告
- 标记设计问题并提供改进建议
- 包含API规范示例和最佳实践
- 评估API的可用性和一致性

### 2. 指定审查维度
```bash
# 仅审查API版本管理
claude /api-design-review src/api/ --dimension versioning

# 仅审查RESTful设计
claude /api-design-review src/api/ --dimension restful-design

# 审查多个维度
claude /api-design-review src/api/ --dimension versioning,error-handling
```

**使用场景**:
- 开发新API时进行针对性审查
- 重构现有API时聚焦特定问题
- 团队代码评审时定向检查

### 3. 详细模式审查
```bash
# 启用详细模式输出
claude /api-design-review src/api/ --verbose

# 详细模式结合特定维度
claude /api-design-review src/api/ --dimension restful-design --verbose
```

**输出增强**:
- 显示详细的分析过程
- 包含代码示例和修改建议
- 提供API规范文档链接
- 列出不符合RESTful原则的具体问题

## 金融特定场景示例

### 1. 交易API设计审查
```bash
# 审查交易API设计
claude /api-design-review src/api/trading/ --verbose
```

**重点检查**:
- 资源命名是否清晰（/orders, /trades）
- HTTP方法使用是否正确（POST创建, GET查询）
- 错误响应是否包含足够的上下文
- 是否支持批量操作
- 版本控制策略是否合理

**示例问题**:
```markdown
### ⚠️ 资源命名不一致
- **位置**: OrderController
- **问题**: 使用了动词前缀 `/createOrder`
- **建议**: 使用RESTful风格 `/orders` (POST)
```

### 2. 支付API安全审查
```bash
# 审查支付API安全性
claude /api-design-review src/api/payments/ --dimension security
```

**关键检查项**:
- 认证机制（OAuth 2.0, JWT）
- 敏感数据保护（加密传输）
- 访问控制（权限验证）
- 审计日志记录
- 限流和防滥用

### 3. 用户账户API验证
```bash
# 审查账户API设计
claude /api-design-review src/api/accounts/ --dimension error-handling
```

**验证要点**:
- 错误码设计是否一致
- 错误信息是否包含足够上下文
- 是否提供开发者友好的错误描述
- 是否支持多语言错误消息

## API设计最佳实践示例

### 1. RESTful资源设计
```cpp
// ❌ 不符合RESTful的设计
GET /api/getOrdersByUser?id=123
POST /api/order/create

// ✅ 符合RESTful的设计
GET /api/users/123/orders
POST /api/orders
```

### 2. 版本控制策略
```cpp
// ✅ URL版本控制
GET /api/v1/users
GET /api/v2/users

// ✅ Header版本控制
GET /api/users
Accept: application/vnd.company.api.v1+json
```

### 3. 错误响应设计
```json
{
  "error": {
    "code": "INVALID_ORDER_AMOUNT",
    "message": "订单金额必须大于0",
    "details": {
      "field": "amount",
      "minimum": 0.01,
      "provided": -100
    },
    "timestamp": "2026-04-10T10:30:00Z",
    "request_id": "req_abc123"
  }
}
```

## CI/CD集成示例

### GitHub Actions集成
```yaml
- name: API Design Review
  run: claude /api-design-review src/api/ --output json > api_report.json
- name: Check Critical Issues
  run: |
    if jq '.issues.critical > 0' api_report.json | grep -q true; then
      echo "❌ 发现API设计严重问题"
      cat api_report.json
      exit 1
    fi
```

### 预提交钩子
```bash
#!/bin/sh
# .git/hooks/pre-commit
claude /api-design-review --high-priority-only || exit 1
```

## 定期审查

### 设置每日API设计检查
```bash
# 将以下命令添加到crontab
0 3 * * * claude /api-design-review src/api/ --high-priority-only > /var/log/api_design_daily.log
```

**使用场景**:
- 定期扫描API设计问题
- 跟踪API规范的演进
- 确保团队遵循API设计指南
- 生成API设计质量报告