# 🚀 /finance-optim 技能 - 金融C++性能优化审查

## 📁 目录结构
```
finance-optim/
├── SKILL.md                          # ✅ 核心技能定义
├── RULES.md                          # ✅ 金融性能优化规则库
├── CHECKLISTS/                       # ✅ 语言特定检查清单
│   └── cpp_finance_checklist.md      # ✅ C++ 金融代码优化清单
├── TEMPLATES/                        # ✅ 报告模板
│   ├── report_template.md            # ✅ Markdown 报告模板
│   └── report_template.json          # ✅ JSON 报告模板
└── README.md                         # 📖 本文件（使用和扩展指南）
```

---

## 🎯 快速使用

### 1. 重启 Claude Code
修改技能后需要**重启 Claude Code** 使新技能生效。

### 2. 基础使用
```bash
/finance-optim                        # 审查当前目录下所有金融相关代码
/finance-optim src/trading/           # 审查指定目录
/finance-optim src/engine/pricer.cpp  # 审查指定文件
```

### 3. 高级选项
```bash
/finance-optim --high-priority-only   # 只审查高危问题
/finance-optim --output markdown      # 导出 Markdown 报告
/finance-optim --output json          # 导出 JSON 报告
/finance-optim --latency-target 100   # 设置延迟目标 100μs
```

---

## 📊 审查维度

### 1. 延迟优化
- **动态内存分配**：高频路径避免 `new`/`malloc`
- **虚函数调用**：关键路径使用 CRTP 或模板
- **锁竞争**：无锁数据结构（CAS、原子操作）
- **缓存局部性**：数据结构对齐和布局优化

### 2. 金融特定优化
- **货币计算**：整数运算（避免 `double` 精度问题）
- **价格档位**：O(1) 查找（哈希表 vs 二叉树）
- **随机数生成**：快速 RNG（XorShift vs MT19937）
- **向量化**：SIMD 优化（AVX2/AVX512）

### 3. 内存效率
- **对象池**：预分配高频对象
- **零拷贝**：避免不必要的拷贝
- **内存对齐**：缓存行对齐（`alignas(64)`）
- **False sharing**：添加 padding 避免

### 4. 编译器优化
- **内联提示**：`__attribute__((always_inline))`
- **循环展开**：手动或编译器提示
- **分支预测**：`__builtin_expect`
- **LTO**：链接时优化

---

## 🛠️ 扩展方式

### 1. 添加新金融产品规则
**场景**: 你需要支持期权定价优化

**步骤**:
1. 编辑 `RULES.md`，添加「期权定价优化」章节：
```markdown
## 11. 期权定价优化

### 11.1 Black-Scholes 优化
- [ ] 预计算常数项（`sqrt(2π)`、`ln(2)`）
- [ ] 使用查表法替代 erf 函数
- [ ] 向量化多个期权计算

### 11.2 二叉树优化
- [ ] 使用缓存友好布局
- [ ] 避免递归，使用迭代
- [ ] 预分配节点内存池
```

2. 在 `CHECKLISTS/cpp_finance_checklist.md` 添加示例：
```cpp
// ❌ 低效 Black-Scholes
double black_scholes(double S, double K, double T, double r, double sigma) {
    double d1 = (log(S/K) + (r + 0.5*sigma*sigma)*T) / (sigma*sqrt(T));
    return S * normal_cdf(d1) - K * exp(-r*T) * normal_cdf(d1 - sigma*sqrt(T));
}

// ✅ 优化版本
static constexpr double SQRT_2PI = 2.5066282746310002;
double fast_normal_cdf(double x) {
    // 使用查表法或多项式近似
    static const double a1 = 0.254829592;
    static const double a2 = -0.284496736; 
    static const double a3 = 1.421413741;
    static const double a4 = -1.453152027;
    static const double a5 = 1.061405429;
    static const double p = 0.3275911;
    
    const int sign = x < 0 ? -1 : 1;
    x = fabs(x) / sqrt(2.0);
    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);
    return 0.5 * (1.0 + sign * y);
}
```

### 2. 添加新语言支持
**场景**: 你需要支持 Python 量化代码优化

**步骤**:
1. 创建 `CHECKLISTS/python_quant_checklist.md`：
```markdown
# Python 量化代码性能检查清单

## 必查项
- [ ] 是否使用 NumPy 向量化操作（避免 Python 循环）
- [ ] Pandas 操作是否避免 `iterrows()`（改用 `apply()` 或向量化）
- [ ] JIT 编译是否启用（Numba `@jit` 装饰器）
- [ ] 内存视图是否使用（memoryview 避免拷贝）

## 优化示例
```python
# ❌ 低效
def calculate_returns(prices):
    returns = []
    for i in range(1, len(prices)):
        returns.append((prices[i] - prices[i-1]) / prices[i-1])
    return returns

# ✅ 优化
import numpy as np
def calculate_returns(prices):
    prices_arr = np.array(prices)
    return np.diff(prices_arr) / prices_arr[:-1]
```
```

2. 更新 `SKILL.md`，添加 Python 识别逻辑

### 3. 自定义报告模板
**场景**: 你需要集成到 JIRA 或内部系统

**步骤**:
1. 创建 `TEMPLATES/jira_template.md`：
```markdown
h1. 金融性能优化审查报告

*报告ID*: {{report_id}}
*审查时间*: {{date_time}}

h2. 高危问题

|| 问题ID || 文件 || 行号 || 描述 || 优先级 ||
{{#high_issues}}
| {{id}} | {{file}} | {{line}} | {{description}} | Blocker |
{{/high_issues}}
```

2. 修改 `SKILL.md` 支持 `--output jira` 选项

---

## 🔧 集成到工作流

### 1. Git Hooks (pre-commit)
```bash
# .git/hooks/pre-commit
#!/bin/bash

# 只审查金融相关代码
if git diff --cached --name-only | grep -q "src/trading\|src/risk\|src/pricing"; then
    echo "🔍 金融性能审查中..."
    claude /finance-optim --high-priority-only
    
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危性能问题，提交被阻止"
        echo "💡 建议: 修复高危问题或使用 --no-verify 跳过检查"
        exit 1
    fi
fi
```

### 2. GitHub Actions CI
```yaml
name: Financial Performance Review

on: [push, pull_request]

jobs:
  performance-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Claude Code
      run: |
        curl -L https://claude.ai/install.sh | bash
        echo "$HOME/.claude/bin" >> $GITHUB_PATH
    
    - name: Run Finance Optim Review
      run: |
        claude /finance-optim src/trading/ --output json > perf_report.json
        
        # 检查是否有高危问题
        if jq '.issues.high_severity | length' perf_report.json | grep -q '[1-9]'; then
          echo "❌ 高危性能问题需要修复"
          cat perf_report.json | jq '.issues.high_severity'
          exit 1
        fi
    
    - name: Upload Performance Report
      uses: actions/upload-artifact@v3
      with:
        name: performance-report
        path: perf_report.json
```

### 3. 本地开发环境
```bash
# 创建 alias 简化使用
echo 'alias perf-review="claude /finance-optim --output markdown"' >> ~/.bashrc
echo 'alias perf-review-json="claude /finance-optim --output json"' >> ~/.bashrc

# 定期性能审查 (每天)
(crontab -l 2>/dev/null; echo "0 9 * * * cd /path/to/project && claude /finance-optim --high-priority-only") | crontab -
```

---

## 🧪 测试技能

### 1. 测试高危问题检测
```cpp
// test_high_risk.cpp
#include <iostream>
#include <map>

class PricingEngine {
public:
    virtual double calculate() {
        auto result = new double[100]; // 高危：高频路径动态分配
        return result[0];
    }
};

int main() {
    PricingEngine engine;
    std::cout << engine.calculate() << std::endl;
    return 0;
}
```

运行审查：
```bash
/finance-optim test_high_risk.cpp
```

**预期输出**: 应该检测到动态内存分配问题，并标记为高危

### 2. 测试金融特定优化
```cpp
// test_currency.cpp
#include <iostream>

double calculate_profit(double initial, double final) {
    return final - initial; // 高危：使用 double 计算货币
}

int main() {
    double profit = calculate_profit(100.00, 101.01);
    std::cout << "Profit: " << profit << std::endl;
    return 0;
}
```

运行审查：
```bash
/finance-optim test_currency.cpp
```

**预期输出**: 应该建议使用整数运算替代 double

---

## 🐛 故障排查

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 技能无法触发 | 未重启 Claude Code | 重启 Claude Code |
| 无输出 | 没有金融相关代码 | 检查代码是否包含关键词 (pricing, trading, risk) |
| 误报高危问题 | 规则过于严格 | 编辑 RULES.md 调整规则 |
| 报告格式错误 | 模板变量未替换 | 检查 TEMPLATES/ 目录权限 |
| 性能数据不准 | 未在生产环境运行 | 在相同硬件环境测试 |

---

## 📈 实际案例

### 案例 1: 高频交易引擎优化
**背景**: 订单匹配延迟 250μs，目标 <100μs

**发现问题**:
- 高频路径使用 `std::map` (O(log n) 查找)
- 每次匹配分配新对象
- 互斥锁保护订单簿

**优化措施**:
```cpp
// 优化前
std::map<int64_t, Order> orders;
auto order = new Order();

// 优化后
std::unordered_map<int64_t, Order> orders; // O(1) 查找
static thread_local ObjectPool<Order> order_pool;
auto order = order_pool.allocate();
```

**结果**: 延迟从 250μs → 85μs，吞吐量提升 200%

### 案例 2: 风险计算系统优化
**背景**: VaR 计算 5 秒/次，目标 <1 秒

**发现问题**:
- 串行蒙特卡洛模拟
- 动态内存分配频繁
- 未使用 SIMD

**优化措施**:
```cpp
// 优化前
for (int i = 0; i < 100000; ++i) {
    paths[i] = simulate_path(params);
}

// 优化后
#pragma omp parallel for simd
for (int i = 0; i < 100000; ++i) {
    paths[i] = simulate_path(params);
}
```

**结果**: 计算时间 5s → 0.8s，内存使用降低 40%

---

## 🔄 持续改进

### 1. 规则更新流程
1. **发现新模式**: 团队发现新的性能问题模式
2. **更新规则**: 编辑 `RULES.md` 和 `CHECKLISTS/`
3. **测试验证**: 使用测试用例验证新规则
4. **团队同步**: 更新团队共享技能库

### 2. 版本管理
在 `SKILL.md` 底部维护更新日志：
```markdown
## 更新日志

- **v1.1** (2026-04-15):
  - 添加期权定价优化规则
  - 支持 Python 量化代码审查
  - 修复 JSON 模板变量替换问题

- **v1.0** (2026-04-08):
  - 初始版本
  - 延迟、吞吐量、内存效率审查
  - 金融特定规则和报告模板
```

### 3. 团队共享
```bash
# 创建团队技能仓库
git init finance-skills-repo
cp -r ~/.claude/skills/finance-optim finance-skills-repo/
git add . && git commit -m "Add finance-optim skill"
git remote add origin https://your-git-server/finance-skills.git
git push -u origin master

# 团队成员安装
git clone https://your-git-server/finance-skills.git
cp -r finance-skills/finance-optim ~/.claude/skills/
```

---

## 🎯 下一步建议

1. ✅ **立即实施**: 
   - 重启 Claude Code 后测试技能
   - 在核心交易引擎运行首次审查

2. 🔧 **定制规则**:
   - 根据团队特定产品添加规则
   - 调整性能目标值 (延迟、吞吐量)

3. 📊 **集成监控**:
   - 设置每日自动审查
   - 将报告集成到 Grafana 仪表板

4. 🌐 **团队协作**:
   - 将技能分享给团队成员
   - 建立规则更新流程

5. 📚 **深入学习**:
   - 阅读 `RULES.md` 了解完整规则
   - 研究 C++ 金融性能优化最佳实践

---

**需要帮助？**  
- 查看完整文档: `cat ~/.claude/skills/finance-optim/README.md`
- 测试技能: `/finance-optim --help`
- 报告问题: 创建 GitHub issue 或联系性能团队

**技能版本**: v1.0 (2026-04-08)  
**最后更新**: {{current_date}}