# 合规性检查清单 (compliance_checklists.md)

## 审计追踪 (Audit Trail)

### 必查项
- [ ] 所有关键操作是否记录完整日志
- [ ] 日志是否包含时间戳、用户ID、操作内容
- [ ] 日志是否使用数字签名确保不可篡改
- [ ] 是否使用可信时间源(NTP)校准
- [ ] 日志保留期是否符合法规要求(7年)

### 验证示例
```cpp
// ❌ 日志记录不完整
void processOrder(Order order) {
    // 缺少操作者信息和完整上下文
    log("Order processed");
    executeOrder(order);
}

// ✅ 符合审计追踪要求
void processOrder(Order order, User user) {
    AuditLog log;
    log.setTimestamp(getNtpTime());
    log.setUser(user.id);
    log.setAction("Order execution");
    log.setDetails(order.toJson());
    log.signWithPrivateKey(); // 数字签名
    auditLogger.save(log);
    
    executeOrder(order);
}
```

## 数据完整性 (Data Integrity)

### 必查项
- [ ] 输入数据是否验证格式和范围
- [ ] 传输过程是否使用校验和或哈希验证
- [ ] 是否有定期备份和恢复测试
- [ ] 数据变更是否记录数据血缘
- [ ] 分布式系统是否保证最终一致性

### 验证示例
```cpp
// ❌ 无数据验证
void storeData(const std::string& raw_data) {
    db.save(raw_data);
}

// ✅ 符合数据完整性要求
bool validateData(const TradeData& data) {
    // 验证必要字段
    if (!data.hasValidTimestamp() || !data.hasValidSymbol()) {
        return false;
    }
    
    // 验证数值范围
    if (data.quantity < 0 || data.price < MIN_PRICE) {
        return false;
    }
    
    // 计算数据校验和
    std::string checksum = computeSha256(data.toJson());
    return checksum == data.getProvidedChecksum();
}
```

## 访问控制 (Access Control)

### 必查项
- [ ] 所有敏感操作是否验证权限
- [ ] 是否遵循最小权限原则
- [ ] 关键操作是否需要双重授权
- [ ] 会话是否配置合理超时时间
- [ ] 权限变更是否需要审批并记录

### 验证示例
```cpp
// ❌ 权限验证缺失
void deleteAccount(int accountId) {
    db.delete("accounts", accountId);
}

// ✅ 符合访问控制要求
void deleteAccount(int accountId, const User& currentUser) {
    // 1. 验证用户权限
    if (!currentUser.hasPermission(Permission::ACCOUNT_DELETE)) {
        throw UnauthorizedException();
    }
    
    // 2. 敏感操作需要双重授权
    if (isHighRiskOperation()) {
        if (!twoFactorAuthRequired(currentUser)) {
            requestTwoFactorAuthentication();
            return;
        }
    }
    
    // 3. 记录权限操作
    auditLog.logPermissionAction(currentUser, "delete_account", accountId);
    
    db.delete("accounts", accountId);
}
```

## 数据保护 (Data Protection)

### 必查项
- [ ] 敏感数据(PII)是否加密存储
- [ ] 传输过程是否使用TLS 1.2+
- [ ] 密钥是否安全存储并定期轮换
- [ ] 非生产环境是否进行数据脱敏
- [ ] 数据删除是否确保不可恢复

### 验证示例
```cpp
// ❌ 明文存储敏感信息
struct User {
    std::string name;
    std::string ssn; // 社保号
};

// ✅ 符合数据保护要求
struct EncryptedData {
    std::string ciphertext;
    std::string iv; // 初始化向量
};

struct SecureUser {
    std::string name;
    EncryptedData encrypted_ssn;
    
    void setSSN(const std::string& ssn, const KeyManager& km) {
        // 使用AES-GCM加密
        auto encrypted = km.encryptAESGCM(ssn);
        encrypted_ssn = encrypted;
    }
    
    std::string getSSN(const KeyManager& km) const {
        return km.decryptAESGCM(encrypted_ssn);
    }
};
```

## 报告要求 (Reporting Requirements)

### 必查项
- [ ] 是否生成符合监管要求的报告
- [ ] 报告数据是否准确完整
- [ ] 是否满足规定提交时间
- [ ] 是否准备好接受外部审计
- [ ] 报告生成是否可重复验证

### 验证示例
```cpp
// ❌ 报告生成不合规
Report generateDailyReport() {
    auto data = getRawData();
    return Report(data);
}

// ✅ 符合报告要求
Report generateRegulatoryReport() {
    // 1. 获取符合要求的数据集
    RegulatoryDataset dataset = dataCollector.collect();
    
    // 2. 验证数据完整性
    if (!dataset.validate()) {
        throw DataIntegrityException();
    }
    
    // 3. 生成带审计追踪的报告
    Report report;
    report.setMetadata({
        {"generation_time", getCurrentTime()},
        {"regulation", "MiFID II"},
        {"auditor_signature", generateAuditSignature()}
    });
    
    // 4. 数据保留用于审计
    auditArchive.storeReport(report);
    
    return report;
}
```

## PCI-DSS 特定检查

### 必查项
- [ ] 持卡人数据是否分区存储
- [ ] 是否使用令牌化替代原始卡号
- [ ] 每年是否进行PCI合规性评估
- [ ] 网络隔离是否符合要求
- [ ] 是否定期进行漏洞扫描

## GDPR 特定检查

### 必查项
- [ ] 是否实现被遗忘权处理流程
- [ ] 是否进行数据保护影响评估(DPIA)
- [ ] 是否有数据泄露通知机制
- [ ] 是否实施数据最小化原则
- [ ] 是否有数据主体权利请求处理流程