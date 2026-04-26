# cpp-backend-review

## 定位

C++ 服务端专项审查 Skill，聚焦所有权、生命周期、接口设计、并发模型、资源管理和热路径行为。

## 适用场景

- 审查 C++ 服务端模块或 PR
- 审查网络、RPC、线程池、配置、日志、存储路径
- 做服务端整体质量评估

## 核心关注点

- 所有权与 RAII
- API 与模块边界
- 错误处理、日志与可观测性
- 线程安全与 shutdown 行为
- 热路径分配、拷贝和锁使用

## 目录说明

- `SKILL.md`：技能定义与输出要求
- `CHECKLISTS/`：通用规则、线程安全、内存管理清单
- `EXAMPLES/`：服务端使用示例
- `TEMPLATES/`：模板占位目录

## 协作边界

- 深度并发问题交给 `concurrency-review`
- 深度内存问题交给 `memory-management-review`
- 性能定位交给 `performance-bottleneck-identification`
