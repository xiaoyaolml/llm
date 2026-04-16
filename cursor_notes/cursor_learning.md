# Cursor 学习指南：从零开始到深入掌握

> 目标：用最短时间学会 Cursor，并把它打造成 **C++ 服务器后端开发** 的高效工作台。

---

## 1. Cursor 是什么

Cursor 可以理解为“带强 AI Agent 能力的代码编辑器”。它不只是代码补全工具，而是一个能够：

- 理解整个代码库
- 搜索相关实现
- 修改多个文件
- 运行终端命令
- 帮你规划任务
- 帮你检查和回滚改动

对于 C++ 服务器后端开发，Cursor 最有价值的地方在于：

- **快速理解大型工程**：网络层、协议层、业务层、存储层之间的关系更容易理清。
- **跨文件修改**：接口、实现、测试、配置常常一起改，Agent 很适合处理这类任务。
- **重构和性能优化辅助**：适合做线程模型梳理、锁竞争分析、内存分配路径排查。
- **文档生成**：可以自动整理设计说明、时序图、接口说明、性能分析报告。

---

## 2. 先建立正确认知

很多人第一次用 Cursor，会把它当成“更强的自动补全”。这是低配用法。

更正确的理解是：

1. **Tab**：负责加速“局部编码”。
2. **Agent**：负责“理解、规划、修改、验证”。
3. **Plan Mode**：负责“复杂需求先出方案，再执行”。
4. **Rules / AGENTS.md**：负责“长期记住你的项目规范和开发习惯”。

如果你只会补全，就只能提升 20%。
如果你会让 Agent 帮你理解工程、改代码、跑命令、验证结果，效率能提升一个量级。

---

## 3. 安装与基础准备

### 3.1 安装流程

1. 下载并安装 Cursor。
2. 登录账号。
3. 打开你的项目目录。
4. 等待代码库索引完成。

首次打开大型 C++ 项目时，不要立刻让 Agent 改代码，建议先做三件事：

- 让它解释工程结构
- 让它找入口文件
- 让它梳理编译与运行方式

建议第一句就这样问：

```text
请先不要修改代码。先分析这个仓库：
1. 入口程序在哪里
2. 核心模块怎么分层
3. 网络、线程、存储、协议相关代码分别在哪
4. 构建、测试、运行方式是什么
5. 给我一个建议阅读顺序
```

这一步非常关键。官方文档的核心思路也是：**先理解，再改动**。

---

## 4. Cursor 的核心能力

## 4.1 Agent

Agent 是 Cursor 的核心。它可以：

- 搜索代码库
- 读取文件
- 修改文件
- 运行终端命令
- 分步骤完成任务
- 自动建立 checkpoint 方便回退

适合场景：

- 新功能开发
- bug 排查
- 重构
- 补测试
- 性能分析辅助
- 文档生成

### 推荐使用方式

不要直接说：

```text
帮我优化一下
```

要改成：

```text
先分析当前实现的瓶颈来源，不要立即修改。
请给出：
1. 热路径
2. 可能的锁竞争点
3. 多余拷贝位置
4. 内存分配热点
5. 三种优化方案及风险
```

这样 Agent 的输出会稳定很多。

---

## 4.2 Plan Mode

Plan Mode 适合复杂需求。官方推荐：**跨多个文件、需求不够明确、需要先讨论方案** 时，优先使用 Plan Mode。

Plan Mode 的典型流程：

1. 先问你澄清问题
2. 自动搜索代码库
3. 产出实现计划
4. 你审阅计划
5. 确认后再开始编码

### 什么时候一定要用 Plan Mode

以下任务建议直接切换到 Plan Mode：

- 增加 RPC 框架能力
- 改线程池或协程调度器
- 引入连接池 / 内存池 / 对象池
- 改网络协议解析链路
- 改日志、监控、追踪体系
- 做模块拆分或目录重构

### 一个适合 C++ 后端的 Plan Prompt

```text
进入 Plan Mode。
目标：为当前 C++ 服务端项目增加“连接级限流 + 请求级超时取消”。

要求：
1. 先梳理现有连接管理、事件循环、请求处理链路
2. 不要立即写代码
3. 给出最小侵入方案
4. 明确会修改的文件、结构体、类、线程安全影响
5. 给出测试方案、压测方案、回滚方案
```

---

## 4.3 Tab 自动补全

Tab 是 Cursor 的 AI 补全能力，适合：

- 补全函数体
- 顺着上下文续写代码
- 批量补多行逻辑
- 自动补 import / include / 小型重构

官方帮助文档里的关键点：

- `Tab`：接受建议
- `Esc`：拒绝建议
- `Ctrl + →`：按词接受
- 接受一次建议后，再按一次 `Tab`，有时会跳到下一个最可能编辑的位置

### C++ 开发里的最佳用法

Tab 最适合以下内容：

- getter / setter
- RAII 封装样板
- switch / if 分支补全
- 单元测试样板
- 日志与错误码处理样板
- 模板代码的重复部分

### 不适合完全依赖 Tab 的场景

- 锁顺序设计
- 生命周期复杂的异步回调
- 高性能内存管理路径
- 无锁结构与原子内存序
- 跨线程资源释放

这些场景更适合先让 Agent 分析，再手工审阅。

---

## 4.4 Rules / AGENTS.md

Cursor 官方推荐用 Rules 来放“长期有效”的项目规范。

规则可放在：

- `.cursor/rules/`：项目规则，适合版本管理
- `AGENTS.md`：简单指令文件，适合直观维护

Rules 的价值是：让 Cursor 每次都更像你的团队成员，而不是陌生外包。

### 适合写进 Rules 的内容

- 目录分层约定
- 命名规范
- 错误码规范
- 日志字段规范
- 接口实现模板
- 测试要求
- 性能敏感点禁忌

### 不适合写进 Rules 的内容

- 巨长编码规范全文复制
- 很少出现的边角约定
- 已经能靠 clang-format / clang-tidy 解决的格式问题

官方建议也很明确：**规则要短、小、聚焦、可执行。**

---

## 5. 从零开始的学习路径

## 第 1 阶段：先会用

目标：能完成简单理解与简单改动。

你应该学会：

- 打开项目并等待索引
- 用 Agent 解释代码结构
- 用 Tab 补全代码
- 让 Agent 改 1~2 个小文件
- 审查 diff
- 用 checkpoint 回退错误改动

### 练习任务

1. 让 Cursor 解释仓库结构。
2. 让它找出程序入口。
3. 让它找出配置加载逻辑。
4. 让它新增一个简单日志字段。
5. 让它补一个基础测试。

---

## 第 2 阶段：学会和 Agent 协作

目标：不只是“让它写代码”，而是“让它按你的意图工作”。

你应该学会：

- 先分析再修改
- 一次只给一个明确目标
- 让它先列方案再执行
- 要求它说明风险
- 要求它给验证步骤

### 高质量提示词模板

```text
先不要修改代码。
请先研究当前实现，并按以下格式输出：
1. 当前实现流程
2. 关键类与关键函数
3. 依赖关系
4. 潜在风险
5. 最小修改方案
6. 需要新增的测试
```

---

## 第 3 阶段：学会做复杂任务

目标：会用 Plan Mode、Rules、测试驱动、性能分析提示词。

你应该学会：

- 对复杂需求先做 plan
- 对高风险改动分批执行
- 先写测试，再让 Agent 实现
- 用规则固定团队约束
- 用 Agent 产出设计文档和复盘文档

---

## 6. C++ 服务器后端开发的最佳实践

这一部分最重要。

如果你的目标是：**让 Cursor 对 C++ 服务器后端开发提供接近“完美”的体验**，核心原则是：

> 把工程上下文、编译系统、静态分析、调试入口、测试入口、规范约束，全部显式化。

也就是说，别让 AI 猜。

---

## 6.1 必备工具链

建议至少具备以下环境：

- CMake
- Ninja
- Clang 或 GCC / MSVC
- `clangd`
- `clang-format`
- `clang-tidy`
- GDB / LLDB / Visual Studio 调试器
- 单元测试框架（GoogleTest / Catch2）
- 基准测试框架（Google Benchmark）

### 为什么 `clangd` 很重要

对于 Cursor / VS Code 系编辑器来说，`clangd` 几乎是 C++ 智能体验的核心支柱之一。

它能提供：

- 跳转定义
- 引用查找
- 补全增强
- 类型推导
- 诊断
- 重构支持

AI 再强，也不应取代基础语言服务。

---

## 6.2 一定要生成 `compile_commands.json`

C++ 项目体验好坏，很多时候取决于 `compile_commands.json` 是否正确。

如果没有它，会出现：

- 头文件找不到
- 宏识别错误
- 条件编译不准确
- 智能跳转不稳定
- AI 读到的代码上下文不完整

### CMake 推荐配置

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

如果你用多配置生成器，也建议显式处理构建目录。

---

## 6.3 建议的 Cursor / Workspace 配置思路

### 推荐目标

让工程具备以下能力：

- 一键构建
- 一键测试
- 一键格式化
- 一键静态检查
- 一键调试
- 清晰的运行参数

如果这些能力都存在，Agent 就能更稳定地替你完成工作。

### 推荐目录与配置文件

一个偏理想的 C++ 服务端项目，建议至少有：

- `CMakeLists.txt`
- `CMakePresets.json`
- `.clang-format`
- `.clang-tidy`
- `.editorconfig`
- `tests/`
- `benchmarks/`
- `docs/`
- `.cursor/rules/`

---

## 6.4 建议的项目规则示例

可以考虑建立 `.cursor/rules/cpp_server_backend.mdc`，内容类似：

```md
---
description: "Rules for C++ server backend development"
alwaysApply: false
globs:
	- "**/*.h"
	- "**/*.hpp"
	- "**/*.cc"
	- "**/*.cpp"
---

- 先理解现有模式，再修改代码。
- 优先复用现有组件，不要重复发明新的工具类。
- 涉及并发、锁、原子、生命周期的修改时，必须先说明线程安全影响。
- 涉及性能路径时，必须说明额外拷贝、分配、锁竞争和缓存局部性影响。
- 新增功能优先补测试；若不补测试，必须解释原因。
- 错误处理必须明确，禁止吞错误。
- 日志应包含关键上下文字段，不要打印无结构的大段文本。
- 修改 public API 时，说明兼容性影响。
```

这个规则的好处是：Cursor 会逐渐更懂你在做的是“C++ 服务端工程”，而不是普通脚本工程。

---

## 6.5 推荐的 AGENTS.md

如果你不想一开始就维护太多 Rules，可以先放一个 `AGENTS.md`：

```md
# Project Instructions

## General

- This repository is a C++ server backend project.
- Prefer minimal, local, low-risk changes.
- Reuse existing patterns before introducing new abstractions.

## Concurrency

- Explain thread-safety impact before changing shared-state code.
- Be explicit about lock ordering, ownership, and lifetime.
- Call out memory ordering assumptions for atomics.

## Performance

- Avoid unnecessary heap allocations on hot paths.
- Avoid unnecessary string copies.
- Explain complexity and likely hotspots for performance-sensitive changes.

## Testing

- Add or update unit tests when behavior changes.
- If tests are not added, explain why.
```

---

## 6.6 给 C++ 后端最实用的提问模板

### 模板 1：理解服务框架

```text
请先不要修改代码。
解释这个 C++ 服务端工程的整体架构：
1. 入口在哪里
2. 配置如何加载
3. 网络事件如何进入业务处理
4. 线程模型是什么
5. 请求生命周期是什么
6. 日志、监控、错误处理在哪里
```

### 模板 2：做功能前先摸清上下文

```text
在实现新功能前，先帮我调研当前工程中：
1. 类似功能已有哪几处实现
2. 最值得复用的代码在哪里
3. 哪些模块不应该被耦合进去
4. 给出最小修改方案
```

### 模板 3：并发风险分析

```text
请分析这段代码的并发风险，不要先改代码。
重点关注：
1. 数据竞争
2. 锁顺序
3. 回调生命周期
4. 悬垂引用
5. 原子变量内存序是否合理
6. 可能的死锁与 ABA 风险
```

### 模板 4：性能优化

```text
请先分析这个模块的性能瓶颈，不要直接修改。
从以下角度说明：
1. 时间复杂度
2. 热路径
3. 内存分配
4. 拷贝次数
5. 锁竞争
6. cache locality
7. 给出收益最大、侵入最小的优化方案
```

### 模板 5：Bug 排查

```text
请帮我系统性排查这个问题：
现象：服务偶发超时，CPU 不高，但尾延迟升高。

要求：
1. 先列出可能原因树
2. 指出代码里最值得看的位置
3. 给出日志/指标/trace 排查顺序
4. 如需改代码，先给最小观测性增强方案
```

### 模板 6：补测试

```text
请为这个类补测试，但先不要改实现。
要求：
1. 按现有测试风格写
2. 先列出需要覆盖的行为
3. 先写失败测试
4. 不要引入不必要 mock
```

---

## 6.7 让 Agent 更稳的开发流程

这是非常适合 C++ 后端团队的工作流：

### 第一步：先问理解问题

先让 Cursor 讲清楚：

- 调用链
- 线程模型
- 所有权模型
- 错误路径
- 性能热点

### 第二步：进入 Plan Mode

让它输出：

- 修改点列表
- 风险点列表
- 测试列表
- 验证步骤
- 回滚方案

### 第三步：小步执行

一次只让它完成一类事情：

- 先改接口
- 再改实现
- 再补测试
- 再补文档

### 第四步：要求验证

让它明确做这些动作：

- 编译
- 单测
- 静态检查
- 格式化
- 必要时做 benchmark

### 第五步：你来做最终审查

重点审查：

- 是否破坏线程安全
- 是否引入额外拷贝
- 是否改坏异常 / 错误码语义
- 是否破坏 ABI / API
- 是否修改了不该动的公共路径

---

## 6.8 C++ 服务端最容易踩的坑

### 1. 让 AI 直接改并发代码

正确做法：先让它分析，再让它给方案，再小步改。

### 2. 没有构建入口和测试入口

如果工程没有统一的 build/test 命令，Agent 很难稳定验证结果。

### 3. 没有项目规则

没有规则时，AI 常会：

- 新造重复抽象
- 命名风格漂移
- 不按既有目录分层
- 忽视性能敏感路径

### 4. 提示词过于模糊

“优化一下”、“重构一下”、“看看哪里有问题” 这类说法过于宽泛。

### 5. 不看 diff

尤其是：

- 头文件改动
- 接口签名改动
- 模板代码改动
- 生命周期与资源管理改动

必须仔细看。

---

## 7. 推荐的日常使用方式

## 场景 1：看陌生项目

建议流程：

1. 先让 Cursor 解释架构。
2. 让它列出阅读顺序。
3. 让它画调用链或 Mermaid 图。
4. 再开始读关键文件。

## 场景 2：做小需求

建议流程：

1. 问清已有实现模式。
2. 让它给最小修改方案。
3. 直接执行。
4. 运行测试并看 diff。

## 场景 3：做复杂需求

建议流程：

1. 用 Plan Mode。
2. 把计划审一遍。
3. 拆成 3~5 个独立步骤。
4. 每一步都验证。

## 场景 4：查线上问题

建议流程：

1. 让它列问题原因树。
2. 让它指出最值得加日志和指标的点。
3. 先增强可观测性。
4. 再修问题。

---

## 8. 推荐你尽快建立的工程资产

如果你真想把 Cursor 用到“非常顺手”，建议尽快把下面这些资产补齐：

- `compile_commands.json`
- 统一构建脚本
- 统一测试脚本
- `.clang-format`
- `.clang-tidy`
- `AGENTS.md` 或 `.cursor/rules/`
- 单测目录
- benchmark 目录
- 常见排障文档
- 模块说明文档

这不是为了形式，而是为了让 AI 能长期稳定协作。

---

## 9. 一套非常实用的学习顺序

### 第 1 周

- 熟悉 Agent
- 熟悉 Tab
- 学会让它解释代码
- 学会让它做小修改

### 第 2 周

- 学会写高质量提示词
- 学会让它先分析后执行
- 学会审查 diff
- 学会回滚 checkpoint

### 第 3 周

- 学会 Plan Mode
- 学会补测试
- 学会让它生成设计文档
- 学会做问题排查

### 第 4 周

- 建立项目规则
- 建立 C++ 后端专用 prompt 模板
- 建立性能优化 / 并发分析工作流
- 把常用命令、目录、测试入口都固定下来

---

## 10. 一句话总结

如果只把 Cursor 当补全工具，你得到的是“更快写代码”。

如果把 Cursor 当成：

- 代码库理解助手
- 规划助手
- 改动执行助手
- 测试与验证助手
- 文档助手

你得到的是 **完整的 AI 协作开发流程**。

对于 C++ 服务器后端开发，最关键的不是“让它写更多”，而是：

> 让它在正确的工程上下文、规则约束、构建验证和性能意识下工作。

做到这一点，体验就会非常接近“完美”。

---

## 11. 建议你下一步就做的事

按优先级排序：

1. 给项目补 `compile_commands.json`
2. 建一个 `AGENTS.md`
3. 固定 build / test / format / tidy 命令
4. 让 Cursor 解释你的 C++ 项目结构
5. 选一个小需求试跑完整流程
6. 再开始用 Plan Mode 做稍大的任务

如果需要，下一步可以继续补一份：

- **适合 C++ 服务器项目的 `.cursor/rules` 完整模板**
- **适合 Cursor 的 `AGENTS.md` 完整模板**
- **CMake + clangd + clang-tidy + GoogleTest 的最佳工程配置模板**
- **面向网络服务 / RPC / 高性能并发场景的提示词模板合集**

---

## 12. C++ 服务端专用 `.cursor/rules` 实战模板

这一节给的是“可直接复制再微调”的版本。

建议你不要只放一个大而全规则，而是拆成多个小规则：

- 通用工程规则
- 并发与生命周期规则
- 性能路径规则
- 测试与验证规则
- RPC / 网络协议规则

这样更符合 Cursor 官方建议：**规则短、小、聚焦、按场景应用。**

---

### 12.1 规则目录建议

建议目录结构：

```text
.cursor/
	rules/
		00-general.mdc
		10-architecture.mdc
		20-concurrency.mdc
		30-performance.mdc
		40-testing.mdc
		50-rpc-network.mdc
		60-observability.mdc
```

---

### 12.2 通用工程规则：`00-general.mdc`

```md
---
description: "General rules for C++ backend development in this repository"
alwaysApply: true
---

- This repository is a C++ server backend project.
- Prefer minimal, local, low-risk changes.
- Understand existing patterns before introducing new abstractions.
- Reuse existing utilities, helpers, and infrastructure where possible.
- Do not rewrite large modules unless explicitly requested.
- When changing behavior, explain what changed and why.
- When a request is ambiguous, propose the smallest correct interpretation first.
- Keep naming, layering, and error-handling style consistent with nearby code.
- Avoid speculative refactors that are not required for the task.
- If a change touches public interfaces, call out compatibility impact.
```

适用目的：

- 让 Agent 默认保持“低风险、小步改动”
- 防止它动不动就重构一大片
- 防止它凭空发明新框架、新工具类

---

### 12.3 架构与分层规则：`10-architecture.mdc`

```md
---
description: "Architecture and layering constraints for service modules"
alwaysApply: false
globs:
	- "src/**/*.h"
	- "src/**/*.hpp"
	- "src/**/*.cc"
	- "src/**/*.cpp"
---

- Keep transport, protocol, business logic, and storage concerns separated.
- Do not mix request parsing code with business decision logic unless existing code already does so nearby.
- Prefer dependency injection through constructors or explicit interfaces over global mutable state.
- Keep cross-module dependencies one-directional when possible.
- Avoid adding new shared utility modules unless at least two real call sites justify them.
- When extending a flow, prefer following the existing request lifecycle instead of inserting side paths.
- If adding a new component, explain where it belongs in the current layering.
```

适用目的：

- 防止 Agent 把网络代码、业务代码、存储代码搅在一起
- 保持 C++ 服务端最重要的分层清晰度

---

### 12.4 并发与生命周期规则：`20-concurrency.mdc`

```md
---
description: "Concurrency, ownership, and lifetime rules for shared-state code"
alwaysApply: false
globs:
	- "**/*.h"
	- "**/*.hpp"
	- "**/*.cc"
	- "**/*.cpp"
---

- Before editing shared-state code, explain the thread-safety impact.
- Be explicit about ownership, lifetime, and destruction ordering.
- Call out whether code runs on an IO thread, worker thread, timer thread, or arbitrary thread.
- Avoid introducing detached threads unless explicitly requested.
- Avoid capturing raw pointers in async callbacks unless lifetime is clearly guaranteed.
- Prefer RAII and scoped cleanup over manual multi-branch cleanup.
- If locks are involved, explain lock ordering and potential deadlock risk.
- If atomics are involved, explain why the chosen memory ordering is sufficient.
- When modifying queues, pools, or callback registries, discuss shutdown behavior.
- Highlight any risk of race conditions, ABA, use-after-free, double-close, or re-entrancy.
```

适用目的：

- 这是 C++ 后端最值钱的一条规则
- 它能显著降低 Agent 在并发代码上“写得像对，其实危险”的概率

---

### 12.5 性能路径规则：`30-performance.mdc`

```md
---
description: "Performance-sensitive coding rules for hot paths"
alwaysApply: false
globs:
	- "**/*.h"
	- "**/*.hpp"
	- "**/*.cc"
	- "**/*.cpp"
---

- Treat request processing, serialization, parsing, scheduling, and logging on hot paths as performance-sensitive.
- Avoid unnecessary heap allocations on hot paths.
- Avoid unnecessary temporary strings, copies, and format conversions.
- Prefer moving work out of locks rather than doing more work under locks.
- Mention algorithmic complexity when touching loops, maps, queues, or batching logic.
- If a change may affect tail latency, say so explicitly.
- Prefer small, measurable optimizations over broad speculative rewrites.
- If performance is the goal, propose how to benchmark before and after.
- Call out cache locality, false sharing, batching, and backpressure implications when relevant.
```

适用目的：

- 告诉 Cursor：这里不是普通业务 CRUD，而是要考虑延迟和吞吐

---

### 12.6 测试与验证规则：`40-testing.mdc`

```md
---
description: "Testing and verification rules for backend changes"
alwaysApply: true
---

- Prefer adding or updating tests when behavior changes.
- Follow the existing testing style and fixture structure.
- Do not silently weaken tests to make code pass.
- If tests are not added, explain why not.
- Suggest verification steps: build, unit test, integration test, and benchmark when relevant.
- For bug fixes, try to add a regression test when practical.
- For parser or protocol changes, include malformed input and boundary-condition coverage.
- For concurrency-related changes, mention stress-test or repeated-run strategy if unit tests are insufficient.
```

---

### 12.7 RPC / 网络规则：`50-rpc-network.mdc`

```md
---
description: "Rules for RPC, networking, protocol, and connection lifecycle changes"
alwaysApply: false
globs:
	- "net/**/*.h"
	- "net/**/*.hpp"
	- "net/**/*.cc"
	- "net/**/*.cpp"
	- "rpc/**/*.h"
	- "rpc/**/*.hpp"
	- "rpc/**/*.cc"
	- "rpc/**/*.cpp"
---

- Preserve protocol compatibility unless breaking changes are explicitly requested.
- Be explicit about framing, partial packets, retries, timeouts, and connection teardown.
- For request/response flows, describe where deadlines, cancellation, and backpressure apply.
- Do not block IO threads with long-running work.
- When changing parsing or serialization, mention boundary cases and malformed input handling.
- For connection lifecycle changes, explain resource cleanup on normal close, timeout, and error close.
- For retry logic, mention idempotency assumptions.
```

---

### 12.8 可观测性规则：`60-observability.mdc`

```md
---
description: "Observability rules for logs, metrics, tracing, and production diagnosis"
alwaysApply: false
---

- Prefer structured logging over large unstructured text blocks.
- Include stable identifiers when useful: request_id, connection_id, user_id, shard_id, peer, error_code.
- Do not log secrets, tokens, raw credentials, or sensitive payloads.
- For production issues, prefer adding targeted logs/metrics before broad invasive code changes.
- When adding a new failure path, consider whether metrics or tracing tags should be updated.
- If a bug is hard to reproduce, suggest the smallest observability change that improves diagnosis.
```

---

### 12.9 如何组合使用这些 Rules

推荐策略：

- `00-general.mdc`、`40-testing.mdc` 设为常驻
- `20-concurrency.mdc` 用于并发、线程池、回调、队列、连接池
- `30-performance.mdc` 用于热路径、协议解析、日志路径、调度路径
- `50-rpc-network.mdc` 用于网络协议、连接生命周期、RPC 链路
- `60-observability.mdc` 用于排障、监控、日志增强

### 一条很重要的经验

不要追求“一份规则管天下”。

最好的规则不是最长的规则，而是：

- 当前任务真正会用到
- 容易被 Agent 正确执行
- 不和现有工程习惯冲突

---

## 13. `AGENTS.md` 完整版

如果你更喜欢“简单、统一、容易读”的风格，可以在仓库根目录放一个完整版 `AGENTS.md`。

这个版本适合大多数 C++ 服务端项目直接起步。

```md
# AGENTS.md

## Project Identity

- This repository is a C++ server backend project.
- Typical concerns include networking, RPC, concurrency, timeouts, cancellation, observability, and performance.
- Changes should preserve stability, debuggability, and predictable latency.

## General Working Style

- Prefer minimal, local, low-risk changes.
- Understand the existing code path before modifying it.
- Reuse existing patterns and utilities before adding new abstractions.
- Do not perform broad refactors unless explicitly requested.
- When requirements are ambiguous, choose the smallest correct solution and explain assumptions.
- Keep code style, naming, and file organization consistent with nearby code.

## Architecture

- Keep transport, protocol, business logic, persistence, and utility concerns separated where the existing codebase allows.
- Avoid introducing unnecessary cross-module coupling.
- If adding a new component, explain where it belongs in the architecture and why.
- Prefer explicit ownership and dependency injection over hidden global state.

## Concurrency and Lifetime

- Before changing shared-state code, explain thread-safety impact.
- Be explicit about ownership, destruction order, callback lifetime, and shutdown behavior.
- Call out whether code runs on IO threads, worker threads, timer threads, or arbitrary threads.
- Avoid detached threads unless explicitly requested.
- Avoid capturing raw pointers in async callbacks unless lifetime is obviously guaranteed.
- Explain lock ordering when multiple locks may be involved.
- Explain memory ordering when atomics are used or modified.
- Highlight risks such as deadlock, race conditions, use-after-free, double-close, ABA, or re-entrancy.

## Performance

- Treat request processing, parsing, serialization, scheduling, and logging on hot paths as performance-sensitive.
- Avoid unnecessary allocations, string copies, and data format conversions.
- Avoid doing expensive work while holding locks.
- Mention complexity changes when touching core data structures or loops.
- If a change may impact throughput or tail latency, say so explicitly.
- Prefer measurable optimizations over speculative rewrites.
- When performance is a goal, propose benchmark or profiling validation.

## Networking and RPC

- Preserve wire or RPC compatibility unless a breaking change is explicitly requested.
- Be explicit about partial packets, framing, retries, deadlines, cancellation, and backpressure.
- Do not block IO threads with slow work.
- When changing protocol parsing or serialization, include malformed input and boundary case considerations.
- For retry logic, mention idempotency assumptions.
- For connection lifecycle changes, explain behavior on normal close, timeout, and error paths.

## Error Handling

- Do not swallow errors silently.
- Preserve existing error propagation conventions unless there is a strong reason to improve them.
- Include actionable context in errors and logs.
- If adding a new error path, consider whether metrics or tracing should be updated.

## Logging, Metrics, and Tracing

- Prefer structured logging.
- Include stable identifiers when useful, such as request_id, session_id, peer, shard_id, and error_code.
- Do not log secrets or sensitive payloads.
- For hard-to-reproduce issues, prefer the smallest observability improvement that helps diagnosis.

## Testing and Verification

- Add or update tests when behavior changes.
- Follow existing test patterns, naming, and fixture style.
- Do not modify tests only to make implementation pass unless the test is clearly wrong.
- For bug fixes, add a regression test when practical.
- For parser or protocol changes, include malformed, truncated, and boundary-condition cases.
- For concurrency changes, mention whether stress tests, repeated runs, or sanitizers are needed.
- Suggest validation steps such as build, unit tests, integration tests, benchmarks, and static analysis.

## Change Scope Discipline

- Keep unrelated formatting or cleanup out of functional changes unless requested.
- Avoid renaming symbols unless necessary for correctness or clarity.
- Avoid changing public interfaces unless required.
- If a task becomes larger than expected, stop and summarize the expanded scope before continuing.

## Documentation

- For non-trivial changes, summarize: what changed, why, risks, and how to verify.
- When useful, produce short design notes, flow summaries, or rollout/checklist notes.
- Prefer concrete explanations over generic prose.

## Preferred Agent Workflow

- First understand the existing implementation.
- Then propose a plan for non-trivial changes.
- Then implement in small verifiable steps.
- Then run or suggest validation.
- Then summarize diffs, risks, and next steps.
```

---

### 13.1 适合放到子目录的局部 `AGENTS.md`

如果你的工程足够大，还可以在子目录放局部版 `AGENTS.md`，例如：

- `rpc/AGENTS.md`
- `net/AGENTS.md`
- `storage/AGENTS.md`
- `tests/AGENTS.md`

例如 `rpc/AGENTS.md` 可以专门约束：

```md
# RPC Module Instructions

- Preserve protocol compatibility unless explicitly requested.
- Be explicit about timeout propagation and cancellation.
- Keep client and server behavior consistent.
- Add tests for malformed payloads and timeout edge cases.
```

这样做的好处是：Agent 在局部上下文中会更精准。

---

## 14. `CMakePresets.json` + `.clang-format` + `.clang-tidy` 模板

这一节给的是一套偏通用、偏稳健的 C++ 服务端工程基础模板。

你可以直接复制，再按项目情况调整编译器、标准版本、告警级别、第三方库路径。

---

### 14.1 `CMakePresets.json` 模板

```json
{
	"version": 6,
	"cmakeMinimumRequired": {
		"major": 3,
		"minor": 25,
		"patch": 0
	},
	"configurePresets": [
		{
			"name": "base",
			"hidden": true,
			"generator": "Ninja",
			"binaryDir": "${sourceDir}/build/${presetName}",
			"cacheVariables": {
				"CMAKE_EXPORT_COMPILE_COMMANDS": "ON",
				"CMAKE_CXX_STANDARD": "20",
				"CMAKE_CXX_STANDARD_REQUIRED": "ON",
				"CMAKE_CXX_EXTENSIONS": "OFF",
				"BUILD_TESTING": "ON"
			}
		},
		{
			"name": "debug-clang",
			"inherits": "base",
			"displayName": "Debug (Clang)",
			"cacheVariables": {
				"CMAKE_BUILD_TYPE": "Debug",
				"CMAKE_C_COMPILER": "clang",
				"CMAKE_CXX_COMPILER": "clang++"
			}
		},
		{
			"name": "release-clang",
			"inherits": "base",
			"displayName": "Release (Clang)",
			"cacheVariables": {
				"CMAKE_BUILD_TYPE": "Release",
				"CMAKE_C_COMPILER": "clang",
				"CMAKE_CXX_COMPILER": "clang++"
			}
		},
		{
			"name": "asan",
			"inherits": "debug-clang",
			"displayName": "Debug + ASan/UBSan",
			"cacheVariables": {
				"CMAKE_CXX_FLAGS_DEBUG": "-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer",
				"CMAKE_C_FLAGS_DEBUG": "-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer",
				"CMAKE_EXE_LINKER_FLAGS": "-fsanitize=address,undefined",
				"CMAKE_SHARED_LINKER_FLAGS": "-fsanitize=address,undefined"
			}
		},
		{
			"name": "tsan",
			"inherits": "debug-clang",
			"displayName": "Debug + TSan",
			"cacheVariables": {
				"CMAKE_CXX_FLAGS_DEBUG": "-O1 -g -fsanitize=thread -fno-omit-frame-pointer",
				"CMAKE_C_FLAGS_DEBUG": "-O1 -g -fsanitize=thread -fno-omit-frame-pointer",
				"CMAKE_EXE_LINKER_FLAGS": "-fsanitize=thread",
				"CMAKE_SHARED_LINKER_FLAGS": "-fsanitize=thread"
			}
		}
	],
	"buildPresets": [
		{
			"name": "build-debug",
			"configurePreset": "debug-clang",
			"jobs": 12
		},
		{
			"name": "build-release",
			"configurePreset": "release-clang",
			"jobs": 12
		},
		{
			"name": "build-asan",
			"configurePreset": "asan",
			"jobs": 12
		},
		{
			"name": "build-tsan",
			"configurePreset": "tsan",
			"jobs": 12
		}
	],
	"testPresets": [
		{
			"name": "test-debug",
			"configurePreset": "debug-clang",
			"output": {
				"outputOnFailure": true
			}
		},
		{
			"name": "test-asan",
			"configurePreset": "asan",
			"output": {
				"outputOnFailure": true
			}
		},
		{
			"name": "test-tsan",
			"configurePreset": "tsan",
			"output": {
				"outputOnFailure": true
			}
		}
	]
}
```

### 这个模板的价值

- 默认导出 `compile_commands.json`
- 同时支持 debug / release / asan / tsan
- 很适合服务端问题排查
- 很适合让 Cursor 明确知道有哪些构建与测试入口

### 如果你在 Windows 上

若使用 MSVC，也可以再补一套 preset；如果跨平台，则建议：

- Linux/macOS 主推 Clang + Ninja
- Windows 可提供 MSVC 版本和 ClangCL 版本

---

### 14.2 `.clang-format` 模板

下面给的是偏工程化、偏服务端团队常见风格的一个基线模板：

```yaml
BasedOnStyle: LLVM
Language: Cpp
Standard: Latest

IndentWidth: 4
TabWidth: 4
UseTab: Never

ColumnLimit: 100
BreakBeforeBraces: Attach
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: false

PointerAlignment: Left
ReferenceAlignment: Left

SortIncludes: CaseSensitive
IncludeBlocks: Regroup

SpaceBeforeParens: ControlStatements
SpacesInParentheses: false
SpacesInSquareBrackets: false
SpaceInEmptyParentheses: false

AlignConsecutiveAssignments: false
AlignConsecutiveDeclarations: false

Cpp11BracedListStyle: true
DerivePointerAlignment: false

NamespaceIndentation: None
IndentCaseLabels: true
IndentGotoLabels: false

AccessModifierOffset: -4
EmptyLineBeforeAccessModifier: LogicalBlock
FixNamespaceComments: true

ReflowComments: true
KeepEmptyLinesAtTheStartOfBlocks: false
MaxEmptyLinesToKeep: 1
```

### 这个模板为什么比较适合后端团队

- 4 空格缩进，适合 C++ 大型工程阅读
- `ColumnLimit: 100` 比 80 更适合现代服务端代码
- 风格保守，不容易引发历史代码大面积 diff
- 足够统一，但不至于太激进

---

### 14.3 `.clang-tidy` 模板

下面是一个比较适合 C++ 服务端项目起步的模板：

```yaml
Checks: >
	-*,
	bugprone-*,
	performance-*,
	readability-*,
	modernize-*,
	cppcoreguidelines-*,
	misc-*,
	portability-*,
	-readability-magic-numbers,
	-cppcoreguidelines-avoid-magic-numbers,
	-cppcoreguidelines-owning-memory,
	-cppcoreguidelines-pro-bounds-array-to-pointer-decay,
	-modernize-use-trailing-return-type,
	-readability-identifier-length,
	-cppcoreguidelines-pro-type-vararg,
	-cppcoreguidelines-avoid-c-arrays

WarningsAsErrors: >
	bugprone-*,
	performance-*

HeaderFilterRegex: '^(src|include|tests|net|rpc)/'

AnalyzeTemporaryDtors: false
FormatStyle: file

CheckOptions:
	- key: readability-function-cognitive-complexity.Threshold
		value: '30'
	- key: readability-function-size.LineThreshold
		value: '120'
	- key: modernize-loop-convert.MinConfidence
		value: reasonable
	- key: bugprone-branch-clone.MinComplexity
		value: '10'
	- key: performance-unnecessary-value-param.AllowedTypes
		value: '::std::string_view;::std::span'
```

### 使用建议

不要一上来把所有 tidy 告警都当错误处理。

更稳妥的做法：

1. 先开 `bugprone-*`、`performance-*`
2. 再逐步引入 `modernize-*`
3. 最后按团队接受度收紧 `cppcoreguidelines-*`

因为 C++ 老项目通常历史包袱很重，过于激进会让团队反感。

---

### 14.4 推荐补的工程配置

除了上面三个文件，还建议逐步补：

- `.editorconfig`
- `scripts/build.sh` 或 `scripts/build.ps1`
- `scripts/test.sh` 或 `scripts/test.ps1`
- `scripts/tidy.sh`
- `scripts/format.sh`
- `docs/architecture.md`
- `docs/debugging.md`

这些资产越清晰，Cursor 的表现越稳定。

---

## 15. 适合网络 / RPC / 高性能后端的 Cursor 提示词手册

这一节是可直接复用的高质量提示词模板。

你可以把它理解成：

> 不同任务下，如何对 Cursor 下达“专业、稳定、少走弯路”的指令。

---

### 15.1 通用提问原则

尽量让提示词包含这 6 个要素：

1. **目标**：你到底想实现什么
2. **范围**：允许改哪些，不允许改哪些
3. **约束**：线程安全、兼容性、性能、风格要求
4. **流程**：先分析还是先计划，还是直接改
5. **验证**：编译、测试、benchmark、静态检查
6. **输出格式**：按清单、按步骤、按表格、按风险说明

---

### 15.2 理解代码库模板

```text
先不要修改代码。
请帮我理解这个 C++ 服务端仓库，重点回答：
1. 程序入口与启动流程
2. 配置加载流程
3. 网络事件进入业务处理的调用链
4. 线程模型与任务调度方式
5. 请求生命周期
6. 日志、指标、trace、错误处理相关代码位置
7. 建议的阅读顺序
```

---

### 15.3 新功能开发模板

```text
先不要写代码，先做分析。

目标：在当前 C++ 服务端项目中增加 [功能名]。

请输出：
1. 当前最相关的实现位置
2. 已有可复用组件
3. 最小侵入实现方案
4. 涉及的类、接口、配置、测试
5. 线程安全影响
6. 性能影响
7. 验证方案
```

---

### 15.4 进入 Plan Mode 的模板

```text
进入 Plan Mode。

目标：实现 [功能名]。

要求：
1. 先研究现有实现
2. 如需求不清楚，先提出澄清问题
3. 生成分步骤计划，不要立刻写代码
4. 每一步标明涉及文件、风险、验证方式
5. 优先最小侵入方案
6. 给出回滚方案
```

---

### 15.5 RPC 接口扩展模板

```text
请先不要改代码。

目标：给当前 RPC 服务增加 [新接口/新字段/新错误码]。

请先分析：
1. 现有 RPC 定义与处理流程
2. 编解码 / 序列化路径
3. 客户端与服务端兼容性影响
4. 超时、重试、取消是否受影响
5. 哪些测试需要补充
6. 最小修改方案
```

---

### 15.6 网络协议解析模板

```text
请分析当前协议解析实现，不要先修改。

重点说明：
1. 报文边界如何识别
2. 半包 / 粘包如何处理
3. 非法报文如何处理
4. 内存分配与拷贝点在哪里
5. 是否有越界、整数溢出、悬垂引用风险
6. 给出最小侵入优化建议
```

---

### 15.7 高性能优化模板

```text
请先做性能分析，不要直接改代码。

目标：降低 [模块名] 的延迟 / 提升吞吐。

从以下角度输出：
1. 热路径识别
2. 时间复杂度
3. 分配热点
4. 拷贝热点
5. 锁竞争
6. cache locality / false sharing 风险
7. 三个优化方案（收益、风险、侵入性）
8. 如何 benchmark 验证
```

---

### 15.8 并发问题排查模板

```text
请系统性分析这段并发相关代码，不要立即修改。

重点关注：
1. 共享状态读写点
2. 锁顺序与锁粒度
3. 回调生命周期
4. 原子变量与内存序
5. shutdown 路径资源释放顺序
6. 是否存在 race / deadlock / ABA / use-after-free 风险
7. 如果修复，建议按最小风险步骤实施
```

---

### 15.9 尾延迟问题排查模板

```text
请帮我分析这个服务尾延迟升高的问题。

现象：
- 平均延迟正常
- P99/P999 升高
- CPU 利用率不高

请输出：
1. 可能原因树
2. 最优先检查的代码路径
3. 最优先检查的指标
4. 最值得补充的日志 / trace
5. 哪些锁、队列、批处理、重试逻辑值得重点看
6. 给出最小观测性增强方案
```

---

### 15.10 超时与取消模板

```text
请分析当前请求处理链路中的超时与取消机制，不要先改代码。

请回答：
1. 超时在哪里设置
2. 超时如何传播
3. 取消是否能中断下游工作
4. 连接断开时后台任务如何收敛
5. 是否存在“请求超时了但工作还在继续”的情况
6. 给出最小侵入修正方案
```

---

### 15.11 日志与可观测性增强模板

```text
请不要先改业务逻辑。

目标：增强这个模块的线上可观测性，便于排查 [某类问题]。

请输出：
1. 当前日志、指标、trace 覆盖情况
2. 缺失的关键上下文
3. 最小日志增强方案
4. 最小指标增强方案
5. 需要避免记录的敏感信息
6. 如何验证这些观测点有效
```

---

### 15.12 写测试模板

```text
请先不要改实现，先补测试。

要求：
1. 按现有测试风格编写
2. 先列出应覆盖的行为清单
3. 先写会失败的测试
4. 覆盖正常路径、异常路径、边界条件
5. 若涉及协议或解析，覆盖半包、坏包、空输入、极限输入
6. 不要引入不必要的 mock
```

---

### 15.13 修 bug 模板

```text
请先不要直接修复。

问题现象：[描述现象]

请先输出：
1. 可能原因排序
2. 最可能的代码位置
3. 如何复现
4. 最小修复方案
5. 需要补哪些回归测试
6. 风险点与回滚方案
```

---

### 15.14 重构模板

```text
进入 Plan Mode。

目标：对 [模块名] 做小步重构。

约束：
1. 不改变外部行为
2. 不破坏协议兼容性
3. 不影响关键性能路径
4. 每一步都可编译、可验证、可回滚

请输出分阶段计划，并说明每一步的收益与风险。
```

---

### 15.15 生成设计文档模板

```text
请根据当前代码实现，帮我生成一份设计说明文档。

内容包括：
1. 模块职责
2. 核心类与关系
3. 请求调用链
4. 线程模型
5. 错误处理
6. 性能关键点
7. 已知限制与后续优化方向

请用适合工程文档的风格输出，尽量具体，不要空话。
```

---

### 15.16 让 Prompt 更强的几个小技巧

#### 技巧 1：先加一句“先不要改代码”

这能显著提高分析质量。

#### 技巧 2：要求“按固定结构输出”

例如：

- 当前实现
- 风险
- 方案
- 验证

这样结果更稳定。

#### 技巧 3：明确“最小侵入”

对服务端项目非常重要。

#### 技巧 4：明确“线程安全影响”和“性能影响”

这两项几乎是 C++ 后端的刚需。

#### 技巧 5：明确“不允许改测试”或“先只写测试”

这对 TDD、修 bug、回归验证非常有效。

---

## 16. 最后建议：把这些模板真正落到项目里

最推荐的落地顺序：

1. 根目录放 `AGENTS.md`
2. `.cursor/rules/` 先放 `00-general.mdc` 和 `40-testing.mdc`
3. CMake 工程补 `CMakePresets.json`
4. 根目录补 `.clang-format` 和 `.clang-tidy`
5. 再按项目复杂度补并发、性能、RPC、可观测性规则

这样做之后，Cursor 在 C++ 服务端项目中的表现会明显提升：

- 更懂你的工程结构
- 更少乱改
- 更重视线程安全与性能
- 更容易给出可验证方案
- 更适合长期团队协作

---

## 17. C++ 服务端项目从新仓库初始化到可交付的完整规范清单

这一节不是“参考建议”，而是更接近一份可执行的工程 checklist。

可以把它当成：

- 新仓库初始化清单
- 技术负责人建仓规范
- 团队交付前自检清单
- 让 Cursor 长期稳定协作的工程基础设施清单

---

### 17.1 阶段 0：立项与仓库初始化

#### 必做项

- 明确项目目标：吞吐、延迟、可靠性、容量、部署环境
- 明确协议类型：HTTP、TCP、自定义二进制、RPC、消息队列
- 明确运行形态：单进程、多进程、多线程、协程
- 明确兼容性约束：协议兼容、配置兼容、ABI/API 兼容
- 明确错误预算和稳定性目标：如 SLA、SLO、错误率、超时率

#### 仓库初始化建议

- 建立 `README.md`
- 建立 `LICENSE`
- 建立 `.gitignore`
- 建立 `AGENTS.md`
- 建立 `.cursor/rules/`
- 建立 `.editorconfig`
- 建立 `.clang-format`
- 建立 `.clang-tidy`
- 建立 `CMakePresets.json`

#### 目录建议

```text
project/
	cmake/
	include/
	src/
	net/
	rpc/
	storage/
	tests/
	benchmarks/
	scripts/
	docs/
	configs/
	third_party/
```

---

### 17.2 阶段 1：基础工程设施

#### 构建系统

- 使用 CMake 统一构建
- 输出 `compile_commands.json`
- 提供 debug / release / asan / tsan preset
- 明确第三方库接入方式
- 明确编译器版本矩阵

#### 必备脚本

- `scripts/build.sh` 或 `scripts/build.ps1`
- `scripts/test.sh` 或 `scripts/test.ps1`
- `scripts/format.sh`
- `scripts/tidy.sh`
- `scripts/bench.sh`

#### CI 最小集合

- 构建检查
- 单元测试
- `clang-format` 检查
- `clang-tidy` 检查
- 至少一套 sanitizer 检查

---

### 17.3 阶段 2：代码组织与架构规范

#### 分层建议

- `net/`：连接、socket、事件循环、收发缓冲
- `rpc/`：协议、编解码、路由、超时、重试、取消
- `src/`：核心业务逻辑
- `storage/`：DB/缓存/持久化封装
- `include/`：对外公共接口
- `tests/`：单元测试、集成测试
- `benchmarks/`：基准测试

#### 规范要求

- 网络层不要夹带业务分支
- 协议层不要直接依赖存储细节
- 业务层不要反向依赖底层实现细节
- 配置读取、日志、监控、trace 要有明确入口
- 公共接口要控制稳定性，避免随意扩散

---

### 17.4 阶段 3：编码规范与静态质量

#### 编码规范

- 风格统一交给 `.clang-format`
- 潜在缺陷优先交给 `.clang-tidy`
- 禁止大量手工格式化造成无意义 diff
- 命名、错误处理、日志字段遵循项目规则

#### C++ 服务端重点要求

- 明确对象所有权
- 明确线程归属
- 明确 shutdown 顺序
- 明确异常或错误码语义
- 明确资源释放策略

#### 推荐检查项

- 未初始化读写
- 悬垂引用/悬垂指针
- 死锁风险
- 锁粒度过粗
- 热路径多余分配
- 不必要拷贝
- 不安全字符串拼接

---

### 17.5 阶段 4：配置、日志、监控、追踪

#### 配置规范

- 默认配置可启动
- 配置项有注释或文档
- 配置变更有合理默认值
- 配置加载失败要可诊断

#### 日志规范

- 结构化日志优先
- 关键字段统一：`request_id`、`connection_id`、`peer`、`error_code`
- 错误日志要可检索、可定位
- 禁止打印敏感信息

#### 指标规范

- QPS
- 错误率
- 超时率
- P50/P95/P99/P999 延迟
- 队列长度
- 连接数
- 重试次数
- 下游依赖耗时

#### Trace 规范

- 跨线程 / 跨服务链路要能跟踪
- 超时、重试、取消要可见
- 关键下游调用要打点

---

### 17.6 阶段 5：测试体系

#### 必备测试层次

- 单元测试
- 集成测试
- 协议/解析测试
- 回归测试
- 压测/基准测试

#### 单元测试要求

- 覆盖正常路径
- 覆盖异常路径
- 覆盖边界条件
- 覆盖空输入、坏输入、极限输入

#### 并发相关测试要求

- 重复运行
- 高并发压力下运行
- shutdown / close / cancel 路径覆盖
- sanitizer 辅助验证

#### 协议与 RPC 测试要求

- 半包
- 粘包
- 超时
- 取消
- 重试
- 错误码
- 向后兼容性

---

### 17.7 阶段 6：性能与稳定性

#### 性能基线必须明确

- 吞吐基线
- 延迟基线
- CPU 基线
- 内存基线
- 连接规模基线

#### 必查热点

- 序列化/反序列化
- 协议解析
- 日志热路径
- 内存分配
- 锁竞争
- 批处理与刷盘策略
- 队列积压

#### 稳定性验证

- 长时间运行稳定性
- 峰值流量下表现
- 依赖故障时退化行为
- 限流/熔断/超时是否生效
- 连接异常断开时资源是否回收

---

### 17.8 阶段 7：上线前交付要求

#### 代码交付前

- 所有主要改动有说明
- build 通过
- 关键测试通过
- 主要日志和监控齐全
- 没有明显未处理 TODO 留在关键路径

#### 文档交付前

- 架构说明
- 配置说明
- 启动方式
- 常见故障排查
- 指标与日志说明
- 回滚方案

#### 运维交付前

- 部署方式明确
- 启停脚本明确
- 健康检查接口明确
- 告警阈值明确
- 容量规划明确

---

### 17.9 阶段 8：上线后与持续迭代

#### 上线后必须关注

- 错误率变化
- 延迟分位数变化
- 新日志是否足够定位问题
- 新指标是否反映真实状态
- 内存与线程数是否异常

#### 持续改进建议

- 每次线上事故都补回归测试
- 每次排障都补文档或 Rule
- 每次性能优化都补 benchmark
- 每次新增模块都补设计说明

---

### 17.10 适合团队执行的交付前总检查

#### 工程基础

- [ ] `AGENTS.md` 已存在
- [ ] `.cursor/rules/` 已建立
- [ ] `.clang-format` 已建立
- [ ] `.clang-tidy` 已建立
- [ ] `CMakePresets.json` 已建立
- [ ] `compile_commands.json` 可生成

#### 构建与测试

- [ ] Debug 构建通过
- [ ] Release 构建通过
- [ ] ASan 构建/测试通过
- [ ] TSan 或并发验证流程明确
- [ ] 单元测试通过
- [ ] 协议/解析测试覆盖关键边界

#### 代码质量

- [ ] 无明显线程安全漏洞
- [ ] 无明显生命周期漏洞
- [ ] 无明显热路径多余分配/拷贝
- [ ] 日志字段足够定位问题
- [ ] 错误处理清晰

#### 交付资料

- [ ] README 可指导本地启动
- [ ] 配置项可理解
- [ ] 关键模块有文档
- [ ] 回滚方案存在
- [ ] 常见故障排查文档存在

---

### 17.11 用 Cursor 执行这份清单的推荐方式

你可以直接把下面这段发给 Cursor：

```text
请对当前 C++ 服务端仓库做一次“可交付性审查”，不要直接修改代码。

请按以下维度输出：
1. 工程基础设施缺失项
2. 构建与测试缺失项
3. 并发与生命周期风险项
4. 性能与可观测性缺失项
5. 文档与交付缺失项
6. 按优先级给出补齐计划
```

这样，Cursor 就不只是帮你写代码，而是在帮你做“项目工程化体检”。

---

## 18. 如何把 Claude Skills 合理迁移到 Cursor

当前仓库已经按“收敛而不是平移”的思路完成第一轮落地：

### 已落地到 Cursor Rules

- `00-general.mdc`
- `20-concurrency.mdc`
- `25-security-compliance.mdc`
- `30-performance.mdc`
- `35-risk-data-protocols.mdc`
- `50-rpc-network.mdc`
- `60-observability.mdc`

### 已落地到 Playbooks

- `cursor_notes/playbooks/code-review.md`
- `cursor_notes/playbooks/performance-latency-review.md`
- `cursor_notes/playbooks/security-compliance-review.md`
- `cursor_notes/playbooks/risk-data-protocol-review.md`
- `cursor_notes/playbooks/cloud-ops-review.md`

### 已落地到 Prompt 模板

- `cursor_notes/prompts/ml-model-validation.md`
- `cursor_notes/prompts/defi-protocol-review.md`
- `cursor_notes/prompts/trading-simulation-review.md`

### 已补的迁移说明

- `cursor_notes/claude-skills-to-cursor-mapping.md`
- `claude_notes/.claude/skills/REVIEW_AND_OPTIMIZATION.md`

核心原则只有一句话：

> Claude Skills 里的“稳定约束”进入 Cursor Rules；
> “流程化审查方法”进入 Playbooks；
> “专项低频任务”进入 Prompt 模板。
