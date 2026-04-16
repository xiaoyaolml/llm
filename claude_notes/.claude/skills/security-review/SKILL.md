---
name: security-review
description: 金融级安全标准代码审查技能 - 专注于金融系统的安全漏洞检测、安全编码规范、数据保护、访问控制等安全领域
---

# 🔒 安全审查技能 (security-review)

## 🎯 功能概述

此技能专注于互联网金融系统的安全审查，特别针对高频交易系统、支付系统、用户账户系统等关键金融业务，全面检测安全漏洞、验证安全编码规范、评估数据保护措施和访问控制机制，确保系统符合金融级安全标准。

## 🔧 支持的安全审查维度

- 输入验证安全 (Input Validation Security)
- 身份认证安全 (Authentication Security)
- 访问控制安全 (Access Control Security)
- 数据保护安全 (Data Protection Security)
- 加密安全 (Cryptography Security)
- 会话管理安全 (Session Management Security)
- 日志审计安全 (Logging & Auditing Security)
- 业务逻辑安全 (Business Logic Security)
- 配置安全 (Configuration Security)
- 依赖安全 (Dependency Security)

## 📋 主要审查功能

### 1. 输入验证安全
- 检查SQL注入漏洞
- 验证命令注入防护
- 审查XSS防护措施
- 检测路径遍历漏洞
- 验证文件上传安全
- 检查XML外部实体注入(XXE)

### 2. 身份认证安全
- 检查密码策略强度
- 验证多因素认证(MFA)实现
- 审查登录暴力破解防护
- 检测凭证泄露风险
- 验证OAuth/OpenID Connect实现
- 检查JWT安全使用

### 3. 访问控制安全
- 验证RBAC权限模型
- 检查水平越权漏洞
- 检测垂直越权漏洞
- 审查敏感操作权限控制
- 验证API访问控制
- 检查敏感数据访问限制

### 4. 数据保护安全
- 检查敏感数据加密存储
- 验证数据传输加密(TLS/SSL)
- 审查数据脱敏处理
- 检测敏感信息泄露
- 验证数据备份加密
- 检查数据销毁机制

### 5. 加密安全
- 验证加密算法选择
- 检查密钥管理安全
- 审查随机数生成安全
- 检测弱加密算法使用
- 验证哈希算法强度
- 检查加密模式安全

### 6. 会话管理安全
- 验证会话ID生成安全
- 检查会话超时设置
- 审查会话固定攻击防护
- 检测会话劫持风险
- 验证会话并发控制
- 检查安全登出机制

### 7. 日志审计安全
- 验证敏感操作审计
- 检查日志完整性保护
- 审查日志防篡改机制
- 检测日志注入风险
- 验证审计日志保留策略
- 检查异常行为监控

### 8. 业务逻辑安全
- 检查交易逻辑漏洞
- 验证金额计算精度
- 审查并发交易处理
- 检测重放攻击防护
- 验证幂等性设计
- 检查业务流程绕过风险

### 9. 配置安全
- 验证默认配置安全
- 检查敏感配置泄露
- 审查调试信息暴露
- 检测错误信息泄露
- 验证安全头部配置
- 检查CORS配置安全

### 10. 依赖安全
- 检查依赖组件漏洞
- 验证依赖版本更新
- 审查第三方库安全
- 检测已知CVE漏洞
- 验证依赖签名检查
- 检查供应链攻击风险

## 🛠 使用方法

```bash
# 基本使用
claude /security-review src/

# 指定审查维度
claude /security-review src/ --dimension authentication

# 指定安全标准
claude /security-review src/ --standard pci-dss

# 输出JSON格式报告
claude /security-review src/ --output json

# 详细模式
claude /security-review src/ --verbose

# 指定严重级别
claude /security-review src/ --severity critical,high
```

## 📊 输出格式

### Markdown 输出
```markdown
# 安全审查报告

## 概述
- 系统名称: trading-platform
- 审查时间: 2026-04-09 10:30:00
- 安全标准: PCI-DSS, OWASP Top 10
- 审查范围: 全部安全维度

## 安全风险统计
- 🔴 严重: 2
- 🟠 高危: 5
- 🟡 中危: 8
- 🟢 低危: 12

## 详细发现

### 输入验证安全 (🔴 严重)
- **漏洞**: SQL注入漏洞
- **位置**: OrderRepository::findOrders (Line 45)
- **风险**: 攻击者可能获取数据库完整访问权限
- **修复建议**: 使用参数化查询替代字符串拼接
- **代码示例**:
```cpp
// 不安全的代码
std::string query = "SELECT * FROM orders WHERE user_id = " + userId;

// 安全的代码
std::string query = "SELECT * FROM orders WHERE user_id = ?";
stmt->setInt(1, userId);
```

### 身份认证安全 (🟠 高危)
- **漏洞**: 弱密码策略
- **位置**: UserService::validatePassword
- **风险**: 容易被暴力破解
- **修复建议**: 实施强密码策略，要求8位以上，包含大小写字母、数字和特殊字符

### 数据保护安全 (🟠 高危)
- **漏洞**: 敏感数据明文存储
- **位置**: UserEntity::password
- **风险**: 数据库泄露导致凭证泄露
- **修复建议**: 使用PBKDF2或bcrypt进行密码哈希

## 合规性检查
- ✅ PCI-DSS: 10/12 要求满足
- ⚠️ GDPR: 数据保护措施需加强
- ❌ SOX: 审计日志不完整

## 修复优先级
1. **P0 - 立即修复**: SQL注入漏洞
2. **P1 - 本周修复**: 弱密码策略、明文存储
3. **P2 - 本月修复**: 会话管理、日志审计
```

### JSON 输出
```json
{
  "system": "trading-platform",
  "review_time": "2026-04-09 10:30:00",
  "security_standard": ["PCI-DSS", "OWASP Top 10"],
  "summary": {
    "critical": 2,
    "high": 5,
    "medium": 8,
    "low": 12
  },
  "findings": [
    {
      "category": "input_validation",
      "severity": "critical",
      "title": "SQL注入漏洞",
      "location": "OrderRepository::findOrders:45",
      "description": "用户输入直接拼接到SQL查询中",
      "risk": "数据库完全泄露",
      "recommendation": "使用参数化查询",
      "cwe": "CWE-89",
      "owasp": "A03:2021 - Injection"
    }
  ],
  "compliance": {
    "pci_dss": {
      "status": "partial",
      "score": "10/12"
    },
    "gdpr": {
      "status": "warning",
      "issues": ["数据加密不足"]
    }
  }
}
```

## 🎯 互联网金融特色安全审查

### 1. 交易系统安全审查
- 交易数据完整性验证
- 防重放攻击机制
- 交易金额精度保护
- 并发交易安全处理
- 异常交易检测

### 2. 支付系统安全审查
- 支付凭证保护
- 支付接口安全
- 防篡改机制
- 重复支付防护
- 敏感信息脱敏

### 3. 用户账户安全审查
- 账户接管防护
- 密码安全策略
- 登录异常检测
- 账户锁定机制
- 敏感操作二次验证

### 4. 风控系统安全审查
- 规则引擎安全
- 数据访问控制
- 模型安全保护
- 决策日志审计
- 反欺诈逻辑完整性

## 🔍 常见安全漏洞检测

### SQL注入检测
```cpp
// 漏洞代码
std::string query = "SELECT * FROM users WHERE id = '" + userId + "'";

// 安全代码
std::string query = "SELECT * FROM users WHERE id = ?";
PreparedStatement stmt = conn.prepareStatement(query);
stmt.setString(1, userId);
```

### XSS防护检测
```cpp
// 漏洞代码
response.write("<div>" + userInput + "</div>");

// 安全代码
response.write("<div>" + htmlEscape(userInput) + "</div>");
```

### 敏感数据泄露检测
```cpp
// 漏洞代码
LOG_INFO << "User password: " << password;

// 安全代码
LOG_INFO << "User login attempt for user: " << username;
```

### 不安全的反序列化检测
```cpp
// 漏洞代码
Object obj = deserialize(untrustedData);

// 安全代码
if (validateDataIntegrity(untrustedData)) {
    Object obj = deserialize(untrustedData);
}
```

## 📈 安全评估标准

### 严重 (Critical)
- 可导致系统完全控制的漏洞
- 可绕过所有安全机制的漏洞
- 可导致大量敏感数据泄露的漏洞
- 评分: 9.0 - 10.0

### 高危 (High)
- 可导致敏感数据泄露的漏洞
- 可提升权限的漏洞
- 可导致业务逻辑绕过的漏洞
- 评分: 7.0 - 8.9

### 中危 (Medium)
- 需要一定条件才能利用的漏洞
- 可导致部分信息泄露的漏洞
- 可导致有限权限提升的漏洞
- 评分: 4.0 - 6.9

### 低危 (Low)
- 影响有限的漏洞
- 需要复杂条件才能利用的漏洞
- 建议性的安全问题
- 评分: 0.1 - 3.9

## 🛡️ 安全编码规范检查

### C++安全编码
- 使用智能指针管理内存
- 避免缓冲区溢出
- 安全使用字符串操作
- 正确处理异常
- 避免使用不安全的函数

### 内存安全
```cpp
// 不安全
char buffer[100];
strcpy(buffer, userInput);

// 安全
std::string buffer(userInput);
```

### 整数溢出防护
```cpp
// 不安全
int result = a * b;

// 安全
if (a > INT_MAX / b) {
    throw std::overflow_error("Integer overflow");
}
int result = a * b;
```

## 🔐 合规性检查

### PCI-DSS 合规
- 要求1: 防火墙配置
- 要求2: 默认密码安全
- 要求3: 存储的持卡人数据保护
- 要求4: 传输中的持卡人数据加密
- 要求5: 防病毒软件
- 要求6: 安全系统和应用开发
- 要求7: 按需限制数据访问
- 要求8: 身份识别和认证
- 要求9: 物理访问限制
- 要求10: 网络资源访问监控
- 要求11: 定期安全测试
- 要求12: 信息安全政策

### GDPR 合规
- 数据最小化原则
- 数据加密存储
- 数据传输加密
- 用户数据访问权
- 数据删除权
- 数据可携带权
- 隐私保护设计
- 数据泄露通知

### 金融行业标准
- 《金融数据安全 数据安全分级指南》
- 《个人金融信息保护技术规范》
- 《金融行业网络安全等级保护实施指引》

## 🛠 安全工具集成

### 静态分析工具
- SonarQube
- Checkmarx
- Fortify
- Semgrep
- CodeQL

### 依赖扫描工具
- Snyk
- OWASP Dependency-Check
- WhiteSource
- GitHub Security Advisories

### 漏洞扫描工具
- Nessus
- OpenVAS
- Nikto
- OWASP ZAP

## 📋 安全审查流程

### 1. 准备阶段
```bash
# 收集代码和配置
claude /security-review src/ --prepare

# 收集依赖信息
claude /security-review src/ --dependencies
```

### 2. 静态分析
```bash
# 执行静态安全分析
claude /security-review src/ --static-analysis
```

### 3. 漏洞检测
```bash
# 检测已知漏洞
claude /security-review src/ --vulnerability-scan
```

### 4. 合规检查
```bash
# 检查PCI-DSS合规
claude /security-review src/ --standard pci-dss

# 检查GDPR合规
claude /security-review src/ --standard gdpr
```

### 5. 生成报告
```bash
# 生成综合安全报告
claude /security-review src/ --report comprehensive
```

## 🎓 最佳实践

### 安全开发流程
1. 安全需求分析
2. 威胁建模
3. 安全编码
4. 安全测试
5. 安全部署
6. 安全监控

### 安全编码原则
- 最小权限原则
- 纵深防御原则
- 失败安全原则
- 不信任输入原则
- 安全默认值原则

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/security-review/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行完整安全审查
claude /security-review src/

# 指定PCI-DSS合规检查
claude /security-review src/ --standard pci-dss

# 输出详细报告
claude /security-review src/ --verbose
```

## 🔮 未来扩展

### 计划功能
- 机器学习漏洞预测
- 实时安全监控集成
- 自动化安全修复建议
- 容器安全扫描
- 云安全态势评估
- 供应链安全分析

---

**技能版本**: v1.0.0 (2026-04-09)
**适用行业**: 互联网金融、银行、证券、保险、支付、区块链
**目标用户**: 安全工程师、开发团队、合规团队、审计人员
**支持语言**: C++, Java, Python, JavaScript, Go, Rust
**安全标准**: OWASP Top 10, PCI-DSS, GDPR, ISO 27001, SOX