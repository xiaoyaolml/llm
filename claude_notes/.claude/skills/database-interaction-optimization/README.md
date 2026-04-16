# 数据库交互优化技能 (database-interaction-optimization)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/database-interaction-optimization/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行数据库交互优化审查
claude /database-interaction-optimization src/database/

# 指定审查维度
claude /database-interaction-optimization src/database/ --dimension query-optimization

# 输出JSON格式报告
claude /database-interaction-optimization src/database/ --output json

# 详细模式
claude /database-interaction-optimization src/database/ --verbose
```

## ✨ 核心功能

- **连接池管理**：验证连接获取释放机制、优化池大小配置
- **查询优化**：分析慢查询、建议缺失索引、解决N+1问题
- **事务管理**：检查事务边界、验证隔离级别、评估回滚机制
- **缓存策略**：评估缓存命中率、验证缓存一致性机制
- **索引优化**：分析查询计划、推荐复合索引、检查索引碎片
- **读写分离**：验证主从同步机制、检查负载均衡策略
- **数据分片**：评估分片策略合理性、优化跨分片查询

## 📚 使用示例

### 1. 查询性能优化
```bash
# 识别和优化慢查询
claude /database-interaction-optimization src/queries/ --dimension query-optimization --verbose
```

### 2. 连接池配置优化
```bash
# 优化高频交易系统的连接池参数
claude /database-interaction-optimization src/pool/ --dimension connection-pool --system-type hft
```

### 3. 缓存策略评估
```bash
# 评估热点数据缓存有效性
claude /database-interaction-optimization src/cache/ --dimension caching --target hot-data
```

## 🔌 集成指南

### CI/CD 集成
```yaml
- name: Database Interaction Review
  run: claude /database-interaction-optimization src/ --output json > db_report.json
```

### 预提交钩子
```bash
#!/bin/sh
claude /database-interaction-optimization --high-priority-only || exit 1
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含数据库交互核心优化功能