# performance-bottleneck-identification

## 定位

性能瓶颈识别 Skill，负责跨 CPU、内存、I/O、网络、数据库、并发与缓存维度定位热点和排序优化优先级。

## 适用场景

- 性能退化、吞吐下降、尾延迟升高
- 不确定瓶颈究竟在代码、系统还是依赖侧
- 需要先做证据驱动的问题定位

## 核心关注点

- 热点证据而非主观猜测
- CPU、内存、I/O、网络、数据库的放大链路
- 锁争用、队列积压、缓存失配
- 优化收益、风险与验证顺序

## 目录说明

- `SKILL.md`：技能定义与输出要求
- `CHECKLISTS/`：瓶颈识别清单
- `EXAMPLES/`：示例
- `TEMPLATES/`：调查模板占位目录

## 协作边界

- 若瓶颈已明确在并发路径，可转 `concurrency-review`
- 若瓶颈已明确在网络层，可转 `network-optimization`
- 若瓶颈已明确在数据库侧，可转 `database-interaction-optimization`
