---
name: code-review
description: 通用代码审查技能，适合先做正确性、安全性、可维护性和测试覆盖的基础筛查
---

# code-review

## 目标

对当前改动做结构化基础审查，并识别是否需要转入更深的专项审查。

## 适用条件

- 审查 PR、分支差异、当前目录或单文件
- 不确定应先使用哪个专项 Skill 时
- 需要先做通用质量把关时

## 审查重点

- 正确性、边界条件、错误路径
- 基础安全问题与敏感信息暴露
- 资源释放、异常路径、测试缺口
- 明显性能问题与可维护性问题

## 输入期望

- 变更范围或目标路径
- 变更目的与风险背景
- 若有测试或报错信息，可一并提供

## 输出要求

- 按严重级别列问题
- 指出位置、原因、风险、建议
- 区分 blocking 与 non-blocking
- 说明是否建议进一步使用专项 Skill

## 协作边界

- 并发问题转 `concurrency-review`
- 内存与所有权问题转 `memory-management-review`
- C++ 服务端整体问题转 `cpp-backend-review`
- 明确性能热点转 `performance-bottleneck-identification`
