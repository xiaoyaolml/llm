---
name: cpp-backend-review
description: C++后端专项审查技能 - 专注于互联网金融场景的C++后端代码审查，包括内存管理、并发模式、性能优化等关键领域
---

# 💻 C++后端专项审查技能 (cpp-backend-review)

## 🎯 功能概述

此技能专门用于互联网金融场景的C++后端代码审查，重点关注C++特定技术、内存管理、并发模型、网络通信和性能优化等关键领域，特别关注金融系统对性能和安全性的极高要求。

## 🔧 支持的C++后端审查维度

- C++特性安全使用 (C++ Features)
- 内存管理 (Memory Management)
- 模板与泛型编程 (Templates & Generic Programming)
- 移动语义 (Move Semantics)
- 性能优化 (Performance Optimization)
- 并发模型 (Concurrency Model)
- 资源管理 (Resource Management)
- API设计模式 (API Design Patterns)
- 内存安全 (Memory Safety)
- 线程安全 (Thread Safety)

## 📋 主要审查功能

### 1. C++特性安全使用
- 检查C++11/14/17/20特性使用合理性
- 验证编译器扩展使用合规性
- 审查模板元编程安全性
- 确保语言特性的正确使用

### 2. 内存管理
- 检查智能指针使用合理性
- 验证RAII模式应用
- 审查内存泄漏风险
- 确保内存池和自定义分配器的正确使用

### 3. 模板与泛型编程
- 检查SFINAE的正确使用
- 验证模板特化安全性
- 审查类型推导正确性
- 确保模板代码的可读性和可维护性

### 4. 移动语义
- 检查移动构造函数和赋值运算符
- 验证完美转发正确性
- 审查noexcept使用
- 确保右值引用使用合理

### 5. 性能优化
- 检查零拷贝技术应用
- 验证SIMD指令集使用
- 审查缓存局部性优化
- 确保高频操作的性能

### 6. 并发模型
- 检查线程安全问题
- 验证内存模型正确性
- 审查原子操作使用
- 确保无锁编程正确性

### 7. 资源管理
- 检查文件描述符泄漏
- 验证网络连接管理
- 审查日志资源使用
- 确保资源清理机制完备

### 8. API设计模式
- 检查Pimpl模式应用
- 验证接口设计一致性
- 审查异常安全保证
- 确保接口易用性

### 9. 内存安全
- 检查内存越界访问
- 验证悬空指针使用
- 审查缓冲区溢出风险
- 确保内存对齐正确

### 10. 线程安全
- 检查共享数据访问同步
- 验证临界区保护
- 审查死锁风险
- 确保线程间通信安全

## 🛠 使用方法

```bash
# 基本使用
claude /cpp-backend-review src/

# 指定审查维度
claude /cpp-backend-review src/ --dimension memory

# 指定C++标准
claude /cpp-backend-review src/ --standard c++17

# 输出JSON格式报告
claude /cpp-backend-review src/ --output json

# 指定特定审查模块
claude /cpp-backend-review src/ --module performance,concurrency

# 详细模式
claude /cpp-backend-review src/ --verbose
```

## 📊 输出格式

### Markdown 输出
```markdown
# C++后端审查报告

## 概述
- 系统名称: trading_platform
- 审查时间: 2026-04-09 10:30:00
- C++标准: C++17

## 审查结果

### 内存管理
- ✅ 智能指针使用合理
- ✅ RAII模式应用良好
- ⚠️ 某些地方可考虑unique_ptr代替shared_ptr
- ✅ 内存泄漏风险低

### 模板编程
- ✅ SFINAE使用正确
- ✅ 模板特化安全
- ✅ 类型推导清晰
- ⚠️ 部分模板过深，影响可读性

### 移动语义
- ✅ 移动构造函数实现
- ✅ 完美转发正确
- ✅ noexcept使用恰当
- ✅ 右值引用使用合理

### 性能优化
- ✅ 零拷贝技术应用
- ✅ SIMD指令优化
- ✅ 缓存局部性优化
- ⚠️ 个别循环可以进一步优化

### 并发模型
- ✅ 线程安全机制
- ✅ 内存模型正确
- ✅ 原子操作使用
- ✅ 无锁编程正确

## 建议
1. 某些地方改用unique_ptr替代shared_ptr
2. 优化过深的模板层次
3. 优化个别循环
```

### JSON 输出
```json
{
  "system_name": "trading_platform",
  "timestamp": "2026-04-09T10:30:00Z",
  "cpp_standard": "C++17",
  "review_results": {
    "memory": {
      "status": "warning",
      "checks": [
        {"name": "smart_pointer_usage", "status": "pass"},
        {"name": "raii_application", "status": "pass"},
        {"name": "shared_ptr_usage", "status": "warning"},
        {"name": "leak_risk", "status": "pass"}
      ]
    },
    "templates": {
      "status": "warning",
      "checks": [
        {"name": "sfinae_usage", "status": "pass"},
        {"name": "template_specialization", "status": "pass"},
        {"name": "type_deduction", "status": "pass"},
        {"name": "template_depth", "status": "warning"}
      ]
    },
    "move_semantics": {
      "status": "pass",
      "checks": [
        {"name": "move_constructors", "status": "pass"},
        {"name": "perfect_forwarding", "status": "pass"},
        {"name": "noexcept_usage", "status": "pass"},
        {"name": "rvalue_references", "status": "pass"}
      ]
    },
    "performance": {
      "status": "warning",
      "checks": [
        {"name": "zero_copy", "status": "pass"},
        {"name": "simd_usage", "status": "pass"},
        {"name": "cache_locality", "status": "pass"},
        {"name": "loop_optimization", "status": "warning"}
      ]
    },
    "concurrency": {
      "status": "pass",
      "checks": [
        {"name": "thread_safety", "status": "pass"},
        {"name": "memory_model", "status": "pass"},
        {"name": "atomic_operations", "status": "pass"},
        {"name": "lock_free", "status": "pass"}
      ]
    }
  },
  "recommendations": [
    "某些地方改用unique_ptr替代shared_ptr",
    "优化过深的模板层次",
    "优化个别循环"
  ]
}
```

## 🔍 审查规则

### C++标准规则
- ✅ 必须使用指定C++标准
- ✅ 应避免编译器特定扩展
- ✅ 模板元编程应有边界控制
- ✅ 应避免过度使用C++特性
- ✅ 应有明确的C++标准声明

### 内存安全规则
- ✅ 内存访问应安全
- ✅ 避免缓冲区溢出
- ✅ 悬空指针应避免
- ✅ 内存越界应防止
- ✅ 内存对齐应正确
- ✅ 动态内存使用应谨慎

### 线程安全规则
- ✅ 共享数据应同步访问
- ✅ 临界区应最小化
- ✅ 数据竞争应避免
- ✅ 死锁风险应预防
- ✅ 线程间通信应安全
- ✅ 原子操作应合理使用

## 🧪 示例用法

### 审查C++17代码
```bash
claude /cpp-backend-review src/ --standard c++17
```

### 检查内存管理
```bash
claude /cpp-backend-review src/ --dimension memory --output json
```

### 验证模板编程
```bash
claude /cpp-backend-review src/ --dimension templates
```

### 审查性能优化
```bash
claude /cpp-backend-review src/ --dimension performance
```

## 📚 注意事项

1. **C++环境**：需要正确配置C++编译环境
2. **代码理解**：需要理解C++特定语言特性
3. **性能考虑**：深度C++分析可能需要较长时间
4. **依赖项**：可能需要访问C++标准文档
5. **合规要求**：确保符合金融行业安全规范

## 🚀 适用场景

### 1. C++后端开发
- 评估C++代码质量
- 验证内存管理合理性
- 审查并发模型正确性
- 检查性能优化效果

### 2. 代码迁移
- 评估C++标准升级风险
- 检查API变更影响
- 评估性能回归风险
- 验证内存模型变更

### 3. 代码审查
- 识别关键内存安全风险
- 发现性能瓶颈
- 评估并发模型可靠性
- 保证代码可维护性

### 4. 项目上线
- 确保内存安全
- 验证性能达标
- 保证线程安全
- 确保API设计一致

## 📈 与金融开发技能组的协同

此技能与以下金融开发技能配合使用效果最佳：

- `/code-review` - 通用代码审查
- `/finance-optim` - 金融性能优化
- `/concurrency-review` - 高频并发审查
- `/system-monitoring` - 系统监控审查
- `/trading-simulation` - 交易模拟器审查
- `/network-optimization` - 网络性能优化

通过组合使用这些技能，可以建立完整的C++后端金融系统开发和运维体系。