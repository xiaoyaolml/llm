# concurrency-review

## 定位

并发专项审查 Skill，聚焦共享状态、锁顺序、回调生命周期、原子与内存序、shutdown 收敛和背压风险。

## 适用场景

- 改动涉及 `thread`、`mutex`、`atomic`、队列、线程池
- 改动涉及超时、取消、关闭连接、资源回收
- 排查竞态、死锁、卡顿或任务堆积

## 核心关注点

- 数据竞争与锁顺序
- 回调与对象生命周期
- 原子与内存序
- 队列、背压、积压和 shutdown 行为
- ABA、use-after-free、re-entrancy 风险

## 目录说明

- `SKILL.md`：技能定义与输出要求
- `CHECKLISTS/`：并发核对项占位目录
- `EXAMPLES/`：并发问题示例占位目录
- `TEMPLATES/`：并发审查模板占位目录

## 协作边界

- 通用服务端问题可先由 `cpp-backend-review` 处理
- 内存回收细节可与 `memory-management-review` 联动
