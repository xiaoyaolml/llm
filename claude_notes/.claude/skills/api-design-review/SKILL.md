---
name: api-design-review
description: 金融API接口设计规范审查 - 专注于金融系统的RESTful API设计、版本管理、错误处理、安全性和兼容性等关键领域
---

# 📡 API设计审查技能 (api-design-review)

## 🎯 功能概述

此技能专注于互联网金融系统的API接口设计审查，特别针对交易系统、风控系统、市场数据系统等关键金融业务，全面评估API设计规范性、一致性、可维护性，确保系统符合RESTful设计原则、金融行业标准以及内部API设计规范。

## 🔧 支持的API设计维度

- API设计原则 (API Design Principles)
- 版本管理策略 (Versioning Strategy)
- 認号规范与错误处理 (Error Handling)
- 安全设计 (Security Design)
- 兼容性与升级策略 (Compatibility & Upgrade)
- 性能考虑 (Performance Considerations)
- 文檔质量 (Documentation Quality)
- 监控集成 (Monitoring Integration)

## 📋 主要审查功能

### 1. API设计原则
- 检查是否遵循RESTful规范
- 验证资源命名合理性
- 审查HTTP方法使用正确性
- 评估请求/响应体设计
- 检测HATEOAS使用

### 2. 版本管理策略
- 检查API版本控制策略
- 驗证URL/HEADER版本管理
- 审查版本过期策略
- 检测兼容性降级处理
- 验证文档版本同步

### 3. 認号规范与错误处理
- 检查HTTP状态码使用
- 验证错误响应格式
- 审查错误代码设计
- 检测业务错误与系统错误分离
- 驗证错误信息国际化

### 4. 安全设计
- 检查认证机制设计
- 驗证授权粒度
- 审查安全头部配置
- 检测权限边界
- 驗证敏感数据处理

### 5. 兼容性与升级策略
- 检查向前/向后兼容性
- 驗证新增字段处理
- 审查废弃API策略
- 检测变更传播影响
- 验证迁移路径

### 6. 性能考虑
- 检查响应大小优化
- 驗证批量接口合理性
- 审查分页策略
- 检测资源密集型操作
- 驗证CORS配置优化

### 7. 文檔质量
- 检查Swagger/Postman文档
- 驗证示例完整性
- 审查文档更新时效性
- 检测缺失端点文档
- 验证参数说明清晰度

### 8. 监控集成
- 检查请求ID传递
- 驗证性能指标采集
- 审查错误率监控
- 检测慢调用报警
- 驗证审计日志完整性

## 🛠 使用方法

```bash
# 基本使用
claude /api-design-review src/api/

# 指定审查维度
claude /api-design-review src/api/ --dimension error-handling

# 指定API类型
claude /api-design-review src/api/ --api-type rest

# 输出JSON格式报告
claude /api-design-review src/api/ --output json

# 详细模式
claude /api-design-review src/api/ --verbose

# 指定特定规范
claude /api-design-review src/api/ --standard restful
```

## 📊 输出格式

### Markdown 输出
```markdown
# API设计审查报告

## 概述
- 系统名称: trading-api
- 审查时间: 2026-04-09 10:30:00
- API类型: REST
- 审查范围: 全部设计维度

## 审查结果

### API设计原则 (⚠️ 警告)
- ✅ 遵循RESTful原则
- ⚠️ 订单资源命名不一致
- ✅ HTTP方法使用正确
- ❌ 缺少HATEOAS支持

### 版本管理策略 (✅ 通过)
- ✅ 采用URL版本控制
- ✅ 版本格式规范
- ✅ 过期策略明确
- ✅ 文檔版本同步

### 错误处理 (❌ 严重)
- ✅ 使用标准HTTP状态码
- ❌ 业务错误与系统错误未分离
- ❌ 错误消息包含敏感信息
- ⚠️ 错误代码缺少文档说明

## 严重问题

### 🟠 警告: 订单资源命名不一致
- **位置**: /v1/orders vs /v1/trade-requests
- **问题**: 不一致的资源命名导致客户端困惑
- **修复建议**: 统一使用/orders作为订单资源

### 🟥 严重: 错误消息包含敏感信息
- **位置**: 400 Bad Request 响应体
- **问题**: 返回内部错误栈信息
- **修复建议**: 移除内部细节，提供用户友好的错误消息

## 优化建议

### 高优先级
1. **修复错误处理中的敏感信息泄露**
2. **统一订单资源命名规范**

### 中优先级
1. **添加HATEOAS支持，提升可发现性**
2. **优化错误代码文档**

## 合规性检查
- ✅ OpenAPI 规范: 符合 OpenAPI 3.0 标准
- ⚠️ 金融API标准: 需要增强错误码规范化
- ✅ 内部设计规范: 大到 85% 要求

## 审查统计
- 82 个API端点
- 21 个问题（2严重, 8警告, 11提示）
- 74% 合规率
```