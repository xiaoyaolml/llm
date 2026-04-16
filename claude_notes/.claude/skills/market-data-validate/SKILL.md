---
name: market-data-validate
description: 金融市场数据质量验证（完整性、准确性、一致性、时效性）
---

# 金融市场数据质量验证

## 触发条件

用户输入 `/market-data-validate` 时执行，或当代码包含以下关键词时自动触发：
- `market_data`、`ticker`、`quote`、`bar`、`ohlcv`、`feed`、`snapshot`、`historical`、`realtime`、`data_quality`

## 执行步骤

### 1. 识别数据类型

```
# 金融市场数据类型识别
1. 行情数据 (Quote Data) - 买卖报价、成交价、量
2. K线数据 (OHLCV Data) - 开高低收、成交量
3. 历史数据 (Historical Data) - 过去价格、因子数据
4. 实时数据 (Real-time Data) - 实时推送、快照
5. 参考数据 (Reference Data) - 股票信息、合约规格
6. 另生品数据 (Derivatives Data) - 期权隐波、希腊字母
7. 宏观数据 (Macro Data) - 利率、汇率、经济指标
8. 替代数据 (Alternative Data) - 卫星图像、社交媒体情绪
```

### 2. 获取代码上下文

```bash
# 检测市场数据相关代码
git diff | grep -E "(market_data|quote|ohlc|bar|feed)"  # 识别数据代码
grep -r "class.*MarketData\|def.*validate" . --include=*.py  # 识别数据类和验证函数
grep -r "Ticker\|Quote\|Bar\|Snapshot" . --include=*.py      # 识别数据结构
```

### 3. 数据质量验证维度

#### 3.1 数据完整性 (Data Completeness)
- [ ] **缺失值检测**: 是否存在缺失数据或空值
- [ ] **数据覆盖率**: 数据覆盖范围是否满足要求
- [ ] **连续性检查**: 数据序列是否连续，有无断点
- [ ] **字段完整性**: 所有必需字段是否都有值
- [ ] **时间序列完整性**: 时间戳是否连续且无重复

#### 3.2 数据准确性 (Data Accuracy)
- [ ] **价格验证**: 价格是否在合理范围内
- [ ] **量价关系**: 成交量与价格变动是否合理
- [ ] **涨跌幅限制**: 是否超过涨跌幅限制
- [ ] **除权除息处理**: 公司行为是否正确处理
- [ ] **货币单位**: 是否正确处理不同货币单位

#### 3.3 数据一致性 (Data Consistency)
- [ ] **跨市场一致性**: 相同标的在不同市场数据是否一致
- [ ] **数据源一致性**: 不同数据源之间的数据一致性
- [ ] **时间戳一致性**: 不同时区的时间戳处理是否一致
- [ ] **字段格式一致性**: 数据格式是否统一
- [ ] **参照数据一致性**: 标的名称、代码是否与参照数据一致

#### 3.4 数据时效性 (Data Timeliness)
- [ ] **实时延迟**: 实时数据延迟是否在可接受范围
- [ ] **更新频率**: 数据更新频率是否满足要求
- [ ] **时序正确性**: 数据时间戳是否正确排序
- [ ] **市场状态**: 是否正确处理开盘、收盘状态
- [ ] **时区处理**: 是否正确处理不同时区数据

#### 3.5 数据合规性 (Data Compliance)
- [ ] **数据许可**: 是否遵守数据许可协议
- [ ] **版权保护**: 是否正确标注数据版权信息
- [ ] **访问控制**: 是否正确控制数据访问权限
- [ ] **隐私保护**: 是否保护个人隐私信息
- [ ] **监管要求**: 是否满足金融监管数据要求

### 4. 严重级别定义

- **高危**：可能导致错误交易决策
  - 价格明显错误（如负股价、超出涨跌幅限制）
  - 重大数据缺失或断点
  - 未处理公司行为导致的数据不连续
  - 数据源不可靠或已过期
- **中危**：影响数据可靠性
  - 轻微的价格异常
  - 部分数据字段缺失
  - 数据延迟超过规定时间
  - 数据格式不统一
- **低危**：建议改进
  - 数据命名不规范
  - 缺少数据质量指标
  - 可选字段未填充

### 5. 输出格式

```
## 金融市场数据质量验证报告

**验证时间**: {{date_time}}
**数据类型**: {{data_type}} (行情/历史/K线/实时/参考/衍生/宏观/替代)
**数据范围**: {{data_range}} ({{symbol}} from {{start_time}} to {{end_time}})
**数据源**: {{data_source}}

### 问题清单

| # | 文件 | 行号 | 问题描述 | 验证维度 | 严重级别 | 修复建议 |
|---|------|------|----------|----------|----------|----------|
| 1 | data/feed_handler.py | 156 | 发现异常价格数据 | 准确性 | 高 | 实施价格过滤检查 |

### 详细分析

#### 1. 数据完整性问题
**问题**: 存在数据断点
```python
# 当前代码
def load_historical_data(symbol, start_date, end_date):
    # 可能跳过缺失日期，导致数据不连续
    raw_data = fetch_from_api(symbol, start_date, end_date)
    return raw_data  # 未检查连续性

# 修复建议
def load_historical_data(symbol, start_date, end_date):
    raw_data = fetch_from_api(symbol, start_date, end_date)
    
    # 检查日期连续性
    expected_dates = pd.date_range(start_date, end_date, freq='D')
    actual_dates = pd.to_datetime([d['date'] for d in raw_data])
    missing_dates = expected_dates.difference(actual_dates)
    
    if len(missing_dates) > 0:
        raise DataIntegrityError(f"Missing dates: {missing_dates}")
    
    return raw_data
```

#### 2. 数据准确性问题
**问题**: 未验证价格合理性
```python
# 当前代码
def process_quote(quote):
    price = quote['last_price']
    # 直接使用，未验证合理性

# 修复建议
def process_quote(quote):
    price = quote['last_price']
    
    # 验证价格合理性
    if price <= 0 or price > 1000000:  # 根据资产类型调整范围
        raise InvalidPriceError(f"Invalid price: {price}")
    
    # 验证涨跌幅
    if 'prev_close' in quote:
        change_pct = abs((price - quote['prev_close']) / quote['prev_close'])
        if change_pct > 0.2:  # 20%涨跌幅限制
            raise ExcessiveChangeError(f"Excessive change: {change_pct:.2%}")
```

### 数据质量评估

- **完整性**: {{completeness_score}}/100 ({{completeness_status}})
- **准确性**: {{accuracy_score}}/100 ({{accuracy_status}})
- **一致性**: {{consistency_score}}/100 ({{consistency_status}})
- **时效性**: {{timeliness_score}}/100 ({{timeliness_status}})
- **合规性**: {{compliance_score}}/100 ({{compliance_status}})

### 业务影响分析

⚠️ **警告**: 以下问题可能导致:
- 基于错误数据做出交易决策
- 策略回测结果失真
- 风险管理模型失效
- 监管报告不准确

**建议**: 高危问题必须在使用前修复
```

### 6. 金融数据特定检查清单

#### 行情数据检查
- [ ] 买卖报价是否合理（买价 < 卖价）
- [ ] 成交量是否非负
- [ ] 成交价是否在买价卖价之间
- [ ] 时间戳是否按序排列
- [ ] 是否处理集合竞价数据

#### K线数据检查
- [ ] 开盘价是否在高低之间
- [ ] 最高价 ≥ 开盘价、收盘价、最低价
- [ ] 最低价 ≤ 开盘价、收盘价、最高价
- [ ] 成交量是否非负
- [ ] 时间周期是否正确

#### 历史数据检查
- [ ] 是否包含除权除息信息
- [ ] 前复权/后复权处理是否正确
- [ ] 数据频率是否与所需一致
- [ ] 是否包含交易日历信息
- [ ] 是否处理分红配股信息

#### 实时数据检查
- [ ] 延迟是否在可接受范围
- [ ] 是否正确处理盘中休市
- [ ] 数据更新频率是否稳定
- [ ] 是否处理网络中断恢复
- [ ] 心跳检测是否正常

#### 参考数据检查
- [ ] 标的信息是否准确
- [ ] 上市/退市状态是否正确
- [ ] 合约规格是否完整
- [ ] 分类信息是否正确
- [ ] 是否及时更新

## 集成建议

### CI/CD 集成
```bash
# .gitlab-ci.yml
market_data_validation:
  stage: test
  script:
    - claude /market-data-validate src/data/ --output json > data_report.json
    - python validate_data_quality.py data_report.json
  artifacts:
    reports:
      data_quality: data_report.json
```

### 本地开发集成
```bash
# pre-commit hook
#!/bin/bash
if git diff --cached | grep -q "market_data\|quote\|bar\|feed"; then
    echo "🔍 市场数据质量验证中..."
    claude /market-data-validate --high-priority-only
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危数据质量问题，提交被阻止"
        exit 1
    fi
fi
```

## 最佳实践

### 1. 数据完整性验证最佳实践
```python
# 数据完整性检查
def validate_data_completeness(df, required_columns, date_col='date'):
    """验证数据完整性"""
    
    # 检查必需字段
    missing_cols = set(required_columns) - set(df.columns)
    if missing_cols:
        raise ValueError(f"Missing required columns: {missing_cols}")
    
    # 检查缺失值
    missing_data = df[required_columns].isnull().sum()
    if missing_data.any():
        raise ValueError(f"Missing values detected:\n{missing_data[missing_data > 0]}")
    
    # 检查时间连续性
    df[date_col] = pd.to_datetime(df[date_col])
    expected_range = pd.date_range(df[date_col].min(), df[date_col].max(), freq='D')
    actual_dates = set(df[date_col].dt.date)
    missing_dates = expected_range[~expected_range.date.isin(actual_dates)]
    
    if len(missing_dates) > 0:
        raise ValueError(f"Data gaps detected: {len(missing_dates)} days")
    
    return True

# 2. 数据准确性验证最佳实践
def validate_price_accuracy(df, symbol_type='equity'):
    """验证价格数据准确性"""
    
    # 价格范围验证
    if symbol_type == 'equity':
        invalid_prices = df[(df['close'] <= 0) | (df['close'] > 1000000)]
    elif symbol_type == 'future':
        invalid_prices = df[df['close'] <= 0]
    elif symbol_type == 'option':
        invalid_prices = df[(df['close'] < 0) | (df['close'] > 10000)]
    
    if not invalid_prices.empty:
        raise ValueError(f"Invalid prices detected:\n{invalid_prices[['date', 'close']]}")
    
    # 涨跌幅验证
    if 'prev_close' in df.columns:
        change_pct = abs((df['close'] - df['prev_close']) / df['prev_close'])
        excessive_changes = df[change_pct > 0.2]  # 20%限制
        if not excessive_changes.empty:
            raise ValueError(f"Excessive price changes detected:\n{excessive_changes[['date', 'close', 'prev_close']]}")
    
    return True

# 3. 数据一致性验证最佳实践
def validate_cross_market_consistency(data_sources, symbol):
    """验证跨市场数据一致性"""
    
    primary_source = data_sources[0]  # 主数据源
    secondary_sources = data_sources[1:]  # 次要数据源
    
    primary_data = primary_source.get_data(symbol)
    
    inconsistencies = []
    for source in secondary_sources:
        secondary_data = source.get_data(symbol)
        
        # 比较关键字段
        price_diff = abs(primary_data['close'] - secondary_data['close']) / primary_data['close']
        if (price_diff > 0.02).any():  # 2%容忍度
            inconsistencies.append({
                'source': source.name,
                'dates': price_diff[price_diff > 0.02].index.tolist(),
                'avg_diff': price_diff.mean()
            })
    
    if inconsistencies:
        raise ValueError(f"Cross-market inconsistencies detected:\n{inconsistencies}")
    
    return True

# 4. 实时数据验证最佳实践
def validate_realtime_feed(feed, symbol, max_latency=100):
    """验证实时数据流质量"""
    
    import time
    
    # 监控延迟
    timestamps = []
    for quote in feed:
        recv_time = time.time() * 1000  # 毫秒
        quote_time = quote['timestamp']
        latency = recv_time - quote_time
        
        if latency > max_latency:
            raise LatencyError(f"High latency detected: {latency}ms for {symbol}")
        
        timestamps.append((recv_time, quote_time))
        
        # 检查时间顺序
        if len(timestamps) > 1 and quote_time < timestamps[-2][1]:
            raise OutOfOrderError(f"Out-of-order data detected: {symbol}")
    
    return True
```

### 5. 数据质量指标最佳实践
```python
# 数据质量指标计算
def calculate_data_quality_metrics(df):
    """计算数据质量指标"""
    
    metrics = {
        'completeness': {
            'total_records': len(df),
            'missing_values': df.isnull().sum().sum(),
            'completeness_ratio': 1 - (df.isnull().sum().sum() / (len(df) * len(df.columns)))
        },
        'accuracy': {
            'invalid_price_count': ((df['close'] <= 0) | (df['close'] > 100000)).sum(),
            'negative_volume_count': (df['volume'] < 0).sum()
        },
        'consistency': {
            'duplicate_timestamps': df.duplicated(subset=['timestamp']).sum(),
            'out_of_order_count': (df['timestamp'].diff() < 0).sum()
        },
        'timeliness': {
            'avg_update_frequency': df['timestamp'].diff().mean(),
            'max_delay': (df.index.to_series().diff() - pd.Timedelta(seconds=1)).max()
        }
    }
    
    return metrics
```

## 避免的陷阱

- ❌ 使用未经验证的第三方数据源
- ❌ 忽略涨跌幅限制检查
- ❌ 未处理除权除息等公司行为
- ❌ 缺少数据完整性验证
- ❌ 实时数据延迟监控不足
- ❌ 跨市场数据一致性检查缺失
- ❌ 历史数据复权处理错误
- ❌ 数据访问权限控制不当

## 更新日志

- **v1.0** (2026-04-09): 初始版本
  - 数据完整性、准确性、一致性、时效性、合规性验证
  - 金融市场数据特定检查清单
  - 数据质量报告模板