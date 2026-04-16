---
name: cpp-backend-review
description: C++ 服务端专项审查技能，聚焦所有权、生命周期、接口边界、并发模型和热路径行为
---

# cpp-backend-review

## 目标

对 C++ 服务端代码做系统级审查，优先识别生产风险、线程安全风险、资源管理问题和接口边界问题。

## 适用条件

- 目标代码以 C++ 服务端为主
- 涉及网络、RPC、线程池、配置、日志、存储等模块
- 需要做通用 C++ 后端评审而非单点专项排查

## 审查重点

- 所有权、RAII、析构与 shutdown 顺序
- 模块分层、接口稳定性、依赖方向
- 错误传播、日志字段、诊断能力
- 热路径上的拷贝、分配、锁和阻塞风险
- 并发路径中的共享状态和回调生命周期

## 输入期望

- 变更范围、模块说明、运行环境
- 是否涉及 public API、协议、配置或线程模型
- 若有性能目标或线上故障背景，可一并提供

## 输出要求

- 先给整体风险概览
- 再给模块级问题和修复建议
- 区分 correctness / concurrency / performance / operability
- 给出验证建议和回归测试建议

## 协作边界

- 无锁、原子、竞态细节转 `concurrency-review`
- 泄漏、悬垂、分配器策略转 `memory-management-review`
- 瓶颈定位和优化优先级转 `performance-bottleneck-identification`
