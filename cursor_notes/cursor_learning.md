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

## 12. 已落地的规则与工程配置（快速索引）

> 以下模板已全部落地为仓库真实文件，不再重复全文。

### Cursor Rules（`.cursor/rules/`）

| 文件 | 用途 | 常驻 |
| --- | --- | --- |
| `00-general.mdc` | 低风险改动、复用优先、风格一致 | 是 |
| `10-architecture.mdc` | 分层约束、模块依赖方向 | 否 |
| `20-concurrency.mdc` | 线程安全、锁序、内存序、shutdown | 否 |
| `25-security-compliance.mdc` | 安全、合规、审计链路 | 否 |
| `30-performance.mdc` | 热路径分配、拷贝、锁内工作量 | 否 |
| `35-risk-data-protocols.mdc` | 风控精度、协议兼容、数据正确性 | 否 |
| `40-testing.mdc` | 测试要求、回归测试、sanitizer | 是 |
| `50-rpc-network.mdc` | 协议兼容、超时、重试、背压 | 否 |
| `60-observability.mdc` | 结构化日志、指标、trace | 否 |

**组合策略**：`00-general` + `40-testing` 常驻；其余按改动类型手动启用。

### 工程配置

| 文件 | 作用 |
| --- | --- |
| `AGENTS.md` | Agent 全局行为约束（先理解→计划→小步实现→验证→总结） |
| `.clang-format` | LLVM 基础 + 4 空格缩进 + 100 列宽 |
| `.clang-tidy` | bugprone + performance 视为阻塞；modernize + readability 逐步收紧 |
| `CMakePresets.json` | debug / release / asan / tsan 一键构建 + 自动导出 compile_commands.json |

---

## 13. 互联网金融 C++ 提示词精选（10 个高频场景）

### 13.1 理解服务框架

```text
先不要修改代码。
请解释这个 C++ 服务端工程的架构：
1. 程序入口与启动流程
2. 配置加载
3. 网络事件 → 业务处理的调用链
4. 线程模型
5. 请求生命周期
6. 日志 / 指标 / trace 位置
7. 建议的阅读顺序
```

### 13.2 新功能开发

```text
先不要写代码。目标：在当前项目中增加 [功能名]。
请输出：
1. 当前最相关的实现位置与可复用组件
2. 最小侵入实现方案
3. 涉及的类、接口、配置、测试
4. 线程安全影响与性能影响
5. 验证方案
```

### 13.3 并发风险分析

```text
请分析这段代码的并发风险，不要先改代码。
重点：数据竞争、锁顺序、回调生命周期、悬垂引用、原子变量内存序、死锁/ABA/use-after-free 风险。
给出最小风险修复步骤。
```

### 13.4 性能瓶颈排查

```text
请先分析性能瓶颈，不要直接修改。
从以下角度：热路径、时间复杂度、内存分配、拷贝次数、锁竞争、cache locality。
给出收益最大、侵入最小的优化方案 + benchmark 验证方式。
```

### 13.5 RPC / 协议扩展

```text
目标：给 RPC 服务增加 [新接口/新字段/新错误码]。
请先分析：现有 RPC 定义、编解码路径、客户端兼容性、超时/重试/取消影响、需补的测试。
给出最小修改方案。
```

### 13.6 协议解析审查

```text
请分析当前协议解析实现，不要先修改。
重点：报文边界、半包/粘包处理、非法报文、内存分配/拷贝、越界/溢出风险。
```

### 13.7 尾延迟排查

```text
现象：平均延迟正常，P99/P999 升高，CPU 利用率不高。
请输出：可能原因树、最优先检查的代码路径和指标、重点看的锁/队列/批处理/重试逻辑、最小观测增强方案。
```

### 13.8 风险计算验证（金融特有）

```text
请审查这段风险计算代码，不要先修改。
重点：数值精度（rounding/overflow/underflow）、金融单位、边界条件（空头寸/零价格/极值）、
蒙特卡洛/VaR/Greeks 收敛性、回测偏差与未来函数、回归测试覆盖。
```

### 13.9 行情数据质量验证（金融特有）

```text
请审查行情数据接入与清洗逻辑，不要先修改。
重点：完整性（缺失/重复/乱序）、准确性（价格跳变/精度/汇率）、时效性（延迟/过期/时钟偏差）、
一致性（多源对齐/快照增量）、异常处理（闪崩/停牌/非交易时段）。
```

### 13.10 可交付性审查

```text
请对当前仓库做可交付性审查，不修改代码。
维度：工程基础设施、构建测试、并发生命周期、性能可观测性、文档交付。
按优先级给出补齐计划。
```

---

## 14. 审查工作流资产索引

### Playbooks（审查流程）

| Playbook | 适用场景 |
| --- | --- |
| `playbooks/code-review.md` | 普通 PR、实现质量 |
| `playbooks/performance-latency-review.md` | 性能、延迟、并发瓶颈 |
| `playbooks/risk-data-protocol-review.md` | 风险、数据、协议、API |
| `playbooks/security-compliance-review.md` | 安全、合规、审计 |
| `playbooks/cloud-ops-review.md` | 部署、观测、运维 |

### PR 审查模板

| 模板 | 专项 |
| --- | --- |
| `pr_review_templates/cpp-backend-general.md` | 通用 C++ 服务端 |
| `pr_review_templates/cpp-backend-concurrency.md` | 并发 / 生命周期 |
| `pr_review_templates/cpp-backend-performance.md` | 性能 / 低延迟 |
| `pr_review_templates/cpp-backend-security-protocol.md` | 安全 / 协议 / 风险 |

### 自动化脚本

| 脚本 | 用途 |
| --- | --- |
| `scripts/review/export_pr_snapshot.py` | 导出 PR diff 上下文 |
| `scripts/review/generate_review_prompt.py` | 自动生成审查 Prompt |
| `scripts/review/repo_delivery_audit.py` | 仓库可交付性检查 |

### 快速导航

- 不确定从哪开始 → `review-cheat-sheet.md`
- 已看到问题现象 → `problem-symptom-navigation.md`
- 需要速查对应关系 → `skill-playbook-quick-reference.md`

---

## 15. 交付检查清单（精简版）

### 工程基础

- [ ] `AGENTS.md` + `.cursor/rules/` + `.clang-format` + `.clang-tidy` + `CMakePresets.json`
- [ ] `compile_commands.json` 可生成
- [ ] build / test / format / tidy 一键脚本

### 代码质量

- [ ] 所有权、线程归属、shutdown 顺序明确
- [ ] 无数据竞争 / 悬垂引用 / 热路径冗余分配
- [ ] 错误处理清晰，日志可定位

### 测试

- [ ] 单元测试 + 协议边界测试 + ASan/TSan 通过
- [ ] 并发路径有压测策略

### 观测与交付

- [ ] QPS / 错误率 / P99 / 队列长度指标覆盖
- [ ] 结构化日志 + trace 链路
- [ ] README + 配置说明 + 回滚方案

---

## 16. 互联网金融 C++ 实战工作流

### 工作流 1：撮合引擎性能优化

```text
进入 Plan Mode。
目标：优化撮合引擎的订单匹配热路径，降低 P99 延迟。
约束：不改撮合语义，不引入新锁争用，先 profiling 再改代码，每步可回滚。
请先分析：订单簿数据结构、匹配循环热路径、分配/拷贝/锁/cache miss 热点。
给出 3 个方案（收益/风险/侵入性）。
```

### 工作流 2：行情网关协议升级

```text
目标：行情网关从 FIX 4.2 升级到 FIX 4.4，增加 MarketDataIncrementalRefresh。
请分析：当前解码器支持、新增 35=X 消息结构、半包/校验和、快照增量一致性、
性能影响、回归测试策略、最小侵入方案。
```

### 工作流 3：风控链路超时治理

```text
现象：风控服务在行情波动高峰期超时。
请先不修改代码：分析网关→撮合→风控调用链、超时配置与传播、取消信号传递、
队列积压与背压、最小侵入修正方案 + 观测增强方案。
```

### 工作流 4：交易系统灰度上线

```text
进入 Plan Mode。
目标：设计交易系统新版本灰度上线方案。
请输出：部署拓扑、灰度策略（按用户/品种/流量）、关键指标监控、
回滚触发条件、新旧版本结果比对、需要补的 runbook 与告警。
```

### 工作流 5：合规审计日志增强

```text
目标：为交易系统增加合规审计日志，满足监管报送要求。
请分析：当前审计覆盖范围、缺失事件（登录/下单/撤单/成交/异常）、
结构化导出格式、敏感信息风险、热路径性能影响、最小侵入增强方案。
```

---

## 17. 提示词技巧总结

### 5 个核心技巧

1. **先加"先不要改代码"**：显著提高分析质量
2. **要求固定输出结构**：当前实现→风险→方案→验证
3. **明确"最小侵入"**：金融系统尤其重要
4. **明确"线程安全+性能影响"**：C++ 金融后端刚需
5. **明确测试边界**："先只写测试"或"不允许改测试"

### Agent 协作流程

1. 先问理解问题（调用链、线程模型、所有权）
2. 进入 Plan Mode（修改点、风险、测试、回滚）
3. 小步执行（接口→实现→测试→文档）
4. 要求验证（编译→单测→静态检查→benchmark）
5. 你做最终审查（线程安全、拷贝、错误码、ABI）

---

## 18. 互联网金融 C++ 后端开发专项指南

> 本章面向使用 Cursor 进行互联网金融 C++ 后端开发的工程师，覆盖架构全景、核心子系统、金融特有约束、低延迟模式、Cursor 配置策略与实战排障。

---

### 18.1 互联网金融后端架构全景

```
┌─────────────────────────────────────────────────────────────────┐
│                        客户端 / API 网关                         │
└──────┬──────────────┬──────────────┬──────────────┬─────────────┘
       │              │              │              │
  ┌────▼────┐   ┌─────▼─────┐  ┌────▼────┐  ┌─────▼─────┐
  │ 订单管理 │   │ 行情网关   │  │ 风控引擎 │  │ 账户/清算  │
  │  (OMS)  │   │(MktData GW)│  │(RiskEng) │  │(Acct/Clr) │
  └────┬────┘   └─────┬─────┘  └────┬────┘  └─────┬─────┘
       │              │              │              │
  ┌────▼────┐   ┌─────▼─────┐  ┌────▼────┐  ┌─────▼─────┐
  │ 撮合引擎 │   │ 行情分发   │  │ 头寸管理 │  │ 结算/报送  │
  │(MatchEng)│   │(MktData Pub)│ │(PosMgr) │  │(Settle)   │
  └────┬────┘   └─────┬─────┘  └────┬────┘  └─────┬─────┘
       │              │              │              │
  ┌────▼──────────────▼──────────────▼──────────────▼─────────────┐
  │               持久化层（WAL / DB / 消息队列）                   │
  └───────────────────────────────────────────────────────────────┘
```

#### 各子系统核心关注点

| 子系统 | 核心指标 | 关键约束 | Cursor 重点审查 |
| --- | --- | --- | --- |
| 撮合引擎 | 单笔 < 10μs | 确定性延迟、无 GC、无锁优先 | 订单簿结构、匹配算法、内存分配 |
| 订单管理 | 吞吐量 > 100K/s | 状态机完整性、幂等、WAL | 状态转换、重复检测、持久化路径 |
| 行情网关 | 延迟 < 1ms | 协议兼容、快照一致性 | 解码器、半包处理、时间戳精度 |
| 风控引擎 | 延迟 < 50μs | 实时计算、规则热更新 | 精度、规则匹配、降级策略 |
| 头寸管理 | 强一致 | 余额不能出负、对账可追溯 | 原子更新、溢出检查、审计日志 |
| 清算结算 | 批量正确 | 资金安全、可回退 | 事务边界、幂等、对账校验 |

---

### 18.2 撮合引擎开发指南

撮合引擎是金融系统最核心、对延迟最敏感的组件。

#### 18.2.1 订单簿数据结构

```cpp
// 典型的 L3 订单簿结构
struct PriceLevel {
    int64_t  price;          // 定点数避免浮点误差 (price * 10^8)
    uint64_t total_qty;      // 该价位总量
    // 同价位订单链表（FIFO 匹配）
    struct Order* head;
    struct Order* tail;
};

struct OrderBook {
    // 买盘：降序（最高买价在前）
    // 卖盘：升序（最低卖价在前）
    // 选型考虑：
    //   std::map       — 简单但 cache 不友好
    //   flat sorted vector — cache 友好但插入 O(n)
    //   skip list      — 折中方案
    //   array indexed by price tick — 最快但内存换速度
};
```

**Cursor 审查提示词**：

```text
请审查撮合引擎的订单簿实现，不要先修改。
重点：
1. 数据结构选型对延迟的影响（cache line、branch prediction）
2. 插入/删除/匹配的时间复杂度
3. 内存分配策略（是否用 pool allocator）
4. 同价位 FIFO 保序正确性
5. 价格比较的精度安全（定点数 vs 浮点）
6. 撤单路径是否 O(1)
7. 空订单簿 / 单边订单簿的边界处理
```

#### 18.2.2 匹配算法核心路径

```text
进入 Plan Mode。
目标：分析撮合引擎匹配循环的关键路径。
请输出：
1. 新订单进入 → 匹配 → 成交 → 残留挂单的完整流程
2. 每一步的 CPU 开销分析（比较次数、内存访问、分支预测）
3. 当前是否在匹配循环中有不必要的：
   - 堆分配（new/malloc）
   - 字符串格式化
   - 日志调用
   - 锁获取
   - 虚函数调用
4. 建议的零拷贝 / 零分配优化方案
5. benchmark 验证方法
```

#### 18.2.3 撮合引擎常用 Cursor 规则

```yaml
# .cursor/rules/70-matching-engine.mdc
description: "撮合引擎开发约束"
globs:
  - "matching/**"
  - "orderbook/**"
rules:
  - 匹配循环内禁止堆分配、禁止日志、禁止虚函数调用
  - 价格必须用定点整数表示，禁止 double/float 比较
  - 订单簿操作必须保证 FIFO 公平性
  - 新增字段必须评估 cache line 对齐影响
  - 所有时间戳使用 nano 精度（chrono::steady_clock 或 rdtsc）
  - 撮合结果必须先写 WAL 再发送回报
```

---

### 18.3 订单管理系统（OMS）开发指南

#### 18.3.1 订单状态机

```
            ┌──────────┐
            │ PendingNew │
            └─────┬────┘
                  │ 风控通过
            ┌─────▼────┐     撤单请求    ┌──────────┐
            │   New     ├───────────────►│PendingCancel│
            └─────┬────┘                └─────┬────┘
                  │ 部分成交                    │
            ┌─────▼────┐                ┌─────▼────┐
            │PartialFill│               │ Cancelled │
            └─────┬────┘                └──────────┘
                  │ 全部成交
            ┌─────▼────┐
            │  Filled   │
            └──────────┘
            
  异常状态：Rejected / Expired / Suspended
```

**Cursor 审查提示词**：

```text
请审查订单状态机实现，不要修改代码。
重点：
1. 所有合法状态转换是否完整覆盖
2. 非法转换是否有明确拒绝 + 日志
3. 并发场景：同一订单的撤单与成交回报是否有竞态
4. 幂等性：重复的成交回报是否安全处理
5. 持久化：状态变更是否先写 WAL/DB 再发送通知
6. 异常恢复：crash 后订单状态是否可从 WAL 恢复
7. 审计：每次状态变更是否记录完整 audit trail
```

#### 18.3.2 OMS 关键开发模式

```cpp
// 订单状态转换核心模式
class OrderStateMachine {
public:
    enum class State : uint8_t {
        PendingNew, New, PartialFill, Filled,
        PendingCancel, Cancelled, Rejected, Expired
    };
    
    // 关键：状态转换必须原子化 —— 检查 + 转换 + 持久化 + 通知
    [[nodiscard]] Result transition(OrderId id, Event event) {
        auto lock = std::unique_lock(mutex_);
        
        State current = orders_[id].state;
        State next = transition_table_[current][event];
        if (next == State::Invalid) {
            LOG_WARN("illegal transition: order={} state={} event={}", id, current, event);
            return Result::IllegalTransition;
        }
        
        // 先持久化
        if (auto r = wal_.append(id, current, next, event); !r.ok()) {
            return Result::PersistFailed;
        }
        
        // 再更新内存
        orders_[id].state = next;
        orders_[id].update_time = now_ns();
        
        // 最后通知（可异步）
        notify_queue_.push({id, current, next, event});
        
        return Result::Ok;
    }
};
```

---

### 18.4 行情系统开发指南

#### 18.4.1 行情数据链路

```
交易所/数据源
    │
    ▼ (UDP multicast / TCP)
行情网关（解码 + 清洗 + 标准化）
    │
    ▼ (shared memory / lock-free queue)
行情分发引擎（快照 + 增量 + 聚合）
    │
    ├──► 撮合引擎（参考价格）
    ├──► 风控引擎（实时估值）
    ├──► 策略引擎（信号计算）
    └──► API 网关（推送客户端）
```

#### 18.4.2 行情解码器开发

**Cursor 开发提示词**：

```text
目标：为行情网关新增 [交易所/协议] 解码器。
请先分析：
1. 当前解码器接口定义与注册机制
2. 新协议报文结构（字段、字节序、对齐）
3. 半包 / 粘包处理策略
4. 序列号检测与乱序/丢包处理
5. 时间戳转换与精度保证（交易所时间 → 本地 steady_clock）
6. 内存分配策略（零拷贝 buffer 还是 copy-out）
7. 异常报文的容错处理
8. 性能基线（单核解码吞吐目标）
给出最小侵入实现方案与测试用例。
```

#### 18.4.3 行情质量监控规则

```yaml
# .cursor/rules/72-market-data.mdc
description: "行情数据质量与解码约束"
globs:
  - "market_data/**"
  - "feed_handler/**"
rules:
  - 行情时间戳必须保留交易所原始精度，不得强制截断
  - 序列号间隔检测不可省略，丢包必须触发告警
  - 多源行情必须有主备切换与一致性校验
  - 价格字段解码后必须做合理性校验（> 0、涨跌停范围）
  - 快照与增量必须有序列号对齐机制
  - 解码器禁止在热路径使用 std::string / std::map
```

---

### 18.5 风控引擎开发指南

#### 18.5.1 风控分层架构

```
前置风控（Pre-Trade）    ← 下单前，延迟敏感（< 50μs）
    │  - 可用资金检查
    │  - 持仓限额检查
    │  - 频率限制
    │  - 自成交检测
    │
盘中风控（At-Trade）     ← 持续监控
    │  - 实时盈亏（PnL）
    │  - 风险度（VaR/Greeks）
    │  - 保证金/追保
    │  - 大额预警
    │
盘后风控（Post-Trade）   ← 日终批量
    │  - 对账
    │  - 压力测试
    │  - 报送数据生成
    │  - 历史 VaR 回测
```

#### 18.5.2 前置风控热路径

```cpp
// 前置风控核心检查 — 必须极低延迟
struct PreTradeResult {
    bool     passed;
    uint32_t reject_code;   // 0 = pass
    char     reject_reason[64];
};

// 关键约束：
// 1. 所有检查使用本地内存数据，不走网络/DB
// 2. 可用资金用原子变量维护，不加锁
// 3. 限额检查用预计算阈值，避免实时聚合
// 4. 所有金额用 int64_t 定点数，精度 10^-8
PreTradeResult check_pre_trade(const NewOrderRequest& req) {
    // 资金检查：冻结前检查
    int64_t required = req.price * req.qty;  // 定点乘法注意溢出
    int64_t available = account_cache_.available(req.account_id);
    if (required > available) {
        return {false, REJECT_INSUFFICIENT_FUND, "insufficient fund"};
    }
    
    // 持仓限额
    int64_t current_pos = position_cache_.net_position(req.account_id, req.instrument_id);
    int64_t limit = limit_cache_.position_limit(req.account_id, req.instrument_id);
    if (abs(current_pos + (req.side == Side::Buy ? req.qty : -req.qty)) > limit) {
        return {false, REJECT_POSITION_LIMIT, "position limit exceeded"};
    }
    
    // 自成交检测
    if (self_trade_detector_.would_self_trade(req)) {
        return {false, REJECT_SELF_TRADE, "self-trade detected"};
    }
    
    // 频率检查
    if (rate_limiter_.exceeds(req.account_id)) {
        return {false, REJECT_RATE_LIMIT, "rate limit exceeded"};
    }
    
    return {true, 0, ""};
}
```

**Cursor 审查提示词**：

```text
请审查前置风控检查逻辑，不要先修改代码。
重点：
1. 每项检查的延迟开销（是否访问远程数据/加锁/分配内存）
2. 定点数运算的溢出保护（price * qty 是否可能超 int64_t）
3. 缓存与真实数据的一致性保证（资金冻结/释放的原子性）
4. 降级策略：风控服务异常时是拒绝还是放行？
5. 规则热更新：新的限额生效是否有竞态？
6. 审计完整性：拒绝/通过必须记录完整上下文
7. 压测数据：单线程 QPS 与 P99 延迟
```

#### 18.5.3 VaR / Greeks 计算

```text
请审查风险计量模块，不要先修改。
重点：
1. VaR 计算方法（历史模拟/参数/蒙特卡洛）的选择合理性
2. 蒙特卡洛路径数与收敛性验证
3. Greeks 计算的数值微分步长选择
4. 波动率曲面插值的精度
5. 极端行情下的数值稳定性（隐含波动率反解失败处理）
6. 定价模型与结算价格的一致性
7. 日切/合约切换的边界处理
8. 多资产组合的相关性矩阵更新频率
```

---

### 18.6 清算结算开发指南

#### 18.6.1 日终清算流程

```
交易日结束
    │
    ▼
  数据冻结（停止接收新成交）
    │
    ▼
  成交汇总（按账户 × 品种聚合）
    │
    ▼
  盈亏计算（逐笔/逐日盯市）
    │
    ▼
  手续费计算（阶梯/返佣/优惠）
    │
    ▼
  保证金调整
    │
    ▼
  资金划转（冻结→结算→可用）
    │
    ▼
  对账校验（内部 + 外部）
    │
    ▼
  报表/报送生成
    │
    ▼
  日终标记完成，允许下一交易日
```

**Cursor 开发提示词**：

```text
请审查清算流程实现，不要先修改。
重点：
1. 事务边界：哪些步骤必须在同一事务中完成
2. 幂等性：重跑清算是否安全（断电恢复场景）
3. 精度：盈亏和手续费计算是否全程用定点数
4. 尾差处理：分摊后的尾差归属规则
5. 对账：内部头寸 vs 上游成交的差异检测与告警
6. 回退：某一步失败时的回退策略
7. 并发：多品种清算是否可并行，共享状态如何保护
8. 审计：每一步是否生成可追溯的审计记录
```

---

### 18.7 金融 C++ 特有工程约束

#### 18.7.1 数值精度铁律

```cpp
// ❌ 禁止：浮点数表示金额
double price = 12345.67;     // 精度丢失
double total = price * qty;   // 误差累积

// ✅ 正确：定点整数
int64_t price = 1234567000000; // price * 10^8
int64_t qty   = 100000000;     // qty * 10^8

// 乘法需要防溢出
// price * qty 可能溢出 int64_t
// 方案 1：__int128 中间结果
// 方案 2：先除再乘（注意精度损失方向）
// 方案 3：限制最大价格和数量的业务上界

// 除法的舍入方向必须明确
// 对客户有利 vs 对平台有利的舍入必须是业务决策
enum class RoundDir { Up, Down, Nearest, ToZero, AwayFromZero };
```

**Cursor 规则**：

```yaml
# .cursor/rules/75-financial-precision.mdc
description: "金融精度与资金安全"
globs:
  - "**/*.cpp"
  - "**/*.h"
rules:
  - 金额/价格/数量/费率禁止使用 float/double，必须使用定点整数
  - 乘法运算必须评估 int64_t 溢出风险，必要时使用 __int128
  - 除法舍入方向必须显式指定，不得依赖默认截断
  - 尾差分摊必须有明确归属规则
  - 资金变动必须先持久化再更新内存
  - 余额不允许出现负数（检查必须在扣减前完成）
```

#### 18.7.2 时间与时钟

```cpp
// 金融系统时间处理要点
// 1. 交易所时间 vs 本地时间：必须区分并显式标注
// 2. 时区：所有时间以 UTC 存储，展示时转换
// 3. 精度：至少微秒级，行情系统要求纳秒级
// 4. 时钟源：
//    - wall clock (system_clock) → 用于业务时间戳、报送
//    - monotonic clock (steady_clock) → 用于延迟测量、超时
//    - TSC → 用于超低延迟场景，但需要标定
// 5. 日切时间：不一定是 00:00，夜盘跨日需特殊处理
// 6. NTP/PTP 同步：多节点时间偏差必须有监控

struct Timestamp {
    int64_t epoch_ns;          // UTC nanoseconds since epoch
    ClockSource source;        // exchange / local_wall / local_mono / tsc
};
```

#### 18.7.3 审计与合规

```text
请审查当前系统的审计日志实现，不要先修改。
重点：
1. 关键事件覆盖：登录/下单/撤单/成交/出入金/参数变更/权限变更
2. 审计记录不可篡改：是否 append-only + 哈希链
3. 敏感信息：是否脱敏（身份证/银行卡/密码）
4. 时间戳：审计时间是否与业务时间独立（防回拨）
5. 保留期限：是否满足监管要求（通常 ≥ 20 年）
6. 查询能力：是否能按账户/时间/事件类型高效查询
7. 性能影响：审计写入是否影响交易热路径延迟
8. 导出格式：是否满足监管报送格式要求
```

---

### 18.8 低延迟开发模式

#### 18.8.1 热路径优化清单

| 优化方向 | 做法 | Cursor 审查关键词 |
| --- | --- | --- |
| 内存分配 | pool allocator / arena / pre-alloc | `new`、`malloc`、`make_shared`、`string` |
| 拷贝消除 | move / string_view / span / 零拷贝 buffer | `copy`、`=`、`push_back`、`substr` |
| 锁消除 | lock-free queue / 单线程 per-core / TLS | `mutex`、`lock`、`atomic` |
| 分支消除 | `[[likely]]` / `[[unlikely]]` / branchless | `if`、`switch`、`virtual` |
| 缓存友好 | SOA / packed struct / prefetch / 连续内存 | `struct`、`padding`、`alignas` |
| 系统调用 | busy poll / io_uring / kernel bypass | `read`、`write`、`epoll`、`recv` |
| 编译优化 | PGO / LTO / `-march=native` / `constexpr` | CMake flags、编译选项 |

#### 18.8.2 内存池模式

```cpp
// 订单对象池 — 撮合引擎关键基础设施
template <typename T, size_t PoolSize = 1 << 20>
class ObjectPool {
    // 关键设计决策：
    // 1. 固定大小 vs 可增长 — 金融系统倾向固定（可预测延迟）
    // 2. 线程安全 — 如果单线程访问则无需锁
    // 3. 对象复用 vs 重新构造 — clear() 还是 placement new
    // 4. 碎片化 — freelist vs bitmap vs bump allocator
    // 5. 内存对齐 — alignas(64) 避免 false sharing
};
```

**Cursor 开发提示词**：

```text
进入 Plan Mode。
目标：为撮合引擎实现订单对象池，替换当前 new/delete。
约束：
1. 单线程使用（撮合线程独占）
2. 固定容量，启动时分配，运行时零分配
3. O(1) 分配和回收
4. 对齐到 cache line（64 字节）
5. 支持统计：当前使用量、历史峰值、分配次数
6. 池耗尽时返回错误而非分配新内存
请给出：数据结构设计、接口定义、异常处理、benchmark 方案。
```

#### 18.8.3 无锁队列选型

```text
请分析当前系统中线程间通信使用的队列，不要先修改。
对比以下方案：
1. SPSC ring buffer（单生产者单消费者）
2. MPSC lock-free queue（多生产者单消费者）
3. MPMC bounded queue（多对多有界队列）
针对每种方案分析：
- 适用场景
- 延迟特征（P50 / P99 / worst case）
- 内存开销
- 空轮询 vs 阻塞等待策略
- cache line padding 和 false sharing 防护
- 失败处理（队列满/空时的行为）
结合当前系统的线程模型，给出推荐方案。
```

---

### 18.9 FIX 协议开发指南

#### 18.9.1 FIX 协议核心概念

```
FIX 消息结构：
  8=FIX.4.4 | 9=BodyLength | 35=MsgType | ... | 10=Checksum

关键消息类型：
  35=A  Logon           登录
  35=D  NewOrderSingle  新单
  35=F  OrderCancelReq  撤单
  35=8  ExecutionReport  成交回报
  35=W  MarketDataSnap  行情快照
  35=X  MarketDataIncr  行情增量
  35=0  Heartbeat       心跳
  35=1  TestRequest     测试请求
  35=5  Logout          登出
```

#### 18.9.2 FIX 引擎开发

**Cursor 开发提示词**：

```text
目标：审查/开发 FIX 协议引擎。
请分析：
1. 会话管理：Logon/Logout/Heartbeat/TestRequest 状态机
2. 序列号管理：gap fill、resend request、PossDup 处理
3. 消息解析：tag=value 解析性能、大消息处理、非法 tag
4. 连接管理：重连策略、failover、多会话隔离
5. 性能：解析是否零拷贝、发送是否 gather write
6. 持久化：消息日志用于回放与审计
7. 监控：会话状态、序列号偏差、延迟统计

FIX 特有陷阱：
- 序列号不连续需要正确处理 ResendRequest
- PossDupFlag=Y 的消息必须做幂等处理
- CompID 校验不严会导致安全漏洞
- Heartbeat 间隔不一致会导致会话断开
- 大批量 ExecutionReport 可能打满接收缓冲区
```

---

### 18.10 金融系统测试策略

#### 18.10.1 测试分层

| 层级 | 覆盖内容 | 运行频率 | 工具 |
| --- | --- | --- | --- |
| 单元测试 | 定点数运算、状态转换、解码器 | 每次提交 | GoogleTest |
| 组件测试 | 订单簿操作、风控检查、清算计算 | 每次提交 | GoogleTest + Mock |
| 协议测试 | FIX 消息编解码、半包、非法报文 | 每次提交 | 自定义 fixture |
| 集成测试 | 下单→风控→撮合→回报→头寸 | 每日 | 测试环境 |
| 回归测试 | 历史 bug 场景 | 每次提交 | Regression suite |
| 压力测试 | 峰值吞吐、P99、资源占用 | 周期性 | Benchmark + Profiler |
| 对账测试 | 内部余额 vs 外部流水 | 日终 | 对账脚本 |

#### 18.10.2 金融特有测试场景

**Cursor 生成测试提示词**：

```text
请为这个模块生成测试用例，不要修改实现。
金融特有场景必须覆盖：
1. 精度边界：最小变动价位、最大订单量、极端价格
2. 溢出边界：price * qty 接近 int64_t 上限
3. 零值边界：零价格、零数量、零费率
4. 负向边界：负价格（允许的品种）、负费用（返佣）
5. 时间边界：日切时刻、夜盘跨日、节假日
6. 状态边界：空订单簿、单边盘、涨跌停
7. 并发边界：同一账户并发下单、成交回报与撤单竞态
8. 恢复边界：crash 后重启、WAL 回放、断点续传
9. 幂等边界：重复消息、重复回报、重复清算
10. 降级边界：风控超时、行情中断、通道切换
```

#### 18.10.3 Sanitizer 策略

```text
金融系统 Sanitizer 使用建议：
- ASan（AddressSanitizer）：每次 CI 必跑，检测越界/double-free/use-after-free
- TSan（ThreadSanitizer）：并发修改必跑，检测数据竞争
- UBSan（UndefinedBehaviorSanitizer）：检测整数溢出（定点数运算关键）
- MSan（MemorySanitizer）：检测未初始化读（可选）

撮合引擎特别注意：
- TSan 与低延迟代码不兼容（开销大），但必须在测试环境跑
- UBSan 对定点数运算的溢出检测特别有价值
- ASan 的 quarantine 功能可检测延迟 use-after-free
```

---

### 18.11 金融系统可观测性

#### 18.11.1 核心指标体系

```text
交易链路指标：
  order_submit_qps           # 下单 QPS
  order_submit_latency_p99   # 下单到进入撮合的 P99 延迟
  match_latency_p99          # 撮合单笔 P99
  execution_report_latency   # 成交回报端到端延迟
  order_reject_rate          # 拒单率（按原因分）

风控指标：
  risk_check_latency_p99     # 前置风控 P99
  risk_check_reject_rate     # 风控拒绝率
  position_utilization       # 持仓/限额使用率
  margin_call_count          # 追保触发次数

行情指标：
  market_data_latency        # 行情端到端延迟
  market_data_gap_count      # 序列号跳变次数
  market_data_stale_count    # 过期数据次数
  snapshot_rebuild_count     # 快照重建次数

系统指标：
  queue_depth                # 各队列深度
  memory_pool_utilization    # 内存池使用率
  gc_pause_ns                # 如有 GC 的组件
  tcp_retransmit_count       # TCP 重传次数
  context_switch_count       # 上下文切换次数
```

#### 18.11.2 Cursor 观测增强流程

```text
请分析当前系统的可观测性覆盖，不要先修改。
按以下维度输出缺失项：
1. 是否每个关键路径都有延迟直方图（P50/P99/P999）
2. 是否每个错误路径都有错误码计数
3. 是否每个队列都有深度指标
4. 是否有 trace_id 贯穿完整链路
5. 日志是否结构化，是否包含 order_id/account_id/instrument_id
6. 是否有资源水位告警（CPU/内存/队列/连接数）
给出最小侵入的增强方案，优先补最容易出问题的指标。
```

---

### 18.12 金融系统常见故障排查

#### 故障 1：交易高峰期尾延迟飙升

```text
现象：平均延迟正常，P99 从 100μs 飙升到 10ms。
请按以下顺序排查：
1. 队列积压：检查撮合前队列深度是否在高峰期暴增
2. 锁竞争：检查是否有多线程竞争同一把锁
3. 内存分配：检查 tcmalloc/jemalloc 的 central cache 争用
4. 日志 IO：检查同步日志是否在热路径上
5. 上下文切换：检查是否 CPU 核不够或绑核策略失效
6. 网络：检查 TCP 缓冲区是否打满触发 Nagle/延迟 ACK
7. GC（如有 JNI）：检查是否 JVM 组件的 GC 影响了 C++ 端
给出每一项的具体检查命令和指标查看方式。
```

#### 故障 2：行情数据不一致

```text
现象：内部撮合价格与客户端显示价格偶发不一致。
请排查：
1. 行情源切换时是否有短暂数据缺失
2. 快照与增量的序列号是否正确校验
3. 多通道行情的时间戳对齐是否有偏差
4. 缓存更新与读取是否有竞态（读到半更新状态）
5. 定点数转换是否在不同模块使用了不同精度
6. 聚合计算（VWAP/TWAP）是否有精度累积误差
```

#### 故障 3：清算对账不平

```text
现象：日终清算后内部头寸与交易所头寸不一致。
请排查：
1. 成交回报是否有丢失（检查 FIX 序列号连续性）
2. 撤单回报处理是否正确（已撤单量是否回退）
3. 部分成交的累计量是否正确（LastQty vs CumQty）
4. 手续费计算舍入方向是否与交易所一致
5. 持仓合并（merge）算法是否正确处理了多次开平仓
6. 日切时刻的跨日订单是否正确归属交易日
7. 是否有异步回报尚未处理就开始了清算
```

#### 故障 4：风控误拦截或放行

```text
现象：风控检查偶发不准，有时误拦正常单，有时放行超限单。
请排查：
1. 风控缓存与实际资金/头寸的同步延迟
2. 并发下单时的余额预扣是否有竞态
3. 限额配置热更新是否有读写竞态
4. 精度问题：判断条件是 >= 还是 >，定点数比较是否正确
5. 降级策略是否合理（超时=拒绝？还是放行？）
6. 规则优先级：多条规则匹配时的执行顺序是否确定
```

---

### 18.13 Cursor 金融项目配置最佳实践

#### 18.13.1 Rules 组合策略

| 开发场景 | 启用规则 |
| --- | --- |
| 撮合引擎开发 | 00-general + 30-performance + 70-matching-engine + 40-testing |
| 风控引擎开发 | 00-general + 75-financial-precision + 20-concurrency + 40-testing |
| 行情网关开发 | 00-general + 50-rpc-network + 72-market-data + 30-performance |
| 清算结算开发 | 00-general + 75-financial-precision + 25-security-compliance + 40-testing |
| OMS 开发 | 00-general + 20-concurrency + 35-risk-data-protocols + 40-testing |
| 合规审计开发 | 00-general + 25-security-compliance + 60-observability |
| 协议升级 | 00-general + 50-rpc-network + 40-testing |
| 性能优化 | 00-general + 30-performance + 40-testing |

#### 18.13.2 AGENTS.md 金融扩展

在现有 `AGENTS.md` 基础上，为金融项目追加以下段落：

```markdown
## Financial Domain Constraints

- All monetary values MUST use fixed-point integers (int64_t, scale 10^8).
- Never use float/double for price, quantity, or amount.
- Explicit overflow checks on price * qty multiplication.
- Division rounding direction must be explicitly specified.
- Balance must never go negative — check BEFORE deduction.
- All state transitions must be persisted (WAL) before in-memory update.
- All order/trade/risk events must produce audit records.
- Timestamps must distinguish exchange time vs local time.
- FIX sequence numbers must be gap-checked; duplicates must be idempotent.
- Risk check failures must include reject code + reason for audit trail.
```

#### 18.13.3 命名与目录约定

```
project_root/
├── gateway/            # 网关层（FIX、行情接入、API）
│   ├── fix/            # FIX 协议引擎
│   ├── market_data/    # 行情接入与分发
│   └── api/            # REST/WebSocket API
├── core/               # 核心业务层
│   ├── matching/       # 撮合引擎
│   ├── order/          # 订单管理（OMS）
│   ├── risk/           # 风控引擎
│   └── position/       # 头寸管理
├── settlement/         # 清算结算
├── infra/              # 基础设施
│   ├── memory/         # 内存池、分配器
│   ├── queue/          # 无锁队列
│   ├── timer/          # 定时器
│   ├── log/            # 日志
│   └── metrics/        # 指标
├── persist/            # 持久化层
│   ├── wal/            # Write-Ahead Log
│   └── db/             # 数据库交互
├── test/               # 测试
│   ├── unit/
│   ├── integration/
│   ├── protocol/       # 协议边界测试
│   ├── benchmark/
│   └── regression/
└── tools/              # 工具
    ├── reconcile/      # 对账工具
    ├── replay/         # 回放工具
    └── monitor/        # 监控仪表板
```

---

### 18.14 金融项目 Cursor 开发速查表

| 我要做什么 | Cursor 操作 | 关键提示词 |
| --- | --- | --- |
| 理解撮合逻辑 | Agent + "先不要改" | "解释订单簿结构和匹配流程" |
| 优化热路径 | Plan Mode | "分析热路径分配/拷贝/锁，给出方案" |
| 新增风控规则 | Agent | "分析现有规则引擎，给出最小侵入方案" |
| 升级 FIX 协议 | Plan Mode | "分析兼容性、解码器、序列号影响" |
| 修复对账不平 | Agent + "先分析" | "排查成交回报丢失、累计量错误" |
| 补充审计日志 | Agent | "分析缺失事件、性能影响、格式要求" |
| 精度问题排查 | Agent + "不要改" | "检查定点数溢出、舍入方向、精度损失" |
| 设计清算流程 | Plan Mode | "事务边界、幂等、回退、审计" |
| 行情解码器开发 | Plan Mode | "协议结构、半包、零拷贝、benchmark" |
| 压测报告生成 | Agent | "输出 QPS/P99/资源/瓶颈分析报告" |
| 日终批处理优化 | Plan Mode | "分析可并行步骤、事务依赖、耗时分布" |
| 监管报送开发 | Agent | "格式要求、字段映射、脱敏、审计链" |

