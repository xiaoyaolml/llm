---
name: network-optimization
description: 互联网金融场景的网络性能优化技能 - 专注于优化网络延迟、带宽使用、连接稳定性等关键指标
---

# 🌐 网络性能优化技能 (network-optimization)

## 🎯 功能概述

此技能专注于互联网金融场景的网络性能优化，特别针对高频交易系统、支付系统和数据流密集型应用，重点优化网络延迟、带宽使用、连接稳定性和数据传输效率。

## 🔧 支持的网络优化维度

- 网络延迟优化 (Network Latency Optimization)
- 带宽使用优化 (Bandwidth Usage Optimization)
- 连接稳定性 (Connection Stability)
- 数据传输效率 (Data Transfer Efficiency)
- 网络错误处理 (Error Handling)
- 负载均衡 (Load Balancing)

## 📋 主要审查功能

### 1. 网络延迟优化
- 检查请求/响应时间是否合理
- 验证关键路径延迟优化
- 审查客户端-服务器通信模式
- 确保最小化网络往返时间

### 2. 带宽使用优化
- 检查数据传输大小是否合理
- 验证数据压缩与编码
- 审查数据传输频率
- 评估网络流量模式

### 3. 连接稳定性
- 检查连接重试机制
- 验证连接池管理
- 审查网络断开处理
- 确保连接超时设置合理

### 4. 数据传输效率
- 检查数据序列化与反序列化
- 验证传输协议选择
- 审查批处理与流式传输
- 评估数据校验开销

### 5. 网络错误处理
- 检查错误重试策略
- 验证错误恢复机制
- 审查网络超时配置
- 确保错误日志记录完整

### 6. 负载均衡
- 检查负载均衡策略
- 验证流量分配
- 审查服务发现机制
- 确保故障转移机制

## 🛠 使用方法

```bash
# 基本使用
claude /network-optimization network-config/

# 指定审查范围
claude /network-optimization network-config/ --dimension latency

# 指定特定组件
claude /network-optimization network-config/ --component client,server,gateway

# 输出JSON格式报告
claude /network-optimization network-config/ --output json

# 指定特定审查模块
claude /network-optimization network-config/ --module latency,bandwidth

# 详细模式
claude /network-optimization network-config/ --verbose
```

## 📊 输出格式

### Markdown 输出
```markdown
# 网络性能优化审查报告

## 概述
- 系统名称: high-frequency-trading
- 审查时间: 2026-04-09 10:30:00
- 网络场景: 高频交易系统

## 审查结果

### 网络延迟
- ✅ 平均请求延迟 < 50ms
- ✅ 关键路径延迟优化
- ⚠️ 部分API延迟超过100ms
- ✅ 服务器到客户端延迟稳定

### 带宽使用
- ✅ 数据传输量合理
- ✅ 压缩使用率高
- ⚠️ 偶尔出现高流量
- ✅ 传输效率良好

### 连接稳定性
- ✅ 连接重试机制有效
- ✅ 连接池配置合理
- ⚠️ 部分连接超时设置
- ✅ 网络断开处理完善

### 数据传输效率
- ✅ 数据序列化高效
- ✅ 协议选择正确
- ✅ 批处理优化
- ⚠️ 偶尔存在重复数据

## 建议
1. 优化关键API的延迟
2. 优化数据传输大小
3. 调整连接池参数
4. 优化错误重试策略