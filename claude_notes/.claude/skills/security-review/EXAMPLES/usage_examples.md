# 安全审查使用示例 (usage_examples.md)

## 基础使用场景

### 1. 全面安全审查
```bash
# 对项目进行完整安全审查
claude /security-review src/
```

**输出说明**:
- 生成包含所有安全维度的综合报告
- 标记高危、中危和低危问题
- 提供每个问题的修复建议
- 包含代码位置和安全风险描述

### 2. 指定审查维度
```bash
# 仅审查身份认证安全
claude /security-review src/ --dimension authentication

# 仅审查数据保护安全
claude /security-review src/ --dimension data-protection

# 同时审查多个维度
claude /security-review src/ --dimension authentication,data-protection
```

**最佳实践**:
- 在进行代码审查时，针对特定功能模块指定相关维度
- 开发认证模块时，聚焦`authentication`维度
- 处理用户数据时，聚焦`data-protection`维度

### 3. 指定合规标准
```bash
# 检查PCI-DSS合规性
claude /security-review src/ --standard pci-dss

# 检查GDPR合规性
claude /security-review src/ --standard gdpr

# 检查SOX合规性
claude /security-review src/ --standard sox
```

**使用场景**:
- 支付系统开发：`--standard pci-dss`
- 用户数据处理：`--standard gdpr`
- 财务报告系统：`--standard sox`

## 高级使用场景

### 1. 详细模式审查
```bash
# 启用详细模式输出
claude /security-review src/ --verbose

# 详细模式结合特定维度
claude /security-review src/ --dimension input-validation --verbose
```

**输出增强**:
- 显示详细的检查过程
- 包含代码片段和上下文
- 提供更深入的分析和建议
- 列出潜在的攻击向量

### 2. 指定输出格式
```bash
# 生成JSON格式报告
claude /security-review src/ --output json > security_report.json

# 生成Markdown格式报告
claude /security-review src/ --output markdown > security_report.md

# 指定严重级别
claude /security-review src/ --severity critical,high
```

**CI/CD集成示例**:
```yaml
- name: Security Review
  run: claude /security-review src/ --output json > security_report.json
- name: Check Critical Issues
  run: |
    if jq '.summary.critical > 0' security_report.json | grep -q true; then
      echo "❌ 发现严重安全问题"
      cat security_report.json
      exit 1
    fi
```

## 金融特定场景示例

### 1. 支付系统PCI-DSS审查
```bash
# 审查支付系统PCI-DSS合规性
claude /security-review src/payments/ --standard pci-dss --verbose
```

**重点检查**:
- 持卡人数据加密存储
- 支付接口安全认证
- 交易记录审计追踪
- 访问控制与权限分离
- 网络隔离与防火墙配置

### 2. 身份认证安全验证
```bash
# 检查JWT令牌实现安全性
claude /security-review src/auth/ --dimension authentication --target jwt --verbose
```

**示例问题**:
```markdown
### 🔴 严重 - JWT签名算法可被绕过
- **位置**: AuthService::validateToken (Line 45)
- **风险**: 攻击者可以使用`none`算法绕过签名验证
- **修复建议**: 强制指定算法类型，验证令牌头中的alg字段
- **代码示例**:

// ❌ 不安全代码
jwt::verify(token);

// ✅ 安全代码
auto verify = jwt::verify().allow_algorithm(jwt::algorithm::rs256(public_key));
verify.verify(token);
```

### 3. 用户数据保护检查
```bash
# 验证用户密码存储和数据保护
claude /security-review src/users/ --dimension data-protection --target password-storage
```

**关键检查项**:
- 密码是否使用PBKDF2/bcrypt/Argon2哈希
- 是否使用强盐值
- 盐值是否独立存储
- 是否实施密钥管理策略
- 敏感数据是否加密传输和存储

## 预提交钩子使用

### 创建安全审查预提交钩子
```bash
#!/bin/sh
# .git/hooks/pre-commit

# 仅检查高危安全问题
claude /security-review --high-priority-only || exit 1
```

**效果**:
- 在代码提交前自动执行安全检查
- 阻止包含高危问题的代码提交
- 快速反馈安全问题
- 确保代码库中不存在已知严重漏洞

## 定期安全审查

### 设置每日安全检查
```bash
# 将以下命令添加到crontab
0 2 * * * claude /security-review src/ --high-priority-only > /var/log/security_daily.log
```

**使用场景**:
- 定期扫描已存在代码中的安全漏洞
- 在夜间执行全面扫描，不影响开发流程
- 跟踪安全问题的修复进度
- 生成安全状况报告供审计使用

## 案例学习

### 案例 1: 修复SQL注入漏洞
**问题**: 在订单查询中直接拼接用户输入

```cpp
// ❌ 不安全的代码
std::string query = "SELECT * FROM orders WHERE user_id = " + userId;

// ✅ 安全的代码
std::string query = "SELECT * FROM orders WHERE user_id = ?";
std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(query));
stmt->setInt(1, std::stoi(userId));
```

**修复效果**:
- 完全消除SQL注入风险
- 提高查询执行效率
- 增强代码可读性和可维护性

### 案例 2: 修复弱密码策略
**问题**: 仅要求6位密码，无复杂度要求

```cpp
// ❌ 不安全的密码验证
bool isValid = password.length() >= 6;

// ✅ 安全的密码验证
bool isStrong = password.length() >= 8 &&
               std::any_of(password.begin(), password.end(), ::isdigit) &&
               std::any_of(password.begin(), password.end(), ::isupper) &&
               std::any_of(password.begin(), password.end(), ::islower) &&
               password.find_first_of("!@#$%^&*") != std::string::npos;
```

**修复效果**:
- 符合NIST密码指南要求
- 防止暴力破解攻击
- 提高账户安全性