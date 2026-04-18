# Claude Skills 与 Cursor Playbooks 速查表

## 按审查阶段选文档

| 审查阶段 | 先看什么 | 适用场景 |
| --- | --- | --- |
| 0. 快速分流 | `cursor_notes/review-cheat-sheet.md` | 还没判断问题属于代码质量、并发、性能、协议还是安全 |
| 1. 问题现象定位 | `cursor_notes/problem-symptom-navigation.md` | 已经看到现象，但还没决定该选哪个 Skill / Playbook / Prompt |
| 2. 通用审查起步 | `cursor_notes/playbooks/code-review.md` | 普通 PR、实现质量、边界条件、可维护性检查 |
| 3. C++ 服务端 PR 深入审查 | `cursor_notes/pr_review_templates/cpp-backend-general.md` | 需要按服务端视角补线程、协议、资源、可观测性检查 |
| 4. 并发 / 生命周期 / 性能 | `cursor_notes/playbooks/performance-latency-review.md` | 锁竞争、尾延迟、吞吐下降、shutdown、回调生命周期 |
| 5. 风险 / 数据 / 协议正确性 | `cursor_notes/playbooks/risk-data-protocol-review.md` | 协议字段、行情链路、风险计算、回测结果、数据一致性 |
| 6. 安全 / 合规 | `cursor_notes/playbooks/security-compliance-review.md` | 输入校验、权限、敏感信息、审计要求、合规约束 |
| 7. 云上交付 / 观测 / 运维 | `cursor_notes/playbooks/cloud-ops-review.md` | 配置、部署、指标、日志、trace、SLO、告警 |
| 8. 专项领域语义任务 | `cursor_notes/prompts/` | 需要 `ml-model-validation`、`defi-protocol-review`、`trading-simulation` 这类专项 Prompt |

### 最短路径

- 不确定从哪开始：先看 `review-cheat-sheet.md`
- 已知问题现象：再看 `problem-symptom-navigation.md`
- 已经知道审查方向：直接进入对应 `playbooks/`
- 如果是 C++ 服务端 PR：补对应 `pr_review_templates/`
- 如果是强领域语义任务：最后才进入 `prompts/`

## 核心 Skill 对应哪些 Cursor Playbooks

| Claude Skill | 推荐 Cursor 资产 |
| --- | --- |
| `code-review` | `cursor_notes/playbooks/code-review.md` |
| `cpp-backend-review` | `cursor_notes/playbooks/code-review.md` + `cursor_notes/pr_review_templates/cpp-backend-general.md` |
| `concurrency-review` | `cursor_notes/playbooks/performance-latency-review.md` + `cursor_notes/pr_review_templates/cpp-backend-concurrency.md` |
| `memory-management-review` | `cursor_notes/playbooks/performance-latency-review.md` |
| `performance-bottleneck-identification` | `cursor_notes/playbooks/performance-latency-review.md` |
| `security-review` | `cursor_notes/playbooks/security-compliance-review.md` |
| `protocol-validate` | `cursor_notes/playbooks/risk-data-protocol-review.md` |

## 按问题类型怎么选

### 普通 PR

- `code-review`
- `cpp-backend-review`

### 并发 / shutdown / 生命周期

- `concurrency-review`
- `memory-management-review`
- `cpp-backend-concurrency.md`

### 性能 / 尾延迟

- `performance-bottleneck-identification`
- `finance-optim`
- `cpp-backend-performance.md`

### 安全 / 合规 / 协议

- `security-review`
- `compliance-review`
- `protocol-validate`
- `cpp-backend-security-protocol.md`

### 风险 / 行情 / 回测

- `risk-check`
- `market-data-validate`
- `quant-backtest`
- `trading-simulation`
## 迁移映射表（Claude Skill → Cursor 资产）

| Claude Skills 主题组 | Cursor 承接方式 |
| --- | --- |
| `code-review` + `cpp-backend-review` | `AGENTS.md` + `.cursor/rules/00-general.mdc` + `playbooks/code-review.md` |
| `concurrency-review` + `memory-management-review` | `.cursor/rules/20-concurrency.mdc` + `playbooks/performance-latency-review.md` |
| 性能组（`finance-optim` / `network-optimization` / `database-interaction-optimization` / `performance-bottleneck-identification` / `low-latency-systems-design`） | `.cursor/rules/30-performance.mdc` + `.cursor/rules/50-rpc-network.mdc` + `playbooks/performance-latency-review.md` |
| `security-review` + `compliance-review` | `.cursor/rules/25-security-compliance.mdc` + `playbooks/security-compliance-review.md` |
| 风险数据组（`risk-check` / `quant-backtest` / `market-data-validate` / `protocol-validate` / `api-design-review`） | `.cursor/rules/35-risk-data-protocols.mdc` + `playbooks/risk-data-protocol-review.md` |
| `cloud-native-review` + `system-monitoring` + `trading-simulation` | `.cursor/rules/60-observability.mdc` + `playbooks/cloud-ops-review.md` |
| `ml-model-validate` | `prompts/ml-model-validation.md` |
| `defi-protocol-review` | `prompts/defi-protocol-review.md` |

## 迁移原则

- **稳定约束** → `.cursor/rules/`
- **审查流程** → `playbooks/`
- **专项低频任务** → `prompts/`