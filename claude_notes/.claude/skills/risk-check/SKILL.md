---
name: risk-check
description: 金融风险计算代码审查（数值稳定性、精度、正确性）
---

# 金融风险计算审查

## 触发条件

用户输入 `/risk-check` 时执行，或当代码包含以下关键词时自动触发：
- `risk`、`var`、`cvar`、`monte carlo`、`simulation`、`greeks`、`hedging`、`exposure`

## 执行步骤

### 1. 识别风险计算类型

```
# 风险计算类型识别
1. 市场风险 (Market Risk) - VaR, CVaR, Greeks
2. 信用风险 (Credit Risk) - PD, LGD, EAD
3. 操作风险 (Operational Risk) - 损失分布
4. 流动性风险 (Liquidity Risk) - 压力测试
5. 模型风险 (Model Risk) - 参数敏感性
```

### 2. 获取代码上下文

```bash
# 自动检测风险计算代码
git diff | grep -E "(var|cvar|monte_carlo|simulation|risk)"  # 识别风险代码
grep -r "std::normal_distribution" . --include=*.cpp          # 识别随机模拟
grep -r "quantile|percentile" . --include=*.cpp               # 识别分位数计算
```

### 3. 风险计算审查维度

#### 3.1 数值稳定性
- [ ] **极端市场条件**: 价格为0、负值、极大值时的行为
- [ ] **除零保护**: 所有除法操作是否检查除数为0
- [ ] **溢出保护**: 大数运算是否检查整数/浮点溢出
- [ ] **收敛性检查**: 迭代算法是否有收敛保证和最大迭代次数

#### 3.2 精度控制
- [ ] **货币精度**: 财务计算使用 `decimal` 或固定点数（避免 `float`）
- [ ] **浮点比较**: 使用误差范围而非直接相等（`abs(a-b) < epsilon`）
- [ ] **累积误差**: 长时间序列计算是否控制累积误差
- [ ] **舍入策略**: 统一舍入规则（银行家舍入法）

#### 3.3 蒙特卡洛模拟
- [ ] **随机数质量**: RNG 是否通过统计测试（Diehard、TestU01）
- [ ] **种子管理**: 是否使用硬件随机源初始化，避免重复种子
- [ ] **路径生成**: 是否使用低差异序列（Sobol、Halton）提高收敛速度
- [ ] **方差缩减**: 是否使用控制变量、重要性采样等技术

#### 3.4 风险指标计算
- [ ] **VaR 计算**: 是否考虑尾部风险，使用合适的历史窗口
- [ ] **Greeks 计算**: 是否使用解析解而非数值微分（避免噪声）
- [ ] **压力测试**: 是否包含极端但合理的场景
- [ ] **相关性矩阵**: 是否正定，处理缺失数据的方式

#### 3.5 边界条件处理
- [ ] **空输入**: 无交易数据时的风险计算行为
- [ ] **单点数据**: 只有一个价格点时的统计计算
- [ ] **非交易日**: 周末/节假日的数据处理
- [ ] **市场关闭**: 极端波动时的熔断机制

### 4. 严重级别定义

- **高危**：可能导致错误风险评估的问题（直接影响业务决策）
  - 未处理的除零错误
  - 错误的随机数生成
  - 不正确的 VaR 计算
- **中危**：可能影响精度但不导致完全错误
  - 未优化的蒙特卡洛收敛
  - 次优的数值方法
  - 未充分的压力测试
- **低危**：风格或文档问题
  - 未注释的复杂公式
  - 未记录的假设条件

### 5. 输出格式

```
## 金融风险计算审查报告

**审查时间**: {{date_time}}
**风险类型**: {{risk_type}} (市场风险/信用风险/操作风险)
**计算方法**: {{calculation_method}} (历史模拟/Monte Carlo/参数法)

### 问题清单

| # | 文件 | 行号 | 问题描述 | 严重级别 | 修复建议 | 业务影响 |
|---|------|------|----------|----------|----------|----------|
| 1 | risk/var_calculator.cpp | 89 | 未检查除数为0 | 高 | 添加除零保护 | 可能导致系统崩溃 |

### 详细分析

#### 1. 数值稳定性问题
**问题**: 极端市场价格下计算崩溃
```cpp
// 当前代码
double return = (current_price - prev_price) / prev_price;

// 修复建议
if (prev_price == 0.0) {
    // 处理价格为0的情况
    return std::numeric_limits<double>::quiet_NaN();
}
double return = (current_price - prev_price) / prev_price;
```

#### 2. 蒙特卡洛模拟问题
**问题**: 随机数种子固定
```cpp
// 当前代码
std::mt19937 rng(42); // 固定种子

// 修复建议
std::random_device rd;
std::mt19937 rng(rd()); // 硬件随机种子
```

### 风险评估

- **高危问题**: {{high_count}} 个 (必须修复)
- **中危问题**: {{medium_count}} 个 (建议修复)
- **低危问题**: {{low_count}} 个 (可选修复)

### 业务影响分析

⚠️ **警告**: 以下问题可能导致:
- 风险资本计算错误
- 监管报告不准确
- 对冲策略失效
- 极端损失未被识别

**建议**: 高危问题必须在生产部署前修复
```

### 6. 风险计算特定检查清单

#### VaR 计算检查
- [ ] 历史窗口是否足够长（至少250天）
- [ ] 是否包含2008年金融危机数据
- [ ] 分位数计算是否使用线性插值
- [ ] 是否验证结果的统计显著性

#### 蒙特卡洛检查
- [ ] 路径数量是否足够（>10000）
- [ ] 随机数生成器是否通过统计测试
- [ ] 是否使用方差缩减技术
- [ ] 收敛性是否经过验证

#### Greeks 计算检查
- [ ] Delta/Gamma 是否使用解析解
- [ ] Vega/Theta 是否考虑波动率曲面
- [ ] 数值微分步长是否合理（1e-4 到 1e-6）
- [ ] 是否验证 Greeks 的一致性

## 集成建议

### CI/CD 集成
```bash
# .gitlab-ci.yml
risk_review:
  stage: test
  script:
    - claude /risk-check src/risk/ --output json > risk_report.json
    - python validate_risk.py risk_report.json
  artifacts:
    reports:
      risk: risk_report.json
```

### 本地开发集成
```bash
# pre-commit hook
#!/bin/bash
if git diff --cached | grep -q "src/risk"; then
    echo "🔍 风险计算审查中..."
    claude /risk-check --high-priority-only
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危风险计算问题，提交被阻止"
        exit 1
    fi
fi
```

## 最佳实践

### 1. 数值计算最佳实践
```cpp
// 使用适当的 epsilon
const double EPSILON = 1e-12;
if (std::abs(denominator) < EPSILON) {
    // 处理接近零的情况
}

// 使用稳定算法
// 差: sqrt(a*a + b*b) 可能溢出
// 好: std::hypot(a, b) 使用稳定算法
```

### 2. 风险计算验证
- **回测**: 使用历史数据验证 VaR 准确性
- **压力测试**: 包含极端但合理的场景
- **敏感性分析**: 测试参数变化的影响
- **基准比较**: 与已知正确实现比较

### 3. 避免的陷阱
- ❌ 使用 `float` 进行财务计算
- ❌ 忽略极端市场条件
- ❌ 固定随机数种子用于生产
- ❌ 未验证数值方法的收敛性

## 更新日志

- **v1.0** (2026-04-08): 初始版本
  - 数值稳定性、精度、蒙特卡洛审查
  - 风险指标计算验证
  - 风险计算报告模板