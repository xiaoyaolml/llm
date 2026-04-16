---
name: cloud-native-review
description: 云原生应用审查技能 - 审查金融应用的容器化、微服务架构、服务网格、自动扩缩容等云原生特性
---

# ☁️ 云原生应用审查技能 (cloud-native-review)

## 🎯 功能概述

此技能专门用于金融应用的云原生特性审查，确保应用程序在云环境中能够安全、高效地运行，特别关注金融行业对稳定性、安全性和合规性的严格要求。

## 🔧 支持的云原生技术栈

- 容器化部署 (Docker, Kubernetes)
- 微服务架构 (Service Mesh, API Gateway)
- 自动扩缩容 (HPA, VPA)
- 服务网格 (Istio, Linkerd)
- CI/CD 流水线 (GitHub Actions, Jenkins, GitLab CI)
- 监控与日志 (Prometheus, Grafana, ELK Stack)

## 📋 主要审查功能

### 1. 容器化部署审查
- 检查Docker镜像安全性和最小化
- 验证容器运行时的安全配置
- 审查容器健康检查机制
- 确保镜像版本管理和依赖项控制

### 2. 微服务架构审查
- 检查服务拆分的合理性
- 验证服务间通信的安全性
- 审查服务发现和负载均衡机制
- 确保服务熔断和降级策略

### 3. 自动扩缩容审查
- 检查水平和垂直自动扩缩容配置
- 验证资源请求和限制设置
- 审查扩缩容触发条件和策略
- 确保扩缩容过程中的服务稳定性

### 4. 服务网格审查
- 检查服务间通信的安全性
- 验证流量管理策略
- 审查可观测性配置
- 确保服务网格的安全性和性能

### 5. CI/CD流程审查
- 检查流水线安全性
- 验证自动化测试覆盖率
- 审查部署策略（蓝绿部署、金丝雀发布）
- 确保回滚机制的有效性

## 🛠 使用方法

```bash
# 基本使用
claude /cloud-native-review kubernetes-manifests/

# 指定审查范围
claude /cloud-native-review kubernetes-manifests/ --validate-container

# 指定特定组件
claude /cloud-native-review kubernetes-manifests/ --component microservices

# 输出JSON格式报告
claude /cloud-native-review kubernetes-manifests/ --output json

# 指定特定审查模块
claude /cloud-native-review kubernetes-manifests/ --module security,scalability

# 详细模式
claude /cloud-native-review kubernetes-manifests/ --verbose
```

## 📊 输出格式

### Markdown 输出
```markdown
# 云原生应用审查报告

## 概述
- 应用名称: trading_platform
- 审查时间: 2026-04-09 10:30:00
- 审查环境: Kubernetes v1.28

## 审查结果

### 容器化部署
- ✅ Docker镜像安全扫描通过
- ⚠️ 建议减少基础镜像层数
- ✅ 健康检查配置完整
- ✅ 环境变量注入安全

### 微服务架构
- ✅ 服务拆分合理
- ✅ 服务间通信加密
- ✅ 负载均衡配置正确
- ⚠️ 缺少服务熔断机制

### 自动扩缩容
- ✅ HPA配置合理
- ✅ 资源请求/限制正确
- ✅ 扩缩容策略有效
- ⚠️ 缺少VPA配置

### 服务网格
- ⚠️ Istio配置简化
- ⚠️ 缺少链路追踪
- ⚠️ 限流策略不完善

### CI/CD流程
- ✅ 流水线安全性良好
- ✅ 测试覆盖率达标
- ⚠️ 部署策略待优化
- ✅ 回滚机制完备

## 建议
1. 优化Docker镜像层数
2. 添加服务熔断机制
3. 补充VPA配置
4. 完善服务网格安全策略
5. 优化部署策略
```

### JSON 输出
```json
{
  "app_name": "trading_platform",
  "timestamp": "2026-04-09T10:30:00Z",
  "environment": "Kubernetes v1.28",
  "review_results": {
    "containerization": {
      "status": "warning",
      "checks": [
        {"name": "image_security", "status": "pass"},
        {"name": "layer_optimization", "status": "warning"},
        {"name": "health_checks", "status": "pass"},
        {"name": "env_injection", "status": "pass"}
      ]
    },
    "microservices": {
      "status": "warning",
      "checks": [
        {"name": "service_split", "status": "pass"},
        {"name": "communication_security", "status": "pass"},
        {"name": "load_balancing", "status": "pass"},
        {"name": "circuit_breaker", "status": "warning"}
      ]
    },
    "autoscaling": {
      "status": "warning",
      "checks": [
        {"name": "hpa_config", "status": "pass"},
        {"name": "resource_limits", "status": "pass"},
        {"name": "scaling_policy", "status": "pass"},
        {"name": "vpa_config", "status": "warning"}
      ]
    },
    "service_mesh": {
      "status": "warning",
      "checks": [
        {"name": "istio_config", "status": "warning"},
        {"name": "tracing", "status": "warning"},
        {"name": "rate_limiting", "status": "warning"}
      ]
    },
    "ci_cd": {
      "status": "warning",
      "checks": [
        {"name": "pipeline_security", "status": "pass"},
        {"name": "test_coverage", "status": "pass"},
        {"name": "deployment_strategy", "status": "warning"},
        {"name": "rollback_mechanism", "status": "pass"}
      ]
    }
  },
  "recommendations": [
    "优化Docker镜像层数",
    "添加服务熔断机制",
    "补充VPA配置",
    "完善服务网格安全策略",
    "优化部署策略"
  ]
}
```

## 🔍 审查规则

### 容器化部署规则
- ✅ 镜像应使用最小化的基础镜像
- ✅ 应启用镜像安全扫描
- ✅ 健康检查应包含存活探针和就绪探针
- ✅ 环境变量应避免硬编码敏感信息
- ✅ 应使用非root用户运行容器

### 微服务架构规则
- ✅ 服务间通信应加密（TLS）
- ✅ 应实施服务熔断机制
- ✅ 负载均衡应支持会话亲和性
- ✅ 服务注册与发现应可靠
- ✅ 应支持服务降级策略

### 自动扩缩容规则
- ✅ HPA应基于合理的指标（CPU、内存、自定义指标）
- ✅ 资源请求和限制应合理设置
- ✅ 扩缩容速率应避免过快导致的抖动
- ✅ 应启用VPA进行资源优化
- ✅ 扩缩容策略应考虑业务特点

### 服务网格规则
- ✅ 服务间通信应强制使用mTLS
- ✅ 应配置流量路由策略
- ✅ 应启用分布式追踪
- ✅ 应实施适当的限流策略
- ✅ 应配置可观测性组件

### CI/CD流程规则
- ✅ 应启用代码签名和验证
- ✅ 应包含自动化安全扫描
- ✅ 应实施多阶段部署
- ✅ 应配置自动回滚机制
- ✅ 应进行部署前和部署后的测试

## 🧪 示例用法

### 验证Kubernetes部署配置
```bash
claude /cloud-native-review k8s/deployments/ --validate-container --validate-scaling
```

### 审查微服务架构
```bash
claude /cloud-native-review services/ --module microservices --output json
```

### 审查CI/CD流水线
```bash
claude /cloud-native-review ci-cd/ --component cicd --validate-security
```

### 完整云原生审查
```bash
claude /cloud-native-review kubernetes-manifests/ --validate-all
```

## 📚 注意事项

1. **环境准备**：需要访问Kubernetes集群配置文件
2. **权限要求**：需要读取相关资源配置的权限
3. **依赖项**：需要安装kubectl工具和相关插件
4. **性能考虑**：大规模集群审查可能需要较长时间
5. **合规要求**：确保审查过程符合金融行业安全规范

## 🚀 适用场景

### 1. 云原生转型
- 传统应用迁移至云原生架构
- 微服务改造项目
- 容器化部署方案评估

### 2. 运维优化
- Kubernetes集群安全加固
- 自动扩缩容策略优化
- 服务网格配置调优

### 3. 合规审计
- 云原生基础设施合规性检查
- 安全基线符合性验证
- 数据保护措施审查

### 4. 性能优化
- 资源利用率分析
- 部署效率优化
- 服务性能监控

## 📈 与金融开发技能组的协同

此技能与以下金融开发技能配合使用效果最佳：

- `/code-review` - 通用代码审查
- `/compliance-review` - 合规性审查
- `/finance-optim` - 金融性能优化
- `/concurrency-review` - 高频并发审查
- `/market-data-validate` - 市场数据质量验证

通过组合使用这些技能，可以建立完整的云原生金融应用开发和运维流程。