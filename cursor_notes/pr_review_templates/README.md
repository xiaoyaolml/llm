# C++ 服务端 PR 审查模板

本目录把已有 Playbooks 进一步细化成可直接用于 PR 审查的模板。

## 模板列表

- `cpp-backend-general.md`：通用 C++ 服务端 PR 审查
- `cpp-backend-concurrency.md`：并发 / 生命周期专项审查
- `cpp-backend-performance.md`：性能 / 低延迟专项审查
- `cpp-backend-security-protocol.md`：安全 / 协议 / 风险专项审查

## 推荐流程

1. 先运行 `Review: Export PR snapshot`
2. 根据 PR 类型生成对应 Prompt
3. 按本目录模板逐项审查
4. 产出阻塞项 / 建议项 / 可后续治理项
