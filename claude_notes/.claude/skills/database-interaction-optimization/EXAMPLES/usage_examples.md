# 数据库交互优化技能使用示例

## 示例 1: 基本使用

```bash
# 审查整个数据库模块
claude /database-interaction-optimization src/database/

# 输出结果将包含：
# - 连接池配置分析
# - 查询性能评估
# - 事务管理检查
# - 缓存策略评估
```

## 示例 2: 指定审查维度

```bash
# 仅审查查询优化
claude /database-interaction-optimization src/database/ --dimension query-optimization

# 仅审查连接池管理
claude /database-interaction-optimization src/database/ --dimension connection-pool

# 审查多个维度
claude /database-interaction-optimization src/database/ --dimension query-optimization,caching,indexing
```

## 示例 3: 指定数据库类型

```bash
# MySQL数据库审查
claude /database-interaction-optimization src/database/ --database mysql

# PostgreSQL数据库审查
claude /database-interaction-optimization src/database/ --database postgresql

# MongoDB数据库审查
claude /database-interaction-optimization src/database/ --database mongodb

# Redis缓存审查
claude /database-interaction-optimization src/cache/ --database redis
```

## 示例 4: 输出不同格式

```bash
# Markdown格式输出（默认）
claude /database-interaction-optimization src/database/

# JSON格式输出
claude /database-interaction-optimization src/database/ --output json

# HTML格式输出
claude /database-interaction-optimization src/database/ --output html

# 输出到文件
claude /database-interaction-optimization src/database/ --output json > database-review.json
```

## 示例 5: 详细模式

```bash
# 详细输出，包含所有检查项
claude /database-interaction-optimization src/database/ --verbose

# 简洁输出，只显示问题和建议
claude /database-interaction-optimization src/database/ --quiet
```

## 示例 6: 针对性审查

```cpp
// 代码示例：待审查的数据库访问代码
class TradingDatabase {
public:
    void executeTrade(const Trade& trade) {
        // 获取数据库连接
        auto conn = connectionPool.getConnection();

        try {
            // 开始事务
            conn->beginTransaction();

            // 执行交易逻辑
            updateBalance(trade.accountId, trade.amount, conn);
            createOrder(trade, conn);
            updatePosition(trade, conn);

            // 提交事务
            conn->commitTransaction();
        } catch (const std::exception& e) {
            // 回滚事务
            conn->rollbackTransaction();
            throw;
        }
    }

private:
    ConnectionPool connectionPool;
};
```

```bash
# 审查上述代码
claude /database-interaction-optimization src/trading_database.cpp --dimension transaction-management
```

## 示例 7: 批量审查

```bash
# 审查整个项目的数据库相关代码
claude /database-interaction-optimization src/ --recursive

# 指定文件模式
claude /database-interaction-optimization src/ --pattern "*_db.cpp"
```

## 示例 8: 集成到CI/CD

```yaml
# GitHub Actions 示例
name: Database Interaction Review

on: [push, pull_request]

jobs:
  database-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Run Database Interaction Review
      run: |
        claude /database-interaction-optimization src/database/ --output json > review.json

    - name: Check Results
      run: |
        if jq '.results.connection_pool.status == "fail"' review.json; then
          echo "连接池配置问题需要修复"
          exit 1
        fi
```

## 示例 9: 定期审查脚本

```bash
#!/bin/bash
# 定期数据库性能审查脚本

DATE=$(date +%Y-%m-%d)
REPORT_DIR="reports/database-reviews"
mkdir -p $REPORT_DIR

# 执行审查
claude /database-interaction-optimization src/database/ \
  --output json \
  --verbose \
  > $REPORT_DIR/review-$DATE.json

# 生成报告摘要
jq '.results | to_entries | map({
  dimension: .key,
  status: .value.status,
  issues: .value.issues
})' $REPORT_DIR/review-$DATE.json > $REPORT_DIR/summary-$DATE.json

echo "数据库审查报告已生成: $REPORT_DIR/review-$DATE.json"
```

## 示例 10: 问题定位和修复

```bash
# 查找慢查询
claude /database-interaction-optimization src/database/ --dimension query-optimization

# 根据报告修复问题
# 1. 添加缺失的索引
# 2. 优化SQL语句
# 3. 实施缓存策略
# 4. 调整连接池配置

# 验证修复效果
claude /database-interaction-optimization src/database/ --dimension query-optimization
```

## 常见使用场景

### 1. 新功能开发
```bash
# 开发新功能时审查数据库交互
claude /database-interaction-optimization src/features/new_trading_system/
```

### 2. 性能优化
```bash
# 系统性能下降时进行数据库审查
claude /database-interaction-optimization src/ --dimension query-optimization,caching
```

### 3. 代码审查
```bash
# PR审查时检查数据库交互质量
claude /database-interaction-optimization pr-changes/
```

### 4. 系统上线前
```bash
# 部署前全面审查
claude /database-interaction-optimization src/ --verbose --output json
```

### 5. 定期维护
```bash
# 每周定期审查数据库性能
0 2 * * 1 claude /database-interaction-optimization src/ --output json > /tmp/weekly-review.json
```

## 输出示例

### Markdown输出示例
```markdown
# 数据库交互优化审查报告

## 概述
- 系统名称: trading-system
- 审查时间: 2026-04-09 10:30:00
- 数据库类型: MySQL
- 审查范围: 全部维度

## 审查结果

### 连接池管理 (⚠️ 警告)
- ✅ 连接池基本配置合理
- ⚠️ 连接超时设置偏短 (当前: 5秒，建议: 30秒)
- ⚠️ 部分查询存在连接泄漏风险
- 建议：优化连接释放机制，调整超时配置

### 查询优化 (❌ 失败)
- ❌ 存在慢查询 (响应时间 > 100ms)
- ❌ 部分查询未使用索引
- ⚠️ 发现N+1查询问题
- 建议：优化SQL语句，添加缺失索引，重构循环查询

### 事务管理 (✅ 通过)
- ✅ 事务边界设置合理
- ✅ 隔离级别配置正确
- ✅ 死锁处理机制完善
- ✅ 事务时间控制在合理范围

### 缓存策略 (⚠️ 警告)
- ⚠️ 缓存命中率较低 (65%)
- ⚠️ 缓存失效策略可优化
- ✅ 缓存击穿防护完善
- 建议：调整缓存策略，增加热点数据缓存

## 优先级建议
1. **高优先级**: 修复慢查询和N+1查询问题
2. **中优先级**: 优化缓存策略提升命中率
3. **低优先级**: 调整连接池配置参数
```

### JSON输出示例
```json
{
  "system": "trading-system",
  "review_time": "2026-04-09 10:30:00",
  "database_type": "MySQL",
  "overall_status": "warning",
  "results": {
    "connection_pool": {
      "status": "warning",
      "score": 75,
      "issues": [
        "连接超时设置偏短",
        "部分查询存在连接泄漏风险"
      ],
      "recommendations": [
        "优化连接释放机制",
        "调整超时配置到30秒"
      ]
    },
    "query_optimization": {
      "status": "fail",
      "score": 55,
      "issues": [
        "存在慢查询 (响应时间 > 100ms)",
        "部分查询未使用索引",
        "发现N+1查询问题"
      ],
      "recommendations": [
        "优化SQL语句",
        "添加缺失索引",
        "重构循环查询"
      ]
    },
    "transaction_management": {
      "status": "pass",
      "score": 95,
      "issues": [],
      "recommendations": []
    },
    "caching": {
      "status": "warning",
      "score": 70,
      "issues": [
        "缓存命中率较低 (65%)",
        "缓存失效策略可优化"
      ],
      "recommendations": [
        "调整缓存策略",
        "增加热点数据缓存"
      ]
    }
  },
  "overall_score": 73.75,
  "priority_recommendations": [
    {
      "priority": "high",
      "item": "修复慢查询和N+1查询问题"
    },
    {
      "priority": "medium",
      "item": "优化缓存策略提升命中率"
    },
    {
      "priority": "low",
      "item": "调整连接池配置参数"
    }
  ]
}
```

## 高级用法

### 自定义规则
```bash
# 使用自定义审查规则
claude /database-interaction-optimization src/database/ --rules custom-rules.json
```

### 性能基准对比
```bash
# 与性能基准对比
claude /database-interaction-optimization src/database/ --baseline performance-baseline.json
```

### 差异分析
```bash
# 对比两次审查结果的差异
claude /database-interaction-optimization src/database/ --compare previous-review.json
```

---

通过这些示例，您可以了解如何在不同场景下使用数据库交互优化技能，提升金融系统的数据库性能和稳定性。