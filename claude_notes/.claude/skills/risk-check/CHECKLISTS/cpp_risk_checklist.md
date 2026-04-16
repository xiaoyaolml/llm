# C++ 金融风险计算检查清单

## 必查项 (高危)

- [ ] **除零保护**: 所有除法操作是否检查除数为0
```cpp
// ❌ 危险：未检查除零
double return = (current_price - prev_price) / prev_price;

// ✅ 安全：添加除零保护
if (prev_price == 0.0) {
    return std::numeric_limits<double>::quiet_NaN();
}
double return = (current_price - prev_price) / prev_price;
```

- [ ] **浮点比较**: 是否使用误差范围而非直接相等
```cpp
// ❌ 危险：直接比较浮点数
if (price == 0.0) { /* handle */ }

// ✅ 安全：使用误差范围
const double EPSILON = 1e-12;
if (std::abs(price) < EPSILON) { /* handle */ }
```

- [ ] **随机数种子**: 是否使用硬件随机源而非固定种子
```cpp
// ❌ 危险：固定种子（生产环境）
std::mt19937 rng(42);

// ✅ 安全：硬件随机种子
std::random_device rd;
std::mt19937 rng(rd());
```

- [ ] **货币精度**: 财务计算是否使用整数或 decimal
```cpp
// ❌ 危险：使用 double 计算货币
double profit = final_amount - initial_amount;

// ✅ 安全：使用整数（单位：分）
int64_t profit_cents = final_cents - initial_cents;
```

## 风险计算特定模式

### 1. VaR 计算优化

| 模式 | 风险 | 修复建议 |
|------|------|----------|
| 使用正态分布假设尾部 | 极端损失低估 | 改用 t 分布或历史模拟 |
| 历史窗口太短 (<100天) | 未包含危机数据 | 扩展到至少250天，包含2008年数据 |
| 线性插值分位数 | 精度不足 | 使用样条插值或核密度估计 |
| 未进行回测验证 | 结果不可信 | 实现定期 VaR 回测 |

### 2. 蒙特卡洛模拟优化

| 模式 | 风险 | 修复建议 |
|------|------|----------|
| 路径数量太少 (<1000) | 方差过大 | 增加到 >10000 路径 |
| 未使用方差缩减 | 收敛太慢 | 添加控制变量或重要性采样 |
| 标准正态生成效率低 | 性能瓶颈 | 使用 Box-Muller 或 Ziggurat 算法 |
| 未验证 RNG 质量 | 结果不可靠 | 运行 Diehard 或 TestU01 测试 |

### 3. Greeks 计算优化

| 模式 | 风险 | 修复建议 |
|------|------|----------|
| 数值微分步长太大 (>1e-3) | 精度不足 | 使用 1e-4 到 1e-6 步长 |
| 未使用解析解 | 噪声过大 | 优先使用 Black-Scholes 解析 Greeks |
| 忽略波动率曲面 | 对冲失效 | 考虑局部波动率或随机波动率 |
| 未验证一致性 | 逻辑错误 | 检查 Delta-Gamma 近似的一致性 |

## 代码示例对比

### 差的实现 (数值不稳定)
```cpp
class VaRCalculator {
public:
    double calculateVaR(const std::vector<double>& returns, double confidence) {
        // 未排序，未处理边界
        size_t index = static_cast<size_t>(returns.size() * (1.0 - confidence));
        return returns[index]; // 可能越界
    }
    
    double monteCarloPricing(int paths) {
        std::mt19937 rng(42); // 固定种子
        double sum = 0.0;
        for (int i = 0; i < paths; ++i) {
            double z = normal_distribution(rng);
            sum += payoff(z);
        }
        return sum / paths;
    }
};
```

### 优化的实现 (数值稳定)
```cpp
class VaRCalculator {
public:
    double calculateVaR(const std::vector<double>& returns, double confidence) {
        if (returns.empty()) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        // 排序并处理边界
        std::vector<double> sorted_returns = returns;
        std::sort(sorted_returns.begin(), sorted_returns.end());
        
        double alpha = 1.0 - confidence;
        double index_f = (sorted_returns.size() - 1) * alpha;
        size_t index_low = static_cast<size_t>(std::floor(index_f));
        size_t index_high = static_cast<size_t>(std::ceil(index_f));
        
        if (index_low == index_high) {
            return sorted_returns[index_low];
        }
        
        // 线性插值
        double weight = index_f - index_low;
        return sorted_returns[index_low] * (1.0 - weight) + 
               sorted_returns[index_high] * weight;
    }
    
    double monteCarloPricing(int paths) {
        std::random_device rd;
        std::mt19937 rng(rd()); // 硬件随机种子
        
        // 使用低差异序列提高收敛速度
        SobolGenerator sobol(paths);
        double sum = 0.0;
        for (int i = 0; i < paths; ++i) {
            double u = sobol.next(); // [0,1) 均匀分布
            double z = inverse_normal_cdf(u); // 转换为标准正态
            sum += payoff(z);
        }
        return sum / paths;
    }
};
```

## 数值计算最佳实践

### 1. 稳定算法选择
```cpp
// ❌ 不稳定：可能溢出
double hypotenuse = sqrt(x*x + y*y);

// ✅ 稳定：使用内置函数
double hypotenuse = std::hypot(x, y);

// ❌ 不稳定：大数相减
double log_diff = log(exp(a) - exp(b)); // a≈b 时精度丢失

// ✅ 稳定：使用 log-sum-exp 技巧
double log_diff = a + log(1.0 - exp(b - a)); // 假设 a > b
```

### 2. 精度保持
```cpp
// ❌ 精度丢失：Kahan 求和
double sum = 0.0;
for (double x : values) {
    sum += x;
}

// ✅ 精度保持：Kahan 求和算法
double sum = 0.0, c = 0.0;
for (double x : values) {
    double y = x - c;
    double t = sum + y;
    c = (t - sum) - y;
    sum = t;
}
```

### 3. 边界条件处理
```cpp
// 处理极端市场价格
double calculate_return(double current, double previous) {
    const double EPSILON = 1e-12;
    
    // 处理价格为0
    if (std::abs(previous) < EPSILON) {
        if (std::abs(current) < EPSILON) {
            return 0.0; // 0/0 = 0
        } else {
            return std::copysign(std::numeric_limits<double>::infinity(), current);
        }
    }
    
    // 处理极大价格（防止溢出）
    if (std::abs(current) > 1e100 || std::abs(previous) > 1e100) {
        return (current > 0 ? 1.0 : -1.0) * std::numeric_limits<double>::infinity();
    }
    
    return (current - previous) / previous;
}
```

## 风险计算验证框架

```cpp
// VaR 回测验证
class VaRBacktest {
public:
    struct Result {
        int violations;      // 违约次数
        int total_days;      // 总天数
        double expected_violations; // 期望违约次数
        bool is_valid;       // 是否通过检验
    };
    
    Result backtest(const std::vector<double>& portfolio_values,
                   const std::vector<double>& var_estimates,
                   double confidence) {
        int violations = 0;
        for (size_t i = 1; i < portfolio_values.size(); ++i) {
            double loss = portfolio_values[i-1] - portfolio_values[i];
            if (loss > var_estimates[i-1]) {
                violations++;
            }
        }
        
        int total_days = portfolio_values.size() - 1;
        double expected = total_days * (1.0 - confidence);
        double tolerance = 2.0 * std::sqrt(expected); // 2 sigma
        
        bool valid = std::abs(violations - expected) <= tolerance;
        
        return {violations, total_days, expected, valid};
    }
};

// 蒙特卡洛收敛验证
void validate_monte_carlo_convergence(
    std::function<double(int)> pricing_func,
    int max_paths = 100000) {
    
    std::vector<double> estimates;
    std::vector<int> path_counts;
    
    for (int paths = 1000; paths <= max_paths; paths *= 2) {
        double estimate = pricing_func(paths);
        estimates.push_back(estimate);
        path_counts.push_back(paths);
    }
    
    // 绘制收敛图（伪代码）
    plot_convergence(path_counts, estimates);
    
    // 检查最后几个估计值的标准差
    double last_std = calculate_std(
        estimates.end() - 5, estimates.end());
    
    if (last_std > 1e-3) {
        std::cout << "⚠️ 蒙特卡洛未充分收敛" << std::endl;
    }
}
```

## 风险系统架构检查

### 1. 数据质量检查
- [ ] 市场数据是否有缺失值
- [ ] 价格序列是否连续
- [ ] 异常值是否被识别和处理
- [ ] 时区和夏令时是否正确处理

### 2. 模型验证检查
- [ ] 模型假设是否合理
- [ ] 参数校准是否准确
- [ ] 极端场景是否被覆盖
- [ ] 模型风险是否被量化

### 3. 计算可靠性检查
- [ ] 数值方法是否稳定
- [ ] 并行计算是否正确同步
- [ ] 内存使用是否合理
- [ ] 计算时间是否可接受

## 更新日志

- **v1.0** (2026-04-08): 初始版本
  - C++ 金融风险计算检查清单
  - VaR、蒙特卡洛、Greeks 优化
  - 数值计算最佳实践和验证框架