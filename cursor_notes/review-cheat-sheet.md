# 审查速记版

## 30 秒选择法

### 1. 先判断问题属于哪一类

- 普通 PR / 代码质量 → `code-review`
- C++ 服务端实现质量 → `cpp-backend-review`
- 并发 / 生命周期 / shutdown → `concurrency-review`
- 内存 / 所有权 / 泄漏 → `memory-management-review`
- 延迟 / 吞吐 / 热点 → `performance-bottleneck-identification`
- 协议 / 风险 / 数据正确性 → `protocol-validate` / `risk-check`
- 安全 / 合规 → `security-review` / `compliance-review`
- 云上运维 / 可观测性 → `cloud-native-review` / `system-monitoring`

### 2. 再选 Cursor 资产

- 普通审查 → `playbooks/code-review.md`
- 并发 / 性能 → `playbooks/performance-latency-review.md`
- 协议 / 风险 / 数据 → `playbooks/risk-data-protocol-review.md`
- 安全 / 合规 → `playbooks/security-compliance-review.md`
- 交付 / 云 / 观测 → `playbooks/cloud-ops-review.md`

### 3. 如果是 C++ 服务端 PR，再补模板

- 通用 → `pr_review_templates/cpp-backend-general.md`
- 并发 → `pr_review_templates/cpp-backend-concurrency.md`
- 性能 → `pr_review_templates/cpp-backend-performance.md`
- 安全 / 协议 → `pr_review_templates/cpp-backend-security-protocol.md`

## 高频阻塞项

- 是否引入线程安全问题
- 是否破坏 shutdown / 生命周期顺序
- 是否在热路径增加分配、锁竞争或日志开销
- 是否改变协议兼容性、边界条件或错误码语义
- 是否缺少测试、基准或回归验证
- 是否缺少日志、指标、trace 上下文

## 高频输出格式

### 阻塞问题

- 现象
- 位置
- 风险
- 为什么会出错
- 最小修复建议
- 验证方式

### 非阻塞建议

- 可读性
- 可维护性
- 可观测性
- 测试补强
- 性能验证补强

## 一句话决策

- 不确定从哪里开始：先看 `code-review`
- 看到锁、原子、回调、析构：切到并发审查
- 看到尾延迟、CPU、分配、拷贝：切到性能审查
- 看到协议字段、行情、风控结果：切到风险/协议审查
- 看到权限、输入校验、敏感信息：切到安全/合规审查
