---
name: system-monitoring
description: 系统监控技能，聚焦指标、日志、trace、告警、SLO 和 runbook 的完备性
---

# system-monitoring

## 目标

评估系统是否具备足够的可观测性和运维诊断能力，以支持上线、排障和持续改进。

## 适用条件

- 涉及指标、日志、trace、告警、仪表板或 runbook
- 准备上线、扩容、迁移或做生产问题复盘
- 需要评估系统是否易于诊断和运维

## 审查重点

- 延迟、错误率、饱和度、队列深度、依赖健康等核心指标
- request_id、connection_id、error_code 等关键日志字段
- trace 链路、告警阈值和仪表板可读性
- runbook、容量、演练与回滚信息

## 输入期望

- 服务角色、关键 SLO、已有观测手段
- 近期故障或痛点
- 若有仪表板、告警定义、日志样例可一起提供

## 输出要求

- 先评估可观测性覆盖面
- 再指出关键缺口和优先级
- 说明哪些问题会直接影响排障效率或上线风险
- 给出最小改进集和验证方式

## 协作边界

- 部署和平台层问题可联动 `cloud-native-review`
- 特定性能问题可联动 `performance-bottleneck-identification`
