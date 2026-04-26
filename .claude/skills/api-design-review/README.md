# api-design-review

## 定位

API 设计审查 Skill，聚焦接口边界、输入输出契约、版本策略、错误模型、一致性和可演进性。

## 适用场景

- 新增 API 或修改请求/响应结构
- 错误码、分页、幂等性、兼容性评审
- 服务间接口治理

## 核心关注点

- 合同清晰度和一致性
- 版本与兼容策略
- 错误码与语义稳定性
- 调用方迁移成本和测试覆盖

## 目录说明

- `SKILL.md`：技能定义与输出要求
- `CHECKLISTS/`：API 清单
- `EXAMPLES/`：使用示例
- `TEMPLATES/`：模板占位目录

## 协作边界

- 协议层细节可联动 `protocol-validate`
- 安全边界可联动 `security-review`
