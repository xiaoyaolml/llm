# finance-optim

## 定位

金融性能优化 Skill，聚焦定价、撮合、风控和行情处理等金融路径中的延迟、吞吐与稳定性优化。

## 适用场景

- 高频路径优化
- 金融计算链路性能审查
- 市场数据、定价、撮合路径优化

## 核心关注点

- 热路径分配、拷贝、格式化
- 数值计算与数据布局成本
- 队列、批处理、缓存和锁争用
- 低延迟预算与验证方式

## 目录说明

- `SKILL.md`：技能定义与输出要求
- `RULES.md`：金融性能规则摘要
- `CHECKLISTS/`：金融 C++ 检查清单
- `EXAMPLES/`：性能示例占位目录
- `TEMPLATES/`：报告模板

## 协作边界

- 瓶颈未明确时先用 `performance-bottleneck-identification`
- 网络热点可联动 `network-optimization`
- 数据库热点可联动 `database-interaction-optimization`
