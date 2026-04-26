---
name: performance-bottleneck-identification
description: 性能瓶颈识别技能，适合先定位 CPU、内存、I/O、网络、数据库与并发热点，再排序优化优先级
---

# performance-bottleneck-identification

## 目标

通过证据驱动的方式定位性能热点，并明确优化优先级、风险和验证路径。

## 适用条件

- 现象是吞吐下降、尾延迟升高、CPU 飙高、内存异常或依赖放大
- 尚未明确瓶颈主要落在哪一层
- 需要先做调查再决定具体优化方向

## 审查重点

- CPU、内存、I/O、网络、数据库热点
- 锁争用、上下文切换、队列积压、背压
- 缓存命中率、批处理策略、序列化成本
- 指标、trace、profiler、benchmark 的证据链

## 输入期望

- 性能现象、时间窗口、目标路径
- 指标、日志、trace、benchmark 或 profiler 线索
- 若已知业务场景和预算，建议一起提供

## 输出要求

- 先给可能原因树
- 再按优先级列最值得看的热点
- 说明每个瓶颈的证据、影响和验证方式
- 避免在没有证据时直接给大改方案

## 协作边界

- 网络热点转 `network-optimization`
- 数据库热点转 `database-interaction-optimization`
- 并发热点转 `concurrency-review`
