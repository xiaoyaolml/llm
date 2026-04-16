# 🚀 /compliance-review 技能 - 金融合规性审查

## 📁 目录结构
```
compliance-review/
├── SKILL.md                          # ✅ 核心技能定义
├── CHECKLISTS/                       # ✅ 合规检查清单
│   └── compliance_checklists.md      # ✅ 各合规框架检查清单
├── TEMPLATES/                        # ✅ 报告模板
│   └── report_template.md            # ✅ 合规审查报告模板
└── README.md                         # 📖 本文件（使用和扩展指南）
```

---

## 🎯 快速使用

### 1. 重启 Claude Code
修改技能后需要**重启 Claude Code** 使新技能生效。

### 2. 基础使用
```bash
/compliance-review                    # 审查当前目录下所有合规相关代码
/compliance-review src/audit/         # 审查审计模块
/compliance-review src/security/      # 审查安全模块
```

### 3. 高级选项
```bash
/compliance-review --framework sox    # 指定合规框架（sox/mifid/gdpr）
/compliance-review --high-priority    # 只审查高危问题
/compliance-review --output markdown  # 导出 Markdown 报告
```

---

## 📊 审查维度

### 1. 审计追踪 (Audit Trail)
- **完整性**: 所有关键操作是否记录
- **不可篡改**: 日志是否数字签名或只读
- **时间戳**: 是否使用可信时间源（NTP）
- **保留期限**: 是否符合法规要求（7年）
- **查询能力**: 是否支持审计查询

### 2. 数据完整性 (Data Integrity)
- **数据验证**: 输入数据是否验证格式和范围
- **校验和**: 传输是否使用校验和
- **备份恢复**: 是否有定期备份和恢复测试
- **数据血缘**: 是否追踪数据来源和转换
- **一致性**: 分布式系统数据一致性

### 3. 访问控制 (Access Control)
- **权限验证**: 所有敏感操作是否验证权限
- **最小权限**: 是否遵循最小权限原则
- **权限分离**: 关键操作是否需要双重授权
- **会话管理**: 会话超时和令牌管理
- **权限变更**: 权限变更是否审批和记录

### 4. 数据保护 (Data Protection)
- **敏感数据**: 个人身份信息（PII）是否加密
- **传输加密**: 是否使用 TLS 1.2+
- **密钥管理**: 密钥是否安全存储和轮换
- **数据脱敏**: 非生产环境是否脱敏
- **数据销毁**: 删除是否不可恢复

### 5. 报告要求 (Reporting Requirements)
- **监管报告**: 是否生成符合要求的报告
- **数据准确性**: 报告数据是否准确完整
- **时间要求**: 是否满足提交时间要求
- **审计准备**: 是否准备好外部审计

---

## 🛠️ 扩展方式

### 1. 添加新合规框架规则
**场景**: 你需要支持 FRTB 合规性审查

**步骤**:
1. 创建 `CHECKLISTS/frtb_compliance.md`：
```markdown
# FRTB 合规性检查清单

## 必查项
- [ ] 敏感性计算是否使用标准方法
- [ ] 风险因子是否符合 FRTB 分类
- [ ] 对冲关系是否正确识别
- [ ] 资本要求计算是否符合公式

## 验证示例
```cpp
// ❌ 不符合 FRTB
double calculate_sensitivity(double price_change, double position) {
    return price_change * position;
}

// ✅ 符合 FRTB
struct FRTBSensitivity {
    RiskClass risk_class;      // 利率、股票、外汇等
    RiskFactor risk_factor;    // 具体风险因子
    double sensitivity;        // 敏感性值
    double curvature;          // 曲率风险
    
    double calculate_capital_requirement() const {
        // FRTB 标准资本计算公式
        return calculate_delta_risk() + calculate_vega_risk() + calculate_curvature_risk();
    }
};
```
```

### 2. 集成到工作流
详见 `README.md` 完整文档

---

## 金融合规技能组合总结

你现在拥有 3 个完整的金融行业技能：

| 技能 | 用途 | 适用场景 |
|------|------|----------|
| `/finance-optim` | 性能优化审查 | 高频交易、风险计算、市场数据处理 |
| `/risk-check` | 风险计算验证 | VaR、蒙特卡洛模拟、Greeks 计算 |
| `/compliance-review` | 合规性审查 | 审计追踪、访问控制、数据保护 |

**下一步建议**:
1. 重启 Claude Code 使所有技能生效
2. 在项目中测试每个技能
3. 根据团队需求定制规则
4. 集成到 CI/CD 和 Git hooks

**技能版本**: v1.0 (2026-04-08)  
**最后更新**: {{current_date}}