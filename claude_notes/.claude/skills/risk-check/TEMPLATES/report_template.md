# 金融风险计算审查报告模板

## 审查概要

**报告ID**: `RISK-CHK-{{timestamp}}`  
**审查时间**: `{{date_time}}`  
**审查者**: `Claude Code /risk-check`  
**代码仓库**: `{{repository}}`  
**分支**: `{{branch}}`  

**风险类型**: `{{risk_type}}` (市场风险/信用风险/操作风险)  
**计算方法**: `{{calculation_method}}` (历史模拟/Monte Carlo/参数法)  
**数据范围**: `{{data_range}}` (最近 {{days}} 天数据)

---

## 问题详情

### 🔴 高危问题 (必须修复)

{{#high_issues}}
#### {{index}}. {{file}}:{{line}} - {{title}}

**问题描述**:  
{{description}}

**当前代码**:
```cpp
{{current_code}}
```

**修复建议**:
```cpp
{{fixed_code}}
```

**业务影响**:  
- **风险评估错误**: {{risk_impact}}
- **监管合规**: {{compliance_impact}}
- **财务影响**: {{financial_impact}}

**验证建议**:  
```bash
# 数值验证命令
./validate_numerical --test {{test_name}} --tolerance 1e-6
```
{{/high_issues}}

### 🟡 中危问题 (建议修复)

{{#medium_issues}}
#### {{index}}. {{file}}:{{line}} - {{title}}

**问题描述**:  
{{description}}

**修复建议**:  
```cpp
{{fixed_code}}
```

**业务影响**:  
- **精度影响**: {{precision_impact}}
- **性能影响**: {{performance_impact}}

**实施难度**: {{difficulty_level}} (1-5)
{{/medium_issues}}

### 🟢 低危问题 (可选修复)

{{#low_issues}}
- **{{file}}:{{line}}** - {{description}} (影响: {{impact}})
{{/low_issues}}

---

## 风险计算验证

### 1. 数值稳定性验证
{{#stability_tests}}
- ✅ **{{test_name}}**: {{result}} (容差: {{tolerance}})
{{/stability_tests}}

### 2. 精度验证
{{#precision_tests}}
- ✅ **{{test_name}}**: {{result}} (精度: {{precision}})
{{/precision_tests}}

### 3. 统计验证
{{#statistical_tests}}
- ✅ **{{test_name}}**: {{result}} (p-value: {{p_value}})
{{/statistical_tests}}

### 4. 业务验证
{{#business_tests}}
- ✅ **{{test_name}}**: {{result}} (符合 {{standard}} 要求)
{{/business_tests}}

---

## 风险评估总结

| 指标 | 当前状态 | 期望状态 | 差距 | 优先级 |
|------|----------|----------|------|--------|
| 数值稳定性 | {{stability_status}} | 稳定 | {{stability_gap}} | 🔴 |
| 计算精度 | {{precision_status}} | ≥1e-6 | {{precision_gap}} | 🔴 |
| VaR 回测 | {{var_backtest_status}} | 通过 | {{backtest_gap}} | 🔴 |
| 蒙特卡洛收敛 | {{mc_convergence_status}} | 收敛 | {{convergence_gap}} | 🟡 |
| 监管合规 | {{compliance_status}} | 符合 | {{compliance_gap}} | 🔴 |

**总体评估**: {{overall_assessment}}  
**部署建议**: {{deployment_recommendation}}

---

## 风险场景分析

### 极端市场条件测试
{{#extreme_scenarios}}
#### {{scenario_name}}
- **描述**: {{description}}
- **当前行为**: {{current_behavior}}
- **期望行为**: {{expected_behavior}}
- **风险等级**: {{risk_level}}
- **修复建议**: {{fix_suggestion}}
{{/extreme_scenarios}}

### 压力测试结果
{{#stress_test_results}}
#### {{stress_test_name}}
- **冲击大小**: {{shock_magnitude}}
- **当前损失**: {{current_loss}}
- **期望损失**: {{expected_loss}}
- **差异**: {{difference}}
- **是否可接受**: {{acceptable}}
{{/stress_test_results}}

---

## 实施计划

### 第一阶段 (立即) - 高危问题修复
{{#phase1_tasks}}
1. [ ] **{{task}}** (负责人: {{owner}}, 预计时间: {{time_estimate}})
   - 影响: {{impact}}
   - 验证: {{validation_method}}
{{/phase1_tasks}}

### 第二阶段 (1-3天) - 中危问题修复
{{#phase2_tasks}}
2. [ ] **{{task}}** (负责人: {{owner}}, 预计时间: {{time_estimate}})
   - 影响: {{impact}}
   - 验证: {{validation_method}}
{{/phase2_tasks}}

### 第三阶段 (1周+) - 架构改进
{{#phase3_tasks}}
3. [ ] **{{task}}** (负责人: {{owner}}, 预计时间: {{time_estimate}})
   - 影响: {{impact}}
   - 验证: {{validation_method}}
{{/phase3_tasks}}

---

## 验证计划

### 数值验证测试
```bash
# 精度测试
./precision_test --tolerance 1e-6 --iterations 1000

# 稳定性测试
./stability_test --extreme-values --boundary-conditions

# 收敛性测试
./convergence_test --max-paths 100000 --tolerance 1e-4
```

### 统计验证测试
```cpp
// VaR 回测测试
TEST_F(RiskTest, VaRBacktest) {
    auto results = var_calculator.backtest(historical_data, var_estimates, 0.99);
    EXPECT_TRUE(results.is_valid);
    EXPECT_NEAR(results.violations, results.expected_violations, 2.0 * sqrt(results.expected_violations));
}

// 蒙特卡洛收敛测试
TEST_F(MonteCarloTest, Convergence) {
    double estimate_10k = monte_carlo.price(10000);
    double estimate_100k = monte_carlo.price(100000);
    EXPECT_NEAR(estimate_10k, estimate_100k, 1e-3);
}
```

### 业务验证测试
```cpp
// 监管合规测试
TEST_F(ComplianceTest, BaselIIIRequirements) {
    auto capital_requirement = calculate_risk_weighted_assets();
    EXPECT_GE(capital_requirement, minimum_capital_ratio * risk_weighted_assets);
}

// 极端场景测试
TEST_F(StressTest, FinancialCrisis2008) {
    auto crisis_data = load_crisis_scenario("2008");
    auto losses = calculate_portfolio_losses(crisis_data);
    EXPECT_LE(losses, maximum_acceptable_loss);
}
```

---

## 附录

### 参考资料
- [Basel III 框架](https://www.bis.org/bcbs/basel3.htm)
- [FRTB 标准](https://www.bis.org/bcbs/publ/d457.htm)
- [QuantLib 文档](https://quantlib.org/documentation/)
- [数值计算最佳实践](https://www.mathworks.com/help/matlab/math/floating-point-numbers.html)

### 工具推荐
- **数值验证**: MATLAB, R, Python (NumPy/SciPy)
- **统计测试**: R, Python (scipy.stats)
- **随机数测试**: TestU01, Dieharder
- **性能分析**: Intel VTune, perf

### 联系方式
- **风险管理团队**: risk-team@company.com
- **模型验证**: model-validation@company.com
- **紧急支持**: +86-138-0013-8000
- **文档**: https://internal-docs.company.com/risk

---

**报告生成时间**: {{generation_time}}  
**工具版本**: Claude Code v2.1 /risk-check v1.0  
**保密级别**: 🔒 内部机密 - 仅限授权人员访问