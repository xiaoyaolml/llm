---
name: quant-backtest
description: 量化策略回测验证（逻辑正确性、数据质量、过拟合检测）
---

# 量化策略回测验证

## 触发条件

用户输入 `/quant-backtest` 时执行，或当代码包含以下关键词时自动触发：
- `backtest`、`strategy`、`portfolio`、`returns`、`sharpe`、`max_drawdown`、`overfitting`、`walk_forward`

## 执行步骤

### 1. 识别策略类型

```
# 量化策略类型识别
1. 趋势跟踪策略 (Trend Following) - 移动平均、通道突破
2. 均值回归策略 (Mean Reversion) - 统计套利、配对交易
3. 动量策略 (Momentum) - 价格动量、成交量动量
4. 套利策略 (Arbitrage) - 期现套利、跨期套利
5. 机器学习策略 (ML Strategy) - 强化学习、深度学习
6. 因子策略 (Factor Strategy) - 多因子模型、风险平价
```

### 2. 获取代码上下文

```bash
# 检测量化策略相关代码
git diff | grep -E "(strategy|backtest|portfolio|returns)"  # 识别策略代码
grep -r "class.*Strategy\|def.*backtest" . --include=*.py  # 识别策略类和回测函数
grep -r "SharpeRatio\|MaxDrawdown" . --include=*.py        # 识别风险指标
```

### 3. 回测验证维度

#### 3.1 策略逻辑正确性
- [ ] **信号生成**: 买卖信号是否逻辑正确
- [ ] **仓位管理**: 仓位调整是否符合策略规则
- [ ] **风控机制**: 止损、止盈、最大持仓等是否生效
- [ ] **交易成本**: 手续费、滑点是否正确计算
- [ ] **时间处理**: 时区、交易时间是否正确处理

#### 3.2 数据质量验证
- [ ] **数据完整性**: 是否存在缺失数据或异常值
- [ ] **数据清洗**: 是否正确处理分红、拆股等公司行为
- [ ] **前窥偏差**: 是否避免使用未来数据
- [ ] **生存者偏差**: 是否包含已退市股票
- [ ] **数据频率**: 数据频率是否与策略匹配

#### 3.3 绩效指标计算
- [ ] **收益率**: 年化收益率、累计收益率计算正确
- [ ] **风险指标**: 夏普比率、最大回撤、波动率计算正确
- [ ] **交易统计**: 胜率、盈亏比、交易频率统计正确
- [ ] **基准比较**: 是否与基准指数进行合理比较
- [ ] **风险调整**: 是否考虑交易成本后的净收益

#### 3.4 过拟合检测
- [ ] **样本外测试**: 是否有独立的样本外测试
- [ ] **参数敏感性**: 策略参数是否过于敏感
- [ ] **数据挖掘偏差**: 是否过度优化历史数据
- [ ] **滚动回测**: 是否使用滚动窗口验证稳定性
- [ ] **交叉验证**: 是否使用多种市场环境验证

#### 3.5 回测框架验证
- [ ] **订单执行**: 订单是否按规则执行
- [ ] **撮合机制**: 撮合逻辑是否符合实际市场
- [ ] **资金管理**: 现金流、保证金是否正确处理
- [ ] **复权处理**: 价格是否正确复权
- [ ] **税务处理**: 是否考虑税收影响

### 4. 严重级别定义

- **高危**：导致错误的投资决策
  - 前窥偏差
  - 错误的收益率计算
  - 未处理的交易成本
  - 生存者偏差
- **中危**：影响策略可靠性
  - 过度优化参数
  - 未验证的风控机制
  - 不合理的绩效指标
  - 数据质量问题
- **低危**：建议改进
  - 未优化的代码结构
  - 未充分的文档
  - 可选的验证检查

### 5. 输出格式

```
## 量化策略回测验证报告

**验证时间**: {{date_time}}
**策略类型**: {{strategy_type}} (趋势跟踪/均值回归/动量/套利/机器学习/因子)
**数据范围**: {{data_range}} ({{start_date}} 至 {{end_date}})

### 问题清单

| # | 文件 | 行号 | 问题描述 | 验证维度 | 严重级别 | 修复建议 |
|---|------|------|----------|----------|----------|----------|
| 1 | strategies/trend_following.py | 156 | 存在前窥偏差 | 数据质量 | 高 | 使用滞后数据生成信号 |

### 详细分析

#### 1. 策略逻辑问题
**问题**: 止损机制未生效
```python
# 当前代码
if current_price < entry_price * 0.95:
    # 止损信号生成，但未执行
    signal = "SELL"

# 修复建议
if current_price < entry_price * 0.95:
    # 立即执行止损订单
    execute_order("SELL", position_size)
```

#### 2. 数据质量问题
**问题**: 未处理股票分红
```python
# 当前代码
# 直接使用原始价格数据进行回测

# 修复建议
# 使用复权价格数据或手动处理分红影响
adjusted_prices = adjust_for_dividends(raw_prices, dividend_info)
```

### 回测质量评估

- **逻辑正确性**: {{logic_score}}/100 ({{logic_status}})
- **数据质量**: {{data_score}}/100 ({{data_status}})
- **绩效计算**: {{performance_score}}/100 ({{performance_status}})
- **过拟合风险**: {{overfitting_score}}/100 ({{overfitting_status}})

### 投资建议

⚠️ **警告**: 以下问题可能导致:
- 实盘表现与回测结果差异巨大
- 错误的投资决策和资金损失
- 策略在实盘中失效

**建议**: 高危问题必须在实盘前修复
```

### 6. 量化策略特定检查清单

#### 策略逻辑检查
- [ ] 买卖信号生成逻辑是否正确
- [ ] 仓位管理是否符合风险控制要求
- [ ] 止损止盈机制是否有效
- [ ] 交易频率是否合理
- [ ] 资金利用率是否优化

#### 数据质量检查
- [ ] 是否存在缺失数据
- [ ] 是否正确处理公司行为（分红、拆股）
- [ ] 是否避免前窥偏差
- [ ] 是否包含生存者偏差
- [ ] 数据清洗是否充分

#### 绩效验证检查
- [ ] 收益率计算是否正确
- [ ] 风险指标是否合理
- [ ] 与基准比较是否公平
- [ ] 交易成本是否充分考虑
- [ ] 统计显著性是否验证

#### 过拟合防范检查
- [ ] 是否有样本外测试
- [ ] 参数是否过度优化
- [ ] 策略是否在不同市场环境下稳定
- [ ] 是否使用交叉验证
- [ ] 滚动回测是否显示稳定性

## 集成建议

### CI/CD 集成
```bash
# .gitlab-ci.yml
quant_backtest:
  stage: test
  script:
    - claude /quant-backtest src/strategies/ --output json > backtest_report.json
    - python validate_backtest.py backtest_report.json
  artifacts:
    reports:
      backtest: backtest_report.json
```

### 本地开发集成
```bash
# pre-commit hook
#!/bin/bash
if git diff --cached | grep -q "strategy\|backtest"; then
    echo "🔍 量化策略回测验证中..."
    claude /quant-backtest --high-priority-only
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危回测问题，提交被阻止"
        exit 1
    fi
fi
```

## 最佳实践

### 1. 策略逻辑最佳实践
```python
# 正确的策略框架
class TradingStrategy:
    def __init__(self, params):
        self.params = params
        self.position = 0
        self.entry_price = 0
        
    def generate_signal(self, data):
        """生成交易信号，避免前窥偏差"""
        # 使用滞后数据
        lagged_data = data.shift(1)
        
        # 生成信号
        if self.should_buy(lagged_data):
            return "BUY"
        elif self.should_sell(lagged_data):
            return "SELL"
        else:
            return "HOLD"
    
    def should_buy(self, data):
        """买入条件判断"""
        # 实现具体买入逻辑
        return ma_short > ma_long and rsi < 30
    
    def should_sell(self, data):
        """卖出条件判断"""
        # 实现具体卖出逻辑
        return current_price < self.entry_price * 0.95  # 止损
```

### 2. 数据处理最佳实践
```python
# 正确的数据处理
def prepare_backtest_data(raw_data):
    """准备回测数据，处理各种偏差"""
    
    # 1. 处理缺失数据
    clean_data = raw_data.fillna(method='ffill').fillna(method='bfill')
    
    # 2. 处理公司行为
    adjusted_data = adjust_for_corporate_actions(clean_data)
    
    # 3. 避免前窥偏差
    lagged_data = adjusted_data.shift(1)
    
    # 4. 处理生存者偏差
    survivor_bias_free_data = include_delisted_stocks(raw_data)
    
    return lagged_data

# 正确的绩效计算
def calculate_performance(returns, benchmark_returns=None):
    """计算策略绩效指标"""
    
    # 年化收益率
    annual_return = (1 + returns.mean()) ** 252 - 1
    
    # 年化波动率
    annual_volatility = returns.std() * np.sqrt(252)
    
    # 夏普比率
    sharpe_ratio = annual_return / annual_volatility if annual_volatility > 0 else 0
    
    # 最大回撤
    cumulative = (1 + returns).cumprod()
    running_max = cumulative.expanding().max()
    drawdown = (cumulative - running_max) / running_max
    max_drawdown = drawdown.min()
    
    return {
        'annual_return': annual_return,
        'annual_volatility': annual_volatility,
        'sharpe_ratio': sharpe_ratio,
        'max_drawdown': max_drawdown
    }
```

### 3. 过拟合防范最佳实践
```python
# 滚动回测验证策略稳定性
def rolling_backtest(data, strategy_class, window_size=252, step_size=63):
    """滚动回测验证策略稳定性"""
    
    results = []
    for i in range(window_size, len(data), step_size):
        # 训练窗口
        train_data = data.iloc[i-window_size:i]
        
        # 测试窗口
        test_data = data.iloc[i:i+step_size]
        
        # 训练策略
        strategy = strategy_class()
        strategy.optimize(train_data)
        
        # 测试策略
        test_returns = strategy.backtest(test_data)
        results.append(test_returns)
    
    # 分析滚动结果的稳定性
    stability_score = analyze_rolling_stability(results)
    return stability_score

# 参数敏感性分析
def parameter_sensitivity_analysis(strategy_class, data, param_ranges):
    """参数敏感性分析，避免过度优化"""
    
    results = {}
    for param_name, param_values in param_ranges.items():
        param_results = []
        for param_value in param_values:
            strategy = strategy_class(**{param_name: param_value})
            returns = strategy.backtest(data)
            param_results.append((param_value, returns.mean()))
        results[param_name] = param_results
    
    # 分析参数敏感性
    sensitivity_scores = analyze_parameter_sensitivity(results)
    return sensitivity_scores
```

## 避免的陷阱

- ❌ 使用未来数据生成信号（前窥偏差）
- ❌ 忽略交易成本和滑点
- ❌ 未处理公司行为（分红、拆股）
- ❌ 过度优化历史数据参数
- ❌ 未进行样本外测试
- ❌ 忽略生存者偏差
- ❌ 错误计算绩效指标
- ❌ 未验证风控机制有效性

## 更新日志

- **v1.0** (2026-04-09): 初始版本
  - 策略逻辑、数据质量、绩效计算、过拟合检测
  - 量化策略回测验证报告模板
  - 量化策略检查清单