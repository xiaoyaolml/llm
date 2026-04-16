# Claude Skills 到 Cursor 的迁移映射

## 迁移原则

不要把 Claude Skills 按目录 1:1 搬成 Cursor Rules。

推荐做法：

- 稳定、长期生效的约束 → `.cursor/rules/`
- 审查步骤与方法 → `cursor_notes/playbooks/`
- 专项、低频、强上下文任务 → `cursor_notes/prompts/`

## 当前 Claude Skills 分层

### 核心

- `code-review`
- `cpp-backend-review`
- `concurrency-review`
- `memory-management-review`
- `performance-bottleneck-identification`
- `security-review`
- `protocol-validate`

### 专项

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

### 按需

- `ml-model-validate`
- `defi-protocol-review`

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

## 当前是否需要继续更新 `cursor_notes`

需要，且本轮已补齐两类对齐内容：

1. **结构对齐**：Claude Skills 已明确分成“核心 / 专项 / 按需”，Cursor 侧的映射说明也应采用相同分层。
2. **风格对齐**：Claude Skills 的 `CHECKLISTS/`、`EXAMPLES/`、`TEMPLATES/` 已收敛为更短、更稳定的格式，Cursor 侧文档应继续保持“规则 / Playbook / Prompt 模板”三层结构，不再复制旧的 Claude CLI 风格内容。

后续如果继续更新 `cursor_notes`，优先建议补：

- 一个“核心 Skill 对应哪些 Cursor Playbooks”的速查表
- 一个“按问题类型选 Skill / 选 Playbook / 选 Prompt”的简图

当前已补充：

- `cursor_notes/skill-playbook-quick-reference.md`
