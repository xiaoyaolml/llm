---
name: compliance-review
description: 金融合规性代码审查（审计追踪、数据完整性、访问控制）
---

# 金融合规性审查

## 触发条件

用户输入 `/compliance-review` 时执行，或当代码包含以下关键词时自动触发：
- `audit`、`log`、`compliance`、`sox`、`mifid`、`dodd-frank`、`gdpr`、`access control`、`encryption`

## 执行步骤

### 1. 识别合规要求

```
# 金融合规框架识别
1. SOX (Sarbanes-Oxley) - 财务报告内部控制
2. MiFID II (EU) - 市场透明度和投资者保护
3. Dodd-Frank (US) - 系统性风险监管
4. Basel III - 银行资本和流动性要求
5. GDPR - 数据保护和隐私
6. FRTB - 市场风险资本要求
```

### 2. 获取代码上下文

```bash
# 检测合规相关代码
git diff | grep -E "(audit|log|permission|encryption)"  # 识别合规代码
grep -r "class.*Audit\|struct.*Audit" . --include=*.cpp  # 识别审计类
grep -r "encrypt\|decrypt\|hash" . --include=*.cpp        # 识别加密操作
```

### 3. 合规性审查维度

#### 3.1 审计追踪 (Audit Trail)
- [ ] **完整记录**: 所有关键操作是否记录完整审计日志
- [ ] **不可篡改**: 审计日志是否防篡改（数字签名、只读存储）
- [ ] **时间戳**: 所有日志是否使用可信时间源（NTP 同步）
- [ ] **保留期限**: 是否符合法规要求的保留期限（通常 7 年）
- [ ] **查询能力**: 是否支持按时间、用户、操作类型查询

#### 3.2 数据完整性 (Data Integrity)
- [ ] **数据验证**: 输入数据是否验证格式和范围
- [ ] **校验和**: 关键数据传输是否使用校验和
- [ ] **备份恢复**: 是否有定期备份和恢复测试
- [ ] **数据血缘**: 是否追踪数据来源和转换过程
- [ ] **一致性**: 分布式系统中数据是否保持一致性

#### 3.3 访问控制 (Access Control)
- [ ] **权限验证**: 所有敏感操作是否验证用户权限
- [ ] **最小权限**: 是否遵循最小权限原则
- [ ] **权限分离**: 关键操作是否需要双重授权（4-eyes principle）
- [ ] **会话管理**: 会话超时和令牌管理是否安全
- [ ] **权限变更**: 权限变更是否需要审批和记录

#### 3.4 数据保护 (Data Protection)
- [ ] **敏感数据**: 个人身份信息（PII）是否加密存储
- [ ] **传输加密**: 敏感数据传输是否使用 TLS 1.2+
- [ ] **密钥管理**: 加密密钥是否安全存储和轮换
- [ ] **数据脱敏**: 非生产环境是否使用脱敏数据
- [ ] **数据销毁**: 数据删除是否不可恢复

#### 3.5 报告要求 (Reporting Requirements)
- [ ] **监管报告**: 是否生成符合监管要求的报告
- [ ] **数据准确性**: 报告数据是否准确和完整
- [ ] **时间要求**: 是否满足报告提交的时间要求
- [ ] **审计准备**: 是否准备好支持外部审计

### 4. 严重级别定义

- **高危**：违反监管要求，可能导致罚款或业务暂停
  - 审计日志缺失或可篡改
  - 敏感数据未加密
  - 未验证权限直接执行敏感操作
- **中危**：可能存在合规风险，需要改进
  - 日志保留期限不足
  - 权限管理不够精细
  - 未充分的数据验证
- **低危**：建议改进，但不影响合规性
  - 日志格式不规范
  - 文档不完整
  - 未优化的查询性能

### 5. 输出格式

```
## 金融合规性审查报告

**审查时间**: {{date_time}}
**合规框架**: {{compliance_framework}} (SOX/MiFID II/Dodd-Frank/GDPR)
**审查范围**: {{scope}} (交易系统/风险系统/报告系统)

### 问题清单

| # | 文件 | 行号 | 问题描述 | 合规要求 | 严重级别 | 修复建议 |
|---|------|------|----------|----------|----------|----------|
| 1 | audit/trade_logger.cpp | 45 | 审计日志未数字签名 | SOX 404 | 高 | 添加 HMAC 签名 |

### 详细分析

#### 1. 审计追踪问题
**问题**: 交易日志可被篡改
```cpp
// 当前代码
void log_trade(const Trade& trade) {
    std::ofstream log("trades.log", std::ios::app);
    log << trade.to_string() << std::endl;
}

// 修复建议
void log_trade(const Trade& trade) {
    std::string log_entry = trade.to_string();
    std::string signature = hmac_sha256(log_entry, secret_key);
    std::ofstream log("trades.log", std::ios::app);
    log << log_entry << "|" << signature << std::endl;
}
```

#### 2. 访问控制问题
**问题**: 权限变更无需审批
```cpp
// 当前代码
void grant_permission(User& user, Permission perm) {
    user.add_permission(perm); // 直接授予
}

// 修复建议
void grant_permission(User& user, Permission perm, Approver& approver) {
    if (!approver.approve(user.id, perm)) {
        throw std::runtime_error("Permission change requires approval");
    }
    user.add_permission(perm);
    audit_log("permission_granted", user.id, perm);
}
```

### 合规性评估

- **审计追踪**: {{audit_score}}/100 ({{audit_status}})
- **数据完整性**: {{integrity_score}}/100 ({{integrity_status}})
- **访问控制**: {{access_score}}/100 ({{access_status}})
- **数据保护**: {{protection_score}}/100 ({{protection_status}})
- **报告要求**: {{reporting_score}}/100 ({{reporting_status}})

### 合规风险

⚠️ **警告**: 以下问题可能导致:
- 监管罚款（最高可达全球营收的 4%）
- 业务暂停或牌照吊销
- 法律诉讼和声誉损失
- 外部审计失败

**建议**: 高危问题必须在下次审计前修复
```

### 6. 金融合规特定检查清单

#### 审计日志检查
- [ ] 所有交易操作是否记录（创建、修改、删除）
- [ ] 是否记录操作者身份（用户ID、IP地址）
- [ ] 是否记录操作时间（精确到毫秒）
- [ ] 是否记录操作前后的数据快照
- [ ] 日志文件是否只读或数字签名保护

#### 权限管理检查
- [ ] 用户登录是否多因素认证（MFA）
- [ ] 权限分配是否基于角色（RBAC）
- [ ] 敏感操作是否需要双重授权
- [ ] 权限变更是否记录和审批
- [ ] 会话是否自动超时（通常 15-30 分钟）

#### 数据保护检查
- [ ] 密码是否加密存储（bcrypt、Argon2）
- [ ] 敏感数据传输是否使用 TLS 1.2+
- [ ] 数据库连接是否加密
- [ ] 密钥是否使用 HSM 或 KMS 管理
- [ ] 是否定期轮换密钥（通常 90 天）

#### 监管报告检查
- [ ] 是否生成交易报告（MiFID II RTS 22）
- [ ] 是否生成大额交易报告（FATF 要求）
- [ ] 是否生成风险暴露报告（Basel III）
- [ ] 报告数据是否可追溯和验证
- [ ] 是否满足报告提交时间要求

## 集成建议

### CI/CD 集成
```bash
# .gitlab-ci.yml
compliance_review:
  stage: test
  script:
    - claude /compliance-review src/audit/ src/security/ --output json > compliance_report.json
    - python validate_compliance.py compliance_report.json
  artifacts:
    reports:
      compliance: compliance_report.json
```

### 本地开发集成
```bash
# pre-commit hook
#!/bin/bash
if git diff --cached | grep -q "src/audit\|src/security\|src/compliance"; then
    echo "🔍 合规性审查中..."
    claude /compliance-review --high-priority-only
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危合规问题，提交被阻止"
        exit 1
    fi
fi
```

## 最佳实践

### 1. 审计日志最佳实践
```cpp
// 完整的审计日志记录
class AuditLogger {
public:
    void log(const std::string& event_type, 
             const std::string& user_id,
             const std::string& data_before,
             const std::string& data_after) {
        
        AuditEntry entry;
        entry.timestamp = get_trusted_time(); // 可信时间源
        entry.event_type = event_type;
        entry.user_id = user_id;
        entry.ip_address = get_client_ip();
        entry.data_before = data_before;
        entry.data_after = data_after;
        entry.signature = sign(entry); // 数字签名
        
        // 写入只读存储
        write_to_worm_storage(entry);
        
        // 同时写入备份位置
        replicate_to_backup(entry);
    }
};
```

### 2. 权限管理最佳实践
```cpp
// 基于角色的访问控制（RBAC）
class PermissionManager {
public:
    bool check_permission(const User& user, Permission required) {
        // 1. 验证用户身份
        if (!user.is_authenticated()) return false;
        
        // 2. 检查角色权限
        if (!user.has_role(required.role())) return false;
        
        // 3. 检查会话有效性
        if (!user.session_is_valid()) return false;
        
        // 4. 记录权限检查
        audit_log("permission_check", user.id(), required);
        
        return true;
    }
    
    // 敏感操作需要双重授权
    bool check_four_eyes(const User& requester, Permission required) {
        if (!check_permission(requester, required)) return false;
        
        // 需要另一个授权者批准
        auto approver = get_approver_for(requester);
        return approver && approver->approve(required);
    }
};
```

### 3. 数据保护最佳实践
```cpp
// 敏感数据加密
class DataProtector {
public:
    std::string encrypt_pii(const std::string& plaintext) {
        // 1. 使用 KMS 获取密钥
        auto key = kms_client.get_key("pii-encryption-key");
        
        // 2. 使用 AES-256-GCM 加密
        auto ciphertext = aes_256_gcm_encrypt(plaintext, key);
        
        // 3. 添加密钥版本信息
        return format_with_key_version(ciphertext, key.version());
    }
    
    std::string decrypt_pii(const std::string& ciphertext) {
        // 1. 解析密钥版本
        auto [encrypted_data, key_version] = parse_key_version(ciphertext);
        
        // 2. 使用对应版本密钥解密
        auto key = kms_client.get_key("pii-encryption-key", key_version);
        return aes_256_gcm_decrypt(encrypted_data, key);
    }
};
```

## 避免的陷阱

- ❌ 在日志中记录密码或密钥
- ❌ 使用硬编码的加密密钥
- ❌ 未验证用户权限直接执行操作
- ❌ 审计日志可被应用程序修改
- ❌ 未加密传输敏感数据
- ❌ 权限变更无需审批
- ❌ 未保留足够的日志历史

## 更新日志

- **v1.0** (2026-04-08): 初始版本
  - 审计追踪、数据完整性、访问控制审查
  - 数据保护和报告要求验证
  - 合规性报告模板