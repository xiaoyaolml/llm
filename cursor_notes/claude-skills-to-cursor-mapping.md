# Claude Skills 到 Cursor 的迁移映射

## 迁移原则

不要把 Claude Skills 按目录 1:1 搬成 Cursor Rules。

推荐做法：

- 稳定、长期生效的约束 → `.cursor/rules/`
- 审查步骤与方法 → `cursor_notes/playbooks/`
- 专项、低频、强上下文任务 → `cursor_notes/prompts/`

## 映射表

| Claude Skills 主题组 | Cursor 承接方式 |
| --- | --- |
| `code-review` + `cpp-backend-review` | `AGENTS.md` + `.cursor/rules/00-general.mdc` + `cursor_notes/playbooks/code-review.md` |
| `concurrency-review` + `memory-management-review` | `.cursor/rules/20-concurrency.mdc` + `cursor_notes/playbooks/performance-latency-review.md` |
| `finance-optim` + `network-optimization` + `database-interaction-optimization` + `performance-bottleneck-identification` + `low-latency-systems-design` | `.cursor/rules/30-performance.mdc` + `.cursor/rules/50-rpc-network.mdc` + `cursor_notes/playbooks/performance-latency-review.md` |
| `security-review` + `compliance-review` | `.cursor/rules/25-security-compliance.mdc` + `cursor_notes/playbooks/security-compliance-review.md` |
| `risk-check` + `quant-backtest` + `market-data-validate` + `protocol-validate` + `api-design-review` | `.cursor/rules/35-risk-data-protocols.mdc` + `cursor_notes/playbooks/risk-data-protocol-review.md` |
| `cloud-native-review` + `system-monitoring` + `trading-simulation` | `.cursor/rules/60-observability.mdc` + `cursor_notes/playbooks/cloud-ops-review.md` + `cursor_notes/prompts/trading-simulation-review.md` |
| `ml-model-validate` | `cursor_notes/prompts/ml-model-validation.md` |
| `defi-protocol-review` | `cursor_notes/prompts/defi-protocol-review.md` |

## 使用建议

### Rules 适合什么

- 线程安全
- 性能敏感路径
- 协议兼容性
- 安全 / 合规要求
- 可观测性要求

### Playbooks 适合什么

- 代码审查流程
- 性能排障流程
- 安全排障流程
- 风险与数据核对流程
- 运维与交付审查流程

### Prompt 模板适合什么

- 特定领域深度评审
- 强业务语义任务
- 并非每个仓库都常用的专项任务
