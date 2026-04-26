# Claude Skills 精简版导航手册

## 一句话用法

如果不知道该用哪个 Skill：

1. 先用 `code-review`
2. C++ 服务端问题再用 `cpp-backend-review`
3. 并发问题用 `concurrency-review`
4. 性能热点先用 `performance-bottleneck-identification`
5. 安全问题用 `security-review`
6. 协议问题用 `protocol-validate`

## 按问题类型找 Skill

### 代码质量与服务端结构

- `code-review`
- `cpp-backend-review`

### 并发、内存、性能

- `concurrency-review`
- `memory-management-review`
- `performance-bottleneck-identification`
- `finance-optim`
- `network-optimization`
- `database-interaction-optimization`
- `low-latency-systems-design`

### 安全、合规、接口

- `security-review`
- `compliance-review`
- `protocol-validate`
- `api-design-review`

### 风险、数据、策略验证

- `risk-check`
- `market-data-validate`
- `quant-backtest`
- `trading-simulation`

### 平台与运维

- `system-monitoring`
- `cloud-native-review`

### 强专项

- `ml-model-validate`
- `defi-protocol-review`

## 最推荐的 5 条路径

### 路径 1：普通 PR 审查

- `code-review`
- `cpp-backend-review`

### 路径 2：并发 / 崩溃 / shutdown 问题

- `cpp-backend-review`
- `concurrency-review`
- `memory-management-review`

### 路径 3：性能 / 尾延迟问题

- `performance-bottleneck-identification`
- `finance-optim`
- `network-optimization` / `database-interaction-optimization`

### 路径 4：安全 / 协议变更

- `security-review`
- `protocol-validate`
- `api-design-review`

### 路径 5：风险 / 行情 / 回测问题

- `risk-check`
- `market-data-validate`
- `quant-backtest` / `trading-simulation`

## 维护建议

- 核心 Skill 优先保持高质量
- 专项 Skill 以清单和示例为主，不重复扩写安装说明
- 按需 Skill 只在真实场景下补充
