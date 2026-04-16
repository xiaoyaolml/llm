# 按问题现象选 Claude Skill / Cursor Playbook / Prompt

## 单页导航图

```mermaid
flowchart LR
  classDef symptom fill:#E8F1FF,stroke:#4C78A8,stroke-width:1px
  classDef skill fill:#EAF7EA,stroke:#2E8B57,stroke-width:1px
  classDef asset fill:#FFF4E5,stroke:#D97706,stroke-width:1px

  subgraph Symptoms["问题现象"]
    S1["普通 PR / 可读性 / 边界条件"]
    S2["并发 / 生命周期 / 退出卡住"]
    S3["延迟抖动 / 吞吐下降 / CPU 异常"]
    S4["协议 / 数据一致性 / 风险结果异常"]
    S5["安全 / 合规 / 敏感数据暴露"]
    S6["运维 / 观测 / 云上交付问题"]
    S7["专项语义任务"]
  end

  subgraph Skills["Claude Skill"]
    K1["code-review"]
    K2["cpp-backend-review"]
    K3["concurrency-review"]
    K4["memory-management-review"]
    K5["performance-bottleneck-identification"]
    K6["finance-optim"]
    K7["protocol-validate"]
    K8["risk-check"]
    K9["security-review"]
    K10["compliance-review"]
    K11["cloud-native-review"]
    K12["system-monitoring"]
    K13["ml-model-validate"]
    K14["defi-protocol-review"]
    K15["trading-simulation"]
  end

  subgraph Assets["Cursor Playbook / Prompt / 模板"]
    A1["playbooks/code-review.md"]
    A2["playbooks/performance-latency-review.md"]
    A3["playbooks/risk-data-protocol-review.md"]
    A4["playbooks/security-compliance-review.md"]
    A5["playbooks/cloud-ops-review.md"]
    A6["pr_review_templates/cpp-backend-general.md"]
    A7["pr_review_templates/cpp-backend-concurrency.md"]
    A8["pr_review_templates/cpp-backend-performance.md"]
    A9["pr_review_templates/cpp-backend-security-protocol.md"]
    A10["prompts/ml-model-validation.md"]
    A11["prompts/defi-protocol-review.md"]
    A12["prompts/trading-simulation-review.md"]
  end

  S1 --> K1
  S1 --> K2
  S2 --> K3
  S2 --> K4
  S3 --> K5
  S3 --> K6
  S4 --> K7
  S4 --> K8
  S5 --> K9
  S5 --> K10
  S6 --> K11
  S6 --> K12
  S7 --> K13
  S7 --> K14
  S7 --> K15

  K1 --> A1
  K1 --> A6
  K2 --> A1
  K2 --> A6
  K3 --> A2
  K3 --> A7
  K4 --> A2
  K5 --> A2
  K5 --> A8
  K6 --> A2
  K6 --> A8
  K7 --> A3
  K7 --> A9
  K8 --> A3
  K8 --> A9
  K9 --> A4
  K9 --> A9
  K10 --> A4
  K11 --> A5
  K12 --> A5
  K13 --> A10
  K14 --> A11
  K15 --> A12

  class S1,S2,S3,S4,S5,S6,S7 symptom
  class K1,K2,K3,K4,K5,K6,K7,K8,K9,K10,K11,K12,K13,K14,K15 skill
  class A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12 asset
```

## 最短使用法

- 先按“问题现象”选一组 `Claude Skill`。
- 再落到对应的 `Cursor Playbook`。
- 如果是 C++ 服务端 PR，再补对应 `pr_review_templates/`。
- 只有遇到强领域语义任务时，才进入 `prompts/`。

## 一句话规则

- 普通代码问题：先 `code-review`
- 并发与性能：优先 `performance-latency-review.md`
- 协议与风险：优先 `risk-data-protocol-review.md`
- 安全与合规：优先 `security-compliance-review.md`
- 云上交付与观测：优先 `cloud-ops-review.md`
