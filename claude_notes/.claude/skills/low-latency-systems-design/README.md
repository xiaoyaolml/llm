# low-latency-systems-design

## 定位

低延迟系统设计 Skill，聚焦延迟预算、线程模型、队列结构、缓存与调度策略，适合设计级评审。

## 适用场景

- 新系统或新模块做低延迟设计评审
- 需要明确预算、关键路径和约束
- 需要比较多种架构方案

## 核心关注点

- 关键路径预算分解
- 线程与队列模型
- 缓存局部性和批处理策略
- 超时、背压、可观测性与退化设计

## 目录说明

- `SKILL.md`：技能定义与输出要求
- `CHECKLISTS/`：延迟核对清单
- `EXAMPLES/`：示例
- `TEMPLATES/`：设计模板占位目录

## 协作边界

- 已落到具体热点定位后，可转 `performance-bottleneck-identification`
- 已落到具体网络链路问题后，可转 `network-optimization`
