# Claude Skills 与 Cursor Playbooks 速查表

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
