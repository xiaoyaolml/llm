# Claude Skills 分层索引

## 核心

这些 Skill 最适合高频使用，也是当前工程最值得优先维护的一层。

- `code-review`
- `cpp-backend-review`
- `concurrency-review`
- `memory-management-review`
- `performance-bottleneck-identification`
- `security-review`
- `protocol-validate`

## 专项

这些 Skill 面向明确主题或明确场景，适合在核心审查之后继续深入。

- `finance-optim`
- `network-optimization`
- `database-interaction-optimization`
- `low-latency-systems-design`
- `risk-check`
- `market-data-validate`
- `api-design-review`
- `compliance-review`
- `system-monitoring`
- `cloud-native-review`
- `quant-backtest`
- `trading-simulation`

## 按需

这些 Skill 具备较强领域前提，不建议默认进入常规工作流。

- `ml-model-validate`
- `defi-protocol-review`

## 使用建议

- **先核心**：先判断是否有 correctness / security / concurrency / protocol 级阻塞问题
- **再专项**：根据性能、数据、监控、合规等主题继续深入
- **按需补充**：只有仓库确实涉及 ML 或 DeFi 时再使用对应 Skill
