# database-interaction-optimization

## 定位

数据库交互优化 Skill，聚焦查询模式、索引使用、连接池、批处理、事务边界和依赖放大问题。

## 适用场景

- 慢查询、锁等待、连接池饱和
- 写放大、批处理不合理、事务过大
- 服务端关键路径涉及数据库交互

## 核心关注点

- 查询与索引是否匹配
- 事务与锁等待成本
- 连接池与重试策略
- 批量读写、缓存和数据访问放大

## 目录说明

- `SKILL.md`：技能定义与输出要求
- `CHECKLISTS/`：数据库交互清单
- `EXAMPLES/`：示例
- `TEMPLATES/`：模板占位目录

## 协作边界

- 通用瓶颈定位可联动 `performance-bottleneck-identification`
- 数据质量和语义问题可联动 `risk-check` 或 `market-data-validate`
