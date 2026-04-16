# 🚀 /risk-check 技能 - 金融风险计算审查

## 📁 目录结构
```
risk-check/
├── SKILL.md                          # ✅ 核心技能定义
├── RULES.md                          # ✅ 金融风险计算规则库
├── CHECKLISTS/                       # ✅ 语言特定检查清单
│   └── cpp_risk_checklist.md         # ✅ C++ 金融风险计算清单
├── TEMPLATES/                        # ✅ 报告模板
│   ├── report_template.md             # ✅ Markdown 报告模板
│   └── report_template.json           # ✅ JSON 报告模板
└── README.md                         # 📖 本文件（使用和扩展指南）
```

---

## 🎯 快速使用

### 1. 重启 Claude Code
修改技能后需要**重启 Claude Code** 使新技能生效。

### 2. 基础使用
```bash
/risk-check                           # 审查当前目录下所有风险相关代码
/risk-check src/risk/                # 审查指定目录
/risk-check src/risk/var_calculator.cpp  # 审查指定文件
```

### 3. 高级选项
```bash
/risk-check --high-priority-only      # 只审查高危问题
/risk-check --output markdown         # 导出 Markdown 报告
/risk-check --output json             # 导出 JSON 报告
/risk-check --risk-type market        # 指定风险类型（market/credit/operational）
```

---

## 📊 审查维度

### 1. 数值稳定性
- **极端条件**: 价格为0、负值、极大值时的行为
- **除零保护**: 所有除法操作检查除数为0
- **溢出保护**: 大数运算检查溢出
- **收敛性**: 迭代算法设置最大迭代次数

### 2. 精度控制
- **货币精度**: 财务计算使用整数或 decimal（避免 `float`）
- **浮点比较**: 使用误差范围而非直接相等
- **累积误差**: 长时间序列控制累积误差
- **舍入策略**: 统一银行家舍入法

### 3. 蒙特卡洛模拟
- **随机数质量**: RNG 通过统计测试（Diehard、TestU01）
- **种子管理**: 硬件随机源初始化，避免重复种子
- **路径生成**: 低差异序列（Sobol、Halton）提高收敛速度
- **方差缩减**: 控制变量、重要性采样等技术

### 4. 风险指标计算
- **VaR 计算**: 包含2008年危机数据，合适的历史窗口
- **Greeks 计算**: 优先使用解析解，合理数值微分步长
- **压力测试**: 包含极端但合理的场景
- **相关性矩阵**: 正定性保证，缺失数据处理

### 5. 边界条件处理
- **空输入**: 无交易数据时的默认行为
- **单点数据**: 只有一个价格点时的统计计算
- **非交易日**: 周末/节假日的数据处理
- **市场关闭**: 极端波动时的熔断机制

---

## 🛠️ 扩展方式

### 1. 添加新风险类型规则
**场景**: 你需要支持信用风险计算优化

**步骤**:
1. 编辑 `RULES.md`，添加「信用风险计算」章节：
```markdown
## 11. 信用风险计算规则

### 11.1 PD/LGD/EAD 计算
- [ ] 违约概率(PD)是否使用合适的统计模型
- [ ] 损失给定违约(LGD)是否考虑回收率分布
- [ ] 风险暴露(EAD)是否包含承诺额度

### 11.2 信用组合模型
- [ ] 是否使用 Gaussian Copula 或其他相关性结构
- [ ] 是否验证模型在压力场景下的表现
- [ ] 是否包含行业集中度风险
```

2. 在 `CHECKLISTS/cpp_risk_checklist.md` 添加示例：
```cpp
// ❌ 简单信用风险模型
double calculate_credit_risk(double pd, double lgd, double ead) {
    return pd * lgd * ead;
}

// ✅ 完整信用风险模型
class CreditRiskModel {
public:
    double calculate_expected_loss(const Counterparty& cp) {
        // 考虑宏观经济因素
        double adjusted_pd = adjust_pd_for_economy(cp.pd, macro_factors);
        // 考虑行业相关性
        double correlated_lgd = apply_industry_correlation(cp.lgd, industry);
        return adjusted_pd * correlated_lgd * cp.ead;
    }
    
private:
    std::vector<double> macro_factors;
    std::unordered_map<std::string, double> industry_correlations;
};
```

### 2. 添加新验证方法
**场景**: 你需要支持 FRTB 合规性验证

**步骤**:
1. 创建 `CHECKLISTS/frtb_compliance_checklist.md`：
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

2. 更新 `SKILL.md`，添加 FRTB 识别逻辑

### 3. 自定义报告模板
**场景**: 你需要集成到内部风险管理系统

**步骤**:
1. 创建 `TEMPLATES/internal_risk_template.json`：
```json
{
  "risk_report": {
    "report_id": "{{report_id}}",
    "system": "Internal Risk Management System",
    "validation_status": "{{validation_status}}",
    "issues": {{#high_issues}}[
      {
        "severity": "CRITICAL",
        "description": "{{description}}",
        "file": "{{file}}",
        "line": {{line}},
        "business_impact": "{{business_impact}}"
      }
    ]{{/high_issues}},
    "approval_required": true,
    "approver_list": ["risk_manager", "model_validation"]
  }
}
```

2. 修改 `SKILL.md` 支持 `--output internal-risk` 选项

---

## 🔧 集成到工作流

### 1. Git Hooks (pre-commit)
```bash
# .git/hooks/pre-commit
#!/bin/bash

# 只审查风险相关代码
if git diff --cached --name-only | grep -q "src/risk\|src/model"; then
    echo "🔍 金融风险计算审查中..."
    claude /risk-check --high-priority-only
    
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危风险计算问题，提交被阻止"
        echo "💡 建议: 修复高危问题或使用 --no-verify 跳过检查"
        exit 1
    fi
fi
```

### 2. GitHub Actions CI
```yaml
name: Financial Risk Review

on: [push, pull_request]

jobs:
  risk-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Claude Code
      run: |
        curl -L https://claude.ai/install.sh | bash
        echo "$HOME/.claude/bin" >> $GITHUB_PATH
    
    - name: Run Risk Check Review
      run: |
        claude /risk-check src/risk/ --output json > risk_report.json
        
        # 检查是否有高危问题
        if jq '.issues.high_severity | length' risk_report.json | grep -q '[1-9]'; then
          echo "❌ 高危风险计算问题需要修复"
          cat risk_report.json | jq '.issues.high_severity'
          exit 1
        fi
    
    - name: Upload Risk Report
      uses: actions/upload-artifact@v3
      with:
        name: risk-report
        path: risk_report.json
```

### 3. 本地开发环境
```bash
# 创建 alias 简化使用
echo 'alias risk-review="claude /risk-check --output markdown"' >> ~/.bashrc
echo 'alias risk-review-json="claude /risk-check --output json"' >> ~/.bashrc

# 定期风险审查 (每天)
(crontab -l 2>/dev/null; echo "0 9 * * * cd /path/to/project && claude /risk-check --high-priority-only") | crontab -
```

---

## 🧪 测试技能

### 1. 测试高危问题检测
```cpp
// test_high_risk.cpp
#include <iostream>
#include <vector>

double calculate_return(double current, double previous) {
    // 高危：未检查除零
    return (current - previous) / previous;
}

int main() {
    double r = calculate_return(100.0, 0.0); // 会导致崩溃
    std::cout << "Return: " << r << std::endl;
    return 0;
}
```

运行审查：
```bash
/risk-check test_high_risk.cpp
```

**预期输出**: 应该检测到除零问题，并标记为高危

### 2. 测试蒙特卡洛问题
```cpp
// test_monte_carlo.cpp
#include <random>

double monte_carlo_pricing(int paths) {
    std::mt19937 rng(42); // 高危：固定种子用于生产
    double sum = 0.0;
    for (int i = 0; i < paths; ++i) {
        // ... 模拟逻辑
    }
    return sum / paths;
}
```

运行审查：
```bash
/risk-check test_monte_carlo.cpp
```

**预期输出**: 应该建议使用硬件随机种子

---

## 🐛 故障排查

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 技能无法触发 | 未重启 Claude Code | 重启 Claude Code |
| 无输出 | 没有风险相关代码 | 检查代码是否包含关键词 (var, risk, monte carlo) |
| 误报高危问题 | 规则过于严格 | 编辑 RULES.md 调整规则 |
| 报告格式错误 | 模板变量未替换 | 检查 TEMPLATES/ 目录权限 |
| 数值验证失败 | 测试环境不匹配 | 在相同硬件环境测试 |

---

## 📈 实际案例

### 案例 1: VaR 计算系统优化
**背景**: VaR 计算在极端市场条件下崩溃

**发现问题**:
- 未处理价格为0的情况
- 历史窗口只有100天，未包含2008年危机
- 未进行回测验证

**优化措施**:
```cpp
// 优化前
double calculate_var(std::vector<double> returns, double confidence) {
    size_t index = returns.size() * (1.0 - confidence);
    return returns[index];
}

// 优化后
double calculate_var(const std::vector<double>& returns, double confidence) {
    if (returns.empty()) return std::numeric_limits<double>::quiet_NaN();
    
    // 排序并处理边界
    auto sorted = returns;
    std::sort(sorted.begin(), sorted.end());
    
    // 线性插值
    double alpha = 1.0 - confidence;
    double exact_index = (sorted.size() - 1) * alpha;
    size_t lower = static_cast<size_t>(std::floor(exact_index));
    size_t upper = static_cast<size_t>(std::ceil(exact_index));
    
    if (lower == upper) return sorted[lower];
    
    double weight = exact_index - lower;
    return sorted[lower] * (1.0 - weight) + sorted[upper] * weight;
}
```

**结果**: 系统在极端条件下稳定运行，VaR 回测通过率从 60% 提升到 95%

### 案例 2: 蒙特卡洛定价引擎优化
**背景**: 蒙特卡洛模拟收敛太慢，计算时间 10 秒/次

**发现问题**:
- 路径数量不足 (1000)
- 未使用方差缩减技术
- 标准正态生成效率低

**优化措施**:
```cpp
// 优化前
double monte_carlo_price(int paths = 1000) {
    std::mt19937 rng(42);
    // 标准 Box-Muller
}

// 优化后
double monte_carlo_price(int paths = 50000) {
    std::random_device rd;
    std::mt19937 rng(rd());
    
    // 使用 Sobol 低差异序列
    SobolGenerator sobol(paths);
    
    // 控制变量方差缩减
    double control_variate = analytical_price();
    double sum = 0.0;
    for (int i = 0; i < paths; ++i) {
        double u = sobol.next();
        double z = inverse_normal_cdf(u);
        double mc_price = simulate_price(z);
        double cv_price = control_variate + (mc_price - analytical_approx(z));
        sum += cv_price;
    }
    return sum / paths;
}
```

**结果**: 计算时间从 10 秒 → 2 秒，精度提升 5 倍

---

## 🔄 持续改进

### 1. 规则更新流程
1. **发现新模式**: 团队发现新的风险计算问题模式
2. **更新规则**: 编辑 `RULES.md` 和 `CHECKLISTS/`
3. **测试验证**: 使用测试用例验证新规则
4. **团队同步**: 更新团队共享技能库

### 2. 版本管理
在 `SKILL.md` 底部维护更新日志：
```markdown
## 更新日志

- **v1.1** (2026-04-15):
  - 添加信用风险计算规则
  - 支持 FRTB 合规性验证
  - 修复 JSON 模板变量替换问题

- **v1.0** (2026-04-08):
  - 初始版本
  - 数值稳定性、精度、蒙特卡洛审查
  - 风险指标计算验证
```

### 3. 团队共享
```bash
# 创建团队技能仓库
git init finance-skills-repo
cp -r ~/.claude/skills/risk-check finance-skills-repo/
git add . && git commit -m "Add risk-check skill"
git remote add origin https://your-git-server/finance-skills.git
git push -u origin master

# 团队成员安装
git clone https://your-git-server/finance-skills.git
cp -r finance-skills/risk-check ~/.claude/skills/
```

---

## 🎯 下一步建议

1. ✅ **立即实施**: 
   - 重启 Claude Code 后测试技能
   - 在核心风险计算模块运行首次审查

2. 🔧 **定制规则**:
   - 根据团队特定产品添加规则
   - 调整风险类型和计算方法配置

3. 📊 **集成监控**:
   - 设置每日自动审查
   - 将报告集成到风险管理系统

4. 🌐 **团队协作**:
   - 将技能分享给风险管理团队
   - 建立规则更新流程

5. 📚 **深入学习**:
   - 阅读 `RULES.md` 了解完整规则
   - 研究金融风险计算最佳实践

---

**需要帮助？**  
- 查看完整文档: `cat ~/.claude/skills/risk-check/README.md`
- 测试技能: `/risk-check --help`
- 报告问题: 创建 GitHub issue 或联系风险管理团队

**技能版本**: v1.0 (2026-04-08)  
**最后更新**: {{current_date}}